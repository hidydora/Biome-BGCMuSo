/* 
mowing.c
do mowing  - decrease the plant material (leafc, leafn, canopy water)

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Biome-BGC version 4.1.1
Copyright 2008, Hidy
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

int mowing(const control_struct* ctrl, const epconst_struct* epc, mowing_struct* MOW, cflux_struct* cf, nflux_struct* nf, wflux_struct* wf,
				  cstate_struct* cs, nstate_struct* ns, wstate_struct* ws)
{
	/* mowing parameters */
	int mowing;									/* flag, 1=mowing; 0=no mowing */
	double LAI_limit;
	double remained_prop;						/* remained proportion of plabnt material is calculated from transport coefficient */

	/* local parameters */
	double befgrass_LAI;						/* value of LAI before mowing */
	double MOWcoeff_leaf, MOWcoeff_fruit;		/* coefficient determining the decrease of plant material caused by mowing */
	double MOWcoeff_gresp;						/* coefficient determining the decrease of gresp pools caused by mowing */

	int ok=1;
	int ny;
	int mgmd = MOW->mgmd;

	/* test variable */
	int nonactpool_coeff=1;

	/* yearly varied or constant management parameters */
	if(MOW->MOW_flag == 2)
	{
		ny = ctrl->simyr;
	}
	else ny=0;

	/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                                                    CALCULATING FLUXES 
	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
	

	/* mowing calculation based on LAI_limit: we assume that due to the mowing LAI (leafc) reduces to the value of LAI_limit  */
	befgrass_LAI = cs->leafc * epc->avg_proj_sla;

	/* mowing type: fixday or LAIlimit */
	if (MOW->fixday_or_fixLAI_flag == 0 || MOW->MOW_flag == 2)
	{	
		if (mgmd >=0) 
		{
	
			mowing = 1;
			remained_prop = (100 - MOW->transport_coeff_array[mgmd][ny])/100.;
			LAI_limit = MOW->LAI_limit_array[mgmd][ny];
		}
		else 
		{
			mowing = 0;
			remained_prop = 0;
			LAI_limit = befgrass_LAI;
		}
	}
	else
	{
		/* mowing if LAI greater than a given value (fixLAI_befMOW) */
		if (cs->leafc * epc->avg_proj_sla > MOW->fixLAI_befMOW)
		{
			mowing = 1;
			remained_prop = (100 - MOW->transport_coeff_array[0][0])/100.;
			LAI_limit = MOW->fixLAI_aftMOW;
		}
		else
		{
			mowing = 0;
			remained_prop = 0;
			LAI_limit = befgrass_LAI;
		}
	}
	


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////


	if (befgrass_LAI > LAI_limit)
	{	
		MOWcoeff_leaf = (1- LAI_limit/befgrass_LAI);
		MOWcoeff_fruit= MOWcoeff_leaf;
		MOWcoeff_gresp = (cs->leafc  / (cs->leafc + cs->frootc + cs->fruitc)) * MOWcoeff_leaf + 
					     (cs->fruitc / (cs->leafc + cs->frootc + cs->fruitc)) * MOWcoeff_fruit;
	}
	else
	{
		/* if LAI before mowing is less than LAI_limit_aftermowing -> no  mowing  */
		MOWcoeff_leaf  = 0.0;
		MOWcoeff_fruit = 0.0;
		MOWcoeff_gresp = 0.0;
	}	
					

	cf->leafc_to_MOW          = cs->leafc * MOWcoeff_leaf;
	cf->leafc_transfer_to_MOW = cs->leafc_transfer * MOWcoeff_leaf * nonactpool_coeff;
	cf->leafc_storage_to_MOW  = cs->leafc_storage * MOWcoeff_leaf * nonactpool_coeff;
	/* fruit simulation - Hidy 2013. */
	cf->fruitc_to_MOW          = cs->fruitc * MOWcoeff_fruit;
	cf->fruitc_transfer_to_MOW = cs->fruitc_transfer * MOWcoeff_fruit * nonactpool_coeff;
	cf->fruitc_storage_to_MOW  = cs->fruitc_transfer * MOWcoeff_fruit * nonactpool_coeff;
	
	cf->gresp_transfer_to_MOW = cs->gresp_transfer * MOWcoeff_gresp * nonactpool_coeff;
	cf->gresp_storage_to_MOW  = cs->gresp_storage * MOWcoeff_gresp * nonactpool_coeff;

	nf->leafn_to_MOW          = ns->leafn * MOWcoeff_leaf;
	nf->leafn_transfer_to_MOW = ns->leafn_transfer * MOWcoeff_leaf * nonactpool_coeff;
	nf->leafn_storage_to_MOW  = ns->leafn_storage * MOWcoeff_leaf * nonactpool_coeff;
	/* fruit simulation - Hidy 2013. */
	nf->fruitn_to_MOW          = ns->fruitn * MOWcoeff_fruit;
	nf->fruitn_transfer_to_MOW = ns->fruitn_transfer * MOWcoeff_fruit * nonactpool_coeff;
	nf->fruitn_storage_to_MOW  = ns->fruitn_storage * MOWcoeff_fruit * nonactpool_coeff;
	/* restranslocated N pool is decreasing also */
	nf->retransn_to_MOW        = ns->retransn * MOWcoeff_leaf * nonactpool_coeff;

	wf->canopyw_to_MOW = ws->canopyw * MOWcoeff_leaf;


	/* if mowed grass remains at the site, returns into the litter */
	cf->MOW_to_litr1c = (cf->leafc_to_MOW * epc->leaflitr_flab + cf->fruitc_to_MOW * epc->fruitlitr_flab + 
					     cf->leafc_transfer_to_MOW  + cf->leafc_storage_to_MOW +
						 cf->fruitc_transfer_to_MOW + cf->fruitc_storage_to_MOW +
					     cf->gresp_storage_to_MOW   + cf->gresp_transfer_to_MOW) * remained_prop;
	cf->MOW_to_litr2c = (cf->leafc_to_MOW * epc->leaflitr_fucel + cf->fruitc_to_MOW * epc->fruitlitr_fucel) * remained_prop;
	cf->MOW_to_litr3c = (cf->leafc_to_MOW * epc->leaflitr_fscel + cf->fruitc_to_MOW * epc->fruitlitr_fscel) * remained_prop;
	cf->MOW_to_litr4c = (cf->leafc_to_MOW * epc->leaflitr_flig  + cf->fruitc_to_MOW * epc->fruitlitr_flig)  * remained_prop;


	nf->MOW_to_litr1n = (nf->leafn_to_MOW * epc->leaflitr_flab + nf->fruitn_to_MOW * epc->fruitlitr_flab + 
					     nf->leafn_transfer_to_MOW  + cf->leafc_storage_to_MOW +
						 nf->fruitn_transfer_to_MOW + cf->fruitc_storage_to_MOW +
						 nf->retransn_to_MOW) * remained_prop;
	nf->MOW_to_litr2n = (nf->leafn_to_MOW * epc->leaflitr_fucel + nf->fruitn_to_MOW * epc->fruitlitr_fucel) * remained_prop;
	nf->MOW_to_litr3n = (nf->leafn_to_MOW * epc->leaflitr_fscel + nf->fruitn_to_MOW * epc->fruitlitr_fscel) * remained_prop;
	nf->MOW_to_litr4n = (nf->leafn_to_MOW * epc->leaflitr_flig  + nf->fruitn_to_MOW * epc->fruitlitr_flig) * remained_prop;
	

	/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                                                    STATE UPDATE 
	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
	
	/* 1. carbon */
	cs->MOWsnk += cf->leafc_to_MOW;
	cs->leafc -= cf->leafc_to_MOW;
	cs->MOWsnk += cf->leafc_transfer_to_MOW;
	cs->leafc_transfer -= cf->leafc_transfer_to_MOW;
	cs->MOWsnk += cf->leafc_storage_to_MOW;
	cs->leafc_storage -= cf->leafc_storage_to_MOW;
	cs->MOWsnk += cf->gresp_transfer_to_MOW;
	cs->gresp_transfer -= cf->gresp_transfer_to_MOW;
	cs->MOWsnk += cf->gresp_storage_to_MOW;
	cs->gresp_storage -= cf->gresp_storage_to_MOW;
	/* fruit simulation - Hidy 2013 */
	cs->MOWsnk += cf->fruitc_to_MOW;
	cs->fruitc -= cf->fruitc_to_MOW;
	cs->MOWsnk += cf->fruitc_transfer_to_MOW;
	cs->fruitc_transfer -= cf->fruitc_transfer_to_MOW;
	cs->MOWsnk += cf->fruitc_storage_to_MOW;
	cs->fruitc_storage -= cf->fruitc_storage_to_MOW;

	cs->litr1c += cf->MOW_to_litr1c;
	cs->litr2c += cf->MOW_to_litr2c;
	cs->litr3c += cf->MOW_to_litr3c;
	cs->litr4c += cf->MOW_to_litr4c;
	
	cs->MOWsrc += cf->MOW_to_litr1c + cf->MOW_to_litr2c + cf->MOW_to_litr3c + cf->MOW_to_litr4c;


	/* 2. nitrogen */
	ns->MOWsnk += nf->leafn_to_MOW;
	ns->leafn -= nf->leafn_to_MOW;
	ns->MOWsnk += nf->leafn_transfer_to_MOW;
	ns->leafn_transfer -= nf->leafn_transfer_to_MOW;
	ns->MOWsnk += nf->leafn_storage_to_MOW;
	ns->leafn_storage -= nf->leafn_storage_to_MOW;	
	/* fruit simulation - Hidy 2013 */
	ns->MOWsnk += nf->fruitn_to_MOW;
	ns->fruitn -= nf->fruitn_to_MOW;
	ns->MOWsnk += nf->fruitn_transfer_to_MOW;
	ns->fruitn_transfer -= nf->fruitn_transfer_to_MOW;
	ns->MOWsnk += nf->fruitn_storage_to_MOW;
	ns->fruitn_storage -= nf->fruitn_storage_to_MOW;	
	ns->MOWsnk += nf->retransn_to_MOW;
	ns->retransn -= nf->retransn_to_MOW;

	ns->litr1n += nf->MOW_to_litr1n;
	ns->litr2n += nf->MOW_to_litr2n;
	ns->litr3n += nf->MOW_to_litr3n;
	ns->litr4n += nf->MOW_to_litr4n;
	
	ns->MOWsrc += nf->MOW_to_litr1n + nf->MOW_to_litr2n + nf->MOW_to_litr3n + nf->MOW_to_litr4n;
	
	/* 3. water */
	ws->canopyw_MOWsnk += wf->canopyw_to_MOW;
	ws->canopyw -= wf->canopyw_to_MOW;

   return (!ok);
}
	