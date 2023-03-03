/* 
flooding.c
flooding  - water from river

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
#include "pointbgc_struct.h"
#include "bgc_struct.h"
#include "pointbgc_func.h"
#include "bgc_constants.h"

int flooding(control_struct* ctrl, const siteconst_struct* sitec, const flooding_struct* FLD, soilprop_struct* sprop, epvar_struct* epv, 
	         wstate_struct* ws, wflux_struct* wf, int* mondays)
{

	int errorCode=0;
	int md, year;
	double FLDheight;	
	int FLDyday_start, FLDyday_end;

	FLDheight = 0;
	year = ctrl->simstartyear + ctrl->simyr;
	md = FLD->mgmdFLD-1;

	/* 2. we assume that the transfer pools contain the palnt material of seeds. Therefore planting increase the transfer pools */ 
	if (FLD->FLD_num && md >= 0)
	{
		FLDyday_start = date_to_doy(mondays, FLD->FLDstart_month_array[md], FLD->FLDstart_day_array[md]);
		FLDyday_end   = date_to_doy(mondays, FLD->FLDend_month_array[md], FLD->FLDend_day_array[md]);

	
		if (year == FLD->FLDstart_year_array[md] && ctrl->yday >= FLDyday_start && ctrl->yday <= FLDyday_end) 
		{	
			FLDheight = FLD->FLDheight[md];
			if (epv->VWC[0] < sprop->VWCsat[0])
			{
				wf->FLD_to_soilw  = (sprop->VWCsat[0]-epv->VWC[0])*sitec->soillayer_thickness[0]* water_density;
				ws->soilw[0]     += wf->FLD_to_soilw;
				epv->VWC[0]       = ws->soilw[0] / (sitec->soillayer_thickness[0]* water_density);
				if (sprop->VWCsat[0] - epv->VWC[0] > CRIT_PRECwater)
				{
					printf("\n");
					printf("ERROR: in water saturation calculation in flooding.c\n");
					errorCode=1;
				}
			}

			/* unit of FLDheight is mm - 1 mm water on 1m2 surface is 1 kg */
			if (FLDheight > ws->pondw)
			{
				wf->FLD_to_pondw  = FLDheight - ws->pondw;
				ws->pondw        += wf->FLD_to_pondw;
			}
		}
		
		
		
	}
	return (errorCode);
}