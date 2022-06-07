/* 
zero_srcsnk.c
fill the source and sink variables with 0.0 at the start of the simulation

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Biome-BGCMuSo v6.0.
Original code: Copyright 2000, Peter E. Thornton
Numerical Terradynamic Simulation Group, The University of Montana, USA
Modified code: Copyright 2019, D. Hidy [dori.hidy@gmail.com]
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
	int errflag=0;
	int layer;
	
	/* zero the water sources and sinks  */
	ws->prcp_src = 0.0;
	ws->soilevap_snk = 0.0;
	ws->snowsubl_snk = 0.0;
	ws->canopyevap_snk = 0.0;
	ws->trans_snk = 0.0;
	ws->pondwevap_snk = 0.0;
	ws->prcp_src = 0.0;
	ws->soilevap_snk = 0.0;
	ws->canopyw_THNsnk = 0.0;
	ws->canopyw_MOWsnk = 0.0;
	ws->canopyw_HRVsnk = 0.0;	
	ws->canopyw_PLGsnk = 0.0;
	ws->canopyw_GRZsnk = 0.0;	
	ws->IRGsrc_W = 0.0;
	ws->FRZsrc_W = 0.0;
	ws->runoff_snk = 0.0;		
	ws->deeppercolation_snk = 0.0;  
	ws->groundwater_src = 0.0;
	ws->WbalanceERR = 0.0;	
	ws->inW = 0.0;
	ws->outW = 0.0;
	ws->storeW = 0.0;


	/* zero the carbon sources and sinks */
	cs->psnsun_src = 0.0;
	cs->psnshade_src = 0.0;
	cs->NSC_mr_snk = 0;
	cs->actC_mr_snk = 0;
	cs->leaf_mr_snk = 0.0;
	cs->leaf_gr_snk = 0.0;
	cs->froot_mr_snk = 0.0;
	cs->froot_gr_snk = 0.0;
	cs->livestem_mr_snk = 0.0;
	cs->livestem_gr_snk = 0.0;
	cs->deadstem_gr_snk = 0.0;
	cs->livecroot_mr_snk = 0.0;
	cs->livecroot_gr_snk = 0.0;
	cs->deadcroot_gr_snk = 0.0;
	cs->litr1_hr_snk = 0.0;
	cs->litr2_hr_snk = 0.0;
	cs->litr4_hr_snk = 0.0;
	cs->soil1_hr_snk = 0.0;
	cs->soil2_hr_snk = 0.0;
	cs->soil3_hr_snk = 0.0;
	cs->soil4_hr_snk = 0.0;
	cs->FIREsnk_C = 0.0;
	cs->SNSCsnk_C = 0.0;
	cs->PLTsrc_C = 0.0; 
	cs->THN_transportC = 0.0; 

	cs->MOW_transportC = 0;
	cs->GRZsnk_C = 0.0;  
	cs->GRZsrc_C = 0.0;
	cs->HRV_transportC = 0.0;
	cs->FRZsrc_C = 0.0;
	cs->fruitC_HRV = 0.0;
	cs->vegC_HRV = 0.0;
	cs->fruit_mr_snk = 0.0;
	cs->fruit_gr_snk = 0.0;
	cs->softstem_mr_snk = 0.0;
	cs->softstem_gr_snk = 0.0;
	cs->CbalanceERR = 0.0;
	cs->inC = 0.0;
	cs->outC = 0.0;
	cs->storeC = 0.0;

	ns->nfix_src = 0.0;
	ns->ndep_src = 0.0;
	ns->nleached_snk = 0.0;
	ns->ndiffused_snk = 0.0;
	ns->nvol_snk = 0.0;
	ns->FIREsnk_N = 0.0;
	ns->SPINUPsrc = 0.0;
	ns->sum_ndemand = 0.0;
	ns->SNSCsnk_N = 0.0;
	ns->PLTsrc_N = 0.0; 
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
	summary->annrunoff = 0.0;
	summary->annoutflow = 0.0;
	summary->cum_npp = 0.0;
	summary->cum_nep = 0.0;
	summary->cum_nee = 0.0;
	summary->cum_gpp = 0.0;
	summary->cum_mr = 0.0;
	summary->cum_gr = 0.0;
	summary->cum_hr = 0.0;
	summary->cum_transp  = 0;
	summary->cum_ET  = 0;	
	summary->cum_ET	= 0.0;
	summary->cum_fire = 0.0;
	summary->cum_n2o = 0.0;
	summary->cum_Closs_MGM  = 0;
	summary->cum_Cplus_MGM  = 0;
	summary->cum_Closs_MOW = 0.0;
	summary->cum_Closs_THN_w = 0.0;
	summary->cum_Closs_THN_nw = 0.0;
	summary->cum_Closs_PLG = 0.0;
	summary->cum_Closs_HRV = 0.0;
	summary->cum_yieldC_HRV = 0.0;
	summary->cum_Closs_GRZ = 0.0;
	summary->cum_Cplus_GRZ = 0.0;
	summary->cum_Cplus_FRZ = 0.0;
	summary->cum_Cplus_PLT = 0.0;
	summary->cum_Nplus_FRZ = 0.0;
	summary->cum_Nplus_GRZ = 0.0;
	summary->cum_Closs_SNSC = 0.0;
	summary->cum_Cplus_CTDB = 0.0;
	summary->cum_Cplus_STDB = 0.0;
	summary->daily_litdecomp = 0.0;
	summary->daily_litfallc = 0.0;
	summary->daily_litfallc_above = 0.0;
	summary->daily_litfallc_below = 0.0;
	summary->daily_litfire = 0.0;
	summary->daily_nbp = 0.0;
	summary->litrc = 0.0;
	summary->daily_gross_nimmob_total = 0.0;
	summary->daily_gross_nmin_total = 0.0;
	summary->daily_net_nmin_total = 0.0;
	summary->soilc = 0.0;
	summary->leaf_DM = 0.0;
	summary->leaflitr_DM = 0.0;
    summary->froot_DM = 0.0;
	summary->fruit_DM = 0.0;
    summary->softstem_DM = 0.0;
    summary->livewood_DM = 0.0;
	summary->deadwood_DM = 0.0;
	summary->vegc = 0.0;
	summary->abgc = 0.0;
	summary->totalc = 0.0;
	summary->soiln = 0.0;
	summary->sminn = 0.0;
	summary->NH4_top30 = 0.0; 
	summary->NO3_top30 = 0.0;
	summary->SOM_C_top30 = 0.0;
	summary->SOM_N_top30 = 0.0;
	summary->humusC_top30 = 0.0;
	summary->SOM_C_30to60 = 0.0;
	summary->SOM_C_60to90 = 0.0;
	summary->leafc_LandD = 0.0;
	summary->frootc_LandD = 0.0;
	summary->fruitc_LandD = 0.0;
	summary->softstemc_LandD = 0.0;

	for (layer = 0; layer < N_SOILLAYERS; layer++)
	{
		summary->sminNH4_ppm[layer] = 0.0;
		summary->sminNO3_ppm[layer] = 0.0;
	}

	return (errflag);
}
