/* 
harvesting_init.c
read harvesting information for pointbgc simulation

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


int harvesting_init(file init, int max_HRVdays, harvesting_struct* HRV)
{
	int ok = 1;
	char key1[] = "HARVESTING";
	char keyword[80];

	/********************************************************************
	**                                                                 **
	** Begin reading initialization file block starting with keyword:  **
	** HARVESTING                                                       ** 
	**                                                                 **
	********************************************************************/
	
	/* scan for the HARVESTING file keyword, exit if not next */
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

/*  ----------------------------- */
/*  ----------------------------- */

	if (ok && scan_value(init, &HRV->HRV_flag, 'i'))
	{
		printf("Error reading harvesting calculating flag\n");
		ok=0;
	}

	if (ok && scan_value(init, &HRV->n_HRVdays, 'i'))
	{
		printf("Error reading n_HRVdays\n");
		ok=0;
	}


	if (HRV->n_HRVdays > max_HRVdays)
	{
		printf("Error in n_HRVdays; maximum value = %d\n", max_HRVdays);
		ok=0;
	}

	if (ok)
	{
        int nhd=0;
		for (nhd=0; nhd<max_HRVdays; nhd++)
		{
			if (nhd < HRV->n_HRVdays)
			{
				if (scan_value(init, &HRV->HRVdays[nhd], 'i'))
				{	
					printf("Error reading %d harvesting days\n", nhd+1);
				}
			}
			else HRV->HRVdays[nhd]=-1;
		}
	}
	
	if (ok && scan_value(init, &HRV->LAI_snag, 'd'))
	{
		printf("Error reading LAI_snag flag\n");
		ok=0;
	}

	if (ok && scan_value(init, &HRV->transport_coeff, 'd'))
	{
		printf("Error reading transport_coeff\n");
		ok=0;
	}
	return (!ok);
}
