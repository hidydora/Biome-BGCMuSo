/* 
soilstress_calculation.c
calculation of drought and saturation soil water content stress layer by layers 

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

int soilstress_calculation(soilprop_struct* sprop, const epconst_struct* epc, 
	                       epvar_struct* epv, wstate_struct* ws, wflux_struct* wf)
{
	int layer;
	double m_vwcR_layer, m_SWCstress_avg, m_Nstress_avg, layerSATfull;
		

	int errorCode = 0;
	m_SWCstress_avg = m_Nstress_avg = layerSATfull = 0;
	
	/* 1. Calculation SWC-stress (can occur only after emergence  - if root is presence) */
	if (epv->rootDepth)
	{		
		/*--------------------------------------------*/
		/* 1.1 SWC-stress calculation based on VWC   */
		if (epc->soilstress_flag == 0)
		{
			layerSATfull = 0;
			for (layer = 0; layer < N_SOILLAYERS; layer++)
			{
				/* DROUGHT STRESS */
				if (epv->VWC[layer] <= epv->VWC_SScrit1[layer])
				{
					/* if VWCratio_crit is set to -9999: no soilstress calculation -> m_SWCstress = 1 */
					if (epc->VWCratio_SScrit1 != DATA_GAP)
					{
						if (epv->VWC[layer] <= sprop->VWCwp[layer])
							m_vwcR_layer  = 0;
						else
							m_vwcR_layer  = pow((epv->VWC[layer] - sprop->VWCwp[layer])/(epv->VWC_SScrit1[layer] - sprop->VWCwp[layer]), sprop->curvature_SS);
					}	
					else
						m_vwcR_layer  = 1;
							
				}
				else
				{
					/* ANOXIC CONDITION STRESS */
					if (epv->VWC[layer] >= epv->VWC_SScrit2[layer])
					{
						/* if VWCratio_crit is set to -9999: no soilstress calculation -> m_SWCstress = 1 */
						if (epc->VWCratio_SScrit2 != DATA_GAP)
						{
							if (fabs(sprop->VWCsat[layer] - epv->VWC_SScrit2[layer]) > CRIT_PREC)
								m_vwcR_layer  = (sprop->VWCsat[layer] - epv->VWC[layer])/(sprop->VWCsat[layer] - epv->VWC_SScrit2[layer]);	
							else
								m_vwcR_layer  = 0;
						}
						else
							m_vwcR_layer  = 1;
					}
					/* NO STRESS */
					else
						m_vwcR_layer  = 1;
				}

				/* if all layers are saturated -> full stress, if not: lower limit for saturation: m_fullstress2 */
				if (epv->VWC[layer] >= epv->VWC_SScrit2[layer] || fabs(epv->VWC[layer] - epv->VWC_SScrit2[layer]) < CRIT_PRECwater)
				{
					if (m_vwcR_layer < epc->m_fullstress2) m_vwcR_layer = epc->m_fullstress2;
					layerSATfull += epv->rootlengthProp[layer];

				}
				
				epv->m_SWCstress_layer[layer] =  m_vwcR_layer;
				m_SWCstress_avg	 += epv->m_SWCstress_layer[layer] * epv->rootlengthProp[layer];
			}
	

		}
		/* 1.2 SWC-stress calculation based on transpiration demand-possibitiy  */
		else
		{
			if (epv->n_rootlayers)
			{
				for (layer = 0; layer < epv->n_rootlayers; layer++)
				{	
					if (wf->soilwTRPdemand[layer])
						epv->m_SWCstress_layer[layer] = wf->soilwTRP[layer] / wf->soilwTRPdemand[layer];
					else
						epv->m_SWCstress_layer[layer] = 1;

					/* control */
					if (epv->m_SWCstress_layer[layer] < 0 || epv->m_SWCstress_layer[layer] > 1) 
					{
						printf("\n");
						printf("ERROR: soilstress calculation error in multilayer_hydrolpocess.c:\n");
						errorCode=1;
					}
		
					m_SWCstress_avg	 += epv->m_SWCstress_layer[layer] * epv->rootlengthProp[layer];
				}
			}
			else
			{
				for (layer = 0; layer < N_SOILLAYERS; layer++) epv->m_SWCstress_layer[layer] = 1;
				m_SWCstress_avg = 1;
			}
		}
	}
	else
	{
		for (layer = 0; layer < N_SOILLAYERS; layer++) 
		{
				epv->m_SWCstress_layer[layer] = 1;
				ws->soilw_avail[layer] = 0;
		}

		m_SWCstress_avg = 1;
	}

	/* balus: if all layers are saturated: full limitation */
	if (fabs(layerSATfull - 1) < CRIT_PREC)
	{
		epv->m_SWCstress = 0;
		for (layer = 0; layer < N_SOILLAYERS; layer++) epv->m_SWCstress_layer[layer] =  0;
	}
	else
		epv->m_SWCstress = m_SWCstress_avg;

	/****************************************************************************************/
	/* 2. calculating cumulative SWC stress and extreme temperature effect */

		
	/* 2.1 calculating WATER STRESS DAYS regarding to the average soil moisture conditions */
	
	if (epv->m_SWCstress == 1) epv->SWCstressLENGTH = 0;
	if (epv->n_actphen > epc->n_emerg_phenophase)
	{
		epv->SWCstressLENGTH += (1 - epv->m_SWCstress);
		epv->cumSWCstress    += (1 - epv->m_SWCstress);
	}

	
	if (epv->SWCstressLENGTH < epc->SWCstressLENGTH_crit)
		epv->m_SWCstressLENGTH = 1;
	else
		epv->m_SWCstressLENGTH = epc->SWCstressLENGTH_crit/epv->SWCstressLENGTH;

	
	

	/****************************************************************************************/
	/* 3. N-stress based on immobilization ratio */

	if (epv->rootDepth)
		for (layer = 0; layer < N_SOILLAYERS; layer++) m_Nstress_avg += epv->IMMOBratio[layer] * epv->rootlengthProp[layer];
	else
		m_Nstress_avg = 1;

	if (1-m_Nstress_avg < CRIT_PREC) m_Nstress_avg = 1;
	
	if (epv->n_actphen > epc->n_emerg_phenophase) epv->cumNstress += (1-m_Nstress_avg);

	
	

	return (errorCode);
}