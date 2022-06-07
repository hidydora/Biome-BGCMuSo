/* 
ramp_ndep_init.c
Initialize the ramped N deposition parameters for bgc simulation

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
BBGC MuSo 2.3
Copyright 2000, Peter E. Thornton
*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Modified:
13/07/2000: Added input of Ndep from file. Changes are made by Galina Churkina.
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

int ramp_ndep_init(file init, ramp_ndep_struct* ramp_ndep, int simyears)
{
	int ok = 1;
	int i;
	char key1[] = "RAMP_NDEP";
	char keyword[80];
	char junk[80];
	file temp;
	/********************************************************************
	**                                                                 **
	** Begin reading initialization file block starting with keyword:  **
	** RAMP_NDEP                                                     ** 
	**                                                                 **
	********************************************************************/

	/* scan for the climate change block keyword, exit if not next */
	if (ok && scan_value(init, keyword, 's'))
	{
		printf("Error reading keyword, ramp_ndep_init()\n");
		ok=0;
	}
	if (ok && strcmp(keyword,key1))
	{
		printf("Expecting keyword --> %s in %s\n",key1,init.name);
		ok=0;
	}

	/* begin reading ramp_ndep information */
	if (ok && scan_value(init, &ramp_ndep->doramp, 'i'))
	{
		printf("Error reading ramp Ndep flag: ramp_ndep_init()\n");
		ok=0;
	}
	if (ok && scan_value(init, &ramp_ndep->ind_year, 'i'))
	{
		printf("Error reading year of reference for industrial Ndep: ramp_ndep_init()\n");
		ok=0;
	}
    if (scan_value(init,&ramp_ndep->ind_ndep,'d')) 
	{
		printf("Error reading industrial Ndep value: ramp_ndep_init()\n");
		ok=0;
	}
	
	/* if using variable Ndep file, open it, otherwise
	discard the next line of the ini file */
	if (ok && ramp_ndep->doramp == 1)
	{
    	if (scan_open(init,&temp,'r')) 
		{
			printf("Error opening annual Ndep file\n");
			ok=0;
		}
		
		/* allocate space for the annual Ndep array */
		if (ok)
		{
			ramp_ndep->ndep_array = (double*) malloc(simyears * sizeof(double));
			if (!ramp_ndep->ndep_array)
			{
				printf("Error allocating for annual Ndep array, ramp_Ndep_init()\n");
				ok=0;
			}
		}
		/* read year and Ndep for each simyear */
		for (i=0 ; ok && i<simyears ; i++)
		{
			if (fscanf(temp.ptr,"%*i%lf",&(ramp_ndep->ndep_array[i]))==EOF)
			{
				printf("Error reading annual Ndep array, ctrl_init()\n");
				printf("Note: file must contain a pair of values for each\n");
				printf("simyear: year and Ndep.\n");
				ok=0;
			}
			if (ramp_ndep->ndep_array[i] < 0.0)
			{
				printf("Error in ramp_ndep_init(): Ndep must be positive\n");
				ok=0;
			}
		}
		fclose(temp.ptr);
	}
	else
	{
		if (scan_value(init, junk, 's'))
		{
			printf("Error scanning annual Ndep filename\n");
			ok=0;
		}
	}	
	return (!ok);
}
