/* 
multilayer_hydrolprocess.c
calculation of soil water content layer by layer taking into account soil hydrological processes 
(precipitation, evaporation, runoff, percolation, diffusion)

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
BBGC MuSo v4
Copyright 2014, D. Hidy
Hungarian Academy of Sciences
*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <malloc.h>
#include "bgc_struct.h"
#include "bgc_constants.h"
#include "bgc_func.h"    

int multilayer_hydrolprocess(const control_struct* ctrl, const siteconst_struct* sitec, epvar_struct* epv, wstate_struct* ws, wflux_struct* wf)
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
	double prcp, runoff, prcp_to_soil,evap_diff;
	double soilw_hw0, soilw_sat0;  /* (kgH2O/m2/min) */
	
	double coeff_soiltype, coeff_soilmoist, RCN, coeff_runoff;
	int ok=1;


	/* *****************************/
	/* 1. PRECIPITATION AND RUNOFF*/

	/* when the precipitation at the surface exceeds the max. infiltration rate, the excess water is put into surfacerunoff (Balsamo et al. 20008; Eq.(7)) */
	prcp = (wf->prcp_to_soilw + wf->snoww_to_soilw);


	/* if the precipitation is greater than critical amount a fixed part of prcp is lost due to runoff (based on Campbell and Diaz, 1988) */

	RCN = sitec->RCN;
	coeff_soiltype  = 254*(100 / RCN - 1);

	coeff_soilmoist = 0.15 * ((sitec->vwc_sat - epv->vwc[0]) / (sitec->vwc_sat  - sitec->vwc_hw));

	coeff_runoff = coeff_soiltype * coeff_soilmoist;

	if (prcp > coeff_runoff)
	{
		runoff = pow(prcp + ws->pond_water- coeff_runoff, 2) / ((prcp + ws->pond_water) + (1 - coeff_soilmoist)*coeff_soiltype);
	}
	else
	{
		runoff = 0;
	}

	wf->prcp_to_runoff = runoff;

	/* amount of prcp after runoff */
	prcp_to_soil = prcp - runoff;

	/* if pond water is avaialable -> prcp to pond water, otherwise: prcp to soil */
	if (ws->pond_water > 0)
	{
		ws->pond_water += prcp_to_soil;
	}
	else
	{
		ws->soilw[0] += prcp_to_soil;
	}

	/* ********************************************/
	/* 2. EVAPORATION */
	

	/* actual soil water content at theoretical lower limit of water content: hygroscopic water content */
	soilw_hw0 = sitec->vwc_hw * sitec->soillayer_thickness[0] * water_density;

	/* evap_diff: control parameter to avoid negative soil water content (due to overestimated evaporation + dry soil) */
	evap_diff = ws->soilw[0] - wf->soilw_evap - soilw_hw0;

	/* theoretical lower limit of water content: hygroscopic water content. */
	if (evap_diff < 0)
	{
		wf->soilw_evap += evap_diff;
		if (ctrl->onscreen) printf("WARNING: Limited evaporation due to dry soil (multilayer_hydrolprocess.c)\n");
	}
	
	ws->soilw[0] -= wf->soilw_evap;



	/* ********************************/
	/* 3.  POND WATER */

	/* 3A. POND WATER ACCUMULATION: water stored on surface which can not infiltrated because of saturation */
	
	/* theoretical upper limit of water content: saturation value (amount above saturation is stored on the surface) */
	soilw_sat0     = sitec->vwc_sat * sitec->soillayer_thickness[0] * water_density;

	if (ws->soilw[0] > soilw_sat0)
	{
		ws->pond_water   += ws->soilw[0] - soilw_sat0;
		ws->soilw[0] = soilw_sat0;
	}

	epv->vwc[0]  = ws->soilw[0] / sitec->soillayer_thickness[0] / water_density;

	
	/* ********************************/
	/* 4. PERCOLATION  AND DIFFUSION  */

	if (ok && richards(sitec, epv, ws, wf))
	{
		printf("ERROR in richards() from bgc()\n");
		ok=0;
	} 
	#ifdef DEBUG
				printf("%d\t%d\tdone richards\n",simyr,yday); 
	#endif	
	
	/* ---------------------------------------------------------*/	
	
	/* BOTTOM LAYER IS SPECIAL: percolated water is net loss for the system, water content does not change */
	if (sitec->gwd_act >= DATA_GAP || ( sitec->gwd_act != DATA_GAP && sitec->gwd_act > sitec->soillayer_depth[N_SOILLAYERS-1]))
	{
		epv->vwc[N_SOILLAYERS-1]  = sitec->vwc_fc;
		ws->soilw[N_SOILLAYERS-1] = sitec->vwc_fc * (sitec->soillayer_thickness[N_SOILLAYERS-1]) * 1000.0;
	}

	
	/* ********************************/
	/* 5. POND WATER EVAPORATION: water stored on surface which can not infiltrated because of saturation */
	if (ws->pond_water > 0)
	{
		if (ctrl->onscreen && ctrl->spinup == 0) printf("INFORMATION: pond water on soil surface (multilayer_hydrolprocess.c)\n");
		if (wf->soilw_evap < ws->pond_water)
			wf->pondw_evap = wf->soilw_evap;
		else 
			wf->pondw_evap = ws->pond_water;
	}

	return (!ok);
}