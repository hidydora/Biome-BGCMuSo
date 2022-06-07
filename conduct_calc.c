/* 
conduct_calc.c
Calculation of conductance values based on limitation factors (in original BBGC this subroutine is partly included in canopy_et.c)
Hidy 2011.
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

int conduct_calc(const control_struct* ctrl, const metvar_struct* metv, const epconst_struct* epc, const siteconst_struct* sitec, 
                 epvar_struct* epv, int simyr)
{
	int ok=1;
	double gl_bl, gl_c, gl_s_sun, gl_s_shade;
	double gl_e_wv, gl_t_wv_sun, gl_t_wv_shade, gl_sh;
	double gc_e_wv, gc_sh;
	double tday;
	double tmin;
	double dayl;
	double vpd,vpd_open,vpd_close;
	double m_ppfd_sun, m_ppfd_shade;
	double m_tmin, m_co2, m_vpd, m_final_sun, m_final_shade;
	double proj_lai;
	double gcorr;

	/* Hidy 2013 - changing MSC value */
	double max_conduct;
	
	/* Hidy 2010. - multiplier for soil properties in multilayer soil (instead of psi: vwc) */
	int layer;
	double m_vwcR_layer; 
	double vwc_ratio;	
	double m_soilprop_avg = 0;

	/* Hidy 2013 - changing MSC value */
	max_conduct=epc->gl_smax;
	if (ctrl->varMSC_flag)
	{
		max_conduct=epc->msc_array[simyr];
	}
	
	
	/* assign variables that are used more than once */
	tday =      metv->tday;
	tmin =      metv->tmin;
	vpd =       metv->vpd;
	dayl =      metv->dayl;
	proj_lai =  epv->proj_lai;

	
	

	/******************************************************************-/
	
	/* multiplier for vpd */
	vpd_open =  epc->vpd_open;
	vpd_close = epc->vpd_close;
	
	/* temperature and pressure correction factor for conductances */
	gcorr = pow((metv->tday+273.15)/293.15, 1.75) * 101300/metv->pa;
	
	/* calculate leaf- and canopy-level conductances to water vapor and
	sensible heat fluxes */
	
	/* leaf boundary-layer conductance */
	gl_bl = epc->gl_bl * gcorr;
	
	/* leaf cuticular conductance */
	gl_c = epc->gl_c * gcorr;
	
	/* leaf stomatal conductance: first generate multipliers, then apply them
	to maximum stomatal conductance */
	/* calculate stomatal conductance radiation multiplier: 
	*** NOTE CHANGE FROM BIOME-BGC CODE ***
	The original Biome-BGC formulation follows the arguments in 
	Rastetter, E.B., A.W. King, B.J. Cosby, G.M. Hornberger, 
	   R.V. O'Neill, and J.E. Hobbie, 1992. Aggregating fine-scale 
	   ecological knowledge to model coarser-scale attributes of 
	   ecosystems. Ecological Applications, 2:55-70.

	gmult->max = (gsmax/(k*lai))*log((gsmax+rad)/(gsmax+(rad*exp(-k*lai))))

	I'm using a much simplified form, which doesn't change relative shape
	as gsmax changes. See Korner, 1995.
	*/

	/* photosynthetic photon flux density conductance control */
	m_ppfd_sun = metv->ppfd_per_plaisun/(PPFD50 + metv->ppfd_per_plaisun);
	m_ppfd_shade = metv->ppfd_per_plaishade/(PPFD50 + metv->ppfd_per_plaishade);

	
	/* ***************************************************************************/
	/* Hidy 2010 - calculate the multipiers for soil properties (soil water content) in multilayer soil */
	
	/* set the maximum and minimum values for water potential limits (MPa) */

	for (layer = 0; layer < epv->n_rootlayers; layer++)
	{
		vwc_ratio  = epv->vwc[layer] / sitec->vwc_fc;

	
		/* Hidy 2010 - soil water content multiplier (ratio) */
		if (vwc_ratio > epv->vwc_ratio_open)    /* no water stress */
		{
			m_vwcR_layer = 1.0;       
		}
		else
		{
			if (vwc_ratio <= epv->vwc_ratio_close)   /* full water stress */
			{
				m_vwcR_layer = 0.0;      
			} 
			else   /* partial water stress */
            {
				m_vwcR_layer = (vwc_ratio - epv->vwc_ratio_close) / (epv->vwc_ratio_open - epv->vwc_ratio_close);
			}
		}
	
		epv->m_soilprop_layer[layer] =  m_vwcR_layer;

		m_soilprop_avg	 += epv->m_soilprop_layer[layer] * epv->soillayer_RZportion[layer];

	}

	/* ***************************************************************************/
	/* CO2 multiplier */
	m_co2 = 1.0;

	/* freezing night minimum temperature multiplier */
	if (tmin > 0.0)        /* no effect */
		m_tmin = 1.0;
	else
	if (tmin < -8.0)       /* full tmin effect */
		m_tmin = 0.0;
	else                   /* partial reduction (0.0 to -8.0 C) */
		m_tmin = 1.0 + (0.125 * tmin);
	
	/* vapor pressure deficit multiplier, vpd in Pa */
	if (vpd < vpd_open)    /* no vpd effect */
		m_vpd = 1.0;
	else
	if (vpd > vpd_close)   /* full vpd effect */
		m_vpd = 0.0;
	else                   /* partial vpd effect */
		m_vpd = (vpd_close - vpd) / (vpd_close - vpd_open);

	
	/* apply all multipliers to the maximum stomatal conductance */

	m_final_sun = m_ppfd_sun * m_soilprop_avg * m_co2 * m_tmin * m_vpd;
	m_final_shade = m_ppfd_shade * m_soilprop_avg * m_co2 * m_tmin * m_vpd;
	gl_s_sun = max_conduct * m_final_sun * gcorr;
	gl_s_shade = max_conduct * m_final_shade * gcorr;
	
	/* calculate leaf-and canopy-level conductances to water vapor and
	sensible heat fluxes, to be used in Penman-Monteith calculations of
	canopy evaporation and canopy transpiration. */
	
	/* Leaf conductance to evaporated water vapor, per unit projected LAI */
	gl_e_wv = gl_bl;
		
	/* Leaf conductance to transpired water vapor, per unit projected
	LAI.  This formula is derived from stomatal and cuticular conductances
	in parallel with each other, and both in series with leaf boundary 
	layer conductance. */
	gl_t_wv_sun = (gl_bl * (gl_s_sun + gl_c)) / (gl_bl + gl_s_sun + gl_c);
	gl_t_wv_shade = (gl_bl * (gl_s_shade + gl_c)) / (gl_bl + gl_s_shade + gl_c);

	/* Leaf conductance to sensible heat, per unit all-sided LAI */
	gl_sh = gl_bl;
	
	/* Canopy conductance to evaporated water vapor */
	gc_e_wv = gl_e_wv * proj_lai;
	
	/* Canopy conductane to sensible heat */
	gc_sh = gl_sh * proj_lai;
	
	/* assign leaf-level conductance to transpired water vapor, 
	for use in calculating co2 conductance for farq_psn() */
	epv->gl_t_wv_sun = gl_t_wv_sun; 
	epv->gl_t_wv_shade = gl_t_wv_shade;
	
	/* assign output variables */
	epv->m_ppfd_sun     = m_ppfd_sun;
	epv->m_ppfd_shade   = m_ppfd_shade;
    epv->m_soilprop     = m_soilprop_avg;
	epv->m_co2			= m_co2;
	epv->m_tmin			= m_tmin;
	epv->m_vpd			= m_vpd;
	epv->m_final_sun	= m_final_sun;
	epv->m_final_shade	= m_final_shade;
	epv->gl_bl			= gl_bl;
	epv->gl_c			= gl_c;
	epv->gl_s_sun		= gl_s_sun;
	epv->gl_s_shade		= gl_s_shade;
	epv->gl_e_wv		= gl_e_wv;
	epv->gl_sh			= gl_sh;
	epv->gc_e_wv		= gc_e_wv;
	epv->gc_sh			= gc_sh;
	
	
    return (!ok);
}
