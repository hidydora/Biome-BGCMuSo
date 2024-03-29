/*
state_update.c
Resolve the fluxes in bgc() daily loop to update state variables

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Biome-BGCMuSo v7.0.
Original code: Copyright 2000, Peter E. Thornton
Numerical Terradynamic Simulation Group, The University of Montana, USA
Modified code: Copyright 2022, D. Hidy [dori.hidy@gmail.com]
Hungarian Academy of Sciences, Hungary
See the website of Biome-BGCMuSo at http://nimbus.elte.hu/bbgc/ for documentation, model executable and example input files.
*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Modified:
4/17/2000 (PET): Included the new denitrification flux. See daily_allocation.c
for complete description of this change.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <malloc.h>
#include "ini.h"
#include "bgc_struct.h"
#include "bgc_func.h"
#include "bgc_constants.h"


int water_state_update(const wflux_struct* wf, wstate_struct* ws)
{
	/* daily update of the water state variables */
	 
	int errorCode=0;
	int layer;
	double preGWsrc;
	
	/* snoww */
	ws->snoww          += wf->prcp_to_snoww;
	ws->snoww          -= wf->snoww_to_soilw;
	ws->snoww          -= wf->snowwSUBL;

	/* precipitation fluxes */
	ws->canopyw        += wf->prcp_to_canopyw;
	ws->canopyw        -= wf->canopywEVP;
	ws->canopyw        -= wf->canopyw_to_soilw;


	/* precipitation src */
	ws->prcp_src       += wf->prcp_to_canopyw;
	ws->prcp_src       += (wf->prcp_to_soilSurface - wf->IRG_to_prcp);
	ws->prcp_src       += wf->prcp_to_snoww;

	
	/* evapotranspiration/sublimation snk */
	ws->canopywEVP_snk += wf->canopywEVP;
	ws->snowSUBL_snk   += wf->snowwSUBL;
	ws->soilEVP_snk   += wf->soilwEVP;
	ws->TRP_snk       += wf->soilwTRP_SUM;
	ws->pondEVP_snk   += wf->pondwEVP;
	
	
	/* runoff */
	ws->runoff_snk	  += wf->pondw_to_runoff;
	 
	ws->deeppercolation_snk += wf->soilwFlux[N_SOILLAYERS-1];
		
	/* groundwater src/snk */

	preGWsrc = ws->groundwater_src;

	for (layer = 0; layer < N_SOILLAYERS; layer++)
	{
		ws->groundwater_src += wf->GWdischarge[layer];

		ws->groundwater_snk += wf->GWrecharge[layer];

		if (wf->GWmovchange[layer] > 0)
			ws->groundwater_src += wf->GWmovchange[layer];
		else 
			ws->groundwater_snk -= wf->GWmovchange[layer];
	}
	ws->groundwater_src += wf->GW_to_pondw;

	ws->cumGWchange += (ws->groundwater_src - preGWsrc);

	/* flooding src */
	ws->FLDsrc += wf->FLD_to_soilw;
	ws->FLDsrc += wf->FLD_to_pondw;

	/* irrigating src*/
	ws->IRGsrc_W += wf->IRG_to_prcp;
	ws->IRGsrc_W += wf->IRG_to_soilw;
	ws->IRGsrc_W += wf->IRG_to_soilSurface;
	
	return (errorCode);
}

