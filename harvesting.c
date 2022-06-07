/* 
harvesting.c
do harvesting  - decrease the plant material (leafc, leafn, canopy water)

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

int harvesting(const control_struct* ctrl, const epconst_struct* epc, harvesting_struct* HRV, 
			cflux_struct* cf, nflux_struct* nf, wflux_struct* wf,  cstate_struct* cs, nstate_struct* ns, wstate_struct* ws)
{
	/* harvesting parameters */
	double remained_prop;					/* remained proportion of plant  */
	double LAI_snag;
	double befharv_LAI;						/* value of LAI before harvesting */
	double HRVcoeff_leaf, HRVcoeff_fruit;	/* decrease of plant material caused by harvest: difference between plant material before and after harvesting */
	double HRVcoeff_gresp;					/* coefficient determining the decrease of gresp pools caused by harvest */


	/* local parameters */
	int ok = 1;
	int ny;
	int mgmd = HRV->mgmd;

	/* test variable */
	int nonactpool_coeff=1;

	/* yearly varied or constant management parameters */
	if(HRV->HRV_flag == 2)
	{
		ny = ctrl->simyr;
	}
	else ny=0;

	/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                                                    CALCULATING FLUXES 
	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

	/* harvesting if gapflag=1 */
	if (mgmd >= 0)
	{ 
		remained_prop = (100 - HRV->transport_coeff_array[mgmd][ny])/100.; /* remained prop. of plant mat.is calculated from transport coeff. */
		LAI_snag = HRV->LAI_snag_array[mgmd][ny];
		
	
		/* if before harvesting the value of the LAI is less than LAI_snag (limit value) - > no harvesting
	   if harvest: plant material decreases as the rate of "harvest effect", which is th ratio of LAI before harvest and LAI snag */
			
		befharv_LAI = cs->leafc * epc->avg_proj_sla;
		if (befharv_LAI > LAI_snag)
		{	
			HRVcoeff_leaf  = 1. - (LAI_snag/befharv_LAI);
			HRVcoeff_fruit = 1.0;
			HRVcoeff_gresp = (cs->leafc  / (cs->leafc + cs->frootc + cs->fruitc)) * HRVcoeff_leaf + 
					         (cs->fruitc / (cs->leafc + cs->frootc + cs->fruitc)) * HRVcoeff_fruit;
		}
		else
		{
			HRVcoeff_leaf = 0.0;
			HRVcoeff_fruit = 0.0;
			HRVcoeff_gresp =  0.0;
			if (ctrl->onscreen) printf("value of LAI before harvesting is less than LAI_snag - no harvest\n");
		}
		
	
	

	
	}
	else
	{
		HRVcoeff_leaf  = 0.0;
		HRVcoeff_fruit = 0.0;
		HRVcoeff_gresp =  0.0;

		remained_prop = 0.0;
	}

	/* as results of the harvesting the carbon, nitrogen and water content of the leaf decreases*/
	cf->leafc_to_HRV          = cs->leafc * HRVcoeff_leaf;
	cf->leafc_storage_to_HRV  = cs->leafc_storage * HRVcoeff_leaf * nonactpool_coeff;
	cf->leafc_transfer_to_HRV = cs->leafc_transfer * HRVcoeff_leaf * nonactpool_coeff;

	/* fruit simulation - Hidy 2013. */
	cf->fruitc_to_HRV          = cs->fruitc * HRVcoeff_fruit;
	cf->fruitc_storage_to_HRV  = cs->fruitc_storage * HRVcoeff_fruit * nonactpool_coeff;
	cf->fruitc_transfer_to_HRV = cs->fruitc_transfer * HRVcoeff_fruit * nonactpool_coeff;

	cf->gresp_storage_to_HRV  = cs->gresp_storage * HRVcoeff_gresp * nonactpool_coeff;
	cf->gresp_transfer_to_HRV = cs->gresp_transfer * HRVcoeff_gresp * nonactpool_coeff;

	nf->leafn_to_HRV          = ns->leafn * HRVcoeff_leaf;
	nf->leafn_storage_to_HRV  = ns->leafn_storage * HRVcoeff_leaf * nonactpool_coeff;
	nf->leafn_transfer_to_HRV = ns->leafn_transfer * HRVcoeff_leaf * nonactpool_coeff;

	/* fruit simulation - Hidy 2013. */
	nf->fruitn_to_HRV          = ns->fruitn * HRVcoeff_fruit;
	nf->fruitn_storage_to_HRV  = ns->fruitn_storage * HRVcoeff_fruit * nonactpool_coeff;
	nf->fruitn_transfer_to_HRV = ns->fruitn_transfer * HRVcoeff_fruit * nonactpool_coeff;

	/* restranslocated N pool is decreasing also */
	nf->retransn_to_HRV        = ns->retransn * HRVcoeff_leaf * nonactpool_coeff;

	wf->canopyw_to_HRV = ws->canopyw * HRVcoeff_leaf;


	/* if harvested plant material remains at the site, returns to the litter */
	cf->HRV_to_litr1c = (cf->leafc_to_HRV * epc->leaflitr_flab   + cf->leafc_transfer_to_HRV + cf->leafc_storage_to_HRV + 
						 cf->fruitc_to_HRV * epc->fruitlitr_flab + cf->fruitc_transfer_to_HRV + cf->fruitc_storage_to_HRV + 
						 cf->gresp_storage_to_HRV  + cf->gresp_transfer_to_HRV) * remained_prop;
	cf->HRV_to_litr2c = (cf->leafc_to_HRV * epc->leaflitr_fucel +
						 cf->fruitc_to_HRV * epc->fruitlitr_fucel) * remained_prop;
	cf->HRV_to_litr3c = (cf->leafc_to_HRV * epc->leaflitr_fscel +
						 cf->fruitc_to_HRV * epc->fruitlitr_fscel) * remained_prop;
	cf->HRV_to_litr4c = (cf->leafc_to_HRV * epc->leaflitr_flig +
						 cf->fruitc_to_HRV * epc->fruitlitr_flig) * remained_prop;


	nf->HRV_to_litr1n = (nf->leafn_to_HRV * epc->leaflitr_flab   + nf->leafn_transfer_to_HRV + cf->leafc_storage_to_HRV +
						 nf->fruitn_to_HRV * epc->fruitlitr_flab + nf->fruitn_transfer_to_HRV + cf->fruitc_storage_to_HRV+
						 nf->retransn_to_HRV) * remained_prop;
	nf->HRV_to_litr2n = (nf->leafn_to_HRV * epc->leaflitr_fucel +
						 nf->fruitn_to_HRV * epc->fruitlitr_fucel) * remained_prop;
	nf->HRV_to_litr3n = (nf->leafn_to_HRV * epc->leaflitr_fscel +
						 nf->fruitn_to_HRV * epc->fruitlitr_fscel) * remained_prop;
	nf->HRV_to_litr4n = (nf->leafn_to_HRV * epc->leaflitr_flig +
						 nf->fruitn_to_HRV * epc->fruitlitr_flig) * remained_prop;
	

	/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                                                    STATE UPDATE 
	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 

	/* 1. carbon */
	cs->HRVsnk += cf->leafc_to_HRV;
	cs->leafc -= cf->leafc_to_HRV;
	cs->HRVsnk += cf->leafc_transfer_to_HRV;
	cs->leafc_transfer -= cf->leafc_transfer_to_HRV;
	cs->HRVsnk += cf->leafc_storage_to_HRV;
	cs->leafc_storage -= cf->leafc_storage_to_HRV;
	cs->HRVsnk += cf->gresp_transfer_to_HRV;
	cs->gresp_transfer -= cf->gresp_transfer_to_HRV;
	cs->HRVsnk += cf->gresp_storage_to_HRV;
	cs->gresp_storage -= cf->gresp_storage_to_HRV;
	/* fruit simulation - Hidy 2013. */
	cs->HRVsnk += cf->fruitc_to_HRV;
	cs->fruitc -= cf->fruitc_to_HRV;
	cs->HRVsnk += cf->fruitc_transfer_to_HRV;
	cs->fruitc_transfer -= cf->fruitc_transfer_to_HRV;
	cs->HRVsnk += cf->fruitc_storage_to_HRV;
	cs->fruitc_storage -= cf->fruitc_storage_to_HRV;


	cs->litr1c += cf->HRV_to_litr1c;
	cs->litr2c += cf->HRV_to_litr2c;
	cs->litr3c += cf->HRV_to_litr3c;
	cs->litr4c += cf->HRV_to_litr4c;
	
	cs->HRVsrc += cf->HRV_to_litr1c + cf->HRV_to_litr2c + cf->HRV_to_litr3c + cf->HRV_to_litr4c;


	/* 2. nitrogen */
	ns->HRVsnk += nf->leafn_to_HRV;
	ns->leafn -= nf->leafn_to_HRV;
	ns->HRVsnk += nf->leafn_transfer_to_HRV;
	ns->leafn_transfer -= nf->leafn_transfer_to_HRV;
	ns->HRVsnk += nf->leafn_storage_to_HRV;
	ns->leafn_storage -= nf->leafn_storage_to_HRV;
	/* fruit simulation - Hidy 2013. */
	ns->HRVsnk += nf->fruitn_to_HRV;
	ns->fruitn -= nf->fruitn_to_HRV;
	ns->HRVsnk += nf->fruitn_transfer_to_HRV;
	ns->fruitn_transfer -= nf->fruitn_transfer_to_HRV;
	ns->HRVsnk += nf->fruitn_storage_to_HRV;
	ns->fruitn_storage -= nf->fruitn_storage_to_HRV;
	ns->HRVsnk += nf->retransn_to_HRV;
	ns->retransn -= nf->retransn_to_HRV;

	ns->litr1n += nf->HRV_to_litr1n;
	ns->litr2n += nf->HRV_to_litr2n;
	ns->litr3n += nf->HRV_to_litr3n;
	ns->litr4n += nf->HRV_to_litr4n;
	ns->HRVsrc += nf->HRV_to_litr1n + nf->HRV_to_litr2n + nf->HRV_to_litr3n + nf->HRV_to_litr4n;

	/* 3. water */
	ws->canopyw_HRVsnk += wf->canopyw_to_HRV;
	ws->canopyw -= wf->canopyw_to_HRV;

   return (!ok);
}
	