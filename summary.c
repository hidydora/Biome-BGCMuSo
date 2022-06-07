/* 
summary.c
summary variables for potential output

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
BBGC MuSo v4
Copyright 2000, Peter E. Thornton
Numerical Terradynamics Simulation Group
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

int cnw_summary(int yday, cstate_struct* cs, cflux_struct* cf, nstate_struct* ns, nflux_struct* nf, wflux_struct* wf, epvar_struct* epv,
				summary_struct* summary)
{
	int ok=1;
	int layer;
	double gpp,mr,gr,hr,tr, fire;
	double sr; /* Hidy 2012 - calculating soil respiration */
	double npp,nep,nee, nbp, disturb_loss, disturb_gain;
	double Closs_THN, Closs_MOW, Closs_HRV, Closs_PLG, Closs_GRZ, Cplus_PLT, Cplus_FRZ, Cplus_GRZ, daily_STDB_to_litr, daily_CTDB_to_litr;

	/* annual summaruzed cumNPP - Hidy 2015 */
	if (yday == 0) summary->cum_npp_ann = 0;

	/*******************************************************************************/
	/* 1. summarize carbon stocks */
	summary->abgc = cs->leafc+cs->fruitc+cs->softstemc;;
	

	summary->vegc = cs->leafc + cs->leafc_storage + cs->leafc_transfer + 
		cs->frootc + cs->frootc_storage + cs->frootc_transfer +
		cs->livestemc + cs->livestemc_storage + cs->livestemc_transfer +
		cs->deadstemc + cs->deadstemc_storage + cs->deadstemc_transfer +
		cs->livecrootc + cs->livecrootc_storage + cs->livecrootc_transfer +
		cs->deadcrootc + cs->deadcrootc_storage + cs->deadcrootc_transfer +
		cs->gresp_storage + cs->gresp_transfer + 
		cs->fruitc + cs->fruitc_storage + cs->fruitc_transfer +
		cs->softstemc + cs->softstemc_storage + cs->softstemc_transfer;

	summary->litrc = cs->cwdc_total + cs->litr1c_total + cs->litr2c_total + cs->litr3c_total + cs->litr4c_total;
	summary->soilc = cs->soil1c_total + cs->soil2c_total + cs->soil3c_total + cs->soil4c_total;
	summary->totalc = summary->vegc + summary->litrc + summary->soilc;

	summary->soiln = ns->soil1n_total + ns->soil2n_total + ns->soil3n_total + ns->soil4n_total;
	summary->sminn = ns->sminn_total;

	summary->soilc_top10 = cs->soil1c[0] + cs->soil1c[1] + cs->soil1c[2] +
						   cs->soil2c[0] + cs->soil2c[1] + cs->soil2c[2] +
						   cs->soil3c[0] + cs->soil3c[1] + cs->soil3c[2] +
						   cs->soil4c[0] + cs->soil4c[1] + cs->soil4c[2];

	summary->soiln_top10 = ns->soil1n[0] + ns->soil1n[1] + ns->soil1n[2] +
						   ns->soil2n[0] + ns->soil2n[1] + ns->soil2n[2] +
						   ns->soil3n[0] + ns->soil3n[1] + ns->soil3n[2] +
						   ns->soil4n[0] + ns->soil4n[1] + ns->soil4n[2];

	summary->sminn_top10 = ns->sminn[0] + ns->sminn[1] + ns->sminn[2];

	/*******************************************************************************/
	/* 2. calculate daily fluxes (GPP, NPP, NEP, MR, GR, HR) positive for net growth: NPP = Gross PSN - Maintenance Resp - Growth Resp */

	gpp = cf->psnsun_to_cpool + cf->psnshade_to_cpool;
	
	mr = cf->leaf_day_mr + cf->leaf_night_mr + 
		 cf->froot_mr + cf->livestem_mr + cf->livecroot_mr +
		 /* fruit simulation - Hidy 2013. */
		 cf->fruit_mr +
		 /* softstem simulation - Hidy 2013. */
		 cf->softstem_mr;

	gr = cf->cpool_leaf_gr + cf->cpool_leaf_storage_gr + cf->transfer_leaf_gr +
		cf->cpool_froot_gr + cf->cpool_froot_storage_gr + cf->transfer_froot_gr + 
		cf->cpool_livestem_gr + cf->cpool_livestem_storage_gr + cf->transfer_livestem_gr +
		cf->cpool_deadstem_gr + cf->cpool_deadstem_storage_gr + cf->transfer_deadstem_gr + 
		cf->cpool_livecroot_gr + cf->cpool_livecroot_storage_gr + cf->transfer_livecroot_gr + 
		cf->cpool_deadcroot_gr + cf->cpool_deadcroot_storage_gr + cf->transfer_deadcroot_gr +
		/* fruit simulation - Hidy 2013. */
		cf->cpool_fruit_gr + cf->cpool_fruit_storage_gr + cf->transfer_fruit_gr +
		/* softstem simulation - Hidy 2013. */
		cf->cpool_softstem_gr + cf->cpool_softstem_storage_gr + cf->transfer_softstem_gr;

	npp = gpp - mr - gr;

	/* heterotrophic respiration (Hidy 2016 - multilayer soil) */
	hr=0;
	for (layer = 0; layer < N_SOILLAYERS; layer++)
	{
		hr += cf->litr1_hr[layer] + cf->litr2_hr[layer] + cf->litr4_hr[layer] + 
			  cf->soil1_hr[layer] + cf->soil2_hr[layer] + cf->soil3_hr[layer] + cf->soil4_hr[layer];
	}

	tr = mr + gr + hr;
	
	nep = npp - hr;
	
	/* soil respiration (Hidy 2012)*/
	sr = cf->froot_mr + cf->livecroot_mr +
		 cf->cpool_froot_gr + cf->cpool_froot_storage_gr + cf->transfer_froot_gr + 
		 cf->cpool_livecroot_gr + cf->cpool_livecroot_storage_gr + cf->transfer_livecroot_gr + 
		 cf->cpool_deadcroot_gr + cf->cpool_deadcroot_storage_gr + cf->transfer_deadcroot_gr +
		 hr;
		
	
	/* calculate daily NEE, positive for net sink: NEE = NEP - fire losses */
	fire =  cf->m_leafc_to_fire +  cf->m_leafc_storage_to_fire + cf->m_leafc_transfer_to_fire +
			cf->m_frootc_to_fire + cf->m_frootc_storage_to_fire + cf->m_frootc_transfer_to_fire +  
			cf->m_livestemc_to_fire + cf->m_livestemc_storage_to_fire + 	cf->m_livestemc_transfer_to_fire + 
			cf->m_deadstemc_to_fire + cf->m_deadstemc_storage_to_fire + cf->m_deadstemc_transfer_to_fire + 
			cf->m_livecrootc_to_fire + cf->m_livecrootc_storage_to_fire + cf->m_livecrootc_transfer_to_fire +
			cf->m_deadcrootc_to_fire + cf->m_deadcrootc_storage_to_fire + cf->m_deadcrootc_transfer_to_fire + 
			cf->m_gresp_storage_to_fire + cf->m_gresp_transfer_to_fire + 
			cf->m_litr1c_to_fire + cf->m_litr2c_to_fire + cf->m_litr3c_to_fire + cf->m_litr4c_to_fire +
			cf->m_cwdc_to_fire +
			/* fruit simulation - Hidy 2013. */
			cf->m_fruitc_to_fire +  cf->m_fruitc_storage_to_fire + cf->m_fruitc_transfer_to_fire +
			/* softstem simulation - Hidy 2013. */
			cf->m_softstemc_to_fire +  cf->m_softstemc_storage_to_fire + cf->m_softstemc_transfer_to_fire;
	nee = nep - fire;
	
	summary->daily_nep = nep;
	summary->daily_npp = npp;
	summary->daily_nee = -1 * nee;	// Hidy: NEE is positive if ecosystem is net source and negative if it is net sink
	summary->daily_gpp = gpp;
	summary->daily_mr = mr;
	summary->daily_gr = gr;
	summary->daily_hr = hr;
	summary->daily_sr = sr;
	summary->daily_tr = tr;
	summary->daily_fire = fire;
	summary->cum_npp_ann += npp;
	summary->cum_npp += npp;
	summary->cum_nep += nep;
	summary->cum_nee += -1*nee;
	summary->cum_gpp += gpp;
	summary->cum_mr += mr;
	summary->cum_gr += gr;
	summary->cum_hr += hr;
	summary->cum_fire += fire;

	summary->cum_n2o += nf->N2O_flux_soil + nf->N2O_flux_GRZ + nf->N2O_flux_FRZ;
	summary->cum_ET += wf->evapotransp;
	summary->vwc_annavg += epv->vwc_avg;
	
	/*******************************************************************************/
	/* 3. calculation litter fluxes and pools- Hidy 2015 */

	summary->daily_litter = cs->litr1c_total + cs->litr2c_total + cs->litr3c_total + cs->litr4c_total;
	
	/* Hidy 2016 - multilayer soil */
	summary->daily_litdecomp = 0;
	for (layer = 0; layer < N_SOILLAYERS; layer++)
	{
		summary->daily_litdecomp += cf->litr1c_to_soil1c[layer] + cf->litr2c_to_soil2c[layer]  + cf->litr4c_to_soil3c[layer];
	}

	summary->daily_litfire = cf->m_litr1c_to_fire + cf->m_litr2c_to_fire + cf->m_litr3c_to_fire + cf->m_litr4c_to_fire;
	
	/* aboveground litter */
	summary->daily_litfallc_above = 
		cf->m_leafc_to_litr1c + cf->m_leafc_to_litr2c + cf->m_leafc_to_litr3c + cf->m_leafc_to_litr4c + 
		cf->m_livestemc_to_cwdc + cf->m_deadstemc_to_cwdc + cf->m_livecrootc_to_cwdc + cf->m_deadcrootc_to_cwdc +
		cf->leafc_to_litr1c + cf->leafc_to_litr2c + cf->leafc_to_litr3c + cf->leafc_to_litr4c + 
		cf->softstemc_to_litr1c + cf->softstemc_to_litr2c + cf->softstemc_to_litr3c + cf->softstemc_to_litr4c + 
		cf->m_softstemc_to_litr1c + cf->m_softstemc_to_litr2c + cf->m_softstemc_to_litr3c + cf->m_softstemc_to_litr4c + 
		cf->fruitc_to_litr1c + cf->fruitc_to_litr2c + cf->fruitc_to_litr3c + cf->fruitc_to_litr4c + 
		cf->m_fruitc_to_litr1c + cf->m_fruitc_to_litr2c + cf->m_fruitc_to_litr3c + cf->m_fruitc_to_litr4c;
	
	/* belowground */
	summary->daily_litfallc_below = 
		cf->m_frootc_to_litr1c + cf->m_frootc_to_litr2c + cf->m_frootc_to_litr3c + cf->m_frootc_to_litr4c +
		cf->m_leafc_storage_to_litr1c + cf->m_frootc_storage_to_litr1c +
		cf->m_leafc_transfer_to_litr1c + cf->m_frootc_transfer_to_litr1c +		
		cf->m_livestemc_storage_to_litr1c + cf->m_deadstemc_storage_to_litr1c +
		cf->m_livestemc_transfer_to_litr1c + cf->m_deadstemc_transfer_to_litr1c +
		cf->m_livecrootc_storage_to_litr1c + cf->m_deadcrootc_storage_to_litr1c +
		cf->m_livecrootc_transfer_to_litr1c + cf->m_deadcrootc_transfer_to_litr1c +
		cf->m_gresp_storage_to_litr1c + cf->m_gresp_transfer_to_litr1c +
		cf->frootc_to_litr1c + cf->frootc_to_litr2c + cf->frootc_to_litr3c + cf->frootc_to_litr4c +
		cf->m_softstemc_storage_to_litr1c + cf->m_softstemc_transfer_to_litr1c + 
		cf->m_fruitc_storage_to_litr1c + cf->m_fruitc_transfer_to_litr1c;


	summary->daily_litfallc = summary->daily_litfallc_above + summary->daily_litfallc_below;

	/*******************************************************************************/
	/* 3. calculation of disturbance and senescence effect - Hidy 2016 */

	/* 3.1 SOURCES: cut-down biomass and standing dead biome */
	daily_STDB_to_litr =
	                        cf->STDB_to_litr1cA + cf->STDB_to_litr2cA + cf->STDB_to_litr3cA + cf->STDB_to_litr4cA +
	                        cf->STDB_to_litr1cB + cf->STDB_to_litr2cB + cf->STDB_to_litr3cB + cf->STDB_to_litr4cB;
	daily_CTDB_to_litr =
	                        cf->CTDB_to_litr1cA + cf->CTDB_to_litr2cA + cf->CTDB_to_litr3cA + cf->CTDB_to_litr4cA +
	                       cf->CTDB_to_litr1cB + cf->CTDB_to_litr2cB + cf->CTDB_to_litr3cB + cf->CTDB_to_litr4cB;
	
	/* 3.2 SINKS: management, senescence */
	Closs_THN = cf->leafc_storage_to_THN + cf->leafc_transfer_to_THN + cf->leafc_to_THN +
				cf->fruitc_storage_to_THN + cf->fruitc_transfer_to_THN + cf->fruitc_to_THN +
				cf->frootc_storage_to_THN + cf->frootc_transfer_to_THN + cf->frootc_to_THN +
				cf->livecrootc_storage_to_THN + cf->livecrootc_transfer_to_THN + cf->livecrootc_to_THN +
				cf->deadcrootc_storage_to_THN + cf->deadcrootc_transfer_to_THN + cf->deadcrootc_to_THN +
				cf->livestemc_storage_to_THN + cf->livestemc_transfer_to_THN + cf->livestemc_to_THN +
				cf->deadstemc_storage_to_THN + cf->deadstemc_transfer_to_THN + cf->deadstemc_to_THN +
				cf->gresp_transfer_to_THN + cf->gresp_storage_to_THN;


	Closs_MOW = cf->leafc_storage_to_MOW + cf->leafc_transfer_to_MOW + cf->leafc_to_MOW +
				cf->fruitc_storage_to_MOW + cf->fruitc_transfer_to_MOW + cf->fruitc_to_MOW +
				cf->softstemc_storage_to_MOW + cf->softstemc_transfer_to_MOW + cf->softstemc_to_MOW +
				cf->gresp_transfer_to_MOW + cf->gresp_storage_to_MOW;


	Closs_HRV = cf->leafc_storage_to_HRV + cf->leafc_transfer_to_HRV + cf->leafc_to_HRV +
				cf->fruitc_storage_to_HRV + cf->fruitc_transfer_to_HRV + cf->fruitc_to_HRV +
				cf->softstemc_storage_to_HRV + cf->softstemc_transfer_to_HRV + cf->softstemc_to_HRV +
		        cf->gresp_transfer_to_HRV + cf->gresp_storage_to_HRV;


	Closs_PLG = cf->leafc_storage_to_PLG - cf->leafc_transfer_to_PLG +  cf->leafc_to_PLG +
				cf->fruitc_storage_to_PLG + cf->fruitc_transfer_to_PLG + cf->fruitc_to_PLG + 
				cf->softstemc_storage_to_PLG - cf->softstemc_transfer_to_PLG - cf->softstemc_to_PLG +
				cf->frootc_storage_to_PLG + cf->frootc_transfer_to_PLG + cf->frootc_to_PLG + 
				cf->gresp_transfer_to_PLG + cf->gresp_storage_to_PLG;


	Closs_GRZ = cf->leafc_storage_to_GRZ + cf->leafc_transfer_to_GRZ + cf->leafc_to_GRZ +
				cf->fruitc_storage_to_GRZ + cf->fruitc_transfer_to_GRZ + cf->fruitc_to_GRZ + 
				cf->softstemc_storage_to_GRZ + cf->softstemc_transfer_to_GRZ + cf->softstemc_to_GRZ + 
				cf->gresp_transfer_to_GRZ + cf->gresp_storage_to_GRZ;

	Cplus_GRZ = cf->GRZ_to_litr1c + cf->GRZ_to_litr2c + cf->GRZ_to_litr3c + cf->GRZ_to_litr4c;

	Cplus_FRZ = cf->FRZ_to_litr1c + cf->FRZ_to_litr2c + cf->FRZ_to_litr3c + cf->FRZ_to_litr4c;

	Cplus_PLT = cf->leafc_transfer_from_PLT + cf->frootc_transfer_from_PLT + 
							cf->fruitc_transfer_from_PLT +
							cf->softstemc_transfer_from_PLT;

	summary->Closs_THN = Closs_THN;
	summary->Closs_MOW = Closs_MOW;
	summary->Closs_HRV = Closs_HRV;
	summary->Closs_PLG = Closs_PLG;
	summary->Closs_GRZ = Closs_GRZ;
	summary->Cplus_GRZ = Cplus_GRZ;
	summary->Cplus_PLT = Cplus_PLT;
	summary->Cplus_FRZ = Cplus_FRZ;
	summary->Cplus_PLT = Cplus_PLT;

	summary->daily_STDB_to_litr = daily_STDB_to_litr;
    summary->daily_CTDB_to_litr = daily_CTDB_to_litr;

	/* summerize senescence effect - Hidy 2008 */
	summary->Closs_SNSC = cf->m_leafc_storage_to_SNSC + cf->m_leafc_transfer_to_SNSC + cf->m_leafc_to_SNSC +
						  cf->m_fruitc_storage_to_SNSC + cf->m_fruitc_transfer_to_SNSC + cf->m_fruitc_to_SNSC +
						  cf->m_softstemc_storage_to_SNSC + cf->m_softstemc_transfer_to_SNSC + cf->m_softstemc_to_SNSC +
						  cf->m_frootc_storage_to_SNSC + cf->m_frootc_transfer_to_SNSC + cf->m_frootc_to_SNSC +
						  cf->m_gresp_transfer_to_SNSC + cf->m_gresp_storage_to_SNSC;

	disturb_loss = Closs_THN + Closs_MOW + Closs_HRV + Closs_PLG;
				
	disturb_gain = Cplus_FRZ + Cplus_GRZ + Cplus_PLT + daily_CTDB_to_litr + daily_STDB_to_litr;
	
	/* nbp is positive is it mean net carbon gain to the system and it is negative is net carbon loss */
	nbp = nep + disturb_gain - disturb_loss;
	summary->daily_nbp = nbp;

	/* main nitrogen information */
	summary->Nplus_GRZ = (nf->GRZ_to_litr1n + nf->GRZ_to_litr2n + nf->GRZ_to_litr3n  + nf->GRZ_to_litr4n);  
	summary->Nplus_FRZ = (nf->FRZ_to_sminn+nf->FRZ_to_litr1n + nf->FRZ_to_litr2n + nf->FRZ_to_litr3n  + nf->FRZ_to_litr4n);  
	
	for (layer = 0; layer < N_SOILLAYERS; layer++)
	{
		summary->daily_gross_nmin   += epv->daily_gross_nmin[layer];
		summary->daily_gross_nimmob += epv->daily_gross_nimmob[layer];
		summary->daily_net_nmin     += epv->daily_net_nmin[layer];
	}

	
	return(!ok);
}
