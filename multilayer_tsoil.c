 /* 
multilayer_tsoil.c
calculation of soil temperature in the different soil layers based on the change of air temperature (direct connection)
to top soil layer and based on empirical function of temperature gradient in soil (Zheng et al.1993)

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
BBGC MuSo v3.0.8
Copyright 2014, D. Hidy
Hungarian Academy of Sciences
*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "ini.h"     
#include "pointbgc_struct.h"
#include "bgc_struct.h"
#include "pointbgc_func.h"
#include "bgc_constants.h"

int multilayer_tsoil(const epconst_struct* epc, const siteconst_struct* sitec, const wstate_struct* ws, 
					 metvar_struct* metv, epvar_struct* epv)
{
	int ok=1;
	int layer;

	/* effect of air temperature change on top soil temperature */
	double effect_of_vegetation = 1;
	double heating_coefficient = 0.25;	
	double tsoil_top_change;
	double tsoil_avg = 0;
	double tsoil_top = 0;


	/* daily averaged air tempreture on the given day (calculated from tmax and tmin), temp.gradient and local temperatures */
	double tday_act, temp_grad, depth_top, depth_bottom, tsoil_bottom;
	
	tsoil_top = metv->tsoil[0];
	tday_act  = metv->tday; //(metv->tmax + metv->tmin)/2;
	

	/* *********************************************************** */
	/* 1. FIRST LAYER PROPERTIES */
	/* surface soil temperature change caused by air temp. change can be estimated from the air temperature using empirical equations */	

	if (epv->proj_lai > 0) /* vegetation period */
	{
		if (metv->tday_pre > metv->tsoil_top_pre) 
		{
			/* shading effect of vegetation (if soil temperature is lower than air temperature the effect is zero) */
			effect_of_vegetation = exp(-1 * epc->ext_coef * epv->proj_lai);
			if (effect_of_vegetation < 0.5) effect_of_vegetation = 0.5;
		}
		else effect_of_vegetation = 1.0;
		
		/* empirical function for the effect of tair changing */
		tsoil_top_change = (tday_act - metv->tsoil_top_pre) * heating_coefficient * effect_of_vegetation;	
	}
	else 	/* dormant period */
	{
		if (ws->snoww) heating_coefficient = 0.1;

		/* no change if soil temperature is higher than air temperature but actual air temperature is higher than previous */
		if (tday_act < tsoil_top && tday_act > metv->tday_pre)
		{
			tsoil_top_change = 0;
		}
		else
			tsoil_top_change = (tday_act - metv->tday_pre) * heating_coefficient;
			
	}


	/* ************************************************- */
	/* 2. TEMPERATURE OF DEEPER LAYER BASED ON TEMPERATURE GRADIENT BETWEEN SURFACE LAYER AND LOWERMOST LAYER (BELOW 3M) */

	/* on the first day the temperature of the soil layers are calculated based on the temperature of top and bottom layer */

	metv->tsoil[0] += tsoil_top_change;
	tsoil_avg	   += metv->tsoil[0] * (sitec->soillayer_thickness[0] / sitec->soillayer_depth[N_SOILLAYERS-2]);
	
	temp_grad = (metv->tsoil[N_SOILLAYERS-1] - metv->tsoil[0]) / sitec->soillayer_depth[N_SOILLAYERS-2];
	
	for (layer = 1; layer < N_SOILLAYERS-1; layer++)
	{
		depth_top	 = sitec->soillayer_depth[layer-1];
		depth_bottom = sitec->soillayer_depth[layer];
				
		tsoil_top	   	   = metv->tsoil[0] + temp_grad * depth_top;
		tsoil_bottom	   = metv->tsoil[0] + temp_grad * depth_bottom;
			
		metv->tsoil[layer] = (tsoil_top + tsoil_bottom)/2.;	
		tsoil_avg		   += metv->tsoil[layer] * (sitec->soillayer_thickness[layer] / sitec->soillayer_depth[N_SOILLAYERS-2]);
	}

	metv->tsoil_avg     = tsoil_avg;
	metv->tsoil_top_pre = metv->tsoil[0];
	metv->tday_pre      = tday_act; 
   
	return (!ok);
}
	