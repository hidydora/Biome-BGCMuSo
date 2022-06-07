/* 
canopy_et.c
A single-function treatment of canopy evaporation and transpiration
fluxes.  

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

int canopy_et(const metvar_struct* metv, epvar_struct* epv, wflux_struct* wf)
{
	int ok=1;

	double tday;
	double tmin;
	double dayl;
	double vpd;
	double canopy_w;
	
	double e, cwe, t, trans, trans_sun, trans_shade, e_dayl,t_dayl;
	

	pmet_struct pmet_in;

	/* assign variables that are used more than once */
	tday =      metv->tday;
	tmin =      metv->tmin;
	vpd  =      0; // Hidy 2012 - error correction
	vpd =       metv->vpd;
	dayl =      metv->dayl;
    canopy_w =  wf->prcp_to_canopyw;
	
	
        cwe = trans = 0.0;
	/* Assign values in pmet_in that don't change */
	pmet_in.ta = tday;
	pmet_in.pa = metv->pa;
	pmet_in.vpd = vpd;
	
	/* Canopy evaporation, if any water was intercepted */
	/* Calculate Penman-Monteith evaporation, given the canopy conductances to
	evaporated water and sensible heat.  Calculate the time required to 
	evaporate all the canopy water at the daily average conditions, and 
	subtract that time from the daylength to get the effective daylength for
	transpiration. */
	if (canopy_w)
	{
		/* assign appropriate resistance and radiation for pmet_in */
		pmet_in.rv = 1.0/epv->gc_e_wv;
		pmet_in.rh = 1.0/epv->gc_sh;
		pmet_in.irad = metv->swabs;
		
		/* call penman-monteith function, returns e in kg/m2/s */
		if (penmon(&pmet_in, 0, &e))
		{
			printf("Error: penmon() for canopy evap... \n");
			ok=0;
		}
		
		/* calculate the time required to evaporate all the canopy water */
		e_dayl = canopy_w/e;
		
		if (e_dayl > dayl)  
		{
			/* day not long enough to evap. all int. water */
			trans = 0.0;    /* no time left for transpiration */
			cwe = e * dayl;   /* daylength limits canopy evaporation */
		}
		else                
		{
			/* all intercepted water evaporated */
			cwe = canopy_w;
			
			/* adjust daylength for transpiration */
			t_dayl = dayl - e_dayl;
			 
			/* calculate transpiration using adjusted daylength */
			/* first for sunlit canopy fraction */
			pmet_in.rv = 1.0/epv->gl_t_wv_sun;
			pmet_in.rh = 1.0/epv->gl_sh;
			pmet_in.irad = metv->swabs_per_plaisun;
			if (penmon(&pmet_in, 0, &t))
			{
				printf("Error: penmon() for adjusted transpiration... \n");
				ok=0;
			}
			trans_sun = t * t_dayl * epv->plaisun;
			
			/* next for shaded canopy fraction */
			pmet_in.rv = 1.0/epv->gl_t_wv_shade;
			pmet_in.rh = 1.0/epv->gl_sh;
			pmet_in.irad = metv->swabs_per_plaishade;
			if (penmon(&pmet_in, 0, &t))
			{
				printf("Error: penmon() for adjusted transpiration... \n");
				ok=0;
			}
			trans_shade = t * t_dayl * epv->plaishade;
			trans = trans_sun + trans_shade;
		}
	}    /* end if canopy_water */
	
	else /* no canopy water, transpiration with unadjusted daylength */
	{
		/* first for sunlit canopy fraction */
		pmet_in.rv = 1.0/epv->gl_t_wv_sun;
		pmet_in.rh = 1.0/epv->gl_sh;
		pmet_in.irad = metv->swabs_per_plaisun;
		if (penmon(&pmet_in, 0, &t))
		{
			printf("Error: penmon() for adjusted transpiration... \n");
			ok=0;
		}
		trans_sun = t * dayl * epv->plaisun;
		
		/* next for shaded canopy fraction */
		pmet_in.rv = 1.0/epv->gl_t_wv_shade;
		pmet_in.rh = 1.0/epv->gl_sh;
		pmet_in.irad = metv->swabs_per_plaishade;
		if (penmon(&pmet_in, 0, &t))
		{
			printf("Error: penmon() for adjusted transpiration... \n");
			ok=0;
		}
		trans_shade = t * dayl * epv->plaishade;
		trans = trans_sun + trans_shade;
		

		
	}
	/* Hidy 2011 - multilayer soil model: transpiration is calculated in multilayer_transpiration.c 
	original: wf->soilw_trans = trans; */	
	if (epv->m_soilprop > 0)
	{
		wf->soilw_trans_SUM = trans;
	}
	else
	{
		wf->soilw_trans_SUM = 0;
	}
	
	/* assign water fluxes, all excess not evaporated goes to soil water compartment */
	wf->canopyw_evap = cwe;
    wf->canopyw_to_soilw = canopy_w - cwe;



	return (!ok);
}

