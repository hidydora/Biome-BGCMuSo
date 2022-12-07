/* 
infiltANDpond.c
calculation of pond water accumulation and potential infiltration

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Biome-BGCMuSo v7.0.
Copyright 2022, D. Hidy [dori.hidy@gmail.com]
Hungarian Academy of Sciences, Hungary
See the website of Biome-BGCMuSo at http://nimbus.elte.hu/bbgc/ for documentation, model executable and example input files.
*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <malloc.h>
#include "ini.h"
#include "bgc_struct.h"
#include "bgc_constants.h"
#include "bgc_func.h"


int infiltANDpond(siteconst_struct* sitec, soilprop_struct* sprop, epvar_struct* epv, wstate_struct* ws, wflux_struct* wf)
{

	
	/* internal variables */
	int errorCode, layer, flagEXTRA;

	double soilw_dist, soilwEXTRA;

	 errorCode=layer=flagEXTRA=0;
     
	/*------------------------------------------*/
	/* 1. calculation of water from above */ 
	
	 wf->waterFromAbove = (wf->prcp_to_soilSurface + wf->snoww_to_soilw + wf->canopyw_to_soilw + wf->IRG_to_soilSurface);

	
	 /*------------------------------------------*/
	/* 2.calculation of the amount of water which can still fits into the soil */ 
	
	soilw_dist = (sprop->hydrCONDUCTsat[0]*sitec->soillayer_thickness[0]/sitec->soillayer_depth[1] + 
		          sprop->hydrCONDUCTsat[1]*sitec->soillayer_thickness[1]/sitec->soillayer_depth[1]) * nSEC_IN_DAY;

	soilwEXTRA = (sprop->VWCsat[0] - epv->VWC[0]) * sitec->soillayer_thickness[0] * water_density;
	if (soilwEXTRA < CRIT_PRECwater) soilwEXTRA = 0;

	while (flagEXTRA == 0 && layer < N_SOILLAYERS-1)
	{
		if (soilw_dist > sitec->soillayer_depth[layer])
			soilwEXTRA += (sprop->VWCsat[layer+1] - epv->VWC[layer+1]) * sitec->soillayer_thickness[layer+1] * water_density;
		else
			flagEXTRA = 1;

		layer += 1;
	}
	
	
	/* ---------------------------------------*/
	/* 3. if there is pond water in the area: pondw_to_soilw, infiltPOT */ 

	if (ws->pondw)
	{
		ws->pondw += wf->waterFromAbove;
	

		/* PRCP to pond water (limitaion: pondmax)  */
		if (ws->pondw > soilwEXTRA)
			wf->pondw_to_soilw = soilwEXTRA;
		else
			wf->pondw_to_soilw = ws->pondw;

		wf->pondw_to_soilw = ws->pondw;
		ws->pondw         -= wf->pondw_to_soilw;
		
		wf->infiltPOT      = wf->pondw_to_soilw;



	/* ---------------------------------------*/
	/* 4. if there is pond water in the area: soilw_to_pondw, infiltPOT */ 
	}
	else
	{
		
		/* if empty space in soil is greater than infiltration -> pond water formation (and runoff - if pond water is small)*/
		if (wf->waterFromAbove > soilwEXTRA)
		{
			wf->infiltPOT       = soilwEXTRA;
			wf->prcp_to_pondw = wf->waterFromAbove - soilwEXTRA;

		}
		else
			wf->infiltPOT = wf->waterFromAbove; 


	} 
	
	

	return (errorCode);
}
