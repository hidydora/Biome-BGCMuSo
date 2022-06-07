/* 
harvesting.c
do harvesting  - decrease the plant material (leafc, leafn, canopy water)

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
BBGC MuSo v3.0.8
Copyright 2014, D. Hidy
Hungarian Academy of Sciences
*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "ini.h"     
#include "pointbgc_struct.h"
#include "bgc_struct.h"
#include "pointbgc_func.h"
#include "bgc_constants.h"

int harvesting(const control_struct* ctrl, const epconst_struct* epc, harvesting_struct* HRV, 
			cflux_struct* cf, nflux_struct* nf, wflux_struct* wf,  cstate_struct* cs, nstate_struct* ns, wstate_struct* ws)
{
	/* harvesting parameters */
	double remained_prop;					/* remained proportion of plant  */
	double LAI_snag;
	double befharv_LAI;						/* value of LAI before harvesting */
	double HRVcoeff_leaf, HRVcoeff_fruit;	/* decrease of plant material caused by harvest: difference between plant material before and after harvesting */
	double HRV_to_litr1c_strg, HRV_to_litr2c_strg, HRV_to_litr3c_strg, HRV_to_litr4c_strg, HRV_to_transpC;
	double HRV_to_litr1n_strg, HRV_to_litr2n_strg, HRV_to_litr3n_strg, HRV_to_litr4n_strg, HRV_to_transpN;
	double diffC, diffN;
	/* local parameters */
	int ok = 1;
	int ny;
	int mgmd = HRV->mgmd;
	double HRV_to_litter_coeff = epc->mort_CnW_to_litter;

	/* test variable */
	double belowbiom_MGMmort=epc->belowbiom_MGMmort;


	/* yearly varied or constant management parameters */
	if(HRV->HRV_flag == 2)
	{
		ny = ctrl->simyr;
	}
	else ny=0;

	/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                                                    CALCULATING FLUXES 
	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

	/* harvesting if gapflag=1 */
	if (mgmd >= 0)
	{ 
		remained_prop = (100 - HRV->transport_coeff_array[mgmd][ny])/100.; /* remained prop. of plant mat.is calculated from transport coeff. */
		LAI_snag = HRV->LAI_snag_array[mgmd][ny];
		
	
		/* if before harvesting the value of the LAI is less than LAI_snag (limit value) - > no harvesting
	   if harvest: plant material decreases as the rate of "harvest effect", which is th ratio of LAI before harvest and LAI snag */
			
		befharv_LAI = cs->leafc * epc->avg_proj_sla;
		if (befharv_LAI > LAI_snag)
		{	
			HRVcoeff_leaf  = 1. - (LAI_snag/befharv_LAI);
			HRVcoeff_fruit = 1.0;
		
		}
		else
		{
			HRVcoeff_leaf = 0.0;
			HRVcoeff_fruit = 0.0;
			if (ctrl->onscreen) printf("value of LAI before harvesting is less than LAI_snag - no harvest\n");
		}
	}
	else
	{
		HRVcoeff_leaf  = 0.0;
		HRVcoeff_fruit = 0.0;
		remained_prop = 0.0;
	}

	/**********************************************************************************************/
	/* 1. as results of the harvesting the carbon, nitrogen and water content of the leaf decreases*/
	/* fruit simulation - Hidy 2013.: harvested fruit is transported from the site  */
	
	cf->leafc_to_HRV          = cs->leafc * HRVcoeff_leaf;
	cf->leafc_storage_to_HRV  = cs->leafc_storage * HRVcoeff_leaf * belowbiom_MGMmort;
	cf->leafc_transfer_to_HRV = cs->leafc_transfer * HRVcoeff_leaf * belowbiom_MGMmort;

	cf->fruitc_to_HRV          = cs->fruitc * HRVcoeff_fruit;
	cf->fruitc_storage_to_HRV  = cs->fruitc_storage * HRVcoeff_fruit;
	cf->fruitc_transfer_to_HRV = cs->fruitc_transfer * HRVcoeff_fruit;

	cf->gresp_storage_to_HRV  = cs->gresp_storage * HRVcoeff_leaf * belowbiom_MGMmort;
	cf->gresp_transfer_to_HRV = cs->gresp_transfer * HRVcoeff_leaf * belowbiom_MGMmort;

	nf->leafn_to_HRV          = ns->leafn * HRVcoeff_leaf;
	nf->leafn_storage_to_HRV  = ns->leafn_storage * HRVcoeff_leaf * belowbiom_MGMmort;
	nf->leafn_transfer_to_HRV = ns->leafn_transfer * HRVcoeff_leaf * belowbiom_MGMmort;

	nf->fruitn_to_HRV          = ns->fruitn * HRVcoeff_fruit;
	nf->fruitn_storage_to_HRV  = ns->fruitn_storage * HRVcoeff_fruit;
	nf->fruitn_transfer_to_HRV = ns->fruitn_transfer * HRVcoeff_fruit;

	nf->retransn_to_HRV        = ns->retransn * HRVcoeff_leaf * belowbiom_MGMmort;

	wf->canopyw_to_HRV = ws->canopyw * HRVcoeff_leaf;

    /**********************************************************************************************/
	/* 2. part of the plant material is transported (HRV_to_transpC and HRV_to_transpN; transp_coeff = 1-remained_prop),
	      the rest remains at the site (HRV_to_litrc_strg, HRV_to_litrn_strg)*/

	HRV_to_transpC = (cf->leafc_to_HRV + cf->leafc_transfer_to_HRV + cf->leafc_storage_to_HRV +
					  cf->gresp_storage_to_HRV + cf->gresp_storage_to_HRV) * (1-remained_prop) +
				      cf->fruitc_to_HRV + cf->fruitc_transfer_to_HRV + cf->fruitc_storage_to_HRV;

	HRV_to_transpN = (nf->leafn_to_HRV + nf->leafn_transfer_to_HRV + nf->leafn_storage_to_HRV + nf->retransn_to_HRV) * (1-remained_prop) +
				      nf->fruitn_to_HRV + nf->fruitn_transfer_to_HRV + nf->fruitn_storage_to_HRV;
	
	HRV_to_litr1c_strg = (cf->leafc_to_HRV * epc->leaflitr_flab   + cf->leafc_transfer_to_HRV + cf->leafc_storage_to_HRV + 
						  cf->gresp_storage_to_HRV  + cf->gresp_transfer_to_HRV) * remained_prop;
	HRV_to_litr2c_strg = (cf->leafc_to_HRV * epc->leaflitr_fucel) * remained_prop;
	HRV_to_litr3c_strg = (cf->leafc_to_HRV * epc->leaflitr_fscel) * remained_prop;
	HRV_to_litr4c_strg = (cf->leafc_to_HRV * epc->leaflitr_flig) * remained_prop;


	HRV_to_litr1n_strg =  (nf->leafn_to_HRV * epc->leaflitr_flab   + nf->leafn_transfer_to_HRV + cf->leafc_storage_to_HRV +
							nf->retransn_to_HRV) * remained_prop;
	HRV_to_litr2n_strg =  (nf->leafn_to_HRV * epc->leaflitr_fucel) * remained_prop;
	HRV_to_litr3n_strg =  (nf->leafn_to_HRV * epc->leaflitr_fscel) * remained_prop;
	HRV_to_litr4n_strg =  (nf->leafn_to_HRV * epc->leaflitr_flig) * remained_prop;
   

	cs->litr1c_strg_HRV += HRV_to_litr1c_strg;
	cs->litr2c_strg_HRV += HRV_to_litr2c_strg;
	cs->litr3c_strg_HRV += HRV_to_litr3c_strg;
	cs->litr4c_strg_HRV += HRV_to_litr4c_strg;

	cs->HRV_transportC  += HRV_to_transpC;

	ns->litr1n_strg_HRV +=  HRV_to_litr1n_strg;
	ns->litr2n_strg_HRV +=  HRV_to_litr2n_strg;
	ns->litr3n_strg_HRV +=  HRV_to_litr3n_strg;
	ns->litr4n_strg_HRV +=  HRV_to_litr4n_strg;

	ns->HRV_transportN  += HRV_to_transpN;

	/**********************************************************************************************/
	/* 3.the remained part of the plant material gradually goes into the litter pool (litrc_strg_HRV, litrn_strg_HRV) */
 
	cf->HRV_to_litr1c = cs->litr1c_strg_HRV * HRV_to_litter_coeff;
	cf->HRV_to_litr2c = cs->litr2c_strg_HRV * HRV_to_litter_coeff;
	cf->HRV_to_litr3c = cs->litr3c_strg_HRV * HRV_to_litter_coeff;
	cf->HRV_to_litr4c = cs->litr4c_strg_HRV * HRV_to_litter_coeff;

	nf->HRV_to_litr1n = ns->litr1n_strg_HRV * HRV_to_litter_coeff;
	nf->HRV_to_litr2n = ns->litr2n_strg_HRV * HRV_to_litter_coeff;
	nf->HRV_to_litr3n = ns->litr3n_strg_HRV * HRV_to_litter_coeff;
	nf->HRV_to_litr4n = ns->litr4n_strg_HRV * HRV_to_litter_coeff;
	

	/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                                                    STATE UPDATE 
	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 

	/* 1. carbon */
	cs->HRVsnk += cf->leafc_to_HRV;
	cs->leafc -= cf->leafc_to_HRV;
	cs->HRVsnk += cf->leafc_transfer_to_HRV;
	cs->leafc_transfer -= cf->leafc_transfer_to_HRV;
	cs->HRVsnk += cf->leafc_storage_to_HRV;
	cs->leafc_storage -= cf->leafc_storage_to_HRV;
	cs->HRVsnk += cf->gresp_transfer_to_HRV;
	cs->gresp_transfer -= cf->gresp_transfer_to_HRV;
	cs->HRVsnk += cf->gresp_storage_to_HRV;
	cs->gresp_storage -= cf->gresp_storage_to_HRV;
	/* fruit simulation - Hidy 2013. */
	cs->HRVsnk += cf->fruitc_to_HRV;
	cs->fruitc -= cf->fruitc_to_HRV;
	cs->HRVsnk += cf->fruitc_transfer_to_HRV;
	cs->fruitc_transfer -= cf->fruitc_transfer_to_HRV;
	cs->HRVsnk += cf->fruitc_storage_to_HRV;
	cs->fruitc_storage -= cf->fruitc_storage_to_HRV;


	cs->litr1c += cf->HRV_to_litr1c;
	cs->litr2c += cf->HRV_to_litr2c;
	cs->litr3c += cf->HRV_to_litr3c;
	cs->litr4c += cf->HRV_to_litr4c;
	
       /* decreasing litter storage state variables*/
	cs->litr1c_strg_HRV -= cf->HRV_to_litr1c;
	cs->litr2c_strg_HRV -= cf->HRV_to_litr2c;
	cs->litr3c_strg_HRV -= cf->HRV_to_litr3c;
	cs->litr4c_strg_HRV -= cf->HRV_to_litr4c;

	/* litter plus because of mowing and returning of dead plant material */
	cs->HRVsrc += cf->HRV_to_litr1c + cf->HRV_to_litr2c + cf->HRV_to_litr3c + cf->HRV_to_litr4c;


	/* 2. nitrogen */
	ns->HRVsnk += nf->leafn_to_HRV;
	ns->leafn -= nf->leafn_to_HRV;
	ns->HRVsnk += nf->leafn_transfer_to_HRV;
	ns->leafn_transfer -= nf->leafn_transfer_to_HRV;
	ns->HRVsnk += nf->leafn_storage_to_HRV;
	ns->leafn_storage -= nf->leafn_storage_to_HRV;
	/* fruit simulation - Hidy 2013. */
	ns->HRVsnk += nf->fruitn_to_HRV;
	ns->fruitn -= nf->fruitn_to_HRV;
	ns->HRVsnk += nf->fruitn_transfer_to_HRV;
	ns->fruitn_transfer -= nf->fruitn_transfer_to_HRV;
	ns->HRVsnk += nf->fruitn_storage_to_HRV;
	ns->fruitn_storage -= nf->fruitn_storage_to_HRV;
	ns->HRVsnk += nf->retransn_to_HRV;
	ns->retransn -= nf->retransn_to_HRV;

	ns->litr1n += nf->HRV_to_litr1n;
	ns->litr2n += nf->HRV_to_litr2n;
	ns->litr3n += nf->HRV_to_litr3n;
	ns->litr4n += nf->HRV_to_litr4n;

	/* decreasing litter storage state variables*/
	ns->litr1n_strg_HRV -= nf->HRV_to_litr1n;
	ns->litr2n_strg_HRV -= nf->HRV_to_litr2n;
	ns->litr3n_strg_HRV -= nf->HRV_to_litr3n;
	ns->litr4n_strg_HRV -= nf->HRV_to_litr4n;

	ns->HRVsrc += nf->HRV_to_litr1n + nf->HRV_to_litr2n + nf->HRV_to_litr3n + nf->HRV_to_litr4n;

	/* 3. water */
	ws->canopyw_HRVsnk += wf->canopyw_to_HRV;
	ws->canopyw -= wf->canopyw_to_HRV;

		/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                                                    TEMPORARY POOLS
	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
	/* temporary HRVed plant material pools: if litr1c_strg_HRV is less than a crit. value, the temporary pool becomes empty */

	if (cs->litr1c_strg_HRV < CRIT_PREC && cs->litr1c_strg_HRV != 0) 
	{
		cs->HRVsrc += cs->litr1c_strg_HRV + cs->litr2c_strg_HRV + cs->litr3c_strg_HRV + cs->litr4c_strg_HRV;
		cs->litr1c_strg_HRV = 0;
		cs->litr2c_strg_HRV = 0;
		cs->litr3c_strg_HRV = 0;
		cs->litr4c_strg_HRV = 0;
		ns->HRVsrc += ns->litr1n_strg_HRV + ns->litr2n_strg_HRV + ns->litr3n_strg_HRV + ns->litr4n_strg_HRV;
		ns->litr1n_strg_HRV = 0;
		ns->litr2n_strg_HRV = 0;
		ns->litr3n_strg_HRV = 0;
		ns->litr4n_strg_HRV = 0;
	}
		
	/* CONTROL */
	diffC = (cs->HRVsnk-cs->HRVsrc) - cs->HRV_transportC - 
			(cs->litr1c_strg_HRV+cs->litr2c_strg_HRV+cs->litr3c_strg_HRV+cs->litr4c_strg_HRV);

	diffN = (ns->HRVsnk-ns->HRVsrc) - ns->HRV_transportN - 
		    (ns->litr1n_strg_HRV+ns->litr2n_strg_HRV+ns->litr3n_strg_HRV+ns->litr4n_strg_HRV) ;

	if (fabs(diffC) > CRIT_PREC && fabs(diffN) > CRIT_PREC)
	{
		printf("Warning: small rounding error in harvesting pools (harvesting.c)\n");
	}
	

   return (!ok);
}
	