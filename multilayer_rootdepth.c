/* 
multilayer_rootdepth.c
calculation of changing rooting depth based on empirical function 
*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Biome-BGCMuSo v6.1.
Copyright 2020, D. Hidy [dori.hidy@gmail.com]
Hungarian Academy of Sciences, Hungary
See the website of Biome-BGCMuSo at http://nimbus.elte.hu/bbgc/ for documentation, model executable and example input files.
*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <malloc.h>
#include "ini.h"
#include "bgc_struct.h"
#include "bgc_func.h"
#include "bgc_constants.h"

int multilayer_rootdepth(const control_struct* ctrl, const phenology_struct* phen, const epconst_struct* epc, const siteconst_struct* sitec, 
	                    const soilprop_struct* sprop, const cstate_struct* cs, const planting_struct* PLT, epvar_struct* epv)
{

	int errorCode=0;
	int layer;

	double RLprop_sum1, RLprop_sum2, RLprop_sum0, frootc;
	double maxRD;


	/* initalizing internal variables */

	RLprop_sum1=RLprop_sum2=RLprop_sum0=0.0;

	if (sprop->soildepth < epc->max_rootzone_depth)
		maxRD = sprop->soildepth;
	else
		maxRD = epc->max_rootzone_depth;

	
	/* ***************************************************************************************************** */	
	/* 1. Calculating the number of the soil layers in which root can be found. It determines the rootzone depth (only on first day) */
	
	if (maxRD > 0)
	{
		if (maxRD > sitec->soillayer_depth[0])
		{
			if (maxRD > sitec->soillayer_depth[1])
			{	
				if (maxRD > sitec->soillayer_depth[2])
				{
					if (maxRD > sitec->soillayer_depth[3])
					{
						if (maxRD > sitec->soillayer_depth[4])
						{
							if (maxRD > sitec->soillayer_depth[5])
							{
								if (maxRD > sitec->soillayer_depth[6])
								{
									if (maxRD > sitec->soillayer_depth[7])
									{
										if (maxRD > sitec->soillayer_depth[8])
										{
												epv->n_maxrootlayers = 10;
										}
										else 
										{
											epv->n_maxrootlayers = 9;
										}
									}
									else 
									{
										epv->n_maxrootlayers = 8;
									}
								}
								else 
								{
									epv->n_maxrootlayers = 7;
								}
							}
							else 
							{
								epv->n_maxrootlayers = 6;
							}
						}
						else 
						{
							epv->n_maxrootlayers = 5;
						}
					}
					else
					{
						epv->n_maxrootlayers = 4;
					}
				}
				else 
				{
					epv->n_maxrootlayers = 3;
				}	
			}
			else 
			{
				epv->n_maxrootlayers = 2;
			}
		}
		else
		{
			epv->n_maxrootlayers = 1;
		}
	}
	else 
	{
		epv->n_maxrootlayers = 0;
		printf("\n");
		printf("ERROR in multilayer_rootdepth: maximum of rooting depth is 0\n");
		errorCode=1;
	}
	


	/* ***************************************************************************************************** */	
	/* 2. Calculating rooting depth in case of non-wwody ecosystems (based on Campbell and Diaz, 1988) 
	      actual rooting depth determines the rootzone depth (epv->n_rootlayers) */
	
	frootc = cs->frootc;
	if (frootc > CRIT_PREC) 
	{
		if (frootc < epc->rootlength_par1)
		{
			/* par1: root weight corresponding to max root depth, par2: root depth function shape parameter */
			epv->rootdepth = epv->germ_depth + (maxRD-epv->germ_depth) * pow(frootc / epc->rootlength_par1, epc->rootlength_par2);
		}
		else
			epv->rootdepth = epv->germ_depth + (maxRD-epv->germ_depth);

		
	}
	else
		epv->rootdepth = 0;

	if (epc->woody) epv->rootdepth = maxRD;

	if (epv->germ_layer)
		epv->rootlength = epv->rootdepth - sitec->soillayer_depth[epv->germ_layer-1];
	else
		epv->rootlength = epv->rootdepth;

	/* ***************************************************************************************************** */	
	/* 3. Calculating the number of the soil layers in which root can be found. It determines the rootzone depth (epv->n_rootlayers) */
	
	if (epv->rootdepth > 0)
	{
		if (epv->rootdepth > sitec->soillayer_depth[0])
		{
			if (epv->rootdepth > sitec->soillayer_depth[1])
			{	
				if (epv->rootdepth > sitec->soillayer_depth[2])
				{
					if (epv->rootdepth > sitec->soillayer_depth[3])
					{
						if (epv->rootdepth > sitec->soillayer_depth[4])
						{
							if (epv->rootdepth > sitec->soillayer_depth[5])
							{
								if (epv->rootdepth > sitec->soillayer_depth[6])
								{
									if (epv->rootdepth > sitec->soillayer_depth[7])
									{
										if (epv->rootdepth > sitec->soillayer_depth[8])
										{	
											epv->n_rootlayers = 10;		
										}
										else 
										{
											epv->n_rootlayers = 9;
										}
									}
									else 
									{
										epv->n_rootlayers = 8;
									}
								}
								else 
								{
									epv->n_rootlayers = 7;
								}
							}
							else 
							{
								epv->n_rootlayers = 6;
							}
						}
						else 
						{
							epv->n_rootlayers = 5;
						}
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
		if (cs->frootc > CRIT_PREC)
		{
			printf("\n");
			printf("ERROR in multilayer_rootdepth: root is available but rooting depth is 0\n");
			errorCode=1;
		}
	}
	
	/* ***************************************************************************************************** */	
	/* 4. Calculating the distribution of the root in the soil layers based on empirical function (Jarvis, 1989)*/
	

	for (layer = 0; layer < N_SOILLAYERS; layer++)
	{
		/* live root distribution for soil water calculation */
		if (layer < epv->n_rootlayers && layer >= epv->germ_layer)
		{
			epv->rootlength_prop[layer]   = epc->rootdistrib_param * (sitec->soillayer_thickness[layer] / epv->rootlength) * 
 												  exp(-epc->rootdistrib_param * (sitec->soillayer_midpoint[layer] / epv->rootlength));
			RLprop_sum1 += epv->rootlength_prop[layer];

		}
		else
			epv->rootlength_prop[layer]   = 0;

		/* dead root distribution for soil water calculation */
		if (epv->n_maxrootlayers)
		{
			if (layer < epv->n_maxrootlayers-1)
				epv->rootlengthLandD_prop[layer] = sitec->soillayer_thickness[layer] / maxRD;
			else
			{
				if (layer == epv->n_maxrootlayers-1) 
					epv->rootlengthLandD_prop[layer] = (maxRD-sitec->soillayer_depth[layer-1]) / maxRD;
				else
					epv->rootlengthLandD_prop[layer] = 0;
			}
			 
		}
		else
			epv->rootlengthLandD_prop[layer]   = 0;
		
		RLprop_sum0 += epv->rootlengthLandD_prop[layer];
	}

	if ((epv->n_maxrootlayers && fabs(1-RLprop_sum0) > CRIT_PREC) || (!epv->n_maxrootlayers && RLprop_sum0 != 0)) 
	{
		printf("\n");
		printf("ERROR in multilayer_rootdepth: sum of soillayer_RZportion is 0.0\n");
		errorCode=1;
	}

	if (RLprop_sum1 == 0) 
	{
		if (epv->rootdepth)
		{
			printf("\n");
			printf("ERROR in multilayer_rootdepth: sum of soillayer_RZportion is 0.0\n");
			errorCode=1;
		}
	}

	/* correction */
	if (RLprop_sum1)
	{
		for (layer =0; layer < N_SOILLAYERS; layer++)
		{	
			epv->rootlength_prop[layer] = epv->rootlength_prop[layer] / RLprop_sum1;
			RLprop_sum2                 += epv->rootlength_prop[layer];
		}

		if (fabs(1. - RLprop_sum2) > 1e-8)
		{
			printf("\n");
			printf("ERROR in multilayer_rootdepth: sum of soillayer_RZportion is not equal to 1.0\n");
			errorCode=1;
		}
	}
	

	



	/* ***************************************************************************************************** */	
	/* 5. calculation of plant height (based on 4M)*/

	/* par1: stem weight corresponding to max plant height, par2: plant height function shape parameter */
	if (epc->woody)
	{
		epv->plant_height = epc->max_plant_height*(1-exp((-5/epc->plantheight_par1)*(cs->livestemc+cs->deadstemc)));
	}
	else
	{
		
		epv->plant_height = epc->max_plant_height * pow(((cs->softstemc+cs->STDBc_softstem)/epc->plantheight_par1),epc->plantheight_par2);
		
	}
	if (epv->plant_height > epc->max_plant_height) epv->plant_height = epc->max_plant_height;

	


	return(errorCode);
}

