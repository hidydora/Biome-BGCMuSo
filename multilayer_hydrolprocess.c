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
	double prcp, evap_diff;
	double vwc_avg, vwc_RZ;
	double soilw_hw0, soilw_before;  /* (kgH2O/m2/min) */
	double soilw_hw, transp_diff, transp_diff_SUM, soilw_trans_ctrl;
	int layer;
	double coeff_soiltype, coeff_soilmoist, RCN, coeff_runoff;
	int errflag=0;
	soilw_before=soilw_hw=transp_diff=transp_diff_SUM=soilw_trans_ctrl=vwc_avg=vwc_RZ=0;

	/* *****************************/
	/* 1. PRECIPITATION AND RUNOFF*/

	/* when the precipitation at the surface exceeds the max. infiltration rate, the excess water is put into surfacerunoff (Balsamo et al. 20008; Eq.(7)) */
	
	prcp = (wf->prcp_to_soilw + wf->snoww_to_soilw + wf->canopyw_to_soilw + wf->IRG_to_prcp + ws->pond_water);


	/* if the precipitation is greater than critical amount a fixed part of prcp is lost due to runoff (based on Campbell and Diaz, 1988) */

	RCN = sprop->RCN;
	coeff_soiltype  = 254*(100 / RCN - 1);

	coeff_soilmoist = 0.15 * ((sprop->vwc_sat[0] - epv->vwc[0]) / (sprop->vwc_sat[0]  - sprop->vwc_hw[0]));

	coeff_runoff = coeff_soiltype * coeff_soilmoist;

	if (prcp > coeff_runoff)
	{
		wf->prcp_to_runoff = pow(prcp - coeff_runoff, 2) / (prcp + (1 - coeff_soilmoist)*coeff_soiltype);

	}
	else
	{
		wf->prcp_to_runoff = 0;
	}


	/* ********************************************/
	/* 3. EVAPORATION */
	
	/* actual soil water content at theoretical lower limit of water content: hygroscopic water content */
	soilw_hw0 = sprop->vwc_hw[0] * sitec->soillayer_thickness[0] * water_density;

	/* evap_diff: control parameter to avoid negative soil water content (due to overestimated evaporation + dry soil) */
	evap_diff = ws->soilw[0] - wf->soilw_evap - soilw_hw0;

	/* theoretical lower limit of water content: hygroscopic water content. */
	if (evap_diff < 0)
	{
		wf->soilw_evap += evap_diff;
		/* limitevap_flag: flag of WARNING writing in log file (only at first time) */
		if (fabs(transp_diff) > CRIT_PREC && !ctrl->limitevap_flag) ctrl->limitevap_flag = 1;
	}
	
	ws->soilw[0] -= wf->soilw_evap;
	epv->vwc[0]  = ws->soilw[0] / water_density / sitec->soillayer_thickness[0];


	/* ********************************************/
	/* 4. TRANSPIRATION */
	
	
	for (layer = 0; layer < N_SOILLAYERS; layer++)
	{		
		/* actual soil water content at theoretical lower limit of water content: hygroscopic water point */
		soilw_hw = sprop->vwc_hw[layer] * sitec->soillayer_thickness[layer] * water_density;

		/* transp_diff: control parameter to avoid negative soil water content (due to overestimated transpiration + dry soil) */
		transp_diff = ws->soilw[layer] - wf->soilw_trans[layer] - soilw_hw;

		

		/* theoretical lower limit of water content: hygroscopic water point (if transp_diff less than 0, limited transpiration flux)  */
		if (transp_diff < 0)
		{
			/* theoretical limit */
			wf->soilw_trans[layer] += transp_diff;
			transp_diff_SUM += transp_diff;

			/* limittransp_flag: writing in log file (only at first time) */
			if (fabs(transp_diff) > CRIT_PREC && !ctrl->limittransp_flag) ctrl->limittransp_flag = 1;
		}

		ws->soilw[layer] -= wf->soilw_trans[layer];
		epv->vwc[layer]  = ws->soilw[layer] / sitec->soillayer_thickness[layer] / water_density;
	

		soilw_trans_ctrl += wf->soilw_trans[layer];
	}

	wf->soilw_trans_SUM += transp_diff_SUM;

	/* control */
	if (fabs(soilw_trans_ctrl - wf->soilw_trans_SUM) > CRIT_PREC)
	{
		printf("\n");
		printf("ERROR: transpiration calculation error in multilayer_hydrolprocess.c:\n");
		errflag=1;
	}


	/* evaportanspiration calculation */	
	wf->evapotransp = wf->canopyw_evap + wf->soilw_evap + wf->soilw_trans_SUM + wf->snoww_subl;

	
    /* ********************************/
	/* 2. PERCOLATION  AND DIFFUSION  */
	
	if (epc->SHCM_flag == 0)
	{
		if (!errflag && richards(sitec, sprop, epc, epv, ws, wf))
		{
			printf("\n");
			printf("ERROR in richards() from bgc()\n");
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
			printf("ERROR in tipping() from bgc()\n");
			errflag=1;
		} 
		#ifdef DEBUG
					printf("%d\t%d\tdone tipping\n",simyr,yday);
		#endif	
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
	/* 6. POND WATER EVAPORATION: water stored on surface which can not infiltrated because of saturation */
	if (ws->pond_water > 0)
	{
		/* pond_flag: flag of WARNING writing (only at first time) */
		if (!ctrl->pond_flag ) ctrl->pond_flag = 1;
		if (wf->pot_evap < ws->pond_water)
			wf->pondw_evap = wf->pot_evap;
		else 
			wf->pondw_evap = ws->pond_water;
	}
	
	/* 7. m_soistress calculation based on VWC or transpiration demand-possibitiy */
	if (epc->soilstress_flag == 0)
	{
		if (!errflag && soilstress_calculation(sprop, epc, epv))
		{
			printf("\n");
			printf("ERROR in soilstress_calculation() from bgc()\n");
			errflag=1; 
		} 
	}

	/* ********************************/
	/* 7. CONTROL and calculating averages - unrealistic VWC content (higher than saturation value or less then hygroscopic) */

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
		
		if (layer < epv->n_maxrootlayers)
		{
			vwc_RZ	  += epv->vwc[layer]     * (sitec->soillayer_thickness[layer] / sitec->soillayer_depth[epv->n_maxrootlayers-1]);
		}

	}

	epv->vwc_avg = vwc_avg;
	epv->vwc_RZ  = vwc_RZ;

	return (errflag);
}