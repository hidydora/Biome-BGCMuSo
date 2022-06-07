/* 
sitec_init.c
Initialize the site physical constants for bgc simulation

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
BBGC MuSo 2.3
Copyright 2000, Peter E. Thornton
Copyright 2014, D. Hidy
*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "ini.h"
#include "bgc_struct.h"
#include "bgc_constants.h"
#include "pointbgc_struct.h"
#include "pointbgc_func.h"

int sitec_init(file init, siteconst_struct* sitec)
{
	/* reads the site physical constants from *.init */ 

	int ok=1;
	char key[] = "SITE";
	char keyword[80];
	/* percent sand, silt, and clay */
	double sand = 0;
	double silt = 0;
	double clay = 0; 

	/* first scan keyword to ensure proper *.init format */ 
	if (ok && scan_value(init, keyword, 's'))
	{
		printf("Error reading keyword, sitec_init()\n");
		ok=0;
	}
	if (ok && strcmp(keyword,key))
	{
		printf("Expecting keyword --> %s in %s\n",key,init.name);
		ok=0;
	}

	/* begin reading constants from *.init */

	/* *************************************************- */
	/* Hidy 2010 - MULTILAYER SOIL MODUL */
	
	/* reading from ini file: max_rootzone_depth */
	if (ok && scan_value(init, &sitec->max_rootzone_depth, 'd'))
	{
		printf("Error reading number of max_rootzone_depth\n");
		ok=0;
	}

  	/* predefined values: depth of the layers (soillayer_depth;[m]) */
	sitec->soillayer_depth[0] = 0.1;
	sitec->soillayer_depth[1] = 0.3;
	sitec->soillayer_depth[2] = 0.6;
	sitec->soillayer_depth[3] = 1.;
	sitec->soillayer_depth[4] = 2.;
	sitec->soillayer_depth[5] = 3.;
	sitec->soillayer_depth[6] = 5.;


	/* calculated values: depth of the layers (soillayer_midpoint;[m]) */
	sitec->soillayer_midpoint[0] = 0.05;
	sitec->soillayer_midpoint[1] = 0.2;
	sitec->soillayer_midpoint[2] = 0.45;
	sitec->soillayer_midpoint[3] = 0.8;
	sitec->soillayer_midpoint[4] = 1.5;
	sitec->soillayer_midpoint[5] = 2.5;
	sitec->soillayer_midpoint[6] = 4;

	/* calculated values: thickness of the layers (soillayer_thickness;[m]) */
	sitec->soillayer_thickness[0] = 0.1;
	sitec->soillayer_thickness[1] = 0.2;
	sitec->soillayer_thickness[2] = 0.3;
	sitec->soillayer_thickness[3] = 0.7;
	sitec->soillayer_thickness[4] = 1.0;
	sitec->soillayer_thickness[5] = 1.0;
	sitec->soillayer_thickness[6] = 2.0;


	/* *************************************************- */
	if (ok && scan_value(init, &sand, 'd'))
	{
		printf("Error reading percent sand, sitec_init()\n");
		ok=0;
	}
	if (ok && scan_value(init, &silt, 'd'))
	{
		printf("Error reading percent clay, sitec_init()\n");
		ok=0;
	}
	if (ok && scan_value(init, &clay, 'd'))
	{
		printf("Error reading percent clay, sitec_init()\n");
		ok=0;
	}
	
	if (ok && scan_value(init, &sitec->elev, 'd'))
	{
		printf("Error reading elevation, sitec_init()\n");
		ok=0;
	}
	if (ok && scan_value(init, &sitec->lat, 'd'))
	{
		printf("Error reading site latitude, sitec_init()\n");
		ok=0;
	}
	if (ok && scan_value(init, &sitec->sw_alb, 'd'))
	{
		printf("Error reading shortwave albedo, sitec_init()\n");
		ok=0;
	}
	if (ok && scan_value(init, &sitec->ndep, 'd'))
	{
		printf("Error reading N deposition, sitec_init()\n");
		ok=0;
	}
	if (ok && scan_value(init, &sitec->nfix, 'd'))
	{
		printf("Error reading N fixation, sitec_init()\n");
		ok=0;
	}

	/* Hidy 2010 - FIRST APPROXIMATION to initalize multilayer soil temperature -> mean_surf_air_temp [Celsius] */
	if (ok && scan_value(init, &sitec->mean_surf_air_temp, 'd'))
	{
		printf("Error reading mean_surf_air_temp, sitec_init()\n");
		ok=0;
	}

	/* Hidy 2010 - runoff parameter (Campbell and Diaz) */
	if (ok && scan_value(init, &sitec->runoff_param, 'd'))
	{
		printf("Error reading runoff parameter: sitec_init()\n");
		ok=0;
	}
	
	/* Hidy 2011 - measured soil water content at wilting point */
	if (ok && scan_value(init, &sitec->vwc_sat_mes, 'd'))
	{
		printf("Error reading soil water content at saturation: sitec_init()\n");
		ok=0;
	}
	
	/* Hidy 2011 - measured soil water content at field capacity */
	if (ok && scan_value(init, &sitec->vwc_fc_mes, 'd'))
	{
		printf("Error reading soil water content at field capacity: sitec_init()\n");
		ok=0;
	}

	/* Hidy 2011 - measured soil water content at saturation */
	if (ok && scan_value(init, &sitec->vwc_wp_mes, 'd'))
	{
		printf("Error reading soil water content at wilting point: sitec_init()\n");
		ok=0;
	}

	/* -------------------------------------------------- CONTROL -----------------------------------------------------------------*/

	/* Hidy 2013 - control to avoid negative meteorological data */
 	if (sitec->max_rootzone_depth < 0 || sand < 0 || silt < 0 || clay < 0 || sitec->sw_alb < 0 || sitec->ndep < 0 || sitec->nfix < 0 || 
		sitec->runoff_param < 0 || sitec->vwc_sat_mes < 0 || sitec->vwc_fc_mes < 0 || sitec->vwc_wp_mes < 0 )
	{
		printf("Error in site data in INI file: negative rootzone_depth/sand/silt/clay/ndep/nfix/runoff/vwc data, sitec_init()\n");
		ok=0;
	}

	/* maximum of the soil depth of the model is 5 m */
	if (sitec->max_rootzone_depth > 5)
	{
		printf("maximum of rooting depth must be less than equal to 5 meter\n");
		ok=0;
	}


	/* calculate the soil pressure-volume coefficients from texture data */
	/* Uses the multivariate regressions from Cosby et al., 1984 and Wagner et al. 2001*/
	/* first check that the percentages add to 100.0 */
	if (ok && sand+silt+clay != 100)
	{
		printf("Error: %%sand + %%silt + %%clay  MUST EQUAL 100%%\n");
		printf("Check values in initialization file.\n");
		ok=0;
	}
	if (ok)
	{
	
		/* soil water content, soil water potential and Clapp-Hornberger parameter */	
		/* !!!!!!!!!!!!!!!!!!!! MEASURED DATA IS USED IF IT IS AVAILABLE (Hidy, 2011) !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
		/* estimated soil water potential at wilting point, field capacity in MPa (1MPa = 10000cm)  (fc: pF = 2.5; wp: pF = 4.2) */
		sitec->psi_fc  = pow(10,pF_fieldcapacity) / (-10000);
		sitec->psi_wp  = pow(10,pF_wiltingpoint) / (-10000);
		sitec->psi_hw  = pow(10,pF_hygroscopw) / (-10000);


		if (sitec->vwc_sat_mes == (double) DATA_GAP)
		{
			/* Clapp-Hornberger parameter from empirical function */
			sitec->soil_b = 3.10 + 0.157*clay - 0.003*sand;
		
			/* saturation value of soil water potential */
	        sitec->psi_sat = -(exp((1.54 - 0.0095*sand + 0.0063*silt)*log(10.0))*9.8e-5);

			/* soil water content at wilting point, field capacity and saturation in m3/m3 */
		    sitec->vwc_sat = (50.5 - 0.142*sand - 0.037*clay)/100.0;
			sitec->vwc_fc = sitec->vwc_sat * (log(sitec->soil_b) / log(sitec->psi_fc/sitec->psi_sat));
			sitec->vwc_wp = sitec->vwc_sat * (log(sitec->soil_b) / log(sitec->psi_wp/sitec->psi_sat));

			/* control for soil type with higy clay content - Hidy 2013. */
			if (sitec->vwc_fc > sitec->vwc_sat) sitec->vwc_fc = sitec->vwc_sat - 0.05;
			if (sitec->vwc_wp > sitec->vwc_fc) sitec->vwc_wp = sitec->vwc_fc - 0.05;
	
		}
		else
		{

			/* measured soil water content at wilting point, field capacity and saturation in m3/m3 */
			sitec->vwc_sat = sitec->vwc_sat_mes;
			sitec->vwc_fc  = sitec->vwc_fc_mes;
			sitec->vwc_wp  = sitec->vwc_wp_mes;

			sitec->soil_b = log(sitec->psi_fc/sitec->psi_wp) / log(sitec->vwc_wp/sitec->vwc_fc);
			
			sitec->psi_sat = sitec->psi_fc / pow(sitec->vwc_sat/sitec->vwc_fc, -1*sitec->soil_b);
		}
		
		sitec->vwc_hw = sitec->vwc_sat * (log(sitec->soil_b) / log(sitec->psi_hw/sitec->psi_sat));	
		/* !!!!!!!!!!!!!!!!!!!! !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/


		/* soil parameters at saturation (Cosby et al. 1984)*/	
		sitec->hydr_conduct_sat = 7.05556 * 1e-6 * pow(10, (-0.6+0.0126*sand-0.0064*clay));
		sitec->hydr_diffus_sat = (sitec->soil_b * sitec->hydr_conduct_sat * (-100*sitec->psi_sat))/sitec->vwc_sat;
	
		/* soil parameters at field capacity (soil water potential: -0.01MPa = 10kPa = 100hPa = 100cm = 1m )*/	
		sitec->hydr_conduct_fc = sitec->hydr_conduct_sat * pow(sitec->vwc_fc/sitec->vwc_sat, 2*sitec->soil_b+3);
		sitec->hydr_diffus_fc = (((sitec->soil_b * sitec->hydr_conduct_sat * (-100*sitec->psi_sat))) / sitec->vwc_sat) * 
							pow(sitec->vwc_fc/sitec->vwc_sat, sitec->soil_b+2);
		

	}

		
	
	return (!ok);
}
