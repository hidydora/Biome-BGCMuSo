/* 
precision_control.c
Detects very low values in state variable structures, and forces them to
0.0, in order to avoid rounding and overflow errors.

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Biome-BGC version 4.1.1
Copyright 2000, Peter E. Thornton
Numerical Terradynamics Simulation Group (NTSG)
School of Forestry, University of Montana
Missoula, MT 59812
*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

Updated:
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <malloc.h>
#include "bgc_struct.h"
#include "bgc_func.h"
#include "bgc_constants.h"

int precision_control(const siteconst_struct* sitec, wstate_struct* ws, cstate_struct* cs, nstate_struct* ns)
{
	int ok = 1;
	int layer;
	
	/* CARBON AND NITROGEN STATE VARIABLES */
	/* force very low leaf C to 0.0, to avoid roundoff
	error in canopy radiation routines. Send excess to litter 1.
	Fine root C and N follow leaf C and N. This control is triggered
	at a higher value than the others because leafc is used in exp()
	in radtrans, and so can cause rounding error at larger values. */
	if (fabs(cs->leafc) < 1e-7)
	{
		cs->litr1c += cs->leafc;
		ns->litr1n += ns->leafn;
		cs->leafc = 0.0;
		ns->leafn = 0.0;
	}

	if (fabs(cs->frootc) < 1e-7)
	{
		cs->litr1c += cs->frootc;
		ns->litr1n += ns->frootn;
		cs->frootc = 0.0;
		ns->frootn = 0.0;
	}

	if (fabs(cs->fruitc) < CRIT_PREC)
	{
		cs->litr1c += cs->fruitc;
		ns->litr1n += ns->fruitn;
		cs->fruitc = 0.0;
		ns->fruitn = 0.0;
	}

	/* tests for other plant pools. Excess goes to litter 1 */
	if (fabs(cs->livestemc) < CRIT_PREC)
	{
		cs->litr1c += cs->livestemc;
		ns->litr1n += ns->livestemn;
		cs->livestemc = 0.0;
		ns->livestemn = 0.0;
	}
	if (fabs(cs->deadstemc) < CRIT_PREC)
	{
		cs->litr1c += cs->deadstemc;
		ns->litr1n += ns->deadstemn;
		cs->deadstemc = 0.0;
		ns->deadstemn = 0.0;
	}
	if (fabs(cs->livecrootc) < CRIT_PREC)
	{
		cs->litr1c += cs->livecrootc;
		ns->litr1n += ns->livecrootn;
		cs->livecrootc = 0.0;
		ns->livecrootn = 0.0;
	}
	if (fabs(cs->deadcrootc) < CRIT_PREC)
	{
		cs->litr1c += cs->deadcrootc;
		ns->litr1n += ns->deadcrootn;
		cs->deadcrootc = 0.0;
		ns->deadcrootn = 0.0;
	}

	/* missing gresp_transfer check - Hidy 2009 */
	if (fabs(cs->gresp_transfer) < CRIT_PREC)
	{
		cs->litr1c += cs->gresp_transfer;
		cs->gresp_transfer = 0.0;

	}
	/* ---------------------------------------*/
	
	if (fabs(cs->cwdc) < CRIT_PREC)
	{
		cs->litr1c += cs->cwdc;
		ns->litr1n += ns->cwdn;
		cs->cwdc = 0.0;
		ns->cwdn = 0.0;
	}
	
	/* test for litter and soil poils. Excess goes to hr sink (C)
	or volatilized sink (N) */
	if (fabs(cs->litr1c) < CRIT_PREC)
	{
		cs->litr1_hr_snk += cs->litr1c;
		ns->nvol_snk += ns->litr1n;
		cs->litr1c = 0.0;
		ns->litr1n = 0.0;
	}
	if (fabs(cs->litr2c) < CRIT_PREC)
	{
		cs->litr2_hr_snk += cs->litr2c;
		ns->nvol_snk += ns->litr2n;
		cs->litr2c = 0.0;
		ns->litr2n = 0.0;
	}
	if (fabs(cs->litr3c) < CRIT_PREC)
	{
		cs->litr4_hr_snk += cs->litr3c; /* NO LITR3C HR SINK */
		ns->nvol_snk += ns->litr3n;
		cs->litr3c = 0.0;
		ns->litr3n = 0.0;
	}
	if (fabs(cs->litr4c) < CRIT_PREC)
	{
		cs->litr4_hr_snk += cs->litr4c;
		ns->nvol_snk += ns->litr4n;
		cs->litr4c = 0.0;
		ns->litr4n = 0.0;
	}
	if (fabs(cs->soil1c) < CRIT_PREC)
	{
		cs->soil1_hr_snk += cs->soil1c;
		ns->nvol_snk += ns->soil1n;
		cs->soil1c = 0.0;
		ns->soil1n = 0.0;
	}
	if (fabs(cs->soil2c) < CRIT_PREC)
	{
		cs->soil2_hr_snk += cs->soil2c;
		ns->nvol_snk += ns->soil2n;
		cs->soil2c = 0.0;
		ns->soil2n = 0.0;
	}
	if (fabs(cs->soil3c) < CRIT_PREC)
	{
		cs->soil3_hr_snk += cs->soil3c;
		ns->nvol_snk += ns->soil3n;
		cs->soil3c = 0.0;
		ns->soil3n = 0.0;
	}
	if (fabs(cs->soil4c) < CRIT_PREC)
	{
		cs->soil4_hr_snk += cs->soil4c;
		ns->nvol_snk += ns->soil4n;
		cs->soil4c = 0.0;
		ns->soil4n = 0.0;
	}
	
	/* additional tests for soil mineral N and retranslocated N */
	
	for (layer = 0; layer < N_SOILLAYERS;layer++)
	{
		if (fabs(ns->sminn[layer]) < CRIT_PREC)
		{
			ns->nvol_snk     += ns->sminn[layer];
			ns->sminn[layer] = 0.0;
		}
	}

	if (fabs(ns->retransn) < CRIT_PREC)
	{
		ns->litr1n += ns->retransn;
		ns->retransn = 0.0;
	}

	
	/* WATER STATE VARIABLES */

	/* hidy 2010 - multilayer soil */
	for (layer = 0; layer < N_SOILLAYERS; layer ++)
	{
		if (fabs(ws->soilw[layer]) < CRIT_PREC)
		{
			ws->soilevap_snk += ws->soilw[layer];
			ws->soilw[layer] = 0.0;
		}
	}

	if (fabs(ws->snoww) < CRIT_PREC)
	{
		ws->snowsubl_snk += ws->snoww;
		ws->snoww = 0.0;
	}
	if (fabs(ws->canopyw) < CRIT_PREC)
	{
		ws->canopyevap_snk += ws->canopyw;
		ws->canopyw = 0.0;
	}
	
	return(!ok);
}	
