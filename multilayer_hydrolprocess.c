/* 
multilayer_hydrolprocess.c
calculation of soil water content layer by layer taking into account soil hydrological processes 
(precipitation, evaporation, runoff, percolation, diffusion)

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
#include "bgc_constants.h"
#include "bgc_func.h"    

int multilayer_hydrolprocess(control_struct* ctrl, const siteconst_struct* sitec, const soilprop_struct* sprop, const epconst_struct* epc, 
							 epvar_struct* epv, wstate_struct* ws, wflux_struct* wf)
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
	double vwc_avg, vwc_RZ, psi_RZ, soilw_RZ, weight, weight_SUM, ratio;
	double soilw_hw, soilw_wp, transp_diff, transp_diff_SUM, soilw_trans_ctrl, soilw_before;
	double vwcSAT_RZ, vwcFC_RZ, vwcWP_RZ, vwcHW_RZ, soilw_RZ_avail, Nlimit_RZ;
	int layer;
	int errflag=0;
	soilw_before=soilw_hw=transp_diff=transp_diff_SUM=soilw_trans_ctrl=vwc_avg=vwc_RZ=psi_RZ=soilw_RZ=weight=weight_SUM=ratio=soilw_wp=soilw_RZ_avail=0;
	vwcSAT_RZ=vwcFC_RZ=vwcWP_RZ=vwcHW_RZ=Nlimit_RZ=0.0;
	/* *****************************/
	/* 1. INFILTRATION, POND AND RUNOFF*/

	
	/* when the precipitation at the surface exceeds the max. infiltration rate, the excess water is put into surfacerunoff (Balsamo et al. 20008; Eq.(7)) */
	/* if the precipitation is greater than critical amount a fixed part of pot_infilt is lost due to runoff (based on Campbell and Diaz, 1988) */
	if (!errflag && pondANDrunoff(sitec,sprop, epv, ws, wf))
	{
		printf("\n");
		printf("ERROR in runoff() from multilayer_hydrolprocess.c()\n");
		errflag=1; 
	} 
	

	 /* ********************************/
	/* 2. PERCOLATION  AND DIFFUSION  */
	
	if (epc->SHCM_flag == 0)
	{
		if (!errflag && richards(sitec, sprop, epc, epv, ws, wf))
		{
			printf("\n");
			printf("ERROR in richards() from multilayer_hydrolprocess.c()\n");
			errflag=1; 
		} 
		#ifdef DEBUG
					printf("%d\t%d\tdone richards\n",simyr,yday);
		#endif	
	}
	else
	{
		if (!errflag && tipping(sitec, sprop, epc, epv, ws, wf))
		{
			printf("\n");
			printf("ERROR in tipping() from multilayer_hydrolprocess.c()\n");
			errflag=1;
		} 
		#ifdef DEBUG
					printf("%d\t%d\tdone tipping\n",simyr,yday);
		#endif	
	}


	/* ********************************************/
	/* 3. EVAPORATION */
	
	/* 3.1: pond water evaporation: water stored on surface which can not infiltrated because of saturation */
	if (ws->pond_water > 0)
	{
		/* pond_flag: flag of WARNING writing (only at first time) */
		if (!ctrl->pond_flag ) ctrl->pond_flag = 1;
		if (wf->pot_evap < ws->pond_water)
			wf->pondw_evap = wf->pot_evap;
		else 
			wf->pondw_evap = ws->pond_water;
	}
	/* 3.2: calculation of actual evaporation from potential evaporation */
	else
	{
		if (!errflag && potEVAP_to_actEVAP(ctrl, sitec, sprop, epv, ws, wf))
		{
			printf("ERROR in potEVAP_to_actEVAP() from multilayer_hydrolprocess.c()\n");
			errflag=547;
		}
	
	}

	/* ********************************************/
	/* 4. TRANSPIRATION */
	
	/* calculate the part-transpiration from total transpiration */
	if (!errflag && multilayer_transpiration(ctrl, epc, sitec, sprop, epv, ws, wf))
	{
		printf("ERROR in multilayer_transpiration() from multilayer_hydrolprocess.c()\n");
		errflag=523;
	}
#ifdef DEBUG
	printf("%d\t%d\tdone multilayer_transpiration\n",simyr,yday);
