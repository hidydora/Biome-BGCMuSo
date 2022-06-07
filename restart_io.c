/*
restart_io.c
functions called to copy restart info between restart structure and
active structures

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

int restart_input(const control_struct* ctrl, const epconst_struct* epc, wstate_struct* ws, cstate_struct* cs, nstate_struct* ns,
				  epvar_struct* epv, int* metyr, restart_data_struct* restart)
{
	int ok=1;
	int layer;
	
	/* Hidy 2010 - multilayer soil */
	for (layer =0; layer < N_SOILLAYERS; layer++)
	{ 
		ws->soilw[layer]                  = restart->soilw[layer];
	}

	ws->snoww                             = restart->snoww;
	ws->canopyw                           = restart->canopyw;

	cs->leafc                             = restart->leafc;
	cs->leafc_storage                     = restart->leafc_storage;
	cs->leafc_transfer                    = restart->leafc_transfer;
	cs->frootc                            = restart->frootc;
	cs->frootc_storage                    = restart->frootc_storage;
	cs->frootc_transfer                   = restart->frootc_transfer;
	/* fruit simulation - Hidy 2013. */
	cs->fruitc                            = restart->fruitc;
	cs->fruitc_storage                    = restart->fruitc_storage;
	cs->fruitc_transfer                   = restart->fruitc_transfer;
	/* softstem simulation - Hidy 2013. */
	cs->softstemc                         = restart->softstemc;
	cs->softstemc_storage                 = restart->softstemc_storage;
	cs->softstemc_transfer                = restart->softstemc_transfer;

	cs->livestemc                         = restart->livestemc;
	cs->livestemc_storage                 = restart->livestemc_storage;
	cs->livestemc_transfer                = restart->livestemc_transfer;
	cs->deadstemc                         = restart->deadstemc;
	cs->deadstemc_storage                 = restart->deadstemc_storage;
	cs->deadstemc_transfer                = restart->deadstemc_transfer;
	cs->livecrootc                        = restart->livecrootc;
	cs->livecrootc_storage                = restart->livecrootc_storage;
	cs->livecrootc_transfer               = restart->livecrootc_transfer;
	cs->deadcrootc                        = restart->deadcrootc;
	cs->deadcrootc_storage                = restart->deadcrootc_storage;
	cs->deadcrootc_transfer               = restart->deadcrootc_transfer;
	cs->gresp_storage                     = restart->gresp_storage;
	cs->gresp_transfer                    = restart->gresp_transfer;


	/* Hidy 2016 - multilayer soil */
	for (layer=0; layer < N_SOILLAYERS; layer++)
	{
		cs->litr1c[layer]                 = restart->litr1c[layer];
		cs->litr2c[layer]                 = restart->litr2c[layer];
		cs->litr3c[layer]                 = restart->litr3c[layer];
		cs->litr4c[layer]                 = restart->litr4c[layer];
		cs->soil1c[layer]                 = restart->soil1c[layer];
		cs->soil2c[layer]                 = restart->soil2c[layer];
		cs->soil3c[layer]                 = restart->soil3c[layer];
		cs->soil4c[layer]                 = restart->soil4c[layer];
		cs->cwdc[layer]                   = restart->cwdc[layer];
	}
	cs->cpool                             = restart->cpool;

	/* Hidy 2016 - standing dead biomass, cut-down dead biomass and litter pools */
	cs->litr1cA_STDB			  = restart->litr1cA_STDB;
	cs->litr1cB_STDB			  = restart->litr1cB_STDB;
	cs->litr2cA_STDB			  = restart->litr2cA_STDB;
	cs->litr2cB_STDB			  = restart->litr2cB_STDB;
	cs->litr3cA_STDB			  = restart->litr3cA_STDB;
	cs->litr3cB_STDB			  = restart->litr3cB_STDB;
	cs->litr4cA_STDB			  = restart->litr4cA_STDB;
	cs->litr4cB_STDB			  = restart->litr4cB_STDB;
	cs->cwdcA_STDB                = restart->cwdcA_STDB;
	cs->cwdcB_STDB                = restart->cwdcB_STDB;
	cs->litr1cA_CTDB			  = restart->litr1cA_CTDB;
	cs->litr1cB_CTDB			  = restart->litr1cB_CTDB;
	cs->litr2cA_CTDB			  = restart->litr2cA_CTDB;
	cs->litr2cB_CTDB			  = restart->litr2cB_CTDB;
	cs->litr3cA_CTDB			  = restart->litr3cA_CTDB;
	cs->litr3cB_CTDB			  = restart->litr3cB_CTDB;
	cs->litr4cA_CTDB			  = restart->litr4cA_CTDB;
	cs->litr4cB_CTDB			  = restart->litr4cB_CTDB;
	cs->cwdcA_CTDB                = restart->cwdcA_CTDB;
	cs->cwdcB_CTDB                = restart->cwdcB_CTDB;


	
	/* spinup - normal C and N pool adjustment in order to avoud negative N pools in case of land use change (changing EOC) */
	if (epc->leaf_cn > 0)
	{
		ns->leafn                         = cs->leafc           / epc->leaf_cn;
		ns->leafn_storage                 = cs->leafc_storage   / epc->leaf_cn;
		ns->leafn_transfer                = cs->leafc_transfer  / epc->leaf_cn;
	}
	else
	{
		ns->leafn                         = 0;
		ns->leafn_storage                 = 0;
		ns->leafn_transfer                = 0;
	}

	if (epc->froot_cn)
	{
		ns->frootn                        = cs->frootc          / epc->froot_cn;
		ns->frootn_storage                = cs->frootc_storage  / epc->froot_cn;
		ns->frootn_transfer               = cs->frootc_transfer / epc->froot_cn;
	}
	else
	{
		ns->frootn                        = 0;
		ns->frootn_storage                = 0;
		ns->frootn_transfer               = 0;
	}

	/* fruit simulation - Hidy 2013. */
	if (epc->fruit_cn > 0)
	{
		ns->fruitn                        = cs->fruitc          / epc->fruit_cn;
		ns->fruitn_storage                = cs->fruitc_storage  / epc->fruit_cn;
		ns->fruitn_transfer               = cs->fruitc_transfer / epc->fruit_cn;
	}
	else
	{
		ns->fruitn                        =	0;
		ns->fruitn_storage                = 0;
		ns->fruitn_transfer               = 0;
	}
	/* softstem simulation - Hidy 2015.*/ 
	if (epc->softstem_cn)
	{
		ns->softstemn                     = cs->softstemc          / epc->softstem_cn;
		ns->softstemn_storage             = cs->softstemc_storage  / epc->softstem_cn;
		ns->softstemn_transfer            = cs->softstemc_transfer / epc->softstem_cn;
	}
	else
	{
		ns->softstemn                     = 0;
		ns->softstemn_storage             = 0;
		ns->softstemn_transfer            = 0;
	}

	if (epc->livewood_cn > 0)
	{
		ns->livestemn                     = cs->livestemc          / epc->livewood_cn;
		ns->livestemn_storage             = cs->livestemc_storage  / epc->livewood_cn;
		ns->livestemn_transfer            = cs->livestemc_transfer / epc->livewood_cn;
		ns->livecrootn                    = cs->livecrootc         / epc->livewood_cn;
		ns->livecrootn_storage            = cs->livecrootc_storage / epc->livewood_cn;
		ns->livecrootn_transfer           = cs->livecrootc_transfer/ epc->livewood_cn;
	}
	else
	{
		ns->livestemn                     = 0;
		ns->livestemn_storage             = 0;
		ns->livestemn_transfer            = 0;
		ns->livecrootn                    = 0;
		ns->livecrootn_storage            = 0;
		ns->livecrootn_transfer           = 0;
	}
	
	if (epc->deadwood_cn > 0)
	{
		ns->deadstemn                     = cs->deadstemc          / epc->deadwood_cn; 
		ns->deadstemn_storage             = cs->deadstemc_storage  / epc->deadwood_cn;
		ns->deadstemn_transfer            = cs->deadstemc_transfer / epc->deadwood_cn;
		ns->deadcrootn                    = cs->deadcrootc         / epc->deadwood_cn;
		ns->deadcrootn_storage            = cs->deadcrootc_storage / epc->deadwood_cn;
		ns->deadcrootn_transfer           = cs->deadcrootc_transfer/ epc->deadwood_cn;
	
	}
	else
	{
		ns->deadstemn                     = 0; 
		ns->deadstemn_storage             = 0;
		ns->deadstemn_transfer            = 0;
		ns->deadcrootn                    = 0;
		ns->deadcrootn_storage            = 0;
		ns->deadcrootn_transfer           = 0;
	
	}

	/* Hidy 2016 - multilayer soil */
	for (layer=0; layer < N_SOILLAYERS; layer++)
	{
 		ns->litr1n[layer]                 = restart->litr1n[layer];
		ns->litr2n[layer]                 = restart->litr2n[layer];
		ns->litr3n[layer]                 = restart->litr3n[layer];
		ns->litr4n[layer]                 = restart->litr4n[layer];
		ns->soil1n[layer]                 = restart->soil1n[layer];
		ns->soil2n[layer]                 = restart->soil2n[layer];
		ns->soil3n[layer]                 = restart->soil3n[layer];
		ns->soil4n[layer]                 = restart->soil4n[layer];
		if (epc->deadwood_cn > 0) 
			ns->cwdn[layer]  = cs->cwdc[layer] / epc->deadwood_cn;
		else
			ns->cwdn[layer]  = 0;
	}

	/* Hidy 2015 - standing dead biomass, cut-down dead biomass and litter pools */
	ns->litr1nA_STDB			  = restart->litr1nA_STDB;
	ns->litr1nB_STDB			  = restart->litr1nB_STDB;
	ns->litr2nA_STDB			  = restart->litr2nA_STDB;
	ns->litr2nB_STDB			  = restart->litr2nB_STDB;
	ns->litr3nA_STDB			  = restart->litr3nA_STDB;
	ns->litr3nB_STDB			  = restart->litr3nB_STDB;
	ns->litr4nA_STDB			  = restart->litr4nA_STDB;
	ns->litr4nB_STDB			  = restart->litr4nB_STDB;
	ns->cwdnA_STDB                = restart->cwdnA_STDB;
	ns->cwdnB_STDB                = restart->cwdnB_STDB;
	ns->litr1nA_CTDB			  = restart->litr1nA_CTDB;
	ns->litr1nB_CTDB			  = restart->litr1nB_CTDB;
	ns->litr2nA_CTDB			  = restart->litr2nA_CTDB;
	ns->litr2nB_CTDB			  = restart->litr2nB_CTDB;
	ns->litr3nA_CTDB			  = restart->litr3nA_CTDB;
	ns->litr3nB_CTDB			  = restart->litr3nB_CTDB;
	ns->litr4nA_CTDB			  = restart->litr4nA_CTDB;
	ns->litr4nB_CTDB			  = restart->litr4nB_CTDB;

	/*-----------------------------*/
	/* Hidy 2016 - multilayer soil */
	
	for (layer =0; layer < N_SOILLAYERS; layer++)
	{ 
		ns->sminn[layer]                  = restart->sminn[layer];
	}
	/*-----------------------------*/
	
	ns->retransn			              = restart->retransn;
	ns->npool                             = restart->npool;

	epv->day_leafc_litfall_increment      = restart->day_leafc_litfall_increment;
	epv->day_frootc_litfall_increment     = restart->day_frootc_litfall_increment;
	/* fruit simulation - Hidy 2013. */
	epv->day_fruitc_litfall_increment     = restart->day_fruitc_litfall_increment;
	/* softstem simulation - Hidy 2015. */
	epv->day_softstemc_litfall_increment     = restart->day_softstemc_litfall_increment;
	
	epv->day_livestemc_turnover_increment = restart->day_livestemc_turnover_increment;
	epv->day_livecrootc_turnover_increment= restart->day_livecrootc_turnover_increment;
	epv->annmax_leafc                     = restart->annmax_leafc;
	epv->annmax_frootc                    = restart->annmax_frootc;
	/* fruit simulation - Hidy 2013. */
	epv->annmax_fruitc                    = restart->annmax_fruitc;
	/* softstem simulation - Hidy 2015. */
	epv->annmax_softstemc                 = restart->annmax_softstemc;

	epv->annmax_livestemc                 = restart->annmax_livestemc;
	epv->annmax_livecrootc                = restart->annmax_livecrootc;
	epv->dsr                              = restart->dsr;
	
	if (ctrl->keep_metyr) *metyr          = restart->metyr; 
	else                  *metyr          = 0;
	
	return(!ok);
}

