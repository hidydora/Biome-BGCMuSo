/* 
fertilizing.c
do fertilization  - increase the mineral soil nitrogen (sminn)

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Biome-BGCMuSo v5.0
Copyright 2018, D. Hidy [dori.hidy@gmail.com]
Hungarian Academy of Sciences, Hungary
See the website of Biome-BGCMuSo at http://nimbus.elte.hu/bbgc/ for documentation, model executable and example input files.
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
#include "bgc_constants.h"

int fertilizing(const control_struct* ctrl, const siteconst_struct* sitec, fertilizing_struct* FRZ, 
				cstate_struct* cs, nstate_struct*ns, cflux_struct* cf, nflux_struct* nf)
{

	/* fertilizing parameters .*/
	int ok=1;
	int ny, mgmd, layer;

	double FRZ_to_litrc_act, N2Oflux_fertil;
	double ratio, ratioSUM,ha_to_m2;
	
	N2Oflux_fertil=FRZ_to_litrc_act=ratio=ratioSUM=0;
	ha_to_m2 = 1./10000;

	/* yearly varied or constant management parameters */
	if(FRZ->FRZ_flag == 2)
	{
		ny = ctrl->simyr;
	}
	else ny=0;

	/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                                                    CALCULATING FLUXES 
	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
	
	/* 1. on management days fertilizer is put on the field */

	mgmd=FRZ->mgmd;

	if (mgmd >=0) 
	{
		/* input parameters in actual year from array */
		FRZ->FRZdepth_act   = FRZ->FRZdepth_array[mgmd][ny]; 
		FRZ->DC_NO3_act     = FRZ->DC_NO3_array[mgmd][ny]; 
		FRZ->DC_NH4_act     = FRZ->DC_NH4_array[mgmd][ny]; 
		FRZ->DC_C_act       = FRZ->DC_C_array[mgmd][ny]; 
		FRZ->flab_act		= FRZ->litr_flab_array[mgmd][ny] / 100.;		/* from % to number */
		FRZ->fucel_act		= FRZ->litr_fucel_array[mgmd][ny] / 100.;		/* from % to number */
		FRZ->fscel_act		= FRZ->litr_fscel_array[mgmd][ny] / 100.;		/* from % to number */
		FRZ->flig_act		= FRZ->litr_flig_array[mgmd][ny] / 100.;		/* from % to number */
		FRZ->EFf_N2O_act	= FRZ->EFfert_N2O[mgmd][ny] /NDAYS_OF_YEAR;

		/* nitrate content of fertilizer: kgN/m2 = kgN/kg fertilizer * kg fertilizer/ha * ha/m2  */
		FRZ->NO3_act     = FRZ->NO3content_array[mgmd][ny] * FRZ->fertilizer_array[mgmd][ny] * ha_to_m2;
		
		/* ammonium content of fertilizer: kgN/m2 = kgN/kg fertilizer * kg fertilizer/ha * ha/m2  */
		FRZ->NH4_act     = FRZ->NH4content_array[mgmd][ny] * FRZ->fertilizer_array[mgmd][ny] * ha_to_m2;
		
		/* carbon content of fertilizer: kgC/m2 = kgC/kg fertilizer * kg fertilizer/ha * ha/m2  */
		FRZ->CARBON_act = FRZ->Ccontent_array[mgmd][ny] * FRZ->fertilizer_array[mgmd][ny] * ha_to_m2;
	

		/* fertilizing layer from depth */
		layer = 1;
		FRZ->FRZlayer = 0;
		if (FRZ->FRZdepth_act > sitec->soillayer_depth[0])
		{
			while (FRZ->FRZlayer == 0 && layer < N_SOILLAYERS)
			{
				if ((FRZ->FRZdepth_act > sitec->soillayer_depth[layer-1]) && (FRZ->FRZdepth_act <= sitec->soillayer_depth[layer])) FRZ->FRZlayer = layer;
				layer += 1;
			}
			if (FRZ->FRZlayer == 0)
			{
				printf("Error in fertilizing depth calculation (fertilizing.c)\n");
				ok=0;
			}
		}

	} /* endif mgmd */

	
	/* 2. on and after fertilizing day a fixed amount of nitrate/ammonium/carbon enters into the soil */
	/* if the nitrogen/carbon from fertilization has been consumed already, the fertilization has no more effect */ 

	if (FRZ->NO3_act  > CRIT_PREC)
	{
		nf->FRZ_to_sminNO3 = FRZ->DC_NO3_act * FRZ->NO3_act;
		FRZ->NO3_act   -= nf->FRZ_to_sminNO3;
	}
	else
	{
		nf->FRZ_to_sminNO3 = 0;
		ns->nvol_snk      += FRZ->NO3_act;
		FRZ->NO3_act       = 0;
	}
	
	if (FRZ->NH4_act  > CRIT_PREC)
	{
		nf->FRZ_to_sminNH4 = FRZ->DC_NH4_act * FRZ->NH4_act;
		FRZ->NH4_act      -= nf->FRZ_to_sminNH4;
	}
	else
	{
		nf->FRZ_to_sminNH4 = 0;
		ns->nvol_snk      += FRZ->NH4_act;
		FRZ->NH4_act       = 0;
	}

	if (FRZ->CARBON_act > CRIT_PREC)
	{
		FRZ_to_litrc_act   = FRZ->DC_C_act * FRZ->CARBON_act;
		FRZ->CARBON_act   -= FRZ_to_litrc_act;

		cf->FRZ_to_litr1c  = FRZ_to_litrc_act * FRZ->flab_act;
		cf->FRZ_to_litr2c  = FRZ_to_litrc_act * FRZ->fucel_act;
		cf->FRZ_to_litr3c  = FRZ_to_litrc_act * FRZ->fscel_act;
		cf->FRZ_to_litr4c  = FRZ_to_litrc_act * FRZ->flig_act;	
	}
	else
	{
		FRZ_to_litrc_act   = 0;
		cf->FRZ_to_litr1c  = 0;
		cf->FRZ_to_litr2c  = 0;
		cf->FRZ_to_litr3c  = 0;
		cf->FRZ_to_litr4c  = 0;	
		cs->soil1_hr_snk  += 0;
		FRZ->CARBON_act    = 0;
	}
	
	

	/* 3. N2O emissions (kgN2O-N:kgN) */ 
	nf->N2O_flux_FRZ   = (FRZ->NO3_act + FRZ->NH4_act) * FRZ->EFf_N2O_act;
	FRZ->NO3_act      -= FRZ->NO3_act * FRZ->EFf_N2O_act;
	FRZ->NH4_act      -= FRZ->NH4_act * FRZ->EFf_N2O_act;
	

	/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                                                    STATE UPDATE 
	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 

	if (nf->FRZ_to_sminNO3 > 0 || nf->FRZ_to_sminNH4 > 0 || FRZ_to_litrc_act > 0)
	{
		for (layer = 0; layer <= FRZ->FRZlayer; layer++)
		{
			if (FRZ->FRZlayer == 0)
				ratio = 1;
			else	
			{
				if (sitec->soillayer_depth[layer] < FRZ->FRZdepth_act)
					ratio = sitec->soillayer_thickness[layer] / FRZ->FRZdepth_act;
				else
					ratio = (FRZ->FRZdepth_act - sitec->soillayer_depth[layer-1]) / FRZ->FRZdepth_act;
			}

			ratioSUM += ratio;

			cs->litr1c[layer] += cf->FRZ_to_litr1c * ratio;
			cs->litr2c[layer] += cf->FRZ_to_litr2c * ratio;
			cs->litr3c[layer] += cf->FRZ_to_litr3c * ratio;
			cs->litr4c[layer] += cf->FRZ_to_litr4c * ratio;

			ns->sminNH4[layer]  += nf->FRZ_to_sminNH4 * ratio;
			ns->sminNO3[layer]  += nf->FRZ_to_sminNO3 * ratio;

		}

		/* control */
		if (fabs(ratioSUM - 1) > CRIT_PREC)
		{
			printf("Error in fertilizing ratio calculation (fertilizing.c)\n");
			ok=0;
		}
		cs->FRZsrc += cf->FRZ_to_litr1c + cf->FRZ_to_litr2c + cf->FRZ_to_litr3c + cf->FRZ_to_litr4c;
		ns->FRZsrc += nf->FRZ_to_sminNH4 + nf->FRZ_to_sminNO3;
	}


   return (!ok);
}
	