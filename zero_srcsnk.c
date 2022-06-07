/* 
zero_srcsnk.c
fill the source and sink variables with 0.0 at the start of the simulation

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Biome-BGC version 4.1.1
Copyright 2000, Peter E. Thornton
Numerical Terradynamics Simulation Group (NTSG)
School of Forestry, University of Montana
Missoula, MT 59812
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
	ws->trans_snk = 0.0;
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
	/* soil-water submodel- Hidy 2010.*/		
	ws->runoff_snk = 0.0;		
	ws->deeppercolation_snk = 0.0; 
	ws->deepdiffusion_snk = 0.0; 

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
	/* senescence - Hidy 2010.*/
	cs->SNSC_snk = 0.0;
	cs->SNSC_src = 0.0;
	/* planting - Hidy 2008.*/
	cs->PLTsrc = 0.0; 
	/* thinning - Hidy 2012.*/
	cs->THNsnk = 0.0; 
	/* mowing - Hidy 2008.*/
	cs->MOWsnk = 0.0; 
	/* grazing - Hidy 2008. */
	cs->GRZsnk = 0.0;  
	cs->GRZsrc = 0.0;
	/* harvesting - Hidy 2012. */
	cs->HRVsnk = 0.0;  
	cs->HRVsrc = 0.0;
	/* ploughing - Hidy 2012. */
	cs->PLGsnk = 0.0;	
	cs->PLGsrc = 0.0;
	/* fertilizing */
	cs->FRZsrc = 0.0;
	
	/* zero the nitrogen sources and sinks */
	ns->nfix_src = 0.0;
	ns->ndep_src = 0.0;
	ns->nleached_snk = 0.0;
	ns->nvol_snk = 0.0;
	ns->fire_snk = 0.0;
	/* senescence - Hidy 2010.*/
	ns->SNSC_snk = 0.0;
	ns->SNSC_src = 0.0;
	/* planting - Hidy 2012.*/
	ns->PLTsrc = 0.0; 
	/* thinning - Hidy 2012.*/
	ns->THNsnk = 0.0; 
	/* mowing - Hidy 2008.*/
	ns->MOWsnk = 0.0; 
	/* grazing - Hidy 2008. */
	ns->GRZsnk = 0.0;  
	ns->GRZsrc = 0.0;
	/* harvesting - Hidy 2012. */
	ns->HRVsnk = 0.0; 
	ns->HRVsrc = 0.0;
	/* ploughing - Hidy 2012. */
	ns->PLGsnk = 0.0;	
	ns->PLGsrc = 0.0;
	/* fertilizing */
	ns->FRZsrc = 0.0;
	
	/* zero the summary variables */
	summary->cum_npp = 0.0;
	summary->cum_nep = 0.0;
	summary->cum_nee = 0.0;
	summary->cum_gpp = 0.0;
	summary->cum_mr = 0.0;
	summary->cum_gr = 0.0;
	summary->cum_hr = 0.0;


	return (!ok);
}
