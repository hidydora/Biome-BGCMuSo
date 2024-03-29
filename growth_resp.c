/* 
growth_resp.c
daily growth respiration rates

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Biome-BGCMuSo v7.0.
Original code: Copyright 2000, Peter E. Thornton
Numerical Terradynamic Simulation Group, The University of Montana, USA
Modified code: Copyright 2022, D. Hidy [dori.hidy@gmail.com]
Hungarian Academy of Sciences, Hungary
See the website of Biome-BGCMuSo at http://nimbus.elte.hu/bbgc/ for documentation, model executable and example input files.
*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <malloc.h>
#include "ini.h"
#include "bgc_struct.h"
#include "bgc_func.h"
#include "bgc_constants.h"

int growth_resp(const epconst_struct* epc, cflux_struct* cf)
{
	int errorCode=0;
	double g1;   /* RATIO   C respired for growth : C grown  */ 
	double g2;   /* proportion of growth resp to release at fixation */

	g1 = epc->GR_ratio;
	g2 = GRPNOW;
	
	/* leaf and fine root growth respiration for both trees and grass */	
	cf->cpool_leaf_GR          = cf->cpool_to_leafc      * g1;
	cf->cpool_froot_GR         = cf->cpool_to_frootc     * g1;
	cf->cpool_yield_GR         = cf->cpool_to_yield     * g1;
	cf->cpool_softstem_GR      = cf->cpool_to_softstemc  * g1;
	cf->cpool_livestem_GR      = cf->cpool_to_livestemc  * g1;
	cf->cpool_deadstem_GR      = cf->cpool_to_deadstemc  * g1;
	cf->cpool_livecroot_GR     = cf->cpool_to_livecrootc * g1;
	cf->cpool_deadcroot_GR     = cf->cpool_to_deadcrootc * g1;
	
	cf->cpool_leaf_storage_GR		= cf->cpool_to_leafc_storage      * g1 * g2;
	cf->cpool_froot_storage_GR		= cf->cpool_to_frootc_storage     * g1 * g2; 
	cf->cpool_yieldc_storage_GR		= cf->cpool_to_yieldc_storage     * g1 * g2;
	cf->cpool_softstem_storage_GR   = cf->cpool_to_softstemc_storage  * g1 * g2;
	cf->cpool_livestem_storage_GR   = cf->cpool_to_livestemc_storage  * g1 * g2;
	cf->cpool_deadstem_storage_GR   = cf->cpool_to_deadstemc_storage  * g1 * g2;
	cf->cpool_livecroot_storage_GR  = cf->cpool_to_livecrootc_storage * g1 * g2;
	cf->cpool_deadcroot_storage_GR = cf->cpool_to_deadcrootc_storage  * g1 * g2;

	cf->transfer_leaf_GR      = cf->leafc_transfer_to_leafc           * g1 * (1.0-g2);
	cf->transfer_froot_GR     = cf->frootc_transfer_to_frootc         * g1 * (1.0-g2);
	cf->transfer_yield_GR     = cf->yieldc_transfer_to_yield         * g1 * (1.0-g2);
	cf->transfer_softstem_GR  = cf->softstemc_transfer_to_softstemc   * g1 * (1.0-g2);
	cf->transfer_livestem_GR  = cf->livestemc_transfer_to_livestemc   * g1 * (1.0-g2);
	cf->transfer_deadstem_GR  = cf->deadstemc_transfer_to_deadstemc   * g1 * (1.0-g2);
	cf->transfer_livecroot_GR = cf->livecrootc_transfer_to_livecrootc * g1 * (1.0-g2);
	cf->transfer_deadcroot_GR = cf->deadcrootc_transfer_to_deadcrootc * g1 * (1.0-g2);
	

	return (errorCode);
}

