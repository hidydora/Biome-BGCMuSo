 /*
tipping.c
Tipping model for INFILT simulation()

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
#include <math.h>
#include <malloc.h>
#include "ini.h"
#include "bgc_struct.h"
#include "bgc_func.h"
#include "bgc_constants.h"
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

int tipping(siteconst_struct* sitec, soilprop_struct* sprop, const epconst_struct* epc, epvar_struct* epv, wstate_struct* ws, wflux_struct* wf)
{

	int errorCode=0;
	int layer, ll;
	double INFILT;

	double VWC, soilw_sat1, soilw1;
	double EXCESS, HOLD, DRAIN, DRMX,  conduct_sat, soilwDiffus_act;
	double VWC0, VWC0_sat, VWC0_fc, VWC0_wp, VWC1, VWC1_sat, VWC1_fc, VWC1_wp, rVWC0, rVWC1;

	double m_to_cm, mm_to_cm, dz0, dz1;

	double DRN[N_SOILLAYERS]; /* drainage rate throug soil layer (cm/day) */
	double GWR[N_SOILLAYERS]; /* groundwater recharge (cm/day) */
	double soilwPercol[N_SOILLAYERS];
	double soilwDiffus[N_SOILLAYERS];
	double soilw_limit = 0;


	m_to_cm   = 100;
	mm_to_cm  = 0.1;

	for  (layer=0 ; layer<N_SOILLAYERS; layer++) GWR[layer]=0;


	/* --------------------------------------------------------------------------------------------------------------------*/
	/* 1.PERCOLATION */

	INFILT = wf->infiltPOT * mm_to_cm;
	
	/* -----------------------------*/
	/* 1.1. rainy days */
	if (INFILT > 0)
	{

		/* 1.1.1. BEGIN LOOP: layer */
		for  (layer=0 ; layer<N_SOILLAYERS; layer++) 
		{
	
			VWC = epv->VWC[layer];
			dz0 = sitec->soillayer_thickness[layer]  * m_to_cm;
	
			/* saturated hydraulic conductivity in actual layer (cm/day = m/s * 100 * sec/day) */
			conduct_sat = sprop->hydrCONDUCTsat[layer] * m_to_cm * nSEC_IN_DAY;
	
	
			/* [cm = m3/m3 * cm */
			HOLD = (sprop->VWCsat[layer] - VWC) * dz0;
			
		
			/* 1.1.2.  IF: INFILT > HOLD */
			if (INFILT > 0.0 && INFILT > HOLD)
			{
				/* drainage from soil profile [cm = m3/m3 * cm ] */
				DRAIN = sprop->drainCoeff[layer] * (sprop->VWCsat[layer] - sprop->VWCfc[layer]) * dz0;
				
				/* drainage rate throug soil layer (cm/day) */
				DRN[layer] = INFILT - HOLD + DRAIN;

			
				/* drainage is limited: cm/h * h/day */
				if ((DRN[layer] - conduct_sat) > 0.0) 
				{
					DRN[layer] = conduct_sat;
					DRAIN      = DRN[layer] + HOLD - INFILT;
				}

				/* Capillary zone: no drainage only out of capillary zone, only if it is saturated */	
				if (sprop->CFeff[layer] > 0 && sprop->GWeff[layer] < 0 && DRN[layer] > 0)
				{	
					soilw_limit = (epv->VWC[layer]- sprop->VWCfc[layer]) * sitec->soillayer_thickness[layer] * water_density;
					if (soilw_limit > 0)
						DRN[layer] = MIN(DRN[layer], soilw_limit);
					else
						DRN[layer] = 0;

				}
				
				/* GW-zone: no drainage only out of groundwater zone, only if it is saturated (GW-recharge) */	
				if (sprop->GWeff[layer] > 0 && DRN[layer] > 0)
				{
					soilw_limit = (epv->VWC[layer]- sprop->VWCfc[layer]) * sitec->soillayer_thickness[layer] * water_density;
				
					if (soilw_limit > 0) GWR[layer] = MIN(DRN[layer], soilw_limit);
					
					DRN[layer] = 0;
				}

			
				/* state update temporal varialbe */
				VWC = VWC + (INFILT - DRN[layer] - GWR[layer])/dz0;

				/* above saturation - */
				if (VWC >= sprop->VWCsat[layer])
				{
					
					EXCESS=(VWC - sprop->VWCsat[layer])*dz0;
					VWC=sprop->VWCsat[layer];
					
					/* if there is excess water, redistribute it in layers above */
					if (EXCESS > 0)
					{
						for (ll=layer-1; ll>=0; ll--)
						{
							dz1						= sitec->soillayer_thickness[ll] * mm_to_cm;
							soilw_sat1				= sprop->VWCsat[ll] * sitec->soillayer_thickness[ll] * water_density * mm_to_cm;
							soilw1					= epv->VWC[ll]       * sitec->soillayer_thickness[ll] * water_density * mm_to_cm;
							HOLD					= MIN(soilw_sat1-soilw1, EXCESS);
							ws->soilw[ll]			+= HOLD/mm_to_cm;
							epv->VWC[ll]			= ws->soilw[ll]/sitec->soillayer_thickness[ll]/water_density;
							DRN[ll]					= MAX(DRN[ll]-EXCESS, 0.0);
							EXCESS					= EXCESS - HOLD;

						}
						/* if too much pondwater -> runoff */
						wf->soilw_to_pondw += EXCESS / mm_to_cm;;
					}		
				}

				INFILT = DRN[layer];
			
			} /* END IF: INFILT > HOLD */
			else
			{ /* 1.1.3. BEGIN ELSE: INFILT < HOLD */

				VWC = VWC + INFILT/dz0;

				
				/* BEGIN IF-ELSE: VWC > FC */
				if (VWC >= sprop->VWCfc[layer])
				{

				   DRAIN = (VWC - sprop->VWCfc[layer]) * sprop->drainCoeff[layer] * dz0;
			
				   	/* drainage rate throug soil layer (cm/day) */
				   DRN[layer] = DRAIN;
				
				   /* drainage is limited */
				   if ((DRN[layer] - conduct_sat) > 0.0) 
				   {
					  DRN[layer] = conduct_sat;
					  DRAIN = DRN[layer];
				   }

				  	/* Capillary zone: no drainage only out of capillary zone, only if it is saturated */	
					if (sprop->CFeff[layer] > 0 && DRN[layer] > 0)
					{	
						if (layer == 0)
							soilw_limit = (epv->VWC[layer]- sprop->VWCfc[layer]) * sitec->soillayer_thickness[layer] * water_density + INFILT;
						else
							soilw_limit = (epv->VWC[layer]- sprop->VWCfc[layer]) * sitec->soillayer_thickness[layer] * water_density + DRN[layer-1];


						if (soilw_limit > 0)
							DRN[layer] = MIN(DRN[layer], soilw_limit);
						else
							DRN[layer] = 0;

					}
				
					/* GW-zone: no drainage only out of groundwater zone, only if it is saturated (GW-recharge) */	
					if (sprop->GWeff[layer] > 0 && DRN[layer] > 0)
					{
						if (layer == 0)
							soilw_limit = (epv->VWC[layer]- sprop->VWCfc[layer]) * sitec->soillayer_thickness[layer] * water_density + INFILT;
						else
							soilw_limit = (epv->VWC[layer]- sprop->VWCfc[layer]) * sitec->soillayer_thickness[layer] * water_density + DRN[layer-1];
				
						if (soilw_limit > 0) GWR[layer] = MIN(DRN[layer], soilw_limit);
					
						DRN[layer] = 0;
					}
			
             
				   VWC  = VWC - DRN[layer] / dz0 - GWR[layer]/dz0;
				   INFILT = DRN[layer];
				}
				else
				{
				   INFILT = 0.0;
				   DRN[layer]   = 0.0;

				} /* END IF-ELSE: VWC > FC */

			} /* END ELSE: INFILT < HOLD */

			/* water flux: cm/day to kg/(m2*day) */
			soilwPercol[layer]    = (DRN[layer] / m_to_cm) * water_density;
			wf->GWrecharge[layer] = (GWR[layer] / m_to_cm) * water_density;

	
			/* state update: with new VWC calcualte soilw */
			epv->VWC[layer]=VWC;
			ws->soilw[layer]=epv->VWC[layer] * sitec->soillayer_thickness[layer] * water_density;
		
		
		} /* END FOR (layer) */

		
	}
	/* -----------------------------*/
	else /* 1.2. rainless days */
	{
		
		/* BEGIN LOOP: VWCsat flow */
		for (layer=0; layer<N_SOILLAYERS; layer++)   
		{

			VWC = epv->VWC[layer];
			dz0 = sitec->soillayer_thickness[layer]  * m_to_cm;

			/* saturated hydraulic conductivity in actual layer (cm/day = m/s * 100 * sec/day) */
			conduct_sat = sprop->hydrCONDUCTsat[layer] * m_to_cm * nSEC_IN_DAY;


			if (VWC  > sprop->VWCfc[layer])
			{
				DRMX = (VWC-sprop->VWCfc[layer]) * sprop->drainCoeff[layer] * dz0;
				DRMX = MAX(0.0,DRMX);

				DRMX = MAX((VWC-sprop->VWCfc[layer]) * sprop->drainCoeff[layer] * dz0,0);
	
			}
			else
				DRMX = 0;

			/* BEGIN IF-ELSE: layer == 0 */
			if (layer == 0)
			{
				DRN[layer] = DRMX;
			}
			else
			{

				if (epv->VWC[layer] < sprop->VWCfc[layer])
					HOLD = (sprop->VWCfc[layer] - epv->VWC[layer]) * dz0;
				else
					HOLD = 0.0;
				
				DRN[layer] = MAX(DRN[layer-1] + DRMX - HOLD,0.0);
			
		
			} 	/* BEGIN IF-ELSE: layer == 0 */

			if (DRN[layer] < CRIT_PRECwater) DRN[layer]=0;
			

			/* Capillary zone: no drainage only out of capillary zone, only if it is saturated */	
			if (sprop->CFeff[layer] > 0 && DRN[layer] > 0)
			{	
				if (layer == 0)
					soilw_limit = (epv->VWC[layer]- sprop->VWCfc[layer]) * sitec->soillayer_thickness[layer] * water_density + INFILT;
				else
					soilw_limit = (epv->VWC[layer]- sprop->VWCfc[layer]) * sitec->soillayer_thickness[layer] * water_density + DRN[layer-1];

				
				if (soilw_limit > 0)
					DRN[layer] = MIN(DRN[layer], soilw_limit);
				else
					DRN[layer] = 0;

			}
				
			/* GW-zone: no drainage only out of groundwater zone, only if it is saturated (GW-recharge) */	
			if (sprop->GWeff[layer] > 0 && DRN[layer] > 0)
			{
				if (layer == 0)
					soilw_limit = (epv->VWC[layer]- sprop->VWCfc[layer]) * sitec->soillayer_thickness[layer] * water_density + INFILT;
				else
					soilw_limit = (epv->VWC[layer]- sprop->VWCfc[layer]) * sitec->soillayer_thickness[layer] * water_density + DRN[layer-1];

				
				if (soilw_limit > 0) GWR[layer] = MIN(DRN[layer], soilw_limit);
					
				DRN[layer] = 0;
			}

			/* limitation of drainage: saturation conductivity */
		   if ((DRN[layer] - conduct_sat) > 0.0) DRN[layer] = conduct_sat;
		  
		  
 
		} /* END LOOP: VWCsat flow */

		for (layer=N_SOILLAYERS-1; layer >= 0; layer--)
		{
			
				VWC = epv->VWC[layer];
				dz0 = sitec->soillayer_thickness[layer]  * m_to_cm;


				if (layer > 0)
				{
					VWC = epv->VWC[layer] + (DRN[layer-1]-DRN[layer]-GWR[layer])/ dz0 ;
					if (VWC > sprop->VWCsat[layer])
					{
						DRN[layer-1] = (sprop->VWCsat[layer] - epv->VWC[layer])*dz0 + DRN[layer] + GWR[layer];
						VWC          = sprop->VWCsat[layer];
					}
				}
				else
				{
					VWC = epv->VWC[layer] - ((DRN[layer] + GWR[layer])/ dz0);
				}

				if (DRN[layer] < CRIT_PRECwater) DRN[layer]=0;
			
                 /* water flux: cm/day to kg/(m2*day) */
			    soilwPercol[layer] = (DRN[layer] / m_to_cm) * water_density;
				wf->GWrecharge[layer] = (GWR[layer] / m_to_cm) * water_density;

				/* state update: with new VWC calcualte soilw */
				epv->VWC[layer]  = VWC;
				ws->soilw[layer] = epv->VWC[layer] * sitec->soillayer_thickness[layer] * water_density;
				
			
		}
	}

	
	/* --------------------------------------------------------------------------------------------------------*/
	/* 2. UPWARD WATER MOVEMENT - based on 4M method  */

	if (epc->SHCM_flag == 0)
	{
		for (layer=N_SOILLAYERS-2; layer>=0; layer--)
		{
			dz0      = sitec->soillayer_thickness[layer];
			VWC0     = epv->VWC[layer];
			rVWC0    = epv->relVWCfc_wp[layer];
			VWC0_sat = sprop->VWCsat[layer];
			VWC0_fc  = sprop->VWCfc[layer];
			VWC0_wp  = sprop->VWCwp[layer];
	
			dz1      = sitec->soillayer_thickness[layer+1];
			VWC1     = epv->VWC[layer+1];
			rVWC1    = epv->relVWCfc_wp[layer+1];
			VWC1_sat = sprop->VWCsat[layer+1];
			VWC1_fc  = sprop->VWCfc[layer+1];
			VWC1_wp  = sprop->VWCwp[layer+1];

			/* capillary layer has higher FC only relative to lower layers  */
			if (sprop->CFeff[layer] < 0)
			{
				rVWC1    = (epv->VWC[layer+1] - sprop->VWCwp[layer+1]) / (sprop->VWCfc_base[layer+1] - sprop->VWCwp[layer+1]);
				VWC1_fc  = sprop->VWCfc_base[layer+1];
			}



			if (!errorCode && diffusCalc(sprop, dz0, VWC0, rVWC0, VWC0_sat, VWC0_fc, VWC0_wp, dz1, VWC1, rVWC1, VWC1_sat, VWC1_fc, VWC1_wp, &soilwDiffus_act))
			{
				printf("\n");
				printf("ERROR: diffusion() in tipping.c\n");
				errorCode = 1;
			}

			if (fabs(soilwDiffus_act) < CRIT_PRECwater) soilwDiffus_act=0;

			soilwDiffus[layer]   = soilwDiffus_act;

			/* GW-effect: source of diffusion in GW layer is GW */
			if (soilwDiffus[layer] < 0 && sprop->GWeff[layer+1] == 1)
			{
				wf->GWdischarge[layer+1] += -1*soilwDiffus[layer];			
				ws->soilw[layer]    -= soilwDiffus[layer];
			}
			else
			{
				ws->soilw[layer]    -= soilwDiffus[layer];
				ws->soilw[layer+1]  += soilwDiffus[layer];
			}
			
		
			epv->VWC[layer]      =  ws->soilw[layer]   / dz0 / water_density;
			epv->VWC[layer+1]    =  ws->soilw[layer+1] / dz1 / water_density;
		}


	}
	else
	{
		for (layer=0; layer<N_SOILLAYERS-1; layer++) soilwDiffus[layer] = 0;
	}


	
	/* ********************************/
	/* 6. BOTTOM LAYER IS SPECIAL 	*/

	/* if no GW, water from above flows through the layer */
	if (sprop->GWeff[N_SOILLAYERS-1] == DATA_GAP)
	{
		soilwDiffus[N_SOILLAYERS-1] = soilwDiffus[N_SOILLAYERS-2];
	}
	else
	{
		/* if GW: no percolation, but if layer is saturated: GWrecharge */
		if (fabs(epv->VWC[N_SOILLAYERS-1]-sprop->VWCsat[N_SOILLAYERS-1]) < CRIT_PRECwater)
		{
			if (soilwDiffus[N_SOILLAYERS-2] > 0)
				wf->GWrecharge[N_SOILLAYERS-2] += soilwDiffus[N_SOILLAYERS-2];
		}

		soilwDiffus[N_SOILLAYERS-1] = 0;
	}

	ws->soilw[N_SOILLAYERS-1]   -= soilwDiffus[N_SOILLAYERS-1];
	epv->VWC[N_SOILLAYERS-1]     = ws->soilw[N_SOILLAYERS-1]   / sitec->soillayer_thickness[N_SOILLAYERS-1]   / water_density;
	
	/* ********************************/
	/* calculation of net water transport */
	for (layer=0; layer<N_SOILLAYERS; layer++) wf->soilwFlux[layer]=soilwPercol[layer]+soilwDiffus[layer];











	return (errorCode);

}
