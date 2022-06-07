/* 
mortality.c
daily mortality fluxes

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
BBGC MuSo v4
Copyright 2000, Peter E. Thornton
Numerical Terradynamics Simulation Group
Copyright 2014, D. Hidy (dori.hidy@gmail.com)
Hungarian Academy of Sciences
*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

Updated:
6 March 2000 (PET): Appears from tests with P.Pine data that combustion of 
deadstem should be higher. setting to 20% for testing.
9 February 1999, PET: Changed the treatment of fire mortality, so that instead
of the entire deadstem and cwd pools being subject to the mortality rates,
only 5% of deadstem and 30% of cwd are subject to fire losses. Mike White
found that there was a big difference between literature estimates of fire
emissions and model estimates for the boreal forest, and this is also a
topic that Bob Keane had pointed out a while ago as a first-order improvement
to the fire treatment. Got the values for consumption efficiency from
Agee, James K., 1993. Fire Ecology of Pacific Northwest Forests. Island Press, 
Washington, D.C. p 42.
The deadstem material that is not wilted (95%) is sent to CWD pools. CWD that
is not burned (70%) stays in CWD pools.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <malloc.h>
#include "bgc_struct.h"
#include "bgc_func.h"
#include "bgc_constants.h"

int mortality(const control_struct* ctrl, const epconst_struct* epc, epvar_struct* epv, cstate_struct* cs, cflux_struct* cf,
nstate_struct* ns, nflux_struct* nf, int simyr)
{
	int ok=1;
	int layer;
	double mort, flux_from_carbon;

	/* dead stem combustion proportion */
	double dscp = 0.2;



	/******************************************************************/
	/* Non-fire mortality: these fluxes all enter litter or CWD pools */
	/******************************************************************/


	/* Hidy 2012 -  if no changing data constant EPC parameter are used - ATTENTION: WPM = daily_mortality_turnover * NDAY_OF_YEAR */
	if (ctrl->varWPM_flag && ctrl->spinup != 1)
	{
		mort = epc->wpm_array[simyr]/NDAY_OF_YEAR;
	}
	else
	{
		mort = epc->daily_mortality_turnover;	
	}

	
	/* daily carbon fluxes due to mortality */
	/* mortality fluxes out of leaf and fine root pools */
	cf->m_leafc_to_litr1c = mort * cs->leafc * epc->leaflitr_flab;  	 
	cf->m_leafc_to_litr2c = mort * cs->leafc * epc->leaflitr_fucel;
	cf->m_leafc_to_litr3c = mort * cs->leafc * epc->leaflitr_fscel;  	 
	cf->m_leafc_to_litr4c = mort * cs->leafc * epc->leaflitr_flig;  	 
	cf->m_frootc_to_litr1c = mort * cs->frootc * epc->frootlitr_flab; 	 
	cf->m_frootc_to_litr2c = mort * cs->frootc * epc->frootlitr_fucel;
	cf->m_frootc_to_litr3c = mort * cs->frootc * epc->frootlitr_fscel; 	 
	cf->m_frootc_to_litr4c = mort * cs->frootc * epc->frootlitr_flig;
	/* fruit simulation - Hidy 2013. */
	cf->m_fruitc_to_litr1c = mort * cs->fruitc * epc->fruitlitr_flab;  	 
	cf->m_fruitc_to_litr2c = mort * cs->fruitc * epc->fruitlitr_fucel;
	cf->m_fruitc_to_litr3c = mort * cs->fruitc * epc->fruitlitr_fscel;  	 
	cf->m_fruitc_to_litr4c = mort * cs->fruitc * epc->fruitlitr_flig;  	 
	/* mortality fluxes out of storage and transfer pools */
	cf->m_leafc_storage_to_litr1c  = mort * cs->leafc_storage;
	cf->m_frootc_storage_to_litr1c  = mort * cs->frootc_storage;
	cf->m_livestemc_storage_to_litr1c  = mort * cs->livestemc_storage;
	cf->m_deadstemc_storage_to_litr1c  = mort * cs->deadstemc_storage;
	cf->m_livecrootc_storage_to_litr1c  = mort * cs->livecrootc_storage;
	cf->m_deadcrootc_storage_to_litr1c  = mort * cs->deadcrootc_storage;
	cf->m_leafc_transfer_to_litr1c = mort * cs->leafc_transfer;
	cf->m_frootc_transfer_to_litr1c = mort * cs->frootc_transfer;
	cf->m_livestemc_transfer_to_litr1c = mort * cs->livestemc_transfer;
	cf->m_deadstemc_transfer_to_litr1c = mort * cs->deadstemc_transfer;
	cf->m_livecrootc_transfer_to_litr1c = mort * cs->livecrootc_transfer;
	cf->m_deadcrootc_transfer_to_litr1c = mort * cs->deadcrootc_transfer;
	cf->m_gresp_storage_to_litr1c = mort * cs->gresp_storage;
	cf->m_gresp_transfer_to_litr1c = mort * cs->gresp_transfer;	
	/* fruit simulation - Hidy 2013. */
	cf->m_fruitc_storage_to_litr1c  = mort * cs->fruitc_storage;
	cf->m_fruitc_transfer_to_litr1c = mort * cs->fruitc_transfer;


	/* TREE-specific and NON-WOODY SPECIFIC fluxes */
	if (epc->woody)
	{
		cf->m_livestemc_to_cwdc  = mort * cs->livestemc;	 
		cf->m_deadstemc_to_cwdc  = (mort + (1.0-dscp)*epc->daily_fire_turnover) * cs->deadstemc;	 
		cf->m_livecrootc_to_cwdc = mort * cs->livecrootc;   
		cf->m_deadcrootc_to_cwdc = (mort + (1.0-dscp)*epc->daily_fire_turnover) * cs->deadcrootc; 
	}
	else
	{
		/* SOFT STEM SIMULATION of non-woody biomes - Hidy 2015 */
		cf->m_softstemc_to_litr1c = mort * cs->softstemc * epc->softstemlitr_flab;  	 
		cf->m_softstemc_to_litr2c = mort * cs->softstemc * epc->softstemlitr_fucel;
		cf->m_softstemc_to_litr3c = mort * cs->softstemc * epc->softstemlitr_fscel;  	 
		cf->m_softstemc_to_litr4c = mort * cs->softstemc * epc->softstemlitr_flig;  
		cf->m_softstemc_storage_to_litr1c  = mort * cs->softstemc_storage;
		cf->m_softstemc_transfer_to_litr1c = mort * cs->softstemc_transfer;


	}
	
	/* daily nitrogen fluxes due to mortality */
	/* mortality fluxes out of leaf and fine root pools */
	nf->m_leafn_to_litr1n = cf->m_leafc_to_litr1c / epc->leaf_cn;  	
	nf->m_leafn_to_litr2n = cf->m_leafc_to_litr2c / epc->leaf_cn;  	
	nf->m_leafn_to_litr3n = cf->m_leafc_to_litr3c / epc->leaf_cn;  	
	nf->m_leafn_to_litr4n = cf->m_leafc_to_litr4c / epc->leaf_cn;  	
	nf->m_frootn_to_litr1n = cf->m_frootc_to_litr1c / epc->froot_cn; 	
	nf->m_frootn_to_litr2n = cf->m_frootc_to_litr2c / epc->froot_cn; 	
	nf->m_frootn_to_litr3n = cf->m_frootc_to_litr3c / epc->froot_cn; 	
	nf->m_frootn_to_litr4n = cf->m_frootc_to_litr4c / epc->froot_cn;
	/* fruit simulation - Hidy 2013. */
	nf->m_fruitn_to_litr1n = cf->m_fruitc_to_litr1c / epc->fruit_cn;  	
	nf->m_fruitn_to_litr2n = cf->m_fruitc_to_litr2c / epc->fruit_cn;  	
	nf->m_fruitn_to_litr3n = cf->m_fruitc_to_litr3c / epc->fruit_cn;  	
	nf->m_fruitn_to_litr4n = cf->m_fruitc_to_litr4c / epc->fruit_cn;  	
	
	/* mortality fluxes out of storage and transfer */
	nf->m_leafn_storage_to_litr1n		= mort * ns->leafn_storage;
	nf->m_frootn_storage_to_litr1n		= mort * ns->frootn_storage;
	nf->m_livestemn_storage_to_litr1n	= mort * ns->livestemn_storage;
	nf->m_deadstemn_storage_to_litr1n	= mort * ns->deadstemn_storage;
	nf->m_livecrootn_storage_to_litr1n  = mort * ns->livecrootn_storage;
	nf->m_deadcrootn_storage_to_litr1n  = mort * ns->deadcrootn_storage;
	nf->m_leafn_transfer_to_litr1n		= mort * ns->leafn_transfer;
	nf->m_frootn_transfer_to_litr1n		= mort * ns->frootn_transfer;
	nf->m_livestemn_transfer_to_litr1n	= mort * ns->livestemn_transfer;
	nf->m_deadstemn_transfer_to_litr1n	= mort * ns->deadstemn_transfer;
	nf->m_livecrootn_transfer_to_litr1n = mort * ns->livecrootn_transfer;
	nf->m_deadcrootn_transfer_to_litr1n = mort * ns->deadcrootn_transfer;
    nf->m_retransn_to_litr1n			= mort * ns->retransn;

	/* fruit simulation - Hidy 2013. */
	nf->m_fruitn_storage_to_litr1n  = mort * ns->fruitn_storage;
	nf->m_fruitn_transfer_to_litr1n = mort * ns->fruitn_transfer;

	/* TREE-specific and NON-WOODY SPECIFIC fluxes */
	if (epc->woody)
	{	/* woody-specific nitrogen fluxes */
		nf->m_livestemn_to_cwdn = cf->m_livestemc_to_cwdc / epc->deadwood_cn;	
       	nf->m_livestemn_to_litr1n = (mort * ns->livestemn) - nf->m_livestemn_to_cwdn;
		nf->m_deadstemn_to_cwdn = cf->m_deadstemc_to_cwdc / epc->deadwood_cn;	
		nf->m_livecrootn_to_cwdn = cf->m_livecrootc_to_cwdc / epc->deadwood_cn;  
		nf->m_livecrootn_to_litr1n = (mort * ns->livecrootn) - nf->m_livecrootn_to_cwdn;
		nf->m_deadcrootn_to_cwdn = cf->m_deadcrootc_to_cwdc / epc->deadwood_cn;  
	}
	else
	{
		/* softstem simulation - Hidy 2013. */
		nf->m_softstemn_to_litr1n = cf->m_softstemc_to_litr1c / epc->softstem_cn;  	
		nf->m_softstemn_to_litr2n = cf->m_softstemc_to_litr2c / epc->softstem_cn;  	
		nf->m_softstemn_to_litr3n = cf->m_softstemc_to_litr3c / epc->softstem_cn;  	
		nf->m_softstemn_to_litr4n = cf->m_softstemc_to_litr4c / epc->softstem_cn;  	
		nf->m_softstemn_storage_to_litr1n  = mort * ns->softstemn_storage;
		nf->m_softstemn_transfer_to_litr1n = mort * ns->softstemn_transfer;	
	}
	
	/*-------------------------------------------------------------------------------------------------------*/
	/* UPDATE STATE VARIALBES */

	/* this is the only place other than daily_state_update() routines where state variables get changed.  
	Mortality is taken care of last and given special treatment for state update so that it doesn't interfere
	with the other fluxes that are based on proportions of state variables, especially the phenological fluxes */

	/* 1. ABOVEGROUND BIOMASS  - into the first layer in multilayer soil - Hidy 2016 */
	/* 1.1 Leaf CARBON mortality  */
	cs->litr1c[0]      += cf->m_leafc_to_litr1c;
	cs->leafc          -= cf->m_leafc_to_litr1c;
	cs->litr2c[0]      += cf->m_leafc_to_litr2c;
	cs->leafc          -= cf->m_leafc_to_litr2c;
	cs->litr3c[0]      += cf->m_leafc_to_litr3c;
	cs->leafc          -= cf->m_leafc_to_litr3c;
	cs->litr4c[0]      += cf->m_leafc_to_litr4c;
	cs->leafc          -= cf->m_leafc_to_litr4c;
	
	/* 1.2 Fruit CARBON mortality */
	cs->litr1c[0]      += cf->m_fruitc_to_litr1c;
	cs->fruitc         -= cf->m_fruitc_to_litr1c;
	cs->litr2c[0]      += cf->m_fruitc_to_litr2c;
	cs->fruitc         -= cf->m_fruitc_to_litr2c;
	cs->litr3c[0]      += cf->m_fruitc_to_litr3c;
	cs->fruitc         -= cf->m_fruitc_to_litr3c;
	cs->litr4c[0]      += cf->m_fruitc_to_litr4c;
	cs->fruitc         -= cf->m_fruitc_to_litr4c;

	
	/* 1.3 softstem CARBON mortality */
	cs->litr1c[0]      += cf->m_softstemc_to_litr1c;
	cs->softstemc      -= cf->m_softstemc_to_litr1c;
	cs->litr2c[0]      += cf->m_softstemc_to_litr2c;
	cs->softstemc      -= cf->m_softstemc_to_litr2c;
	cs->litr3c[0]      += cf->m_softstemc_to_litr3c;
	cs->softstemc      -= cf->m_softstemc_to_litr3c;
	cs->litr4c[0]      += cf->m_softstemc_to_litr4c;
	cs->softstemc      -= cf->m_softstemc_to_litr4c;


	/* 1.4 Leaf NITROGEN mortality */
	ns->litr1n[0]      += nf->m_leafn_to_litr1n;
	ns->leafn          -= nf->m_leafn_to_litr1n;
	ns->litr2n[0]      += nf->m_leafn_to_litr2n;
	ns->leafn          -= nf->m_leafn_to_litr2n;
	ns->litr3n[0]      += nf->m_leafn_to_litr3n;
	ns->leafn          -= nf->m_leafn_to_litr3n;
	ns->litr4n[0]      += nf->m_leafn_to_litr4n;
	ns->leafn          -= nf->m_leafn_to_litr4n;

	/* 1.5 Fruit NITROGEN mortality - Hidy 2013. */
	ns->litr1n[0]      += nf->m_fruitn_to_litr1n;
	ns->fruitn         -= nf->m_fruitn_to_litr1n;
	ns->litr2n[0]      += nf->m_fruitn_to_litr2n;
	ns->fruitn         -= nf->m_fruitn_to_litr2n;
	ns->litr3n[0]      += nf->m_fruitn_to_litr3n;
	ns->fruitn         -= nf->m_fruitn_to_litr3n;
	ns->litr4n[0]      += nf->m_fruitn_to_litr4n;
	ns->fruitn         -= nf->m_fruitn_to_litr4n;

	/* 1.6 softstem NITROGEN mortality - Hidy 2013. */
	ns->litr1n[0]		+= nf->m_softstemn_to_litr1n;
	ns->softstemn		-= nf->m_softstemn_to_litr1n;
	ns->litr2n[0]		+= nf->m_softstemn_to_litr2n;
	ns->softstemn		-= nf->m_softstemn_to_litr2n;
	ns->litr3n[0]		+= nf->m_softstemn_to_litr3n;
	ns->softstemn		-= nf->m_softstemn_to_litr3n;
	ns->litr4n[0]		+= nf->m_softstemn_to_litr4n;
	ns->softstemn		-= nf->m_softstemn_to_litr4n;
	
	ns->litr1n[0]       += nf->m_retransn_to_litr1n;
	ns->retransn	    -= nf->m_retransn_to_litr1n;
	

	/* 2. BELOWGROUND BIOMASS  - DIVIDED BETWEEN THE DIFFERENT SOIL LAYERS in multilayer soil - Hidy 2016 */
	for (layer = 0; layer < N_SOILLAYERS; layer++)
	{
		/*   2.1 Fine root CARBON mortality */
		cs->litr1c[layer]  += cf->m_frootc_to_litr1c * epv->rootlength_prop[layer];
		cs->frootc         -= cf->m_frootc_to_litr1c * epv->rootlength_prop[layer];
		cs->litr2c[layer]  += cf->m_frootc_to_litr2c * epv->rootlength_prop[layer];
		cs->frootc         -= cf->m_frootc_to_litr2c * epv->rootlength_prop[layer];
		cs->litr3c[layer]  += cf->m_frootc_to_litr3c * epv->rootlength_prop[layer];
		cs->frootc         -= cf->m_frootc_to_litr3c * epv->rootlength_prop[layer];
		cs->litr4c[layer]  += cf->m_frootc_to_litr4c * epv->rootlength_prop[layer];
		cs->frootc         -= cf->m_frootc_to_litr4c * epv->rootlength_prop[layer];

		/*  2.2 Fine root NITROGEN mortality */
		ns->litr1n[layer]  += nf->m_frootn_to_litr1n * epv->rootlength_prop[layer];
		ns->frootn         -= nf->m_frootn_to_litr1n * epv->rootlength_prop[layer];
		ns->litr2n[layer]  += nf->m_frootn_to_litr2n * epv->rootlength_prop[layer];
		ns->frootn         -= nf->m_frootn_to_litr2n * epv->rootlength_prop[layer];
		ns->litr3n[layer]  += nf->m_frootn_to_litr3n * epv->rootlength_prop[layer];
		ns->frootn         -= nf->m_frootn_to_litr3n * epv->rootlength_prop[layer];
		ns->litr4n[layer]  += nf->m_frootn_to_litr4n * epv->rootlength_prop[layer];
		ns->frootn         -= nf->m_frootn_to_litr4n * epv->rootlength_prop[layer];

	
		/*   2.3 Storage and transfer CARBON mortality */
		cs->litr1c[layer]		+= cf->m_leafc_storage_to_litr1c * epv->rootlength_prop[layer];
		cs->leafc_storage       -= cf->m_leafc_storage_to_litr1c * epv->rootlength_prop[layer];
		cs->litr1c[layer]		+= cf->m_frootc_storage_to_litr1c * epv->rootlength_prop[layer];
		cs->frootc_storage      -= cf->m_frootc_storage_to_litr1c * epv->rootlength_prop[layer];
		cs->litr1c[layer]		+= cf->m_livestemc_storage_to_litr1c * epv->rootlength_prop[layer];
		cs->livestemc_storage   -= cf->m_livestemc_storage_to_litr1c * epv->rootlength_prop[layer];
		cs->litr1c[layer]		+= cf->m_deadstemc_storage_to_litr1c * epv->rootlength_prop[layer];
		cs->deadstemc_storage   -= cf->m_deadstemc_storage_to_litr1c * epv->rootlength_prop[layer];
		cs->litr1c[layer]		+= cf->m_livecrootc_storage_to_litr1c * epv->rootlength_prop[layer];
		cs->livecrootc_storage  -= cf->m_livecrootc_storage_to_litr1c * epv->rootlength_prop[layer];
		cs->litr1c[layer]		+= cf->m_deadcrootc_storage_to_litr1c * epv->rootlength_prop[layer];
		cs->deadcrootc_storage  -= cf->m_deadcrootc_storage_to_litr1c * epv->rootlength_prop[layer];
		cs->litr1c[layer]		+= cf->m_leafc_transfer_to_litr1c * epv->rootlength_prop[layer];
		cs->leafc_transfer      -= cf->m_leafc_transfer_to_litr1c * epv->rootlength_prop[layer];
		cs->litr1c[layer]		+= cf->m_frootc_transfer_to_litr1c * epv->rootlength_prop[layer];
		cs->frootc_transfer     -= cf->m_frootc_transfer_to_litr1c * epv->rootlength_prop[layer];
		cs->litr1c[layer]		+= cf->m_livestemc_transfer_to_litr1c * epv->rootlength_prop[layer];
		cs->livestemc_transfer  -= cf->m_livestemc_transfer_to_litr1c * epv->rootlength_prop[layer];
		cs->litr1c[layer]		+= cf->m_deadstemc_transfer_to_litr1c * epv->rootlength_prop[layer];
		cs->deadstemc_transfer  -= cf->m_deadstemc_transfer_to_litr1c * epv->rootlength_prop[layer];
		cs->litr1c[layer]		+= cf->m_livecrootc_transfer_to_litr1c * epv->rootlength_prop[layer];
		cs->livecrootc_transfer -= cf->m_livecrootc_transfer_to_litr1c * epv->rootlength_prop[layer];
		cs->litr1c[layer]		+= cf->m_deadcrootc_transfer_to_litr1c * epv->rootlength_prop[layer];
		cs->deadcrootc_transfer -= cf->m_deadcrootc_transfer_to_litr1c * epv->rootlength_prop[layer];
		cs->litr1c[layer]		+= cf->m_gresp_storage_to_litr1c * epv->rootlength_prop[layer];
		cs->gresp_storage       -= cf->m_gresp_storage_to_litr1c * epv->rootlength_prop[layer];
		cs->litr1c[layer]		+= cf->m_gresp_transfer_to_litr1c * epv->rootlength_prop[layer];
		cs->gresp_transfer      -= cf->m_gresp_transfer_to_litr1c * epv->rootlength_prop[layer];
		cs->litr1c[layer]		+= cf->m_fruitc_storage_to_litr1c * epv->rootlength_prop[layer];
		cs->fruitc_storage      -= cf->m_fruitc_storage_to_litr1c * epv->rootlength_prop[layer];
		cs->litr1c[layer]		+= cf->m_fruitc_transfer_to_litr1c * epv->rootlength_prop[layer];
		cs->fruitc_transfer     -= cf->m_fruitc_transfer_to_litr1c * epv->rootlength_prop[layer];
		cs->litr1c[layer]       += cf->m_softstemc_storage_to_litr1c * epv->rootlength_prop[layer];
		cs->softstemc_storage   -= cf->m_softstemc_storage_to_litr1c * epv->rootlength_prop[layer];
		cs->litr1c[layer]       += cf->m_softstemc_transfer_to_litr1c * epv->rootlength_prop[layer];
		cs->softstemc_transfer  -= cf->m_softstemc_transfer_to_litr1c * epv->rootlength_prop[layer];


		/* 2.4 TREE-specific and NON-WOODY SPECIFIC CARBON fluxes */
		if (epc->woody)
		{
			/*    Stem wood mortality */
			cs->cwdc[layer]     += cf->m_livestemc_to_cwdc * epv->rootlength_prop[layer];
			cs->livestemc		-= cf->m_livestemc_to_cwdc * epv->rootlength_prop[layer];
			cs->cwdc[layer]     += cf->m_deadstemc_to_cwdc * epv->rootlength_prop[layer];
			cs->deadstemc		-= cf->m_deadstemc_to_cwdc * epv->rootlength_prop[layer];
			/* STEP 1e  Coarse root wood mortality */
			cs->cwdc[layer]     += cf->m_livecrootc_to_cwdc * epv->rootlength_prop[layer];
			cs->livecrootc		-= cf->m_livecrootc_to_cwdc * epv->rootlength_prop[layer];
			cs->cwdc[layer]     += cf->m_deadcrootc_to_cwdc * epv->rootlength_prop[layer];
			cs->deadcrootc		-= cf->m_deadcrootc_to_cwdc * epv->rootlength_prop[layer];
		}

		/*  2.5 Storage, transfer, excess, and npool NITROGEN mortality */
		ns->litr1n[layer]       += nf->m_leafn_storage_to_litr1n * epv->rootlength_prop[layer];
		ns->leafn_storage       -= nf->m_leafn_storage_to_litr1n * epv->rootlength_prop[layer];
		ns->litr1n[layer]       += nf->m_frootn_storage_to_litr1n * epv->rootlength_prop[layer];
		ns->frootn_storage      -= nf->m_frootn_storage_to_litr1n * epv->rootlength_prop[layer];
		ns->litr1n[layer]       += nf->m_livestemn_storage_to_litr1n * epv->rootlength_prop[layer];
		ns->livestemn_storage   -= nf->m_livestemn_storage_to_litr1n * epv->rootlength_prop[layer];
		ns->litr1n[layer]       += nf->m_deadstemn_storage_to_litr1n * epv->rootlength_prop[layer];
		ns->deadstemn_storage   -= nf->m_deadstemn_storage_to_litr1n * epv->rootlength_prop[layer];
		ns->litr1n[layer]       += nf->m_livecrootn_storage_to_litr1n * epv->rootlength_prop[layer];
		ns->livecrootn_storage  -= nf->m_livecrootn_storage_to_litr1n * epv->rootlength_prop[layer];
		ns->litr1n[layer]       += nf->m_deadcrootn_storage_to_litr1n * epv->rootlength_prop[layer];
		ns->deadcrootn_storage  -= nf->m_deadcrootn_storage_to_litr1n * epv->rootlength_prop[layer];
		ns->litr1n[layer]       += nf->m_leafn_transfer_to_litr1n * epv->rootlength_prop[layer];
		ns->leafn_transfer      -= nf->m_leafn_transfer_to_litr1n * epv->rootlength_prop[layer];
		ns->litr1n[layer]       += nf->m_frootn_transfer_to_litr1n * epv->rootlength_prop[layer];
		ns->frootn_transfer     -= nf->m_frootn_transfer_to_litr1n * epv->rootlength_prop[layer];
		ns->litr1n[layer]       += nf->m_livestemn_transfer_to_litr1n * epv->rootlength_prop[layer];
		ns->livestemn_transfer  -= nf->m_livestemn_transfer_to_litr1n * epv->rootlength_prop[layer];
		ns->litr1n[layer]       += nf->m_deadstemn_transfer_to_litr1n * epv->rootlength_prop[layer];
		ns->deadstemn_transfer  -= nf->m_deadstemn_transfer_to_litr1n * epv->rootlength_prop[layer];
		ns->litr1n[layer]       += nf->m_livecrootn_transfer_to_litr1n * epv->rootlength_prop[layer];
		ns->livecrootn_transfer -= nf->m_livecrootn_transfer_to_litr1n * epv->rootlength_prop[layer];
		ns->litr1n[layer]       += nf->m_deadcrootn_transfer_to_litr1n * epv->rootlength_prop[layer];
		ns->deadcrootn_transfer -= nf->m_deadcrootn_transfer_to_litr1n * epv->rootlength_prop[layer];

		ns->litr1n[layer]       += nf->m_fruitn_storage_to_litr1n * epv->rootlength_prop[layer];
		ns->fruitn_storage      -= nf->m_fruitn_storage_to_litr1n * epv->rootlength_prop[layer];
		ns->litr1n[layer]       += nf->m_fruitn_transfer_to_litr1n * epv->rootlength_prop[layer];
		ns->fruitn_transfer     -= nf->m_fruitn_transfer_to_litr1n * epv->rootlength_prop[layer];
		ns->litr1n[layer]       += nf->m_softstemn_storage_to_litr1n * epv->rootlength_prop[layer];
		ns->softstemn_storage   -= nf->m_softstemn_storage_to_litr1n * epv->rootlength_prop[layer];
		ns->litr1n[layer]       += nf->m_softstemn_transfer_to_litr1n * epv->rootlength_prop[layer];
		ns->softstemn_transfer  -= nf->m_softstemn_transfer_to_litr1n * epv->rootlength_prop[layer];

		/* TREE-specific and NON-WOODY SPECIFIC NITROGEN fluxes */
		if (epc->woody)
		{
			/*  Stem wood mortality */
			ns->litr1n[layer]    += nf->m_livestemn_to_litr1n * epv->rootlength_prop[layer];
			ns->livestemn		-= nf->m_livestemn_to_litr1n * epv->rootlength_prop[layer];
			ns->cwdn[layer]     += nf->m_livestemn_to_cwdn * epv->rootlength_prop[layer];
			ns->livestemn		-= nf->m_livestemn_to_cwdn * epv->rootlength_prop[layer];
			ns->cwdn[layer]     += nf->m_deadstemn_to_cwdn * epv->rootlength_prop[layer];
			ns->deadstemn		-= nf->m_deadstemn_to_cwdn * epv->rootlength_prop[layer];
			ns->litr1n[layer]   += nf->m_livecrootn_to_litr1n * epv->rootlength_prop[layer];
			ns->livecrootn		-= nf->m_livecrootn_to_litr1n * epv->rootlength_prop[layer];
			ns->cwdn[layer]     += nf->m_livecrootn_to_cwdn * epv->rootlength_prop[layer];
			ns->livecrootn		-= nf->m_livecrootn_to_cwdn * epv->rootlength_prop[layer];
			ns->cwdn[layer]     += nf->m_deadcrootn_to_cwdn * epv->rootlength_prop[layer];
			ns->deadcrootn		-= nf->m_deadcrootn_to_cwdn * epv->rootlength_prop[layer];
		}
	
	
	}




	
	/************************************************************/
	/* Fire mortality: these fluxes all enter atmospheric sinks */
	/************************************************************/

	
	mort = epc->daily_fire_turnover;
	
	/* daily carbon fluxes due to mortality */
	/* mortality fluxes out of leaf and fine root pools */
	cf->m_leafc_to_fire = mort * cs->leafc;  	 
	cf->m_frootc_to_fire = mort * cs->frootc;
	/* fruit simulation - Hidy 2013. */
	cf->m_fruitc_to_fire = mort * cs->fruitc;  
	
	/* mortality fluxes out of storage and transfer pools */
	cf->m_leafc_storage_to_fire  = mort * cs->leafc_storage;
	cf->m_frootc_storage_to_fire  = mort * cs->frootc_storage;
	cf->m_livestemc_storage_to_fire  = mort * cs->livestemc_storage;
	cf->m_deadstemc_storage_to_fire  = mort * cs->deadstemc_storage;
	cf->m_livecrootc_storage_to_fire  = mort * cs->livecrootc_storage;
	cf->m_deadcrootc_storage_to_fire  = mort * cs->deadcrootc_storage;
	cf->m_leafc_transfer_to_fire = mort * cs->leafc_transfer;
	cf->m_frootc_transfer_to_fire = mort * cs->frootc_transfer;
	cf->m_livestemc_transfer_to_fire = mort * cs->livestemc_transfer;
	cf->m_deadstemc_transfer_to_fire = mort * cs->deadstemc_transfer;
	cf->m_livecrootc_transfer_to_fire = mort * cs->livecrootc_transfer;
	cf->m_deadcrootc_transfer_to_fire = mort * cs->deadcrootc_transfer;
	cf->m_gresp_storage_to_fire =  mort * cs->gresp_storage;
	cf->m_gresp_transfer_to_fire = mort * cs->gresp_transfer;
	/* fruit simulation - Hidy 2013. */
	cf->m_fruitc_storage_to_fire  = mort * cs->fruitc_storage;
	cf->m_fruitc_transfer_to_fire = mort * cs->fruitc_transfer;

	
		
	/* TREE-specific and NON-WOODY SPECIFIC fluxes */
	if (epc->woody)
	{
        cf->m_livestemc_to_fire = mort * cs->livestemc;	 
		cf->m_deadstemc_to_fire = dscp * mort * cs->deadstemc;	 
		cf->m_livecrootc_to_fire = mort * cs->livecrootc;   
		cf->m_deadcrootc_to_fire = dscp * mort * cs->deadcrootc; 
	}
	else
    {
         /* SOFT STEM SIMULATION of non-woody biomes - Hidy 2015 */
	     cf->m_softstemc_to_fire = mort * cs->softstemc;  
         cf->m_softstemc_storage_to_fire  = mort * cs->softstemc_storage;
	     cf->m_softstemc_transfer_to_fire = mort * cs->softstemc_transfer;

     } 
	
	/* litter and CWD fire fluxes */
	/* Hidy 2016 - fire affects only the top layer */
	cf->m_litr1c_to_fire = mort * cs->litr1c[0];
	cf->m_litr2c_to_fire = mort * cs->litr2c[0];
	cf->m_litr3c_to_fire = mort * cs->litr3c[0];
	cf->m_litr4c_to_fire = mort * cs->litr4c[0];
	cf->m_cwdc_to_fire	 = 0.30 * mort * cs->cwdc[0];
	
	
	/* daily nitrogen fluxes due to mortality */
	/* mortality fluxes out of leaf and fine root pools */
	nf->m_leafn_to_fire	 = cf->m_leafc_to_fire / epc->leaf_cn;  	
	nf->m_frootn_to_fire = cf->m_frootc_to_fire / epc->froot_cn; 	
	/* fruit simulation - Hidy 2013. */
	nf->m_fruitn_to_fire = cf->m_fruitc_to_fire / epc->fruit_cn;  	
	
	/* mortality fluxes out of storage and transfer */
	nf->m_leafn_storage_to_fire			=  mort * ns->leafn_storage;
	nf->m_frootn_storage_to_fire		=  mort * ns->frootn_storage;
	nf->m_livestemn_storage_to_fire		= mort * ns->livestemn_storage;
	nf->m_deadstemn_storage_to_fire		= mort * ns->deadstemn_storage;
	nf->m_livecrootn_storage_to_fire	= mort * ns->livecrootn_storage;
	nf->m_deadcrootn_storage_to_fire	= mort * ns->deadcrootn_storage;
	nf->m_leafn_transfer_to_fire		= mort * ns->leafn_transfer;
	nf->m_frootn_transfer_to_fire		= mort * ns->frootn_transfer;
	nf->m_livestemn_transfer_to_fire	= mort * ns->livestemn_transfer;
	nf->m_deadstemn_transfer_to_fire	= mort * ns->deadstemn_transfer;
	nf->m_livecrootn_transfer_to_fire	= mort * ns->livecrootn_transfer;
	nf->m_deadcrootn_transfer_to_fire	= mort * ns->deadcrootn_transfer;
	nf->m_retransn_to_fire				= mort * ns->retransn;

	/* fruit simulation - Hidy 2013. */
	nf->m_fruitn_storage_to_fire	= mort * ns->fruitn_storage;
	nf->m_fruitn_transfer_to_fire	= mort * ns->fruitn_transfer;
	
	/* TREE-specific and NON-WOODY SPECIFIC fluxes */
	if (epc->woody)
	{   /* woody-specific nitrogen fluxes */
        nf->m_livestemn_to_fire		= mort * ns->livestemn;	
		nf->m_deadstemn_to_fire		= dscp * mort * ns->deadstemn;
		nf->m_livecrootn_to_fire	=  mort * ns->livecrootn;
		nf->m_deadcrootn_to_fire	=  dscp * mort * ns->deadcrootn;
	}
	else
	{ /* softstem simulation - Hidy 2015 */
		nf->m_softstemn_to_fire = cf->m_softstemc_to_fire / epc->softstem_cn;  
		nf->m_softstemn_storage_to_fire  = mort * ns->softstemn_storage;
		nf->m_softstemn_transfer_to_fire = mort * ns->softstemn_transfer;
	
	}

	
	/* litter and CWD fire fluxes - Hidy 2015: modified in order to avoid negative fluxes */
	/* Hidy 2016 - fire affects only the top layer */

	nf->m_litr1n_to_fire = mort * ns->litr1n[0];
	flux_from_carbon = cf->m_litr1c_to_fire * (cs->litr1c[0] / ns->litr1n[0]);
	if (nf->m_litr1n_to_fire > flux_from_carbon) nf->m_litr1n_to_fire = flux_from_carbon;

	nf->m_litr2n_to_fire = mort * ns->litr2n[0];
	flux_from_carbon = cf->m_litr2c_to_fire * (cs->litr2c[0] / ns->litr2n[0]);
	if (nf->m_litr2n_to_fire > flux_from_carbon) nf->m_litr2n_to_fire = flux_from_carbon;
		
	nf->m_litr3n_to_fire = mort * ns->litr3n[0];
	flux_from_carbon = cf->m_litr3c_to_fire * (cs->litr3c[0] / ns->litr3n[0]);
	if (nf->m_litr3n_to_fire > flux_from_carbon) nf->m_litr3n_to_fire = flux_from_carbon;
		
	nf->m_litr4n_to_fire = mort * ns->litr4n[0];
	flux_from_carbon = cf->m_litr4c_to_fire * (cs->litr4c[0] / ns->litr4n[0]);
	if (nf->m_litr4n_to_fire > flux_from_carbon) nf->m_litr4n_to_fire = flux_from_carbon;

	nf->m_cwdn_to_fire = 0.30 * mort * ns->cwdn[0];
	flux_from_carbon = cf->m_cwdc_to_fire * (cs->cwdc[0] / ns->cwdn[0]);
	if (nf->m_cwdn_to_fire > flux_from_carbon) nf->m_cwdn_to_fire = flux_from_carbon;

	
	/* update state variables for fire fluxes */
	/* this is the only place other than daily_state_update() routines where
	state variables get changed.  Mortality is taken care of last and 
	given special treatment for state update so that it doesn't interfere
	with the other fluxes that are based on proportions of state variables,
	especially the phenological fluxes */
	/* CARBON mortality state variable update */	
	/*   Leaf mortality */
	cs->fire_snk       += cf->m_leafc_to_fire;
	cs->leafc          -= cf->m_leafc_to_fire;
	/*   Fine root mortality */
	cs->fire_snk       += cf->m_frootc_to_fire;
	cs->frootc         -= cf->m_frootc_to_fire;
	/*   Fruit mortality - Hidy 2013. */
	cs->fire_snk       += cf->m_fruitc_to_fire;
	cs->fruitc         -= cf->m_fruitc_to_fire;
	/*   Storage and transfer mortality */
	cs->fire_snk            += cf->m_leafc_storage_to_fire;
	cs->leafc_storage       -= cf->m_leafc_storage_to_fire;
	cs->fire_snk            += cf->m_frootc_storage_to_fire;
	cs->frootc_storage      -= cf->m_frootc_storage_to_fire;
	cs->fire_snk            += cf->m_livestemc_storage_to_fire;
	cs->livestemc_storage   -= cf->m_livestemc_storage_to_fire;
	cs->fire_snk            += cf->m_deadstemc_storage_to_fire;
	cs->deadstemc_storage   -= cf->m_deadstemc_storage_to_fire;
	cs->fire_snk            += cf->m_livecrootc_storage_to_fire;
	cs->livecrootc_storage  -= cf->m_livecrootc_storage_to_fire;
	cs->fire_snk            += cf->m_deadcrootc_storage_to_fire;
	cs->deadcrootc_storage  -= cf->m_deadcrootc_storage_to_fire;
	cs->fire_snk            += cf->m_leafc_transfer_to_fire;
	cs->leafc_transfer      -= cf->m_leafc_transfer_to_fire;
	cs->fire_snk            += cf->m_frootc_transfer_to_fire;
	cs->frootc_transfer     -= cf->m_frootc_transfer_to_fire;
	cs->fire_snk            += cf->m_livestemc_transfer_to_fire;
	cs->livestemc_transfer  -= cf->m_livestemc_transfer_to_fire;
	cs->fire_snk            += cf->m_deadstemc_transfer_to_fire;
	cs->deadstemc_transfer  -= cf->m_deadstemc_transfer_to_fire;
	cs->fire_snk            += cf->m_livecrootc_transfer_to_fire;
	cs->livecrootc_transfer -= cf->m_livecrootc_transfer_to_fire;
	cs->fire_snk            += cf->m_deadcrootc_transfer_to_fire;
	cs->deadcrootc_transfer -= cf->m_deadcrootc_transfer_to_fire;
	cs->fire_snk            += cf->m_gresp_storage_to_fire;
	cs->gresp_storage       -= cf->m_gresp_storage_to_fire;
	cs->fire_snk            += cf->m_gresp_transfer_to_fire;
	cs->gresp_transfer      -= cf->m_gresp_transfer_to_fire;
	/* fruit simulation - Hidy 2013. */
	cs->fire_snk            += cf->m_fruitc_storage_to_fire;
	cs->fruitc_storage      -= cf->m_fruitc_storage_to_fire;
	cs->fire_snk            += cf->m_fruitc_transfer_to_fire;
	cs->fruitc_transfer     -= cf->m_fruitc_transfer_to_fire;

	
	/* TREE-specific and NON-WOODY SPECIFIC fluxes */
	if (epc->woody)
	{
		/*  Stem wood mortality */

		cs->fire_snk   += cf->m_livestemc_to_fire;
		cs->livestemc  -= cf->m_livestemc_to_fire;
		cs->fire_snk   += cf->m_deadstemc_to_fire;
		cs->deadstemc  -= cf->m_deadstemc_to_fire;
		/* STEP 1e  Coarse root wood mortality */
		cs->fire_snk   += cf->m_livecrootc_to_fire;
		cs->livecrootc -= cf->m_livecrootc_to_fire;
		cs->fire_snk   += cf->m_deadcrootc_to_fire;
		cs->deadcrootc -= cf->m_deadcrootc_to_fire;
	}
	else /* softstem simulation - Hidy 2015*/
	{
		cs->fire_snk            += cf->m_softstemc_to_fire;
		cs->softstemc           -= cf->m_softstemc_to_fire;
		cs->fire_snk            += cf->m_softstemc_storage_to_fire;
		cs->softstemc_storage   -= cf->m_softstemc_storage_to_fire;
		cs->fire_snk            += cf->m_softstemc_transfer_to_fire;
		cs->softstemc_transfer  -= cf->m_softstemc_transfer_to_fire;
	}

	/* litter and CWD carbon state updates */
    /* Hidy 2016 - fire addects only top soil layer */
	cs->fire_snk	+= cf->m_litr1c_to_fire;
	cs->litr1c[0]   -= cf->m_litr1c_to_fire;
	cs->fire_snk	+= cf->m_litr2c_to_fire;
	cs->litr2c[0]   -= cf->m_litr2c_to_fire;
	cs->fire_snk	+= cf->m_litr3c_to_fire;
	cs->litr3c[0]   -= cf->m_litr3c_to_fire;
	cs->fire_snk	+= cf->m_litr4c_to_fire;
	cs->litr4c[0]   -= cf->m_litr4c_to_fire;
	cs->fire_snk	+= cf->m_cwdc_to_fire;
	cs->cwdc[0]		-= cf->m_cwdc_to_fire;
	
	/* NITROGEN mortality state variable update */
	/*    Leaf mortality */
	ns->fire_snk       += nf->m_leafn_to_fire;
	ns->leafn          -= nf->m_leafn_to_fire;
	/*    Fine root mortality */
	ns->fire_snk       += nf->m_frootn_to_fire;
	ns->frootn         -= nf->m_frootn_to_fire;
	/*    Fruit mortality - Hidy 2013. */
	ns->fire_snk       += nf->m_fruitn_to_fire;
	ns->fruitn         -= nf->m_fruitn_to_fire;
	/*    Storage, transfer, excess, and npool mortality */
	ns->fire_snk            += nf->m_leafn_storage_to_fire;
	ns->leafn_storage       -= nf->m_leafn_storage_to_fire;
	ns->fire_snk            += nf->m_frootn_storage_to_fire;
	ns->frootn_storage      -= nf->m_frootn_storage_to_fire;
	ns->fire_snk            += nf->m_livestemn_storage_to_fire;
	ns->livestemn_storage   -= nf->m_livestemn_storage_to_fire;
	ns->fire_snk            += nf->m_deadstemn_storage_to_fire;
	ns->deadstemn_storage   -= nf->m_deadstemn_storage_to_fire;
	ns->fire_snk            += nf->m_livecrootn_storage_to_fire;
	ns->livecrootn_storage  -= nf->m_livecrootn_storage_to_fire;
	ns->fire_snk            += nf->m_deadcrootn_storage_to_fire;
	ns->deadcrootn_storage  -= nf->m_deadcrootn_storage_to_fire;
	ns->fire_snk            += nf->m_leafn_transfer_to_fire;
	ns->leafn_transfer      -= nf->m_leafn_transfer_to_fire;
	ns->fire_snk            += nf->m_frootn_transfer_to_fire;
	ns->frootn_transfer     -= nf->m_frootn_transfer_to_fire;
	ns->fire_snk            += nf->m_livestemn_transfer_to_fire;
	ns->livestemn_transfer  -= nf->m_livestemn_transfer_to_fire;
	ns->fire_snk            += nf->m_deadstemn_transfer_to_fire;
	ns->deadstemn_transfer  -= nf->m_deadstemn_transfer_to_fire;
	ns->fire_snk            += nf->m_livecrootn_transfer_to_fire;
	ns->livecrootn_transfer -= nf->m_livecrootn_transfer_to_fire;
	ns->fire_snk            += nf->m_deadcrootn_transfer_to_fire;
	ns->deadcrootn_transfer -= nf->m_deadcrootn_transfer_to_fire;
	ns->fire_snk            += nf->m_retransn_to_fire;
	/* Hidy 2016 - fire addects only top soil layer */
	ns->retransn            -= nf->m_retransn_to_fire;
	/*  fruit simulation - Hidy 2013. */
	ns->fire_snk            += nf->m_fruitn_storage_to_fire;
	ns->fruitn_storage      -= nf->m_fruitn_storage_to_fire;
	ns->fire_snk            += nf->m_fruitn_transfer_to_fire;
	ns->fruitn_transfer     -= nf->m_fruitn_transfer_to_fire;
	
	/* TREE-specific and NON-WOODY SPECIFIC fluxes */
	if (epc->woody)
	{
		/*    Stem wood mortality */
		ns->fire_snk   += nf->m_livestemn_to_fire;
		ns->livestemn  -= nf->m_livestemn_to_fire;
		ns->fire_snk   += nf->m_deadstemn_to_fire;
		ns->deadstemn  -= nf->m_deadstemn_to_fire;
		ns->fire_snk   += nf->m_livecrootn_to_fire;
		ns->livecrootn -= nf->m_livecrootn_to_fire;
		ns->fire_snk   += nf->m_deadcrootn_to_fire;
		ns->deadcrootn -= nf->m_deadcrootn_to_fire;
	}
	else 	/* SOFT STEM SIMULATION of non-woody biomes - Hidy 2015 */

	{
		ns->fire_snk			+= nf->m_softstemn_to_fire;
		ns->softstemn			-= nf->m_softstemn_to_fire;
		ns->fire_snk            += nf->m_softstemn_storage_to_fire;
		ns->softstemn_storage   -= nf->m_softstemn_storage_to_fire;
		ns->fire_snk            += nf->m_softstemn_transfer_to_fire;
		ns->softstemn_transfer  -= nf->m_softstemn_transfer_to_fire;
	
	}
	/* litter and CWD nitrogen state updates */
	/* Hidy 2016 - fire addects only top soil layer */
	
	ns->fire_snk    += nf->m_litr1n_to_fire;
	ns->litr1n[0]   -= nf->m_litr1n_to_fire;
	ns->fire_snk	+= nf->m_litr2n_to_fire;
	ns->litr2n[0]   -= nf->m_litr2n_to_fire;
	ns->fire_snk	+= nf->m_litr3n_to_fire;
	ns->litr3n[0]   -= nf->m_litr3n_to_fire;
	ns->fire_snk	+= nf->m_litr4n_to_fire;
	ns->litr4n[0]   -= nf->m_litr4n_to_fire;
	ns->fire_snk	+= nf->m_cwdn_to_fire;
	ns->cwdn[0]     -= nf->m_cwdn_to_fire;
	
	return (!ok);
}

