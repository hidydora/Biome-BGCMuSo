/* 
fertilizing.c
do fertilization  - increase the mineral soil nitrogen (sminn)

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

int fertilizing(int yday, const control_struct* ctrl, fertilizing_struct* FRZ, 
				cstate_struct* cs, nstate_struct*ns, cflux_struct* cf, nflux_struct* nf)
{

	/* fertilizing parameters .*/
	int fertil;
	int ok=1;
	double nitrate_content = FRZ->nitrate_content / 100.;		/* from % to number */
	double ammonium_content = FRZ->ammonium_content / 100.;	/* from % to number */
	double carbon_content = FRZ->carbon_content / 100.;		/* from % to number */
	double litr_flab = FRZ->litr_flab / 100.;					/* from % to number */
	double litr_fucel = FRZ->litr_fucel / 100.;				/* from % to number */
	double litr_fscel = FRZ->litr_fscel / 100.;				/* from % to number */
	double litr_flig = FRZ->litr_flig / 100.;					/* from % to number */
	double utilization_coeff = FRZ->utilization_coeff / 100.;	/* from % to number */
	double dissolv_coeff = FRZ->dissolv_coeff /100;			/* from % to number */
	double FRZ_to_sminn_act = 0;
	double FRZ_to_litrn_act = 0;
	double FRZ_to_litrc_act = 0;
	
	/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                                                    CALCULATING FLUXES 
	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
	
	/* do fertilization if gapflag=1  - Hidy 2008.*/
	if (FRZ->FRZ_flag)
	{
	
		for(fertil=0; fertil < FRZ->n_FRZdays; ++fertil)
		{
			if (yday == FRZ->FRZdays[fertil])
			{
				/* on fertilization days a given amunt of fertilizer is put out on the ground (FRZ_pool_act contains the actual fertilizer content) */	
				FRZ->FRZ_pool_act += FRZ->fertilizer[fertil] / 10000.; // kgN/ha -> kgN/m2
				
				if (ctrl->onscreen) printf("fertilizing on yearday:%d\t\n",yday);
				break;
			}
		}

				
		if (FRZ->FRZ_pool_act> 0.0)
		{
			/* not all the amount of the nitrogen from fertilization is available on the given fertilization day to plants ->
			dissolv_coeff define the ratio */

			/* nitrate content of fertilizer can be uptaken by plant directly -> get to the sminn pool */
			FRZ_to_sminn_act = dissolv_coeff * FRZ->FRZ_pool_act * nitrate_content;
			/* ammonium content of fertilizer have to be nitrificatied before be uptaken by plant  -> get to the litrn pool */
			FRZ_to_litrn_act = dissolv_coeff * FRZ->FRZ_pool_act * ammonium_content;
			/* carbon content of fertilizer turn to the litter pool */
			FRZ_to_litrc_act = dissolv_coeff * FRZ->FRZ_pool_act * carbon_content;
			
			FRZ->FRZ_pool_act = FRZ->FRZ_pool_act - FRZ->FRZ_pool_act * dissolv_coeff;
		

			/* if N from fertilization is available (in FRZ_pool_act) a given ratio of its N content (defined by useful part)
				get into the soil mineral nitrogen pool, the rest disappers from the system (leaching...) */
			if (FRZ_to_sminn_act > 0.00001)
			{
				nf->FRZ_to_sminn = FRZ_to_sminn_act * utilization_coeff;
				
				nf->FRZ_to_litr1n = FRZ_to_litrn_act * litr_flab  * utilization_coeff;
				nf->FRZ_to_litr2n = FRZ_to_litrn_act * litr_fucel * utilization_coeff;
				nf->FRZ_to_litr3n = FRZ_to_litrn_act * litr_fscel * utilization_coeff;
				nf->FRZ_to_litr4n = FRZ_to_litrn_act * litr_flig  * utilization_coeff;

				cf->FRZ_to_litr1c = FRZ_to_litrc_act * litr_flab  * utilization_coeff;
				cf->FRZ_to_litr2c = FRZ_to_litrc_act * litr_fucel * utilization_coeff;
				cf->FRZ_to_litr3c = FRZ_to_litrc_act * litr_fscel * utilization_coeff;
				cf->FRZ_to_litr4c = FRZ_to_litrc_act * litr_flig  * utilization_coeff;	
				
				if (ctrl->onscreen) printf("fertilizing's effect on yearday:%d\t\n",yday);

			}

			/* if the nitrogen from fertilization has been consumed already, the fertilization has no more effect .*/ 
			else
			{
				nf->FRZ_to_sminn = 0;
				
				nf->FRZ_to_litr1n = 0;
				nf->FRZ_to_litr2n = 0;
				nf->FRZ_to_litr3n = 0;
				nf->FRZ_to_litr4n = 0;

				cf->FRZ_to_litr1c = 0;
				cf->FRZ_to_litr2c = 0;
				cf->FRZ_to_litr3c = 0;
				cf->FRZ_to_litr4c = 0;	
				
			}
		}
		else
		{
			nf->FRZ_to_sminn = 0;
				
			nf->FRZ_to_litr1n = 0;
			nf->FRZ_to_litr2n = 0;
			nf->FRZ_to_litr3n = 0;
			nf->FRZ_to_litr4n = 0;

			cf->FRZ_to_litr1c = 0;
			cf->FRZ_to_litr2c = 0;
			cf->FRZ_to_litr3c = 0;
			cf->FRZ_to_litr4c = 0;		
		}

	}

	/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                                                    STATE UPDATE 
	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 

	/* carbon */	
	cs->litr1c += cf->FRZ_to_litr1c;
	cs->litr2c += cf->FRZ_to_litr2c;
	cs->litr3c += cf->FRZ_to_litr3c;
	cs->litr4c += cf->FRZ_to_litr4c;
	
	cs->FRZsrc += cf->FRZ_to_litr1c + cf->FRZ_to_litr2c + cf->FRZ_to_litr3c + cf->FRZ_to_litr4c;

	/* nitrogen */
	ns->litr1n += nf->FRZ_to_litr1n;
	ns->litr2n += nf->FRZ_to_litr2n;
	ns->litr3n += nf->FRZ_to_litr3n;
	ns->litr4n += nf->FRZ_to_litr4n;

	ns->sminn[0]   += nf->FRZ_to_sminn;
	
	ns->FRZsrc += nf->FRZ_to_sminn + nf->FRZ_to_litr1n + nf->FRZ_to_litr2n + nf->FRZ_to_litr3n + nf->FRZ_to_litr4n;


   return (!ok);
}
	