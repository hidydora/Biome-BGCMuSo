/* 
multilayer_hydrolparams.c
Hidy 2010 - soil water potential, hydr. conductivity and hydr. diffusivity as a function of volumetric water content and
constants related to texture

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
#include "bgc_func.h"
#include "bgc_constants.h"

int multilayer_hydrolparams(const control_struct* ctrl, const siteconst_struct* sitec,  wstate_struct* ws, epvar_struct* epv)
{
	/* given a list of site constants and the soil water mass (kg/m2),
	this function returns the soil water potential (MPa)
	inputs:
	ws.soilw				     (kg/m2) water mass per unit area
	sitec.max_rootzone_depth     (m)     maximum depth of rooting zone               
	sitec.soil_b				 (DIM)   slope of log(psi) vs log(rwc)
	sitec.vwc_sat				 (DIM)   volumetric water content at saturation
	sitec.psi_sat			   	(MPa)   soil matric potential at saturation
	output:
	psi						 (MPa)   soil matric potential

	uses the relation:
	psi = psi_sat * (vwc/vwc_sat)^(-b)
	hydr_conduct = hydr_conduct_sat * (vwc/vwc_sat)^(2b+3)
	hydr_diffus = b* hydr_diffus * (-1*psi) * (vwc/vwc_sat)^(b+2)

	For further discussion see:
	Cosby, B.J., G.M. Hornberger, R.B. Clapp, and T.R. Ginn, 1984.  A     
	   statistical exploration of the relationships of soil moisture      
	   characteristics to the physical properties of soils.  Water Res.
	   Res. 20:682-690.
	
	Saxton, K.E., W.J. Rawls, J.S. Romberger, and R.I. Papendick, 1986.
		Estimating generalized soil-water characteristics from texture.
		Soil Sci. Soc. Am. J. 50:1031-1036.

	Balsamo et al 2009 - 
	A Revised Hydrology for the ECMWF Model - Verification from Field Site to Water Storage IFS - JHydromet.pdf

	Chen and Dudhia 2001 - 
	Coupling an Advanced Land Surface-Hydrology Model with the PMM5 Modeling System Part I - MonWRev.pdf


	*/

	int ok=1;

	/* day since water sress, end of water stress */
	double psi_act;
	double hydr_conduct_act;
	double hydr_diffus_act;
	double vwc_act;
	double pF_act;

	/* multilayer soil */
	double vwc_avg = 0.0;
	double psi_avg = 0.0;
	double hydr_conduct_avg = 0.0;
	double hydr_diffus_avg = 0.0;
	int layer;

	/* sitec parameters */
	double soil_b = sitec->soil_b;
	double vwc_sat = sitec->vwc_sat;
	double psi_sat = sitec->psi_sat;
	

	/* ***************************************************************************************************** */
	/* calculating vwc psi and hydr. cond. to every layer */


	for (layer=0 ; layer < N_SOILLAYERS-1; layer++)
	{

		/* convert kg/m2 --> m3/m2 --> m3/m3 */
		vwc_act = ws->soilw[layer] / (1000.0 * sitec->soillayer_thickness[layer]);

		/* psi, hydr_conduct and hydr_diffus ( Cosby et al.) from vwc ([1MPa=100m] [m/s] [m2/s] */
		psi_act = psi_sat * pow( (vwc_sat/vwc_act), soil_b);
		
		hydr_conduct_act = sitec->hydr_conduct_sat * pow(vwc_act/vwc_sat, 2*soil_b+3);
		
		hydr_diffus_act = (((soil_b * sitec->hydr_conduct_sat * (-100*psi_sat))) / vwc_sat) * 
							pow(vwc_act/vwc_sat, soil_b+2);

	
		/* pF from psi: cm from MPa */
		pF_act=log10(fabs(10000*psi_act));
	
		/* assign arrays in ecophsiological variable struct */
		epv->vwc[layer] = vwc_act;
		epv->psi[layer] = psi_act;
		epv->pF[layer] = pF_act;
        epv->hydr_conduct[layer] = hydr_conduct_act;
		epv->hydr_diffus[layer] = hydr_diffus_act;

	
		vwc_avg		     +=	vwc_act			* epv->soillayer_RZportion[layer];
		psi_avg			 +=	psi_act			* epv->soillayer_RZportion[layer];
		hydr_conduct_avg += hydr_conduct_act* epv->soillayer_RZportion[layer];
		hydr_diffus_avg  += hydr_diffus_act * epv->soillayer_RZportion[layer];

	}

	epv->vwc_avg			= vwc_avg;
	epv->psi_avg			= psi_avg;
	epv->hydr_conduct_avg   = hydr_conduct_avg;
	epv->hydr_diffus_avg    = hydr_diffus_avg;

	/* Hidy 2012 - bottom layer is special: infinite depth, saturated */
	if (ctrl->yday == 0)
	{
		epv->vwc[N_SOILLAYERS-1] = sitec->vwc_sat;
		epv->psi[N_SOILLAYERS-1] = sitec->psi_sat;
		epv->pF[N_SOILLAYERS-1]  = log10(fabs(10000*sitec->psi_sat));
		epv->hydr_conduct[N_SOILLAYERS-1] = sitec->hydr_conduct_sat;
		epv->hydr_diffus[N_SOILLAYERS-1] = sitec->hydr_diffus_sat;
	}


	return(!ok);
}

