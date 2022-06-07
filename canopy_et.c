/* 
canopy_et.c
A single-function treatment of canopy evaporation and transpiration
fluxes.  

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Biome-BGCMuSo v6.0.
Copyright 2000, Peter E. Thornton
Numerical Terradynamic Simulation Group (NTSG)
School of Forestry, University of Montana
Missoula, MT 59812
*-*-*-*-*-*-*-*-*-*-*-*-*-*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <malloc.h>
#include "ini.h"
#include "bgc_struct.h"
#include "bgc_func.h"
#include "bgc_constants.h"

int canopy_et(const epconst_struct* epc, const metvar_struct* metv, const soilprop_struct* sprop, epvar_struct* epv, wflux_struct* wf)
{
	int errflag=0;
	double m_soilstress_avg, transp_lack;

	int layer;
	double e, cwe, t, trans, trans_sun, trans_shade, e_dayl,t_dayl;
	
	pmet_struct pmet_in;
    
	e=cwe=t=trans=trans_sun=trans_shade=e_dayl=t_dayl=m_soilstress_avg=transp_lack=0;


	/* Assign values in pmet_in that don't change */
	pmet_in.ta = metv->tday;
	pmet_in.pa = metv->pa;
	pmet_in.vpd = metv->vpd;

	
	/* Canopy evaporation, if any water was intercepted */
	/* Calculate Penman-Monteith evaporation, given the canopy conductances to
	evaporated water and sensible heat.  Calculate the time required to 
	evaporate all the canopy water at the daily average conditions, and 
	subtract that time from the daylength to get the effective daylength for
	transpiration. */
	if (wf->prcp_to_canopyw)
	{
		/* assign appropriate resistance and radiation for pmet_in */
		pmet_in.rv = 1.0/epv->gc_e_wv;
		pmet_in.rh = 1.0/epv->gc_sh;

		/* choose radiation calculation method */
		if (epc->radiation_flag == 0)
			pmet_in.irad = metv->swabs;
		else
			pmet_in.irad = metv->RADnet;
		
		/* call penman-monteith function, returns e in kg/m2/s */
		if (penmon(&pmet_in, 0, &e))
		{
			printf("ERROR: penmon() for canopy evap... \n");
			errflag=1;
		}
		
		/* calculate the time required to evaporate all the canopy water */
		e_dayl = wf->prcp_to_canopyw/e;
		
		if (e_dayl > metv->dayl)  
		{
			/* day not long enough to evap. all int. water */
			trans = 0.0;    /* no time left for transpiration */
			cwe = e * metv->dayl;   /* daylength limits canopy evaporation */
		}
		else                
		{
			/* all intercepted water evaporated */
			cwe = wf->prcp_to_canopyw;
			
			/* adjust daylength for transpiration */
			t_dayl = metv->dayl - e_dayl;
			 
			/* calculate transpiration using adjusted daylength */
			/* first for sunlit canopy fraction */
			pmet_in.rv = 1.0/epv->gl_t_wv_sun;
			pmet_in.rh = 1.0/epv->gl_sh;

			/* choose radiation calculation method */
			if (epc->radiation_flag == 0)
				pmet_in.irad = metv->swabs_per_plaisun;
			else
				pmet_in.irad = metv->RADnet_per_plaisun;

			/* call Penman-Monthieth function */
			if (penmon(&pmet_in, 0, &t))
			{
				printf("ERROR: penmon() for adjusted transpiration... \n");
				errflag=1;
			}
			trans_sun = t * t_dayl * epv->plaisun;
			
			/* next for shaded canopy fraction */
			pmet_in.rv = 1.0/epv->gl_t_wv_shade;
			pmet_in.rh = 1.0/epv->gl_sh;
			
			/* choose radiation calculation method */
			if (epc->radiation_flag == 0)
				pmet_in.irad = metv->swabs_per_plaishade;
			else
				pmet_in.irad = metv->RADnet_per_plaishade;
		
			/* call Penman-Monthieth function */
			if (penmon(&pmet_in, 0, &t))
			{
				printf("ERROR: penmon() for adjusted transpiration... \n");
				errflag=1;
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
	
		/* choose radiation calculation method */
		if (epc->radiation_flag == 0)
			pmet_in.irad = metv->swabs_per_plaisun;
		else
			pmet_in.irad = metv->RADnet_per_plaisun;

		/* call Penman-Monthieth function */
		if (penmon(&pmet_in, 0, &t))
		{
			printf("ERROR: penmon() for adjusted transpiration... \n");
			errflag=1;
		}
		trans_sun = t * metv->dayl * epv->plaisun;
		
		/* next for shaded canopy fraction */
		pmet_in.rv = 1.0/epv->gl_t_wv_shade;
		pmet_in.rh = 1.0/epv->gl_sh;
		
		/* choose radiation calculation method */
		if (epc->radiation_flag == 0)
			pmet_in.irad = metv->swabs_per_plaishade;
		else
			pmet_in.irad = metv->RADnet_per_plaishade;
		
		/* call Penman-Monthieth function */
		if (penmon(&pmet_in, 0, &t))
		{
			printf("ERROR: penmon() for adjusted transpiration... \n");
			errflag=1;
		}
		trans_shade = t * metv->dayl * epv->plaishade;
		trans = trans_sun + trans_shade;
		

		
	}
	/* multilayer soil model: multilayer transpiration is calculated in multilayer_transpiration.c */
	wf->soilw_trans_SUM = trans;
	
	/* assign water fluxes, all excess not evaporated goes to soil water compartment */
	wf->canopyw_evap = cwe;
    wf->canopyw_to_soilw = wf->prcp_to_canopyw - cwe;


	
	/* m_soistress calculation based on VWC or transpiration demand-possibitiy  */
	if (epc->soilstress_flag == 1)
	{
		m_soilstress_avg = 0;
		for (layer = 0; layer < N_SOILLAYERS; layer++)
		{
			if (epv->vwc[layer] > sprop->vwc_wp[layer])
			{
				transp_lack = (epv->vwc[layer] - sprop->vwc_wp[layer]) - wf->soilw_trans_SUM * epv->rootlength_prop[layer];
				if (transp_lack > 0)
					epv->m_soilstress_layer[layer] = 1;
				else
					epv->m_soilstress_layer[layer] = 1 - fabs(transp_lack)/(wf->soilw_trans_SUM * epv->rootlength_prop[layer]);
			}
			else
				epv->m_soilstress_layer[layer] = 0;

			m_soilstress_avg	 += epv->m_soilstress_layer[layer] * epv->rootlength_prop[layer];
		}
		epv->m_soilstress  = m_soilstress_avg;
		epv->m_final_sun   = epv->m_ppfd_sun * epv->m_soilstress *  epv->m_tmin * epv->m_vpd;
		epv->m_final_shade = epv->m_ppfd_shade * epv->m_soilstress * epv->m_tmin * epv->m_vpd;
		epv->gl_s_sun      = epv->max_conduct * epv->m_final_sun * epv->gcorr;
		epv->gl_s_shade    = epv->max_conduct * epv->m_final_shade * epv->gcorr;
		epv->gl_t_wv_sun   = (epv->gl_bl * (epv->gl_s_sun + epv->gl_c)) / (epv->gl_bl + epv->gl_s_sun + epv->gl_c);
		epv->gl_t_wv_shade = (epv->gl_bl * (epv->gl_s_shade + epv->gl_c)) / (epv->gl_bl + epv->gl_s_shade + epv->gl_c);
	}


	return (errflag);
}