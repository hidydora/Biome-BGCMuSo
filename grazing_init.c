/* 
grazing_init.c
read grazinz information for pointbgc simulation

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


int grazing_init(file init,  grazing_struct* GRZ)
{
	int ok = 1;
	char key1[] = "GRAZING";
	char keyword[80];

	/********************************************************************
	**                                                                 **
	** Begin reading initialization file block starting with keyword:  **
	** GRAZING                                                       ** 
	**                                                                 **
	********************************************************************/
	
	/* scan for the GRAZING file keyword, exit if not next */
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
	
	if (ok && scan_value(init, &GRZ->GRZ_flag, 'i'))
	{
		printf("Error reading grazing calculating flag\n");
		ok=0;
	}

	if (ok && scan_value(init, &GRZ->first_day_of_GRZ, 'i'))
	{
		printf("Error reading first_day_of_grazing\n");
		ok=0;
	}


	if (ok && scan_value(init, &GRZ->last_day_of_GRZ, 'i'))
	{
		printf("Error reading last_day_of_grazing\n");
		ok=0;
	}


	if (ok && scan_value(init, &GRZ->drymatter_intake, 'd'))
	{
		printf("Error reading value of drymatter_intake\n");
		ok=0;
	}

	if (ok && scan_value(init, &GRZ->stocking_rate, 'd'))
	{
		printf("Error reading value of stocking_rate\n");
		ok=0;
	}


	if (ok && scan_value(init, &GRZ->prop_DMintake_formed_excrement, 'd'))
	{
		printf("Error reading value of prop_DMintake_formed_excrement\n");
		ok=0;
	}

	if (ok && scan_value(init, &GRZ->prop_excrement_return2litter, 'd'))
	{
		printf("Error reading value of prop_excrement_return2litter\n");
		ok=0;
	}

	
	if (ok && scan_value(init, &GRZ->C_content_of_drymatter, 'd'))
	{
		printf("Error reading value of C_content_of_drymatter\n");
		ok=0;
	}
	

	if (ok && scan_value(init, &GRZ->N_content_of_excrement, 'd'))
	{
		printf("Error reading value of N_content_of_excrement\n");
		ok=0;
	}
	

	if (ok && scan_value(init, &GRZ->C_content_of_excrement, 'd'))
	{
		printf("Error reading value of C_content_of_excrement\n");
		ok=0;
	}

		
	return (!ok);
}
