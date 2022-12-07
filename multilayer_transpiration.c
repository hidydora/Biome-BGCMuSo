/* 
multilayer_transpiration.c
Hidy 2011 - part-transpiration (regarding to the different layers of the soil) calculation based on the layer's soil water content

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

int multilayer_transpiration(control_struct* ctrl, const siteconst_struct* sitec, const soilprop_struct* sprop, 
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
	int layer;
	double soilwTransp_SUM, soilw_wp, transp_lack;
	
	int errorCode=0;

	soilwTransp_SUM=soilw_wp=0;


	for (layer = 0; layer < N_SOILLAYERS; layer++)
	{
		/* actual soil water content at theoretical lower limit of water content: hygroscopic water point */
		soilw_wp = sprop->VWCwp[layer] * sitec->soillayer_thickness[layer] * water_density;

		/* transp_lack: control parameter to avoid negative soil water content (due to overestimated transpiration + dry soil) */
		ws->soilw_avail[layer] = (ws->soilw[layer] - soilw_wp);
	}
	
		/* *****************************************************************************************************************/
	/* 1. PART-TRANSPIRATION: first approximation tanspiration from every soil layer equally */

	for (layer = epv->germ_layer; layer < epv->n_rootlayers; layer++)
	{		
		
		/* transpiration based on rootlenght proportion */
		wf->soilwTranspDemand[layer] = wf->soilwTranspDemand_SUM * epv->rootlengthProp[layer]; 


		/* transp_lack: control parameter to avoid negative soil water content (due to overestimated transpiration + dry soil) */
		transp_lack = wf->soilwTranspDemand[layer] - ws->soilw_avail[layer];

		/* if transpiration demand is greater than theoretical lower limit of water content: wilting point -> limited transpiration flux)  */
		if (transp_lack > 0)
		{
			/* theoretical limit */
			if (ws->soilw_avail[layer]  > CRIT_PREC)
				wf->soilwTransp[layer] = ws->soilw_avail[layer];
			else
				wf->soilwTransp[layer] = 0;

	
			/* limittransp_flag: writing in log file (only at first time) */
			if (transp_lack > CRIT_PREC && !ctrl->limittransp_flag) ctrl->limittransp_flag = 1;
		}
		else
			wf->soilwTransp[layer] = wf->soilwTranspDemand[layer];

		ws->soilw[layer] -= wf->soilwTransp[layer];
		epv->VWC[layer]  = ws->soilw[layer] / sitec->soillayer_thickness[layer] / water_density;
	
		soilwTransp_SUM += wf->soilwTransp[layer];
	}

	wf->soilwTransp_SUM = soilwTransp_SUM;

	/* control */
	if (wf->soilwTransp_SUM - wf->soilwTranspDemand_SUM > CRIT_PREC)
	{
		printf("\n");
		printf("ERROR: transpiration calculation error in multilayer_hydrolprocess.c:\n");
		errorCode=1;
	}

	/* extreme dry soil - no transpiration occurs */
	if (soilwTransp_SUM == 0 && wf->soilwTransp_SUM != 0)
	{
		wf->soilwTransp_SUM = 0;
		/* notransp_flag: flag of WARNING writing in log file (only at first time) */
		if (!ctrl->notransp_flag) ctrl->notransp_flag = 1;
	}





	return (errorCode);
}