int CN_state_update(const siteconst_struct* sitec, const epconst_struct* epc, control_struct* ctrl, epvar_struct* epv, 
	                      cflux_struct* cf, nflux_struct* nf, cstate_struct* cs, nstate_struct* ns, int alloc, int evergreen)
{
	/* daily update of the carbon state variables */
	
	int errorCode=0;
	int layer, pp;
	double leafc_to_litr, leafn_to_litr, frootc_to_litr, frootn_to_litr, yieldc_to_litr, yieldn_to_litr, softstemc_to_litr, softstemn_to_litr;
	double propLAYER0, propLAYER1, propLAYER2;

	/* estimating aboveground litter and cwdc*/
	double cwdc_total1, cwdc_total2, litrc_total1, litrc_total2;
	cwdc_total1=cwdc_total2=litrc_total1=litrc_total2=0;
			
	for (layer = 0; layer < N_SOILLAYERS; layer++) 
	{
		cwdc_total1 += cs->cwdc[layer];
		litrc_total1 += cs->litr1c[layer] + cs->litr2c[layer] + cs->litr3c[layer] + cs->litr4c[layer];
	}
	
	/* C state variables are updated below in the order of the relevant fluxes in the daily model loop */
	
	/* NOTE: Mortality fluxes are all accounted for in a separate routine, which is to be called after this routine.  
	This is a special case where the updating of state variables is order-sensitive, since otherwise the complications of possibly having 
	mortality fluxes drive the pools negative would create big, unnecessary headaches. */
	

	/* 1. Phenology fluxes */
	if (!errorCode && epc->leaf_cn && CNratio_control(cs, epc->leaf_cn, cs->leafc, ns->leafn, cf->leafc_transfer_to_leafc, nf->leafn_transfer_to_leafn, 0)) 
	{
		cs->leafc               += cf->leafc_transfer_to_leafc;
		cs->leafc_transfer      -= cf->leafc_transfer_to_leafc;
		ns->leafn               = cs->leafc / epc->leaf_cn;
		ns->leafn_transfer      = cs->leafc_transfer / epc->leaf_cn;
	}
	else if (epc->leaf_cn)		
	{
		if (!errorCode) printf("ERROR in leaf CN calculation in state_update.c\n");
		errorCode=1;
	}

	if (!errorCode && epc->froot_cn && CNratio_control(cs, epc->froot_cn, cs->frootc, ns->frootn, cf->frootc_transfer_to_frootc, nf->frootn_transfer_to_frootn, 0)) 
	{
		cs->frootc               += cf->frootc_transfer_to_frootc;
		cs->frootc_transfer      -= cf->frootc_transfer_to_frootc;
		ns->frootn               = cs->frootc / epc->froot_cn;
		ns->frootn_transfer      = cs->frootc_transfer / epc->froot_cn;
	}
	else if (epc->froot_cn)		
	{
		if (!errorCode) printf("ERROR in froot CN calculation in state_update.c\n");
		errorCode=1;
	}

	if (!errorCode && epc->yield_cn && CNratio_control(cs, epc->yield_cn, cs->yieldc, ns->yieldn, cf->yieldc_transfer_to_yield, nf->yieldn_transfer_to_yieldn, 0)) 
	{
		cs->yieldc               += cf->yieldc_transfer_to_yield;
		cs->yieldc_transfer      -= cf->yieldc_transfer_to_yield;
		ns->yieldn               = cs->yieldc / epc->yield_cn;
		ns->yieldn_transfer      = cs->yieldc_transfer / epc->yield_cn;
	}
	else if (epc->yield_cn)	
	{	
		if (!errorCode) printf("ERROR in yield CN calculation in state_update.c\n");
		errorCode=1;
	}

	if (!errorCode && epc->softstem_cn && CNratio_control(cs, epc->softstem_cn, cs->softstemc, ns->softstemn, cf->softstemc_transfer_to_softstemc, nf->softstemn_transfer_to_softstemn, 0)) 
	{
		cs->softstemc               += cf->softstemc_transfer_to_softstemc;
		cs->softstemc_transfer      -= cf->softstemc_transfer_to_softstemc;
		ns->softstemn               = cs->softstemc / epc->softstem_cn;
		ns->softstemn_transfer      = cs->softstemc_transfer / epc->softstem_cn;
	}
	else if (epc->softstem_cn)		
	{
		if (!errorCode) printf("ERROR in softstem CN calculation in state_update.c\n");
		errorCode=1;
	}
	
	if (!errorCode && epc->livewood_cn && CNratio_control(cs, epc->livewood_cn, cs->livestemc, ns->livestemn, cf->livestemc_transfer_to_livestemc, nf->livestemn_transfer_to_livestemn, 0)) 
	{
		cs->livestemc               += cf->livestemc_transfer_to_livestemc;
		cs->livestemc_transfer      -= cf->livestemc_transfer_to_livestemc;
		ns->livestemn               = cs->livestemc / epc->livewood_cn;
		ns->livestemn_transfer      = cs->livestemc_transfer / epc->livewood_cn;
	}
	else if (epc->livewood_cn)		
	{
		if (!errorCode) printf("ERROR in livestem CN calculation in state_update.c\n");
		errorCode=1;
	}

	if (!errorCode && epc->livewood_cn && CNratio_control(cs, epc->livewood_cn, cs->livecrootc, ns->livecrootn, cf->livecrootc_transfer_to_livecrootc, nf->livecrootn_transfer_to_livecrootn, 0)) 
	{
		cs->livecrootc               += cf->livecrootc_transfer_to_livecrootc;
		cs->livecrootc_transfer      -= cf->livecrootc_transfer_to_livecrootc;
		ns->livecrootn               = cs->livecrootc / epc->livewood_cn;
		ns->livecrootn_transfer      = cs->livecrootc_transfer / epc->livewood_cn;
	}
	else if (epc->livewood_cn)		
	{
		if (!errorCode) printf("ERROR in livecroot CN calculation in state_update.c\n");
		errorCode=1;
	}

	if (!errorCode && epc->deadwood_cn && CNratio_control(cs, epc->deadwood_cn, cs->deadstemc, ns->deadstemn, cf->deadstemc_transfer_to_deadstemc, nf->deadstemn_transfer_to_deadstemn, 0)) 
	{
		cs->deadstemc               += cf->deadstemc_transfer_to_deadstemc;
		cs->deadstemc_transfer      -= cf->deadstemc_transfer_to_deadstemc;
		ns->deadstemn               = cs->deadstemc / epc->deadwood_cn;
		ns->deadstemn_transfer      = cs->deadstemc_transfer / epc->deadwood_cn;
	}
	else if (epc->deadwood_cn)		
	{
		if (!errorCode) printf("ERROR in deadstem CN calculation in state_update.c\n");
		errorCode=1;
	}

	if (!errorCode && epc->deadwood_cn && CNratio_control(cs, epc->deadwood_cn, cs->deadcrootc, ns->deadcrootn, cf->deadcrootc_transfer_to_deadcrootc, nf->deadcrootn_transfer_to_deadcrootn, 0)) 
	{
		cs->deadcrootc               += cf->deadcrootc_transfer_to_deadcrootc;
		cs->deadcrootc_transfer      -= cf->deadcrootc_transfer_to_deadcrootc;
		ns->deadcrootn               = cs->deadcrootc / epc->deadwood_cn;
		ns->deadcrootn_transfer      = cs->deadcrootc_transfer / epc->deadwood_cn;
	}
	else if (epc->deadwood_cn)		
	{
		if (!errorCode) printf("ERROR in deadcroot CN calculation in state_update.c\n");
		errorCode=1;
	}

	
	/* 2. Aboveground pool litterfall and retranslocation to the first soil layer */
	leafc_to_litr = cf->leafc_to_litr1c + cf->leafc_to_litr2c + cf->leafc_to_litr3c + cf->leafc_to_litr4c;
	leafn_to_litr = nf->leafn_to_litr1n + nf->leafn_to_litr2n + nf->leafn_to_litr3n + nf->leafn_to_litr4n;
		
	/* new feature: litter turns into the first AND the second soil layer */
	propLAYER0 = sitec->soillayer_thickness[0]/sitec->soillayer_depth[2];
	propLAYER1 = sitec->soillayer_thickness[1]/sitec->soillayer_depth[2];
	propLAYER2 = sitec->soillayer_thickness[2]/sitec->soillayer_depth[2];

	

	if (!errorCode && epc->leaf_cn && CNratio_control(cs, epc->leaf_cn, cs->leafc, ns->leafn, leafc_to_litr, leafn_to_litr, epc->leaflitr_cn)) 
	{
		cs->litr1c[0]  += (cf->leafc_to_litr1c) * propLAYER0;
		cs->litr2c[0]  += (cf->leafc_to_litr2c) * propLAYER0;
		cs->litr3c[0]  += (cf->leafc_to_litr3c) * propLAYER0;
		cs->litr4c[0]  += (cf->leafc_to_litr4c) * propLAYER0;

		cs->litr1c[1]  += (cf->leafc_to_litr1c) * propLAYER1;
		cs->litr2c[1]  += (cf->leafc_to_litr2c) * propLAYER1;
		cs->litr3c[1]  += (cf->leafc_to_litr3c) * propLAYER1;
		cs->litr4c[1]  += (cf->leafc_to_litr4c) * propLAYER1;

		cs->litr1c[2]  += (cf->leafc_to_litr1c) * propLAYER2;
		cs->litr2c[2]  += (cf->leafc_to_litr2c) * propLAYER2;
		cs->litr3c[2]  += (cf->leafc_to_litr3c) * propLAYER2;
		cs->litr4c[2]  += (cf->leafc_to_litr4c) * propLAYER2;
		
		cs->leafc      -= (cf->leafc_to_litr1c + cf->leafc_to_litr2c + cf->leafc_to_litr3c + cf->leafc_to_litr4c);

		ns->litr1n[0]  += (nf->leafn_to_litr1n) * propLAYER0;
		ns->litr2n[0]  += (nf->leafn_to_litr2n) * propLAYER0;
		ns->litr3n[0]  += (nf->leafn_to_litr3n) * propLAYER0;
		ns->litr4n[0]  += (nf->leafn_to_litr4n) * propLAYER0;

		ns->litr1n[1]  += (nf->leafn_to_litr1n) * propLAYER1;
		ns->litr2n[1]  += (nf->leafn_to_litr2n) * propLAYER1;
		ns->litr3n[1]  += (nf->leafn_to_litr3n) * propLAYER1;
		ns->litr4n[1]  += (nf->leafn_to_litr4n) * propLAYER1;
		
		ns->litr1n[2]  += (nf->leafn_to_litr1n) * propLAYER2;
		ns->litr2n[2]  += (nf->leafn_to_litr2n) * propLAYER2;
		ns->litr3n[2]  += (nf->leafn_to_litr3n) * propLAYER2;
		ns->litr4n[2]  += (nf->leafn_to_litr4n) * propLAYER2;

		ns->leafn       = cs->leafc / epc->leaf_cn;

		ns->retransn   += nf->leafn_to_retransn;   

		
	}
	else if (epc->leaflitr_cn || epc->leaf_cn)		
	{
		if (!errorCode) printf("ERROR in leaf_to_litr CN calculation in state_update.c\n");
		errorCode=1;
	}
	
	yieldc_to_litr = cf->yieldc_to_litr1c + cf->yieldc_to_litr2c + cf->yieldc_to_litr3c + cf->yieldc_to_litr4c;
	yieldn_to_litr = nf->yieldn_to_litr1n + nf->yieldn_to_litr2n + nf->yieldn_to_litr3n + nf->yieldn_to_litr4n;
	if (!errorCode && epc->yield_cn && CNratio_control(cs, epc->yield_cn, cs->yieldc, ns->yieldn, yieldc_to_litr, yieldn_to_litr, 0)) 
	{
		cs->litr1c[0]  += (cf->yieldc_to_litr1c) * propLAYER0;
		cs->litr2c[0]  += (cf->yieldc_to_litr2c) * propLAYER0;
		cs->litr3c[0]  += (cf->yieldc_to_litr3c) * propLAYER0;
		cs->litr4c[0]  += (cf->yieldc_to_litr4c) * propLAYER0;

		cs->litr1c[1]  += (cf->yieldc_to_litr1c) * propLAYER1;
		cs->litr2c[1]  += (cf->yieldc_to_litr2c) * propLAYER1;
		cs->litr3c[1]  += (cf->yieldc_to_litr3c) * propLAYER1;
		cs->litr4c[1]  += (cf->yieldc_to_litr4c) * propLAYER1;

		cs->litr1c[2]  += (cf->yieldc_to_litr1c) * propLAYER2;
		cs->litr2c[2]  += (cf->yieldc_to_litr2c) * propLAYER2;
		cs->litr3c[2]  += (cf->yieldc_to_litr3c) * propLAYER2;
		cs->litr4c[2]  += (cf->yieldc_to_litr4c) * propLAYER2;

		cs->yieldc      -= (cf->yieldc_to_litr1c + cf->yieldc_to_litr2c + cf->yieldc_to_litr3c + cf->yieldc_to_litr4c);

		ns->litr1n[0]  += (nf->yieldn_to_litr1n) * propLAYER0;
		ns->litr2n[0]  += (nf->yieldn_to_litr2n) * propLAYER0;
		ns->litr3n[0]  += (nf->yieldn_to_litr3n) * propLAYER0;
		ns->litr4n[0]  += (nf->yieldn_to_litr4n) * propLAYER0;

		ns->litr1n[1]  += (nf->yieldn_to_litr1n) * propLAYER1;
		ns->litr2n[1]  += (nf->yieldn_to_litr2n) * propLAYER1;
		ns->litr3n[1]  += (nf->yieldn_to_litr3n) * propLAYER1;
		ns->litr4n[1]  += (nf->yieldn_to_litr4n) * propLAYER1;	

		ns->litr1n[2]  += (nf->yieldn_to_litr1n) * propLAYER2;
		ns->litr2n[2]  += (nf->yieldn_to_litr2n) * propLAYER2;
		ns->litr3n[2]  += (nf->yieldn_to_litr3n) * propLAYER2;
		ns->litr4n[2]  += (nf->yieldn_to_litr4n) * propLAYER2;	

		ns->yieldn      = cs->yieldc / epc->yield_cn;
	}
	else if (epc->yield_cn)		
	{
		if (!errorCode) printf("ERROR in yieldc_to_litr CN calculation in state_update.c\n");
		errorCode=1;
	}

	softstemc_to_litr = cf->softstemc_to_litr1c + cf->softstemc_to_litr2c + cf->softstemc_to_litr3c + cf->softstemc_to_litr4c;
	softstemn_to_litr = nf->softstemn_to_litr1n + nf->softstemn_to_litr2n + nf->softstemn_to_litr3n + nf->softstemn_to_litr4n;
	if (!errorCode && epc->softstem_cn && CNratio_control(cs, epc->softstem_cn, cs->softstemc, ns->softstemn, softstemc_to_litr, softstemn_to_litr, 0)) 
	{
		cs->litr1c[0]  += (cf->softstemc_to_litr1c) * propLAYER0;
		cs->litr2c[0]  += (cf->softstemc_to_litr2c) * propLAYER0;
		cs->litr3c[0]  += (cf->softstemc_to_litr3c) * propLAYER0;
		cs->litr4c[0]  += (cf->softstemc_to_litr4c) * propLAYER0;
				
		cs->litr1c[1]  += (cf->softstemc_to_litr1c) * propLAYER1;
		cs->litr2c[1]  += (cf->softstemc_to_litr2c) * propLAYER1;
		cs->litr3c[1]  += (cf->softstemc_to_litr3c) * propLAYER1;
		cs->litr4c[1]  += (cf->softstemc_to_litr4c) * propLAYER1;

		cs->litr1c[2]  += (cf->softstemc_to_litr1c) * propLAYER2;
		cs->litr2c[2]  += (cf->softstemc_to_litr2c) * propLAYER2;
		cs->litr3c[2]  += (cf->softstemc_to_litr3c) * propLAYER2;
		cs->litr4c[2]  += (cf->softstemc_to_litr4c) * propLAYER2;
		cs->softstemc  -= (cf->softstemc_to_litr1c + cf->softstemc_to_litr2c + cf->softstemc_to_litr3c + cf->softstemc_to_litr4c);

		ns->litr1n[0]  += (nf->softstemn_to_litr1n) * propLAYER0;
		ns->litr2n[0]  += (nf->softstemn_to_litr2n) * propLAYER0;
		ns->litr3n[0]  += (nf->softstemn_to_litr3n) * propLAYER0;
		ns->litr4n[0]  += (nf->softstemn_to_litr4n) * propLAYER0;
		
		ns->litr1n[1]  += (nf->softstemn_to_litr1n) * propLAYER1;
		ns->litr2n[1]  += (nf->softstemn_to_litr2n) * propLAYER1;
		ns->litr3n[1]  += (nf->softstemn_to_litr3n) * propLAYER1;
		ns->litr4n[1]  += (nf->softstemn_to_litr4n) * propLAYER1;

		ns->litr1n[2]  += (nf->softstemn_to_litr1n) * propLAYER2;
		ns->litr2n[2]  += (nf->softstemn_to_litr2n) * propLAYER2;
		ns->litr3n[2]  += (nf->softstemn_to_litr3n) * propLAYER2;
		ns->litr4n[2]  += (nf->softstemn_to_litr4n) * propLAYER2;

		ns->softstemn   = cs->softstemc / epc->softstem_cn;
	}
	else if (epc->softstem_cn)		
	{
		if (!errorCode) printf("ERROR in softstem_to_litr CN calculation in state_update.c\n");
		errorCode=1;
	}
	
	/* 3. Belowground litterfall is distributed between the different soil layers */
	
	frootc_to_litr = cf->frootc_to_litr1c + cf->frootc_to_litr2c + cf->frootc_to_litr3c + cf->frootc_to_litr4c;
	frootn_to_litr = nf->frootn_to_litr1n + nf->frootn_to_litr2n + nf->frootn_to_litr3n + nf->frootn_to_litr4n;
	if (!errorCode && epc->froot_cn && CNratio_control(cs, epc->froot_cn, cs->frootc, ns->frootn, frootc_to_litr, frootn_to_litr, 0)) 
	{
		for (layer = 0; layer < N_SOILLAYERS; layer++)
		{
			cs->litr1c[layer] += cf->frootc_to_litr1c * epv->rootlengthProp[layer];
			cs->litr2c[layer] += cf->frootc_to_litr2c * epv->rootlengthProp[layer];
			cs->litr3c[layer] += cf->frootc_to_litr3c * epv->rootlengthProp[layer];
			cs->litr4c[layer] += cf->frootc_to_litr4c * epv->rootlengthProp[layer];

			ns->litr1n[layer] += nf->frootn_to_litr1n * epv->rootlengthProp[layer];
			ns->litr2n[layer] += nf->frootn_to_litr2n * epv->rootlengthProp[layer];
			ns->litr3n[layer] += nf->frootn_to_litr3n * epv->rootlengthProp[layer];
			ns->litr4n[layer] += nf->frootn_to_litr4n * epv->rootlengthProp[layer];
		}
		cs->frootc       -= frootc_to_litr;
		ns->frootn	      = cs->frootc / epc->froot_cn;
	}
	else if (epc->froot_cn)		
	{
		if (!errorCode) printf("ERROR in froot_to_litr CN calculation in state_update.c\n");
		errorCode=1;
	}

	 /* 4. Livewood turnover fluxes */
	cs->deadstemc  += cf->livestemc_to_deadstemc;
	cs->livestemc  -= cf->livestemc_to_deadstemc;
	cs->deadcrootc += cf->livecrootc_to_deadcrootc;
	cs->livecrootc -= cf->livecrootc_to_deadcrootc;
	ns->retransn      += nf->livestemn_to_retransn;  
	ns->livestemn     -= nf->livestemn_to_retransn;
	ns->retransn      += nf->livecrootn_to_retransn; 
	ns->livecrootn    -= nf->livecrootn_to_retransn;
	

	ns->deadstemn  += nf->livestemn_to_deadstemn;
	ns->livestemn  -= nf->livestemn_to_deadstemn;
	ns->deadcrootn += nf->livecrootn_to_deadcrootn;
	ns->livecrootn -= nf->livecrootn_to_deadcrootn;
	

	/* 6. Photosynthesis fluxes */
	cs->psnsun_src   += cf->psnsun_to_cpool;
	cs->psnshade_src += cf->psnshade_to_cpool;
	cs->cpool        += (cf->psnsun_to_cpool + cf->psnshade_to_cpool);

	/* 7. Plant allocation flux, from N retrans pool */
	ns->npool		    += nf->retransn_to_npool_total;
	ns->retransn        -= nf->retransn_to_npool_total;

	
	/* 8. Litter decomposition fluxes - MULTILAYER SOIL */
	for (layer = 0; layer < N_SOILLAYERS; layer++)
	{
		/* Fluxes out of coarse woody debris into litter pools */
		cs->litr2c[layer]       += cf->cwdc_to_litr2c[layer];
		cs->litr3c[layer]       += cf->cwdc_to_litr3c[layer];
		cs->litr4c[layer]       += cf->cwdc_to_litr4c[layer];
		cs->cwdc[layer]         -= (cf->cwdc_to_litr2c[layer] + cf->cwdc_to_litr3c[layer] + cf->cwdc_to_litr4c[layer]);
		/* Fluxes out of labile litter pool */
		cs->HRlitr1_snk		+= cf->litr1_hr[layer];
		cs->litr1c[layer]       -= cf->litr1_hr[layer];
		cs->soil1c[layer]       += cf->litr1c_to_soil1c[layer];
		cs->litr1c[layer]       -= cf->litr1c_to_soil1c[layer];
		/* Fluxes out of cellulose litter pool */
		cs->HRlitr2_snk		+= cf->litr2_hr[layer];
		cs->litr2c[layer]       -= cf->litr2_hr[layer];
		cs->soil2c[layer]       += cf->litr2c_to_soil2c[layer];
		cs->litr2c[layer]       -= cf->litr2c_to_soil2c[layer];
		/* Fluxes from shielded to unshielded cellulose pools */
		cs->litr2c[layer]       += cf->litr3c_to_litr2c[layer];
		cs->litr3c[layer]       -= cf->litr3c_to_litr2c[layer];
		/* Fluxes out of lignin litter pool */
		cs->HRlitr4_snk		+= cf->litr4_hr[layer];
		cs->litr4c[layer]       -= cf->litr4_hr[layer];
		cs->soil3c[layer]       += cf->litr4c_to_soil3c[layer];
		cs->litr4c[layer]       -= cf->litr4c_to_soil3c[layer];
		/* Fluxes out of fast soil pool */
		cs->HRsoil1_snk		+= cf->soil1_hr[layer];
		cs->soil1c[layer]       -= cf->soil1_hr[layer];
		cs->soil2c[layer]       += cf->soil1c_to_soil2c[layer];
		cs->soil1c[layer]       -= cf->soil1c_to_soil2c[layer];
		/* Fluxes out of medium soil pool */
		cs->HRsoil2_snk		+= cf->soil2_hr[layer];
		cs->soil2c[layer]       -= cf->soil2_hr[layer];
		cs->soil3c[layer]       += cf->soil2c_to_soil3c[layer];
		cs->soil2c[layer]       -= cf->soil2c_to_soil3c[layer];
		/* Fluxes out of slow soil pool */
		cs->HRsoil3_snk		+= cf->soil3_hr[layer];
		cs->soil3c[layer]       -= cf->soil3_hr[layer];
		cs->soil4c[layer]       += cf->soil3c_to_soil4c[layer];
		cs->soil3c[layer]       -= cf->soil3c_to_soil4c[layer];
		/* Fluxes out of recalcitrant SOM pool */
		cs->HRsoil4_snk		+= cf->soil4_hr[layer];
		cs->soil4c[layer]       -= cf->soil4_hr[layer];

	
		/* Fluxes out of coarse woody debris into litter pools */
		ns->litr2n[layer]       += nf->cwdn_to_litr2n[layer];
		ns->litr3n[layer]       += nf->cwdn_to_litr3n[layer];
		ns->litr4n[layer]       += nf->cwdn_to_litr4n[layer];
		ns->cwdn[layer]         -= (nf->cwdn_to_litr2n[layer] + nf->cwdn_to_litr3n[layer] + nf->cwdn_to_litr4n[layer]);
		/* Fluxes out of labile litter pool */
		ns->soil1n[layer]       += nf->litr1n_to_soil1n[layer];
		ns->litr1n[layer]       -= nf->litr1n_to_soil1n[layer];
		ns->litr1n[layer]       -= nf->litr1n_to_release[layer];
		/* Fluxes out of cellulose litter pool */
		ns->soil2n[layer]       += nf->litr2n_to_soil2n[layer];
		ns->litr2n[layer]       -= nf->litr2n_to_soil2n[layer];
		ns->litr2n[layer]       -= nf->litr2n_to_release[layer];
		/* Fluxes from shielded to unshielded cellulose pools */
		ns->litr2n[layer]       += nf->litr3n_to_litr2n[layer];
		ns->litr3n[layer]       -= nf->litr3n_to_litr2n[layer];
		/* Fluxes out of lignin litter pool */
		ns->soil3n[layer]       += nf->litr4n_to_soil3n[layer];
		ns->litr4n[layer]       -= nf->litr4n_to_soil3n[layer];
		ns->litr4n[layer]       -= nf->litr4n_to_release[layer];
		/* Fluxes out of fast soil pool */
		ns->soil2n[layer]       += nf->soil1n_to_soil2n[layer];
		ns->soil1n[layer]       -= nf->soil1n_to_soil2n[layer];
		/* Fluxes out of medium soil pool */
		ns->soil3n[layer]       += nf->soil2n_to_soil3n[layer];
		ns->soil2n[layer]       -= nf->soil2n_to_soil3n[layer];
		/* Fluxes out of slow soil pool */
		ns->soil4n[layer]       += nf->soil3n_to_soil4n[layer];
		ns->soil3n[layer]       -= nf->soil3n_to_soil4n[layer];
	}


	/* 9. Daily allocation fluxes */
	/* daily leaf allocation fluxes */
	
	if (!errorCode && epc->leaf_cn && CNratio_control(cs, epc->leaf_cn, cs->leafc, ns->leafn, cf->cpool_to_leafc, nf->npool_to_leafn, 0)) 
	{
		cs->cpool          -= cf->cpool_to_leafc;
		cs->leafc          += cf->cpool_to_leafc;
		
		ns->npool          -= nf->npool_to_leafn;
		ns->leafn           = cs->leafc / epc->leaf_cn;
	}
	else if (epc->leaf_cn)		
	{
		if (!errorCode) printf("ERROR in cpool_to_leafc CN calculation in state_update.c\n");
		errorCode=1;
	}

	if (!errorCode && epc->leaf_cn && CNratio_control(cs, epc->leaf_cn, cs->leafc_storage, ns->leafn_storage, cf->cpool_to_leafc_storage, nf->npool_to_leafn_storage, 0)) 
	{
		cs->cpool          -= cf->cpool_to_leafc_storage;
		cs->leafc_storage  += cf->cpool_to_leafc_storage;
		
		ns->npool          -= nf->npool_to_leafn_storage;
		ns->leafn_storage   = cs->leafc_storage / epc->leaf_cn;
	}
	else if (epc->leaf_cn)		
	{
		if (!errorCode) printf("ERROR in cpool_to_leafc_storage CN calculation in state_update.c\n");
		errorCode=1;
	}

	/* Daily fine root allocation fluxes */
	if (!errorCode && epc->froot_cn && CNratio_control(cs, epc->froot_cn, cs->frootc, ns->frootn, cf->cpool_to_frootc, nf->npool_to_frootn, 0)) 
	{
		cs->cpool          -= cf->cpool_to_frootc;
		cs->frootc         += cf->cpool_to_frootc;
		
		ns->npool          -= nf->npool_to_frootn;
		ns->frootn          = cs->frootc / epc->froot_cn;
	}
	else if (epc->froot_cn)		
	{
		if (!errorCode) printf("ERROR in cpool_to_frootc CN calculation in state_update.c\n");
		errorCode=1;
	}

	if (!errorCode && epc->froot_cn && CNratio_control(cs, epc->froot_cn, cs->frootc_storage, ns->frootn_storage, cf->cpool_to_frootc_storage, nf->npool_to_frootn_storage, 0)) 
	{
		cs->cpool          -= cf->cpool_to_frootc_storage;
		cs->frootc_storage += cf->cpool_to_frootc_storage;
		
		ns->npool          -= nf->npool_to_frootn_storage;
		ns->frootn_storage  = cs->frootc_storage / epc->froot_cn;
	}
	else if (epc->froot_cn)		
	{
		if (!errorCode) printf("ERROR in cpool_to_frootc_storage CN calculation in state_update.c\n");
		errorCode=1;
	}

	/* Daily yield allocation fluxes + EXTRA: effect of heat stress during flowering in yield filling phenophase */
	if (!errorCode && epc->yield_cn && CNratio_control(cs, epc->yield_cn, cs->yieldc, ns->yieldn, cf->cpool_to_yield, nf->npool_to_yieldn, 0)) 
	{
		cs->cpool          -= cf->cpool_to_yield;
		cs->yieldc         += cf->cpool_to_yield;
		
		ns->npool          -= nf->npool_to_yieldn;
		ns->yieldn          = cs->yieldc / epc->yield_cn;
			
		cs->yieldc         -= cf->yieldc_to_flowHS;
		cs->STDBc_yield    += cf->yieldc_to_flowHS;

		ns->yieldn         -= nf->yieldn_to_flowHS;
		ns->STDBn_yield    += nf->yieldn_to_flowHS;

		/* control */
		if ((cf->yieldc_to_flowHS > 0 && nf->yieldn_to_flowHS > 0 && epv->n_actphen != epc->n_flowHS_phenophase) || 
			(cs->yieldc < 0 && fabs(cs->yieldc) > CRIT_PREC)  || (ns->yieldn < 0 && fabs(ns->yieldn) > CRIT_PREC)  )
		{
			if (!errorCode) printf("ERROR in flowering heat stress calculation in state_update.c\n");
			errorCode=1;
		}
	}
	else if (epc->yield_cn)		
	{
		if (!errorCode) printf("ERROR in cpool_to_yield CN calculation in state_update.c\n");
		errorCode=1;
	}

	if (!errorCode && epc->yield_cn && CNratio_control(cs, epc->yield_cn, cs->yieldc_storage, ns->yieldn_storage, cf->cpool_to_yieldc_storage, nf->npool_to_yieldn_storage, 0)) 
	{
		cs->cpool          -= cf->cpool_to_yieldc_storage;
		cs->yieldc_storage += cf->cpool_to_yieldc_storage;
		
		ns->npool          -= nf->npool_to_yieldn_storage;
		ns->yieldn_storage  = cs->yieldc_storage / epc->yield_cn;
	}
	else if (epc->yield_cn)		
	{
		if (!errorCode) printf("ERROR in cpool_to_yieldc_storage CN calculation in state_update.c\n");
		errorCode=1;
	}
	
	/* Daily sofstem allocation fluxes */
	if (!errorCode && epc->softstem_cn && CNratio_control(cs, epc->softstem_cn, cs->softstemc, ns->softstemn, cf->cpool_to_softstemc, nf->npool_to_softstemn, 0)) 
	{
		cs->cpool          -= cf->cpool_to_softstemc;
		cs->softstemc       += cf->cpool_to_softstemc;
		
		ns->npool          -= nf->npool_to_softstemn;
		ns->softstemn       = cs->softstemc / epc->softstem_cn;
	}
	else if (epc->softstem_cn)		
	{
		if (!errorCode) printf("ERROR in cpool_to_softstemc CN calculation in state_update.c\n");
		errorCode=1;
	}

	if (!errorCode && epc->softstem_cn && CNratio_control(cs, epc->softstem_cn, cs->softstemc_storage, ns->softstemn_storage, cf->cpool_to_softstemc_storage, nf->npool_to_softstemn_storage, 0)) 
	{
		cs->cpool              -= cf->cpool_to_softstemc_storage;
		cs->softstemc_storage  += cf->cpool_to_softstemc_storage;
		
		ns->npool              -= nf->npool_to_softstemn_storage;
		ns->softstemn_storage   = cs->softstemc_storage / epc->softstem_cn;
	}
	else if (epc->softstem_cn)		
	{
		if (!errorCode) printf("ERROR in cpool_to_softstemc_storage CN calculation in state_update.c\n");
		errorCode=1;
	}			

	/* Daily live stem wood allocation fluxes */
	if (!errorCode && epc->livewood_cn && CNratio_control(cs, epc->livewood_cn, cs->livestemc, ns->livestemn, cf->cpool_to_livestemc, nf->npool_to_livestemn, 0)) 
	{
		cs->cpool      -= cf->cpool_to_livestemc;
		cs->livestemc  += cf->cpool_to_livestemc;
		
		ns->npool      -= nf->npool_to_livestemn;
		ns->livestemn   = cs->livestemc / epc->livewood_cn;
	}
	else if (epc->livewood_cn)		
	{
		if (!errorCode) printf("ERROR in cpool_to_livestemc CN calculation in state_update.c\n");
		errorCode=1;
	}

	if (!errorCode && epc->livewood_cn && CNratio_control(cs, epc->livewood_cn, cs->livestemc_storage, ns->livestemn_storage, cf->cpool_to_livestemc_storage, nf->npool_to_livestemn_storage, 0)) 
	{
		cs->cpool              -= cf->cpool_to_livestemc_storage;
		cs->livestemc_storage  += cf->cpool_to_livestemc_storage;
		
		ns->npool              -= nf->npool_to_livestemn_storage;
		ns->livestemn_storage   = cs->livestemc_storage / epc->livewood_cn;
	}
	else if (epc->livewood_cn)		
	{
		if (!errorCode) printf("ERROR in cpool_to_livestemc_storage CN calculation in state_update.c\n");
		errorCode=1;
	}
	
	/* Daily dead stem wood allocation fluxes */
	if (!errorCode && epc->deadwood_cn && CNratio_control(cs, epc->deadwood_cn, cs->deadstemc, ns->deadstemn, cf->cpool_to_deadstemc, nf->npool_to_deadstemn, 0)) 
	{
		cs->cpool          -= cf->cpool_to_deadstemc;
		cs->deadstemc          += cf->cpool_to_deadstemc;
		
		ns->npool          -= nf->npool_to_deadstemn;
		ns->deadstemn           = cs->deadstemc / epc->deadwood_cn;
	}
	else if (epc->deadwood_cn)		
	{
		if (!errorCode) printf("ERROR in cpool_to_deadstemc CN calculation in state_update.c\n");
		errorCode=1;
	}

	if (!errorCode && epc->deadwood_cn && CNratio_control(cs, epc->deadwood_cn, cs->deadstemc_storage, ns->deadstemn_storage, cf->cpool_to_deadstemc_storage, nf->npool_to_deadstemn_storage, 0)) 
	{
		cs->cpool          -= cf->cpool_to_deadstemc_storage;
		cs->deadstemc_storage  += cf->cpool_to_deadstemc_storage;
		
		ns->npool          -= nf->npool_to_deadstemn_storage;
		ns->deadstemn_storage   = cs->deadstemc_storage / epc->deadwood_cn;
	}
	else if (epc->deadwood_cn)		
	{
		if (!errorCode) printf("ERROR in cpool_to_deadstemc_storage CN calculation in state_update.c\n");
		errorCode=1;
	}
	
	/* Daily live coarse root wood allocation fluxes */
	if (!errorCode && epc->livewood_cn && CNratio_control(cs, epc->livewood_cn, cs->livecrootc, ns->livecrootn, cf->cpool_to_livecrootc, nf->npool_to_livecrootn, 0)) 
	{
		cs->cpool          -= cf->cpool_to_livecrootc;
		cs->livecrootc          += cf->cpool_to_livecrootc;
		
		ns->npool          -= nf->npool_to_livecrootn;
		ns->livecrootn           = cs->livecrootc / epc->livewood_cn;
	}
	else if (epc->livewood_cn)		
	{
		if (!errorCode) printf("ERROR in cpool_to_livecrootc CN calculation in state_update.c\n");
		errorCode=1;
	}

	if (!errorCode && epc->livewood_cn && CNratio_control(cs, epc->livewood_cn, cs->livecrootc_storage, ns->livecrootn_storage, cf->cpool_to_livecrootc_storage, nf->npool_to_livecrootn_storage, 0)) 
	{
		cs->cpool          -= cf->cpool_to_livecrootc_storage;
		cs->livecrootc_storage  += cf->cpool_to_livecrootc_storage;
		
		ns->npool          -= nf->npool_to_livecrootn_storage;
		ns->livecrootn_storage   = cs->livecrootc_storage / epc->livewood_cn;
	}
	else if (epc->livewood_cn)		
	{
		if (!errorCode) printf("ERROR in cpool_to_livecrootc_storage CN calculation in state_update.c\n");
		errorCode=1;
	}

	/* Daily dead coarse root wood allocation fluxes */
	if (!errorCode && epc->deadwood_cn && CNratio_control(cs, epc->deadwood_cn, cs->deadcrootc, ns->deadcrootn, cf->cpool_to_deadcrootc, nf->npool_to_deadcrootn, 0)) 
	{
		cs->cpool          -= cf->cpool_to_deadcrootc;
		cs->deadcrootc          += cf->cpool_to_deadcrootc;
		
		ns->npool          -= nf->npool_to_deadcrootn;
		ns->deadcrootn           = cs->deadcrootc / epc->deadwood_cn;
	}
	else if (epc->deadwood_cn)		
	{
		if (!errorCode) printf("ERROR in cpool_to_deadcrootc CN calculation in state_update.c\n");
		errorCode=1;
	}

	if (!errorCode && epc->deadwood_cn && CNratio_control(cs, epc->deadwood_cn, cs->deadcrootc_storage, ns->deadcrootn_storage, cf->cpool_to_deadcrootc_storage, nf->npool_to_deadcrootn_storage, 0)) 
	{
		cs->cpool          -= cf->cpool_to_deadcrootc_storage;
		cs->deadcrootc_storage  += cf->cpool_to_deadcrootc_storage;
		
		ns->npool          -= nf->npool_to_deadcrootn_storage;
		ns->deadcrootn_storage   = cs->deadcrootc_storage / epc->deadwood_cn;
	}
	else if (epc->deadwood_cn)		
	{
		if (!errorCode) printf("ERROR in cpool_to_deadcrootc_storage CN calculation in state_update.c\n");
		errorCode=1;
	}
	

	/* Daily allocation for transfer growth respiration */
	cs->gresp_storage  += cf->cpool_to_gresp_storage;
	cs->cpool          -= cf->cpool_to_gresp_storage;
	 
	/* Calculate sum of leafC in a given phenophase */
	pp = (int) epv->n_actphen - 1;
	cs->leafcSUM_phenphase[pp] += cf->cpool_to_leafc + cf->leafc_transfer_to_leafc;

	
	/* 10. Daily growth respiration fluxes */
	/* Leaf growth respiration */
	cs->GRleaf_snk     += cf->cpool_leaf_GR;
	cs->cpool           -= cf->cpool_leaf_GR;
	cs->GRleaf_snk     += cf->cpool_leaf_storage_GR;
	cs->cpool           -= cf->cpool_leaf_storage_GR;
	cs->GRleaf_snk     += cf->transfer_leaf_GR;
	cs->gresp_transfer  -= cf->transfer_leaf_GR;
	/* Fine root growth respiration */
	cs->GRfroot_snk    += cf->cpool_froot_GR;
	cs->cpool           -= cf->cpool_froot_GR;
	cs->GRfroot_snk    += cf->cpool_froot_storage_GR;
	cs->cpool           -= cf->cpool_froot_storage_GR;
	cs->GRfroot_snk    += cf->transfer_froot_GR;
	cs->gresp_transfer  -= cf->transfer_froot_GR;
	/* yield growth respiration */
	cs->GRyield_snk     += cf->cpool_yield_GR;
	cs->cpool            -= cf->cpool_yield_GR;
	cs->GRyield_snk     += cf->cpool_yieldc_storage_GR;
	cs->cpool            -= cf->cpool_yieldc_storage_GR;
	cs->GRyield_snk     += cf->transfer_yield_GR;
	cs->gresp_transfer   -= cf->transfer_yield_GR;
	/* yield growth respiration. */
	cs->GRsoftstem_snk  += cf->cpool_softstem_GR;
	cs->cpool            -= cf->cpool_softstem_GR;
	cs->GRsoftstem_snk  += cf->cpool_softstem_storage_GR;
	cs->cpool            -= cf->cpool_softstem_storage_GR;
	cs->GRsoftstem_snk  += cf->transfer_softstem_GR;
	cs->gresp_transfer   -= cf->transfer_softstem_GR;
	/* Live stem growth respiration */ 
	cs->GRlivestem_snk  += cf->cpool_livestem_GR;
	cs->cpool            -= cf->cpool_livestem_GR;
	cs->GRlivestem_snk  += cf->cpool_livestem_storage_GR;
	cs->cpool            -= cf->cpool_livestem_storage_GR;
	cs->GRlivestem_snk  += cf->transfer_livestem_GR;
	cs->gresp_transfer   -= cf->transfer_livestem_GR;
	/* Dead stem growth respiration */ 
	cs->GRdeadstem_snk  += cf->cpool_deadstem_GR;
	cs->cpool            -= cf->cpool_deadstem_GR;
	cs->GRdeadstem_snk  += cf->cpool_deadstem_storage_GR;
	cs->cpool            -= cf->cpool_deadstem_storage_GR;
	cs->GRdeadstem_snk  += cf->transfer_deadstem_GR;
	cs->gresp_transfer   -= cf->transfer_deadstem_GR;
	/* Live coarse root growth respiration */ 
	cs->GRlivecroot_snk += cf->cpool_livecroot_GR;
	cs->cpool            -= cf->cpool_livecroot_GR;
	cs->GRlivecroot_snk += cf->cpool_livecroot_storage_GR;
	cs->cpool            -= cf->cpool_livecroot_storage_GR;
	cs->GRlivecroot_snk += cf->transfer_livecroot_GR;
	cs->gresp_transfer   -= cf->transfer_livecroot_GR;
	/* Dead coarse root growth respiration */ 
	cs->GRdeadcroot_snk += cf->cpool_deadcroot_GR;
	cs->cpool            -= cf->cpool_deadcroot_GR;
	cs->GRdeadcroot_snk += cf->cpool_deadcroot_storage_GR;
	cs->cpool            -= cf->cpool_deadcroot_storage_GR;
	cs->GRdeadcroot_snk += cf->transfer_deadcroot_GR;
	cs->gresp_transfer   -= cf->transfer_deadcroot_GR;

	

	/* 11. Maintanance respiration fluxes
	       covering of maintananance respiration fluxes from NSC (non-structural carbohydrate), namely storage and transfer pools*/
	if (!errorCode && nsc_maintresp(epc, ctrl, epv, cf, nf, cs, ns))
	{
		errorCode=1;
		if (!errorCode) printf("ERROR in nsc_maintresp() from bgc()\n");
	}
	
	
	
	
	/* 12. Annual allocation fluxes, one day per year */
	if (alloc)
	{
		/* Move storage material into transfer compartments on the annual
		allocation day. This is a special case, where a flux is assessed in
		the state_variable update routine.  This is required to have the
		allocation of excess C and N show up as new growth in the next growing
		season, instead of two growing seasons from now. */
		cf->leafc_storage_to_leafc_transfer				= cs->leafc_storage;
		cf->frootc_storage_to_frootc_transfer			= cs->frootc_storage;
		cf->yieldc_storage_to_yieldc_transfer			= cs->yieldc_storage;
		cf->softstemc_storage_to_softstemc_transfer		= cs->softstemc_storage;
		cf->gresp_storage_to_gresp_transfer				= cs->gresp_storage;
		cf->livestemc_storage_to_livestemc_transfer		= cs->livestemc_storage;
		cf->deadstemc_storage_to_deadstemc_transfer		= cs->deadstemc_storage;
		cf->livecrootc_storage_to_livecrootc_transfer	= cs->livecrootc_storage;
		cf->deadcrootc_storage_to_deadcrootc_transfer	= cs->deadcrootc_storage;

		nf->leafn_storage_to_leafn_transfer				= ns->leafn_storage;
		nf->frootn_storage_to_frootn_transfer			= ns->frootn_storage;
		nf->yieldn_storage_to_yieldn_transfer			= ns->yieldn_storage;
		nf->softstemn_storage_to_softstemn_transfer		= ns->softstemn_storage;
		nf->livestemn_storage_to_livestemn_transfer		= ns->livestemn_storage;
		nf->deadstemn_storage_to_deadstemn_transfer		= ns->deadstemn_storage;
		nf->livecrootn_storage_to_livecrootn_transfer	= ns->livecrootn_storage;
		nf->deadcrootn_storage_to_deadcrootn_transfer	= ns->deadcrootn_storage;
		

		/* update states variables */
		cs->leafc_transfer     += cf->leafc_storage_to_leafc_transfer;
		cs->leafc_storage      -= cf->leafc_storage_to_leafc_transfer;
		cs->frootc_transfer    += cf->frootc_storage_to_frootc_transfer;
		cs->frootc_storage     -= cf->frootc_storage_to_frootc_transfer;
		cs->yieldc_transfer    += cf->yieldc_storage_to_yieldc_transfer;
		cs->yieldc_storage     -= cf->yieldc_storage_to_yieldc_transfer;
		cs->softstemc_transfer += cf->softstemc_storage_to_softstemc_transfer;
		cs->softstemc_storage  -= cf->softstemc_storage_to_softstemc_transfer;
		cs->gresp_transfer     += cf->gresp_storage_to_gresp_transfer;
		cs->gresp_storage      -= cf->gresp_storage_to_gresp_transfer;

		ns->leafn_transfer     += nf->leafn_storage_to_leafn_transfer;
		ns->leafn_storage      -= nf->leafn_storage_to_leafn_transfer;
		ns->frootn_transfer    += nf->frootn_storage_to_frootn_transfer;
		ns->frootn_storage     -= nf->frootn_storage_to_frootn_transfer;
		ns->yieldn_transfer    += nf->yieldn_storage_to_yieldn_transfer;
		ns->yieldn_storage     -= nf->yieldn_storage_to_yieldn_transfer;
		ns->softstemn_transfer += nf->softstemn_storage_to_softstemn_transfer;
		ns->softstemn_storage  -= nf->softstemn_storage_to_softstemn_transfer;


		cs->livestemc_transfer  += cf->livestemc_storage_to_livestemc_transfer;
		cs->livestemc_storage   -= cf->livestemc_storage_to_livestemc_transfer;
		cs->deadstemc_transfer  += cf->deadstemc_storage_to_deadstemc_transfer;
		cs->deadstemc_storage   -= cf->deadstemc_storage_to_deadstemc_transfer;
		cs->livecrootc_transfer += cf->livecrootc_storage_to_livecrootc_transfer;
		cs->livecrootc_storage  -= cf->livecrootc_storage_to_livecrootc_transfer;
		cs->deadcrootc_transfer += cf->deadcrootc_storage_to_deadcrootc_transfer;
		cs->deadcrootc_storage  -= cf->deadcrootc_storage_to_deadcrootc_transfer;

		ns->livestemn_transfer  += nf->livestemn_storage_to_livestemn_transfer;
		ns->livestemn_storage   -= nf->livestemn_storage_to_livestemn_transfer;
		ns->deadstemn_transfer  += nf->deadstemn_storage_to_deadstemn_transfer;
		ns->deadstemn_storage   -= nf->deadstemn_storage_to_deadstemn_transfer;
		ns->livecrootn_transfer += nf->livecrootn_storage_to_livecrootn_transfer;
		ns->livecrootn_storage  -= nf->livecrootn_storage_to_livecrootn_transfer;
		ns->deadcrootn_transfer += nf->deadcrootn_storage_to_deadcrootn_transfer;
		ns->deadcrootn_storage  -= nf->deadcrootn_storage_to_deadcrootn_transfer;
		


		/* for deciduous system, force leafc and frootc to exactly 0.0 on the last day */
		if (!evergreen)
		{
			if (-cs->leafc > CRIT_PREC || -cs->frootc > CRIT_PREC || -cs->yieldc > CRIT_PREC || -cs->softstemc > CRIT_PREC)
			{
				printf("\n");
				if (!errorCode) printf("ERROR: negative plant carbon pool in state_update.c\n");
				errorCode=1;
			}
			if (-ns->leafn > CRIT_PREC || -ns->frootn > CRIT_PREC || -ns->yieldn > CRIT_PREC || -ns->softstemn > CRIT_PREC)
			{
				printf("\n");
				if (!errorCode) printf("ERROR: negative plant nitrogen pool in state_update.c\n");
				errorCode=1;
			}
			if (cs->leafc < CRIT_PREC || ns->leafn  < CRIT_PREC) 
			{
				cs->leafc = 0.0;
				ns->leafn = 0.0;
			}
			if (cs->frootc < CRIT_PREC || ns->frootn  < CRIT_PREC) 
			{
				cs->frootc = 0.0;
				ns->frootn = 0.0;
			}
			if (cs->yieldc < CRIT_PREC || ns->yieldn  < CRIT_PREC) 
			{
				cs->yieldc = 0.0;
				ns->yieldn = 0.0;
			}
			if (cs->softstemc < CRIT_PREC || ns->softstemn  < CRIT_PREC) 
			{
				cs->softstemc = 0.0;
				ns->softstemn = 0.0;
			}
		
		}
	} /* end if allocation day */



	return (errorCode);
}			

