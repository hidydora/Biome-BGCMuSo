/*
spinup_bgc.c
Core BGC model logic, modified for spinup runs.  Instead of running for
a fixed number of years, as in a normal simulation, a spinup run proceeds
until steady-state is reached for the soil carbon, or until a specified
maximum number of years is reached.

Includes in-line output handling routines that write to daily and annual
output files. 

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Biome-BGCMuSo v7.0.
Original code: Copyright 2000, Peter E. Thornton
Numerical Terradynamic Simulation Group, The University of Montana, USA
Modified code: Copyright 2022, D. Hidy [dori.hidy@gmail.com]
Hungarian Academy of Sciences, Hungary
See the website of Biome-BGCMuSo at http://nimbus.elte.hu/bbgc/ for documentation, model executable and example input files.
*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <malloc.h>
#include "ini.h" 
#include "pointbgc_struct.h"   
#include "bgc_struct.h"     
#include "bgc_constants.h"
#include "bgc_func.h"
#include "bgc_io.h"
#include "pointbgc_func.h" 

int spinup_bgc(bgcin_struct* bgcin, bgcout_struct* bgcout)
{
	
	/* variable declarations */
	int errorCode=0;
	int nyears, leap;
	double spinup_tolerance;

	/* iofiles and program control variables */
	control_struct     ctrl;

	/* meteorological variables */
	metarr_struct			metarr;
	metvar_struct			metv;
	co2control_struct		co2;
	NdepControl_struct		ndep;
	
	/* state and flux variables for water, carbon, and nitrogen */
	wstate_struct      ws;
	wflux_struct       wf;
	cinit_struct       cinit;
	cstate_struct      cs;
	cflux_struct       cf;
	nstate_struct      ns;
	nflux_struct       nf;

	/* primary ecophysiological variables */
	epvar_struct       epv;

	/* site physical constants */
	siteconst_struct   sitec;

	/* soil proportion variables */
	soilprop_struct   sprop;

	/* groundwater calcultaion */
	GWcalc_struct gwc;

	/* phenological data */
	phenarray_struct   phenarr;
	phenology_struct   phen;
	
	/* ecophysiological constants */
	epconst_struct     epc;

	/* photosynthesis structures */
	psn_struct         psn_sun, psn_shade;
	
	/* temporary nitrogen variables for decomposition and allocation */
	ntemp_struct       nt;
	
	/* summary variable structure */
	summary_struct     summary;


	/* management variables */
	planting_struct     PLT;
	thinning_struct     THN;
	mowing_struct       MOW;
	grazing_struct      GRZ;
	harvesting_struct   HRV;
	ploughing_struct    PLG;
	fertilizing_struct  FRZ;
	irrigating_struct   IRG;
	mulching_struct     MUL;
	CWDextract_struct	CWE;
	flooding_struct     FLD;
	groundwater_struct  GWS;
	
	/* local storage for daily and annual output variables and output mapping (array of pointers to double)  */
	double* dayarr=0;
 	double* monavgarr=0;
	double* annavgarr=0;
	double* annarr=0;
	double** output_map=0;

	/* miscelaneous variables for program control in main */
	int endofspinup = 0;
	int spincycle = 0; 
	int simyr = 0;
	int yday  = 0;
    int nblockyear = 0; 
	int first_balance;
	int annual_alloc;
    int i, nmetdays;
	double tair_annavg;
	double CbalanceERR = -100;
	double NbalanceERR = -100;
	double WbalanceERR = -100;
	double CNratioERR = -100;

	/* variables used for monthly average output */
	int* mondays=0;
	int* enddays=0;
	
	/* spinup control */
	int ntimesmet, nblock;
	int steady1, steady2, rising, metcycle, spinyears;
	double t1      = 0;
	double tally1  = 0.0;
	double tally1b = 0;
	double tally2  = 0;
	double tally2b = 0;
	double naddfrac;
	
	/* copy the input structures into local structures */
	ws = bgcin->ws;
	cinit = bgcin->cinit;
	cs = bgcin->cs;
	ns = bgcin->ns;
	sitec = bgcin->sitec;
	sprop = bgcin->sprop;
	epc = bgcin->epc;

	PLT = bgcin->PLT; 		/* planting variables */
	THN = bgcin->THN; 		/* thinning variables */
	MOW = bgcin->MOW; 		/* mowing variables */
	GRZ = bgcin->GRZ; 		/* grazing variables */
	HRV = bgcin->HRV;		/* harvesting variables */
	PLG = bgcin->PLG;		/* harvesting variables */
	FRZ = bgcin->FRZ;		/* fertilizing variables */
	IRG = bgcin->IRG; 		/* irrigating variables */
	MUL	= bgcin->MUL;		/* mulching variables */
	CWE	= bgcin->CWE;		/* CWDextract variables */
	FLD	= bgcin->FLD;		/* flooding variables */
	GWS = bgcin->GWS;       /* groundwater variables */



	/* temporal solution for spinup tolerance differentiate (woody vs non-woody) - from MuSo7: independent parameter */
	if (epc.woody)
		spinup_tolerance = SPINUP_TOLERANCE_w;
	else
		spinup_tolerance = SPINUP_TOLERANCE_nw;

	/* note that the following three structures have dynamic memory elements,
	and so the notion of copying the input structure to a local structure
	value-by-value is not the same as above. In this case, the array pointers
	are being copied, so the local members use the same memory that was
	allocated in the calling function. Note also that bgc() does not modify
	the contents of these structures. */
	ctrl	= bgcin->ctrl;
	metarr	= bgcin->metarr;
	co2		= bgcin->co2;
	ndep	= bgcin->ndep;

 	summary.tally1=0;
	summary.tally2=0;
	summary.metcycle=0;
	summary.steady1=0;
	summary.steady2=0;
	
	/********************************************************************************************************* */
	/* writing log file */
	fprintf(bgcout->log_file.ptr, "SPINUP RUN\n");
	fprintf(bgcout->log_file.ptr, " \n");


	fprintf(bgcout->log_file.ptr, "VEGETATION TYPE\n");
	if (epc.woody == 0)
		fprintf(bgcout->log_file.ptr, "biome type           - NON-WOODY\n");
	else
		fprintf(bgcout->log_file.ptr, "biome type           - WOODY\n");
	
	if (epc.evergreen == 0)
		fprintf(bgcout->log_file.ptr, "woody type           - DECIDUOUS\n");
	else
		fprintf(bgcout->log_file.ptr, "woody type           - EVERGREEN\n");

	if (epc.c3_flag == 0)
		fprintf(bgcout->log_file.ptr, "photosyn.type        - C4 PSN\n");
	else
		fprintf(bgcout->log_file.ptr, "photosyn.type        - C3 PSN\n");
	fprintf(bgcout->log_file.ptr, " \n");

	fprintf(bgcout->log_file.ptr, "CALCULATION METHODS\n");
	if (epc.SHCM_flag == 0 || epc.SHCM_flag == 2)
	{
		if (epc.SHCM_flag == 0)
			fprintf(bgcout->log_file.ptr, "hydrology            - tipping (with diffusion)\n");
		else
			fprintf(bgcout->log_file.ptr, "hydrology            - tipping (without diffusion)\n");
	}
	else
	{
		fprintf(bgcout->log_file.ptr, "hydrology            - Richards\n");
	}

	if (epc.STCM_flag == 0)
		fprintf(bgcout->log_file.ptr, "temperature          - MuSo\n");
	else
		fprintf(bgcout->log_file.ptr, "temperature          - DSSAT\n");

	if (epc.photosynt_flag == 0)
		fprintf(bgcout->log_file.ptr, "photosynthesis       - Farquhar\n");
	else
		fprintf(bgcout->log_file.ptr, "photosynthesis       - DSSAT\n");

	if (epc.ET_flag == 0)
		fprintf(bgcout->log_file.ptr, "evapotranspiration   - Penman-Monteith\n");
	else
		fprintf(bgcout->log_file.ptr, "evapotranspiration   - Priestley-Taylor\n");

	if (epc.radiation_flag == 0)
		fprintf(bgcout->log_file.ptr, "radiation            - based on SWabs\n");
	else
		fprintf(bgcout->log_file.ptr, "radiation            - based on Rn\n");

	if (epc.soilstress_flag == 0)
		fprintf(bgcout->log_file.ptr, "soilstress           - based on VWC\n");
	else
		fprintf(bgcout->log_file.ptr, "soilstress           - based on transpiration demand\n");

	if (epc.interception_flag == 0)
		fprintf(bgcout->log_file.ptr, "interception         - based on linear function of PRCP\n");
	else
		fprintf(bgcout->log_file.ptr, "interception         - based on exponential (saturating) function of PRCP\n");

	if (epc.transferGDD_flag == 0)
		fprintf(bgcout->log_file.ptr, "transfer period      - EPC\n");
	else
		fprintf(bgcout->log_file.ptr, "transfer period      - GDD\n");

	if (epc.q10depend_flag == 0)
		fprintf(bgcout->log_file.ptr, "q10 value            - constant\n");
	else
		fprintf(bgcout->log_file.ptr, "q10 value            - temperature dependent\n");

	if (epc.phtsyn_acclim_flag == 0)
		fprintf(bgcout->log_file.ptr, "photosyn. acclim.    - no\n");
	else
		fprintf(bgcout->log_file.ptr, "photosyn. acclim.    - yes\n");

	if (epc.resp_acclim_flag == 0)
		fprintf(bgcout->log_file.ptr, "respiration acclim.  - no\n");
	else
		fprintf(bgcout->log_file.ptr, "respiration acclim.  - yes\n");

	if (epc.CO2conduct_flag == 0)
		fprintf(bgcout->log_file.ptr, "CO2 conduct. effect  - no effect\n");
	else
		fprintf(bgcout->log_file.ptr, "CO2 conduct. effect  - reduction\n");

	if (sprop.Tp1_decomp == DATA_GAP)
		fprintf(bgcout->log_file.ptr, "Decomposition Tresp. - Lloyd and Taylor\n");
	else
		fprintf(bgcout->log_file.ptr, "Decomposition Tresp. - Bell function\n");

	if (sprop.Tp1_nitrif == DATA_GAP)
		fprintf(bgcout->log_file.ptr, "Nitrification Tresp. - Lloyd and Taylor\n");
	else
		fprintf(bgcout->log_file.ptr, "Nitrification Tresp. - Bell function\n");

	fprintf(bgcout->log_file.ptr, " \n");

	fprintf(bgcout->log_file.ptr, "DATA SOURCES\n");
	if (epc.phenology_flag == 0) 
	{
		if (ctrl.varSGS_flag == 0) 
			fprintf(bgcout->log_file.ptr, "SGS data - user-defined from EPC file\n");
		else
		{
			fprintf(bgcout->log_file.ptr, "SGS data - user-defined from annual varying SGS file\n");
			if (ctrl.onscreen) printf("INFORMATION: reading onday file: annual varying SGS data\n");
		}

		if (ctrl.varEGS_flag == 0) 
			fprintf(bgcout->log_file.ptr, "EGS data - user-defined from EPC file\n");
		else
		{
			fprintf(bgcout->log_file.ptr, "EGS data - user-defined from annual varying EGS file\n");
			if (ctrl.onscreen) printf("INFORMATION: reading offday file: annual varying EGS data\n");
		}
	}
	else
	{
		if (ctrl.GSI_flag == 0) 
		{
			fprintf(bgcout->log_file.ptr, "SGS data - model estimation (with original method)\n");
			fprintf(bgcout->log_file.ptr, "EGS data - model estimation (with original method)\n");
		}
		else
		{
			fprintf(bgcout->log_file.ptr, "SGS data - model estimation (with GSI method)\n");
			fprintf(bgcout->log_file.ptr, "EGS data - model estimation (with GSI method)\n");
		}
	}

	if (ctrl.varWPM_flag == 0) 
		fprintf(bgcout->log_file.ptr, "FM data - constant \n");
	else
	{
		fprintf(bgcout->log_file.ptr, "FM data - annual varying \n");
		if (ctrl.onscreen) printf("INFORMATION: reading FM_spinup.txt or FM_transient.txt file: annual varying FM data\n");
	}

	if (ctrl.varWPM_flag == 0) 
		fprintf(bgcout->log_file.ptr, "WPM data - constant \n");
	else
	{
		fprintf(bgcout->log_file.ptr, "WPM data - annual varying \n");
		if (ctrl.onscreen) printf("INFORMATION: reading WPM_spinup.txt or WPM_transient.txt file: annual varying WPM data\n");
	}

	if (ctrl.varMSC_flag == 0) 
		fprintf(bgcout->log_file.ptr, "MSC data - constant\n");
	else
	{
		fprintf(bgcout->log_file.ptr, "MSC data - annual varying\n");
		if (ctrl.onscreen) printf("INFORMATION: reading conductance file: annual varying MSC data\n");
	}

	if (PLT.PLT_num || THN.THN_num || MOW.MOW_num || GRZ.GRZ_num || HRV.HRV_num || PLG.PLG_num || FRZ.FRZ_num || IRG.IRG_num || 
		ctrl.condIRG_flag || ctrl.condMOW_flag)
		fprintf(bgcout->log_file.ptr, "management  - YES (in transient phase)\n");
	else
		fprintf(bgcout->log_file.ptr, "management  - NO\n");

	if (GWS.GWD_num == 0) 
		fprintf(bgcout->log_file.ptr, "groundwater - NO\n");
	else
	{
		fprintf(bgcout->log_file.ptr, "groundwater - YES\n");
		if (ctrl.onscreen) printf("INFORMATION: reading groundwater_spinup.txt: daily GWD data\n");
	}

	if (FLD.FLD_num == 0) 
		fprintf(bgcout->log_file.ptr, "flooding - NO\n");
	else
	{
		fprintf(bgcout->log_file.ptr, "flooding - YES\n");
		if (ctrl.onscreen) printf("INFORMATION: reading flooding_spinup.txt - daily FLD data\n");
	}

	fprintf(bgcout->log_file.ptr, " \n");

	fprintf(bgcout->log_file.ptr, "SOIL PROPERTIES FOR 10 SOIL LAYERS (POTENTIALLY) ESTIMATED BY THE MODEL \n");
	fprintf(bgcout->log_file.ptr, "Clapp-Hornberger b parameter [dimless]:%12.3f%12.3f%12.3f%12.3f%12.3f%12.3f%12.3f%12.3f%12.3f%12.3f\n",sprop.soilB[0],sprop.soilB[1],sprop.soilB[2],sprop.soilB[3],sprop.soilB[4],sprop.soilB[5],sprop.soilB[6],sprop.soilB[7],sprop.soilB[8],sprop.soilB[9]);
	fprintf(bgcout->log_file.ptr, "bulk density [g/cm3]:                  %12.3f%12.3f%12.3f%12.3f%12.3f%12.3f%12.3f%12.3f%12.3f%12.3f\n",sprop.BD[0],sprop.BD[1],sprop.BD[2],sprop.BD[3],sprop.BD[4],sprop.BD[5],sprop.BD[6],sprop.BD[7],sprop.BD[8],sprop.BD[9]);
	fprintf(bgcout->log_file.ptr, "VWC at saturation [m3/m3]:             %12.3f%12.3f%12.3f%12.3f%12.3f%12.3f%12.3f%12.3f%12.3f%12.3f\n",sprop.VWCsat[0],sprop.VWCsat[1],sprop.VWCsat[2],sprop.VWCsat[3],sprop.VWCsat[4],sprop.VWCsat[5],sprop.VWCsat[6],sprop.VWCsat[7],sprop.VWCsat[8],sprop.VWCsat[9]);
	fprintf(bgcout->log_file.ptr, "VWC at field capacity [m3/m3]:         %12.3f%12.3f%12.3f%12.3f%12.3f%12.3f%12.3f%12.3f%12.3f%12.3f\n",sprop.VWCfc[0],sprop.VWCfc[1],sprop.VWCfc[2],sprop.VWCfc[3],sprop.VWCfc[4],sprop.VWCfc[5],sprop.VWCfc[6],sprop.VWCfc[7],sprop.VWCfc[8],sprop.VWCfc[9]);
	fprintf(bgcout->log_file.ptr, "VWC at wilting point [m3/m3]:          %12.3f%12.3f%12.3f%12.3f%12.3f%12.3f%12.3f%12.3f%12.3f%12.3f\n",sprop.VWCwp[0],sprop.VWCwp[1],sprop.VWCwp[2],sprop.VWCwp[3],sprop.VWCwp[4],sprop.VWCwp[5],sprop.VWCwp[6],sprop.VWCwp[7],sprop.VWCwp[8],sprop.VWCwp[9]);
	fprintf(bgcout->log_file.ptr, "VWC at hygroscopic water [m3/m3]:      %12.3f%12.3f%12.3f%12.3f%12.3f%12.3f%12.3f%12.3f%12.3f%12.3f\n",sprop.VWChw[0],sprop.VWChw[1],sprop.VWChw[2],sprop.VWChw[3],sprop.VWChw[4],sprop.VWChw[5],sprop.VWChw[6],sprop.VWChw[7],sprop.VWChw[8],sprop.VWChw[9]);
	fprintf(bgcout->log_file.ptr, "PSI at saturation [MPa]:               %12.3f%12.3f%12.3f%12.3f%12.3f%12.3f%12.3f%12.3f%12.3f%12.3f\n",sprop.PSIsat[0],sprop.PSIsat[1],sprop.PSIsat[2],sprop.PSIsat[3],sprop.PSIsat[4],sprop.PSIsat[5],sprop.PSIsat[6],sprop.PSIsat[7],sprop.PSIsat[8],sprop.PSIsat[9]);
	fprintf(bgcout->log_file.ptr, "PSI at field capacity [MPa]:           %12.3f%12.3f%12.3f%12.3f%12.3f%12.3f%12.3f%12.3f%12.3f%12.3f\n",sprop.PSIfc[0],sprop.PSIfc[1],sprop.PSIfc[2],sprop.PSIfc[3],sprop.PSIfc[4],sprop.PSIfc[5],sprop.PSIfc[6],sprop.PSIfc[7],sprop.PSIfc[8],sprop.PSIfc[9]);
	fprintf(bgcout->log_file.ptr, "PSI at wilting point [MPa]:            %12.3f%12.3f%12.3f%12.3f%12.3f%12.3f%12.3f%12.3f%12.3f%12.3f\n",sprop.PSIwp[0],sprop.PSIwp[1],sprop.PSIwp[2],sprop.PSIwp[3],sprop.PSIwp[4],sprop.PSIwp[5],sprop.PSIwp[6],sprop.PSIwp[7],sprop.PSIwp[8],sprop.PSIwp[9]);
	fprintf(bgcout->log_file.ptr, "drainage coefficient [prop]:           %12.3f%12.3f%12.3f%12.3f%12.3f%12.3f%12.3f%12.3f%12.3f%12.3f\n",sprop.drainCoeff[0],sprop.drainCoeff[1],sprop.drainCoeff[2],sprop.drainCoeff[3],sprop.drainCoeff[4],sprop.drainCoeff[5],sprop.drainCoeff[6],sprop.drainCoeff[7],sprop.drainCoeff[8],sprop.drainCoeff[9]);
	fprintf(bgcout->log_file.ptr, "hydr. conduct. at saturation [m/day]:  %12.3f%12.3f%12.3f%12.3f%12.3f%12.3f%12.3f%12.3f%12.3f%12.3f\n",sprop.hydrCONDUCTsat[0]*nSEC_IN_DAY,sprop.hydrCONDUCTsat[1]*nSEC_IN_DAY,sprop.hydrCONDUCTsat[2]*nSEC_IN_DAY,sprop.hydrCONDUCTsat[3]*nSEC_IN_DAY,sprop.hydrCONDUCTsat[4]*nSEC_IN_DAY,sprop.hydrCONDUCTsat[5]*nSEC_IN_DAY,sprop.hydrCONDUCTsat[6]*nSEC_IN_DAY,sprop.hydrCONDUCTsat[7]*nSEC_IN_DAY,sprop.hydrCONDUCTsat[8]*nSEC_IN_DAY,sprop.hydrCONDUCTsat[9]*nSEC_IN_DAY);
	fprintf(bgcout->log_file.ptr, "capillary fringe [m]:                  %12.3f%12.3f%12.3f%12.3f%12.3f%12.3f%12.3f%12.3f%12.3f%12.3f\n",sprop.CapillFringe[0],sprop.CapillFringe[1],sprop.CapillFringe[2],sprop.CapillFringe[3],sprop.CapillFringe[4],sprop.CapillFringe[5],sprop.CapillFringe[6],sprop.CapillFringe[7],sprop.CapillFringe[8],sprop.CapillFringe[9]);
	fprintf(bgcout->log_file.ptr, " \n");

	/********************************************************************************************************* */

	/* in case of natural ecosystem, nyears = number of meteorological year, in case agricultural system: nyears = number of plantings */
	if (!PLT.PLT_num)
		nyears = ctrl.simyears;
	else
		nyears = PLT.PLT_num + ctrl.simyears;
	
	/* allocate memory for local output arrays */
	if (!errorCode && (ctrl.dodaily || ctrl.domonavg || ctrl.doannavg)) 
	{
		dayarr = (double*) malloc(ctrl.ndayout * sizeof(double));
		if (!dayarr)
		{
			printf("ERROR allocating for local daily output array in bgc.c\n");
			errorCode=301;
		}
		for (i=0 ; i<ctrl.ndayout ; i++) dayarr[i] = 0.0;
	}

	if (!errorCode && ctrl.domonavg) 
	{
		monavgarr = (double*) malloc(ctrl.ndayout * sizeof(double));
		if (!monavgarr)
		{
			printf("ERROR allocating for monthly average output array in bgc.c\n");
			errorCode=302;
		}

		for (i=0 ; i<ctrl.ndayout ; i++) monavgarr[i] = 0.0;
	}
	if (!errorCode && ctrl.doannavg) 
	{
		annavgarr = (double*) malloc(ctrl.ndayout * sizeof(double));
		if (!annavgarr)
		{
			printf("ERROR allocating for annual average output array in bgc.c\n");
			errorCode=303;
		}
		for (i=0 ; i<ctrl.ndayout ; i++) annavgarr[i] = 0.0;
	}
	if (!errorCode && ctrl.doannual)
	{
		annarr = (double*) malloc(ctrl.nannout * sizeof(double));
		if (!annarr)
		{
			printf("ERROR allocating for local annual output array in bgc.c\n");
			errorCode=304;
		}
		for (i=0 ; i<ctrl.nannout ; i++) annarr[i] = 0.0;

	}
	

	/* allocate space for the output map pointers */
	if (!errorCode) 
	{
		output_map = (double**) malloc(NMAP * sizeof(double*));
		if (!output_map)
		{
			printf("ERROR allocating for output map in output_map_init.c\n");
			errorCode=305;
		}
	}
	
	/* allocate space for the onday_arr and offday_arr: first column - year, second column: day*/
	if (!errorCode)
	{
		phenarr.onday_arr  = (int**) malloc(nyears*sizeof(int*));  
        phenarr.offday_arr = (int**) malloc(nyears*sizeof(int*));  
			
		for (i = 0; i<nyears; i++)
		{
			phenarr.onday_arr[i]  = (int*) malloc(2*sizeof(int));  
			phenarr.offday_arr[i] = (int*) malloc(2*sizeof(int));  
			phenarr.onday_arr[i]  = (int*) malloc(2*sizeof(int));  
			phenarr.offday_arr[i] = (int*) malloc(2*sizeof(int));  
		}


		if (!phenarr.onday_arr || !phenarr.onday_arr)
		{
			printf("ERROR allocating for onday_arr/offday_arr, prephenology.c\n");
			errorCode=306;
		}
	}

	
	/* allocate space for enddays */
	if (!errorCode) 
	{
		enddays = (int*) malloc(nMONTHS_OF_YEAR * sizeof(int));
		if (!enddays)
		{
			printf("ERROR allocating for enddays in bgc.c()\n");
			errorCode=307;
		}
	}


	/* allocate space for mondays */	
	if (!errorCode) 
	{
		mondays = (int*) malloc(nMONTHS_OF_YEAR * sizeof(int));
		if (!mondays)
		{
			printf("ERROR allocating for enddays in bgc.c()\n");
			errorCode=308;
		}
	}
	
	/* initialize the output mapping array*/ 
	if (!errorCode && output_map_init(output_map,&phen,&metv,&ws,&wf,&cs,&cf,&ns,&nf,&sprop,&epv,&psn_sun,&psn_shade,&summary))
	{
		printf("ERROR in call to output_map_init.c from spinup_bgc.c\n");
		errorCode=401;
	}
	
	
	/* atmospheric pressure (Pa) as a function of elevation (m) */
	if (!errorCode && atm_pres(sitec.elev, &metv.pa))
	{
		printf("ERROR in atm_pres.c from spinup_bgc.c\n");
		errorCode=402;
	}
	

	/* calculate GSI to deterime onday and offday 	*/
	if (ctrl.GSI_flag && !PLT.PLT_num)
	{
		if (!errorCode && GSI_calculation(&metarr, &sitec, &epc, &phenarr, &ctrl))
		{
			printf("ERROR in call to GSI_calculation.c, from spinup_bgc.c\n");
			errorCode=403;
		}
	}

	/* calculate conductance limitation factors 	*/	
	if (!errorCode && conduct_limit_factors(bgcout->log_file, &ctrl, &sprop, &epc, &epv))
	{
		printf("ERROR in call to conduct_limit_factors.c, from spinup_bgc.c\n");
		errorCode=404;
	}

	/********************************************************************************************************* */
	fprintf(bgcout->log_file.ptr, "---------------- \n");
	fprintf(bgcout->log_file.ptr, "spinup phase\n");
	fprintf(bgcout->log_file.ptr, "---------------- \n");
	/********************************************************************************************************* */

	/* determine phenological signals */		
	if (!errorCode && prephenology(bgcout->log_file, &epc, &metarr, &PLT, &HRV, &ctrl, &phenarr))
	{
		printf("ERROR in call to prephenology.c, from spinup_bgc.c\n");
		errorCode=405;
	}

	
    /* calculate the annual average air temperature for use in soil temperature corrections */
	tair_annavg = 0.0;
	nmetdays = ctrl.simyears * nDAYS_OF_YEAR;
 	for (i=0 ; i<nmetdays ; i++)
	{
		tair_annavg += metarr.Tavg_array[i];
	}
	tair_annavg /= (double)nmetdays;


	/* if this simulation is using a restart file for its initialconditions, then copy restart info into structures */
	if (!errorCode && ctrl.read_restart)
	{
		if (!errorCode && restart_input(&ctrl, &epc, &sprop, &sitec, &ws, &cs, &ns, &epv, &(bgcin->restart_input)))
		{
			printf("ERROR in call to restart_input.c from spinup_bgc.c\n");
			errorCode=406;
		}
	}
	
     /* initialize leaf C and N pools depending on phenology signals for the first metday */
	 if (!errorCode && firstday(&ctrl,  &sprop,&epc, &PLT, &sitec,&cinit, &phen, &epv, &cs, &ns, &psn_sun,&psn_shade))
	 {
		printf("ERROR in call to firstday.c, from spinup_bgc.c\n");
		errorCode=407;
	 }

		
	/* zero water, carbon, and nitrogen source and sink variables */
	if (!errorCode && zero_srcsnk(&cs,&ns,&ws,&summary))
	{
		printf("ERROR in call to zero_srcsnk.c, from spinup_bgc.c\n");
		errorCode=408;
	}
	
	/* initialize the indicator for first day of current simulation, so that the checks for mass balance can have two days for comparison */
	first_balance = 1;
	
	/* for simulations with fewer than 50 metyears, find the multiple of metyears that gets close to 100, 
            use this as the block size in spinup control */
	if (ctrl.simyears < 50)
	{
		ntimesmet = 100 / ctrl.simyears;
		nblock = ctrl.simyears * ntimesmet;
	}
	else
	{
		nblock = ctrl.simyears;
	}
	
	/* initialize spinup control variables */
	spinyears = 0;
	metcycle = 0;
	steady1 = 0;
	steady2 = 0;
	rising = 1;


			

	/* do loop for spinup */
	do
	{	
	
			/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
	        /* 1. BEGIN OF THE ANNUAL LOOP */

		
		for (nblockyear=0 ; !errorCode && nblockyear<nblock ; nblockyear++)
		{
			
			/* set current month to 0 (january) at the beginning of each year */
			ctrl.curmonth = 0;

			/* controling input data: in leap year the 366th (last) day must be missing */
			if (!errorCode && leapControl(ctrl.simstartyear+simyr, enddays, mondays, &leap))
			{
				printf("ERROR in call to leapControl() from spinup_bgc.c\n");
				errorCode=500;
			}

			/* set the max lai, maturity and flowering variables, for annual diagnostic output */
			epv.annmax_lai = 0.0;
			epv.annmax_rootDepth = 0.0;
			epv.annmax_plantHeight = 0.0;
			summary.annmax_livingBIOMabove = 0.0;
			summary.annmax_livingBIOMbelow = 0.0;
			summary.annmax_totalBIOMabove = 0.0;
			summary.annmax_totalBIOMbelow = 0.0;
			
			/* atmospheric concentration of CO2 (ppm) */
			metv.co2 = co2.co2ppm;

		
			/* calculate scaling for N additions (decreasing with time since the beginning of metcycle = 0 block */	
			if ((!steady1 && rising && metcycle == 0 && ctrl.NaddSPINUP_flag))
			{
				naddfrac = (1.0 - ((double)simyr/(double)nblock));
			}
			else
			{
				naddfrac = 0;
			}

			
			if (metcycle == 0)
			{
				tally1 = 0.0;
				tally1b = 0.0;
				tally2 = 0.0;
				tally2b = 0.0;
			}
			
			/* test whether simyr needs to be reset  */
			if (simyr == ctrl.simyears)
			{
				simyr = 0;
				phen.onday = -1;
				phen.offday = -1;
				GWS.mgmdGWD = 0;
			}

	
			/* counters into control sturct */ 
			ctrl.simyr = simyr;
			ctrl.spinyears = spinyears;

	

			/* output to screen to indicate start of simulation year */
			if (spinyears == 0 && ctrl.onscreen) 
			{
				printf("-----------------------------------------\n");
				printf("Start of spinup simulation. Please, wait!\n");
			}
		
			if (ctrl.onscreen) printf("*");
			
	
			/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
		    /* 2. BEGIN OF THE DAILY LOOP */

			for (yday=0 ; !errorCode && yday<nDAYS_OF_YEAR ; yday++)
			{
				
				/* set the day index for meteorological and phenological arrays */
				ctrl.yday   = yday;
				ctrl.metday	= simyr*nDAYS_OF_YEAR + yday;			
				

			

				/* set fluxes to zero */
				if (!errorCode && make_zero_flux_struct(&wf, &cf, &nf, &gwc))
				{
					printf("ERROR in call to make_zero_flux_struct.c from spinup_bgc.c\n");
					errorCode=501;
				}

				/* initalizing annmax and cumulative variables */
				if (yday == 0)
				{
					if (!errorCode && annVARinit(&summary, &epv, &cs, &ws, &cf, &nf))
					{
						printf("ERROR in call to annVARinit() from bgc.c\n");
						errorCode=502;
					}
				}

                /* nitrogen deposition and fixation */
			    nf.ndep_to_sminn_total = ndep.ndep / nDAYS_OF_YEAR;
			    nf.nfix_to_sminn_total = epc.nfix / nDAYS_OF_YEAR;
		
				/* calculating actual onday and offday */
				if (!errorCode && dayphen(&ctrl, &epc, &phenarr, &PLT, &phen))
				{
					printf("ERROR in dayphen from spinup_bgc.c\n");
					errorCode=503;
				}


				/* setting MANAGEMENT and GW days based on input data */
				if (!errorCode && management(&ctrl, &FRZ, &GRZ, &HRV, &MOW, &PLT, &PLG, &THN, &IRG, &MUL, &CWE, &FLD, &GWS, mondays))
				{
					printf("ERROR in management days() from bgc.c\n");
					errorCode=504;
				}
				
	
	                      /* determining soil hydrological parameters  */
 				if (!errorCode && multilayer_hydrolparams(&sitec, &sprop, &ws, &epv))
				{
					printf("ERROR in multilayer_hydrolparams() from spinup_bgc.c\n");
					errorCode=505;
				}
				

				/* daily meteorological variables from metarrays */
				if (!errorCode && daymet(&ctrl, &metarr, &epc, &metv, ws.snoww))
				{
					printf("ERROR in daymet() from spinup_bgc.c\n");
					errorCode=506;
				}
		        

				/* phenophases calculation */
				if (!errorCode && phenphase(bgcout->log_file, &ctrl, &epc, &sprop, &PLT, &phen, &metv, &epv, &cs))
				{
					printf("ERROR in phenphase() from spinup_bgc.c\n");
					errorCode=507;
				}
				


				/* soil temperature calculations */
				if (!errorCode && multilayer_tsoil(&epc, &sitec, &sprop, &epv, yday, ws.snoww, &metv))
				{
					printf("ERROR in multilayer_tsoil() from spinup_bgc.c\n");
					errorCode=508;
				}
				

	
				/* soilCover calculations */
				if (!errorCode && soilCover(&sitec, &sprop, &metv, &epv, &cs))
				{
					printf("ERROR in soilCover() from spinup_bgc.c\n");
					errorCode=509;
				}
				


				/* phenology calculation */
				if (!errorCode && phenology(&epc, &cs, &ns, &phen, &metv, &epv, &cf, &nf))
				{
					printf("ERROR in phenology() from spinup_bgc.c\n");
					errorCode=510;
				}
				

				/* calculate leaf area index, sun and shade fractions, and specific leaf area for sun and shade canopy fractions, then calculate canopy radiation interception and transmission */
				if (!errorCode && radtrans(&ctrl, &phen, &cs, &epc, &sitec, &metv, &epv))
				{
					printf("ERROR in radtrans() from spinup_bgc.c\n");
					errorCode=511;
				}


				/* update the annmax LAI/rootingDepth/plantHeight  for annual diagnostic output */
				if (epv.proj_lai > epv.annmax_lai)             epv.annmax_lai = epv.proj_lai;
				if (epv.rootDepth > epv.annmax_rootDepth)      epv.annmax_rootDepth = epv.rootDepth;
				if (epv.plantHeight > epv.annmax_plantHeight) epv.annmax_plantHeight = epv.plantHeight;
		

				/* precip routing (when there is precip) */
				if (!errorCode && metv.prcp && prcpANDrunoffH(&metv, &sprop, &epc, &epv, &wf))
				{
					printf("ERROR in prcpANDrunoffH() from spinup_bgc.c\n");
					errorCode=513;
				}
				

				/* snowmelt (when there is a snowpack) */
				if (!errorCode && ws.snoww && snowmelt(&metv, &wf, ws.snoww))
				{
					printf("ERROR in snowmelt() from spinup_bgc.c\n");
					errorCode=514;
				}
				
	
				/* potential evaporation and transpiration */
				if (!errorCode && Elimit_and_PET(&epc, &sprop, &metv, &epv, &wf))
				{
					printf("ERROR in Elimit_and_PET() from bgc.c\n");
					errorCode=515;
				}
				

				/* conductance calculation */
				if (!errorCode && conduct_calc(&ctrl, &metv, &epc, &epv, simyr))
				{
					printf("ERROR in conduct_calc() from spinup_bgc.c\n");
					errorCode=516;
				}
			        
	

				/* begin canopy bio-physical process simulation */
				/* do canopy ET calculations whenever there is leaf areadisplayed, since there may be intercepted water on the canopy that needs to be dealt with */
				if (!errorCode && epv.n_actphen > epc.n_emerg_phenophase && metv.dayl)
				{
					/* evapotranspiration */
					if (!errorCode && cs.leafc && canopy_et(&epc, &metv, &epv, &wf))
					{
						printf("ERROR in canopy_et() from spinup_bgc.c\n");
						errorCode=517;
					}

				}
				
				

				/* daily maintenance respiration */
				if (!errorCode && maint_resp(&PLT, &cs, &ns, &epc, &metv, &epv, &cf))
				{
					printf("ERROR in m_resp.c from spinup_bgc.c\n");
					errorCode=518;
				}
				

		
				/* do photosynthesis calculation */
				if (!errorCode && cs.leafc && photosynthesis(&epc, &metv, &cs, &ws, &phen, &epv, &psn_sun, &psn_shade, &cf))
				{
					printf("ERROR in photosynthesis.c from spinup_bgc.c\n");
					errorCode=519;
				}
				

			
				/* daily litter and soil decomp and nitrogen fluxes */
				if (!errorCode && decomp(&metv,&epc,&sprop,&sitec,&cs,&ns,&epv,&cf,&nf,&nt))
				{
					printf("ERROR in decomp.c from bgc.c\n");
					errorCode=520;
				}
				

				/* Daily allocation gets called whether or not this is a current growth day, because the competition between decomp
				immobilization fluxes and plant growth N demand is resolved here.  
				On days with no growth, no allocation occurs, but immobilization fluxes are updated normally */
				/* spinup control: in the rising limb, use the spinup allocation code that supplements N supply */
				if ((!steady1 && rising && metcycle == 0)) 
				{
					if (!errorCode && daily_allocation(&epc,&sprop,&metv,&ndep,&cs,&ns,&cf,&nf,&epv,&nt,naddfrac))
					{
						printf("ERROR in daily_allocation.c from bgc.c\n");
						errorCode=521;
					}

				}
				else
				{
				
					if (!errorCode && daily_allocation(&epc,&sprop,&metv,&ndep,&cs,&ns,&cf,&nf,&epv,&nt,0))
					{
						printf("ERROR in daily_allocation.c from bgc.c\n");
						errorCode=521;
					}
				}
				

            
				/* heat stress during flowering can affect daily allocation of yield */
				if (epc.n_flowHS_phenophase > 0)
				{
					if (!errorCode && flowering_heatstress(&epc, &metv, &cs, &epv, &cf, &nf))
					{
						printf("ERROR in flowering_heatstress.c from spinup_bgc.c\n");
						errorCode=522;
					}
				}
				

				/* reassess the annual turnover rates for livewood --> deadwood, and for evergreen leaf and fine root litterfall. 
				This happens once each year, on the annual_alloc day (the last litterfall day - test for annual allocation day) */
			
				if (phen.remdays_litfall == 1) 
					annual_alloc = 1; 
				else 
					annual_alloc = 0;

                                /* litterfall rates once a year */
				if (!errorCode && annual_alloc)
				{
					if (!errorCode && annual_rates(&epc,&epv))
					{
						printf("ERROR in annual_rates.c from spinup_bgc.c\n");
						errorCode=523;
					}


				} 
				

				/* daily growth respiration */
				if (!errorCode && growth_resp(&epc, &cf))
				{
					printf("ERROR in growth_resp.c from bgc.c\n");
					errorCode=524;
				}
				


				/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
				/* 3. WATER CALCULATIONS WITH STATE UPDATE */

				/* EVAPORATION: calculation of actual evaporation from potential evaporation */
				if (!errorCode && potEVPsurface_to_actEVPsurface(&ctrl, &sitec, &sprop, &epv, &ws, &wf))
				{
					printf("ERROR in potEVPsurface_to_actEVPsurface() from bgc.c()\n");
					errorCode=525;
				}
				

				/* multilayer soil hydrology: percolation calculation based on PRCP, RUNOFF, EVP, TRANS */
     			if (!errorCode && multilayer_hydrolprocess(&ctrl, &sitec, &sprop, &epc, &epv, &ws, &wf, &GWS, &gwc, &FLD, mondays))
				{ 
					printf("ERROR in multilayer_hydrolprocess() from spinup_bgc.c\n");
					errorCode=526;
				}
				

		
				/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
				/* 4. STATE UPDATE */

				/* daily update of the water state variables */
				if (!errorCode && water_state_update(&wf, &ws))
				{
					printf("ERROR in water_state_update.c from spinup_bgc.c\n");
					errorCode=527;
				}
				

				/* daily update of carbon and nitrogen state variables */
				if (!errorCode && CN_state_update(&sitec, &epc, &ctrl, &epv, &cf, &nf, &cs, &ns, annual_alloc, epc.evergreen))
				{
					printf("ERROR in CN_state_update.c from spinup_bgc.c\n");
					errorCode=528;
				}
				

		
		
				/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
				/* 5. MORTALITY AND NITROGEN FLUXES CALCULATION WITH OWN STATE UPDATE: 
				to insure that pools don't go negative due to mortality/leaching fluxes conflicting with other proportional fluxes */

				/* calculate daily senescence mortality fluxes and update state variables */
				if (!errorCode && senescence(&sitec, &epc, &GRZ, &metv, &ctrl, &cs, &cf, &ns, &nf, &epv))
				{
					printf("ERROR in senescence.c from spinup_bgc.c\n");
					errorCode=529;
				}
				

		        
				/* calculate daily mortality fluxes  and update state variables */
				if (!errorCode && mortality(&ctrl, &sitec, &epc, &epv, &cs, &cf, &ns, &nf, simyr))
				{
					printf("ERROR in mortality.c from spinup_bgc.c\n");
					errorCode=530;
				}
				

		
				/* calculate the change of soil mineralized N in multilayer soil */ 
				if (!errorCode && multilayer_sminn(&ctrl, &metv, &sprop, &sitec, &cf, &ndep, &epv, &ns, &nf))
				{
					printf("ERROR in multilayer_sminn.c from spinup_bgc.c\n");
					errorCode=531;
				}
				


				/* calculate the leaching of N, DOC and DON from multilayer soil */
				if (!errorCode && multilayer_leaching(&sprop, &epv, &ctrl, &cs, &cf, &ns, &nf, &ws, &wf))
				{
					printf("ERROR in multilayer_leaching.c from spinup_bgc.c\n");
					errorCode=532;
				}

				 /* calculating rooting depth, n_rootlayers, n_maxrootlayers, rootlengthProp */
 				 if (!errorCode && multilayer_rootDepth(&epc, &sprop, &cs, &sitec, &epv))
				 {
					printf("ERROR in multilayer_rootDepth.c from spinup_bgc.c\n");
					errorCode=543;
				 }
				 


				/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
				/* 6. ERROR CHECKING AND SUMMARY VARIABLES  */
			
				/* test for very low state variable values and force them to 0.0 to avoid rounding and floating point overflow errors */
				if (!errorCode && precision_control(&ws, &cs, &ns))
				{
					printf("ERROR in call to precision_control.c from spinup_bgc.c\n");
					errorCode=544;
				} 
				



				/* test for water balance */
				if (!errorCode && check_water_balance(&ws, first_balance))
				{
					printf("ERROR in check_water_balance.c from spinup_bgc.c\n");
					errorCode=545;
				}
				


				/* test for carbon balance */
				if (!errorCode && check_carbon_balance(&cs, first_balance))
				{
					printf("ERROR in check_carbon_balance.c from spinup_bgc.c\n");
					errorCode=546;
				}
				
	

				/* test for nitrogen balance  */
				if (!errorCode && check_nitrogen_balance(&ns, first_balance))
				{
					printf("ERROR in check_nitrogen_balance.c from spinup_bgc.c\n");
					errorCode=547;
				}
				

			
				/* calculate summary variables */
				if (!errorCode && cnw_summary(&epc, &sitec, &sprop, &metv, &cs, &cf, &ns, &nf, &wf, &epv, &summary))
				{
					printf("ERROR in cnw_summary.c from spinup_bgc.c\n");
					errorCode=548;
				} 
				
	
				/* output handling */
				if (!errorCode && output_handling(mondays, enddays, &ctrl, output_map, dayarr, monavgarr, annavgarr, annarr, 
					                            bgcout->dayout, bgcout->monavgout, bgcout->annavgout, bgcout->annout))
				{
					printf("ERROR in output_handling.c from spinup_bgc.c\n");
					errorCode=549;
				}
				
	
	
			/*  if no dormant period (e.g. evergreen): last day is the dormant day */
			if (phen.offday - phen.onday == 364 && phen.offday == phen.yday_total) 
			{
				epv.n_actphen = 0;
				phen.onday = -1;
				phen.offday = -1;
				phen.remdays_litfall =-1;
			}
				/* if this is the last day of the current month: increment current month counter */
				if (yday == enddays[ctrl.curmonth]) ctrl.curmonth++;	
				
				/* spinup control */
				/* keep a tally of total soil C during successive met cycles for comparison */
				if (metcycle == 1)
				{
					tally1 += summary.soilC_total;
					tally1b += summary.totalC;
				}
				if (metcycle == 2)
				{
					tally2 += summary.soilC_total;
					tally2b += summary.totalC;
				}


				/* at the end of first day of simulation, turn off the 
				first_balance switch */
				if (first_balance) first_balance = 0;

		}   /* end of daily model loop */

		simyr++;
		/* spinup control */
  		spinyears++;
			
		}   /* end of annual model loop */
		
		/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
		/* 7. spinup control */
		/* if this is the third pass through metcycle, do comparison */
		/* first block is during the rising phase */
	
		/* convert tally1 and tally2 to average daily soilc */
		if (metcycle == 1) tally1 /= (double)nblock * nDAYS_OF_YEAR;
		if (metcycle == 2) tally2 /= (double)nblock * nDAYS_OF_YEAR;

		if (!steady1 && metcycle == 2)
		{
			rising = (tally2 > tally1);
			t1 = (tally2-tally1)/(double)nblock;
			steady1 = (fabs(t1) < spinup_tolerance);
			metcycle = 0;
	
		}
		/* second block is after supplemental N turned off */
		else if (steady1 && metcycle == 2)
		{
		
			t1 = (tally2-tally1)/(double)nblock;
			steady2 = (fabs(t1) < spinup_tolerance);

			/* if rising above critical rate, back to steady1=0 */
			if (t1 > spinup_tolerance)
			{
				steady1 = 0;
				rising = 1;
			}	
			metcycle = 0;
		}
			
		else
		{
			metcycle++;
		}

	
	summary.tally1=tally1;
	summary.tally2=tally2;
	summary.metcycle=metcycle*1.0;
	summary.steady1=steady1*1.0;
	summary.steady2=steady2*1.0;

	

	/* end of spinup test:  errorCode and maximum spinup cycle are determinative */
	spincycle = spinyears / ctrl.simyears;
	if (!errorCode)
	{
		if (spincycle < ctrl.maxspincycles)
		{
			if ((steady1 || steady2) && metcycle != 0)
			{
				endofspinup = 1;
			}
			else
			{
				endofspinup = 0;
				
				/* new option from MuSo6.4-b2: end of spinup if totalSOC reaches the critical (user-defined) value */
				if (sprop.totalSOCcrit != DATA_GAP) 
				{
					if (summary.soilC_total > sprop.totalSOCcrit) endofspinup = 1;
				}
			}

		
		}
		else
			endofspinup = 1;
	}
	else
		endofspinup = 1;
	


	
	/* end of do block, test for steady state */	
	} while (endofspinup == 0);
	

	/********************************************************************************************************* */
	/* 8. writing log file */
    /********************************************************************************************************* */

	/* most important informations - onscreen */

