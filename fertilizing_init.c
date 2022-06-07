/* 
fertilizing_init.c
read fertilizing information for pointbgc simulation

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


int fertilizing_init(file init, int max_FRZdays, fertilizing_struct* FRZ)
{
	int ok = 1;
	char key1[] = "FERTILIZING";
	char keyword[80];

	/********************************************************************
	**                                                                 **
	** Begin reading initialization file block starting with keyword:  **
	** FERTILIZING                                                       ** 
	**                                                                 **
	********************************************************************/
	
	/* scan for the FERTILIZING file keyword, exit if not next */
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

	if (ok && scan_value(init, &FRZ->FRZ_flag, 'i'))
	{
		printf("Error reading harvest_or_grazing flag\n");
		ok=0;
	}


	if (ok && scan_value(init, &FRZ->n_FRZdays, 'i'))
	{
		printf("Error reading number of fertilizing days\n");
		ok=0;
	}
	if (FRZ->n_FRZdays > max_FRZdays)
	{
		printf("Error in n_FRZdays; maximum value = %d\n", max_FRZdays);
		ok=0;
	}

	if (ok)
	{
        int nfd=0;
		for (nfd=0; nfd<max_FRZdays; nfd++)
		{
			if (nfd<FRZ->n_FRZdays)
			{
				if (scan_value(init, &FRZ->FRZdays[nfd], 'i'))
				{	
					printf("Error reading %d fertilizing days\n", nfd+1);
				}
			}
			else FRZ->FRZdays[nfd]=-1;
		}
	}
	
		if (ok)
	{
        int nfd=0;
		for (nfd=0; nfd<max_FRZdays; nfd++)
		{
			if (nfd<FRZ->n_FRZdays)
			{
				if (scan_value(init, &FRZ->fertilizer[nfd], 'd'))
				{	
					printf("Error reading %d the amuont of fertilizer\n", nfd+1);
				}
			}
			else FRZ->fertilizer[nfd]=-1;
		}
	}

	if (ok && scan_value(init, &FRZ->nitrate_content, 'd'))
	{
		printf("Error reading nitrate_content\n");
		ok=0;
	}

	if (ok && scan_value(init, &FRZ->ammonium_content, 'd'))
	{
		printf("Error reading ammonium_content\n");
		ok=0;
	}

	if (ok && scan_value(init, &FRZ->carbon_content, 'd'))
	{
		printf("Error reading carbon_content\n");
		ok=0;
	}

	if (ok && scan_value(init, &FRZ->litr_flab, 'd'))
	{
		printf("Error reading labile fraction\n");
		ok=0;
	}

	if (ok && scan_value(init, &FRZ->litr_fucel, 'd'))
	{
		printf("Error reading unshielded cellulose fraction\n");
		ok=0;
	}

	if (ok && scan_value(init, &FRZ->litr_fscel, 'd'))
	{
		printf("Error reading shielded cellulose fraction\n");
		ok=0;
	}

	if (ok && scan_value(init, &FRZ->litr_flig, 'd'))
	{
		printf("Error reading lignin fraction\n");
		ok=0;
	}
	
	if (ok && scan_value(init, &FRZ->dissolv_coeff, 'd'))
	{
		printf("Error reading dissolving coefficient\n");
		ok=0;
	}
	
	if (ok && scan_value(init, &FRZ->utilization_coeff, 'd'))
	{
		printf("Error reading utilization_coeff\n");
		ok=0;
	}
	return (!ok);
}