int CNratio_control(cstate_struct* cs, double CNratio, double cpool, double npool, double cflux, double nflux, double CNratio_flux)
{
	int errorCode = 0;
	double CNdiff = 0;

	if (CNratio_flux == 0) CNratio_flux = CNratio;

	/* control for leaf C:N ratio of pools */
	if ((npool ==0 && cpool > CRIT_PREC ) || (npool > CRIT_PREC  && cpool == 0))
	{
		if (!errorCode) printf("ERROR: CNratio_control in CN_state_update.c\n");
		errorCode = 1;
	}
	
	if(npool > 0 && cpool > 0) CNdiff = cpool/npool - CNratio;
	

	if(fabs(CNdiff) > 0)
	{
		npool = cpool / CNratio;
		if (fabs(CNdiff) > cs->CNratioERR) 
		{
			cs->CNratioERR = fabs(CNdiff);
		}
	
	
	}

	CNdiff=0;

	/* control for leaf C:N ratio of fluxes */
	if ((nflux == 0 && cflux > CRIT_PREC ) || (nflux > CRIT_PREC  && cflux == 0))
	{
		if (!errorCode) printf("ERROR: CNratio_control in CN_state_update.c\n");
		errorCode = 1;
	}

	if(nflux > 0 && cflux > 0) CNdiff = cflux/nflux - CNratio_flux;


	if(fabs(CNdiff) > 0)
	{
		nflux = cflux / CNratio_flux;
		if (fabs(CNdiff) > cs->CNratioERR) cs->CNratioERR = fabs(CNdiff);
		
		CNdiff = cflux/nflux - CNratio;

	}
	
	return (!errorCode);
}

