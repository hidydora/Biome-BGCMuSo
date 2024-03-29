/* 
harvesting.c
do harvesting  - decrease the plant material (leafc, leafn, canopy water)

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Biome-BGCMuSo v7.0.
Copyright 2022, D. Hidy [dori.hidy@gmail.com]
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

int harvesting(file econout, control_struct* ctrl, phenology_struct* phen, const epconst_struct* epc, const harvesting_struct* HRV, const irrigating_struct* IRG, 
	           epvar_struct* epv, cstate_struct* cs, nstate_struct* ns, wstate_struct* ws, cflux_struct* cf, nflux_struct* nf, wflux_struct* wf)
{
	int errorCode=0;
		                                    
	double HRVcoeff_leaf, HRVcoeff_yield, HRVcoeff_softstem, remained_prop;	/* decrease of plant material caused by harvest: difference between plant material before and after harvesting */
	double outc, outn, inc, inn, HRV_to_transpC, HRV_to_transpN;
	double yieldC_HRV, leafstemC_HRV;
	int md, year;

	year = ctrl->simstartyear + ctrl->simyr;
	md = HRV->mgmdHRV-1;


	HRV_to_transpC=HRV_to_transpN=outc=outn=inc=inn=HRVcoeff_leaf=HRVcoeff_yield=HRVcoeff_softstem=remained_prop=0;
	
	/**********************************************************************************************/
	/* I. CALCULATING HRVcoeff */

	if (HRV->HRV_num && md >= 0)
	{
		if (year == HRV->HRVyear_array[md] && ctrl->month == HRV->HRVmonth_array[md] && ctrl->day == HRV->HRVday_array[md])
		{ 
			remained_prop = (100 - HRV->transportHRV_array[md])/100.; /* remained prop. of plant mat.is calculated from transport coeff. */
	
			epv->rootDepth  = CRIT_PREC;
			epv->rootlength = CRIT_PREC;
	
			/* if harvest: plant material decreases as the rate of "harvest effect", which is th ratio of LAI before harvest and LAI snag */
			HRVcoeff_leaf  = 1.0;
			HRVcoeff_yield = 1.0;
			HRVcoeff_softstem = HRV->snagprop_array[md] / 100;

			cs->frootC_HRV  = cs->frootc;
		}
	}
		
		
	/**********************************************************************************************/
	/* II. CALCULATING FLUXES */
	
	if (HRVcoeff_leaf || HRVcoeff_yield || HRVcoeff_softstem)
	{
		/*----------------------------------------------------------*/
		/* 1. OUT: daily loss due to HRV */ 

		/* 1.1. actual and transfer plant pools*/
		if (epc->leaf_cn)
		{
			cf->leafc_to_HRV              = cs->leafc          * HRVcoeff_leaf;
			cf->leafc_storage_to_HRV      = cs->leafc_storage  * HRVcoeff_leaf;
			cf->leafc_transfer_to_HRV     = cs->leafc_transfer * HRVcoeff_leaf;

			nf->leafn_to_HRV              = cf->leafc_to_HRV          / epc->leaf_cn;
			nf->leafn_storage_to_HRV      = cf->leafc_storage_to_HRV  / epc->leaf_cn;
			nf->leafn_transfer_to_HRV     = cf->leafc_transfer_to_HRV / epc->leaf_cn;
		}

		if (epc->yield_cn)
		{
			cf->yieldc_to_HRV              = cs->yieldc          * HRVcoeff_yield;
			cf->yieldc_storage_to_HRV      = cs->yieldc_storage  * HRVcoeff_yield;
			cf->yieldc_transfer_to_HRV     = cs->yieldc_transfer * HRVcoeff_yield;

			nf->yieldn_to_HRV              = cf->yieldc_to_HRV          / epc->yield_cn;
			nf->yieldn_storage_to_HRV      = cf->yieldc_storage_to_HRV  / epc->yield_cn;
			nf->yieldn_transfer_to_HRV     = cf->yieldc_transfer_to_HRV / epc->yield_cn;
		}

		if (epc->softstem_cn)
		{
			cf->softstemc_to_HRV              = cs->softstemc          * HRVcoeff_softstem;
			cf->softstemc_storage_to_HRV      = cs->softstemc_storage  * HRVcoeff_softstem;
			cf->softstemc_transfer_to_HRV     = cs->softstemc_transfer * HRVcoeff_softstem;

			nf->softstemn_to_HRV              = cf->softstemc_to_HRV          / epc->softstem_cn;
			nf->softstemn_storage_to_HRV      = cf->softstemc_storage_to_HRV  / epc->softstem_cn;
			nf->softstemn_transfer_to_HRV     = cf->softstemc_transfer_to_HRV / epc->softstem_cn;
		}

		cf->gresp_storage_to_HRV      = cs->gresp_storage  * HRVcoeff_leaf;
		cf->gresp_transfer_to_HRV     = cs->gresp_transfer * HRVcoeff_leaf;
	
		nf->retransn_to_HRV           = 0;

		/* 1.2 standing dead biome */
		cf->STDBc_leaf_to_HRV     = cs->STDBc_leaf     * HRVcoeff_leaf; 
		cf->STDBc_yield_to_HRV    = cs->STDBc_yield    * HRVcoeff_yield; 
		cf->STDBc_softstem_to_HRV = cs->STDBc_softstem * HRVcoeff_softstem;
		
		nf->STDBn_leaf_to_HRV     = ns->STDBn_leaf     * HRVcoeff_leaf; 
		nf->STDBn_yield_to_HRV    = ns->STDBn_yield    * HRVcoeff_yield; 
		nf->STDBn_softstem_to_HRV = ns->STDBn_softstem * HRVcoeff_softstem;


		
		/* 1.3 Water */
		wf->canopyw_to_HRV   = ws->canopyw * HRVcoeff_leaf;


 		/*----------------------------------------------------------*/
		/* 2. TRANSPORT: part of the plant material is transported (all of the yield, 1-remained_prop part of leaf and softstem, but no transfer pools!)*/

		HRV_to_transpC = (cf->leafc_to_HRV  +  cf->leafc_transfer_to_HRV + cf->leafc_storage_to_HRV   + cf->STDBc_leaf_to_HRV +
			              cf->gresp_storage_to_HRV + cf->gresp_transfer_to_HRV +
			              cf->softstemc_to_HRV  +  cf->softstemc_transfer_to_HRV + cf->softstemc_storage_to_HRV   + cf->STDBc_softstem_to_HRV) * (1-remained_prop) +
						  cf->yieldc_to_HRV +   cf->yieldc_transfer_to_HRV + cf->yieldc_storage_to_HRV + cf->STDBc_yield_to_HRV;
		HRV_to_transpN = (nf->leafn_to_HRV  +  nf->leafn_transfer_to_HRV + nf->leafn_storage_to_HRV   + nf->STDBn_leaf_to_HRV +
			              nf->retransn_to_HRV +
			              nf->softstemn_to_HRV  +  nf->softstemn_transfer_to_HRV + nf->softstemn_storage_to_HRV   + nf->STDBn_softstem_to_HRV) * (1-remained_prop) +
						  nf->yieldn_to_HRV +   nf->yieldn_transfer_to_HRV + nf->yieldn_storage_to_HRV + nf->STDBn_yield_to_HRV;

		/*----------------------------------------------------------*/
		/* 3. IN: cut-down biomass - the rest remains at the site */

		cf->HRV_to_CTDBc_leaf     = (cf->leafc_to_HRV  + cf->leafc_transfer_to_HRV + cf->leafc_storage_to_HRV   + cf->STDBc_leaf_to_HRV +
			                         cf->gresp_storage_to_HRV + cf->gresp_transfer_to_HRV) * remained_prop;

		cf->HRV_to_CTDBc_yield    = 0;

		cf->HRV_to_CTDBc_softstem = (cf->softstemc_to_HRV + cf->softstemc_transfer_to_HRV + cf->softstemc_storage_to_HRV + cf->STDBc_softstem_to_HRV) * remained_prop;

	

		nf->HRV_to_CTDBn_leaf     = (nf->leafn_to_HRV  + nf->leafn_transfer_to_HRV + nf->leafn_storage_to_HRV   + nf->STDBn_leaf_to_HRV) * remained_prop;

		nf->HRV_to_CTDBn_yield    = 0;

		nf->HRV_to_CTDBn_softstem = (nf->softstemn_to_HRV + nf->softstemn_transfer_to_HRV + nf->softstemn_storage_to_HRV + nf->STDBn_softstem_to_HRV) * remained_prop;


  


		/**********************************************************************************************/
		/* III. STATE UPDATE */

		/* 1. OUT */
		/* 1.1.leaf, yield, softstem, gresp */

		cs->leafc				-= cf->leafc_to_HRV;
		cs->leafc_transfer		-= cf->leafc_transfer_to_HRV;
		cs->leafc_storage		-= cf->leafc_storage_to_HRV;
		cs->yieldc				-= cf->yieldc_to_HRV;
		cs->yieldc_transfer		-= cf->yieldc_transfer_to_HRV;
		cs->yieldc_storage		-= cf->yieldc_storage_to_HRV;
		cs->softstemc			-= cf->softstemc_to_HRV;
		cs->softstemc_transfer	-= cf->softstemc_transfer_to_HRV;
		cs->softstemc_storage	-= cf->softstemc_storage_to_HRV;
		cs->gresp_transfer		-= cf->gresp_transfer_to_HRV;
		cs->gresp_storage		-= cf->gresp_storage_to_HRV;

		ns->leafn				-= nf->leafn_to_HRV;
		ns->leafn_transfer		-= nf->leafn_transfer_to_HRV;
		ns->leafn_storage		-= nf->leafn_storage_to_HRV;
		ns->yieldn				-= nf->yieldn_to_HRV;
		ns->yieldn_transfer		-= nf->yieldn_transfer_to_HRV;
		ns->yieldn_storage		-= nf->yieldn_storage_to_HRV;
		ns->softstemn			-= nf->softstemn_to_HRV;
		ns->softstemn_transfer	-= nf->softstemn_transfer_to_HRV;
		ns->softstemn_storage	-= nf->softstemn_storage_to_HRV;
		ns->retransn			-= nf->retransn_to_HRV;

	

		/* 1.2 dead standing biomass */
		cs->STDBc_leaf     -= cf->STDBc_leaf_to_HRV;
		cs->STDBc_yield    -= cf->STDBc_yield_to_HRV;
		cs->STDBc_softstem -= cf->STDBc_softstem_to_HRV;
	
		ns->STDBn_leaf     -= nf->STDBn_leaf_to_HRV;
		ns->STDBn_yield    -= nf->STDBn_yield_to_HRV;
		ns->STDBn_softstem -= nf->STDBn_softstem_to_HRV;


		/* 1.3. water */
		ws->canopyw_HRVsnk += wf->canopyw_to_HRV;
		ws->canopyw -= wf->canopyw_to_HRV;

		/* 2. TRANSPORT*/
		cs->HRV_transportC  += HRV_to_transpC;
		ns->HRV_transportN  += HRV_to_transpN;

		/* 3. IN: cut-down biome  */
		cs->CTDBc_leaf     += cf->HRV_to_CTDBc_leaf;
		cs->CTDBc_yield    += cf->HRV_to_CTDBc_yield;
		cs->CTDBc_softstem += cf->HRV_to_CTDBc_softstem;

		ns->CTDBn_leaf     += nf->HRV_to_CTDBn_leaf;
		ns->CTDBn_yield    += nf->HRV_to_CTDBn_yield;
		ns->CTDBn_softstem += nf->HRV_to_CTDBn_softstem;

		/* 4. after harvest, remaining softstem and froot and transfer pools transferred to standing dead biomass */
		
		if (cs->leafc_transfer || ns->leafn_transfer)
		{
			cf->HRV_leafc_transfer_to_SNSC      = cs->leafc_transfer;
			cs->leafc_transfer                 -= cf->HRV_leafc_transfer_to_SNSC;
			cs->STDBc_leaf                     += cf->HRV_leafc_transfer_to_SNSC;
			cs->SNSCsnk_C                      += cf->HRV_leafc_transfer_to_SNSC;

			nf->HRV_leafn_transfer_to_SNSC      = ns->leafn_transfer;
			ns->leafn_transfer                 -= nf->HRV_leafn_transfer_to_SNSC;
			ns->STDBn_leaf                     += nf->HRV_leafn_transfer_to_SNSC;
			ns->SNSCsnk_N                      += nf->HRV_leafn_transfer_to_SNSC;
		}

		if (cs->leafc_storage || ns->leafn_storage)
		{
			cf->HRV_leafc_storage_to_SNSC      = cs->leafc_storage;
			cs->leafc_storage                 -= cf->HRV_leafc_storage_to_SNSC;
			cs->STDBc_leaf                     += cf->HRV_leafc_storage_to_SNSC;
			cs->SNSCsnk_C                      += cf->HRV_leafc_storage_to_SNSC;

			nf->HRV_leafn_storage_to_SNSC      = ns->leafn_storage;
			ns->leafn_storage                 -= nf->HRV_leafn_storage_to_SNSC;
			ns->STDBn_leaf                    += nf->HRV_leafn_storage_to_SNSC;
			ns->SNSCsnk_N                     += nf->HRV_leafn_storage_to_SNSC;
		}

		if (cs->yieldc_transfer || ns->yieldn_transfer)
		{
			cf->HRV_yieldc_transfer_to_SNSC    = cs->yieldc_transfer;
			cs->yieldc_transfer               -= cf->HRV_yieldc_transfer_to_SNSC;
			cs->STDBc_yield                  += cf->HRV_yieldc_transfer_to_SNSC;
			cs->SNSCsnk_C                    += cf->HRV_yieldc_transfer_to_SNSC;

			nf->HRV_yieldn_transfer_to_SNSC    = ns->yieldn_transfer;
			ns->yieldn_transfer               -= nf->HRV_yieldn_transfer_to_SNSC;
			ns->STDBn_yield                   += nf->HRV_yieldn_transfer_to_SNSC;
			ns->SNSCsnk_N                     += nf->HRV_yieldn_transfer_to_SNSC;
		}
		
		if (cs->yieldc_storage || ns->yieldn_storage)
		{
			cf->HRV_yieldc_storage_to_SNSC     = cs->yieldc_storage;
			cs->yieldc_storage                -= cf->HRV_yieldc_storage_to_SNSC;
			cs->STDBc_yield                   += cf->HRV_softstemc_transfer_to_SNSC;
			cs->SNSCsnk_C                     += cf->HRV_yieldc_storage_to_SNSC;

			nf->HRV_yieldn_storage_to_SNSC     = ns->yieldn_storage;
			ns->yieldn_storage                -= nf->HRV_yieldn_storage_to_SNSC;
			ns->STDBn_yield                     += nf->HRV_yieldn_storage_to_SNSC;
			ns->SNSCsnk_N                     += nf->HRV_yieldn_storage_to_SNSC;
		}

		if (cs->softstemc_transfer || ns->softstemn_transfer)
		{
			cf->HRV_softstemc_transfer_to_SNSC	 = cs->softstemc_transfer;
			cs->softstemc_transfer				-= cf->HRV_softstemc_transfer_to_SNSC;
			cs->STDBc_softstem                  += cf->HRV_softstemc_transfer_to_SNSC;
			cs->SNSCsnk_C                       += cf->HRV_softstemc_transfer_to_SNSC;

			nf->HRV_softstemn_transfer_to_SNSC    = ns->softstemn_transfer;
			ns->softstemn_transfer               -= nf->HRV_softstemn_transfer_to_SNSC;
			ns->STDBn_softstem                  += nf->HRV_softstemn_transfer_to_SNSC;
			ns->SNSCsnk_N                        += nf->HRV_softstemn_transfer_to_SNSC;
		}
		
		if (cs->softstemc || ns->softstemn)
		{
			cf->HRV_softstemc_to_SNSC  = cs->softstemc;
			cs->softstemc             -= cf->HRV_softstemc_to_SNSC;
			cs->STDBc_softstem        += cf->HRV_softstemc_to_SNSC;
			cs->SNSCsnk_C             += cf->HRV_softstemc_to_SNSC;

			nf->HRV_softstemn_to_SNSC  = ns->softstemn;
			ns->softstemn             -= nf->HRV_softstemn_to_SNSC;
			ns->STDBn_softstem        += nf->HRV_softstemn_to_SNSC;
			ns->SNSCsnk_N               += nf->HRV_softstemn_to_SNSC;
		}

		if (cs->softstemc_storage || ns->softstemn_storage)
		{
			cf->HRV_softstemc_storage_to_SNSC  = cs->softstemc_storage;
			cs->softstemc_storage             -= cf->HRV_softstemc_storage_to_SNSC;
			cs->STDBc_softstem                += cf->HRV_softstemc_storage_to_SNSC;
			cs->SNSCsnk_C                     += cf->HRV_softstemc_storage_to_SNSC;

			nf->HRV_softstemn_storage_to_SNSC  = ns->softstemn_storage;
			ns->softstemn_storage             -= nf->HRV_softstemn_storage_to_SNSC;
			ns->STDBn_softstem                += nf->HRV_softstemn_storage_to_SNSC;
			ns->SNSCsnk_N                     += nf->HRV_softstemn_storage_to_SNSC;
		}

		if (cs->softstemc_transfer || ns->softstemn_transfer)
		{
			cf->HRV_softstemc_transfer_to_SNSC = cs->softstemc_transfer;
			cs->softstemc_transfer            -= cf->HRV_softstemc_transfer_to_SNSC;
			cs->STDBc_softstem                += cf->HRV_softstemc_transfer_to_SNSC;
			cs->SNSCsnk_C                     += cf->HRV_softstemc_transfer_to_SNSC;

			nf->HRV_softstemn_transfer_to_SNSC = ns->softstemn_transfer;
			ns->softstemn_transfer            -= nf->HRV_softstemn_transfer_to_SNSC;
			ns->STDBn_softstem                += nf->HRV_softstemn_transfer_to_SNSC;
			ns->SNSCsnk_N                     += nf->HRV_softstemn_transfer_to_SNSC;
		}

		if (cs->frootc || ns->frootn)
		{
			cf->HRV_frootc_to_SNSC  = cs->frootc;
			cs->frootc             -= cf->HRV_frootc_to_SNSC;
			cs->STDBc_froot        += cf->HRV_frootc_to_SNSC;
			cs->SNSCsnk_C            += cf->HRV_frootc_to_SNSC;

			nf->HRV_frootn_to_SNSC  = ns->frootn;
			ns->frootn             -= nf->HRV_frootn_to_SNSC;
			ns->STDBn_froot        += nf->HRV_frootn_to_SNSC;
			ns->SNSCsnk_N          += nf->HRV_frootn_to_SNSC;
		}

		if (cs->frootc_storage || ns->frootn_storage)
		{
			cf->HRV_frootc_storage_to_SNSC  = cs->frootc_storage;
			cs->frootc_storage             -= cf->HRV_frootc_storage_to_SNSC;
			cs->STDBc_froot                += cf->HRV_frootc_storage_to_SNSC;
			cs->SNSCsnk_C                  += cf->HRV_frootc_storage_to_SNSC;

			nf->HRV_frootn_storage_to_SNSC  = ns->frootn_storage;
			ns->frootn_storage             -= nf->HRV_frootn_storage_to_SNSC;
			ns->STDBn_froot                += nf->HRV_frootn_storage_to_SNSC;
			ns->SNSCsnk_N                  += nf->HRV_frootn_storage_to_SNSC;
		}

		if (cs->frootc_transfer || ns->frootn_transfer)
		{
			cf->HRV_frootc_transfer_to_SNSC  = cs->frootc_transfer;
			cs->frootc_transfer             -= cf->HRV_frootc_transfer_to_SNSC;
			cs->STDBc_froot                 += cf->HRV_frootc_transfer_to_SNSC;
			cs->SNSCsnk_C                   += cf->HRV_frootc_transfer_to_SNSC;

			nf->HRV_frootn_transfer_to_SNSC  = ns->frootn_transfer;
			ns->frootn_transfer             -= nf->HRV_frootn_transfer_to_SNSC;
			ns->STDBn_froot                 += nf->HRV_frootn_transfer_to_SNSC;
			ns->SNSCsnk_N                   += nf->HRV_frootn_transfer_to_SNSC;
		}

		if (cs->gresp_storage)
		{
			cf->HRV_gresp_storage_to_SNSC  = cs->gresp_storage;
			cs->gresp_storage             -= cf->HRV_gresp_storage_to_SNSC;
			cs->STDBc_leaf                += cf->HRV_gresp_storage_to_SNSC;
			cs->SNSCsnk_C                 += cf->HRV_gresp_storage_to_SNSC;
		}

		if (cs->gresp_transfer)
		{
			cf->HRV_gresp_transfer_to_SNSC  = cs->gresp_transfer;
			cs->gresp_transfer             -= cf->HRV_gresp_transfer_to_SNSC;
			cs->STDBc_leaf			       += cf->HRV_gresp_transfer_to_SNSC;
			cs->SNSCsnk_C				   += cf->HRV_gresp_transfer_to_SNSC;
		}

		if (ns->retransn)
		{
			nf->HRV_retransn_to_SNSC        = ns->retransn;
			ns->retransn                   -= nf->HRV_retransn_to_SNSC;
			ns->STDBn_froot			       += nf->HRV_retransn_to_SNSC;
			ns->SNSCsnk_N				   += nf->HRV_retransn_to_SNSC;
		}

		/* harvested yield and leaf-stem carbon content */

		yieldC_HRV      = cf->yieldc_to_HRV + cf->STDBc_yield_to_HRV;
		leafstemC_HRV  = cf->leafc_to_HRV + cf->STDBc_leaf_to_HRV + cf->softstemc_to_HRV + cf->STDBc_softstem_to_HRV;
		

		cs->yieldC_HRV += yieldC_HRV;
		cs->vegC_HRV   += yieldC_HRV + leafstemC_HRV;

		fprintf(econout.ptr, "%6i %12.0f %12.4f %12.4f %12.4f %6i\n", ctrl->simyr+ctrl->simstartyear, phen->planttype, 
			                                                          yieldC_HRV * m2_to_ha * kg_to_t, leafstemC_HRV * m2_to_ha * kg_to_t,
																	  ws->condIRGsrc, IRG->condIRG_flag);  

		ws->condIRGsrc = 0;
		/**********************************************************************************************/
		/* IV. CONTROL */

		outc = cf->leafc_to_HRV         + cf->leafc_transfer_to_HRV     + cf->leafc_storage_to_HRV +
			   cf->yieldc_to_HRV         + cf->yieldc_transfer_to_HRV     + cf->yieldc_storage_to_HRV +
			   cf->softstemc_to_HRV     + cf->softstemc_transfer_to_HRV + cf->softstemc_storage_to_HRV +
			   cf->gresp_storage_to_HRV + cf->gresp_transfer_to_HRV + 
			   cf->STDBc_leaf_to_HRV + cf->STDBc_yield_to_HRV + cf->STDBc_softstem_to_HRV;


		outn = nf->leafn_to_HRV         + nf->leafn_transfer_to_HRV     + nf->leafn_storage_to_HRV +
			   nf->yieldn_to_HRV        + nf->yieldn_transfer_to_HRV    + nf->yieldn_storage_to_HRV +
			   nf->softstemn_to_HRV     + nf->softstemn_transfer_to_HRV + nf->softstemn_storage_to_HRV +
			   nf->retransn_to_HRV + 
			   nf->STDBn_leaf_to_HRV + nf->STDBn_yield_to_HRV + nf->STDBn_softstem_to_HRV;

		inc = cf->HRV_to_CTDBc_leaf + cf->HRV_to_CTDBc_yield  + cf->HRV_to_CTDBc_softstem;

		inn = nf->HRV_to_CTDBn_leaf + nf->HRV_to_CTDBn_yield  + nf->HRV_to_CTDBn_softstem;


		if (fabs(inc + HRV_to_transpC - outc) > CRIT_PREC || fabs(inn + HRV_to_transpN - outn) > CRIT_PREC )
		{
 			printf("BALANCE ERROR in harvest calculation in harvesting.c\n");
			errorCode=1;
		}	

	}

	

   return (errorCode);
}
	