/* 
conduct_limit_factors.c
calculate the limitation factors of conductance
*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
BBGC MuSo 2.3
Copyright 2014, D. Hidy
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
	/* control: using vwc_ratio OR psi to calculate conductance limitation */
	if ((epc->relVWC_crit1  != (double) DATA_GAP && epc->relPSI_crit1  != (double) DATA_GAP) ||
		(epc->relVWC_crit2 != (double) DATA_GAP && epc->relPSI_crit2 != (double) DATA_GAP))
	{
		printf("Warning: if relSWC and relPSI data are set simultaneously in EPC file, relSWC data are used\n");
	
	}

	/* -------------------------------------------*/
	/* if no relSWC data are available: the crit1 and crit2 VWC value is calculated from relPSI
	   if no relPSI data are available: the crit1 and crit2 VWC value is calculated from relSWC  */
	
	if (epc->relVWC_crit1 == (double) DATA_GAP)
	{
		if (epc->relPSI_crit1 == (double) DATA_GAP)
		{
			epv->psi_crit1 = sitec->psi_fc;  
			epv->vwc_crit1 = sitec->vwc_fc;
		}
		else
		{
			epv->psi_crit1 = sitec->psi_fc * epc->relPSI_crit1;  
			epv->vwc_crit1 = sitec->vwc_sat * (log(sitec->soil_b) / log(epv->psi_crit1/sitec->psi_sat));
		}
	}
	else
	{
		epv->vwc_crit1 = sitec->vwc_fc * epc->relVWC_crit1;
		epv->psi_crit1 = sitec->psi_fc * pow((epv->vwc_crit1/sitec->vwc_fc), -1*sitec->soil_b);
	}
	


	if (epc->relVWC_crit2 == (double) DATA_GAP)
	{
		if (epc->relPSI_crit2 == (double) DATA_GAP)
		{
			epv->psi_crit2 = sitec->psi_sat;  
			epv->vwc_crit2 = sitec->vwc_sat;
		}
		else
		{
			epv->psi_crit2 = sitec->psi_fc * epc->relPSI_crit2;  
			if (epv->psi_crit2/sitec->psi_sat < sitec->soil_b)
			{
				 if (epv->psi_crit2/sitec->psi_sat < 1)
				 {
					 printf("Error: relPSI_crit2 in EPC file MUST less than 1 (saturation value)\n");
					 ok=0;
				 }
				 else
				 {
					 epv->vwc_crit2 = sitec->vwc_sat;
					 printf("Warning: relPSI_crit2 in EPC file MUST less than a soil-dependent value -> saturation values used to calculate soil moisture limitation factors: conduct_limit_factors()\n");
				 }
			}
			else
				epv->vwc_crit2 = sitec->vwc_sat * (log(sitec->soil_b) / log(epv->psi_crit2/sitec->psi_sat));
		}
	}
	else
	{
		epv->vwc_crit2 = sitec->vwc_fc * epc->relVWC_crit2;
		epv->psi_crit2 = sitec->psi_sat * pow((epv->vwc_crit2/sitec->vwc_sat), -1*sitec->soil_b);
	}

	epv->vwc_ratio_crit1  = (epv->vwc_crit1 - sitec->vwc_wp)/(sitec->vwc_sat - sitec->vwc_wp);
	epv->vwc_ratio_crit2  = (epv->vwc_crit2 - sitec->vwc_wp)/(sitec->vwc_sat - sitec->vwc_wp);

	/* CONTROL */
	if (epv->vwc_ratio_crit1 > epv->vwc_ratio_crit2)
	{
		 epv->psi_crit2 = sitec->psi_sat;
		 epv->vwc_crit2 = sitec->vwc_sat;
		 printf("Error: VWC_crit2 in EPC file MUST GREATER than VWC_crit1 value, please check EPC file (relVWC_crit or relPSI_crit values) \n");
		 ok=0;
	}
		
	return (!ok);
}
