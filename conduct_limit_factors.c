/* 
conduct_limit_factors.c
calculate the limitation factors of conductance
*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Biome-BGC version 4.1.1
Copyright 2008, Hidy
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


int conduct_limit_factors(const siteconst_struct* sitec, const epconst_struct* epc, epvar_struct* epv)
{
	int ok = 1;


	/* -------------------------------------------*/
	/* if no avaialbel data: the open and closing VWC value is calculated from PSI set by user or if no PSI adat is available
	field capacity and wilting point values are used */
	
	if (epc->vwc_ratio_open == DATA_GAP)
	{
		if (epc->psi_open == DATA_GAP)
		{
			epv->vwc_ratio_open  = 1.0;
			epv->vwc_open = sitec->vwc_fc;
			epv->psi_open = sitec->psi_fc;
		}
		else
		{
			epv->psi_open = epc->psi_open; 
			epv->vwc_open = sitec->vwc_sat * (log(sitec->soil_b) / log(epc->psi_open/sitec->psi_sat));
			epv->vwc_ratio_open = epv->vwc_open/sitec->vwc_fc;
		}
	}
	else
	{
		epv->vwc_ratio_open = epc->vwc_ratio_open;
		epv->vwc_open = sitec->vwc_fc * epc->vwc_ratio_open;
		epv->psi_open = sitec->psi_sat * pow((epv->vwc_open/sitec->vwc_fc), sitec->soil_b);
	}


	if (epc->vwc_ratio_close == DATA_GAP)
	{
		if (epc->psi_close == DATA_GAP)
		{
			epv->vwc_ratio_close = sitec->vwc_wp/sitec->vwc_fc;
			epv->vwc_close = sitec->vwc_wp;
			epv->psi_close = sitec->psi_wp;
		}
		else
		{
			epv->psi_close = epc->psi_open; 
			epv->vwc_close = sitec->vwc_sat * (log(sitec->soil_b) / log(epc->psi_close/sitec->psi_sat));
			epv->vwc_ratio_close = epv->vwc_close/sitec->vwc_fc;
		}
	}
	else
	{
		epv->vwc_ratio_close = epc->vwc_ratio_close;
		epv->vwc_close = sitec->vwc_fc * epc->vwc_ratio_close;
		epv->psi_close = sitec->psi_sat * pow((epv->vwc_close/sitec->vwc_fc), sitec->soil_b);
	}

	

		
	return (!ok);
}
