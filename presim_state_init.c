/* 
presim_state_init.c
Initialize water, carbon, and nitrogen state variables to 0.0 before
each simulation.

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Biome-BGCMuSo v7.0.
Original code: Copyright 2000, Peter E. Thornton
Numerical Terradynamic Simulation Group, The University of Montana, USA
Modified code: Copyright 2022 D. Hidy [dori.hidy@gmail.com]
Hungarian Academy of Sciences, Hungary
See the website of Biome-BGCMuSo at http://nimbus.elte.hu/bbgc/ for documentation, model executable and example input files.
*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "ini.h"
#include "bgc_struct.h"
#include "pointbgc_struct.h"
#include "pointbgc_func.h"

int presim_state_init(wstate_struct* ws, cstate_struct* cs, nstate_struct* ns, cinit_struct* cinit)
{
	int errorCode=0;
	int layer;
	

	cinit->max_leafc = 0.0;
	cinit->max_frootc = 0.0;
	cinit->max_yieldc = 0.0;
	cinit->max_softstemc = 0.0;
	cinit->max_livestemc = 0.0;
	cinit->max_livecrootc = 0.0;
	
	ws->soilw_SUM = 0;
	ws->soilw_RZ = 0;
	ws->soilw_RZ_avail=0;
	ws->soilw_2m = 0;
	ws->pondw = 0;
	ws->snoww = 0;
	ws->canopyw = 0;
	ws->prcp_src = 0;
	ws->soilEVP_snk = 0;
	ws->snowSUBL_snk = 0;
	ws->canopywEVP_snk = 0;
	ws->TRP_snk = 0;
	ws->runoff_snk = 0;
	ws->pondEVP_snk = 0;
	ws->deeppercolation_snk = 0;
	ws->groundwater_src = 0;
	ws->groundwater_snk = 0;
	ws->canopyw_THNsnk = 0;
	ws->canopyw_MOWsnk = 0;
	ws->canopyw_HRVsnk = 0;
	ws->canopyw_PLGsnk = 0;
	ws->canopyw_GRZsnk = 0;
	ws->IRGsrc_W = 0;
	ws->condIRGsrc = 0;
	ws->FRZsrc_W = 0;
	ws->GW_waterlogging = 0.0;
	ws->WbalanceERR = 0;
	ws->inW = 0;
	ws->outW = 0;
	ws->storeW = 0;
	ws->cumEVPsoil1 = 0.0;
	ws->cumEVPsoil2 = 0.0;
	ws->timestepRichards = 0;
	cs->leafc = 0;
	cs->leafc_storage = 0;
	cs->leafc_transfer = 0;
	cs->frootc = 0;
	cs->frootc_storage = 0;
	cs->frootc_transfer = 0;
	cs->yieldc = 0;
	cs->yieldc_storage = 0;
	cs->yieldc_transfer = 0;
	cs->softstemc = 0;
	cs->softstemc_storage = 0;
	cs->softstemc_transfer = 0;
	cs->livestemc = 0;
	cs->livestemc_storage = 0;
	cs->livestemc_transfer = 0;
	cs->deadstemc = 0;
	cs->deadstemc_storage = 0;
	cs->deadstemc_transfer = 0;
	cs->livecrootc = 0;
	cs->livecrootc_storage = 0;
	cs->livecrootc_transfer = 0;
	cs->deadcrootc = 0;
	cs->deadcrootc_storage = 0;
	cs->deadcrootc_transfer = 0;
	cs->gresp_storage = 0;
	cs->gresp_transfer = 0;
	cs->litr1c_total = 0;
	cs->litr2c_total = 0;
	cs->litr3c_total = 0;
	cs->litr4c_total = 0;
	cs->cwdc_total = 0;
	cs->cwdc_above = 0;
	cs->litrc_above = 0;
	cs->STDBc_leaf = 0;
	cs->STDBc_froot = 0;
	cs->STDBc_yield = 0;
	cs->STDBc_softstem = 0;
	cs->STDBc_above = 0;
	cs->STDBc_below = 0;
	cs->CTDBc_leaf = 0;
	cs->CTDBc_froot = 0;
	cs->CTDBc_yield = 0;
	cs->CTDBc_softstem = 0;
	cs->CTDBc_cstem = 0;
	cs->CTDBc_croot = 0;
	cs->CTDBc_above = 0;
	cs->CTDBc_below = 0;
	cs->soil1c_total = 0;
	cs->soil2c_total = 0;
	cs->soil3c_total = 0;
	cs->soil4c_total = 0;			
	cs->cpool = 0;
	cs->psnsun_src = 0;
	cs->psnshade_src = 0;
	cs->NSC_MR_snk = 0;
	cs->actC_MR_snk = 0;
	cs->leaf_MR_snk = 0;
	cs->leaf_GR_snk = 0;
	cs->froot_MR_snk = 0;
	cs->froot_GR_snk = 0;
	cs->yield_GR_snk = 0;
	cs->yield_MR_snk = 0;
	cs->softstem_GR_snk = 0;
	cs->softstem_MR_snk = 0;
	cs->livestem_MR_snk = 0;
	cs->livestem_GR_snk = 0;
	cs->deadstem_GR_snk = 0;
	cs->livecroot_MR_snk = 0;
	cs->livecroot_GR_snk = 0;
	cs->deadcroot_GR_snk = 0;
	cs->litr1_hr_snk = 0;
	cs->litr2_hr_snk = 0;
	cs->litr4_hr_snk = 0;
	cs->soil1_hr_snk = 0;
	cs->soil2_hr_snk = 0;
	cs->soil3_hr_snk = 0;
	cs->soil4_hr_snk = 0;
	cs->Cdeepleach_snk = 0;
	cs->flowHSsnk_C = 0;
	cs->calc_flowHS = 0;
	cs->FIREsnk_C = 0;
	cs->SNSCsnk_C = 0;
	cs->PLTsrc_C = 0;
	cs->THN_transportC = 0;
	cs->HRV_transportC = 0;
	cs->MOW_transportC = 0;
	cs->GRZsnk_C = 0;
	cs->GRZsrc_C = 0;
	cs->FRZsrc_C = 0;
	cs->yieldC_HRV = 0.0;
	cs->frootC_HRV = 0.0;
	cs->vegC_HRV = 0.0;
	cs->CbalanceERR = 0;
	cs->CNratioERR = 0.0;
	cs->inC = 0;
	cs->outC = 0;
	cs->storeC = 0;
	ns->leafn = 0;
	ns->leafn_storage = 0;
	ns->leafn_transfer = 0;
	ns->frootn = 0;
	ns->frootn_storage = 0;
	ns->frootn_transfer = 0;
	ns->yieldn = 0;
	ns->yieldn_storage = 0;
	ns->yieldn_transfer = 0;
	ns->softstemn = 0;
	ns->softstemn_storage = 0;
	ns->softstemn_transfer = 0;
	ns->livestemn = 0;
	ns->livestemn_storage = 0;
	ns->livestemn_transfer = 0;
	ns->deadstemn = 0;
	ns->deadstemn_storage = 0;
	ns->deadstemn_transfer = 0;
	ns->livecrootn = 0;
	ns->livecrootn_storage = 0;
	ns->livecrootn_transfer = 0;
	ns->deadcrootn = 0;
	ns->deadcrootn_storage = 0;
	ns->deadcrootn_transfer = 0;
	ns->npool = 0;
	ns->litr1n_total = 0;
	ns->litr2n_total = 0;
	ns->litr3n_total = 0;
	ns->litr4n_total = 0;
	ns->cwdn_total = 0;
	ns->STDBn_leaf = 0;
	ns->STDBn_froot = 0;
	ns->STDBn_yield = 0;
	ns->STDBn_softstem = 0;
	ns->STDBn_above = 0;
	ns->STDBn_below = 0;
	ns->CTDBn_leaf = 0;
	ns->CTDBn_froot = 0;
	ns->CTDBn_yield = 0;
	ns->CTDBn_softstem = 0;
	ns->CTDBn_cstem = 0;
	ns->CTDBn_croot = 0;
	ns->CTDBn_above = 0;
	ns->CTDBn_below = 0;
	ns->soil1n_total = 0;
	ns->soil2n_total = 0;
	ns->soil3n_total = 0;
	ns->soil4n_total = 0;
	ns->retransn = 0;
	ns->sminNH4_total = 0;
	ns->sminNO3_total = 0;

	ns->Nfix_src = 0;
	ns->Ndep_src = 0;
	ns->Ndeepleach_snk = 0;
	ns->Nvol_snk = 0;
	ns->Nprec_snk = 0;
	ns->FIREsnk_N = 0;
	ns->SNSCsnk_N = 0;
	ns->FRZsrc_N = 0;
	ns->PLTsrc_N = 0;
	ns->THN_transportN = 0;
	ns->MOW_transportN = 0;
	ns->HRV_transportN = 0;
	ns->GRZsnk_N = 0;
	ns->GRZsrc_N = 0;
	ns->SPINUPsrc = 0;
	ns->NbalanceERR = 0;
	ns->inN = 0;
	ns->outN = 0;
	ns->storeN = 0;


	for (layer = 0; layer < N_SOILLAYERS; layer++)
	{
		ws->soilw[layer] = 0;
		ws->soilw_avail[layer] = 0;
		cs->cwdc[layer] = 0;
		cs->litr1c[layer] = 0;
		cs->litr2c[layer] = 0;
		cs->litr3c[layer] = 0;
		cs->litr4c[layer] = 0;
		cs->litrC[layer] = 0;
		cs->soil1c[layer] = 0;
		cs->soil2c[layer] = 0;
		cs->soil3c[layer] = 0;
		cs->soil4c[layer] = 0;
		cs->soilC[layer] = 0;
		cs->soil1DOC[layer] = 0;
		cs->soil2DOC[layer] = 0;
		cs->soil3DOC[layer] = 0;
		cs->soil4DOC[layer] = 0;
		cs->soilDOC[layer] = 0;
		ns->cwdn[layer] = 0;
		ns->litr1n[layer] = 0;
		ns->litr2n[layer] = 0;
		ns->litr3n[layer] = 0;
		ns->litr4n[layer] = 0;
		ns->litrN[layer] = 0;
		ns->soil1n[layer] = 0;
		ns->soil2n[layer] = 0;
		ns->soil3n[layer] = 0;
		ns->soil4n[layer] = 0;
		ns->soilN[layer] = 0;
		ns->soil1DON[layer] = 0;
		ns->soil2DON[layer] = 0;
		ns->soil3DON[layer] = 0;
		ns->soil4DON[layer] = 0;
		ns->soilDON[layer] = 0;
		ns->sminNH4[layer] = 0;
		ns->sminNO3[layer] = 0;
	}

	return(errorCode);
}
	
