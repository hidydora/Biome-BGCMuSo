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

int grazing(const control_struct* ctrl, const epconst_struct* epc, grazing_struct* GRZ, 
			cflux_struct* cf, nflux_struct* nf, wflux_struct* wf,  cstate_struct* cs, nstate_struct* ns, wstate_struct* ws)
{

	/* grazing parameters */
	int ny;
	double grazing_period;
	double DMintake;		
	double stocking_rate;			

	/* local variables */
	double prop_DMintake2excr;	
	double DM_Ccontent;				    
	double EXCR_Ccontent_array;				
	double EXCR_Ncontent_array;				
    double prop_excr2litter;				/* proportion of excrement return to litter */
	double befgrazing_leafc = 0;			/* value of leafc before grazing */
	double aftergrazing_leafc = 0;			/* value of leafc before grazing */
	double grazing_effect = 0;				/* decreasing of plant material caused by grazing: plant_material_before_grazing - plant_material_after_grazing  */
	double rate_of_removal = 0;				/* if grazing calculation based on LSU, a fixed proportion of the abovegroind biomass is removed */
	double daily_excr_prod = 0;	/* daily excrement production */	
	double daily_C_loss = 0;				/* daily carbon loss due to grazing */
	double Cplus_from_excrement = 0;		/* daily carbon plus from excrement */
	double Nplus_from_excrement = 0;        /* daily nitrogen plus from excrement */


	int mgmd = GRZ->mgmd;
	

	double grazing_limit = 0.01;	/* limitation for carbon content before grazing in order to avoid negative leaf carbon content */
	
	int ok=1;


	/* yearly varied or constant management parameters */
	if(GRZ->GRZ_flag == 2)
	{
		ny = ctrl->simyr;
	}
	else ny=0;

	/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                                                    CALCULATING FLUXES 
	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/


	if (mgmd >= 0) 
	{
		grazing_period		= GRZ->GRZ_end_array[mgmd][ny] - GRZ->GRZ_start_array[mgmd][ny];

		DMintake            = GRZ->DMintake_array[mgmd][ny];						 /*  unit: kgDM/LSU (DM:dry matter)*/
		stocking_rate       = GRZ->stocking_rate_array[mgmd][ny]/10000;				 /*  unit: LSU/ha -> new unit: LSU/m2 */


		prop_DMintake2excr  = GRZ->prop_DMintake2excr_array[mgmd][ny] / 100.;	     /* from proporiton(%) to ratio(number) */
		DM_Ccontent         = GRZ->DM_Ccontent_array[mgmd][ny] / 100.;				 /* from proporiton(%) to ratio(number) */
		EXCR_Ccontent_array = GRZ->EXCR_Ccontent_array[mgmd][ny] / 100.;			 /* from proporiton(%) to ratio(number) */
		EXCR_Ncontent_array = GRZ->EXCR_Ncontent_array[mgmd][ny] / 100.;			 /* from proporiton(%) to ratio(number) */
		prop_excr2litter    = GRZ->prop_excr2litter_array[mgmd][ny] / 100;
		
		/* daily total ingested carbon per m2 from daily ingested drymatter and carbon content of drymatter and stocking rate
						[kgC/m2 = kgDM/LSU * (kgC/kgDM) * (LSU/m2)] */	
		daily_C_loss = (DMintake * DM_Ccontent) * stocking_rate;	
	
		
		/* effect of grazing: decrease of leafc and increase of soilc and soiln (manure)*/
		befgrazing_leafc = cs->leafc;
		if (befgrazing_leafc - daily_C_loss > grazing_limit)
		{
			
			aftergrazing_leafc = befgrazing_leafc - daily_C_loss;
			grazing_effect = 1-aftergrazing_leafc/befgrazing_leafc;	
			daily_excr_prod = (daily_C_loss/DM_Ccontent) * prop_DMintake2excr;/* kg manure/m2/day -> kgC/m2/day */
		
		}
		else
		{
			grazing_effect = 0.0;
			daily_excr_prod = 0;
			prop_excr2litter = 0;
			if (ctrl->onscreen) printf("not enough grass for grazing\n");
		}
			
				
	}
	else 
	{
		grazing_effect = 0.0;
		daily_excr_prod = 0;
		prop_excr2litter = 0;
		EXCR_Ccontent_array = 0;
		EXCR_Ncontent_array = 0;
	}


	/* daily manure production per m2 (return to the litter) from daily total ingested dry matter and litter_return_ratio and its C and N content
					[kgMANURE = (kgDM/LSU) * (LSU/m2) * (%)] */

	Cplus_from_excrement = daily_excr_prod * EXCR_Ccontent_array;
	Nplus_from_excrement = daily_excr_prod * EXCR_Ncontent_array;

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
	cf->GRZ_to_litr1c = (Cplus_from_excrement) * epc->leaflitr_flab * prop_excr2litter;
	cf->GRZ_to_litr2c = (Cplus_from_excrement) * epc->leaflitr_fucel * prop_excr2litter;
	cf->GRZ_to_litr3c = (Cplus_from_excrement) * epc->leaflitr_fscel * prop_excr2litter;
	cf->GRZ_to_litr4c = (Cplus_from_excrement) * epc->leaflitr_flig * prop_excr2litter;

	nf->GRZ_to_litr1n = (Nplus_from_excrement) * epc->leaflitr_flab * prop_excr2litter;  
	nf->GRZ_to_litr2n = (Nplus_from_excrement) * epc->leaflitr_fucel * prop_excr2litter; 
	nf->GRZ_to_litr3n = (Nplus_from_excrement) * epc->leaflitr_fscel * prop_excr2litter; 
	nf->GRZ_to_litr4n = (Nplus_from_excrement) * epc->leaflitr_flig * prop_excr2litter;


	
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
	