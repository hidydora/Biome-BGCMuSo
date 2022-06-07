/* 
ploughing_init.c
read ploughing information for pointbgc simulation

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


int ploughing_init(file init, int max_PLGdays, ploughing_struct* PLG)
{
	int ok = 1;
	char key1[] = "PLOUGHING";
	char keyword[80];

	/********************************************************************
	**                                                                 **
	** Begin reading initialization file block starting with keyword:  **
	** ploughing                                                       ** 
	**                                                                 **
	********************************************************************/
	
	/* scan for the ploughing file keyword, exit if not next */
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


	if (ok && scan_value(init, &PLG->PLG_flag, 'i'))
	{
		printf("Error reading plough calculating flag\n");
		ok=0;
	}

	if (ok && scan_value(init, &PLG->n_PLGdays, 'i'))
	{
		printf("Error reading n_PLGdays\n");
		ok=0;
	}


	if (PLG->n_PLGdays > max_PLGdays)
	{
		printf("Error in n_PLGdays; maximum value = %d\n", max_PLGdays);
		ok=0;
	}

	if (ok)
	{
        int npd=0;
		for (npd=0; npd<max_PLGdays; npd++)
		{
			if (npd < PLG->n_PLGdays)
			{
				if (scan_value(init, &PLG->PLGdays[npd], 'i'))
				{	
					printf("Error reading %d PLGdays\n", npd+1);
				}
			}
			else PLG->PLGdays[npd]=-1;
		}
	}

	
	return (!ok);
}
