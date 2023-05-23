/* 
multilayer_hydrolprocess.c
calculation of soil water content layer by layer taking into account soil hydrological processes 
(precipitation, evaporation, runoff, percolation, diffusion)

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Biome-BGCMuSo v7.0.
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

int multilayer_hydrolprocess(control_struct* ctrl, siteconst_struct* sitec, soilprop_struct* sprop, const epconst_struct* epc, epvar_struct* epv, 
	                         wstate_struct* ws, wflux_struct* wf, groundwater_struct* GWS, GWcalc_struct* gwc, flooding_struct* FLD, int* mondays)
{
	/* given a list of site constants and the soil water mass (kg/m2),
	this function returns the soil water potential (MPa)
	inputs:


	For further discussion see:
	Cosby, B.J., G.M. Hornberger, R.B. Clapp, and T.R. Ginn, 1984.     

	Balsamo et al 2009 - 
	A Revised Hydrology for the ECMWF Model - Verification from Field Site to Water Storage IFS - JHydromet.pdf

	Chen and Dudhia 2001 - 
	Coupling an Advanced Land Surface-Hydrology Model with the PMM5 Modeling System Part I - MonWRev.pdf*/
	

	/* internal variables */
	double VWC_avg, VWC_maxRZ, relVWCsat_fc_maxRZ, relVWCfc_wp_maxRZ, VWC_RZ, PSI_RZ, soilw_RZ, weight, weight_SUM, ratio, hydrCONDUCTsat_avg;
	double soilw_hw, soilw_wp, TRP_diff, TRP_diff_SUM, soilw_trans_ctrl, soilw_before;
	double VWCsat_RZ, VWCfc_RZ, VWCwp_RZ, VWChw_RZ, soilw_RZ_avail;
	int layer;
	int errorCode=0;
	soilw_before=soilw_hw=TRP_diff=TRP_diff_SUM=soilw_trans_ctrl=VWC_avg=VWC_RZ=PSI_RZ=soilw_RZ=weight=weight_SUM=ratio=soilw_wp=soilw_RZ_avail=0;
	VWC_RZ=VWC_maxRZ=VWCsat_RZ=VWCfc_RZ=VWCwp_RZ=VWChw_RZ=hydrCONDUCTsat_avg=relVWCsat_fc_maxRZ=relVWCfc_wp_maxRZ=0.0;
	
	/* ---------------------------------------------------------------------------------------- */
	/* 1. Richards-method */
	/* ---------------------------------------------------------------------------------------- */
	if (epc->SHCM_flag == 1)
	{
		/* *****************************/
		/* 0. GROUNDWATER PREPROCESS: 10 layers to 12 layers */
		if (!errorCode && groundwaterR_preproc(ctrl, sitec, sprop, epv, ws, wf, GWS, gwc))
		{
			printf("ERROR in groundwater() from bgc.c\n");
			errorCode=1;
		}

		/* *****************************/
		/* 1. HYDROLOGICAL CALCULATION BASED ON RICHARDS-METHOD: infiltration, percolation, diffusion, evaporation, transpiration */
		if (!errorCode && richards(epc, sprop, ws, wf, gwc))
		{
			printf("\n");
			printf("ERROR in richards() from multilayer_hydrolprocess.c()\n");
			errorCode=1; 
		} 
	

		/* *****************************/
		/* 2. GROUNDWATER POSTPROCESS: 12 layers to 10 layers  */
		if (!errorCode && groundwaterR_postproc(sitec,epv, ws, wf, gwc))
		{
			printf("ERROR in groundwater() from bgc.c\n");
			errorCode=1;
		}

		/* *****************************/
		/*  3. POND WATER from soil */
		if (ws->pondw + wf->GW_to_pondw  > sprop->pondmax)
		{
			wf->prcp_to_runoff += ws->pondw + wf->GW_to_pondw  - sprop->pondmax;
			ws->pondw = sprop->pondmax;
		}
		else
			ws->pondw += wf->GW_to_pondw;

		/* pond_flag: flag of WARNING writing (only at first time) */
		if (ws->pondw > 0) if (!ctrl->pond_flag ) ctrl->pond_flag = 1;

	}
	/* ---------------------------------------------------------------------------------------- */
	/* 2. Tipping-method */
	/* ---------------------------------------------------------------------------------------- */
	else
	{
		/* ********************************************/
		/* 0. GROUNDWATER preprocess: calculation depth of GW and CF, GW-movchange */
	
		if (!errorCode && groundwaterT_preproc(ctrl, epc, sitec, sprop, epv, ws, wf, GWS))
		{
			printf("ERROR in groundwaterT_preproc() from bgc.c\n");
			errorCode=1;
		}


		/* *****************************/
		/* 2. INFILTRATION AND PONDW FORMATION */
	
		if (!errorCode && infiltANDpond(sitec,sprop, epv,ws, wf))
		{
			printf("\n");
			printf("ERROR in infiltANDpond() from multilayer_hydrolprocess.c()\n");
			errorCode=1; 
		} 

		/* ********************************/
		/* 3. PERCOLATION  AND DIFFUSION  */
		
		if (!errorCode && tipping(sitec, sprop, epc, epv, ws, wf))
		{
			printf("\n");
			printf("ERROR in tipping() from multilayer_hydrolprocess.c()\n");
			errorCode=1;
		} 
	
		/* ********************************************/
		/* 4. SOIL EVAPORATION */


		if (!errorCode && soilEVP_calc(ctrl,sitec,sprop, epv, ws,wf))
		{
			printf("ERROR in soilEVP_calc() from multilayer_hydrolprocess.c()\n");
			errorCode=1;
		}
	
		
		/* ********************************************/
		/* 5. TRANSPIRATION */

		if (!errorCode && multilayer_transpiration(ctrl, sitec, sprop, epv, ws, wf))
		{
			printf("ERROR in multilayer_transpiration() from multilayer_hydrolprocess.c()\n");
			errorCode=1;
		}
	
		/* *****************************/
		/* 6. POND AND RUNOFF */
	
		if (!errorCode && pondANDrunoffD(ctrl,sitec,sprop, epv,ws, wf))
		{
			printf("\n");
			printf("ERROR in pondANDrunoffD() from multilayer_hydrolprocess.c()\n");
			errorCode=1; 
		} 
			
		/* **********************************/
		/* 7. GROUNDWATER CF-charge: diffusion between GW and CF */
	
		if (!errorCode && groundwaterT_CFcharge(sitec, sprop, epv, ws, wf, GWS))
		{
			printf("ERROR in groundwaterT_CFcharge() from multilayer_hydrolprocess.cc\n");
			errorCode=1;
		}

		/* FLOODING */
		if (!errorCode && flooding(ctrl, sitec, FLD, sprop, epv, ws, wf, mondays))
		{
			printf("ERROR in flooding() from bgc.c\n");
			errorCode=1;
		}
	
	}

	

	/* evaportanspiration calculation */	
	
	wf->ET         = wf->canopywEVP + wf->soilwEVP + wf->soilwTRP_SUM + wf->pondwEVP + wf->snowwSUBL;
	wf->surfaceEVP = wf->soilwEVP + wf->pondwEVP;

	if (wf->ET - wf->PET > CRIT_PRECwater)
	{
		printf("ERROR in potential evaporation calculation (ET > PET) in multilayer_hydrolprocess.c\n");
		errorCode=1;
	}
	/* ********************************/
	/* 6. BOTTOM LAYER IS SPECIAL: percolated water is net loss for the system, water content does not change 	*/
	
	if (sprop->GWD == DATA_GAP || (sprop->GWlayer == DATA_GAP))
	{
		soilw_before              = ws->soilw[N_SOILLAYERS-1];
		epv->VWC[N_SOILLAYERS-1]  = sprop->VWCfc[N_SOILLAYERS-1];
		ws->soilw[N_SOILLAYERS-1] = sprop->VWCfc[N_SOILLAYERS-1] * (sitec->soillayer_thickness[N_SOILLAYERS-1]) * water_density;
	
		wf->soilwFlux[N_SOILLAYERS-1] += soilw_before - ws->soilw[N_SOILLAYERS-1];

	}
	

	
	/* ********************************************/
	/* 7. Soilstress calculation based on VWC or transpiration demand-possibitiy */
	
	if (!errorCode && soilstress_calculation(sprop, epc, epv, ws, wf))
	{
		printf("\n");
		printf("ERROR in soilstress_calculation() from multilayer_hydrolprocess.c()\n");
		errorCode=1; 
	} 

	/* ********************************/
	/* 8. CONTROL and calculating averages - unrealistic VWC content (higher than saturation value or less then hygroscopic) - GWtest */

	for (layer = 0; layer < N_SOILLAYERS; layer++)
	{
		if (epv->VWC[layer] < sprop->VWChw[layer])       
		{
			if (sprop->VWChw[layer] - epv->VWC[layer] < 1e-3)
			{
				wf->soilwFlux[N_SOILLAYERS-1] -= (sprop->VWChw[layer] - epv->VWC[layer])* water_density * sitec->soillayer_thickness[layer];
				epv->VWC[layer]                = sprop->VWChw[layer];
				ws->soilw[layer]               = epv->VWC[layer] * water_density * sitec->soillayer_thickness[layer];
			}
			else
			{
				printf("\n");
				printf("ERROR in soil water content calculation (multilayer_hydrolprocess.c)\n");
				errorCode=1;	
			}

		}
		
		if (epv->VWC[layer] - sprop->VWCsat[layer] > CRIT_PREC)       
		{
			if (sprop->GWlayer == layer)
			{
				wf->GWrecharge[layer] = (epv->VWC[layer] - sprop->VWCsat[layer]) * water_density * sitec->soillayer_thickness[layer];
				epv->VWC[layer] = sprop->VWCsat[layer];
				ws->soilw[layer] = epv->VWC[layer] * water_density * sitec->soillayer_thickness[layer];
			}
			else
			{
				if (epv->VWC[layer] - sprop->VWCsat[layer] < CRIT_PRECwater)
				{
					wf->soilwFlux[N_SOILLAYERS-1] += (epv->VWC[layer] - sprop->VWCsat[layer])/(water_density * sitec->soillayer_thickness[layer]);
					epv->VWC[layer] = sprop->VWCsat[layer];
					ws->soilw[layer] = epv->VWC[layer] * water_density * sitec->soillayer_thickness[layer];
				}
				else
				{
					printf("\n");
					printf("ERROR in soil water content calculation (multilayer_hydrolprocess)\n");
					errorCode=1;	
				}
			}

		}
		VWC_avg	            += epv->VWC[layer]    * (sitec->soillayer_thickness[layer] / sitec->soillayer_depth[N_SOILLAYERS-1]);
		hydrCONDUCTsat_avg	+= sprop->hydrCONDUCTsat[layer]   * (sitec->soillayer_thickness[layer] / sitec->soillayer_depth[N_SOILLAYERS-1]);
		
		
		/* calculation of rootzone variables - weight of the last layer depends on the depth of the root */
		if (epv->n_maxrootlayers && layer < epv->n_maxrootlayers)
		{
			VWC_maxRZ          += epv->VWC[layer]          * sitec->soillayer_thickness[layer]/sitec->soillayer_depth[epv->n_maxrootlayers-1];
			relVWCsat_fc_maxRZ += epv->relVWCsat_fc[layer] * sitec->soillayer_thickness[layer]/sitec->soillayer_depth[epv->n_maxrootlayers-1];
			relVWCfc_wp_maxRZ  += epv->relVWCfc_wp[layer]  * sitec->soillayer_thickness[layer]/sitec->soillayer_depth[epv->n_maxrootlayers-1];
		}
	
		if (epv->n_rootlayers) 
		{
			weight_SUM += epv->rootlengthProp[layer];

			VWC_RZ			+= epv->VWC[layer]      * epv->rootlengthProp[layer];
			PSI_RZ			+= epv->PSI[layer]      * epv->rootlengthProp[layer];
			VWCsat_RZ		+= sprop->VWCsat[layer] * epv->rootlengthProp[layer];
			VWCfc_RZ		+= sprop->VWCfc[layer]  * epv->rootlengthProp[layer];
			VWCwp_RZ		+= sprop->VWCwp[layer]  * epv->rootlengthProp[layer];
			VWChw_RZ		+= sprop->VWChw[layer]  * epv->rootlengthProp[layer];
			soilw_RZ        += ws->soilw[layer]     * epv->rootlengthProp[layer];
			if (epv->VWC[layer] > sprop->VWCwp[layer])
				soilw_RZ_avail  += ((epv->VWC[layer]-sprop->VWCwp[layer]) * sitec->soillayer_thickness[layer] * water_density) * epv->rootlengthProp[layer];
		}
		else
		{
			VWC_RZ			= 0;
			PSI_RZ			= 0;
			VWCsat_RZ		= 0;
			VWCfc_RZ		= 0;
			VWCwp_RZ		= 0;
			VWChw_RZ		= 0;
			soilw_RZ        = 0;
			soilw_RZ_avail  = 0;
		}

	
	}

	if (epv->rootDepth && fabs(1-weight_SUM) > CRIT_PREC)
	{
		printf("ERROR in calculation of rootzone variables (multilayer_hydrolprocess.c) \n");
		errorCode=1;
	}
	epv->VWCsat_RZ = VWCsat_RZ;
	epv->VWCfc_RZ  = VWCfc_RZ;
	epv->VWCwp_RZ  = VWCwp_RZ;
	epv->VWChw_RZ  = VWChw_RZ;
	epv->VWC_maxRZ = VWC_maxRZ;
	epv->relVWCsat_fc_maxRZ = relVWCsat_fc_maxRZ;
	epv->relVWCfc_wp_maxRZ  = relVWCfc_wp_maxRZ;
	
	epv->hydrCONDUCTsat_avg = hydrCONDUCTsat_avg;
	epv->VWC_avg = VWC_avg;
	epv->VWC_RZ  = VWC_RZ;
	epv->PSI_RZ  = PSI_RZ;
	ws->soilw_RZ  = soilw_RZ;
	ws->soilw_RZ_avail  = soilw_RZ_avail;
	

	
	return (errorCode);
}