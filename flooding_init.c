/* 
flooding_init.c
read flooding depth information if it is available

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


int flooding_init(flooding_struct* FLD, control_struct* ctrl)
{
	int errorCode=0;
	file FLD_file;	

	int dataread, leap;
	int ndata = 0;

	int p1,p2,p3,p4,p5,p6, maxFLD_num, nmgm;
	double p7;
	char tempvar;

	int* FLDstart_year_array;			
	int* FLDstart_month_array;						
	int* FLDstart_day_array;
	int* FLDend_year_array;			
	int* FLDend_month_array;						
	int* FLDend_day_array;
	double* FLDheight_array;

	int* mondays=0;
	int* enddays=0;

	nmgm=0;
	maxFLD_num=ctrl->simyears* nDAYS_OF_YEAR;
	FLD->FLD_num = 0;

	/********************************************************************
	**                                                                 **
	** Reading FLD data if available                                   ** 
	**                                                                 **
	********************************************************************/

	if (!errorCode)
	{
		if (ctrl->spinup == 0)   /* normal run */
		{
			strcpy(FLD_file.name, "flooding_normal.txt");
			if (!file_open(&FLD_file,'j',1)) FLD->FLD_num = 1;
		}
		else                     /* spinup and transient run */        
		{ 	
			strcpy(FLD_file.name, "flooding_spinup.txt");
			if (!file_open(&FLD_file,'j',1)) FLD->FLD_num = 1;	
		}
	}


	if (!errorCode && FLD->FLD_num > 0) 
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
		FLDstart_year_array    = (int*) malloc(maxFLD_num*sizeof(int));  
		FLDstart_month_array   = (int*) malloc(maxFLD_num*sizeof(int)); 
		FLDstart_day_array     = (int*) malloc(maxFLD_num*sizeof(int)); 
		FLDend_year_array    = (int*) malloc(maxFLD_num*sizeof(int));  
		FLDend_month_array   = (int*) malloc(maxFLD_num*sizeof(int)); 
		FLDend_day_array     = (int*) malloc(maxFLD_num*sizeof(int)); 
        FLDheight_array   = (double*) malloc(maxFLD_num*sizeof(double)); 

		
		ndata=0;
		while (!errorCode && !(dataread = scan_array (FLD_file, &p1, 'i', 0, 0)))
		{
			dataread = fscanf(FLD_file.ptr, "%c%d%c%d%c%d%c%d%c%d%lf%*[^\n]", &tempvar,&p2,&tempvar,&p3,&tempvar,&p4,&tempvar,&p5,&tempvar,&p6, &p7);
				
			if (p1 >= ctrl->simstartyear && p1 < ctrl->simstartyear + ctrl->simyears)
			{
				FLDstart_year_array[ndata]     = p1;
				FLDstart_month_array[ndata]    = p2;
				FLDstart_day_array[ndata]      = p3;
				FLDend_year_array[ndata]       = p4;
				FLDend_month_array[ndata]      = p5;
				FLDend_day_array[ndata]        = p6;
				FLDheight_array[ndata]         = p7;

				if (!errorCode && leapControl(FLDstart_year_array[ndata], enddays, mondays, &leap))
				{
					printf("ERROR in call to leapControl() from mgm_init.c\n");
					errorCode=1;
				}
				if (leap == 1 && FLDstart_month_array[ndata] == 12 && FLDstart_day_array[ndata] == 31)
				{
					printf("ERROR in flooding date in flooding_init.c: data from 31 December in a leap year is found in flooding file\n");
					printf("Please read the manual and modify the input data\n");
					errorCode=1;
				}

				if (!errorCode && leapControl(FLDend_year_array[ndata], enddays, mondays, &leap))
				{
					printf("ERROR in call to leapControl() from mgm_init.c\n");
					errorCode=1;
				}
				if (leap == 1 && FLDend_month_array[ndata] == 12 && FLDend_day_array[ndata] == 31)
				{
					printf("ERROR in flooding date in flooding_init.c: data from 31 December in a leap year is found in flooding file\n");
					printf("Please read the manual and modify the input data\n");
					errorCode=1;
				}

                nmgm += 1;
				ndata += 1;
			}
		}

		FLD->FLD_num = nmgm;
		nmgm = 0;
	
		FLD->FLDstart_year_array      = (int*) malloc(FLD->FLD_num*sizeof(double));  
		FLD->FLDstart_month_array     = (int*) malloc(FLD->FLD_num*sizeof(double)); 
		FLD->FLDstart_day_array       = (int*) malloc(FLD->FLD_num*sizeof(double)); 
		FLD->FLDend_year_array        = (int*) malloc(FLD->FLD_num*sizeof(double));  
		FLD->FLDend_month_array       = (int*) malloc(FLD->FLD_num*sizeof(double)); 
		FLD->FLDend_day_array         = (int*) malloc(FLD->FLD_num*sizeof(double)); 
		FLD->FLDheight                = (double*) malloc(FLD->FLD_num*sizeof(double)); 

		for (nmgm = 0; nmgm < FLD->FLD_num; nmgm++)
		{
			FLD->FLDstart_year_array[nmgm]    = FLDstart_year_array[nmgm];
			FLD->FLDstart_month_array[nmgm]   = FLDstart_month_array[nmgm];
			FLD->FLDstart_day_array[nmgm]     = FLDstart_day_array[nmgm];

			FLD->FLDend_year_array[nmgm]      = FLDend_year_array[nmgm];
			FLD->FLDend_month_array[nmgm]     = FLDend_month_array[nmgm];
			FLD->FLDend_day_array[nmgm]       = FLDend_day_array[nmgm];

			FLD->FLDheight[nmgm]              = FLDheight_array[nmgm];
		}

		if (nmgm > maxFLD_num)
		{
			printf("ERROR in flooding data reading flooding_init.c\n");
			errorCode=1;
		}

		/* read year and FLD for each simday in each simyear */
		
		free(FLDstart_year_array);	
		free(FLDstart_month_array);	
		free(FLDstart_day_array);	
		free(FLDend_year_array);	
		free(FLDend_month_array);	
		free(FLDend_day_array);	
        free(FLDheight_array);	

		fclose(FLD_file.ptr);
	}	

	

	FLD->mgmdFLD = 0;

	if (!errorCode && FLD->FLD_num > 0) 
	{
		free(enddays);
		free(mondays);
	}
		
	return (errorCode);
}
