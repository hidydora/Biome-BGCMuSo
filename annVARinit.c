/* 
annVARinit.c
initalization of annual, cumulative variables on first day of every simulation year

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Biome-BGCMuSo v7.0.
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

int annVARinit(summary_struct* summary, epvar_struct* epv, cstate_struct* cs, wstate_struct* ws, cflux_struct* cf, nflux_struct* nf)
{

	int layer;
	int errorCode=0;
	
	
	summary->annprcp     = 0;
	summary->anntavg     = 0;
	summary->cumRunoff   = 0;
	summary->cumWleachRZ  = 0;
	summary->cumNleachRZ  = 0;
	summary->cumNPP  = 0;
	summary->cumNEP  = 0;
	summary->cumNEE  = 0;
	summary->cumGPP  = 0;
	summary->cumNGB  = 0;
	summary->cumNBP  = 0;
	summary->cumMR  = 0;
	summary->cumGR  = 0;
	summary->cumHR  = 0;
	summary->cumAR = 0.0;
	summary->cumTR  = 0;
	summary->cumSR  = 0;
	summary->cumN2Oflux  = 0;
	summary->cumN2OfluxCeq = 0;
	summary->cumCH4flux = 0;
	summary->cumCflux_lateral = 0;
	summary->cumCH4flux  = 0;
	summary->cumCloss_MGM  = 0;
	summary->cumCplus_MGM  = 0;
	summary->cumCloss_THN_w = 0;
	summary->cumCloss_THN_nw = 0;
	summary->cumCloss_MOW  = 0;

	summary->cumCloss_GRZ  = 0;
	summary->cumCplus_GRZ  = 0;
	summary->cumCplus_FRZ  = 0;
		
	summary->cumCplus_PLT  = 0;
	summary->cumCloss_PLT  = 0;
	summary->cumCloss_HRV  = 0;
	summary->cumYieldC_HRV = 0;
	summary->cumCloss_PLG  = 0;

	summary->cumNplus_GRZ  = 0;
	summary->cumNplus_FRZ  = 0;
	summary->cumCloss_SNSC  = 0;
	summary->cumCplus_STDB  = 0;
	summary->cumCplus_CTDB  = 0;
	summary->cumEVP  = 0;
	summary->cumTRP  = 0;
	summary->cumET  = 0;
	

	
	cs->yieldC_HRV        = 0;
	cs->vegC_HRV          = 0;
	cs->frootC_HRV        = 0;
	epv->cumSWCstress     = 0;
	epv->cumNstress       = 0;
	epv->plantCalloc_CUM  = 0;
	epv->plantNalloc_CUM  = 0;


	epv->annmax_leafc = 0;
	epv->annmax_frootc = 0;
	epv->annmax_yieldc = 0;
	epv->annmax_softstemc = 0;
	epv->annmax_livestemc = 0;
	epv->annmax_livecrootc = 0;

	nf->minerFlux_StoS_totalCUM       = 0;
	nf->minerFlux_LtoS_totalCUM       = 0;
	nf->immobFlux_LtoS_totalCUM   = 0; 
	nf->immobFlux_StoS_totalCUM   = 0; 
	nf->sminn_to_npool_totalCUM       = 0;
	nf->netMINERflux_totalCUM         = 0;
	nf->actIMMOBflux_totalCUM         = 0;
	nf->environment_to_sminn_totalCUM = 0;

	for (layer = 0; layer < N_SOILLAYERS; layer++)
	{
		cf->soilDOC_leachCUM[layer] = 0;
		nf->sminNH4_leachCUM[layer] = 0;    
		nf->sminNO3_leachCUM[layer] = 0;
		nf->soilDON_leachCUM[layer] = 0; 
	}

	ws->cumGWchange = 0;

	return (errorCode);

}

