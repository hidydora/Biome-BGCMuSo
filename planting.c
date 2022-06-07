/* 
planting.c
planting  - planting seeds in soil - increase transfer pools

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Biome-BGCMuSo v6.0.
Copyright 2019, D. Hidy [dori.hidy@gmail.com]
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
#include "pointbgc_struct.h"
#include "bgc_struct.h"
#include "pointbgc_func.h"
#include "bgc_constants.h"

int planting(control_struct* ctrl, const siteconst_struct* sitec, const planting_struct* PLT, epconst_struct* epc, 
	         epvar_struct* epv, phenology_struct* phen, cstate_struct* cs, nstate_struct*ns, cflux_struct* cf, nflux_struct* nf)
{


	/* planting parameters */	   

	double seed_quantity,seed_Ccontent;					
	double g_to_kg;
	double prop_leaf, prop_froot, prop_fruit, prop_softstem;
	
	double total_allocation = epc->alloc_leafc[epc->n_germ_phenophase-1]+epc->alloc_frootc[epc->n_germ_phenophase-1]+
		                      epc->alloc_softstemc[epc->n_germ_phenophase-1]+epc->alloc_fruitc[epc->n_germ_phenophase-1];
	
	int errflag=0;
	int flag_layerIMP, layer, EPCfromINI; 
	file epc_file;
	int md, year;

	year = ctrl->simstartyear + ctrl->simyr;
	md = PLT->mgmdPLT-1;
	
	flag_layerIMP = 0;
	layer = 1;
	seed_quantity=prop_softstem=0;
	g_to_kg=0.001;

	

	/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                                                    CALCULATING FLUXES 
	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

	
	/* 2. we assume that the transfer pools contain the palnt material of seeds. Therefore planting increase the transfer pools */ 
	if (PLT->PLT_num && md >= 0)
	{
		if (year == PLT->PLTyear_array[md] && ctrl->month == PLT->PLTmonth_array[md] && ctrl->day == PLT->PLTday_array[md]) 
		{
			/* 2.0 new EPC in case of annual changing planting information */
			strcpy(epc_file.name, PLT->filename_array[md]);
			
			/* read ecophysiological constants */
			EPCfromINI = 0;
			if (epc_init(epc_file, epc, ctrl, EPCfromINI))
			{
				printf("ERROR in EPC file reading (from PLANTING file)\n");
				errflag=1;
			}


			seed_quantity = PLT->n_seedlings_array[md] * (PLT->weight_1000seed_array[md] * g_to_kg / 1000);	/* n/m2 * (g/1000n) -> kg seed/m2 */
			seed_Ccontent = PLT->seed_carbon_array[md]/100;														/* change unit: % to number */

			epv->germ_depth = PLT->germ_depth_array[md]; 

			/* 2.2 germination depth and layer */
			while (!flag_layerIMP)
			{
				if (epv->germ_depth >= sitec->soillayer_depth[layer-1] && epv->germ_depth < sitec->soillayer_depth[layer])
				{
					epv->germ_layer = layer;
					flag_layerIMP = 1;
				}
				else
					layer += 1;
			}

			/* initalize harvest index parameters */
			cs->fruitC_HRV = 0;
			cs->vegC_HRV = 0;
	
		}

	}

	if (seed_quantity && epc->n_germ_phenophase)
	{
		/* 2.3 Allocation is calculated based on  EPC alloc.params */
		prop_leaf     = epc->alloc_leafc[epc->n_germ_phenophase-1]    / total_allocation;											
		prop_froot    = epc->alloc_frootc[epc->n_germ_phenophase-1]   / total_allocation;	
		prop_fruit    = epc->alloc_fruitc[epc->n_germ_phenophase-1]   / total_allocation;	
		prop_softstem = epc->alloc_softstemc[epc->n_germ_phenophase-1]/ total_allocation;	

		if (epc->leaf_cn)
		{
			cf->leafc_transfer_from_PLT  = (seed_quantity * prop_leaf)  * seed_Ccontent;
			nf->leafn_transfer_from_PLT  =  cf->leafc_transfer_from_PLT  / epc->leaf_cn;
		}
		
		
		if (epc->froot_cn)
		{
			cf->frootc_transfer_from_PLT = (seed_quantity * prop_froot) * seed_Ccontent;
			nf->frootn_transfer_from_PLT =  cf->frootc_transfer_from_PLT / epc->froot_cn;
		}
		
		
		if (epc->fruit_cn)
		{
			cf->fruitc_transfer_from_PLT = (seed_quantity * prop_fruit) * seed_Ccontent;
			nf->fruitn_transfer_from_PLT =  cf->fruitc_transfer_from_PLT / epc->fruit_cn;
		}
		
		
		if (epc->softstem_cn)
		{
			cf->softstemc_transfer_from_PLT = (seed_quantity * prop_softstem) * seed_Ccontent;
			nf->softstemn_transfer_from_PLT =  cf->softstemc_transfer_from_PLT / epc->softstem_cn;
		}
		

			
		/* 2.4 STATE UPDATE */ 
		/* carbon */
		cs->leafc_transfer += cf->leafc_transfer_from_PLT;
		cs->PLTsrc_C += cf->leafc_transfer_from_PLT;
		cs->frootc_transfer += cf->frootc_transfer_from_PLT;
		cs->PLTsrc_C += cf->frootc_transfer_from_PLT;
		cs->fruitc_transfer += cf->fruitc_transfer_from_PLT;
		cs->PLTsrc_C += cf->fruitc_transfer_from_PLT;
		cs->softstemc_transfer += cf->softstemc_transfer_from_PLT;
		cs->PLTsrc_C += cf->softstemc_transfer_from_PLT;

		/* nitrogen */
		ns->leafn_transfer += nf->leafn_transfer_from_PLT;
		ns->PLTsrc_N += nf->leafn_transfer_from_PLT;
		ns->frootn_transfer += nf->frootn_transfer_from_PLT;
		ns->PLTsrc_N += nf->frootn_transfer_from_PLT;
		ns->fruitn_transfer += nf->fruitn_transfer_from_PLT;
		ns->PLTsrc_N += nf->fruitn_transfer_from_PLT;
		ns->softstemn_transfer += nf->softstemn_transfer_from_PLT;
		ns->PLTsrc_N += nf->softstemn_transfer_from_PLT;


	}
	




   return (errflag);
}
	