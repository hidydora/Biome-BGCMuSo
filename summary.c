/* 
summary.c
summary variables for potential output

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

int cnw_summary(const epconst_struct* epc, const siteconst_struct* sitec, const soilprop_struct* sprop, const metvar_struct* metv, 
	            const cstate_struct* cs, const cflux_struct* cf, const nstate_struct* ns, const nflux_struct* nf, const wflux_struct* wf, 
				epvar_struct* epv, summary_struct* summary)
{
	int errorCode=0;
	int layer;
	double GPP,MR,GR,HR,TR, AR, fire;
	double SR; /* calculating soil respiration */
	double NPP,NEP,NEE, NBP, disturb_loss, disturb_gain, BD_top30, BD_act, g_per_cm3_to_kg_per_m3, prop_to_percent;
	double Closs_THN_w, Closs_THN_nw, Closs_MOW, Closs_HRV, yieldC_HRV, Closs_PLG, Closs_PLT, Closs_GRZ, Cplus_PLT, Cplus_FRZ, Cplus_GRZ, Nplus_GRZ, Nplus_FRZ;
	double Closs_SNSC, STDB_to_litr, CTDB_to_litr;

	summary->leafCN = summary->frootCN = summary->yieldN = summary->softstemCN =0;


	/*******************************************************************************/
	/* 1. summarize meteorological and water variables */


	summary->annprcp += metv->prcp;
	summary->anntavg += metv->Tavg / nDAYS_OF_YEAR;

		
	summary->cumRunoff += wf->prcp_to_runoff + wf->pondw_to_runoff;
	summary->cumWleachRZ += wf->soilwLeach_RZ;	
	summary->cumNleachRZ += nf->sminN_leachRZ;

	/*******************************************************************************/
	/* 2. summarize carbon and nitrogen stocks */


	/*  biomass C (live+dead) without NSC */
	summary->LDaboveC_nw      = cs->leafc + cs->yieldc + cs->softstemc + 
		                        cs->STDBc_leaf  + cs->STDBc_yield + cs->STDBc_softstem;
	summary->LDaboveC_w       = cs->livestemc + cs->deadstemc;

	summary->LDbelowC_nw      = cs->frootc + cs->STDBc_froot;
	summary->LDbelowC_w       = cs->livecrootc + cs->deadcrootc;

	/* biomass C (live+dead) with NSC */
	summary->LDaboveCnsc_nw = cs->leafc + cs->yieldc + cs->softstemc + 
		                      cs->STDBc_above + 
		                      cs->leafc_transfer + cs->leafc_storage  + cs->frootc_storage    + cs->frootc_transfer +
						      cs->yieldc_storage  + cs->yieldc_transfer + cs->softstemc_storage + cs->softstemc_transfer +
		                      cs->gresp_storage  + cs->gresp_transfer;
	summary->LDaboveCnsc_w = cs->livestemc + cs->deadstemc +
		                     cs->livestemc_storage + cs->livestemc_transfer +
		                     cs->deadstemc_storage + cs->deadstemc_transfer;

	summary->LDbelowCnsc_nw = cs->frootc      + 
		                      cs->STDBc_below + 
		                      cs->frootc_storage + cs->frootc_transfer;

	summary->LDbelowCnsc_w = cs->livecrootc + cs->deadcrootc +
		                     cs->livecrootc_storage + cs->livecrootc_transfer +
		                     cs->deadcrootc_storage + cs->deadcrootc_transfer;


	/* living biomass C  */
	summary->LaboveC_nw = cs->leafc+cs->yieldc+cs->softstemc;
	summary->LaboveC_w  = cs->livestemc;

	summary->LbelowC_nw = cs->frootc;
	summary->LbelowC_w  = cs->deadcrootc;

	/* living biomass C with NSC */
	summary->LaboveCnsc_nw = cs->leafc + cs->yieldc + cs->softstemc + 
		                     cs->leafc_transfer + cs->leafc_storage   + 
						     cs->yieldc_storage + cs->yieldc_transfer +  cs->softstemc_storage + cs->softstemc_transfer +
		                     cs->gresp_storage + cs->gresp_transfer;
	summary->LaboveCnsc_w = cs->livestemc + cs->livestemc_storage + cs->livestemc_transfer;

	summary->LbelowCnsc_nw = cs->frootc  + cs->frootc_storage + cs->frootc_transfer;
	summary->LbelowCnsc_w = cs->deadstemc + cs->deadstemc_storage + cs->deadstemc_transfer;
	

	/* dead biomass C */
	summary->DaboveC_nw = cs->STDBc_leaf  + cs->STDBc_yield + cs->STDBc_softstem;
	summary->DaboveC_w = cs->deadstemc;

	summary->DbelowC_nw = cs->STDBc_froot;
	summary->DbelowC_w = cs->deadcrootc;

	/* dead biomass C with NSC  */
	summary->DaboveCnsc_nw = cs->STDBc_leaf  + cs->STDBc_yield + cs->STDBc_softstem;
	summary->DaboveCnsc_w = cs->deadstemc + cs->deadstemc_storage + cs->deadstemc_transfer;

	summary->DbelowCnsc_nw = cs->STDBc_froot;
	summary->DbelowCnsc_w = cs->deadstemc + cs->deadstemc_storage + cs->deadstemc_transfer;

	/* living and total, above- and belowground biomass (C+N) */
	summary->livingBIOMabove  = cs->leafc + cs->leafc_storage + cs->leafc_transfer + 
		                        cs->gresp_storage + cs->gresp_transfer + 
		                        cs->yieldc + cs->yieldc_storage + cs->yieldc_transfer +
		                        cs->softstemc + cs->softstemc_storage + cs->softstemc_transfer + 
								cs->livestemc + cs->livestemc_storage + cs->livestemc_transfer +
								cs->deadstemc + cs->deadstemc_storage + cs->deadstemc_transfer +
								ns->leafn + ns->leafn_storage + ns->leafn_transfer + 
		                        ns->retransn + 
		                        ns->yieldn + ns->yieldn_storage + ns->yieldn_transfer +
		                        ns->softstemn + ns->softstemn_storage + ns->softstemn_transfer + 
								ns->livestemn + ns->livestemn_storage + ns->livestemn_transfer +
								ns->deadstemn + ns->deadstemn_storage + ns->deadstemn_transfer;
	summary->livingBIOMbelow  = cs->frootc + cs->frootc_storage + cs->frootc_transfer +
	                            cs->livecrootc + cs->livecrootc_storage + cs->livecrootc_transfer +
							    cs->deadcrootc + cs->deadcrootc_storage + cs->deadcrootc_transfer +
							    ns->frootn + ns->frootn_storage + ns->frootn_transfer +
	                            ns->livecrootn + ns->livecrootn_storage + ns->livecrootn_transfer +
							    ns->deadcrootn + ns->deadcrootn_storage + ns->deadcrootn_transfer;
	summary->totalBIOMabove = summary->livingBIOMabove + cs->STDBc_above + ns->STDBn_above; 
		                      
	summary->totalBIOMbelow = summary->livingBIOMbelow + cs->STDBc_below + ns->STDBn_below; 

		
	if (summary->livingBIOMabove > summary->annmax_livingBIOMabove) summary->annmax_livingBIOMabove  = summary->livingBIOMabove;
	if (summary->livingBIOMbelow > summary->annmax_livingBIOMbelow) summary->annmax_livingBIOMbelow  = summary->livingBIOMbelow;
	if (summary->totalBIOMabove  > summary->annmax_totalBIOMabove)  summary->annmax_totalBIOMabove   = summary->totalBIOMabove;
	if (summary->totalBIOMbelow  > summary->annmax_totalBIOMbelow)  summary->annmax_totalBIOMbelow   = summary->totalBIOMbelow;

	summary->vegC = cs->leafc + cs->leafc_storage + cs->leafc_transfer + 
		            cs->frootc + cs->frootc_storage + cs->frootc_transfer +
					cs->yieldc + cs->yieldc_storage + cs->yieldc_transfer +
		            cs->softstemc + cs->softstemc_storage + cs->softstemc_transfer +
		            cs->livestemc + cs->livestemc_storage + cs->livestemc_transfer +
		            cs->deadstemc + cs->deadstemc_storage + cs->deadstemc_transfer +
		            cs->livecrootc + cs->livecrootc_storage + cs->livecrootc_transfer +
		            cs->deadcrootc + cs->deadcrootc_storage + cs->deadcrootc_transfer +
		            cs->gresp_storage + cs->gresp_transfer + 
					cs->STDBc_above + cs->STDBc_below;

			

	summary->leafc_LandD     = cs->leafc     + cs->STDBc_leaf;
	summary->frootc_LandD    = cs->frootc    + cs->STDBc_froot;
	summary->yield_LandD    = cs->yieldc    + cs->STDBc_yield;
	summary->softstemc_LandD = cs->softstemc + cs->STDBc_softstem;

	summary->leafDM     =  summary->leafc_LandD / epc->leafC_DM;
    summary->frootDM    =  summary->frootc_LandD / epc->frootC_DM;
	summary->yieldDM    =  summary->yield_LandD / epc->yield_DM;
    summary->softstemDM =  summary->softstemc_LandD / epc->softstemC_DM;


	if (ns->leafn + ns->STDBn_leaf != 0)         summary->leafCN     = (cs->leafc + cs->STDBc_leaf) / (ns->leafn + ns->STDBn_leaf);
	if (ns->frootn + ns->STDBn_froot != 0)       summary->frootCN    = (cs->frootc + cs->STDBc_froot) / (ns->frootn + ns->STDBn_froot);
	if (ns->yieldn + ns->STDBn_yield != 0)       summary->yieldN    = (cs->yieldc + cs->STDBc_yield) / (ns->yieldn + ns->STDBn_yield);
	if (ns->softstemn + ns->STDBn_softstem != 0) summary->softstemCN = (cs->softstemc + cs->STDBc_softstem) / (ns->softstemn + ns->STDBn_softstem);

	summary->yieldDM_HRV = cs->yieldC_HRV / epc->yield_DM;

	summary->leaflitrDM = (cs->litr1c_total + cs->litr2c_total + cs->litr3c_total + cs->litr4c_total) / epc->leaflitrC_DM;
    summary->livewoodDM = (cs->livestemc + cs->livecrootc) / epc->livewoodC_DM;
	summary->deadwoodDM = (cs->deadstemc + cs->deadcrootc) / epc->deadwoodC_DM;



	summary->litrC_total = cs->litr1c_total + cs->litr2c_total + cs->litr3c_total + cs->litr4c_total;
	summary->litrN_total = ns->litr1n_total + ns->litr2n_total + ns->litr3n_total + ns->litr4n_total;

	summary->litrCwdC_total = cs->litr1c_total + cs->litr2c_total + cs->litr3c_total + cs->litr4c_total + cs->cwdc_total;
	summary->litrCwdN_total = ns->litr1n_total + ns->litr2n_total + ns->litr3n_total + ns->litr4n_total + ns->cwdn_total;;
	
	summary->soilC_total = cs->soil1c_total + cs->soil2c_total + cs->soil3c_total + cs->soil4c_total;
	summary->soilN_total = ns->soil1n_total + ns->soil2n_total + ns->soil3n_total + ns->soil4n_total;
	
	if (summary->litrN_total) 
		summary->CNlitr_total = summary->litrC_total/summary->litrN_total;
	else
		summary->CNlitr_total = 0;

	if (summary->soilN_total) 
		summary->CNsoil_total = summary->soilC_total/summary->soilN_total;
	else
		summary->CNsoil_total = 0;

	summary->sminN_total = ns->sminNH4_total + ns->sminNO3_total;
	summary->sminNavail_total = ns->sminNH4_total * sprop->NH4_mobilen_prop + ns->sminNO3_total* NO3_mobilen_prop;
	
	summary->totalC      = summary->vegC + summary->litrC_total + summary->soilC_total + cs->cwdc_total;

	/* carbon and nitrogen content of top soil layer (10 cm layer depth):
	   kg (C or N)/m2 -> g (C or N) / kg soil: kgC/m2 = kgCN/0.1m3 = 10 * kgCN/m3 */

	g_per_cm3_to_kg_per_m3 = 1000;
	prop_to_percent = 100;
	
	summary->sminN_maxRZ = 0;
	summary->soilC_maxRZ = 0;
	summary->soilN_maxRZ = 0;
	summary->litrC_maxRZ = 0;
	summary->litrN_maxRZ = 0;
	summary->sminNavail_maxRZ = 0;

	for (layer = 0; layer < N_SOILLAYERS; layer++)
	{
		/* sminNH4: kgN/m2; BD: g/cm3 -> kg/m3: *10-3; ppm: *1000000 */
		summary->sminNH4_ppm[layer] = ns->sminNH4[layer] / (sprop->BD[layer] * g_per_cm3_to_kg_per_m3 * sitec->soillayer_thickness[layer]) * 1000000;
		summary->sminNO3_ppm[layer] = ns->sminNO3[layer] / (sprop->BD[layer] * g_per_cm3_to_kg_per_m3 * sitec->soillayer_thickness[layer]) * 1000000;
		if (layer < epv->n_maxrootlayers) 
		{
			summary->sminN_maxRZ += (ns->sminNH4[layer] + ns->sminNO3[layer]);
			summary->sminNavail_maxRZ += (ns->sminNH4avail[layer] + ns->sminNO3avail[layer]);
			summary->soilC_maxRZ += (cs->soilC[layer]);
			summary->soilN_maxRZ += (ns->soilN[layer]);
			summary->litrC_maxRZ += (cs->litrC[layer]);
			summary->litrN_maxRZ += (ns->litrN[layer]);
		}
		BD_act=sprop->BD[layer] * g_per_cm3_to_kg_per_m3 * sitec->soillayer_thickness[layer];
		summary->SOM_C[layer]=(cs->soil1c[layer] + cs->soil2c[layer] + cs->soil3c[layer] + cs->soil4c[layer]) / BD_act * prop_to_percent;
	}

	/* g/cm3 to kg/m2 */
	BD_top30 = (sitec->soillayer_thickness[0] * sprop->BD[0] + 
		        sitec->soillayer_thickness[1] * sprop->BD[1] +  
				sitec->soillayer_thickness[2] * sprop->BD[2]) * g_per_cm3_to_kg_per_m3;

	summary->stableSOC_top30 = (cs->soil4c[0] + cs->soil4c[1] + cs->soil4c[2]) / BD_top30 * prop_to_percent;

	summary->SOM_C_top30 = (cs->soil1c[0] + cs->soil1c[1] + cs->soil1c[2] +
						    cs->soil2c[0] + cs->soil2c[1] + cs->soil2c[2] +
						    cs->soil3c[0] + cs->soil3c[1] + cs->soil3c[2] +
						    cs->soil4c[0] + cs->soil4c[1] + cs->soil4c[2]) / BD_top30 * prop_to_percent;

	summary->SOM_N_top30 = (ns->soil1n[0] + ns->soil1n[1] + ns->soil1n[2] +
							ns->soil2n[0] + ns->soil2n[1] + ns->soil2n[2] +
							ns->soil3n[0] + ns->soil3n[1] + ns->soil3n[2] +
							ns->soil4n[0] + ns->soil4n[1] + ns->soil4n[2]) / BD_top30 * prop_to_percent;


	summary->NH4_top30avail = (summary->sminNH4_ppm[0] * sitec->soillayer_thickness[0]/sitec->soillayer_depth[2] + 
		                       summary->sminNH4_ppm[1] * sitec->soillayer_thickness[1]/sitec->soillayer_depth[2] + 
							   summary->sminNH4_ppm[2] * sitec->soillayer_thickness[2]/sitec->soillayer_depth[2]) * sprop->NH4_mobilen_prop;

	summary->NO3_top30avail = (summary->sminNO3_ppm[0] * sitec->soillayer_thickness[0]/sitec->soillayer_depth[2] + 
		                       summary->sminNO3_ppm[1] * sitec->soillayer_thickness[1]/sitec->soillayer_depth[2] + 
							   summary->sminNO3_ppm[2] * sitec->soillayer_thickness[2]/sitec->soillayer_depth[2]) * NO3_mobilen_prop;

	summary->sminN_top30avail = summary->NO3_top30avail + summary->NH4_top30avail;

	summary->N2Oflux    = nf->N2O_flux_NITRIF_total + nf->N2O_flux_DENITR_total + nf->N2O_flux_GRZ + nf->N2O_flux_FRZ;
	summary->N2OfluxCeq = summary->N2Oflux * 298 * (12./44.);

	summary->CH4flux_total = cf->CH4flux_animal + cf->CH4flux_manure + cf->CH4flux_soil;

	/*******************************************************************************/
	/* 3. calculate daily fluxes (GPP, NPP, NEP, MR, GR, HR) positive for net growth: NPP = gross PSN - Maintenance Resp - growth Resp */

	GPP = cf->psnsun_to_cpool + cf->psnshade_to_cpool;
	
	MR = cf->leaf_day_MR + cf->leaf_night_MR + 
		 cf->froot_MR + cf->livestem_MR + cf->livecroot_MR +
		 /* yield simulation */
		 cf->yield_MR +
		 /* softstem simulation */
		 cf->softstem_MR;

	GR = cf->cpool_leaf_GR + cf->cpool_leaf_storage_GR + cf->transfer_leaf_GR +
		cf->cpool_froot_GR + cf->cpool_froot_storage_GR + cf->transfer_froot_GR + 
		cf->cpool_livestem_GR + cf->cpool_livestem_storage_GR + cf->transfer_livestem_GR +
		cf->cpool_deadstem_GR + cf->cpool_deadstem_storage_GR + cf->transfer_deadstem_GR + 
		cf->cpool_livecroot_GR + cf->cpool_livecroot_storage_GR + cf->transfer_livecroot_GR + 
		cf->cpool_deadcroot_GR + cf->cpool_deadcroot_storage_GR + cf->transfer_deadcroot_GR +
		/* yield simulation */
		cf->cpool_yield_GR + cf->cpool_yieldc_storage_GR + cf->transfer_yield_GR +
		/* softstem simulation */
		cf->cpool_softstem_GR + cf->cpool_softstem_storage_GR + cf->transfer_softstem_GR;

	NPP = GPP - MR - GR;

	/* heterotrophic respiration  */
	HR=0;
	summary->litr1HR_total=0;
	summary->litr2HR_total=0;
	summary->litr4HR_total=0;
	summary->soil1HR_total=0;
	summary->soil2HR_total=0;
	summary->soil3HR_total=0;
	summary->soil4HR_total=0;

	for (layer = 0; layer < N_SOILLAYERS; layer++)
	{
		summary->litr1HR_total += cf->litr1_hr[layer];
		summary->litr2HR_total += cf->litr2_hr[layer];
		summary->litr4HR_total += cf->litr4_hr[layer];
		summary->soil1HR_total += cf->soil1_hr[layer];
		summary->soil2HR_total += cf->soil2_hr[layer];
		summary->soil3HR_total += cf->soil3_hr[layer];
		summary->soil4HR_total += cf->soil4_hr[layer];

		HR += cf->litr1_hr[layer] + cf->litr2_hr[layer] + cf->litr4_hr[layer] + 
			  cf->soil1_hr[layer] + cf->soil2_hr[layer] + cf->soil3_hr[layer] + cf->soil4_hr[layer];
	}

	TR = MR + GR + HR;

	AR = MR + GR;
	
	NEP = NPP - HR;
	
	/* soil respiration */
	SR = cf->froot_MR + cf->livecroot_MR +
		 cf->cpool_froot_GR + cf->cpool_froot_storage_GR + cf->transfer_froot_GR + 
		 cf->cpool_livecroot_GR + cf->cpool_livecroot_storage_GR + cf->transfer_livecroot_GR + 
		 cf->cpool_deadcroot_GR + cf->cpool_deadcroot_storage_GR + cf->transfer_deadcroot_GR +
		 HR;
		
	
	/* calculate daily NEE, positive for net sink: NEE = NEP - fire losses */
	fire =  cf->m_leafc_to_fire +  cf->m_leafc_storage_to_fire + cf->m_leafc_transfer_to_fire +
			cf->m_frootc_to_fire + cf->m_frootc_storage_to_fire + cf->m_frootc_transfer_to_fire +  
			cf->m_livestemc_to_fire + cf->m_livestemc_storage_to_fire + 	cf->m_livestemc_transfer_to_fire + 
			cf->m_deadstemc_to_fire + cf->m_deadstemc_storage_to_fire + cf->m_deadstemc_transfer_to_fire + 
			cf->m_livecrootc_to_fire + cf->m_livecrootc_storage_to_fire + cf->m_livecrootc_transfer_to_fire +
			cf->m_deadcrootc_to_fire + cf->m_deadcrootc_storage_to_fire + cf->m_deadcrootc_transfer_to_fire + 
			cf->m_gresp_storage_to_fire + cf->m_gresp_transfer_to_fire + 
			cf->m_litr1c_to_fire_total + cf->m_litr2c_to_fire_total + cf->m_litr3c_to_fire_total + cf->m_litr4c_to_fire_total +
			cf->m_cwdc_to_fire_total +
			/* yield simulation */
			cf->m_yieldc_to_fire +  cf->m_yieldc_storage_to_fire + cf->m_yieldc_transfer_to_fire +
			/* softstem simulation */
			cf->m_softstemc_to_fire +  cf->m_softstemc_storage_to_fire + cf->m_softstemc_transfer_to_fire;
	/* NEE is positive if ecosystem is net source and negative if it is net sink */
	NEE = -1* (NEP - fire);
	

	summary->NEP = NEP;
	summary->NPP = NPP;
	summary->NEE = NEE;	
	summary->GPP = GPP;
	summary->MR = MR;
	summary->GR = GR;
	summary->HR = HR;
	summary->SR = SR;
	summary->TR = TR;
	summary->fire = fire;
	summary->cumNPP += NPP;
	summary->cumNEP += NEP;
	summary->cumNEE += NEE;
	summary->cumGPP += GPP;
	summary->cumMR += MR;
	summary->cumGR += GR;
	summary->cumHR += HR;
	summary->cumAR += AR;
	summary->cumTR += (MR+GR+HR);
	summary->cumSR += SR;


	summary->cumN2Oflux    += summary->N2Oflux;
	summary->cumN2OfluxCeq += summary->N2OfluxCeq;
	summary->cumCH4flux    += summary->CH4flux_total;
	summary->cumEVP       += (wf->canopywEVP + wf->soilwEVP);
	summary->cumTRP     += wf->soilwTRP_SUM;
	summary->cumET         += wf->ET;
	
	/*******************************************************************************/
	/* 4. calculation litter fluxes and pools */

	summary->litter = cs->litr1c_total + cs->litr2c_total + cs->litr3c_total + cs->litr4c_total;
	
	summary->litdecomp = 0;
	for (layer = 0; layer < N_SOILLAYERS; layer++)
	{
		summary->litdecomp += cf->litr1c_to_soil1c[layer] + cf->litr2c_to_soil2c[layer]  + cf->litr4c_to_soil3c[layer];
	}

	summary->litfire = cf->m_litr1c_to_fire_total + cf->m_litr2c_to_fire_total + cf->m_litr3c_to_fire_total + cf->m_litr4c_to_fire_total;
	
	/* aboveground litter */
	summary->litfallc_above = 
		cf->m_leafc_to_litr1c + cf->m_leafc_to_litr2c + cf->m_leafc_to_litr3c + cf->m_leafc_to_litr4c + 
		cf->m_livestemc_to_cwdc + cf->m_deadstemc_to_cwdc + cf->m_livecrootc_to_cwdc + cf->m_deadcrootc_to_cwdc +
		cf->leafc_to_litr1c + cf->leafc_to_litr2c + cf->leafc_to_litr3c + cf->leafc_to_litr4c + 
		cf->softstemc_to_litr1c + cf->softstemc_to_litr2c + cf->softstemc_to_litr3c + cf->softstemc_to_litr4c + 
		cf->m_softstemc_to_litr1c + cf->m_softstemc_to_litr2c + cf->m_softstemc_to_litr3c + cf->m_softstemc_to_litr4c + 
		cf->yieldc_to_litr1c + cf->yieldc_to_litr2c + cf->yieldc_to_litr3c + cf->yieldc_to_litr4c + 
		cf->m_yieldc_to_litr1c + cf->m_yieldc_to_litr2c + cf->m_yieldc_to_litr3c + cf->m_yieldc_to_litr4c;
	
	/* belowground */
	summary->litfallc_below = 
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
		cf->m_yieldc_storage_to_litr1c + cf->m_yieldc_transfer_to_litr1c;


	summary->litfallc = summary->litfallc_above + summary->litfallc_below;

	/*******************************************************************************/
	/* 5. calculation of disturbance and senescence effect  */

	/* 5.1 cut-down biomass and standing dead biome */
	STDB_to_litr = cf->STDBc_to_litr;
	CTDB_to_litr = cf->CTDBc_to_litr;

	summary->cumCplus_STDB += STDB_to_litr;
	summary->cumCplus_CTDB += CTDB_to_litr;

	/* 5.2 management */
	Closs_THN_w = cf->livestemc_storage_to_THN + cf->livestemc_transfer_to_THN + cf->livestemc_to_THN +
				  cf->deadstemc_storage_to_THN + cf->deadstemc_transfer_to_THN + cf->deadstemc_to_THN +
				  cf->livecrootc_storage_to_THN + cf->livecrootc_transfer_to_THN + cf->livecrootc_to_THN +
				  cf->deadcrootc_storage_to_THN + cf->deadcrootc_transfer_to_THN + cf->deadcrootc_to_THN;

	Closs_THN_nw = cf->leafc_storage_to_THN + cf->leafc_transfer_to_THN + cf->leafc_to_THN +
		           cf->frootc_storage_to_THN + cf->frootc_transfer_to_THN + cf->frootc_to_THN +
				   cf->yieldc_storage_to_THN + cf->yieldc_transfer_to_THN + cf->yieldc_to_THN +
				   cf->gresp_transfer_to_THN + cf->gresp_storage_to_THN;


	Closs_MOW = cf->leafc_storage_to_MOW + cf->leafc_transfer_to_MOW + cf->leafc_to_MOW +
				cf->yieldc_storage_to_MOW + cf->yieldc_transfer_to_MOW + cf->yieldc_to_MOW +
				cf->softstemc_storage_to_MOW + cf->softstemc_transfer_to_MOW + cf->softstemc_to_MOW +
				cf->gresp_transfer_to_MOW + cf->gresp_storage_to_MOW;


	Closs_HRV = cf->leafc_storage_to_HRV + cf->leafc_transfer_to_HRV + cf->leafc_to_HRV +
				cf->yieldc_storage_to_HRV + cf->yieldc_transfer_to_HRV + cf->yieldc_to_HRV +
				cf->softstemc_storage_to_HRV + cf->softstemc_transfer_to_HRV + cf->softstemc_to_HRV +
		        cf->gresp_transfer_to_HRV + cf->gresp_storage_to_HRV;

	yieldC_HRV = cf->yieldc_to_HRV;


	Closs_PLG = cf->leafc_storage_to_PLG - cf->leafc_transfer_to_PLG +  cf->leafc_to_PLG +
				cf->yieldc_storage_to_PLG + cf->yieldc_transfer_to_PLG + cf->yieldc_to_PLG + 
				cf->softstemc_storage_to_PLG - cf->softstemc_transfer_to_PLG - cf->softstemc_to_PLG +
				cf->frootc_storage_to_PLG + cf->frootc_transfer_to_PLG + cf->frootc_to_PLG + 
				cf->gresp_transfer_to_PLG + cf->gresp_storage_to_PLG;


	Closs_GRZ = cf->leafc_storage_to_GRZ + cf->leafc_transfer_to_GRZ + cf->leafc_to_GRZ +
				cf->yieldc_storage_to_GRZ + cf->yieldc_transfer_to_GRZ + cf->yieldc_to_GRZ + 
				cf->softstemc_storage_to_GRZ + cf->softstemc_transfer_to_GRZ + cf->softstemc_to_GRZ + 
				cf->gresp_transfer_to_GRZ + cf->gresp_storage_to_GRZ;

	Cplus_GRZ = cf->GRZ_to_litr1c + cf->GRZ_to_litr2c + cf->GRZ_to_litr3c + cf->GRZ_to_litr4c;

	Cplus_FRZ = cf->FRZ_to_litr1c + cf->FRZ_to_litr2c + cf->FRZ_to_litr3c + cf->FRZ_to_litr4c;

	Cplus_PLT = cf->leafc_transfer_from_PLT + cf->frootc_transfer_from_PLT + 
							cf->yieldc_transfer_from_PLT +
							cf->softstemc_transfer_from_PLT;

	Closs_PLT = cf->STDBc_leaf_to_PLT + cf->STDBc_froot_to_PLT + cf->STDBc_yield_to_PLT + cf->STDBc_softstem_to_PLT;	

	Nplus_GRZ = (nf->GRZ_to_litr1n  + nf->GRZ_to_litr2n  + nf->GRZ_to_litr3n  + nf->GRZ_to_litr4n);  
	Nplus_FRZ = (nf->FRZ_to_sminNH4 + nf->FRZ_to_sminNO3) +
		         nf->FRZ_to_litr1n + nf->FRZ_to_litr2n + nf->FRZ_to_litr3n + nf->FRZ_to_litr4n;



	summary->cumCloss_THN_w  += Closs_THN_w;
	summary->cumCloss_THN_nw += Closs_THN_nw;
	summary->cumCloss_MOW    += Closs_MOW;
	summary->cumCloss_HRV    += Closs_HRV;
	summary->cumYieldC_HRV   += yieldC_HRV;
	summary->cumCloss_PLG    += Closs_PLG;
	summary->cumCloss_GRZ    += Closs_GRZ;
	summary->cumCplus_GRZ    += Cplus_GRZ;
	summary->cumCplus_PLT    += Cplus_PLT;
	summary->cumCloss_PLT    += Closs_PLT;
	summary->cumCplus_FRZ    += Cplus_FRZ;	
	summary->cumNplus_GRZ    += Nplus_GRZ;  
	summary->cumNplus_FRZ    += Nplus_FRZ;  

	if (cs->vegC_HRV)
	{
		summary->harvestIndex = cs->yieldC_HRV/cs->vegC_HRV;
		summary->rootIndex    = cs->frootC_HRV/(cs->frootC_HRV+cs->vegC_HRV);

	}
	else
	{
		summary->harvestIndex = 0;
		summary->rootIndex    = 0;
	}

	if (epc->woody)
	{
		if (summary->frootc_LandD + summary->leafc_LandD + summary->yield_LandD + cs->livecrootc + cs->deadcrootc + cs->livestemc + cs->deadstemc)
			summary->belowground_ratio = (summary->frootc_LandD + cs->livecrootc + cs->deadcrootc) / (summary->frootc_LandD + summary->leafc_LandD + summary->yield_LandD + cs->livecrootc + cs->deadcrootc + cs->livestemc + cs->deadstemc);
		else
			summary->belowground_ratio = 0;
	}
	else
	{
		if (summary->frootc_LandD + summary->leafc_LandD + summary->yield_LandD + summary->softstemc_LandD)
			summary->belowground_ratio = summary->frootc_LandD / (summary->frootc_LandD + summary->leafc_LandD + summary->yield_LandD + summary->softstemc_LandD);
		else
			summary->belowground_ratio = 0;
	}

	/* senescence effect  */
	Closs_SNSC = cf->m_leafc_storage_to_SNSC + cf->m_leafc_transfer_to_SNSC + cf->m_leafc_to_SNSC +
				 cf->m_yieldc_storage_to_SNSC + cf->m_yieldc_transfer_to_SNSC + cf->m_yieldc_to_SNSC +
				 cf->m_softstemc_storage_to_SNSC + cf->m_softstemc_transfer_to_SNSC + cf->m_softstemc_to_SNSC +
				 cf->m_frootc_storage_to_SNSC + cf->m_frootc_transfer_to_SNSC + cf->m_frootc_to_SNSC +
				 cf->m_gresp_transfer_to_SNSC + cf->m_gresp_storage_to_SNSC +
				 cf->HRV_frootc_to_SNSC + cf->HRV_softstemc_to_SNSC + cf->HRV_frootc_storage_to_SNSC +cf->HRV_frootc_transfer_to_SNSC +
				 cf->HRV_softstemc_storage_to_SNSC + cf->HRV_softstemc_transfer_to_SNSC + cf->HRV_gresp_storage_to_SNSC + cf->HRV_gresp_transfer_to_SNSC;

	summary->cumCloss_SNSC += Closs_SNSC ;

	/* NBP calculation: positive - mean net carbon gain to the system and negative - mean net carbon loss */
	disturb_loss = Closs_THN_w + Closs_THN_nw + Closs_MOW + Closs_HRV + Closs_PLG  + Closs_GRZ;		
	disturb_gain = Cplus_FRZ + Cplus_GRZ + Cplus_PLT;

	NBP = NEP + disturb_gain - disturb_loss;
	summary->NBP = NBP;
	summary->cumNBP += summary->NBP;

	/* lateral flux calculation */
	summary->cumCloss_MGM += disturb_loss;
	summary->cumCplus_MGM += disturb_gain;
	summary->Cflux_lateral = disturb_loss - disturb_gain;
	summary->cumCflux_lateral += summary->Cflux_lateral;

	/* NGB calculation: net greenhouse gas balance - NBP - N2O(Ceq) -CH(Ceq) */
	summary->NGB = summary->NBP - summary->N2OfluxCeq - summary->CH4flux_total;
	summary->cumNGB += summary->NGB;
	

	return(errorCode);
}
