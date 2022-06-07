/* 
multilayer_hydrolprocess.c
Hidy 2010 - calculation of soil water content layer by layer taking into account soil hydrological processes 
(precipitation, evaporation, runoff, percolation, diffusion)

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
BBGC MuSo 2.3
Copyright 2014, D. Hidy
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

int multilayer_hydrolprocess(const control_struct* ctrl, const siteconst_struct* sitec, epvar_struct* epv, wstate_struct* ws, wflux_struct* wf)
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
	double prcp, runoff, prcp_to_soil;
	double evap_diff;
	double first_part;
	double second_part;

	/* discretization level of percolation and diffusion calculation */
	double discretization_level, endstep;
	
	/* diffusion and percolation calculation: second -> daily value */
	double hydr_conduct;  /* hydrological conduction coefficient (m/s) */
	double hydr_diffus;	  /* hydrological diffusion coefficient (m2/s) */
	
	double diffus, percol, diffus_max, diffus_max0, diffus_max1, percol_max, percol_max0, percol_max1;
	double soilw_hw0, soilw_hw1, soilw_sat0, soilw_sat1;  /* (kgH2O/m2/min) */
	double soillayer_thickness_i0, soillayer_thickness_i1, soillayer_midpoint_i0, soillayer_midpoint_i1;

	double soilw_i1 = 0;
	double soilw_i0 = 0;
	double vwc_i1 = 0;
	double vwc_i0 = 0;
	double vwc = 0;

	int ok=1;
	int step = 0;
    int n_min_in_hour = n_sec_in_min;

	double vwc_sat			= sitec->vwc_sat;
	double soil_b			= sitec->soil_b;
	double hydr_conduct_sat = sitec->hydr_conduct_sat;
	double psi_sat			= sitec->psi_sat;

	double vwc_crit1=0.20;
	double vwc_crit2=0.25;
	double vwc_crit3=0.30;
	double vwc_crit4=0.35;
	/* *****************************/
	/* 1A. PRECIPITATION AND RUNOFF*/
	/* when the precipitation at the surface exceeds the max. infiltration rate, the excess water is put into surfacerunoff (Balsamo et al. 20008; Eq.(7)) */

	/* Dinamically changing time step: 
	no precipitation: 3-hours; small precipitation (<10mm): huge precipitation (>10mm) 10-min; */ 
	prcp = (wf->prcp_to_soilw + wf->snoww_to_soilw);


	/* if the precipitation is greater than critical amount a fixed part of prcp is lost due to runoff (based on Campbell and Diaz, 1988) */
	if (prcp > CAMPBELL_PARAM * sitec->runoff_param)  
	{
		first_part = prcp - CAMPBELL_PARAM * sitec->runoff_param;
		second_part = prcp + 4*CAMPBELL_PARAM * sitec->runoff_param;
		runoff = (first_part*first_part)/second_part;
	}
	else
	{
		runoff = 0;
	}

	wf->prcp_to_runoff = runoff;

	/* amount of prcp after runoff */
	prcp_to_soil = prcp - runoff;

	ws->soilw[0] += prcp_to_soil;

	/* ********************************************/
	/* 1B. EVAPORATION */
	

	/* actual soil water content at theoretical lower limit of water content: hygroscopic water content */
	soilw_hw0 = sitec->vwc_hw * sitec->soillayer_thickness[0] * water_density;

	/* evap_diff: control parameter to avoid negative soil water content (due to overestimated evaporation + dry soil) */
	evap_diff = ws->soilw[0] - wf->soilw_evap - soilw_hw0;

	/* theoretical lower limit of water content: hygroscopic water content. */
	if (evap_diff < 0)
	{
		wf->soilw_evap += evap_diff;
		if (ctrl->onscreen) printf("Limited evaporation due to dry soil (multilayer_hydrolprocess.c)\n");
	}
	
	ws->soilw[0] -= wf->soilw_evap;



	/* ********************************/
	/* 1C. POND WATER: water stored on surface which can not infiltrated because of saturation */
	
	/* theoretical upper limit of water content: saturation value (amount above saturation is stored on the surface) */
	soilw_sat0     = vwc_sat * sitec->soillayer_thickness[0] * water_density;

	if (ws->soilw[0] > soilw_sat0)
	{
		ws->pond_water   += ws->soilw[0] - soilw_sat0;
		ws->soilw[0] = soilw_sat0;
	}

	epv->vwc[0]  = ws->soilw[0] / sitec->soillayer_thickness[0] / water_density;

	/* ********************************/
	/* 1D. DISCRETIZATION LEVEL: */


	if ((epv->vwc[0] < vwc_crit1 && epv->vwc_avg < vwc_crit1))
	{
		discretization_level = 180 ; 
	}
	else
	{
		if ((epv->vwc[0] > vwc_crit4 || epv->vwc_avg > vwc_crit4))
		{
			discretization_level = 10; 
		}
		else
		{
			if ((epv->vwc[0] > vwc_crit3 || epv->vwc_avg > vwc_crit3))
			{
				discretization_level = 10; 
			}
			else
			{
				if ((epv->vwc[0] > vwc_crit2 || epv->vwc_avg > vwc_crit2))
				{
					discretization_level = 30; 
				}
				else
				{
					discretization_level = 60; 
				}
			}
		}

	}
 
	endstep = n_hour_in_day * (n_min_in_hour / discretization_level);


	/* ********************************/
	/* ********************************/
	/* 2. PERCOLATION  AND DIFFUSION 
	
	 10min discretization level in order to taking account the change of the diffusivity and conductivity depending on soil moisture 
	 (after rain event the soil moisture rises suddenly causing high diffusivity and conductivity. If we assumed that these diffusivity 
	 and conductivity values were valid all the day we would overestimate the velocity diffusion and percolation process) */


	
	for (step = 0; step < endstep; step++)
	{
		
		/* calculate processes layer to layer (i0: actual layer, i1:deeper layer)  - EXCEPT OF THE BOTTOM LAYER */
 		for (layer=0 ; layer < N_SOILLAYERS-1 ; layer++)
		{
			soilw_i0				= ws->soilw[layer];
			soilw_i1				= ws->soilw[layer+1];
			vwc_i0					= epv->vwc[layer];
			vwc_i1					= epv->vwc[layer+1];
			soillayer_midpoint_i0	= sitec->soillayer_midpoint[layer];
			soillayer_midpoint_i1	= sitec->soillayer_midpoint[layer+1];
			soillayer_thickness_i0  = sitec->soillayer_thickness[layer];
			soillayer_thickness_i1  = sitec->soillayer_thickness[layer+1];

			/* actual soil water content at theoretical lower and upper limit of water content: hygroscopic water content and saturation */
			soilw_hw0  = sitec->vwc_hw * soillayer_thickness_i0 * water_density;
			soilw_hw1  = sitec->vwc_hw * soillayer_thickness_i1 * water_density;
			soilw_sat0 = sitec->vwc_sat * soillayer_thickness_i0 * water_density;
			soilw_sat1 = sitec->vwc_sat * soillayer_thickness_i1 * water_density;


			/* only in the first layer: in case of pont water possible water flux from pond to the first layer */
			if (layer == 0 && ws->pond_water > 0)
			{
				if ((soilw_sat0 - soilw_i0) <= ws->pond_water)
				{
					ws->pond_water -= (soilw_sat0 - soilw_i0);
					soilw_i0 = soilw_sat0;
				}
				else
				{
					soilw_i0       += ws->pond_water;
					ws->pond_water = 0;
				}

				ws->soilw[0]     = soilw_i0;
				epv->vwc[layer]  = soilw_i0 / sitec->soillayer_thickness[0] / water_density;
				vwc_i0           = epv->vwc[layer];  
			}
			
			/* CONTROL - avoiding negative pools */
			if (ws->soilw[layer] < 0.0)       
			{
				printf("Fatal error: negative soil water content (multilayer_hydrolprocess.c)\n");
				ok=0;	
			}

			
			/* -----------------------------*/
			/* conductivity coefficient - theoretical upper limit: saturation value */
			hydr_conduct = hydr_conduct_sat * pow(vwc_i0/vwc_sat, 2*(soil_b)+3);


			/* percolation flux - ten minute amount */
			percol = hydr_conduct * water_density * n_sec_in_min * discretization_level;
			
			/* limit of the percolation: hygroscopic water content of actual layer and saturation of deeper layer */
			percol_max0 = soilw_i0 - soilw_hw0;
			percol_max1 = soilw_sat1 - soilw_i1;

			if (percol_max0 < percol_max1) percol_max = percol_max0;
				else percol_max = percol_max1;
			
			if (percol > 0 && percol > percol_max) 
			{
				percol = percol_max;
				if (ctrl->onscreen && !ctrl->GWD_flag) printf("Limited percolation flux (multilayer_hydrolprocess.c)\n");
			}

		
			/* -----------------------------*/
			/* diffusivity coefficient */
		
			vwc = (soillayer_thickness_i0 * vwc_i0 + soillayer_thickness_i1 * vwc_i1) / (soillayer_thickness_i0 + soillayer_thickness_i1) ;
			
			    
			hydr_diffus = (((soil_b * hydr_conduct_sat * (-100*psi_sat))) / vwc_sat) * pow(vwc/vwc_sat, soil_b+2);
			
			/* diffusion flux  */
			diffus = (((vwc_i0 - vwc_i1) / (soillayer_midpoint_i1-soillayer_midpoint_i0)) * hydr_diffus) 
							* water_density * n_sec_in_min * discretization_level; 
	
			/* limit of the diffusion: HW content of the source layer, saturation of the sink layer*/
			if (diffus > 0)
			{
				diffus_max0 = soilw_i0 - percol - soilw_hw0;
				diffus_max1 = soilw_sat1 - percol - soilw_i1;
				if (diffus_max0 < diffus_max1) diffus_max = diffus_max0;
					else diffus_max = diffus_max1;
				if (diffus > diffus_max) 
				{
					diffus = diffus_max;
					if (ctrl->onscreen && !ctrl->GWD_flag) printf("Limited diffusion flux (multilayer_hydrolprocess.c)\n");
				}
			}
			else
			{
				diffus_max0 = soilw_i1 - soilw_hw0;
				diffus_max1 = soilw_sat0 - soilw_i0;
				if (diffus_max0 < diffus_max1) diffus_max = diffus_max0;
					else diffus_max = diffus_max1;

				if (fabs(diffus) > diffus_max) 
				{
					diffus = -1 * diffus_max;
					if (ctrl->onscreen && !ctrl->GWD_flag) printf("Limited diffusion flux (multilayer_hydrolprocess.c)\n");
				}
			}

			if (fabs(diffus) > fabs(diffus_max)) 
			{
				diffus = diffus_max;
			}
			
			/* -----------------------------*/	
			/* intermediate variables state update */
			ws->soilw[layer]    = soilw_i0 - diffus - percol;
			ws->soilw[layer+1]  = soilw_i1 + diffus + percol;

			/* CONTROL - avoiding negative pools */
			if (ws->soilw[layer] < 0.0)       
			{
				printf("Fatal error: negative soil water content (multilayer_hydrolprocess.c)\n");
				ok=0;	
			}


			epv->vwc[layer]   = ws->soilw[layer]   / soillayer_thickness_i0 / water_density;
			epv->vwc[layer+1] = ws->soilw[layer+1] / soillayer_thickness_i1 / water_density;

			/* CONTROL - unrealistic VWC content (higher than saturation value) */
			if (epv->vwc[layer] > sitec->vwc_sat)       
			{
				if (epv->vwc[layer] - sitec->vwc_sat > 0.001)       
				{
					printf("Fatal error: soil water content is higher than saturation value (multilayer_hydrolprocess.c)\n");
					ok=0;	
				}
				else
				{
					ws->deeppercolation_snk += epv->vwc[layer] - sitec->vwc_sat;
					epv->vwc[layer] = sitec->vwc_sat;
					ws->soilw[layer] = epv->vwc[layer] * soillayer_thickness_i0 * water_density;
				}
			}
			
		
			wf->soilw_percolated[layer] += percol;
			wf->soilw_diffused[layer]   += diffus;

			/* BOTTOM LAYER IS SPECIAL: percolated water is net loss for the system, water content does not change */
			if (layer == N_SOILLAYERS-2 && sitec->gwd_act > sitec->soillayer_depth[N_SOILLAYERS-1])
			{
				epv->vwc[layer+1]  = sitec->vwc_fc;
				ws->soilw[layer+1] = sitec->vwc_fc * (sitec->soillayer_thickness[layer+1]) * 1000.0;
			}

		}

	}
	
	/* ********************************/
	/* 3. POND WATER EVAPORATION: water stored on surface which can not infiltrated because of saturation */
	if (ws->pond_water > 0)
	{
		printf("pond water on soil surface (multilayer_hydrolprocess.c)\n");
		if (wf->soilw_evap < ws->pond_water)
			wf->pondw_evap = wf->soilw_evap;
		else 
			wf->pondw_evap = ws->pond_water;
	}

	return (!ok);
}



