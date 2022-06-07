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


int ploughing_init(file init, control_struct* ctrl, ploughing_struct* PLG)
{
	char key1[] = "PLOUGHING";
	char keyword[80];
	char bin[100];

	char PLG_filename[100];
	file PLG_file;

	int i;
	int ok = 1;
	int ny=1;
	int n_PLGparam=1;
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
		if (ok && scan_value(init, PLG_filename, 's'))
		{
			printf("Error reading ploughing calculating flag\n");
			ok=0;
		}
		else
		{
			
			ok=1;
			printf("ploughing information from file\n");
			PLG->PLG_flag = 2;
			strcpy(PLG_file.name, PLG_filename);
		}
	}

	/* yeary varied garzing parameters (PLG_flag=2); else: constant garzing parameters (PLG_flag=1) */
	if (PLG->PLG_flag == 2)
	{
		ny = ctrl->simyears; 
	
		/* open the main init file for ascii read and check for errors */
		if (file_open(&PLG_file,'i'))
		{
			printf("Error opening PLG_file, ploughing_int.c\n");
			exit(1);
		}

		/* step forward in init file */
		for (i=0; i < n_PLGparam; i++) scan_value(init, bin, 'd');

	}
	else PLG_file=init;
	

	if (ok && read_mgmarray(ny, PLG->PLG_flag, PLG_file, &(PLG->PLGdays_array)))
	{
		printf("Error reading first day of ploughing\n");
		ok=0;
	}


	if (PLG->PLG_flag == 2)
	{
		fclose (PLG_file.ptr);
	}
	
	return (!ok);
}
