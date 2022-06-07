/*
metarr_init.c
Initialize meteorological data arrays for pointbgc simulation

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Biome-BGCMuSo v6.0.
Original code: Copyright 2000, Peter E. Thornton
Numerical Terradynamic Simulation Group, The University of Montana, USA
Modified code: Copyright 2019, D. Hidy [dori.hidy@gmail.com]
Hungarian Academy of Sciences, Hungary
See the website of Biome-BGCMuSo at http://nimbus.elte.hu/bbgc/ for documentation, model executable and example input files.
*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "ini.h"
#include "bgc_struct.h"
#include "bgc_constants.h"
#include "pointbgc_struct.h"
#include "pointbgc_func.h"
#include "misc_func.h"

/* It is assumed here that the meteorological datafile contains the following 
list of variables, with the indicated units.  All other variables in the metv
arrays are derived from this basic set:

VARIABLE    UNITS
yday        (none) (yearday)
prcp        cm     (daily total precipitation, water equivalent)  
tmax        deg C  (daily maximum temperature) 
tmin        deg C  (daily minimum temperature)
VPD         Pa     (daylight average VPD)
swavgfd     W/m2   (daylight average shortwave flux density)
daylength   s      (daylight duration)

*/

int metarr_init(point_struct* point, metarr_struct* metarr, const climchange_struct* scc,const siteconst_struct* sitec, const control_struct* ctrl) 
{
	int errflag=0;
	int sd,j, metread;
	int ndays, nsimdays, dd, n_METvar;
	int nyears, year, day;
	double tmax, tmin, tday, tavg, prcp, vpd, swavgfd, dayl;
	double sw_MJ;

	n_METvar = 9;
	tmax=tmin=tday=tavg=prcp=vpd=swavgfd=dayl=0;
	nyears = ctrl->simyears;
	ndays    = NDAYS_OF_YEAR * nyears;
	nsimdays = NDAYS_OF_YEAR * (nyears-1) + point->nday_lastsimyear;

	/* allocate space for the metv arrays */
	if (!errflag)
	{
		metarr->tmax = (double*) malloc(ndays * sizeof(double));
		if (!metarr->tmax)
		{
			printf("ERROR allocating for tmax array\n");
			errflag=218;
		}
	}

	if (!errflag)
	{
		metarr->tmin = (double*) malloc(ndays * sizeof(double));
		if (!metarr->tmin)
		{
			printf("ERROR allocating for tmin array\n");
			errflag=218;
		}
	}

	if (!errflag)
	{
		metarr->prcp = (double*) malloc(ndays * sizeof(double));
		if (!metarr->prcp)
		{
			printf("ERROR allocating for prcp array\n");
			errflag=218;
		}
	}

	if (!errflag)
	{
		metarr->vpd = (double*) malloc(ndays * sizeof(double));
		if (!metarr->vpd)
		{
			printf("ERROR allocating for vpd array\n");
			errflag=218;
		}
	}

	if (!errflag)
	{
		metarr->tday = (double*) malloc(ndays * sizeof(double));
		if (!metarr->tday)
		{
			printf("ERROR allocating for tday array\n");
			errflag=218;
		}
	}

	if (!errflag)
	{
		metarr->tavg = (double*) malloc(ndays * sizeof(double));
		if (!metarr->tavg)
		{
			printf("ERROR allocating for tavg11 array\n");
			errflag=218;
		}
	}

	if (!errflag)
	{
		metarr->tavg11_ra = (double*) malloc(ndays * sizeof(double));
		if (!metarr->tavg11_ra)
		{
			printf("ERROR allocating for tavg11_ra array\n");
			errflag=218;
		}
	}

	/* new arrays */
	
	if (!errflag)
	{
		metarr->tavg30_ra = (double*) malloc(ndays * sizeof(double));
		if (!metarr->tavg30_ra)
		{
			printf("ERROR allocating for tavg30_ra array\n");
			errflag=218;
		}
	}

	if (!errflag)
	{
		metarr->tavg10_ra = (double*) malloc(ndays * sizeof(double));
		if (!metarr->tavg10_ra)
		{
			printf("ERROR allocating for tavg10_ra array\n");
			errflag=218;
		}
	}

	if (!errflag)
	{
		metarr->F_temprad = (double*) malloc(ndays * sizeof(double));
		if (!metarr->F_temprad)
		{
			printf("ERROR allocating for F_temprad array\n");
			errflag=218;
		}
	}

	if (!errflag)
	{
		metarr->F_temprad_ra = (double*) malloc(ndays * sizeof(double));
		if (!metarr->F_temprad_ra)
		{
			printf("ERROR allocating for F_temprad_ra array\n");
			errflag=218;
		}
	}

	if (!errflag)
	{
		metarr->swavgfd = (double*) malloc(ndays * sizeof(double));
		if (!metarr->swavgfd)
		{
			printf("ERROR allocating for swavgfd array\n");
			errflag=218;
		}
	}
	
	if (!errflag)
	{
		metarr->par = (double*) malloc(ndays * sizeof(double));
		if (!metarr->par)
		{
			printf("ERROR allocating for par array\n");
			errflag=218;
		}
	}
	
	if (!errflag)
	{
		metarr->dayl = (double*) malloc(ndays * sizeof(double));
		if (!metarr->dayl)
		{
			printf("ERROR allocating for dayl array\n");
			errflag=218;
		}
	}

	
	/* begin daily loop: read input file, generate array values */
	sd=0;
	while (!errflag && sd < ndays)
	{
		metread = fscanf(point->metf.ptr,"%i%i%lf%lf%lf%lf%lf%lf%lf%*[^\n]",&year,&day,&tmax,&tmin,&tday,&prcp,&vpd,&swavgfd,&dayl);
		if (sd < nsimdays && metread != n_METvar)
		{
			printf("ERROR reading MET variables (must be 9 columns: year,day,tmax,tmin,tday,prcp,vpd,swavgfd,dayl)\n");
			errflag=21801;
		}

		if (sd == 0 && year > ctrl->simstartyear)
		{
			printf("ERROR reading MET variables: file must contain meteorological data for each simulation day (from the beginning)\n");
			errflag=21802;
		}
		
		if (point->nday_lastsimyear < NDAYS_OF_YEAR && sd >= nsimdays)
		{
			tmax=tmin=tday=prcp=vpd=swavgfd=dayl=0;
			for (j=0; j<nyears-1;j++)
			{
				dd = (j * NDAYS_OF_YEAR) + (point->nday_lastsimyear - 1);
				tmax     += metarr->tmax[dd];
				tmin     += metarr->tmin[dd];
				tday     += metarr->tday[dd];
				prcp     += metarr->prcp[dd];
				vpd      += metarr->vpd[dd];
				swavgfd  += metarr->swavgfd[dd];
				dayl     += metarr->dayl[dd];
			}
			tmax    = tmax    / (nyears-1);
			tmin    = tmin    / (nyears-1);
			tday    = tday    / (nyears-1);
			prcp    = prcp    / (nyears-1);
			vpd     = vpd     / (nyears-1);
			swavgfd = swavgfd / (nyears-1);
			dayl    = dayl    / (nyears-1);
		}
	

		/* read tmax, tmin, prcp, vpd, and srad */
		/* the following scan statement discards the tday field in the standard MTCLIM version 3.1 input file */
		if (year >= ctrl->simstartyear && year < ctrl->simstartyear + ctrl->simyears)
		{
		
			/* control to avoid negative meteorological data */
 			if (prcp < 0 || vpd < 0 || swavgfd < 0 || dayl <= 0)
			{
				printf("ERROR in met file: negative prcp/vpd/swavgfd/dayl, metv_init()\n");
				errflag=21803;
			}
		}
		else
		{
			if (sd == 0 && year > ctrl->simstartyear)
			{
				printf("ERROR reading met array, metarr_init()\n");
				printf("Note: file must contain meteorological data for each simulation day\n");
				errflag=21804;
			}
		}

		
		/* apply the climate change scenario and store */
		if (!errflag && year >= ctrl->simstartyear)
		{
			metarr->tmax[sd] = tmax + scc->s_tmax;
			metarr->tmin[sd] = tmin + scc->s_tmin;
			tavg = (tmax + tmin) / 2.0;
			
			metarr->tavg[sd] = (metarr->tmax[sd] + metarr->tmin[sd]) / 2.0;
			metarr->tday[sd] = tday + (metarr->tavg[sd] - tavg); 

			metarr->prcp[sd] = prcp * scc->s_prcp;
			metarr->vpd[sd] = vpd * scc->s_vpd;
			metarr->swavgfd[sd] = swavgfd * scc->s_swavgfd;
			metarr->par[sd] = swavgfd * 0.45 * scc->s_swavgfd;
			metarr->dayl[sd] = dayl;

			/* factor for soil temp. calculation */
			sw_MJ = swavgfd * dayl / 1000000;
			if (sd == 0)
				metarr->F_temprad[sd] = metarr->tavg[sd] +  (tmax - metarr->tavg[sd]) * sqrt(sw_MJ*0.03);
			else
				metarr->F_temprad[sd] = (1 - sitec->albedo_sw) * (metarr->tavg[sd] +  (tmax - metarr->tavg[sd]) * sqrt(sw_MJ*0.03))
										+ sitec->albedo_sw * metarr->F_temprad[sd-1];

			/* counter of simdays*/			
			sd += 1;
		}
	}



	/* perform running averages of daily average temperature for 
	use in soil temperature routine. 

	This implementation uses a linearly ramped 11-day running average 
	of daily mean air temperature, with days 1-10 based on a 1-10 day
	running average, respectively. 
	*/
	
	if (!errflag && run_avg(metarr->tavg, metarr->tavg11_ra, ndays, 11, 1))
	{
		printf("ERROR: run_avg() in metv_init.c \n");
		errflag=21805;
	}
	
	/*  new averages */
	if (!errflag && run_avg(metarr->tavg, metarr->tavg30_ra, ndays, 30, 0))
	{
		printf("ERROR: run_avg() in metv_init.c \n");
		errflag=21805;
	}

	if (!errflag && run_avg(metarr->tavg, metarr->tavg10_ra, ndays, 10, 0))
	{
		printf("ERROR: run_avg() in metv_init.c \n");
		errflag=21805;
	}

	if (!errflag && run_avg(metarr->F_temprad, metarr->F_temprad_ra, ndays, 5, 0))
	{
		printf("ERROR: run_avg() in metv_init.c \n");
		errflag=21805;
	}
	return (errflag);
}
