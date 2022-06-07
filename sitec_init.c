/* 
sitec_init.c
Initialize the site physical constants for bgc simulation

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
#include <string.h>
#include <time.h>
#include <math.h>
#include "ini.h"
#include "bgc_struct.h"
#include "bgc_constants.h"
#include "pointbgc_struct.h"
#include "pointbgc_func.h"
#include "bgc_func.h"

int sitec_init(file init, siteconst_struct* sitec)
{
	/* reads the site physical constants from *.init */ 

	int errflag=0;

	int layer;
	char key[] = "SITE";
	char keyword[STRINGSIZE];
	
	

	/*--------------------------------------------------------------------------------------
	READING INPUT DATA */

	/* first scan keyword to ensure proper *.init format */ 
	if (!errflag && scan_value(init, keyword, 's'))
	{
		printf("ERROR reading keyword, sitec_init()\n");
		errflag=207;
	}
	if (!errflag && strcmp(keyword,key))
	{
		printf("Expecting keyword --> %s in %s\n",key,init.name);
		errflag=207;
	}


	/* other site data - elev, lat, alb */
	if (!errflag && scan_value(init, &sitec->elev, 'd'))
	{
		printf("ERROR reading elevation, sitec_init()\n");
		errflag=20701;
	}
	if (!errflag && scan_value(init, &sitec->lat, 'd'))
	{
		printf("ERROR reading site latitude, sitec_init()\n");
		errflag=20702;
	}
	if (!errflag && scan_value(init, &sitec->albedo_sw, 'd'))
	{
		printf("ERROR reading shortwave albedo, sitec_init()\n");
		errflag=20703;
	}
	

	/* FIRST APPROXIMATION to initalize multilayer soil temperature -> mean_surf_air_temp [Celsius] */
	if (!errflag && scan_value(init, &sitec->tair_annavg, 'd'))
	{
		printf("ERROR reading tair_annavg, sitec_init()\n");
		errflag=20704;
	}

	if (!errflag && scan_value(init, &sitec->tair_annrange, 'd'))
	{
		printf("ERROR reading tair_annrange, sitec_init()\n");
		errflag=20705;
	}
	
	if (!errflag && scan_value(init, &sitec->NdepNH4_coeff, 'd'))
	{
		printf("ERROR reading NdepNH4_coeff: sitec_init()\n");
		errflag=20706;
	}


	/* CONTROL to avoid negative meteorological data */
 	if (sitec->albedo_sw < 0 || sitec->NdepNH4_coeff < 0)
	{
		printf("ERROR in site data: swalb and NdepNH4_coeff must be positive\n");
		errflag=20707;
	}

	

	/*--------------------------------------------------------------------------------------*/
	/* INITALIZING */

	/* predefined values: depth of the layers (soillayer_depth;[m])  */
	sitec->soillayer_depth[0] = 0.03;
	sitec->soillayer_depth[1] = 0.10;
	sitec->soillayer_depth[2] = 0.30;
	sitec->soillayer_depth[3] = 0.60;
	sitec->soillayer_depth[4] = 0.90;
	sitec->soillayer_depth[5] = 1.20;
	sitec->soillayer_depth[6] = 1.50;
	sitec->soillayer_depth[7] = 2.00;
	sitec->soillayer_depth[8] = 4.00;
	sitec->soillayer_depth[9] = 10.0;


	/* calculated values: thickness and depth of the layers (soillayer_thickness[m] and soillayer_midpoint[m]) */
	
	for (layer = 0; layer < N_SOILLAYERS; layer++)
	{
		if (layer == 0)
		{
			sitec->soillayer_thickness[layer] = sitec->soillayer_depth[layer];
			sitec->soillayer_midpoint[layer]  = sitec->soillayer_thickness[layer] / 2.;
		}
		else
		{
			sitec->soillayer_thickness[layer] = sitec->soillayer_depth[layer]-sitec->soillayer_depth[layer-1];
			sitec->soillayer_midpoint[layer]  = sitec->soillayer_depth[layer-1]+ sitec->soillayer_thickness[layer] / 2.;
		}
	}
	
	
 	return (errflag);
}


