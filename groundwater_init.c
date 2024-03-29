/* 
groundwater_init.c
read groundwater depth information if it is available

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Biome-BGCMuSo v7.0.
Copyright 2022, D. Hidy [dori.hidy@gmail.com]
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
#include "bgc_func.h"


int groundwater_init(groundwater_struct* GWS, control_struct* ctrl)
{
	int errorCode=0;
	file GWD_file;	

	int dataread, leap;
	int ndata = 0;

	int p1,p2,p3, maxGWD_num, nmgm;
	double p4;
	char tempvar;

	int* GWyear_array;			
	int* GWmonth_array;						
	int* GWday_array;
	double* GWdepth_array;

	int* mondays=0;
	int* enddays=0;

	nmgm=0;
	maxGWD_num=ctrl->simyears* nDAYS_OF_YEAR;
	GWS->GWD_num = 0;

	/********************************************************************
	**                                                                 **
	** Reading GWD data if available                                   ** 
	**                                                                 **
	********************************************************************/

	if (!errorCode)
	{
		if (ctrl->spinup == 0)   /* normal run */
		{
			strcpy(GWD_file.name, "groundwater_normal.txt");
			if (!file_open(&GWD_file,'j',1)) GWS->GWD_num = 1;
		}
		else                     /* spinup and transient run */        
		{ 	
			strcpy(GWD_file.name, "groundwater_spinup.txt");
			if (!file_open(&GWD_file,'j',1)) GWS->GWD_num = 1;	
		}
	}


	if (!errorCode && GWS->GWD_num > 0) 
	{		
		
		if (!errorCode) 
		{
			enddays = (int*) malloc(nMONTHS_OF_YEAR * sizeof(int));
			if (!enddays)
			{
				printf("ERROR allocating for enddays in bgc.c()\n");
				errorCode=1;
			}
		}

		if (!errorCode) 
		{
			mondays = (int*) malloc(nMONTHS_OF_YEAR * sizeof(int));
			if (!mondays)
			{
				printf("ERROR allocating for enddays in bgc.c()\n");
				errorCode=1;
			}
		}

		/* allocate space for the temporary MGM array */
		GWyear_array    = (int*) malloc(maxGWD_num*sizeof(int));  
		GWmonth_array   = (int*) malloc(maxGWD_num*sizeof(int)); 
		GWday_array     = (int*) malloc(maxGWD_num*sizeof(int)); 
        GWdepth_array   = (double*) malloc(maxGWD_num*sizeof(double)); 

		
		ndata=0;
		while (!errorCode && !(dataread = scan_array (GWD_file, &p1, 'i', 0, 0)))
		{
			dataread = fscanf(GWD_file.ptr, "%c%d%c%d%lf%*[^\n]", &tempvar,&p2,&tempvar,&p3,&p4);
				
			if (p1 >= ctrl->simstartyear && p1 < ctrl->simstartyear + ctrl->simyears)
			{
				GWyear_array[ndata]     = p1;
				GWmonth_array[ndata]    = p2;
				GWday_array[ndata]      = p3;
				GWdepth_array[ndata]    = p4;

				if (!errorCode && leapControl(GWyear_array[ndata], enddays, mondays, &leap))
				{
					printf("ERROR in call to leapControl() from mgm_init.c\n");
					errorCode=1;
				}
				if (leap == 1 && GWmonth_array[ndata] == 12 && GWday_array[ndata] == 31)
				{
					printf("ERROR in groundwater date in groundwater_init.c: data from 31 December in a leap year is found in groundwater file\n");
					printf("Please read the manual and modify the input data\n");
					errorCode=1;
				}

                nmgm += 1;
				ndata += 1;
			}
		}

		GWS->GWD_num = nmgm;
		nmgm = 0;
	
		GWS->GWyear_array      = (int*) malloc(GWS->GWD_num*sizeof(double));  
		GWS->GWmonth_array     = (int*) malloc(GWS->GWD_num*sizeof(double)); 
		GWS->GWday_array       = (int*) malloc(GWS->GWD_num*sizeof(double)); 
		GWS->GWdepth_array  = (double*) malloc(GWS->GWD_num*sizeof(double)); 

		for (nmgm = 0; nmgm < GWS->GWD_num; nmgm++)
		{
			GWS->GWyear_array[nmgm]      = GWyear_array[nmgm];
			GWS->GWmonth_array[nmgm]     = GWmonth_array[nmgm];
			GWS->GWday_array[nmgm]       = GWday_array[nmgm];

			GWS->GWdepth_array[nmgm]  = GWdepth_array[nmgm];
		}

		if (nmgm > maxGWD_num)
		{
			printf("ERROR in groundwater data reading groundwater_init.c\n");
			errorCode=1;
		}

		/* read year and GWD for each simday in each simyear */
		
		free(GWyear_array);	
		free(GWmonth_array);	
		free(GWday_array);	
        free(GWdepth_array);	

		fclose(GWD_file.ptr);
	}	

	

	GWS->mgmdGWD = 0;

	if (!errorCode && GWS->GWD_num > 0) 
	{
		free(enddays);
		free(mondays);
	}
		
	return (errorCode);
}
