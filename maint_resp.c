/* 
maint_resp.c
daily maintenance respiration

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Biome-BGC version 4.1.1
Copyright 2000, Peter E. Thornton
Numerical Terradynamics Simulation Group (NTSG)
School of Forestry, University of Montana
Missoula, MT 59812
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

int maint_resp(const cstate_struct* cs, const nstate_struct* ns,const epconst_struct* epc, const metvar_struct* metv,
		const siteconst_struct* sitec, cflux_struct* cf, epvar_struct* epv)
{
	/*
	maintenance respiration routine
	Uses reference values at 20 deg C and an empirical relationship between
	tissue N content and respiration rate given in:

	Ryan, M.G., 1991. Effects of climate change on plant respiration.
	Ecological Applications, 1(2):157-167.
	
	Uses the same value of Q_10 (2.0) for all compartments, leaf, stem, 
	coarse and fine roots.
	
	From Ryan's figures and regressions equations, the maintenance respiration
	in kgC/day per kg of tissue N is:
	mrpern = 0.218 (kgC/kgN/d)
	
	Leaf maintenance respiration is calculated separately for day and
	night, since the PSN routine needs the daylight value.
	
	Leaf and fine root respiration are dependent on phenology.
	*/
	
	int ok=1;
	double t1;
	double mrpern = 0.218;
	double exponent;
	double n_area_sun, n_area_shade, dlmr_area_sun, dlmr_area_shade;
	
	/* Hidy 2010 - calculate the tsoil exponenet regarding to froot_resp in multilayer soil */
	int layer;
	double tsoil, frootn_layer, livecrootn_layer;
	double froot_mr = 0;
	double livecroot_mr = 0;

	/* Hidy 2012 - using external data as temperature coefficient */ 
	double q10;
	q10 = Q10_VALUE;




	/* ********************************************************* */
	/* leaf day and night maintenance respiration when leaves on */
	if (cs->leafc)
	{
		t1 = ns->leafn * mrpern;
		
		/* leaf, day */
		exponent = (metv->tday - 20.0) / 10.0;
		cf->leaf_day_mr = t1 * pow(q10, exponent) * metv->dayl / 86400.0;

		/* for day respiration, also determine rates of maintenance respiration
		per unit of projected leaf area in the sunlit and shaded portions of
		the canopy, for use in the photosynthesis routine */
		/* first, calculate the mass of N per unit of projected leaf area
		in each canopy fraction (kg N/m2 projected area) */
		n_area_sun   = 1.0/(epv->sun_proj_sla * epc->leaf_cn);
		n_area_shade = 1.0/(epv->shade_proj_sla * epc->leaf_cn);
		/* convert to respiration flux in kg C/m2 projected area/day, and
		correct for temperature */
		dlmr_area_sun   = n_area_sun * mrpern * pow(q10, exponent);
		dlmr_area_shade = n_area_shade * mrpern * pow(q10, exponent);
		/* finally, convert from mass to molar units, and from a daily rate to 
		a rate per second */
		epv->dlmr_area_sun = dlmr_area_sun/(86400.0 * 12.011e-9);
		epv->dlmr_area_shade = dlmr_area_shade/(86400.0 * 12.011e-9);
		
		/* leaf, night */
		exponent = (metv->tnight - 20.0) / 10.0;
		cf->leaf_night_mr = t1 * pow(q10, exponent) * 
			(86400.0 - metv->dayl) / 86400.0;
	}
	else /* no leaves on */
	{
		cf->leaf_day_mr = 0.0;
		epv->dlmr_area_sun = 0.0;
		epv->dlmr_area_shade = 0.0;
		cf->leaf_night_mr = 0.0;
	}

	/* ********************************************************************/
	/* Hidy 2010 - calculate the tsoil exponenet regarding to froot_resp in multilayer soil */


	/* fine root maintenance respiration when fine roots on */
	/* ammended to consider only the specified n concentration,
	to avoid excessive MR with n-loading to fine roots */
	if (cs->frootc)
	{
		for (layer = 0; layer < epv->n_rootlayers; layer++)
		{
			tsoil = metv->tsoil[layer];
			
			frootn_layer = ns->frootn * epv->soillayer_RZportion[layer];
			exponent = (tsoil - 20.0) / 10.0;
			t1 = pow(q10, exponent);
			froot_mr += frootn_layer * mrpern * t1;
		}
		
	}
	else froot_mr = 0;/* no fine roots on */

	cf->froot_mr = froot_mr;

	/* TREE-specific fluxes */
	if (epc->woody)
	{
		/* live stem maintenance respiration */
		exponent = (metv->tavg - 20.0) / 10.0;
		t1 = pow(q10, exponent);
		cf->livestem_mr = ns->livestemn * mrpern * t1;

		/* live coarse root maintenance respiration */
		for (layer = 0; layer < epv->n_rootlayers; layer++)
		{
			tsoil = metv->tsoil[layer];

			livecrootn_layer = ns->livecrootn * epv->soillayer_RZportion[layer];
			
			exponent = (tsoil - 20.0) / 10.0;
			t1 = pow(q10, exponent);
			livecroot_mr += livecrootn_layer * mrpern * t1;
		}	
	}
	else livecroot_mr = 0;
	cf->livecroot_mr = livecroot_mr;
	
	return (!ok);
}
