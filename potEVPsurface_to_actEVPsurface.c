/* 
potEVPsurface_to_actEVPsurface.c
calculation of actual soil evaporation from potential soil evaporation regarding to dry day limit (Joe Rictchie, 1970)

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Biome-BGCMuSo v7.0.
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
#include "bgc_constants.h"
#include "bgc_func.h"    
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))   

int potEVPsurface_to_actEVPsurface(control_struct* ctrl, const siteconst_struct* sitec, soilprop_struct* sprop, 
	                   epvar_struct* epv, wstate_struct* ws, wflux_struct* wf)
{

	/* internal variables */
	int errorCode=0;
	double infiltPOT, soilw_hw0,  EVP_lack;
	
	sprop->coeff_EVPlim = 0.4;
	sprop->coeff_EVPcum = 3.5;
	sprop->coeff_DSRmax  = 5;

	
	/* assign water fluxes, all excess not evaporated goes to soil water compartment */
    wf->canopyw_to_soilw = wf->prcp_to_canopyw - wf->canopywEVP;

	infiltPOT     = wf->prcp_to_soilSurface + wf->snoww_to_soilw + wf->canopyw_to_soilw + wf->IRG_to_prcp;

	
	/*-----------------------------------------------------------------------------*/
	/* 1. first evaporation phase - no limit */
	if (ws->cumEVPsoil1 < sprop->soilEVPlim)
	{
		epv->DSR = 0;
	
		if (infiltPOT >= ws->cumEVPsoil1)
			ws->cumEVPsoil1 = 0;
		else
			ws->cumEVPsoil1 -= infiltPOT;
		
		if (!errorCode && EVPphase1TOphase2(sprop, epv, ws, wf))
		{
			printf("\n");
			printf("ERROR in EVPphase1TOphase2() in potEVPsurface_to_actEVPsurface.c\n");
			errorCode=1; 
		} 

	}
	/*-----------------------------------------------------------------------------*/
	/* 2. second evaporation phase - limit of DSR: ws->cumEVPsoil1 > soilEVPlim */
	else
	{
		if (infiltPOT >= ws->cumEVPsoil2) 
		{
			epv->DSR = 0;

			/* berszivárgó csapadék feltölt CUM2-t, a maradék meg a CUM1-bõl amennyit csak tud */
			infiltPOT             -= ws->cumEVPsoil2;
			ws->cumEVPsoil2   =0;

			if (infiltPOT > sprop->soilEVPlim)
				ws->cumEVPsoil1  = 0;
			else
				ws->cumEVPsoil1  -= infiltPOT;

			if (!errorCode && EVPphase1TOphase2(sprop, epv, ws, wf))
			{
				printf("\n");
				printf("ERROR in EVPphase1TOphase2() in potEVPsurface_to_actEVPsurface.c\n");
				errorCode=1; 
			} 
		}
		else
		{

			ws->cumEVPsoil2 -= infiltPOT;
			epv->DSR = pow((ws->cumEVPsoil2/sprop->coeff_EVPcum),2); 
			epv->DSR += 1;
			wf->soilwEVP = sprop->coeff_EVPcum * pow(epv->DSR, 0.5) - ws->cumEVPsoil2;
			
			if (wf->soilwEVP > wf->potEVPsurface) wf->soilwEVP = wf->potEVPsurface;
		
			ws->cumEVPsoil2 += wf->soilwEVP;
			epv->DSR = pow((ws->cumEVPsoil2/sprop->coeff_EVPcum),2); 
		}
	}


	/* control */
	if (wf->soilwEVP < 0 ||  epv->DSR < 0) 
	{
		printf("ERROR in soil evaporation calculation (potEVPsurface_to_actEVPsurface)\n");
		errorCode=1;
	}

	
	/*-----------------------------------------------------------------------------*/
	/* 3. Soil evaporation can not be larger than the extractable soil water in the top layer */

	/* actual soil water content at theoretical lower limit of water content: hygroscopic water content */
	soilw_hw0 = sprop->VWChw[0] * sitec->soillayer_thickness[0] * water_density;

	/* EVP_lack: control parameter to avoid negative soil water content (due to overestimated evaporation + dry soil) */
	EVP_lack = wf->soilwEVP - (ws->soilw[0] - soilw_hw0);

	/* theoretical lower limit of water content: hygroscopic water content. */
	if (EVP_lack > 0)
	{
		if (ws->cumEVPsoil2 >= sprop->soilEVPlim)
		{
			if (ws->cumEVPsoil2 > wf->soilwEVP)
			{
				ws->cumEVPsoil2 = ws->cumEVPsoil2 - wf->soilwEVP + (ws->soilw[0] - soilw_hw0);
				epv->DSR = pow((ws->cumEVPsoil2/sprop->coeff_EVPcum),2); 
			}
			else
			{
				ws->cumEVPsoil1 =  ws->cumEVPsoil1 - (wf->soilwEVP - ws->cumEVPsoil2);
				
				ws->cumEVPsoil2 = (ws->soilw[0] - soilw_hw0) - sprop->soilEVPlim;
				if (ws->cumEVPsoil2 < 0) ws->cumEVPsoil2 = 0;

				ws->cumEVPsoil1 =  ws->cumEVPsoil1 + (ws->soilw[0] - soilw_hw0);
				if (ws->cumEVPsoil1 < sprop->soilEVPlim) ws->cumEVPsoil2 = sprop->soilEVPlim;

				epv->DSR = pow((ws->cumEVPsoil2/sprop->coeff_EVPcum),2); 
			}
		}
		else
		{
			ws->cumEVPsoil1 =  ws->cumEVPsoil1 - wf->soilwEVP + (ws->soilw[0] - soilw_hw0);
		}

		wf->soilwEVP = (ws->soilw[0] - soilw_hw0);
		/* limitEVP_flag: flag of WARNING writing in log file (only at first time) */
		if (fabs(EVP_lack) > CRIT_PREC && !ctrl->limitEVP_flag) ctrl->limitEVP_flag = 1;
	}

	wf->soilwEVP *= epv->SC_EVPred;

	return (errorCode);
}

int EVPphase1TOphase2(const soilprop_struct* sprop, epvar_struct* epv, wstate_struct* ws, wflux_struct* wf)
{
	/* internal variables */
	int errorCode=0;


	ws->cumEVPsoil1 += wf->potEVPsurface;
	if (ws->cumEVPsoil1 > sprop->soilEVPlim)
	{
		wf->soilwEVP = wf->potEVPsurface - sprop->coeff_EVPlim*(ws->cumEVPsoil1 - sprop->soilEVPlim);
		ws->cumEVPsoil2 = (1-sprop->coeff_EVPlim)*(ws->cumEVPsoil1 - sprop->soilEVPlim);
		epv->DSR = pow(ws->cumEVPsoil2/sprop->coeff_EVPcum,2);
		ws->cumEVPsoil1 = sprop->soilEVPlim;
	}
	else
	{
		wf->soilwEVP = wf->potEVPsurface; 
	}





return (errorCode);
}