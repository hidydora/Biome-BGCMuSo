/* 
thinning_init.c
read thinning information for pointbgc simulation

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


int thinning_init(file init, int max_THNdays, thinning_struct* THN)
{
	int ok = 1;
	char key1[] = "THINNING";
	char keyword[80];

	/********************************************************************
	**                                                                 **
	** Begin reading initialization file block starting with keyword:  **
	** thinning                                                       ** 
	**                                                                 **
	********************************************************************/
	
	/* scan for the thinning file keyword, exit if not next */
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
	
	if (ok && scan_value(init, &THN->THN_flag, 'i'))
	{
		printf("Error reading thinning calculating flag\n");
		ok=0;
	}


	if (ok && scan_value(init, &THN->n_THNdays, 'i'))
	{
		printf("Error reading number of thinning days\n");
		ok=0;
	}
	if (THN->n_THNdays > max_THNdays)
	{
		printf("Error in n_THNdays; maximum value = %d\n", max_THNdays);
		ok=0;
	}

	if (ok)
	{
        int ngd=0;
		for (ngd=0; ngd<max_THNdays; ngd++)
		{
			if (ngd<THN->n_THNdays)
			{
				if (scan_value(init, &THN->THNdays[ngd], 'i'))
				{	
					printf("Error reading %d thinning days\n", ngd+1);
				}
			}
			else THN->THNdays[ngd]=-1;
		}
	}
	


	if (ok && scan_value(init, &THN->thinning_rate, 'd'))
	{
		printf("Error reading thinning rate\n");
		ok=0;
	}


	if (ok && scan_value(init, &THN->transp_stem_rate, 'd'))
	{
		printf("Error reading rate of the transported stem\n");
		ok=0;
	}

	if (ok && scan_value(init, &THN->transp_leaf_rate, 'd'))
	{
		printf("Error reading rate of the transported leaf\n");
		ok=0;
	}
	
	return (!ok);
}