if (ctrl.onscreen) 
	{
		printf("\n");
		printf("INFORMATION FROM THE LAST SIMULATION YEAR.\n");
	    printf("Maximum projected LAI [m2/m2]:                 %12.1f\n",epv.annmax_lai);
		printf("Aboveground litter carbon content [kgC/m2]:    %12.1f\n",cs.litrc_above);
		printf("Aboveground CWD carbon content [kgC/m2]:       %12.1f\n",cs.cwdc_above);
		printf("Total soil carbon content [kgC/m2]:            %12.1f\n",summary.soilC_total);
	}

	if (cs.CbalanceERR != 0) CbalanceERR = log10(cs.CbalanceERR);
	if (ns.NbalanceERR != 0) NbalanceERR = log10(ns.NbalanceERR);
	if (ws.WbalanceERR != 0) WbalanceERR = log10(ws.WbalanceERR);
	if (cs.CNratioERR  != 0) CNratioERR  = log10(cs.CNratioERR);

	fprintf(bgcout->log_file.ptr, "SOME IMPORTANT ANNUAL OUTPUTS FROM LAST SIMULATION YEAR\n");
	fprintf(bgcout->log_file.ptr, "Cumulative sum of GPP [gC/m2/year]:                      %12.1f\n",summary.cumGPP*1000);
	fprintf(bgcout->log_file.ptr, "Cumulative sum of NEE [gC/m2/year]:                      %12.1f\n",summary.cumNEE*1000);
	fprintf(bgcout->log_file.ptr, "Cumulative sum of ET  [kgH2O/m2/year]:                   %12.1f\n",summary.cumET);
	fprintf(bgcout->log_file.ptr, "Cumulative sum of soil evaporation [kgH2O/m2/year]:      %12.1f\n",summary.cumEVP);
	fprintf(bgcout->log_file.ptr, "Cumulative sum of transpiration [kgH2O/m2/year]:         %12.1f\n",summary.cumTRP);
	fprintf(bgcout->log_file.ptr, "Cumulative sum of N2O flux [gN/m2/year]:                 %12.2f\n",summary.cumN2Oflux*1000);
	fprintf(bgcout->log_file.ptr, "Maximum projected LAI [m2/m2]:                           %12.2f\n",epv.annmax_lai);
	fprintf(bgcout->log_file.ptr, "Aboveground litter carbon content [kgC/m2/year]:         %12.2f\n",cs.litrc_above);
	fprintf(bgcout->log_file.ptr, "Aboveground CWD carbon content [kgC/m2/year]:            %12.2f\n",cs.cwdc_above);
	fprintf(bgcout->log_file.ptr, "Soil carbon content (in 0-30 cm soil layer) [%%]:         %12.2f\n",summary.SOM_C_top30);
	fprintf(bgcout->log_file.ptr, "Total soil carbon content [kgC/m2/year]:                 %12.2f\n",summary.soilC_total);
	fprintf(bgcout->log_file.ptr, "Averaged available soil ammonium content (0-30 cm) [ppm]:%12.2f\n",summary.NH4_top30avail);
	fprintf(bgcout->log_file.ptr, "Averaged available soil nitrate content (0-30 cm) [ppm]: %12.2f\n",summary.NO3_top30avail);
	fprintf(bgcout->log_file.ptr, "Averaged soil water content  [m3/m3]:                    %12.2f\n",epv.VWC_avg);
	fprintf(bgcout->log_file.ptr, " \n");
	fprintf(bgcout->log_file.ptr, "10-base logarithm of the maximum carbon balance diff.:   %12.1f\n",CbalanceERR);
	fprintf(bgcout->log_file.ptr, "10-base logarithm of the maximum nitrogen balance diff.: %12.1f\n",NbalanceERR);
	fprintf(bgcout->log_file.ptr, "10-base logarithm of the maximum water balance diff.:    %12.1f\n",WbalanceERR);
	fprintf(bgcout->log_file.ptr, "10-base logarithm of the C-N calc. numbering error:      %12.1f\n",CNratioERR);
	fprintf(bgcout->log_file.ptr, " \n");
 	fprintf(bgcout->log_file.ptr, "Total N-plus (spinup daily allocation) (kgN):	         %12.2f\n",ns.SPINUPsrc);
	fprintf(bgcout->log_file.ptr, " \n");
		fprintf(bgcout->log_file.ptr, "WARNINGS \n");
	if (!ctrl.limitTRP_flag && !ctrl.limitEVP_flag && !ctrl.limitleach_flag && !ctrl.limitleach_flag && !ctrl.limitdiffus_flag &&
		!ctrl.limitSNSC_flag && !ctrl.limitMR_flag && !ctrl.noTRP_flag && !ctrl.noMR_flag && !ctrl.pond_flag && 
		!ctrl.condMOWerr_flag && !ctrl.condIRGerr_flag && !ctrl.condIRGerr_flag && !ctrl.prephen1_flag && !ctrl.prephen2_flag && 
		!ctrl.bareground_flag && !ctrl.vegper_flag && !ctrl.allocControl_flag)
	{
		fprintf(bgcout->log_file.ptr, "no WARNINGS\n");
	}
	else
	{
		if (ctrl.limitTRP_flag)
		{
			fprintf(bgcout->log_file.ptr, "Limited transpiration due to dry soil\n");
			ctrl.limitTRP_flag = -1;
		}

		if (ctrl.limitEVP_flag)
		{
			fprintf(bgcout->log_file.ptr, "Limited evaporation due to dry soil\n");
			ctrl.limitEVP_flag = -1;
		}

		if (ctrl.limitleach_flag)
		{
			fprintf(bgcout->log_file.ptr, "Limited leaching\n");
			ctrl.limitleach_flag = -1;
		}

		if (ctrl.limitdiffus_flag)
		{
			fprintf(bgcout->log_file.ptr, "Limited diffusing\n");
			ctrl.limitdiffus_flag = -1;
		}

		if (ctrl.limitSNSC_flag)
		{
			fprintf(bgcout->log_file.ptr, "Limited genetically programmed leaf senescence\n");
			ctrl.limitSNSC_flag = -1;
		}

		if (ctrl.limitMR_flag)
		{
			fprintf(bgcout->log_file.ptr, "Limited maintanance respiration \n");
			ctrl.limitMR_flag = -1;
		}

		if (ctrl.noTRP_flag)
		{
			fprintf(bgcout->log_file.ptr, "No transpiration (full limitation) due to dry soil\n");
			ctrl.noTRP_flag = -1;
		}

		if (ctrl.noMR_flag)
		{
			fprintf(bgcout->log_file.ptr, "No woody maintanance respiration  (full limitation)\n");
			ctrl.noMR_flag = -1;
		}

		if (ctrl.pond_flag)
		{
			fprintf(bgcout->log_file.ptr, "Pond water on soil surface\n");
			ctrl.noTRP_flag = -1;
		}

	

	
		if (ctrl.prephen1_flag)
		{
			fprintf(bgcout->log_file.ptr, "In case of planting model-defined phenology is not possible (firstday:planting, lasTday:harvesting)\n");
			ctrl.prephen1_flag = -1;
		}

		if (ctrl.prephen2_flag)
		{
			fprintf(bgcout->log_file.ptr, "In case of user-defined phenology GSI calculation is not possible\n");
			ctrl.prephen2_flag = -1;
		}

		if (ctrl.bareground_flag)
		{
			fprintf(bgcout->log_file.ptr, "User-defined bareground run (onday and offday set to -9999 in EPC)\n");
			ctrl.bareground_flag = -1;
		}

		if (ctrl.vegper_flag)
		{
			fprintf(bgcout->log_file.ptr, "Vegetation period has not ended until the last day of year, the offday is equal to the last day of year\n");
			ctrl.vegper_flag = -1;
		}

		if (ctrl.allocControl_flag)
		{
			fprintf(bgcout->log_file.ptr, "Adjustment of allocation parameters due to small error (<10-4) in the setting of allocation parameters\n");
			ctrl.allocControl_flag = -1;
		}
	}

	fprintf(bgcout->log_file.ptr, " \n");
	fprintf(bgcout->log_file.ptr,"spinyears = %d \n",spinyears);
	fprintf(bgcout->log_file.ptr, " \n");
	
	/********************************************************************************************************* */

	/* save some information on the end status of spinup */
	tally1b /= (double)nblock * nDAYS_OF_YEAR;
	tally2b /= (double)nblock * nDAYS_OF_YEAR;
	bgcout->spinup_resid_trend = (tally2b-tally1b)/(double)nblock;
	bgcout->spinup_years = spinyears;
	
	if (ctrl.onscreen) printf("\n");
	if (ctrl.onscreen) printf("SPINUP: residual trend   = %.6lf\n",bgcout->spinup_resid_trend);
 	if (ctrl.onscreen) printf("SPINUP: number of years  = %d\n",bgcout->spinup_years);

	
	/********************************************************************************************************* */
	
	/* free memory for local output arrays */

 	if (((errorCode == 0 || errorCode > 403) && ctrl.GSI_flag) || ((errorCode == 0 || errorCode > 405) && !ctrl.GSI_flag)) 
	{
		free(phenarr.onday_arr);
		free(phenarr.offday_arr);
		if (ctrl.GSI_flag)
		{
			free(phenarr.Tmin_index);
			free(phenarr.vpd_index);
			free(phenarr.dayl_index);
			free(phenarr.gsi_indexAVG);
			free(phenarr.heatsum_index);
			free(phenarr.heatsum);
		}
	}
    if ((errorCode == 0 || errorCode > 301) && ctrl.dodaily) free(dayarr);
	if ((errorCode == 0 || errorCode > 302) && ctrl.domonavg) free(monavgarr);
	if ((errorCode == 0 || errorCode > 303) && ctrl.doannavg) free(annavgarr);
	if ((errorCode == 0 || errorCode > 304) && ctrl.doannual) free(annarr); 
	if ((errorCode == 0 || errorCode > 305)) free(output_map);
	if (errorCode == 0 || errorCode > 307) free(enddays);
	if (errorCode == 0 || errorCode > 308) free(mondays);
		
	/* print timing info if error */
	if (errorCode)
	{
		printf("ERROR at year %d\n",ctrl.spinyears);
		printf("ERROR at yday %d\n",yday);
	}
	

	/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
	/* 9. TRANSIENT RUN between spinup and normal run  */
  	if (!errorCode && (co2.varco2 || ndep.varndep))
	{
	
		bgcin->ws = ws;
		bgcin->cs = cs;
		bgcin->ns = ns;
		
		bgcin->cinit.max_leafc      = epv.annmax_leafc;
		bgcin->cinit.max_frootc     = epv.annmax_frootc;
		bgcin->cinit.max_yieldc     = epv.annmax_yieldc;
		bgcin->cinit.max_softstemc  = epv.annmax_softstemc;
		bgcin->cinit.max_livestemc  = epv.annmax_livestemc;
		bgcin->cinit.max_livecrootc = epv.annmax_livecrootc;


		if (ctrl.onscreen) 
		{
			printf("-----------------------------------------\n");
			printf("Start of transient run.\n");
		}

		errorCode = transient_bgc(bgcin, bgcout);
		if (errorCode)
		{
			printf("ERROR in call to transient_bgc.c\n");
		}

		ws = bgcin->ws;
		cs = bgcin->cs;
		ns = bgcin->ns;
	}

	/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */

	/* 10. RESTART OUTPUT HANDLING */
	/* if write_restart flag is set, copy data to the output restart struct */
	if (!errorCode && ctrl.write_restart)
	{
		if (restart_output(&ws, &cs, &ns, &epv, &(bgcout->restart_output)))
		{
			printf("ERROR in call to restart_output.c from spinup_bgc.c\n");
			errorCode=600;
		}
		
	
	}

	/* return error status */	
	return (errorCode);
 }
