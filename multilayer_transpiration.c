/* 
multilayer_transpiration.c
Hidy 2011 - part-transpiration (regarding to the different layers of the soil) calculation based on the layer's soil water content

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Biome-BGC version vHD2
*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <malloc.h>
#include "bgc_struct.h"
#include "bgc_constants.h"
#include "bgc_func.h"    

int multilayer_transpiration(const control_struct* ctrl, const siteconst_struct* sitec, const epconst_struct* epc,epvar_struct* epv, 
							 wstate_struct* ws, wflux_struct* wf)
{
	/* given a list of site constants and the soil water mass (kg/m2),
	this function returns the soil water potential (MPa)
	inputs:


	For further discussion see:
	Cosby, B.J., G.M. Hornberger, R.B. Clapp, and T.R. Ginn, 1984.     

	Balsamo et al 2009 - 
	A Revised Hydrology for the ECMWF Model - Verification from Field Site to Water Storage IFS - JHydromet.pdf

	Chen and Dudhia 2001 - 
	Coupling an Advanced Land Surface-Hydrology Model with the PMM5 Modeling System Part I - MonWRev.pdf*/
	

	/* internal variables */
	int layer;
	double soilw_hw;
	double soilw_trans_control;
	double transp_diff;
	int ok=1;
	double transp_diff_SUM = 0;



	/* *****************************************************************************************************************/
	/* 1. PART-TRANSPIRATION: first approximation tanspiration from every soil layer equally */
	

	for (layer = 0; layer < N_SOILLAYERS-1; layer++)
	{
		/* actual soil water content at theoretical lowe limit of water content: hygroscopic water point */
		soilw_hw = ws->soilw[layer] * sitec->vwc_hw;

		/* root water uptake is be possible from the layers where root is located  */
		if (layer < epv->n_rootlayers)
		{

			/* if stomata is not closed - root water uptake is divided between soil layers where enough soil moisture is available */
			if (epv->m_soilprop > 0)
			{
				wf->soilw_trans[layer] = wf->soilw_trans_SUM * (epv->m_soilprop_layer[layer] / epv->m_soilprop) * epv->soillayer_RZportion[layer];
			}
			/* if stomata is closed -> only cuticular water exchange - root water uptake from the top soil layer */
			else 
			{	
				wf->soilw_trans[layer] = wf->soilw_trans_SUM * epv->soillayer_RZportion[layer];
			}
		}
		else wf->soilw_trans[layer] = 0;


		/* transp_diff: control parameter to avoid negative soil water content (due to overestimated transpiration + dry soil) */
		transp_diff = ws->soilw[layer] - wf->soilw_trans[layer] - soilw_hw;

		/* theoretical lower limit of water content: wilting point: if transp_diff less than 0, limited transpiration flux  */
		if (transp_diff < 0)
		{
			wf->soilw_trans[layer] += transp_diff;
			transp_diff_SUM += transp_diff;
			if (ctrl->onscreen) printf("Warning: limited transpiration due to dry soil (multilayer_transpiration.c)\n");
		}

		ws->soilw[layer] -= wf->soilw_trans[layer];
	}

	wf->soilw_trans_SUM += transp_diff_SUM;

	/* no transpiration from the bottom layer (deeper than 4m */
	wf->soilw_trans[N_SOILLAYERS-1] = 0;

	/* control */
	soilw_trans_control = 0;
	for (layer = 0; layer < N_SOILLAYERS; layer++)
	{
		soilw_trans_control += wf->soilw_trans[layer];
	}

	if (fabs(soilw_trans_control - wf->soilw_trans_SUM) > 0.000001)
	{
		printf("FATAL ERRROR: transpiration calculation error in multilayer_transpiration.c:\n");
		ok=0;
	}




	return (!ok);
}



