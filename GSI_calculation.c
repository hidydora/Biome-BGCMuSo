/* 
GSI_calculation.c
Calculation of pheonological state of vegetation (onset and offset day) from Tmin, VPD, daylength and cumulative Tavg 
based on literure (Jolly et al, 2005) and own method. The goal is to replace prephenology.c subroutines becuse the calulation
of the model-defined onset and offset day does not work correctly

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Biome-BGC version 4.1.1
Copyright 2009, Hidy
*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-**/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <malloc.h>
#include "ini.h"
#include "bgc_struct.h"
#include "pointbgc_struct.h"
#include "bgc_func.h"     
#include "bgc_constants.h"
#include "misc_func.h"


int GSI_calculation(const metarr_struct* metarr, const control_struct* ctrl, GSI_struct* GSI, phenarray_struct* phenarr)

{
	int ok=1;
	int ny, yday, back;

	int firstdayLP = 180;		/* theoretically first day of litterfall */

	/* flags and counters */
	int onday_flag = 0;
	int offday_flag = 0;
	int onday = 0;
	int offday = 0;
	int nyears = ctrl->metyears;
	int n_yday = NDAY_OF_YEAR;
	int ndays = NDAY_OF_YEAR * nyears;

	
	/*  enviromental conditions taken account to calculate onset and offset days */
	double tmax_act, tmin_act, tavg_act, vpd_act, photoperiod_act, heatsum_act;	

	
	/* threshold limits for each variable, between assuming that phenological activity varied linearly from inactive to unconstrained */

	
	double basic_temperature = GSI->basic_temperature;
	double heatsum_limit1 = GSI->heatsum_limit1;
	double heatsum_limit2 = GSI->heatsum_limit2;
	double tmin_limit1 = GSI->tmin_limit1;
	double tmin_limit2 = GSI->tmin_limit2;
	double vpd_limit1 = GSI->vpd_limit1;
	double vpd_limit2 = GSI->vpd_limit2;
	double photoperiod_limit1 = GSI->photoperiod_limit1;
	double photoperiod_limit2 = GSI->photoperiod_limit2;
	int n_moving_avg = GSI->n_moving_avg-1;	/* moving averages are calculated from indicatiors 
											to avoid the effects of single extreme events; -1: number to index */
	double GSI_limit_SGS = GSI->GSI_limit_SGS;	/* when GSI index fisrt time greater that limit -> start of the growing season */
	double GSI_limit_EGS = GSI->GSI_limit_EGS;	/* when GSI index fisrt time less that limit -> end of the growing season */


	/* indexes for the different variables and total index (multiplication of  partial indexes)*/
	double tmin_index, vpd_index, photoperiod_index, heatsum_index, GSI_index;
	
	/* to calculate moving average from total index values */
	double GSI_index_SUM = 0;
	double GSI_index_avg = 0;
	double GSI_index_total = 0;
	
	int *onday_arr, *offday_arr;
	
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////allocate memory for arrays containing ondays and offdays /////////////////////////////////////
	if (ok && !(onday_arr = (int*) malloc((nyears+1) * sizeof(int))))
	{
		printf("Error allocating for onday_arr, prephenology()\n");
		ok=0;
	}
	if (ok && !(offday_arr = (int*) malloc((nyears+1) * sizeof(int))))
	{
		printf("Error allocating for offday_arr, prephenology()\n");
		ok=0;
	}
		
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
		if (ctrl->onscreen)
		{
			fprintf(GSI->GSI_file.ptr, "year yday tmin tday VPD photoperiod heatsum tmin_index vpd_index photoperiod_index heatsum_index GSI_index_avg GSI_index_total\n");
		}
		////////////////////////////////////////////////////////////////////

		for (ny=0 ; ny<nyears ; ny++)
		{
					
			/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
			/* !!!!!!!!!!!!!!!!!!!!!!!!! calculating onday !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
			yday = 0;
			onday_flag = 0;
			while ((yday < n_yday) && (onday_flag==0))
			{

				tmin_index = 0;
				vpd_index = 0;
				photoperiod_index = 0;
				heatsum_index=0;
				GSI_index_avg = 0;
				GSI_index_total = 0;
				
				/* writing out the enviromental parameters and GSI indexes */
				if (ctrl->onscreen && yday!=n_moving_avg)
				{
					/* search actual values of variables */
					tmax_act = metarr->tmax[ny*n_yday+yday];
					tmin_act = metarr->tmin[ny*n_yday+yday];
					tavg_act = (tmax_act+tmin_act)/2.;
					vpd_act = metarr->vpd[ny*n_yday+yday];
					photoperiod_act = metarr->dayl[ny*n_yday+yday];
					heatsum_act = 0;
				
					fprintf(GSI->GSI_file.ptr, "%i %i %f %f %f %f %f %f %f %f %f %f %f\n", ctrl->simstartyear+ny, yday, 
										tmin_act, tavg_act, vpd_act, photoperiod_act, heatsum_act, 
										tmin_index, vpd_index, photoperiod_index, heatsum_index, GSI_index_avg, GSI_index_total);
				}
				
				while ((yday>=n_moving_avg) && (yday<n_yday) && (onday_flag==0))
				{
					GSI_index_SUM = 0;
					heatsum_act = 0;
					for (back=0; back<=n_moving_avg; back++)
					{
						/* search actual values of variables */
						tmax_act = metarr->tmax[ny*n_yday+yday-(n_moving_avg-back)];
						tmin_act = metarr->tmin[ny*n_yday+yday-(n_moving_avg-back)];
						tavg_act = (tmax_act+tmin_act)/2.;
						vpd_act = metarr->vpd[ny*n_yday+yday-(n_moving_avg-back)];

			
						photoperiod_act = metarr->dayl[ny*n_yday+yday-(n_moving_avg-back)];

						/* calculation of heatsum regarding to the basic temperature and n_moving_avg long period */
						if (tavg_act > basic_temperature) 
						{
							heatsum_act += (tavg_act-basic_temperature);
						}

						/* calculation of indexes regarding to the different variables */
						/* !!!!!!!!!!!!!!!!!!!!!!!!!!  1: tmin !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
						if (tmin_act < tmin_limit1)
						{
							tmin_index=0;
						}
						else
						{
							if (tmin_act < tmin_limit2)
							{
								tmin_index = (tmin_act-tmin_limit1)/(tmin_limit2-tmin_limit1);
							}
							else
							{
								tmin_index = 1;
							}

						}
						/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!  2: vpd !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
						if (vpd_act > vpd_limit1)
						{
							vpd_index=0;
						}
						else
						{
							if (vpd_act > vpd_limit2)
							{
								vpd_index = (vpd_act-vpd_limit1)/(vpd_limit2-vpd_limit1);
							}
							else
							{
								vpd_index = 1;
							}

						}
						/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!  3: photoperiod !!!!!!!!!!!!!!!!!!!!!!!!!!! */
						if (photoperiod_act < photoperiod_limit1)
						{
							photoperiod_index=0;
						}
						else
						{
							if (photoperiod_act < photoperiod_limit2)
							{
								photoperiod_index = (photoperiod_act-photoperiod_limit1)/(photoperiod_limit2-photoperiod_limit1);
							}
							else
							{
								photoperiod_index = 1;
							}

						}
							
						
						GSI_index = tmin_index * vpd_index * photoperiod_index;
						GSI_index_SUM += GSI_index;

					} /* endfor - calculating indexes for the n_moving_average long period  */

					/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!  4: heatsum !!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
					if (heatsum_act < heatsum_limit1)
					{
						heatsum_index=0;
					}
					else
					{
						if (heatsum_act < heatsum_limit2)
						{
							heatsum_index = (heatsum_act-heatsum_limit1)/(heatsum_limit2-heatsum_limit1);
						}
						else
						{
							heatsum_index = 1;
						}

					}

					GSI_index_avg = GSI_index_SUM / (n_moving_avg+1);
					GSI_index_total = GSI_index_avg * heatsum_index;

					/* writing out the enviromental parameters and GSI indexes */
					if (ctrl->onscreen)
					{
						fprintf(GSI->GSI_file.ptr, "%i %i %f %f %f %f %f %f %f %f %f %f %f\n", ctrl->simstartyear+ny, yday, 
											tmin_act, tavg_act, vpd_act, photoperiod_act, heatsum_act, 
											tmin_index, vpd_index, photoperiod_index, heatsum_index, GSI_index_avg, GSI_index_total);
					}
					
					if (GSI_index_total > GSI_limit_SGS) 
					{
						onday_flag = 1;
						onday = yday;
						onday_arr[ny] = onday;
						if (ctrl->onscreen) printf("year:%i onday:%i\n", ctrl->simstartyear+ny, onday);
					}
					else
					{
						yday = yday+1;
					}
				
					
				} /* endwhile - determine the onday; calculating n_moving_average long period */

				yday = yday+1;
				
			} /* endwhile - determine the onday */

		
			/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
			/* !!!!!!!!!!!!!!!!!!!!!!!!! calculating offday !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
			offday_flag = 0;
			while ((yday > onday)  && (yday < n_yday) && (offday_flag==0))
			{
				vpd_index = 1;
				photoperiod_index = 1;
				GSI_index_avg = 1;
				GSI_index_total = 1;
				
			
				
				while (yday < n_yday)
				{
					GSI_index_SUM = 0;
					heatsum_act = 0;
					for (back=0; back<=n_moving_avg; back++)
					{
						/* search actual values of variables */
						tmax_act = metarr->tmax[ny*n_yday+yday-(n_moving_avg-back)];
						tmin_act = metarr->tmin[ny*n_yday+yday-(n_moving_avg-back)];
						tavg_act = (tmax_act+tmin_act)/2.;
						vpd_act = metarr->vpd[ny*n_yday+yday-(n_moving_avg-back)];
						photoperiod_act = metarr->dayl[ny*n_yday+yday-(n_moving_avg-back)];

						/* calculation of heatsum regarding to the basic temperature and n_moving_avg long period */
						if (tavg_act > basic_temperature) 
						{
							heatsum_act += (tavg_act-basic_temperature);
						}

						/* calculation of indexes regarding to the different variables */
						/* !!!!!!!!!!!!!!!!!!!!!!!!!!  1: tmin !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
						if (tmin_act < tmin_limit1)
						{
							tmin_index=0;
						}
						else
						{
							if (tmin_act < tmin_limit2)
							{
								tmin_index = (tmin_act-tmin_limit1)/(tmin_limit2-tmin_limit1);
							}
							else
							{
								tmin_index = 1;
							}

						}
						/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!  2: vpd !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
						if (vpd_act	> vpd_limit1)
						{
							vpd_index=0;
						}
						else
						{
							if (vpd_act > vpd_limit1)
							{
								vpd_index = (vpd_act-vpd_limit1)/(vpd_limit2-vpd_limit1);
							}
							else
							{
								vpd_index = 1;
							}

						}
						/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!  3: photoperiod !!!!!!!!!!!!!!!!!!!!!!!!!!! */
						if (photoperiod_act < photoperiod_limit1)
						{
							photoperiod_index=0;
						}
						else
						{
							if (photoperiod_act < photoperiod_limit2)
							{
								photoperiod_index = (photoperiod_act-photoperiod_limit1)/(photoperiod_limit2-photoperiod_limit1);
							}
							else
							{
								photoperiod_index = 1;
							}

						}
							
						
						GSI_index = tmin_index * vpd_index * photoperiod_index; // * heatsum_index;
						GSI_index_SUM += GSI_index;

					} /* endfor - calculating indexes for the n_moving_average long period  */

					/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!  4: heatsum !!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
					if (heatsum_act < heatsum_limit1)
					{
						heatsum_index=0;
					}
					else
					{
						if (heatsum_act < heatsum_limit2)
						{
							heatsum_index = (heatsum_act-heatsum_limit1)/(heatsum_limit2-heatsum_limit1);
						}
						else
						{
							heatsum_index = 1;
						}

					}

					GSI_index_avg = GSI_index_SUM / (n_moving_avg+1);
					GSI_index_total = GSI_index_avg * heatsum_index;
					
					/* writing out the enviromental parameters and GSI indexes */
					if (ctrl->onscreen)
					{
						fprintf(GSI->GSI_file.ptr, "%i %i %f %f %f %f %f %f %f %f %f %f %f\n", ctrl->simstartyear+ny, yday, 
											tmin_act, tavg_act, vpd_act, photoperiod_act, heatsum_act, 
											tmin_index, vpd_index, photoperiod_index, heatsum_index, GSI_index_avg, GSI_index_total);
					}
					
					if (GSI_index_total <= GSI_limit_EGS  && yday > firstdayLP && offday_flag == 0) 
					{
						offday_flag = 1;
						offday = yday;
						offday_arr[ny] = offday;

						if (yday < firstdayLP)
						{
							printf("Error in GSI calculation (too early litterfall)\n"); 
						}
					
						if (ctrl->onscreen) printf("year:%i offday:%i\n", ctrl->simstartyear+ny, offday);
					}

					/* if no external limitation for vegettion period, the offday is the last yday (364) */
					if (offday_flag == 0 && yday == NDAY_OF_YEAR-1)
					{
						offday_flag = 1;
						offday = yday;
						offday_arr[ny] = offday;
					
						if (ctrl->onscreen) printf("no offest in year:%i\n", ctrl->simstartyear+ny);
					}
					yday = yday+1;
				
				
				} /* endwhile - determine the offday; calculating n_moving_average long period */

				yday = yday+1;
				
			} /* endwhile - determine the offday */
		

		} /* endfor - simyears */

		/* writing out the date of onday and offday for every simulation year */
		if (ctrl->onscreen)
		{
			for (ny=0 ; ny<nyears ; ny++)
			{
				fprintf(GSI->GSI_file.ptr, "%i\n", ctrl->simstartyear+ny);
				fprintf(GSI->GSI_file.ptr, "%i\n", onday_arr[ny]);
				fprintf(GSI->GSI_file.ptr, "%i\n", offday_arr[ny]);
			}
		}

	


	phenarr->onday_arr = onday_arr;
	phenarr->offday_arr= offday_arr;

	return (!ok);

} /* end - subroutine */



	