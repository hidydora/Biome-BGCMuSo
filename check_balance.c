/* 
check_balance.c
daily test of mass balance (water, carbon, and nitrogen state variables)

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
BBGC MuSo v4
Copyright 2000, Peter E. Thornton
Numerical Terradynamics Siulation Group
Copyright 2014, D. Hidy (dori.hidy@gmail.com)
Hungarian Academy of Sciences
*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <malloc.h>
#include "bgc_struct.h"
#include "bgc_func.h"
#include "bgc_constants.h"

int check_water_balance(wstate_struct* ws, int first_balance)
{
	int ok=1;
	static double old_balance;
	double in, out, store, balance;
	
	/* DAILY CHECK ON WATER BALANCE */
	
	/* sum of sources */
	in = ws->prcp_src + ws->groundwater_src + ws->IRGsrc;
	
	/* sum of sinks */
	out = ws->soilevap_snk + ws->snowsubl_snk + 
		ws->canopyevap_snk + ws->trans_snk + ws->pondwevap_snk +
		ws->canopyw_THNsnk +		/* thinning - Hidy 2012.*/
		ws->canopyw_MOWsnk +		/* mowing - Hidy 2008.*/
		ws->canopyw_HRVsnk +		/* harvesting - Hidy 2008.*/
		ws->canopyw_PLGsnk +			/* ploughing - Hidy 2008.*/
		ws->canopyw_GRZsnk +			/* grazing - Hidy 2008.*/
		ws->runoff_snk	  +			/* soil-water submodel - Hidy 2010.*/
		ws->deeppercolation_snk +   /* soil-water submodel - Hidy 2010.*/
		ws->deepdiffusion_snk;		/* soil-water submodel - Hidy 2010.*/

	/* sum of current storage */
	store = ws->soilw_SUM + ws->pond_water + ws->snoww + ws->canopyw;

	if (ws->snoww < 0.0 || ws->canopyw < 0  || ws->soilw_SUM < 0 || ws->pond_water < 0)
	{	
		printf("ERROR: negative water storage\n");
		ok=0;
	}
	
	/* calculate current balance */
	balance = in - out - store;
	 
	
	if (!first_balance)
	{
		if (fabs(old_balance - balance) > 0)
		{
			if (fabs(old_balance - balance) > ws->balance) ws->balance = fabs(old_balance - balance);
		}
	}
	old_balance = balance;
	
	return (!ok);
}

