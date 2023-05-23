/* 
make_zero_flux_struct.c
create structures initialized with zero for forcing fluxes to zero
between simulation days

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Biome-BGCMuSo v7.0.
Original code: Copyright 2000, Peter E. Thornton
Numerical Terradynamic Simulation Group, The University of Montana, USA
Modified code: Copyright 2022, D. Hidy [dori.hidy@gmail.com]
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

int make_zero_flux_struct(wflux_struct* wf, cflux_struct* cf, nflux_struct* nf, GWcalc_struct* gwc)
{
	int errorCode=0;
	int layer;

	wf->prcp_to_canopyw = 0;
	wf->prcp_to_soilSurface = 0;
	wf->prcp_to_snoww = 0;
	wf->prcp_to_runoff = 0;
	wf->pondw_to_runoff = 0;
	wf->canopywEVP = 0;
	wf->canopyw_to_soilw = 0;
	wf->pondwEVP = 0;
	wf->surfaceEVP = 0;
	wf->snowwSUBL = 0;
	wf->snoww_to_soilw = 0;
	wf->soilwEVP = 0;
	wf->potEVPsurface = 0;
	wf->potETcanopy = 0;
	wf->potSUBLsnow = 0;
	wf->potEVPandSUBLsurface = 0;
	wf->ET_Elimit = 0;
	wf->soilwTRP_POT = 0;

	wf->soilwTRP_SUM = 0;
	wf->ET = 0;
	wf->PET = 0;
	wf->pondw_to_soilw = 0;
	wf->soilw_to_pondw = 0;
	wf->infilt_to_soilw = 0;
	wf->prcp_to_pondw = 0;
	wf->GW_to_pondw = 0;
	wf->FLD_to_pondw = 0;
	wf->FLD_to_soilw = 0;
	wf->soilwLeach_RZ = 0;
	wf->canopyw_to_THN = 0;
	wf->canopyw_to_MOW = 0;
	wf->canopyw_to_HRV = 0;
	wf->canopyw_to_PLG = 0;
	wf->canopyw_to_GRZ = 0;
	wf->IRG_to_prcp = 0;
	wf->IRG_to_soilw = 0;
	wf->IRG_to_soilSurface = 0;
	wf->FRZ_to_soilw = 0;
	wf->infiltPOT = 0;
	wf->waterFromAbove = 0;

	cf->m_leafc_to_litr1c = 0;
	cf->m_leafc_to_litr2c = 0;
	cf->m_leafc_to_litr3c = 0;
	cf->m_leafc_to_litr4c = 0;
	cf->m_frootc_to_litr1c = 0;
	cf->m_frootc_to_litr2c = 0;
	cf->m_frootc_to_litr3c = 0;
	cf->m_frootc_to_litr4c = 0;
	cf->m_yieldc_to_litr1c = 0;
	cf->m_yieldc_to_litr2c = 0;
	cf->m_yieldc_to_litr3c = 0;
	cf->m_yieldc_to_litr4c = 0;
	cf->m_softstemc_to_litr1c = 0;
	cf->m_softstemc_to_litr2c = 0;
	cf->m_softstemc_to_litr3c = 0;
	cf->m_softstemc_to_litr4c = 0;
	cf->m_leafc_storage_to_litr1c = 0;
	cf->m_frootc_storage_to_litr1c = 0;
	cf->m_softstemc_storage_to_litr1c = 0;
	cf->m_yieldc_storage_to_litr1c = 0;
	cf->m_livestemc_storage_to_litr1c = 0;
	cf->m_deadstemc_storage_to_litr1c = 0;
	cf->m_livecrootc_storage_to_litr1c = 0;
	cf->m_deadcrootc_storage_to_litr1c = 0;
	cf->m_leafc_transfer_to_litr1c = 0;
	cf->m_frootc_transfer_to_litr1c = 0;
	cf->m_yieldc_transfer_to_litr1c = 0;
	cf->m_softstemc_transfer_to_litr1c = 0;
	cf->m_livestemc_transfer_to_litr1c = 0;
	cf->m_deadstemc_transfer_to_litr1c = 0;
	cf->m_livecrootc_transfer_to_litr1c = 0;
	cf->m_deadcrootc_transfer_to_litr1c = 0;
	cf->m_livestemc_to_cwdc = 0;
	cf->m_deadstemc_to_cwdc = 0;
	cf->m_livecrootc_to_cwdc = 0;
	cf->m_deadcrootc_to_cwdc = 0;
	cf->m_gresp_storage_to_litr1c = 0;
	cf->m_gresp_transfer_to_litr1c = 0;
	cf->m_leafc_to_fire = 0;
	cf->m_frootc_to_fire = 0;
	cf->m_yieldc_to_fire = 0;
	cf->m_softstemc_to_fire = 0;
	cf->m_STDBc_to_fire = 0;
	cf->m_CTDBc_to_fire = 0;
	cf->m_leafc_storage_to_fire = 0;
	cf->m_frootc_storage_to_fire = 0;
	cf->m_yieldc_storage_to_fire = 0;
	cf->m_softstemc_storage_to_fire = 0;
	cf->m_livestemc_storage_to_fire = 0;
	cf->m_deadstemc_storage_to_fire = 0;
	cf->m_livecrootc_storage_to_fire = 0;
	cf->m_deadcrootc_storage_to_fire = 0;
	cf->m_leafc_transfer_to_fire = 0;
	cf->m_frootc_transfer_to_fire = 0;
	cf->m_yieldc_transfer_to_fire = 0;
	cf->m_softstemc_transfer_to_fire = 0;
	cf->m_livestemc_transfer_to_fire = 0;
	cf->m_deadstemc_transfer_to_fire = 0;
	cf->m_livecrootc_transfer_to_fire = 0;
	cf->m_deadcrootc_transfer_to_fire = 0;
	cf->m_livestemc_to_fire = 0;
	cf->m_deadstemc_to_fire = 0;
	cf->m_livecrootc_to_fire = 0;
	cf->m_deadcrootc_to_fire = 0;
	cf->m_gresp_storage_to_fire = 0;
	cf->m_gresp_transfer_to_fire = 0;
	cf->m_litr1c_to_fire_total = 0;
	cf->m_litr2c_to_fire_total = 0;
	cf->m_litr3c_to_fire_total = 0;
	cf->m_litr4c_to_fire_total = 0;
	cf->m_cwdc_to_fire_total = 0;
	cf->m_vegc_to_SNSC = 0;
	cf->m_leafc_to_SNSC = 0;
	cf->m_leafc_to_SNSCgenprog = 0;
	cf->m_frootc_to_SNSC = 0;
	cf->m_yieldc_to_SNSC = 0;
	cf->m_softstemc_to_SNSC = 0;
	cf->m_leafc_storage_to_SNSC = 0;
	cf->m_frootc_storage_to_SNSC = 0;
	cf->m_leafc_transfer_to_SNSC = 0;
	cf->m_frootc_transfer_to_SNSC = 0;
	cf->m_yieldc_storage_to_SNSC = 0;
	cf->m_yieldc_transfer_to_SNSC = 0;
	cf->m_softstemc_storage_to_SNSC = 0;
	cf->m_softstemc_transfer_to_SNSC = 0;
	cf->m_gresp_storage_to_SNSC = 0;
	cf->m_gresp_transfer_to_SNSC = 0;
	cf->HRV_leafc_storage_to_SNSC = 0;               
	cf->HRV_leafc_transfer_to_SNSC = 0;    
	cf->HRV_yieldc_storage_to_SNSC = 0;              
	cf->HRV_yieldc_transfer_to_SNSC = 0;   
	cf->HRV_frootc_to_SNSC = 0;                          
	cf->HRV_softstemc_to_SNSC = 0;                 
	cf->HRV_frootc_storage_to_SNSC = 0;               
	cf->HRV_frootc_transfer_to_SNSC = 0;               
	cf->HRV_softstemc_storage_to_SNSC = 0;        
	cf->HRV_softstemc_transfer_to_SNSC = 0;       
	cf->HRV_gresp_storage_to_SNSC = 0;
	cf->HRV_gresp_transfer_to_SNSC = 0;
	cf->yieldc_to_flowHS = 0;
	cf->STDBc_leaf_to_litr = 0;
	cf->STDBc_froot_to_litr = 0;
	cf->STDBc_yield_to_litr = 0;
	cf->STDBc_softstem_to_litr = 0;
	cf->STDBc_to_litr = 0;
	cf->CTDBc_leaf_to_litr = 0;
	cf->CTDBc_froot_to_litr = 0;
	cf->CTDBc_yield_to_litr = 0;
	cf->CTDBc_softstem_to_litr = 0;
	cf->CTDBc_cstem_to_cwd = 0;
	cf->CTDBc_croot_to_cwd = 0;
	cf->CTDBc_to_litr = 0;
	cf->leafc_transfer_to_leafc = 0;
	cf->frootc_transfer_to_frootc = 0;
	cf->yieldc_transfer_to_yield = 0;
	cf->softstemc_transfer_to_softstemc = 0;
	cf->livestemc_transfer_to_livestemc = 0;
	cf->deadstemc_transfer_to_deadstemc = 0;
	cf->livecrootc_transfer_to_livecrootc = 0;
	cf->deadcrootc_transfer_to_deadcrootc = 0;
	cf->leafc_to_litr1c = 0;
	cf->leafc_to_litr2c = 0;
	cf->leafc_to_litr3c = 0;
	cf->leafc_to_litr4c = 0;
	cf->frootc_to_litr1c = 0;
	cf->frootc_to_litr2c = 0;
	cf->frootc_to_litr3c = 0;
	cf->frootc_to_litr4c = 0;
	cf->yieldc_to_litr1c = 0;
	cf->yieldc_to_litr2c = 0;
	cf->yieldc_to_litr3c = 0;
	cf->yieldc_to_litr4c = 0;
	cf->softstemc_to_litr1c = 0;
	cf->softstemc_to_litr2c = 0;
	cf->softstemc_to_litr3c = 0;
	cf->softstemc_to_litr4c = 0;
	cf->leaf_day_MR = 0;
	cf->leaf_night_MR = 0;
	cf->froot_MR = 0;
	cf->yield_MR = 0;
	cf->softstem_MR = 0;
	cf->livestem_MR = 0;
	cf->livecroot_MR = 0;
	cf->psnsun_to_cpool = 0;
	cf->psnshade_to_cpool = 0;
	cf->DOC_leachRZ = 0;
	cf->cpool_to_leafc = 0;
	cf->cpool_to_leafc_storage = 0;
	cf->cpool_to_frootc = 0;
	cf->cpool_to_frootc_storage = 0;
	cf->cpool_to_yield = 0;
	cf->cpool_to_yieldc_storage = 0;
	cf->cpool_to_softstemc = 0;
	cf->cpool_to_softstemc_storage = 0;
	cf->cpool_to_livestemc = 0;
	cf->cpool_to_livestemc_storage = 0;
	cf->cpool_to_deadstemc = 0;
	cf->cpool_to_deadstemc_storage = 0;
	cf->cpool_to_livecrootc = 0;
	cf->cpool_to_livecrootc_storage = 0;
	cf->cpool_to_deadcrootc = 0;
	cf->cpool_to_deadcrootc_storage = 0;
	cf->cpool_to_gresp_storage = 0;
	cf->cpool_leaf_GR = 0;
	cf->cpool_leaf_storage_GR = 0;
	cf->transfer_leaf_GR = 0;
	cf->cpool_froot_GR = 0;
	cf->cpool_froot_storage_GR = 0;
	cf->transfer_froot_GR = 0;
	cf->cpool_yield_GR = 0;
	cf->cpool_yieldc_storage_GR = 0;
	cf->transfer_yield_GR = 0;
	cf->cpool_softstem_GR = 0;
	cf->cpool_softstem_storage_GR = 0;
	cf->transfer_softstem_GR = 0;
	cf->cpool_livestem_GR = 0;
	cf->cpool_livestem_storage_GR = 0;
	cf->transfer_livestem_GR = 0;
	cf->cpool_deadstem_GR = 0;
	cf->cpool_deadstem_storage_GR = 0;
	cf->transfer_deadstem_GR = 0;
	cf->cpool_livecroot_GR = 0;
	cf->cpool_livecroot_storage_GR = 0;
	cf->transfer_livecroot_GR = 0;
	cf->cpool_deadcroot_GR = 0;
	cf->cpool_deadcroot_storage_GR = 0;
	cf->transfer_deadcroot_GR = 0;
	cf->leafc_storage_to_maintresp = 0;
	cf->frootc_storage_to_maintresp = 0;
	cf->yieldc_storage_to_maintresp = 0;
	cf->softstemc_storage_to_maintresp = 0;
	cf->livestemc_storage_to_maintresp = 0;
	cf->livecrootc_storage_to_maintresp = 0;
	cf->deadstemc_storage_to_maintresp = 0;
	cf->deadcrootc_storage_to_maintresp = 0;
	cf->leafc_transfer_to_maintresp = 0;
	cf->frootc_transfer_to_maintresp = 0;
	cf->yieldc_transfer_to_maintresp = 0;
	cf->softstemc_transfer_to_maintresp = 0;
	cf->livestemc_transfer_to_maintresp = 0;
	cf->livecrootc_transfer_to_maintresp = 0;
	cf->deadstemc_transfer_to_maintresp = 0;
	cf->deadcrootc_transfer_to_maintresp = 0;
	cf->leafc_to_maintresp = 0;
	cf->frootc_to_maintresp = 0;
	cf->yieldc_to_maintresp = 0;
	cf->softstemc_to_maintresp = 0;
	cf->livestemc_to_maintresp = 0;
	cf->livecrootc_to_maintresp = 0;
	cf->NSC_nw_to_maintresp = 0;
	cf->actC_nw_to_maintresp = 0;
	cf->NSC_w_to_maintresp = 0;
	cf->actC_w_to_maintresp = 0;
	cf->leafc_storage_to_leafc_transfer = 0;
	cf->frootc_storage_to_frootc_transfer = 0;
	cf->yieldc_storage_to_yieldc_transfer = 0;
	cf->softstemc_storage_to_softstemc_transfer = 0;
	cf->livestemc_storage_to_livestemc_transfer = 0;
	cf->deadstemc_storage_to_deadstemc_transfer = 0;
	cf->livecrootc_storage_to_livecrootc_transfer = 0;
	cf->deadcrootc_storage_to_deadcrootc_transfer = 0;
	cf->gresp_storage_to_gresp_transfer = 0;
	cf->livestemc_to_deadstemc = 0;
	cf->livecrootc_to_deadcrootc = 0;
	cf->leafc_transfer_from_PLT = 0;
	cf->frootc_transfer_from_PLT = 0;
	cf->yieldc_transfer_from_PLT = 0;
	cf->softstemc_transfer_from_PLT = 0;
	cf->STDBc_leaf_to_PLT = 0;				 			 
	cf->STDBc_yield_to_PLT = 0;
	cf->STDBc_froot_to_PLT = 0;	
	cf->STDBc_softstem_to_PLT = 0;	
	cf->leafc_to_THN = 0;
	cf->leafc_storage_to_THN = 0;
	cf->leafc_transfer_to_THN = 0;
	cf->frootc_to_THN = 0;
	cf->frootc_storage_to_THN = 0;
	cf->frootc_transfer_to_THN = 0;
	cf->yieldc_to_THN = 0;
	cf->yieldc_storage_to_THN = 0;
	cf->yieldc_transfer_to_THN = 0;
	cf->livestemc_to_THN = 0;
	cf->livestemc_storage_to_THN = 0;
	cf->livestemc_transfer_to_THN = 0;
	cf->deadstemc_to_THN = 0;
	cf->deadstemc_storage_to_THN = 0;
	cf->deadstemc_transfer_to_THN = 0;
	cf->livecrootc_to_THN = 0;
	cf->livecrootc_storage_to_THN = 0;
	cf->livecrootc_transfer_to_THN = 0;
	cf->deadcrootc_to_THN = 0;
	cf->deadcrootc_storage_to_THN = 0;
	cf->deadcrootc_transfer_to_THN = 0;
	cf->gresp_storage_to_THN = 0;
	cf->gresp_transfer_to_THN = 0;
	cf->THN_to_CTDBc_leaf = 0;
	cf->THN_to_CTDBc_froot = 0;
	cf->THN_to_CTDBc_yield = 0;
	cf->THN_to_CTDBc_cstem = 0;
	cf->THN_to_CTDBc_croot = 0;
	cf->STDBc_leaf_to_THN = 0;
	cf->STDBc_froot_to_THN = 0;
	cf->STDBc_yield_to_THN = 0;
	cf->leafc_to_MOW = 0;
	cf->leafc_storage_to_MOW = 0;
	cf->leafc_transfer_to_MOW = 0;
	cf->yieldc_to_MOW = 0;
	cf->yieldc_storage_to_MOW = 0;
	cf->yieldc_transfer_to_MOW = 0;
	cf->softstemc_to_MOW = 0;
	cf->softstemc_storage_to_MOW = 0;
	cf->softstemc_transfer_to_MOW = 0;
	cf->gresp_storage_to_MOW = 0;
	cf->gresp_transfer_to_MOW = 0;
	cf->MOW_to_CTDBc_leaf = 0;
	cf->MOW_to_CTDBc_yield = 0;
	cf->MOW_to_CTDBc_softstem = 0;
	cf->STDBc_leaf_to_MOW = 0;
	cf->STDBc_yield_to_MOW = 0;
	cf->STDBc_softstem_to_MOW = 0;
	cf->leafc_to_HRV = 0;
	cf->leafc_storage_to_HRV = 0;
	cf->leafc_transfer_to_HRV = 0;
	cf->yieldc_to_HRV = 0;
	cf->yieldc_storage_to_HRV = 0;
	cf->yieldc_transfer_to_HRV = 0;
	cf->softstemc_to_HRV = 0;
	cf->softstemc_storage_to_HRV = 0;
	cf->softstemc_transfer_to_HRV = 0;
	cf->gresp_storage_to_HRV = 0;
	cf->gresp_transfer_to_HRV = 0;
	cf->HRV_to_CTDBc_leaf = 0;
	cf->HRV_to_CTDBc_yield = 0;
	cf->HRV_to_CTDBc_softstem = 0;
	cf->STDBc_leaf_to_HRV = 0;
	cf->STDBc_yield_to_HRV = 0;
	cf->STDBc_softstem_to_HRV = 0;
	cf->leafc_to_PLG = 0;
	cf->leafc_storage_to_PLG = 0;
	cf->leafc_transfer_to_PLG = 0;
	cf->frootc_to_PLG = 0;
	cf->frootc_storage_to_PLG = 0;
	cf->frootc_transfer_to_PLG = 0;
	cf->yieldc_to_PLG = 0;
	cf->yieldc_storage_to_PLG = 0;
	cf->yieldc_transfer_to_PLG = 0;
	cf->softstemc_to_PLG = 0;
	cf->softstemc_storage_to_PLG = 0;
	cf->softstemc_transfer_to_PLG = 0;
	cf->gresp_storage_to_PLG = 0;
	cf->gresp_transfer_to_PLG = 0;
	cf->STDBc_leaf_to_PLG = 0;
	cf->STDBc_froot_to_PLG = 0;
	cf->STDBc_yield_to_PLG = 0;
	cf->STDBc_softstem_to_PLG = 0;
	cf->CTDBc_leaf_to_PLG = 0;
	cf->CTDBc_yield_to_PLG = 0;
	cf->CTDBc_softstem_to_PLG = 0;
	cf->leafc_to_GRZ = 0;
	cf->leafc_storage_to_GRZ = 0;
	cf->leafc_transfer_to_GRZ = 0;
	cf->yieldc_to_GRZ = 0;
	cf->yieldc_storage_to_GRZ = 0;
	cf->yieldc_transfer_to_GRZ = 0;
	cf->softstemc_to_GRZ = 0;
	cf->softstemc_storage_to_GRZ = 0;
	cf->softstemc_transfer_to_GRZ = 0;
	cf->gresp_storage_to_GRZ = 0;
	cf->gresp_transfer_to_GRZ = 0;
	cf->STDBc_leaf_to_GRZ = 0;
	cf->STDBc_yield_to_GRZ = 0;
	cf->STDBc_softstem_to_GRZ = 0;
	cf->GRZ_to_litr1c = 0;
	cf->GRZ_to_litr2c = 0;
	cf->GRZ_to_litr3c = 0;
	cf->GRZ_to_litr4c = 0;
	cf->FRZ_to_litr1c = 0;
	cf->FRZ_to_litr2c = 0;
	cf->FRZ_to_litr3c = 0;
	cf->FRZ_to_litr4c = 0;
	cf->litrc_from_MUL = 0;
	cf->cwdc_from_MUL = 0;
	cf->cwdc0_to_CWE = 0;
	cf->cwdc1_to_CWE = 0;
	cf->cwdc2_to_CWE = 0;
	cf->CH4flux_soil = 0;
	cf->CH4flux_manure = 0;
	cf->CH4flux_animal = 0;
	cf->litr1_hr_total = 0;		
	cf->litr2_hr_total = 0;		
	cf->litr4_hr_total = 0;		
	cf->soil1_hr_total = 0;		
	cf->soil2_hr_total = 0;		
	cf->soil3_hr_total = 0;		
	cf->soil4_hr_total = 0;	
				
	cf->litrc_to_soilc_total = 0;						
	cf->cwdc_to_litrc_total = 0;						
	cf->litrc_to_release_total = 0;  
	

	nf->m_leafn_to_litr1n = 0;
	nf->m_leafn_to_litr2n = 0;
	nf->m_leafn_to_litr3n = 0;
	nf->m_leafn_to_litr4n = 0;
	nf->m_frootn_to_litr1n = 0;
	nf->m_frootn_to_litr2n = 0;
	nf->m_frootn_to_litr3n = 0;
	nf->m_frootn_to_litr4n = 0;
	nf->m_yieldn_to_litr1n = 0;
	nf->m_yieldn_to_litr2n = 0;
	nf->m_yieldn_to_litr3n = 0;
	nf->m_yieldn_to_litr4n = 0;
	nf->m_softstemn_to_litr1n = 0;
	nf->m_softstemn_to_litr2n = 0;
	nf->m_softstemn_to_litr3n = 0;
	nf->m_softstemn_to_litr4n = 0;
	nf->m_leafn_storage_to_litr1n = 0;
	nf->m_frootn_storage_to_litr1n = 0;
	nf->m_yieldn_storage_to_litr1n = 0;
	nf->m_yieldn_transfer_to_litr1n = 0;
	nf->m_softstemn_storage_to_litr1n = 0;
	nf->m_softstemn_transfer_to_litr1n = 0;
	nf->m_livestemn_storage_to_litr1n = 0;
	nf->m_deadstemn_storage_to_litr1n = 0;
	nf->m_livecrootn_storage_to_litr1n = 0;
	nf->m_deadcrootn_storage_to_litr1n = 0;
	nf->m_leafn_transfer_to_litr1n = 0;
	nf->m_frootn_transfer_to_litr1n = 0;
	nf->m_livestemn_transfer_to_litr1n = 0;
	nf->m_deadstemn_transfer_to_litr1n = 0;
	nf->m_livecrootn_transfer_to_litr1n = 0;
	nf->m_deadcrootn_transfer_to_litr1n = 0;
	nf->m_livestemn_to_litr1n = 0;
	nf->m_livestemn_to_cwdn = 0;
	nf->m_deadstemn_to_cwdn = 0;
	nf->m_livecrootn_to_litr1n = 0;
	nf->m_livecrootn_to_cwdn = 0;
	nf->m_deadcrootn_to_cwdn = 0;
	nf->m_retransn_to_litr1n = 0;
	nf->m_vegn_to_SNSC = 0;
	nf->m_leafn_to_SNSC = 0;
	nf->m_leafn_to_SNSCgenprog = 0;
	nf->m_frootn_to_SNSC = 0;
	nf->m_leafn_storage_to_SNSC = 0;
	nf->m_frootn_storage_to_SNSC = 0;
	nf->m_leafn_transfer_to_SNSC = 0;
	nf->m_frootn_transfer_to_SNSC = 0;
	nf->m_yieldn_to_SNSC = 0;
	nf->m_yieldn_storage_to_SNSC = 0;
	nf->m_yieldn_transfer_to_SNSC = 0;
	nf->m_softstemn_to_SNSC = 0;
	nf->m_softstemn_storage_to_SNSC = 0;
	nf->m_softstemn_transfer_to_SNSC = 0;
	nf->m_retransn_to_SNSC = 0;
	nf->SNSC_to_retrans = 0;
	nf->leafSNSCgenprog_to_retrans = 0;
	nf->leafSNSC_to_retrans = 0;
	nf->frootSNSC_to_retrans = 0;
	nf->yieldSNSC_to_retrans = 0;
	nf->softstemSNSC_to_retrans = 0;
	nf->leaf_transferSNSC_to_retrans = 0;
	nf->froot_transferSNSC_to_retrans = 0;
	nf->yieldc_transferSNSC_to_retrans = 0;
	nf->softstem_transferSNSC_to_retrans = 0;
	nf->leaf_storageSNSC_to_retrans = 0;
	nf->froot_storageSNSC_to_retrans = 0;
	nf->yieldc_storageSNSC_to_retrans = 0;
	nf->softstem_storageSNSC_to_retrans = 0;
	nf->HRV_leafn_storage_to_SNSC = 0;               
	nf->HRV_leafn_transfer_to_SNSC = 0;    
	nf->HRV_yieldn_storage_to_SNSC = 0;              
	nf->HRV_yieldn_transfer_to_SNSC = 0;  
	nf->HRV_frootn_to_SNSC = 0;                           
	nf->HRV_softstemn_to_SNSC = 0;                 
	nf->HRV_frootn_storage_to_SNSC = 0;               
	nf->HRV_frootn_transfer_to_SNSC = 0;               
	nf->HRV_softstemn_storage_to_SNSC = 0;        
	nf->HRV_softstemn_transfer_to_SNSC = 0;       
	nf->HRV_retransn_to_SNSC = 0;
	nf->yieldn_to_flowHS = 0;
	nf->STDBn_leaf_to_litr = 0;
	nf->STDBn_froot_to_litr = 0;
	nf->STDBn_yield_to_litr = 0;
	nf->STDBn_softstem_to_litr = 0;
	nf->STDBn_to_litr = 0;
	nf->CTDBn_leaf_to_litr = 0;
	nf->CTDBn_froot_to_litr = 0;
	nf->CTDBn_yield_to_litr = 0;
	nf->CTDBn_softstem_to_litr = 0;

	nf->CTDBn_cstem_to_cwd = 0;
	nf->CTDBn_croot_to_cwd = 0;
	nf->CTDBn_to_litr = 0;
	nf->m_leafn_to_fire = 0;
	nf->m_frootn_to_fire = 0;
	nf->m_yieldn_to_fire = 0;
	nf->m_softstemn_to_fire = 0;
	nf->m_STDBn_to_fire = 0;
	nf->m_CTDBn_to_fire = 0;
	nf->m_leafn_storage_to_fire = 0;
	nf->m_frootn_storage_to_fire = 0;
	nf->m_yieldn_storage_to_fire = 0;
	nf->m_yieldn_transfer_to_fire = 0;
	nf->m_softstemn_storage_to_fire = 0;
	nf->m_softstemn_transfer_to_fire = 0;
	nf->m_livestemn_storage_to_fire = 0;
	nf->m_deadstemn_storage_to_fire = 0;
	nf->m_livecrootn_storage_to_fire = 0;
	nf->m_deadcrootn_storage_to_fire = 0;
	nf->m_leafn_transfer_to_fire = 0;
	nf->m_frootn_transfer_to_fire = 0;
	nf->m_livestemn_transfer_to_fire = 0;
	nf->m_deadstemn_transfer_to_fire = 0;
	nf->m_livecrootn_transfer_to_fire = 0;
	nf->m_deadcrootn_transfer_to_fire = 0;
	nf->m_livestemn_to_fire = 0;
	nf->m_deadstemn_to_fire = 0;
	nf->m_livecrootn_to_fire = 0;
	nf->m_deadcrootn_to_fire = 0;
	nf->m_retransn_to_fire = 0;
	nf->m_litr1n_to_fire_total = 0;
	nf->m_litr2n_to_fire_total = 0;
	nf->m_litr3n_to_fire_total = 0;
	nf->m_litr4n_to_fire_total = 0;
	nf->m_cwdn_to_fire_total = 0;
	nf->leafn_transfer_to_leafn = 0;
	nf->frootn_transfer_to_frootn = 0;
	nf->yieldn_transfer_to_yieldn = 0;
	nf->softstemn_transfer_to_softstemn = 0;
	nf->livestemn_transfer_to_livestemn = 0;
	nf->deadstemn_transfer_to_deadstemn = 0;
	nf->livecrootn_transfer_to_livecrootn = 0;
	nf->deadcrootn_transfer_to_deadcrootn = 0;
	nf->leafn_to_litr1n = 0;
	nf->leafn_to_litr2n = 0;
	nf->leafn_to_litr3n = 0;
	nf->leafn_to_litr4n = 0;
	nf->leafn_to_retransn = 0;
	nf->frootn_to_litr1n = 0;
	nf->frootn_to_litr2n = 0;
	nf->frootn_to_litr3n = 0;
	nf->frootn_to_litr4n = 0;
	nf->yieldn_to_litr1n = 0;
	nf->yieldn_to_litr2n = 0;
	nf->yieldn_to_litr3n = 0;
	nf->yieldn_to_litr4n = 0;
	nf->softstemn_to_litr1n = 0;
	nf->softstemn_to_litr2n = 0;
	nf->softstemn_to_litr3n = 0;
	nf->softstemn_to_litr4n = 0;
	nf->ndep_to_sminn_total = 0;
	nf->nfix_to_sminn_total = 0;
	
	nf->cwdn_to_litrn_total = 0;                               
	nf->litrn_to_release_total = 0;  

	nf->sminNO3_to_denitr_total = 0;
	nf->sminNH4_to_nitrif_total = 0;
	nf->N2_flux_DENITR_total = 0;
	nf->N2O_flux_NITRIF_total = 0;
	nf->N2O_flux_DENITR_total = 0;
	nf->sminNH4_to_npool_total = 0;
	nf->sminNO3_to_npool_total = 0;
	nf->sminn_to_npool_total = 0;
	nf->sminN_leachRZ = 0;
	nf->DON_leachRZ = 0;
	nf->retransn_to_npool_total = 0;
	nf->npool_to_leafn = 0;
	nf->npool_to_leafn_storage = 0;
	nf->npool_to_frootn = 0;
	nf->npool_to_frootn_storage = 0;
	nf->npool_to_yieldn = 0;
	nf->npool_to_yieldn_storage = 0;
	nf->npool_to_softstemn = 0;
	nf->npool_to_softstemn_storage = 0;
	nf->npool_to_livestemn = 0;
	nf->npool_to_livestemn_storage = 0;
	nf->npool_to_deadstemn = 0;
	nf->npool_to_deadstemn_storage = 0;
	nf->npool_to_livecrootn = 0;
	nf->npool_to_livecrootn_storage = 0;
	nf->npool_to_deadcrootn = 0;
	nf->npool_to_deadcrootn_storage = 0;
	nf->leafn_storage_to_maintresp = 0;
	nf->frootn_storage_to_maintresp = 0;
	nf->yieldn_storage_to_maintresp = 0;
	nf->softstemn_storage_to_maintresp = 0;
	nf->livestemn_storage_to_maintresp = 0;
	nf->livecrootn_storage_to_maintresp = 0;
	nf->deadstemn_storage_to_maintresp = 0;
	nf->deadcrootn_storage_to_maintresp = 0;
	nf->leafn_transfer_to_maintresp = 0;
	nf->frootn_transfer_to_maintresp = 0;
	nf->yieldn_transfer_to_maintresp = 0;
	nf->softstemn_transfer_to_maintresp = 0;
	nf->livestemn_transfer_to_maintresp = 0;
	nf->livecrootn_transfer_to_maintresp = 0;
	nf->deadstemn_transfer_to_maintresp = 0;
	nf->deadcrootn_transfer_to_maintresp = 0;
	nf->leafn_to_maintresp = 0;
	nf->frootn_to_maintresp = 0;
	nf->yieldn_to_maintresp = 0;
	nf->softstemn_to_maintresp = 0;
	nf->livestemn_to_maintresp = 0;
	nf->livecrootn_to_maintresp = 0;
	nf->NSN_nw_to_maintresp = 0;
	nf->actN_nw_to_maintresp = 0;
	nf->NSN_w_to_maintresp = 0;
	nf->actN_w_to_maintresp = 0;
	nf->leafn_storage_to_leafn_transfer = 0;
	nf->frootn_storage_to_frootn_transfer = 0;
	nf->livestemn_storage_to_livestemn_transfer = 0;
	nf->deadstemn_storage_to_deadstemn_transfer = 0;
	nf->livecrootn_storage_to_livecrootn_transfer = 0;
	nf->deadcrootn_storage_to_deadcrootn_transfer = 0;
	nf->yieldn_storage_to_yieldn_transfer = 0;
	nf->softstemn_storage_to_softstemn_transfer = 0;
	nf->livestemn_to_deadstemn = 0;
	nf->livestemn_to_retransn = 0;
	nf->livecrootn_to_deadcrootn = 0;
	nf->livecrootn_to_retransn = 0;
	nf->leafn_transfer_from_PLT = 0;
	nf->frootn_transfer_from_PLT = 0;
	nf->yieldn_transfer_from_PLT = 0;
	nf->softstemn_transfer_from_PLT = 0;
	nf->STDBn_leaf_to_PLT = 0;	
	nf->STDBn_froot_to_PLT = 0;	
	nf->STDBn_yield_to_PLT = 0;	
	nf->STDBn_softstem_to_PLT = 0;
	nf->leafn_to_THN = 0;
	nf->leafn_storage_to_THN = 0;
	nf->leafn_transfer_to_THN = 0;
	nf->frootn_to_THN = 0;
	nf->frootn_storage_to_THN = 0;
	nf->frootn_transfer_to_THN = 0;
	nf->yieldn_to_THN = 0;
	nf->yieldn_storage_to_THN = 0;
	nf->yieldn_transfer_to_THN = 0;
	nf->livestemn_to_THN = 0;
	nf->livestemn_storage_to_THN = 0;
	nf->livestemn_transfer_to_THN = 0;
	nf->deadstemn_to_THN = 0;
	nf->deadstemn_storage_to_THN = 0;
	nf->deadstemn_transfer_to_THN = 0;
	nf->livecrootn_to_THN = 0;
	nf->livecrootn_storage_to_THN = 0;
	nf->livecrootn_transfer_to_THN = 0;
	nf->deadcrootn_to_THN = 0;
	nf->deadcrootn_storage_to_THN = 0;
	nf->deadcrootn_transfer_to_THN = 0;
	nf->retransn_to_THN = 0;
	nf->THN_to_CTDBn_leaf = 0;
	nf->THN_to_CTDBn_froot = 0;
	nf->THN_to_CTDBn_yield = 0;
	nf->THN_to_CTDBn_cstem = 0;
	nf->THN_to_CTDBn_croot = 0;
	nf->STDBn_leaf_to_THN = 0;
	nf->STDBn_froot_to_THN = 0;
	nf->STDBn_yield_to_THN = 0;
	nf->leafn_to_MOW = 0;
	nf->leafn_storage_to_MOW = 0;
	nf->leafn_transfer_to_MOW = 0;
	nf->yieldn_to_MOW = 0;
	nf->yieldn_storage_to_MOW = 0;
	nf->yieldn_transfer_to_MOW = 0;
	nf->softstemn_to_MOW = 0;
	nf->softstemn_storage_to_MOW = 0;
	nf->softstemn_transfer_to_MOW = 0;
	nf->MOW_to_CTDBn_leaf = 0;
	nf->MOW_to_CTDBn_yield = 0;
	nf->MOW_to_CTDBn_softstem = 0;
	nf->STDBn_leaf_to_MOW = 0;
	nf->STDBn_yield_to_MOW = 0;
	nf->STDBn_softstem_to_MOW = 0;
	nf->leafn_to_HRV = 0;
	nf->leafn_storage_to_HRV = 0;
	nf->leafn_transfer_to_HRV = 0;
	nf->yieldn_to_HRV = 0;
	nf->yieldn_storage_to_HRV = 0;
	nf->yieldn_transfer_to_HRV = 0;
	nf->softstemn_to_HRV = 0;
	nf->softstemn_storage_to_HRV = 0;
	nf->softstemn_transfer_to_HRV = 0;
	nf->retransn_to_HRV = 0;
	nf->HRV_to_CTDBn_leaf = 0;
	nf->HRV_to_CTDBn_yield = 0;
	nf->HRV_to_CTDBn_softstem = 0;
	nf->STDBn_leaf_to_HRV = 0;
	nf->STDBn_yield_to_HRV = 0;
	nf->STDBn_softstem_to_HRV = 0;
	nf->leafn_to_PLG = 0;
	nf->leafn_storage_to_PLG = 0;
	nf->leafn_transfer_to_PLG = 0;
	nf->frootn_to_PLG = 0;
	nf->frootn_storage_to_PLG = 0;
	nf->frootn_transfer_to_PLG = 0;
	nf->yieldn_to_PLG = 0;
	nf->yieldn_storage_to_PLG = 0;
	nf->yieldn_transfer_to_PLG = 0;
	nf->softstemn_to_PLG = 0;
	nf->softstemn_storage_to_PLG = 0;
	nf->softstemn_transfer_to_PLG = 0;
	nf->retransn_to_PLG = 0;
	nf->STDBn_leaf_to_PLG = 0;
	nf->STDBn_froot_to_PLG = 0;
	nf->STDBn_yield_to_PLG = 0;
	nf->STDBn_softstem_to_PLG = 0;
	nf->CTDBn_leaf_to_PLG = 0;
	nf->CTDBn_yield_to_PLG = 0;
	nf->CTDBn_softstem_to_PLG = 0;
	nf->leafn_to_GRZ = 0;
	nf->leafn_storage_to_GRZ = 0;
	nf->leafn_transfer_to_GRZ = 0;
	nf->yieldn_to_GRZ = 0;
	nf->yieldn_storage_to_GRZ = 0;
	nf->yieldn_transfer_to_GRZ = 0;
	nf->softstemn_to_GRZ = 0;
	nf->softstemn_storage_to_GRZ = 0;
	nf->softstemn_transfer_to_GRZ = 0;
	nf->STDBn_leaf_to_GRZ = 0;
	nf->STDBn_yield_to_GRZ = 0;
	nf->STDBn_softstem_to_GRZ = 0;

	nf->retransn_to_GRZ = 0;
	nf->GRZ_to_litr1n = 0;
	nf->GRZ_to_litr2n = 0;
	nf->GRZ_to_litr3n = 0;
	nf->GRZ_to_litr4n = 0;
	nf->FRZ_to_sminNH4 = 0;
	nf->FRZ_to_sminNO3 = 0;
	nf->FRZ_to_litr1n = 0;
	nf->FRZ_to_litr2n = 0;
	nf->FRZ_to_litr3n = 0;
	nf->FRZ_to_litr4n = 0;
	nf->N2O_flux_GRZ = 0;
	nf->N2O_flux_FRZ = 0;

	nf->litrn_from_MUL = 0;
	nf->cwdn_from_MUL = 0;
	nf->cwdn0_to_CWE = 0;
	nf->cwdn1_to_CWE = 0;
	nf->cwdn2_to_CWE = 0;

	nf->soil4n_to_sminn_total = 0;
    nf->sminn_to_soil_SUM_total = 0;

	nf->litrn_to_sminn_total = 0;
	nf->soiln_to_sminn_total = 0;
	nf->sminn_to_soil_LtoS_total = 0; 
	nf->sminn_to_soil_StoS_total = 0; 
	nf->environment_to_sminn_total = 0;


	for (layer = 0; layer < N_SOILLAYERS; layer++)
	{
		wf->soilwTRP[layer] = 0;
		wf->soilwTRPdemand[layer] = 0;
		wf->soilwFlux[layer] = 0;
		wf->GWdischarge[layer] = 0;
		wf->GWrecharge[layer] = 0;
		wf->GWmovchange[layer] = 0;
		cf->cwdc_to_litr2c[layer] = 0;
		cf->cwdc_to_litr3c[layer] = 0;
		cf->cwdc_to_litr4c[layer] = 0;
		cf->litr1_hr[layer] = 0;
		cf->litr1c_to_soil1c[layer] = 0;
		cf->litr2_hr[layer] = 0;
		cf->litr2c_to_soil2c[layer] = 0;
		cf->litr3c_to_litr2c[layer] = 0;
		cf->litr4_hr[layer] = 0;
		cf->litr4c_to_soil3c[layer] = 0;
		cf->soil1_hr[layer] = 0;
		cf->soil1c_to_soil2c[layer] = 0;
		cf->soil2_hr[layer] = 0;
		cf->soil2c_to_soil3c[layer] = 0;
		cf->soil3_hr[layer] = 0;
		cf->soil3c_to_soil4c[layer] = 0;
		cf->soil4_hr[layer] = 0;
		cf->soil1DOC_leach[layer] = 0;
		cf->soil2DOC_leach[layer] = 0;
		cf->soil3DOC_leach[layer] = 0;
		cf->soil4DOC_leach[layer] = 0;

		cf->m_litr1c_to_fire[layer] = 0;  
		cf->m_litr2c_to_fire[layer] = 0;              
		cf->m_litr3c_to_fire[layer] = 0;             
		cf->m_litr4c_to_fire[layer] = 0;          
		cf->m_cwdc_to_fire[layer] = 0;  
		
		cf->litr1c_to_release[layer] = 0;  
		cf->litr2c_to_release[layer] = 0;  
		cf->litr4c_to_release[layer] = 0;  

	
		nf->m_litr1n_to_fire[layer] = 0;  
		nf->m_litr2n_to_fire[layer] = 0;              
		nf->m_litr3n_to_fire[layer] = 0;             
		nf->m_litr4n_to_fire[layer] = 0;          
		nf->m_cwdn_to_fire[layer] = 0; 
		nf->cwdn_to_litr2n[layer] = 0;
		nf->cwdn_to_litr3n[layer] = 0;
		nf->cwdn_to_litr4n[layer] = 0;
		nf->litr1n_to_soil1n[layer] = 0;
		nf->litr2n_to_soil2n[layer] = 0;
		nf->litr3n_to_litr2n[layer] = 0;
		nf->litr4n_to_soil3n[layer] = 0;
		nf->soil1n_to_soil2n[layer] = 0;
		nf->soil2n_to_soil3n[layer] = 0;
		nf->soil3n_to_soil4n[layer] = 0;
		nf->soil4n_to_sminn[layer] = 0;
		nf->sminn_to_soil_SUM[layer] = 0;
		nf->sminNH4_to_soil_SUM[layer] = 0;
		nf->sminNO3_to_soil_SUM[layer] = 0;
		nf->sminn_to_soil1n_l1[layer] = 0;
		nf->sminn_to_soil2n_l2[layer] = 0;
		nf->sminn_to_soil3n_l4[layer] = 0;
		nf->sminn_to_soil2n_s1[layer] = 0;
		nf->sminn_to_soil3n_s2[layer] = 0;
		nf->sminn_to_soil4n_s3[layer] = 0;
		nf->sminNO3_to_denitr[layer] = 0;
		nf->sminNH4_to_nitrif[layer] = 0;
		nf->N2_flux_DENITR[layer] = 0;
		nf->N2O_flux_NITRIF[layer] = 0;
		nf->N2O_flux_DENITR[layer] = 0;
		nf->sminNH4_to_npool[layer] = 0;
		nf->sminNO3_to_npool[layer] = 0;
		nf->sminn_to_npool[layer] = 0;
		nf->sminNH4_leach[layer] = 0;
		nf->sminNO3_leach[layer] = 0;
		nf->soil1DON_leach[layer] = 0;
		nf->soil2DON_leach[layer] = 0;
		nf->soil3DON_leach[layer] = 0;
		nf->soil4DON_leach[layer] = 0;
		nf->retransn_to_npool[layer] = 0;
		nf->ndep_to_sminNH4[layer] = 0;
		nf->ndep_to_sminNO3[layer] = 0;
		nf->nfix_to_sminNH4[layer] = 0;
		nf->litr1n_to_release[layer] = 0;  
		nf->litr2n_to_release[layer] = 0;  
		nf->litr4n_to_release[layer] = 0; 
		nf->soiln_to_sminn[layer] = 0;
		nf->litrn_to_sminn[layer] = 0;
		nf->environment_to_sminn[layer] = 0;    
		nf->sminn_to_soil_LtoS[layer] = 0; 
		nf->sminn_to_soil_StoS[layer] = 0; 
	}

	for (layer = 0; layer < N_SOILLAYERS; layer++)
	{
		gwc->soilwFlux_GWC[layer] = 0;
		gwc->soilwTRP_GWC[layer] = 0;
		gwc->soilwTRPdemand_GWC[layer] = 0;
		gwc->GWrecharge_GWC[layer] = 0;
	}


	return (errorCode);
}
