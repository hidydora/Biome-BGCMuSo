/* 
planting.c
planting  - planting seeds in soil - increase transfer pools

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Biome-BGC version 4.1.1
Copyright 2009, Hidy
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

int planting(int yday, const control_struct* ctrl, const epconst_struct* epc, planting_struct* PLT, 
			 cstate_struct* cs, nstate_struct* ns, cflux_struct* cf, nflux_struct* nf, phenology_struct* phen)
{
	/* planting parameters Hidy 2012.*/	   
	int planting;
	int plti;
	int PLTday;

	double seed = PLT->seed_quantity/10;					 /* change unit: kg seed/ha -> g seed/m2 */
	double carbon_content = PLT->seed_carbon/100;			 /* change unit: % to number */
	double prop_leaf_product = PLT->prop_leaf_product/100;	 /* change unit: % to number */
	
	int ok=1;

	/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                                                    CALCULATING FLUXES 
	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

	/* planting if gapflag=1 */
	if (PLT->PLT_flag == 1)
	{
	
		planting=0;
		for(plti=0; plti < PLT->n_PLTdays; ++plti)
		{
			/* if no data is available about the date of planting we use the first day of growing season */
			if (PLT->PLTdays[plti] == DATA_GAP)
			{
				PLTday = phen->onday;
			}
			else
			{
				PLTday = PLT->PLTdays[plti];
			}
			
			if (yday == PLTday)
			{
 				planting = 1;
				if (ctrl->onscreen) printf("planting on yearday:%d\t\n",yday);
				break;
			}
			else
			{
				planting = 0;
			}
		}
		
		/* we assume that the transfer pools contain the palnt material of seeds. Therefore planting increase the transfer pools */ 
		if (planting) 
		{	
			cf->leafc_transfer_from_PLT =  (seed * prop_leaf_product) * carbon_content;
			cf->frootc_transfer_from_PLT = (seed * (1-prop_leaf_product)) * carbon_content;
			nf->leafn_transfer_from_PLT =  cf->leafc_transfer_from_PLT / epc->leaf_cn;
			nf->frootn_transfer_from_PLT = cf->frootc_transfer_from_PLT  / epc->froot_cn;

		}
		else
		{
			cf->leafc_transfer_from_PLT = 0.;
			cf->frootc_transfer_from_PLT = 0.;
			nf->leafn_transfer_from_PLT = 0.;
			nf->frootn_transfer_from_PLT = 0.;
		}
	}
	else
	{
			cf->leafc_transfer_from_PLT = 0.;
			cf->frootc_transfer_from_PLT = 0.;
			nf->leafn_transfer_from_PLT = 0.;
			nf->frootn_transfer_from_PLT = 0.;
	}

	
	/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                                                    STATE UPDATE 
	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 

	/* carbon */
	cs->leafc_transfer += cf->leafc_transfer_from_PLT;
	cs->PLTsrc += cf->leafc_transfer_from_PLT;
	cs->frootc_transfer += cf->frootc_transfer_from_PLT;
	cs->PLTsrc += cf->frootc_transfer_from_PLT;

	/* nitrogen */
	ns->leafn_transfer += nf->leafn_transfer_from_PLT;
	ns->PLTsrc += nf->leafn_transfer_from_PLT;
	ns->frootn_transfer += nf->frootn_transfer_from_PLT;
	ns->PLTsrc += nf->frootn_transfer_from_PLT;


   return (!ok);
}
	