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


int planting_init(file init, control_struct* ctrl, planting_struct* PLT)
{
	
	char key0[]  = "-------------------";
	char key00[] = "MANAGEMENT_SECTION";
	char key1[] = "PLANTING";
	char keyword[80];
	char bin[100];

	char PLT_filename[100];
	file PLT_file;

	int i;
	int ok = 1;
	int ny=1;
	int n_PLTparam=4;

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
	
	if (ok && scan_value(init, &PLT->PLT_flag, 'i'))
	{
		if (ok && scan_value(init, PLT_filename, 's'))
		{
			printf("Error reading planting calculating flag\n");
			ok=0;
		}
		else
		{
			
			ok=1;
			printf("planting information from file\n");
			PLT->PLT_flag = 2;
			strcpy(PLT_file.name, PLT_filename);
		}
	}
	else PLT_file=init;

	/* yeary varied garzing parameters (PLT_flag=2); else: constant garzing parameters (PLT_flag=1) */
	if (PLT->PLT_flag == 2)
	{
		ny = ctrl->simyears; 
	
		/* open the main init file for ascii read and check for errors */
		if (file_open(&PLT_file,'i'))
		{
			printf("Error opening PLT_file, planting_int.c\n");
			exit(1);
		}

		/* step forward in init file */
		for (i=0; i < n_PLTparam; i++) scan_value(init, bin, 'd');

	}
	else PLT_file=init;

	if (ok && read_mgmarray(ny, PLT->PLT_flag, PLT_file, &(PLT->PLTdays_array)))
	{
		printf("Error reading PLTdays_array\n");
		ok=0;
	}

	if (ok && read_mgmarray(ny, PLT->PLT_flag, PLT_file, &(PLT->seed_quantity_array)))
	{
		printf("Error reading seed_quantity_array\n");
		ok=0;
	}

	if (ok && read_mgmarray(ny, PLT->PLT_flag, PLT_file, &(PLT->seed_carbon_array)))
	{
		printf("Error reading seed_carbon_array\n");
		ok=0;
	}

	if (ok && read_mgmarray(ny, PLT->PLT_flag, PLT_file, &(PLT->prop_leaf_product_array)))
	{
		printf("Error reading prop_leaf_product_array\n");
		ok=0;
	}


	if (PLT->PLT_flag == 2)
	{
		fclose (PLT_file.ptr);
	}
	
		
	return (!ok);
}
