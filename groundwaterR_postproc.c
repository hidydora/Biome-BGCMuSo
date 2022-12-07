/* 
groundwaterR_postproc.c
calculate the effect of gound water depth in the soil

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



int groundwaterR_postproc(const siteconst_struct* sitec,  epvar_struct* epv, wstate_struct* ws, wflux_struct* wf, GWcalc_struct* gwc)
{
	int errorCode,layer,layerGWC;	
	double soilw_act,soilwFlux_act,soilwTRP_act,soilwTRPdemand_act,GWrecharge_act,GWdischarge_act;

	layer=layerGWC=errorCode=0;



	/* calculation of soillayer thickness, soilw, rootlength proportion in GWC arrays */
	layerGWC = 0;
	for (layer = 0; layer < N_SOILLAYERS; layer++)
	{
		
		/* same bottom layer */
		if (sitec->soillayer_depth[layer] == gwc->soillayer_depthGWC[layerGWC])
		{
			ws->soilw[layer]             = gwc->soilw_GWC[layerGWC];
			wf->soilwFlux[layer]         = gwc->soilwFlux_GWC[layerGWC];
			wf->soilwTRP[layer]       = gwc->soilwTRP_GWC[layerGWC];
			wf->soilwTRPdemand[layer] = gwc->soilwTRPdemand_GWC[layerGWC];
			wf->GWrecharge[layer]        = gwc->GWrecharge_GWC[layerGWC];
			wf->GWdischarge[layer]       = gwc->GWdischarge_GWC[layerGWC];
			layerGWC = layerGWC+1;
		}
		/* different bottom layer */
		else
		{
			soilw_act=0;
			soilwFlux_act=0;
			soilwTRP_act=0;
			soilwTRPdemand_act=0;
			GWrecharge_act=0;
			GWdischarge_act=0;
			while (sitec->soillayer_depth[layer] >= gwc->soillayer_depthGWC[layerGWC] && layerGWC < N_SOILLAYERS_GWC)
			{
				soilw_act             += gwc->soilw_GWC[layerGWC];
				soilwFlux_act         = gwc->soilwFlux_GWC[layerGWC];
				soilwTRP_act       += gwc->soilwTRP_GWC[layerGWC];
				soilwTRPdemand_act += gwc->soilwTRPdemand_GWC[layerGWC];
				GWrecharge_act        += gwc->GWrecharge_GWC[layerGWC];
				GWdischarge_act       += gwc->GWdischarge_GWC[layerGWC];
				layerGWC              += 1;
			}
			ws->soilw[layer]             = soilw_act;
			wf->soilwFlux[layer]         = soilwFlux_act;
			wf->soilwTRP[layer]       = soilwTRP_act;
			wf->soilwTRPdemand[layer] = soilwTRPdemand_act;
			wf->GWrecharge[layer]        = GWrecharge_act;
			wf->GWdischarge[layer]       = GWdischarge_act;
			
		}

		epv->VWC[layer]              = ws->soilw[layer] / (sitec->soillayer_thickness[layer] * water_density);

	}

		
	
	return (errorCode);
}
