/* 
multilayer_sminn.c
Calculating the change in content of soil mineral nitrogen in multilayer soil (soil processes, nitrogen leaching, depostion and fixing)

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Biome-BGC version 4.1.1
Copyright 2000, Peter E. Thornton
Numerical Terradynamics Simulation Group (NTSG)
School of Forestry, University of Montana
Missoula, MT 59812
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

int multilayer_sminn(const siteconst_struct* sitec, const epvar_struct* epv, nstate_struct* ns, nflux_struct* nf, 
					 wstate_struct* ws, wflux_struct* wf)
{
	int ok=1;
	int layer;
	double soilwater_nconc, wflux_downward, sminn_from_top;
	double sminn_change_ctrl, weighting_factor_ctrl, sminn_SUM, sminn_RZ;
	double weighting_factor = 1;
	double sminn_toplayers = 0;

	sminn_change_ctrl = weighting_factor_ctrl = sminn_SUM = sminn_RZ = 0;

	/* ****************************************************************************/
	/* 1. Calculating the change in content of soil mineral nitrogen in multilayer soil and update state variables. 

	/* In the rootlayers the change of SMINN_RZ (soil mineral N content if rootzone) is caused by soil processes, leaching, deposition
		and nfix. The produced/consumed N (calculated by decomposition sburoutine) is divided into rootzone soil layers based on 
		their soil mineral N content.In the further soil layers (where no root can be found) N content is changed only by leaching */


	for (layer = 0; layer < epv->n_rootlayers; layer++)
	{
		/* weighting factor is calculated in order to divide the produced/consumed N into rootzone layers based on their N content */
		if (epv->n_rootlayers > 1)
		{
			weighting_factor = (ns->sminn[layer] / ns->sminn_RZ_befsoilproc);
			
			if (layer == epv->n_rootlayers-1) 
			{
				weighting_factor = (ns->sminn_RZ_befsoilproc - sminn_toplayers) / ns->sminn_RZ_befsoilproc;
			}
			
			sminn_toplayers  += ns->sminn[layer];
		}
	
	
		nf->sminn_soilproc[layer] = (ns->sminn_RZ - ns->sminn_RZ_befsoilproc) * weighting_factor;
		ns->sminn[layer]        += nf->sminn_soilproc[layer];
		sminn_change_ctrl       += nf->sminn_soilproc[layer];
		weighting_factor_ctrl   += weighting_factor;
	}
	
	/* ****************************************************************************/
	/* 2. CONTROL */
	if (fabs(sminn_change_ctrl - (ns->sminn_RZ - ns->sminn_RZ_befsoilproc)) > 1e-8)
	{
		printf("Error in sminn change calculation in multilayer_sminn.c\n");
		ok=0;
	}

	if (1.0 - weighting_factor_ctrl > 1e-8)
	{
		printf("Error in sminn change calculation in multilayer_sminn.c\n");
		ok=0;
	}

	/* ****************************************************************************/
	/* 3. N leaching flux is calculated after all the other nfluxes are reconciled to avoid the possibility of removing more N than is there. This follows
	the implicit logic of precedence for soil mineral N resources:
	A) microbial processes and plant uptake (competing)
	B) leaching (as a function of the presumed proportion of the SMINN pool which is soluble (nitrates), the SWC and the percolation */

	sminn_from_top = 0;
	for (layer = 0; layer < N_SOILLAYERS-1; layer++)
	{
		soilwater_nconc			= MOBILEN_PROPORTION * ns->sminn[layer] / ws->soilw[layer];

		wflux_downward			= wf->soilw_percolated[layer];

		nf->sminn_leached[layer]= soilwater_nconc * wflux_downward;
		
		ns->sminn[layer]        += sminn_from_top - nf->sminn_leached[layer];

		sminn_from_top			= nf->sminn_leached[layer];
	}
	ns->nleached_snk	+= nf->sminn_leached[N_SOILLAYERS-2];
	

	/* ****************************************************************************/
	/* 4. Update state variables */

	/* 4.1: soil mineral N content of rootzone */
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
			
		sminn_RZ	+= ns->sminn[epv->n_rootlayers-1] * (epv->rooting_depth - sitec->soillayer_depths[layer-1]) / sitec->soillayer_thickness[layer];
	}

	ns->sminn_RZ = sminn_RZ;

	/* 4.2: sum of the soil mineral N content of soil layers and control of negative Npools*/
	for (layer = 0; layer < N_SOILLAYERS; layer++)
	{
		if (ns->sminn[layer] < 0.0)       
		{
			printf("Fatal error: negative N content (multilayer_sminn.c)\n");
			ok=0;	
		}
		
		sminn_SUM += ns->sminn[layer];
	}
	ns->sminn_SUM = sminn_SUM;

	/* 4.3: note the valeu of sminn_RZ to calculate the soil process effect */	
	ns->sminn_RZ_befsoilproc = ns->sminn_RZ;	
		
	
	return (!ok);
}

