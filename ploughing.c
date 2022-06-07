/* 
ploughing.c
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

int ploughing(const control_struct* ctrl, const epconst_struct* epc, ploughing_struct* PLG, 
			cflux_struct* cf, nflux_struct* nf, wflux_struct* wf,  cstate_struct* cs, nstate_struct* ns, wstate_struct* ws)
{
	/* ploughing parameters */
	double PLGcoeff;	/* decrease of plant material caused by ploughing: difference between plant material before and after harvesting */

	int ok=1;
	int ny;
	int mgmd = PLG->mgmd;

	/* test variable */
	int nonactpool_coeff=1;


	/* yearly varied or constant management parameters */
	if(PLG->PLG_flag == 2)
	{
		ny = ctrl->simyr;
	}
	else ny=0;

	/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                                                    CALCULATING FLUXES 
	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

	/* ploughing if gapflag=1 */
	if (mgmd >=0)
	{
		PLGcoeff = 1.0;
	}
	else
	{
		PLGcoeff=0.0;
	}

	/* as result of the ploughing no plant material above the ground */
	cf->leafc_to_PLG = cs->leafc * PLGcoeff;
	cf->leafc_storage_to_PLG = cs->leafc_storage * PLGcoeff * nonactpool_coeff;
	cf->leafc_transfer_to_PLG = cs->leafc_transfer * PLGcoeff * nonactpool_coeff;

	cf->gresp_storage_to_PLG = cs->gresp_storage * PLGcoeff * nonactpool_coeff;
	cf->gresp_transfer_to_PLG = cs->gresp_transfer * PLGcoeff * nonactpool_coeff;

	cf->frootc_to_PLG = cs->frootc * PLGcoeff;
	cf->frootc_storage_to_PLG = cs->frootc_storage * PLGcoeff * nonactpool_coeff;
	cf->frootc_transfer_to_PLG = cs->frootc_transfer * PLGcoeff * nonactpool_coeff;

	/* fruit simulation - Hidy 2013 */
	cf->fruitc_to_PLG = cs->fruitc * PLGcoeff;
	cf->fruitc_storage_to_PLG = cs->fruitc_storage * PLGcoeff * nonactpool_coeff;
	cf->fruitc_transfer_to_PLG = cs->fruitc_transfer * PLGcoeff * nonactpool_coeff;

	nf->leafn_to_PLG = ns->leafn * PLGcoeff;
	nf->leafn_storage_to_PLG  = ns->leafn_storage * PLGcoeff * nonactpool_coeff;
	nf->leafn_transfer_to_PLG = ns->leafn_transfer * PLGcoeff * nonactpool_coeff;

	nf->frootn_to_PLG = ns->frootn * PLGcoeff;
	nf->frootn_storage_to_PLG = ns->frootn_storage * PLGcoeff * nonactpool_coeff;
	nf->frootn_transfer_to_PLG = ns->frootn_transfer * PLGcoeff * nonactpool_coeff;

	/* fruit simulation - Hidy 2013 */
	nf->fruitn_to_PLG = ns->fruitn * PLGcoeff;
	nf->fruitn_storage_to_PLG = ns->fruitn_storage * PLGcoeff * nonactpool_coeff;
	nf->fruitn_transfer_to_PLG = ns->fruitn_transfer * PLGcoeff * nonactpool_coeff;

	/* restranslocated N pool is decreasing also */
	nf->retransn_to_PLG = ns->retransn * PLGcoeff * nonactpool_coeff;

	wf->canopyw_to_PLG = ws->canopyw * PLGcoeff;


	/* increasing litter content*/
	/* carbon */
	cf->PLG_to_litr1c = cf->leafc_to_PLG * epc->leaflitr_flab + 
						cf->frootc_to_PLG * epc->frootlitr_flab +
						cf->fruitc_to_PLG * epc->fruitlitr_flab +
						cf->leafc_storage_to_PLG + cf->leafc_transfer_to_PLG +
						cf->frootc_storage_to_PLG + cf->frootc_transfer_to_PLG +
						cf->fruitc_storage_to_PLG + cf->fruitc_transfer_to_PLG +
						cf->gresp_storage_to_PLG + cf->gresp_transfer_to_PLG;
	
	cf->PLG_to_litr2c = cf->leafc_to_PLG* epc->leaflitr_fucel + 
						cf->frootc_to_PLG * epc->frootlitr_fucel +
						cf->fruitc_to_PLG * epc->fruitlitr_fucel;

	cf->PLG_to_litr3c = cf->leafc_to_PLG * epc->leaflitr_fscel + 
						cf->frootc_to_PLG * epc->frootlitr_fscel +
						cf->frootc_to_PLG * epc->frootlitr_fscel;

	cf->PLG_to_litr4c = cf->leafc_to_PLG * epc->leaflitr_flig + 
						cf->frootc_to_PLG * epc->frootlitr_flig +
						cf->fruitc_to_PLG * epc->fruitlitr_flig;




	/* nitrogen*/
	nf->PLG_to_litr1n = nf->leafn_to_PLG * epc->leaflitr_flab + 
						nf->frootn_to_PLG * epc->frootlitr_flab +
						nf->fruitn_to_PLG * epc->fruitlitr_flab +
						nf->leafn_storage_to_PLG + nf->leafn_transfer_to_PLG +
						nf->frootn_storage_to_PLG + nf->frootn_transfer_to_PLG +
						nf->fruitn_storage_to_PLG + nf->fruitn_transfer_to_PLG + 
						nf->retransn_to_PLG;
	
	nf->PLG_to_litr2n = nf->leafn_to_PLG* epc->leaflitr_fucel + 
						nf->frootn_to_PLG * epc->frootlitr_fucel +
						nf->fruitn_to_PLG * epc->fruitlitr_fucel;

	nf->PLG_to_litr3n = nf->leafn_to_PLG * epc->leaflitr_fscel + 
						nf->frootn_to_PLG * epc->frootlitr_fscel +
						nf->frootn_to_PLG * epc->frootlitr_fscel;

	nf->PLG_to_litr4n = nf->leafn_to_PLG * epc->leaflitr_flig + 
						nf->frootn_to_PLG * epc->frootlitr_flig +
						nf->fruitn_to_PLG * epc->fruitlitr_flig;


	/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                                                    STATE UPDATE 
	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 

	/* 1. carbon */
	cs->PLGsnk += cf->leafc_to_PLG;
	cs->leafc -= cf->leafc_to_PLG;
	cs->PLGsnk += cf->leafc_transfer_to_PLG;
	cs->leafc_transfer -= cf->leafc_transfer_to_PLG;
	cs->PLGsnk += cf->leafc_storage_to_PLG;
	cs->leafc_storage -= cf->leafc_storage_to_PLG;
	cs->PLGsnk += cf->gresp_transfer_to_PLG;
	
	cs->gresp_transfer -= cf->gresp_transfer_to_PLG;
	cs->PLGsnk += cf->gresp_storage_to_PLG;
	cs->gresp_storage -= cf->gresp_storage_to_PLG;
	
	cs->PLGsnk += cf->frootc_to_PLG;
	cs->frootc -= cf->frootc_to_PLG;
	cs->PLGsnk += cf->frootc_transfer_to_PLG;
	cs->frootc_transfer -= cf->frootc_transfer_to_PLG;
	cs->PLGsnk += cf->frootc_storage_to_PLG;
	cs->frootc_storage -= cf->frootc_storage_to_PLG;

	/* fruit simulation - Hidy 2013. */
	cs->PLGsnk += cf->fruitc_to_PLG;
	cs->fruitc -= cf->fruitc_to_PLG;
	cs->PLGsnk += cf->fruitc_transfer_to_PLG;
	cs->fruitc_transfer -= cf->fruitc_transfer_to_PLG;
	cs->PLGsnk += cf->fruitc_storage_to_PLG;
	cs->fruitc_storage -= cf->fruitc_storage_to_PLG;

	cs->litr1c += cf->PLG_to_litr1c;
	cs->litr2c += cf->PLG_to_litr2c;
	cs->litr3c += cf->PLG_to_litr3c;
	cs->litr4c += cf->PLG_to_litr4c;
	
	cs->PLGsrc += cf->PLG_to_litr1c + cf->PLG_to_litr2c + cf->PLG_to_litr3c + cf->PLG_to_litr4c;
	
	/* 2. nitrogen */
	ns->PLGsnk += nf->leafn_to_PLG;
	ns->leafn -= nf->leafn_to_PLG;
	ns->PLGsnk += nf->leafn_transfer_to_PLG;
	ns->leafn_transfer -= nf->leafn_transfer_to_PLG;
	ns->PLGsnk += nf->leafn_storage_to_PLG;
	ns->leafn_storage -= nf->leafn_storage_to_PLG;
	
	ns->PLGsnk += nf->frootn_to_PLG;
	ns->frootn -= nf->frootn_to_PLG;
	ns->PLGsnk += nf->frootn_transfer_to_PLG;
	ns->frootn_transfer -= nf->frootn_transfer_to_PLG;
	ns->PLGsnk += nf->frootn_storage_to_PLG;
	ns->frootn_storage -= nf->frootn_storage_to_PLG;

	/* fruit simulation - Hidy 2013. */
	ns->PLGsnk += nf->fruitn_to_PLG;
	ns->fruitn -= nf->fruitn_to_PLG;
	ns->PLGsnk += nf->fruitn_transfer_to_PLG;
	ns->fruitn_transfer -= nf->fruitn_transfer_to_PLG;
	ns->PLGsnk += nf->fruitn_storage_to_PLG;
	ns->fruitn_storage -= nf->fruitn_storage_to_PLG;
	ns->PLGsnk += nf->retransn_to_PLG;
	ns->retransn -= nf->retransn_to_PLG;

	ns->litr1n += nf->PLG_to_litr1n;
	ns->litr2n += nf->PLG_to_litr2n;
	ns->litr3n += nf->PLG_to_litr3n;
	ns->litr4n += nf->PLG_to_litr4n;
	ns->PLGsrc += nf->PLG_to_litr1n + nf->PLG_to_litr2n + nf->PLG_to_litr3n + nf->PLG_to_litr4n;

	/* 3. water */
	ws->canopyw_PLGsnk += wf->canopyw_to_PLG;
	ws->canopyw -= wf->canopyw_to_PLG;
	ws->soilw[0] += wf->canopyw_to_PLG;


   return (!ok);
}
	