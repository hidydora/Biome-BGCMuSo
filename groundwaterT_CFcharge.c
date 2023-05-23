/* 
groundwaterT_CFcharge.c
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
#include "bgc_func.h"


int groundwaterT_CFcharge(const siteconst_struct* sitec, soilprop_struct* sprop, epvar_struct* epv, wstate_struct* ws, wflux_struct* wf, groundwater_struct* GWS)
{
	int errorCode,GWlayer, CFlayer;
	double soilwDiffus_act;
	double dz0, VWC0, VWC0_sat, VWC0_fc, VWC0_wp, dz1, VWC1, VWC1_sat, VWC1_fc, VWC1_wp, soilw0, soilw1,soilw2, rVWC0, rVWC1;

	/*------------------------------------*/
	/* 0. initialization */
	errorCode=0;

	GWlayer = (int) sprop->GWlayer;
	CFlayer = (int) sprop->CFlayer;

	if (GWS->GWD_num)
	{
		/* if GW and CF are in the same layer, GW-zone fills the capillary zone in the same layer - special diffusion routine */
		if (GWlayer < N_SOILLAYERS && sprop->GWD != sitec->soillayer_depth[GWlayer-1] && sprop->GWD > sprop->CFD)
		{

			/* capillary zone is from GW to CF, the VWC can be calculated from the saturated GW part and the averaged out value */
			if (CFlayer == GWlayer)
				dz0 = sprop->GWD - sprop->CFD;
			else
				dz0 = sprop->GWD - sitec->soillayer_depth[GWlayer-1];
			
			dz1      = sitec->soillayer_depth[GWlayer] - sprop->GWD;
			
			VWC0_sat = sprop->VWCsat[GWlayer];
			VWC1_sat = sprop->VWCsat[GWlayer];

			VWC0_fc  = sprop->VWCsat[GWlayer];
			VWC1_fc  = sprop->VWCsat[GWlayer];

			VWC0_wp  = sprop->VWCwp[GWlayer];
			VWC1_wp  = sprop->VWCwp[GWlayer];

			VWC0 = (epv->VWC[GWlayer]*sitec->soillayer_thickness[GWlayer]-VWC1_sat*dz1)/(sitec->soillayer_thickness[GWlayer]-dz1);
			VWC1 = sprop->VWCsat[GWlayer];
			
			rVWC0    = (VWC0 - VWC0_wp) / (VWC0_fc- VWC0_wp);
			rVWC1    = (VWC1 - VWC1_wp) / (VWC1_fc- VWC1_wp);
			
		
			soilw0   = VWC0  * dz0 * water_density;	
			soilw1   = VWC1  * dz1 * water_density;

			soilw2   = VWC0  * (sprop->CFD - sitec->soillayer_depth[GWlayer-1]) * water_density;

			if ((soilw0 + soilw1 + soilw2) - ws->soilw[GWlayer] > CRIT_PRECwater)
			{
				printf("\n");
				printf("ERROR: in groundwaterT_CFcharge.c\n");
				errorCode = 1;
			}

			if (!errorCode && diffusCalc(sprop, dz0, VWC0, rVWC0, VWC0_sat, VWC0_fc, VWC0_wp, dz1, VWC1, rVWC0, VWC1_sat, VWC1_fc, VWC1_wp, &soilwDiffus_act))
			{
				printf("\n");
				printf("ERROR: diffusion() in groundwaterT_CFcharge.c\n");
				errorCode = 1;
			}

			if (fabs(soilwDiffus_act) < CRIT_PRECwater) soilwDiffus_act=0;
			if (soilwDiffus_act <= 0)
			{
				wf->GWdischarge[GWlayer] += -1 *soilwDiffus_act;
				ws->soilw[GWlayer]       += -1 *soilwDiffus_act;
				epv->VWC[GWlayer]         = ws->soilw[GWlayer] / (sitec->soillayer_thickness[GWlayer] * water_density);
			}
			else
			{
				printf("\n");
				printf("ERROR:  in groundwaterT_CFcharge.c\n");
				errorCode = 1;
			}

		}

		/*  if GW is below 10, but CF is in the bottom layer, GW-zone fills the capillary zone  - special diffusion routine */
		if (sprop->GWlayer == N_SOILLAYERS && sprop->CFlayer == N_SOILLAYERS-1)
		{
			
			/* capillary zone is from depth of soil system to CF, the VWC can be calculated from the saturated GW part and the averaged out value */
			dz0      = sitec->soillayer_depth[CFlayer] - sprop->CFD;

			VWC0     = epv->VWC[CFlayer];
			VWC0_sat = sprop->VWCsat[CFlayer];
			VWC0_fc  = sprop->VWCsat[CFlayer];
			VWC0_wp  = sprop->VWCwp[CFlayer];
			rVWC0    = epv->relVWCfc_wp[CFlayer];
			soilw0   = VWC0  * dz0 * water_density;
			
			dz1      = sitec->soillayer_depth[CFlayer] - sprop->CFD;
			VWC1     = sprop->VWCsat[CFlayer];
			
			VWC1_sat = sprop->VWCsat[CFlayer];
			VWC1_fc  = sprop->VWCsat[CFlayer];
			VWC1_wp  = sprop->VWCwp[CFlayer];
			rVWC1    = (VWC1 - VWC1_wp)/(VWC1_fc-VWC1_wp);
			soilw1   = VWC1  * dz1 * water_density;

			soilw2   = VWC0  * (sprop->CFD - sitec->soillayer_depth[GWlayer-1]) * water_density;

		
			if (!errorCode && diffusCalc(sprop, dz0, VWC0, rVWC0, VWC0_sat, VWC0_fc, VWC0_wp, dz1, VWC1, rVWC1, VWC1_sat, VWC1_fc, VWC1_wp, &soilwDiffus_act))
			{
				printf("\n");
				printf("ERROR: diffusion() in groundwaterT_CFcharge.c\n");
				errorCode = 1;
			}

			if (soilwDiffus_act <= 0)
			{
				wf->GWdischarge[CFlayer] += -1 *soilwDiffus_act;
				ws->soilw[CFlayer]       += -1 *soilwDiffus_act;
				epv->VWC[CFlayer]         = ws->soilw[CFlayer] / (sitec->soillayer_thickness[CFlayer] * water_density);
			}
			else
			{
				printf("\n");
				printf("ERROR:  in groundwaterT_CFcharge.c\n");
				errorCode = 1;
			}

		}

	}

	return (errorCode);
}
