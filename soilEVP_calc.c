/* 
Elimit_and_PET.c
calculate limitation of soil evaporation and update top soil water content

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
#include "bgc_func.h"
#include "bgc_constants.h"

int soilEVP_calc(control_struct* ctrl, const siteconst_struct* sitec,const soilprop_struct* sprop, epvar_struct* epv, wstate_struct* ws, wflux_struct* wf)
{
	int errorCode=0;
	double soilw_avail;
	
	/* SOILEVAP UPDATE: if GW-table is in the top soil layer, the source of evaporation is the GW-table */
	

	if (sprop->GWeff[0] == 1)
	{
		wf->GWdischarge[0] += wf->soilwEVP;
	}
	else
	{
		/* soilw evaporation - limitation: hygroscopic water */
		soilw_avail = ws->soilw[0] - sprop->VWChw[0] * sitec->soillayer_thickness[0] * water_density;
		if (soilw_avail > wf->soilwEVP)
			ws->soilw[0] -= (wf->soilwEVP);
		else
		{
			wf->soilwEVP = soilw_avail;
			ws->soilw[0] = sprop->VWChw[0] * sitec->soillayer_thickness[0] * water_density;

			/* limitEVP_flag: flag of WARNING writing in log file (only at first time) */
			if (!ctrl->limitEVP_flag) ctrl->limitEVP_flag = 1;
		}
		epv->VWC[0]  = ws->soilw[0] / water_density / sitec->soillayer_thickness[0];

	}


		
	return(errorCode);
}

