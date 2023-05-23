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


int groundwaterT_preproc(const control_struct* ctrl, const epconst_struct* epc, const siteconst_struct* sitec, soilprop_struct* sprop, epvar_struct* epv, 
	            wstate_struct* ws, wflux_struct* wf, groundwater_struct* GWS)
{
	int layerSAT,errorCode,layer,md, year, GWlayer, CFlayer;
	double soilw_fc,soilw_sat, CFact;
	double GWboundL,GWboundU,ratio, VWCnew;

	/*------------------------------------*/
	/* Initialization */
	
	layer=layerSAT=errorCode=0;
	GWlayer=CFlayer = DATA_GAP;

	md = GWS->mgmdGWD-1;
	year = ctrl->simstartyear + ctrl->simyr;

	
	/*------------------------------------*/
	/* I. relative soil water content (SAT-FCbase) */

	for (layer=0; layer < N_SOILLAYERS; layer++)
	{
		
		/* 1. relative soil water content (SAT-FCbase) */
		if (sprop->VWCsat[layer] - sprop->VWCfc_base[layer] > CRIT_PRECwater)
			epv->relVWCsat_fc[layer] = (epv->VWC[layer] - sprop->VWCfc_base[layer]) / (sprop->VWCsat[layer] - sprop->VWCfc_base[layer]);
		else
			epv->relVWCsat_fc[layer] = 1;
		if (fabs(epv->relVWCsat_fc[layer]) < CRIT_PRECwater) epv->relVWCsat_fc[layer] = 0;

	}	
	
	/*------------------------------------*/
	/* II. Groundwater calculation only if GWD data is available from GW-file */

	if (GWS->GWD_num != 0)	
	{	
		/*-----------------------------------------------------------------------------*/
		/* 1. Determination of groundwater days */
	
		if (md >= 0 && year == GWS->GWyear_array[md] && ctrl->month == GWS->GWmonth_array[md] && ctrl->day == GWS->GWday_array[md])
		{
			sprop->GWD = GWS->GWdepth_array[md];
			
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
		/* 2. Calculation of actual value of the depth of groundwater and capillary zone and GWmovchange - necessary only if GWD has changed */

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
						if (sprop->CFD >= sitec->soillayer_depth[N_SOILLAYERS-1]) CFlayer = N_SOILLAYERS;

					}

				}
				else
					layer -= 1;

			}
			
			/* 3. define GWlayer and actual CF (maximum of capillary fringe upper layer is the top soil zone */
			if (CFlayer == DATA_GAP) 
			{
				CFlayer = 0;
				sprop->CFD = 0;
			}
			sprop->CFlayer=(double) CFlayer;
			sprop->GWlayer=(double) GWlayer;

		
			/* 4. soil layers below the groundwater table are saturated - net water gain from soil system */
			for (layer = N_SOILLAYERS-1; layer >= 0; layer--)
			{
				GWboundL = sitec->soillayer_depth[layer];
			
				if (layer == 0)
					GWboundU  = -1*CRIT_PREC;
				else
					GWboundU  = sitec->soillayer_depth[layer-1];

				/* 4.1 BELOW the GW-table: saturation */
				if (layer > sprop->GWlayer)
				{
					sprop->GWeff[layer]     = 1;
					sprop->CFeff[layer]     = 0;
			
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
				/* 4.2 IN and ABOVE the GW-table: saturation */
				else
				{
					/* 4.2.1 IN the GW-layer: saturation proportionally to the GW depth, higher FC-values, plus water from GW */
					if (sprop->GWlayer == (double) layer)
					{
						/* saturation proportionally to the GW depth */
						sprop->GWeff[layer]     = (GWboundL - sprop->GWD)/sitec->soillayer_thickness[layer];
						if (sprop->CFlayer == (double) layer)
							sprop->CFeff[layer] = (sprop->GWD - sprop->CFD)/sitec->soillayer_thickness[layer];
						else
							sprop->CFeff[layer] = 1- sprop->GWeff[layer];

						/* higher FC */
						ratio = sprop->GWeff[layer]+sprop->CFeff[layer];
						if (ratio > 1 || ratio < 0)
						{
							printf("ERROR with higher FC calculation, groundwaterT_preproc()\n");
							errorCode = 1;
						}

						sprop->VWCfc[GWlayer] = sprop->VWCfc_base[GWlayer] * (1-ratio) + sprop->VWCsat[GWlayer] * ratio;
						
						/* plus water or water loss because of  GW moving - on first simulation day - discharge, not movchange */
						VWCnew = sprop->GWeff[layer] * sprop->VWCsat[layer] + (1-sprop->GWeff[layer]) * epv->VWC[layer];
					
						if (VWCnew > sprop->VWCfc[layer]) VWCnew = sprop->VWCfc[layer];
						
						if (ctrl->simyr == 0 && ctrl->yday == 0)
							wf->GWdischarge[layer]   = (VWCnew - epv->VWC[layer]) * sitec->soillayer_thickness[layer] * water_density;		
						else
							wf->GWmovchange[layer]   = (VWCnew - epv->VWC[layer]) * sitec->soillayer_thickness[layer] * water_density;	

						
						epv->VWC[layer]          = VWCnew;
						ws->soilw[layer]         = VWCnew * sitec->soillayer_thickness[layer] * water_density;
	
					}
					/* 4.2.2 ABOVE GW-table */
					else
					{
						sprop->GWeff[layer]   = 0;

						/* 4.2.2.1 CF-layer */
						if (sprop->CFlayer <= (double) layer)
						{
							if (sprop->CFlayer == (double) layer)
								sprop->CFeff[layer] = (GWboundL - sprop->CFD)/sitec->soillayer_thickness[layer];
							else
								sprop->CFeff[layer] = 1;

							sprop->VWCfc[layer] = sprop->VWCfc_base[layer] * (1-sprop->CFeff[layer]) + sprop->VWCsat[layer] * sprop->CFeff[layer];
							
						}
						/* 4.2.2.2 normal zone */
						else
						{
							sprop->GWeff[layer] = DATA_GAP;
							sprop->CFeff[layer] = DATA_GAP;
							sprop->VWCfc[layer] = sprop->VWCfc_base[layer];
						}

						/*  water loss because of  GW downward moving */
						if (sprop->preGWlayer != DATA_GAP && layer >= sprop->preGWlayer && sprop->preGWD > sprop->GWD && epv->VWC[layer] > sprop->VWCfc[layer])
						{
							soilw_fc            = sprop->VWCfc[layer]   * sitec->soillayer_thickness[layer] * water_density;

							/* on first simulation day - discharge, not movchange */
							if (ctrl->simyr == 0 && ctrl->yday == 0)
								wf->GWdischarge[layer]   = (soilw_fc - ws->soilw[layer]);
							else
								wf->GWmovchange[layer]   = (soilw_fc - ws->soilw[layer]);	
							
							ws->soilw[layer]         = soilw_fc;
							epv->VWC[layer]          = ws->soilw[layer] / (sitec->soillayer_thickness[layer] * water_density);
						}

					}
				}
			}
				
		}
		else
		{
			/* define GWlayer and actual CF from the previous day value */
			GWlayer=(int) sprop->GWlayer;
			CFlayer=(int) sprop->CFlayer;
		}

		/* ------------------------------------------------------------- */
		/* 5. determination of real capillary layers: potentially capillary zone is real CF if relSWC in layer below is higher than actual */

		
		for (layer = GWlayer-1; layer >= CFlayer; layer--)
		{	
			if (layer < N_SOILLAYERS-1 && (sprop->VWCsat[layer+1] - epv->VWC[layer+1]) > CRIT_PRECwater)
			{
				sprop->CFeff[layer] = -1;
				sprop->VWCfc[layer] = sprop->VWCfc_base[layer];
			}
		}
		
	
	}
	else
	{
		sprop->GWD = DATA_GAP;
		sprop->CFD = DATA_GAP;
	}
	

		
	/*------------------------------------*/
	/* III. relative soil water content (FC-WP) */

	for (layer=0; layer < N_SOILLAYERS; layer++)
	{
		
		if (sprop->VWCfc[layer] - sprop->VWCwp[layer] > CRIT_PRECwater)
			epv->relVWCfc_wp[layer] = (epv->VWC[layer] - sprop->VWCwp[layer]) / (sprop->VWCfc[layer] - sprop->VWCwp[layer]);
		else
		{
			printf("ERROR with relative VWC calculation, groundwaterT_preproc()\n");
			errorCode = 1;
		}
		if (fabs(epv->relVWCfc_wp[layer]) < CRIT_PRECwater) epv->relVWCfc_wp[layer] = 0;

		
		epv->VWC_SScrit1[layer] = sprop->VWCwp[layer] + epc->VWCratio_SScrit1 * (sprop->VWCfc[layer] - sprop->VWCwp[layer]);
		epv->VWC_SScrit2[layer] = sprop->VWCfc[layer] + epc->VWCratio_SScrit2 * (sprop->VWCsat[layer] - sprop->VWCfc[layer]);
	}	

	

	sprop->preGWD     = sprop->GWD;
	sprop->preGWlayer = sprop->GWlayer;


	return (errorCode);
}
