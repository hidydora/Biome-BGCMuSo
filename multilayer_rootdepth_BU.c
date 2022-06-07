/* 
multilayer_rootdepth.c
Hidy 2011 - calculation of changing rooting depth based on empirical function
*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Biome-BGC version vHD2
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

int multilayer_rootdepth(const control_struct* ctrl, const siteconst_struct* sitec, const phenology_struct* phen, 
						 epvar_struct* epv, nstate_struct* ns)
{


	int ok=1;
	double plant_day, matur_day;
	double maturity_coeff = 0.5;

	/* soil mineral N in changing rooting zone */
	double sminn_RZ = 0;
	double sminn_SUM = 0;
	int layer;

	/* important dayw of year */
	int yday   = ctrl->yday;
	int onday  = phen->onday_GSI;
	int offday = phen->offday_GSI;
	
	/* ***************************************************************************************************** */	
	/* 1. calculating planting date and maturity date (Campbell and Diaz) based on empirical function */

	plant_day = onday;
	matur_day = onday + maturity_coeff * (offday - onday);
	
	/* ***************************************************************************************************** */	
	/* 2. calculating rooting depth (based on Campbell and Diaz, 1988) and number of the soil layers in which root can be find 
			the actual length of the root (rooting_depth) which determines the rootzone depth (epv->n_rootlayers) */
	
	if (yday < offday) 
	{
		epv->rooting_depth = sitec->max_rootzone_depth * (1./(1 + 44.2 * exp(-8.5*((yday - plant_day)/(matur_day - plant_day)))));
	}
	else 
	{
		epv->rooting_depth = sitec->max_rootzone_depth - (yday - offday)/(NDAY_OF_YEAR - offday) * sitec->max_rootzone_depth;
	}

	/* ***************************************************************************************************** */	
	/* 3. calculating the number of the soil layers in which root can be found. It determines the rootzone depth (epv->n_rootlayers) */
	
	if (epv->rooting_depth > 0)
	{
		if (epv->rooting_depth > sitec->soillayer_depths[0])
		{
			if (epv->rooting_depth > sitec->soillayer_depths[1])
			{	
				if (epv->rooting_depth > sitec->soillayer_depths[2])
				{
					if (epv->rooting_depth > sitec->soillayer_depths[3])
					{
						epv->n_rootlayers = 5;
					}
					else
					{
						epv->n_rootlayers = 4;
					}
				}
				else 
				{
					epv->n_rootlayers = 3;
				}	
			}
			else 
			{
				epv->n_rootlayers = 2;
			}
		}
		else
		{
			epv->n_rootlayers = 1;
		}
	}
	else 
	{
		epv->n_rootlayers = 0;
		printf("Error in multilayer_rootdepth: maximum of rooting depth is 0\n");
		ok=0;
	}

	/* ***************************************************************************************************** */	
	/* 4. calculating the the active portion of the given soillayer (according to rooting depth) */
	
	

	for (layer =0; layer < N_SOILLAYERS; layer++)
	{
		if (layer < epv->n_rootlayers)
		{
			if (layer == epv->n_rootlayers-1)
			{
				epv->soillayer_RZportion[layer] = (epv->rooting_depth - sitec->soillayer_depths[layer-1]) / epv->rooting_depth;
			}
			else 
			{
				epv->soillayer_RZportion[layer] = sitec->soillayer_thickness[layer] / epv->rooting_depth;
			}
		}
		else 
		{
			epv->soillayer_RZportion[layer] = 0.;
		}
	}
	

	/* ***************************************************************************************************** */	
	/* 5. calculating the soil mineral N content of rooting zone taking into account changing rooting depth 
		R: SUM of N elimitated/added to rootzone Ncontent because of the decrease/increase of rootzone depth */
	
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
				
	ns->sminn_RZ		     = sminn_RZ;
	ns->sminn_RZ_befsoilproc = sminn_RZ;


	return(!ok);
}