int nsc_maintresp(const epconst_struct* epc, control_struct *ctrl, epvar_struct* epv, cflux_struct* cf, nflux_struct* nf, cstate_struct* cs, nstate_struct* ns)
{	
	/* Covering of maintananance respiration fluxes from storage pools */
	
	int errorCode=0;
	double MR_nw, MR_w, NSCnw, NSCw, SCnw, SCw, nsc_crit, diff_total, diff_total_nw, diff_total_w, diff, day_MR_ratio, excess, nsc_avail;
	
	diff_total_nw = diff_total_w = day_MR_ratio = excess = 0;
	
	
	/* summarizing maint.resp fluxes and available non-structural carbohydrate fluxes - non-woody and woody */
	MR_nw = cf->leaf_day_MR + cf->leaf_night_MR + cf->froot_MR + cf->yield_MR + cf->softstem_MR;
	MR_w  = cf->livestem_MR + cf->livecroot_MR;
	
		
	NSCnw  = (cs->leafc_storage      +  cs->frootc_storage     + cs->yieldc_storage     + cs->softstemc_storage + 
			   cs->leafc_transfer     +  cs->frootc_transfer    + cs->yieldc_transfer    + cs->softstemc_transfer);

	NSCw  = (cs->livestemc_storage  + cs->livecrootc_storage  + cs->deadstemc_storage  + cs->deadcrootc_storage +
		      cs->livestemc_transfer + cs->livecrootc_transfer + cs->deadstemc_transfer + cs->deadcrootc_transfer);

	SCnw = cs->leafc     +  cs->frootc    + cs->yieldc     + cs->softstemc;

	SCw = cs->livestemc  + cs->livecrootc  + cs->deadstemc  + cs->deadcrootc;

	if (fabs(NSCnw) < CRIT_PREC) NSCnw = 0;
	if (fabs(NSCw)  < CRIT_PREC) NSCw = 0;
	if (fabs(SCnw)  < CRIT_PREC) SCnw = 0;
	if (fabs(SCw)   < CRIT_PREC) SCw = 0;

	
	/* calculation of difference between between the demand (MR) and the source (cpool) - non-woody and woody */
	diff_total  = MR_nw + MR_w - cs->cpool;

	if (MR_nw + MR_w)
	{
		diff_total_nw = diff_total * (MR_nw / (MR_nw + MR_w));
		diff_total_w  = diff_total * (MR_w  / (MR_nw + MR_w));
	}



	/* 1: non-woody biomass */
	if (MR_nw)
	{

		/* 1.1. calculation the difference between NSC and diff (based on available amount) */
		if (diff_total_nw > CRIT_PREC)
		{
			/* critical NSC value: NSC pool = fixed ratio of theroretical maximum of NSC value -> but not all is available */
			nsc_crit  = epc->NSC_avail_prop * (SCnw * epc->NSC_SC_prop);
			nsc_avail = NSCnw - nsc_crit;
			if (nsc_avail < 0) 
			{
				nsc_avail = 0;
			}
		
			/* MRdeficit_nw = 1: diff_total_nw < nsc_avail
			   MRdeficit_nw = 2: diff_total_nw > nsc_avail */
			 
			if (diff_total_nw < nsc_avail)
			{
				diff = diff_total_nw;
				epv->MRdeficit_nw = 1;
			}
			else
			{
				diff = nsc_avail;
				epv->MRdeficit_nw = 2;
			}
			


			/* 1.2. calculation of flxues from nsc pools */
			if (NSCnw && diff)
			{
				cf->leafc_storage_to_MR		 = diff * cs->leafc_storage/NSCnw;
				cf->frootc_storage_to_MR	 = diff * cs->frootc_storage/NSCnw;
				cf->yieldc_storage_to_MR	 = diff * cs->yieldc_storage/NSCnw;
				cf->softstemc_storage_to_MR	 = diff * cs->softstemc_storage/NSCnw;

				cf->leafc_transfer_to_MR	 = diff * cs->leafc_transfer/NSCnw;
				cf->frootc_transfer_to_MR	 = diff * cs->frootc_transfer/NSCnw;
				cf->yieldc_transfer_to_MR	 = diff * cs->yieldc_transfer/NSCnw;
				cf->softstemc_transfer_to_MR = diff * cs->softstemc_transfer/NSCnw;
			

				cf->NSCnw_to_MR  = cf->leafc_storage_to_MR  + cf->frootc_storage_to_MR  + cf->yieldc_storage_to_MR  + cf->softstemc_storage_to_MR +
								   cf->leafc_transfer_to_MR + cf->frootc_transfer_to_MR + cf->yieldc_transfer_to_MR + cf->softstemc_transfer_to_MR;
	
				if (epc->leaf_cn)     nf->leafn_storage_to_MR		  = cf->leafc_storage_to_MR / epc->leaf_cn;
				if (epc->froot_cn)    nf->frootn_storage_to_MR		  = cf->frootc_storage_to_MR / epc->froot_cn;
				if (epc->yield_cn)    nf->yieldn_storage_to_MR		  = cf->yieldc_storage_to_MR / epc->yield_cn;
				if (epc->softstem_cn) nf->softstemn_storage_to_MR      = cf->softstemc_storage_to_MR / epc->softstem_cn;

				if (epc->leaf_cn)     nf->leafn_transfer_to_MR		  = cf->leafc_transfer_to_MR / epc->leaf_cn;
				if (epc->froot_cn)    nf->frootn_transfer_to_MR		  = cf->frootc_transfer_to_MR / epc->froot_cn;
				if (epc->yield_cn)    nf->yieldn_transfer_to_MR		  = cf->yieldc_transfer_to_MR / epc->yield_cn;
				if (epc->softstem_cn) nf->softstemn_transfer_to_MR     = cf->softstemc_transfer_to_MR / epc->softstem_cn;
	

			
				nf->NSNnw_to_MR = nf->leafn_storage_to_MR  + nf->frootn_storage_to_MR  + nf->yieldn_storage_to_MR  + nf->softstemn_storage_to_MR +
								  nf->leafn_transfer_to_MR + nf->frootn_transfer_to_MR + nf->yieldn_transfer_to_MR + nf->softstemn_transfer_to_MR;
			
			
				/* 1.3. state update of storage and transfer pools */
				cs->leafc_storage					-= cf->leafc_storage_to_MR;
				cs->frootc_storage					-= cf->frootc_storage_to_MR;
				cs->yieldc_storage					-= cf->yieldc_storage_to_MR;
				cs->softstemc_storage				-= cf->softstemc_storage_to_MR;
		

				cs->leafc_transfer					-= cf->leafc_transfer_to_MR;
				cs->frootc_transfer					-= cf->frootc_transfer_to_MR;
				cs->yieldc_transfer					-= cf->yieldc_transfer_to_MR;
				cs->softstemc_transfer				-= cf->softstemc_transfer_to_MR;
		
				ns->leafn_storage					-= nf->leafn_storage_to_MR;
				ns->frootn_storage					-= nf->frootn_storage_to_MR;
				ns->yieldn_storage					-= nf->yieldn_storage_to_MR;
				ns->softstemn_storage				-= nf->softstemn_storage_to_MR;
		
				ns->leafn_transfer					-= nf->leafn_transfer_to_MR;
				ns->frootn_transfer					-= nf->frootn_transfer_to_MR;
				ns->yieldn_transfer					-= nf->yieldn_transfer_to_MR;
				ns->softstemn_transfer				-= nf->softstemn_transfer_to_MR;
	
				ns->retransn                        += nf->NSNnw_to_MR;
			
			}

			/* 1.4. if NSC is not enough -> transfer from actual pool */
			if (NSCnw == 0 || (diff_total_nw - cf->NSCnw_to_MR) > CRIT_PREC)
			{
				diff = diff_total_nw - cf->NSCnw_to_MR;
				
				if (cf->leaf_day_MR > 0)
				{
					if (cs->leafc > CRIT_PREC)
					{
						cf->leafc_to_MR = diff * (cf->leaf_day_MR / MR_nw); 
						if (cf->leafc_to_MR > cs->leafc)
						{
							cf->leafc_to_MR = cs->leafc;
							excess += diff * (cf->leaf_day_MR / MR_nw) - cf->leafc_to_MR;
						}
						nf->leafn_to_MR = cf->leafc_to_MR / epc->leaf_cn;
					}
					else
					{
						excess += diff * (cf->leaf_day_MR / MR_nw);
					}
					
				}

				if (cf->leaf_night_MR > 0 && cs->leafc > cf->leafc_to_MR)
				{
					if (cs->leafc > CRIT_PREC)
					{
						cf->leafc_to_MR += diff * (cf->leaf_night_MR / MR_nw); 
						if (cf->leafc_to_MR > cs->leafc)
						{
							cf->leafc_to_MR = cs->leafc;
							excess += diff * (cf->leaf_day_MR / MR_nw) - cf->leafc_to_MR;
						}
						nf->leafn_to_MR = cf->leafc_to_MR / epc->leaf_cn;
					}
					else
					{
						excess += diff * (cf->leaf_night_MR / MR_nw);
					}
				}

				if (cf->froot_MR > 0)
				{
					if (cs->frootc > CRIT_PREC)
					{
						cf->frootc_to_MR = diff * (cf->froot_MR / MR_nw); 
						if (cf->frootc_to_MR > cs->frootc)
						{
							cf->frootc_to_MR = cs->frootc;
							excess += diff * (cf->froot_MR / MR_nw) - cf->frootc_to_MR;
						}
						nf->frootn_to_MR = cf->frootc_to_MR / epc->froot_cn; 
					}
					else
					{
						excess += diff * (cf->froot_MR / MR_nw);
					}
				}

				if (cf->yield_MR > 0)
				{
					if (cs->yieldc > CRIT_PREC)
					{
						cf->yieldc_to_MR = diff * (cf->yield_MR / MR_nw); 
						if (cf->yieldc_to_MR > cs->yieldc)
						{
							cf->yieldc_to_MR = cs->yieldc;
							excess += diff * (cf->yield_MR / MR_nw) - cf->yieldc_to_MR;
						}
						nf->yieldn_to_MR = cf->yieldc_to_MR / epc->yield_cn; 
					}
					else
					{
						excess += diff * (cf->yield_MR / MR_nw);
					}
				}

				if (cf->softstem_MR > 0)
				{
					if (cs->softstemc > CRIT_PREC)
					{
						cf->softstemc_to_MR = diff * (cf->softstem_MR / MR_nw); 
						if (cf->softstemc_to_MR > cs->softstemc)
						{
							cf->softstemc_to_MR = cs->softstemc;
							excess += diff * (cf->softstem_MR / MR_nw) - cf->softstemc_to_MR;
						}
						nf->softstemn_to_MR = cf->softstemc_to_MR / epc->softstem_cn; 
					}
					else
					{
						excess += diff * (cf->softstem_MR / MR_nw);
					}
				}

			

				cf->SCnw_to_MR  = cf->leafc_to_MR + cf->frootc_to_MR + cf->yieldc_to_MR + cf->softstemc_to_MR;

				nf->actNnw_to_MR  = nf->leafn_to_MR + nf->frootn_to_MR + nf->yieldn_to_MR + nf->softstemn_to_MR;

				/* 1.5. state update of actual pools */
				cs->leafc					-= cf->leafc_to_MR;
				cs->frootc					-= cf->frootc_to_MR;
				cs->yieldc					-= cf->yieldc_to_MR;
				cs->softstemc				-= cf->softstemc_to_MR;

				ns->leafn					-= nf->leafn_to_MR;
				ns->frootn					-= nf->frootn_to_MR;
				ns->yieldn					-= nf->yieldn_to_MR;
				ns->softstemn				-= nf->softstemn_to_MR;
			
				ns->retransn                += nf->actNnw_to_MR;
				
				
				MR_nw = cf->leaf_day_MR + cf->leaf_night_MR + cf->froot_MR + cf->yield_MR + cf->softstem_MR;

				/* if maintresp of non-woody biomass can not be covered from non-woody biomass -> added to woody demand */
				if (excess)
				{
					diff_total_w += excess;
				}

		
			}
		}
	}

	/* 2: woody biomass */
	
	if (MR_w)
	{
		
		/* 2.1. calculation the difference between NSC and diff (based on available amount) */
		if (diff_total_w > CRIT_PREC)
		{
			/* critical NSC value: NSC pool = fixed ratio of theroretical maximum of NSC value -> but not all is available */
			nsc_crit  = epc->NSC_avail_prop * (SCw * epc->NSC_SC_prop);
			nsc_avail = NSCw - nsc_crit;
			if (nsc_avail < 0) 
			{
				nsc_avail = 0;
			}
		
			/* MRdeficit_w = 1: diff_total_w < nsc_avail
			   MRdeficit_w = 2: diff_total_w > nsc_avail */
			 
			if (diff_total_w < nsc_avail)
			{
				diff = diff_total_w;
				epv->MRdeficit_w = 1;
			}
			else
			{
				diff = nsc_avail;
				epv->MRdeficit_w = 2;
			}


			/* 2.2. calculation of flxues from nsc pools */
			if (NSCw && diff)
			{
				cf->livestemc_storage_to_MR	 = diff * cs->livestemc_storage/NSCw;
				cf->livecrootc_storage_to_MR  = diff * cs->livecrootc_storage/NSCw;
				cf->deadstemc_storage_to_MR	 = diff * cs->deadstemc_storage/NSCw;
				cf->deadcrootc_storage_to_MR  = diff * cs->deadcrootc_storage/NSCw;

				cf->livestemc_transfer_to_MR	 = diff * cs->livestemc_transfer/NSCw;
				cf->livecrootc_transfer_to_MR = diff * cs->livecrootc_transfer/NSCw;
				cf->deadstemc_transfer_to_MR	 = diff * cs->deadstemc_transfer/NSCw;
				cf->deadcrootc_transfer_to_MR = diff * cs->deadcrootc_transfer/NSCw;

				cf->NSCw_to_MR  = cf->livestemc_storage_to_MR  + cf->livecrootc_storage_to_MR  + cf->deadstemc_storage_to_MR  + cf->deadcrootc_storage_to_MR +
								   cf->livestemc_transfer_to_MR + cf->livecrootc_transfer_to_MR + cf->deadstemc_transfer_to_MR + cf->deadcrootc_transfer_to_MR;
	
				if (epc->livewood_cn) nf->livestemn_storage_to_MR  = cf->livestemc_storage_to_MR / epc->livewood_cn;
				if (epc->livewood_cn) nf->livecrootn_storage_to_MR = cf->livecrootc_storage_to_MR / epc->livewood_cn;
				if (epc->deadwood_cn) nf->deadstemn_storage_to_MR  = cf->deadstemc_storage_to_MR / epc->deadwood_cn;
				if (epc->deadwood_cn) nf->deadcrootn_storage_to_MR = cf->deadcrootc_storage_to_MR / epc->deadwood_cn;

				if (epc->livewood_cn) nf->livestemn_transfer_to_MR  = cf->livestemc_transfer_to_MR / epc->livewood_cn;
				if (epc->livewood_cn) nf->livecrootn_transfer_to_MR = cf->livecrootc_transfer_to_MR / epc->livewood_cn;
				if (epc->deadwood_cn) nf->deadstemn_transfer_to_MR  = cf->deadstemc_transfer_to_MR / epc->deadwood_cn;
				if (epc->deadwood_cn) nf->deadcrootn_transfer_to_MR = cf->deadcrootc_transfer_to_MR / epc->deadwood_cn;

			
				nf->NSNw_to_MR = nf->livestemn_storage_to_MR  + nf->livecrootn_storage_to_MR  + nf->deadstemn_storage_to_MR  + nf->deadcrootn_storage_to_MR +
								 nf->livestemn_transfer_to_MR + nf->livecrootn_transfer_to_MR + nf->deadstemn_transfer_to_MR + nf->deadcrootn_transfer_to_MR;
			
			
				/* 2.3. state update of storage and transfer pools */
				cs->livestemc_storage				-= cf->livestemc_storage_to_MR;
				cs->livecrootc_storage				-= cf->livecrootc_storage_to_MR;
				cs->deadstemc_storage				-= cf->deadstemc_storage_to_MR;
				cs->deadcrootc_storage				-= cf->deadcrootc_storage_to_MR;

				cs->livestemc_transfer				-= cf->livestemc_transfer_to_MR;
				cs->livecrootc_transfer				-= cf->livecrootc_transfer_to_MR;
				cs->deadstemc_transfer				-= cf->deadstemc_transfer_to_MR;
				cs->deadcrootc_transfer				-= cf->deadcrootc_transfer_to_MR;
			
				ns->livestemn_storage				-= nf->livestemn_storage_to_MR;
				ns->livecrootn_storage				-= nf->livecrootn_storage_to_MR;
				ns->deadstemn_storage				-= nf->deadstemn_storage_to_MR;
				ns->deadcrootn_storage				-= nf->deadcrootn_storage_to_MR;

				ns->livestemn_transfer				-= nf->livestemn_transfer_to_MR;
				ns->livecrootn_transfer				-= nf->livecrootn_transfer_to_MR;
				ns->deadstemn_transfer				-= nf->deadstemn_transfer_to_MR;
				ns->deadcrootn_transfer				-= nf->deadcrootn_transfer_to_MR;
			
				ns->retransn                        += nf->NSNw_to_MR;
			
			}

			/* 1.5. if NSC is not enough -> transfer from actual pool */
			if (NSCw == 0 || (diff_total_w - cf->NSCw_to_MR) > CRIT_PREC)
			{
				diff = diff_total_w - cf->NSCw_to_MR;

				if (cf->livestem_MR > 0)
				{
					if (cs->livestemc > CRIT_PREC)
					{
						cf->livestemc_to_MR = diff * (cf->livestem_MR / MR_w); 
						nf->livestemn_to_MR = cf->livestemc_to_MR / epc->livewood_cn; 
					}
					else
					{
						cf->livestem_MR       = 0;
					}
				}

				if (cf->livecroot_MR > 0)
				{
					if (cs->livecrootc > CRIT_PREC)
					{
						cf->livecrootc_to_MR = diff * (cf->livecroot_MR / MR_w); 
						nf->livecrootn_to_MR = cf->livecrootc_to_MR / epc->livewood_cn;
					}
					else
					{
						cf->livecroot_MR       = 0;
					}
				}

				/* noMR_flag: flag for WARNING writing in log file (only at first time) */
				if ((cf->livecroot_MR == 0 || cf->livecroot_MR == 0) && ctrl->noMR_flag == 0) ctrl->noMR_flag = 1;

				cf->SCw_to_MR  = cf->livestemc_to_MR + cf->livecrootc_to_MR;

				nf->actNw_to_MR  = nf->livestemn_to_MR + nf->livecrootn_to_MR;

				/* 1.6. state update of actual pools */
				cs->livestemc				-= cf->livestemc_to_MR;
				cs->livecrootc				-= cf->livecrootc_to_MR;

				ns->livestemn				-= nf->livestemn_to_MR;
				ns->livecrootn				-= nf->livecrootn_to_MR;
			
				ns->retransn                += nf->actNw_to_MR;
				
				
				MR_w = cf->livestem_MR + cf->livecroot_MR;

			
			}
		}

	

	}

	/* state update of cpool */
	cs->cpool			 -= (MR_nw - cf->NSCnw_to_MR - cf->SCnw_to_MR);
	cs->cpool			 -= (MR_w  - cf->NSCw_to_MR  - cf->SCw_to_MR);
	if (cs->cpool < 0 && fabs(cs->cpool) > CRIT_PREC)
	{
		cf->leaf_day_MR   += cs->cpool * cf->leaf_day_MR   / MR_nw;
 		cf->leaf_night_MR += cs->cpool * cf->leaf_night_MR / MR_nw;
		cf->froot_MR      += cs->cpool * cf->froot_MR / MR_nw;
		cf->yield_MR      += cs->cpool * cf->yield_MR / MR_nw;
		cf->softstem_MR   += cs->cpool * cf->softstem_MR / MR_nw;

		cs->cpool = 0;
		/* limitMR_flag: flag for WARNING writing in log file (only at first time) */
		if (!ctrl->limitMR_flag) ctrl->limitMR_flag = 1;
	}
		
	/* 4. state update MR sink pools */
		
	cs->MRleaf_snk			+= cf->leaf_day_MR;
	cs->MRleaf_snk			+= cf->leaf_night_MR;
	cs->MRfroot_snk			+= cf->froot_MR;
	cs->MRyield_snk			+= cf->yield_MR;
	cs->MRsoftstem_snk	    += cf->softstem_MR;
	cs->MRlivestem_snk		+= cf->livestem_MR;
	cs->MRlivecroot_snk		+= cf->livecroot_MR;
	cs->MRdeficitNSC_snk    += cf->NSCnw_to_MR + cf->NSCw_to_MR;
	cs->MRdeficitSC_snk     += cf->SCnw_to_MR + cf->SCw_to_MR;
	
		

	cs->NSCnw = cs->leafc_storage      +  cs->frootc_storage     + cs->yieldc_storage     + cs->softstemc_storage +
		         cs->leafc_transfer     +  cs->frootc_transfer    + cs->yieldc_transfer    + cs->softstemc_transfer;

	cs->NSCw = cs->livestemc_storage  + cs->livecrootc_storage  + cs->deadstemc_storage  + cs->deadcrootc_storage +
			    cs->livestemc_transfer + cs->livecrootc_transfer + cs->deadstemc_transfer + cs->deadcrootc_transfer;

	cs->SCnw = cs->leafc     +  cs->frootc    + cs->yieldc     + cs->softstemc;

	cs->SCw = cs->livestemc  + cs->livecrootc  + cs->deadstemc  + cs->deadcrootc;

	return (errorCode);
}			

