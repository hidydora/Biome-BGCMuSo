/* 
mowing_init.c
read mowing information for pointbgc simulation

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


int mowing_init(file init, int max_MOWdays, mowing_struct* MOW)
{
	int ok = 1;
	char key1[] = "MOWING";
	char keyword[80];

	/********************************************************************
	**                                                                 **
	** Begin reading initialization file block starting with keyword:  **
	** MOWING                                                       ** 
	**                                                                 **
	********************************************************************/
	
	/* scan for the MOWING file keyword, exit if not next */
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
	
	if (ok && scan_value(init, &MOW->MOW_flag, 'i'))
	{
		printf("Error reading mowing calculating flag\n");
		ok=0;
	}

	if (ok && scan_value(init, &MOW->fixday_or_fixLAI_flag, 'i'))
	{
		printf("Error reading fixday_or_fixLAI_flag\n");
		ok=0;
	}


	if (ok && scan_value(init, &MOW->n_MOWdays, 'i'))
	{
		printf("Error reading number of mowing days\n");
		ok=0;
	}
	if (MOW->n_MOWdays > MAX_MOWDAYS)
	{
		printf("Error in n_MOWdays; maximum value = %d\n", MAX_MOWDAYS);
		ok=0;
	}

	if (ok)
	{
        int ngd=0;
		for (ngd=0; ngd<max_MOWdays; ngd++)
		{
			if (ngd<MOW->n_MOWdays)
			{
				if (scan_value(init, &MOW->MOWdays[ngd], 'i'))
				{	
					printf("Error reading %d mowing days\n", ngd+1);
				}
			}
			else MOW->MOWdays[ngd]=-1;
		}
	}
	
	if (ok && scan_value(init, &MOW->fixLAI_befMOW, 'd'))
	{
		printf("Error reading fixed value of LAI beforemowing\n");
		ok=0;
	}


	if (ok && scan_value(init, &MOW->LAI_limit, 'd'))
	{
		printf("Error reading value of the LAI after mowing\n");
		ok=0;
	}

	if (ok && scan_value(init, &MOW->transport_coeff, 'd'))
	{
		printf("Error reading transport_coeff\n");
		ok=0;
	}
		
	return (!ok);
}
