/* 
daymet.c
transfer one day of meteorological data from metarr struct to metv struct

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
BBGC MuSo v3.0.8
Copyright 2000, Peter E. Thornton
Numerical Terradynamics Simulation Group
Copyright 2014, D. Hidy
Hungarian Academy of Sciences
*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <malloc.h>
#include "bgc_struct.h"
#include "bgc_func.h"
#include "bgc_constants.h"

int daymet(const control_struct* ctrl, const metarr_struct* metarr, const siteconst_struct* sitec, const wstate_struct* ws, 
		   metvar_struct* metv, double* tair_annavg_ptr, int metday)
{
	/* generates daily meteorological variables from the metarray struct */
	int ok=1;
	int layer;
	double tmax,tmin,tavg,tavg_ra,tday,tdiff;

	/* Hidy 2010 - multilayer soil */
	double depth_top, depth_bottom, tsoil_top, tsoil_bottom;

	/* Hidy 2010 - new tsoil calculation */
	double temp_grad;
	double tsoil_avg = 0;

	/* convert prcp from cm --> kg/m2 */
	metv->prcp = metarr->prcp[metday] * 10.0;

	/* air temperature calculations (all temperatures deg C) */
	metv->tmax    = tmax    = metarr->tmax[metday];
	metv->tmin    = tmin    = metarr->tmin[metday];
	metv->tavg    = tavg    = metarr->tavg[metday];
	metv->tday    = tday	= metarr->tday[metday];
	metv->tnight  = (tday + tmin) / 2.0;
	metv->tavg_ra = tavg_ra = metarr->tavg_ra[metday];
	
	/* **********************************************************************************/
	/* Hidy 2010 - new estimation of tsoil () - on the first day original method is used */
	
	/* ORIGINAL: for this version, an 11-day running weighted average of daily average temperature is used as the soil temperature at 10 cm.
	For days 1-10, a 1-10 day running weighted average is used instead.The tail of the running average is weighted linearly from 1 to 11.
	There are no corrections for snowpack or vegetation cover.	*/

	if (metday < 1)
	{
		tsoil_top = metv->tavg_ra;
		/* soil temperature correction using difference from annual average tair */
		tdiff =  *tair_annavg_ptr - tsoil_top;
		
		if (ws->snoww)
		{
			tsoil_top += 0.2 * tdiff;
		}
		else
		{
			tsoil_top += 0.1 * tdiff;
		}

		metv->tsoil[0]      = tsoil_top;

		metv->tsoil_top_pre = tsoil_top;
		metv->tday_pre      = (metv->tmax + metv->tmin)/2.;
	}
	
	/* **********************************************************************************/
	/* Hidy 2010 - initalizing  multilayer soil temperatures */
	
	depth_top = 0;
	depth_bottom = 0;

	if (ctrl->spinyears == 0 && ctrl->yday == 0)
	{
		/* 3 m below the ground surface (last layer) is specified by the annual mean surface air temperature */
		metv->tsoil[N_SOILLAYERS-1] = sitec->mean_surf_air_temp;
		
	
		/* on the first day the temperature of the soil layers are calculated based on the temperature of top and bottom layer */
		temp_grad = (metv->tsoil[N_SOILLAYERS-1] - metv->tsoil[0]) / sitec->soillayer_depth[N_SOILLAYERS-2];
		
		for (layer = 1; layer < N_SOILLAYERS-1; layer++)
		{
			depth_top	 = sitec->soillayer_depth[layer-1];
			depth_bottom = sitec->soillayer_depth[layer];
				
			tsoil_top	   	   = metv->tsoil[0] + temp_grad * depth_top;
			tsoil_bottom	   = metv->tsoil[0] + temp_grad * depth_bottom;
			
			metv->tsoil[layer] = (tsoil_top + tsoil_bottom)/2.;	

			/* average value regarding to soil without bottom layer */
			tsoil_avg		   += metv->tsoil[layer] * (sitec->soillayer_thickness[layer] / sitec->soillayer_depth[N_SOILLAYERS-2]);
		}
			
		metv->tsoil_avg = tsoil_avg;
		
		/* on the first day no soil tempreture change is calculated -> initalizing */
		for (layer = 0; layer < N_SOILLAYERS; layer++)
		{
			metv->tsoil_change[layer] = 0;
		}

	}



	
	/* **********************************************************************************/
	
	/* daylight average vapor pressure deficit (Pa) */
	metv->vpd = metarr->vpd[metday];

	/* daylight average	shortwave flux density (W/m2) */
	metv->swavgfd =  metarr->swavgfd[metday];
	
	/* PAR (W/m2) */
	metv->par = metarr->par[metday];

	/* daylength (s) */
	metv->dayl = metarr->dayl[metday];

	return (!ok);
}
