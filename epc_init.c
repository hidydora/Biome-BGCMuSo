/* 
epc_init.c
read epc file for pointbgc simulation

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Biome-BGC version 4.1.1
Copyright 2000, Peter E. Thornton
Numerical Terradynamics Simulation Group (NTSG)
School of Forestry, University of Montana
Missoula, MT 59812
*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
20.03.01 Galina Churkina added variable "sum" substituting  t1+t2+t3 in IF statement,
which gave an error.
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



int epc_init(file init, const siteconst_struct* sitec, epconst_struct* epc, control_struct* ctrl)
{
	int ok = 1;
	double t1,t2,t3,t4,r1,sum;
	int i;
	file temp, wpm_file, msc_file; 	// Hidy 2011.
	char key1[] = "EPC_FILE";
	char key2[] = "ECOPHYS";
	char keyword[80];

	
	/********************************************************************
	**                                                                 **
	** Begin reading initialization file block starting with keyword:  **
	** EPC_FILES                                                       ** 
	**                                                                 **
	********************************************************************/
	
	/* scan for the EPC file keyword, exit if not next */
	if (ok && scan_value(init, keyword, 's'))
	{
		printf("Error reading keyword for control data\n");
		ok=0;
	}
	if (ok && strcmp(keyword, key1))
	{
		printf("Expecting keyword --> %s in file %s\n",key1,init.name);
		ok=0;
	}

	/* open file  */
	if (ok && scan_open(init,&temp,'r')) 
	{
		printf("Error opening epconst file, epc_init()\n");
		ok=0;
	}
	
	/* first scan epc keyword to ensure proper *.init format */
	if (ok && scan_value(temp, keyword, 's'))
	{
		printf("Error reading keyword, epc_init()\n");
		ok=0;
	}
	if (ok && strcmp(keyword,key2))
	{
		printf("Expecting keyword --> %s in %s\n",key2,init.name);
		ok=0;
	}
	
	/* begin reading constants from *.init */
	if (ok && scan_value(temp, &epc->woody, 'i'))
	{
		printf("Error reading woody/non-woody flag, epc_init()\n");
		ok=0;
	}
	if (ok && scan_value(temp, &epc->evergreen, 'i'))
	{
		printf("Error reading evergreen/deciduous flag, epc_init()\n");
		ok=0;
	}
	if (ok && scan_value(temp, &epc->c3_flag, 'i'))
	{
		printf("Error reading C3/C4 flag, epc_init()\n");
		ok=0;
	}
	if (ok && scan_value(temp, &epc->phenology_flag, 'i'))
	{
		printf("Error reading phenology flag, epc_init()\n");
		ok=0;
	}
	if (ok && scan_value(temp, &epc->onday, 'i'))
	{
		printf("Error reading onday, epc_init()\n");
		ok=0;
	}
	if (ok && scan_value(temp, &epc->offday, 'i'))
	{
		printf("Error reading offday, epc_init()\n");
		ok=0;
	}
	if (ok && scan_value(temp, &epc->transfer_pdays, 'd'))
	{
		printf("Error reading transfer_pdays, epc_init()\n");
		ok=0;
	}
	if (ok && scan_value(temp, &epc->litfall_pdays, 'd'))
	{
		printf("Error reading litfall_pdays, epc_init()\n");
		ok=0;
	}
	if (ok && scan_value(temp, &epc->leaf_turnover, 'd'))
	{
		printf("Error reading leaf turnover, epc_init()\n");
		ok=0;
	}
	/* force leaf turnover fraction to 1.0 if deciduous */
	if (!epc->evergreen)
	{
		epc->leaf_turnover = 1.0;
	}
	epc->froot_turnover = epc->leaf_turnover;
	if (ok && scan_value(temp, &epc->livewood_turnover, 'd'))
	{
		printf("Error reading livewood turnover, epc_init()\n");
		ok=0;
	}
	if (ok && scan_value(temp, &t1, 'd'))
	{
		printf("Error reading whole-plant mortality, epc_init()\n");
		ok=0;
	}
	epc->daily_mortality_turnover = t1/365;
	/* ------------------------------------------------------ */
	/* Hidy 2011 - using varying whole plant mortality values */


	strcpy(wpm_file.name, "mortality.txt");
	
	/* open the main init file for ascii read and check for errors */
	if (file_open(&wpm_file,'i') || ctrl->spinup)
	{
		ctrl->varWPM_flag = 0;
	}
	else ctrl->varWPM_flag = 1;


	/* allocate space for the annual WPM array */
	if (!(epc->wpm_array = (double*) malloc(ctrl->simyears * sizeof(double))))
	{
		printf("Error allocating for annual WPM array, epc_init()\n");
		ok=0;
	}
	if (ctrl->varWPM_flag) 
	{
		printf("mortality.txt is found, changing  WPM is used\n");

		/* read year and co2 concentration for each simyear */
		for (i=0 ; ok && i<ctrl->simyears ; i++)
		{
			if (fscanf(wpm_file.ptr,"%*lf%lf", &(epc->wpm_array[i]))==EOF)
			{
				printf("Error reading annual WPM array, epc_init()\n");
				printf("Note: file must contain a pair of values for each\n");
				printf("simyear: year and WPM.\n");
				ok=0;
			}
			if (epc->wpm_array[i] < 0.0)
			{
				printf("Error in epc_init(): wpm must be positive\n");
				ok=0;
			}
		}
		fclose(wpm_file.ptr);
	}	
	else /* if no changing data constant EPC parameter are used - ATTENTION: WPM = daily_mortality_turnover * 365 */
	{
		if (!ctrl->spinup)
		{
			printf("mortality.txt not found, constant WPM is used\n"); 
		}
		
		for (i=0 ; ok && i<ctrl->simyears ; i++)
		{
			epc->wpm_array[i]=DATA_GAP;
		}
	}	
	

	if (ok && scan_value(temp, &t1, 'd'))
	{
		printf("Error reading fire mortality, epc_init()\n");
		ok=0;
	}
	epc->daily_fire_turnover = t1/365;
	if (ok && scan_value(temp, &epc->alloc_frootc_leafc, 'd'))
	{
		printf("Error reading froot C:leaf C, epc_init()\n");
		ok=0;
	}
	if (ok && scan_value(temp, &epc->alloc_newstemc_newleafc, 'd'))
	{
		printf("Error reading new stemC:new leaf C, epc_init()\n");
		ok=0;
	}
	if (ok && scan_value(temp, &epc->alloc_newlivewoodc_newwoodc, 'd'))
	{
		printf("Error reading new livewood C:new wood C, epc_init()\n");
		ok=0;
	}
	if (ok && scan_value(temp, &epc->alloc_crootc_stemc, 'd'))
	{
		printf("Error reading croot C:stem C, epc_init()\n");
		ok=0;
	}
	if (ok && scan_value(temp, &epc->alloc_prop_curgrowth, 'd'))
	{
		printf("Error reading new growth:storage growth, epc_init()\n");
		ok=0;
	}
	if (ok && scan_value(temp, &epc->leaf_cn, 'd'))
	{
		printf("Error reading average leaf C:N, epc_init()\n");
		ok=0;
	}
	if (ok && scan_value(temp, &epc->leaflitr_cn, 'd'))
	{
		printf("Error reading leaf litter C:N, epc_init()\n");
		ok=0;
	}
	/* test for leaflitter C:N > leaf C:N */
	if (epc->leaflitr_cn < epc->leaf_cn)
	{
		printf("Error: leaf litter C:N must be >= leaf C:N\n");
		printf("change the values in ECOPHYS block of initialization file\n");
		ok=0;
	}
	if (ok && scan_value(temp, &epc->froot_cn, 'd'))
	{
		printf("Error reading initial fine root C:N, epc_init()\n");
		ok=0;
	}
	if (ok && scan_value(temp, &epc->livewood_cn, 'd'))
	{
		printf("Error reading initial livewood C:N, epc_init()\n");
		ok=0;
	}
	if (ok && scan_value(temp, &epc->deadwood_cn, 'd'))
	{
		printf("Error reading initial deadwood C:N, epc_init()\n");
		ok=0;
	}
	/* test for deadwood C:N > livewood C:N */
	if (epc->deadwood_cn < epc->livewood_cn)
	{
		printf("Error: livewood C:N must be >= deadwood C:N\n");
		printf("change the values in ECOPHYS block of initialization file\n");
		ok=0;
	}
	if (ok && scan_value(temp, &t1, 'd'))
	{
		printf("Error reading leaf litter labile proportion, epc_init()\n");
		ok=0;
	}
	epc->leaflitr_flab = t1;
	if (ok && scan_value(temp, &t2, 'd'))
	{
		printf("Error reading leaf litter cellulose proportion, epc_init()\n");
		ok=0;
	}
	if (ok && scan_value(temp, &t3, 'd'))
	{
		printf("Error reading leaf litter lignin proportion, epc_init()\n");
		ok=0;
	}
	epc->leaflitr_flig = t3;
	/* test for litter fractions sum to 1.0 */
	if (ok && (fabs(t1+t2+t3-1.0) > 1e-10))
	{
		printf("Error:\n");
		printf("leaf litter proportions of labile, cellulose, and lignin\n");
		printf("must sum to 1.0. Check initialization file and try again.\n");
		ok=0;
	}
	/* calculate shielded and unshielded cellulose fraction */
	if (ok)
	{
		r1 = t3/t2;
		if (r1 <= 0.45)
		{
			epc->leaflitr_fscel = 0.0;
			epc->leaflitr_fucel = t2;
		}
		else if (r1 > 0.45 && r1 < 0.7)
		{
			t4 = (r1 - 0.45)*3.2;
			epc->leaflitr_fscel = t4*t2;
			epc->leaflitr_fucel = (1.0 - t4)*t2;
		}
		else
		{
			epc->leaflitr_fscel = 0.8*t2;
			epc->leaflitr_fucel = 0.2*t2;
		}
	}
	if (ok && scan_value(temp, &t1, 'd'))
	{
		printf("Error reading froot litter labile proportion, epc_init()\n");
		ok=0;
	}
	epc->frootlitr_flab = t1;
	if (ok && scan_value(temp, &t2, 'd'))
	{
		printf("Error reading froot litter cellulose proportion, epc_init()\n");
		ok=0;
	}
	if (ok && scan_value(temp, &t3, 'd'))
	{
		printf("Error reading froot litter lignin proportion, epc_init()\n");
		
		ok=0;
	}
	epc->frootlitr_flig = t3;
	/* test for litter fractions sum to 1.0 */
	sum=t1+t2+t3;		/* NEW 20.03.01*/
	//printf("sum %.10f\n",sum);
	if (ok && sum != 1.0)
	{
		printf("Error in fractions: %.2f %.2f %.2f %.3f\n",t1, t2, t3, t1+t2+t3);
		printf("froot litter proportions of labile, cellulose, and lignin\n");
		printf("must sum to 1.0. Check initialization file and try again.\n");
		ok=0;
	}
	/* calculate shielded and unshielded cellulose fraction */
	if (ok)
	{
		r1 = t3/t2;
		if (r1 <= 0.45)
		{
			epc->frootlitr_fscel = 0.0;
			epc->frootlitr_fucel = t2;
		}
		else if (r1 > 0.45 && r1 < 0.7)
		{
			t4 = (r1 - 0.45)*3.2;
			epc->frootlitr_fscel = t4*t2;
			epc->frootlitr_fucel = (1.0 - t4)*t2;
		}
		else
		{
			epc->frootlitr_fscel = 0.8*t2;
			epc->frootlitr_fucel = 0.2*t2;
		}
	}
	if (ok && scan_value(temp, &t1, 'd'))
	{
		printf("Error reading dead wood %% cellulose, epc_init()\n");
		ok=0;
	}
	if (ok && scan_value(temp, &t2, 'd'))
	{
		printf("Error reading dead wood %% lignin, epc_init()\n");
		ok=0;
	}
	epc->deadwood_flig = t2;
	/* test for litter fractions sum to 1.0 */
	sum=t1+t2;	/* NEW 20.03.01*/
	if (ok && sum != 1.0)
	{
		printf("Error:\n");
		printf("deadwood proportions of cellulose and lignin must sum\n");
		printf("to 1.0. Check initialization file and try again.\n");
		ok=0;
	}
	/* calculate shielded and unshielded cellulose fraction */
	if (ok)
	{
		r1 = t2/t1;
		if (r1 <= 0.45)
		{
			epc->deadwood_fscel = 0.0;
			epc->deadwood_fucel = t1;
		}
		else if (r1 > 0.45 && r1 < 0.7)
		{
			t4 = (r1 - 0.45)*3.2;
			epc->deadwood_fscel = t4*t1;
			epc->deadwood_fucel = (1.0 - t4)*t1;
		}
		else
		{
			epc->deadwood_fscel = 0.8*t1;
			epc->deadwood_fucel = 0.2*t1;
		}
	}
	if (ok && scan_value(temp, &epc->int_coef, 'd'))
	{
		printf("Error reading canopy water int coef, epc_init()\n");
		ok=0;
	}
	if (ok && scan_value(temp, &epc->ext_coef, 'd'))
	{
		printf("Error reading canopy light ext coef, epc_init()\n");
		ok=0;
	}
	if (ok && scan_value(temp, &epc->lai_ratio, 'd'))
	{
		printf("Error reading all to projected LA ratio, epc_init()\n");
		ok=0;
	}
	if (ok && scan_value(temp, &epc->avg_proj_sla, 'd'))
	{
		printf("Error reading canopy average projected specific leaf area, epc_init()\n");
		ok=0;
	}
	if (ok && scan_value(temp, &epc->sla_ratio, 'd'))
	{
		printf("Error reading shaded to sunlit SLA ratio, epc_init()\n");
		ok=0;
	}
	if (ok && scan_value(temp, &epc->flnr, 'd'))
	{
		printf("Error reading Rubisco N fraction, epc_init()\n");
		ok=0;
	}
	if (ok && scan_value(temp, &epc->gl_smax, 'd'))
	{
		printf("Error reading gl_smax, epc_init()\n");
		ok=0;
	}

		/* ------------------------------------------------------ */
	/* Hidy 2013 - using varying maximum stomatal conductance values */


	strcpy(msc_file.name, "conductance.txt");
	
	/* open the main init file for ascii read and check for errors */
	if (file_open(&msc_file,'i') || ctrl->spinup)
	{
		ctrl->varMSC_flag = 0;
	}
	else ctrl->varMSC_flag = 1;


	/* allocate space for the annual MSC array */
	if (!(epc->msc_array = (double*) malloc(ctrl->simyears * sizeof(double))))
	{
		printf("Error allocating for annual MSC array, epc_init()\n");
		ok=0;
	}
	if (ctrl->varMSC_flag) 
	{
		printf("conductance.txt is found, changing  MSC is used\n");

		/* read year and co2 concentration for each simyear */
		for (i=0 ; ok && i<ctrl->simyears ; i++)
		{
			if (fscanf(msc_file.ptr,"%*lf%lf", &(epc->msc_array[i]))==EOF)
			{
				printf("Error reading annual MSC array, epc_init()\n");
				printf("Note: file must contain a pair of values for each\n");
				printf("simyear: year and MSC.\n");
				ok=0;
			}
			if (epc->msc_array[i] < 0.0)
			{
				printf("Error in epc_init(): msc must be positive\n");
				ok=0;
			}
		}
		fclose(msc_file.ptr);
	}	
	else /* if no changing data constant EPC parameter are used */
	{
		if (!ctrl->spinup)
		{
			printf("conductance.txt not found, constant MSC is used\n"); 
		}
		
		for (i=0 ; ok && i<ctrl->simyears ; i++)
		{
			epc->msc_array[i]=DATA_GAP;
		}
	}	
	
	/* ------------------------------------------------------ */
	if (ok && scan_value(temp, &epc->gl_c, 'd'))
	{
		printf("Error reading gl_c, epc_init()\n");
		ok=0;
	}
	if (ok && scan_value(temp, &epc->gl_bl, 'd'))
	{
		printf("Error reading gl_bl, epc_init()\n");
		ok=0;
	}
    /******************************************************************-/
	/* Hidy 2012: - multiplier for conductance limitation*/
	
	if (ok && scan_value(temp, &epc->vwc_ratio_open, 'd'))
	{
		printf("Error reading vwc_ratio_open, epc_init()\n");
		ok=0;
	}
	
    if (ok && scan_value(temp, &epc->vwc_ratio_close, 'd'))
	{
		printf("Error reading vwc_ratio_close, epc_init()\n");
		ok=0;
	}

	/* -------------------------------------------*/
	/* multiplier for PSI */
	if (ok && scan_value(temp, &epc->psi_open, 'd'))
	{
		printf("Error reading psi_open, epc_init()\n");
		ok=0;
	}
	
    if (ok && scan_value(temp, &epc->psi_close, 'd'))
	{
		printf("Error reading psi_close, epc_init()\n");
		ok=0;
	}
	/* -------------------------------------------*/
	/* control: using vwc_ratio OR psi to calculate conductance limitation */
	if ((epc->vwc_ratio_open != DATA_GAP || epc->vwc_ratio_close != DATA_GAP) && (epc->psi_open != DATA_GAP || epc->psi_close != DATA_GAP) )
	{
		printf("Warning: if any VWC_ratio data is available, VWC_ratio is used to calculate conductance limitation()\n");
		printf("Check epc file, set PSI_close and PSI_open data to -999.9 and try again.\n");
		ok=0;
	}

	/******************************************************************/

	if (ok && scan_value(temp, &epc->vpd_open, 'd')) 
	{
		printf("Error reading vpd_max, epc_init()\n");
		ok=0;
	}
	if (ok && scan_value(temp, &epc->vpd_close, 'd'))
	{
		printf("Error reading vpd_min, epc_init()\n");
		ok=0;
	}
	
	/* -------------------------------------------*/
	/* Hidy 2011 - plant wilting mortality parameter */
	if (ok && scan_value(temp, &epc->mort_SNSC_displayed, 'd'))
	{
		printf("Error reading senescence mortality parameter of displayed plant material: epc_init()\n");
		ok=0;
	}

	if (ok && scan_value(temp, &epc->mort_SNSC_storaged, 'd'))
	{
		printf("Error reading storaged senescence mortality parameter of displayed plant material: epc_init()\n");
		ok=0;
	}

	if (ok && scan_value(temp, &epc->mort_SNSC_to_litter, 'd'))
	{
		printf("Error reading turnover rate of wilted standing biomass to litter parameter: epc_init()\n");
		ok=0;
	}

	/* -------------------------------------------*/
	/* Hidy 2013 - Q10 parameter (temperature coefficient for calculating maint.resp.) and GRPERC (growth resp ratio )
		original: BBGC constant - new version: can be set in EPC file*/

	if (ok && scan_value(temp, &epc->q10_value, 'd'))
	{
		printf("Error reading Q10 parameter: epc_init()\n");
		ok=0;
	}

	if (ok && scan_value(temp, &epc->GR_ratio, 'd'))
	{
		printf("Error reading growth resp.ratio: epc_init()\n");
		ok=0;
	}
	/* -------------------------------------------*/
	fclose(temp.ptr);
		
	return (!ok);
}
