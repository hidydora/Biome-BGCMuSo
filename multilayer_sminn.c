/* 
multilayer_sminn.c
Calculating the change in content of soil mineral nitrogen in multilayer soil (plant N upate, soil processes, nitrogen leaching, 
depostion and fixing). 

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
BBGC MuSo v4
Copyright 2014, D. Hidy (dori.hidy@gmail.com)
Hungarian Academy of Sciences
*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <malloc.h>
#include "bgc_struct.h"
#include "bgc_func.h"
#include "bgc_constants.h"

int multilayer_sminn(const epconst_struct* epc, const epvar_struct* epv, 
					 nstate_struct* ns, nflux_struct* nf, wstate_struct* ws, wflux_struct* wf)
{
	int ok=1;
	int layer=0;
	double soilwater_nconc_downward, soilwater_nconc_upward, wflux_downward, wflux_upward, sminn0, sminn1;
	double sminn_SOILPROC_SUM;
	double diff, sminn_flux, sminn_to_npool, sminn_to_denitrif, change_ctrl, sminn_total, layer_prop;


	sminn_SOILPROC_SUM = sminn_flux = change_ctrl = sminn_to_npool = sminn_to_denitrif = sminn_total =layer_prop = 0;



	/* ***************************************************************************************************** */	
	/* 1. SOILPROC:
		1.A.Plant N uptake from SMINN (sminn_to_npool is determined in daily_allocation routine) - 
	    1.B.Decomposition: due microbial soil processes SMINN (soil mineral N content) is changing in the soil (determined in daily_allocation).
	    The produced/consumed N is divided between soil layers based on their N content. */

	/* 2. State update SMINN: soil processes (decomposition + plant uptake), deposition and fixation */

	
	for (layer = 0; layer < N_SOILLAYERS; layer++)
	{
		sminn_total += ns->sminn[layer];
	}
		

	for (layer = 0; layer < N_SOILLAYERS; layer++)
	{
		if (sminn_total)
			layer_prop= ns->sminn[layer]/sminn_total;
		else 
			layer_prop= 0;
		sminn_to_npool          = (nf->sminn_to_npool)    * layer_prop;
		sminn_to_denitrif       = (nf->sminn_to_denitrif) * layer_prop;
		nf->sminn_change[layer] = sminn_to_npool + sminn_to_denitrif + nf->sminn_to_soil_SUM[layer] + nf->sminn_to_nvol_SUM[layer];

		/* 1. soil processes (decomposition + plant uptake) */
		diff = ns->sminn[layer] - nf->sminn_change[layer];
	
		if (diff < 0.0)       
		{	
			nf->sminn_change[layer]      += diff;
			nf->sminn_to_soil_SUM[layer] += diff;
			ns->nvol_snk += diff;	
		}

		ns->sminn[layer] -= nf->sminn_change[layer];

		

	
	
		/* 2. deposition: only in top soil layer */
		if (layer == 0) ns->sminn[0]     += nf->ndep_to_sminn;

		/* 3. fixation: based on the quantity of the root mass in the given layer */
		ns->sminn[layer] += nf->nfix_to_sminn * epv->rootlength_prop[layer];

		/* CONTROL */
		change_ctrl  += layer_prop;

	}

	if ((sminn_total) && fabs(1 - change_ctrl) > CRIT_PREC)
	{
		printf("ERROR in calculation of mineralized nitrogen in multilayer_sminn.c\n");
		ok=0;
	}

	
	/* State update */
	ns->nvol_snk  += nf->sminn_to_denitrif; /* bulk denitrification of soil mineral N */
	ns->npool	  += nf->sminn_to_npool;
	ns->nfix_src  += nf->nfix_to_sminn;
	ns->ndep_src  += nf->ndep_to_sminn;


	/* ****************************************************************************/
	/* 3. N leaching:
		  Leaching fluxes are calculated after all the other nfluxes are reconciled to avoid the possibility of removing more N than is 
		  here. Leaching calculation based on the a function of the presumed proportion of the SMINN pool which is soluble (nitrates),
		  the SWC and the percolation */


	for (layer = 0; layer < N_SOILLAYERS; layer++)
	{
		if (wf->soilw_diffused[layer] > 0)
		{
			wflux_downward = wf->soilw_percolated[layer] + wf->soilw_diffused[layer];
			wflux_upward   = 0;

			soilwater_nconc_downward = epc->mobilen_prop * ns->sminn[layer] / ws->soilw[layer];
			soilwater_nconc_upward   = 0;
		}
		else
		{
			wflux_downward = wf->soilw_percolated[layer];
			wflux_upward   = wf->soilw_diffused[layer];

			soilwater_nconc_downward = epc->mobilen_prop * ns->sminn[layer]   / ws->soilw[layer];
			soilwater_nconc_upward   = epc->mobilen_prop * ns->sminn[layer+1] / ws->soilw[layer+1];
		}


		nf->sminn_leached[layer]  = soilwater_nconc_downward * wflux_downward;
		nf->sminn_diffused[layer] = soilwater_nconc_upward * wflux_upward;

	}

	/* STATE UPDATE */

	ns->sminn_total = 0;
	for (layer = 0; layer < N_SOILLAYERS-1; layer++)
	{
		sminn0 = ns->sminn[layer]   - (nf->sminn_leached[layer] + nf->sminn_diffused[layer]); 
		sminn1 = ns->sminn[layer+1] + (nf->sminn_leached[layer] + nf->sminn_diffused[layer]); 
		if (sminn0 < 0)
		{
			nf->sminn_leached[layer] += sminn0;
			if (fabs(sminn0) > CRIT_PREC) 
			{
				printf("WARNING: limited N-leaching (multilayer_sminn.c)\n");
			}
		}

		if (sminn1 < 0)
		{
			nf->sminn_diffused[layer] -= sminn1;
			if (fabs(sminn1) > CRIT_PREC) 
			{
				printf("WARNING: limited N-diffusion (multilayer_sminn.c)\n");
			}
		}

		ns->sminn[layer]   -= (nf->sminn_leached[layer] + nf->sminn_diffused[layer]); 
		ns->sminn[layer+1] += (nf->sminn_leached[layer] + nf->sminn_diffused[layer]); 
		ns->sminn_total    += ns->sminn[layer];

		/* CONTROL */
		if(ns->sminn[layer] < 0)
		{
			if (fabs (ns->sminn[layer]) > CRIT_PREC)
			{
				printf("FATAL ERROR: negative sminn pool (multilayer_sminn.c)\n");
				ok=0;
			}
			else
			{
				ns->nvol_snk     += ns->sminn[layer];
				ns->sminn[layer] = 0;

			}

		}
	}
	
	
	return (!ok);
}

