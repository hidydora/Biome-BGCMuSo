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
	int mowing;						/* flag, 1=mowing; 0=no mowing */
	double LAI_limit;
	double remained_prop;			/* remained proportion of plabnt material is calculated from transport coefficient */

	/* local parameters */
	double befgrass_LAI;			/* value of LAI before mowing */
	double mowing_effect;				/* decreasing of plant material caused by mowing*/


	int ok=1;
	int ny;
	int mgmd = MOW->mgmd;

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
		mowing_effect = (1- LAI_limit/befgrass_LAI);			
	}
	else
	{
		/* if LAI before mowing is less than LAI_limit_aftermowing -> no  mowing  */
		mowing_effect = 0.0;
	}	
					

	cf->leafc_to_MOW          = cs->leafc * mowing_effect;
	cf->leafc_transfer_to_MOW = 0;
	cf->leafc_storage_to_MOW  = 0;
	
	cf->gresp_transfer_to_MOW = 0;
	cf->gresp_storage_to_MOW  = 0;

	nf->leafn_to_MOW          = ns->leafn * mowing_effect;
	nf->leafn_transfer_to_MOW = 0;
	nf->leafn_storage_to_MOW  = 0;

	wf->canopyw_to_MOW = ws->canopyw * mowing_effect;


	/* if mowed grass remains at the site, returns into the litter */
	cf->MOW_to_litr1c = (cf->leafc_to_MOW * epc->leaflitr_flab + 
					 cf->leafc_transfer_to_MOW + cf->leafc_storage_to_MOW +
					 cf->gresp_storage_to_MOW  + cf->gresp_transfer_to_MOW) * remained_prop;
	cf->MOW_to_litr2c = (cf->leafc_to_MOW * epc->leaflitr_fucel) * remained_prop;
	cf->MOW_to_litr3c = (cf->leafc_to_MOW * epc->leaflitr_fscel) * remained_prop;
	cf->MOW_to_litr4c = (cf->leafc_to_MOW * epc->leaflitr_flig) * remained_prop;


	nf->MOW_to_litr1n = (nf->leafn_to_MOW * epc->leaflitr_flab + 
					 nf->leafn_transfer_to_MOW + cf->leafc_storage_to_MOW) * remained_prop;
	nf->MOW_to_litr2n = (nf->leafn_to_MOW * epc->leaflitr_fucel) * remained_prop;
	nf->MOW_to_litr3n = (nf->leafn_to_MOW * epc->leaflitr_fscel) * remained_prop;
	nf->MOW_to_litr4n = (nf->leafn_to_MOW * epc->leaflitr_flig) * remained_prop;
	

	/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                                                    STATE UPDATE 
	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
	
	/* carbon */
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
	
	cs->litr1c += cf->MOW_to_litr1c;
	cs->litr2c += cf->MOW_to_litr2c;
	cs->litr3c += cf->MOW_to_litr3c;
	cs->litr4c += cf->MOW_to_litr4c;
	
	cs->MOWsrc += cf->MOW_to_litr1c + cf->MOW_to_litr2c + cf->MOW_to_litr3c + cf->MOW_to_litr4c;


	/* nitrogen */
	ns->MOWsnk += nf->leafn_to_MOW;
	ns->leafn -= nf->leafn_to_MOW;
	ns->MOWsnk += nf->leafn_transfer_to_MOW;
	ns->leafn_transfer -= nf->leafn_transfer_to_MOW;
	ns->MOWsnk += nf->leafn_storage_to_MOW;
	ns->leafn_storage -= nf->leafn_storage_to_MOW;	
	
	ns->litr1n += nf->MOW_to_litr1n;
	ns->litr2n += nf->MOW_to_litr2n;
	ns->litr3n += nf->MOW_to_litr3n;
	ns->litr4n += nf->MOW_to_litr4n;
	
	ns->MOWsrc += nf->MOW_to_litr1n + nf->MOW_to_litr2n + nf->MOW_to_litr3n + nf->MOW_to_litr4n;

	ws->canopyw_MOWsnk += wf->canopyw_to_MOW;
	ws->canopyw -= wf->canopyw_to_MOW;

   return (!ok);
}
	