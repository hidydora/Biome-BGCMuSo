/* 
mowing.c
do mowing  - decrease the plant material (leafc, leafn, canopy water)

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

int mowing(const control_struct* ctrl, const epconst_struct* epc, mowing_struct* MOW, cflux_struct* cf, nflux_struct* nf, wflux_struct* wf,
				  cstate_struct* cs, nstate_struct* ns, wstate_struct* ws)
{
	/* mowing parameters */
	double LAI_limit;
	double remained_prop;						/* remained proportion of plabnt material is calculated from transport coefficient */
	double MOW_to_litr1c_strg, MOW_to_litr2c_strg, MOW_to_litr3c_strg, MOW_to_litr4c_strg, MOW_to_transpC;
	double MOW_to_litr1n_strg, MOW_to_litr2n_strg, MOW_to_litr3n_strg, MOW_to_litr4n_strg, MOW_to_transpN;
	double diffC, diffN;
	/* local parameters */
	double befgrass_LAI;						/* value of LAI before mowing */
	double MOWcoeff;							/* coefficient determining the decrease of plant material caused by mowing */

	int ok=1;
	int ny;
	int mgmd = MOW->mgmd;
	double MOW_to_litter_coeff = epc->mort_CnW_to_litter;


	/* test variable */
	double belowbiom_MGMmort=epc->belowbiom_MGMmort;

	/* yearly varied or constant management parameters */
	if(MOW->MOW_flag == 2)
	{
		ny = ctrl->simyr;
	}
	else ny=0;

	/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                                                    CALCULATING FLUXES 
	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
	

	/* 1. mowing calculation based on LAI_limit: we assume that due to the mowing LAI (leafc) reduces to the value of LAI_limit  */
	befgrass_LAI = cs->leafc * epc->avg_proj_sla;

	/* 2. mowing type: fixday or LAIlimit */
	if (MOW->fixday_or_fixLAI_flag == 0 || MOW->MOW_flag == 2)
	{	
		if (mgmd >=0) 
		{
			remained_prop = (100 - MOW->transport_coeff_array[mgmd][ny])/100.;
			LAI_limit = MOW->LAI_limit_array[mgmd][ny];
		}
		else 
		{
			remained_prop = 0;
			LAI_limit = befgrass_LAI;
		}
	}
	else
	{
		/* mowing if LAI greater than a given value (fixLAI_befMOW) */
		if (cs->leafc * epc->avg_proj_sla > MOW->fixLAI_befMOW)
		{
			remained_prop = (100 - MOW->transport_coeff_array[0][0])/100.;
			LAI_limit = MOW->fixLAI_aftMOW;
		}
		else
		{
			remained_prop = 0;
			LAI_limit = befgrass_LAI;
		}
	}

	/* 3. effect of mowing: MOWcoeff */
	if (befgrass_LAI > LAI_limit)
	{	
		MOWcoeff = (1- LAI_limit/befgrass_LAI);
	}
	else
	{
		/* if LAI before mowing is less than LAI_limit_aftermowing -> no  mowing  */
		MOWcoeff  = 0.0;
	}	
					

	/**********************************************************************************************/
	/* 1. as results of the mowing the carbon, nitrogen and water content of the leaf decreases*/
	/* fruit simulation - Hidy 2013.: harvested fruit is transported from the site  */
	

	cf->leafc_to_MOW          = cs->leafc * MOWcoeff;
	cf->leafc_transfer_to_MOW = cs->leafc_transfer * MOWcoeff * belowbiom_MGMmort;
	cf->leafc_storage_to_MOW  = cs->leafc_storage * MOWcoeff * belowbiom_MGMmort;

	cf->fruitc_to_MOW          = cs->fruitc * MOWcoeff;
	cf->fruitc_transfer_to_MOW = cs->fruitc_transfer * MOWcoeff * belowbiom_MGMmort;
	cf->fruitc_storage_to_MOW  = cs->fruitc_storage * MOWcoeff * belowbiom_MGMmort;
	
	cf->gresp_transfer_to_MOW = cs->gresp_transfer * MOWcoeff * belowbiom_MGMmort;
	cf->gresp_storage_to_MOW  = cs->gresp_storage * MOWcoeff * belowbiom_MGMmort;

	nf->leafn_to_MOW          = ns->leafn * MOWcoeff;
	nf->leafn_transfer_to_MOW = ns->leafn_transfer * MOWcoeff * belowbiom_MGMmort;
	nf->leafn_storage_to_MOW  = ns->leafn_storage * MOWcoeff * belowbiom_MGMmort;
   
	/**********************************************************************************************/
	/* 2. part of the plant material is transported (MOW_to_transpC and MOW_to_transpN; transp_coeff = 1-remained_prop),
	      the rest remains at the site (MOW_to_litrc_strg, MOW_to_litrn_strg)*/

	MOW_to_transpC = (cf->leafc_to_MOW + cf->leafc_transfer_to_MOW + cf->leafc_storage_to_MOW +
				      cf->fruitc_to_MOW + cf->fruitc_transfer_to_MOW + cf->fruitc_storage_to_MOW + 
					  cf->gresp_storage_to_MOW  + cf->gresp_transfer_to_MOW)                       * (1-remained_prop);

	MOW_to_transpN = (nf->leafn_to_MOW + nf->leafn_transfer_to_MOW + nf->leafn_storage_to_MOW +
				      nf->fruitn_to_MOW + nf->fruitn_transfer_to_MOW + nf->fruitn_storage_to_MOW + 
					  nf->retransn_to_MOW)															* (1-remained_prop);
	
	MOW_to_litr1c_strg = (cf->leafc_to_MOW * epc->leaflitr_flab   + cf->leafc_transfer_to_MOW  + cf->leafc_storage_to_MOW + 
		                  cf->fruitc_to_MOW* epc->fruitlitr_flab  + cf->fruitc_transfer_to_MOW + cf->fruitc_storage_to_MOW + 
						  cf->gresp_storage_to_MOW  + cf->gresp_transfer_to_MOW) * remained_prop;
	MOW_to_litr2c_strg = (cf->leafc_to_MOW * epc->leaflitr_fucel  + cf->fruitc_to_MOW * epc->fruitlitr_fucel) * remained_prop;
	MOW_to_litr3c_strg = (cf->leafc_to_MOW * epc->leaflitr_fscel  + cf->fruitc_to_MOW * epc->fruitlitr_fscel) * remained_prop;
	MOW_to_litr4c_strg = (cf->leafc_to_MOW * epc->leaflitr_flig   + cf->fruitc_to_MOW * epc->fruitlitr_flig) * remained_prop;


	MOW_to_litr1n_strg =  (nf->leafn_to_MOW * epc->leaflitr_flab  + nf->leafn_transfer_to_MOW  + nf->leafn_storage_to_MOW +
		                   nf->fruitn_to_MOW* epc->fruitlitr_flab + nf->fruitn_transfer_to_MOW + nf->fruitn_storage_to_MOW +
						   nf->retransn_to_MOW) * remained_prop;
	MOW_to_litr2n_strg =  (nf->leafn_to_MOW * epc->leaflitr_fucel + nf->fruitn_to_MOW * epc->fruitlitr_fucel) * remained_prop;
	MOW_to_litr3n_strg =  (nf->leafn_to_MOW * epc->leaflitr_fscel + nf->fruitn_to_MOW * epc->fruitlitr_fscel) * remained_prop;
	MOW_to_litr4n_strg =  (nf->leafn_to_MOW * epc->leaflitr_flig  + nf->fruitn_to_MOW * epc->fruitlitr_flig) * remained_prop;
   

	cs->litr1c_strg_MOW += MOW_to_litr1c_strg;
	cs->litr2c_strg_MOW += MOW_to_litr2c_strg;
	cs->litr3c_strg_MOW += MOW_to_litr3c_strg;
	cs->litr4c_strg_MOW += MOW_to_litr4c_strg;

	cs->MOW_transportC  += MOW_to_transpC;


	ns->litr1n_strg_MOW +=  MOW_to_litr1n_strg;
	ns->litr2n_strg_MOW +=  MOW_to_litr2n_strg;
	ns->litr3n_strg_MOW +=  MOW_to_litr3n_strg;
	ns->litr4n_strg_MOW +=  MOW_to_litr4n_strg;

	ns->MOW_transportN  += MOW_to_transpN;

	/**********************************************************************************************/
	/* 3.the remained part of the plant material gradually goes into the litter pool (litrc_strg_MOW, litrn_strg_MOW) */
 
	cf->MOW_to_litr1c = cs->litr1c_strg_MOW * MOW_to_litter_coeff;
	cf->MOW_to_litr2c = cs->litr2c_strg_MOW * MOW_to_litter_coeff;
	cf->MOW_to_litr3c = cs->litr3c_strg_MOW * MOW_to_litter_coeff;
	cf->MOW_to_litr4c = cs->litr4c_strg_MOW * MOW_to_litter_coeff;

	nf->MOW_to_litr1n = ns->litr1n_strg_MOW * MOW_to_litter_coeff;
	nf->MOW_to_litr2n = ns->litr2n_strg_MOW * MOW_to_litter_coeff;
	nf->MOW_to_litr3n = ns->litr3n_strg_MOW * MOW_to_litter_coeff;
	nf->MOW_to_litr4n = ns->litr4n_strg_MOW * MOW_to_litter_coeff;
	

	/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                                                    STATE UPDATE 
	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 

	/* 1. carbon */
	cs->MOWsnk += cf->leafc_to_MOW;
	cs->leafc -= cf->leafc_to_MOW;
	cs->MOWsnk += cf->leafc_transfer_to_MOW;
	cs->leafc_transfer -= cf->leafc_transfer_to_MOW;
	cs->MOWsnk += cf->leafc_storage_to_MOW;
	cs->leafc_storage -= cf->leafc_storage_to_MOW;
	cs->MOWsnk += cf->gresp_transfer_to_MOW;
	cs->gresp_transfer -= cf->gresp_transfer_to_MOW;
	cs->MOWsnk += cf->gresp_storage_to_MOW;
	cs->gresp_storage -= cf->gresp_storage_to_MOW;
	/* fruit simulation - Hidy 2013. */
	cs->MOWsnk += cf->fruitc_to_MOW;
	cs->fruitc -= cf->fruitc_to_MOW;
	cs->MOWsnk += cf->fruitc_transfer_to_MOW;
	cs->fruitc_transfer -= cf->fruitc_transfer_to_MOW;
	cs->MOWsnk += cf->fruitc_storage_to_MOW;
	cs->fruitc_storage -= cf->fruitc_storage_to_MOW;


	cs->litr1c += cf->MOW_to_litr1c;
	cs->litr2c += cf->MOW_to_litr2c;
	cs->litr3c += cf->MOW_to_litr3c;
	cs->litr4c += cf->MOW_to_litr4c;
	
    /* decreasing litter storage state variables*/
	cs->litr1c_strg_MOW -= cf->MOW_to_litr1c;
	cs->litr2c_strg_MOW -= cf->MOW_to_litr2c;
	cs->litr3c_strg_MOW -= cf->MOW_to_litr3c;
	cs->litr4c_strg_MOW -= cf->MOW_to_litr4c;

	/* litter plus because of mowing and returning of dead plant material */
	cs->MOWsrc += cf->MOW_to_litr1c + cf->MOW_to_litr2c + cf->MOW_to_litr3c + cf->MOW_to_litr4c;


	/* 2. nitrogen */
	ns->MOWsnk += nf->leafn_to_MOW;
	ns->leafn -= nf->leafn_to_MOW;
	ns->MOWsnk += nf->leafn_transfer_to_MOW;
	ns->leafn_transfer -= nf->leafn_transfer_to_MOW;
	ns->MOWsnk += nf->leafn_storage_to_MOW;
	ns->leafn_storage -= nf->leafn_storage_to_MOW;
	/* fruit simulation - Hidy 2013. */
	ns->MOWsnk += nf->fruitn_to_MOW;
	ns->fruitn -= nf->fruitn_to_MOW;
	ns->MOWsnk += nf->fruitn_transfer_to_MOW;
	ns->fruitn_transfer -= nf->fruitn_transfer_to_MOW;
	ns->MOWsnk += nf->fruitn_storage_to_MOW;
	ns->fruitn_storage -= nf->fruitn_storage_to_MOW;
	ns->MOWsnk += nf->retransn_to_MOW;
	ns->retransn -= nf->retransn_to_MOW;

	ns->litr1n += nf->MOW_to_litr1n;
	ns->litr2n += nf->MOW_to_litr2n;
	ns->litr3n += nf->MOW_to_litr3n;
	ns->litr4n += nf->MOW_to_litr4n;



	/* decreasing litter storage state variables*/
	ns->litr1n_strg_MOW -= nf->MOW_to_litr1n;
	ns->litr2n_strg_MOW -= nf->MOW_to_litr2n;
	ns->litr3n_strg_MOW -= nf->MOW_to_litr3n;
	ns->litr4n_strg_MOW -= nf->MOW_to_litr4n;

	ns->MOWsrc += nf->MOW_to_litr1n + nf->MOW_to_litr2n + nf->MOW_to_litr3n + nf->MOW_to_litr4n;


	/* 3. water */
	ws->canopyw_MOWsnk += wf->canopyw_to_MOW;
	ws->canopyw -= wf->canopyw_to_MOW;

	/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                                                    TEMPORARY POOLS
	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
	/* temporary MOWed plant material pools: if litr1c_strg_MOW is less than a crit. value, the temporary pool becomes empty */

	if (cs->litr1c_strg_MOW < CRIT_PREC && cs->litr1c_strg_MOW != 0) 
	{
		cs->MOWsrc += (cs->litr1c_strg_MOW + cs->litr2c_strg_MOW + cs->litr3c_strg_MOW + cs->litr4c_strg_MOW);
		cs->litr1c_strg_MOW = 0;
		cs->litr2c_strg_MOW = 0;
		cs->litr3c_strg_MOW = 0;
		cs->litr4c_strg_MOW = 0;
		ns->MOWsrc += (ns->litr1n_strg_MOW + ns->litr2n_strg_MOW + ns->litr3n_strg_MOW + ns->litr4n_strg_MOW);
		ns->litr1n_strg_MOW = 0;
		ns->litr2n_strg_MOW = 0;
		ns->litr3n_strg_MOW = 0;
		ns->litr4n_strg_MOW = 0;
	}
		
	/* CONTROL */
	diffC = (cs->MOWsnk-cs->MOWsrc) - cs->MOW_transportC - 
		    (cs->litr1c_strg_MOW+cs->litr2c_strg_MOW+cs->litr3c_strg_MOW+cs->litr4c_strg_MOW);

	diffN = (ns->MOWsnk-ns->MOWsrc) - ns->MOW_transportN - 
		    (ns->litr1n_strg_MOW+ns->litr2n_strg_MOW+ns->litr3n_strg_MOW+ns->litr4n_strg_MOW);

	if (fabs(diffC) > CRIT_PREC && fabs(diffN) > CRIT_PREC)
	{
	 	printf("Warning: small rounding error in mowing pools (mowing.c)\n");
	}
	


  return (!ok);
}	