int restart_output(wstate_struct* ws,cstate_struct* cs, nstate_struct* ns, epvar_struct* epv, int metyr, 
				   restart_data_struct* restart)
{
	int ok=1;
	int layer;

	/* Hidy 2010 - multilayer soil */
	for (layer =0; layer < N_SOILLAYERS; layer++)
	{ 
		restart->soilw[layer]  				  = ws->soilw[layer];
	}
	

	restart->snoww  						  = ws->snoww;
	restart->canopyw						  = ws->canopyw;

	restart->leafc  						  = cs->leafc;
	restart->leafc_storage  				  = cs->leafc_storage;
	restart->leafc_transfer 				  = cs->leafc_transfer;
	restart->frootc 						  = cs->frootc;
	restart->frootc_storage 				  = cs->frootc_storage;
	restart->frootc_transfer				  = cs->frootc_transfer;
	/* fruit simulation - Hidy 2013. */
	restart->fruitc 						  = cs->fruitc;
	restart->fruitc_storage 				  = cs->fruitc_storage;
	restart->fruitc_transfer				  = cs->fruitc_transfer;
	/* softstem simulation - Hidy 2013. */
	restart->softstemc 						  = cs->softstemc;
	restart->softstemc_storage 				  = cs->softstemc_storage;
	restart->softstemc_transfer				  = cs->softstemc_transfer;

	restart->livestemc  					  = cs->livestemc;
	restart->livestemc_storage  			  = cs->livestemc_storage;
	restart->livestemc_transfer 			  = cs->livestemc_transfer;
	restart->deadstemc  					  = cs->deadstemc;
	restart->deadstemc_storage  			  = cs->deadstemc_storage;
	restart->deadstemc_transfer 			  = cs->deadstemc_transfer;
	restart->livecrootc 					  = cs->livecrootc;
	restart->livecrootc_storage 			  = cs->livecrootc_storage;
	restart->livecrootc_transfer			  = cs->livecrootc_transfer;
	restart->deadcrootc 					  = cs->deadcrootc;
	restart->deadcrootc_storage 			  = cs->deadcrootc_storage;
	restart->deadcrootc_transfer			  = cs->deadcrootc_transfer;
	restart->gresp_storage  				  = cs->gresp_storage;
	restart->gresp_transfer 				  = cs->gresp_transfer;


	/* Hidy 2015 - standing dead biomass, cut-down dead biomass and litter pools */
	restart->litr1cA_STDB		= cs->litr1cA_STDB;
	restart->litr1cB_STDB		= cs->litr1cB_STDB;
	restart->litr2cA_STDB		= cs->litr2cA_STDB;
	restart->litr2cB_STDB		= cs->litr2cB_STDB;
	restart->litr3cA_STDB		= cs->litr3cA_STDB;
	restart->litr3cB_STDB		= cs->litr3cB_STDB;
	restart->litr4cA_STDB		= cs->litr4cA_STDB;
	restart->litr4cB_STDB		= cs->litr4cB_STDB;
	restart->cwdcA_STDB         = cs->cwdcA_STDB;
	restart->cwdcB_STDB         = cs->cwdcB_STDB;
	restart->litr1cA_CTDB		= cs->litr1cA_CTDB;
	restart->litr1cB_CTDB		= cs->litr1cB_CTDB;
	restart->litr2cA_CTDB		= cs->litr2cA_CTDB;
	restart->litr2cB_CTDB		= cs->litr2cB_CTDB;
	restart->litr3cA_CTDB		= cs->litr3cA_CTDB;
	restart->litr3cB_CTDB		= cs->litr3cB_CTDB;
	restart->litr4cA_CTDB		= cs->litr4cA_CTDB;
	restart->litr4cB_CTDB		= cs->litr4cB_CTDB;
	restart->cwdcA_CTDB         = cs->cwdcA_CTDB;
	restart->cwdcB_CTDB         = cs->cwdcB_CTDB;

	/* Hidy 2016 - multilayer soil */
	for (layer=0; layer < N_SOILLAYERS; layer++)
	{
		restart->soil1c[layer] 	= cs->soil1c[layer];
		restart->soil2c[layer]  = cs->soil2c[layer];
		restart->soil3c[layer]  = cs->soil3c[layer];
		restart->soil4c[layer]  = cs->soil4c[layer];
		restart->litr1c[layer]  = cs->litr1c[layer];
		restart->litr2c[layer]  = cs->litr2c[layer];
		restart->litr3c[layer]  = cs->litr3c[layer];
		restart->litr4c[layer]  = cs->litr4c[layer];
		restart->cwdc[layer]	= cs->cwdc[layer];

	}
	restart->cpool  						  = cs->cpool;

	restart->leafn  						  = ns->leafn;
	restart->leafn_storage  				  = ns->leafn_storage;
	restart->leafn_transfer 				  = ns->leafn_transfer;
	restart->frootn 						  = ns->frootn;
	restart->frootn_storage 				  = ns->frootn_storage;
	restart->frootn_transfer				  = ns->frootn_transfer;
	/* fruit simulation - Hidy 2013. */
	restart->fruitn  						  = ns->fruitn;
	restart->fruitn_storage  				  = ns->fruitn_storage;
	restart->fruitn_transfer 				  = ns->fruitn_transfer;
	/* softstem simulation - Hidy 2013. */
	restart->softstemn  					  = ns->softstemn;
	restart->softstemn_storage  			  = ns->softstemn_storage;
	restart->softstemn_transfer 			  = ns->softstemn_transfer;

	restart->livestemn  					  = ns->livestemn;
	restart->livestemn_storage  			  = ns->livestemn_storage;
	restart->livestemn_transfer 			  = ns->livestemn_transfer;
	restart->deadstemn  					  = ns->deadstemn;
	restart->deadstemn_storage  			  = ns->deadstemn_storage;
	restart->deadstemn_transfer 			  = ns->deadstemn_transfer;
	restart->livecrootn 					  = ns->livecrootn;
	restart->livecrootn_storage 			  = ns->livecrootn_storage;
	restart->livecrootn_transfer			  = ns->livecrootn_transfer;
	restart->deadcrootn 					  = ns->deadcrootn;
	restart->deadcrootn_storage 			  = ns->deadcrootn_storage;
	restart->deadcrootn_transfer			  = ns->deadcrootn_transfer;

	
	/* Hidy 2016 - standing dead biomass, cut-down dead biomass and litter pools */
	restart->litr1nA_STDB		= ns->litr1nA_STDB;
	restart->litr1nB_STDB		= ns->litr1nB_STDB;
	restart->litr2nA_STDB		= ns->litr2nA_STDB;
	restart->litr2nB_STDB		= ns->litr2nB_STDB;
	restart->litr3nA_STDB		= ns->litr3nA_STDB;
	restart->litr3nB_STDB		= ns->litr3nB_STDB;
	restart->litr4nA_STDB		= ns->litr4nA_STDB;
	restart->litr4nB_STDB		= ns->litr4nB_STDB;
	restart->cwdnA_STDB         = ns->cwdnA_STDB;
	restart->cwdnB_STDB         = ns->cwdnB_STDB;
	restart->litr1nA_CTDB		= ns->litr1nA_CTDB;
	restart->litr1nB_CTDB		= ns->litr1nB_CTDB;
	restart->litr2nA_CTDB		= ns->litr2nA_CTDB;
	restart->litr2nB_CTDB		= ns->litr2nB_CTDB;
	restart->litr3nA_CTDB		= ns->litr3nA_CTDB;
	restart->litr3nB_CTDB		= ns->litr3nB_CTDB;
	restart->litr4nA_CTDB		= ns->litr4nA_CTDB;
	restart->litr4nB_CTDB		= ns->litr4nB_CTDB;
	restart->cwdnA_CTDB         = ns->cwdnA_CTDB;
	restart->cwdnB_CTDB         = ns->cwdnB_CTDB;

	/*-----------------------------*/
	/* Hidy 2016 - multilayer soil */
	for (layer = 0; layer < N_SOILLAYERS; layer++)
	{
		restart->soil1n[layer] 	= ns->soil1n[layer];
		restart->soil2n[layer]  = ns->soil2n[layer];
		restart->soil3n[layer]  = ns->soil3n[layer];
		restart->soil4n[layer]  = ns->soil4n[layer];
		restart->litr1n[layer]  = ns->litr1n[layer];
		restart->litr2n[layer]  = ns->litr2n[layer];
		restart->litr3n[layer]  = ns->litr3n[layer];
		restart->litr4n[layer]  = ns->litr4n[layer];
		restart->cwdn[layer]	= ns->cwdn[layer];
	}
	/*-----------------------------*/
	/* Hidy 2011 - multilayer soil */
	
	for (layer =0; layer < N_SOILLAYERS; layer++)
	{ 
		restart->sminn[layer]                 = ns->sminn[layer];

	}
	/*-----------------------------*/
	
	restart->retransn           			  = ns->retransn;
	restart->npool  						  = ns->npool;

	restart->day_leafc_litfall_increment	  = epv->day_leafc_litfall_increment;
	restart->day_frootc_litfall_increment	  = epv->day_frootc_litfall_increment;
	/* fruit simulation - Hidy 2013. */
	restart->day_fruitc_litfall_increment	  = epv->day_fruitc_litfall_increment;
	/* softstem simulation - Hidy 2013. */
	restart->day_softstemc_litfall_increment  = epv->day_softstemc_litfall_increment;

	restart->day_livestemc_turnover_increment = epv->day_livestemc_turnover_increment;
	restart->day_livecrootc_turnover_increment= epv->day_livecrootc_turnover_increment;
	restart->annmax_leafc					  = epv->annmax_leafc;
	restart->annmax_frootc  				  = epv->annmax_frootc;
	/* fruit simulation - Hidy 2013. */
	restart->annmax_fruitc  				  = epv->annmax_fruitc;
	/* softstem simulation - Hidy 2013. */
	restart->annmax_softstemc  				  = epv->annmax_softstemc;

	restart->annmax_livestemc				  = epv->annmax_livestemc;
	restart->annmax_livecrootc  			  = epv->annmax_livecrootc;
	restart->dsr							  = epv->dsr;

	restart->metyr                            = metyr;



	         
	
	return(!ok);
}
