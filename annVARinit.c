/* 
annVARinit.c
initalization of annual, cumulative variables on first day of every simulation year

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Biome-BGCMuSo v6.4.
Copyright 2022, D. Hidy [dori.hidy@gmail.com]
Hungarian Academy of Sciences, Hungary
See the website of Biome-BGCMuSo at http://nimbus.elte.hu/bbgc/ for documentation, model executable and example input files.
*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
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


#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

int annVARinit(summary_struct* summary, epvar_struct* epv, phenology_struct *phen, cstate_struct* cs, cflux_struct* cf, nflux_struct* nf)
{

	int layer;
	int errorCode=0;
	
	
	summary->annprcp     = 0;
	summary->anntavg     = 0;
	summary->cum_runoff   = 0;
	summary->cum_WleachRZ  = 0;
	summary->cum_NleachRZ  = 0;
	summary->cum_npp  = 0;
	summary->cum_nep  = 0;
	summary->cum_nee  = 0;
	summary->cum_gpp  = 0;
	summary->cum_ngb  = 0;
	summary->cum_nbp  = 0;
	summary->cum_mr  = 0;
	summary->cum_gr  = 0;
	summary->cum_hr  = 0;
	summary->cum_tr  = 0;
	summary->cum_sr  = 0;
	summary->cum_n2o  = 0;
	summary->cum_ch4  = 0;
	summary->cum_Closs_MGM  = 0;
	summary->cum_Cplus_MGM  = 0;
	summary->cum_Closs_THN_w = 0;
	summary->cum_Closs_THN_nw = 0;
	summary->cum_Closs_MOW  = 0;

	summary->cum_Closs_GRZ  = 0;
	summary->cum_Cplus_GRZ  = 0;
	summary->cum_Cplus_FRZ  = 0;
		
	summary->cum_Cplus_PLT  = 0;
	summary->cum_Closs_PLT  = 0;
	summary->cum_Closs_HRV  = 0;
	summary->cum_yieldC_HRV = 0;
	summary->cum_Closs_PLG  = 0;

	summary->cum_Nplus_GRZ  = 0;
	summary->cum_Nplus_FRZ  = 0;
	summary->cum_Closs_SNSC  = 0;
	summary->cum_Cplus_STDB  = 0;
	summary->cum_Cplus_CTDB  = 0;
	summary->cum_evap  = 0;
	summary->cum_transp  = 0;
	summary->cum_ET  = 0;
	

	
	cs->yield_HRV        = 0;
	cs->vegC_HRV          = 0;
	cs->frootC_HRV        = 0;
	epv->cumSWCstress     = 0;
	epv->cumNstress       = 0;
	epv->plantCalloc_CUM  = 0;
	epv->plantNalloc_CUM  = 0;


	epv->annmax_leafc = 0;
	epv->annmax_frootc = 0;
	epv->annmax_yield = 0;
	epv->annmax_softstemc = 0;
	epv->annmax_livestemc = 0;
	epv->annmax_livecrootc = 0;

	nf->sminn_to_soil1n_l1_totalCUM = 0;
	nf->sminn_to_soil2n_l2_totalCUM = 0;
	nf->sminn_to_soil3n_l4_totalCUM = 0;
	nf->sminn_to_soil2n_s1_totalCUM = 0;
	nf->sminn_to_soil3n_s2_totalCUM = 0;
	nf->sminn_to_soil4n_s3_totalCUM = 0;
	nf->soil4n_to_sminn_totalCUM  = 0;
	nf->netMINER_totalCUM           = 0;
	nf->sminn_to_npool_totalCUM     = 0;

	for (layer = 0; layer < N_SOILLAYERS; layer++)
	{
		cf->soilDOC_leachCUM[layer] = 0;
		nf->sminNH4_leachCUM[layer] = 0;    
		nf->sminNO3_leachCUM[layer] = 0;
		nf->soilDON_leachCUM[layer] = 0; 
	}

	return (errorCode);

}

