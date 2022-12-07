/* 
pondANDrunoffD.c
state update of pond water, calculation of Dunnian runoff 

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


int pondANDrunoffD(control_struct* ctrl, siteconst_struct* sitec, soilprop_struct* sprop, epvar_struct* epv, wstate_struct* ws, wflux_struct* wf)
{

	/* internal variables */
	int errorCode, layer, flagEXTRA;
	double soilw_diff, soilw_sat;

	errorCode=layer=flagEXTRA=0;
	soilw_diff=soilw_sat=0;


	/*--------------------------------------*/
	/* Water flux from soil to pond */
	
	ws->pondw += wf->soilw_to_pondw + wf->prcp_to_pondw;

	/*--------------------------------------*/
	/* Pond water evaporation: water stored on surface  */

	if (ws->pondw)
	{
		if (wf->potEVPsurface < ws->pondw)
			wf->pondwEVP = wf->potEVPsurface;
		else 
			wf->pondwEVP = ws->pondw;

		soilw_diff   = wf->soilwEVP - (wf->potEVPsurface-wf->pondwEVP);
		soilw_sat     = sprop->VWCsat[layer] * sitec->soillayer_thickness[layer] * water_density;
		if (ws->soilw[0] + soilw_diff > soilw_sat)
		{
			soilw_diff = soilw_sat - ws->soilw[0];
		}
			
		ws->soilw[0] += soilw_diff;
		epv->VWC[0]   = ws->soilw[0] / (sitec->soillayer_thickness[0] * water_density);
		wf->soilwEVP -= soilw_diff;

		ws->pondw    -= wf->pondwEVP;
	}


	
	/* saturation of top soil layer: correction of water flux from pond to soil */
	soilw_diff = (sprop->VWCsat[0] - epv->VWC[0]) * sitec->soillayer_thickness[0] * water_density;

	if (soilw_diff && ws->pondw)
	{
		if (soilw_diff > ws->pondw)
			soilw_diff = ws->pondw;

		ws->pondw          -= soilw_diff;
		wf->pondw_to_soilw += soilw_diff;

		ws->soilw[0]       += soilw_diff;
		epv->VWC[0]         = ws->soilw[0] / (sitec->soillayer_thickness[0] * water_density);
		if (fabs(epv->VWC[0] - sprop->VWCsat[0]) > CRIT_PRECwater && ws->pondw)
		{
			printf("\n");
			printf("ERROR: pondwANDrunoffD() in tipping.c\n");
			errorCode=1;
		}
	}

	
	/* Dunnian runoff */
	if (ws->pondw > sprop->pondmax)
	{
		wf->pondw_to_runoff  = ws->pondw - sprop->pondmax;
		ws->pondw            = sprop->pondmax;	
	}


	/* pond_flag: flag of WARNING writing (only at first time) */
	if (!ctrl->pond_flag ) ctrl->pond_flag = 1;


	return (errorCode);
}
