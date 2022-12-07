/* 
richards.c
calculation of soil water content layer by layer taking into account soil hydrological processes 
(percolation, diffusion)  using inamic discretization level in order to taking account the change of the diffusivity and conductivity depending on soil moisture 
(after rain event the soil moisture rises suddenly causing high diffusivity and conductivity. If we assumed that these diffusivity 
 and conductivity values were valid all the day we would overestimate the velocity diffusion and percolation process) 

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Biome-BGCMuSo v6.4.
Copyright 2022, D. Hidy [dori.hidy@gmail.com]
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
#include "bgc_constants.h"
#include "bgc_func.h"
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))   

int richards(const epconst_struct* epc, soilprop_struct* sprop, wstate_struct* ws, wflux_struct* wf, GWcalc_struct* gwc)
{

	
	/* internal variables */
	int errorCode, l;
	double INFILT,INFILT_sum, INFILT_act,INFILT_ctrl,EVAP,EVAP_act,EVAP_ctrl,TRANSP_ctrl;
	double transpDEM[N_SOILLAYERS_GWC],transpDEM_act[N_SOILLAYERS_GWC];
	double VWCequilib,diffus_limit;
	double diffus_ctrl,VWC0_ctrl,VWC1_ctrl,D0_ctrl,D1_ctrl,Dact_ctrl;

	/* diffusion and percolation calculation */
	double D0, D1, Dact;	        /* hydrological diffusion coefficient (m2/s) */

	int layer, n_second, n_sec, discretlevel;

	double VWCdiff_max, VWCdiff, diff, diff1, diff2, soilw_satUPPER, extra, extra_act;
	double localvalue,  exponent_discretlevel;
	double soilw0, VWC0, VWC1, VWChw0,VWCsat0, VWCsat1, PSIsat0, PSIsat1, soilB0, soilB1, soilw_sat0, soilw1, soilw_sat1;
	double Ksat0, Ksat1, Kact, Kact0, Kact1;

	double pondw_act, infilt_limit,waterFromAbove, evap;
	double soilw_to_pondw, infilt_to_soilw, infilt_to_pondw, pondwEvap, soilwEvap, pondw_to_soilw;

	double dz0, dz1;
	double transp[N_SOILLAYERS_GWC],diffus[N_SOILLAYERS_GWC],percol[N_SOILLAYERS_GWC],wflux[N_SOILLAYERS_GWC],GWdischarge[N_SOILLAYERS_GWC],GWrecharge[N_SOILLAYERS_GWC];
	
	errorCode = 0;
	
	
	INFILT=EVAP=INFILT_ctrl=EVAP_act=EVAP_ctrl=TRANSP_ctrl=0;
	pondw_to_soilw=soilw_to_pondw=infilt_to_soilw=infilt_to_pondw=pondwEvap=soilwEvap=pondw_act=0;
	
	ws->timestepRichards=0;

	/* --------------------------------------------------------------------------------------------------------*/
	/* I. INITALIZATION */
	
	n_second=0;
	n_sec=1;
	discretlevel = epc->discretlevel_Richards + 3; /* discretization level: change in VWC 1% -> timestep: second */
	
	INFILT_sum = (wf->prcp_to_soilw + wf->snoww_to_soilw + wf->canopyw_to_soilw + wf->IRG_to_prcp);
	INFILT     = INFILT_sum;
	INFILT_act = INFILT/nSEC_IN_DAY;
	
	pondw_act = ws->pondw;

	EVAP         = wf->soilwEvap_POT;
	EVAP_act     = EVAP/nSEC_IN_DAY;

	for (layer=0 ; layer < N_SOILLAYERS_GWC; layer++) 
	{
		gwc->soilwTranspDemand_GWC[layer]  = wf->soilwTranspDemand_SUM * gwc->rootlengthProp_GWC[layer]; 
		transpDEM[layer]                   = gwc->soilwTranspDemand_GWC[layer];
		transpDEM_act[layer]               = transpDEM[layer] /nSEC_IN_DAY;
		gwc->soilwTransp_GWC[layer]        = 0;
		gwc->soilwFlux_GWC[layer]          = 0;
		gwc->GWdischarge_GWC[layer]        = 0;
		gwc->GWrecharge_GWC[layer]         = 0;

		transp[layer]=0;
		diffus[layer]=0;
		percol[layer]=0;
		wflux[layer]=0;
		GWdischarge[layer]=0;
		GWrecharge[layer]=0;
	}
	
	

	/* --------------------------------------------------------------------------------------------------------*/	
	/* II. CALCULATE PROCESSES WITH DYNAMIC TIME STEP LAYER TO LAYER (except of bottom layer) */

	while (n_second < nSEC_IN_DAY)
	{
	
		VWCdiff_max=0;

	
		/* ----------------------------------------*/
		/* 1. CALCULATE PROCESSES  LAYER TO LAYER */
		for (layer=0 ; layer < N_SOILLAYERS_GWC; layer++)
		{	
			
			/* -------------------*/
			/* 1.0. INITALIZATION */
			dz0        = gwc->soillayer_thicknessGWC[layer];
			soilw0     = gwc->soilw_GWC[layer];
			soilw1     = gwc->soilw_GWC[layer+1];
			VWC0       = gwc->VWC_GWC[layer];
			VWChw0     = gwc->VWChw_GWC[layer];
			VWCsat0    = gwc->VWCsat_GWC[layer];
			PSIsat0    = gwc->PSIsat_GWC[layer];		
			soilB0     = gwc->soilB_GWC[layer];		
			soilw_sat0 = gwc->VWCsat_GWC[layer]   * dz0 * water_density;

			Ksat0 = gwc->hydrCONDUCTsat_GWC[layer];

			if (layer < N_SOILLAYERS_GWC-1)
			{
				dz1        = gwc->soillayer_thicknessGWC[layer+1];
				VWC1       = gwc->VWC_GWC[layer+1];
				VWCsat1    = gwc->VWCsat_GWC[layer+1];
				PSIsat1    = gwc->PSIsat_GWC[layer+1];
				soilB1     = gwc->soilB_GWC[layer+1];
				Ksat1      = gwc->hydrCONDUCTsat_GWC[layer+1];
				soilw1     = gwc->soilw_GWC[layer+1];
				soilw_sat1 = gwc->VWCsat_GWC[layer+1] * dz1 * water_density;
			}
			else
			{
				soilw1     = 0;
				soilw_sat1 = 0;
			}
			
		

			/* ------------------- */
			/* 1.1. INFILTRATION */
  			if (layer == 0) 
			{
 				if (pondw_act == 0)
				{
					if ((soilw0 + INFILT_act - soilw_sat0) > CRIT_PRECwater)			
					{
						infilt_to_soilw     = soilw_sat0 - soilw0;
						infilt_to_pondw     = INFILT_act - infilt_to_soilw;
					}
					else
					{
						infilt_to_soilw     = INFILT_act;
						infilt_to_pondw     = 0;
					}
					pondw_to_soilw = 0; 
				}
				else
				{
					infilt_to_pondw = INFILT_act;
					infilt_to_soilw = 0;
						
					infilt_limit = soilw_sat0-soilw0;
					pondw_to_soilw = MIN(pondw_act, infilt_limit);
				
				}

				/* -------------------*/
				/* 1.2. EVAPORATION */
  		

				if (EVAP_act > pondw_act) /* potential evaporation > pondw -> both pondw evaporation */
				{
					pondwEvap = pondw_act;
					soilwEvap = EVAP_act - pondwEvap;
				}
				else /* potential evaporation < pondw -> only pondw is evaporated */
				{
					pondwEvap = EVAP_act;
					soilwEvap = 0;
				}

			}

			/* -------------------*/
			/* 1.3. TRANSPIRATION */

			/* transpiration based on rootlenght proportion */
			transp[layer] = transpDEM_act[layer]; 

			/* -------------------*/
			/* 1.4. PERCOLATION */

						
			if (layer < N_SOILLAYERS_GWC-1)
			{
				/* conductivity coefficient - theoretical upper limit: saturation value */
				Kact0 = Ksat0 * pow(VWC0/VWCsat0, 2*(soilB0)+3);
				Kact1 = Ksat1 * pow(VWC1/VWCsat1, 2*(soilB1)+3);

				Kact=(Kact0 * dz0/(dz0+dz1) + Kact1 * dz1/(dz0+dz1));
				Kact=Kact0;

				/* percolation flux */
				percol[layer] = Kact * water_density * n_sec; // kg/m2
			}
			else
				percol[layer]=0;


			/* -------------------*/
			/* 1.5. DIFFUSION */
		
	
			if (layer < N_SOILLAYERS_GWC-1)
			{
				/* diffusivity coefficient 	*/
				D0 = (((soilB0    * Ksat0   * (-100*PSIsat0)))) * pow(VWC0/VWCsat0,  soilB0 +2);
			
				D1 = (((soilB1    * Ksat1   * (-100*PSIsat1)))) * pow(VWC1/VWCsat1,  soilB1 +2);

				Dact = (D0 * dz0/(dz0+dz1) + D1 * dz1/(dz0+dz1));
	
				/* diffusion flux - control (recalculation of diffusion coefficient  */
				if (fabs(VWC0 - VWC1) > CRIT_PRECwater) 
				{
					diffus_ctrl = (VWC0 - VWC1)/((dz0+dz1)/2) 	* Dact  * water_density * n_sec;
					VWC0_ctrl = VWC0 - diffus_ctrl;
					VWC1_ctrl = VWC1 + diffus_ctrl;
					D0_ctrl = (((soilB0    * Ksat0   * (-100*PSIsat0)))) * pow(VWC0_ctrl/VWCsat0,  soilB0 +2);
					D1_ctrl = (((soilB1    * Ksat1   * (-100*PSIsat1)))) * pow(VWC1_ctrl/VWCsat1,  soilB1 +2);
					Dact_ctrl = ((D0_ctrl * dz0/(dz0+dz1) + D1_ctrl * dz1/(dz0+dz1)) + Dact)/2;
				}
		
				/* diffusion flux  */
				if (fabs(VWC0 - VWC1) > CRIT_PRECwater) 
					diffus[layer]  = (VWC0 - VWC1)/((dz0+dz1)/2) 	* Dact  * water_density * n_sec;
				else
					diffus[layer]  = 0;

				/* limit of diffusion: balanced soil moisture  */
				VWCequilib = (VWC0 * dz0 + VWC1 * dz1) / (dz0+dz1);
				diffus_limit = (VWCequilib - VWC1) * dz1 * water_density;
				if (fabs(diffus[layer]) > CRIT_PRECwater && fabs(diffus[layer]) > fabs(diffus_limit)) diffus[layer]  = diffus_limit;
			}
			else
				diffus[layer]  = 0;

			
			/* -------------------*/
			/* 1.6 GW-effect */
			
			
			/* water from above */
			if (layer == 0)
			{
				waterFromAbove = infilt_to_soilw + pondw_to_soilw;
				evap           = soilwEvap; 
			}
			else
			{
				/* diffusion from actual layer to upper layer */
				if (wflux[layer-1] < 0)
					waterFromAbove     = 0;
				/* no percolation, water from above goes into GW */
				else
					waterFromAbove = wflux[layer-1];

				evap = 0; 
			}

			/* Capillary-zone */	
			if (gwc->HYDROflag[layer] == 1)
			{

				/* if CF is saturated or i+1 layer is GW-layer, water from above goes into GW   */
				if (fabs(VWC1-VWCsat1)<0.000001)
				{
					if (soilw_sat0-soilw0 < waterFromAbove)
						GWrecharge[layer]  = waterFromAbove - (soilw_sat0-soilw0);
					
					percol[layer]       = 0;
					if (diffus[layer] > 0) diffus[layer] = 0;
				}

			}
			
	
			/* GW-zone */
			if (gwc->HYDROflag[layer] == 2)
			{

				/* diffusion from actual layer to upper layer */
				if (wflux[layer-1] < 0)
				{
					GWdischarge[layer] = -1* wflux[layer-1];
				}
				
				/* water flux into groundwater (only water from above) */
				GWrecharge[layer]  = waterFromAbove;

				/* source of evaporation and transpiration is GW */
				GWdischarge[layer] += transp[layer]+evap;

				percol[layer] = 0;
				diffus[layer] = 0;
			}

			/* -------------------*/
			/* 1.5. PRECISION CONTROL */

			if (fabs(infilt_to_soilw)    < CRIT_PREC) infilt_to_soilw=0;
			if (fabs(pondw_to_soilw)     < CRIT_PREC) pondw_to_soilw=0;
			if (fabs(soilwEvap)          < CRIT_PREC) soilwEvap=0;
			if (fabs(transp[layer])      < CRIT_PREC) transp[layer]=0;
			if (fabs(GWdischarge[layer]) < CRIT_PREC) GWdischarge[layer]=0;
			if (fabs(GWrecharge[layer])  < CRIT_PREC) GWrecharge[layer]=0;
			if (fabs(diffus[layer])      < CRIT_PREC) diffus[layer]=0;
			if (fabs(percol[layer])      < CRIT_PREC) percol[layer]=0;

							

			/* -------------------*/
			/* 1.6. WFLUX */

			wflux[layer]=percol[layer]+diffus[layer];

			
		}


		/* ----------------------------------------*/
		/* 2. UPDATING */

		
		/* -------------------*/
		/* 2.2 SOIL WATER CONTENT */
		for (layer=0 ; layer < N_SOILLAYERS_GWC; layer++)
		{
			VWC0       = gwc->VWC_GWC[layer];
			dz0        = gwc->soillayer_thicknessGWC[layer];
			VWChw0     = gwc->VWChw_GWC[layer];
			VWCsat0    = gwc->VWCsat_GWC[layer];
			soilw_sat0 = VWCsat0 * dz0 * water_density;
			soilw0     = gwc->VWC_GWC[layer] * dz0 * water_density;

			if (layer == 0)
				gwc->soilw_GWC[layer] = gwc->soilw_GWC[layer]  + infilt_to_soilw + pondw_to_soilw - soilwEvap 
				                        - wflux[layer] - transp[layer] + GWdischarge[layer] - GWrecharge[layer];
			else
				gwc->soilw_GWC[layer] = gwc->soilw_GWC[layer]  + wflux[layer-1] 
			                            - wflux[layer] - transp[layer] + GWdischarge[layer] - GWrecharge[layer];
			
		
			/* control to avoid oversaturated SWC pool */

			extra             = gwc->soilw_GWC[layer] - soilw_sat0;

			
			if (extra >	0)
			{
				/* if layer is GW-layer: extra water goes into groundwater - net loss for the system */
 				if (gwc->HYDROflag[layer] == 2)
				{
					GWrecharge[layer] -= extra;
					gwc->soilw_GWC[layer]        = soilw_sat0;
				}
				/* distribution in upper layers (at the end: pond water) */
				else
				{
					gwc->soilw_GWC[layer] = soilw_sat0;
					gwc->VWC_GWC[layer]   = gwc->soilw_GWC[layer] / (water_density * gwc->soillayer_thicknessGWC[layer]);
					l=layer-1;
					while (extra > 0 && l>=0)
					{
						soilw_satUPPER=gwc->VWCsat_GWC[l] * gwc->soillayer_thicknessGWC[l] * water_density;
				
						if (soilw_satUPPER - gwc->soilw_GWC[l] > extra)
						{
							gwc->soilw_GWC[l]     += extra;
							gwc->soilwFlux_GWC[l] -= extra;
							extra                  = 0;
						}
						else
						{
							extra_act              = soilw_satUPPER - gwc->soilw_GWC[l];
							gwc->soilw_GWC[l]     += extra_act;
							gwc->soilwFlux_GWC[l] -= extra_act;
							extra                 -= extra_act;   
							l                     -= 1;
						}

						gwc->VWC_GWC[l]        = gwc->soilw_GWC[l] / (water_density * gwc->soillayer_thicknessGWC[l]);
						
					}
					/* top soil layer - extra water goes into pond water */
					if (l == -1 && extra > 0) 
					{
						soilw_to_pondw += extra;
						extra           = 0;
					}
				
				}
			}
		
			/* control to avoid negative SWC pool */
			diff             = VWChw0 * dz0 * water_density - gwc->soilw_GWC[layer];
			if (diff >	0)
			{
				/*  top soil layer: limitation of evaporation AND transpiration */
				if (layer == 0)
				{
					if (soilwEvap+transp[layer] > diff)
					{
						diff1                  = soilwEvap   *(diff/(soilwEvap+transp[layer]));
						diff2                  = transp[layer]*(diff/(soilwEvap+transp[layer]));
						soilwEvap             -= diff1;
						transp[layer]         -= diff2;
						gwc->soilw_GWC[layer]  = VWChw0 * dz0 * water_density;
						diff                  -= (diff1+diff2);
						if (diff > CRIT_PRECwater) ws->runoff_snk += diff;
					}
					else
					{
						diff            -= (soilwEvap +transp[layer]);
						soilwEvap       = 0;
						transp[layer]    = 0;
						if (diff > CRIT_PRECwater)
						{
							if (wflux[layer] > diff)
							{
								wflux[layer]         -= diff;
								gwc->soilw_GWC[layer] = VWChw0 * dz0 * water_density;
								diff                  = 0;
							}
							else
							{
								printf("\n");
								printf("ERROR: negative soil water content\n");
								errorCode=1;
							}
						}
						else
							gwc->soilw_GWC[layer]  = VWChw0 * dz0 * water_density;
					}
				}
				/* except of top soil layer: limitation of transpiration */
				else
				{
					if (transp[layer] > diff)
					{
						transp[layer]         -= diff;
						gwc->soilw_GWC[layer]  = VWChw0 * dz0 * water_density;
						diff                   = 0;
					}
					else
					{
						diff  -= transp[layer];
						transp[layer] = 0;
						if (diff > CRIT_PRECwater)
						{
							if (wflux[layer] > diff)
							{
								wflux[layer]         -= diff;
								gwc->soilw_GWC[layer] = VWChw0 * dz0  * water_density;
								diff                  = 0;
							}
							else
							{
								printf("\n");
								printf("ERROR: negative soil water content\n");
								errorCode=1;
							}
						}
						else
							gwc->soilw_GWC[layer]  = VWChw0 * dz0 * water_density;
					}
				}
			}	
		

			gwc->VWC_GWC[layer]          = gwc->soilw_GWC[layer] / (water_density * dz0);
			gwc->soilwTransp_GWC[layer]  += transp[layer];
			gwc->soilwFlux_GWC[layer]    += wflux[layer];
			gwc->GWdischarge_GWC[layer]  += GWdischarge[layer];
			gwc->GWrecharge_GWC[layer]   += GWrecharge[layer];

	
		
			transpDEM[layer]-= transp[layer];			
			TRANSP_ctrl     += transp[layer];

			GWdischarge[layer] = 0;
			GWrecharge[layer]  = 0;

		
            /* rounding errors */
			if (fabs(gwc->VWC_GWC[layer] - VWCsat0) != 0 && fabs(gwc->VWC_GWC[layer] - VWCsat0) < 0.000001)
			{
				ws->deeppercolation_snk += (gwc->soilw_GWC[layer]-soilw_sat0);
				gwc->VWC_GWC[layer]      = VWCsat0;
				gwc->soilw_GWC[layer]    = VWCsat0 * dz0 * water_density;
				
			}

		
		
		
			/* -------------------*/
			/* 2.3. FINDING THE MAXIMAL CHANGE to determine discretization step */
			
			VWCdiff = fabs(VWC0 - gwc->VWC_GWC[layer]);
			
			if (VWCdiff_max < VWCdiff) VWCdiff_max = VWCdiff;		
		
		}

		/* -------------------*/
		/* 2.1 POND WATER */

		/* control to avoid SWC greater than saturation */
		
		pondw_act          += infilt_to_pondw + soilw_to_pondw - pondw_to_soilw - pondwEvap;
		
	
		
		if (pondw_act < 0)
		{
			diff               = -1*pondw_act;
			diff1              = diff * pondwEvap    /(pondwEvap+pondw_to_soilw);
			diff2              = diff * pondw_to_soilw/(pondwEvap+pondw_to_soilw);
			pondwEvap        -= diff1;
			pondw_to_soilw    -= diff2;
			gwc->soilw_GWC[0] -= diff2;
			gwc->VWC_GWC[0]    = gwc->soilw_GWC[0] / (water_density * gwc->soillayer_thicknessGWC[0]);
			if (gwc->VWC_GWC[0] > gwc->VWCsat_GWC[0])
			{
				printf("\n");
				printf("ERROR: in pondw water calculation (richards.c)\n");
			}
			pondw_act       = 0;
			diff           -= (diff1+diff2);
			if (diff > CRIT_PRECwater) ws->runoff_snk += diff;
		}
		

		/* 2.2 pondw and FLUX VARIABLES */
			
		wf->infilt_to_pondw += infilt_to_pondw;
		wf->infilt_to_soilw += infilt_to_soilw;

		
		wf->soilw_to_pondw  += soilw_to_pondw;
		wf->soilwEvap       += soilwEvap;
		wf->pondw_to_soilw  += pondw_to_soilw;
		wf->pondwEvap       += pondwEvap;
	

		INFILT          -= (infilt_to_soilw + infilt_to_pondw);
		INFILT_ctrl     += (infilt_to_soilw + infilt_to_pondw);

		EVAP           -= (pondwEvap + soilwEvap);
		EVAP_ctrl      += (pondwEvap + soilwEvap);

	
		soilw_to_pondw  = 0;
		soilwEvap       = 0;
		pondw_to_soilw  = 0;
		pondwEvap       = 0;
		infilt_to_soilw = 0;
		infilt_to_pondw = 0;


			
		n_second += n_sec;

		/* ----------------------------------------*/
		/* 3. CALCULATION OF DISCRETE LEVEL: if the maximal change of VWC is not equal to 0 (greater than) -> discretlevel is the function of local value of the change */
		VWCdiff_max = VWCdiff_max/n_sec;
		
		if (epc->discretlevel_Richards < 3)
		{
			if (VWCdiff_max > CRIT_PRECwater)
			{
				localvalue=floor(log10(VWCdiff_max));
				if (localvalue + discretlevel < 0)
					exponent_discretlevel = fabs(localvalue + discretlevel);
				else
					exponent_discretlevel = 0;

				n_sec = (int) pow(10, exponent_discretlevel);
				if (n_sec + n_second > nSEC_IN_DAY) 
					n_sec = nSEC_IN_DAY - n_second;
			}
			else
			{
				n_sec = nSEC_IN_DAY - n_second;
			}
		}
		else
		{
			if (epc->discretlevel_Richards == 3) 
				n_sec = 10;
			else
				n_sec = 1;
		}
			

		VWCdiff_max = 0;

	
		if (n_second < nSEC_IN_DAY)
		{
			INFILT_act = INFILT/(nSEC_IN_DAY-n_second) * n_sec; 
			EVAP_act = EVAP/(nSEC_IN_DAY-n_second) * n_sec;
			for (layer=0 ; layer < N_SOILLAYERS_GWC; layer++) transpDEM_act[layer] = transpDEM[layer]/(nSEC_IN_DAY-n_second) * n_sec;
		}
		else
		{
			INFILT_act = 0;
			EVAP_act = 0;
			for (layer=0 ; layer < N_SOILLAYERS_GWC; layer++) transpDEM_act[layer] = 0;

			/* pond water formation from the non-infiltrated water */
			if (INFILT_sum - INFILT_ctrl > 0)
			{
				infilt_to_soilw      = INFILT_sum - INFILT_ctrl;
				wf->infilt_to_soilw += infilt_to_soilw;
				pondw_act           += infilt_to_soilw;
				INFILT_ctrl         += infilt_to_soilw;
			}
			
		

			/* precision control */
			if (pondw_act > 0 && pondw_act < CRIT_PRECwater)
			{
				wf->pondwEvap += pondw_act;
				pondw_act       = 0;
			}
		}

	
		ws->timestepRichards=ws->timestepRichards+1;	
	
	}
	
	
	/* if pond water is greatedr than a maximum height -> runoff */
	if (pondw_act > sprop->pondmax)
	{
		wf->pondw_to_runoff  = pondw_act - sprop->pondmax;
		pondw_act            = sprop->pondmax;
	}

	ws->pondw = pondw_act;

	/* --------------------------------------------------------------------------------------------------------*/	
	/* III. CONTROLS OF CALCULATION */

	/* ---------------------------------------- */
	/* 1. control of soil evaporation  */
	if (wf->soilwEvap > wf->soilwEvap_POT)
	{
		diff = wf->soilwEvap - wf->soilwEvap_POT;
		if (diff < CRIT_PRECwater)
		{
			wf->soilwEvap    -= diff;
			EVAP_ctrl         -= diff;
			gwc->soilw_GWC[0] += diff;
			gwc->VWC_GWC[0]    = gwc->soilw_GWC[0] / (water_density * gwc->soillayer_thicknessGWC[0]);
		}
		else
		{
			printf("\n");
			printf("ERROR: actual evaporation is greater than potential evaporation (richards.c)\n");
			errorCode=1;
		}
	}

	if (fabs(wf->soilwEvap + wf->pondwEvap - EVAP_ctrl) > CRIT_PRECwater)
	{
	//	printf("\n");
		printf("WARNING: balance problem in evaporation calculation (richards.c)\n");
	}

	/* ----------------------------------------*/
	/* 2. control of transpiration */

	wf->soilwTransp_SUM = 0;
	for (layer=0 ; layer < N_SOILLAYERS_GWC-1; layer++)
	{
		wf->soilwTransp_SUM               += gwc->soilwTransp_GWC[layer];
		gwc->soilwTranspDemand_GWC[layer]  = wf->soilwTranspDemand_SUM * gwc->rootlengthProp_GWC[layer]; 
	}

	if (wf->soilwTransp_SUM > wf->soilwTranspDemand_SUM)
	{
		diff = wf->soilwTransp_SUM - wf->soilwTranspDemand_SUM;
		if (diff < CRIT_PRECwater)
		{
			for (layer=0 ; layer < N_SOILLAYERS_GWC-1; layer++) 
			{
				gwc->soilwTransp_GWC[layer] *= (wf->soilwTranspDemand_SUM - diff)/wf->soilwTranspDemand_SUM;
				TRANSP_ctrl                 -= diff;
				gwc->soilw_GWC[layer]       += diff;
				gwc->VWC_GWC[layer]          = gwc->soilw_GWC[layer] / (water_density * gwc->soillayer_thicknessGWC[layer]);
			}
		}
		else
		{
			printf("\n");
			printf("ERROR: in transpiration calculation (richards.c)\n");
			errorCode=1;
		}
	}

	if (fabs(wf->soilwTransp_SUM - TRANSP_ctrl) > CRIT_PRECwater)
	{
		//printf("\n");
		printf("WARNING: balance problem in transpiration calculation (richards.c)\n");
	}

	wf->infiltPOT = (wf->prcp_to_soilw + wf->snoww_to_soilw + wf->canopyw_to_soilw + wf->IRG_to_prcp + wf->pondw_to_soilw - wf->prcp_to_runoff - wf->prcp_to_pondw);


	/* ----------------------------------------*/
	/* 2. control of infiltration */
	if (fabs(INFILT_sum - INFILT_ctrl) > CRIT_PRECwater)
	{
		//printf("\n");
		printf("WARNING: balance problem in infiltration calculation (richards.c)\n");
	}


	/* BOTTOM LAYER IS SPECIAL */
	if (gwc->HYDROflag[N_SOILLAYERS_GWC-1] == 0)
		diff = gwc->soilw_GWC[N_SOILLAYERS_GWC-1] - gwc->VWCfc_GWC[N_SOILLAYERS_GWC-1]* gwc->soillayer_thicknessGWC[N_SOILLAYERS_GWC-1] * water_density;
	else
		diff = gwc->soilw_GWC[N_SOILLAYERS_GWC-1] - gwc->VWCsat_GWC[N_SOILLAYERS_GWC-1]* gwc->soillayer_thicknessGWC[N_SOILLAYERS_GWC-1] * water_density;
	if (fabs(diff) > 0)
	{
		gwc->soilwFlux_GWC[N_SOILLAYERS_GWC-1]  = diff;
		gwc->soilw_GWC[N_SOILLAYERS_GWC-1] -= diff;
		gwc->VWC_GWC[N_SOILLAYERS_GWC-1] = gwc->soilw_GWC[N_SOILLAYERS_GWC-1] / (water_density * gwc->soillayer_thicknessGWC[layer]);
	}
	


	return (errorCode);
}

