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

int mowing(int yday, phenology_struct* phen, const control_struct* ctrl, const epconst_struct* epc, mowing_struct* MOW, cflux_struct* cf,
				 nflux_struct* nf, wflux_struct* wf, cstate_struct* cs, nstate_struct* ns, wstate_struct* ws)
{
	/* mowing parameters */
	int mowing;		/* flag, 1=mowing; 0=no mowing */
	int mowi;
	
	double befgrass_LAI;										/* value of LAI before mowing */
	double mowing_rate;										/* decreasing of plant material caused by mowing*/
	double remained_prop = (100 - MOW->transport_coeff)/100.;	/* remained proportion of plabnt material is calculated from transport coefficient */

	int ok=1;

	/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                                                    CALCULATING FLUXES 
	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
	
	/* mowing if flag=1 */
	if (MOW->MOW_flag == 1)
	{
	
		mowing=0;

		/* mowing type: fixday or LAIlimit */
		if (MOW->fixday_or_fixLAI_flag == 0)
		{
			for(mowi=0; mowi < MOW->n_MOWdays; ++mowi)
			{
				if (yday == MOW->MOWdays[mowi])
				{
					if (phen->remdays_curgrowth > 0)
					{
						mowing = 1;
						break;
					}
					else
					{
						mowing = 0;
						if (ctrl->onscreen) printf("out of growing season no plantmaterial is available - no mowing on yearday:%d\t\n",yday);
					}
				}
					else
					mowing = 0;
			}
		}
		else
		{
			/* mowing if LAI greater than a given value (fixLAI_befMOW) */
			if (cs->leafc * epc->avg_proj_sla > MOW->fixLAI_befMOW)
			{
				mowing = 1;
			}
			else
			{
				mowing = 0;
			}
		}
		


		//////////////////////////////////////////////////////////////////////////////////////////////////////////////
		if (mowing) 
		{	
			/* mowing calculation based on LAI_limit: we assume that due to the mowing LAI (leafc) reduces to the value of LAI_limit  */
			befgrass_LAI = cs->leafc * epc->avg_proj_sla;
			if (befgrass_LAI > MOW->LAI_limit)
			{	
				mowing_rate = 1. - (MOW->LAI_limit/befgrass_LAI);			
				if (ctrl->onscreen) printf("mowing (LAI limit) on yearday:%d\t\n",yday);
			}
			else
			{
				/* if LAI before mowing is less than LAI_limit_aftermowing -> no  mowing  */
				mowing_rate = 0.0;
				if (ctrl->onscreen) printf("value of LAI before mowing is less than LAI_limit - no mowing on yearday:%d\t\n",yday);
			}	
						
		}
		else 
		{
			mowing_rate = 0.0;
		}
	}
	else
	{
		mowing_rate=0.0;
	}

	cf->leafc_to_MOW          = cs->leafc * mowing_rate;
	cf->leafc_transfer_to_MOW = 0;
	cf->leafc_storage_to_MOW  = 0;
	
	cf->gresp_transfer_to_MOW = 0;
	cf->gresp_storage_to_MOW  = 0;

	nf->leafn_to_MOW          = ns->leafn * mowing_rate;
	nf->leafn_transfer_to_MOW = 0;
	nf->leafn_storage_to_MOW  = 0;

	wf->canopyw_to_MOW = ws->canopyw * mowing_rate;


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

   return (!ok);
}
	