int penmon(const pmet_struct* in, int out_flag,	double* et)
{
    /*
	Combination equation for determining evaporation and transpiration. 
	
    For output in units of (kg/m2/s)  out_flag = 0
    For output in units of (W/m2)     out_flag = 1
   
    INPUTS:
    in->ta     (deg C)   air temperature
    in->pa     (Pa)      air pressure
    in->vpd    (Pa)      vapor pressure deficit
    in->irad   (W/m2)    incident radient flux density
    in->rv     (s/m)     resistance to water vapor flux
    in->rh     (s/m)     resistance to sensible heat flux

    INTERNAL VARIABLES:
    rho    (kg/m3)       density of air
    CP     (J/kg/degC)   specific heat of air
    lhvap  (J/kg)        latent heat of vaporization of water
    s      (Pa/degC)     slope of saturation vapor pressure vs T curve

    OUTPUT:
    et     (kg/m2/s)     water vapor mass flux density  (flag=0)
    et     (W/m2)        latent heat flux density       (flag=1)
    */

    int ok=1;
    double ta;
    double rho,lhvap,s;
    double t1,t2,pvs1,pvs2,e,tk;
    double rr,rh,rhr,rv;
    double dt = 0.2;     /* set the temperature offset for slope calculation */
   
    /* assign ta (Celsius) and tk (Kelvins) */
    ta = in->ta;
    tk = ta + 273.15;
        
    /* calculate density of air (rho) as a function of air temperature */
    rho = 1.292 - (0.00428 * ta);
    
    /* calculate resistance to radiative heat transfer through air, rr */
    rr = rho * CP / (4.0 * SBC * (tk*tk*tk));
    
    /* resistance to convective heat transfer */
    rh = in->rh;
    
    /* resistance to latent heat transfer */
    rv = in->rv;
    
    /* calculate combined resistance to convective and radiative heat transfer,
    parallel resistances : rhr = (rh * rr) / (rh + rr) */
    rhr = (rh * rr) / (rh + rr);

    /* calculate latent heat of vaporization as a function of ta */
    lhvap = 2.5023e6 - 2430.54 * ta;

    /* calculate temperature offsets for slope estimate */
    t1 = ta+dt;
    t2 = ta-dt;
    
    /* calculate saturation vapor pressures at t1 and t2 */
    pvs1 = 610.7 * exp(17.38 * t1 / (239.0 + t1));
    pvs2 = 610.7 * exp(17.38 * t2 / (239.0 + t2));

    /* calculate slope of pvs vs. T curve, at ta */
    s = (pvs1-pvs2) / (t1-t2);
    
    /* calculate evaporation, in W/m^2  */
    e = ( ( s * in->irad ) + ( rho * CP * in->vpd / rhr ) ) /
    	( ( ( in->pa * CP * rv ) / ( lhvap * EPS * rhr ) ) + s );
    
    /* return either W/m^2 or kg/m^2/s, depending on out_flag */	
    if (out_flag)
    	*et = e;
    
    if (!out_flag)
    	*et = e / lhvap;
    
    return (!ok);
}