int check_carbon_balance(cstate_struct* cs, int first_balance)
{
	int ok=1;
	int layer=0;
	static double old_balance;
	double in, out, store, balance;
	
	/* Hidy 2010 - control avoiding negative pools */
	if (cs->leafc < 0.0 ||  cs->leafc_storage < 0.0 || cs->leafc_transfer < 0.0 || 
		cs->frootc < 0.0 || cs->frootc_storage < 0.0 || cs->frootc_transfer < 0.0 || 
		cs->fruitc < 0.0 || cs->fruitc_storage < 0.0 || cs->fruitc_transfer < 0.0 || 
		cs->softstemc < 0.0 || cs->softstemc_storage < 0.0 || cs->softstemc_transfer < 0.0 || 
		cs->livestemc < 0.0 || cs->livestemc_storage < 0.0 || cs->livestemc_transfer < 0.0 || 
		cs->deadstemc < 0.0 || cs->deadstemc_storage < 0.0 || cs->deadstemc_transfer < 0.0 || 
		cs->livecrootc < 0.0 ||  cs->livecrootc_storage < 0.0 || cs->livecrootc_transfer < 0.0 || 
		cs->deadcrootc < 0.0 || cs->deadcrootc_storage < 0.0 || cs->deadcrootc_transfer < 0.0 || 
		cs->gresp_storage < 0.0 || cs->gresp_transfer < 0.0 ||
		cs->litr1cA_STDB < 0 || cs->litr2cA_STDB < 0 || cs->litr3cA_STDB < 0|| cs->litr4cA_STDB < 0 || cs->cwdcA_STDB < 0 ||
		cs->litr1cB_STDB < 0 || cs->litr2cB_STDB < 0 || cs->litr3cB_STDB < 0|| cs->litr4cB_STDB < 0 || cs->cwdcB_STDB < 0 ||
		cs->litr1cA_CTDB < 0 || cs->litr2cA_CTDB < 0 || cs->litr3cA_CTDB < 0|| cs->litr4cA_CTDB < 0 || cs->cwdcA_CTDB < 0 ||
		cs->litr1cB_CTDB < 0 || cs->litr2cB_CTDB < 0 || cs->litr3cB_CTDB < 0|| cs->litr4cB_CTDB < 0 || cs->cwdcB_CTDB < 0)
		{	
			printf("ERROR: negative carbon stock\n");
			ok=0;
		}

	
	/* Hidy 2016 - control avoiding negative pools in SOILC array and calculate soil1c_total */

	cs->soil1c_total = 0;
	cs->soil2c_total = 0;
	cs->soil3c_total = 0;
	cs->soil4c_total = 0;
	cs->litr1c_total = 0;
	cs->litr2c_total = 0;
	cs->litr3c_total = 0;
	cs->litr4c_total = 0;
	cs->cwdc_total   = 0;

	for (layer=0; layer < N_SOILLAYERS; layer++)

	{
		if (cs->soil1c[layer] < 0.0 || cs->soil2c[layer] < 0.0 || cs->soil3c[layer] < 0.0 || cs-> soil4c[layer] < 0.0)
		{	
			printf("ERROR: negative soil carbon stock\n");
			ok=0;
		}
		cs->soil1c_total += cs->soil1c[layer];
		cs->soil2c_total += cs->soil2c[layer];
		cs->soil3c_total += cs->soil3c[layer];
		cs->soil4c_total += cs->soil4c[layer];

		if (cs->litr1c[layer] < 0.0 || cs->litr2c[layer] < 0.0 || cs->litr3c[layer] < 0.0 || cs-> litr4c[layer] < 0.0  || 
			cs->cwdc[layer] < 0.0)
		{	
			printf("ERROR: negative litter carbon stock\n");
			ok=0;
		}
		cs->litr1c_total += cs->litr1c[layer];
		cs->litr2c_total += cs->litr2c[layer];
		cs->litr3c_total += cs->litr3c[layer];
		cs->litr4c_total += cs->litr4c[layer];
		cs->cwdc_total   += cs->cwdc[layer];
	}

	/* DAILY CHECK ON CARBON BALANCE */
	
	/* sum of sources: photosynthesis and managenet */
	in = cs->psnsun_src + cs->psnshade_src + 
 		 cs->PLTsrc + cs->GRZsrc + cs->FRZsrc;
	
	/* sum of sinks: respiration, fire and management */
	out = cs->leaf_mr_snk      + cs->leaf_gr_snk      + cs->froot_mr_snk     + cs->froot_gr_snk + 
		  cs->fruit_mr_snk     + cs->fruit_gr_snk     + cs->softstem_mr_snk  + cs->softstem_gr_snk + 
		  cs->livestem_mr_snk  + cs->livestem_gr_snk  + cs->deadstem_gr_snk  + 
		  cs->livecroot_mr_snk + cs->livecroot_gr_snk + cs->deadcroot_gr_snk + 
		  cs->litr1_hr_snk + cs->litr2_hr_snk + cs->litr4_hr_snk + 
		  cs->soil1_hr_snk + cs->soil2_hr_snk + cs->soil3_hr_snk + cs->soil4_hr_snk + 
		  cs->fire_snk + 
		  cs->GRZsnk + cs->THN_transportC + cs->MOW_transportC + cs->HRV_transportC; 
		
		     
		
	/* sum of current storage */
	store = cs->leafc      + cs->leafc_storage      + cs->leafc_transfer +
			cs->frootc     + cs->frootc_storage     + cs->frootc_transfer + 
			cs->fruitc     + cs->fruitc_storage     + cs->fruitc_transfer +
			cs->softstemc  + cs->softstemc_storage  + cs->softstemc_transfer +
			cs->livestemc  + cs->livestemc_storage  + cs->livestemc_transfer + 
			cs->deadstemc  + cs->deadstemc_storage  + cs->deadstemc_transfer +
			cs->livecrootc + cs->livecrootc_storage + cs->livecrootc_transfer + 
			cs->deadcrootc + cs->deadcrootc_storage + cs->deadcrootc_transfer + 
			cs->gresp_storage + cs->gresp_transfer  + 
			cs->cwdc_total + 
			cs->litr1c_total + cs->litr2c_total + cs->litr3c_total + cs->litr4c_total + 
			cs->soil1c_total + cs->soil2c_total + cs->soil3c_total + cs->soil4c_total + 
			cs->cpool +
			cs->CTDBc_above + cs->STDBc_above + cs->CTDBc_below + cs->STDBc_below;   
	
	/* calculate current balance */
	balance = in - out - store;
	 
	if (!first_balance)
	{
		if (fabs(old_balance - balance) > 0)
		{
	 		if (fabs(old_balance - balance) > cs->balance) cs->balance = fabs(old_balance - balance);
		}
	}
	old_balance = balance;


	return (!ok);
}		

