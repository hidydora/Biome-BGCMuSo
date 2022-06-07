	/*
pointbgc.c
front-end to BIOME-BGC for single-point, single-biome simulations
Uses BBGC MuSo v6 library function

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
#include <math.h>
#include <malloc.h>
#include <string.h>
#include <time.h>

#include "ini.h"              /* general file structure and I/O prototypes */
#include "bgc_struct.h"       /* data structures for bgc() */
#include "pointbgc_struct.h"   /* data structures for point driver */
#include "pointbgc_func.h"     /* function prototypes for point driver */
#include "bgc_io.h"           /* bgc() interface definition */
#include "bgc_epclist.h"      /* array structure for epc-by-vegtype */
#include "bgc_constants.h"      /* array structure for epc-by-vegtype */

int main(int argc, char *argv[])
{
	int errflag=0;
	int transient=0;
	char printVERSION[] = "-v";
	char keyword[STRINGSIZE];

	/* bgc input and output structures */
	bgcin_struct bgcin;
	bgcout_struct bgcout;

	/* local control information */
	point_struct point;
	restart_ctrl_struct restart;
	climchange_struct scc;
	output_struct output;
	
	/* initialization file */
	file init;
	
	/* system time variables */
	struct tm *tm_ptr;
	time_t lt;
	
	/* get the system time at start of simulation  - WARNING: unsafe functions: localtime, asctime */
	lt = time(NULL);
	tm_ptr = localtime(&lt);
	strcpy(point.systime,asctime(tm_ptr));
	
	
	/* initialize the bgcin state variable structures before filling with
	values from ini file */
	if (presim_state_init(&bgcin.ws, &bgcin.cs, &bgcin.ns, &bgcin.cinit))
	{
		printf("ERROR in call to presim_state_init() from pointbgc.c ... Exiting()\n");
		exit(101);
	}

	/* initialization */
	bgcin.ctrl.simyr = 0;								
	bgcin.ctrl.yday = 0;								
	bgcin.ctrl.plantyr = -1;								
	bgcin.ctrl.spinyears = 0;							
	bgcin.ctrl.month = 1;								
	bgcin.ctrl.day = 1;									
	bgcin.ctrl.CNerror = 0;
	bgcin.ctrl.gwd_act=DATA_GAP;
	bgcin.ctrl.limitevap_flag = 0;
	bgcin.ctrl.limittransp_flag = 0;
	bgcin.ctrl.limitMR_flag = 0;
	bgcin.ctrl.limitSNSC_flag = 0;
	bgcin.ctrl.limitleach_flag = 0;
	bgcin.ctrl.limitdiffus_flag = 0;
	bgcin.ctrl.pond_flag = 0;
	bgcin.ctrl.notransp_flag = 0;
	bgcin.ctrl.noMR_flag = 0;
	bgcin.ctrl.grazingW_flag = 0;
	bgcin.ctrl.condMOWerr_flag = 0;      
	bgcin.ctrl.condIRGerr_flag = 0;        
	bgcin.ctrl.prephen1_flag = 0;         
	bgcin.ctrl.prephen2_flag = 0;          
	bgcin.ctrl.bareground_flag = 0;
	bgcin.ctrl.vegper_flag = 0;
	bgcin.ctrl.south_shift = 0;

	/******************************
	**                           **
	**  BEGIN READING INIT FILE  **
	**                           **
	******************************/
	
	/* wrinting on screen: model version 

	strcpy(keyword,argv[1]);
	if (!strcmp(keyword, printVERSION))
	{
		printf("Model version: Biome-BGCMuSo6.0-4 OR Biome-BGCMAg2.0-4\n");
		exit(0);
	} 
	*/
	/* read the name of the main init file from the command line and store as init.name */
	if (argc != 2)
	{
		printf("ERROR in reading the main init file from command line. Exiting\n");
		printf("Correct usage: <executable name>  <initialization file name>\n");
		exit(102);
	} 
	
	strcpy(init.name, argv[1]);

	/* open the main init file for ascii read and check for errors */
	if (file_open(&init,'i',1))
	{
		printf("ERROR opening init file, pointbg.c ... Exiting\n");
		exit(103);
	}

	/* read the header string from the init file */
	if (fgets(point.header, 100, init.ptr)==NULL)
	{
		printf("ERROR reading header string: pointbgc.c ... Exiting\n");
		exit(201);
	}

	/* open met file, discard header lines */
	errflag = met_init(init, &point);
	
	if (errflag)
	{
		printf("ERROR in call to met_init() from pointbgc.c... Exiting\n");
		exit(errflag);
	}

	/* read restart control parameters */
	errflag = restart_init(init, &restart);
	if (errflag)
	{
		printf("ERROR in call to restart_init() from pointbgc.c... Exiting\n");
		exit(errflag);
	}

	/* read simulation timing control parameters */
	errflag = time_init(init, &(bgcin.ctrl));
	if (errflag)
	{
		printf("ERROR in call to time_init() from pointbgc.c... Exiting\n");
		exit(errflag);
	}
	
	
	/* read CO2 control parameters */
	errflag = co2_init(init, &(bgcin.co2), &(bgcin.ctrl));
	if (errflag)
	{
		printf("ERROR in call to co2_init() from pointbgc.c... Exiting\n");
		exit(errflag);
	}


	/* read varied nitrogen deposition block */
	errflag = ndep_init(init, &bgcin.ndep, &(bgcin.ctrl));
	if (errflag)
	{
		printf("ERROR in call to ndep_init() from pointbgc.c... Exiting\n");
		exit(errflag);
	}


	/* read site constants */
	errflag = sitec_init(init, &bgcin.sitec, &(bgcin.ctrl));
	if (errflag)
	{
		printf("ERROR in call to sitec_init() from pointbgc.c... Exiting\n");
		exit(errflag);
	}
	
	/* read soil properties */
	errflag = sprop_init(init, &bgcin.sprop, &bgcin.ctrl);
	if (errflag)
	{
		printf("ERROR in call to sprop_init() from pointbgc.c... Exiting\n");
		exit(errflag);
	}
	
	/* read ecophysiological constants */
	errflag = epc_init(init, &bgcin.epc, &bgcin.ctrl, 1);
	if (errflag)
	{
		printf("ERROR in call to epc_init() from pointbgc.c... Exiting\n");
		exit(errflag);
	}

	/* read management file with management information */
	errflag = mgm_init(init, &bgcin.ctrl, &bgcin.epc, &bgcin.FRZ, &bgcin.GRZ, &bgcin.HRV, &bgcin.MOW, &bgcin.PLT, &bgcin.PLG, &bgcin.THN, &bgcin.IRG, &output);
	if (errflag)
	{
		printf("ERROR in call to mgm_init() from pointbgc.c... Exiting\n");
		exit(errflag);
	}

	/* read simulation control flags */
	errflag = simctrl_init(init, &bgcin.epc, &bgcin.ctrl, &bgcin.PLT);
	if (errflag)
	{
		printf("ERROR in call to simctrl_init() from pointbgc.c... Exiting\n");
		exit(errflag);
	}

	/* initialize water state structure */
	errflag = wstate_init(init, &bgcin.sitec, &bgcin.sprop, &bgcin.ws);
	if (errflag)
	{
		printf("ERROR in call to wstate_init() from pointbgc.c... Exiting\n");
		exit(errflag);
	}

	/* initialize carbon and nitrogen state structures */
	errflag = cnstate_init(init, &bgcin.epc, &bgcin.cs, &bgcin.cinit, &bgcin.ns);
	if (errflag)
	{
		printf("ERROR in call to cstate_init() from pointbgc.c... Exiting\n");
		exit(errflag);
	}
  
	/* read scalar climate change parameters */
	errflag = scc_init(init, &scc);
	if (errflag)
	{
		printf("ERROR in call to scc_init() from pointbgc.c... Exiting\n");
		exit(errflag);
	}

	/* read conditional management strategies parameters */
	errflag = conditionalMGM_init(init, &bgcin.ctrl, &bgcin.IRG, &bgcin.MOW);
	if (errflag)
	{
		printf("ERROR in call to scc_init() from pointbgc.c... Exiting\n");
		exit(errflag);
	}

	/* read the output control information */
	if ((bgcin.co2.varco2 == 1 || bgcin.ndep.varndep == 1) && bgcin.ctrl.spinup == 1) transient = 1;
	errflag = output_init(init, transient, &output);
	if (errflag)
	{
		printf("ERROR in call to output_init() from pointbgc.c... Exiting\n");
		exit(errflag);
	}
	

	
	/* -------------------------------------------------------------------------*/

	/* read final line out of init file to test for proper file structure */
	errflag = end_init(init);
	if (errflag)
	{
		printf("ERROR in call to end_init() from pointbgc.c... exiting\n");
		exit(errflag);
	}
	fclose(init.ptr);

	/* read meteorology file, build metarr arrays, compute running avgs */
	errflag = metarr_init(&point, &bgcin.metarr, &scc, &bgcin.sitec, &bgcin.ctrl);
	if (errflag)
	{
		printf("ERROR in call to metarr_init() from pointbgc.c... Exiting\n");
		exit(errflag);
	}
	fclose(point.metf.ptr);

	/* read groundwater depth if it is available */
	errflag = groundwater_init(&bgcin.sitec, &bgcin.ctrl);
	if (errflag)
	{
		printf("ERROR in call to groundwater_init() from pointbgc.c... Exiting\n");
		exit(errflag);
	}
	


	/* copy some of the info from input structure to bgc simulation control structure */
 	bgcin.ctrl.onscreen = output.onscreen;
	bgcin.ctrl.dodaily = output.dodaily;
	bgcin.ctrl.domonavg = output.domonavg;
	bgcin.ctrl.doannavg = output.doannavg;
	bgcin.ctrl.doannual = output.doannual;
	bgcin.ctrl.ndayout = output.ndayout;
	bgcin.ctrl.nannout = output.nannout;
	bgcin.ctrl.daycodes = output.daycodes;
	bgcin.ctrl.daynames = output.daynames;
	bgcin.ctrl.anncodes = output.anncodes;
	bgcin.ctrl.annnames = output.annnames;
	bgcin.ctrl.read_restart = restart.read_restart;
	bgcin.ctrl.write_restart = restart.write_restart;
	bgcin.ctrl.GSI_flag = bgcin.epc.GSI_flag;			
	bgcin.ctrl.condIRG_flag = bgcin.IRG.condIRG_flag;  
	bgcin.ctrl.condMOW_flag = bgcin.MOW.condMOW_flag;   

	/* copy the output file structures into bgcout */
	bgcout.dayout  = output.dayout;
	bgcout.dayoutT = output.dayoutT;
	
	bgcout.monavgout  = output.monavgout;
	bgcout.monavgoutT = output.monavgoutT;
	
	bgcout.annavgout  = output.annavgout;
	bgcout.annavgoutT = output.annavgoutT;
	
	bgcout.annout  = output.annout;
	bgcout.annoutT = output.annoutT;

	bgcout.log_file = output.log_file;
	
	
	
	/* if using an input restart file, read a record */
	if (restart.read_restart)
	{
		fread(&(bgcin.restart_input),sizeof(restart_data_struct),1,restart.in_restart.ptr);
	}

	/*********************
	**                  **
	**  CALL BIOME-BGC  **
	**                  **
	*********************/

	
	/* all initialization complete, call model */
	/* either call the spinup code or the normal simulation code */
	if (bgcin.ctrl.spinup)
	{
		errflag = spinup_bgc(&bgcin, &bgcout);
	 	if (errflag)
		{
			fprintf(output.log_file.ptr, "\n");
			fprintf(output.log_file.ptr, "ERROR in spinup run\n");
			fprintf(output.log_file.ptr, "error code: %i\n", errflag);
			fprintf(output.log_file.ptr, "SIMULATION STATUS [0 - failure; 1 - success]\n");
			fprintf(output.log_file.ptr, "0\n");
			exit(errflag);
		}
		else
		{
			fprintf(output.log_file.ptr, "\n");
			fprintf(output.log_file.ptr, "SIMULATION STATUS [0 - failure; 1 - success]\n");
			fprintf(output.log_file.ptr, "1\n");
		}
	
	}
	else
	{   
		errflag = bgc(&bgcin, &bgcout);
		if (errflag)
		{
			fprintf(output.log_file.ptr, "\n");
			fprintf(output.log_file.ptr, "ERROR in normal run\n");
			fprintf(output.log_file.ptr, "error code: %i\n", errflag);
			fprintf(output.log_file.ptr, "\n");
			fprintf(output.log_file.ptr, "SIMULATION STATUS [0 - failure; 1 - success]\n");
			fprintf(output.log_file.ptr, "0\n");
			exit(errflag);
		}
		else
		{
			fprintf(output.log_file.ptr, "\n");
			fprintf(output.log_file.ptr, "SIMULATION STATUS [0 - failure; 1 - success]\n");
			fprintf(output.log_file.ptr, "1\n");
		}
	}
		

	/* if using an output restart file, write a record */
	if (restart.write_restart)
	{
		fwrite(&(bgcout.restart_output),sizeof(restart_data_struct),1,
			restart.out_restart.ptr);
	}

	/* post-processing output handling, if any, goes here */
	
	/* free memory */
	free(bgcin.metarr.tmax);
	free(bgcin.metarr.tmin);
	free(bgcin.metarr.prcp);
	free(bgcin.metarr.vpd);
	free(bgcin.metarr.tday);
	free(bgcin.metarr.tavg);
	free(bgcin.metarr.tavg11_ra);
	free(bgcin.metarr.tavg30_ra);
	free(bgcin.metarr.tavg10_ra);
	free(bgcin.metarr.F_temprad);
	free(bgcin.metarr.F_temprad_ra);
	free(bgcin.metarr.swavgfd);
	free(bgcin.metarr.par);
	free(bgcin.metarr.dayl);

	if(bgcin.PLT.PLT_num)
	{
		free(bgcin.PLT.PLTyear_array);  
		free(bgcin.PLT.PLTmonth_array); 
		free(bgcin.PLT.PLTday_array); 
		free(bgcin.PLT.germ_depth_array); 
		free(bgcin.PLT.n_seedlings_array); 
		free(bgcin.PLT.weight_1000seed_array); 
		free(bgcin.PLT.seed_carbon_array); 
		free(bgcin.PLT.filename_array);
	}

	if (bgcin.THN.THN_num)
	{
		free(bgcin.THN.THNyear_array);  
		free(bgcin.THN.THNmonth_array); 
		free(bgcin.THN.THNday_array); 
		free(bgcin.THN.thinningrate_w_array); 
		free(bgcin.THN.thinningrate_nw_array); 
		free(bgcin.THN.transpcoeff_w_array); 
		free(bgcin.THN.transpcoeff_nw_array); 
	}

	if (bgcin.MOW.MOW_num)
	{
		free(bgcin.MOW.MOWyear_array);  
		free(bgcin.MOW.MOWmonth_array); 
		free(bgcin.MOW.MOWday_array); 
		free(bgcin.MOW.LAI_limit_array); 
		free(bgcin.MOW.transportMOW_array); 
	}

	if (bgcin.GRZ.GRZ_num)
	{
		free(bgcin.GRZ.GRZstart_year_array);  
		free(bgcin.GRZ.GRZstart_month_array); 
		free(bgcin.GRZ.GRZstart_day_array); 
		free(bgcin.GRZ.GRZend_year_array);  
		free(bgcin.GRZ.GRZend_month_array); 
		free(bgcin.GRZ.GRZend_day_array); 
		free(bgcin.GRZ.trampling_effect); 
		free(bgcin.GRZ.weight_LSU); 
		free(bgcin.GRZ.stocking_rate_array); 
		free(bgcin.GRZ.DMintake_array);
		free(bgcin.GRZ.prop_DMintake2excr_array); 
		free(bgcin.GRZ.prop_excr2litter_array); 
		free(bgcin.GRZ.DM_Ccontent_array); 
		free(bgcin.GRZ.EXCR_Ncontent_array);
		free(bgcin.GRZ.EXCR_Ccontent_array); 
		free(bgcin.GRZ.Nexrate); 
		free(bgcin.GRZ.EFman_N2O); 
		free(bgcin.GRZ.EFman_CH4);
		free(bgcin.GRZ.EFfer_CH4);
	}

	if (bgcin.HRV.HRV_num)
	{
		free(bgcin.HRV.HRVyear_array);  
		free(bgcin.HRV.HRVmonth_array); 
		free(bgcin.HRV.HRVday_array); 
		free(bgcin.HRV.snagprop_array); 
		free(bgcin.HRV.transportHRV_array); 
	}

	if (bgcin.PLG.PLG_num)
	{
		free(bgcin.PLG.PLGyear_array);  
		free(bgcin.PLG.PLGmonth_array); 
		free(bgcin.PLG.PLGday_array); 
		free(bgcin.PLG.PLGdepths_array); 
	}

	if (bgcin.FRZ.FRZ_num)
	{
		free(bgcin.FRZ.FRZyear_array);  
		free(bgcin.FRZ.FRZmonth_array); 
		free(bgcin.FRZ.FRZday_array); 
		free(bgcin.FRZ.FRZdepth_array); 
		free(bgcin.FRZ.fertilizer_array); 
		free(bgcin.FRZ.DM_array); 
		free(bgcin.FRZ.NO3content_array); 
		free(bgcin.FRZ.NH4content_array); 
		free(bgcin.FRZ.UREAcontent_array); 
		free(bgcin.FRZ.orgCcontent_array); 
		free(bgcin.FRZ.orgNcontent_array);
		free(bgcin.FRZ.litr_flab_array); 
		free(bgcin.FRZ.litr_fcel_array); 
		free(bgcin.FRZ.EFfert_N2O);
	}

	if (bgcin.IRG.IRG_num)
	{
		free(bgcin.IRG.IRGyear_array);  
		free(bgcin.IRG.IRGmonth_array); 
		free(bgcin.IRG.IRGday_array); 
		free(bgcin.IRG.IRGquantity_array); 
	}
	if (bgcin.co2.varco2) free(bgcin.co2.co2ppm_array);
	if (bgcin.co2.varco2) free(bgcin.co2.co2yrs_array);
	if (bgcin.ndep.varndep) free(bgcin.ndep.Ndep_array);
	if (bgcin.ndep.varndep) free(bgcin.ndep.Nyrs_array);

	if (bgcin.ctrl.varSGS_flag) free(bgcin.epc.sgs_array);
	if (bgcin.ctrl.varEGS_flag) free(bgcin.epc.egs_array);
	if (bgcin.ctrl.varWPM_flag) free(bgcin.epc.wpm_array);
	if (bgcin.ctrl.varMSC_flag) free(bgcin.epc.msc_array);
	if (bgcin.ctrl.GWD_flag)    free(bgcin.sitec.gwd_array);

	if (output.ndayout != 0) free(output.daycodes);
	if (output.ndayout != 0) free(output.daynames);
	if (output.nannout != 0) free(output.anncodes);
	if (output.nannout != 0) free(output.annnames);
	
	/* close files */
	if (restart.read_restart) fclose(restart.in_restart.ptr);
	if (restart.write_restart) fclose(restart.out_restart.ptr);
	if (output.dodaily) 
	{
		fclose(output.dayout.ptr);
		if (transient) fclose(output.dayoutT.ptr);
	}
	if (output.domonavg) 
	{
		fclose(output.monavgout.ptr);
		if (transient) fclose(output.monavgoutT.ptr);
	}
	if (output.doannavg) 
	{
		fclose(output.annavgout.ptr);
		if (transient) fclose(output.annavgoutT.ptr);
	}
	if (output.doannual) 
	{
		fclose(output.annout.ptr);
		if (transient) fclose(output.annoutT.ptr);
	}

	fclose(output.log_file.ptr);

	return (errflag); 

    /* end of main */	
 } 
	
