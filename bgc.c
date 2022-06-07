/*
bgc.c
Core BGC model logic

Includes in-line output handling routines that write to daily and annual
output files. This is the only library module that has external
I/O connections, and so it is the only module that includes bgc_io.h.

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Biome-BGC version 4.1.1
Copyright 2000, Peter E. Thornton
Numerical Terradynamics Simulation Group (NTSG)
School of Forestry, University of Montana
Missoula, MT 59812
*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Modified:
4/17/2000 (PET): Added annual average temperature and annual total precipitation
to the simple annual text output file.
Modified:
13/07/2000: Added input of Ndep from file. Changes are made by Galina Churkina. 
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <malloc.h>
#include <time.h>

#include "ini.h"    
#include "bgc_struct.h"     
#include "bgc_constants.h"
#include "bgc_func.h"
#include "pointbgc_struct.h"   /* data structures for point driver */
#include "bgc_io.h"
#include "pointbgc_func.h"
//#define DEBUG



int bgc(bgcin_struct* bgcin, bgcout_struct* bgcout)
{
	
	/* variable declarations */
	int ok=1;
    

	/* iofiles and program control variables */
	control_struct     ctrl;

	/* meteorological variables */
	metarr_struct      metarr;
	metvar_struct      metv;
	co2control_struct  co2;
	ramp_ndep_struct ramp_ndep;

	
	/* state and flux variables for water, carbon, and nitrogen */
	wstate_struct      ws;
	wflux_struct       wf, zero_wf;
	cinit_struct       cinit;
	cstate_struct      cs;
	cflux_struct       cf, zero_cf;
	nstate_struct      ns;
	nflux_struct       nf, zero_nf;


	/* primary ecophysiological variables */
	epvar_struct       epv;

	/* planting variables - by Hidy 2008*/
	planting_struct       PLT;

	/* thinning variables - by Hidy 2008*/
	thinning_struct     THN;

	/* mowing variables - by Hidy 2008*/
	mowing_struct       MOW;

	/* grazing variables - by Hidy 2009*/
	grazing_struct       GRZ;

	/* harvesting variables - by Hidy 2008*/
	harvesting_struct       HRV;

	/* ploughing variables - by Hidy 2012*/
	ploughing_struct      PLG;

	/* fertilizing variables - by Hidy 2008*/
	fertilizing_struct       FRZ;

	/* GSI variables - by Hidy 2009. */
	GSI_struct      GSI;

	/* site physical constants */
	siteconst_struct   sitec;

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
	
	/* output mapping (array of pointers to double) */
	double **output_map;
	
	/* local storage for daily and annual output variables */
	float *dayarr, *monavgarr, *annavgarr, *annarr;

	

	/* miscelaneous variables for program control in main */
	int simyr, yday, metyr, metday;
	int first_balance;
	int annual_alloc;
	int outv;
	int dayout;
	double daily_ndep; 
	double tair_annavg = 0;
	double nmetdays;
	int i;

	/* variables used for monthly average output */
	int curmonth;
	int mondays[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
	int endday[12] = {30,58,89,119,150,180,211,242,272,303,333,364};
	
	/* simple annual variables for text output  - Hidy 2008. */
	double annmaxlai,annet,anndeeppercol,annnpp, annnee, annnbp, annprcp,anntavg, ann_carbonchange_THN,  ann_carbonchange_MOW,  ann_carbonchange_HRV,
		ann_carbonchange_PLG, ann_carbonchange_GRZ, ann_carbonchange_FRZ, ann_carbonchange_PLT, ann_nitrogen_GRZplus, ann_nitrogen_FRZplus;
	

	/* copy the input structures into local structures */
	ws = bgcin->ws;
	cinit = bgcin->cinit;
	cs = bgcin->cs;
	ns = bgcin->ns;
	sitec = bgcin->sitec;
	epc = bgcin->epc;
	PLT = bgcin->PLT; 		/* planting variables - Hidy 2008.*/
	THN = bgcin->THN; 		/* thinning variables - Hidy 2008.*/
	MOW = bgcin->MOW; 		/* mowing variables - Hidy 2008.*/
	GRZ = bgcin->GRZ; 		/* grazing variables - Hidy 2008.*/
	HRV = bgcin->HRV;		/* harvesting variables - Hidy 2008.*/
	PLG = bgcin->PLG;		/* harvesting variables - Hidy 2008.*/
	FRZ = bgcin->FRZ;		/* fertilizing variables - Hidy 2008.*/
	GSI = bgcin->GSI;		/* GSI variables - Hidy 2009. */

	/* note that the following three structures have dynamic memory elements,
	and so the notion of copying the input structure to a local structure
	value-by-value is not the same as above. In this case, the array pointers
	are being copied, so the local members use the same memory that was
	allocated in the calling function. Note also that bgc() does not modify
	the contents of these structures. */
	ctrl = bgcin->ctrl;
	metarr = bgcin->metarr;
	co2 = bgcin->co2;
	ramp_ndep = bgcin->ramp_ndep;
	

#ifdef DEBUG
	printf("done copy input\n");
#endif

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (ctrl.onscreen)
	{
		if (ctrl.GSI_flag)
		{
			file_open (&GSI.GSI_file, 'w');				/* file of GSI parameters - Hidy 2009.*/
		}
	
		file_open (&bgcout->control_file, 'w');		/* file of BBGC variables to control the simulation - Hidy 2009.*/
		fprintf(bgcout->control_file.ptr, "yday VWC1 VWC3 w_scalar cum_NPP SNSC_src litr1c soil4c(kg) soil4n(kg) GPP AR HR EVAP \n");

	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	
	/* local variable that signals the need for daily output array */
	dayout = (ctrl.dodaily || ctrl.domonavg || ctrl.doannavg);
	
	/* allocate memory for local output arrays */
	if (ok && dayout &&	!(dayarr = (float*) malloc(ctrl.ndayout * sizeof(float))))
	{
		printf("Error allocating for local daily output array in bgc()\n");
		ok=0;
	}
	if (ok && ctrl.domonavg && !(monavgarr = (float*) malloc(ctrl.ndayout * sizeof(float))))
	{
		printf("Error allocating for monthly average output array in bgc()\n");
		ok=0;
	}
	if (ok && ctrl.doannavg && !(annavgarr = (float*) malloc(ctrl.ndayout * sizeof(float))))
	{
		printf("Error allocating for annual average output array in bgc()\n");
		ok=0;
	}
	if (ok && ctrl.doannual && !(annarr = (float*) malloc(ctrl.nannout * sizeof(float))))
	{
		printf("Error allocating for local annual output array in bgc()\n");
		ok=0;
	}
	/* allocate space for the output map pointers */
	if (ok && !(output_map = (double**) malloc(NMAP * sizeof(double*))))
	{
		printf("Error allocating for output map in output_map_init()\n");
		ok=0;
	}
	
#ifdef DEBUG
	printf("done allocate out arrays\n");
#endif
	
	/* initialize monavg and annavg to 0.0 */
	if (ctrl.domonavg)
	{
		for (outv=0 ; outv<ctrl.ndayout ; outv++)
		{
			monavgarr[outv] = 0.0;
		}
	}
	if (ctrl.doannavg)
	{
		for (outv=0 ; outv<ctrl.ndayout ; outv++)
		{
			annavgarr[outv] = 0.0;
		}
	}
	
	/* initialize the output mapping array */
	if (ok && output_map_init(output_map,&metv,&ws,&wf,&cs,&cf,&ns,&nf,&phen,
		&epv,&psn_sun,&psn_shade,&summary))
	{
		printf("Error in call to output_map_init() from bgc()\n");
		ok=0;
	}
	
#ifdef DEBUG
	printf("done initialize outmap\n");
#endif
	
	/* make zero-flux structures for use inside annual and daily loops */
	if (ok && make_zero_flux_struct(&zero_wf, &zero_cf, &zero_nf))
	{
		printf("Error in call to make_zero_flux_struct() from bgc()\n");
		ok=0;
	}

#ifdef DEBUG
	printf("done make_zero_flux\n");
#endif
	
	/* atmospheric pressure (Pa) as a function of elevation (m) */
	if (ok && atm_pres(sitec.elev, &metv.pa))
	{
		printf("Error in atm_pres() from bgc()\n");
		ok=0;
	}
	
#ifdef DEBUG
	printf("done atm_pres\n");
#endif


	/* Hidy 2012. - calculate conductance limitation factors 	*/	
	if (ok && conduct_limit_factors(&sitec, &epc, &epv))
	{
		printf("Error in call to conduct_limit_factors(), from bgc()\n");
		ok=0;
	}

#ifdef DEBUG
	printf("done conduct_limit_factors\n");
#endif

	if (ctrl.GSI_flag)
	{
		/* Hidy 2009. - calculate GSI to deterime onday and offday 	*/	
		if (ok && GSI_calculation(&metarr, &ctrl, &GSI, &phenarr))
		{
			printf("Error in call to GSI_calculation(), from bgc()\n");
			ok=0;
		}

		#ifdef DEBUG
			printf("done GSI_calculation\n");
		#endif
	}



	/* determine phenological signals */
	if (ok && prephenology(&ctrl, &epc, &sitec, &metarr, &phenarr))
	{
		printf("Error in call to prephenology(), from bgc()\n");
		ok=0;
	}
	
#ifdef DEBUG
	printf("done prephenology\n");
#endif
	
	/* calculate the annual average air temperature for use in soil 
	temperature corrections. This code added 9 February 1999, in
	conjunction with soil temperature testing done with Mike White. 
	tair_annavg = 0.0; */
	nmetdays = ctrl.metyears * 365;
	for (i=0 ; i<nmetdays ; i++)
	{
		tair_annavg += metarr.tavg[i];
	}
	tair_annavg /= (double)nmetdays;

	
	/* if this simulation is using a restart file for its initial
	conditions, then copy restart info into structures */
	if (ok && ctrl.read_restart)
	{
		if (ok && restart_input(&sitec, &ctrl, &ws, &cs, &ns, &epv, &metyr,
			&(bgcin->restart_input)))
		{
			printf("Error in call to restart_input() from bgc()\n");
			ok=0;
		}
		
#ifdef DEBUG
		printf("done restart_input\n");
#endif
	
	}
	else
	/* no restart file, user supplies initial conditions */
	{
		/* initialize leaf C and N pools depending on phenology signals for
		the first metday */
		if (ok && firstday(&sitec, &epc, &cinit, &epv, &phenarr, &cs, &ns, &metv))
		{
			printf("Error in call to firstday(), from bgc()\n");
			ok=0;
		}
		
		/* initial value for metyr */
		metyr = 0;
		
#ifdef DEBUG
		printf("done firstday\n");
#endif
	}

	/* zero water, carbon, and nitrogen source and sink variables */
	if (ok && zero_srcsnk(&cs,&ns,&ws,&summary))
	{
		printf("Error in call to zero_srcsnk(), from bgc()\n");
		ok=0;
	}

#ifdef DEBUG
	printf("done zero_srcsnk\n");
#endif
	
	/* initialize the indicator for first day of current simulation, so
	that the checks for mass balance can have two days for comparison */
	first_balance = 1;
		
	/* begin the annual model loop */
	for (simyr=0 ; ok && simyr<ctrl.simyears ; simyr++)
	{
		/* reset the simple annual output variables for text output */
		annmaxlai = 0.0;
		annet = 0.0;
		anndeeppercol = 0.0;/* by Hidy 2011. - deeppercolation is calculated instead of outflow */
		annnpp = 0.0;
		annnee= 0.0;
		annnbp = 0.0;		/* by Hidy 2009. - including effect of management strategies */
		annprcp = 0.0;
		anntavg = 0.0;
		ann_carbonchange_THN = 0.0;       /* by Hidy 2012. */
		ann_carbonchange_MOW = 0.0;       /* by Hidy 2008. */
		ann_carbonchange_HRV = 0.0;        /* by Hidy 2008. */
		ann_carbonchange_PLG = 0.0;         /* by Hidy 2008. */
		ann_carbonchange_GRZ = 0.0;       /* by Hidy 2008. */
		ann_carbonchange_FRZ= 0.0;
		ann_carbonchange_PLT = 0.0;
		ann_nitrogen_GRZplus = 0.0;     /* by Hidy 2008. */
		ann_nitrogen_FRZplus = 0.0;  /* by Hidy 2008. */

		/* set vegetation period flag to 0 - Hidy 2013 */
		ctrl.vegper_flag = 0;

	
		/* set current month to 0 (january) at the beginning of each year */
		curmonth = 0;
		
		/* test whether metyr needs to be reset */
		if (metyr == ctrl.metyears)
		{
			if (ctrl.onscreen) printf("Resetting met data for cyclic input\n");
			metyr = 0;
		}

		/* output to screen to indicate start of simulation year */
		if (ctrl.onscreen) printf("Year: %6d\n",ctrl.simstartyear+simyr);

		/* set the max lai variable, for annual diagnostic output */
		epv.ytd_maxplai = 0.0;
		
		/* atmospheric CO2 and Ndep handling */
		if (!(co2.varco2))
		{
			/* constant CO2, constant Ndep */
			metv.co2 = co2.co2ppm;
			daily_ndep = sitec.ndep / NDAY_OF_YEAR;
		}
		else 
		{
			if (co2.varco2 == 1) metv.co2 = co2.co2ppm_array[simyr];
		}
		
		/* atmospheric Ndep handling */
		if (!(ramp_ndep.doramp))
		{
			/*constant Ndep */
			daily_ndep = sitec.ndep / NDAY_OF_YEAR;
		}
		else
		{	
			/* Ndep from file */
			daily_ndep = ramp_ndep.ndep_array[simyr]/365;
		}
		

		/* begin the daily model loop */
		for (yday=0 ; ok && yday<365 ; yday++)
		{
#ifdef DEBUG
			printf("year %d\tyday %d\n",simyr,yday);
#endif
			/* Hidy 2010. - counters into control sturct */ 
			ctrl.simyr = simyr;
			ctrl.yday = yday;
			ctrl.spinyears = 0;
			
			/* Hidy 2013. - determine vegetation period */ 
			if (ok && vegetation_period_determ(&ctrl, &phen))
			{
				printf("Error in call to vegetation_period_determ() from bgc()\n");
				ok=0;
			} 
			
			/* Test for very low state variable values and force them
			to 0.0 to avoid rounding and floating point overflow errors */
			if (ok && precision_control(&sitec, &ws, &cs, &ns))
			{
				printf("Error in call to precision_control() from bgc()\n");
				ok=0;
			} 
			
			/* set the day index for meteorological and phenological arrays */
			metday = metyr*365 + yday;
			
			/* zero all the daily flux variables */
			wf = zero_wf;
			cf = zero_cf;
			nf = zero_nf;


			/* daily meteorological variables from metarrays */
			if (ok && daymet(&ctrl, &metarr, &sitec, &ws, &metv, &tair_annavg, metday))
			{
				printf("Error in daymet() from bgc()\n");
				ok=0;
			}
			
#ifdef DEBUG
			printf("%d\t%d\tdone daymet\n",simyr,yday);
#endif

			
			/* daily phenological variables from phenarrays */
			if (ok && dayphen(&phenarr, &phen, metday, metyr))
			{
				printf("Error in dayphen() from bgc()\n");
				ok=0;
			}
			
#ifdef DEBUG
			printf("%d\t%d\tdone dayphen\n",simyr,yday);
#endif
	
			/* test for the annual allocation day */
			if (phen.remdays_litfall == 1) annual_alloc = 1;
			else annual_alloc = 0;

			
			/* phenology fluxes */
			if (ok && phenology(&epc, &phen, &epv, &cs, &cf, &ns, &nf))
			{
				printf("Error in phenology() from bgc()\n");
				ok=0;
			}
			
#ifdef DEBUG
			printf("%d\t%d\tdone phenology\n",simyr,yday);
#endif


			/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!! Hidy 2011 - MULTILAYER SOIL!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
	

			/* rooting depth */
 			 if (ok && multilayer_rootdepth(&ctrl, &epc, &sitec, &phen, &epv, &ns))
			 {
				printf("Error in multilayer_rootdepth() from bgc()\n");
				ok=0;
			 }
#ifdef DEBUG
			printf("%d\t%d\tdone multilayer_rootdepth\n",simyr,yday);
#endif

			/* soil temperature calculations */
			if (ok && multilayer_tsoil(&epc, &sitec, &ws, &metv, &epv))
			{
				printf("Error in multilayer_tsoil() from bgc()\n");
				ok=0;
			}
#ifdef DEBUG
			printf("%d\t%d\tdone multilayer_tsoil\n",simyr,yday);
#endif

			/* soil hydrological parameters: psi, vwc, conductivity  */
 			if (ok && multilayer_hydrolparams(&ctrl, &sitec, &ws, &epv))
			{
				printf("Error in multilayer_hydrolparams() from bgc()\n");
				ok=0;
			}
#ifdef DEBUG
			printf("%d\t%d\tdone multilayer_hydrolparams\n",simyr,yday);
#endif
			
			/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
	

			/* calculate leaf area index, sun and shade fractions, and specific
			leaf area for sun and shade canopy fractions, then calculate
			canopy radiation interception and transmission */
                        
			if (ok && radtrans(&cs, &epc, &metv, &epv, sitec.sw_alb))
			{
				printf("Error in radtrans() from bgc()\n");
				ok=0;
			}
			

			
			/* update the ann max LAI for annual diagnostic output */
			if (epv.proj_lai > epv.ytd_maxplai) epv.ytd_maxplai = epv.proj_lai;
			
#ifdef DEBUG
			printf("%d\t%d\tdone radtrans\n",simyr,yday);
#endif
			

			/* precip routing (when there is precip) */
			if (ok && metv.prcp && prcp_route(&metv, epc.int_coef, epv.all_lai,	&wf))
			{
				printf("Error in prcp_route() from bgc()\n");
				ok=0;
			}
			
#ifdef DEBUG
			printf("%d\t%d\tdone prcp_route\n",simyr,yday);
#endif

			/* snowmelt (when there is a snowpack) */
			if (ok && ws.snoww && snowmelt(&metv, &wf, ws.snoww))
			{
				printf("Error in snowmelt() from bgc()\n");
				ok=0;
			}
			
#ifdef DEBUG
			printf("%d\t%d\tdone snowmelt\n",simyr,yday);
#endif


			/* bare-soil evaporation (when there is no snowpack) */
			if (ok && baresoil_evap(&metv, &wf, &epv.dsr))
			{
				printf("Error in baresoil_evap() from bgc()\n");
				ok=0;
			}

#ifdef DEBUG
			printf("%d\t%d\tdone bare_soil evap\n",simyr,yday);
#endif

		
			
			/* daily maintenance respiration */
			if (ok && maint_resp(&cs, &ns, &epc, &metv, &sitec, &cf, &epv))
			{
				printf("Error in m_resp() from bgc()\n");
				ok=0;
			}

#ifdef DEBUG
			printf("%d\t%d\tdone maint resp\n",simyr,yday);
#endif


			/* conductance - Hidy 2011 */
			if (ok && conduct_calc(&ctrl, &metv, &epc, &sitec, &epv, simyr))
			{
				printf("Error in conduct_calc() from bgc()\n");
				ok=0;
			}
			
#ifdef DEBUG
			printf("%d\t%d\tdone conduct_calc\n",simyr,yday);
#endif

		
			/* begin canopy bio-physical process simulation */
			/* do canopy ET calculations whenever there is leaf area
			displayed, since there may be intercepted water on the 
			canopy that needs to be dealt with */
			if (ok && cs.leafc && metv.dayl)
			{
		
				/* evapo-transpiration */
				if (ok && canopy_et(&metv, &epv, &wf))
				{
					printf("Error in canopy_et() from bgc()\n");
					ok=0;
				}
				
#ifdef DEBUG
				printf("%d\t%d\tdone canopy_et\n",simyr,yday);
#endif

			}
			/* do photosynthesis only when it is part of the current
			growth season, as defined by the remdays_curgrowth flag.  This
			keeps the occurrence of new growth consistent with the treatment
			of litterfall and allocation */

			if (ok && cs.leafc && phen.remdays_curgrowth && metv.dayl && ws.snoww == 0)
			{
				/* SUNLIT canopy fraction photosynthesis */
				/* set the input variables */
				psn_sun.c3 = epc.c3_flag;
				psn_sun.co2 = metv.co2;
				psn_sun.pa = metv.pa;
				psn_sun.t = metv.tday;
				psn_sun.lnc = 1.0 / (epv.sun_proj_sla * epc.leaf_cn);
				psn_sun.flnr = epc.flnr;
				psn_sun.ppfd = metv.ppfd_per_plaisun;
				/* convert conductance from m/s --> umol/m2/s/Pa, and correct
				for CO2 vs. water vapor */
				psn_sun.g = epv.gl_t_wv_sun * 1e6/(1.6*R*(metv.tday+273.15));
				psn_sun.dlmr = epv.dlmr_area_sun;
				if (ok && photosynthesis(&psn_sun))
				{
					printf("Error in photosynthesis() from bgc()\n");
					ok=0;
				}
				
#ifdef DEBUG
				printf("%d\t%d\tdone sun psn\n",simyr,yday);
#endif

				epv.assim_sun = psn_sun.A;
				
				/* for the final flux assignment, the assimilation output
				needs to have the maintenance respiration rate added, this
				sum multiplied by the projected leaf area in the relevant canopy
				fraction, and this total converted from umol/m2/s -> kgC/m2/d */
	
				cf.psnsun_to_cpool = (epv.assim_sun + epv.dlmr_area_sun) * epv.plaisun * metv.dayl * 12.011e-9; 
				
						
				/* SHADED canopy fraction photosynthesis */
				psn_shade.c3 = epc.c3_flag;
				psn_shade.co2 = metv.co2;
				psn_shade.pa = metv.pa;
				psn_shade.t = metv.tday;
				psn_shade.lnc = 1.0 / (epv.shade_proj_sla * epc.leaf_cn);
				psn_shade.flnr = epc.flnr;
				psn_shade.ppfd = metv.ppfd_per_plaishade;
				/* convert conductance from m/s --> umol/m2/s/Pa, and correct
				for CO2 vs. water vapor */
				psn_shade.g = epv.gl_t_wv_shade * 1e6/(1.6*R*(metv.tday+273.15));
				psn_shade.dlmr = epv.dlmr_area_shade;
			
				if (ok && photosynthesis(&psn_shade))
				{
					printf("Error in photosynthesis() from bgc()\n");
					ok=0;
				}
				
#ifdef DEBUG
				printf("%d\t%d\tdone shade_psn\n",simyr,yday);
#endif

				epv.assim_shade = psn_shade.A;

				/* for the final flux assignment, the assimilation output
				needs to have the maintenance respiration rate added, this
				sum multiplied by the projected leaf area in the relevant canopy
				fraction, and this total converted from umol/m2/s -> kgC/m2/d */

				cf.psnshade_to_cpool = (epv.assim_shade + epv.dlmr_area_shade) * epv.plaishade * metv.dayl * 12.011e-9; 
			


			} /* end of photosynthesis calculations */

			else
			{
			       /* original */
				epv.assim_sun = epv.assim_shade = 0.0;
    				/*new 29.5.02 */
				psn_sun.Ci=psn_shade.Ci=0.0;
				psn_sun.lnc=psn_shade.lnc=0.0;
				psn_sun.g=psn_shade.g=0.0;
				psn_sun.dlmr=psn_shade.dlmr=0.0;
				psn_sun.Vmax=psn_shade.Vmax=0.0;
				psn_sun.Jmax=psn_shade.Jmax=0.0;
				psn_sun.A=psn_shade.A=0.0;
				epv.m_ppfd_sun=epv.m_ppfd_shade=0.0;
			}

			/* Hidy 2010 - calculation water stress days */
 			if (ok && waterstress_days(yday, &phen, &epv))
			{
				printf("Error in waterstress_days() from bgc()\n");
				ok=0;
			}
#ifdef DEBUG
			printf("%d\t%d\tdone waterstress_days\n",simyr,yday);
#endif

			/* !!!!!!!!!!!!!!!!!!!!!! TRANSPIRATION AND SOILPSI IN MULTILAYER SOIL!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
		
			/* Hidy 2010 - calculate the part-transpiration from total transpiration */
			if (ok && multilayer_transpiration(&ctrl, &sitec, &epc, &epv, &ws, &wf))
			{
				printf("Error in multilayer_transpiration() from bgc()\n");
				ok=0;
			}
#ifdef DEBUG
			printf("%d\t%d\tdone multilayer_transpiration\n",simyr,yday);
#endif
			/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
		
			/* nitrogen deposition and fixation */
			nf.ndep_to_sminn = daily_ndep;
			nf.nfix_to_sminn = sitec.nfix / NDAY_OF_YEAR;
			


			/* daily litter and soil decomp and nitrogen fluxes */
			if (ok && decomp(&metv,&epc,&epv,&sitec,&cs,&cf,&ns,&nf,&nt))
			{
				printf("Error in decomp() from bgc.c\n");
				ok=0;
			}
			
#ifdef DEBUG
			printf("%d\t%d\tdone decomp\n",simyr,yday);
#endif

			/* Daily allocation gets called whether or not this is a
			current growth day, because the competition between decomp
			immobilization fluxes and plant growth N demand is resolved
			here.  On days with no growth, no allocation occurs, but
			immobilization fluxes are updated normally */
			ctrl.n_limitation = 0;
			if (ok && daily_allocation(&cf,&cs,&nf,&ns,&epc,&epv,&nt,&ctrl.n_limitation))
			{
				printf("Error in daily_allocation() from bgc.c\n");
				ok=0;
			}

#ifdef DEBUG
			printf("%d\t%d\tdone daily_allocation\n",simyr,yday);
#endif



			/* reassess the annual turnover rates for livewood --> deadwood,
			and for evergreen leaf and fine root litterfall. This happens
			once each year, on the annual_alloc day (the last litterfall day) */
			if (ok && annual_alloc)
			{
				if (ok && annual_rates(&epc,&epv))
				{
					printf("Error in annual_rates() from bgc()\n");
					ok=0;
				}
				
#ifdef DEBUG
				printf("%d\t%d\tdone annual rates\n",simyr,yday);
#endif
			} 


			/* daily growth respiration */
			if (ok && growth_resp(&epc, &cf))
			{
				printf("Error in daily_growth_resp() from bgc.c\n");
				ok=0;
			}
			
#ifdef DEBUG
			printf("%d\t%d\tdone growth_resp\n",simyr,yday);
#endif
		
			/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!  MULTILAYER SOIL !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
			/* Hidy 2010 - multilayer soil hydrology: percolation calculation based on PRCP, RUNOFF, EVAP, TRANS */
			
			if (ok && multilayer_hydrolprocess(&ctrl, &sitec, &epc, &epv, &ws, &wf))
			{
				printf("Error in multilayer_hydrolprocess() from bgc()\n");
				ok=0;
			}

#ifdef DEBUG
			printf("%d\t%d\tdone multilayer_hydrolprocess\n",simyr,yday);
#endif	
			/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */


			/* daily update of the water state variables */
			if (ok && daily_water_state_update(&ctrl, &sitec, &wf, &ws))
			{
				printf("Error in daily_water_state_update() from bgc()\n");
				ok=0;
			}
			
#ifdef DEBUG
			printf("%d\t%d\tdone water state update\n",simyr,yday);
#endif
		
			

			/* daily update of carbon state variables */
			if (ok && daily_carbon_state_update(&ctrl, &cf, &cs, annual_alloc,
				epc.woody, epc.evergreen))
			{
				printf("Error in daily_carbon_state_update() from bgc()\n");
				ok=0;
			}

#ifdef DEBUG
			printf("%d\t%d\tdone carbon state update\n",simyr,yday);
#endif

			/* daily update of nitrogen state variables */
			if (ok && daily_nitrogen_state_update(&ctrl, &nf, &ns, annual_alloc,
				epc.woody, epc.evergreen))
			{
				printf("Error in daily_nitrogen_state_update() from bgc()\n");
				ok=0;
			}
			
#ifdef DEBUG
			printf("%d\t%d\tdone nitrogen state update\n",simyr,yday);
#endif

			/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!  MANAGEMENT SUBMODULES !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */

			/* PLANTING - Hidy 2009. */
           if (ok && planting(yday, &ctrl, &epc, &PLT, &cs, &ns, &cf, &nf, &phen))
			{
				printf("Error in planting() from bgc()\n");
				ok=0;
		   }

		   	/* THINNIG - Hidy 2012. */
           if (ok && thinning(yday, &phen, &ctrl, &epc, &THN, &cf, &nf, &wf, &cs, &ns, &ws))
			{
				printf("Error in thinning() from bgc()\n");
				ok=0;
		   }

			/* MOWING - Hidy 2008. */
           if (ok && mowing(yday, &phen, &ctrl, &epc, &MOW, &cf, &nf, &wf, &cs, &ns, &ws))
			{
				printf("Error in mowing() from bgc()\n");
				ok=0;
		   }

		   /* GRAZING - Hidy 2009. */
           if (ok && grazing(yday, &phen, &ctrl, &epc, &GRZ, &cf, &nf, &wf, &cs, &ns, &ws))
			{
				printf("Error in grazing() from bgc()\n");
				ok=0;
		   }
	
		   	/* HARVESTING - Hidy 2012. */
           if (ok && harvesting(yday, &phen, &ctrl, &epc, &HRV, &cf, &nf, &wf, &cs, &ns, &ws))
			{
				printf("Error in harvesting() from bgc()\n");
				ok=0;
		   }
 
		   /* PLOUGHING - Hidy 2012. */
           if (ok && ploughing(yday, &phen, &ctrl, &epc, &PLG, &cf, &nf, &wf, &cs, &ns, &ws))
			{
				printf("Error in ploughing() from bgc()\n");
				ok=0;
		   }
		 
		   /* FERTILIZING -  Hidy 2008 */
	    	if (ok && fertilizing(yday, &ctrl, &FRZ, &cs, &ns, &cf, &nf))
			{
				printf("Error in fertilizing() from bgc()\n");
				ok=0;
			}		

			/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */


			/* Hidy 2010 - test again for very low state variable values and force them
				to 0.0 to avoid rounding and floating point overflow errors */
			if (ok && precision_control(&sitec, &ws, &cs, &ns))
			{
				printf("Error in call to precision_control() from bgc()\n");
				ok=0;
			} 
				
#ifdef DEBUG
				printf("%d\t%d\tdone precision_control\n",simyr,yday);
#endif
			/* calculate daily mortality fluxes and update state variables */
			/* this is done last, with a special state update procedure, to
			insure that pools don't go negative due to mortality fluxes
			conflicting with other proportional fluxes */
			if (ok && mortality(&ctrl, &epc, &cs, &cf, &ns, &nf, simyr))
			{
				printf("Error in mortality() from bgc()\n");
				ok=0;
			}
			
#ifdef DEBUG
			printf("%d\t%d\tdone mortality\n",simyr,yday);
#endif
			/* Hidy 2013 - calculate daily senescence mortality fluxes and update state variables */
			if (ok && senescence(&epc, &metv, &sitec, &cs, &cf, &ns, &nf, &epv))
			{
				printf("Error in senescence() from bgc()\n");
				ok=0;
			}
			
#ifdef DEBUG
			printf("%d\t%d\tdone mortality\n",simyr,yday);
#endif

			/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!  MULTILAYER SOIL !!!!!!!!!!!!!!!!!!!!!!!!! */
			/* Hidy 2011 - calculate the change of soil mineralized N in multilayer soil.  
                        This is a special state variable update routine, done after the other fluxes and states are
                        reconciled (nleaching is included) */

			if (ok && multilayer_sminn(&sitec, &epv, &ns, &nf, &ws, &wf))
			{
				printf("Error in multilayer_sminn() from bgc()\n");
				ok=0;
			}
			
#ifdef DEBUG
			printf("%d\t%d\tdone multilayer_sminn\n",simyr,yday);
#endif



			/* test for water balance*/
			if (ok && check_water_balance(&ws, &ctrl, first_balance))
			{
				printf("Error in check_water_balance() from bgc()\n");
				printf("%d\n",metday);
				ok=0;
			}
			 
#ifdef DEBUG
			printf("%d\t%d\tdone water balance\n",simyr,yday);
#endif

		
	
			if (ok  && check_carbon_balance(&cs, &ctrl, first_balance))
			{
				printf("Error in check_carbon_balance() from bgc()\n");
				printf("%d\n",metday);
				ok=0;
			}
	
		
			
#ifdef DEBUG
			printf("%d\t%d\tdone carbon balance\n",simyr,yday); 
#endif

			/* test for nitrogen balance -  by Hidy 2008 */
				if (ok && check_nitrogen_balance(&ns, &ctrl, first_balance))
				{
					printf("Error in check_nitrogen_balance() from bgc()\n");
					printf("%d\n",metday);
					ok=0;
				}
			
			
#ifdef DEBUG
			printf("%d\t%d\tdone nitrogen balance\n",simyr,yday);
#endif


			

			/* calculate summary variables */
			if (ok && csummary(&cf, &cs, &nf, &summary))
			{
				printf("Error in csummary() from bgc()\n");
				ok=0;
			}
	
	
#ifdef DEBUG
			printf("%d\t%d\tdone carbon summary\n",simyr,yday);
#endif

			/* INTERNAL VARIALBE CONTROL - Hidy 2013 */
			if (ctrl.onscreen && ctrl.simyr < 10)
			{
				fprintf(bgcout->control_file.ptr, "%i %f %f %f %f %f %f %f %f %f %f %f %f\n", 
				yday, 
				epv.vwc[0], epv.vwc[3], epv.w_scalar, 
				summary.cum_npp*1000, cs.SNSC_src*1000, cs.litr1c, cs.soil4c, ns.soil4n,
				summary.daily_gpp*1000, (summary.daily_mr+summary.daily_gr)*1000, summary.daily_hr*1000,
				wf.canopyw_evap+wf.soilw_evap+wf.soilw_trans_SUM+wf.snoww_subl); 

			}

			/* DAILY OUTPUT HANDLING */
			/* fill the daily output array if daily output is requested,
			or if the monthly or annual average of daily output variables
			have been requested */
			if (ok && dayout)
			{
				/* fill the daily output array */
				for (outv=0 ; outv<ctrl.ndayout ; outv++)
				{
					dayarr[outv] = (float) *(output_map[ctrl.daycodes[outv]]);
				}
			}
			/* only write daily outputs if requested */
			if (ok && ctrl.dodaily)
			{
				/* write the daily output array to daily output file */
				if (fwrite(dayarr, sizeof(float), ctrl.ndayout, bgcout->dayout.ptr)
					!= (size_t)ctrl.ndayout)
				{
					printf("Error writing to %s: simyear = %d, simday = %d\n",
						bgcout->dayout.name,simyr,yday);
					ok=0;
				}
				
#ifdef DEBUG
				printf("%d\t%d\tdone daily output\n",simyr,yday);
#endif
				
			}
			
			/* MONTHLY AVERAGE OF DAILY OUTPUT VARIABLES */
			if (ctrl.domonavg)
			{
				/* update the monthly average array */
				for (outv=0 ; outv<ctrl.ndayout ; outv++)
				{
					monavgarr[outv] += dayarr[outv];
				}
				
				/* if this is the last day of the current month, output... */
				if (yday == endday[curmonth])
				{
					/* finish the averages */
					for (outv=0 ; outv<ctrl.ndayout ; outv++)
					{
						monavgarr[outv] /= (float)mondays[curmonth];
					}
					
					/* write to file */
					if (fwrite(monavgarr, sizeof(float), ctrl.ndayout, bgcout->monavgout.ptr)
						!= (size_t)ctrl.ndayout)
					{
						printf("Error writing to %s: simyear = %d, simday = %d\n",
							bgcout->monavgout.name,simyr,yday);
						ok=0;
					}
					
					/* reset monthly average variables for next month */
					for (outv=0 ; outv<ctrl.ndayout ; outv++)
					{
						monavgarr[outv] = 0.0;
					}
					
					/* increment current month counter */
					curmonth++;
					
#ifdef DEBUG
					printf("%d\t%d\tdone monavg output\n",simyr,yday);
#endif
				
				}
			}
			
			/* ANNUAL AVERAGE OF DAILY OUTPUT VARIABLES */
			if (ctrl.doannavg)
			{
				/* update the annual average array */
				for (outv=0 ; outv<ctrl.ndayout ; outv++)
				{
					annavgarr[outv] += dayarr[outv];
				}
				
				/* if this is the last day of the year, output... */
				if (yday == 364)
				{
					/* finish averages */
					for (outv=0 ; outv<ctrl.ndayout ; outv++)
					{
						annavgarr[outv] /= NDAY_OF_YEAR;
					}
					
					/* write to file */
					if (fwrite(annavgarr, sizeof(float), ctrl.ndayout, bgcout->annavgout.ptr)
						!= (size_t)ctrl.ndayout)
					{
						printf("Error writing to %s: simyear = %d, simday = %d\n",
							bgcout->annavgout.name,simyr,yday);
						ok=0;
					}
					
					/* reset annual average variables for next month */
					for (outv=0 ; outv<ctrl.ndayout ; outv++)
					{
						annavgarr[outv] = 0.0;
					}
					
#ifdef DEBUG
					printf("%d\t%d\tdone annavg output\n",simyr,yday);
#endif
				
				}
			}
	
	
			/* very simple annual summary variables for text file output */
			if (epv.proj_lai > annmaxlai) annmaxlai = epv.proj_lai;
			
			if (ctrl.spinup == 0 && epv.dsws > 0 && ctrl.onscreen) printf("stomatal conductance is SWC-limited:%d\t\n",yday);
			
			annet += wf.canopyw_evap + wf.snoww_subl + wf.soilw_evap + wf.soilw_trans_SUM;			
			anndeeppercol += wf.soilw_percolated[N_SOILLAYERS-2];
			annnpp += summary.daily_npp * 1000.0;		/* (kgC/m2 -> gC/m2) */
			annnee += summary.daily_nee * 1000.0;		/* (kgC/m2 -> gC/m2) */
			annnbp += summary.daily_nbp * 1000.0;
			annprcp += metv.prcp;
			anntavg += metv.tavg / NDAY_OF_YEAR;
			ann_carbonchange_THN += summary.carbonchange_THN * 1000.0;		/* (kgC/m2 -> gC/m2) ;  Hidy 2012. */
			ann_carbonchange_MOW += summary.carbonchange_MOW * 1000.0;		/* (kgC/m2 -> gC/m2) ;  Hidy 2008. */
			ann_carbonchange_HRV += summary.carbonchange_HRV * 1000.0;     /* (kgC/m2 -> gC/m2) ;  Hidy 2008. */
			ann_carbonchange_PLG += summary.carbonchange_PLG * 1000.0;       /* (kgC/m2 -> gC/m2) ;  Hidy 2008. */
			ann_carbonchange_GRZ += summary.carbonchange_GRZ * 1000.0;		/* (kgC/m2 -> gC/m2) ;  Hidy 2008. */
			ann_carbonchange_FRZ += summary.carbonchange_FRZ * 1000.0;		/* (kgC/m2 -> gC/m2) ;  Hidy 2008. */
			ann_carbonchange_PLT += summary.carbonchange_PLT * 1000.0;		/* (kgC/m2 -> gC/m2) ;  Hidy 2008. */
			ann_nitrogen_GRZplus += (nf.GRZ_to_litr1n + nf.GRZ_to_litr2n + nf.GRZ_to_litr3n  + nf.GRZ_to_litr4n) * 1000.0;  /* (kgC/m2 -> gC/m2) ;  Hidy 2008. */	
			ann_nitrogen_FRZplus += (nf.FRZ_to_sminn+nf.FRZ_to_litr1n + nf.FRZ_to_litr2n + nf.FRZ_to_litr3n  + nf.FRZ_to_litr4n) * 1000.0;  /* (kgC/m2 -> gC/m2) ;  Hidy 2008. */
	
			/* at the end of first day of simulation, turn off the 
			first_balance switch */
			if (first_balance) first_balance = 0;

		}   /* end of daily model loop */
		
		/* ANNUAL OUTPUT HANDLING */
		/* only write annual outputs if requested */
		if (ok && ctrl.doannual)
		{
			/* fill the annual output array */
			for (outv=0 ; outv<ctrl.nannout ; outv++)
			{
				annarr[outv] = (float) *output_map[ctrl.anncodes[outv]];
			}
			/* write the annual output array to annual output file */
			if (fwrite(annarr, sizeof(float), ctrl.nannout, bgcout->annout.ptr)
				!= (size_t)ctrl.nannout)
			{
				printf("Error writing to %s: simyear = %d, simday = %d\n",
					bgcout->annout.name,simyr,yday);
				ok=0;
			}
			
#ifdef DEBUG
			printf("%d\t%d\tdone annual output\n",simyr,yday);
#endif
		}
		/* write the simple annual text output - Hidy 2008. */
		fprintf(bgcout->anntext.ptr,"%i %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f\n",
			ctrl.simstartyear+simyr,annprcp,anntavg,annmaxlai,annet,anndeeppercol,annnee,annnbp,ann_carbonchange_THN, ann_carbonchange_MOW,
			ann_carbonchange_HRV,ann_carbonchange_PLG,ann_carbonchange_GRZ, ann_carbonchange_FRZ, ann_carbonchange_PLT, 
			ann_nitrogen_GRZplus, ann_nitrogen_FRZplus);

		metyr++;

	}   /* end of annual model loop */
	
	/* RESTART OUTPUT HANDLING */
	/* if write_restart flag is set, copy data to the output restart struct */
	if (ok && ctrl.write_restart)
	{
		if (restart_output(&sitec, &ctrl, &ws, &cs, &ns, &epv, metyr, 
			&(bgcout->restart_output)))
		{
			printf("Error in call to restart_output() from bgc()\n");
			ok=0;
		}
		
#ifdef DEBUG
		printf("%d\t%d\tdone restart output\n",simyr,yday);
#endif
	}

	/* free phenology memory */
	if (ok && free_phenmem(&phenarr))
	{
		printf("Error in free_phenmem() from bgc()\n");
		ok=0;
	}

#ifdef DEBUG
	printf("%d\t%d\tdone free phenmem\n",simyr,yday);
#endif
	
	/* free memory for local output arrays */
	
	if (dayout) free(dayarr);
	if (ctrl.domonavg) free(monavgarr);
	if (ctrl.doannavg) free(annavgarr);
	if (ctrl.doannual) free(annarr);
	free(output_map);
	
	/* print timing info if error */
	if (!ok)
	{
		printf("ERROR at year %d\n",simyr-1);
		printf("ERROR at yday %d\n",yday-1);
	}
	

	/* !!!!!!!!!!! close control file - Hidy 2009.!!!!!!!!!!!!*/
	if (ctrl.onscreen)
	{
		if (ctrl.GSI_flag)
		{
			fclose (GSI.GSI_file.ptr);
		}
		fclose (bgcout->control_file.ptr);
	}

	/* return error status */	
	return (!ok);
}
