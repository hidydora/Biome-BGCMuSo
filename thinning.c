 /* 
thinning.c
do thinning  - decrease the plant material (leafc, leafn, canopy water, frootc, frootn, stemc, stemn, crootc, crootn)

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

int thinning(const control_struct* ctrl, const epconst_struct* epc,  
			 thinning_struct* THN,cflux_struct* cf,nflux_struct* nf,wflux_struct* wf,cstate_struct* cs, nstate_struct* ns, wstate_struct* ws)
{
	/* thinning parameters */
	double remained_prop_nwoody, remained_prop_woody;	
	double STDBc_to_THN, STDBn_to_THN;
	double THNcoeff, THN_to_transpC, THN_to_transpN, inc, inn, outc, outn;				

	int ok=1;
	int ny;
	int mgmd = THN->mgmd;


	double storage_MGMmort=epc->storage_MGMmort;

	/* yearly varied or constant management parameters */
	if(THN->THN_flag == 2)
	{
		ny = ctrl->simyr;
	}
	else ny=0;

	/**********************************************************************************************/
	/* I. CALCULATING THNcoeff */

	/* thinning if flag=1 */
	if (mgmd >= 0)
	{	

		THNcoeff    = THN->thinning_rate_array[mgmd][ny]; 		/* coefficient determining decrease of plant material caused by thinning  */	
		remained_prop_nwoody = (100 - THN->transpcoeff_nwoody_array[mgmd][ny])/100.;
		remained_prop_woody = (100 - THN->transpcoeff_woody_array[mgmd][ny])/100.;

	
	}
	else
	{
		THNcoeff    = 0;
		THNcoeff   = 0;
		THNcoeff   = 0;
		remained_prop_nwoody = 0;
		remained_prop_woody = 0;
	}

	/**********************************************************************************************/
	/* II. CALCULATING FLUXES */

	/* 1. as results of the thinning the carbon, nitrogen and water content of the leaf/root/stem decreases*/
	/* fruit simulation - Hidy 2013.: harvested fruit is transported from the site  */


	/* 1.1.1 CARBON: leaf, root, fruit, stem */
	cf->leafc_to_THN          = cs->leafc * THNcoeff;
	cf->leafc_transfer_to_THN = cs->leafc_transfer * THNcoeff * storage_MGMmort; 
	cf->leafc_storage_to_THN  = cs->leafc_storage * THNcoeff * storage_MGMmort; 

	cf->frootc_to_THN          = cs->frootc * THNcoeff;
	cf->frootc_transfer_to_THN = cs->frootc_transfer * THNcoeff * storage_MGMmort; 
	cf->frootc_storage_to_THN  = cs->frootc_storage * THNcoeff * storage_MGMmort; 

	cf->fruitc_to_THN          = cs->fruitc * THNcoeff;
	cf->fruitc_transfer_to_THN = cs->fruitc_transfer * THNcoeff * storage_MGMmort; 
	cf->fruitc_storage_to_THN  = cs->fruitc_storage * THNcoeff * storage_MGMmort; 

	cf->livecrootc_to_THN          = cs->livecrootc * THNcoeff;
	cf->livecrootc_transfer_to_THN = cs->livecrootc_transfer * THNcoeff * storage_MGMmort; 
	cf->livecrootc_storage_to_THN  = cs->livecrootc_storage * THNcoeff * storage_MGMmort;  

	cf->deadcrootc_to_THN          = cs->deadcrootc * THNcoeff;
	cf->deadcrootc_transfer_to_THN = cs->deadcrootc_transfer * THNcoeff * storage_MGMmort; 
	cf->deadcrootc_storage_to_THN  = cs->deadcrootc_storage * THNcoeff * storage_MGMmort; 

	cf->livestemc_to_THN           = cs->livestemc * THNcoeff;
	cf->livestemc_transfer_to_THN  = cs->livestemc_transfer * THNcoeff * storage_MGMmort; 
	cf->livestemc_storage_to_THN   = cs->livestemc_storage * THNcoeff * storage_MGMmort;  

	cf->deadstemc_to_THN          = cs->deadstemc * THNcoeff;
	cf->deadstemc_transfer_to_THN = cs->deadstemc_transfer * THNcoeff * storage_MGMmort; 
	cf->deadstemc_storage_to_THN  = cs->deadstemc_storage * THNcoeff * storage_MGMmort; 
	
	cf->gresp_storage_to_THN      = cs->gresp_storage * THNcoeff * storage_MGMmort; 
	cf->gresp_transfer_to_THN     = cs->gresp_transfer * THNcoeff * storage_MGMmort; 

	/* 1.1.2 CARBON:  standing dead biome */
	cf->litr1c_STDB_to_THN = cs->litr1cA_STDB * THNcoeff;
	cf->litr2c_STDB_to_THN = cs->litr2cA_STDB * THNcoeff;
	cf->litr3c_STDB_to_THN = cs->litr3cA_STDB * THNcoeff;
	cf->litr4c_STDB_to_THN = cs->litr4cA_STDB * THNcoeff;
	cf->cwdc_STDB_to_THN   = cs->cwdcA_STDB   * THNcoeff;

	STDBc_to_THN = cf->litr1c_STDB_to_THN + cf->litr2c_STDB_to_THN + cf->litr3c_STDB_to_THN + cf->litr4c_STDB_to_THN + cf->cwdc_STDB_to_THN;

	/*-----------------------------------------*/
	/* 1.2.1 NITROGEN: leaf, root, fruit, stem, retrans */

	nf->leafn_to_THN           = ns->leafn * THNcoeff;
	nf->leafn_transfer_to_THN  = ns->leafn_transfer * THNcoeff * storage_MGMmort; 
	nf->leafn_storage_to_THN   = ns->leafn_storage * THNcoeff * storage_MGMmort;  

	nf->frootn_to_THN          = ns->frootn * THNcoeff;
	nf->frootn_transfer_to_THN = ns->frootn_transfer * THNcoeff * storage_MGMmort; 
	nf->frootn_storage_to_THN  = ns->frootn_storage * THNcoeff * storage_MGMmort; 

	nf->fruitn_to_THN          = ns->fruitn * THNcoeff;
	nf->fruitn_transfer_to_THN = ns->fruitn_transfer * THNcoeff * storage_MGMmort; 
	nf->fruitn_storage_to_THN  = ns->fruitn_storage * THNcoeff * storage_MGMmort; 

	nf->livecrootn_to_THN          = ns->livecrootn * THNcoeff;
	nf->livecrootn_transfer_to_THN = ns->livecrootn_transfer * THNcoeff * storage_MGMmort; 
	nf->livecrootn_storage_to_THN  = ns->livecrootn_storage * THNcoeff * storage_MGMmort;  
	
	nf->deadcrootn_to_THN          = ns->deadcrootn * THNcoeff;
	nf->deadcrootn_transfer_to_THN = ns->deadcrootn_transfer * THNcoeff * storage_MGMmort; 
	nf->deadcrootn_storage_to_THN  = ns->deadcrootn_storage * THNcoeff * storage_MGMmort;  

	nf->livestemn_to_THN           = ns->livestemn * THNcoeff;
	nf->livestemn_transfer_to_THN  = ns->livestemn_transfer * THNcoeff * storage_MGMmort; 
	nf->livestemn_storage_to_THN   = ns->livestemn_storage * THNcoeff * storage_MGMmort;  

	nf->deadstemn_to_THN          = ns->deadstemn * THNcoeff;
	nf->deadstemn_transfer_to_THN = ns->deadstemn_transfer * THNcoeff * storage_MGMmort; 
	nf->deadstemn_storage_to_THN  = ns->deadstemn_storage * THNcoeff * storage_MGMmort; 

	nf->retransn_to_THN           = ns->retransn * THNcoeff * storage_MGMmort;
	
	/* 1.2.2 NITROGEN: standing dead biome */
	nf->litr1n_STDB_to_THN = ns->litr1nA_STDB * THNcoeff;
	nf->litr2n_STDB_to_THN = ns->litr2nA_STDB * THNcoeff;
	nf->litr3n_STDB_to_THN = ns->litr3nA_STDB * THNcoeff;
	nf->litr4n_STDB_to_THN = ns->litr4nA_STDB * THNcoeff;
	nf->cwdn_STDB_to_THN   = ns->cwdnA_STDB   * THNcoeff;

  	STDBn_to_THN = nf->litr1n_STDB_to_THN + nf->litr2n_STDB_to_THN + nf->litr3n_STDB_to_THN + nf->litr4n_STDB_to_THN + nf->cwdn_STDB_to_THN; 

	/*-----------------------------------------*/
	/* 1.3. water */
	wf->canopyw_to_THN = ws->canopyw * THNcoeff;



	/*-----------------------------------------------------------------------------------*/
	/* 2. part of the plant material is transported (THN_to_transpC and THN_to_transpN; transp_coeff = 1-remained_prop),*/	
	/* transp:(leaf_total+fruit_total+gresp)*(1-remprop_nwoody) + (livestem_total+deadstem_total)* (1-remprop_woody) */
	

	THN_to_transpC = (cf->leafc_to_THN + cf->fruitc_to_THN + STDBc_to_THN)                        * (1-remained_prop_nwoody) +
					 (cf->livestemc_to_THN +  cf->deadstemc_to_THN) * (1-remained_prop_woody);

	THN_to_transpN = (nf->leafn_to_THN + nf->fruitn_to_THN + STDBn_to_THN )				       * (1-remained_prop_nwoody) +
					 (nf->livestemn_to_THN + nf->deadstemn_to_THN)  * (1-remained_prop_woody);
	

	/*-----------------------------------------------------------------------------------*/
	/* 3. the rest remains at the site (THN_to_litr_strg, THN_to_cwd_strg -  cwd_strg: temporary cwd storage pool which contains the cut-down part of coarse root and stem  */
	/* MULTILAYER SOIL: above and belowground plant material separately */
	
	/* 3.1 litter CARBON  */
	cf->THN_to_CTDB_litr1cA = (cf->leafc_to_THN * epc->leaflitr_flab   + cf->fruitc_to_THN * epc->fruitlitr_flab + STDBc_to_THN)  * (remained_prop_nwoody);
	cf->THN_to_CTDB_litr1cB  = cf->frootc_to_THN * epc->frootlitr_flab + cf->frootc_transfer_to_THN  + cf->frootc_storage_to_THN +
								cf->leafc_transfer_to_THN  + cf->leafc_storage_to_THN +
								cf->fruitc_transfer_to_THN + cf->fruitc_storage_to_THN + 
								cf->gresp_storage_to_THN  + cf->gresp_transfer_to_THN + 
								cf->livecrootc_transfer_to_THN + cf->livecrootc_storage_to_THN + 
								cf->deadcrootc_transfer_to_THN + cf->deadcrootc_storage_to_THN +
								cf->livestemc_transfer_to_THN + cf->livestemc_storage_to_THN +
								cf->deadstemc_transfer_to_THN + cf->deadstemc_storage_to_THN;
       


	cf->THN_to_CTDB_litr2cA = (cf->leafc_to_THN * epc->leaflitr_fucel + cf->fruitc_to_THN * epc->fruitlitr_fucel) * remained_prop_nwoody;
	cf->THN_to_CTDB_litr2cB =  cf->frootc_to_THN * epc->frootlitr_fucel;
	
	cf->THN_to_CTDB_litr3cA = (cf->leafc_to_THN * epc->leaflitr_fscel + cf->fruitc_to_THN * epc->fruitlitr_fscel) * remained_prop_nwoody;
	cf->THN_to_CTDB_litr3cB =  cf->frootc_to_THN * epc->frootlitr_fscel;
	
	cf->THN_to_CTDB_litr4cA = (cf->leafc_to_THN * epc->leaflitr_flig  + cf->fruitc_to_THN * epc->fruitlitr_flig)  * remained_prop_nwoody;
	cf->THN_to_CTDB_litr4cB =  cf->frootc_to_THN * epc->frootlitr_flig;
	
	cf->THN_to_CTDB_cwdcA   = (cf->livestemc_to_THN + cf->deadstemc_to_THN) * remained_prop_woody;
	cf->THN_to_CTDB_cwdcB   = (cf->livecrootc_to_THN + cf->deadcrootc_to_THN);

	
	/* 3.2 litter NITROGEN  */
	nf->THN_to_CTDB_litr1nA = (nf->leafn_to_THN * epc->leaflitr_flab  + nf->fruitn_to_THN * epc->fruitlitr_flab + STDBn_to_THN) * remained_prop_nwoody;
	nf->THN_to_CTDB_litr1nB =  nf->frootn_to_THN * epc->frootlitr_flab+ nf->frootn_transfer_to_THN  + nf->frootn_storage_to_THN +	
								nf->leafn_transfer_to_THN  + nf->leafn_storage_to_THN +
								nf->fruitn_transfer_to_THN + nf->fruitn_storage_to_THN + 
								nf->retransn_to_THN + 
								nf->livestemn_transfer_to_THN + nf->livestemn_storage_to_THN + 
								nf->deadstemn_transfer_to_THN + nf->deadstemn_storage_to_THN +  
								nf->livecrootn_transfer_to_THN + nf->livecrootn_storage_to_THN + 
								nf->deadcrootn_transfer_to_THN + nf->deadcrootn_storage_to_THN;

	nf->THN_to_CTDB_litr2nA = (nf->leafn_to_THN  * epc->leaflitr_fucel + nf->fruitn_to_THN * epc->fruitlitr_fucel) * remained_prop_nwoody;
	nf->THN_to_CTDB_litr2nB =  nf->frootn_to_THN * epc->frootlitr_fucel;

	nf->THN_to_CTDB_litr3nA = (nf->leafn_to_THN  * epc->leaflitr_fscel + nf->fruitn_to_THN * epc->fruitlitr_fscel) * remained_prop_nwoody;
	nf->THN_to_CTDB_litr3nB =  nf->frootn_to_THN * epc->frootlitr_fscel;
	
	nf->THN_to_CTDB_litr4nA = (nf->leafn_to_THN  * epc->leaflitr_flig  + nf->fruitn_to_THN * epc->fruitlitr_flig)  * remained_prop_nwoody;
	nf->THN_to_CTDB_litr4nB =  nf->frootn_to_THN * epc->frootlitr_flig;

	nf->THN_to_CTDB_cwdnA   = (nf->livestemn_to_THN + nf->deadstemn_to_THN) * remained_prop_woody;
	nf->THN_to_CTDB_cwdnB   =  nf->livecrootn_to_THN + nf->deadcrootn_to_THN;


	/**********************************************************************************************/
	/* III. STATE UPDATE */

	/* 1.1. CARBON: leaf, root, fruit, stem, standing dead biome */
	cs->leafc          -= cf->leafc_to_THN;
	cs->leafc_transfer -= cf->leafc_transfer_to_THN;
	cs->leafc_storage  -= cf->leafc_storage_to_THN;
	cs->frootc          -= cf->frootc_to_THN;
	cs->frootc_transfer -= cf->frootc_transfer_to_THN;
	cs->frootc_storage  -= cf->frootc_storage_to_THN;
	cs->fruitc          -= cf->fruitc_to_THN;
	cs->fruitc_transfer -= cf->fruitc_transfer_to_THN;
	cs->fruitc_storage  -= cf->fruitc_storage_to_THN;
	cs->gresp_storage   -= cf->gresp_storage_to_THN;
	cs->gresp_transfer  -= cf->gresp_transfer_to_THN;
	cs->livecrootc          -= cf->livecrootc_to_THN;
	cs->livecrootc_transfer -= cf->livecrootc_transfer_to_THN;
	cs->livecrootc_storage  -= cf->livecrootc_storage_to_THN;
	cs->deadcrootc          -= cf->deadcrootc_to_THN;
	cs->deadcrootc_transfer -= cf->deadcrootc_transfer_to_THN;
	cs->deadcrootc_storage  -= cf->deadcrootc_storage_to_THN; 
	cs->livestemc           -= cf->livestemc_to_THN;
	cs->livestemc_transfer  -= cf->livestemc_transfer_to_THN;
	cs->livestemc_storage   -= cf->livestemc_storage_to_THN;
	cs->deadstemc           -= cf->deadstemc_to_THN;
	cs->deadstemc_transfer  -= cf->deadstemc_transfer_to_THN;
	cs->deadstemc_storage   -= cf->deadstemc_storage_to_THN;

	cs->litr1cA_STDB		-= cf->litr1c_STDB_to_THN;
	cs->litr2cA_STDB		-= cf->litr2c_STDB_to_THN;
	cs->litr3cA_STDB		-= cf->litr3c_STDB_to_THN;
	cs->litr4cA_STDB		-= cf->litr4c_STDB_to_THN;


	/* 1.2. NITROGEN: leaf, root, fruit, stem, standing dead biome */
	ns->leafn				-= nf->leafn_to_THN;
	ns->leafn_transfer		-= nf->leafn_transfer_to_THN;
	ns->leafn_storage		-= nf->leafn_storage_to_THN;	
	ns->frootn				-= nf->frootn_to_THN;
	ns->frootn_transfer		-= nf->frootn_transfer_to_THN;
	ns->frootn_storage		-= nf->frootn_storage_to_THN;  	
	ns->fruitn				-= nf->fruitn_to_THN;
	ns->fruitn_transfer		-= nf->fruitn_transfer_to_THN;
	ns->fruitn_storage		-= nf->fruitn_storage_to_THN;	
	ns->livestemn			-= nf->livestemn_to_THN;
	ns->livestemn_transfer  -= nf->livestemn_transfer_to_THN;
	ns->livestemn_storage	-= nf->livestemn_storage_to_THN;	
	ns->deadstemn			-= nf->deadstemn_to_THN;
	ns->deadstemn_transfer  -= nf->deadstemn_transfer_to_THN;
	ns->deadstemn_storage	-= nf->deadstemn_storage_to_THN;	
	ns->livecrootn			-= nf->livecrootn_to_THN;
	ns->livecrootn_transfer -= nf->livecrootn_transfer_to_THN;
	ns->livecrootn_storage	-= nf->livecrootn_storage_to_THN;		
	ns->deadcrootn			-= nf->deadcrootn_to_THN;
	ns->deadcrootn_transfer -= nf->deadcrootn_transfer_to_THN;
	ns->deadcrootn_storage	-= nf->deadcrootn_storage_to_THN; 

	ns->litr1nA_STDB	-= nf->litr1n_STDB_to_THN;
	ns->litr2nA_STDB	-= nf->litr2n_STDB_to_THN;
	ns->litr3nA_STDB	-= nf->litr3n_STDB_to_THN;
	ns->litr4nA_STDB	-= nf->litr4n_STDB_to_THN;

	ns->retransn        -= nf->retransn_to_THN;
	

	/* 1.3. water */
	ws->canopyw_THNsnk += wf->canopyw_to_THN;
	ws->canopyw -= wf->canopyw_to_THN;

	/* 2. transport */
	cs->THN_transportC  += THN_to_transpC;
	ns->THN_transportN  += THN_to_transpN;

	/* 3. cut-down biome */
	cs->litr1cA_CTDB    +=	cf->THN_to_CTDB_litr1cA;
	cs->litr1cB_CTDB    +=	cf->THN_to_CTDB_litr1cB;
	cs->litr2cA_CTDB    +=	cf->THN_to_CTDB_litr2cA;
	cs->litr2cB_CTDB    +=	cf->THN_to_CTDB_litr2cB;
	cs->litr3cA_CTDB    +=	cf->THN_to_CTDB_litr3cA;
	cs->litr3cB_CTDB    +=	cf->THN_to_CTDB_litr3cB;
	cs->litr4cA_CTDB    +=	cf->THN_to_CTDB_litr4cA;
	cs->litr4cB_CTDB    +=	cf->THN_to_CTDB_litr4cB;
	cs->cwdcA_CTDB      +=	cf->THN_to_CTDB_cwdcA; 
	cs->cwdcB_CTDB      +=	cf->THN_to_CTDB_cwdcB; 

	ns->litr1nA_CTDB    +=	nf->THN_to_CTDB_litr1nA;
	ns->litr1nB_CTDB    +=	nf->THN_to_CTDB_litr1nB;
	ns->litr2nA_CTDB    +=	nf->THN_to_CTDB_litr2nA;
	ns->litr2nB_CTDB    +=	nf->THN_to_CTDB_litr2nB;
	ns->litr3nA_CTDB    +=	nf->THN_to_CTDB_litr3nA;
	ns->litr3nB_CTDB    +=	nf->THN_to_CTDB_litr3nB;
	ns->litr4nA_CTDB    +=	nf->THN_to_CTDB_litr4nA;
	ns->litr4nB_CTDB    +=	nf->THN_to_CTDB_litr4nB;
	ns->cwdnA_CTDB      +=	nf->THN_to_CTDB_cwdnA; 
	ns->cwdnB_CTDB      +=	nf->THN_to_CTDB_cwdnB; 
	

	/**********************************************************************************************/
	/* IV. CONTROL */

	outc = cf->leafc_to_THN + cf->leafc_transfer_to_THN + cf->leafc_storage_to_THN +
		   cf->fruitc_to_THN + cf->fruitc_transfer_to_THN + cf->fruitc_storage_to_THN +
		   cf->frootc_to_THN + cf->frootc_transfer_to_THN + cf->frootc_storage_to_THN +
		   cf->livestemc_to_THN + cf->livestemc_transfer_to_THN + cf->livestemc_storage_to_THN +
		   cf->deadstemc_to_THN + cf->deadstemc_transfer_to_THN + cf->deadstemc_storage_to_THN +
		   cf->livecrootc_to_THN + cf->livecrootc_transfer_to_THN + cf->livecrootc_storage_to_THN +
		   cf->deadcrootc_to_THN + cf->deadcrootc_transfer_to_THN + cf->deadcrootc_storage_to_THN +
		   cf->gresp_storage_to_THN + cf->gresp_transfer_to_THN + 
		   STDBc_to_THN;

	outn = nf->leafn_to_THN + nf->leafn_transfer_to_THN + nf->leafn_storage_to_THN +
		   nf->fruitn_to_THN + nf->fruitn_transfer_to_THN + nf->fruitn_storage_to_THN +
		   nf->frootn_to_THN + nf->frootn_transfer_to_THN + nf->frootn_storage_to_THN +
		   nf->livestemn_to_THN + nf->livestemn_transfer_to_THN + nf->livestemn_storage_to_THN +
		   nf->deadstemn_to_THN + nf->deadstemn_transfer_to_THN + nf->deadstemn_storage_to_THN +
		   nf->livecrootn_to_THN + nf->livecrootn_transfer_to_THN + nf->livecrootn_storage_to_THN +
		   nf->deadcrootn_to_THN + nf->deadcrootn_transfer_to_THN + nf->deadcrootn_storage_to_THN +
		   nf->retransn_to_THN + 
		   STDBn_to_THN;

	inc = cf->THN_to_CTDB_litr1cA + cf->THN_to_CTDB_litr2cA  + cf->THN_to_CTDB_litr3cA + cf->THN_to_CTDB_litr4cA + cf->THN_to_CTDB_cwdcA +
		  cf->THN_to_CTDB_litr1cB + cf->THN_to_CTDB_litr2cB  + cf->THN_to_CTDB_litr3cB + cf->THN_to_CTDB_litr4cB + cf->THN_to_CTDB_cwdcB;
	inn = nf->THN_to_CTDB_litr1nA + nf->THN_to_CTDB_litr2nA  + nf->THN_to_CTDB_litr3nA + nf->THN_to_CTDB_litr4nA + nf->THN_to_CTDB_cwdnA +
		  nf->THN_to_CTDB_litr1nB + nf->THN_to_CTDB_litr2nB  + nf->THN_to_CTDB_litr3nB + nf->THN_to_CTDB_litr4nB + nf->THN_to_CTDB_cwdnB;

	if (fabs(inc + THN_to_transpC - outc) > CRIT_PREC || fabs(inn + THN_to_transpN - outn) > CRIT_PREC )
	{
		printf("BALANCE ERROR in thinning calculation in thinning.c\n");
		ok=0;
	}

   return (!ok);
}
	