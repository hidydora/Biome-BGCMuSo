/* 
groundwater.c
calculate the effect of gound water depth in the soil

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Biome-BGCMuSo v6.2.
Copyright 2020, D. Hidy [dori.hidy@gmail.com]
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



int groundwaterTIPPING(const control_struct* ctrl, const siteconst_struct* sitec, soilprop_struct* sprop, epvar_struct* epv, 
	            wstate_struct* ws, wflux_struct* wf, groundwater_struct* gws)
{
	int layerSAT,errorCode,layer,layerLOW,md, year;
	double soilw_fc,soilw_sat;
	double GWdistM, GWdistB, GWboundL,GWboundU,critVWCdiff,ratio;

	

	layer=layerLOW=layerSAT=errorCode=0;
	GWdistM=GWdistB=ratio=0;

	/* critical VWC-difference */
	critVWCdiff = 0.01;
	md = gws->mgmdGWD-1;
	year = ctrl->simstartyear + ctrl->simyr;

	/* initialization */
	sprop->CapillFringe_act = 0;
	for (layer = 0; layer < N_SOILLAYERS; layer++) sprop->GWeff[layer]=0;
	


	/* if gound water depth information is available using in multilayer calculation */
	if (gws->GWD_num != 0)	
	{	
		if (md >= 0 && year == gws->GWyear_array[md] && ctrl->month == gws->GWmonth_array[md] && ctrl->day == gws->GWday_array[md])
			sprop->GWD = gws->GWdepth_array[md];
		else
		{
			sprop->GWD = 100;
			for (layer = 0; layer < N_SOILLAYERS-1; layer++) sprop->VWCfc[layer] = sprop->VWCfc_base[layer];
		}

		/* GW above the surface */
		if (sprop->GWD < 0)
		{
			wf->GW_to_pondw      = -1*sprop->GWD*1000;
			sprop->GWD           = 0;
		}

		/* define GWlayer and actual CF */
		sprop->GWlayer=DATA_GAP;
		
		layer = N_SOILLAYERS-1;
		while (sprop->GWlayer == DATA_GAP && layer >= 0)
		{
			GWboundL = sitec->soillayer_depth[layer];
			if (layer == 0)
				GWboundU  = -1*CRIT_PREC;
			else
				GWboundU  = sitec->soillayer_depth[layer-1];
				
			/* if groundwater table is in actual layer (above lower boundary): GWlayer = actual layer, lower layers are charged */
			if (sprop->GWD <= GWboundL && sprop->GWD > GWboundU) sprop->GWlayer=(double) layer;
			layer -= 1;
		}
		if (sprop->GWlayer != DATA_GAP) sprop->CapillFringe_act = sprop->CapillFringe[layer];


							
		/*  METHOD1: increasing FC values */

		/* calculate processes while layer is enough close to groundwater table */
		layer = N_SOILLAYERS-1;
		while (GWdistM <= sprop->CapillFringe_act && layer >= 0)
		{
			soilw_fc     = sprop->VWCfc[layer]   * sitec->soillayer_thickness[layer] * water_density;

			/* if water table (GW) is closer than a critical distance (200 cm), the effect of GW is noticable: 
					higher FC values - less holding capacity (SAT-FC) 
					plus water from groundwater table */

			GWdistM   = sprop->GWD - sitec->soillayer_midpoint[layer];
			GWboundL = sitec->soillayer_depth[layer];
			
			if (layer == 0)
				GWboundU  = -1*CRIT_PREC;
			else
				GWboundU  = sitec->soillayer_depth[layer-1];
				
			if (GWdistM <= sprop->CapillFringe_act)
			{
				/* if groundwater table is in actual layer (above lower boundary): GWlayer = actual layer, lower layers are charged */
				if (sprop->GWD <= GWboundL && sprop->GWD > GWboundU) 
				{
					/* soil layers below the groundwater table are saturated - net water gain from soil system */
					for (layerSAT = 1; layer+layerSAT < N_SOILLAYERS; layerSAT++)
					{
						soilw_sat                             = sprop->VWCsat[layer+layerSAT] * sitec->soillayer_thickness[layer+layerSAT] * water_density;
							
						if (soilw_sat - ws->soilw[layer+layerSAT] > 0)
							wf->GWdischarge[layer+layerSAT]    += (soilw_sat - ws->soilw[layer+layerSAT]);
						else
							wf->GWrecharge[layer+layerSAT]      += (ws->soilw[layer+layerSAT] - soilw_sat);

						ws->soilw[layer+layerSAT]             = soilw_sat;
						epv->VWC[layer+layerSAT]              = sprop->VWCsat[layer+layerSAT];
						sprop->VWCfc[layer+layerSAT]          = sprop->VWCsat[layer+layerSAT];
					}
				}
				
				/* GWeff is calculated if lower layer contains GW table and it has already charged */
				if (layer == N_SOILLAYERS-1 || 
					(layer < N_SOILLAYERS-1 && (sprop->VWCfc[layer+1]-epv->VWC[layer+1]) < critVWCdiff && sprop->VWCfc[layer+1] > sprop->VWCfc_base[layer+1]))
				{
					/* if groundwater table is below the midpoint -> GWdist is positive -> GWeff is less than 1 */
					if (GWdistM > 0)
						sprop->GWeff[layer] = 1-GWdistM/sprop->CapillFringe_act;
					else
						sprop->GWeff[layer] = 1;

					/* higher FC values */
					sprop->VWCfc[layer] = sprop->VWCfc_base[layer] + sprop->GWeff[layer] * (sprop->VWCsat[layer]-sprop->VWCfc_base[layer]);
					soilw_fc            = sprop->VWCfc[layer]   * sitec->soillayer_thickness[layer] * water_density;

					/* plus water from groundwater table */
					if (epv->VWC[layer] < sprop->VWCfc[layer]) 
					{	
						if (sprop->GWlayer == (double) layer)
							wf->GWdischarge[layer] += (soilw_fc - ws->soilw[layer]);
						else
							wf->GWdischarge[layer] += (soilw_fc - ws->soilw[layer]) * sprop->drain_coeff[layer];

						ws->soilw[layer]         += wf->GWdischarge[layer];
						epv->VWC[layer]           = ws->soilw[layer] / (sitec->soillayer_thickness[layer] * water_density);
					}
				}
			}
			else
			{
				sprop->VWCfc[layer] = sprop->VWCfc_base[layer];
			}
			layer = layer-1;
		}

		/* special case: GWD on the ground */
		if (sprop->GWD == 0)
		{
			soilw_sat  = sprop->VWCsat[0] * sitec->soillayer_thickness[0] * water_density;
			wf->GWdischarge[0]    += (soilw_sat - ws->soilw[0]);
			ws->soilw[0]             = soilw_sat;
			epv->VWC[0]              = sprop->VWCsat[0];
			sprop->VWCfc[0]          = sprop->VWCsat[0];
		}
	
	}
	else
		sprop->GWD = DATA_GAP;

		
	return (errorCode);
}
