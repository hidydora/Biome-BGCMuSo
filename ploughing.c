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

int ploughing(int yday, phenology_struct* phen, const control_struct* ctrl, const epconst_struct* epc, ploughing_struct* PLG, 
			cflux_struct* cf, nflux_struct* nf, wflux_struct* wf,  cstate_struct* cs, nstate_struct* ns, wstate_struct* ws)
{
	/* ploughing parameters */
	int plough;				/* flag, 1=plough; 0=no plough */
	int count;
	double plough_effect;	/* decrease of plant material caused by ploughing: difference between plant material before and after ploughing */
	int ok=1;

	/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                                                    CALCULATING FLUXES 
	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

	/* ploughing if gapflag=1 */
	if (PLG->PLG_flag == 1)
	{
	
		/* ploughing on PLGdays (ini file) */
		plough=0;
		for(count=0; count < PLG->n_PLGdays; ++count)
		{
			if (yday == PLG->PLGdays[count])
			{
				plough = 1;
				break;
			}
				else
				plough = 0;
		}
		
		/* we assume that as result of the ploughing all the plant material of snags returns to the soil*/
		if (plough) 
		{						
			plough_effect = 1.0;
			if (ctrl->onscreen) printf("ploughing on yearday:%d\t\n",yday);
		}
		else 
		{
			plough_effect = 0.0;
		}

	}
	else
	{
		plough_effect=0.0;
	}

	/* as result of the ploughing no plant material above the ground */
	cf->leafc_to_PLG = cs->leafc * plough_effect;
	cf->leafc_storage_to_PLG = cs->leafc_storage * plough_effect;
	cf->leafc_transfer_to_PLG = cs->leafc_transfer * plough_effect;

	cf->gresp_storage_to_PLG = cs->gresp_storage * plough_effect;
	cf->gresp_transfer_to_PLG = cs->gresp_transfer * plough_effect;

	cf->frootc_to_PLG = cs->frootc * plough_effect;
	cf->frootc_storage_to_PLG = cs->frootc_storage * plough_effect;
	cf->frootc_transfer_to_PLG = cs->frootc_transfer * plough_effect;


	nf->leafn_to_PLG = ns->leafn * plough_effect;
	nf->leafn_storage_to_PLG  = ns->leafn_storage * plough_effect;
	nf->leafn_transfer_to_PLG = ns->leafn_transfer * plough_effect;

	nf->frootn_to_PLG = ns->frootn * plough_effect;
	nf->frootn_storage_to_PLG = ns->frootn_storage * plough_effect;
	nf->frootn_transfer_to_PLG = ns->frootn_transfer * plough_effect;

	wf->canopyw_to_PLG = ws->canopyw * plough_effect;


	/* increasing litter content*/
	/* carbon */
	cf->PLG_to_litr1c = cf->leafc_to_PLG * epc->leaflitr_flab + cf->frootc_to_PLG * epc->frootlitr_flab +
		 cf->leafc_storage_to_PLG + cf->leafc_transfer_to_PLG + cf->gresp_storage_to_PLG + cf->gresp_transfer_to_PLG +
		 cf->frootc_storage_to_PLG + cf->frootc_transfer_to_PLG;
	
	cf->PLG_to_litr2c = cf->leafc_to_PLG* epc->leaflitr_fucel + cf->frootc_to_PLG * epc->frootlitr_fucel;;

	cf->PLG_to_litr3c = cf->leafc_to_PLG * epc->leaflitr_fscel + cf->frootc_to_PLG * epc->frootlitr_fscel;

	cf->PLG_to_litr4c = cf->leafc_to_PLG * epc->leaflitr_flig + cf->frootc_to_PLG * epc->frootlitr_flig;




	/* nitrogen*/
	nf->PLG_to_litr1n = nf->leafn_to_PLG  * epc->leaflitr_flab + nf->frootn_to_PLG * epc->frootlitr_flab
		+ nf->leafn_storage_to_PLG + nf->leafn_transfer_to_PLG + nf->frootn_storage_to_PLG + nf->frootn_transfer_to_PLG;
	
	nf->PLG_to_litr2n = nf->leafn_to_PLG * epc->leaflitr_fucel + nf->frootn_to_PLG * epc->frootlitr_fucel;;

	nf->PLG_to_litr3n = nf->leafn_to_PLG * epc->leaflitr_fscel + nf->frootn_to_PLG * epc->frootlitr_fscel;

	nf->PLG_to_litr4n = nf->leafn_to_PLG * epc->leaflitr_flig + nf->frootn_to_PLG * epc->frootlitr_flig;



	/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                                                    STATE UPDATE 
	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 

	/* carbon */
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

	cs->litr1c += cf->PLG_to_litr1c;
	cs->litr2c += cf->PLG_to_litr2c;
	cs->litr3c += cf->PLG_to_litr3c;
	cs->litr4c += cf->PLG_to_litr4c;
	
	cs->PLGsrc += cf->PLG_to_litr1c + cf->PLG_to_litr2c + cf->PLG_to_litr3c + cf->PLG_to_litr4c;
	
	/* nitrogen */
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

	ns->litr1n += nf->PLG_to_litr1n;
	ns->litr2n += nf->PLG_to_litr2n;
	ns->litr3n += nf->PLG_to_litr3n;
	ns->litr4n += nf->PLG_to_litr4n;
	ns->PLGsrc += nf->PLG_to_litr1n + nf->PLG_to_litr2n + nf->PLG_to_litr3n + nf->PLG_to_litr4n;

	


   return (!ok);
}
	