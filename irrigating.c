/* 
irrigating.c
irrigating  - irrigating seeds in soil - increase transfer pools

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Biome-BGCMuSo v6.2.
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
#include "pointbgc_struct.h"
#include "bgc_struct.h"
#include "pointbgc_func.h"
#include "bgc_constants.h"

int irrigating(const control_struct* ctrl, const irrigating_struct* IRG, const siteconst_struct* sitec, const soilprop_struct* sprop,
	           epvar_struct* epv, wstate_struct* ws, wflux_struct* wf)
{
	/* irrigating parameters */	   
	
	int errorCode=0;
	int condIRG, condIRG_flag, condIRG_startyr;
	double critVWCbef, critVWCaft, critSOILWaft, condIRG_amount, VWCact_condIRG, VWCfc_condIRG, VWCwp_condIRG, soilw_condIRG;
	double LAIcrit_condIRG;
	int md, year, layer, nl;

	LAIcrit_condIRG = 0.2; // fixedParam
	year = ctrl->simstartyear + ctrl->simyr;
	md = IRG->mgmdIRG-1;
	condIRG_flag=condIRG_startyr=0;
	


	critVWCbef=condIRG_amount=critSOILWaft=VWCact_condIRG=VWCfc_condIRG=VWCwp_condIRG=soilw_condIRG=0;
	condIRG=0;

	/* type of conditional irrigation: 1 -based on SWC, 2: - based on SMSI, 12002 - based on SWC but only after 2002, 22002, based on SMSI but only after 2002 */
	if (IRG->condIRG_flag)
	{
		if (IRG->condIRG_flag == 1) condIRG_flag = 1;
		if (IRG->condIRG_flag == 2) condIRG_flag = 2;
		if (IRG->condIRG_flag > 10000 && IRG->condIRG_flag < 20000) 
		{
			condIRG_flag = 1;
			condIRG_startyr = IRG->condIRG_flag - 10000;
		}
		if (IRG->condIRG_flag > 20000 && IRG->condIRG_flag < 30000) 
		{
			condIRG_flag = 2;
			condIRG_startyr = IRG->condIRG_flag - 20000;
		}
	}
	
	/*  1. CALCULATING FLUXES: amount of water (kgH2O) * (%_to_prop) */
	if (IRG->IRG_num && md >= 0)
	{
		if (year == IRG->IRGyear_array[md] && ctrl->month == IRG->IRGmonth_array[md] && ctrl->day == IRG->IRGday_array[md])
		{
 			wf->IRG_to_prcp=IRG->IRGquantity_array[md];
		}
		
		else
			wf->IRG_to_prcp=0;
	}


	/* 2. conditional irrigating: in case of dry soil */
	if (IRG->condIRG_flag)
	{
		/* number of layer taking into account */
		if (IRG->nLayer_condIRG != DATA_GAP)
		{
			nl = (int) IRG->nLayer_condIRG-1;
			for (layer = 0; layer <= nl; layer++)
			{
				VWCact_condIRG += epv->VWC[layer]     * sitec->soillayer_thickness[layer]/sitec->soillayer_depth[nl];
				VWCfc_condIRG   += sprop->VWCfc[layer] * sitec->soillayer_thickness[layer]/sitec->soillayer_depth[nl];
				VWCwp_condIRG   += sprop->VWCwp[layer] * sitec->soillayer_thickness[layer]/sitec->soillayer_depth[nl];
				soilw_condIRG   += ws->soilw[layer];
			}
		}
		else
		{
			VWCact_condIRG = epv->VWC_RZ;
			VWCfc_condIRG   = epv->VWCfc_RZ;
			VWCwp_condIRG   = epv->VWCwp_RZ;
			soilw_condIRG   = ws->soilw_RZ;
		}
	
		/* in case of critical VWCratio is used */
		if (condIRG_flag == 1 && year >= condIRG_startyr)
		{
			critVWCbef = VWCwp_condIRG + IRG->startPoint_condIRG * (VWCfc_condIRG - VWCwp_condIRG);
			if (VWCact_condIRG < critVWCbef) condIRG = 1;

		}
		/* in case of critical SMSI is used */
		if (condIRG_flag == 2 && year >= condIRG_startyr)
		{
			if (epv->SMSI > IRG->startPoint_condIRG) condIRG = 1;
		}
		

		/* if condIRG - calculation of irrigating amount */
		if (condIRG && epv->proj_lai > LAIcrit_condIRG)
		{
			critVWCaft = VWCwp_condIRG + IRG->aftVWCratio_condIRG * (VWCfc_condIRG - VWCwp_condIRG);

			if (IRG->nLayer_condIRG != DATA_GAP)
				critSOILWaft = critVWCaft * sitec->soillayer_depth[nl] * water_density;
			else
				critSOILWaft = critVWCaft * epv->rootlength * water_density;

			condIRG_amount = critSOILWaft - soilw_condIRG;
			if (condIRG_amount < 0)
			{
				if (ctrl->onscreen && ctrl->spinup == 0) printf("WARNING: too low starting point of cond. IRRIGATION parameter in conditional IRRIGATING\n");
				condIRG_amount = 0;
			}
		}
		

		/* irrigating amount is added to IRG_to_prcp (limitation is possible based on condIRG parameters*/	
		if (condIRG_amount)
		{
			if (condIRG_amount <= IRG->maxAMOUNT_condIRG)
			{
				wf->IRG_to_prcp = condIRG_amount;
				if (ctrl->onscreen && ctrl->spinup == 0) printf("conditional IRRIGATING on %i%s%i\n", ctrl->month, "/", ctrl->day);
			}
			else
			{
				wf->IRG_to_prcp = IRG->maxAMOUNT_condIRG;
				if (ctrl->onscreen && ctrl->spinup == 0) printf("limited conditional IRRIGATING on %i%s%i\n", ctrl->month, "/", ctrl->day);
			}

			ws->condIRGsrc += wf->IRG_to_prcp;
		}

	}
	

   return (errorCode);
}
	