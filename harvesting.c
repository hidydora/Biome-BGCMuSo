/* 
harvesting.c
do harvesting  - decrease the plant material (leafc, leafn, canopy water)

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

int harvesting(const control_struct* ctrl, const epconst_struct* epc, harvesting_struct* HRV, 
			cflux_struct* cf, nflux_struct* nf, wflux_struct* wf,  cstate_struct* cs, nstate_struct* ns, wstate_struct* ws)
{
	/* harvesting parameters */
	double remained_prop;					/* remained proportion of plant  */
	double snag;
	double befharv_stem;						/* value of LAI before harvesting */
	double HRVcoeff_leaf, HRVcoeff_fruit, HRVcoeff_softstem;	/* decrease of plant material caused by harvest: difference between plant material before and after harvesting */
	double outc, outn, inc, inn, HRV_to_transpC, HRV_to_transpN;
	double STDBc_to_HRV, STDBn_to_HRV;
	/* local parameters */
	int ok = 1;
	int ny;
	int mgmd = HRV->mgmd;
	/* test variable */
	double storage_MGMmort=epc->storage_MGMmort;

	HRV_to_transpC=HRV_to_transpN=outc=outn=inc=inn=0;

	/* yearly varied or constant management parameters */
	if(HRV->HRV_flag == 2)
	{
		ny = ctrl->simyr;
	}
	else ny=0;



	/**********************************************************************************************/
	/* I. CALCULATING HRVcoeff */

	/* harvesting if gapflag=1  */
	if (mgmd >= 0)
	{ 
		remained_prop = (100 - HRV->transport_coeff_array[mgmd][ny])/100.; /* remained prop. of plant mat.is calculated from transport coeff. */
		snag = HRV->snag_array[mgmd][ny];
		
	
		/* if before harvesting the value of the LAI is less than snag (limit value) - > no harvesting
	   if harvest: plant material decreases as the rate of "harvest effect", which is th ratio of LAI before harvest and LAI snag */
			
		befharv_stem = cs->softstemc;
		if (befharv_stem > snag && befharv_stem > 0)
		{	
			HRVcoeff_leaf  = 1.0;
			HRVcoeff_fruit = 1.0;
			HRVcoeff_softstem = 1. - (snag/befharv_stem);
		
		}
		else
		{
			HRVcoeff_leaf = 1.0;
			HRVcoeff_fruit = 1.0;
			HRVcoeff_softstem = 1.0;
			if (ctrl->onscreen) printf("sofstem carbon content is less than snag set in INI file\n");
		}
	}
	else
	{
		HRVcoeff_leaf  = 0.0;
		HRVcoeff_fruit = 0.0;
		HRVcoeff_softstem = 0.0;
		remained_prop = 0.0;
	}

	/**********************************************************************************************/
	/* II. CALCULATING FLUXES */

	/* 1. as results of the harvesting the carbon, nitrogen and water content of the leaf decreases*/

	/* 1.1.1 Carbon: leaf, fruit, softstem, gresp */
	cf->leafc_to_HRV          = cs->leafc          * HRVcoeff_leaf;
	cf->leafc_storage_to_HRV  = cs->leafc_storage  * HRVcoeff_leaf * storage_MGMmort;
	cf->leafc_transfer_to_HRV = cs->leafc_transfer * HRVcoeff_leaf * storage_MGMmort;

	cf->fruitc_to_HRV          = cs->fruitc          * HRVcoeff_fruit;
	cf->fruitc_storage_to_HRV  = cs->fruitc_storage  * HRVcoeff_fruit * storage_MGMmort;
	cf->fruitc_transfer_to_HRV = cs->fruitc_transfer * HRVcoeff_fruit * storage_MGMmort;

	cf->softstemc_to_HRV          = cs->softstemc          * HRVcoeff_softstem;
	cf->softstemc_storage_to_HRV  = cs->softstemc_storage  * HRVcoeff_softstem * storage_MGMmort;
	cf->softstemc_transfer_to_HRV = cs->softstemc_transfer * HRVcoeff_softstem * storage_MGMmort;

	cf->gresp_storage_to_HRV  = cs->gresp_storage  * HRVcoeff_leaf * storage_MGMmort;
	cf->gresp_transfer_to_HRV = cs->gresp_transfer * HRVcoeff_leaf * storage_MGMmort;

	/* 1.1.2 Carbon: standing dead biome */
	cf->litr1c_STDB_to_HRV = cs->litr1cA_STDB * HRVcoeff_leaf; 
	cf->litr2c_STDB_to_HRV = cs->litr2cA_STDB * HRVcoeff_leaf;
	cf->litr3c_STDB_to_HRV = cs->litr3cA_STDB * HRVcoeff_leaf;
	cf->litr4c_STDB_to_HRV = cs->litr4cA_STDB * HRVcoeff_leaf;


	STDBc_to_HRV = cf->litr1c_STDB_to_HRV + cf->litr2c_STDB_to_HRV + cf->litr3c_STDB_to_HRV + cf->litr4c_STDB_to_HRV;

	/* 1.2.1 Nitrogen: leaf, fruit, softstem, retrans */
	nf->leafn_to_HRV              = ns->leafn          * HRVcoeff_leaf;
	nf->leafn_storage_to_HRV      = ns->leafn_storage  * HRVcoeff_leaf * storage_MGMmort;
	nf->leafn_transfer_to_HRV     = ns->leafn_transfer * HRVcoeff_leaf * storage_MGMmort;
 
	nf->fruitn_to_HRV             = ns->fruitn          * HRVcoeff_fruit;
	nf->fruitn_storage_to_HRV     = ns->fruitn_storage  * HRVcoeff_fruit * storage_MGMmort;
	nf->fruitn_transfer_to_HRV    = ns->fruitn_transfer * HRVcoeff_fruit * storage_MGMmort;
	
	nf->softstemn_to_HRV          = ns->softstemn          * HRVcoeff_softstem;
	nf->softstemn_storage_to_HRV  = ns->softstemn_storage  * HRVcoeff_softstem * storage_MGMmort;
	nf->softstemn_transfer_to_HRV = ns->softstemn_transfer * HRVcoeff_softstem * storage_MGMmort;
	
	nf->retransn_to_HRV           = ns->retransn * HRVcoeff_leaf * storage_MGMmort;

	/* 1.2.1 Nitrogen: standing dead biome */
	nf->litr1n_STDB_to_HRV = ns->litr1nA_STDB * HRVcoeff_leaf; /* standing dead biome */
	nf->litr2n_STDB_to_HRV = ns->litr2nA_STDB * HRVcoeff_leaf;
	nf->litr3n_STDB_to_HRV = ns->litr3nA_STDB * HRVcoeff_leaf;
	nf->litr4n_STDB_to_HRV = ns->litr4nA_STDB * HRVcoeff_leaf;

	STDBn_to_HRV    = nf->litr1n_STDB_to_HRV + nf->litr2n_STDB_to_HRV + nf->litr3n_STDB_to_HRV + nf->litr4n_STDB_to_HRV;
   
	/* 1.3 Water */
	wf->canopyw_to_HRV   = ws->canopyw * HRVcoeff_leaf;


 	/*----------------------------------------------------------*/
	/* 2. transport: part of the plant material is transported (HRV_to_transpC and HRV_to_transpN; transp_coeff = 1-remained_prop)*/

	HRV_to_transpC = (cf->leafc_to_HRV + cf->softstemc_to_HRV + STDBc_to_HRV) * (1-remained_prop) +
				      cf->fruitc_to_HRV;
	HRV_to_transpN = (nf->leafn_to_HRV + nf->softstemn_to_HRV + STDBn_to_HRV) * (1-remained_prop) +
				      nf->fruitn_to_HRV;

	/*----------------------------------------------------------*/
	/* 3. cut-down biomass: the rest remains at the site (HRV_to_litrc_strg, HRV_to_litrn_strg)*/

	cf->HRV_to_CTDB_litr1cA = (cf->leafc_to_HRV * epc->leaflitr_flab + cf->softstemc_to_HRV * epc->softstemlitr_flab + 
		                       cf->litr1c_STDB_to_HRV) * remained_prop +
							   cf->leafc_transfer_to_HRV     + cf->leafc_storage_to_HRV + 
							   cf->fruitc_transfer_to_HRV     + cf->fruitc_storage_to_HRV + 
							   cf->softstemc_transfer_to_HRV + cf->softstemc_storage_to_HRV + 
							   cf->gresp_storage_to_HRV      + cf->gresp_transfer_to_HRV;

	cf->HRV_to_CTDB_litr2cA  = (cf->leafc_to_HRV * epc->leaflitr_fucel + cf->softstemc_to_HRV * epc->softstemlitr_fucel + 
								cf->litr2c_STDB_to_HRV) * remained_prop;

    cf->HRV_to_CTDB_litr3cA  = (cf->leafc_to_HRV * epc->leaflitr_fscel + cf->softstemc_to_HRV * epc->softstemlitr_fscel +
								cf->litr3c_STDB_to_HRV) * remained_prop;

	cf->HRV_to_CTDB_litr4cA  = (cf->leafc_to_HRV * epc->leaflitr_flig  + cf->softstemc_to_HRV * epc->softstemlitr_flig +
								cf->litr4c_STDB_to_HRV) * remained_prop;


	nf->HRV_to_CTDB_litr1nA  = (nf->leafn_to_HRV  * epc->leaflitr_flab + nf->softstemn_to_HRV * epc->softstemlitr_flab  + 
		                        nf->litr1n_STDB_to_HRV) * remained_prop +
							    nf->leafn_transfer_to_HRV     + nf->leafn_storage_to_HRV + 
								nf->fruitn_transfer_to_HRV    + nf->fruitn_storage_to_HRV + 
								nf->softstemn_transfer_to_HRV + nf->softstemn_storage_to_HRV +
								nf->retransn_to_HRV;

	nf->HRV_to_CTDB_litr2nA  = (nf->leafn_to_HRV * epc->leaflitr_fucel + nf->softstemn_to_HRV * epc->softstemlitr_fucel +
								nf->litr2n_STDB_to_HRV) * remained_prop;
	
	nf->HRV_to_CTDB_litr3nA  = (nf->leafn_to_HRV * epc->leaflitr_fscel + nf->softstemn_to_HRV * epc->softstemlitr_fscel +
								nf->litr3n_STDB_to_HRV) * remained_prop;

	nf->HRV_to_CTDB_litr4nA  = (nf->leafn_to_HRV * epc->leaflitr_flig  + nf->softstemn_to_HRV * epc->softstemlitr_flig +
								nf->litr4n_STDB_to_HRV) * remained_prop;
  


	/**********************************************************************************************/
	/* III. STATE UPDATE */

	/* 1. OUT */
	/* 1.1.1 Carbon: leaf, fruit, softstem, gresp */
	cs->leafc				-= cf->leafc_to_HRV;
	cs->leafc_transfer		-= cf->leafc_transfer_to_HRV;
	cs->leafc_storage		-= cf->leafc_storage_to_HRV;
	cs->fruitc				-= cf->fruitc_to_HRV;
	cs->fruitc_transfer		-= cf->fruitc_transfer_to_HRV;
	cs->fruitc_storage		-= cf->fruitc_storage_to_HRV;
	cs->softstemc			-= cf->softstemc_to_HRV;
	cs->softstemc_transfer	-= cf->softstemc_transfer_to_HRV;
	cs->softstemc_storage	-= cf->softstemc_storage_to_HRV;
	cs->gresp_transfer		-= cf->gresp_transfer_to_HRV;
	cs->gresp_storage		-= cf->gresp_storage_to_HRV;

	/* 1.1.2 Carbon: dead standing biomass */
	cs->litr1cA_STDB     -= cf->litr1c_STDB_to_HRV;
	cs->litr2cA_STDB     -= cf->litr2c_STDB_to_HRV;
	cs->litr3cA_STDB     -= cf->litr3c_STDB_to_HRV;
	cs->litr4cA_STDB     -= cf->litr4c_STDB_to_HRV;


	
	/* 1.2.1 Nitrogen: leaf, fruit, softstem, retrans */
	ns->leafn				-= nf->leafn_to_HRV;
	ns->leafn_transfer		-= nf->leafn_transfer_to_HRV;
	ns->leafn_storage		-= nf->leafn_storage_to_HRV;
	ns->fruitn				-= nf->fruitn_to_HRV;
	ns->fruitn_transfer		-= nf->fruitn_transfer_to_HRV;
	ns->fruitn_storage		-= nf->fruitn_storage_to_HRV;
	ns->softstemn			-= nf->softstemn_to_HRV;
	ns->softstemn_transfer	-= nf->softstemn_transfer_to_HRV;
	ns->softstemn_storage	-= nf->softstemn_storage_to_HRV;
	ns->retransn			-= nf->retransn_to_HRV;

       
     /* 1.2.2 Nitrogen: dead standing biomass */
	ns->litr1nA_STDB	-= nf->litr1n_STDB_to_HRV;
	ns->litr2nA_STDB	-= nf->litr2n_STDB_to_HRV;
	ns->litr3nA_STDB	-= nf->litr3n_STDB_to_HRV;
	ns->litr4nA_STDB	-= nf->litr4n_STDB_to_HRV;


	/* 1.3. water */
	ws->canopyw_HRVsnk += wf->canopyw_to_HRV;
	ws->canopyw -= wf->canopyw_to_HRV;

	/* 2.: TRANSPORT*/
	cs->HRV_transportC  += HRV_to_transpC;
	ns->HRV_transportN  += HRV_to_transpN;

	/* 3. IN */
	/* 3.1. carbon cut-down biome */
	cs->litr1cA_CTDB += cf->HRV_to_CTDB_litr1cA;
	cs->litr2cA_CTDB += cf->HRV_to_CTDB_litr2cA;
	cs->litr3cA_CTDB += cf->HRV_to_CTDB_litr3cA;
	cs->litr4cA_CTDB += cf->HRV_to_CTDB_litr4cA;

	/* 3.2. nitrogen cut-down biome */
	ns->litr1nA_CTDB += nf->HRV_to_CTDB_litr1nA;
	ns->litr2nA_CTDB += nf->HRV_to_CTDB_litr2nA;
	ns->litr3nA_CTDB += nf->HRV_to_CTDB_litr3nA;
	ns->litr4nA_CTDB += nf->HRV_to_CTDB_litr4nA;

	

	/**********************************************************************************************/
	/* IV. CONTROL */

	outc = cf->leafc_to_HRV         + cf->leafc_transfer_to_HRV     + cf->leafc_storage_to_HRV +
		   cf->fruitc_to_HRV        + cf->fruitc_transfer_to_HRV    + cf->fruitc_storage_to_HRV +
		   cf->softstemc_to_HRV     + cf->softstemc_transfer_to_HRV + cf->softstemc_storage_to_HRV +
		   cf->gresp_storage_to_HRV + cf->gresp_transfer_to_HRV + 
		   STDBc_to_HRV;


	outn = nf->leafn_to_HRV         + nf->leafn_transfer_to_HRV     + nf->leafn_storage_to_HRV +
		   nf->fruitn_to_HRV        + nf->fruitn_transfer_to_HRV    + nf->fruitn_storage_to_HRV +
		   nf->softstemn_to_HRV     + nf->softstemn_transfer_to_HRV + nf->softstemn_storage_to_HRV +
		   nf->retransn_to_HRV + 
		   STDBn_to_HRV;

	inc = cf->HRV_to_CTDB_litr1cA + cf->HRV_to_CTDB_litr2cA  + cf->HRV_to_CTDB_litr3cA + cf->HRV_to_CTDB_litr4cA;

	inn = nf->HRV_to_CTDB_litr1nA + nf->HRV_to_CTDB_litr2nA  + nf->HRV_to_CTDB_litr3nA + nf->HRV_to_CTDB_litr4nA;


	if (fabs(inc + HRV_to_transpC - outc) > CRIT_PREC || fabs(inn + HRV_to_transpN - outn) > CRIT_PREC )
	{
 		printf("BALANCE ERROR in harvest calculation in harvesting.c\n");
		ok=0;
	}	


   return (!ok);
}
	