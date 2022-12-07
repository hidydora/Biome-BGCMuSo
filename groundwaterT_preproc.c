/* 
groundwaterT_preproc.c
calculate the effect of gound water depth in the soil

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Biome-BGCMuSo v7.0.
Copyright 2022, D. Hidy [dori.hidy@gmail.com]
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
#include "bgc_func.h"


int groundwaterT_preproc(const control_struct* ctrl, const siteconst_struct* sitec, soilprop_struct* sprop, epvar_struct* epv, 
	            wstate_struct* ws, wflux_struct* wf, groundwater_struct* gws)
{
	int layerSAT,errorCode,layer,md, year, GWlayer, CFlayer;
	double soilw_fc,soilw_sat, CFact;
	double GWboundL,GWboundU,ratio,diff;

	/*------------------------------------*/
	/* 0. initialization */
	layer=layerSAT=errorCode=0;
	ratio=diff=0;
	GWlayer=CFlayer = DATA_GAP;


	/*-----------------------------------------------------------------------------*/
	/* 0. Groundwater calculation only if GWD data is available from GW-file */

	md = gws->mgmdGWD-1;
	year = ctrl->simstartyear + ctrl->simyr;

	
	if (gws->GWD_num != 0)	
	{	
		/*-----------------------------------------------------------------------------*/
		/* 1. Determination of groundwater days */
	
		if (md >= 0 && year == gws->GWyear_array[md] && ctrl->month == gws->GWmonth_array[md] && ctrl->day == gws->GWday_array[md])
		{
			sprop->GWD = gws->GWdepth_array[md];
			
			/* GW above the surface: pond water formation (limitation: maximal pond water height */
			if (sprop->GWD < 0)
			{	
				ws->GW_waterlogging = -1*sprop->GWD*1000;
				sprop->GWD          = 0;
			}
		}
		else
		{
			sprop->GWD = 100;
			for (layer = 0; layer < N_SOILLAYERS; layer++) sprop->VWCfc[layer] = sprop->VWCfc_base[layer];
		}

		/*-----------------------------------------------------------------------------*/
		/* 3. Calculation of actual value of the depth of groundwater and capillary zone - necessary only if GWD has changed */

		if (sprop->GWD != sprop->preGWD)
		{
			/* new calculation of GW-eff */
			for (layer = 0; layer<N_SOILLAYERS; layer++) sprop->GWeff[layer] = DATA_GAP;

			/* lower and upper boundary of the laye */
			layer = N_SOILLAYERS-1;
			while (GWlayer == DATA_GAP && layer >=0)
			{
				GWboundL = sitec->soillayer_depth[layer];
				if (layer == 0)
					GWboundU  = -1*CRIT_PREC;
				else
					GWboundU  = sitec->soillayer_depth[layer-1];
				
				/* if groundwater table is in actual layer (above lower boundary): GWlayer   */
				if (sprop->GWD >= sitec->soillayer_depth[N_SOILLAYERS-1] || (sprop->GWD < GWboundL && sprop->GWD >= GWboundU)) 
				{
					/* if GW-table is under 10 meter */
					if (sprop->GWD >= sitec->soillayer_depth[N_SOILLAYERS-1])
					{
						GWlayer=N_SOILLAYERS;
						CFact=sprop->CapillFringe[N_SOILLAYERS-1];
					}
					else
					{
						GWlayer=layer;
						CFact=sprop->CapillFringe[layer];
					}
			
					/* searching of upper boundary of capillary zone */

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
								layer -= 1;
								CFact  = sprop->CapillFringe[layer] - (sprop->GWD - GWboundU);
								if (CFact < 0)
								{
									CFlayer = layer+1;
									sprop->CFD = GWboundU; 
								}
							}
							else
							{
								CFlayer = layer;
								if (CFact > 0)
									sprop->CFD = GWboundL - CFact;
								else
									sprop->CFD = GWboundL;
							}
						}
					}
					else
					{
						CFlayer = layer;
						sprop->CFD = sprop->GWD - CFact;
						if (sprop->CFD > sitec->soillayer_depth[N_SOILLAYERS-1]) CFlayer = N_SOILLAYERS;

					}

				}
				else
					layer -= 1;

			}
			
			/* define GWlayer and actual CF (maximum of capillary fringe upper layer is the top soil zone */
			if (CFlayer == DATA_GAP) 
			{
				CFlayer = 0;
				sprop->CFD = 0;
			}
			sprop->CFlayer=(double) CFlayer;
			sprop->GWlayer=(double) GWlayer;

			/* soil layers below the groundwater table are saturated - net water gain from soil system */
			for (layer = N_SOILLAYERS-1; layer >= 0; layer--)
			{
				GWboundL = sitec->soillayer_depth[layer];
			
				if (layer == 0)
					GWboundU  = -1*CRIT_PREC;
				else
					GWboundU  = sitec->soillayer_depth[layer-1];

				/* BELOW the GW-table: saturation */
				if (layer > sprop->GWlayer)
				{
					sprop->GWeff[layer]     = 1;
			
					soilw_sat               = sprop->VWCsat[layer] * sitec->soillayer_thickness[layer] * water_density;
						
					/* on first simulation day - discharge, not movchange */
					if (ctrl->simyr == 0 && ctrl->yday == 0)
						wf->GWdischarge[layer]  = (soilw_sat - ws->soilw[layer]);
					else
						wf->GWmovchange[layer]  = (soilw_sat - ws->soilw[layer]);

					ws->soilw[layer]        = soilw_sat;
					epv->VWC[layer]         = sprop->VWCsat[layer];
					sprop->VWCfc[layer]     = sprop->VWCsat[layer];
			 

				}
				/* IN and ABOVE the GW-table: saturation */
				else
				{
					/* IN the GW-layer: saturation proportionally to the GW depth, higher FC-values, plus water from GW */
					if (sprop->GWlayer == (double) layer)
					{
						/* saturation proportionally to the GW depth */
						sprop->GWeff[layer]     = (GWboundL - sprop->GWD)/sitec->soillayer_thickness[layer];
								
						/* higher FC values */
						sprop->VWCfc[layer] = sprop->VWCfc_base[layer] + sprop->GWeff[layer] * (sprop->VWCsat[layer]-sprop->VWCfc_base[layer]);
						soilw_fc            = sprop->VWCfc[layer]   * sitec->soillayer_thickness[layer] * water_density;

						/* plus water or water loss because of  GW moving - on first simulation day - discharge, not movchange */
						if (ctrl->simyr == 0 && ctrl->yday == 0)
							wf->GWdischarge[layer]   = (soilw_fc - ws->soilw[layer]);	
						else
							wf->GWmovchange[layer]   = (soilw_fc - ws->soilw[layer]);	

						ws->soilw[layer]         = soilw_fc;
						epv->VWC[layer]          = ws->soilw[layer] / (sitec->soillayer_thickness[layer] * water_density);
				
					}
					else
					{
						sprop->VWCfc[layer] = sprop->VWCfc_base[layer];
						soilw_fc            = sprop->VWCfc[layer]   * sitec->soillayer_thickness[layer] * water_density;

						/*  water loss because of  GW downward moving */
						if (sprop->preGWlayer != DATA_GAP && layer >= sprop->preGWlayer && epv->VWC[layer] > sprop->VWCfc[layer])
						{
							/* on first simulation day - discharge, not movchange */
							if (ctrl->simyr == 0 && ctrl->yday == 0)
								wf->GWmovchange[layer]   = (soilw_fc - ws->soilw[layer]);
							else
								wf->GWdischarge[layer]   = (soilw_fc - ws->soilw[layer]);

							ws->soilw[layer]         = soilw_fc;
							epv->VWC[layer]          = ws->soilw[layer] / (sitec->soillayer_thickness[layer] * water_density);
						}

						if ((double) layer >= sprop->CFlayer) 
							sprop->GWeff[layer]     = 0;
						else
							sprop->GWeff[layer]     = DATA_GAP;

					}
				}
			}
				
		}
		else
		{
			/* GW layer: GW saturates capillary zone with diffusion - GWeff */
			for (layer = N_SOILLAYERS-1; layer >= 0; layer--)
			{
				GWboundL = sitec->soillayer_depth[layer];
			
				if (layer == 0)
					GWboundU  = -1*CRIT_PREC;
				else
					GWboundU  = sitec->soillayer_depth[layer-1];
					
				/* IN the GW-layer: saturation proportionally to the GW depth, higher FC-values, plus water from GW */
				if (sprop->GWlayer == (double) layer)
				{
					soilw_fc            = sprop->VWCfc[layer]   * sitec->soillayer_thickness[layer] * water_density;

					if (ws->soilw[layer] < soilw_fc)
					{
						/* plus water or water loss because of  GW moving */
						wf->GWdischarge[layer]   = (soilw_fc - ws->soilw[layer]);		
						ws->soilw[layer]         = soilw_fc;
						epv->VWC[layer]          = ws->soilw[layer] / (sitec->soillayer_thickness[layer] * water_density);
					}
				}
			}

			/* define GWlayer and actual CF from the previous day value */
			GWlayer=(int) sprop->GWlayer;
			CFlayer=(int) sprop->CFlayer;
		}


	}
	else
	{
		sprop->GWD = DATA_GAP;
		sprop->CFD = DATA_GAP;
	}

	sprop->preGWD     = sprop->GWD;
	sprop->preGWlayer = sprop->GWlayer;


	return (errorCode);
}
