/* 
mowing.c
do mowing  - decrease the plant material (leafc, leafn, canopy water)

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
BBGC MuSo v4
Copyright 2014, D. Hidy (dori.hidy@gmail.com)
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
	double LAI_limit, outc, outn, inc, inn, MOW_to_transpC, MOW_to_transpN;
	double remained_prop;						/* remained proportion of plabnt material is calculated from transport coefficient */

	/* local parameters */
	double befgrass_LAI;						/* value of LAI before mowing */
	double MOWcoeff;							/* coefficient determining the decrease of plant material caused by mowing */
	int ok=1;
	int ny;
	int mgmd = MOW->mgmd;

	/* test variable */
	double storage_MGMmort=epc->storage_MGMmort;
	LAI_limit=MOW_to_transpC=MOW_to_transpN=outc=outn=inc=inn=0;

	/* yearly varied or constant management parameters */
	if(MOW->MOW_flag == 2)
	{
		ny = ctrl->simyr;
	}
	else ny=0;


	/**********************************************************************************************/
	/* I. CALCULATING: MOWcoeff */
	
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
	/* II. CALCULATING FLUXES */

	/* 1. as results of the mowing the carbon, nitrogen and water content of the leaf decreases*/
	
	/* 1.1.1 Carbon: leaf, frut, softstem, gresp */
	cf->leafc_to_MOW          = cs->leafc * MOWcoeff;
	cf->leafc_transfer_to_MOW = cs->leafc_transfer * MOWcoeff * storage_MGMmort;
	cf->leafc_storage_to_MOW  = cs->leafc_storage * MOWcoeff * storage_MGMmort;

	cf->fruitc_to_MOW          = cs->fruitc * MOWcoeff;
	cf->fruitc_transfer_to_MOW = cs->fruitc_transfer * MOWcoeff * storage_MGMmort;
	cf->fruitc_storage_to_MOW  = cs->fruitc_storage * MOWcoeff * storage_MGMmort;

	cf->softstemc_to_MOW          = cs->softstemc * MOWcoeff;
	cf->softstemc_transfer_to_MOW = cs->softstemc_transfer * MOWcoeff * storage_MGMmort;
	cf->softstemc_storage_to_MOW  = cs->softstemc_storage * MOWcoeff * storage_MGMmort;
	
	cf->gresp_transfer_to_MOW = cs->gresp_transfer * MOWcoeff * storage_MGMmort;
	cf->gresp_storage_to_MOW  = cs->gresp_storage * MOWcoeff * storage_MGMmort;

	/* 1.1.1 Carbon: standing dead biome */
	cf->litr1c_STDB_to_MOW = cs->litr1cA_STDB * MOWcoeff;
	cf->litr2c_STDB_to_MOW = cs->litr2cA_STDB * MOWcoeff;
	cf->litr3c_STDB_to_MOW = cs->litr3cA_STDB * MOWcoeff;
	cf->litr4c_STDB_to_MOW = cs->litr4cA_STDB * MOWcoeff;


	/* 1.2.1 Nitrogen: leaf, frut, softstem, retrans */
	nf->leafn_to_MOW          = ns->leafn * MOWcoeff;
	nf->leafn_transfer_to_MOW = ns->leafn_transfer * MOWcoeff * storage_MGMmort;
	nf->leafn_storage_to_MOW  = ns->leafn_storage * MOWcoeff * storage_MGMmort;

	nf->fruitn_to_MOW          = ns->fruitn * MOWcoeff;
	nf->fruitn_transfer_to_MOW = ns->fruitn_transfer * MOWcoeff * storage_MGMmort;
	nf->fruitn_storage_to_MOW  = ns->fruitn_storage * MOWcoeff * storage_MGMmort;

	nf->softstemn_to_MOW          = ns->softstemn * MOWcoeff;
	nf->softstemn_transfer_to_MOW = ns->softstemn_transfer * MOWcoeff * storage_MGMmort;
	nf->softstemn_storage_to_MOW  = ns->softstemn_storage * MOWcoeff * storage_MGMmort;

	nf->retransn_to_MOW           = ns->retransn * MOWcoeff * storage_MGMmort;

	/* 1.2.1 Nitrogen: standing dead biome */
	nf->litr1n_STDB_to_MOW = ns->litr1nA_STDB  * MOWcoeff;
	nf->litr2n_STDB_to_MOW = ns->litr2nA_STDB  * MOWcoeff;
	nf->litr3n_STDB_to_MOW = ns->litr3nA_STDB  * MOWcoeff;
	nf->litr4n_STDB_to_MOW = ns->litr4nA_STDB  * MOWcoeff;


	/*----------------------------------------------------------*/
	/* 2. transport: part of the plant material is transported (MOW_to_transpC and MOW_to_transpN; transp_coeff = 1-remained_prop),*/

	MOW_to_transpC = (cf->leafc_to_MOW + cf->fruitc_to_MOW + cf->softstemc_to_MOW + 
		              cf->litr1c_STDB_to_MOW + cf->litr2c_STDB_to_MOW + cf->litr3c_STDB_to_MOW + cf->litr4c_STDB_to_MOW)  * (1-remained_prop);
	MOW_to_transpN = (nf->leafn_to_MOW + nf->fruitn_to_MOW + nf->softstemn_to_MOW + 
		              nf->litr1n_STDB_to_MOW + nf->litr2n_STDB_to_MOW + nf->litr3n_STDB_to_MOW + nf->litr4n_STDB_to_MOW) 	* (1-remained_prop);


	/*----------------------------------------------------------*/
	/* 3. cut-down biomass: the rest remains at the site (MOW_to_litrc_strg, MOW_to_litrn_strg)*/
	cf->MOW_to_CTDB_litr1cA = (cf->leafc_to_MOW * epc->leaflitr_flab         + cf->fruitc_to_MOW* epc->fruitlitr_flab  + 
							   cf->softstemc_to_MOW * epc->softstemlitr_flab + cf->litr1c_STDB_to_MOW) * remained_prop +
							   cf->leafc_transfer_to_MOW     + cf->leafc_storage_to_MOW + 
							   cf->fruitc_transfer_to_MOW    + cf->fruitc_storage_to_MOW + 
							   cf->softstemc_transfer_to_MOW + cf->softstemc_storage_to_MOW + 
							   cf->gresp_storage_to_MOW      + cf->gresp_transfer_to_MOW;

	cf->MOW_to_CTDB_litr2cA  = (cf->leafc_to_MOW * epc->leaflitr_fucel         + cf->fruitc_to_MOW * epc->fruitlitr_fucel +
								cf->softstemc_to_MOW * epc->softstemlitr_fucel + cf->litr2c_STDB_to_MOW) * remained_prop;

	cf->MOW_to_CTDB_litr3cA  = (cf->leafc_to_MOW * epc->leaflitr_fscel         + cf->fruitc_to_MOW * epc->fruitlitr_fscel +
								cf->softstemc_to_MOW * epc->softstemlitr_fscel + cf->litr3c_STDB_to_MOW) * remained_prop;

	cf->MOW_to_CTDB_litr4cA  = (cf->leafc_to_MOW * epc->leaflitr_flig          + cf->fruitc_to_MOW * epc->fruitlitr_flig +
								cf->softstemc_to_MOW * epc->softstemlitr_flig  + cf->litr4c_STDB_to_MOW) * remained_prop;

	nf->MOW_to_CTDB_litr1nA  =  (nf->leafn_to_MOW * epc->leaflitr_flab         + nf->fruitn_to_MOW * epc->fruitlitr_flab + 
								 nf->softstemn_to_MOW* epc->softstemlitr_flab  + nf->litr1n_STDB_to_MOW) * remained_prop + 
								 nf->leafn_transfer_to_MOW  + nf->leafn_storage_to_MOW + 
								 nf->fruitn_transfer_to_MOW + nf->fruitn_storage_to_MOW +
								 nf->softstemn_transfer_to_MOW + nf->softstemn_storage_to_MOW + 
								 nf->retransn_to_MOW;

	nf->MOW_to_CTDB_litr2nA =  (nf->leafn_to_MOW * epc->leaflitr_fucel         + nf->fruitn_to_MOW * epc->fruitlitr_fucel +
								nf->softstemn_to_MOW * epc->softstemlitr_fucel + nf->litr2n_STDB_to_MOW) * remained_prop;

	nf->MOW_to_CTDB_litr3nA =  (nf->leafn_to_MOW * epc->leaflitr_fscel         + nf->fruitn_to_MOW * epc->fruitlitr_fscel +
								nf->softstemn_to_MOW * epc->softstemlitr_fscel + nf->litr3n_STDB_to_MOW) * remained_prop;

	nf->MOW_to_CTDB_litr4nA =  (nf->leafn_to_MOW * epc->leaflitr_flig          + nf->fruitn_to_MOW * epc->fruitlitr_flig +
								nf->softstemn_to_MOW * epc->softstemlitr_flig  + nf->litr4n_STDB_to_MOW) * remained_prop;


	/**********************************************************************************************/
	/* III. STATE UPDATE */

	/* 1.1.1 Carbon: leaf, frut, softstem, gresp */

	cs->leafc				-= cf->leafc_to_MOW;
	cs->leafc_transfer		-= cf->leafc_transfer_to_MOW;
	cs->leafc_storage		-= cf->leafc_storage_to_MOW;
	cs->fruitc				-= cf->fruitc_to_MOW;
	cs->fruitc_transfer		-= cf->fruitc_transfer_to_MOW;
	cs->fruitc_storage		-= cf->fruitc_storage_to_MOW;
	cs->softstemc			-= cf->softstemc_to_MOW;
	cs->softstemc_transfer -= cf->softstemc_transfer_to_MOW;
	cs->softstemc_storage	-= cf->softstemc_storage_to_MOW;
	cs->gresp_transfer		-= cf->gresp_transfer_to_MOW;
	cs->gresp_storage		-= cf->gresp_storage_to_MOW;

	/* 1.1.2 Carbon: dead standing biomass */
	cs->litr1cA_STDB -= cf->litr1c_STDB_to_MOW;
	cs->litr2cA_STDB -= cf->litr2c_STDB_to_MOW;
	cs->litr3cA_STDB -= cf->litr3c_STDB_to_MOW;
	cs->litr4cA_STDB -= cf->litr4c_STDB_to_MOW;


	/* 1.2.1 Nitrogen: leaf, frut, softstem, retrans */
	ns->leafn				-= nf->leafn_to_MOW;
	ns->leafn_transfer		-= nf->leafn_transfer_to_MOW;
	ns->leafn_storage		-= nf->leafn_storage_to_MOW;
	ns->fruitn				-= nf->fruitn_to_MOW;
	ns->fruitn_transfer		-= nf->fruitn_transfer_to_MOW;
	ns->fruitn_storage		-= nf->fruitn_storage_to_MOW;
	ns->softstemn			-= nf->softstemn_to_MOW;
	ns->softstemn_transfer  -= nf->softstemn_transfer_to_MOW;
	ns->softstemn_storage	-= nf->softstemn_storage_to_MOW;
	ns->retransn			-= nf->retransn_to_MOW;

	/* 1.2.2 Nitrogen:  dead standing biomass */
	ns->litr1nA_STDB	-= nf->litr1n_STDB_to_MOW;
	ns->litr2nA_STDB	-= nf->litr2n_STDB_to_MOW;
	ns->litr3nA_STDB	-= nf->litr3n_STDB_to_MOW;
	ns->litr4nA_STDB	-= nf->litr4n_STDB_to_MOW;

	/* 1.3. water */
	ws->canopyw_MOWsnk += wf->canopyw_to_MOW;
	ws->canopyw -= wf->canopyw_to_MOW;

	/* 2. transport */
	cs->MOW_transportC  += MOW_to_transpC;
	ns->MOW_transportN  += MOW_to_transpN;
	
	/* 3.1: CARBON cut-down biome */
	cs->litr1cA_CTDB += cf->MOW_to_CTDB_litr1cA;
	cs->litr2cA_CTDB += cf->MOW_to_CTDB_litr2cA;
	cs->litr3cA_CTDB += cf->MOW_to_CTDB_litr3cA;
	cs->litr4cA_CTDB += cf->MOW_to_CTDB_litr4cA;

	/* 3.2: NITROGEN cut-down biome */
	ns->litr1nA_CTDB += nf->MOW_to_CTDB_litr1nA;
	ns->litr2nA_CTDB += nf->MOW_to_CTDB_litr2nA;
	ns->litr3nA_CTDB += nf->MOW_to_CTDB_litr3nA;
	ns->litr4nA_CTDB += nf->MOW_to_CTDB_litr4nA;


	/**********************************************************************************************/
	/* IV. CONTROL */

	outc = cf->leafc_to_MOW + cf->leafc_transfer_to_MOW + cf->leafc_storage_to_MOW +
		   cf->fruitc_to_MOW + cf->fruitc_transfer_to_MOW + cf->fruitc_storage_to_MOW +
		   cf->softstemc_to_MOW + cf->softstemc_transfer_to_MOW + cf->softstemc_storage_to_MOW +
		   cf->gresp_storage_to_MOW + cf->gresp_transfer_to_MOW + 
		   cf->litr1c_STDB_to_MOW + cf->litr2c_STDB_to_MOW + cf->litr3c_STDB_to_MOW + cf->litr4c_STDB_to_MOW;

	outn = nf->leafn_to_MOW + nf->leafn_transfer_to_MOW + nf->leafn_storage_to_MOW +
		   nf->fruitn_to_MOW + nf->fruitn_transfer_to_MOW + nf->fruitn_storage_to_MOW +
		   nf->softstemn_to_MOW + nf->softstemn_transfer_to_MOW + nf->softstemn_storage_to_MOW +
		   nf->retransn_to_MOW + 
		   nf->litr1n_STDB_to_MOW + nf->litr2n_STDB_to_MOW + nf->litr3n_STDB_to_MOW + nf->litr4n_STDB_to_MOW;

	inc = cf->MOW_to_CTDB_litr1cA + cf->MOW_to_CTDB_litr2cA  + cf->MOW_to_CTDB_litr3cA + cf->MOW_to_CTDB_litr4cA;
	inn = nf->MOW_to_CTDB_litr1nA + nf->MOW_to_CTDB_litr2nA  + nf->MOW_to_CTDB_litr3nA + nf->MOW_to_CTDB_litr4nA;

	if (fabs(inc + MOW_to_transpC - outc) > CRIT_PREC || fabs(inn + MOW_to_transpN - outn) > CRIT_PREC )
	{
		printf("BALANCE ERROR in mowing calculation in mowing.c\n");
		ok=0;
	}



  return (!ok);
}	