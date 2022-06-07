/* 
groundwaterRICHARDS.c
calculate the effect of gound water depth in the soil

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Biome-BGCMuSo v6.2.
Copyright 2020, D. Hidy [dori.hidy@gmail.com]
Hungarian Academy of Sciences, Hungary
See the website of Biome-BGCMuSo at http://nimbus.elte.hu/bbgc/ for documentation, model executable and example input files.
*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "ini.h"
#include "bgc_struct.h"
#include "pointbgc_struct.h"
#include "pointbgc_func.h"
#include "bgc_constants.h"



int groundwaterRICHARDS(const control_struct* ctrl, const siteconst_struct* sitec, soilprop_struct* sprop, epvar_struct* epv, 
	            wstate_struct* ws, wflux_struct* wf, groundwater_struct* gws, GWcalc_struct* gwc)
{
	int errorCode,layer,layerGWC,n_layerGWC, md, year, GWlayer, CFlayer,  l;
	double GWboundL,GWboundU, ratio, GWdischarge, soilw_sat0, CFact, ctrl_rootlengthProp_GWC;	
	
	
	layer=layerGWC=errorCode=n_layerGWC=0;

	GWlayer=CFlayer = DATA_GAP;

	ctrl_rootlengthProp_GWC= 0;

	/* critical epv->VWC-difference */
	md = gws->mgmdGWD-1;
	year = ctrl->simstartyear + ctrl->simyr;

	

	/* initialization */
	sprop->CapillFringe_act = 0;
	if (ctrl->simyr == 0 && ctrl->yday == 0) sprop->GWDpre = DATA_GAP;

	
	/*A. if gound water depth information is available using in multilayer calculation */
	if (gws->GWD_num != 0)	
	{	
		if (md >= 0 && year == gws->GWyear_array[md] && ctrl->month == gws->GWmonth_array[md] && ctrl->day == gws->GWday_array[md])
			sprop->GWD = gws->GWdepth_array[md];
		else
			sprop->GWD = 100;


		/* 1. GW above the surface */
		if (sprop->GWD < 0)
		{
			wf->GW_to_pondw      = -1*sprop->GWD*1000;
			sprop->GWD           = 0;
		}

		if (sprop->GWD != sprop->GWDpre)
		{
			/* 2. define GWlayer and actual CF */
			layer = N_SOILLAYERS-1;
			while (GWlayer == DATA_GAP && layer >=0)
			{
				GWboundL = sitec->soillayer_depth[layer];
				if (layer == 0)
					GWboundU  = -1*CRIT_PREC;
				else
					GWboundU  = sitec->soillayer_depth[layer-1];
				
				/* 2.1 if groundwater table is in actual layer (above lower boundary): GWlayer  AND searching of upper boundary of capillary zone */
				if (sprop->GWD <= GWboundL && sprop->GWD > GWboundU) 
				{
					GWlayer=layer;
			
					/* 2.1.2 below the upper boundary of capillary zone */
					CFact=sprop->CapillFringe[layer];
					if (sprop->GWD - CFact <= GWboundU)
					{
					
						layer -= 1;
						CFact  = sprop->CapillFringe[layer] - (sprop->GWD - GWboundU);
						while (CFlayer == DATA_GAP && layer >=0)
						{
							GWboundL = sitec->soillayer_depth[layer];
							if (layer == 0)
								GWboundU  = -1*CRIT_PREC;
							else
								GWboundU  = sitec->soillayer_depth[layer-1];

							if (GWboundL - CFact <= GWboundU)
							{
								CFact  -= (GWboundL - GWboundU);
								layer -= 1;
							}
							else
							{
								CFlayer = layer;
								sprop->CFD = GWboundL - CFact;
								sprop->CapillFringe_act = sprop->GWD - sprop->CFD;
							}
						}
					}
					else
					{
						CFlayer = layer;
						sprop->CFD = sprop->GWD - CFact;
						sprop->CapillFringe_act = CFact;
					}

				}
				else
					layer -= 1;

			}

			/* 3.  calculation of VWC and soil properties */
			layerGWC = N_SOILLAYERS_GWC-1;
			layer=N_SOILLAYERS-1;
			while (layerGWC>=0)
			{
				GWboundL = sitec->soillayer_depth[layer];
				if (layer == 0)
					GWboundU  = -1*CRIT_PREC;
				else
					GWboundU  = sitec->soillayer_depth[layer-1]; 

				/* 3.1 below GW-table */
				if (GWboundL > sprop->GWD)
				{
					gwc->soillayer_depthGWC[layerGWC] = GWboundL;
					gwc->VWC_GWC[layerGWC]            = epv->VWC[layer];
					gwc->VWCsat_GWC[layerGWC]         = sprop->VWCsat[layer];
					gwc->VWCfc_GWC[layerGWC]          = sprop->VWCfc[layer];
					gwc->VWChw_GWC[layerGWC]          = sprop->VWChw[layer];
					gwc->hydrCONDUCTsat_GWC[layerGWC] = sprop->hydrCONDUCTsat[layer];
					gwc->PSIsat_GWC[layerGWC]         = sprop->PSIsat[layer];
					gwc->soilB_GWC[layerGWC]          = sprop->soilB[layer];
					gwc->CapillFringe_GWC[layerGWC]   = sprop->CapillFringe[layer];
					gwc->HYDROflag[layerGWC]          = 2;

					layerGWC -= 1;
					layer -= 1;
				}
				/* 3.2 above GW-table */
				else
				{
					/* 3.2.1 GW-layer */
					if (GWboundU < sprop->GWD)
					{
						gwc->soillayer_depthGWC[layerGWC] = sprop->GWD;
						gwc->VWC_GWC[layerGWC]            = epv->VWC[layer+1];
						gwc->VWCsat_GWC[layerGWC]         = sprop->VWCsat[layer+1];
						gwc->VWCfc_GWC[layerGWC]          = sprop->VWCfc[layer+1];
						gwc->VWChw_GWC[layerGWC]          = sprop->VWChw[layer+1];
						gwc->hydrCONDUCTsat_GWC[layerGWC] = sprop->hydrCONDUCTsat[layer+1];
						gwc->PSIsat_GWC[layerGWC]         = sprop->PSIsat[layer+1];
						gwc->soilB_GWC[layerGWC]          = sprop->soilB[layer+1];
						gwc->CapillFringe_GWC[layerGWC]   = sprop->CapillFringe[layer+1];
						gwc->HYDROflag[layerGWC]          = 1;

						/* 3.2.2 above the GW-layer */
						layerGWC -= 1;
						l= layer;
						while (l >=0)
						{
							/* 3.2.2.1 below the upper layer of capillary zone (CFD) */
							if (sitec->soillayer_depth[l] > sprop->CFD)
							{
								gwc->soillayer_depthGWC[layerGWC] = sitec->soillayer_depth[l];
								gwc->VWC_GWC[layerGWC]            = epv->VWC[l];
								gwc->VWCsat_GWC[layerGWC]         = sprop->VWCsat[l];
								gwc->VWCfc_GWC[layerGWC]          = sprop->VWCfc[l];
								gwc->VWChw_GWC[layerGWC]          = sprop->VWChw[l];
								gwc->hydrCONDUCTsat_GWC[layerGWC] = sprop->hydrCONDUCTsat[l];
								gwc->PSIsat_GWC[layerGWC]         = sprop->PSIsat[l];
								gwc->soilB_GWC[layerGWC]          = sprop->soilB[l];
								gwc->CapillFringe_GWC[layerGWC]   = sprop->CapillFringe[l];
								gwc->HYDROflag[layerGWC]          = 1;

								layerGWC -= 1;
								l -= 1;
							}
							/* 3.2.2.2 in and above the upper layer of capillary zone (CFD) */
							else
							{
								gwc->soillayer_depthGWC[layerGWC] = sprop->CFD;
								gwc->VWC_GWC[layerGWC]            = epv->VWC[l+1];
								gwc->VWCsat_GWC[layerGWC]         = sprop->VWCsat[l+1];
								gwc->VWCfc_GWC[layerGWC]          = sprop->VWCfc[l+1];
								gwc->VWChw_GWC[layerGWC]          = sprop->VWChw[l+1];
								gwc->hydrCONDUCTsat_GWC[layerGWC] = sprop->hydrCONDUCTsat[l+1];
								gwc->PSIsat_GWC[layerGWC]         = sprop->PSIsat[l+1];
								gwc->soilB_GWC[layerGWC]          = sprop->soilB[l+1];
								gwc->CapillFringe_GWC[layerGWC]   = sprop->CapillFringe[l+1];
								gwc->HYDROflag[layerGWC]          =	0;

								CFlayer=l+1;
								layerGWC -= 1;
								while (l >= 0)
								{
									gwc->soillayer_depthGWC[layerGWC] = sitec->soillayer_depth[l];
									gwc->VWC_GWC[layerGWC]            = epv->VWC[l];
									gwc->VWCsat_GWC[layerGWC]         = sprop->VWCsat[l];
									gwc->VWCfc_GWC[layerGWC]          = sprop->VWCfc[l];
									gwc->VWChw_GWC[layerGWC]          = sprop->VWChw[l];
									gwc->hydrCONDUCTsat_GWC[layerGWC] = sprop->hydrCONDUCTsat[l];
									gwc->PSIsat_GWC[layerGWC]         = sprop->PSIsat[l];
									gwc->soilB_GWC[layerGWC]          = sprop->soilB[l];
									gwc->CapillFringe_GWC[layerGWC]   = sprop->CapillFringe[l];
									gwc->HYDROflag[layerGWC]          = 0;
									layerGWC -= 1;
									l -= 1;
								}
							} /* end of 3.2.2.2 in and above the upper layer of capillary zone (CFD) */
						}/* end of 3.2.2 above the GW-layer */
					} /* end of 3.2.1 GW-layer */
				} /* end of 3.2 above GW-table */

			} /* end of 3. calculation of VWC and soil properties */

	
			/* 4. calculation of soillayer thickness, soilw in GWC arrays */
			layer = 0;
			for (layerGWC = 0; layerGWC < N_SOILLAYERS_GWC; layerGWC++)
			{
				if (layerGWC > 0) 
					gwc->soillayer_thicknessGWC[layerGWC] = gwc->soillayer_depthGWC[layerGWC] - gwc->soillayer_depthGWC[layerGWC-1];
				else
					gwc->soillayer_thicknessGWC[layerGWC] = gwc->soillayer_depthGWC[layerGWC];

				/* 4.1. same bottom layer */
				if (sitec->soillayer_depth[layer] == gwc->soillayer_depthGWC[layerGWC])
				{
					/* top layer: always same upper layer */
					if (layerGWC == 0)
						gwc->soilw_GWC[layerGWC]          = ws->soilw[layer];
					else
					{
						/* same upper layer */
						if (sitec->soillayer_depth[layer-1] == gwc->soillayer_depthGWC[layerGWC-1])
							gwc->soilw_GWC[layerGWC]          = ws->soilw[layer];
					
						/* different upper layer */
						else
						{
							ratio                             = gwc->soillayer_thicknessGWC[layerGWC]/sitec->soillayer_thickness[layer];
							gwc->soilw_GWC[layerGWC]          = ws->soilw[layer] * ratio;
						}
					}
					layer = layer+1;
				}
				/* 4.2 different bottom layer */
				else
				{
					ratio                             = gwc->soillayer_thicknessGWC[layerGWC]/sitec->soillayer_thickness[layer];
					gwc->soilw_GWC[layerGWC]          = ws->soilw[layer] * ratio;
				}

				/* 4.3 initialization of GW 	 */
				if (gwc->HYDROflag[layerGWC] == 2)
				{
					soilw_sat0=gwc->VWCsat_GWC[layerGWC] * gwc->soillayer_thicknessGWC[layerGWC] * water_density;
					if (soilw_sat0 > gwc->soilw_GWC[layerGWC])
					{
						GWdischarge               = soilw_sat0 - gwc->soilw_GWC[layerGWC];
						gwc->soilw_GWC[layerGWC]  = soilw_sat0;
						ws->groundwater_src      += GWdischarge;
						gwc->VWC_GWC[layerGWC]    = gwc->VWCsat_GWC[layer];
					}
				}
		
			} /* end of 4. calculation of soillayer thickness, soilw, rootlength proportion in GWC arrays */

		
			sprop->GWlayer=(double) GWlayer;
			sprop->CFlayer=(double) CFlayer;
		}

	}
	/* B. no ground water depth information is available using in multilayer calculation */
	else
	{
		sprop->GWD = DATA_GAP;
		sprop->GWlayer=DATA_GAP;
		sprop->CFlayer=DATA_GAP;

		for (layer = 0; layer < 8; layer++) 
		{
			gwc->soillayer_depthGWC[layer]     = sitec->soillayer_depth[layer];
			gwc->VWC_GWC[layer]                = epv->VWC[layer];
			gwc->VWCsat_GWC[layer]             = sprop->VWCsat[layer];
			gwc->VWCfc_GWC[layer]              = sprop->VWCfc[layer];
			gwc->VWChw_GWC[layer]              = sprop->VWChw[layer];
			gwc->hydrCONDUCTsat_GWC[layer]     = sprop->hydrCONDUCTsat[layer];
			gwc->PSIsat_GWC[layer]             = sprop->PSIsat[layer];
			gwc->soilB_GWC[layer]              = sprop->soilB[layer];
			gwc->HYDROflag[layer]              = 0;
			gwc->soillayer_thicknessGWC[layer] = sitec->soillayer_thickness[layer];
			gwc->soilw_GWC[layer]	           = ws->soilw[layer];
			gwc->rootlengthProp_GWC[layer]     = epv->rootlengthProp[layer];
		}
		
		for (layerGWC = 8; layerGWC < N_SOILLAYERS_GWC; layerGWC++) 
		{

			gwc->VWC_GWC[layerGWC]						= epv->VWC[layer];
			gwc->VWCsat_GWC[layerGWC]					= sprop->VWCsat[layer];
			gwc->VWCfc_GWC[layerGWC]					= sprop->VWCfc[layer];
			gwc->VWChw_GWC[layerGWC]					= sprop->VWChw[layer];
			gwc->hydrCONDUCTsat_GWC[layerGWC]			= sprop->hydrCONDUCTsat[layer];
			gwc->PSIsat_GWC[layerGWC]					= sprop->PSIsat[layer];
			gwc->soilB_GWC[layerGWC]					= sprop->soilB[layer];
			gwc->HYDROflag[layerGWC]					= 0;
			ratio                                       = 1;
			
			if (layerGWC == 8)
			{
				gwc->soillayer_depthGWC[layerGWC]       = 3;
				gwc->soillayer_thicknessGWC[layerGWC]   = 1;
				ratio                                   = gwc->soillayer_thicknessGWC[layerGWC]/sitec->soillayer_thickness[layer];
				gwc->soilw_GWC[layerGWC]				= ws->soilw[layer] * ratio;
				gwc->rootlengthProp_GWC[layerGWC]		= epv->rootlengthProp[layer] * ratio;
			}

			if (layerGWC == 9 || layerGWC == 11)
			{
				gwc->soillayer_depthGWC[layerGWC]       = sitec->soillayer_depth[layer];
				gwc->soillayer_thicknessGWC[layerGWC]   = gwc->soillayer_depthGWC[layerGWC] - gwc->soillayer_depthGWC[layerGWC-1];
				ratio                                   = gwc->soillayer_thicknessGWC[layerGWC]/sitec->soillayer_thickness[layer];
				gwc->soilw_GWC[layerGWC]				= ws->soilw[layer] * ratio;
				gwc->rootlengthProp_GWC[layerGWC]		= epv->rootlengthProp[layer] * ratio;
				layer                                  += 1;
			}
			
			if (layerGWC == 10)
			{
				gwc->soillayer_depthGWC[layerGWC]       = 7;
				gwc->soillayer_thicknessGWC[layerGWC]   = 3;
				ratio                                   = gwc->soillayer_thicknessGWC[layerGWC]/sitec->soillayer_thickness[layer];
				gwc->soilw_GWC[layerGWC]				= ws->soilw[layer] * ratio;
				gwc->rootlengthProp_GWC[layerGWC]		= epv->rootlengthProp[layer] * ratio;
			}

			
		
			
		}

	}

	/* calculation of rootlength proportion */
	layer = 0;
	for (layerGWC = 0; layerGWC < N_SOILLAYERS_GWC; layerGWC++)
	{
		/* 4.1. same bottom layer */
		if (sitec->soillayer_depth[layer] == gwc->soillayer_depthGWC[layerGWC])
		{
			/* top layer: always same upper layer */
			if (layerGWC == 0) 
				gwc->rootlengthProp_GWC[layerGWC] = epv->rootlengthProp[layer];
			
			else
			{
				/* same upper layer */
				if (sitec->soillayer_depth[layer-1] == gwc->soillayer_depthGWC[layerGWC-1])
					gwc->rootlengthProp_GWC[layerGWC] = epv->rootlengthProp[layer];
				/* different upper layer */
				else
				{
					ratio                             = gwc->soillayer_thicknessGWC[layerGWC]/sitec->soillayer_thickness[layer];
					gwc->rootlengthProp_GWC[layerGWC] = epv->rootlengthProp[layer] * ratio;
				}
			}
			layer = layer+1;
		}
		/* 4.2 different bottom layer */
		else
		{
			ratio                             = gwc->soillayer_thicknessGWC[layerGWC]/sitec->soillayer_thickness[layer];
			gwc->rootlengthProp_GWC[layerGWC] = epv->rootlengthProp[layer] * ratio;
		}
		ctrl_rootlengthProp_GWC += gwc->rootlengthProp_GWC[layerGWC];
	}

	if (ctrl_rootlengthProp_GWC > 0 && fabs(ctrl_rootlengthProp_GWC - 1) > CRIT_PREC)
	{
		printf("ERROR in rootlenght proportion calculation in GWC (groundwaterRICHADRS.c)\n");
		errorCode=1;
	}

	sprop->GWDpre=sprop->GWD;

	return (errorCode);
}
