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


int grazing_init(file init, control_struct* ctrl, grazing_struct* GRZ)
{

	char key1[] = "GRAZING";
	char keyword[80];
	char bin[100];

	char GRZ_filename[100];
	file GRZ_file;

	int i;
	int ok = 1;
	int ny=1;
	int n_GRZparam=9;


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
		if (ok && scan_value(init, GRZ_filename, 's'))
		{
			printf("Error reading grazing calculating flag\n");
			ok=0;
		}
		else
		{
			
			ok=1;
			printf("grazing information from file\n");
			GRZ->GRZ_flag = 2;
			strcpy(GRZ_file.name, GRZ_filename);
		}
	}

	/* yeary varied garzing parameters (GRZ_flag=2); else: constant garzing parameters (GRZ_flag=1) */
	if (GRZ->GRZ_flag == 2)
	{
		ny = ctrl->simyears; 
	
		/* open the main init file for ascii read and check for errors */
		if (file_open(&GRZ_file,'i'))
		{
			printf("Error opening GRZ_file, grazing_int.c\n");
			exit(1);
		}

		/* step forward in init file */
		for (i=0; i < n_GRZparam; i++) scan_value(init, bin, 'd');

	}
	else GRZ_file=init;
	

	if (ok && read_mgmarray(ny, GRZ->GRZ_flag, GRZ_file, &(GRZ->GRZ_start_array)))
	{
		printf("Error reading first day of grazing\n");
		ok=0;
	}

	if (ok && read_mgmarray(ny, GRZ->GRZ_flag, GRZ_file, &(GRZ->GRZ_end_array)))
	{
		printf("Error reading first day of grazing\n");
		ok=0;
	}

	if (ok && read_mgmarray(ny, GRZ->GRZ_flag, GRZ_file, &(GRZ->DMintake_array)))
	{
		printf("Error reading last day of grazing\n");
		ok=0;
	}

	if (ok && read_mgmarray(ny, GRZ->GRZ_flag, GRZ_file, &(GRZ->stocking_rate_array)))
	{
		printf("Error reading animal stocking rate\n");
		ok=0;
	}

	if (ok && read_mgmarray(ny, GRZ->GRZ_flag, GRZ_file, &(GRZ->prop_DMintake2excr_array)))
	{
		printf("Error reading prop. of the dry matter intake formed excrement\n");
		ok=0;
	}

	if (ok && read_mgmarray(ny, GRZ->GRZ_flag, GRZ_file, &(GRZ->prop_excr2litter_array)))
	{
		printf("Error reading prop. of excrement return to litter\n");
		ok=0;
	}

	if (ok && read_mgmarray(ny, GRZ->GRZ_flag, GRZ_file, &(GRZ->DM_Ccontent_array)))
	{
		printf("Error reading carbon content of dry matter\n");
		ok=0;
	}
		
	if (ok && read_mgmarray(ny, GRZ->GRZ_flag, GRZ_file, &(GRZ->EXCR_Ncontent_array)))
	{
		printf("Error reading nitrogen content of the fertilizer\n");
		ok=0;
	}
			
	if (ok && read_mgmarray(ny, GRZ->GRZ_flag, GRZ_file, &(GRZ->EXCR_Ccontent_array)))
	{
		printf("Error reading carbon content of the fertilizer\n");
		ok=0;
	}

	if (GRZ->GRZ_flag == 2)
	{
		fclose (GRZ_file.ptr);
	}
		
	return (!ok);	

}
