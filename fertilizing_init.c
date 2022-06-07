/* 
fertilizing_init.c
read fertilizing information for pointbgc simulation

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Biome-BGCMuSo v5.0
Copyright 2018, D. Hidy [dori.hidy@gmail.com]
Hungarian Academy of Sciences, Hungary
See the website of Biome-BGCMuSo at http://nimbus.elte.hu/bbgc/ for documentation, model executable and example input files.
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
#include "bgc_constants.h"


int fertilizing_init(file init, const control_struct* ctrl, fertilizing_struct* FRZ)
{
	char key1[] = "FERTILIZING";
	char keyword[80];
	char bin[100];

	char FRZ_filename[100];
	file FRZ_file;

	int i;
	int ok = 1;
	int ny=1;


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
		if (ok && scan_value(init, FRZ_filename, 's'))
		{
			printf("Error reading fertilizing calculating file\n");
			ok=0;
		}
		else
		{
			
			ok=1;
			printf("But it is not a problem (it is only due to the reading of fertilizing file)\n");
			if (ctrl->onscreen) printf("INFORMATION: fertilizing information from file\n");
			FRZ->FRZ_flag = 2;
			strcpy(FRZ_file.name, FRZ_filename);
		}
	}

	/* yeary varied garzing parameters (FRZ_flag=2); else: constant garzing parameters (FRZ_flag=1) */
	if (FRZ->FRZ_flag == 2)
	{
		ny = ctrl->simyears; 
	
		/* open the main init file for ascii read and check for errors */
		if (file_open(&FRZ_file,'i'))
		{
			printf("Error opening FRZ_file, fertilizing_int.c\n");
			exit(1);
		}

		/* step forward in init file */
		for (i=0; i < n_FRZparam; i++) scan_value(init, bin, 'd');

	}
	else FRZ_file=init;
	

	if (ok && read_mgmarray(ny, FRZ->FRZ_flag, FRZ_file, &(FRZ->FRZdays_array)))
	{
		printf("Error reading FRZdays\n");
		ok=0;
	}

	if (ok && read_mgmarray(ny, FRZ->FRZ_flag, FRZ_file, &(FRZ->FRZdepth_array)))
	{
		printf("Error reading FRZdepth\n");
		ok=0;
	}

	if (ok && read_mgmarray(ny, FRZ->FRZ_flag, FRZ_file, &(FRZ->fertilizer_array)))
	{
		printf("Error reading fertilizer\n");
		ok=0;
	}


	if (ok && read_mgmarray(ny, FRZ->FRZ_flag, FRZ_file, &(FRZ->NO3content_array)))
	{
		printf("Error reading nitrate content of fertilizer\n");
		ok=0;
	}

	if (ok && read_mgmarray(ny, FRZ->FRZ_flag, FRZ_file, &(FRZ->NH4content_array)))
	{
		printf("Error reading ammonium content of fertilizer\n");
		ok=0;
	}

	if (ok && read_mgmarray(ny, FRZ->FRZ_flag, FRZ_file, &(FRZ->Ccontent_array)))
	{
		printf("Error reading carbon_content\n");
		ok=0;
	}
		
	if (ok && read_mgmarray(ny, FRZ->FRZ_flag, FRZ_file, &(FRZ->litr_flab_array)))
	{
		printf("Error reading litr_flab\n");
		ok=0;
	}
			
	if (ok && read_mgmarray(ny, FRZ->FRZ_flag, FRZ_file, &(FRZ->litr_fucel_array)))
	{
		printf("Error reading litr_fucel\n");
		ok=0;
	}


	if (ok && read_mgmarray(ny, FRZ->FRZ_flag, FRZ_file, &(FRZ->litr_fscel_array)))
	{
		printf("Error reading litr_fscel\n");
		ok=0;
	}


	if (ok && read_mgmarray(ny, FRZ->FRZ_flag, FRZ_file, &(FRZ->litr_flig_array)))
	{
		printf("Error reading litr_flig\n");
		ok=0;
	}

	if (ok && read_mgmarray(ny, FRZ->FRZ_flag, FRZ_file, &(FRZ->DC_NO3_array)))
	{
		printf("Error reading dissolv.coeff of NO3\n");
		ok=0;
	}

	if (ok && read_mgmarray(ny, FRZ->FRZ_flag, FRZ_file, &(FRZ->DC_NH4_array)))
	{
		printf("Error reading dissolv.coeff of NH4\n");
		ok=0;
	}

	if (ok && read_mgmarray(ny, FRZ->FRZ_flag, FRZ_file, &(FRZ->DC_C_array)))
	{
		printf("Error reading dissolv.coeff of carbon\n");
		ok=0;
	}


	if (ok && read_mgmarray(ny, FRZ->FRZ_flag, FRZ_file, &(FRZ->EFfert_N2O)))
	{
		printf("Error reading fertilization emission factor of N2O\n");
		ok=0;
	}


	if (FRZ->FRZ_flag == 2)
	{
		fclose (FRZ_file.ptr);
	}

	/* local variables */
	FRZ->FRZdepth_act   = 0;
	FRZ->DC_NO3_act     = 0;    
	FRZ->DC_NH4_act     = 0;   
	FRZ->DC_C_act       = 0;   
	FRZ->CARBON_act     = 0;
	FRZ->NO3_act        = 0;	
	FRZ->NH4_act        = 0;
	FRZ->flab_act       = 0;
	FRZ->fucel_act      = 0;
	FRZ->fscel_act		= 0;
	FRZ->flig_act		= 0;
	FRZ->EFf_N2O_act	= 0;
	FRZ->FRZlayer       = 0;
	FRZ->mgmd           = -1;


	return (!ok);
}
