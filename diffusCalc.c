 /*
diffusCalc.c
Calculation of diffusion flux between two soil layers

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
#include <math.h>
#include <malloc.h>
#include "ini.h"
#include "bgc_struct.h"
#include "bgc_func.h"
#include "bgc_constants.h"
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

int diffusCalc(const soilprop_struct* sprop, double dz0, double VWC0, double rVWC0, double VWC0_sat, double VWC0_fc, double VWC0_wp, 
	                                         double dz1, double VWC1, double rVWC1, double VWC1_sat, double VWC1_fc, double VWC1_wp, double* soilwDiffus)
{

	int errorCode=0;
	double ESWi0, ESWi1, THETi0, THETi1, inner, DBAR, GRAD, FLOW;
	double dz0_cm, dz1_cm, m_to_cm, soilw0, soilw1, soilw_sat0, soilw_sat1, soilwDiffus_act;
	double rVWC_limit, VWC0_limit, VWC1_limit, soilw0_limit, soilw1_limit;	

	dz0_cm = dz0 * 100;
	dz1_cm = dz1 * 100;
	m_to_cm = 100;

	rVWC_limit=(rVWC0+rVWC1)/2;
	VWC0_limit=rVWC_limit*(VWC0_fc-VWC0_wp)+VWC0_wp;
	VWC1_limit=rVWC_limit*(VWC1_fc-VWC1_wp)+VWC1_wp;
	soilw0_limit=VWC0_limit * dz0 * water_density;
	soilw1_limit=VWC1_limit * dz1 * water_density;

	soilw0 = VWC0 * dz0 * water_density;
	soilw1 = VWC1 * dz1 * water_density;

	/* the plant-extractable soil water */
	ESWi0 = (VWC0_fc - VWC0_wp);
	ESWi1 = (VWC1_fc - VWC1_wp);

	
	/* interation for determine diffusion 	*/
	THETi0 = MIN(VWC0 - VWC0_wp, ESWi0);
	THETi1 = MIN(VWC1 - VWC1_wp, ESWi1);

	
	THETi0 = MAX(THETi0,   0);
	THETi1 = MAX(THETi1,   0);

	inner  = (THETi0 * dz0_cm + THETi1 * dz1_cm) /(dz0_cm+dz1_cm);
	DBAR   = sprop->p1diffus_tipping * exp(sprop->p2diffus_tipping * inner);
	DBAR   = MIN(DBAR, sprop->p3diffus_tipping);

	inner  = (ESWi0 * dz0_cm+ ESWi1 * dz1_cm)/((dz0_cm+dz1_cm));
	GRAD   = ((THETi1/ESWi1 - THETi0/ESWi0) * inner)/((dz0_cm+dz1_cm) * 0.5);
			
	FLOW   = DBAR * GRAD;

	if (fabs(FLOW) > CRIT_PREC)
		soilwDiffus_act = -1*(FLOW / m_to_cm) * water_density;
	else
		soilwDiffus_act = 0;
	
	/* tipping diffusion limitation */	
	soilw_sat0 = VWC0_sat * dz0   * water_density;
	soilw_sat1 = VWC1_sat * dz1 * water_density;

	/* control to remain balance */
	if (soilwDiffus_act != 0)
	{
		if (soilwDiffus_act > 0)
		{
			if (soilw0  - soilwDiffus_act < soilw0_limit)
				soilwDiffus_act = soilw0 - soilw0_limit;
		}
		else
		{
			if (soilw1  + soilwDiffus_act < soilw1_limit)
				soilwDiffus_act = soilw0_limit - soilw1_limit;
		}
	}

	soilw0  -= soilwDiffus_act;
			
	/* control to avoid oversaturation*/
	if (soilw0 > soilw_sat0)
	{
		soilwDiffus_act += soilw0 - soilw_sat0;
	}
			
	soilw1 += soilwDiffus_act;
			
	/* control to avoid oversaturation*/ 
	if (soilw1 > soilw_sat1)
	{
		soilwDiffus_act -= soilw1 - soilw_sat1;
	}

	
			
	VWC0  =  soilw0 / dz0 / water_density;
	VWC1  =  soilw1 / dz1 / water_density;
		
	*soilwDiffus = soilwDiffus_act;
	




	return (errorCode);

}
