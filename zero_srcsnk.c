/* 
zero_srcsnk.c
fill the source and sink variables with 0.0 at the start of the simulation

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Biome-BGCMuSo v7.0.
Original code: Copyright 2000, Peter E. Thornton
Numerical Terradynamic Simulation Group, The University of Montana, USA
Modified code: Copyright 2022, D. Hidy [dori.hidy@gmail.com]
Hungarian Academy of Sciences, Hungary
See the website of Biome-BGCMuSo at http://nimbus.elte.hu/bbgc/ for documentation, model executable and example input files.
*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <malloc.h>
#include "ini.h"
#include "bgc_struct.h"     /* structure definitions */
#include "bgc_func.h"       /* function prototypes */
#include "bgc_constants.h"

/* zero the source and sink state variables */
int zero_srcsnk(cstate_struct* cs, nstate_struct* ns, wstate_struct* ws, summary_struct* summary)
{
	int errorCode=0;
	
	/* zero the water sources and sinks  */
	ws->prcp_src = 0.0;
	ws->soilEVP_snk = 0.0;
	ws->snowSUBL_snk = 0.0;
	ws->canopywEVP_snk = 0.0;
	ws->TRP_snk = 0.0;
	ws->prcp_src = 0.0;
	ws->soilEVP_snk = 0.0;
	ws->canopyw_THNsnk = 0.0;
	ws->canopyw_MOWsnk = 0.0;
	ws->canopyw_HRVsnk = 0.0;	
	ws->canopyw_PLGsnk = 0.0;
	ws->canopyw_GRZsnk = 0.0;	
	ws->IRGsrc_W = 0.0;
	ws->condIRGsrc = 0;
	ws->FRZsrc_W = 0.0;
	ws->runoff_snk = 0.0;
	ws->pondEVP_snk = 0;
	ws->deeppercolation_snk = 0.0;  
	ws->groundwater_src = 0.0;
	ws->groundwater_snk = 0.0;
	ws->FLDsrc = 0;
	ws->WbalanceERR = 0.0;	
	ws->inW = 0.0;
	ws->outW = 0.0;
	ws->storeW = 0.0;
	ws->cumEVPsoil1 = 0.0;
	ws->cumEVPsoil2 = 0.0;
	ws->GW_waterlogging = 0.0;

	/* zero the carbon sources and sinks */
	cs->psnsun_src = 0.0;
	cs->psnshade_src = 0.0;
	cs->MRdeficitNSC_snk = 0;
	cs->MRdeficitSC_snk = 0;
	cs->MRleaf_snk = 0.0;
	cs->GRleaf_snk = 0.0;
	cs->MRfroot_snk = 0.0;
	cs->GRfroot_snk = 0.0;
	cs->MRlivestem_snk = 0.0;
	cs->GRlivestem_snk = 0.0;
	cs->GRdeadstem_snk = 0.0;
	cs->MRlivecroot_snk = 0.0;
	cs->GRlivecroot_snk = 0.0;
	cs->GRdeadcroot_snk = 0.0;
	cs->HRlitr1_snk = 0.0;
	cs->HRlitr2_snk = 0.0;
	cs->HRlitr4_snk = 0.0;
	cs->HRsoil1_snk = 0.0;
	cs->HRsoil2_snk = 0.0;
	cs->HRsoil3_snk = 0.0;
	cs->HRsoil4_snk = 0.0;
	cs->Cdeepleach_snk = 0;
	cs->Cdeepleach_snk = 0;
	cs->flowHSsnk_C = 0;
	cs->FIREsnk_C = 0.0;

	cs->SNSCsnk_C = 0.0;
	cs->PLTsrc_C = 0.0; 
	cs->MULsrc_C = 0.0;
	cs->CWEsnk_C = 0.0;
	cs->THN_transportC = 0.0; 

	cs->MOW_transportC = 0;
	cs->GRZsnk_C = 0.0;  
	cs->GRZsrc_C = 0.0;
	cs->HRV_transportC = 0.0;
	cs->FRZsrc_C = 0.0;
	cs->yieldC_HRV = 0.0;
	cs->frootC_HRV = 0.0;
	cs->vegC_HRV = 0.0;
	cs->MRyield_snk = 0.0;
	cs->GRyield_snk = 0.0;
	cs->MRsoftstem_snk = 0.0;
	cs->GRsoftstem_snk = 0.0;
	cs->CbalanceERR = 0.0;
	cs->CNratioERR = 0.0;
	cs->inC = 0.0;
	cs->outC = 0.0;
	cs->storeC = 0.0;
	cs->NSCnw = 0.0;
	cs->NSCw = 0.0;
	cs->SCnw = 0.0;
	cs->SCw = 0.0;

	ns->Nfix_src = 0.0;
	ns->Ndep_src = 0.0;
	ns->Ndeepleach_snk = 0.0;
	ns->Nvol_snk = 0.0;
	ns->Nprec_snk = 0;
	ns->FIREsnk_N = 0.0;
	ns->SPINUPsrc = 0.0;
	ns->SNSCsnk_N = 0.0;
	ns->PLTsrc_N = 0.0; 
	ns->MULsrc_N = 0.0;
	ns->CWEsnk_N = 0.0;
	ns->THN_transportN = 0.0; 
	ns->MOW_transportN = 0;
	ns->GRZsnk_N = 0.0;  
	ns->GRZsrc_N = 0.0;
	ns->HRV_transportN = 0.0; 
	ns->FRZsrc_N = 0.0; 
	ns->NbalanceERR = 0.0;
	ns->inN = 0.0;
	ns->outN = 0.0;
	ns->storeN = 0.0;
	
	/* zero the summary variables */
	summary->annprcp = 0.0;
	summary->anntavg = 0.0;
	summary->cumRunoff = 0.0;
	summary->cumWleachRZ = 0.0;
	summary->cumNleachRZ = 0.0;
	summary->cumNPP = 0.0;
	summary->cumNEP = 0.0;
	summary->cumNEE = 0.0;
	summary->cumGPP = 0.0;
	summary->cumNGB = 0.0;
	summary->cumNBP = 0.0;
	summary->cumMR = 0.0;
	summary->cumGR = 0.0;
	summary->cumHR = 0.0;
	summary->cumAR = 0.0;
	summary->cumTRP  = 0;
	summary->cumET  = 0;	
	summary->cumET	= 0.0;
	summary->cumTR = 0.0;
	summary->cumSR = 0.0;
	summary->cumN2Oflux = 0.0;
	summary->cumCloss_MGM  = 0;
	summary->cumCplus_MGM  = 0;
	summary->cumCloss_MOW = 0.0;
	summary->cumCloss_THN_w = 0.0;
	summary->cumCloss_THN_nw = 0.0;
	summary->cumCloss_PLG = 0.0;
	summary->cumCloss_HRV = 0.0;
	summary->cumYieldC_HRV = 0.0;
	summary->cumCloss_GRZ = 0.0;
	summary->cumCplus_GRZ = 0.0;
	summary->cumCplus_FRZ = 0.0;
	summary->cumCplus_PLT = 0.0;
	summary->cumCloss_PLT = 0.0;
	summary->cumNplus_FRZ = 0.0;
	summary->cumNplus_GRZ = 0.0;
	summary->cumCloss_SNSC = 0.0;
	summary->cumCplus_CTDB = 0.0;
	summary->cumCplus_STDB = 0.0;
	summary->litdecomp = 0.0;
	summary->litfallc = 0.0;
	summary->litfallc_above = 0.0;
	summary->litfallc_below = 0.0;
	summary->litfire = 0.0;
	summary->mortc = 0.0;
	summary->mortc_above = 0.0;
	summary->mortc_below = 0.0;
	summary->NBP = 0.0;
	summary->soilC_total = 0.0;
	summary->litrC_total = 0.0;
	summary->litrCwdC_total = 0.0;
	summary->soilN_total = 0.0;
	summary->litrN_total = 0.0;
	summary->litrCwdN_total = 0.0;
	summary->sminN_total = 0.0;
	summary->sminNavail_total = 0.0;
	summary->sminNavail_maxRZ = 0.0;
	summary->sminN_maxRZ = 0.0;
	summary->soilC_maxRZ = 0.0;
	summary->soilN_maxRZ = 0.0;
	summary->litrC_maxRZ = 0.0;
	summary->litrN_maxRZ = 0.0;
	summary->leafDM = 0.0;
	summary->leaflitrDM = 0.0;
    summary->frootDM = 0.0;
	summary->yieldDM = 0.0;
	summary->yieldDM_HRV = 0.0;
    summary->softstemDM = 0.0;
    summary->livewoodDM = 0.0;
	summary->deadwoodDM = 0.0;
	summary->vegC = 0.0;
	summary->LDaboveC_nw = 0.0;
	summary->LDaboveC_w = 0.0;
	summary->LDaboveCwithNSC_nw = 0.0;
	summary->LDaboveCwithNSC_w = 0.0;
	summary->LaboveC_nw = 0.0;
	summary->LaboveC_w = 0.0;
	summary->LaboveCwithNSC_nw = 0.0;
	summary->LaboveCwithNSC_w = 0.0;
	summary->DaboveC_nw = 0.0;
	summary->DaboveC_w = 0.0;
	summary->DaboveCwithNSC_nw = 0.0;
	summary->DaboveCwithNSC_w = 0.0;
	summary->LDbelowC_nw = 0.0;
	summary->LDbelowC_w = 0.0;
	summary->LDbelowCwithNSC_nw = 0.0;
	summary->LDbelowCwithNSC_w = 0.0;
	summary->LbelowC_nw = 0.0;
	summary->LbelowC_w = 0.0;
	summary->LbelowCwithNSC_nw = 0.0;
	summary->LbelowCwithNSC_w = 0.0;
	summary->DbelowC_nw = 0.0;
	summary->DbelowC_w = 0.0;
	summary->DbelowCwithNSC_nw = 0.0;
	summary->DbelowCwithNSC_w = 0.0;
	summary->livingNSC = 0.0;
	summary->livingSC = 0.0;
	summary->livingBIOMabove = 0.0;
	summary->livingBIOMbelow = 0.0;
	summary->totalBIOMabove = 0.0;
	summary->totalBIOMbelow = 0.0;
	summary->annmax_livingBIOMabove = 0.0;
	summary->annmax_livingBIOMbelow = 0.0;
	summary->annmax_totalBIOMabove = 0.0;
	summary->annmax_totalBIOMbelow = 0.0;
	summary->totalC = 0.0;
	summary->CNlitr_total = 0.0;
	summary->CNsoil_total = 0.0;
	summary->leafCN = 0.0;
	summary->frootCN = 0.0;
	summary->softstemCN = 0.0;
	summary->yieldN = 0.0;

	summary->NH4_top30avail = 0.0; 
	summary->NO3_top30avail = 0.0;
	summary->sminN_top30avail = 0.0;
	summary->SOM_C_top30 = 0.0;
	summary->SOM_N_top30 = 0.0;
	summary->stableSOC_top30 = 0.0;
	summary->leafc_LandD = 0.0;
	summary->frootc_LandD = 0.0;
	summary->yield_LandD = 0.0;
	summary->softstemc_LandD = 0.0;
	summary->Cflux_lateral = 0.0;
	summary->cumCflux_lateral = 0.0;
	summary->harvestIndex = 0;
	summary->rootIndex = 0;
	summary->belowground_ratio = 0;
	
	return (errorCode);
}
