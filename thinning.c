/* 
thinning.c
do thinning  - decrease the plant material (leafc, leafn, canopy water, frootc, frootn, stemc, stemn, crootc, crootn)

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Biome-BGC version 4.1.1
Copyright 2012, Hidy
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

int thinning(const control_struct* ctrl, const epconst_struct* epc, thinning_struct* THN, cflux_struct* cf,nflux_struct* nf,wflux_struct* wf, 
				 cstate_struct* cs, nstate_struct* ns, wstate_struct* ws)
{
	/* thinning parameters */
	double transp_leaf_rate;
	double transp_stem_rate;
	
	double thinning_rate;	/* decreasing of plant material caused by thinning*/
	int ok=1;
	int ny;
	int mgmd = THN->mgmd;

	/* yearly varied or constant management parameters */
	if(THN->THN_flag == 2)
	{
		ny = ctrl->simyr;
	}
	else ny=0;

	/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                                                    CALCULATING FLUXES 
	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

	/* thinning if flag=1 */
	if (mgmd >= 0)
	{	

		thinning_rate = THN->thinning_rate_array[mgmd][ny];
		transp_leaf_rate=THN->transp_leaf_rate_array[mgmd][ny]/100.;
		transp_stem_rate=THN->transp_stem_rate_array[mgmd][ny]/100.;
	
	}
	else
	{
		thinning_rate = 0;
		transp_leaf_rate=0;
		transp_stem_rate=0;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	

	/* leaf */
	cf->leafc_to_THN          = cs->leafc * thinning_rate;
	cf->leafc_transfer_to_THN = cs->leafc_transfer * thinning_rate; 
	cf->leafc_storage_to_THN  = cs->leafc_storage * thinning_rate; 

	nf->leafn_to_THN          = ns->leafn * thinning_rate;
	nf->leafn_transfer_to_THN = ns->leafn_transfer * thinning_rate; 
	nf->leafn_storage_to_THN  = ns->leafn_storage * thinning_rate;  

	/* fine root */
	cf->frootc_to_THN          = cs->frootc * thinning_rate;
	cf->frootc_transfer_to_THN = cs->frootc_transfer * thinning_rate; 
	cf->frootc_storage_to_THN  = cs->frootc_storage * thinning_rate; 

	nf->frootn_to_THN          = ns->frootn * thinning_rate;
	nf->frootn_transfer_to_THN = ns->frootn_transfer * thinning_rate; 
	nf->frootn_storage_to_THN  = ns->frootn_storage * thinning_rate; 

	/* coarse root */
	cf->livecrootc_to_THN          = cs->livecrootc * thinning_rate;
	cf->livecrootc_transfer_to_THN = cs->livecrootc_transfer * thinning_rate; 
	cf->livecrootc_storage_to_THN = cs->livecrootc_storage * thinning_rate;  

	nf->livecrootn_to_THN          = ns->livecrootn * thinning_rate;
	nf->livecrootn_transfer_to_THN = ns->livecrootn_transfer * thinning_rate; 
	nf->livecrootn_storage_to_THN  = ns->livecrootn_storage * thinning_rate;  

	cf->deadcrootc_to_THN          = cs->deadcrootc * thinning_rate;
	cf->deadcrootc_transfer_to_THN = cs->deadcrootc_transfer * thinning_rate; 
	cf->deadcrootc_storage_to_THN  = cs->deadcrootc_storage * thinning_rate; 

	nf->deadcrootn_to_THN          = ns->deadcrootn * thinning_rate;
	nf->deadcrootn_transfer_to_THN = ns->deadcrootn_transfer * thinning_rate; 
	nf->deadcrootn_storage_to_THN  = ns->deadcrootn_storage * thinning_rate;  

	/* stem */
	cf->livestemc_to_THN           = cs->livestemc * thinning_rate;
	cf->livestemc_transfer_to_THN  = cs->livestemc_transfer * thinning_rate; 
	cf->livestemc_storage_to_THN   = cs->livestemc_storage * thinning_rate;  

	nf->livestemn_to_THN           = ns->livestemn * thinning_rate;
	nf->livestemn_transfer_to_THN  = ns->livestemn_transfer * thinning_rate; 
	nf->livestemn_storage_to_THN   = ns->livestemn_storage * thinning_rate;  

	cf->deadstemc_to_THN          = cs->deadstemc * thinning_rate;
	cf->deadstemc_transfer_to_THN = cs->deadstemc_transfer * thinning_rate; 
	cf->deadstemc_storage_to_THN  = cs->deadstemc_storage * thinning_rate; 

	nf->deadstemn_to_THN          = ns->deadstemn * thinning_rate;
	nf->deadstemn_transfer_to_THN = ns->deadstemn_transfer * thinning_rate; 
	nf->deadstemn_storage_to_THN  = ns->deadstemn_storage * thinning_rate; 

	/* diverse pools */
	nf->restransn_to_THN          = ns->retransn * thinning_rate; 
	cf->gresp_storage_to_THN      = cs->gresp_storage * thinning_rate; 
	cf->gresp_transfer_to_THN     = cs->gresp_transfer * thinning_rate; 

	wf->canopyw_to_THN = ws->canopyw * thinning_rate;



	/* on thinning days the whole amount of THNed grass get onto the ground -> THN_cpool adn THN_npool. Carbon and nitrogen content
	   of THNed grass are returns into the soil determined by dissolving coefficient */

	cf->THN_to_litr1c = (cf->leafc_to_THN * epc->leaflitr_flab) * transp_leaf_rate +
						(cf->frootc_to_THN * epc->frootlitr_flab) +
						(cf->leafc_transfer_to_THN + cf->leafc_storage_to_THN) * transp_leaf_rate +
						(cf->frootc_transfer_to_THN + cf->frootc_storage_to_THN)  +
						(cf->livestemc_transfer_to_THN + cf->livestemc_storage_to_THN) * transp_stem_rate +
						(cf->deadstemc_transfer_to_THN + cf->deadstemc_storage_to_THN) * transp_stem_rate +
						(cf->gresp_storage_to_THN  + cf->gresp_transfer_to_THN) * transp_stem_rate;
	cf->THN_to_litr2c = (cf->leafc_to_THN * epc->leaflitr_fucel * transp_leaf_rate) +
						(cf->frootc_to_THN * epc->frootlitr_fucel);
	cf->THN_to_litr3c = (cf->leafc_to_THN * epc->leaflitr_fscel) + 
						(cf->frootc_to_THN * epc->frootlitr_fucel);
	cf->THN_to_litr4c = (cf->leafc_to_THN * epc->leaflitr_flig) + 
						(cf->frootc_to_THN * epc->frootlitr_flig);

	cf->THN_to_cwdc =   cf->livestemc_to_THN * transp_stem_rate +
						cf->deadstemc_to_THN * transp_stem_rate +
						cf->livecrootc_to_THN +
						cf->deadcrootc_to_THN;


	nf->THN_to_litr1n = (nf->leafn_to_THN * epc->leaflitr_flab) * transp_leaf_rate +
						(nf->frootn_to_THN * epc->frootlitr_flab) +
						(nf->leafn_transfer_to_THN + nf->leafn_storage_to_THN) * transp_leaf_rate +
						(nf->frootn_transfer_to_THN + nf->frootn_storage_to_THN)  +
						(nf->livestemn_transfer_to_THN + nf->livestemn_storage_to_THN) * transp_stem_rate +
						(nf->deadstemn_transfer_to_THN + nf->deadstemn_storage_to_THN) * transp_stem_rate +
						nf->restransn_to_THN;
	nf->THN_to_litr2n = (nf->leafn_to_THN * epc->leaflitr_fucel * transp_leaf_rate) +
						(nf->frootn_to_THN * epc->frootlitr_fucel);
	nf->THN_to_litr3n = (nf->leafn_to_THN * epc->leaflitr_fscel) + 
						(nf->frootn_to_THN * epc->frootlitr_fucel);
	nf->THN_to_litr4n = (nf->leafn_to_THN * epc->leaflitr_flig) + 
						(nf->frootn_to_THN * epc->frootlitr_flig);

	nf->THN_to_cwdn =   nf->livestemn_to_THN * transp_stem_rate +
						nf->deadstemn_to_THN * transp_stem_rate +
						nf->livecrootn_to_THN +
						nf->deadcrootn_to_THN;


		
	/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                                                    STATE UPDATE 
	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 

	/* carbon */
	cs->THNsnk += cf->leafc_to_THN;
	cs->leafc -= cf->leafc_to_THN;
	cs->THNsnk += cf->leafc_transfer_to_THN;
	cs->leafc_transfer -= cf->leafc_transfer_to_THN;
	cs->THNsnk += cf->leafc_storage_to_THN;
	cs->leafc_storage -= cf->leafc_storage_to_THN;
	
	cs->THNsnk += cf->frootc_to_THN;
	cs->frootc -= cf->frootc_to_THN;
	cs->THNsnk += cf->frootc_transfer_to_THN;
	cs->frootc_transfer -= cf->frootc_transfer_to_THN;
	cs->THNsnk += cf->frootc_storage_to_THN;
	cs->frootc_storage -= cf->frootc_storage_to_THN;
	
	cs->THNsnk += cf->livecrootc_to_THN;
	cs->livecrootc -= cf->livecrootc_to_THN;
	cs->THNsnk += cf->livecrootc_transfer_to_THN;
	cs->livecrootc_transfer -= cf->livecrootc_transfer_to_THN;
	cs->THNsnk += cf->livecrootc_storage_to_THN;
	cs->livecrootc_storage -= cf->livecrootc_storage_to_THN;

	cs->THNsnk += cf->deadcrootc_to_THN;
	cs->deadcrootc -= cf->deadcrootc_to_THN;
	cs->THNsnk += cf->deadcrootc_transfer_to_THN;
	cs->deadcrootc_transfer -= cf->deadcrootc_transfer_to_THN;
	cs->THNsnk += cf->deadcrootc_storage_to_THN;
	cs->deadcrootc_storage -= cf->deadcrootc_storage_to_THN;

	cs->THNsnk += cf->livestemc_to_THN;
	cs->livestemc -= cf->livestemc_to_THN;
	cs->THNsnk += cf->livestemc_transfer_to_THN;
	cs->livestemc_transfer -= cf->livestemc_transfer_to_THN;
	cs->THNsnk += cf->livestemc_storage_to_THN;
	cs->livestemc_storage -= cf->livestemc_storage_to_THN;

	cs->THNsnk += cf->deadstemc_to_THN;
	cs->deadstemc -= cf->deadstemc_to_THN;
	cs->THNsnk += cf->deadstemc_transfer_to_THN;
	cs->deadstemc_transfer -= cf->deadstemc_transfer_to_THN;
	cs->THNsnk += cf->deadstemc_storage_to_THN;
	cs->deadstemc_storage -= cf->deadstemc_storage_to_THN;

	cs->THNsnk += cf->gresp_transfer_to_THN;
	cs->gresp_transfer -= cf->gresp_transfer_to_THN;
	cs->THNsnk += cf->gresp_storage_to_THN;
	cs->gresp_storage -= cf->gresp_storage_to_THN;
	
	cs->litr1c += cf->THN_to_litr1c;
	cs->litr2c += cf->THN_to_litr2c;
	cs->litr3c += cf->THN_to_litr3c;
	cs->litr4c += cf->THN_to_litr4c;
	cs->cwdc  += cf->THN_to_cwdc;
	
	cs->THNsrc += cf->THN_to_litr1c + cf->THN_to_litr2c + cf->THN_to_litr3c + cf->THN_to_litr4c + cf->THN_to_cwdc;

	/* nitrogen */
	ns->THNsnk += nf->leafn_to_THN;
	ns->leafn -= nf->leafn_to_THN;
	ns->THNsnk += nf->leafn_transfer_to_THN;
	ns->leafn_transfer -= nf->leafn_transfer_to_THN;
	ns->THNsnk += nf->leafn_storage_to_THN;
	ns->leafn_storage -= nf->leafn_storage_to_THN;	
	
	ns->THNsnk += nf->frootn_to_THN;
	ns->frootn -= nf->frootn_to_THN;
	ns->THNsnk += nf->frootn_transfer_to_THN;
	ns->frootn_transfer -= nf->frootn_transfer_to_THN;
	ns->THNsnk += nf->frootn_storage_to_THN;
	ns->frootn_storage -= nf->frootn_storage_to_THN;	
	ns->THNsnk += nf->livestemn_to_THN;
	
	ns->livestemn -= nf->livestemn_to_THN;
	ns->THNsnk += nf->livestemn_transfer_to_THN;
	ns->livestemn_transfer -= nf->livestemn_transfer_to_THN;
	ns->THNsnk += nf->livestemn_storage_to_THN;
	ns->livestemn_storage -= nf->livestemn_storage_to_THN;	
	
	ns->THNsnk += nf->deadstemn_to_THN;
	ns->deadstemn -= nf->deadstemn_to_THN;
	ns->THNsnk += nf->deadstemn_transfer_to_THN;
	ns->deadstemn_transfer -= nf->deadstemn_transfer_to_THN;
	ns->THNsnk += nf->deadstemn_storage_to_THN;
	ns->deadstemn_storage -= nf->deadstemn_storage_to_THN;	

	ns->THNsnk += nf->livecrootn_to_THN;
	ns->livecrootn -= nf->livecrootn_to_THN;
	ns->THNsnk += nf->livecrootn_transfer_to_THN;
	ns->livecrootn_transfer -= nf->livecrootn_transfer_to_THN;
	ns->THNsnk += nf->livecrootn_storage_to_THN;
	ns->livecrootn_storage -= nf->livecrootn_storage_to_THN;	
	
	ns->THNsnk += nf->deadcrootn_to_THN;
	ns->deadcrootn -= nf->deadcrootn_to_THN;
	ns->THNsnk += nf->deadcrootn_transfer_to_THN;
	ns->deadcrootn_transfer -= nf->deadcrootn_transfer_to_THN;
	ns->THNsnk += nf->deadcrootn_storage_to_THN;
	ns->deadcrootn_storage -= nf->deadcrootn_storage_to_THN;
	
	ns->THNsnk += nf->restransn_to_THN;
	ns->retransn -= nf->restransn_to_THN;
	
	ns->litr1n += nf->THN_to_litr1n;
	ns->litr2n += nf->THN_to_litr2n;
	ns->litr3n += nf->THN_to_litr3n;
	ns->litr4n += nf->THN_to_litr4n;
	ns->cwdn   += nf->THN_to_cwdn;
	
	ns->THNsrc += nf->THN_to_litr1n + nf->THN_to_litr2n + nf->THN_to_litr3n + nf->THN_to_litr4n + nf->THN_to_cwdn;


	ws->canopyw_THNsnk += wf->canopyw_to_THN;
	ws->canopyw -= wf->canopyw_to_THN;



   return (!ok);
}
	