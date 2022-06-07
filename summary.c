/* 
summary.c
summary variables for potential output

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
BBGC MuSo 2.3
Copyright 2000, Peter E. Thornton
Copyright 2014, D. Hidy
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

int csummary(cflux_struct* cf, cstate_struct* cs, nflux_struct* nf, summary_struct* summary)
{
	int ok=1;
	double gpp,mr,gr,hr,tr, fire;
	double sr; /* Hidy 2012 - calculating soil respiration */
	double npp,nep,nee, nbp, disturb_loss, disturb_surplus;

	/* summarize carbon stocks */
	summary->vegc = cs->leafc + cs->leafc_storage + cs->leafc_transfer + 
		cs->frootc + cs->frootc_storage + cs->frootc_transfer +
		cs->livestemc + cs->livestemc_storage + cs->livestemc_transfer +
		cs->deadstemc + cs->deadstemc_storage + cs->deadstemc_transfer +
		cs->livecrootc + cs->livecrootc_storage + cs->livecrootc_transfer +
		cs->deadcrootc + cs->deadcrootc_storage + cs->deadcrootc_transfer +
		cs->gresp_storage + cs->gresp_transfer + 
		/* fruit simulation - Hidy 2013. */
		cs->fruitc + cs->fruitc_storage + cs->fruitc_transfer;
	summary->litrc = cs->cwdc + cs->litr1c + cs->litr2c + cs->litr3c + 
		cs->litr4c;
	summary->soilc = cs->soil1c + cs->soil2c + cs->soil3c + cs->soil4c;
	summary->totalc = summary->vegc + summary->litrc + summary->soilc;

	
	/* calculate daily NPP, positive for net growth */
	/* NPP = Gross PSN - Maintenance Resp - Growth Resp */
	gpp = cf->psnsun_to_cpool + cf->psnshade_to_cpool;
	
	mr = cf->leaf_day_mr + cf->leaf_night_mr + 
		 cf->froot_mr + cf->livestem_mr + cf->livecroot_mr +
		 /* fruit simulation - Hidy 2013. */
		 cf->fruit_mr;

	gr = cf->cpool_leaf_gr + cf->cpool_leaf_storage_gr + cf->transfer_leaf_gr +
		cf->cpool_froot_gr + cf->cpool_froot_storage_gr + cf->transfer_froot_gr + 
		cf->cpool_livestem_gr + cf->cpool_livestem_storage_gr + cf->transfer_livestem_gr +
		cf->cpool_deadstem_gr + cf->cpool_deadstem_storage_gr + cf->transfer_deadstem_gr + 
		cf->cpool_livecroot_gr + cf->cpool_livecroot_storage_gr + cf->transfer_livecroot_gr + 
		cf->cpool_deadcroot_gr + cf->cpool_deadcroot_storage_gr + cf->transfer_deadcroot_gr +
		/* fruit simulation - Hidy 2013. */
		cf->cpool_fruit_gr + cf->cpool_fruit_storage_gr + cf->transfer_fruit_gr;

	npp = gpp - mr - gr;

	
	/* calculate daily NEP, positive for net sink */
	/* NEP = NPP - Autotrophic Resp */
	hr = cf->litr1_hr + cf->litr2_hr + cf->litr4_hr + cf->soil1_hr +
		cf->soil2_hr + cf->soil3_hr + cf->soil4_hr;

	tr = mr + gr + hr;
	
	nep = npp - hr;
	
	/* Hidy 2012 - calculating soil respiration */
	sr = cf->froot_mr + cf->livecroot_mr +
		 cf->cpool_froot_gr + cf->cpool_froot_storage_gr + cf->transfer_froot_gr + 
		 cf->cpool_livecroot_gr + cf->cpool_livecroot_storage_gr + cf->transfer_livecroot_gr + 
		 cf->cpool_deadcroot_gr + cf->cpool_deadcroot_storage_gr + cf->transfer_deadcroot_gr +
		 hr;
		
	
	/* calculate daily NEE, positive for net sink */
	/* NEE = NEP - fire losses */
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
			cf->m_fruitc_to_fire +  cf->m_fruitc_storage_to_fire + cf->m_fruitc_transfer_to_fire;
	nee = nep - fire;
	
	summary->daily_npp = npp;
	summary->daily_nee = -1 * nee;	// Hidy: NEE is positive if ecosystem is net source and negative if it is net sink
	summary->daily_gpp = gpp;
	summary->daily_mr = mr;
	summary->daily_gr = gr;
	summary->daily_hr = hr;
	summary->daily_sr = sr;
	summary->daily_tr = tr;
	summary->daily_fire = fire;
	summary->cum_npp += npp;
	summary->cum_nep += nep;
	summary->cum_nee += nee;
	summary->cum_gpp += gpp;
	summary->cum_mr += mr;
	summary->cum_gr += gr;
	summary->cum_hr += hr;
	summary->cum_fire += fire;
	
	/* other flux summary variables */
	summary->daily_litfallc = 
		cf->m_leafc_to_litr1c + cf->m_leafc_to_litr2c + cf->m_leafc_to_litr3c + cf->m_leafc_to_litr4c + 
		cf->m_frootc_to_litr1c + cf->m_frootc_to_litr2c + cf->m_frootc_to_litr3c + cf->m_frootc_to_litr4c +
		cf->m_leafc_storage_to_litr1c + cf->m_frootc_storage_to_litr1c +
		cf->m_leafc_transfer_to_litr1c + cf->m_frootc_transfer_to_litr1c +
		cf->m_livestemc_storage_to_litr1c + cf->m_deadstemc_storage_to_litr1c +
		cf->m_livestemc_transfer_to_litr1c + cf->m_deadstemc_transfer_to_litr1c +
		cf->m_livecrootc_storage_to_litr1c + cf->m_deadcrootc_storage_to_litr1c +
		cf->m_livecrootc_transfer_to_litr1c + cf->m_deadcrootc_transfer_to_litr1c +
		cf->m_livestemc_to_cwdc + cf->m_deadstemc_to_cwdc + cf->m_livecrootc_to_cwdc + cf->m_deadcrootc_to_cwdc +
		cf->m_gresp_storage_to_litr1c + cf->m_gresp_transfer_to_litr1c +
		cf->leafc_to_litr1c + cf->leafc_to_litr2c + cf->leafc_to_litr3c + cf->leafc_to_litr4c + 
		cf->frootc_to_litr1c + cf->frootc_to_litr2c + cf->frootc_to_litr3c + cf->frootc_to_litr4c +
		/* fruit simulation - Hidy 2013. */
		cf->m_fruitc_to_litr1c + cf->m_fruitc_to_litr2c + cf->m_fruitc_to_litr3c + cf->m_fruitc_to_litr4c + 
		cf->m_fruitc_storage_to_litr1c + cf->m_fruitc_transfer_to_litr1c;
		
	
	/* ditrurbance is positive is it mean net carbon surplus to the system and it is negative is net carbon loss */
	/* summerize thinning effect - Hidy 2012 */
	summary->Cchange_THN =  cf->THN_to_litr1c + cf->THN_to_litr2c + cf->THN_to_litr3c + cf->THN_to_litr4c + cf->THN_to_cwdc - 
								 cf->leafc_storage_to_THN - cf->leafc_transfer_to_THN - cf->leafc_to_THN -
								 /* fruit simulation - Hidy 2013. */
								 cf->fruitc_storage_to_THN - cf->fruitc_transfer_to_THN - cf->fruitc_to_THN -
								 cf->frootc_storage_to_THN - cf->frootc_transfer_to_THN - cf->frootc_to_THN -
								 cf->livecrootc_storage_to_THN - cf->livecrootc_transfer_to_THN - cf->livecrootc_to_THN -
								 cf->deadcrootc_storage_to_THN - cf->deadcrootc_transfer_to_THN - cf->deadcrootc_to_THN -
								 cf->livestemc_storage_to_THN - cf->livestemc_transfer_to_THN - cf->livestemc_to_THN -
								 cf->deadstemc_storage_to_THN - cf->deadstemc_transfer_to_THN - cf->deadstemc_to_THN -
								 cf->gresp_transfer_to_THN - cf->gresp_storage_to_THN;

	/* summerize mowing effect - Hidy 2008 */
	summary->Cchange_MOW =  cf->MOW_to_litr1c + cf->MOW_to_litr2c + cf->MOW_to_litr3c + cf->MOW_to_litr4c - 
								 cf->leafc_storage_to_MOW - cf->leafc_transfer_to_MOW - cf->leafc_to_MOW -
								 /* fruit simulation - Hidy 2013. */
								 cf->fruitc_storage_to_MOW - cf->fruitc_transfer_to_MOW - cf->fruitc_to_MOW -
								 cf->gresp_transfer_to_MOW - cf->gresp_storage_to_MOW;

	/* summerize harvesting effect - Hidy 2008 */
	summary->Cchange_HRV = - cf->leafc_storage_to_HRV - cf->leafc_transfer_to_HRV - cf->leafc_to_HRV -
							/* fruit simulation - Hidy 2013. */
							 cf->fruitc_storage_to_HRV - cf->fruitc_transfer_to_HRV - cf->fruitc_to_HRV -
		                     cf->gresp_transfer_to_HRV - cf->gresp_storage_to_HRV;

	/* summerize ploughing effect - Hidy 2008 */
	summary->Cchange_PLG = -cf->leafc_storage_to_PLG - cf->leafc_transfer_to_PLG - cf->leafc_to_PLG -
							/* fruit simulation - Hidy 2013. */
							cf->fruitc_storage_to_PLG - cf->fruitc_transfer_to_PLG - cf->fruitc_to_PLG -
							cf->frootc_storage_to_PLG - cf->frootc_transfer_to_PLG - cf->frootc_to_PLG - 
							cf->gresp_transfer_to_PLG - cf->gresp_storage_to_PLG;

	/* summerize grazing effect - Hidy 2009 */
	summary->Cchange_GRZ =  cf->GRZ_to_litr1c + cf->GRZ_to_litr2c + cf->GRZ_to_litr3c + cf->GRZ_to_litr4c - 
								cf->leafc_storage_to_GRZ - cf->leafc_transfer_to_GRZ - cf->leafc_to_GRZ - 
								/* fruit simulation - Hidy 2013. */
								cf->fruitc_storage_to_GRZ - cf->fruitc_transfer_to_GRZ - cf->fruitc_to_GRZ - 
								cf->gresp_transfer_to_GRZ - cf->gresp_storage_to_GRZ;


	summary->Cchange_FRZ = cf->FRZ_to_litr1c + cf->FRZ_to_litr2c + cf->FRZ_to_litr3c + cf->FRZ_to_litr4c;

	summary->Cchange_PLT = cf->leafc_transfer_from_PLT + cf->frootc_transfer_from_PLT + 
							/* fruit simulation - Hidy 2013. */
							cf->fruitc_transfer_from_PLT;

	/* summerize senescence effect - Hidy 2008 */
	summary->Cchange_SNSC =  cf->SNSC_to_litr1c + cf->SNSC_to_litr2c + cf->SNSC_to_litr3c + cf->SNSC_to_litr4c - 
								 cf->m_leafc_storage_to_SNSC - cf->m_leafc_transfer_to_SNSC - cf->m_leafc_to_SNSC -
								 /* fruit simulation - Hidy 2013. */
								 cf->m_fruitc_storage_to_SNSC - cf->m_fruitc_transfer_to_SNSC - cf->m_fruitc_to_SNSC -
								 cf->m_frootc_storage_to_SNSC - cf->m_frootc_transfer_to_SNSC - cf->m_frootc_to_SNSC -
								 cf->m_gresp_transfer_to_SNSC - cf->m_gresp_storage_to_SNSC;

	disturb_loss = cf->leafc_storage_to_MOW + cf->leafc_transfer_to_MOW + cf->leafc_to_MOW +  cf->gresp_transfer_to_MOW + cf->gresp_storage_to_MOW +
				   cf->leafc_storage_to_HRV + cf->leafc_transfer_to_HRV + cf->leafc_to_HRV + cf->gresp_transfer_to_HRV + cf->gresp_storage_to_HRV +
				   cf->leafc_storage_to_PLG + cf->leafc_transfer_to_PLG + cf->leafc_to_PLG + cf->gresp_transfer_to_PLG + cf->gresp_storage_to_PLG +
				   cf->leafc_storage_to_GRZ + cf->leafc_transfer_to_GRZ + cf->leafc_to_GRZ + cf->gresp_transfer_to_GRZ + cf->gresp_storage_to_GRZ +
				   cf->frootc_to_PLG + cf->frootc_storage_to_PLG + cf->frootc_transfer_to_PLG + 
				     /* fruit simulation - Hidy 2013. */
				   cf->fruitc_storage_to_MOW + cf->fruitc_transfer_to_MOW + cf->fruitc_to_MOW + 
				   cf->fruitc_storage_to_HRV + cf->fruitc_transfer_to_HRV + cf->fruitc_to_HRV + 
				   cf->fruitc_storage_to_PLG + cf->fruitc_transfer_to_PLG + cf->fruitc_to_PLG + 
				   cf->fruitc_storage_to_GRZ + cf->fruitc_transfer_to_GRZ + cf->fruitc_to_GRZ;
				
	disturb_surplus = cf->MOW_to_litr1c + cf->MOW_to_litr2c + cf->MOW_to_litr3c + cf->MOW_to_litr4c + 
					  cf->GRZ_to_litr1c + cf->GRZ_to_litr2c + cf->GRZ_to_litr3c + cf->GRZ_to_litr4c + 
					  cf->FRZ_to_litr1c + cf->FRZ_to_litr2c + cf->FRZ_to_litr3c + cf->FRZ_to_litr4c + 
                      cf->leafc_transfer_from_PLT + cf->frootc_transfer_from_PLT + cf->frootc_transfer_from_PLT;
	
	/* nbp is positive is it mean net carbon surplus to the system and it is negative is net carbon loss */
	nbp = nep + disturb_surplus - disturb_loss;
	summary->daily_nbp = nbp;

	/* main nitrogen information */
	summary->Nplus_GRZ = (nf->GRZ_to_litr1n + nf->GRZ_to_litr2n + nf->GRZ_to_litr3n  + nf->GRZ_to_litr4n);  
	summary->Nplus_FRZ = (nf->FRZ_to_sminn+nf->FRZ_to_litr1n + nf->FRZ_to_litr2n + nf->FRZ_to_litr3n  + nf->FRZ_to_litr4n);  
	

	
	return(!ok);
}
