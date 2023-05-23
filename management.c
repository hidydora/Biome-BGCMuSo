/* 
management.c
manages the management issues

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

int management(control_struct* ctrl, fertilizing_struct* FRZ, grazing_struct* GRZ, harvesting_struct* HRV, mowing_struct* MOW, 
			   planting_struct* PLT, ploughing_struct* PLG, thinning_struct* THN, irrigating_struct* IRG, 
			   mulching_struct* MUL, CWDextract_struct* CWE, flooding_struct* FLD, groundwater_struct* GWS, int* mondays)
{

	int errorCode=0;
	int md = 0;
	int year;
	int GRZstart_yday, GRZend_yday, FLDstart_yday, FLDend_yday, yday_wyr;

	year = ctrl->simstartyear + ctrl->simyr;

	/* do planting if PLT_num > 0  */
	if (PLT->PLT_num)
	{
		md = PLT->mgmdPLT;
		if (year == PLT->PLTyear_array[md] && ctrl->month == PLT->PLTmonth_array[md] && ctrl->day == PLT->PLTday_array[md])
		{
			PLT->mgmdPLT += 1;
			if (ctrl->onscreen && ctrl->spinup != 1 && ctrl->simyr == 0) printf("PLANTING on %i%s%i\n", ctrl->month, "/", ctrl->day);
		}
	}

	/* do thinning if PLT_num > 0 */
	if (THN->THN_num)
	{
		md = THN->mgmdTHN;
		if (year == THN->THNyear_array[md] && ctrl->month == THN->THNmonth_array[md]&& ctrl->day == THN->THNday_array[md])
		{
			THN->mgmdTHN += 1;
			if (ctrl->onscreen && ctrl->spinup != 1 && ctrl->simyr == 0) printf("THINNING on %i%s%i\n", ctrl->month, "/", ctrl->day);
		}
	}

	
	/* do mowing if MOW_num > 0 */
	if (MOW->MOW_num)
	{
		md = MOW->mgmdMOW;
		if (year == MOW->MOWyear_array[md] && ctrl->month == MOW->MOWmonth_array[md]&& ctrl->day == MOW->MOWday_array[md])
		{
			MOW->mgmdMOW += 1;
			if (ctrl->onscreen && ctrl->spinup != 1 && ctrl->simyr == 0) printf("MOWING on %i%s%i\n", ctrl->month, "/", ctrl->day);
		}
	}


	/* do grazing if GRZ_num > 0 */
	if (GRZ->GRZ_num)
	{
		md = GRZ->mgmdGRZ;

		GRZstart_yday = GRZ->GRZstart_year_array[md] * nDAYS_OF_YEAR + date_to_doy(mondays, GRZ->GRZstart_month_array[md], GRZ->GRZstart_day_array[md]);
		GRZend_yday   = GRZ->GRZend_year_array[md] * nDAYS_OF_YEAR + date_to_doy(mondays, GRZ->GRZend_month_array[md], GRZ->GRZend_day_array[md]);
		yday_wyr = (ctrl->simstartyear + ctrl->simyr) * nDAYS_OF_YEAR + ctrl->yday;


		if (yday_wyr >= GRZstart_yday && yday_wyr <= GRZend_yday) 
		{
			if (yday_wyr == GRZend_yday) GRZ->mgmdGRZ += 1;
			if (ctrl->onscreen && ctrl->spinup != 1 && yday_wyr == GRZstart_yday) 
			{
				printf("grazing: FIRST DAY - %i%s%i\n", ctrl->month, "/", ctrl->day);
				printf("grazing: LAST DAY - %i%s%i\n", ctrl->month, "/", ctrl->day);
			}

			GRZ->mgmdGRZ += 1;
		}
	}
	
	
	/* do planting if HRV_num > 0  */
	if (HRV->HRV_num)
	{
		md = HRV->mgmdHRV;
		if (year == HRV->HRVyear_array[md] && ctrl->month == HRV->HRVmonth_array[md] && ctrl->day == HRV->HRVday_array[md])
		{
			HRV->mgmdHRV += 1;
			if (ctrl->onscreen && ctrl->spinup != 1 && ctrl->simyr == 0) printf("HARVESTING on %i%s%i\n", ctrl->month, "/", ctrl->day);
		}
	}

	/* do ploughing if PLG_num > 0  */
	if (PLG->PLG_num)
	{
		md = PLG->mgmdPLG;
		if (year == PLG->PLGyear_array[md] && ctrl->month == PLG->PLGmonth_array[md]&& ctrl->day == PLG->PLGday_array[md])
		{
			PLG->mgmdPLG += 1;
			if (ctrl->onscreen && ctrl->spinup != 1 && ctrl->simyr == 0) printf("PLOUGHING on %i%s%i\n", ctrl->month, "/", ctrl->day);
		}
	}


	/* do fertilizing if FRZ_num > 0 */
	if (FRZ->FRZ_num)
	{
		md = FRZ->mgmdFRZ;
		if (year == FRZ->FRZyear_array[md] && ctrl->month == FRZ->FRZmonth_array[md]&& ctrl->day == FRZ->FRZday_array[md])
		{
			FRZ->mgmdFRZ += 1;
			if (ctrl->onscreen && ctrl->spinup != 1 && ctrl->simyr == 0) printf("FERTILIZING on %i%s%i\n", ctrl->month, "/", ctrl->day);
		}
	}

	/* do irrigating if IRG_num > 0 */
	if (IRG->IRG_num)
	{
		md = IRG->mgmdIRG;
		if (year == IRG->IRGyear_array[md] && ctrl->month == IRG->IRGmonth_array[md]&& ctrl->day == IRG->IRGday_array[md])
		{
			IRG->mgmdIRG += 1;
			if (ctrl->onscreen && ctrl->spinup != 1 && ctrl->simyr == 0) printf("IRRIGATING on %i%s%i\n", ctrl->month, "/", ctrl->day);
		}
	}

	/* do mulching if MUL_num > 0 */
	if (MUL->MUL_num)
	{
		md = MUL->mgmdMUL;
		if (year == MUL->MULyear_array[md] && ctrl->month == MUL->MULmonth_array[md]&& ctrl->day == MUL->MULday_array[md])
		{
			MUL->mgmdMUL += 1;
			if (ctrl->onscreen && ctrl->spinup != 1 && ctrl->simyr == 0) printf("MULCHING on %i%s%i\n", ctrl->month, "/", ctrl->day);
		}
	}

	/* do CWD-extract if CWE_num > 0 */
	if (CWE->CWE_num)
	{
		md = CWE->mgmdCWE;
		if (year == CWE->CWEyear_array[md] && ctrl->month == CWE->CWEmonth_array[md]&& ctrl->day == CWE->CWEday_array[md])
		{
			CWE->mgmdCWE += 1;
			if (ctrl->onscreen && ctrl->spinup != 1 && ctrl->simyr == 0) printf("CWD-extract on %i%s%i\n", ctrl->month, "/", ctrl->day);
		}
	}

	/* do flooding if FLD_num > 0 */
	if (FLD->FLD_num)
	{
		md = FLD->mgmdFLD;
		FLDstart_yday = FLD->FLDstart_year_array[md] * nDAYS_OF_YEAR + date_to_doy(mondays, FLD->FLDstart_month_array[md], FLD->FLDstart_day_array[md]);
		FLDend_yday   = FLD->FLDend_year_array[md] * nDAYS_OF_YEAR + date_to_doy(mondays, FLD->FLDend_month_array[md], FLD->FLDend_day_array[md]);
		yday_wyr = (ctrl->simstartyear + ctrl->simyr) * nDAYS_OF_YEAR + ctrl->yday;


		if (yday_wyr >= FLDstart_yday && yday_wyr <= FLDend_yday) 
		{
			if (yday_wyr == FLDend_yday) FLD->mgmdFLD += 1;
			if (ctrl->onscreen && ctrl->spinup != 1 && yday_wyr == FLDstart_yday) 
			{
				printf("flooding: FIRST DAY - %i%s%i\n", ctrl->month, "/", ctrl->day);
				printf("flooding: LAST DAY - %i%s%i\n", ctrl->month, "/", ctrl->day);
			}

			FLD->mgmdFLD += 1;
		}
	}

	/* do groundwater if GWD_num > 0 */
	if (GWS->GWD_num)
	{
		md = GWS->mgmdGWD;

	
		if (year == GWS->GWyear_array[md] && ctrl->month == GWS->GWmonth_array[md] && ctrl->day == GWS->GWday_array[md])
		{
			GWS->mgmdGWD += 1;
			if (ctrl->onscreen && ctrl->spinup != 1 && ctrl->simyr == 0) printf("groundwater on %i%s%i\n", ctrl->month, "/", ctrl->day);
		}
	}

   return (errorCode);
}
	