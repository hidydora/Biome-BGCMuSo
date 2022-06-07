/* 
sitec_init.c
Initialize the site physical constants for bgc simulation

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Biome-BGC version 4.1.1
Copyright 2000, Peter E. Thornton
Numerical Terradynamics Simulation Group (NTSG)
School of Forestry, University of Montana
Missoula, MT 59812
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
	double sand,silt,clay; /* percent sand, silt, and clay */
	int layer; /* Hidy 2011: multilyer soil */

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
		printf("Error reading number of max_rootzone_depths\n");
		ok=0;
	}

	if (sitec->max_rootzone_depth < 0)
	{
		printf("maximum of rooting depth must have positive value\n");
		ok=0;
	}


  	/* predefined values: depth of the layers (soillayer_depths;[m]) */
	sitec->soillayer_depths[0] = 0.1;
	sitec->soillayer_depths[1] = 0.3;
	sitec->soillayer_depths[2] = 1.;
	sitec->soillayer_depths[3] = 3.;

	/* calculated values: depth of the layers (delta_z;[m]) */
	sitec->delta_z[0] = -0.05;
	sitec->delta_z[1] = -0.2;
	sitec->delta_z[2] = -0.65;
	sitec->delta_z[3] = -2.0;

	/* calculated values: thickness of the layers (soillayer_thickness;[m]) */
	sitec->soillayer_thickness[0] = 0.1;
	sitec->soillayer_thickness[1] = 0.2;
	sitec->soillayer_thickness[2] = 0.7;
	sitec->soillayer_thickness[3] = 2.0;

	/* ratio of actual layer and the total soil depth (bottom layer is special: "infinite depth")*/
	for (layer = 0; layer < N_SOILLAYERS; layer++)
	{
		sitec->rel_soillayer_thickness[layer] = sitec->soillayer_thickness[layer] / sitec->soillayer_depths[N_SOILLAYERS-2];
		
	}

	/* Hidy 2012 - the 5th (bottom) layer is "fictive": infinitive depth */
	sitec->rel_soillayer_thickness[N_SOILLAYERS-1] = DATA_GAP;
	sitec->soillayer_depths[N_SOILLAYERS-1]        = DATA_GAP;
	sitec->delta_z[N_SOILLAYERS-1]                 = DATA_GAP;
	sitec->soillayer_thickness[N_SOILLAYERS-1]     = DATA_GAP;

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


		if (sitec->vwc_sat_mes == DATA_GAP)
		{
			/* Clapp-Hornberger parameter from empirical function */
			sitec->soil_b = 3.10 + 0.157*clay - 0.003*sand;
		
			/* saturation value of soil water potential */
			sitec->psi_sat = -(exp((1.54 - 0.0095*sand + 0.0063*silt)*log(10.0))*9.8e-5);

			/* soil water content at wilting point, field capacity and saturation in m3/m3 */
			sitec->vwc_sat = (50.5 - 0.142*sand - 0.037*clay)/100.0;
			sitec->vwc_fc = sitec->vwc_sat * (log(sitec->soil_b) / log(sitec->psi_fc/sitec->psi_sat));
			sitec->vwc_wp = sitec->vwc_sat * (log(sitec->soil_b) / log(sitec->psi_wp/sitec->psi_sat));
		
		}
		else
		{

			/* measured soil water content at wilting point, field capacity and saturation in m3/m3 */
			sitec->vwc_sat = sitec->vwc_sat_mes;
			sitec->vwc_fc  = sitec->vwc_fc_mes;
			sitec->vwc_wp  = sitec->vwc_wp_mes;

			sitec->soil_b = log(sitec->psi_fc/sitec->psi_wp) / log(sitec->vwc_wp/sitec->vwc_fc);
			
			sitec->psi_sat = sitec->psi_fc / pow(sitec->vwc_sat/sitec->vwc_fc, sitec->soil_b);
			sitec->psi_sat = sitec->psi_wp / pow(sitec->vwc_sat/sitec->vwc_wp, sitec->soil_b);


		}
		
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
