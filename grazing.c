/* 
grazing.c
grazing  - decrease the plant material (leafc, leafn, canopy water) and increase N-content (mature)

method: Vuichard et al, 2007
NOTE: LSU: livestock unit = unit used to compare or aggregate different species and it is equivalnet to 500 kg live weight (1 adult cattle = 1 LSU)

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

int grazing(int yday, phenology_struct* phen, const control_struct* ctrl, const epconst_struct* epc, grazing_struct* GRZ, 
			cflux_struct* cf, nflux_struct* nf, wflux_struct* wf,  cstate_struct* cs, nstate_struct* ns, wstate_struct* ws)
{

	/* local variables */
	int grazing = 0;						/* flag, 1=grazing; 0=no grazing */
	double befgrazing_leafc = 0;			/* value of leafc before grazing */
	double aftergrazing_leafc = 0;			/* value of leafc before grazing */
	double grazing_effect = 0;				/* decreasing of plant material caused by grazing: plant_material_before_grazing - plant_material_after_grazing  */
	double rate_of_removal = 0;				/* if grazing calculation based on LSU, a fixed proportion of the abovegroind biomass is removed */
	double daily_excrement_production = 0;	/* daily excrement production */	
	double daily_C_loss = 0;				/* daily carbon loss due to grazing */
	double Cplus_from_excrement = 0;		/* daily carbon plus from excrement */
	double Nplus_from_excrement = 0;        /* daily nitrogen plus from excrement */

	int first_day_of_GRZ = GRZ->first_day_of_GRZ;
	int last_day_of_GRZ = GRZ->last_day_of_GRZ;
	int grazing_period = last_day_of_GRZ - first_day_of_GRZ;

	double drymatter_intake = GRZ->drymatter_intake;			  /*  unit: kgDM/LSU (DM:dry matter)*/
	double stocking_rate = GRZ->stocking_rate  / 10000;			 /*  unit: LSU/ha -> new unit: LSU/m2 */


	double prop_DMintake_formed_excrement = GRZ->prop_DMintake_formed_excrement / 100.;	/* from proporiton(%) to ratio(number) */
	double C_content_of_drymatter = GRZ->C_content_of_drymatter / 100.;				    /* from proporiton(%) to ratio(number) */
	double C_content_of_excrement = GRZ->C_content_of_excrement / 100.;					/* from proporiton(%) to ratio(number) */
	double N_content_of_excrement = GRZ->N_content_of_excrement / 100.;					/* from proporiton(%) to ratio(number) */
    double prop_excrement_return2litter = GRZ->prop_excrement_return2litter / 100;

	

	double grazing_limit = 0.01;	/* limitation for carbon content before grazing in order to avoid negative leaf carbon content */
	
	int ok=1;

	/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                                                    CALCULATING FLUXES 
	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

	/* grazing if gapflag=1 */
	if (GRZ->GRZ_flag == 1)
	{
	
			grazing=0;	/* flag, 1=grazing; 0=no grazing */
			
			if (yday >= first_day_of_GRZ && yday <= last_day_of_GRZ )
			{
				if (phen->remdays_curgrowth > 0)
				{
					grazing = 1;
				}
				else
				{
					grazing = 0;
					if (ctrl->onscreen) printf("out of growing season no plantmaterial is available - no grazing on yearday:%d\t\n",yday);
				}
		
			}
			else
				grazing = 0;
			
	}
				

	if (grazing) 
	{	
		
		/* daily total ingested carbon per m2 from daily ingested drymatter and carbon content of drymatter and stocking rate
						[kgC/m2 = kgDM/LSU * (kgC/kgDM) * (LSU/m2)] */	
		daily_C_loss = (drymatter_intake * C_content_of_drymatter) * stocking_rate;	
	
		
		/* effect of grazing: decrease of leafc and increase of soilc and soiln (manure)*/
		befgrazing_leafc = cs->leafc;
		if (befgrazing_leafc - daily_C_loss > grazing_limit)
		{
			
			aftergrazing_leafc = befgrazing_leafc - daily_C_loss;
			grazing_effect = 1-aftergrazing_leafc/befgrazing_leafc;	
			if (ctrl->onscreen && yday == first_day_of_GRZ) printf("start of grazing on yearday:%d\t\n",yday);
			if (ctrl->onscreen && yday == last_day_of_GRZ) printf("end of grazing on yearday:%d\t\n",yday);
		
		}
		else
		{
			aftergrazing_leafc = 0.0;
			grazing_effect = 0.0;
			daily_C_loss = 0;
			daily_excrement_production = 0;
			Cplus_from_excrement = 0;
			Nplus_from_excrement = 0;
			if (ctrl->onscreen) printf("not enough grass for grazing on yearday:%d\t\n",yday);
		}
			
				
	}
	else 
	{
		grazing_effect = 0.0;
		daily_C_loss = 0;
		daily_excrement_production = 0;
		Cplus_from_excrement = 0;
		Nplus_from_excrement = 0;
	}


	/* daily manure production per m2 (return to the litter) from daily total ingested dry matter and litter_return_ratio and its C and N content
					[kgMANURE = (kgDM/LSU) * (LSU/m2) * (%)] */
	daily_excrement_production = (daily_C_loss/C_content_of_drymatter) * prop_DMintake_formed_excrement;	 /* unit change: kg manure/m2/day -> kgC/m2/day */
	Cplus_from_excrement = daily_excrement_production * C_content_of_excrement;
	Nplus_from_excrement = daily_excrement_production * N_content_of_excrement;

	/* carbon loss from grazing is determined using grazing_effect */
	cf->leafc_to_GRZ = cs->leafc * grazing_effect;
	cf->leafc_storage_to_GRZ = 0;
	cf->leafc_transfer_to_GRZ = 0;

	cf->gresp_storage_to_GRZ = 0;
	cf->gresp_transfer_to_GRZ = 0;
	
	nf->leafn_to_GRZ = ns->leafn * grazing_effect;
	nf->leafn_storage_to_GRZ = 0;
	nf->leafn_transfer_to_GRZ = 0;
	 
	wf->canopyw_to_GRZ = ws->canopyw * grazing_effect;

	/* if grazing manure production is taken account (plant material goes into the litter pool) - surplus to ecosystem is loss for "atmosphere" - negatice sign*/	
	cf->GRZ_to_litr1c = (Cplus_from_excrement) * epc->leaflitr_flab * prop_excrement_return2litter;
	cf->GRZ_to_litr2c = (Cplus_from_excrement) * epc->leaflitr_fucel * prop_excrement_return2litter;
	cf->GRZ_to_litr3c = (Cplus_from_excrement) * epc->leaflitr_fscel * prop_excrement_return2litter;
	cf->GRZ_to_litr4c = (Cplus_from_excrement) * epc->leaflitr_flig * prop_excrement_return2litter;

	nf->GRZ_to_litr1n = (Nplus_from_excrement) * epc->leaflitr_flab * prop_excrement_return2litter;  
	nf->GRZ_to_litr2n = (Nplus_from_excrement) * epc->leaflitr_fucel * prop_excrement_return2litter; 
	nf->GRZ_to_litr3n = (Nplus_from_excrement) * epc->leaflitr_fscel * prop_excrement_return2litter; 
	nf->GRZ_to_litr4n = (Nplus_from_excrement) * epc->leaflitr_flig * prop_excrement_return2litter;

	
	/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                                                    STATE UPDATE 
	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 

	/* carbon */	
	cs->GRZsnk += cf->leafc_to_GRZ;
	cs->leafc -= cf->leafc_to_GRZ;
	cs->GRZsnk += cf->leafc_transfer_to_GRZ;
	cs->leafc_transfer -= cf->leafc_transfer_to_GRZ;
	cs->GRZsnk += cf->leafc_storage_to_GRZ;
	cs->leafc_storage -= cf->leafc_storage_to_GRZ;
	cs->GRZsnk += cf->gresp_transfer_to_GRZ;
	
	cs->gresp_transfer -= cf->gresp_transfer_to_GRZ;
	cs->GRZsnk += cf->gresp_storage_to_GRZ;
	cs->gresp_storage -= cf->gresp_storage_to_GRZ;
	
	cs->litr1c += cf->GRZ_to_litr1c;
	cs->litr2c += cf->GRZ_to_litr2c;
	cs->litr3c += cf->GRZ_to_litr3c;
	cs->litr4c += cf->GRZ_to_litr4c;
	
	cs->GRZsrc += cf->GRZ_to_litr1c + cf->GRZ_to_litr2c + cf->GRZ_to_litr3c + cf->GRZ_to_litr4c;

	/* nitrogen */
	ns->GRZsnk += nf->leafn_to_GRZ;
	ns->leafn -= nf->leafn_to_GRZ;
	ns->GRZsnk += nf->leafn_transfer_to_GRZ;
	ns->leafn_transfer -= nf->leafn_transfer_to_GRZ;
	ns->GRZsnk += nf->leafn_storage_to_GRZ;
	ns->leafn_storage -= nf->leafn_storage_to_GRZ;
	
	ns->litr1n += nf->GRZ_to_litr1n;
	ns->litr2n += nf->GRZ_to_litr2n;
	ns->litr3n += nf->GRZ_to_litr3n;
	ns->litr4n += nf->GRZ_to_litr4n;
	
	ns->GRZsrc += nf->GRZ_to_litr1n + nf->GRZ_to_litr2n + nf->GRZ_to_litr3n + nf->GRZ_to_litr4n;

	ws->canopyw_GRZsnk += wf->canopyw_to_GRZ;
	ws->canopyw -= wf->canopyw_to_GRZ;
	
   return (!ok);
}
	