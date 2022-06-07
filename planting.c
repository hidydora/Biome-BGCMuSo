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

int planting(const control_struct* ctrl,const epconst_struct* epc, planting_struct* PLT, cflux_struct* cf, nflux_struct* nf, 
			 cstate_struct* cs, nstate_struct*ns)
{
	/* planting parameters Hidy 2012.*/	   

	double seed_quantity;					
	double seed_Ccontent;				 
	double prop_leaf_product;	
	
	int ok=1;
	int ny;
	int mgmd = PLT->mgmd;

	/* yearly varied or constant management parameters */
	if(PLT->PLT_flag == 2)
	{
		ny = ctrl->simyr;
	}
	else ny=0;

	/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                                                    CALCULATING FLUXES 
	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

		
	/* we assume that the transfer pools contain the palnt material of seeds. Therefore planting increase the transfer pools */ 
	if (mgmd >= 0) 
	{	
		seed_quantity     = PLT->seed_quantity_array[mgmd][ny]/10;				 /* change unit: kg seed/ha -> g seed/m2 */
		seed_Ccontent     = PLT->seed_carbon_array[mgmd][ny]/100;			 /* change unit: % to number */
		prop_leaf_product = PLT->prop_leaf_product_array[mgmd][ny]/100;	     /* change unit: % to number */

		cf->leafc_transfer_from_PLT  =  (seed_quantity * prop_leaf_product) * seed_Ccontent;
		cf->frootc_transfer_from_PLT = (seed_quantity * (1-prop_leaf_product)) * seed_Ccontent;
		nf->leafn_transfer_from_PLT  =  cf->leafc_transfer_from_PLT / epc->leaf_cn;
		nf->frootn_transfer_from_PLT = cf->frootc_transfer_from_PLT  / epc->froot_cn;

	}
	else
	{
		cf->leafc_transfer_from_PLT  = 0.;
		cf->frootc_transfer_from_PLT = 0.;
		nf->leafn_transfer_from_PLT  = 0.;
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
	