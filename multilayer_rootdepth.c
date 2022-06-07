/* 
multilayer_rootdepth.c
calculation of changing rooting depth based on empirical function 
*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Biome-BGCMuSo v6.0.
Copyright 2019, D. Hidy [dori.hidy@gmail.com]
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

	int errflag=0;
	int layer;

	double RLprop_sum1, RLprop_sum2, frootc;
	double vwcSAT_RZ, vwcFC_RZ, vwcWP_RZ, vwcHW_RZ, maxRD;


	/* initalizing internal variables */

	RLprop_sum1=RLprop_sum2=vwcSAT_RZ=vwcFC_RZ=vwcWP_RZ=vwcHW_RZ=0.0;

	if (sprop->soildepth < epc->max_rootzone_depth)
		maxRD = sprop->soildepth;
	else
		maxRD = epc->max_rootzone_depth;

	
	/* ***************************************************************************************************** */	
	/* 1. Calculating the number of the soil layers in which root can be found. It determines the rootzone depth (only on first day) */
	
	if (ctrl->simyr == 0 && ctrl->yday == 0)
	{
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
			errflag=1;
		}
	
		/*calculation of critical VWC values for rootzone */

		for (layer = 0; layer < epv->n_maxrootlayers; layer++)
		{
			vwcSAT_RZ += sprop->vwc_sat[layer]* (sitec->soillayer_thickness[layer] / sitec->soillayer_depth[epv->n_maxrootlayers-1]);
			vwcFC_RZ  += sprop->vwc_fc[layer] * (sitec->soillayer_thickness[layer] / sitec->soillayer_depth[epv->n_maxrootlayers-1]);
			vwcWP_RZ  += sprop->vwc_wp[layer] * (sitec->soillayer_thickness[layer] / sitec->soillayer_depth[epv->n_maxrootlayers-1]);
			vwcHW_RZ  += sprop->vwc_hw[layer] * (sitec->soillayer_thickness[layer] / sitec->soillayer_depth[epv->n_maxrootlayers-1]);
		}
		epv->vwcSAT_RZ = vwcSAT_RZ;
		epv->vwcFC_RZ  = vwcFC_RZ;
		epv->vwcWP_RZ  = vwcWP_RZ;
		epv->vwcHW_RZ  = vwcHW_RZ;
	}

	/* ***************************************************************************************************** */	
	/* 2. Calculating rooting depth in case of non-wwody ecosystems (based on Campbell and Diaz, 1988) 
	      actual rooting depth determines the rootzone depth (epv->n_rootlayers) */
	
	
	if (cs->frootc) 
	{
		frootc = cs->frootc + cs->STDBc_froot;
		if (frootc < epc->rootlenght_par1)
		{
			/* par1: root weight corresponding to max root depth, par2: root depth function shape parameter */
			epv->rooting_depth = epv->germ_depth + maxRD * pow(frootc / epc->rootlenght_par1, epc->rootlenght_par2);
		}
		else
			epv->rooting_depth = epv->germ_depth + maxRD;
	}
	else
		epv->rooting_depth = 0;

	if (epc->woody) epv->rooting_depth = maxRD;

	/* ***************************************************************************************************** */	
	/* 3. Calculating the number of the soil layers in which root can be found. It determines the rootzone depth (epv->n_rootlayers) */
	
	if (epv->rooting_depth > 0)
	{
		if (epv->rooting_depth > sitec->soillayer_depth[0])
		{
			if (epv->rooting_depth > sitec->soillayer_depth[1])
			{	
				if (epv->rooting_depth > sitec->soillayer_depth[2])
				{
					if (epv->rooting_depth > sitec->soillayer_depth[3])
					{
						if (epv->rooting_depth > sitec->soillayer_depth[4])
						{
							if (epv->rooting_depth > sitec->soillayer_depth[5])
							{
								if (epv->rooting_depth > sitec->soillayer_depth[6])
								{
									if (epv->rooting_depth > sitec->soillayer_depth[7])
									{
										if (epv->rooting_depth > sitec->soillayer_depth[8])
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
		if (cs->frootc)
		{
			printf("\n");
			printf("ERROR in multilayer_rootdepth: root is available but rooting depth is 0\n");
			errflag=1;
		}
	}
	
	/* ***************************************************************************************************** */	
	/* 4. Calculating the distribution of the root in the soil layers based on empirical function (Jarvis, 1989)*/
	
	/* calculation in active soil layer from 2 active soil layers */
	
	for (layer = 0; layer < N_SOILLAYERS; layer++)
	{
		if (layer < epv->n_rootlayers && layer >= epv->germ_layer)
		{
			epv->rootlength_prop[layer]   = epc->rootdistrib_param * (sitec->soillayer_thickness[layer] / epv->rooting_depth) * 
 												  exp(-epc->rootdistrib_param * (sitec->soillayer_midpoint[layer] / epv->rooting_depth));
			RLprop_sum1 += epv->rootlength_prop[layer];
		}
		else
			epv->rootlength_prop[layer]   = 0;
	}

	if (RLprop_sum1 == 0) 
	{
		epv->rootlength_prop[epv->germ_layer] = 1;
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
			errflag=1;
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

	


	return(errflag);
}

