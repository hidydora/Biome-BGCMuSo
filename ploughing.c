 /* 
ploughing.c
do ploughing  - decrease the plant material (leafc, leafn, canopy water)

 *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
BBGC MuSo v4
Copyright 2014, D. Hidy (dori.hidy@gmail.com)
Hungarian Academy of Sciences
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

int ploughing(const control_struct* ctrl, const epconst_struct* epc, siteconst_struct* sitec, metvar_struct* metv,  epvar_struct* epv,
			  ploughing_struct* PLG, cflux_struct* cf, nflux_struct* nf, wflux_struct* wf,  cstate_struct* cs, nstate_struct* ns, wstate_struct* ws)
{
	/* ploughing parameters */

	int ny, PLGdepth, layer, ok, mgmd;
	double PLGcoeff, sminn_SUM, soilw_SUM, tsoil_SUM, sand_SUM, silt_SUM;	 
	double litr1c_SUM, litr2c_SUM, litr3c_SUM, litr4c_SUM, litr1n_SUM, litr2n_SUM, litr3n_SUM, litr4n_SUM;
	double soil1c_SUM, soil2c_SUM, soil3c_SUM, soil4c_SUM, soil1n_SUM, soil2n_SUM, soil3n_SUM, soil4n_SUM;
	double outc, outn, inc, inn;
	/* test variable */
	double storage_MGMmort=epc->storage_MGMmort;


	ok=1;
	mgmd = PLG->mgmd;

	PLGdepth=0;
	PLGcoeff=sminn_SUM=soilw_SUM=tsoil_SUM=sand_SUM=silt_SUM=0;	 
	litr1c_SUM=litr2c_SUM=litr3c_SUM=litr4c_SUM=litr1n_SUM=litr2n_SUM=litr3n_SUM=litr4n_SUM=0;
	soil1c_SUM=soil2c_SUM=soil3c_SUM=soil4c_SUM=soil1n_SUM=soil2n_SUM=soil3n_SUM=soil4n_SUM=0;
	outc=outn=inc=inn=0;

	/* yearly varied or constant management parameters */
	if(PLG->PLG_flag == 2)
	{
		ny = ctrl->simyr;
	}
	else ny=0;

	/**********************************************************************************************/
	/* I. CALCULATING PLGcoeff AND PLGdepth */

	if (mgmd >=0)
	{
		PLGcoeff      = 1.0; /* decrease of plant material caused by ploughing: difference between plant material before and after harvesting */
		PLG->DC_act   = PLG->dissolv_coeff_array[mgmd][ny]; 	 
		
		if (PLG->PLGdepths_array[mgmd][ny] >= 1 && PLG->PLGdepths_array[mgmd][ny] <= 3)
		{
			if (PLG->PLGdepths_array[mgmd][ny] == 1)
			{
				PLGdepth=3;
				printf("Shallow ploughing at 0-10 cm (ploughing.c)\n");
			}
			else
			{
				if (PLG->PLGdepths_array[mgmd][ny] == 2)
				{
					PLGdepth=4;
					printf("Medium ploughing at 10-20 cm (ploughing.c)\n");
				}
				else
				{
					if (PLG->PLGdepths_array[mgmd][ny] == 3)
					{
						PLGdepth=5;
						printf("Deep ploughing at 20-50 cm (ploughing.c)\n");
					}
					else
					{
						printf("Error: incorrect ploughing depth data(1, 2 or 3) (ploughing.c)\n");
						ok=0;
					}
				}
			}
		
		}
		else
		{
			printf("Error: missing ploughing depth data (ploughing.c)\n");
			ok=0;
		}
	}
			
	else
	{
		PLGcoeff=0.0;
	}
	

	/**********************************************************************************************/
	/* II. UNIFORM DISTRIBUTION OF SMINN, VWC litterC, litterN and soilC and soilN AFTER PLOUGHING */

	soilw_SUM=sminn_SUM=sand_SUM=silt_SUM=tsoil_SUM=0;


	if (mgmd >=0)
	{
		for (layer = 0; layer<PLGdepth; layer++)
		{
			tsoil_SUM += metv->tsoil[layer] * sitec->soillayer_thickness[layer] / sitec->soillayer_depth[PLGdepth-1];

			soilw_SUM += ws->soilw[layer];
			sminn_SUM += ns->sminn[layer];
			sand_SUM  += sitec->sand[layer];
			silt_SUM  += sitec->silt[layer];
			litr1c_SUM += cs->litr1c[layer];
			litr2c_SUM += cs->litr2c[layer];
			litr3c_SUM += cs->litr3c[layer];
			litr4c_SUM += cs->litr4c[layer];
			litr1n_SUM += ns->litr1n[layer];  
			litr2n_SUM += ns->litr2n[layer]; 
			litr3n_SUM += ns->litr3n[layer]; 
			litr4n_SUM += ns->litr4n[layer]; 
			soil1c_SUM += cs->soil1c[layer];
			soil2c_SUM += cs->soil2c[layer];
			soil3c_SUM += cs->soil3c[layer];
			soil4c_SUM += cs->soil4c[layer];
			soil1n_SUM += ns->soil1n[layer];
			soil2n_SUM += ns->soil2n[layer];
			soil3n_SUM += ns->soil3n[layer];
			soil4n_SUM += ns->soil4n[layer];

		}

		for (layer = 0; layer<PLGdepth; layer++)
		{
			metv->tsoil[layer] = tsoil_SUM;

			ws->soilw[layer]   = soilw_SUM * sitec->soillayer_thickness[layer] / sitec->soillayer_depth[PLGdepth-1];
			epv->vwc[layer]    = ws->soilw[layer] / (water_density * sitec->soillayer_thickness[layer]);

			sitec->sand[layer] = sand_SUM/PLGdepth;
			sitec->silt[layer] = silt_SUM/PLGdepth;
			sitec->clay[layer] = 100-sitec->sand[layer]-sitec->silt[layer];

			ns->sminn[layer]   = sminn_SUM * sitec->soillayer_thickness[layer] / sitec->soillayer_depth[PLGdepth-1];
			cs->litr1c[layer]  = litr1c_SUM * sitec->soillayer_thickness[layer] / sitec->soillayer_depth[PLGdepth-1];
			cs->litr2c[layer]  = litr2c_SUM * sitec->soillayer_thickness[layer] / sitec->soillayer_depth[PLGdepth-1];
			cs->litr3c[layer]  = litr3c_SUM * sitec->soillayer_thickness[layer] / sitec->soillayer_depth[PLGdepth-1];
			cs->litr4c[layer]  = litr4c_SUM * sitec->soillayer_thickness[layer] / sitec->soillayer_depth[PLGdepth-1];
			ns->litr1n[layer]  = litr1n_SUM * sitec->soillayer_thickness[layer] / sitec->soillayer_depth[PLGdepth-1];
			ns->litr2n[layer]  = litr2n_SUM * sitec->soillayer_thickness[layer] / sitec->soillayer_depth[PLGdepth-1];
			ns->litr3n[layer]  = litr3n_SUM * sitec->soillayer_thickness[layer] / sitec->soillayer_depth[PLGdepth-1];
			ns->litr4n[layer]  = litr4n_SUM * sitec->soillayer_thickness[layer] / sitec->soillayer_depth[PLGdepth-1];
			cs->soil1c[layer]  = soil1c_SUM * sitec->soillayer_thickness[layer] / sitec->soillayer_depth[PLGdepth-1];
			cs->soil2c[layer]  = soil2c_SUM * sitec->soillayer_thickness[layer] / sitec->soillayer_depth[PLGdepth-1];
			cs->soil3c[layer]  = soil3c_SUM * sitec->soillayer_thickness[layer] / sitec->soillayer_depth[PLGdepth-1];
			cs->soil4c[layer]  = soil4c_SUM * sitec->soillayer_thickness[layer] / sitec->soillayer_depth[PLGdepth-1];
			ns->soil1n[layer]  = soil1n_SUM * sitec->soillayer_thickness[layer] / sitec->soillayer_depth[PLGdepth-1];
			ns->soil2n[layer]  = soil2n_SUM * sitec->soillayer_thickness[layer] / sitec->soillayer_depth[PLGdepth-1];
			ns->soil3n[layer]  = soil3n_SUM * sitec->soillayer_thickness[layer] / sitec->soillayer_depth[PLGdepth-1];
			ns->soil4n[layer]  = soil4n_SUM * sitec->soillayer_thickness[layer] / sitec->soillayer_depth[PLGdepth-1];
		}

		/* update TSOIL values */
		metv->tsoil_surface_pre = metv->tsoil[0];
		metv->tsoil_surface     = metv->tsoil[0];



	}


	/**********************************************************************************************/
	/* III. CALCULATING FLUXES */
	
	/* 1. plant material decrease due to ploughing */
	
	/* 1.1.1 CARBON leaf, froot, fruit, sofstem, gresp*/

	cf->leafc_to_PLG				= cs->leafc * PLGcoeff;
	cf->leafc_storage_to_PLG		= cs->leafc_storage * PLGcoeff * storage_MGMmort;
	cf->leafc_transfer_to_PLG		= cs->leafc_transfer * PLGcoeff * storage_MGMmort;

	cf->frootc_to_PLG				= cs->frootc * PLGcoeff;
	cf->frootc_storage_to_PLG		= cs->frootc_storage * PLGcoeff * storage_MGMmort;
	cf->frootc_transfer_to_PLG		= cs->frootc_transfer * PLGcoeff * storage_MGMmort;

	cf->fruitc_to_PLG				= cs->fruitc * PLGcoeff;
	cf->fruitc_storage_to_PLG		= cs->fruitc_storage * PLGcoeff * storage_MGMmort;
	cf->fruitc_transfer_to_PLG		= cs->fruitc_transfer * PLGcoeff * storage_MGMmort;

	cf->softstemc_to_PLG			= cs->softstemc * PLGcoeff;
	cf->softstemc_storage_to_PLG	= cs->softstemc_storage  * PLGcoeff * storage_MGMmort;
	cf->softstemc_transfer_to_PLG	= cs->softstemc_transfer * PLGcoeff * storage_MGMmort;

	cf->gresp_storage_to_PLG		= cs->gresp_storage * PLGcoeff * storage_MGMmort;
	cf->gresp_transfer_to_PLG		= cs->gresp_transfer * PLGcoeff * storage_MGMmort;


	/* 1.2.1 NITROGEN leaf, froot, fruit, sofstem, gresp*/

	nf->leafn_to_PLG = ns->leafn * PLGcoeff;
	nf->leafn_storage_to_PLG  = ns->leafn_storage * PLGcoeff * storage_MGMmort;;
	nf->leafn_transfer_to_PLG = ns->leafn_transfer * PLGcoeff * storage_MGMmort;

	nf->frootn_to_PLG = ns->frootn * PLGcoeff;
	nf->frootn_storage_to_PLG = ns->frootn_storage * PLGcoeff * storage_MGMmort;
	nf->frootn_transfer_to_PLG = ns->frootn_transfer * PLGcoeff * storage_MGMmort;

	nf->fruitn_to_PLG = ns->fruitn * PLGcoeff;
	nf->fruitn_storage_to_PLG = ns->fruitn_storage * PLGcoeff * storage_MGMmort;
	nf->fruitn_transfer_to_PLG = ns->fruitn_transfer * PLGcoeff * storage_MGMmort;

	nf->softstemn_to_PLG = ns->softstemn * PLGcoeff;
	nf->softstemn_storage_to_PLG = ns->softstemn_storage * PLGcoeff * storage_MGMmort;
	nf->softstemn_transfer_to_PLG = ns->softstemn_transfer * PLGcoeff * storage_MGMmort;
	
	nf->softstemn_to_PLG = ns->softstemn * PLGcoeff;
	nf->softstemn_storage_to_PLG = ns->softstemn_storage * PLGcoeff * storage_MGMmort;
	nf->softstemn_transfer_to_PLG = ns->softstemn_transfer * PLGcoeff * storage_MGMmort;

	nf->retransn_to_PLG = ns->retransn * PLGcoeff;


	/* 1.3. WATER */ 
	wf->canopyw_to_PLG = ws->canopyw * PLGcoeff;

	/*--------------------------------------------------------------------*/
	/* 2. standing dead biome to cut-down belowground materail: PLGcoeff part of aboveground and whole belowground */

	cf->litr1c_STDB_to_PLG	= cs->litr1cA_STDB * PLGcoeff;
	cf->litr2c_STDB_to_PLG	= cs->litr2cA_STDB * PLGcoeff;
	cf->litr3c_STDB_to_PLG	= cs->litr3cA_STDB * PLGcoeff;
	cf->litr4c_STDB_to_PLG	= cs->litr4cA_STDB * PLGcoeff ;

	nf->litr1n_STDB_to_PLG = ns->litr1nA_STDB * PLGcoeff;
	nf->litr2n_STDB_to_PLG = ns->litr2nA_STDB * PLGcoeff;
	nf->litr3n_STDB_to_PLG = ns->litr3nA_STDB * PLGcoeff;
	nf->litr4n_STDB_to_PLG = ns->litr4nA_STDB * PLGcoeff;

	/*--------------------------------------------------------------------*/
	 /* 3. cut-down dead biome: abovegound to belowground  */

	cf->litr1c_CTDB_AtoB_PLG = cs->litr1cA_CTDB * PLGcoeff;
	cf->litr2c_CTDB_AtoB_PLG = cs->litr2cA_CTDB * PLGcoeff;
	cf->litr3c_CTDB_AtoB_PLG = cs->litr3cA_CTDB * PLGcoeff;
	cf->litr4c_CTDB_AtoB_PLG = cs->litr4cA_CTDB * PLGcoeff;

	nf->litr1n_CTDB_AtoB_PLG = ns->litr1nA_CTDB * PLGcoeff;
	nf->litr2n_CTDB_AtoB_PLG = ns->litr2nA_CTDB * PLGcoeff;
	nf->litr3n_CTDB_AtoB_PLG = ns->litr3nA_CTDB * PLGcoeff;
	nf->litr4n_CTDB_AtoB_PLG = ns->litr4nA_CTDB * PLGcoeff;

	

	/*--------------------------------------------------------------------*/
	/* 4. fluxes to belowground cut-down biomass */
	/* 4.1 carbon */
	cf->PLG_to_CTDB_litr1cB = cf->leafc_to_PLG * epc->leaflitr_flab + 
							  cf->frootc_to_PLG * epc->frootlitr_flab +
							  cf->fruitc_to_PLG * epc->fruitlitr_flab +
							  cf->softstemc_to_PLG * epc->softstemlitr_flab +
							  cf->leafc_storage_to_PLG + cf->leafc_transfer_to_PLG +
							  cf->frootc_storage_to_PLG + cf->frootc_transfer_to_PLG +
							  cf->fruitc_storage_to_PLG + cf->fruitc_transfer_to_PLG +
							  cf->softstemc_storage_to_PLG + cf->softstemc_transfer_to_PLG +
							  cf->gresp_storage_to_PLG + cf->gresp_transfer_to_PLG;
	
	cf->PLG_to_CTDB_litr2cB = cf->leafc_to_PLG* epc->leaflitr_fucel + 
						      cf->frootc_to_PLG * epc->frootlitr_fucel +
						      cf->fruitc_to_PLG * epc->fruitlitr_fucel +
						      cf->softstemc_to_PLG * epc->softstemlitr_fucel;

	cf->PLG_to_CTDB_litr3cB = cf->leafc_to_PLG * epc->leaflitr_fscel + 
						      cf->frootc_to_PLG * epc->frootlitr_fscel +
						      cf->fruitc_to_PLG * epc->fruitlitr_fscel +
						      cf->softstemc_to_PLG * epc->softstemlitr_fscel;

	cf->PLG_to_CTDB_litr4cB = cf->leafc_to_PLG * epc->leaflitr_flig + 
						      cf->frootc_to_PLG * epc->frootlitr_flig +
						      cf->fruitc_to_PLG * epc->fruitlitr_flig +
						      cf->softstemc_to_PLG * epc->softstemlitr_flig;


	/* 4.2 nitrogen*/
	nf->PLG_to_CTDB_litr1nB = nf->leafn_to_PLG * epc->leaflitr_flab + 
						      nf->frootn_to_PLG * epc->frootlitr_flab +
						      nf->fruitn_to_PLG * epc->fruitlitr_flab +
						      nf->softstemn_to_PLG * epc->softstemlitr_flab +
						      nf->leafn_storage_to_PLG + nf->leafn_transfer_to_PLG +
						      nf->frootn_storage_to_PLG + nf->frootn_transfer_to_PLG +
						      nf->fruitn_storage_to_PLG + nf->fruitn_transfer_to_PLG + 
						      nf->softstemn_storage_to_PLG + nf->softstemn_transfer_to_PLG + 
						      nf->retransn_to_PLG;
	
	nf->PLG_to_CTDB_litr2nB = nf->leafn_to_PLG* epc->leaflitr_fucel + 
						      nf->frootn_to_PLG * epc->frootlitr_fucel +
						      nf->fruitn_to_PLG * epc->fruitlitr_fucel +
						      nf->softstemn_to_PLG * epc->softstemlitr_fucel;

	nf->PLG_to_CTDB_litr3nB = nf->leafn_to_PLG * epc->leaflitr_fscel + 
						      nf->frootn_to_PLG * epc->frootlitr_fscel +
						      nf->fruitn_to_PLG * epc->fruitlitr_fscel +
						      nf->softstemn_to_PLG * epc->softstemlitr_fscel;

	nf->PLG_to_CTDB_litr4nB = nf->leafn_to_PLG * epc->leaflitr_flig + 
						      nf->frootn_to_PLG * epc->frootlitr_flig +
						      nf->fruitn_to_PLG * epc->fruitlitr_flig +
						      nf->softstemn_to_PLG * epc->softstemlitr_flig;

	


	/**********************************************************************************************/
	/* III. STATE UPDATE */

	/* 1.1.1 CARBON leaf, froot, fruit, sofstem, gresp*/
	cs->leafc				-= cf->leafc_to_PLG;
	cs->leafc_transfer		-= cf->leafc_transfer_to_PLG;
	cs->leafc_storage		-= cf->leafc_storage_to_PLG;
	cs->gresp_transfer		-= cf->gresp_transfer_to_PLG;
	cs->gresp_storage		-= cf->gresp_storage_to_PLG;
	cs->frootc				-= cf->frootc_to_PLG;
	cs->frootc_transfer		-= cf->frootc_transfer_to_PLG;
	cs->frootc_storage		-= cf->frootc_storage_to_PLG;
	cs->fruitc				-= cf->fruitc_to_PLG;
	cs->fruitc_transfer		-= cf->fruitc_transfer_to_PLG;
	cs->fruitc_storage		-= cf->fruitc_storage_to_PLG;
	cs->softstemc			-= cf->softstemc_to_PLG;
	cs->softstemc_transfer  -= cf->softstemc_transfer_to_PLG;
	cs->softstemc_storage   -= cf->softstemc_storage_to_PLG;

	
	/* 1.1.2 CARBON  dead standing biomass */
	cs->litr1cA_STDB	-= cf->litr1c_STDB_to_PLG;
	cs->litr2cA_STDB	-= cf->litr2c_STDB_to_PLG;
	cs->litr3cA_STDB	-= cf->litr3c_STDB_to_PLG;
	cs->litr4cA_STDB	-= cf->litr4c_STDB_to_PLG;

	/* 1.2.1 NITROGEN leaf, froot, fruit, sofstem, retrans*/
	ns->leafn				-= nf->leafn_to_PLG;
	ns->leafn_transfer		-= nf->leafn_transfer_to_PLG;
	ns->leafn_storage		-= nf->leafn_storage_to_PLG;
	ns->frootn				-= nf->frootn_to_PLG;
	ns->frootn_transfer		-= nf->frootn_transfer_to_PLG;
	ns->frootn_storage		-= nf->frootn_storage_to_PLG;
	ns->fruitn				-= nf->fruitn_to_PLG;
	ns->fruitn_transfer		-= nf->fruitn_transfer_to_PLG;
	ns->fruitn_storage		-= nf->fruitn_storage_to_PLG;
	ns->softstemn			-= nf->softstemn_to_PLG;
	ns->softstemn_transfer  -= nf->softstemn_transfer_to_PLG;
	ns->softstemn_storage	-= nf->softstemn_storage_to_PLG;
	ns->retransn			-= nf->retransn_to_PLG;
   
	/* 1.2.2 NITROGEN  dead standing biomass */
	ns->litr1nA_STDB		-= nf->litr1n_STDB_to_PLG;
	ns->litr2nA_STDB		-= nf->litr2n_STDB_to_PLG;
	ns->litr3nA_STDB		-= nf->litr3n_STDB_to_PLG;
	ns->litr4nA_STDB		-= nf->litr4n_STDB_to_PLG;

	/* 1.3. WATER */
	ws->canopyw_PLGsnk += wf->canopyw_to_PLG;
	ws->canopyw -= wf->canopyw_to_PLG;
	ws->soilw[0] += wf->canopyw_to_PLG;


	/*--------------------------------------------------------------------*/
	/* 2. standing dead biome to cut-down belowground materail: PLGcoeff part of aboveground and whole belowground */

	cs->litr1cA_STDB -= cf->litr1c_STDB_to_PLG;
	cs->litr1cB_CTDB += cf->litr1c_STDB_to_PLG;
	cs->litr2cA_STDB -= cf->litr2c_STDB_to_PLG;
	cs->litr2cB_CTDB += cf->litr2c_STDB_to_PLG;
	cs->litr3cA_STDB -= cf->litr3c_STDB_to_PLG;
	cs->litr3cB_CTDB += cf->litr3c_STDB_to_PLG;
	cs->litr4cA_STDB -= cf->litr4c_STDB_to_PLG;
	cs->litr4cB_CTDB += cf->litr4c_STDB_to_PLG;

	ns->litr1nA_STDB -= nf->litr1n_STDB_to_PLG;
	ns->litr1nB_CTDB += nf->litr1n_STDB_to_PLG;
	ns->litr2nA_STDB -= nf->litr2n_STDB_to_PLG;
	ns->litr2nB_CTDB += nf->litr2n_STDB_to_PLG;
	ns->litr3nA_STDB -= nf->litr3n_STDB_to_PLG;
	ns->litr3nB_CTDB += nf->litr3n_STDB_to_PLG;
	ns->litr4nA_STDB -= nf->litr4n_STDB_to_PLG;
	ns->litr4nB_CTDB += nf->litr4n_STDB_to_PLG;
	
	/*--------------------------------------------------------------------*/
	 /* 3. cut-down dead biome: abovegound to belowground  */

	cs->litr1cA_CTDB -= cf->litr1c_CTDB_AtoB_PLG;
	cs->litr1cB_CTDB += cf->litr1c_CTDB_AtoB_PLG; 
	cs->litr2cA_CTDB -= cf->litr2c_CTDB_AtoB_PLG;
	cs->litr2cB_CTDB += cf->litr2c_CTDB_AtoB_PLG; 
	cs->litr3cA_CTDB -= cf->litr3c_CTDB_AtoB_PLG;
	cs->litr3cB_CTDB += cf->litr3c_CTDB_AtoB_PLG; 
	cs->litr4cA_CTDB -= cf->litr4c_CTDB_AtoB_PLG;
	cs->litr4cB_CTDB += cf->litr4c_CTDB_AtoB_PLG; 

	ns->litr1nA_CTDB -= nf->litr1n_CTDB_AtoB_PLG;
	ns->litr1nB_CTDB += nf->litr1n_CTDB_AtoB_PLG; 
	ns->litr2nA_CTDB -= nf->litr2n_CTDB_AtoB_PLG;
	ns->litr2nB_CTDB += nf->litr2n_CTDB_AtoB_PLG; 
	ns->litr3nA_CTDB -= nf->litr3n_CTDB_AtoB_PLG;
	ns->litr3nB_CTDB += nf->litr3n_CTDB_AtoB_PLG; 
	ns->litr4nA_CTDB -= nf->litr4n_CTDB_AtoB_PLG;
	ns->litr4nB_CTDB += nf->litr4n_CTDB_AtoB_PLG; 
	

	/*--------------------------------------------------------------------*/
	/* 4. fluxes to belowground cut-down biomass */
	
	cs->litr1cB_CTDB += cf->PLG_to_CTDB_litr1cB;
	cs->litr2cB_CTDB += cf->PLG_to_CTDB_litr2cB;
	cs->litr3cB_CTDB += cf->PLG_to_CTDB_litr3cB;
	cs->litr4cB_CTDB += cf->PLG_to_CTDB_litr4cB;

	ns->litr1nB_CTDB += nf->PLG_to_CTDB_litr1nB;
	ns->litr2nB_CTDB += nf->PLG_to_CTDB_litr2nB;
	ns->litr3nB_CTDB += nf->PLG_to_CTDB_litr3nB;
	ns->litr4nB_CTDB += nf->PLG_to_CTDB_litr4nB;



   return (!ok);
}
	