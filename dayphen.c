/* 
dayphen.c
transfer one day of phenological data from phenarr struct to phen struct

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Biome-BGCMuSo v6.0.
Original code: Copyright 2000, Peter E. Thornton
Numerical Terradynamic Simulation Group, The University of Montana, USA
Modified code: Copyright 2019, D. Hidy [dori.hidy@gmail.com]
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

int dayphen(control_struct* ctrl, const epconst_struct* epc, const phenarray_struct* phenarr, const planting_struct* PLT, 
	        epvar_struct* epv, phenology_struct* phen)
{
	int errflag=0;
	int nyear;


	/* determining onday, offday, n_growthday, n_transferday and n_litfallday value on the first simulation day */

 	if (PLT->PLT_num)
		nyear = PLT->PLT_num;
	else
		nyear = ctrl->simyears;

	/* spinup: resetting plantyr with simyr */
	if (ctrl->plantyr+1 == nyear)
	{
		ctrl->plantyr = -1;
	}

	/* start of year: when onday or in the first siulation day in year in case of bareground simulation (onday == DATA_GAP and offday ==  DATA_GAP) */
	if (ctrl->plantyr+1 < nyear && phen->onday == -1 && 
		((ctrl->simyr+ctrl->simstartyear == phenarr->onday_arr[ctrl->plantyr+1][0] && 
		 (ctrl->yday == phenarr->onday_arr[ctrl->plantyr+1][1] || phenarr->onday_arr[ctrl->plantyr+1][1] == DATA_GAP))))
	{
		ctrl->plantyr += 1;
		
		phen->onday         = (double)(phenarr->onday_arr[ctrl->plantyr][1]) + NDAYS_OF_YEAR * (phenarr->onday_arr[ctrl->plantyr][0] - ctrl->simstartyear);
		phen->offday        = (double)(phenarr->offday_arr[ctrl->plantyr][1] + NDAYS_OF_YEAR * (phenarr->offday_arr[ctrl->plantyr][0] - ctrl->simstartyear));

		if (phen->offday <= phen->onday && (phen->offday != DATA_GAP && phen->onday != DATA_GAP))
		{
			printf("FATAL ERROR: onday is greater or equal than offday (dayphen.c)\n");
			errflag=1;
		}

	}

	phen->n_growthday   = phen->offday - phen->onday + 1;
	phen->n_transferday = floor(phen->n_growthday * epc->transfer_pdays);
	phen->n_litfallday  = floor(phen->n_growthday * epc->litfall_pdays);
	


	
	

	return(errflag);


}
	
