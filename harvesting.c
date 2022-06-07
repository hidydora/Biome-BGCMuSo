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

int harvesting(int yday, phenology_struct* phen, const control_struct* ctrl, const epconst_struct* epc, harvesting_struct* HRV, 
			cflux_struct* cf, nflux_struct* nf, wflux_struct* wf,  cstate_struct* cs, nstate_struct* ns, wstate_struct* ws)
{
	/* harvesting parameters */
	int harvesting;			/* flag, 1=harvesting; 0=no harvesting */
	int count;
	double befharv_LAI;		/* value of LAI before harvesting */
	double harvest_effect;	/* decrease of plant material caused by harvest: difference between plant material before and after harvesting */
 
	int ok=1;
	double remained_prop = (100 - HRV->transport_coeff)/100.;	/* remained proportion of plabnt material is calculated from transport coefficient */
	

	/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                                                    CALCULATING FLUXES 
	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

	/* harvesting if gapflag=1 */
	if (HRV->HRV_flag == 1)
	{ 
	
		/* harvesting on HRVdays (ini file) */
		harvesting=0;
		for(count=0; count < HRV->n_HRVdays; ++count)
		{
			if (yday == HRV->HRVdays[count])
			{
				if (phen->remdays_curgrowth > 0)
				{
					harvesting = 1;
				}
				else
				{
					harvesting = 0;
					if (ctrl->onscreen) printf("out of growing season no plantmaterial is available - no harvesting on yearday:%d\t\n",yday);
				}
			}
				else
				harvesting = 0;
		}
		

		if (harvesting) 
		{	
		
		/* if before harvesting the value of the LAI is less than LAI_snag (limit value) - > no harvesting
		   if harvest: plant material decreases as the rate of "harvest effect", which is th ratio of LAI before harvest and LAI snag */
				
				befharv_LAI = cs->leafc * epc->avg_proj_sla;
				if (befharv_LAI > HRV->LAI_snag)
				{	
					harvest_effect = 1. - (HRV->LAI_snag/befharv_LAI);			
					if (ctrl->onscreen) printf("harvesting on yearday:%d\t\n",yday);
				}
				else
				{
					harvest_effect = 0.0;
					if (ctrl->onscreen) printf("value of LAI before harvesting is less than LAI_snag - no harvest\n");
				}
					
		}
		else 
		{
			harvest_effect = 0.0;
		}

	}
	else
	{
		harvest_effect=0.0;
	}

	/* as results of the harvesting the carbon, nitrogen and water content of the leaf decreases*/
	cf->leafc_to_HRV          = cs->leafc * harvest_effect;
	cf->leafc_storage_to_HRV  = cs->leafc_storage * harvest_effect;
	cf->leafc_transfer_to_HRV = cs->leafc_transfer * harvest_effect;

	cf->gresp_storage_to_HRV  = cs->gresp_storage * harvest_effect;
	cf->gresp_transfer_to_HRV = cs->gresp_transfer * harvest_effect;


	nf->leafn_to_HRV          = ns->leafn * harvest_effect;
	nf->leafn_storage_to_HRV  = ns->leafn_storage * harvest_effect;;
	nf->leafn_transfer_to_HRV = ns->leafn_transfer * harvest_effect;;

	wf->canopyw_to_HRV = ws->canopyw * harvest_effect;


	/* if harvested plant material remains at the site, returns to the litter */
	cf->HRV_to_litr1c = (cf->leafc_to_HRV * epc->leaflitr_flab + 
					 cf->leafc_transfer_to_HRV + cf->leafc_storage_to_HRV +
					 cf->gresp_storage_to_HRV  + cf->gresp_transfer_to_HRV) * remained_prop;
	cf->HRV_to_litr2c = (cf->leafc_to_HRV * epc->leaflitr_fucel) * remained_prop;
	cf->HRV_to_litr3c = (cf->leafc_to_HRV * epc->leaflitr_fscel) * remained_prop;
	cf->HRV_to_litr4c = (cf->leafc_to_HRV * epc->leaflitr_flig) * remained_prop;


	nf->HRV_to_litr1n = (nf->leafn_to_HRV * epc->leaflitr_flab + 
					 nf->leafn_transfer_to_HRV + cf->leafc_storage_to_HRV) * remained_prop;
	nf->HRV_to_litr2n = (nf->leafn_to_HRV * epc->leaflitr_fucel) * remained_prop;
	nf->HRV_to_litr3n = (nf->leafn_to_HRV * epc->leaflitr_fscel) * remained_prop;
	nf->HRV_to_litr4n = (nf->leafn_to_HRV * epc->leaflitr_flig) * remained_prop;
	

	/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                                                    STATE UPDATE 
	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 

	/* carbon */
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


	cs->litr1c += cf->HRV_to_litr1c;
	cs->litr2c += cf->HRV_to_litr2c;
	cs->litr3c += cf->HRV_to_litr3c;
	cs->litr4c += cf->HRV_to_litr4c;
	
	cs->HRVsrc += cf->HRV_to_litr1c + cf->HRV_to_litr2c + cf->HRV_to_litr3c + cf->HRV_to_litr4c;


	/* nitrogen */
	ns->HRVsnk += nf->leafn_to_HRV;
	ns->leafn -= nf->leafn_to_HRV;
	ns->HRVsnk += nf->leafn_transfer_to_HRV;
	ns->leafn_transfer -= nf->leafn_transfer_to_HRV;
	ns->HRVsnk += nf->leafn_storage_to_HRV;
	ns->leafn_storage -= nf->leafn_storage_to_HRV;

	ns->litr1n += nf->HRV_to_litr1n;
	ns->litr2n += nf->HRV_to_litr2n;
	ns->litr3n += nf->HRV_to_litr3n;
	ns->litr4n += nf->HRV_to_litr4n;
	ns->HRVsrc += nf->HRV_to_litr1n + nf->HRV_to_litr2n + nf->HRV_to_litr3n + nf->HRV_to_litr4n;


   return (!ok);
}
	