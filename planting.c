/* 
planting.c
planting  - planting seeds in soil - increase transfer pools

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
BBGC MuSo 2.3
Copyright 2014, D. Hidy
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

	double seed_quantity,seed_Ccontent;					
	double utiliz_coeff;
	double prop_leaf, prop_froot, prop_fruit;	
	
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
		seed_quantity = PLT->seed_quantity_array[mgmd][ny]/1000;				 /* change unit: kg seed/ha -> kg seed/m2 */
		seed_Ccontent = PLT->seed_carbon_array[mgmd][ny]/100;			     /* change unit: % to number */
		utiliz_coeff  = PLT->utiliz_coeff_array[mgmd][ny]/100;	         /* change unit: % to number */
	
		/* allocation is calculated based on leafC - EPC alloc.params: unit is leafC content */
		prop_leaf     = 1.0/(epc->alloc_frootc_leafc + epc->alloc_fruitc_leafc + 1.);											
		prop_froot    = epc->alloc_frootc_leafc/(epc->alloc_frootc_leafc + epc->alloc_fruitc_leafc + 1.);
		prop_fruit    = epc->alloc_fruitc_leafc/(epc->alloc_frootc_leafc + epc->alloc_fruitc_leafc + 1.);

		/* CONTROL: seed C content must cover plant material required leaf/froot/fruit composition 
		            EPC allocation paramters (new frootC:leafC, new fruitC:leafC) and INI planting paramters (seed prod. to leaf) */
		if (prop_leaf+prop_froot+prop_fruit - 1.0 > 0.0001)
		{
			printf("Fatal error: seed C content does not cover plant material required leaf/froot/fruit composition - see EPC allocation and INI planting paramters (planting.c)\n");
			ok=0;
		}

		cf->leafc_transfer_from_PLT  = (seed_quantity * utiliz_coeff * prop_leaf)  * seed_Ccontent;
		nf->leafn_transfer_from_PLT  =  cf->leafc_transfer_from_PLT  / epc->leaf_cn;
		cf->frootc_transfer_from_PLT = (seed_quantity * utiliz_coeff * prop_froot) * seed_Ccontent;
		nf->frootn_transfer_from_PLT =  cf->frootc_transfer_from_PLT / epc->froot_cn;
		/* fruit simulation - Hidy 2013 */
		cf->fruitc_transfer_from_PLT = (seed_quantity * utiliz_coeff * prop_fruit) * seed_Ccontent;
		nf->fruitn_transfer_from_PLT =  cf->fruitc_transfer_from_PLT / epc->fruit_cn;

	}
	else
	{
		cf->leafc_transfer_from_PLT  = 0.;
		cf->frootc_transfer_from_PLT = 0.;
		nf->leafn_transfer_from_PLT  = 0.;
		nf->frootn_transfer_from_PLT = 0.;
		cf->fruitc_transfer_from_PLT = 0.;
		nf->fruitn_transfer_from_PLT = 0.;
	}

	
	/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                                                    STATE UPDATE 
	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 

	/* 1. carbon */
	cs->leafc_transfer += cf->leafc_transfer_from_PLT;
	cs->PLTsrc += cf->leafc_transfer_from_PLT;
	cs->frootc_transfer += cf->frootc_transfer_from_PLT;
	cs->PLTsrc += cf->frootc_transfer_from_PLT;
	/* fruit simulation - Hidy 2013 */
	cs->fruitc_transfer += cf->fruitc_transfer_from_PLT;
	cs->PLTsrc += cf->fruitc_transfer_from_PLT;

	/* 2. nitrogen */
	ns->leafn_transfer += nf->leafn_transfer_from_PLT;
	ns->PLTsrc += nf->leafn_transfer_from_PLT;
	ns->frootn_transfer += nf->frootn_transfer_from_PLT;
	ns->PLTsrc += nf->frootn_transfer_from_PLT;
	/* fruit simulation - Hidy 2013 */
	ns->fruitn_transfer += nf->fruitn_transfer_from_PLT;
	ns->PLTsrc += nf->fruitn_transfer_from_PLT;


   return (!ok);
}
	