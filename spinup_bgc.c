/*
spinup_bgc.c
Core BGC model logic, modified for spinup runs.  Instead of running for
a fixed number of years, as in a normal simulation, a spinup run proceeds
until steady-state is reached for the soil carbon, or until a specified
maximum number of years is reached.

Includes in-line output handling routines that write to daily and annual
output files. 

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Biome-BGCMuSo v6.0.
Original code: Copyright 2000, Peter E. Thornton
Numerical Terradynamic Simulation Group, The University of Montana, USA
Modified code: Copyright 2019, D. Hidy [dori.hidy@gmail.com]
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
/* #define DEBUG  set this to see function roll-call on-screen */
/* #define DEBUG_SPINUP set this to see the spinup details on-screen */

int spinup_bgc(bgcin_struct* bgcin, bgcout_struct* bgcout)
{
	
	/* variable declarations */
	int errflag=0;
	int nyears;

	/* iofiles and program control variables */
	control_struct     ctrl;

	/* meteorological variables */
	metarr_struct			metarr;
	metvar_struct			metv;
	co2control_struct		co2;
	ndep_control_struct		ndep;
	
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

	
	/* local storage for daily and annual output variables and output mapping (array of pointers to double)  */
	double* dayarr=0;
	double* monavgarr=0;
	double* annavgarr=0;
	double* annarr=0;
	double** output_map=0;

	/* miscelaneous variables for program control in main */
	int simyr = 0;
	int yday  = 0;
    int nblockyear = 0; 
	int metday= 0; 
	int first_balance;
	int annual_alloc;
    int i, nmetdays;
	double tair_annavg;
	double CbalanceERR = 0;
	double NbalanceERR = 0;
	double WbalanceERR = 0;
	double CNerror = 0;

	/* variables used for monthly average output */
	int curmonth;
	int mondays[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
	int endday[12] = {30,58,89,119,150,180,211,242,272,303,333,364};
	
	/* spinup control */
	int ntimesmet, nblock;
	int steady1, steady2, rising, metcycle, spinyears;
	double t1      = 0;
	double tally1  = 0;
	double tally1b = 0;
	double tally2  = 0;
	double tally2b = 0;
	double naddfrac;
	int nadd_flag = 0;
	
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
#ifdef DEBUG
	printf("done copy input\n");
#endif

 
	
	/********************************************************************************************************* */
	/* writing log file */
	fprintf(bgcout->log_file.ptr, "SPINUP RUN\n");
	fprintf(bgcout->log_file.ptr, " \n");


	fprintf(bgcout->log_file.ptr, "VEGETATION TYPE\n");
	if (epc.woody == 0)
		fprintf(bgcout->log_file.ptr, "biome type          - NON-WOODY\n");
	else
		fprintf(bgcout->log_file.ptr, "biome type          - WOODY\n");
	
	if (epc.evergreen == 0)
		fprintf(bgcout->log_file.ptr, "woody type          - DECIDUOUS\n");
	else
		fprintf(bgcout->log_file.ptr, "woody type          - EVERGREEN\n");

	if (epc.c3_flag == 0)
		fprintf(bgcout->log_file.ptr, "photosyn.type       - C4 PSN\n");
	else
		fprintf(bgcout->log_file.ptr, "photosyn.type       - C3 PSN\n");
	fprintf(bgcout->log_file.ptr, " \n");

	fprintf(bgcout->log_file.ptr, "CALCULATION METHODS\n");
	if (epc.SHCM_flag == 0)
		fprintf(bgcout->log_file.ptr, "hydrology           - MuSo\n");
	else
		fprintf(bgcout->log_file.ptr, "hydrology           - DSSAT\n");

	if (epc.STCM_flag == 0)
		fprintf(bgcout->log_file.ptr, "temperature         - MuSo\n");
	else
		fprintf(bgcout->log_file.ptr, "temperature         - DSSAT\n");

	if (epc.photosynt_flag == 0)
		fprintf(bgcout->log_file.ptr, "photosynthesis      - Farquhar\n");
	else
		fprintf(bgcout->log_file.ptr, "photosynthesis      - DSSAT\n");

	if (epc.evapotransp_flag == 0)
		fprintf(bgcout->log_file.ptr, "evapotranspiration  - Penman-Montieth\n");
	else
		fprintf(bgcout->log_file.ptr, "evapotranspiration  - Priestly-Taylor\n");

	if (epc.radiation_flag == 0)
		fprintf(bgcout->log_file.ptr, "radiation           - based on SWabs\n");
	else
		fprintf(bgcout->log_file.ptr, "radiation           - based on Rn\n");

	if (epc.soilstress_flag == 0)
		fprintf(bgcout->log_file.ptr, "soilstress          - based on VWC\n");
	else
		fprintf(bgcout->log_file.ptr, "soilstress          - based on transp.demand\n");

	if (epc.transferGDD_flag == 0)
		fprintf(bgcout->log_file.ptr, "transfer period     - EPC\n");
	else
		fprintf(bgcout->log_file.ptr, "transfer period     - GDD\n");

	if (epc.q10depend_flag == 0)
		fprintf(bgcout->log_file.ptr, "q10 value           - constant\n");
	else
		fprintf(bgcout->log_file.ptr, "q10 value           - temperature dependent\n");

	if (epc.phtsyn_acclim_flag == 0)
		fprintf(bgcout->log_file.ptr, "photosyn. acclim.   - no\n");
	else
		fprintf(bgcout->log_file.ptr, "photosyn. acclim.   - yes\n");

	if (epc.resp_acclim_flag == 0)
		fprintf(bgcout->log_file.ptr, "respiration acclim. - no\n");
	else
		fprintf(bgcout->log_file.ptr, "respiration acclim. - yes\n");

	if (epc.CO2conduct_flag == 0)
		fprintf(bgcout->log_file.ptr, "CO2 conduct. effect - no effect\n");
	else
		fprintf(bgcout->log_file.ptr, "CO2 conduct. effect - reduction\n");

	fprintf(bgcout->log_file.ptr, " \n");

	fprintf(bgcout->log_file.ptr, "DATA SOURCES\n");
	if (epc.phenology_flag == 0) 
	{
		if (ctrl.varSGS_flag == 0) 
			fprintf(bgcout->log_file.ptr, "SGS data - user-defined from EPC file\n");
		else
		{
			fprintf(bgcout->log_file.ptr, "SGS data - user-defined from annual varying EGS file\n");
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
		fprintf(bgcout->log_file.ptr, "WPM data - constant \n");
	else
	{
		fprintf(bgcout->log_file.ptr, "WPM data - annual varying \n");
		if (ctrl.onscreen) printf("INFORMATION: reading mortality file: annual varying WPM data\n");
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

	if (ctrl.GWD_flag == 0) 
		fprintf(bgcout->log_file.ptr, "groundwater - NO\n");
	else
	{
		fprintf(bgcout->log_file.ptr, "groundwater - YES\n");
		if (ctrl.onscreen) printf("INFORMATION: reading groundwater_spinup.txt: annual varying GWD data\n");
	}

	fprintf(bgcout->log_file.ptr, " \n");

	fprintf(bgcout->log_file.ptr, "Critical VWC (m3/m3) and PSI (MPa) values of top soil layer \n");
	fprintf(bgcout->log_file.ptr, "saturation:                    %12.3f%12.4f\n",sprop.vwc_sat[0],sprop.psi_sat[0]);
	fprintf(bgcout->log_file.ptr, "field capacity:                %12.3f%12.4f\n",sprop.vwc_fc[0], sprop.psi_fc[0]);
	fprintf(bgcout->log_file.ptr, "wilting point:                 %12.3f%12.4f\n",sprop.vwc_wp[0], sprop.psi_wp[0]);
	fprintf(bgcout->log_file.ptr, "hygroscopic water:             %12.3f%12.4f\n",sprop.vwc_hw[0], sprop.psi_hw);
	fprintf(bgcout->log_file.ptr, "bulk density:                  %12.3f\n",sprop.BD[0]);
	fprintf(bgcout->log_file.ptr, "Clapp-Hornberger b parameter:  %12.3f\n",sprop.soil_b[0]);
	fprintf(bgcout->log_file.ptr, " \n");
	/********************************************************************************************************* */

	/* in case of natural ecosystem, nyears = number of meteorological year, in case agricultural system: nyears = number of plantings */
	if (!PLT.PLT_num)
		nyears = ctrl.simyears;
	else
		nyears = PLT.PLT_num + ctrl.simyears;
	
	/* allocate memory for local output arrays */
	if (!errflag && (ctrl.dodaily || ctrl.domonavg || ctrl.doannavg)) 
	{
		dayarr = (double*) malloc(ctrl.ndayout * sizeof(double));
		if (!dayarr)
		{
			printf("ERROR allocating for local daily output array in bgc()\n");
			errflag=301;
		}
		for (i=0 ; i<ctrl.ndayout ; i++) dayarr[i] = 0.0;
	}

	if (!errflag && ctrl.domonavg) 
	{
		monavgarr = (double*) malloc(ctrl.ndayout * sizeof(double));
		if (!monavgarr)
		{
			printf("ERROR allocating for monthly average output array in bgc()\n");
			errflag=302;
		}

		for (i=0 ; i<ctrl.ndayout ; i++) monavgarr[i] = 0.0;
	}
	if (!errflag && ctrl.doannavg) 
	{
		annavgarr = (double*) malloc(ctrl.ndayout * sizeof(double));
		if (!annavgarr)
		{
			printf("ERROR allocating for annual average output array in bgc()\n");
			errflag=303;
		}
		for (i=0 ; i<ctrl.ndayout ; i++) annavgarr[i] = 0.0;
	}
	if (!errflag && ctrl.doannual)
	{
		annarr = (double*) malloc(ctrl.nannout * sizeof(double));
		if (!annarr)
		{
			printf("ERROR allocating for local annual output array in bgc()\n");
			errflag=304;
		}
		for (i=0 ; i<ctrl.nannout ; i++) annarr[i] = 0.0;

	}
	

	/* allocate space for the output map pointers */
	if (!errflag) 
	{
		output_map = (double**) malloc(NMAP * sizeof(double*));
		if (!output_map)
		{
			printf("ERROR allocating for output map in output_map_init()\n");
			errflag=305;
		}
	}
	
	/* allocate space for the onday_arr and offday_arr: first column - year, second column: day*/
	if (!errflag)
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
			printf("ERROR allocating for onday_arr/offday_arr, prephenology()\n");
			errflag=306;
		}
	}

	
	
	/* initialize the output mapping array*/ 
	if (!errflag && output_map_init(output_map,&phen,&metv,&ws,&wf,&cs,&cf,&ns,&nf,&sprop,&epv,&psn_sun,&psn_shade,&summary))
	{
		printf("ERROR in call to output_map_init() from bgc()\n");
		errflag=401;
	}
	
#ifdef DEBUG
	printf("done initialize outmap\n");
#endif
	
	
	/* atmospheric pressure (Pa) as a function of elevation (m) */
	if (!errflag && atm_pres(sitec.elev, &metv.pa))
	{
		printf("ERROR in atm_pres() from bgc()\n");
		errflag=402;
	}
	
#ifdef DEBUG
	printf("done atm_pres\n");
#endif
		
     

	/* calculate GSI to deterime onday and offday 	*/
	if (ctrl.GSI_flag && !PLT.PLT_num)
	{
		if (!errflag && GSI_calculation(&metarr, &sitec, &epc, &phenarr, &ctrl))
		{
			printf("ERROR in call to GSI_calculation(), from bgc()\n");
			errflag=403;
		}
	}



#ifdef DEBUG
	printf("done GSI_calculation\n");
#endif

	/* calculate conductance limitation factors 	*/	
	if (!errflag && conduct_limit_factors(bgcout->log_file, &ctrl, &sprop, &epc, &epv))
	{
		printf("ERROR in call to conduct_limit_factors(), from bgc()\n");
		errflag=404;
	}

#ifdef DEBUG
	printf("done conduct_limit_factors\n");
#endif

	/********************************************************************************************************* */
	fprintf(bgcout->log_file.ptr, "---------------- \n");
	fprintf(bgcout->log_file.ptr, "spinup phase\n");
	fprintf(bgcout->log_file.ptr, "---------------- \n");
	/********************************************************************************************************* */

	/* determine phenological signals */		
	if (!errflag && prephenology(bgcout->log_file, &epc, &sitec, &metarr, &PLT, &HRV, &ctrl, &phenarr))
	{
		printf("ERROR in call to prephenology(), from bgc()\n");
		errflag=405;
	}
	

#ifdef DEBUG
	printf("done prephenology\n");
#endif
	
    /* calculate the annual average air temperature for use in soil temperature corrections */
	tair_annavg = 0.0;
	nmetdays = ctrl.simyears * NDAYS_OF_YEAR;
 	for (i=0 ; i<nmetdays ; i++)
	{
		tair_annavg += metarr.tavg[i];
	}
	tair_annavg /= (double)nmetdays;


	/* if this simulation is using a restart file for its initialconditions, then copy restart info into structures */
	if (!errflag && ctrl.read_restart)
	{
		if (!errflag && restart_input(&epc, &ws, &cs, &ns, &epv, &(bgcin->restart_input)))
		{
			printf("ERROR in call to restart_input() from bgc()\n");
			errflag=406;
		}
		 
#ifdef DEBUG
		printf("done restart_input\n");
#endif
	
	}
	
     /* initialize leaf C and N pools depending on phenology signals for the first metday */
	 if (!errflag && firstday(&ctrl, &sitec, &sprop,&epc, &cinit, &PLT, &phen, &epv, &cs, &ns, &metv,&psn_sun,&psn_shade))
	 {
		printf("ERROR in call to firstday(), from bgc()\n");
		errflag=407;
	 }
#ifdef DEBUG
		printf("done firstday\n");
#endif
		
	/* zero water, carbon, and nitrogen source and sink variables */
	if (!errflag && zero_srcsnk(&cs,&ns,&ws,&summary))
	{
		printf("ERROR in call to zero_srcsnk(), from bgc()\n");
		errflag=408;
	}

#ifdef DEBUG
	printf("done zero_srcsnk\n");
#endif


	
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

		for (nblockyear=0 ; !errflag && nblockyear<nblock ; nblockyear++)
		{
			
			/* set current month to 0 (january) at the beginning of each year */
			curmonth = 0;

			/* set the max lai, maturity and flowering variables, for annual diagnostic output */
			epv.annmax_lai = 0.0;
		
			/* atmospheric concentration of CO2 (ppm) */
			metv.co2 = co2.co2ppm;

			
		
			/* calculate scaling for N additions (decreasing with time since the beginning of metcycle = 0 block */	
			if ((!steady1 && rising && metcycle == 0))
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
			
			/* test whether simyr needs to be reset (for simulations with fewer than 50 metyears, metyr is not equal to simyr) */
			if (simyr == ctrl.simyears)
			{
				simyr = 0;
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

			for (yday=0 ; !errflag && yday<NDAYS_OF_YEAR ; yday++)
			{
#ifdef DEBUG
				printf("year %d\tyday %d\n",simyr,yday);
#endif

				
				/*  counters into control sturct */ 
				ctrl.yday = yday;

				/* initalizing annmax variables */
				if (yday == 0)
				{
					epv.annmax_leafc = 0;
					epv.annmax_frootc = 0;
					epv.annmax_fruitc = 0;
					epv.annmax_softstemc = 0;
					epv.annmax_livestemc = 0;
					epv.annmax_livecrootc = 0;
				}

				/* set fluxes to zero */
				if (!errflag && make_zero_flux_struct(&wf, &cf, &nf))
				{
					printf("ERROR in call to make_zero_flux_struct() from bgc()\n");
					errflag=501;
				}

#ifdef DEBUG
	printf("done make_zero_flux\n");
#endif

			
				/* set the day index for meteorological and phenological arrays */
				metday = simyr*NDAYS_OF_YEAR + yday;


                /* nitrogen deposition and fixation */
			    nf.ndep_to_sminn = ndep.ndep / NDAYS_OF_YEAR;
			    nf.nfix_to_sminn = epc.nfix / NDAYS_OF_YEAR;
		
				/* actual onday and offday */
				if (!errflag && dayphen(&ctrl, &epc, &phenarr, &PLT, &epv, &phen))
				{
					printf("ERROR in dayphen from bgc()\n");
					errflag=502;
				}

	            /* determining soil hydrological parameters  */
 				if (!errflag && multilayer_hydrolparams(&epc, &sitec, &sprop, &ws, &epv))
				{
					printf("ERROR in multilayer_hydrolparams() from bgc()\n");
					errflag=504;
				}

#ifdef DEBUG
				printf("%d\t%d\tdone multilayer_hydrolparams\n",simyr,yday);
#endif

				/* daily meteorological variables from metarrays */
				if (!errflag && daymet(&metarr, &epc, &sitec, &metv, &tair_annavg, ws.snoww, metday))
				{
					printf("ERROR in daymet() from bgc()\n");
					errflag=505;
				}
		

#ifdef DEBUG
				printf("%d\t%d\tdone daymet\n",simyr,yday);
#endif

				/* phenophases calculation */
				if (!errflag && phenphase(bgcout->log_file, &ctrl, &epc, &sprop, &PLT, &phen, &metv, &epv, &cs))
				{
					printf("ERROR in phenphase() from bgc()\n");
					errflag=506;
				}

#ifdef DEBUG
				printf("%d\t%d\tdone phenphase\n",simyr,yday);
#endif

				

				/* soil temperature calculations */
				if (!errflag && multilayer_tsoil(&epc, &sitec, &sprop, &epv, yday, ws.snoww, &metv))
				{
					printf("ERROR in multilayer_tsoil() from bgc()\n");
					errflag=507;
				}

#ifdef DEBUG
				printf("%d\t%d\tdone multilayer_tsoil\n",simyr,yday);
#endif
	

				/* phenology calculation */
				if (!errflag && phenology(&ctrl, &epc, &cs, &ns, &phen, &metv, &epv, &cf, &nf))
				{
					printf("ERROR in phenology() from bgc()\n");
					errflag=508;
				}

#ifdef DEBUG
				printf("%d\t%d\tdone phenology\n",simyr,yday);
#endif
		

				/* calculating rooting depth, n_rootlayers, n_maxrootlayers, rootlength_prop */
 				 if (!errflag && multilayer_rootdepth(&ctrl, &phen, &epc, &sitec, &sprop, &cs, &PLT, &epv))
				 {
					printf("ERROR in multilayer_rootdepth() from bgc()\n");
					errflag=509;
				 }

#ifdef DEBUG
				printf("%d\t%d\tdone multilayer_rootdepth\n",simyr,yday);
			
#endif

		
				/* calculate leaf area index, sun and shade fractions, and specific leaf area for sun and shade canopy fractions, then calculate canopy radiation interception and transmission */
				if (!errflag && radtrans(&ctrl, &phen, &cs, &epc, &sitec, &metv, &epv))
				{
					printf("ERROR in radtrans() from bgc()\n");
					errflag=510;
				}

				/* update the ann max LAI for annual diagnostic output */
				if (epv.proj_lai > epv.annmax_lai) epv.annmax_lai = epv.proj_lai;

#ifdef DEBUG
				printf("%d\t%d\tdone radtrans\n",simyr,yday);
#endif

				/* precip routing (when there is precip) */
				if (!errflag && metv.prcp && prcp_route(&metv, epc.int_coef, epv.all_lai,	&wf))
				{
					printf("ERROR in prcp_route() from bgc()\n");
					errflag=511;
				}

#ifdef DEBUG
				printf("%d\t%d\tdone prcp_route\n",simyr,yday);
#endif

				/* snowmelt (when there is a snowpack) */
				if (!errflag && ws.snoww && snowmelt(&metv, &wf, ws.snoww))
				{
					printf("ERROR in snowmelt() from bgc()\n");
					errflag=512;
				}

#ifdef DEBUG
				printf("%d\t%d\tdone snowmelt\n",simyr,yday);
#endif


					
				/* bare-soil evaporation  */
				if (!errflag && baresoil_evap(&sprop, &metv, &wf, &epv.dsr))
				{
					printf("ERROR in baresoil_evap() from bgc()\n");
					errflag=513;
				}

#ifdef DEBUG
				printf("%d\t%d\tdone bare_soil evap\n",simyr,yday);
#endif
	

				/* begin canopy bio-physical process simulation */
				/* do canopy ET calculations whenever there is leaf areadisplayed, since there may be intercepted water on the canopy that needs to be dealt with */
				if (!errflag && epv.n_actphen > epc.n_emerg_phenophase && metv.dayl)
				{

					/* conductance calculation */
					if (!errflag && conduct_calc(&ctrl, &metv, &epc, &sitec, &epv, simyr))
					{
						printf("ERROR in conduct_calc() from bgc()\n");
						errflag=514;
					}
			
#ifdef DEBUG
			printf("%d\t%d\tdone conduct_calc\n",simyr,yday);
#endif

					/* evapo-transpiration */
					if (!errflag && cs.leafc && canopy_et(&epc, &metv, &sprop, &epv, &wf))
					{
						printf("ERROR in canopy_et() from bgc()\n");
						errflag=515;
					}

#ifdef DEBUG
					printf("%d\t%d\tdone canopy_et\n",simyr,yday);
#endif
				}


				/* daily maintenance respiration */
				if (!errflag && maint_resp(&cs, &ns, &epc, &metv, &epv, &cf))
				{
					printf("ERROR in m_resp() from bgc()\n");
					errflag=516;
				}

#ifdef DEBUG
				printf("%d\t%d\tdone maint resp\n",simyr,yday);
#endif


				/* do photosynthesis only when it is part of the current growth season, as defined by the remdays_curgrowth flag */
				if (!errflag && cs.leafc && photosynthesis(&epc, &metv, &cs, &ws, &phen, &epv, &psn_sun, &psn_shade, &cf))
				{
					printf("ERROR in photosynthesis() from bgc()\n");
					errflag=517;
				}
				
#ifdef DEBUG
				printf("%d\t%d\tdone photosynthesis\n",simyr,yday);
#endif
		

				/* daily litter and soil decomp and nitrogen fluxes */
				if (!errflag && decomp(&metv,&epc,&sprop,&sitec,&cs,&ns,&epv,&cf,&nf,&nt))
				{
					printf("ERROR in decomp() from bgc.c\n");
					errflag=518;
				}

#ifdef DEBUG
			printf("%d\t%d\tdone decomp\n",simyr,yday);
#endif
		
	
				/* Daily allocation gets called whether or not this is a current growth day, because the competition between decomp
				immobilization fluxes and plant growth N demand is resolved here.  
				On days with no growth, no allocation occurs, but immobilization fluxes are updated normally */
				/* spinup control: in the rising limb, use the spinup allocation code that supplements N supply */
				if ((!steady1 && rising && metcycle == 0 && nadd_flag == 0))
				{
					if (!errflag && daily_allocation(&epc,&sprop,&metv,&cs,&ns,&cf,&nf,&epv,&nt,naddfrac))
					{
						printf("ERROR in daily_allocation() from bgc.c\n");
						errflag=519;
					}

				}
				else
				{
					if (!errflag && daily_allocation(&epc,&sprop,&metv,&cs,&ns,&cf,&nf,&epv,&nt,0))
					{
						printf("ERROR in daily_allocation() from bgc.c\n");
						errflag=519;
					}
					nadd_flag = 1;
				}


#ifdef DEBUG
			printf("%d\t%d\tdone daily_allocation\n",simyr,yday);
#endif
            
				/* heat stress during flowering can affect daily allocation of fruit */
				if (epc.n_flowHS_phenophase > 0)
				{
					if (!errflag && flowering_heatstress(&epc, &metv, &epv, &cf, &nf))
					{
						printf("ERROR in flowering_heatstress() from bgc()\n");
						errflag=520;
					}
				}

				/* reassess the annual turnover rates for livewood --> deadwood, and for evergreen leaf and fine root litterfall. 
				This happens once each year, on the annual_alloc day (the last litterfall day - test for annual allocation day) */
			
				if (phen.remdays_litfall == 1) 
					annual_alloc = 1; 
				else 
					annual_alloc = 0;

				if (!errflag && annual_alloc)
				{
					if (!errflag && annual_rates(&epc,&epv))
					{
						printf("ERROR in annual_rates() from bgc()\n");
						errflag=521;
					}

#ifdef DEBUG
				printf("%d\t%d\tdone annual rates\n",simyr,yday);
#endif
				} 


				/* daily growth respiration */
				if (!errflag && growth_resp(&epc, &cf))
				{
					printf("ERROR in daily_growth_resp() from bgc.c\n");
					errflag=522;
				}

#ifdef DEBUG
			printf("%d\t%d\tdone growth_resp\n",simyr,yday);
#endif


				/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
				/* 3. WATER CALCULATIONS WITH STATE UPDATE */

				/* calculate the part-transpiration from total transpiration */
				if (!errflag && multilayer_transpiration(&ctrl, &sitec, &epv, &ws, &wf))
				{
					printf("ERROR in multilayer_transpiration() from bgc()\n");
					errflag=523;
				}
#ifdef DEBUG
			printf("%d\t%d\tdone multilayer_transpiration\n",simyr,yday);
#endif	
			
		
				/* ground water calculation */
				if (!errflag && groundwater(&sitec, &sprop, &ctrl, &epv, &ws, &wf))
				{
					printf("ERROR in groundwater() from bgc()\n");
					errflag=525;
				}
#ifdef DEBUG
			printf("%d\t%d\tdone groundwater\n",simyr,yday);
#endif	

	
				/* multilayer soil hydrology: percolation calculation based on PRCP, RUNOFF, EVAP, TRANS */
     			if (!errflag && multilayer_hydrolprocess(&ctrl, &sitec, &sprop, &epc, &epv, &ws, &wf))
				{ 
					printf("ERROR in multilayer_hydrolprocess() from bgc()\n");
					errflag=526;
				}

#ifdef DEBUG
		printf("%d\t%d\tdone multilayer_hydrolprocess\n",simyr,yday);
#endif
			
		
				/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
				/* 4. STATE UPDATE */

				/* daily update of the water state variables */
				if (!errflag && daily_water_state_update(&wf, &ws))
				{
					printf("ERROR in daily_water_state_update() from bgc()\n");
					errflag=527;
				}

#ifdef DEBUG
			printf("%d\t%d\tdone water state update\n",simyr,yday);
#endif

			
				/* daily update of carbon and nitrogen state variables */
				if (!errflag && daily_CN_state_update(&epc, &ctrl, &epv, &cf, &nf, &cs, &ns, annual_alloc, epc.woody, epc.evergreen))
				{
					printf("ERROR in daily_CN_state_update() from bgc()\n");
					errflag=528;
				}

#ifdef DEBUG
			printf("%d\t%d\tdone CN state update\n",simyr,yday);
#endif
		

				/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
				/* 5. MORTALITY AND NITROGEN FLUXES CALCULATION WITH OWN STATE UPDATE: 
				to insure that pools don't go negative due to mortality/leaching fluxes conflicting with other proportional fluxes */

				/* calculate daily senescence mortality fluxes and update state variables */
				if (!errflag && senescence(yday, &epc, &GRZ, &metv, &phen, &ctrl, &cs, &cf, &ns, &nf, &epv))
				{
					printf("ERROR in senescence() from bgc()\n");
					errflag=529;
				}
			
#ifdef DEBUG
			printf("%d\t%d\tdone senescence\n",simyr,yday);
#endif
		        
				/* calculate daily mortality fluxes  and update state variables */
				if (!errflag && mortality(&ctrl, &epc, &epv, &phen,&cs, &cf, &ns, &nf, simyr))
				{
					printf("ERROR in mortality() from bgc()\n");
					errflag=530;
				}
			
#ifdef DEBUG
			printf("%d\t%d\tdone mortality\n",simyr,yday);
#endif
		
		
				/* calculate the change of soil mineralized N in multilayer soil */ 
				if (!errflag && multilayer_sminn(&epc, &sprop, &epv, &sitec, &cf, &ns, &nf))
				{
					printf("ERROR in multilayer_sminn() from bgc()\n");
					errflag=531;
				}
			
#ifdef DEBUG
			printf("%d\t%d\tdone multilayer_sminn\n",simyr,yday);
#endif

				/* calculate the leaching of N, DOC and DON from multilayer soil */
				if (!errflag && multilayer_leaching(&epc, &sprop, &epv, &ctrl, &cs, &cf, &ns, &nf, &ws, &wf))
				{
					printf("ERROR in multilayer_leaching() from bgc()\n");
					errflag=532;
				}

	
#ifdef DEBUG
			printf("%d\t%d\tdone multilayer_leaching\n",simyr,yday);
#endif	
				/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
				/* 7. ERROR CHECKING AND SUMMARY VARIABLES  */
			
				/* test for very low state variable values and force them to 0.0 to avoid rounding and floating point overflow errors */
				if (!errflag && precision_control(&ws, &cs, &ns))
				{
					printf("ERROR in call to precision_control() from bgc()\n");
					errflag=541;
				} 
			
#ifdef DEBUG
			printf("%d\t%d\tdone precision_control\n",simyr,yday);
#endif


				/* test for water balance */
				if (!errflag && check_water_balance(&ws, first_balance))
				{
					printf("ERROR in check_water_balance() from bgc()\n");
					errflag=542;
				}

#ifdef DEBUG
			printf("%d\t%d\tdone water balance\n",simyr,yday);
#endif

				/* test for carbon balance */
				if (!errflag && check_carbon_balance(&cs, first_balance))
				{
					printf("ERROR in check_carbon_balance() from bgc()\n");
					errflag=543;
				}

#ifdef DEBUG
			printf("%d\t%d\tdone carbon balance\n",simyr,yday);
#endif
			

				/* test for nitrogen balance  */
				if (!errflag && check_nitrogen_balance(&ns, first_balance))
				{
					printf("ERROR in check_nitrogen_balance() from bgc()\n");
					errflag=544;
				}

#ifdef DEBUG
			printf("%d\t%d\tdone nitrogen balance\n",simyr,yday);
#endif
			

			
				/* calculate summary variables */
				if (!errflag && cnw_summary(yday, &epc, &sitec, &sprop, &metv, &cs, &cf, &ns, &nf, &wf, &epv, &summary))
				{
					printf("ERROR in cnw_summary() from bgc()\n");
					errflag=545;
				} 
#ifdef DEBUG
			printf("%d\t%d\tdone carbon summary\n",simyr,yday);
#endif
			
				/* output handling */
				if (!errflag && output_handling(mondays[curmonth], endday[curmonth], &ctrl, output_map, dayarr, monavgarr, annavgarr, annarr, 
					bgcout->dayout, bgcout->monavgout, bgcout->annavgout, bgcout->annout))
				{
					printf("ERROR in output_handling() from bgc()\n");
					errflag=546;
				}
	
	
#ifdef DEBUG
			printf("%d\t%d\tdoneoutput_handling\n",simyr,yday); 
#endif
			/*  if no dormant period (e.g. evergreen): last day is the dormant day */
			if (phen.offday - phen.onday == 364 && phen.offday == phen.yday_total) 
			{
				epv.n_actphen = 0;
				phen.onday = -1;
				phen.offday = -1;
				phen.remdays_litfall =-1;
			}
				/* if this is the last day of the current month: increment current month counter */
				if (yday == endday[curmonth]) curmonth++;	
				
				/* spinup control */
				/* keep a tally of total soil C during successive
				met cycles for comparison */
				if (metcycle == 1)
				{
					tally1 += summary.soilc;
					tally1b += summary.totalc;
				}
				if (metcycle == 2)
				{
					tally2 += summary.soilc;
					tally2b += summary.totalc;
				}

				

				/* at the end of first day of simulation, turn off the 
				first_balance switch */
				if (first_balance) first_balance = 0;

		}   /* end of daily model loop */

		simyr++;
		/* spinup control */
		spinyears++;
			
		}   /* end of annual model loop */
		
		/* spinup control */
		/* if this is the third pass through metcycle, do comparison */
		/* first block is during the rising phase */
		if (!steady1 && metcycle == 2)
		{
			/* convert tally1 and tally2 to average daily soilc */
			tally1 /= (double)nblock * NDAYS_OF_YEAR;
			tally2 /= (double)nblock * NDAYS_OF_YEAR;
			rising = (tally2 > tally1);
			t1 = (tally2-tally1)/(double)nblock;
			steady1 = (fabs(t1) < SPINUP_TOLERANCE);
			
#ifdef DEBUG_SPINUP
			printf("spinyears = %d rising = %d steady1 = %d\n",spinyears,
				rising,steady1);
			printf("metcycle = %d tally1 = %lf tally2 = %lf pdif = %lf\n\n",
				metcycle,tally1,tally2,t1);
			if (steady1) printf("SWITCH\n\n");
#endif
				
			metcycle = 0;
	
		}
		/* second block is after supplemental N turned off */
		else if (steady1 && metcycle == 2)
		{
			/* convert tally1 and tally2 to average daily soilc */
			tally1 /= (double)nblock * NDAYS_OF_YEAR;
			tally2 /= (double)nblock * NDAYS_OF_YEAR;
			t1 = (tally2-tally1)/(double)nblock;
			steady2 = (fabs(t1) < SPINUP_TOLERANCE);

			/* if rising above critical rate, back to steady1=0 */
			if (t1 > SPINUP_TOLERANCE)
			{
#ifdef DEBUG_SPINUP
				printf("\nSWITCH BACK\n");
#endif
				
				steady1 = 0;
				rising = 1;
			}
			
#ifdef DEBUG_SPINUP
			printf("spinyears = %d rising = %d steady2 = %d\n",spinyears,
				rising,steady2);
			printf("metcycle = %d tally1 = %lf tally2 = %lf pdif = %lf\n\n",
				metcycle,tally1,tally2,t1);
#endif
				
			metcycle = 0;
		}
			
		else
		{
#ifdef DEBUG_SPINUP
			printf("spinyears = %d rising = %d steady1 = %d\n",spinyears,
				rising,steady1);
			printf("metcycle = %d tally1 = %lf tally2 = %lf pdif = %lf\n",
				metcycle,tally1,tally2,t1);

		
#endif
		
			metcycle++;
		}

	
	/* end of do block, test for steady state */	
	} while (!(steady1 && steady2) && (spinyears < ctrl.maxspinyears || 
		metcycle != 0));
	

	/********************************************************************************************************* */
	/* writing log file */

	if (cs.CbalanceERR != 0) CbalanceERR = log10(cs.CbalanceERR);
	if (ns.NbalanceERR != 0) NbalanceERR = log10(ns.NbalanceERR);
	if (ws.WbalanceERR != 0) WbalanceERR = log10(ws.WbalanceERR);
	if (ctrl.CNerror  != 0) CNerror     = log10(ctrl.CNerror);

	fprintf(bgcout->log_file.ptr, "SOME IMPORTANT ANNUAL OUTPUTS FROM LAST SIMULATION YEAR\n");
	fprintf(bgcout->log_file.ptr, "Cumulative sum of  GPP [gC/m2/year]:                     %12.1f\n",summary.cum_gpp*1000);
	fprintf(bgcout->log_file.ptr, "Cumulative sum of  NEE [gC/m2/year):                     %12.1f\n",summary.cum_nee*1000);
	fprintf(bgcout->log_file.ptr, "Cumulative sum of  ET  [kgH2O/m2/year):                  %12.1f\n",summary.cum_ET);
	fprintf(bgcout->log_file.ptr, "Cumulative sum of N2O flux [gN/m2/year):                 %12.1f\n",summary.cum_n2o*1000);
	fprintf(bgcout->log_file.ptr, "Maximum projected LAI [m2/m2):                           %12.2f\n",epv.annmax_lai);
    fprintf(bgcout->log_file.ptr, "Humus carbon content (in 0-30 cm soil layer) [%%]:        %12.2f\n",summary.humusC_top30);
	fprintf(bgcout->log_file.ptr, "SOM carbon content (in 0-30 cm soil layer) [%%]:          %12.2f\n",summary.SOM_C_top30);
	fprintf(bgcout->log_file.ptr, "SOM nitrogen content (in 0-30 cm soil layer) [%%]:        %12.2f\n",summary.SOM_N_top30);
	fprintf(bgcout->log_file.ptr, "Available soil ammonium content (0-30 cm) [mgN/kg soil]: %12.1f\n",summary.NH4_top30);
	fprintf(bgcout->log_file.ptr, "Available soil nitrate content (0-30 cm) [mgN/kg soil]:  %12.1f\n",summary.NO3_top30);
	fprintf(bgcout->log_file.ptr, "VWC of rootzone on last simulation day [m3/m3]:          %12.3f\n",epv.vwc_RZ);
	fprintf(bgcout->log_file.ptr, " \n");
 	fprintf(bgcout->log_file.ptr, "Total N-plus (spinup daily allocation) (kgN):	        %12.2f\n",ns.SPINUPsrc);
	fprintf(bgcout->log_file.ptr, " \n");
	fprintf(bgcout->log_file.ptr, "10-base logarithm of the maximum carbon balance diff.:   %12.1f\n",CbalanceERR);
	fprintf(bgcout->log_file.ptr, "10-base logarithm of the maximum nitrogen balance diff.: %12.1f\n",NbalanceERR);
	fprintf(bgcout->log_file.ptr, "10-base logarithm of the maximum water balance diff.:    %12.1f\n",WbalanceERR);
	fprintf(bgcout->log_file.ptr, "10-base logarithm of the C-N calc. numbering error:      %12.1f\n",CNerror);
	fprintf(bgcout->log_file.ptr, " \n");
		fprintf(bgcout->log_file.ptr, "WARNINGS \n");
	if (!ctrl.limittransp_flag && !ctrl.limitevap_flag && !ctrl.limitleach_flag && !ctrl.limitleach_flag && !ctrl.limitdiffus_flag &&
		!ctrl.limitSNSC_flag && !ctrl.limitMR_flag && !ctrl.notransp_flag && !ctrl.noMR_flag && !ctrl.pond_flag && 
		!ctrl.condMOWerr_flag && !ctrl.condIRGerr_flag && !ctrl.condIRGerr_flag && !ctrl.prephen1_flag && !ctrl.prephen2_flag && 
		!ctrl.bareground_flag && !ctrl.vegper_flag)
	{
		fprintf(bgcout->log_file.ptr, "no WARNINGS\n");
	}
	else
	{
		if (ctrl.limittransp_flag)
		{
			fprintf(bgcout->log_file.ptr, "Limited transpiration due to dry soil\n");
			ctrl.limittransp_flag = -1;
		}

		if (ctrl.limitevap_flag)
		{
			fprintf(bgcout->log_file.ptr, "Limited evaporation due to dry soil\n");
			ctrl.limitevap_flag = -1;
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

		if (ctrl.notransp_flag)
		{
			fprintf(bgcout->log_file.ptr, "No transpiration (full limitation) due to dry soil\n");
			ctrl.notransp_flag = -1;
		}

		if (ctrl.noMR_flag)
		{
			fprintf(bgcout->log_file.ptr, "No woody maintanance respiration  (full limitation)\n");
			ctrl.noMR_flag = -1;
		}

		if (ctrl.pond_flag)
		{
			fprintf(bgcout->log_file.ptr, "Pond water on soil surface\n");
			ctrl.notransp_flag = -1;
		}

				if (ctrl.condMOWerr_flag)
		{
			fprintf(bgcout->log_file.ptr, "If conditional MOWING flag is on, no MOWING is possible\n");
			ctrl.condMOWerr_flag = -1;
		}

		if (ctrl.condIRGerr_flag)
		{
			fprintf(bgcout->log_file.ptr, "If conditional IRRIGATING flag is on, no IRRIGATING is possible\n");
			ctrl.condIRGerr_flag = -1;
		}

		if (ctrl.condIRGerr_flag)
		{
			fprintf(bgcout->log_file.ptr, "If conditional IRRIGATING flag is on, no IRRIGATING is possible\n");
			ctrl.condIRGerr_flag = -1;
		}

		if (ctrl.prephen1_flag)
		{
			fprintf(bgcout->log_file.ptr, "In case of planting model-defined phenology is not possible (firstday:planting, lastday:harvesting)\n");
			ctrl.prephen1_flag = -1;
		}

		if (ctrl.prephen2_flag)
		{
			fprintf(bgcout->log_file.ptr, "In case of user-defined phenology GSI calculation is not possible\n");
			ctrl.prephen2_flag = -1;
		}

		if (ctrl.bareground_flag)
		{
			fprintf(bgcout->log_file.ptr, "User-defined bare-ground run (onday and offday set to -1 in EPC)\n");
			ctrl.bareground_flag = -1;
		}

		if (ctrl.vegper_flag)
		{
			fprintf(bgcout->log_file.ptr, "Vegetation period has not ended until the last day of year, the offday is equal to the last day of year\n");
			ctrl.vegper_flag = -1;
		}
	}

	fprintf(bgcout->log_file.ptr, " \n");
	fprintf(bgcout->log_file.ptr,"spinyears = %d \n",spinyears);
	fprintf(bgcout->log_file.ptr, " \n");
	
	/********************************************************************************************************* */

	/* save some information on the end status of spinup */
	tally1b /= (double)nblock * NDAYS_OF_YEAR;
	tally2b /= (double)nblock * NDAYS_OF_YEAR;
	bgcout->spinup_resid_trend = (tally2b-tally1b)/(double)nblock;
	bgcout->spinup_years = spinyears;
	
	if (ctrl.onscreen) printf("\n");
	if (ctrl.onscreen) printf("SPINUP: residual trend   = %.6lf\n",bgcout->spinup_resid_trend);
 	if (ctrl.onscreen) printf("SPINUP: number of years  = %d\n",bgcout->spinup_years);

	/********************************************************************************************************* */

	/* 8. RESTART OUTPUT HANDLING */
	/* if write_restart flag is set, copy data to the output restart struct */
	if (!errflag && ctrl.write_restart)
	{
		if (restart_output(&ws, &cs, &ns, &epv, &(bgcout->restart_output)))
		{
			printf("ERROR in call to restart_output() from bgc()\n");
			errflag=600;
		}
		
#ifdef DEBUG
		printf("%d\t%d\tdone restart output\n",simyr,yday);
#endif
	}



	
	/* free memory for local output arrays */
	if (((errflag == 0 || errflag > 403) && ctrl.GSI_flag) || ((errflag == 0 || errflag > 405) && !ctrl.GSI_flag)) 
	{
		free(phenarr.onday_arr);
		free(phenarr.offday_arr);
	}
    if ((errflag == 0 || errflag > 301) && ctrl.dodaily) free(dayarr);
	if ((errflag == 0 || errflag > 302) && ctrl.domonavg) free(monavgarr);
	if ((errflag == 0 || errflag > 303) && ctrl.doannavg) free(annavgarr);
	if ((errflag == 0 || errflag > 304) && ctrl.doannual) free(annarr); 
	if ((errflag == 0 || errflag > 305)) free(output_map);
		
	/* print timing info if error */
	if (errflag)
	{
		printf("ERROR at year %d\n",ctrl.spinyears);
		printf("ERROR at yday %d\n",yday);
	}
	

	/*----------------------------------------------------------*/
	/* TRANSIENT RUN between spinup and normal run  */
  	if (!errflag && (co2.varco2 || ndep.varndep))
	{
	
		bgcin->ws = ws;
		bgcin->cs = cs;
		bgcin->ns = ns;
		
		bgcin->cinit.max_leafc      = epv.annmax_leafc;
		bgcin->cinit.max_frootc     = epv.annmax_frootc;
		bgcin->cinit.max_fruitc     = epv.annmax_fruitc;
		bgcin->cinit.max_softstemc  = epv.annmax_softstemc;
		bgcin->cinit.max_livestemc  = epv.annmax_livestemc;
		bgcin->cinit.max_livecrootc = epv.annmax_livecrootc;


		if (ctrl.onscreen) 
		{
			printf("-----------------------------------------\n");
			printf("Start of transient run.\n");
		}

		errflag = transient_bgc(bgcin, bgcout);
		if (errflag)
		{
			printf("ERROR in call to transient_bgc()\n");
		}

		ws = bgcin->ws;
		cs = bgcin->cs;
		ns = bgcin->ns;
	}


	/* return error status */	
	return (errflag);
 }
