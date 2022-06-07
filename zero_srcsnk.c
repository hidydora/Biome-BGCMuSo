/* 
zero_srcsnk.c
fill the source and sink variables with 0.0 at the start of the simulation

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
BBGC MuSo v4
Copyright 2000, Peter E. Thornton
Numerical Terradynamics Simulation Group
Copyright 2014, D. Hidy (dori.hidy@gmail.com)
Hungarian Academy of Sciences
*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <malloc.h>
#include "bgc_struct.h"     /* structure definitions */
#include "bgc_func.h"       /* function prototypes */
#include "bgc_constants.h"

/* zero the source and sink state variables */
int zero_srcsnk(cstate_struct* cs, nstate_struct* ns, wstate_struct* ws,
	summary_struct* summary)
{
	int ok=1;
	
	/* zero the water sources and sinks  */
	ws->prcp_src = 0.0;
	ws->soilevap_snk = 0.0;
	ws->snowsubl_snk = 0.0;
	ws->canopyevap_snk = 0.0;
	ws->pondwevap_snk = 0.0;
	ws->trans_snk = 0.0;
	ws->pondwevap_snk = 0.0;
	ws->prcp_src = 0.0;
	ws->soilevap_snk = 0.0;
	
	/* thinning - Hidy 2012.*/
	ws->canopyw_THNsnk = 0.0;
	/* mowing - Hidy 2008.*/
	ws->canopyw_MOWsnk = 0.0;
	 /* harvesting - Hidy 2008.*/
	ws->canopyw_HRVsnk = 0.0;	
	/* ploughing - Hidy 2008.*/
	ws->canopyw_PLGsnk = 0.0;
	 /* grazing - Hidy 2008.*/
	ws->canopyw_GRZsnk = 0.0;	
	/* irrigation - Hidy 2015 */
	ws->IRGsrc = 0.0;
	ws->balance = 0.0;
	
	/* soil-water submodel- Hidy 2010.*/		
	ws->runoff_snk = 0.0;		
	ws->deeppercolation_snk = 0.0; 
	ws->deepdiffusion_snk = 0.0; 
	ws->deeptrans_src = 0.0; 
	ws->groundwater_src = 0.0;

;

	/* zero the carbon sources and sinks */
	cs->psnsun_src = 0.0;
	cs->psnshade_src = 0.0;
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
	cs->fire_snk = 0.0;
	/* senescence, standig dead biome and cut-down - Hidy 2010.*/
	cs->SNSCsnk = 0.0;
	cs->STDBsrc=0;
	cs->CTDBsrc=0;
	/* planting - Hidy 2008.*/
	cs->PLTsrc = 0.0; 
	/* thinning - Hidy 2012.*/
	cs->THN_transportC = 0.0; 
	/* mowing - Hidy 2008.*/
	cs->MOW_transportC = 0.0; 
	/* grazing - Hidy 2008. */
	cs->GRZsnk = 0.0;  
	cs->GRZsrc = 0.0;
	/* harvesting - Hidy 2012. */
	cs->HRV_transportC = 0.0;
	/* fertilizing - Hidy 2012.  */
	cs->FRZsrc = 0.0;
	/* fruit simulation - Hidy 2013.  */
	cs->fruit_mr_snk = 0.0;
	cs->fruit_gr_snk = 0.0;
	/* softstem simulation - Hidy 2013.  */
	cs->softstem_mr_snk = 0.0;
	cs->softstem_gr_snk = 0.0;
	cs->balance = 0.0;

	/* zero the nitrogen sources and sinks */
	ns->nfix_src = 0.0;
	ns->ndep_src = 0.0;
	ns->nleached_snk = 0.0;
	ns->ndiffused_snk = 0.0;
	ns->nvol_snk = 0.0;
	ns->fire_snk = 0.0;
	/* effect of boundary layer with constant N-content - Hidy 2015 */
	ns->BNDRYsrc = 0.0;
	ns->SPINUPsrc = 0.0;
	ns->sum_ndemand = 0.0;
	/* senescence, standing dead biome, cut-down dead biome.*/
	ns->SNSCsnk = 0.0;
	ns->STDBsrc = 0.0;
	ns->CTDBsrc = 0.0;
	/* planting - Hidy 2012.*/
	ns->PLTsrc = 0.0; 
	/* thinning - Hidy 2012.*/
	ns->THN_transportN = 0.0; 
	/* mowing - Hidy 2008.*/
	ns->MOW_transportN = 0.0; 
	/* grazing - Hidy 2008. */
	ns->GRZsnk = 0.0;  
	ns->GRZsrc = 0.0;
	/* harvesting - Hidy 2012. */
	ns->HRV_transportN = 0.0; 
	/* fertilizing - Hidy 2012. */
	ns->FRZsrc = 0.0; 
	ns->balance = 0.0;
	
	/* zero the summary variables */
	summary->cum_npp_ann = 0.0;
	summary->cum_npp = 0.0;
	summary->cum_nep = 0.0;
	summary->cum_nee = 0.0;
	summary->cum_gpp = 0.0;
	summary->vwc_annavg = 0.0;
	summary->cum_mr = 0.0;
	summary->cum_gr = 0.0;
	summary->cum_hr = 0.0;
	summary->cum_ET	= 0.0;
	summary->cum_fire = 0.0;
	summary->cum_n2o = 0.0;
	
	summary->Closs_MOW = 0.0;
	summary->Closs_THN = 0.0;
	summary->Closs_PLG = 0.0;
	summary->Closs_HRV = 0.0;
	summary->Closs_GRZ = 0.0;
	summary->Cplus_GRZ = 0.0;
	summary->Cplus_FRZ = 0.0;
	summary->Cplus_PLT = 0.0;
	summary->Nplus_FRZ = 0.0;
	summary->Nplus_GRZ = 0.0;
	summary->Closs_SNSC = 0.0;
	summary->Cplus_CTDB = 0.0;
	summary->Cplus_STDB = 0.0;
	summary->daily_litdecomp = 0.0;
	summary->daily_litfallc = 0.0;
	summary->daily_litfallc_above = 0.0;
	summary->daily_litfallc_below = 0.0;
	summary->daily_STDB_to_litr = 0.0;
	summary->daily_CTDB_to_litr = 0.0;
	summary->daily_litfire = 0.0;
	summary->daily_nbp = 0.0;
	summary->litrc = 0.0;
	summary->Nplus_FRZ = 0.0;
	summary->Nplus_GRZ = 0.0;
	summary->daily_gross_nimmob = 0.0;
	summary->daily_gross_nmin = 0.0;
	summary->daily_net_nmin = 0.0;
	summary->soilc = 0.0;
	summary->vegc = 0.0;
	summary->abgc = 0.0;
	summary->totalc = 0.0;
	summary->soiln = 0.0;
	summary->sminn = 0.0;
	summary->sminn_top10 = 0.0; 
	summary->soiln_top10 = 0.0;
	summary->sminn_top10 = 0.0;

	return (!ok);
}