#endif
		

	/* evaportanspiration calculation */	
	wf->evapotransp = wf->canopyw_evap + wf->soilw_evap + wf->soilw_trans_SUM + wf->snoww_subl;

	
	/* ********************************************/
	/* 5. Soilstress calculation based on VWC or transpiration demand-possibitiy */
	
	if (!errflag && soilstress_calculation(sitec, sprop, epc, epv, ws, wf))
	{
		printf("\n");
		printf("ERROR in soilstress_calculation() from multilayer_hydrolprocess.c()\n");
		errflag=1; 
	} 

   
	/* ********************************/
	/* 5. BOTTOM LAYER IS SPECIAL: percolated water is net loss for the system, water content does not change */
	
	if (ctrl->gwd_act == DATA_GAP || ( ctrl->gwd_act != DATA_GAP && ctrl->gwd_act > sitec->soillayer_depth[N_SOILLAYERS-1]))
	{
		soilw_before              = ws->soilw[N_SOILLAYERS-1];
		epv->vwc[N_SOILLAYERS-1]  = sprop->vwc_fc[N_SOILLAYERS-1];
		ws->soilw[N_SOILLAYERS-1] = sprop->vwc_fc[N_SOILLAYERS-1] * (sitec->soillayer_thickness[N_SOILLAYERS-1]) * water_density;
		ws->deeppercolation_snk += (soilw_before - ws->soilw[N_SOILLAYERS-1]);
	}

	

	/* ********************************/
	/* 6. CONTROL and calculating averages - unrealistic VWC content (higher than saturation value or less then hygroscopic) */

	for (layer = 0; layer < N_SOILLAYERS; layer++)
	{
		if (epv->vwc[layer] < sprop->vwc_hw[layer])       
		{
			if (sprop->vwc_hw[layer] - epv->vwc[layer] < 1e-3)
			{
				wf->soilw_percolated[layer] -= (sprop->vwc_hw[layer] - epv->vwc[layer]);
				ws->deeppercolation_snk -= (sprop->vwc_hw[layer] - epv->vwc[layer]);
				epv->vwc[layer] = sprop->vwc_hw[layer];
				ws->soilw[layer] = epv->vwc[layer] * water_density * sitec->soillayer_thickness[layer];
			}
			else
			{
				printf("\n");
				printf("ERROR in soil water content calculation (multilayer_hydrolprocess.c)\n");
				errflag=1;	
			}

		}
		
		if (epv->vwc[layer] > sprop->vwc_sat[layer])       
		{
			if (epv->vwc[layer] - sprop->vwc_sat[layer] < 1e-2)
			{
				wf->soilw_percolated[layer] += (epv->vwc[layer] - sprop->vwc_sat[layer]);
				ws->deeppercolation_snk += (epv->vwc[layer] - sprop->vwc_sat[layer]);
				epv->vwc[layer] = sprop->vwc_sat[layer];
				ws->soilw[layer] = epv->vwc[layer] * water_density * sitec->soillayer_thickness[layer];
			}
			else
			{
				printf("\n");
				printf("ERROR in soil water content calculation (multilayer_hydrolprocess)\n");
				errflag=1;	
			}

		}
		vwc_avg	  += epv->vwc[layer]    * (sitec->soillayer_thickness[layer] / sitec->soillayer_depth[N_SOILLAYERS-1]);
		
		
		
		/* calculation of rootzone variables - weight of the last layer depends on the depth of the root */
		if (epv->rooting_depth && layer < epv->n_rootlayers) 
		{	
			if (epv->n_rootlayers == 1)
			{
				weight = 1;
				soilw_RZ  += ws->soilw[layer];
			}
			else
			{
				soilw_wp = sprop->vwc_wp[layer] * sitec->soillayer_thickness[layer] * water_density;
				if (layer < epv->n_rootlayers-1)
				{
					weight = sitec->soillayer_thickness[layer] / epv->rooting_depth;
					soilw_RZ  += ws->soilw[layer];
					if (ws->soilw[layer] - soilw_wp > 0) soilw_RZ_avail += ws->soilw[layer] - soilw_wp;
				}
				else	
				{
					weight = (epv->rooting_depth-sitec->soillayer_depth[layer-1])/ epv->rooting_depth;
					/* soil water content in rootzone: last layer - only only as long as it reaches */
					ratio = (epv->rooting_depth - sitec->soillayer_depth[epv->n_rootlayers-2])/sitec->soillayer_thickness[epv->n_rootlayers-1];
					soilw_RZ  += ws->soilw[layer] * ratio;
					if (ws->soilw[layer] - soilw_wp > 0) soilw_RZ_avail += (ws->soilw[layer] - soilw_wp) * ratio;
				}
			}
			weight_SUM += weight;

			vwc_RZ	  += epv->vwc[layer]      * weight;
			psi_RZ	  += epv->psi[layer]      * weight;
			vwcSAT_RZ += sprop->vwc_sat[layer]* weight;
			vwcFC_RZ  += sprop->vwc_fc[layer] * weight;
			vwcWP_RZ  += sprop->vwc_wp[layer] * weight;
			vwcHW_RZ  += sprop->vwc_hw[layer] * weight;
			Nlimit_RZ += epv->Nlimit[layer] * weight;

		}

	}

	if (epv->rooting_depth && fabs(1-weight_SUM) > CRIT_PREC)
	{
		printf("ERROR in calculation of rootzone variables (multilayer_hydrolprocess.c) \n");
		errflag=1;
	}
	epv->Nlimit_RZ = Nlimit_RZ;
	epv->vwcSAT_RZ = vwcSAT_RZ;
	epv->vwcFC_RZ  = vwcFC_RZ;
	epv->vwcWP_RZ  = vwcWP_RZ;
	epv->vwcHW_RZ  = vwcHW_RZ;
	
	epv->vwc_avg = vwc_avg;
	epv->vwc_RZ  = vwc_RZ;
	epv->psi_RZ  = psi_RZ;
	ws->soilw_RZ  = soilw_RZ;
	ws->soilw_RZ_avail  = soilw_RZ_avail;
	
		epv->vwc[layer] = ws->soilw[layer] / (water_density * sitec->soillayer_thickness[layer]);
	return (errflag);
}