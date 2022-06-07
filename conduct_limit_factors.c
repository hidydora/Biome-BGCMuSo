/* 
conduct_limit_factors.c
calculate the limitation factors of conductance
*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Biome-BGCMuSo v6.1.
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


int conduct_limit_factors(file logfile, const control_struct* ctrl, const soilprop_struct* sprop, const epconst_struct* epc, 
						  epvar_struct* epv)
{
	int errorCode=0;
	int layer;
	double vwc_sat,vwc_fc,vwc_wp, vwc_crit1, vwc_crit2;
    
	vwc_sat=vwc_fc=vwc_wp=vwc_crit1=vwc_crit2=0;

	

	/* calculations layer by layer (due to different soil properties) */
	for (layer=0; layer < N_SOILLAYERS; layer++)
	{
		vwc_sat = sprop->vwc_sat[layer]; 
		vwc_fc  = sprop->vwc_fc[layer]; 
		vwc_wp  = sprop->vwc_wp[layer];

		if (epc->VWCratio_crit1 != DATA_GAP)
			vwc_crit1 = vwc_wp + epc->VWCratio_crit1 * (vwc_fc - vwc_wp);
		else
			vwc_crit1 = vwc_fc;

		if (epc->VWCratio_crit2 != DATA_GAP)
			vwc_crit2 = vwc_fc + epc->VWCratio_crit2 * (vwc_sat - vwc_fc);
		else
			vwc_crit2 = vwc_sat;


		/* CONTROL */
		if (vwc_crit2 > vwc_sat)
		{
			printf("\n");
			printf("ERROR: VWC_crit2 data is greater than saturation value in layer:%i\n", layer);
			errorCode=1;
		}

		if (!errorCode && vwc_crit2 < vwc_crit1)
		{
			printf("\n");
			printf("ERROR: VWC_crit1 data is greater then VWC_crit2 data in layer:%i\n", layer);
			errorCode=1;
		}
	
		epv->vwc_crit1[layer]		= vwc_crit1;
		epv->vwc_crit2[layer]		= vwc_crit2;

	}
	if (ctrl->spinup < 2) fprintf(logfile.ptr, "LIMITATION VALUES OF SWC (m3/m3) IN TOP SOIL LAYER \n");  
	if (ctrl->spinup < 2) fprintf(logfile.ptr, "SWC (limit1 and limit2):  %12.3f %12.3f\n", epv->vwc_crit1[0], epv->vwc_crit2[0]);
	if (ctrl->spinup < 2) fprintf(logfile.ptr, " \n");
		
	return (errorCode);
}
