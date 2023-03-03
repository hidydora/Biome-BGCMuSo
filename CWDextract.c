/* 
CWDextract.c
CWD-extract  - remove CWD from forest

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

int CWDextract(control_struct* ctrl, const CWDextract_struct* CWE, cstate_struct* cs, nstate_struct*ns, cflux_struct* cf, nflux_struct* nf)
{

	int errorCode=0;
	int md, year;
	double removePROP, cwdc_to_CWE, remainCWE;			            

	removePROP = 0;
	year = ctrl->simstartyear + ctrl->simyr;
	md = CWE->mgmdCWE-1;

	/* 2. we assume that the transfer pools contain the palnt material of seeds. Therefore planting increase the transfer pools */ 
	if (CWE->CWE_num && md >= 0)
	{
		if (year == CWE->CWEyear_array[md] && ctrl->month == CWE->CWEmonth_array[md] && ctrl->day == CWE->CWEday_array[md]) 
		{
			
			removePROP  = CWE->removePROP_CWE[md]/100.;
			cwdc_to_CWE = cs->cwdc_above * removePROP;

			/* if removed material is greater than 0 */
			if (cwdc_to_CWE)
			{
				/* if the first layer covers CWE */
				if (cs->cwdc[0] >= cwdc_to_CWE)
				{
					cf->cwdc0_to_CWE  = cwdc_to_CWE;
				}
				else
				{
					
					cf->cwdc0_to_CWE  = cs->cwdc[0];
					remainCWE         = cwdc_to_CWE - cf->cwdc0_to_CWE;
					
					/* if the second layer covers CWE */
					if (cs->cwdc[1] >= remainCWE)
					{
						cf->cwdc1_to_CWE  = remainCWE;
					}
					else
					{					
						printf("\n");
						printf("ERROR in CWDextract calculation in CWDextract.c\n");
						errorCode=1;
					}
				}	
			}
		}	
	}

	/* nitrogen fluxes from the ratio of CWD in given layer */
	if (cf->cwdc0_to_CWE > 0)
	{
		if (cs->cwdc[0] == 0)
		{
			printf("\n");
			printf("ERROR in CWDextract calculation in CWDextract.c\n");
			errorCode=1;
		}
		else
			nf->cwdn0_to_CWE  = cf->cwdc0_to_CWE * (ns->cwdn[0]/cs->cwdc[0]);
	}


	if (cf->cwdc1_to_CWE > 0)
	{
		if (cs->cwdc[1] == 0)
		{
			printf("\n");
			printf("ERROR in CWDextract calculation in CWDextract.c\n");
			errorCode=1;
		}
		else
			nf->cwdn1_to_CWE  = cf->cwdc1_to_CWE * (ns->cwdn[1]/cs->cwdc[1]);
	}
	
	/* state update */
	cs->cwdc[0] -= 	cf->cwdc0_to_CWE;	
	cs->cwdc[1] -= 	cf->cwdc1_to_CWE;
	ns->cwdn[0] -= 	nf->cwdn0_to_CWE;	
	ns->cwdn[1] -= 	nf->cwdn1_to_CWE;	

	cs->CWEsnk_C += cf->cwdc0_to_CWE + cf->cwdc1_to_CWE;
	ns->CWEsnk_N += nf->cwdn0_to_CWE + nf->cwdn1_to_CWE;

	
	return (errorCode);
}