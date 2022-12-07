/* 
multilayer_leaching.c
Calculating soil mineral nitrogen and DOC-DON leaching in multilayer soil 
*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Biome-BGCMuSo v6.4.
Copyright 2022, D. Hidy [dori.hidy@gmail.com]
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

int multilayer_leaching(const soilprop_struct* sprop, const epvar_struct* epv,
					    control_struct* ctrl, cstate_struct* cs, cflux_struct* cf, nstate_struct* ns, nflux_struct* nf, wstate_struct* ws, wflux_struct* wf)
{
	int errorCode=0;
	int layer=0;
	int datatype=0;
	int n_data=9;
	double soilwater_NH4conc_downward, soilwater_NH4conc_upward, soilwater_NO3conc_downward, soilwater_NO3conc_upward; 
	double soilwater_DOC1conc_downward, soilwater_DON1conc_downward, soilwater_DOC1conc_upward, soilwater_DON1conc_upward;
	double soilwater_DOC2conc_downward, soilwater_DON2conc_downward, soilwater_DOC2conc_upward, soilwater_DON2conc_upward;
	double soilwater_DOC3conc_downward, soilwater_DON3conc_downward, soilwater_DOC3conc_upward, soilwater_DON3conc_upward;
	double soilwater_DOC4conc_downward, soilwater_DON4conc_downward, soilwater_DOC4conc_upward, soilwater_DON4conc_upward;
	double state0, state1,lflux, dflux, pool0, pool1;
	double cn_ratio1, cn_ratio2, cn_ratio3, cn_ratio4;



	soilwater_NH4conc_downward=soilwater_NH4conc_upward=soilwater_NO3conc_downward=soilwater_NO3conc_upward=0;
	soilwater_DOC1conc_downward=soilwater_DON1conc_downward=soilwater_DOC1conc_upward=soilwater_DON1conc_upward=0;
	soilwater_DOC2conc_downward=soilwater_DON2conc_downward=soilwater_DOC2conc_upward=soilwater_DON2conc_upward=0;
	soilwater_DOC3conc_downward=soilwater_DON3conc_downward=soilwater_DOC3conc_upward=soilwater_DON3conc_upward=0;
	soilwater_DOC4conc_downward=soilwater_DON4conc_downward=soilwater_DOC4conc_upward=soilwater_DON4conc_upward=0;

    state0=state1=lflux=dflux=cn_ratio1=cn_ratio2=cn_ratio3=cn_ratio4=0;

	

	for (layer = 0; layer < N_SOILLAYERS; layer++)
	{
		soilwater_NH4conc_downward = ns->sminNH4avail[layer]  / ws->soilw[layer];
		soilwater_NO3conc_downward = ns->sminNO3avail[layer]  / ws->soilw[layer];

		if (layer < N_SOILLAYERS-1)
		{
			soilwater_NH4conc_upward   = ns->sminNH4avail[layer+1]  / ws->soilw[layer+1];
			soilwater_NO3conc_upward   = ns->sminNO3avail[layer+1]  / ws->soilw[layer+1];
		}
		else
		{
			soilwater_NH4conc_upward   = 0;
			soilwater_NO3conc_upward   = 0;
		}

		
		if (ns->soil1n[layer] > 0)
		{
				cn_ratio1=cs->soil1c[layer]/ns->soil1n[layer]; 
				soilwater_DOC1conc_downward = sprop->SOIL1_dissolv_prop * cs->soil1c[layer]   / ws->soilw[layer];
				soilwater_DON1conc_downward = soilwater_DOC1conc_downward/cn_ratio1;
				if (layer < N_SOILLAYERS-1)
				{
					soilwater_DOC1conc_upward   = sprop->SOIL1_dissolv_prop * cs->soil1c[layer+1] / ws->soilw[layer+1];
					soilwater_DON1conc_upward   = soilwater_DOC1conc_upward/cn_ratio1;
				}
				else
				{
					soilwater_DOC1conc_upward   = 0;
					soilwater_DON1conc_upward   = 0;
				}
		}
		else
		{
				soilwater_DOC1conc_downward = 0;
				soilwater_DON1conc_downward = 0;
				soilwater_DOC1conc_upward   = 0;
				soilwater_DON1conc_upward   = 0;
		
		}

		
		if (ns->soil2n[layer] > 0)
		{
				cn_ratio2=cs->soil2c[layer]/ns->soil2n[layer]; 
				soilwater_DOC2conc_downward = sprop->SOIL2_dissolv_prop * cs->soil2c[layer]   / ws->soilw[layer];
				soilwater_DON2conc_downward = soilwater_DOC2conc_downward/cn_ratio2;
				if (layer < N_SOILLAYERS-1)
				{
					soilwater_DOC2conc_upward   = sprop->SOIL2_dissolv_prop * cs->soil2c[layer+1] / ws->soilw[layer+1];
					soilwater_DON2conc_upward   = soilwater_DOC2conc_upward/cn_ratio2;
				}
				else
				{
					soilwater_DOC2conc_upward   = 0;
					soilwater_DON2conc_upward   = 0;
				}
		}
		else
		{	
				soilwater_DOC2conc_downward = 0;
				soilwater_DON2conc_downward = 0;
				soilwater_DOC2conc_upward   = 0;
				soilwater_DON2conc_upward   = 0;
		}
		
		if (ns->soil3n[layer] > 0)
		{
				cn_ratio3=cs->soil3c[layer]/ns->soil3n[layer]; 
				soilwater_DOC3conc_downward = sprop->SOIL3_dissolv_prop * cs->soil3c[layer]   / ws->soilw[layer];
				soilwater_DON3conc_downward = soilwater_DOC3conc_downward/cn_ratio3;
				if (layer < N_SOILLAYERS-1)
				{
					soilwater_DOC3conc_upward   = sprop->SOIL3_dissolv_prop * cs->soil3c[layer+1] / ws->soilw[layer+1];
					soilwater_DON3conc_upward   = soilwater_DOC3conc_upward/cn_ratio3;
				}
				else
				{
					soilwater_DOC3conc_upward   = 0;
					soilwater_DON3conc_upward   = 0;
				}
		}
		else
		{	
				soilwater_DOC3conc_downward = 0;
				soilwater_DON3conc_downward = 0;
				soilwater_DOC3conc_upward   = 0;
				soilwater_DON3conc_upward   = 0;
		}

		if (ns->soil4n[layer] > 0)
		{
				cn_ratio4=cs->soil4c[layer]/ns->soil4n[layer]; 
				soilwater_DOC4conc_downward = sprop->SOIL4_dissolv_prop * cs->soil4c[layer]   / ws->soilw[layer];
				soilwater_DON4conc_downward = soilwater_DOC4conc_downward/cn_ratio4;
				if (layer < N_SOILLAYERS-1)
				{
					soilwater_DOC4conc_upward   = sprop->SOIL4_dissolv_prop * cs->soil4c[layer+1] / ws->soilw[layer+1];
					soilwater_DON4conc_upward   = soilwater_DOC4conc_upward/cn_ratio4;
				}
				else
				{
					soilwater_DOC4conc_upward   = 0;
					soilwater_DON4conc_upward   = 0;
				}
		}
		else
		{	
				soilwater_DOC4conc_downward = 0;
				soilwater_DON4conc_downward = 0;
				soilwater_DOC4conc_upward   = 0;
				soilwater_DON4conc_upward   = 0;
		}
		


		if (wf->soilwFlux[layer] > 0)
		{
			
			nf->sminNH4_leach[layer]  = soilwater_NH4conc_downward * wf->soilwFlux[layer];
			nf->sminNO3_leach[layer]  = soilwater_NO3conc_downward * wf->soilwFlux[layer];
			
			cf->soil1DOC_leach[layer]  = soilwater_DOC1conc_downward * wf->soilwFlux[layer];
			cf->soil2DOC_leach[layer]  = soilwater_DOC2conc_downward * wf->soilwFlux[layer];
			cf->soil3DOC_leach[layer]  = soilwater_DOC3conc_downward * wf->soilwFlux[layer];
			cf->soil4DOC_leach[layer]  = soilwater_DOC4conc_downward * wf->soilwFlux[layer];

			nf->soil1DON_leach[layer]  = soilwater_DON1conc_downward * wf->soilwFlux[layer];
			nf->soil2DON_leach[layer]  = soilwater_DON2conc_downward * wf->soilwFlux[layer];
			nf->soil3DON_leach[layer]  = soilwater_DON3conc_downward * wf->soilwFlux[layer];
			nf->soil4DON_leach[layer]  = soilwater_DON4conc_downward * wf->soilwFlux[layer];
		}
		else
		{
			nf->sminNH4_leach[layer]  = soilwater_NH4conc_upward * wf->soilwFlux[layer];
			nf->sminNO3_leach[layer]  = soilwater_NO3conc_upward * wf->soilwFlux[layer];
			
			cf->soil1DOC_leach[layer]  = soilwater_DOC1conc_upward * wf->soilwFlux[layer];
			cf->soil2DOC_leach[layer]  = soilwater_DOC2conc_upward * wf->soilwFlux[layer];
			cf->soil3DOC_leach[layer]  = soilwater_DOC3conc_upward * wf->soilwFlux[layer];
			cf->soil4DOC_leach[layer]  = soilwater_DOC4conc_upward * wf->soilwFlux[layer];

			nf->soil1DON_leach[layer]  = soilwater_DON1conc_upward * wf->soilwFlux[layer];
			nf->soil2DON_leach[layer]  = soilwater_DON2conc_upward * wf->soilwFlux[layer];
			nf->soil3DON_leach[layer]  = soilwater_DON3conc_upward * wf->soilwFlux[layer];
			nf->soil4DON_leach[layer]  = soilwater_DON4conc_upward * wf->soilwFlux[layer];
		}


	
	


	
	


	}

	/* rootzone leaching variables: kg to g */
	cf->DOC_leachRZ   = (cf->soil1DOC_leach[epv->n_maxrootlayers-1] + cf->soil2DOC_leach[epv->n_maxrootlayers-1] + 
		                 cf->soil3DOC_leach[epv->n_maxrootlayers-1] + cf->soil4DOC_leach[epv->n_maxrootlayers-1]);

	nf->DON_leachRZ   = (nf->soil1DON_leach[epv->n_maxrootlayers-1] + nf->soil2DON_leach[epv->n_maxrootlayers-1] + 
		                 nf->soil3DON_leach[epv->n_maxrootlayers-1] + nf->soil4DON_leach[epv->n_maxrootlayers-1]);

	nf->sminN_leachRZ  = (nf->sminNH4_leach[epv->n_maxrootlayers-1]   + nf->sminNO3_leach[epv->n_maxrootlayers-1]);
		
	wf->soilwLeach_RZ  = wf->soilwFlux[epv->n_maxrootlayers-1];
	
	
	ns->sminNH4_total=0;
	ns->sminNO3_total=0;

	for (layer = 0; layer < N_SOILLAYERS; layer++)
	{

		for (datatype = 0; datatype <= n_data; datatype++)
		{	
			/* NH4 pool  */
			if (datatype == 0)
			{	
			
				state0 = ns->sminNH4[layer];
				if (layer < N_SOILLAYERS-1) state1 = ns->sminNH4[layer+1];
				lflux  = nf->sminNH4_leach[layer];
			}

			/* NO3 pool  */
			if (datatype == 1)
			{	
				state0 = ns->sminNO3[layer];
				if (layer < N_SOILLAYERS-1) state1 = ns->sminNO3[layer+1];
				lflux  = nf->sminNO3_leach[layer];


			}

			/* SOIL1C pool  */
			if (datatype == 2)
			{	
				state0 = cs->soil1c[layer];
				if (layer < N_SOILLAYERS-1) state1 = cs->soil1c[layer+1];
				lflux  = cf->soil1DOC_leach[layer];
			}

			/* SOIL2C pool  */
			if (datatype == 3)
			{	
				state0 = cs->soil2c[layer];
				if (layer < N_SOILLAYERS-1) state1 = cs->soil2c[layer+1];
				lflux  = cf->soil2DOC_leach[layer];
			}

			/* SOIL3C pool  */
			if (datatype == 4)
			{	
				state0 = cs->soil3c[layer];
				if (layer < N_SOILLAYERS-1) state1 = cs->soil3c[layer+1];
				lflux  = cf->soil3DOC_leach[layer];
			}

			/* SOIL4C pool  */
			if (datatype == 5)
			{	
				state0 = cs->soil4c[layer];
				if (layer < N_SOILLAYERS-1) state1 = cs->soil4c[layer+1];
				lflux  = cf->soil4DOC_leach[layer];
			}

			/* SOIL1N pool  */
			if (datatype == 6)
			{	
				state0 = ns->soil1n[layer];
				if (layer < N_SOILLAYERS-1) state1 = ns->soil1n[layer+1];
				lflux  = nf->soil1DON_leach[layer];
			}

			/* SOIL2N pool  */
			if (datatype == 7)
			{	
				state0 = ns->soil2n[layer];
				if (layer < N_SOILLAYERS-1) state1 = ns->soil2n[layer+1];
				lflux  = nf->soil2DON_leach[layer];
			}

			/* SOIL3N pool  */
			if (datatype == 8)
			{	
				state0 = ns->soil3n[layer];
				if (layer < N_SOILLAYERS-1) state1 = ns->soil3n[layer+1];
				lflux  = nf->soil3DON_leach[layer];
			}

			/* SOIL4N pool  */
			if (datatype == 9)
			{	
				state0 = ns->soil4n[layer];
				if (layer < N_SOILLAYERS-1) state1 = ns->soil4n[layer+1];
				lflux  = nf->soil4DON_leach[layer];
			}
			
			pool0 = state0 - lflux; 
			pool1 = state1 + lflux;
		
			if (pool0 < 0)
			{
				lflux += pool0;
				/* limitleach_flag: flag of WARNING writing in log file (only at first time) */
				if (fabs(pool0) > CRIT_PREC && !ctrl->limitleach_flag) ctrl->limitleach_flag = 1;
			}

			if (pool1 < 0)
			{
				lflux -= pool1;
				/* limitdiffus_flag: flag of WARNING writing in log file (only at first time) */
				if (fabs(pool1) > CRIT_PREC && !ctrl->limitdiffus_flag) ctrl->limitdiffus_flag = 1; 
			}

			state0    -= lflux; 
			state1    += lflux; 

			/* control */
			if(state0 < 0)
			{
				if (fabs (state0) > CRIT_PREC)
				{
					printf("FATAL ERROR: negative pool (multilayer_leaching.c)\n");
					errorCode=1;
				}
				else
				{
					ns->Nprec_snk     += state0;
					state0 = 0;
				}
			}


			/* NH4 pool  */
			if (datatype == 0)
			{	
				ns->sminNH4[layer]			= state0;
				if (layer < N_SOILLAYERS-1) ns->sminNH4[layer+1]		= state1;
				nf->sminNH4_leach[layer]   = lflux;
				nf->sminNH4_leachCUM[layer] += nf->sminNH4_leach[layer];
			}

			/* NO3 pool  */
			if (datatype == 1)
			{	
				ns->sminNO3[layer]			= state0;
				if (layer < N_SOILLAYERS-1) ns->sminNO3[layer+1]		= state1;
				nf->sminNO3_leach[layer]   = lflux;
				nf->sminNO3_leachCUM[layer] += nf->sminNO3_leach[layer];
			}

		

			/* SOIL1C pool  */
			if (datatype == 2)
			{	
				cs->soil1c[layer]			  = state0;
				if (layer < N_SOILLAYERS-1) cs->soil1c[layer+1]		      = state1;
				cf->soil1DOC_leach[layer]   = lflux;
				cs->soil1DOC[layer]		  = sprop->SOIL1_dissolv_prop * cs->soil1c[layer];
				if (layer < N_SOILLAYERS-1) cs->soil1DOC[layer+1]		  = sprop->SOIL1_dissolv_prop * cs->soil1c[layer+1];

				cf->soilDOC_leachCUM[layer] += cf->soil1DOC_leach[layer];
			}

			/* SOIL2C pool  */
			if (datatype == 3)
			{	
				cs->soil2c[layer]			  = state0;
				if (layer < N_SOILLAYERS-1) cs->soil2c[layer+1]		      = state1;
				cf->soil2DOC_leach[layer]   = lflux;
				cs->soil2DOC[layer]		  = sprop->SOIL2_dissolv_prop * cs->soil2c[layer];
				if (layer < N_SOILLAYERS-1) cs->soil2DOC[layer+1]		  = sprop->SOIL2_dissolv_prop * cs->soil2c[layer+1];

				cf->soilDOC_leachCUM[layer] += cf->soil2DOC_leach[layer];
			}

			/* SOIL3C pool  */
			if (datatype == 4)
			{	
				cs->soil3c[layer]			  = state0;
				if (layer < N_SOILLAYERS-1) cs->soil3c[layer+1]		      = state1;
				cf->soil3DOC_leach[layer]   = lflux;
				cs->soil3DOC[layer]		  = sprop->SOIL3_dissolv_prop * cs->soil3c[layer];
				if (layer < N_SOILLAYERS-1) cs->soil3DOC[layer+1]		  = sprop->SOIL3_dissolv_prop * cs->soil3c[layer+1];

				cf->soilDOC_leachCUM[layer] += cf->soil3DOC_leach[layer];
			}

			/* SOIL4C pool  */
			if (datatype == 5)
			{	
				cs->soil4c[layer]			  = state0;
				if (layer < N_SOILLAYERS-1) cs->soil4c[layer+1]		      = state1;
				cf->soil4DOC_leach[layer]   = lflux;
				cs->soil4DOC[layer]		  = sprop->SOIL4_dissolv_prop * cs->soil4c[layer];
				if (layer < N_SOILLAYERS-1) cs->soil4DOC[layer+1]		  = sprop->SOIL4_dissolv_prop * cs->soil4c[layer+1];

				cf->soilDOC_leachCUM[layer] += cf->soil4DOC_leach[layer];
			}

			/* SOIL1N pool  */
			if (datatype == 6)
			{	
				ns->soil1n[layer]			  = state0;
				if (layer < N_SOILLAYERS-1) ns->soil1n[layer+1]		      = state1;
				nf->soil1DON_leach[layer]   = lflux;
				ns->soil1DON[layer]		  = sprop->SOIL1_dissolv_prop * ns->soil1n[layer];
				if (layer < N_SOILLAYERS-1) ns->soil1DON[layer+1]		  = sprop->SOIL1_dissolv_prop * ns->soil1n[layer+1];

				nf->soilDON_leachCUM[layer] += nf->soil1DON_leach[layer];
			}

			/* SOIL2N pool  */
			if (datatype == 7)
			{	
				ns->soil2n[layer]			  = state0;
				if (layer < N_SOILLAYERS-1) ns->soil2n[layer+1]		      = state1;
				nf->soil2DON_leach[layer]   = lflux;
				ns->soil2DON[layer]		  = sprop->SOIL2_dissolv_prop * ns->soil2n[layer];
				if (layer < N_SOILLAYERS-1) ns->soil2DON[layer+1]		  = sprop->SOIL2_dissolv_prop * ns->soil2n[layer+1];

				nf->soilDON_leachCUM[layer] += nf->soil2DON_leach[layer];
			}

			/* SOIL3N pool  */
			if (datatype == 8)
			{	
				ns->soil3n[layer]			  = state0;
				if (layer < N_SOILLAYERS-1) ns->soil3n[layer+1]		      = state1;
				nf->soil3DON_leach[layer]   = lflux;
				ns->soil3DON[layer]		  = sprop->SOIL3_dissolv_prop * ns->soil3n[layer];
				if (layer < N_SOILLAYERS-1) ns->soil3DON[layer+1]		  = sprop->SOIL3_dissolv_prop * ns->soil3n[layer+1];

				nf->soilDON_leachCUM[layer] += nf->soil3DON_leach[layer];
			}

			/* SOIL4N pool  */
			if (datatype == 9)
			{	
				ns->soil4n[layer]			  = state0;
				if (layer < N_SOILLAYERS-1) ns->soil4n[layer+1]		      = state1;
				nf->soil4DON_leach[layer]   = lflux;
				ns->soil4DON[layer]		  = sprop->SOIL4_dissolv_prop * ns->soil4n[layer];
				if (layer < N_SOILLAYERS-1) ns->soil4DON[layer+1]		  = sprop->SOIL4_dissolv_prop * ns->soil4n[layer+1];

				nf->soilDON_leachCUM[layer] += nf->soil4DON_leach[layer];
			}


		} /* endfor of datatpye */
		/*-----------------------------------*/

	} /* endfor of layer */
	

	/* deepleach calculation from the bottom layer */
	ns->Ndeepleach_snk += nf->sminNH4_leach[N_SOILLAYERS-1] + nf->sminNO3_leach[N_SOILLAYERS-1]  + 
		                  nf->soil1DON_leach[N_SOILLAYERS-1] +nf->soil2DON_leach[N_SOILLAYERS-1] +
						  nf->soil3DON_leach[N_SOILLAYERS-1] +nf->soil4DON_leach[N_SOILLAYERS-1];

	cs->Cdeepleach_snk += cf->soil1DOC_leach[N_SOILLAYERS-1] +cf->soil2DOC_leach[N_SOILLAYERS-1] +
		                  cf->soil3DOC_leach[N_SOILLAYERS-1] +cf->soil4DOC_leach[N_SOILLAYERS-1];



	/* state update available SMINN */
	for (layer = 0; layer < N_SOILLAYERS; layer++)
	{
		ns->sminNH4avail[layer] = ns->sminNH4[layer] * sprop->NH4_mobilen_prop;
		ns->sminNO3avail[layer] = ns->sminNO3[layer] * NO3_mobilen_prop;
	}
	
	return (errorCode);
}

