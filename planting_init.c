/* 
planting_init.c
read planting information for pointbgc simulation

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


int planting_init(file init, int max_PLTdays, planting_struct* PLT)
{
	int ok = 1;
	char key0[]  = "-------------------";
	char key00[] = "MANAGEMENT_SECTION";
	char key1[] = "PLANTING";
	char keyword[80];

	/********************************************************************
	**                                                                 **
	** Begin reading initialization file block starting with keyword:  **
	** PLANTING                                                       ** 
	**                                                                 **
	********************************************************************/
	
	/* keyword control - Hidy 2012. */
	if (ok && scan_value(init, keyword, 's'))
	{
		printf("Error reading keyword for management section\n");
		ok=0;
	}
	if (ok && strcmp(keyword, key0))
	{
		printf("Expecting keyword --> %s in file %s\n",key0,init.name);
		ok=0;
	}

	if (ok && scan_value(init, keyword, 's'))
	{
		printf("Error reading keyword for management section\n");
		ok=0;
	}
	if (ok && strcmp(keyword, key00))
	{
		printf("Expecting keyword --> %s in file %s\n",key00,init.name);
		ok=0;
	}

	if (ok && scan_value(init, keyword, 's'))
	{
		printf("Error reading keyword for management section\n");
		ok=0;
	}
	if (ok && strcmp(keyword, key0))
	{
		printf("Expecting keyword --> %s in file %s\n",key0,init.name);
		ok=0;
	}

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
	
	/* planting parameters */
	if (ok && scan_value(init, &PLT->PLT_flag, 'i'))
	{
		printf("Error reading planting calculating flag\n");
		ok=0;
	}


	if (ok && scan_value(init, &PLT->n_PLTdays, 'i'))
	{
		printf("Error reading number of planting days\n");
		ok=0;
	}
	if (PLT->n_PLTdays > max_PLTdays)
	{
		printf("Error in n_PLTdays; maximum value = %d\n", max_PLTdays);
		ok=0;
	}

	if (ok)
	{
        int nsd=0;
		for (nsd=0; nsd<max_PLTdays; nsd++)
		{
			if (nsd<PLT->n_PLTdays)
			{
				if (scan_value(init, &PLT->PLTdays[nsd], 'i'))
				{	
					printf("Error reading %d planting days\n", nsd+1);
				}
			}
			else PLT->PLTdays[nsd]=-1;
		}
	}
	
	if (ok && scan_value(init, &PLT->seed_quantity, 'd'))
	{
		printf("Error reading quantity of seed\n");
		ok=0;
	}

	if (ok && scan_value(init, &PLT->seed_carbon, 'd'))
	{
		printf("Error reading carbon content of seed\n");
		ok=0;
	}

	if (ok && scan_value(init, &PLT->prop_leaf_product, 'd'))
	{
		printf("Error reading ratio_leaf_construct\n");
		ok=0;
	}
		
	return (!ok);
}
