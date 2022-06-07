/* 
multilayer_transpiration.c
Hidy 2011 - part-transpiration (regarding to the different layers of the soil) calculation based on the layer's soil water content

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Biome-BGCMuSo v6.0.
Copyright 2019, D. Hidy [dori.hidy@gmail.com]
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

int multilayer_transpiration(control_struct* ctrl, const siteconst_struct* sitec, epvar_struct* epv, const wstate_struct* ws, wflux_struct* wf)
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
	double ratio, soilw_trans_ctrl, transp_diff_SUM;
	double transp_diff = 0;
	
	int errflag=0;

	ratio=soilw_trans_ctrl=transp_diff=transp_diff_SUM=0;

	/* *****************************************************************************************************************/
	/* 1. PART-TRANSPIRATION: first approximation tanspiration from every soil layer equally */

	for (layer = epv->germ_layer; layer < epv->n_rootlayers; layer++)
	{
		/* root water uptake is be possible from the layers where root is located  */ 
		if (epv->n_rootlayers > 1)
		{
			if (epv->m_soilstress > 0)
			{
				ratio=(epv->m_soilstress_layer[layer] * epv->rootlength_prop[layer]) / epv->m_soilstress;
			}
			else
				ratio=0;
		}
		
		else 
			ratio = 1;

		wf->soilw_trans[layer] = wf->soilw_trans_SUM * ratio;
		
		
		soilw_trans_ctrl += wf->soilw_trans[layer];
		
	}

	/* extreme dry soil - no transpiration occurs */
	if (soilw_trans_ctrl == 0 && wf->soilw_trans_SUM != 0)
	{
		wf->soilw_trans_SUM = 0;
		/* notransp_flag: flag of WARNING writing in log file (only at first time) */
		if (!ctrl->notransp_flag) ctrl->notransp_flag = 1;
	}

	/* control */
	if (fabs(soilw_trans_ctrl - wf->soilw_trans_SUM) > CRIT_PREC)
	{
		printf("\n");
		printf("FATAL ERRROR: transpiration calculation error in multilayer_transpiration.c:\n");
		errflag=1;
		wf->soilw_trans_SUM = soilw_trans_ctrl;
	}



	return (errflag);
}



