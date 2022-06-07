/* 
multilayer_hydrolprocess.c
Hidy 2010 - calculation of soil water content layer by layer taking into account soil hydrological processes 
(precipitation, evaporation, runoff, percolation, diffusion)

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

int multilayer_hydrolprocess(const control_struct* ctrl, const siteconst_struct* sitec,  const epconst_struct* epc, epvar_struct* epv,
			wstate_struct* ws,  wflux_struct* wf)
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
	
	double diffus, percol, diffus_max, percol_max, soilw_hw; /* (kgH2O/m2/min) */

	double soilw_i1 = 0;
	double soilw_i0 = 0;
	double vwc_i1 = 0;
	double vwc_i0 = 0;

	int ok=1;
	int step = 0;


	double vwc_sat			= sitec->vwc_sat;
	double soil_b			= sitec->soil_b;
	double hydr_conduct_sat = sitec->hydr_conduct_sat;
	double hydr_diffus_sat  = sitec->hydr_diffus_sat;
	double psi_sat			= sitec->psi_sat;
	double toplayer_depth   = sitec->soillayer_depths[0];
	double soilw_sat_i0     = vwc_sat * sitec->soillayer_thickness[0] * water_density;


	/* *****************************/
	/* 1A. PRECIPITATION AND RUNOFF*/
	/* when the precipitation at the surface exceeds the max. infiltration rate, the excess water is put into surfacerunoff (Balsamo et al. 20008; Eq.(7)) */

	/* Dinamically changing time step: 
	no precipitation: 3-hours; small precipitation (<10mm): huge precipitation (>10mm) 10-min; */ 
	prcp = (wf->prcp_to_soilw + wf->snoww_to_soilw);

	if (prcp > 0)
	{
		if (prcp > 10)
		{
			discretization_level = 1;
		}
		else 
		{
			discretization_level = 10;
		}
	}
	else 
	{
		discretization_level = 180;
	}
	endstep = n_hour_in_day * (n_sec_in_min / discretization_level);

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
	soilw_hw = ws->soilw[0] * sitec->vwc_hw;

	/* evap_diff: control parameter to avoid negative soil water content (due to overestimated evaporation + dry soil) */
	evap_diff = ws->soilw[0] - wf->soilw_evap - soilw_hw;

	/* theoretical lower limit of water content: wilting point: if evap_diff less than 0, evaporation from the next layer  */
	if (evap_diff < 0)
	{
		wf->soilw_evap += evap_diff;
		if (ctrl->onscreen) printf("Warning: limited evaporation due to dry soil (multilayer_hydrolprocess.c)\n");
	}
	
	ws->soilw[0] -= wf->soilw_evap;



	/* ********************************/
	/* 1C. POND WATER: water stored on surface which can not infiltrated because of saturation */
	

	/* theoretical upper limit of water content: saturation value (amount above saturation is stored on the surface) */
	if (ws->soilw[0] > soilw_sat_i0)
	{
		ws->pond_water   = ws->soilw[0] - soilw_sat_i0;
		ws->soilw[0] = soilw_sat_i0;
	}

	epv->vwc[0]  = ws->soilw[0] / sitec->soillayer_thickness[0] / water_density;




	/* ********************************/
	/* ********************************/
	/* 2. PERCOLATION  AND DIFFUSION 
	
	 10min discretization level in order to taking account the change of the diffusivity and conductivity depending on soil moisture 
	 (after rain event the soil moisture rises suddenly causing high diffusivity and conductivity. If we assumed that these diffusivity 
	 and conductivity values were valid all the day we would overestimate the velocity diffusion and percolation process) */


	
	for (step = 0; step < endstep; step++)
	{
		
		/* calculate processes layer to layer (i0: actual layer, i1:deeper layer)  - EXCEPT OF THE BOTTOM LAYER */
 		for (layer=0 ; layer < N_SOILLAYERS-2 ; layer++)
		{
			if (layer == 0 && step > 0 && ws->pond_water > 0)
			{
				if ((soilw_sat_i0 - ws->soilw[layer]) < ws->pond_water)
				{
					ws->pond_water -= (soilw_sat_i0 - ws->soilw[layer]);
					ws->soilw[layer] = soilw_sat_i0;
				}
				else
				{
					ws->soilw[layer] += ws->pond_water;
					ws->pond_water = 0;
				}

				epv->vwc[layer]  = ws->soilw[0] / sitec->soillayer_thickness[0] / water_density;
			}
			
			
			soilw_i0 = ws->soilw[layer];
			soilw_i1 = ws->soilw[layer+1];
			vwc_i0  = epv->vwc[layer];
			vwc_i1	= epv->vwc[layer+1];

			/* actual soil water content at theoretical lower limit of water content: hygroscopic water content */
			soilw_hw = soilw_i0 * sitec->vwc_hw;
			
			/* -----------------------------*/
			/* conductivity coefficient - theoretical upper limit: saturation value */
			hydr_conduct = hydr_conduct_sat * pow(vwc_i0/vwc_sat, 2*(soil_b)+3);

			/* percolation flux - ten minute amount */
			percol = hydr_conduct * water_density * n_sec_in_min * discretization_level;
			
			/* limit of the percolation: hygroscopic water content */
			percol_max = soilw_i0 - soilw_hw;
			
			if (percol > percol_max) 
			{
				percol = percol_max;
				if (ctrl->onscreen) printf("Warning: limited percolation flux (multilayer_hydrolprocess.c)\n");
			}
		
			/* -----------------------------*/
			/* diffusivity coefficient */
			hydr_diffus = (((soil_b * hydr_conduct_sat * (-100*psi_sat))) / vwc_sat) * pow(vwc_i0/vwc_sat, soil_b+2);
			
			/* diffusion flux  */
			diffus = (((vwc_i1 - vwc_i0) / (sitec->delta_z[layer+1]-sitec->delta_z[layer])) * hydr_diffus) 
							* water_density * n_sec_in_min * discretization_level; 
	
			/* limit of the diffusion: balanced water content */
			if (diffus > 0)
			{
				diffus_max = soilw_i0 - (vwc_i1 / vwc_i0) * soilw_i0;
			}
			else
			{
				diffus_max = -1 * (soilw_i1 - (vwc_i0 / vwc_i1) * soilw_i1);
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
	

			epv->vwc[layer]   = ws->soilw[layer]   / sitec->soillayer_thickness[layer] / water_density;
			epv->vwc[layer+1] = ws->soilw[layer+1] / sitec->soillayer_thickness[layer+1] / water_density;
			
		
			wf->soilw_percolated[layer] += percol;
			wf->soilw_diffused[layer]   += diffus;


		}

		/* BOTTOM LAYER IS SPECIAL: only percolation is taking into account  - percolated water is net loss for the system /
			
		/* conductivity coefficient - theoretical upper limit: saturation value */		
		hydr_conduct = hydr_conduct_sat * pow(epv->vwc[layer]/vwc_sat, 2*(soil_b)+3);			
		if (hydr_conduct > hydr_conduct_sat) hydr_conduct = hydr_conduct_sat; 

		/* percolation flux - ten minute amount */
		percol = hydr_conduct * water_density * n_sec_in_min * discretization_level;
		
		/* state update */
		ws->soilw[layer] -= percol;
		epv->vwc[layer]  = ws->soilw[layer] / sitec->soillayer_thickness[layer] / water_density;

		wf->soilw_percolated[layer] += percol;
		wf->soilw_diffused[layer]   += 0;

		/* CONTROL - avoiding negative pools */
		if (ws->soilw[layer] < 0.0)       
		{
			printf("Fatal error: negative soil water content (multilayer_hydrolprocess.c)\n");
			ok=0;	
		}
	
	}
	
	if (ws->pond_water > 0)
	{
		printf("pond water is formed on soil surface (multilayer_hydrolprocess.c)\n");
	}

	return (!ok);
}



