/* 
conduct_limit_factors.c
calculate the limitation factors of conductance
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


int conduct_limit_factors(file logfile, const control_struct* ctrl, const soilprop_struct* sprop, const epconst_struct* epc, 
						  epvar_struct* epv)
{
	int errorCode=0;
	int layer;
	double VWCsat,VWCfc,VWCwp, VWC_SScrit1, VWC_SScrit2;
    double PSI_SScrit1[N_SOILLAYERS];
	double PSI_SScrit2[N_SOILLAYERS];

	VWCsat=VWCfc=VWCwp=VWC_SScrit1=VWC_SScrit2=0;

	

	/* calculations layer by layer (due to different soil properties) */
	for (layer=0; layer < N_SOILLAYERS; layer++)
	{
		VWCsat = sprop->VWCsat[layer]; 
		VWCfc  = sprop->VWCfc[layer]; 
		VWCwp  = sprop->VWCwp[layer];

		if (epc->VWCratio_SScrit1 != DATA_GAP)
			VWC_SScrit1 = VWCwp + epc->VWCratio_SScrit1 * (VWCfc - VWCwp);
		else
			VWC_SScrit1 = VWCfc;

		if (epc->VWCratio_SScrit2 != DATA_GAP)
			VWC_SScrit2 = VWCfc + epc->VWCratio_SScrit2 * (VWCsat - VWCfc);
		else
			VWC_SScrit2 = VWCsat;


		/* CONTROL */
		if (!errorCode && VWC_SScrit2 > VWCsat)
		{
			if (VWC_SScrit2 - VWCsat > CRIT_PRECwater)
			{
				printf("\n");
				printf("ERROR: VWC_SScrit2 data is greater than saturation value in layer:%i\n", layer);
				errorCode=1; 
			}
			else
				VWC_SScrit2 = VWCsat;
		}

		if (!errorCode && VWC_SScrit2 < VWC_SScrit1)
		{
			if (VWC_SScrit1 - VWC_SScrit2 > CRIT_PRECwater)
			{
				printf("\n");
				printf("ERROR: VWC_SScrit1 data is greater then VWC_SScrit2 data in layer:%i\n", layer);
				errorCode=1; 
			}
			else
				VWC_SScrit1 = VWC_SScrit2;
		}
	
		epv->VWC_SScrit1[layer]		= VWC_SScrit1;
		epv->VWC_SScrit2[layer]		= VWC_SScrit2;
		PSI_SScrit1[layer]             = exp(VWCsat/VWC_SScrit1*log(sprop->soilB[layer]))*sprop->PSIsat[layer];
		PSI_SScrit2[layer]             = exp(VWCsat/VWC_SScrit2*log(sprop->soilB[layer]))*sprop->PSIsat[layer];

	}
	if (ctrl->spinup < 2) fprintf(logfile.ptr, "CRITICAL VALUES OF VWC and PSI FOR 10 SOIL LAYERS\n");  
	if (ctrl->spinup < 2) fprintf(logfile.ptr, "VWC [m3/m3] at limit1:      %12.3f %12.3f %12.3f %12.3f %12.3f %12.3f %12.3f %12.3f %12.3f %12.3f\n", epv->VWC_SScrit1[0], epv->VWC_SScrit1[1], epv->VWC_SScrit1[2], epv->VWC_SScrit1[3], epv->VWC_SScrit1[4], epv->VWC_SScrit1[5], epv->VWC_SScrit1[6], epv->VWC_SScrit1[7], epv->VWC_SScrit1[8], epv->VWC_SScrit1[9]);
	if (ctrl->spinup < 2) fprintf(logfile.ptr, "VWC [m3/m3] at limit2:      %12.3f %12.3f %12.3f %12.3f %12.3f %12.3f %12.3f %12.3f %12.3f %12.3f\n", epv->VWC_SScrit2[0], epv->VWC_SScrit2[1], epv->VWC_SScrit2[2], epv->VWC_SScrit2[3], epv->VWC_SScrit2[4], epv->VWC_SScrit2[5], epv->VWC_SScrit2[6], epv->VWC_SScrit2[7], epv->VWC_SScrit2[8], epv->VWC_SScrit2[9]);
	if (ctrl->spinup < 2) fprintf(logfile.ptr, "PSI [MPa]   at limit1:      %12.3f %12.3f %12.3f %12.3f %12.3f %12.3f %12.3f %12.3f %12.3f %12.3f\n", PSI_SScrit1[0], PSI_SScrit1[1], PSI_SScrit1[2], PSI_SScrit1[3], PSI_SScrit1[4], PSI_SScrit1[5], PSI_SScrit1[6], PSI_SScrit1[7], PSI_SScrit1[8], PSI_SScrit1[9]);
	if (ctrl->spinup < 2) fprintf(logfile.ptr, "PSI [MPa]   at limit2:      %12.3f %12.3f %12.3f %12.3f %12.3f %12.3f %12.3f %12.3f %12.3f %12.3f\n", PSI_SScrit2[0], PSI_SScrit2[1], PSI_SScrit2[2], PSI_SScrit2[3], PSI_SScrit2[4], PSI_SScrit2[5], PSI_SScrit2[6], PSI_SScrit2[7], PSI_SScrit2[8], PSI_SScrit2[9]);

	if (ctrl->spinup < 2) fprintf(logfile.ptr, " \n");
		
	return (errorCode);
}
