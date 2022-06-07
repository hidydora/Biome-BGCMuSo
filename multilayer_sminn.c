/* 
multilayer_sminn.c
Calculating the change in content of soil mineral nitrogen in multilayer soil (plant N upate, soil processes, nitrogen leaching, 
depostion and fixing). State update of sminn_RZ (mineral N content of rootzone).

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
BBGC MuSo 2.3
Copyright 2014, D. Hidy
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

int multilayer_sminn(const epconst_struct* epc, const siteconst_struct* sitec, const epvar_struct* epv, 
					 nstate_struct* ns, nflux_struct* nf, wstate_struct* ws, wflux_struct* wf)
{
	int ok=1;
	int layer=0;
	double soilwater_nconc, wflux_downward, sminn_from_top;
	double sminn_SOILPROC_SUM, sminn_RZ;
	double sminn_to_soil_ctrl, diff;

	sminn_SOILPROC_SUM = sminn_to_soil_ctrl = sminn_RZ = 0;



	/* ***************************************************************************************************** */	
	/* 1. SOILPROC:
		1.A.Plant N uptake from SMINN (sminn_to_npool is determined in daily_allocation routine) - 
	    1.B.Decomposition: due microbial soil processes SMINN (soil mineral N content) is changing in the soil (determined in daily_allocation).
	    The produced/consumed N is divided between soil layers based on their N content. */

	sminn_SOILPROC_SUM= 0;

	if (ns->sminn_RZ > 0)
	{
		for (layer = 0; layer < epv->n_rootlayers-1; layer++)
		{	
			nf->sminn_to_soil[layer] = (nf->sminn_to_soil_SUM + nf->sminn_to_npool) * (ns->sminn[layer]/ns->sminn_RZ);
			sminn_SOILPROC_SUM += nf->sminn_to_soil[layer];
		}
	}
	else nf->sminn_to_soil[layer] = 0;


	/* the rest comes from the last layer */
	nf->sminn_to_soil[epv->n_rootlayers-1] = (nf->sminn_to_soil_SUM + nf->sminn_to_npool) - sminn_SOILPROC_SUM;
	sminn_SOILPROC_SUM                     += nf->sminn_to_soil[epv->n_rootlayers-1];

	diff = ns->sminn[epv->n_rootlayers-1] - nf->sminn_to_soil[epv->n_rootlayers-1];


	/* calculation control */
	diff = (nf->sminn_to_soil_SUM + nf->sminn_to_npool) - sminn_SOILPROC_SUM;
	if (fabs(diff) > CRIT_PREC)          
	{
		printf("Error in sminn change calculation in multilayer_sminn.c\n");
		ok=0;
	}
	

	/* ***************************************************************************************************** */	
	/* 2. State update SMINN: soil processes (decomposition + plant uptake), deposition and fixation */

	for (layer = 0; layer < epv->n_rootlayers; layer++)
	{
		/* 1. soil processes (decomposition + plant uptake) */
		diff = ns->sminn[layer] - nf->sminn_to_soil[layer];
	
		if (diff < 0.0 && fabs(diff) > CRIT_PREC)       
		{
 			printf("Fatal error: negative N content (multilayer_sminn.c)\n");
			ok = 0;
		}
		else
		{
			ns->sminn[layer] -= nf->sminn_to_soil[layer];
		}
		/* 2. deposition: only in top soil layer */
		if (layer == 0) ns->sminn[0]     += nf->ndep_to_sminn;

		/* 3. fixation: based on the quantity of the root mass in the given layer */
		ns->sminn[layer] += nf->nfix_to_sminn * epv->rootlength_prop[layer];
	
	}

	ns->npool     += nf->sminn_to_npool;
	ns->nfix_src  += nf->nfix_to_sminn;
	ns->ndep_src  += nf->ndep_to_sminn;

	nf->sminn_to_soil_SUM = 0;

	/* ****************************************************************************/
	/* 3. N leaching:
		  Leaching fluxes are calculated after all the other nfluxes are reconciled to avoid the possibility of removing more N than is 
		  here. Leaching calculation based on the a function of the presumed proportion of the SMINN pool which is soluble (nitrates),
		  the SWC and the percolation */

	sminn_from_top = 0;
	for (layer = 0; layer < N_SOILLAYERS-1; layer++)
	{
		soilwater_nconc			= epc->mobilen_prop * ns->sminn[layer] / ws->soilw[layer];
 
		wflux_downward			= wf->soilw_percolated[layer];

		nf->sminn_leached[layer]= soilwater_nconc * wflux_downward;
		
		ns->sminn[layer]        += sminn_from_top - nf->sminn_leached[layer];

		
		/* control */
		if (ns->sminn[layer] < 0.0 && fabs(ns->sminn[layer]) > CRIT_PREC)       
		{
			nf->sminn_leached[layer] += ns->sminn[layer];
			ns->sminn[layer]  = 0;
 			printf("Limited N leaching (multilayer_sminn.c)\n");	
		}

		sminn_from_top			= nf->sminn_leached[layer];
	
	}
	ns->nleached_snk	+= nf->sminn_leached[N_SOILLAYERS-2];
	
	/* ***************************************************************************************************** */	
	/* 4. Calculating the soil mineral N content of rooting zone taking into account changing rooting depth 
		  N elimitated/added to rootzone Ncontent because of changes of the soil layer's N content */
	
	sminn_RZ = 0;
	if (epv->n_rootlayers == 1)
	{
		sminn_RZ = ns->sminn[0];
	}
	else
	{
		for (layer = 0; layer < epv->n_rootlayers-1; layer++)
		{
			sminn_RZ	+= ns->sminn[layer];
		}	
		sminn_RZ	+= ns->sminn[epv->n_rootlayers-1] * (epv->rooting_depth - sitec->soillayer_depth[layer-1]) / sitec->soillayer_thickness[layer];
	}
	ns->sminn_RZ	  = sminn_RZ;



	
	return (!ok);
}