int check_nitrogen_balance(nstate_struct* ns, int first_balance)
{
	int ok=1;
	int layer=0;
	double in,out,store,balance;
	static double old_balance = 0.0;
	
	/* Hidy 2010 -	CONTROL AVOIDING NITROGEN POOLS */
	if (ns->leafn < 0.0 || ns->leafn < 0.0 ||  ns->leafn_storage < 0.0 || ns->leafn_transfer < 0.0 || 
		ns->frootn < 0.0 || ns->frootn_storage < 0.0 || ns->frootn_transfer < 0.0 || 
		ns->fruitn < 0.0 || ns->fruitn_storage < 0.0 || ns->fruitn_transfer < 0.0 || 
		ns->softstemn < 0.0 || ns->softstemn_storage < 0.0 || ns->softstemn_transfer < 0.0 || 
		ns->livestemn < 0.0 || ns->livestemn_storage < 0.0 || ns->livestemn_transfer < 0.0 || 
		ns->deadstemn < 0.0 || ns->deadstemn_storage < 0.0 || ns->deadstemn_transfer < 0.0 || 
		ns->livecrootn < 0.0 ||  ns->livecrootn_storage < 0.0 || ns->livecrootn_transfer < 0.0 || 
		ns->deadcrootn < 0.0 || ns->deadcrootn_storage < 0.0 || ns->deadcrootn_transfer < 0.0 ||
		ns->retransn < 0.0 ||
		ns->litr1nA_STDB < 0 || ns->litr2nA_STDB < 0 || ns->litr3nA_STDB < 0|| ns->litr4nA_STDB < 0 || ns->cwdnA_STDB < 0 ||
		ns->litr1nB_STDB < 0 || ns->litr2nB_STDB < 0 || ns->litr3nB_STDB < 0|| ns->litr4nB_STDB < 0 || ns->cwdnB_STDB < 0 ||
		ns->litr1nA_CTDB < 0 || ns->litr2nA_CTDB < 0 || ns->litr3nA_CTDB < 0|| ns->litr4nA_CTDB < 0 || ns->cwdnA_CTDB < 0 ||
		ns->litr1nB_CTDB < 0 || ns->litr2nB_CTDB < 0 || ns->litr3nB_CTDB < 0|| ns->litr4nB_CTDB < 0 || ns->cwdnB_CTDB < 0)
	{	
		printf("ERROR: negative nitrogen stock\n");
		ok=0;
	}

	/* Hidy 2016 - control avoiding negative pools in SOILC array and calculate soil1c_total */
	ns->soil1n_total	= 0;
	ns->soil2n_total	= 0;
	ns->soil3n_total	= 0;
	ns->soil4n_total	= 0;
	ns->sminn_total		= 0;
	ns->litr1n_total	= 0;
	ns->litr2n_total	= 0;
	ns->litr3n_total	= 0;
	ns->litr4n_total	= 0;
	ns->cwdn_total		= 0;

	for (layer=0; layer < N_SOILLAYERS; layer++)

	{
		if (ns->soil1n[layer] < 0.0 || ns->soil2n[layer] < 0.0 || ns->soil3n[layer] < 0.0 || ns-> soil4n[layer] < 0.0 || ns->sminn[layer] < 0)
		{	
			printf("ERROR: negative soil nitrogen stock\n");
			ok=0;
		}
		ns->soil1n_total += ns->soil1n[layer];
		ns->soil2n_total += ns->soil2n[layer];
		ns->soil3n_total += ns->soil3n[layer];
		ns->soil4n_total += ns->soil4n[layer];
		

		if (ns->litr1n[layer] < 0.0 || ns->litr2n[layer] < 0.0 || ns->litr3n[layer] < 0.0 || ns-> litr4n[layer] < 0.0 || 
			ns->cwdn[layer] < 0.0)
		{	
			printf("ERROR: negative litter nitrogen stock\n");
			ok=0;
		}

		if (ns->sminn[layer] < 0 )
		{	
			printf("ERROR: negative mineralized nitrogen stock\n");
			ok=0;
		}

		ns->litr1n_total	+= ns->litr1n[layer];
		ns->litr2n_total	+= ns->litr2n[layer];
		ns->litr3n_total	+= ns->litr3n[layer];
		ns->litr4n_total	+= ns->litr4n[layer];
		ns->sminn_total		+= ns->sminn[layer];
		ns->cwdn_total		+= ns->cwdn[layer];

	}


	/* DAILY CHECK ON NITROGEN BALANCE */
	
	/* sum of sources: fixation, deposition, spinup add, management */
	in = ns->nfix_src + ns->ndep_src + ns->SPINUPsrc +
		 ns->PLTsrc +  ns->GRZsrc +  ns->FRZsrc;
	
	
	/* sum of sinks: volatilization, fire, leach, diffusion, management */
	out = ns->nvol_snk + ns->fire_snk + ns->nleached_snk + ns->ndiffused_snk + 
		  ns->GRZsnk + ns->THN_transportN + ns->MOW_transportN + ns->HRV_transportN;

		
	/* sum of current storage */
	store = ns->leafn      + ns->leafn_storage      + ns->leafn_transfer +
		    ns->frootn     + ns->frootn_storage     + ns->frootn_transfer + 			
		    ns->fruitn     + ns->fruitn_storage     + ns->fruitn_transfer + 
		    ns->softstemn  + ns->softstemn_storage  + ns->softstemn_transfer + 
			ns->livestemn  + ns->livestemn_storage  + ns->livestemn_transfer + 
			ns->deadstemn  + ns->deadstemn_storage  + ns->deadstemn_transfer + 
			ns->livecrootn + ns->livecrootn_storage + ns->livecrootn_transfer + 
			ns->deadcrootn + ns->deadcrootn_storage + ns->deadcrootn_transfer +
			ns->retransn + 
			ns->litr1n_total + ns->litr2n_total + ns->litr3n_total + ns->litr4n_total +
			ns->soil1n_total + ns->soil2n_total + ns->soil3n_total + ns->soil4n_total +
			ns->sminn_total  + ns->cwdn_total + 
			ns->npool        + 
			ns->CTDBn_above + ns->STDBn_above + ns->CTDBn_below + ns->STDBn_below;

	
	/* calculate current balance */
	balance = in - out - store;
	 
	if (!first_balance)
	{
	
		if (fabs(old_balance - balance) > 0)
		{
			if (fabs(old_balance - balance) > ns->balance) ns->balance = fabs(old_balance - balance);
		}
	}
	old_balance = balance;

	
	
	return (!ok);
}

