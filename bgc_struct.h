   /*
bgc_struct.h 
header file for structure definitions

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Biome-BGCMuSo v7.0.
Original code: Copyright 2000, Peter E. Thornton
Numerical Terradynamic Simulation Group, The University of Montana, USA
Modified code: Copyright 2018, D. Hidy [dori.hidy@gmail.com]
Hungarian Academy of Sciences, Hungary
See the website of Biome-BGCMuSo at http://nimbus.elte.hu/bbgc/ for documentation, model executable and example input files.
*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
*/

#define N_POOLS 3			    /*  number of type of pools: water, carbon, nitrogen */
#define N_MGMDAYS 7		        /*  number of type of management events in a single year */
#define N_SOILLAYERS 10		    /*  number of type of soil layers in multilayer soil module */
#define N_SOILLAYERS_GWC 12		/*  number of type of soil layers in multilayer soil module (in case of GW-calculation) */
#define N_PHENPHASES 7		    /*  number of phenological phases */
#define nDAYS_OF_YEAR 365       /* number of days in a year */

/* VAR ctrl: simulation control variables */
typedef struct
{
    int simyears;				/* (n) years of simulation */
    int simstartyear;			/* (year) first year of simulation */
	int spinup;					/* (flag) 1=spinup run, 0=normal run */
	int maxspincycles;			/* (n) maximum number of spinup cycles for spinup run */
	int dodaily;				/* (flag)  for daily output */
	int domonavg;				/* (flag)  for monthly average of daily outputs */
	int doannavg;				/* (flag)  for annual average of daily outputs */
	int doannual;				/* (flag)  for annual output */
	int ndayout;				/* (n) number of daily outputs */
	int nannout;				/* (n) number of annual outputs */
	int* daycodes;				/* (flag)  ARRAY of indices for daily outputs */
	char** daynames;			/* (flag)  ARRAY of names for daily outputs */
	int* anncodes;				/* (flag)  ARRAY of indices for annual outputs */
	char** annnames;			/* (flag)  ARRAY of names for annual outputs */
	int read_restart;			/* (flag)  to read restart file */
	int write_restart;			/* (flag) to write restart file */
	int onscreen;				/* (flag) 1=show progress on-screen 0=don't */
	int GSI_flag;				/* (flag) for using GSI index to calculate growing season */
	int condMOW_flag;			/* (flag) for conditional mowing management option */
	int condIRG_flag;			/* (flag) for conditional irrigating management option */

	int simyr;					/* (n) counter for simulation years */
	int plantyr;                /* (n) counter for planting years (natur.veg=simyr; agroecosys=PLT_num */
	int yday;					/* (n) counter for simulation days of year  */
	int curmonth;				/* (n) index of current month */
	int metday;                 /* (n) counter for days of the whole simulation  */
	int spinyears;				/* (n) counter for spinup years  */
	int day;					/* (n) number of the day in a month */
	int month;					/* (n) number of the month in a year  */
	int varFM_flag;			    /* (flag) for changing FM value */
	int varWPM_flag;			/* (flag) for changing WPM value */
	int varMSC_flag;			/* (flag) for changing MSC value */
	int varSGS_flag;			/* (flag) for changing WPM value */
	int varEGS_flag;			/* (flag) for changing WPM value */
	
	int limitEVP_flag;         /* (flag) for warnings into logfile */	
	int limitTRP_flag;       /* (flag) for warnings into logfile */	
	int limitMR_flag;           /* (flag) for warnings into logfile */	
	int limitSNSC_flag;         /* (flag) for warnings into logfile */	
	int limitleach_flag;        /* (flag) for warnings into logfile */	
	int limitdiffus_flag;       /* (flag) for warnings into logfile */	
	int pond_flag;              /* (flag) for warnings into logfile */	
	int noTRP_flag;          /* (flag) for warnings into logfile */	
	int noMR_flag;              /* (flag) for warnings into logfile */	
	int grazingW_flag;          /* (flag) for warnings into logfile */	
	int condMOWerr_flag;        /* (flag) for warnings into logfile */	
	int condIRGerr_flag;        /* (flag) for warnings into logfile */	
	int prephen1_flag;          /* (flag) for warnings into logfile */
	int prephen2_flag;          /* (flag) for warnings into logfile */	
	int bareground_flag;        /* (flag) for warnings into logfile */	
	int vegper_flag;            /* (flag) for warnings into logfile */	
	int allocControl_flag;       /* (flag) for warnings into logfile */	
	int south_shift;            /* (int) shifting of meteo data for southern hemisphere */
	char* planttypeName;        /* (string) name of the plant tpye in the header of EPC file */
	int NaddSPINUP_flag;        /* (flag) for using artificial N-addition during spinup phase */
	int soiltype;               /* (flag) soiltype */

} control_struct;
/* endVAR */

/* VAR ndep: a structure to hold information about varied N-deposition scenario */
typedef struct
{
	int varndep;            /* (flag) 1=use file Ndep, 0=constant Ndep */
	double ndep;			/* (kgN/m2/yr) wet+dry atmospheric deposition of N */
	double NdepNH4_coeff;	/*  (prop) proprortion of NH4 flux of N-deposition */
	double* Ndep_array;	    /* (kgN/m2/yr) ARRAY of annual Ndep */
	int* Nyrs_array;		/* (year)  ARRAY of years of annual CO2 concentration */
} NdepControl_struct;
/* endVAR */

/*VAR co2: a structure to hold information on the annual co2 concentration */
typedef struct
{
	int varco2;             /* (flag) 0=const 1=use file  */
    double co2ppm;          /* (ppm)  constant CO2 concentration */
	double* co2ppm_array;   /* (ppm)  ARRAY of annual CO2 concentration */
	int* co2yrs_array;   /* (year)  ARRAY of years of annual CO2 concentration */
} co2control_struct;	
/* endVAR */

/* VAR phenarr: phenological control arrays */
typedef struct
{
	int** onday_arr;			/* (doy) ARRAY of first day of transfer period [year;yearday]*/
	int** offday_arr;		    /* (doy) ARRAY of last day of transfer period [year;yearday]*/
	double** Tmin_index;
	double** vpd_index;
	double** dayl_index;
	double** gsi_indexAVG;
	double** heatsum_index;
	double** heatsum;
} phenarray_struct;
/* endVAR */

/* OUT phen: daily phenological data  */
typedef struct
{  
	double remdays_curgrowth;		/* (n) days left in current growth season */
	double remdays_transfer;		/* (n) number of transfer days remaining */
	double remdays_litfall;			/* (n) number of litfall days remaining */
	double predays_transfer;		/* (n) number of transfer days previous */
	double predays_litfall;			/* (n) number of litfall days previous */
	double n_growthday;				/* (n) number of growing days in actual simulation year */
	double n_transferday;			/* (n) number of transfer days in actual simulation year */
	double n_litfallday;			/* (n) number of litterfall days in actual simulation year */
	double yday_total;				/* (n) counter for simdays of the whole simulation */
	double phpsl_dev_rate;			/* (dimless) relative development rate of photoslowing effect */
	double vern_dev_rate;			/* (dimless) relative development rate of vernalization */
	double vern_days;				/* (n) number of vernalization days */
	double GDD_limit;				/* (Celsius) lower limit of GDD in the given phen.phase */
	double GDD_crit[N_PHENPHASES];	/* (Celsius) critical GDD at the beginning of phen.phases */
	double GDD_emergSTART;	        /* (Celsius) start of emergence period */
	double GDD_emergEND;	        /* (Celsius) end of emergence period */
	double onday;					/* (doy) first day of vegetation period */
	double offday;					/* (doy) last day of vegetation period */
	double ondayANN;               /* (doy) annual value for onday */
	double offdayANN;				/* (doy) annual value for offday */
	double planttype;				/* (dimless) type of the plant (fallow:0, maize:1, wheat:2, barley:3, sunflower:4, canola:5, grass:6, unknown:-1) */
	double Tmin_index;
	double vpd_index;
	double dayl_index;
	double gsi_indexAVG;
	double heatsum_index;
	double heatsum;
} phenology_struct;
/* endOUT  */
/* VAR metarr: meteorological variable arrays */
/* inputs from mtclim, except Tavg11, Tavg30, TavgRA30 and TavgRA11
which are used for an 11-day running average of daily average air T,
computed for the whole length of the met ARRAY prior to the 
daily model loop */
typedef struct
{
    double* Tmax_array;          /* (Celsius) daily maximum air temperature */
    double* Tmin_array;          /* (Celsius) daily minimum air temperature */
    double* prcp_array;          /* (cm)    precipitation */
    double* vpd_array;           /* (Pa)    vapor pressure deficit */
    double* swavgfd_array;    /* (W/m2)  daylight avg shortwave flux density */
    double* par_array;           /* (W/m2)  photosynthetically active radiation */
    double* dayl_array;          /* (s)     daylength */
	double* Tday_array;          /* (Celsius) daylight average temperature */
	double* Tavg_array;          /* (Celsius) daily average temperature */
    double* TavgRA11_array;      /* (Celsius) 11-day running avg of daily avg temp */
	double* TavgRA10_array;      /* (Celsius) 10-day running avg of daily avg temp */
    double* TavgRA30_array;      /* (Celsius) 30-day running avg of daily avg temp */
	double* tempradF_array;	     /* (dimless) soil temperature factor of radtiation and air temperature  */
	double* tempradFra_array;    /* (dimless) 5-day running avg soil temperature factor of radtiation and air temperature  */
	double* annTavg_array;       /* (Celsius) annual average temperature */
	double* monTavg_array;       /* (Celsius) monthly average temperature */
	double* annTrange_array;     /* (Celsius) annual average temperature range (difference between Tavg of the coldest and warmest month) */
	double* annTavgRA_array;     /* (Celsius) annual average temperature */
	double* annTrangeRA_array;   /* (Celsius) annual average temperature range (difference between Tavg of the coldest and warmest month) */
} metarr_struct;
/* endVAR */

/* OUT metv: daily values that are passed to daily model subroutines */
typedef struct
{
	double prcp;							 /* (kg/m2) precipitation - INPUT */
	double Tmax;							 /* (Celsius) daily maximum air temperature - INPUT */
	double Tmin;							 /* (Celsius) daily minimum air temperature - INPUT */
	double Tavg;							 /* (Celsius) daily average air temperature - INPUT */
	double Tday;							 /* (Celsius) daylight average air temperature - INPUT */
	double par;								 /* (W/m2)  photosynthetically active radiation - INPUT */
	double vpd;								 /* (Pa)    vapor pressure deficit - INPUT */
	double co2;								 /* (ppm)   atmospheric concentration of CO2 - INPUT */
	double dayl;							 /* (s)     daylength - INPUT */
	double tACCLIMpre;						 /* (Celsius) acclimation temperature  on the previous day  */
	double tACCLIM;							 /* (Celsius) acclimation temperature */
	double tnight;							 /* (Celsius) nightime average air temperature */
	double TavgRA11;						 /* (Celsius)  11-day running average air temperature (linear weighted) */
	double TavgRA10;						 /* (Celsius)  10-day running average air temperature */
	double TavgRA30;						 /* (Celsius)  30-day running average air temperature */
	double tempradF;				         /* (dimless) soil temperature factor of radiation and air temperature  */
	double tempradFra;				     /* (dimless) 5-day running avg soil temperature factor  */
	double tsoil_surface;				   	 /* (Celsius)  surface temperature  */
	double tsoil_surface_pre;			     /* (Celsius)  daily soil surface temperature of the previous day */
	double tsoil_avg;						 /* (Celsius)  average soil temperature */
	double tsoil[N_SOILLAYERS];				 /* (Celsius)  daily soil layer temperature */
	double swRADnet;						 /* (W/m2) net short-wave radiation */
	double lwRADnet;						 /* (W/m2) net outgoing long-wave-radation */
	double RADnet;                           /* (W/m2) daylight average net radiation flux */
	double RADnet_per_plaisun;				 /* (W/m2) daylight average net radiation flux per unit sunlit proj LAI */
	double RADnet_per_plaishade;			 /* (W/m2) daylight average net radiation flux per unit sunshade proj LAI */
	double swavgfd;							 /* (W/m2) daylight average shortwave flux */
	double swabs;							 /* (W/m2) canopy absorbed shortwave flux */ 
	double swtrans;							 /* (W/m2) transmitted shortwave flux */
	double swabs_per_plaisun;				 /* (W/m2) swabs per unit sunlit proj LAI */
	double swabs_per_plaishade;				 /* (W/m2) swabs per unit shaded proj LAI */
	double ppfd_per_plaisun;				 /* (umol/m2/s) ppfd per unit sunlit proj LAI */
	double ppfd_per_plaishade;				 /* (umol/m2/s) ppfd per unit shaded proj LAI */
	double parabs;							 /* (W/m2)  PAR absorbed by canopy */
	double parabs_plaisun;					 /* (W/m2)  PAR absorbed by sunlit canopy fraction */
	double parabs_plaishade;				 /* (W/m2)  PAR absorbed by shaded canopy fraction */
	double GDD;								 /* (Celsius) growing degree day */
	double GDD_wMOD;						 /* (Celsius) GDD with modification of vern. and photop.eff. */
	double pa;								 /* (Pa)    atmospheric pressure  */
	double annTavg;						     /* (Celsius)  mean annual air temperature  */
	double annTavgRA;						 /* (Celsius)  10-year running average mean annual air temperature  */
	double annTrange;						 /* (Celsius)  mean annual air temperature range */
	double annTrangeRA;						 /* (Celsius)  10-year running average mean annual air temperature range */
} metvar_struct;
/* endOUT */

/* OUT ws: water state variables */
/* dimension: (kgH2O/m2) */
typedef struct
{
	double timestepRichards;           /* (n) number of iterataion step (using Richards-method) */
    double soilw[N_SOILLAYERS];		 /* water stored in the soil layers */
	double soilw_SUM;				 /* water stored in soil */
	double soilw_2m;				 /* water stored in 0-200 cm */
	double soilw_RZ;				 /* water stored in rootzone */
	double soilw_RZ_avail;		     /* water stored in rootzone available for plants */
	double pondw;				    /* water stored on surface because of saturation */
    double snoww;					 /* water stored in snowpack */
    double canopyw;					 /* water stored on canopy */
    double prcp_src;				 /* SUM of precipitation */
    double soilEVP_snk;			 /* SUM of soil water evaporation */
    double snowSUBL_snk;			 /* SUM of snow water sublimation */
    double canopywEVP_snk;			 /* SUM of canopy water evaporation */
    double TRP_snk;				 /* SUM of transpiration */
	double runoff_snk;			 	 /* SUM of runoff */
	double pondEVP_snk;			 /* SUM of pond output */
	double deeppercolation_snk;		 /* SUM of percolated water out of the system */
	double groundwater_src;			 /* SUM of water plus from goundwater */
	double groundwater_snk;			 /* SUM of water loss to goundwater */
	double canopyw_THNsnk;			 /* SUM of water stored on canopy is disappered because of thinning*/
	double canopyw_MOWsnk;		     /* SUM of water stored on canopy is disappered because of mowing */
	double canopyw_HRVsnk;           /* SUM of water stored on canopy is disappered because of harvesting */
	double canopyw_PLGsnk;		     /* SUM of water stored on canopy is disappered because of ploughing */
	double canopyw_GRZsnk;			 /* SUM of water stored on canopy is disappered because of grazing */
    double IRGsrc_W;			     /* SUM of water from irrigating */
	double condIRGsrc;				 /* sum of conditional irrigatied water amount in a year*/	
	double FRZsrc_W;				 /* SUM of water from fertilization */
	double FLDsrc;					 /* SUM of water from flooding */
	double cumEVPsoil1;              /* cumulated soil evaporation in first evaporation phase (no limit) */
	double cumEVPsoil2;              /* cumulated soil evaporation in second evaporation phase (DSR limit) */
	double soilw_avail[N_SOILLAYERS];/* transpiration lack in a given layer */
	double GW_waterlogging;          /* amount of water above the surface (negative GWD data) */
	double cumGWchange;              /* cumulative water change in soil column due to groundwater */
    double WbalanceERR;              /* SUM of water balance error  */
	double inW;						 /* SUM of nitrogen input */
	double outW;					 /* SUM of nitrogen output */
	double storeW;					 /* SUM of nitrogen store */
} wstate_struct;                        
/* endOUT */

/* OUT wf: water flux variables */
/* dimension: (kgH2O/m2/d) */
typedef struct
{
    double prcp_to_canopyw;							/* interception on canopy */
    double prcp_to_soilSurface;							/* precip entering soilwater pool  */
    double prcp_to_snoww;							/* snowpack accumulation */
	double prcp_to_runoff;						    /* Hortonian runoff flux */
	double pondw_to_runoff;						    /* Dunnian runoff flux */
    double canopywEVP;							    /* evaporation from canopy */
    double canopyw_to_soilw;						/* canopy drip and stemflow  */
	double pondwEVP;                                 /* pond water evaporation  */
	double surfaceEVP;                              /* SUM of soilw and pond water evaporation  */
	double snowwSUBL;								/* sublimation from snowpack */
	double potSUBLsnow;                             /* potential sublimation of snow */
    double snoww_to_soilw;							/* melt from snowpack  */
    double soilwEVP;								/* evaporation from soil */
	double potEVPsurface;                           /* potential evaporation (soilw and pondw) */
	double potETcanopy;                             /* potential canopy evaporation and transpiration (canopyw) */
	double potEVPandSUBLsurface;                    /* potential evaporation (soilw and pondw) + sublimation  */
	double ET_Elimit;                                /* energy limit of evaporation */
	double soilwTRP_POT;					        /* potential transpiration (no SWC-limit) */
	double soilwTRPdemand[N_SOILLAYERS];		    /* transpiration demand from the soil layers */
    double soilwTRP[N_SOILLAYERS];				    /* transpiration from the soil layers */
    double soilwTRP_SUM;	                        /* SUM of transpiration from the soil layers */
	double ET;								        /* evapotranspiration (EVP+TRP+subl) */
	double PET;								        /* potential evapotranspiration (sum of potEVPsurface+potETcanopy) */
	double pondw_to_soilw;                          /* water flux from pond to soil */
	double soilw_to_pondw;                          /* water flux from soil to pond */
	double infilt_to_soilw;                         /* infiltration flux from prcp to soilw */
	double prcp_to_pondw;                          /* infiltration flux from prcp to pondw */ 
	double GW_to_pondw;                             /* water flux from groundwater to pondw */
	double soilwFlux[N_SOILLAYERS];			        /* net sum of percolation+diffusion flux between the soil layers */
	double GWdischarge[N_SOILLAYERS];				/* soil water plus from groundwater */
	double GWrecharge[N_SOILLAYERS];			    /* recharge: soil water to groundwater */
	double GWmovchange[N_SOILLAYERS];			    /* soil water change due to the movement of GW */
	double FLD_to_soilw;                            /* water flux from flooding to pondw */
	double FLD_to_pondw;                            /* water flux from flooding to soilw */
	double soilwLeach_RZ;				         	/* soil water leached from rootzone (percol+diffus) in gH2O/m2/d */
	double canopyw_to_THN;							/* water stored on canopy is disappered because of thinning */
	double canopyw_to_MOW;							/* water stored on canopy is disappered because of mowing */
	double canopyw_to_HRV;							/* water stored on canopy is disappered because of harvesting */
	double canopyw_to_PLG;							/* water stored on canopy is disappered because of ploughing */
	double canopyw_to_GRZ;							/* water stored on canopy is disappered because of grazing */
	double IRG_to_prcp;								/* irrigatied water amount (above canopy)*/	
	double IRG_to_soilSurface;						/* irrigatied water amount (into the soil - drain)*/	
	double IRG_to_soilw;							/* irrigatied water amount (into the soil - drain)*/	
	double FRZ_to_soilw;                            /* water flux from fertilization */
	double waterFromAbove;                          /* water flux from above: prcp_to_soilSurface+snoww_to_soilw+canopyw_to_soilw+IRG_to_prcp OR pondw */
	double infiltPOT;                               /* potential infiltraion water flux from above */
} wflux_struct;
/* endOUT */

/* VAR cinit: carbon state initialization structure  */
typedef struct
{
	double max_leafc;			/* (kgC/m2) first-year displayed + stored leafc */
	double max_frootc;			/* (kgC/m2) first-year displayed + stored froot */
	double max_yieldc;			/* (kgC/m2) first-year displayed + stored yield */
	double max_softstemc;		/* (kgC/m2) first-year displayed + stored softstem */
	double max_livestemc;		/* (kgC/m2) first-year displayed + stored live woody stem */
	double max_livecrootc;		/* (kgC/m2) first-year displayed + stored live coarse root */
} cinit_struct;
/* endVAR */

/* OUT cs: carbon state variables */
/* dimension: (kgC/m2)  */
typedef struct 
{
	double leafcSUM_phenphase[N_PHENPHASES];  	/* sum of leafC in a given phenophase */
    double leafc;								/* leaf C actual pool */
    double leafc_storage;						/* leaf C storage */
    double leafc_transfer;						/* leaf C transfer */
    double frootc;								/* fine root C actual pool*/
    double frootc_storage;						/* fine root C storage */
    double frootc_transfer;						/* fine root C transfer */
	double yieldc;             					/* yield actual pool */
    double yieldc_storage;     					/* yield storage pool */
    double yieldc_transfer;    					/* yield transfer pool */
	double softstemc;          					/* softstemc actual poo */
    double softstemc_storage;  					/* softstemc storage pool */
    double softstemc_transfer; 					/* softstemc transfer pool */
    double livestemc;		   					/* live stem C */
    double livestemc_storage;  					/* live stem C storage */
    double livestemc_transfer; 					/* live stem C transfer */
    double deadstemc;          					/* dead stem C */
    double deadstemc_storage;  					/* dead stem C storage */
    double deadstemc_transfer; 					/* dead stem C transfer */
    double livecrootc;         					/* live coarse root C */
    double livecrootc_storage; 					/* live coarse root C storage */
    double livecrootc_transfer; 				/* live coarse root C transfer */
    double deadcrootc;          				/* dead coarse root C */
    double deadcrootc_storage;  				/* dead coarse root C storage */
    double deadcrootc_transfer; 				/* dead coarse root C transfer */
	double gresp_storage;       				/* growth respiration storage */
	double gresp_transfer;      				/* growth respiration transfer */
	double nsc_w;                               /* non-stuctured woody carbohydrate pool */
	double nsc_nw;                              /* non-stuctured non-woody carbohydrate pool */
	double sc_w;                                /* stuctured woody carbohydrate pool */
	double sc_nw;                               /* stuctured non-woody carbohydrate pool */
    double cwdc[N_SOILLAYERS];					/* coarse woody debris N */
    double litr1c[N_SOILLAYERS];				/* litter labile C */
    double litr2c[N_SOILLAYERS];				/* litter unshielded cellulose C */
    double litr3c[N_SOILLAYERS];				/* litter shielded cellulose C */
    double litr4c[N_SOILLAYERS];				/* litter lignin C */
	double litrC[N_SOILLAYERS];					/* litter total C */
	double litr1c_total;						/* litter labile C */
    double litr2c_total;            			/* litter unshielded cellulose C */
    double litr3c_total;            			/* litter shielded cellulose C */
    double litr4c_total;            			/* litter lignin C */
	double cwdc_total;              			/* coarse woody debris C */
	double cwdc_above;              			/* aboveground coarse woody debris C */
	double litrc_above;              			/* aboveground litter C */
	double STDBc_leaf;							/*  wilted leaf biomass  */
	double STDBc_froot;							/*  wilted froot biomass  */
	double STDBc_yield;							/*  wilted yield biomass  */
	double STDBc_softstem;						/*  wilted sofstem biomass  */
	double STDBc_above;             			/*  wilted aboveground plant biomass  */
	double STDBc_below;             			/*  wilted belowground plant biomass  */
	double CTDBc_leaf;							/*  cut-down leaf biomass  */
	double CTDBc_froot;							/*  cut-down froot biomass  */
	double CTDBc_yield;							/*  cut-down yield biomass  */
	double CTDBc_softstem;						/*  cut-down sofstem biomass  */
	double CTDBc_cstem;			    			/*  cut-down coarse stem biomass  */
	double CTDBc_croot;			    			/*  cut-down coarse root biomass  */
	double CTDBc_above;             			/*  cut-down aboveground plant biomass  */
	double CTDBc_below;             			/*  cut-down belowground plant biomass  */
    double soil1c[N_SOILLAYERS];		        /* carbon content of labile SOM */
    double soil2c[N_SOILLAYERS];				/* carbon content of fast decomposing SOM */
    double soil3c[N_SOILLAYERS];				/* carbon content of slowly decomposing SOM */
    double soil4c[N_SOILLAYERS];				/* carbon content of stable SOM */
	double soilC[N_SOILLAYERS];					/* soil total C */
	double soil1DOC[N_SOILLAYERS];		/* dissolved part of carbon content of labile SOM */
    double soil2DOC[N_SOILLAYERS];		/* dissolved part of carbon content of fast decomposing SOM */
    double soil3DOC[N_SOILLAYERS];		/* dissolved part of carbon content of slowly decomposing SOM */
    double soil4DOC[N_SOILLAYERS];		/* dissolved part of carbon content of stable SOM */
	double soilDOC[N_SOILLAYERS];	    /* dissolved part of soil total C */
	double soil1c_total;				/* SUM of carbon content of labile SOM */
    double soil2c_total;				/* SUM of carbon content of fast decomposing SOM */
    double soil3c_total;				/* SUM of carbon content of slowly decomposing SOM */
    double soil4c_total;				/* SUM of carbon content of stable SOM */
	double cpool;						/* temporary photosynthate C pool */
    double psnsun_src;					/* SUM of gross PSN from sulit canopy */
    double psnshade_src;				/* SUM of gross PSN from shaded canopy */
	double NSC_MR_snk;                  /* SUM of non-structured carobhydrate MR loss */
	double actC_MR_snk;                 /* SUM of actual pool's MR loss */
    double leaf_MR_snk;					/* SUM of leaf maint resp */
    double froot_MR_snk;				/* SUM of fine root maint resp */
	double yield_MR_snk;				/* SUM of yield maint resp.*/
	double softstem_MR_snk;				/* SUM of softstem maint resp.*/
	double livestem_MR_snk;				/* SUM of live stem maint resp */
	double livecroot_MR_snk;			/* SUM of live coarse root maint resp */
	double leaf_GR_snk;					/* SUM of leaf growth resp */
    double froot_GR_snk;				/* SUM of fine root growth resp */
	double yield_GR_snk;				/* SUM of yield growth resp. */
	double softstem_GR_snk;				/* SUM of softstem growth resp. */
    double livestem_GR_snk;				/* SUM of live stem growth resp */
	double livecroot_GR_snk;			/* SUM of live coarse root growth resp */
    double deadstem_GR_snk;				/* SUM of dead stem growth resp */ 
    double deadcroot_GR_snk;			/* SUM of dead coarse root growth resp */
    double litr1_hr_snk;				/* SUM of labile litr microbial resp */
    double litr2_hr_snk;				/* SUM of cellulose litr microbial resp */
    double litr4_hr_snk;				/* SUM of lignin litr microbial resp */
    double soil1_hr_snk;				/* SUM of labile SOM respiration */
    double soil2_hr_snk;				/* SUM of fast SOM respiration */
    double soil3_hr_snk;				/* SUM of slow SOM respiration */
    double soil4_hr_snk;				/* SUM of stable SOM respiration */
	double calc_flowHS;                 /* SUM of C loss due to flower heat stress to calc.flowHS */
	double flowHSsnk_C;                 /* SUM of C loss due to flower heat stress (sink/annout variable) */
	double FIREsnk_C;					/* SUM of fire losses */
	double SNSCsnk_C;					/* SUM of senescence losses */
    double PLTsrc_C;					/* SUM of C content of planted plant material */
	double MULsrc_C;					/* SUM of C content of mulched material */
	double CWEsnk_C;                    /* SUM of C content of CWD-extract */
	double Cdeepleach_snk;				/* SUM of C deep leaching */
	double THN_transportC;				/* SUM of C content of thinned and transported plant material*/
	double HRV_transportC;				/* SUM of C content of harvested and transported plant material*/
	double MOW_transportC;              /* SUM of mowed and transported plant material (C content)  */
	double GRZsnk_C;					/* SUM of C content of grazed leaf */
	double GRZsrc_C;					/* SUM of added carbon from fertilizer */
	double FRZsrc_C;					/* SUM of carbon content of fertilizer return to the litter pool */
	double yieldC_HRV;                  /* SUM of carbon content of havested yield */
	double frootC_HRV;                  /* SUM of carbon content of fine root at harvest */
	double vegC_HRV;                    /* SUM of carbon content of havested leaf, stem and yield */
	double CbalanceERR;					/* SUM of carbon balance error */
	double CNratioERR;                  /* SUM of carbon-nitrogen ratio error */
	double inC;							/* SUM of carbon input */
	double outC;						/* SUM of carbon output */
	double storeC;						/* SUM of carbon store */
} cstate_struct;
/* endOUT */

/* OUT cf: daily carbon flux variables:  */
/* dimension: (kgC/m2/d)   */
typedef struct
{
	/* group: mortality fluxes */ 
	double m_leafc_to_litr1c;                        
	double m_leafc_to_litr2c;              
	double m_leafc_to_litr3c;              
	double m_leafc_to_litr4c;              
	double m_frootc_to_litr1c;             
	double m_frootc_to_litr2c;             
	double m_frootc_to_litr3c;             
	double m_frootc_to_litr4c;  
	double m_yieldc_to_litr1c;             
	double m_yieldc_to_litr2c;             
	double m_yieldc_to_litr3c;             
	double m_yieldc_to_litr4c;       
	double m_softstemc_to_litr1c;            
	double m_softstemc_to_litr2c;            
	double m_softstemc_to_litr3c;            
	double m_softstemc_to_litr4c;  
	double m_leafc_storage_to_litr1c;      
	double m_frootc_storage_to_litr1c;
	double m_softstemc_storage_to_litr1c;   
	double m_yieldc_storage_to_litr1c;     
	double m_livestemc_storage_to_litr1c;  
	double m_deadstemc_storage_to_litr1c;  
	double m_livecrootc_storage_to_litr1c; 
	double m_deadcrootc_storage_to_litr1c; 
	double m_leafc_transfer_to_litr1c;     
	double m_frootc_transfer_to_litr1c;
	double m_yieldc_transfer_to_litr1c;  
	double m_softstemc_transfer_to_litr1c;   
	double m_livestemc_transfer_to_litr1c; 
	double m_deadstemc_transfer_to_litr1c; 
	double m_livecrootc_transfer_to_litr1c;
	double m_deadcrootc_transfer_to_litr1c;
	double m_livestemc_to_cwdc;            
	double m_deadstemc_to_cwdc;            
	double m_livecrootc_to_cwdc;           
	double m_deadcrootc_to_cwdc;           
	double m_gresp_storage_to_litr1c;      
	double m_gresp_transfer_to_litr1c;        
	/* group: fire fluxes */
	double m_leafc_to_fire;                
	double m_frootc_to_fire;               
	double m_yieldc_to_fire;               
	double m_softstemc_to_fire; 
	double m_STDBc_to_fire;
	double m_CTDBc_to_fire;
	double m_leafc_storage_to_fire;        
	double m_frootc_storage_to_fire;       
	double m_yieldc_storage_to_fire;       
	double m_softstemc_storage_to_fire;    
	double m_livestemc_storage_to_fire;    
	double m_deadstemc_storage_to_fire;    
	double m_livecrootc_storage_to_fire;   
	double m_deadcrootc_storage_to_fire;   
	double m_leafc_transfer_to_fire;       
	double m_frootc_transfer_to_fire;      
	double m_yieldc_transfer_to_fire;      
	double m_softstemc_transfer_to_fire;   
	double m_livestemc_transfer_to_fire;   
	double m_deadstemc_transfer_to_fire;   
	double m_livecrootc_transfer_to_fire;  
	double m_deadcrootc_transfer_to_fire;  
	double m_livestemc_to_fire;            
	double m_deadstemc_to_fire;            
	double m_livecrootc_to_fire;           
	double m_deadcrootc_to_fire;           
	double m_gresp_storage_to_fire;        
	double m_gresp_transfer_to_fire;       
	double m_litr1c_to_fire_total;               
	double m_litr2c_to_fire_total;               
	double m_litr3c_to_fire_total;               
	double m_litr4c_to_fire_total;               
	double m_cwdc_to_fire_total;    
	double m_litr1c_to_fire[N_SOILLAYERS];               
	double m_litr2c_to_fire[N_SOILLAYERS];               
	double m_litr3c_to_fire[N_SOILLAYERS];               
	double m_litr4c_to_fire[N_SOILLAYERS];               
	double m_cwdc_to_fire[N_SOILLAYERS];    
	/* group: senescence fluxes */
	double m_vegc_to_SNSC;                
	double m_leafc_to_SNSC; 
	double m_leafc_to_SNSCgenprog;
	double m_frootc_to_SNSC;               
	double m_yieldc_to_SNSC;                
	double m_softstemc_to_SNSC;                
	double m_leafc_storage_to_SNSC;                
	double m_frootc_storage_to_SNSC;               
	double m_leafc_transfer_to_SNSC;                
	double m_frootc_transfer_to_SNSC;               
	double m_yieldc_storage_to_SNSC;        
	double m_yieldc_transfer_to_SNSC;       
	double m_softstemc_storage_to_SNSC;        
	double m_softstemc_transfer_to_SNSC;       
	double m_gresp_storage_to_SNSC;
	double m_gresp_transfer_to_SNSC;
	/* group: harvesting senescence fluxes */
	double HRV_leafc_storage_to_SNSC;               
	double HRV_leafc_transfer_to_SNSC;    
	double HRV_yieldc_storage_to_SNSC;               
	double HRV_yieldc_transfer_to_SNSC;    
	double HRV_frootc_to_SNSC;                           
	double HRV_softstemc_to_SNSC;                 
	double HRV_frootc_storage_to_SNSC;               
	double HRV_frootc_transfer_to_SNSC;               
	double HRV_softstemc_storage_to_SNSC;        
	double HRV_softstemc_transfer_to_SNSC;       
	double HRV_gresp_storage_to_SNSC;
	double HRV_gresp_transfer_to_SNSC;
	/* group: CWD-extract */
	double cwdc0_to_CWE;
	double cwdc1_to_CWE;
	double cwdc2_to_CWE;
	/* group: flowering heat stress  */
	double yieldc_to_flowHS;
	/* group: standing dead biomass to litter fluxes */
	double STDBc_leaf_to_litr;
	double STDBc_froot_to_litr;
	double STDBc_yield_to_litr;
	double STDBc_softstem_to_litr;	
	double STDBc_to_litr;
	/* group: cut-dowc dead biomass to litter pool */
	double CTDBc_leaf_to_litr;
	double CTDBc_froot_to_litr;
	double CTDBc_yield_to_litr;
	double CTDBc_softstem_to_litr;	
	double CTDBc_cstem_to_cwd;			   
	double CTDBc_croot_to_cwd;			    
	double CTDBc_to_litr;  
	/* group: phenology fluxes from transfer pool */
	double leafc_transfer_to_leafc;          
	double frootc_transfer_to_frootc;        
	double yieldc_transfer_to_yield;
	double softstemc_transfer_to_softstemc;
	double livestemc_transfer_to_livestemc;  
	double deadstemc_transfer_to_deadstemc;  
	double livecrootc_transfer_to_livecrootc;
	double deadcrootc_transfer_to_deadcrootc;
	/* group: leaf and fine root litterfall */
	double leafc_to_litr1c;              
	double leafc_to_litr2c;              
	double leafc_to_litr3c;              
	double leafc_to_litr4c;              
	double frootc_to_litr1c;             
	double frootc_to_litr2c;             
	double frootc_to_litr3c;             
	double frootc_to_litr4c;             
	double yieldc_to_litr1c;
	double yieldc_to_litr2c;
	double yieldc_to_litr3c;
	double yieldc_to_litr4c;
	double softstemc_to_litr1c;
	double softstemc_to_litr2c;
	double softstemc_to_litr3c;
	double softstemc_to_litr4c;
	/* group: maintenance respiration fluxes */
	double leaf_day_MR;                  
	double leaf_night_MR;                
	double froot_MR;
	double yield_MR;   	
	double softstem_MR;
	double livestem_MR;                  
	double livecroot_MR;                 
	/* group: photosynthesis fluxes */
	double psnsun_to_cpool;              
	double psnshade_to_cpool;      
	/* group: litter decomposition fluxes  */
	double cwdc_to_litr2c[N_SOILLAYERS];	
	double cwdc_to_litr3c[N_SOILLAYERS];	
	double cwdc_to_litr4c[N_SOILLAYERS];	
	double litr1_hr[N_SOILLAYERS];	
	double litr2_hr[N_SOILLAYERS];		
	double litr4_hr[N_SOILLAYERS];	
	double soil1_hr[N_SOILLAYERS];		
	double soil2_hr[N_SOILLAYERS];
	double soil3_hr[N_SOILLAYERS];		
	double soil4_hr[N_SOILLAYERS];	
	double litr1_hr_total;					
	double litr2_hr_total;	
	double litr4_hr_total;		
	double soil1_hr_total;					
	double soil2_hr_total;	
	double soil3_hr_total;	
	double soil4_hr_total;		
	double litr1c_to_soil1c[N_SOILLAYERS];  
	double litr2c_to_soil2c[N_SOILLAYERS];  
	double litr3c_to_litr2c[N_SOILLAYERS];  
	double litr4c_to_soil3c[N_SOILLAYERS]; 
	double soil1c_to_soil2c[N_SOILLAYERS];			
	double soil2c_to_soil3c[N_SOILLAYERS];	
	double soil3c_to_soil4c[N_SOILLAYERS];	
			
	double litr1c_to_release[N_SOILLAYERS];
	double litr2c_to_release[N_SOILLAYERS];
	double litr4c_to_release[N_SOILLAYERS];
	
	double litrc_to_release_total;
	double litrc_to_soilc_total;											
	double cwdc_to_litrc_total;		
	
	/* group: SOIL components leaching*/
	double soil1DOC_leach[N_SOILLAYERS]; 
	double soil2DOC_leach[N_SOILLAYERS]; 
	double soil3DOC_leach[N_SOILLAYERS]; 
	double soil4DOC_leach[N_SOILLAYERS]; 
	double soilDOC_leachCUM[N_SOILLAYERS];
	double DOC_leachRZ;
	/* group: daily allocation fluxes from current GPP */
	double cpool_to_leafc;               
	double cpool_to_leafc_storage;       
	double cpool_to_frootc;              
	double cpool_to_frootc_storage;    
	double cpool_to_yield;                
	double cpool_to_yieldc_storage;        
	double cpool_to_softstemc;           
	double cpool_to_softstemc_storage; 
	double cpool_to_livestemc;           
	double cpool_to_livestemc_storage;   
	double cpool_to_deadstemc;           
	double cpool_to_deadstemc_storage;   
	double cpool_to_livecrootc;          
	double cpool_to_livecrootc_storage;  
	double cpool_to_deadcrootc;          
	double cpool_to_deadcrootc_storage;  
	double cpool_to_gresp_storage;       
	/* group: daily growth respiration fluxes */
	double cpool_leaf_GR;                
	double cpool_leaf_storage_GR;        
	double transfer_leaf_GR;             
	double cpool_froot_GR;               
	double cpool_froot_storage_GR;       
	double transfer_froot_GR;   
	double cpool_yield_GR;                 
	double cpool_yieldc_storage_GR;         
	double transfer_yield_GR;             
	double cpool_softstem_GR;                 
	double cpool_softstem_storage_GR;         
	double transfer_softstem_GR;             
	double cpool_livestem_GR;            
	double cpool_livestem_storage_GR;    
	double transfer_livestem_GR;         
	double cpool_deadstem_GR;            
	double cpool_deadstem_storage_GR;    
	double transfer_deadstem_GR;         
	double cpool_livecroot_GR;           
	double cpool_livecroot_storage_GR;   
	double transfer_livecroot_GR;        
	double cpool_deadcroot_GR;           
	double cpool_deadcroot_storage_GR;   
	double transfer_deadcroot_GR;   
	/* group: daily maintanance respiration ensuring fluxes */
	double leafc_storage_to_maintresp;
	double frootc_storage_to_maintresp;
	double yieldc_storage_to_maintresp;
	double softstemc_storage_to_maintresp;
	double livestemc_storage_to_maintresp;
	double livecrootc_storage_to_maintresp;
	double deadstemc_storage_to_maintresp;
	double deadcrootc_storage_to_maintresp;
	double leafc_transfer_to_maintresp;
	double frootc_transfer_to_maintresp;
	double yieldc_transfer_to_maintresp;
	double softstemc_transfer_to_maintresp;
	double livestemc_transfer_to_maintresp;
	double livecrootc_transfer_to_maintresp;
	double deadstemc_transfer_to_maintresp;
	double deadcrootc_transfer_to_maintresp;
	double leafc_to_maintresp;
	double frootc_to_maintresp;
	double yieldc_to_maintresp;
	double softstemc_to_maintresp;
	double livestemc_to_maintresp;
	double livecrootc_to_maintresp;
	double NSC_nw_to_maintresp;
	double actC_nw_to_maintresp;
	double NSC_w_to_maintresp;
	double actC_w_to_maintresp;
	/* group: annual turnover of storage to transfer pools */
	double leafc_storage_to_leafc_transfer;           
	double frootc_storage_to_frootc_transfer;     
    double yieldc_storage_to_yieldc_transfer;   
	double softstemc_storage_to_softstemc_transfer;   
	double livestemc_storage_to_livestemc_transfer;    
	double deadstemc_storage_to_deadstemc_transfer;    
	double livecrootc_storage_to_livecrootc_transfer; 
	double deadcrootc_storage_to_deadcrootc_transfer; 
	double gresp_storage_to_gresp_transfer;           
	/* group: turnover of live wood to dead wood */
	double livestemc_to_deadstemc;        
	double livecrootc_to_deadcrootc;      
	/* group: planting fluxes */
	double leafc_transfer_from_PLT;		
	double frootc_transfer_from_PLT;	
	double yieldc_transfer_from_PLT;		
	double softstemc_transfer_from_PLT;		
	double STDBc_leaf_to_PLT;				 			 
	double STDBc_yield_to_PLT;
	double STDBc_froot_to_PLT;
	double STDBc_softstem_to_PLT;	
	/* group: thinning fluxes */
	double leafc_to_THN;				    
	double leafc_storage_to_THN;            
	double leafc_transfer_to_THN;
	double frootc_to_THN;				    
	double frootc_storage_to_THN;            
	double frootc_transfer_to_THN;  
	double yieldc_to_THN;				 
	double yieldc_storage_to_THN;         
	double yieldc_transfer_to_THN;        
	double livestemc_to_THN;				 
	double livestemc_storage_to_THN;         
	double livestemc_transfer_to_THN;        
	double deadstemc_to_THN;				 
	double deadstemc_storage_to_THN;         
	double deadstemc_transfer_to_THN;     
	double livecrootc_to_THN;				 
	double livecrootc_storage_to_THN;         
	double livecrootc_transfer_to_THN;        
	double deadcrootc_to_THN;				 
	double deadcrootc_storage_to_THN;         
	double deadcrootc_transfer_to_THN;      
	double gresp_storage_to_THN;         
	double gresp_transfer_to_THN;        
	double THN_to_CTDBc_leaf;		
	double THN_to_CTDBc_froot;	
	double THN_to_CTDBc_yield;				 			 
	double THN_to_CTDBc_cstem;		
	double THN_to_CTDBc_croot;	
	double STDBc_leaf_to_THN;	
	double STDBc_froot_to_THN;
	double STDBc_yield_to_THN;				 
	/* group: mowing fluxes */
	double leafc_to_MOW;				 
	double leafc_storage_to_MOW;         
	double leafc_transfer_to_MOW;        
	double yieldc_to_MOW;				 
	double yieldc_storage_to_MOW;         
	double yieldc_transfer_to_MOW;        
	double softstemc_to_MOW;				 
	double softstemc_storage_to_MOW;         
	double softstemc_transfer_to_MOW;        
	double gresp_storage_to_MOW;         
	double gresp_transfer_to_MOW;        
	double MOW_to_CTDBc_leaf;				 		 
	double MOW_to_CTDBc_yield;
	double MOW_to_CTDBc_softstem;				 		 			    	 
	double STDBc_leaf_to_MOW;				 			 
	double STDBc_yield_to_MOW;	
	double STDBc_softstem_to_MOW;		
	/* group: harvesting fluxes */
	double leafc_to_HRV;				 
	double leafc_storage_to_HRV;         
	double leafc_transfer_to_HRV;        
	double yieldc_to_HRV;				 
	double yieldc_storage_to_HRV;         
	double yieldc_transfer_to_HRV;        
	double softstemc_to_HRV;				 
	double softstemc_storage_to_HRV;         
	double softstemc_transfer_to_HRV;        
	double gresp_storage_to_HRV;         
	double gresp_transfer_to_HRV;        
	double HRV_to_CTDBc_leaf;				 			 
	double HRV_to_CTDBc_yield;
	double HRV_to_CTDBc_softstem;				 		 			    	 
	double STDBc_leaf_to_HRV;				 			 
	double STDBc_yield_to_HRV;	
	double STDBc_softstem_to_HRV;		
	/* group: ploughing fluxes */		 
	double leafc_to_PLG;					 
	double leafc_storage_to_PLG;			
	double leafc_transfer_to_PLG;	
	double frootc_to_PLG;					
	double frootc_storage_to_PLG;         
	double frootc_transfer_to_PLG;  
	double yieldc_to_PLG;					
	double yieldc_storage_to_PLG;			
	double yieldc_transfer_to_PLG;			
	double softstemc_to_PLG;				 
	double softstemc_storage_to_PLG;         
	double softstemc_transfer_to_PLG;        
	double gresp_storage_to_PLG;          
	double gresp_transfer_to_PLG;         
	double STDBc_leaf_to_PLG;
	double STDBc_froot_to_PLG;
	double STDBc_yield_to_PLG;
	double STDBc_softstem_to_PLG;
	double CTDBc_leaf_to_PLG;
	double CTDBc_yield_to_PLG;
	double CTDBc_softstem_to_PLG;
	/* group: grazing fluxes */	
	double leafc_to_GRZ;					
	double leafc_storage_to_GRZ;         
	double leafc_transfer_to_GRZ;      
	double yieldc_to_GRZ;				 
	double yieldc_storage_to_GRZ;         
	double yieldc_transfer_to_GRZ;        
	double softstemc_to_GRZ;				 
	double softstemc_storage_to_GRZ;         
	double softstemc_transfer_to_GRZ;        
	double gresp_storage_to_GRZ;         
	double gresp_transfer_to_GRZ;        
	double STDBc_leaf_to_GRZ;
	double STDBc_yield_to_GRZ;
	double STDBc_softstem_to_GRZ;
	double GRZ_to_litr1c;				 
	double GRZ_to_litr2c;				 
	double GRZ_to_litr3c;				 
	double GRZ_to_litr4c;
	/* group: fertilizing fluxes */
	double FRZ_to_litr1c;				 
	double FRZ_to_litr2c;				 
	double FRZ_to_litr3c;				 
	double FRZ_to_litr4c;	
	/* group: mulching struct */
	double litrc_from_MUL;
	double cwdc_from_MUL;
	/* group: CH4 flux based on empirical estimation */
	double CH4flux_soil;			
	double CH4flux_manure;			
	double CH4flux_animal;

} cflux_struct;
/* endOUT */

/* OUT ns: nitrogen state variables */ 
/* dimension: (kgN/m2)   */
typedef struct
{
    double leafn;					/* leaf N actual pool */
    double leafn_storage;			/* leaf N storage pool */
    double leafn_transfer;			/* leaf N transfer pool */
    double frootn;					/* fine root N actual pool */ 
    double frootn_storage;			/* fine root N storage pool */ 
    double frootn_transfer;			/* fine root N transfer pool */ 
	double yieldn;					/* yield N actual pool */
    double yieldn_storage;			/* yield N storage pool */
    double yieldn_transfer;			/* yield N transfer pool */
	double softstemn;				/* softstem N actual pool */
    double softstemn_storage;		/* softstem N storage pool */
    double softstemn_transfer;		/* softstem N transfer pool */
    double livestemn;				/* live stem N actual pool */
    double livestemn_storage;		/* live stem N storage pool */
    double livestemn_transfer;		/* live stem N transfer pool */
    double deadstemn;				/* dead stem N actual pool */
    double deadstemn_storage;		/* dead stem N storage pool */
    double deadstemn_transfer;		/* dead stem N transfer pool */
    double livecrootn;				/* live coarse root N actual pool */
    double livecrootn_storage;		/* live coarse root N storage pool */
    double livecrootn_transfer;		/* live coarse root N transfer pool */
    double deadcrootn;				/* dead coarse root N actual pool */
    double deadcrootn_storage;		/* dead coarse root N storage pool */
    double deadcrootn_transfer;		/* dead coarse root N transfer pool */
	double npool;					/* temporary plant N pool */
    double cwdn[N_SOILLAYERS];      /* coarse woody debris N */
    double litr1n[N_SOILLAYERS];    /* litter labile N */
    double litr2n[N_SOILLAYERS];    /* litter unshielded cellulose N */
    double litr3n[N_SOILLAYERS];    /* litter shielded cellulose N */
    double litr4n[N_SOILLAYERS];    /* litter lignin N */
	double litrN[N_SOILLAYERS];	    /* litter total N */
	double litr1n_total;            /* litter labile N */
    double litr2n_total;            /* litter unshielded cellulose N */
    double litr3n_total;            /* litter shielded cellulose N */
    double litr4n_total;            /* litter lignin N */
	double cwdn_total;				/* coarse woody debris N */
	double STDBn_leaf;				/*  wilted leaf biomass  */
	double STDBn_froot;				/*  wilted froot biomass  */
	double STDBn_yield;				/*  wilted yield biomass  */
	double STDBn_softstem;			/*  wilted sofstem biomass  */
	double STDBn_above;             /*  wilted aboveground plant biomass  */
	double STDBn_below;             /*  wilted belowground plant biomass  */
	double CTDBn_leaf;				/*  cut-down leaf biomass  */
	double CTDBn_froot;				/*  cut-down froot biomass  */
	double CTDBn_yield;				/*  cut-down yield biomass  */
	double CTDBn_softstem;			/*  cut-down sofstem biomass  */
	double CTDBn_cstem;			    /*  cut-down coarse stem biomass  */
	double CTDBn_croot;			    /*  cut-down coarse root biomass  */
	double CTDBn_above;             /*  cut-down aboveground plant biomass  */
	double CTDBn_below;             /*  cut-down belowground plant biomass  */
    double soil1n[N_SOILLAYERS];		/* Nitrogen content of labile SOM */
    double soil2n[N_SOILLAYERS];		/* Nitrogen content of fast decomposing SOM */
    double soil3n[N_SOILLAYERS];		/* Nitrogen content of slowly decomposing SOM */
    double soil4n[N_SOILLAYERS];		/* Nitrogen content of stable SOM */
	double soilN[N_SOILLAYERS];	        /* Nitrogen content of total SOM */
	double soil1DON[N_SOILLAYERS];		/* Dissolved Nitrogen content of labile SOM */
    double soil2DON[N_SOILLAYERS];		/* Dissolved Nitrogen content of fast decomposing SOM */
    double soil3DON[N_SOILLAYERS];		/* Dissolved Nitrogen content of slowly decomposing SOM */
    double soil4DON[N_SOILLAYERS];		/* Dissolved Nitrogen content of stable SOM */
	double soilDON[N_SOILLAYERS];	    /* soil total N */
	double soil1n_total;				/* SUM of Nitrogen content of labile SOM */
    double soil2n_total;				/* SUM of Nitrogen content of fast decomposing SOM */
    double soil3n_total;				/* SUM of Nitrogen content of slowly decomposing SOM */
    double soil4n_total;				/* SUM of Nitrogen content of total SOM */
	double retransn;					/* plant pool of retranslocated N */
    double sminNH4[N_SOILLAYERS];		/* soil mineral N in multilayer soil */
	double sminNO3[N_SOILLAYERS];		/* soil mineral N in multilayer soil */
	double sminNH4_total;               /* SUM of the soil mineral N in the total soil */
	double sminNO3_total;               /* SUM of the soil mineral N in the total soil */
	double sminNH4avail[N_SOILLAYERS];	/* Available soil mineral N in multilayer soil */
	double sminNO3avail[N_SOILLAYERS];	/* Available soil mineral N in multilayer soil */
	double sminNH4avail_total;          /* SUM of the available soil mineral N in the total soil */
	double sminNO3avail_total;          /* SUM of the available soil mineral N in the total soil */
	double sminNavail_RZ;
    double Nfix_src;					/* SUM of biological N fixation */
    double Ndep_src;					/* SUM of N deposition inputs */
    double Ndeepleach_snk;				/* SUM of N deep leaching */
    double Nvol_snk;					/* SUM of N lost to volatilization */
	double Nprec_snk;					/* SUM of N lost to precision control */
	double FIREsnk_N;					/* SUM of N lost to fire */
	double SNSCsnk_N;					/* SUM of senescence losses */
	double FRZsrc_N;					/* SUM of N fertilization inputs */	
    double PLTsrc_N;					/* SUM of planted leaf N */
	double MULsrc_N;					/* SUM of C content of mulched material */
	double THN_transportN; 				/* SUM of thinned and transported plant material (N content)  */
	double CWEsnk_N;                    /* SUM of N content of CWD-extract */
	double HRV_transportN; 				/* SUM of harvested and transported plant material (N content)  */
	double MOW_transportN;              /* SUM of mowed and transported plant material (N content)  */
	double GRZsnk_N;					/* SUM of grazed leaf N */
	double GRZsrc_N;					/* SUM of leaf N from fertilizer*/
	double SPINUPsrc;					/* SUM of leaf N from spinup correction*/
    double NbalanceERR;                 /* SUM of nitrogen balance error */
	double inN;							/* SUM of nitrogen input */
	double outN;						/* SUM of nitrogen output */
	double storeN;						/* SUM of nitrogen store */
} nstate_struct;
/* endOUT */

/* OUT nf: daily nitrogen flux variables */
/* dimension: (kgN/m2/d)   */
typedef struct
{
	/* group: mortality fluxes */
	double m_leafn_to_litr1n;              
	double m_leafn_to_litr2n;              
	double m_leafn_to_litr3n;              
	double m_leafn_to_litr4n;              
	double m_frootn_to_litr1n;             
	double m_frootn_to_litr2n;             
	double m_frootn_to_litr3n;             
	double m_frootn_to_litr4n; 		
	double m_yieldn_to_litr1n;              
	double m_yieldn_to_litr2n;              
	double m_yieldn_to_litr3n;              
	double m_yieldn_to_litr4n;              
	double m_softstemn_to_litr1n;          
	double m_softstemn_to_litr2n;          
	double m_softstemn_to_litr3n;          
	double m_softstemn_to_litr4n;      
	double m_leafn_storage_to_litr1n;      
	double m_frootn_storage_to_litr1n; 
    double m_yieldn_storage_to_litr1n;      
	double m_yieldn_transfer_to_litr1n;      
	double m_softstemn_storage_to_litr1n;      
	double m_softstemn_transfer_to_litr1n;
	double m_livestemn_storage_to_litr1n;  
	double m_deadstemn_storage_to_litr1n;  
	double m_livecrootn_storage_to_litr1n; 
	double m_deadcrootn_storage_to_litr1n; 
	double m_leafn_transfer_to_litr1n;     
	double m_frootn_transfer_to_litr1n;    
	double m_livestemn_transfer_to_litr1n; 
	double m_deadstemn_transfer_to_litr1n; 
	double m_livecrootn_transfer_to_litr1n;
	double m_deadcrootn_transfer_to_litr1n;
    double m_livestemn_to_litr1n;          
	double m_livestemn_to_cwdn;            
	double m_deadstemn_to_cwdn;            
	double m_livecrootn_to_litr1n;         
	double m_livecrootn_to_cwdn;           
	double m_deadcrootn_to_cwdn;           
	double m_retransn_to_litr1n;              
	/* group: senescence fluxes */
	double m_vegn_to_SNSC;                
	double m_leafn_to_SNSC;  
	double m_leafn_to_SNSCgenprog; 
	double m_frootn_to_SNSC;               
	double m_leafn_storage_to_SNSC;		   
	double m_frootn_storage_to_SNSC;       
	double m_leafn_transfer_to_SNSC;       
	double m_frootn_transfer_to_SNSC;     
	double m_yieldn_to_SNSC;                
	double m_yieldn_storage_to_SNSC;		   
	double m_yieldn_transfer_to_SNSC;       
	double m_softstemn_to_SNSC;                
	double m_softstemn_storage_to_SNSC;		   
	double m_softstemn_transfer_to_SNSC; 
	double m_retransn_to_SNSC;
	double SNSC_to_retrans;
	double leafSNSCgenprog_to_retrans;
	double leafSNSC_to_retrans;
	double frootSNSC_to_retrans;
	double yieldSNSC_to_retrans;
	double softstemSNSC_to_retrans;
	double leaf_transferSNSC_to_retrans;
	double froot_transferSNSC_to_retrans;
	double yieldc_transferSNSC_to_retrans;
	double softstem_transferSNSC_to_retrans;
	double leaf_storageSNSC_to_retrans;
	double froot_storageSNSC_to_retrans;
	double yieldc_storageSNSC_to_retrans;
	double softstem_storageSNSC_to_retrans;
	/* group: harvesting senescence fluxes */
	double HRV_leafn_storage_to_SNSC;               
	double HRV_leafn_transfer_to_SNSC;    
	double HRV_yieldn_storage_to_SNSC;               
	double HRV_yieldn_transfer_to_SNSC;    
	double HRV_frootn_to_SNSC;                           
	double HRV_softstemn_to_SNSC;                 
	double HRV_frootn_storage_to_SNSC;               
	double HRV_frootn_transfer_to_SNSC;               
	double HRV_softstemn_storage_to_SNSC;        
	double HRV_softstemn_transfer_to_SNSC;       
	double HRV_retransn_to_SNSC;
	/* group: flowering heat stress  */
	double yieldn_to_flowHS;
	/* group: standing dead biomass to litter pool */
	double STDBn_leaf_to_litr;
	double STDBn_froot_to_litr;
	double STDBn_yield_to_litr;
	double STDBn_softstem_to_litr;	

	double STDBn_to_litr;
	/* group: cut-down dead biomass to litter pool */
	double CTDBn_leaf_to_litr;
	double CTDBn_froot_to_litr;
	double CTDBn_yield_to_litr;
	double CTDBn_softstem_to_litr;	
	double CTDBn_cstem_to_cwd;			   
	double CTDBn_croot_to_cwd;			    
	double CTDBn_to_litr;  
	/* group: fire fluxes */
	double m_leafn_to_fire;                
	double m_frootn_to_fire;    
	double m_yieldn_to_fire;    
	double m_softstemn_to_fire; 
	double m_STDBn_to_fire;
	double m_CTDBn_to_fire;
	double m_leafn_storage_to_fire;        
	double m_frootn_storage_to_fire; 
	double m_yieldn_storage_to_fire;		   
	double m_yieldn_transfer_to_fire;              
	double m_softstemn_storage_to_fire;		   
	double m_softstemn_transfer_to_fire; 
	double m_livestemn_storage_to_fire;    
	double m_deadstemn_storage_to_fire;    
	double m_livecrootn_storage_to_fire;   
	double m_deadcrootn_storage_to_fire;   
	double m_leafn_transfer_to_fire;       
	double m_frootn_transfer_to_fire;      
	double m_livestemn_transfer_to_fire;   
	double m_deadstemn_transfer_to_fire;   
	double m_livecrootn_transfer_to_fire;  
	double m_deadcrootn_transfer_to_fire;  
	double m_livestemn_to_fire;            
	double m_deadstemn_to_fire;            
	double m_livecrootn_to_fire;           
	double m_deadcrootn_to_fire;           
	double m_retransn_to_fire;             
	double m_litr1n_to_fire_total;               
	double m_litr2n_to_fire_total;               
	double m_litr3n_to_fire_total;               
	double m_litr4n_to_fire_total;               
	double m_cwdn_to_fire_total;    
	double m_litr1n_to_fire[N_SOILLAYERS];               
	double m_litr2n_to_fire[N_SOILLAYERS];               
	double m_litr3n_to_fire[N_SOILLAYERS];               
	double m_litr4n_to_fire[N_SOILLAYERS];               
	double m_cwdn_to_fire[N_SOILLAYERS];                 
	/* group: phenology fluxes from transfer pool */
	double leafn_transfer_to_leafn;           
	double frootn_transfer_to_frootn;    
	double yieldn_transfer_to_yieldn;         
	double softstemn_transfer_to_softstemn; 
	double livestemn_transfer_to_livestemn;   
	double deadstemn_transfer_to_deadstemn;   
	double livecrootn_transfer_to_livecrootn; 
	double deadcrootn_transfer_to_deadcrootn;        
	/* group: litterfall fluxes */
	double leafn_to_litr1n;               
	double leafn_to_litr2n;                
	double leafn_to_litr3n;               
	double leafn_to_litr4n;               
	double leafn_to_retransn;             
	double frootn_to_litr1n;              
	double frootn_to_litr2n;              
	double frootn_to_litr3n;              
	double frootn_to_litr4n;              
	double yieldn_to_litr1n;               
	double yieldn_to_litr2n;                
	double yieldn_to_litr3n;               
	double yieldn_to_litr4n;               
	double softstemn_to_litr1n;               
	double softstemn_to_litr2n;                
	double softstemn_to_litr3n;               
	double softstemn_to_litr4n;               
	/* group: N input flux */
	double ndep_to_sminNH4[N_SOILLAYERS];      
	double ndep_to_sminNO3[N_SOILLAYERS];  
	double nfix_to_sminNH4[N_SOILLAYERS]; 
	double ndep_to_sminn_total;                 
	double nfix_to_sminn_total;    
	/* group: litter and soil decomposition fluxes  */
	double cwdn_to_litr2n[N_SOILLAYERS];                
	double cwdn_to_litr3n[N_SOILLAYERS];                
	double cwdn_to_litr4n[N_SOILLAYERS];                
	double litr1n_to_soil1n[N_SOILLAYERS];              
	double litr2n_to_soil2n[N_SOILLAYERS];              
	double litr3n_to_litr2n[N_SOILLAYERS];              
	double litr4n_to_soil3n[N_SOILLAYERS]; 
	double soil1n_to_soil2n[N_SOILLAYERS];              
	double soil2n_to_soil3n[N_SOILLAYERS];              
	double soil3n_to_soil4n[N_SOILLAYERS];                
	
	double litr1n_to_release[N_SOILLAYERS];
	double litr2n_to_release[N_SOILLAYERS];
	double litr4n_to_release[N_SOILLAYERS];
	double litrn_to_release_total;

	/* group: mineralization-immobilization fluxes  */
	double sminn_to_soil_SUM[N_SOILLAYERS];             
	double sminNH4_to_soil_SUM[N_SOILLAYERS];           
	double sminNO3_to_soil_SUM[N_SOILLAYERS];  

	double sminn_to_soil1n_l1[N_SOILLAYERS];            
	double sminn_to_soil2n_l2[N_SOILLAYERS];            
	double sminn_to_soil3n_l4[N_SOILLAYERS];            
	double sminn_to_soil2n_s1[N_SOILLAYERS];            
	double sminn_to_soil3n_s2[N_SOILLAYERS];            
	double sminn_to_soil4n_s3[N_SOILLAYERS]; 
	

	/* grup: summary variables */
	double soil4n_to_sminn[N_SOILLAYERS];    
	double soiln_to_sminn[N_SOILLAYERS];
	double litrn_to_sminn[N_SOILLAYERS];
	double environment_to_sminn[N_SOILLAYERS];
              
	double sminn_to_soil_LtoS[N_SOILLAYERS]; 
	double sminn_to_soil_StoS[N_SOILLAYERS]; 
	
	double cwdn_to_litrn_total;  
	double soil4n_to_sminn_total; 
	double litrn_to_soiln_total;
	double sminn_to_soil_SUM_total; 

	double litrn_to_sminn_total;
	double soiln_to_sminn_total;
	double sminn_to_soil_LtoS_total; 
	double sminn_to_soil_StoS_total; 
	double environment_to_sminn_total;
	double soiln_to_sminn_totalCUM;
	double litrn_to_sminn_totalCUM;
	double sminn_to_soil_LtoS_totalCUM; 
	double sminn_to_soil_StoS_totalCUM; 
	double environment_to_sminn_totalCUM;
	
	double grossMINERflux[N_SOILLAYERS];			    /* (kgN/m2/d) daily gross N mineralization layer by layer */
	double potIMMOBflux[N_SOILLAYERS];			        /* (kgN/m2/d) daily potential immobilization layer by layer */
	double netMINERflux[N_SOILLAYERS];					/* (kgN/m2/d) daily net N mineralization layer by layer */
	double actIMMOBflux[N_SOILLAYERS];		            /* (kgN/m2/d) daily actual N immobilization layer by layer */ 
	double potIMMOBflux_total;							/* (kgN/m2/d) total daily potential immobilization */
	double grossMINERflux_total;						/* (kgN/m2/d) total daily gross mineralization */
	double netMINERflux_total;							/* (kgN/m2/d) total net N mineralization */
	double actIMMOBflux_total;							/* (kgN/m2/d) total actual N immobilization */ 
	double netMINERflux_totalCUM;						/* (kgN/m2/d) cumulative total net N mineralization */
	double actIMMOBflux_totalCUM;						/* (kgN/m2/d) cumulative total actual N immobilization */ 


	/* group: nitrification and denitrification fluxes */
	double sminNO3_to_denitr[N_SOILLAYERS];
	double sminNH4_to_nitrif[N_SOILLAYERS];
	double N2_flux_DENITR[N_SOILLAYERS];				
	double N2O_flux_NITRIF[N_SOILLAYERS];				
	double N2O_flux_DENITR[N_SOILLAYERS];
	double sminNO3_to_denitr_total;
	double sminNH4_to_nitrif_total;			
	double N2_flux_DENITR_total;				
	double N2O_flux_NITRIF_total;				
	double N2O_flux_DENITR_total;	
	/* group: SMINN change caused by soil process */
	double sminNH4_to_npool[N_SOILLAYERS];                 
	double sminNO3_to_npool[N_SOILLAYERS]; 
	double sminn_to_npool[N_SOILLAYERS];
	double sminNH4_to_npool_total;          
	double sminNO3_to_npool_total;			
	double sminn_to_npool_total;
	double sminn_to_npool_totalCUM;
	/* group: SOIL components leaching*/
	double sminNH4_leach[N_SOILLAYERS];    
	double sminNO3_leach[N_SOILLAYERS];    
	double sminN_leachRZ;
	double soil1DON_leach[N_SOILLAYERS]; 
	double soil2DON_leach[N_SOILLAYERS]; 
	double soil3DON_leach[N_SOILLAYERS]; 
	double soil4DON_leach[N_SOILLAYERS]; 
	double sminNH4_leachCUM[N_SOILLAYERS];    
	double sminNO3_leachCUM[N_SOILLAYERS];
	double soilDON_leachCUM[N_SOILLAYERS]; 
	double DON_leachRZ;
	/* group: daily allocation fluxes */
	double retransn_to_npool[N_SOILLAYERS];   
	double retransn_to_npool_total; 
	double npool_to_leafn;                
	double npool_to_leafn_storage;  
	double npool_to_frootn;               
	double npool_to_frootn_storage;   
	double npool_to_yieldn;                
	double npool_to_yieldn_storage;        
	double npool_to_softstemn;                
	double npool_to_softstemn_storage;        
	double npool_to_livestemn;            
	double npool_to_livestemn_storage;    
	double npool_to_deadstemn;            
	double npool_to_deadstemn_storage;    
	double npool_to_livecrootn;           
	double npool_to_livecrootn_storage;   
	double npool_to_deadcrootn;           
	double npool_to_deadcrootn_storage;  
	/* group: daily maintanance respiration ensuring fluxes - retranslocation */
	double leafn_storage_to_maintresp;
	double frootn_storage_to_maintresp;
	double yieldn_storage_to_maintresp;
	double softstemn_storage_to_maintresp;
	double livestemn_storage_to_maintresp;
	double livecrootn_storage_to_maintresp;
	double deadstemn_storage_to_maintresp;
	double deadcrootn_storage_to_maintresp;
	double leafn_transfer_to_maintresp;
	double frootn_transfer_to_maintresp;
	double yieldn_transfer_to_maintresp;
	double softstemn_transfer_to_maintresp;
	double livestemn_transfer_to_maintresp;
	double livecrootn_transfer_to_maintresp;
	double deadstemn_transfer_to_maintresp;
	double deadcrootn_transfer_to_maintresp;
	double leafn_to_maintresp;
	double frootn_to_maintresp;
	double yieldn_to_maintresp;
	double softstemn_to_maintresp;
	double livestemn_to_maintresp;
	double livecrootn_to_maintresp;
	double NSN_nw_to_maintresp;
	double actN_nw_to_maintresp;
	double NSN_w_to_maintresp;
	double actN_w_to_maintresp;
	/* group: annual turnover of storage to transfer */
	double leafn_storage_to_leafn_transfer;           
	double frootn_storage_to_frootn_transfer;         
	double livestemn_storage_to_livestemn_transfer;   
	double deadstemn_storage_to_deadstemn_transfer;   
	double livecrootn_storage_to_livecrootn_transfer; 
	double deadcrootn_storage_to_deadcrootn_transfer; 
	double yieldn_storage_to_yieldn_transfer;           
	double softstemn_storage_to_softstemn_transfer;           
	/* group: turnover of live wood to dead wood, with retranslocation */
	double livestemn_to_deadstemn;        
	double livestemn_to_retransn;         
	double livecrootn_to_deadcrootn;      
	double livecrootn_to_retransn;        
	/* group: planting fluxes */
	double leafn_transfer_from_PLT;		
	double frootn_transfer_from_PLT;		
	double yieldn_transfer_from_PLT;		
	double softstemn_transfer_from_PLT;	
	double STDBn_leaf_to_PLT;				 			 
	double STDBn_yield_to_PLT;	
	double STDBn_froot_to_PLT;	
	double STDBn_softstem_to_PLT;	
	/* group: thinning fluxes */
	double leafn_to_THN;				 
	double leafn_storage_to_THN;         
	double leafn_transfer_to_THN;       
	double frootn_to_THN;				 
	double frootn_storage_to_THN;         
	double frootn_transfer_to_THN;      
	double yieldn_to_THN;				 
	double yieldn_storage_to_THN;         
	double yieldn_transfer_to_THN;  
	double livestemn_to_THN;				 
	double livestemn_storage_to_THN;         
	double livestemn_transfer_to_THN;        
	double deadstemn_to_THN;				 
	double deadstemn_storage_to_THN;         
	double deadstemn_transfer_to_THN;    
	double livecrootn_to_THN;				 
	double livecrootn_storage_to_THN;         
	double livecrootn_transfer_to_THN;        
	double deadcrootn_to_THN;				 
	double deadcrootn_storage_to_THN;         
	double deadcrootn_transfer_to_THN;    
	double retransn_to_THN;
	double THN_to_CTDBn_leaf;
	double THN_to_CTDBn_froot;
	double THN_to_CTDBn_yield;				  
	double THN_to_CTDBn_cstem;		
	double THN_to_CTDBn_croot;	
	double STDBn_leaf_to_THN;	
	double STDBn_froot_to_THN;	
	double STDBn_yield_to_THN;				 
	/* group: mowing fluxes */
	double leafn_to_MOW;                 
	double leafn_storage_to_MOW;         
	double leafn_transfer_to_MOW;
	double yieldn_to_MOW;				 
	double yieldn_storage_to_MOW;         
	double yieldn_transfer_to_MOW;        
	double softstemn_to_MOW;				 
	double softstemn_storage_to_MOW;         
	double softstemn_transfer_to_MOW; 
	double MOW_to_CTDBn_leaf;				 			 
	double MOW_to_CTDBn_yield;
	double MOW_to_CTDBn_softstem;			 		 			    	 
	double STDBn_leaf_to_MOW;				 			 
	double STDBn_yield_to_MOW;	
	double STDBn_softstem_to_MOW;	
	/* group: harvesting fluxes */
	double leafn_to_HRV;                 
	double leafn_storage_to_HRV;         
	double leafn_transfer_to_HRV;  
	double yieldn_to_HRV;				 
	double yieldn_storage_to_HRV;         
	double yieldn_transfer_to_HRV;        
	double softstemn_to_HRV;				 
	double softstemn_storage_to_HRV;         
	double softstemn_transfer_to_HRV;   
	double retransn_to_HRV;
	double HRV_to_CTDBn_leaf;				 
	double HRV_to_CTDBn_yield;
	double HRV_to_CTDBn_softstem;			 			    	 
	double STDBn_leaf_to_HRV;				 	 
	double STDBn_yield_to_HRV;	
	double STDBn_softstem_to_HRV;	  
	/* group: CWD-extract */
	double cwdn0_to_CWE;
	double cwdn1_to_CWE;
	double cwdn2_to_CWE;
	/* group: ploughing fluxes  */
	double leafn_to_PLG;					 
	double leafn_storage_to_PLG;			
	double leafn_transfer_to_PLG;	
	double frootn_to_PLG;					
	double frootn_storage_to_PLG;         
	double frootn_transfer_to_PLG;  
	double yieldn_to_PLG;					
	double yieldn_storage_to_PLG;			
	double yieldn_transfer_to_PLG;			
	double softstemn_to_PLG;				 
	double softstemn_storage_to_PLG;         
	double softstemn_transfer_to_PLG;        
	double retransn_to_PLG;                 
	double STDBn_leaf_to_PLG;
	double STDBn_froot_to_PLG;
	double STDBn_yield_to_PLG;
	double STDBn_softstem_to_PLG;
	double CTDBn_leaf_to_PLG;
	double CTDBn_yield_to_PLG;
	double CTDBn_softstem_to_PLG;
	/* group: grazing fluxes */
	double leafn_to_GRZ;                 
	double leafn_storage_to_GRZ;         
	double leafn_transfer_to_GRZ; 
	double yieldn_to_GRZ;				 
	double yieldn_storage_to_GRZ;         
	double yieldn_transfer_to_GRZ;        
	double softstemn_to_GRZ;				 
	double softstemn_storage_to_GRZ;         
	double softstemn_transfer_to_GRZ;  
	double STDBn_leaf_to_GRZ;
	double STDBn_yield_to_GRZ;
	double STDBn_softstem_to_GRZ;
	double retransn_to_GRZ;
	double GRZ_to_litr1n;				 
	double GRZ_to_litr2n;				 
	double GRZ_to_litr3n;				 
	double GRZ_to_litr4n;				 
	/* group: fertilizing fluxes */
	double FRZ_to_sminNH4;      
	double FRZ_to_sminNO3; 
	double FRZ_to_litr1n;				 
	double FRZ_to_litr2n;				 
	double FRZ_to_litr3n;				 
	double FRZ_to_litr4n;
	/* group: mulching struct */
	double litrn_from_MUL;
	double cwdn_from_MUL;
	/* group: management N2O flux based on empirical estimation */
	double N2O_flux_GRZ;
	double N2O_flux_FRZ;

} nflux_struct;
/* endOUT */


/* VAR nt: temporary nitrogen variables */
typedef struct 
{
	double mineralized[N_SOILLAYERS];     /* (kgN/m2/d) gross mineralization */
	double potential_immob[N_SOILLAYERS]; /* (kgN/m2/d) gross immobilization */
	double plant_ndemand[N_SOILLAYERS];	  /* (kgN/m2/d) nitrogen demand of plant */	
	double plitr1c_loss[N_SOILLAYERS];    /* (kgN/m2/d) potential loss of labile litter */
	double pmnf_l1s1[N_SOILLAYERS];       /* (kgN/m2/d) mineralized nitrogen flux of labile litter */
	double plitr2c_loss[N_SOILLAYERS];    /* (kgN/m2/d) potential loss of unshielded litter */
	double pmnf_l2s2[N_SOILLAYERS];       /* (kgN/m2/d) mineralized nitrogen flux of cellulose litter */
	double plitr4c_loss[N_SOILLAYERS];	  /* (kgN/m2/d) potential loss of lignin litter */
	double pmnf_l4s3[N_SOILLAYERS];       /* (kgN/m2/d) mineralized nitrogen flux of lignin litter */
	double psoil1c_loss[N_SOILLAYERS];    /* (kgN/m2/d) potential loss of labile SOM pool */
	double pmnf_s1s2[N_SOILLAYERS];       /* (kgN/m2/d) mineralized nitrogen flux of labile SOM pool */
	double psoil2c_loss[N_SOILLAYERS];    /* (kgN/m2/d) potential loss of fast SOM pool */
	double pmnf_s2s3[N_SOILLAYERS];       /* (kgN/m2/d) mineralized nitrogen flux of fast SOM pool */
	double psoil3c_loss[N_SOILLAYERS];    /* (kgN/m2/d) potential loss of slow SOM pool */
	double pmnf_s3s4[N_SOILLAYERS];       /* (kgN/m2/d) mineralized nitrogen flux of slow SOM pool */
	double psoil4c_loss[N_SOILLAYERS];    /* (kgN/m2/d) potential loss of stable SOM pool */
	double pmnf_s4[N_SOILLAYERS];         /* (kgN/m2/d) mineralized nitrogen flux of stable SOM pool */
	double kl4[N_SOILLAYERS];             /* (1/day) corrected rate constants from the rate scalar */
} ntemp_struct;
/* endVAR */
	

/* OUT epv: ecophysiological variables */
typedef struct
{
	int n_rootlayers;							/* (n) actual number of soil layers in which root can be found  */
	int n_maxrootlayers;						/* (n) maximum number of soil layers in which root can be found  */
	int germ_layer;								/* (n) number of germination layer */
	double germDepth;                          /* (m) actual germination depth*/
	double cpool_to_leafcARRAY[nDAYS_OF_YEAR*2];   /* (kgC/m2/day) array of carbon from cpool to leafC */                 
	double npool_to_leafnARRAY[nDAYS_OF_YEAR*2];   /* (kgN/m2/day) array of nitrogen from npool to leafN */
	double thermal_timeARRAY[nDAYS_OF_YEAR*2];	   /* (Celsius) actual thermal_timeSUM */
	int    gpSNSC_phenARRAY[nDAYS_OF_YEAR*2];	   /* (dimless) array of phenophase of genprog senescence */  
	double leafday;                             /* (n) counter array for days of year when leaves are on*/
	double leafday_lastmort;                    /* (n) last day of genetically senescence*/
	double thermal_time;			            /* (Celsius) difference between Tavg and base temperature */
	double n_actphen;							/* (n) number of the actual phenophase (from 1 to 7)*/
	double flowHS_mort;                         /* (prop.) mortality coefficient of flwoering heat stress */
	double transfer_ratio;                      /* (prop) transfer proportion on actual day  */ 
	double day_leafc_litfall_increment;			/* (kgC/m2/d) rate leaf litfall */
	double day_yield_litfall_increment;		/* (kgC/m2/d) rate yield litfall  */
	double day_softstemc_litfall_increment;		/* (kgC/m2/d) rate softstem litfall  */
	double day_frootc_litfall_increment;		/* (kgC/m2/d) rate froot litfall */
	double day_livestemc_turnover_increment;	/* (kgC/m2/d) rate livestem turnover */
	double day_livecrootc_turnover_increment;	/* (kgC/m2/d) rate livecroot turnover */
	double annmax_leafc;						/* (kgC/m2) annual maximum daily leaf C */
	double annmax_yieldc;						/* (kgC/m2) annual maximum daily yield C  */
	double annmax_softstemc;					/* (kgC/m2) annual maximum daily softstem C */
	double annmax_frootc;						/* (kgC/m2) annual maximum daily froot C */
	double annmax_livestemc;					/* (kgC/m2) annual maximum daily livestem C */
	double annmax_livecrootc;					/* (kgC/m2) annual maximum daily livecroot C */
	double DSR;									/* (n) number of days since rain, for soil EVP */
	double SWCstressLENGTH;                     /* (dimless) lenght of the soil water stress */
	double cumSWCstress;						/* (dimless) cumulative water stress */
	double cumNstress;							/* (dimless) cumulative nitrogen stress */
    double proj_lai;							/* (m2/m2) live projected leaf area index */
    double all_lai;								/* (m2/m2) live all-sided leaf area index */
	double sla_avg;                             /* (m2/m2) canopy average proj. SLA */
    double plaisun;								/* (m2/m2) sunlit projected leaf area index */
    double plaishade;							/* (m2/m2) shaded projected leaf area index */
    double sun_proj_sla;						/* (m2/kgC) sunlit projected SLA */
    double shade_proj_sla;						/* (m2/kgC) shaded projected SLA */
	double plantHeight;                        /* (m) height of plant (based on stemw and LAI)*/
	double NDVI;                                /* (ratio) normalized difference vegetation index */
	double rootlengthProp[N_SOILLAYERS];		    /* (prop) proportion of root lenght in the given soil layer  */
	double rootlengthLandD_prop[N_SOILLAYERS];		/* (prop) proportion of dead+live root lenght in the given soil layer  */
	double PSI[N_SOILLAYERS];						/* (MPa) water potential of soil and leaves   */
	double pF[N_SOILLAYERS];						/* (cm) soil water suction derived from log(soil water potential)  */
	double hydrCONDUCTsat_avg;			            /* (m/s) averaged hydraulic conductivity on the actual day in the rooting zone  */
	double hydrCONDUCTact[N_SOILLAYERS];			/* (m/s) hydraulic conductivity on the actual day  */
	double hydrDIFFUSact[N_SOILLAYERS];				/* (m2/s) hydraulic diffusivity on the actual day    */
	double VWCsat_RZ;								/* (m3/m3) average value of VWC saturation (max.soil.depth) */
	double VWCfc_RZ;								/* (m3/m3) average value of VWC field capacity (max.soil.depth) */
	double VWCwp_RZ;								/* (m3/m3) average value of VWC wilting point (max.soil.depth) */
	double VWChw_RZ;								/* (m3/m3) average value of VWC wilting point (max.soil.depth) */
    double VWC[N_SOILLAYERS];						/* (m3/m3) volumetric water content  */
	double relVWCsat_fc[N_SOILLAYERS];				/* (m3/m3) relative volumetric water content (SAT-FC) */
	double relVWCfc_wp[N_SOILLAYERS];				/* (m3/m3) relative volumetric water content (FC-WP)  */
    double VWC_SScrit1[N_SOILLAYERS];				/* (m3/m3) volumetric water content at start of soil stress */
	double VWC_SScrit2[N_SOILLAYERS];				/* (m3/m3) volumetric water content at full soil stress */
	double WFPS[N_SOILLAYERS];						/* (prop) water filled pore space */
    double VWC_avg;									/* (m3/m3) average volumetric water content in active layers */
	double VWC_maxRZ;									/* (m3/m3) average volumetric water content in max.rootzone (max.soil.depth) */
	double VWC_RZ;									/* (m3/m3) average volumetric water content in rootzone (act.soil.depth) */
	double PSI_RZ;									/* (MPa) average water potential of soil and leaves */
	double rootDepth;			     				/* (m) actual depth of the root and rooting zone */
	double rootlength;			     				/* (m) actual length of the root and rooting zone (rootdpeth - germination depth */
	double dlmr_area_sun;							/* (umolC/m2projected leaf area/s) sunlit leaf MR */
    double dlmr_area_shade;				  			/* (umolC/m2projected leaf area/s) shaded leaf MR */
    double gl_t_wv_sun;								/* (m/s) leaf-scale conductance to transpired water */
    double gl_t_wv_shade;							/* (m/s) leaf-scale conductance to transpired water */
	double gl_t_wv_sunPOT;							/* (m/s) leaf-scale conductance to transpired water without SWC-limitation */
    double gl_t_wv_shadePOT;						/* (m/s) leaf-scale conductance to transpired water without SWC-limitation  */
    double assim_sun;								/* (umol/m2/s) sunlit assimilation per unit pLAI */
    double assim_shade;								/* (umol/m2/s) shaded assimilation per unit pLAI */
	double ws_nitrif[N_SOILLAYERS];					/* (dimless) water filled pore space lscalar function for nitrification */
	double ps_nitrif[N_SOILLAYERS];				    /* (dimless) pH response function for nitrification */
	double ts_nitrif[N_SOILLAYERS];				    /* (dimless) Tsoil scalar function for nitrification */
    double ts_decomp[N_SOILLAYERS];					/* (dimless) decomp temperature scalar  */
    double ws_decomp[N_SOILLAYERS];					/* (dimless) decomp water scalar  */
    double rs_decomp[N_SOILLAYERS];					/* (dimless) decomp combined scalar  */
	double rs_decomp_avg;							/* (dimless) decomp combined and averaged scalar  */

	double stomaCONDUCT_max;						/* (m/s) maximum stomatal conductance */
	double m_Tmin;									/* (dimless) freezing night temperature multiplier */
	double m_SWCstress_layer[N_SOILLAYERS];		    /* (dimless) soil water stress multiplier */
	double m_SWCstress;							    /* (dimless) soil water stress  multiplier */
	double m_ppfd_sun;								/* (dimless) PAR flux density multiplier */
	double m_ppfd_shade;							/* (dimless) PAR flux density multiplier */
	double m_vpd;									/* (dimless) vapor pressure deficit multiplier */
	double m_final_sun;								/* (dimless) product of all other multipliers */
	double m_final_shade;							/* (dimless) product of all other multipliers */
	double m_SWCstressLENGTH;						/* (dimless) soil water stress length multiplier */
	double m_extremT;								/* (dimless) extem temp. multiplier */
	double m_co2;                                   /* (dimless) CO2 concentration multiplier */
	double assim_Tcoeff;                            /* (dimless) Tmax limitation factor of photosynthesis */
	double assim_SScoeff;                           /* (dimless) Soilstress limitation factor of photosynthesis */
	double SMSI;									/* (prop.) soil moisture stress index */
	double gcorr;									/* (dimless) temperature and pressure correction factor for conductances */
	double gl_bl;									/* (m/s) leaf boundary layer conductance */
    double gl_c;									/* (m/s) leaf cuticular conductance */
    double gl_s_sun;								/* (m/s) leaf-scale stomatal conductance */
    double gl_s_shade;								/* (m/s) leaf-scale stomatal conductance */
	double gl_e_wv;									/* (m/s) leaf conductance to evaporated water */
	double gl_sh;									/* (m/s) leaf conductance to sensible heat */
    double gc_e_wv;									/* (m/s) canopy conductance to evaporated water */
    double gc_sh;									/* (m/s) canopy conductance to sensible heat */
	double annmax_lai;								/* (m2/m2) year-to-date maximum projected LAI */
	double annmax_rootDepth;						/* (m) year-to-date maximum projected rooting depth */
	double annmax_plantHeight;	                    /* (m2) year-to-date maximum projected plant height */
	
	double IMMOBratio[N_SOILLAYERS];				/* (prop) ratio of actual and potential immobilization layer by layer */
	double plantCalloc;							    /* (kgC/m2) amount of C allocated */
	double plantNalloc;							    /* (kgN/m2) amount of N allocated */
	double plantCalloc_CUM;							/* (kgC/m2) cumulated amount of C allocated */
	double plantNalloc_CUM;							/* (kgN/m2) cumulated amount of N allocated */
	double excess_c;								/* (kgC/m2) difference between available and allocated C */
	double pnow;									/* (prop) proportion of growth displayed on current day */ 
	double MRdeficit_nw;							/* (flag) of maint.resp.calculation deficit for nw-biomass */
	double MRdeficit_w;								/* (flag) of maint.resp.calculation deficit for nw-biomass */
	double albedo_LAI;                              /* (dimless) LAI dependent albedo */
	double phenphase_date[N_PHENPHASES];			/* (DOY) date of phenphase's start */
	double rootDepth_phen[N_PHENPHASES];			/* (m) depth of the rootzone at the end of the given phenphase */
	double flower_date;								/* (DOY) date of flowering phenphase's start */
	double winterEnd_date;							/* (DOY) date of end of wintering */
	double WPM;                                     /* (1/day) daily whole plant mortality value */
	double FM ;                                     /* (1/day) daily fire mortality value */
	double SCpercent;                               /* (%) percent of soil coverage */
	double SC_EVPred;                               /* (prop) evaporation reduction effect of soil cover) */
	double plantNdemand;							/* (kgN/m2/d) N demand of plant */ 
} epvar_struct;
/* endOUT */

/* VAR sitec: site constants */
typedef struct
{
    double elev;								        /* (m) site elevation */
    double lat;				 					        /* (degrees) site latitude (negative for south) */
    double albedo_sw;								    /* (dimless) surface shortwave albedo */
	double soillayer_depth[N_SOILLAYERS];			    /* (m) contains the soil layer depths (positive values)*/
	double soillayer_thickness[N_SOILLAYERS];		    /* (m) contains the soil layer thicknesses (positive values) */
	double soillayer_midpoint[N_SOILLAYERS];			/* (m) contains the depths of the middle layers (positive values)*/
} siteconst_struct;								
/* endVAR */

/* VAR epc: canopy ecophysiological constants */
typedef struct
{
	int woody;					/* (flag) 1=woody, 0=non-woody */
	int evergreen;				/* (flag) 1=evergreen, 0=deciduous */
	int c3_flag;				/* (flag) 1 = C3,  0 = C4 */
	int phenology_flag;			/* (flag) 1=phenology model, 0=user defined */
	int transferGDD_flag;       /* (flag) 1= transfer calculation from GDD, 0 = transfer calculation from EPC */
	int q10depend_flag;			/* (flag) 1 = temperature dependent q10 value; 0= constans q10 value */
	int phtsyn_acclim_flag;		/* (flag) 1 = acclimation of photosynthesis 0 = no acclimation of photosynthesis */
	int resp_acclim_flag;		/* (flag) 1 = acclimation of respiration 0 = no acclimation of respiration*/
	int CO2conduct_flag;	    /* (flag) CO2 conductance reduction flag (0: no effect, 1: multiplier) */
	int SHCM_flag;			    /* (flag) water movement calculation method (0:Richards, 1:DSSAT) */
	int discretlevel_Richards;  /* (int) discretization level of VWC calculation */
	int STCM_flag;			    /* (flag) soil temperature calculation method (0:Zheng, 1:DSSAT) */
	int photosynt_flag;         /* (flag) photosynthesis calculation method (0: Farquhar, 1: DSSAT) */
    int ET_flag;	           /* (flag) evapotranspiration calculation method (0: Penman-Montieth, 1: Priestley-Taylor) */
    int radiation_flag;	        /* (flag) radiation calculation method (0: SWabs, 1: Rn) */
	int soilstress_flag;	    /* (flag) soilstress calculation method (0: based on VWC, 1: based on transpiration demand) */
	int interception_flag;      /* (flag) water interception (0: original based on allLAI, 1: MuSo7 based on projLAI) */
	int ondayUSER;              /* (doy) yearday leaves on */
	int offdayUSER;				/* (doy) yearday leaves off */
	double transfer_pdays;		/* (prop) fraction of growth period for transfer */
	double litfall_pdays;		/* (prop) fraction of growth period for litfall */
	double base_temp;			/* (Celsius) basic temperature fo GDD/heatsum calculation */
	double pnow_minT;			/* (Celsius) min. temp. for growth displayed on current day (-9999: no T-dep) */
	double pnow_opt1T;			/* (Celsius) opt1.temp. for growth displayed on current day (-9999: no T-dep) */ 
	double pnow_opt2T;			/* (Celsius) opt2.temp. for growth displayed on current day (-9999: no T-dep) */ 
	double pnow_maxT;			/* (Celsius) max. temp. for growth displayed on current day (-9999: no T-dep) */ 
	double assim_minT;			/* (Celsius) min. temp. for C-assim displayed on current day (-9999: no limit) */
	double assim_opt1T;			/* (Celsius) opt1.temp. for C-assim displayed on current day (-9999: no limit) */ 
	double assim_opt2T;			/* (Celsius) opt2.temp. for C-assim displayed on current day (-9999: no limit) */ 
	double assim_maxT;			/* (Celsius) max. temp. for C-assim displayed on current day (-9999: no limit) */
	double PT_ETcritT;          /* (Celsius) threshold temperature for ET-calculation (only in Priestley-Taylor method) */ 
    double nonwoody_turnover;	/* (1/yr) non-woody biomass turnover fraction */
    double woody_turnover;		/* (1/yr) woody biomass turnover fraction */
    double WPMyr;				/* (1/yr) whole plant mortality */
    double FMyr;				/* (1/yr) fire mortality */
	double dscp;                /* (prop) dead stem biomass combustion proportion */
	double cwcp;                /* (prop) coarse woody biomass combustion proportion */
	double leaf_cn;        /* (kgC/kgN)  C:N for leaves */
	double leaflitr_cn;    /* (kgC/kgN)  C:N for leaf litter */
    double froot_cn;       /* (kgC/kgN)  C:N for fine roots */
	double yield_cn;       /* (kgC/kgN)  C:N for yields */
    double softstem_cn;    /* (kgC/kgN)  C:N for softstems */
    double livewood_cn;    /* (kgC/kgN)  C:N for live wood */
	double deadwood_cn;    /* (kgC/kgN)  C:N for dead wood */
	double leafC_DM;       /* (kgC/kgDM) carbon content of leaf dry matter */
	double leaflitrC_DM;   /* (kgC/kgDM) carbon content of leaf litter dry matter */
    double frootC_DM;      /* (kgC/kgDM) carbon content of fine roots dry matter */
	double yield_DM;      /* (kgC/kgDM) carbon content of yields dry matter */
    double softstemC_DM;   /* (kgC/kgDM) carbon content of softstems dry matter */
    double livewoodC_DM;   /* (kgC/kgDM) carbon content of live wood dry matter  */
	double deadwoodC_DM;   /* (kgC/kgDM) carbon content of dead wood dry matter  */
	double leaflitr_flab;    /* (prop) leaf litter labile fraction */
    double leaflitr_fucel;   /* (prop) leaf litter unshielded cellulose fract. */
    double leaflitr_fscel;   /* (prop) leaf litter shielded cellulose fract. */
    double leaflitr_flig;    /* (prop) leaf litter lignin fraction */
    double frootlitr_flab;   /* (prop) froot litter labile fraction */
    double frootlitr_fucel;  /* (prop) froot litter unshielded cellulose fract */
    double frootlitr_fscel;  /* (prop) froot litter shielded cellulose fract */
    double frootlitr_flig;   /* (prop) froot litter lignin fraction */
	double yieldlitr_flab;          /* (prop) yield litter labile fraction */
    double yieldlitr_fucel;         /* (prop) yield litter unshielded cellulose fract. */
    double yieldlitr_fscel;         /* (prop) yield litter shielded cellulose fract. */
    double yieldlitr_flig;          /* (prop) yield litter lignin fraction */
	double softstemlitr_flab;       /* (prop) softstem litter labile fraction */
    double softstemlitr_fucel;      /* (prop) softstem litter unshielded cellulose fract. */
    double softstemlitr_fscel;      /* (prop) softstem litter shielded cellulose fract. */
    double softstemlitr_flig;       /* (prop) softstem litter lignin fraction */
    double deadwood_fucel;			/* (prop) dead wood unshileded cellulose fraction */
    double deadwood_fscel;			/* (prop) dead wood shielded cellulose fraction */
    double deadwood_flig;			/* (prop) dead wood lignin fraction */
    double sla_ratio;				/* (ratio) ratio of shaded to sunlit projected SLA */
    double lai_ratio;				/* (ratio) ratio of (all-sided LA / one-sided LA) */
    double int_coef;				/* (kg/kg/LAI/d) canopy precip interception coef */
    double ext_coef;				/* (dimless) canopy light extinction coefficient */
	double potRUE;					/* (g/MJ) potential radiation use efficiency */
	double rad_param1;				/* ((DIM) radiation parameter1 (Jiang et al.2015) */
	double rad_param2;				/* ((DIM) radiation parameter2 (Jiang et al.2015) */
    double flnr;					/* (kg NRub/kg Nleaf) leaf N in Rubisco */
	double flnp;					/* (kg PeP/kg Nleaf) fraction of leaf N in PEP Carboxylase */
    double gl_sMAX;					/* (m/s) maximum leaf-scale stomatal conductance */
    double gl_c;					/* (m/s) leaf-scale cuticular conductance */
	double gl_bl;					/* (m/s) leaf-scale boundary layer conductance */
	double VWCratio_SScrit1;		/* (prop) VWC ratio to calc. soil moisture limit 1 (prop. to FC-WP)*/
    double VWCratio_SScrit2;		/* (prop) VWC ratio to calc. soil moisture limit 2 (prop. to SAT-FC) */
	double m_fullstress2;			/* (prop) minimum of soil moisture limit2 multiplicator (full stress value) */
	double vpd_open;				/* (Pa)  vpd at start of conductance reduction */
	double vpd_close;				/* (Pa)  vpd at complete conductance reduction */
	double GR_ratio;				/*  (ratio) growth resp per unit of C grown */
	double nfix;					/*  (kgN/m2/yr) symbiotic+asymbiotic fixation of N */
	double tau;						/*	(day) time delay for temperature in photosynthesis acclimation */
	double rootlength_par1;			/*  (kgC/m2) rootlengh parameter 1 (maximal weight) */
	double rootlength_par2;			/*  (dimless) rootlenght parameter 1 (slope) */
	double plantHeight_max;        /* (m) maximum plant height */
	double plantHeight_par1;        /* (kgC/m2) plantHeight parameter 1 (stem weight at which maximum height attended)  */
	double plantHeight_par2;        /* (dimless) plantHeight parameter 1 (slope of plantHeight function) */
	double rootzoneDepth_max;		/* (m)   maximum depth of rooting zone */
	double rootdistrib_param;       /* (dimless) root distribution parameter (Jarvis 1989) */
	double rootweight2length;       /* (m/kg) root weight to length parameter */
	double MRperN;                  /* (kgC/kgN/d) maintenance respiration in kgC/day per kg of tissue N */
	double NSC_SC_prop;             /* (prop) theoretical maximum prop. of non-structural and structural carbohydrates */
	double NSC_avail_prop;		    /* (prop) ratio of non-structural carbohydrates available for maintanance respiration */
	double maxSNSCmort_leaf;	    /* (prop) maximum mortality of senescence of leaf */
	double maxSNSCmort_other;	    /* (prop) maximum mortality of senescence of other plant pool (softstem, froot) */
	double m_nscSNSCmort;           /* (prop) multiplier for senescence calculation of non-structured pools */
	double SNSC_extremT1;           /* (Ceslius) lower limit extreme high temperature effect on senesncene mortality */
	double SNSC_extremT2;           /* (Ceslius) upper limit extreme high temperature effect on senesncene mortality */
    double mort_SNSC_to_litter;		/*  (prop) turnover rate of wilted standing biomass to litter*/
	double mort_CnW_to_litter;		/*  (prop) turnover rate of cut-down non-woody biomass to litter*/
	double mort_CW_to_litter;		/*  (prop) turnover rate of cut-down woody biomass to litter*/
	double SWCstressLENGTH_crit;	/*  (nday) critical value of SWC-length limitation */
	double photoSTRESSeffect;		/*  (dimless) effect of soil moisture stress on photosynthesis */
	int n_germ_phenophase;	        /* (DIM) number of phenophase when emergence occurs */
	int n_emerg_phenophase;	        /* (DIM) number of phenophase when emergence occurs */
	int n_phpsl_phenophase;	        /* (DIM) number of phenophase when photoperiodic slowing effect occurs  */
	double phpsl_parDL;				/* (hour) critical photoslow daylength 1 */
	double phpsl_parDR;				/* (DIM) slope of relative photoslow development rate */
	int n_vern_phenophase;	        /* (DIM) number of phenophase when vernalization occurs  */
	double grmn_paramVWC;           /* (prop.) critical VWC ratio (prop. to FC-WP) in germination */
	double vern_parT1;              /* (Celsius) critical vernalization temperature 1 */
	double vern_parT2;              /* (Celsius) critical vernalization temperature 2 */
	double vern_parT3;              /* (Celsius) critical vernalization temperature 3 */
	double vern_parT4;              /* (Celsius) critical vernalization temperature 4 */
	double vern_parDR1;             /* (DIM) slope of relative vernalization development rate */
	double vern_parDR2;             /* (DIM) max of vernalization days */
	int n_flowHS_phenophase;        /* (DIM) number of phenophase of flowering heat stress */ 
	double flowHS_parT1;            /* (Celsius) critical flowering heat stress temperature 1 */
	double flowHS_parT2;            /* (Celsius) critical flowering heat stress temperature 1 */
	double flowHS_parMORT;          /* (prop.) mortality parameter of flowering heat stress */
	double* FMyr_array;				/* (flag) ARRAY of changing FM flag */
	double* WPMyr_array;			/* (flag) ARRAY of changing WPM flag */
	double* MSC_array;				/* (flag) ARRAY of changing MSC flag */
	double* SGS_array;				/* (flag) ARRAY of changing SGS flag */
	double* EGS_array;				/* (flag) ARRAY of changing EGS flag */
	int GSI_flag;					/* flag for using GSI or not */
	double snowcover_limit;		/* critical amount of snow (above: no vegetation period) */
	double heatsum_limit1;			/* lower limit of heatsum to calculate heatsum index */
    double heatsum_limit2;			/* upper limit of heatsum to calculate heatsum index */
	double Tmin_limit1;				/* lower limit of Tmin to calculate Tmin index */
	double Tmin_limit2;				/* upper limit of Tmin to calculate Tmin index  */
	double vpd_limit1;				/* lower limit of Tmin to calculate vpd index  */
	double vpd_limit2;				/* upper limit of Tmin to calculate vpd index  */
	double dayl_limit1;				/* lower limit of dayl to calculate vpd index  */
	double dayl_limit2;				/* upper limit of dayl to calculate vpd index  */
	int n_moving_avg;				/* moving average (calculated from indicatiors to avoid the effects of single extreme events)*/
	double GSI_limit_SGS;			/* when GSI first time greater that limit -> start of growing season */
	double GSI_limit_EGS;			/* when GSI first time less that limit -> end of growing season */
	double phenophase_length[N_PHENPHASES];				/* (Celsius) length of phenphase (critical heatsums) */
    double alloc_leafc[N_PHENPHASES];					/* (ratio) new leaf C to new C */
	double alloc_frootc[N_PHENPHASES];					/* (ratio) new fine root C to new C */
	double alloc_yield[N_PHENPHASES];					/* (ratio) new yield C to new C */
	double alloc_softstemc[N_PHENPHASES];				/* (ratio) new softstem c to new  C */
	double alloc_livestemc[N_PHENPHASES];				/* (ratio) new live stem C to new C */
	double alloc_deadstemc[N_PHENPHASES];				/* (ratio) new dead stem C to new C */
    double alloc_livecrootc[N_PHENPHASES];				/* (ratio) new live croot C to new  C */
	double alloc_deadcrootc[N_PHENPHASES];				/* (ratio) new dead croot C to new  C */
	double avg_proj_sla[N_PHENPHASES];					/* (m2/kgC) canopy average proj. SLA in a given phenphase */
    double curgrowth_prop[N_PHENPHASES];			    /* (prop) daily allocation to current growth */
	double max_plantlifetime[N_PHENPHASES];				/* (Celsius) maximal lifetime of plant tissue */
} epconst_struct;
/* endVAR */

/* OUT sprop: soil properties */
typedef struct
{
	double denitr_coeff;			/*  (prop) denitrification coefficient - multilayer_sminn.c */
	double netMiner_to_nitrif;		/*  (prop) proportion of net mineralization that is nitrified */
	double maxNitrif_rate;			/*  (1/day) maxmimum nitrification rate (max. fraction of NH4 nitrified) */
	double N2Ocoeff_nitrif;			/*  (prop) coefficient of N2O emission of nitrification */
	double NH4_mobilen_prop;		/*  (prop) fraction mineral N avail for plant.soil processes and leaching */
	double critWFPS_denitr;		    /*  (prop) critical WFPS value for denitrification */
	double N2Oratio_denitr;         /*  (dimless) texture dependence of N2O:N2 ratio of denitrification */
	double efolding_depth;          /* (m) e-folding depth of decomposition rate's depth scalar (Koven et al. 2013) */
	double SOIL1_dissolv_prop;      /* (prop) fraction of dissolved part of SOIL1 organic matter */
	double SOIL2_dissolv_prop;      /* (prop) fraction of dissolved part of SOIL2 organic matter  */
	double SOIL3_dissolv_prop;      /* (prop) fraction of dissolved part of SOIL3 organic matter */
	double SOIL4_dissolv_prop;      /* (prop) fraction of dissolved part of SOIL4 organic matter */
	double minWFPS_nitrif;			/* (prop) minimum WFPS for scalar function of nitrification calculation */
	double opt1WFPS_nitrif;         /* (prop) lower optimum WFPS for scalar function of nitrification calculation */
	double opt2WFPS_nitrif;         /* (prop) higher optimum  WFPS for scalar function of nitrification calculation */
	double scalarWFPSmin_nitrif;    /* (prop) minimum value for saturated WFPS scalar of nitrification calculation */
	double pHp1_nitrif;             /* (dimless) parameter 1 for pH response function of nitrification */
	double pHp2_nitrif;             /* (dimless) parameter 2 for pH response function of nitrification */
	double pHp3_nitrif;             /* (dimless) parameter 3 for pH response function of nitrification */
	double pHp4_nitrif;             /* (dimless) parameter 4 for pH response function of nitrification */
	double Tp1_nitrif;              /* (dimless) parameter 1 for Tsoil response function of nitrification */
	double Tp2_nitrif;              /* (dimless) parameter 2 for Tsoil response function of nitrification */
	double Tp3_nitrif;              /* (dimless) parameter 3 for Tsoil response function of nitrification */
	double Tp4_nitrif;              /* (dimless) parameter 4 for Tsoil response function of nitrification */
	double Tp1_decomp;              /* (dimless) parameter 1 for Tsoil response function of decomposition */
	double Tp2_decomp;              /* (dimless) parameter 2 for Tsoil response function of decomposition */
	double Tp3_decomp;              /* (dimless) parameter 3 for Tsoil response function of decomposition */
	double Tp4_decomp;              /* (dimless) parameter 4 for Tsoil response function of decomposition */
	double Tmin_decomp;             /* (Celsius) minimum T for decomposition (below which no decomposition is assumed) */
	double pLAY_soilCover;            /* (dimless) soil cover parameter: layer effect */
	double p1_soilCover;            /* (dimless) parameter 1 for soil cover function  */
	double p2_soilCover;            /* (dimless) parameter 2 for soil cover function  */
	double p3_soilCover;            /* (dimless) parameter 3 for soil cover function  */
	double pRED_soilCover;              /* (dimless) soil cover parameter: evaporation reduction */
	double pCRIT_soilCover;             /* (dimless) soil cover parameter: critical amount */
	double p1diffus_tipping;        /* (dimless) parameter 1 for diffusion calculation */
	double p2diffus_tipping;        /* (dimless) parameter 2 for diffusion calculation */
	double p3diffus_tipping;        /* (dimless) parameter 3 for diffusion calculation */
	double GWD;						/* (m) actual depth of the groundwater on a given day */
	double CFD;						/* (m) actual depth of the upper layer of capillary fringeon a given day */
	double GWlayer;					/* (n) number of layer containing groundwater table */
	double CFlayer;					/* (n) number of layer containing upper boundary of capillary fringe */
	double preGWD;				    /* (m) actual depth of the groundwater on a previous day */
	double preGWlayer;				/* (m) actual depth of the groundwater on a previous day */

	double soil1_CN;				/* (prop) C:N for labile SOM pool   */
	double soil2_CN;				/* (prop) C:N for fast decomposing SOM pool   */
	double soil3_CN;				/* (prop) C:N for slowdecomposing SOM pool   */
	double soil4_CN;				/* (prop) C:N for stable SOM pool   */
	double totalSOCcrit;			/* (kgC/m2) critical (user-defined) total SOC content   */
	double soilEVPlim;             /* (mm) limitation of soil evaporation (Joe Ritchie-method) */
	double rfl1s1;                  /* (prop) respiration fractions for fluxes between compartments  */
	double rfl2s2;                  /* (prop) respiration fractions for fluxes between compartments  */
	double rfl4s3;                  /* (prop) respiration fractions for fluxes between compartments  */
	double rfs1s2;                  /* (prop) respiration fractions for fluxes between compartments  */
	double rfs2s3;                  /* (prop) respiration fractions for fluxes between compartments  */
	double rfs3s4;                  /* (prop) respiration fractions for fluxes between compartments  */		
	double L1release_ratio;         /* (1/day) Litter release for labile pool   */    
	double L2release_ratio;			/* (1/day) Litter release for cellulose pool   */ 
	double L4release_ratio;			/* (1/day) Litter release for lignin pool   */    
	double kl1_base;                /* (1/day) base values of rate constants   */    
	double kl2_base;                /* (1/day) base values of rate constants   */    
	double kl4_base;                /* (1/day) base values of rate constants   */     
	double ks1_base;                /* (1/day) base values of rate constants   */     
	double ks2_base;                /* (1/day) base values of rate constants   */     
	double ks3_base;                /* (1/day) base values of rate constants   */    
	double ks4_base;                /* (1/day) base values of rate constants   */    
	double kfrag_base;              /* (1/day) base values of rate constants   */  
	double pBD1_CH4;                  /* (dimless) bulk density parameter of empirical CH4 modeling */
	double pBD2_CH4;                  /* (dimless) bulk density parameter of empirical CH4 modeling */
	double pVWC1_CH4;                 /* (dimless) soil water content parameter of empirical CH4 modeling */
	double pVWC2_CH4;                 /* (dimless) soil water content parameter of empirical CH4 modeling */
	double pVWC3_CH4;                 /* (dimless) soil water content parameter of empirical CH4 modeling */
	double C_pVWC4;                 /* (dimless) soil water content parameter of empirical CH4 modeling */
	double pTS_CH4;                   /* (dimless) soil temperature parameter of empirical CH4 modeling */
	double soildepth;               /* (m) soil depth */
	double pondmax;								        /* (mm) maximum of pond water */
	double curvature_SS;								/* (dimless) curvature of soil stress function */
	double sand[N_SOILLAYERS];							/* (%) sand content of soil in the given soil layer */	
	double silt[N_SOILLAYERS];							/* (%) silt content of soil in the given soil layer */	
	double clay[N_SOILLAYERS];							/* (%) clay content of soil in the given soil layer */
	double pH[N_SOILLAYERS];							/* (%) soil pH in the given soil layer */
	double RCN_mes;								        /* (m) measured runoff curve number */
	double aerodyn_resist;					     		/* (s/m)  aerodynamic resistance (Wallace and Holwill, 1997) */
	double BD_mes[N_SOILLAYERS];					    /* (g/cm3)  measured bulk density */
	double VWCsat_mes[N_SOILLAYERS];					/* (m3/m3)  measured soil water content at saturation*/
	double VWCfc_mes[N_SOILLAYERS];						/* (m3/m3)  measured soil water content at field capacity*/
	double VWCwp_mes[N_SOILLAYERS];						/* (m3/m3)  measured soil water content at wilting point*/
	double VWChw_mes[N_SOILLAYERS];						/* (m3/m3)  measured hygroscopic water content */
	double drainCoeff_mes[N_SOILLAYERS];				/* (dimless)  measured drainage coefficient */
	double drainCoeff[N_SOILLAYERS];				    /* (dimless)  drainage coefficient */
	double hydrCONTUCTsatMES_cmPERday[N_SOILLAYERS];	/* (cm/day)  measured saturated conductivity */
	double PSIhw;										/* (MPa) soil matric potential at hygroscopic water point */
	double RCN;								            /* (m) runoff curve number */
	double CapillFringe[N_SOILLAYERS];					/* (m) Thickness of capillarity fringe (from PSIsat or from user-defined value) */
	double CapillFringeMES_cm[N_SOILLAYERS];			/* (cm) Thickness of capillarity fringe (user-defined value) */
    double soilB[N_SOILLAYERS];							/* (dimless) Clapp-Hornberger "b" parameter */
	double BD[N_SOILLAYERS];							/* (g/cm3) bulk density */
    double VWCsat[N_SOILLAYERS];						/* (m3/m3) volumetric water content at saturation */
    double VWCfc[N_SOILLAYERS];							/* (m3/m3) VWC at field capacity ( = -0.033 MPa) */
	double VWCwp[N_SOILLAYERS];							/* (m3/m3) VWC at wilting point ( = pF 4.2) */
	double VWChw[N_SOILLAYERS];							/* (m3/m3) VWC at hygroscopic water point  ( = pF 6.2) */
	double VWCfc_base[N_SOILLAYERS];					/* (m3/m3) VWC at field capacity without the effect of groundwater table */
    double PSIsat[N_SOILLAYERS];						/* (MPa) soil matric potential at saturation */
	double PSIfc[N_SOILLAYERS];							/* (MPa) soil matric potential at field capacity */
	double PSIwp[N_SOILLAYERS];							/* (MPa) soil matric potential at wilting point */
	double hydrCONDUCTsat[N_SOILLAYERS];				/* (m/s) hidraulic conductivity at saturation  */
	double hydrDIFFUSsat[N_SOILLAYERS];					/* (m2/s) hidraulic diffusivity at saturation  */
	double hydrCONDUCTfc[N_SOILLAYERS];					/* (m/s) hidraulic conductivity at field capacity  */
	double hydrDIFFUSfc[N_SOILLAYERS];					/* (m2/s) hidraulic diffusivity at field capacity  */
	double GWeff[N_SOILLAYERS];					        /* (dimless) coefficient of groundwater effect  */
	double CFeff[N_SOILLAYERS];					        /* (dimless) coefficient of capillary effect  */
	double coeff_EVPlim;								/* (ratio) coefficient of soil evaporation calculations by Joe Ritchie */
	double coeff_EVPcum;								/* (dimless) coefficient of soil evaporation calculations by Joe Ritchie */
	double coeff_DSRmax;								/* (dimless) coefficient of maximal DRS in soil evaporation limitation */
	double curvature_DC;								/* (dimless) curvature of decomposition stress function */
	double VWCratio_DCcrit1;							/* (m3/m3) volumetric water content ratio at start of decomposition limitation */
	double VWCratio_DCcrit2;							/* (m3/m3) volumetric water content ratio at full decomposition limitation */
} soilprop_struct;
/* endOUT */

/* struct fo groundwater calculation */
typedef struct
{
	double HYDROflag[N_SOILLAYERS_GWC];	                /* (flag) for defining hydrological layers (0: normal, 1: capillary, 2: saturated with GW */
	double soillayer_depthGWC[N_SOILLAYERS_GWC];	    
	double soillayer_thicknessGWC[N_SOILLAYERS_GWC];	
	double VWC_GWC[N_SOILLAYERS_GWC];	  
	double VWCsat_GWC[N_SOILLAYERS_GWC];
	double VWCfc_GWC[N_SOILLAYERS_GWC];
	double VWChw_GWC[N_SOILLAYERS_GWC];
	double soilw_GWC[N_SOILLAYERS_GWC];
	double hydrCONDUCTsat_GWC[N_SOILLAYERS_GWC];
	double soilB_GWC[N_SOILLAYERS_GWC];
	double PSIsat_GWC[N_SOILLAYERS_GWC];
	double CapillFringe_GWC[N_SOILLAYERS_GWC];
	double rootlengthProp_GWC[N_SOILLAYERS_GWC];
	double soilwFlux_GWC[N_SOILLAYERS_GWC];
	double soilwTRP_GWC[N_SOILLAYERS_GWC];
	double soilwTRPdemand_GWC[N_SOILLAYERS_GWC];
	double GWrecharge_GWC[N_SOILLAYERS_GWC];
	double GWdischarge_GWC[N_SOILLAYERS_GWC];

} GWcalc_struct;

/* VAR GWD: strucure for groundwater paramteres */
typedef struct
{
	int mgmdGWD;								/* (int) number of the management action (-1: no management) */
	int GWD_num;								/* (int) number of planting in a simulation */	
	int* GWyear_array;							/* (int) ARRAY of contains the groundwater depth year */
	int* GWmonth_array;						    /* (int) ARRAY of contains the groundwater depth month */
    int* GWday_array;							/* (int) ARRAY of contains the groundwater depth day */
	double* GWdepth_array;						/* (m) ARRAY of depth of the groundwater */	
} groundwater_struct;
/* endVAR */

/* VAR PLT: strucure for planting paramteres */
typedef struct
{
	int mgmdPLT;								/* (int) number of the management action (-1: no management) */
	int PLT_num;								/* (int) number of planting in a simulation */	
	int* PLTyear_array;							/* (int) ARRAY of contains the planting year */
	int* PLTmonth_array;						/* (int) ARRAY of contains the planting month */
    int* PLTday_array;							/* (int) ARRAY of contains the planting day */
	double* germDepth_array;			        /* (m) ARRAY of germination_depth */
	double* n_seedlings_array;				    /* (n/m2) ARRAY of number of seedlings */
	double* weight_1000seed_array;				/* (g/1000n) ARRAY of specific weight of seed */
	double* seed_carbon_array;					/* (%) ARRAY of carbon content of seed*/
	char** filename_array;						/* (filename) ARRAY of changing EPC files (croprotation) */
} planting_struct;
/* endVAR */

/* varTHN: strucure for thinning paramteres */
typedef struct
{
	int mgmdTHN;								/* (int) number of the management action (-1: no management) */
	int THN_num;								/* (int) number of thinning in a simulation */
	int* THNyear_array;							/* (int) ARRAY of thinning year */
	int* THNmonth_array;						/* (int) ARRAY of thinning month */
	int* THNday_array;							/* (int)  ARRAY of thinning days*/
	double* thinningRate_w_array;				/* (%) ARRAY of proportion of the thinned woody plant biomass */
	double* thinningRate_nw_array;				/* (%) ARRAY of proportion of the thinned non-woody plant biomass */
	double* transpCoeff_w_array;				/* (%) ARRAY of rate of the transported woody matter */
	double* transpCoeff_nw_array;				/* (%) ARRAY of rate of the transported non woody matter */
} thinning_struct;
/* endVAR */

/* VAR MOW: strucure for mowing paramteres  */
typedef struct
{
	int mgmdMOW;								/* (int) number of the management action (-1: no management) */
	int MOW_num;								/* (int) number of mowing in a simulation */
	int* MOWyear_array;							/* (int) ARRAY of the mowing year */
	int* MOWmonth_array;						/* (int) ARRAY of the mowing month */
	int* MOWday_array;							/* (int) ARRAY of mowing days */
	double* LAI_limit_array;					/* (m2/m2) ARRAY of LAI after mowing */
	double* transportMOW_array;					/* (%) ARRAY of proportion of plant material transported away */
	int condMOW_flag;							/* (flag) for conditional irrigating management option */
	double fixLAIbef_condMOW;					/* (value) LAI before mowing (conditional mowing)*/
	double fixLAIaft_condMOW;					/* (value) LAI after mowing (conditional mowing) */
	double transpCOEFF_condMOW;					/* (%) proportion of plant material transported away (conditional mowing)*/
} mowing_struct;
/* endVAR */

/* VAR GRZ: strucure for grazing paramteres */
typedef struct
{
	int mgmdGRZ;								/* (int) number of the management action (-1: no management) */
	int GRZ_num;								/* (int) number of grazing in a simulation */
	int* GRZstart_year_array;					/* (int) ARRAY of grazing start year */
	int* GRZstart_month_array;					/* (int) ARRAY of grazing start  month */
	int* GRZstart_day_array;					/* (int)  ARRAY of grazing end days*/
	int* GRZend_year_array;						/* (int) ARRAY of grazing end year */
	int* GRZend_month_array;					/* (int) ARRAY of grazing end  month */
	int* GRZend_day_array;						/* (int)  ARRAY of grazing start days*/
	double* trampling_effect;					/* (%) ARRAY of trampling effect coefficient (standing dead biomass to litter) */
	double* weight_LSU;							/* (kg/LSU)) ARRAY of weight equivalent of an averaged animal */
	double* stocking_rate_array;				/* (LSU/ha) ARRAY of animal stocking rate: Livestock Units per hectare */
	double* DMintake_array;						/* (kgDM/m2/d) ARRAY of pasture forgage dry matter intake */
	double* DMintake2excr_array;			    /* (%) ARRAY of ratio of the dry matter intake formed excrement */
	double* excr2litter_array;				    /* (%) ARRAY of ratio of excrement return to litter */ 
	double* DM_Ccontent_array;					/* (%) ARRAY of carbon content of dry matter*/
	double* EXCR_Ncontent_array;				/* (%) ARRAY of nitrogen content of the fertilizer */
	double* EXCR_Ccontent_array;				/* (%) ARRAY of carbon content of the fertilizer */
	double* Nexrate;                            /* (kgN/1000 kg animal mass/day) ARRAY of default N excretion rate */
	double* EFman_N2O;							/* (kgN2O-N:kgN) ARRAY of manure emission factor for N2O emission	 */
	double* EFman_CH4;							/* (kgCH4/LSU/yr) ARRAY of manure emission factor for CH4 emission */
	double* EFfer_CH4;							/* (kgCH4/LSU/yr) ARRAY of animal emission factor for CH4 emission */
	double trampleff_act;						/* (%) actual b (standing dead biomass to litter) */
} grazing_struct;
/* endVAR */


/* VAR HRV: strucure for harvesting paramteres */
typedef struct
{
	int mgmdHRV;								/* (int) number management action (-1 = no management) */
	int HRV_num;								/* (int) number of harvesting in a simulation */
    int* HRVyear_array;							/* (int) ARRAY of the harvesting year */
	int* HRVmonth_array;						/* (int) ARRAY of the harvesting month */
	int* HRVday_array;							/* (int) ARRAY of harvesting days */
	double* snagprop_array;						/* (%) ARRAY of proportion of snag before and after harvesting*/
	double* transportHRV_array;					/* (%) ARRAY of rate of plant material transported away */
} harvesting_struct;
/* endVAR */

/* VAR PLG: strucure for ploughing paramteres  */
typedef struct
{
	int mgmdPLG;								/* (int) number of the management action (-1: no management) */
	int PLG_num;								/* (int) number of ploughing in a simulation */
	int* PLGyear_array;							/* (int) ARRAY of the ploughing year */
	int* PLGmonth_array;						/* (int) ARRAY of the ploughing month */
	int* PLGday_array;							/* (int) ARRAY of ploughing days */
	double* PLGdepths_array;					/* (dimless) ARRAY of ploughing depths in 1 year*/
} ploughing_struct;
/* endVAR */


/* VAR FRZ: strucure for fertilizing paramteres  */
typedef struct
{
	int mgmdFRZ;								/* (int) number of the management action (-1: no management) */
	int FRZ_num;								/* (int) number of fertilizing in a simulation */
	int* FRZyear_array;							/* (int) ARRAY of the fertilizing year */
	int* FRZmonth_array;						/* (int) ARRAY of the fertilizing month */
	int* FRZday_array;							/* (int) ARRAY of fertilizing days */
	double* FRZdepth_array;						/* (doy) ARRAY of contains the fertilizing depth */
	double* fertilizer_array;					/* (kg fertilizer/ha) ARRAY of amount of fertilizer on the fertilizing days */
	double* DM_array;				            /* (%) ARRAY of dry matter of fertilizer */
	double* NO3content_array;					/* (kgN/kg fertilizerDM) ARRAY of nitrate content of fertilizerDM */
 	double* NH4content_array;					/* (kgN/kg fertilizerDM) ARRAY of ammonium content of fertilizerDM */
	double* UREAcontent_array;					/* (kgN/kg fertilizerDM) ARRAY of UREA content of fertilizerDM */
	double* orgCcontent_array;					/* (kgC/kg fertilizerDM) ARRAY of organic carbon content of fertilizerDM */
	double* orgNcontent_array;					/* (kgC/kg fertilizerDM) ARRAY of organic nitrogen content of fertilizerDM */
	double* litr_flab_array;					/* (%) ARRAY of labile fraction of fertilizer organic carbon content */
	double* litr_fcel_array;					/* (%) ARRAY of cellulose fraction of fertilizer organic carbon content */
	double* EFfert_N2O;							/* (kgN2O-N:kgN) ARRAY of fertilization emission factor for direct N2O emissions */
	char** ferttype_array;						/* (char) ARRAY of fertilizing type */
} fertilizing_struct;
/* endVAR */

/* VAR IRG: strucure for irrigating paramteres  */
typedef struct
{
	int mgmdIRG;								/* (int) number of the management action (-1: no management) */
	int IRG_num;								/* (int) number of irrigating in the simulation */	
    int* IRGyear_array;							/* (int) ARRAY of the irrigating year */
	int* IRGmonth_array;						/* (int) ARRAY of the irrigating month */
	int* IRGday_array;							/* (int) ARRAY of irrigating days */
	double* IRGquantity_array;					/* (kgH2O/m2/d) ARRAY of quantity of water*/
	double* IRGheight_array;				    	/* (m) ARRAY of depth of irrigation*/
	int condIRG_flag;							/* (flag) for conditional irrigating management option(0:no, 1:SMSI, 2:SWCratio */
	double nLayer_condIRG;                      /* (n) SMSI before cond. IRRIGATING (-9999: crit. VWCratio is used)  */
	double startPoint_condIRG;                  /* (prop) starting point of condIRG (if flag=1:SMSI, flag=2:SWCratio) */
	double aftVWCratio_condIRG;                 /* (ratio) VWC ratio after conditional IRRIGATING  */
	double maxAMOUNT_condIRG;                   /* (kgH2O/m2) maximum amount of irrigated water */
	
} irrigating_struct;
/* endVAR */

/* VAR MUL: strucure for mulching paramteres */
typedef struct
{
	int mgmdMUL;								/* (int) number of the management action (-1: no management) */
	int MUL_num;								/* (int) number of mulching in a simulation */	
	int* MULyear_array;							/* (int) ARRAY of contains the mulching year */
	int* MULmonth_array;						/* (int) ARRAY of contains the mulching month */
    int* MULday_array;							/* (int) ARRAY of contains the mulching day */
	double* litrCabove_MUL;			            /* (kgC/m2) ARRAY of non-woody mulch carbon content */
	double* cwdCabove_MUL;				        /* (kgC/m2) ARRAY of woody mulch carbon content  */
	double* litrCNabove_MUL;				    /* (prop) ARRAY of CN ratio of non-woody mulch */
	double* cwdCNabove_MUL;				      	/* (prop) ARRAY of CN ratio of woody mulch*/
} mulching_struct;
/* endVAR */

/* VAR CWE: strucure for CWDextract paramteres */
typedef struct
{
	int mgmdCWE;								/* (int) number of the management action (-1: no management) */
	int CWE_num;								/* (int) number of CWD-extract in a siCWEation */	
	int* CWEyear_array;							/* (int) ARRAY of contains the CWD-extract year */
	int* CWEmonth_array;						/* (int) ARRAY of contains the CWD-extract month */
    int* CWEday_array;							/* (int) ARRAY of contains the CWD-extract day */
	double* removePROP_CWE;			            /* (prop) ARRAY of proportion of remove */
} CWDextract_struct;
/* endVAR */

/* VAR FLD: strucure for flooding paramteres */
typedef struct
{
	int mgmdFLD;								/* (int) number of the management action (-1: no management) */
	int FLD_num;								/* (int) number of flooding in a siFLDation */	
	int* FLDstart_year_array;					/* (int) ARRAY of contains the flooding year */
	int* FLDstart_month_array;					/* (int) ARRAY of contains the flooding month */
    int* FLDstart_day_array;					/* (int) ARRAY of contains the flooding day */
	int* FLDend_year_array;						/* (int) ARRAY of contains the flooding year */
	int* FLDend_month_array;					/* (int) ARRAY of contains the flooding month */
    int* FLDend_day_array;						/* (int) ARRAY of contains the flooding day */
	double* FLDheight;						/* (m) depth of saturation */
} flooding_struct;
/* endVAR */

/* OUT psn: structure for the photosynthesis routine */
typedef struct
{
	int c3;                 /* (flag) set to 1 for C3 model, 0 for C4 model */
	double pa;              /* (Pa) atmospheric pressure */
	double co2;             /* (ppm) atmospheric [CO2] */
	double t;               /* (Celsius) temperature */
	double lnc;             /* (kg Nleaf/m2) leaf N per unit sunlit leaf area */
	double flnr;            /* (kg NRub/kg Nleaf) fract. of leaf N in Rubisco */
	double flnp;            /* (kg NPep/kg Nleaf)fraction of leaf N in PEP Carboxylase */
	double ppfd;            /* (umol/m2/s) PAR flux per unit sunlit leaf area */
	double g;               /* (umol/m2/s/Pa) conductance to CO2 */
	double dlmr;            /* (umol/m2/s) day leaf m. resp, proj. area basis */
	double Ci;              /* (Pa) intercellular [CO2] */
	double O2;              /* (Pa) atmospheric [O2] */
	double Ca;              /* (Pa) atmospheric [CO2] */
	double gamma;           /* (Pa) CO2 compensation point, no Rd */
	double Kc;              /* (Pa) MM constant carboxylation */
	double Ko;              /* (Pa) MM constant oxygenation */
	double Vmax;            /* (umol/m2/s) max rate carboxylation */
	double Jmax;            /* (umol/m2/s) max rate electron transport */
	double J;               /* (umol/m2/s) rate of RuBP regeneration */
	double Av;              /* (umol/m2/s) carboxylation limited assimilation */
	double Aj;              /* (umol/m2/s) RuBP regen limited assimilation */
	double A;               /* (umol/m2/s) final assimilation rate */
} psn_struct;
/* endOUT */

/* VAR pmet_in: input structure for penmon function */ 
typedef struct
{
	double ta;              /* (Celsius) air temperature */
	double pa;              /* (Pa)   air pressure */
	double vpd;             /* (Pa)   vapor pressure deficit */
	double irad;            /* (W/m2) incident shortwave flux density */
	double rv;              /* (s/m)  resistance to water vapor flux */
	double rh;              /* (s/m)  resistance to sensible heat flux */
} pmet_struct;
/* endVAR */


/* OUT summary: structure for summarzing variables */
typedef struct
{
	double annprcp;                     /* (mm/yr) annual total precipitation */
	double anntavg;                     /* (Celsius) annual average air temperature */
	double cumRunoff;                  /* (kgH2O/m2) cumulative  SUM of runoff */
	double cumWleachRZ;                /* (kgH2O/m2) cumulative SUM of water leaching from rootzone */
	double cumNleachRZ;                /* (kgN/m2/yr) cumulative SUM of nitrogenleaching from rootzone */
	double N2Oflux;                     /* (kgN/m2/day)  daily N2O flux */
	double N2OfluxCeq;                  /* (kgC/m2/day)  daily N2O flux in C eq. */
	double NEP;					/* (kgC/m2/day)  NPP - Rheretotrop */
	double NPP;					/* (kgC/m2/day)  GPP - Rmaint - Rgrowth */
	double NEE;					/* (kgC/m2/day)  GPP - Rmaint - Rgrowth - Rheretotrop - fire losses */
	double NBP;					/* (kgC/m2/day)  GPP - Rmaint - Rgrowth - Rheretotrop - disturb_emission - fire losses*/
	double NGB;					/* (kgC/m2/day)  NBP - N2Oflux(Ceq) - CH4(Ceq)*/
	double GPP;					/* (kgC/m2/day)  gross PSN source */
	double MR;					/* (kgC/m2/day)  maintenance respiration */
	double GR;					/* (kgC/m2/day)  growth respiration */
	double HR;					/* (kgC/m2/day)  heterotrophic respiration */
	double SR;					/* (kgC/m2/day)  soil respiration */
	double TR;					/* (kgC/m2/day)  total respiration */
	double fire;					/* (kgC/m2/day)  fire losses */
	double litfallc;				/* (kgC/m2/day)  total litterfall */
	double litfallc_above;		/* (kgC/m2/day)  total litterfall aboveground */
	double litfallc_below;		/* (kgC/m2/day)  total litterfall belowground */
	double litdecomp;				/* (kgC/m2/day)  total litter decomposition */
	double litfire;               /* (kgC/m2/day)  total litter fire mortality */
	double litter;				/* (kgC/m2)  total amount of litter */
	double cumNPP;						/* (kgC/m2)  cumulative SUM of NPP */
	double cumNEP;						/* (kgC/m2)  cumulative SUM of NEP */
	double cumNEE;						/* (kgC/m2)  cumulative SUM of NEE */
	double cumGPP;						/* (kgC/m2)  cumulative SUM of GPP */
	double cumNBP;					    /* (kgC/m2)  cumulative SUM of NBP */
	double cumNGB;					    /* (kgC/m2)  cumulative SUM of NGB */
	double cumMR;						/* (kgC/m2)  cumulative SUM of MR */
	double cumGR;						/* (kgC/m2)  cumulative SUM of GR */
	double cumHR;						/* (kgC/m2)  cumulative SUM of HR */
	double cumAR;						/* (kgC/m2)  cumulative SUM of HR */
	double cumTR;					    /* (kgC/m2)  cumulative SUM of total ecosystem respiration */
	double cumSR;					    /* (kgC/m2)  cumulative SUM of soil respiration */
	double cumN2Oflux;					/* (kgN/m2)  cumulative SUM N2O flux */
	double cumN2OfluxCeq;				/* (kgC/m2)  cumulative SUM N2O flux in C eq.*/
	double cumCH4flux;					/* (kgC/m2)  cumulative SUM CH4 flux */
	double cumCloss_MGM;				/* (kgC/m2)  cumulative SUM of management carbon loss  */
	double cumCplus_MGM;				/* (kgC/m2)  cumulative SUM of management carbon plus  */
	double cumCloss_THN_w;				/* (kgC/m2)  cumulative SUM of thinning woody carbon loss  */
	double cumCloss_THN_nw;			    /* (kgC/m2)  cumulative SUM of thinning non-woody carbon loss  */
	double cumCloss_MOW;				/* (kgC/m2)  cumulative SUM of mowing carbon loss */
	double cumCloss_HRV;				/* (kgC/m2)  cumulative SUM of harvesting carbon loss */
	double cumYieldC_HRV;				/* (kgC/m2)  cumulative SUM of harvested yield */
	double cumCloss_PLG;				/* (kgC/m2)  cumulative SUM of plouging carbon loss   */
	double cumCloss_GRZ;				/* (kgC/m2)  cumulative SUM of grazing carbon loss   */
	double cumCplus_GRZ;				/* (kgC/m2)  cumulative SUM of grazing carbon plus   */
	double cumCplus_FRZ;				/* (kgC/m2)  cumulative SUM of fertilizing carbon plus */
	double cumCplus_PLT;				/* (kgC/m2)  cumulative SUM of planting carbon plus  */
	double cumCloss_PLT;				/* (kgC/m2)  cumulative SUM of planting carbon loss  */
	double cumNplus_GRZ;				/* (kgN/m2)  cumulative SUM of grazing nitrogen plus   */
	double cumNplus_FRZ;				/* (kgN/m2)  cumulative SUM of fertilizing nitrogen plus   */
	double cumCloss_SNSC;				/* (kgC/m2)  cumulative SUM of senescence carbon loss  */
	double cumCplus_STDB;				/* (kgC/m2)  cumulative SUM of standing dead biome carbon plus  */
	double cumCplus_CTDB;				/* (kgC/m2)  cumulative SUM of cut-down dead biome carbon plus  */
	double cumEVP;						/* (kgH2O/m2) cumulative SUM of evaporation over a year */
	double cumTRP;						/* (kgH2O/m2) cumulative SUM of transpiration over a year */
	double cumET;						/* (kgH2O/m2) cumulative SUM of evapotranspiration over a year */
	double leafCN;						/* (kgC/kgN) CN ratio of leaves (live+standing dead) */
    double frootCN;						/* (kgC/kgN)) CN ratio of fine roots (live+standing dead) */
	double yieldN;						/* (kgC/kgN)) CN ratio of yields (live+standing dead) */
    double softstemCN;					/* (kgC/kgN)) CN ratio of softstems (live+standing dead) */
	double leafDM;						/* (kgDM/m2) dry matter carbon content of leaves */
	double leaflitrDM;					/* (kgDM/m2)  dry matter carbon content of  for leaf litter */
    double frootDM;						/* (kgDM/m2) dry matter content of fine roots */
	double yieldDM;						/* (kgDM/m2) dry matter content of yields */
    double softstemDM;					/* (kgDM/m2) dry matter content of softstems */
    double livewoodDM;					/* (kgDM/m2) dry matter content of live wood */
	double deadwoodDM;					/* (kgDM/m2) dry matter content of dead wood */
	double yieldDM_HRV;                 /* (kgDM/m2) dry matter content of yield at harvest */
	double vegC;						/* (kgC/m2)  total vegetation C */
	double LDaboveC_nw;					/* (kgC/m2)  living+dead aboveground non-woody biomass C */
	double LDaboveC_w;					/* (kgC/m2)  living+dead aboveground woody biomass C */
	double LDaboveCnsc_nw;				/* (kgC/m2)  living+dead aboveground non-woody biomass C with non-structured carbohydrate */
	double LDaboveCnsc_w;				/* (kgC/m2)  living+dead aboveground woody biomass C with non-structured carbohydrate  */
	double LaboveC_nw;					/* (kgC/m2)  living aboveground non-woody biomass C */
	double LaboveC_w;					/* (kgC/m2)  living aboveground woody biomass C */
	double LaboveCnsc_nw;				/* (kgC/m2)  living aboveground non-woody biomass C with non-structured carbohydrate */
	double LaboveCnsc_w;				/* (kgC/m2)  living aboveground woody biomass C with non-structured carbohydrate  */
	double DaboveC_nw;					/* (kgC/m2)  dead aboveground non-woody biomass C */
	double DaboveC_w;					/* (kgC/m2)  dead aboveground woody biomass C */
	double DaboveCnsc_nw;				/* (kgC/m2)  dead aboveground non-woody biomass C with non-structured carbohydrate */
	double DaboveCnsc_w;				/* (kgC/m2)  dead aboveground woody biomass C with non-structured carbohydrate  */
	double LDbelowC_nw;					/* (kgC/m2)  living+dead belowground non-woody biomass C */
	double LDbelowC_w;					/* (kgC/m2)  living+dead belowground woody biomass C */
	double LDbelowCnsc_nw;				/* (kgC/m2)  living+dead belowground non-woody biomass C with non-structured carbohydrate */
	double LDbelowCnsc_w;				/* (kgC/m2)  living+dead belowground woody biomass C with non-structured carbohydrate  */
	double LbelowC_nw;					/* (kgC/m2)  living belowground non-woody biomass C */
	double LbelowC_w;					/* (kgC/m2)  living belowground woody biomass C */
	double LbelowCnsc_nw;				/* (kgC/m2)  living belowground non-woody biomass C with non-structured carbohydrate */
	double LbelowCnsc_w;				/* (kgC/m2)  living belowground woody biomass C with non-structured carbohydrate  */
	double DbelowC_nw;					/* (kgC/m2)  dead belowground non-woody biomass C */
	double DbelowC_w;					/* (kgC/m2)  dead belowground woody biomass C */
	double DbelowCnsc_nw;				/* (kgC/m2)  dead belowground non-woody biomass C with non-structured carbohydrate */
	double DbelowCnsc_w;				/* (kgC/m2)  dead belowground woody biomass C with non-structured carbohydrate  */
	double livingBIOMabove;				/* (kg/m2)  living aboveground biomass */
	double livingBIOMbelow;				/* (kg/m2)  living belowground biomass */
	double totalBIOMabove;				/* (kg/m2)  living aboveground biomass */
	double totalBIOMbelow;				/* (kg/m2)  living belowground biomass */
	double annmax_livingBIOMabove;		/* (kgC/m2)  annual maximum living aboveground biomass */
	double annmax_livingBIOMbelow;		/* (kgC/m2)  annual maximum living belowground biomass */
	double annmax_totalBIOMabove;		/* (kgC/m2)  annual maximum living aboveground biomass */
	double annmax_totalBIOMbelow;		/* (kgC/m2)  annual maximum living belowground biomass */
	double litrCwdC_total;				/* (kgC/m2)  total litter and cwdc C */
	double litrCwdN_total;				/* (kgC/m2)  total litter and cwdc N */
	double litrN_total;					/* (kgC/m2)  total litter N */
	double litrC_total;					/* (kgC/m2)  total litter C */
	double soilC_total;					/* (kgC/m2)  total soil C */
	double soilN_total;					/* (kgN/m2)  total soil N */
	double sminN_total;					/* (kgN/m2)  total soil mineralized N */
	double sminNavail_total;			/* (kgN/m2)  available total soil mineralized N */
	double sminNavail_maxRZ;			/* (kgN/m2)  available soil mineralized N in maximal rooting zone */
	double sminN_maxRZ;			    	/* (kgN/m2)  soil mineralized N in rooting zone */
	double soilC_maxRZ;			    	/* (kgC/m2)  soil carbon content in rooting zone */
	double soilN_maxRZ;			    	/* (kgN/m2)  soil nitrogen content in rooting zone */
	double litrC_maxRZ;			    	/* (kgC/m2)  litter carbon content in rooting zone */
	double litrN_maxRZ;			    	/* (kgN/m2)  litter nitrogen content in rooting zone */
	double stableSOC_top30;				/* (%)  humus C content in 0-30 cm */
	double totalC;						/* (kgC/m2)  total of vegc, litrc, and soilc */
	double SOM_C_top30;					/* (%)  soil organic matter C content in 0-30 cm [carbon/soil] */
	double SOM_N_top30;					/* (%)  soil organic matter N content in 0-30 cm [nitrogen/soil] */
	double NH4_top30avail;				/* (ppm)  available soil NH4-N content in 0-30 cm */
	double NO3_top30avail;				/* (ppm)  available soil NO3-N content in 0-30 cm */
	double sminN_top30avail;			/* (ppm)  available soil mineralized N-content in 0-30 cm */
	double SOM_C[N_SOILLAYERS];	        /* (%)  soil organic matter C content [carbon/soil] */
	double leafc_LandD;                 /* (kgC/m2)  live and dead leaf carbon content */
	double frootc_LandD;                /* (kgC/m2)  live and dead froot carbon content */
	double yield_LandD;                 /* (kgC/m2)  live and dead yield carbon content */
	double softstemc_LandD;             /* (kgC/m2)  live and dead sofstem carbon content */
	double sminNH4_ppm[N_SOILLAYERS];	/* (ppm)  soil ammonium content in ppm */
	double sminNO3_ppm[N_SOILLAYERS];	/* (ppm)  soil nitrate content in ppm */
	double CH4flux_total;               /* (kgC/m2/d) total ecosystem CH4 flux */
	double Cflux_lateral;               /* (kgC/m2/d) lateral carbon flux */
	double cumCflux_lateral;            /* (kgC/m2)  cumulative lateral carbon flux */
	double harvestIndex;                /* (prop) ratio of harvested yield C content and harvested aboveground C content */
	double rootIndex;                   /* (prop) ratio of fine root C content and vegetation C content on harvest day */
	double belowground_ratio;           /* (prop) ratio fine root to total vegetation C content */
	double CNlitr_total;		        /* (prop) C:N ratio of litter pool  */
	double CNsoil_total;				/* (prop) C:N ratio of soil pool  */
	double litr1HR_total;				/* (kgC/m2/d) Heterotroph respiration of labile litter in soil */
	double litr2HR_total;				/* (kgC/m2/d) Heterotroph respiration of unshielded cellulose soil */
	double litr4HR_total;				/* (kgC/m2/d) Heterotroph respiration of lignin portion of litter in soil */
	double soil1HR_total;				/* (kgC/m2/d) Heterotroph respiration of SOM pool (labile) in soil */
	double soil2HR_total;				/* (kgC/m2/d) Heterotroph respiration of SOM pool (fast) in soil */
	double soil3HR_total;				/* (kgC/m2/d) Heterotroph respiration of SOM pool (slow) in soil */
	double soil4HR_total;				/* (kgC/m2/d) Heterotroph respiration of SOM pool (stable) in soil */
	double tally1;                      /* (kgC/m2) tally of total soil C during successive met cycles (metcyle=1) for comparison */
	double tally2;                      /* (kgC/m2) tally of total soil C during successive met cycles (metcyle=2) for comparison */
	double metcycle;                    /* (dimless) counter for metcyles (0,1 or 2) */
	double steady1;                     /* (flag) marker for comparison of soilC change and spinup tolerance in metcyle=1 */
	double steady2;                     /* (flag) marker for comparison of soilC change and spinup tolerance in metcyle=2 */
} summary_struct;

/* endOUT */

/* OUT summary: structure for summarzing variables */
typedef struct
{
	double plantDensity;
	double FRZamount;
	double IRGamount;
} in2out_struct;

/* VAR restart: restart data structure */
typedef struct
{
	double soilw[N_SOILLAYERS];		/* (kgH2O/m2) soil water content */
	double snoww;					/* (kgH2O/m2) snow water content */
	double canopyw;					/* (kgH2O/m2) canpoy water content */
	double leafc;					/* (kgC/m2) leaf carbon actual pool  */
	double leafc_storage;			/* (kgC/m2) leaf carbon storage pool  */
	double leafc_transfer;			/* (kgC/m2) leaf carbon transfer pool  */
	double frootc;					/* (kgC/m2) froot carbon actual pool  */
	double frootc_storage;			/* (kgC/m2) froot carbon storage pool  */
	double frootc_transfer;			/* (kgC/m2) froot carbon transfer pool  */
	double yield;					/* (kgC/m2) yield carbon actual pool  */
	double yieldc_storage;			/* (kgC/m2) yield carbon storage pool  */
	double yieldc_transfer;			/* (kgC/m2) yield carbon transfer pool  */
	double softstemc;				/* (kgC/m2) softstem carbon actual pool  */
	double softstemc_storage;		/* (kgC/m2) softstem carbon storage pool  */
	double softstemc_transfer;		/* (kgC/m2) softstem carbon transfer pool  */
	double livestemc;				/* (kgC/m2) livestem carbon actual pool  */
	double livestemc_storage;		/* (kgC/m2) livestem carbon storage pool  */
	double livestemc_transfer;		/* (kgC/m2) livestem carbon transfer pool  */
	double deadstemc;				/* (kgC/m2) deadstem carbon actual pool  */
	double deadstemc_storage;		/* (kgC/m2) deadstem carbon storage pool  */
	double deadstemc_transfer;		/* (kgC/m2) deadstem carbon transfer pool  */
	double livecrootc;				/* (kgC/m2) livecroot carbon actual pool  */
	double livecrootc_storage;		/* (kgC/m2) livecroot carbon storage pool  */
	double livecrootc_transfer;		/* (kgC/m2) livecroot carbon transfer pool  */
	double deadcrootc;				/* (kgC/m2) deadcroot carbon actual pool  */
	double deadcrootc_storage;		/* (kgC/m2) deadcroot carbon storage pool  */
	double deadcrootc_transfer;		/* (kgC/m2) deadcroot carbon transfer pool  */ 
	double gresp_storage;			/* (kgC/m2) gresp carbon storage pool  */
	double gresp_transfer;			/* (kgC/m2) gresp carbon transfer pool  */
	double cwdc[N_SOILLAYERS];		/* (kgC/m2) coarse woody debris carbon pool  */
	double litr1c[N_SOILLAYERS];	/* (kgC/m2) labile litter carbon pool  */
	double litr2c[N_SOILLAYERS];	/* (kgC/m2) litter unshielded cellulose carbon pool  */
	double litr3c[N_SOILLAYERS];	/* (kgC/m2) litter shielded cellulose carbon pool  */
	double litr4c[N_SOILLAYERS];	/* (kgC/m2) litter lignin carbon pool  */
	double STDBc_leaf;				/*  wilted leaf biomass  */
	double STDBc_froot;				/*  wilted froot biomass  */
	double STDBc_yield;				/*  wilted yield biomass  */
	double STDBc_softstem;			/*  wilted sofstem biomass  */
	double CTDBc_leaf;				/*  cut-down leaf biomass  */
	double CTDBc_froot;				/*  cut-down froot biomass  */
	double CTDBc_yield;				/*  cut-down yield biomass  */
	double CTDBc_softstem;			/*  cut-down sofstem biomass  */
	double CTDBc_cstem;			    /*  cut-down coarse stem biomass  */
	double CTDBc_croot;			    /*  cut-down coarse root biomass  */
	double soil1c[N_SOILLAYERS];	/* (kgC/m2) carbon content of labile SOM */
	double soil2c[N_SOILLAYERS];	/* (kgC/m2) carbon content of fast decomposing SOM */
	double soil3c[N_SOILLAYERS];	/* (kgC/m2) carbon content of slowly decomposing SOM */ 
	double soil4c[N_SOILLAYERS];	/* (kgC/m2) carbon content of stable SOM */
	double cpool;					/* (kgC/m2) temporary photosynthate C pool */
	double leafn;					/* (kgN/m2) leaf carbon actual pool  */
	double leafn_storage;			/* (kgN/m2) leaf carbon storage pool  */
	double leafn_transfer;			/* (kgN/m2) leaf carbon transfer pool  */
	double frootn;					/* (kgN/m2) froot carbon actual pool  */
	double frootn_storage;			/* (kgN/m2) froot carbon storage pool  */
	double frootn_transfer;			/* (kgN/m2) froot carbon transfer pool  */
	double yieldn;					/* (kgN/m2) yield carbon actual pool  */
	double yieldn_storage;			/* (kgN/m2) yield carbon storage pool  */
	double yieldn_transfer;			/* (kgN/m2) yield carbon transfer pool  */
	double softstemn;				/* (kgN/m2) softstem carbon actual pool  */
	double softstemn_storage;		/* (kgN/m2) softstem carbon actual pool  */
	double softstemn_transfer;		/* (kgN/m2) softstem carbon actual pool  */
	double livestemn;				/* (kgN/m2) livestem carbon actual pool  */
	double livestemn_storage;		/* (kgN/m2) livestem carbon storage pool  */
	double livestemn_transfer;		/* (kgN/m2) livestem carbon transfer pool  */
	double deadstemn;				/* (kgN/m2) deadstem carbon actual pool  */
	double deadstemn_storage;		/* (kgN/m2) deadstem carbon storage pool  */
	double deadstemn_transfer;		/* (kgN/m2) deadstem carbon transfer pool  */
	double livecrootn;				/* (kgN/m2) livecroot carbon actual pool  */
	double livecrootn_storage;		/* (kgN/m2) livecroot carbon storage pool  */
	double livecrootn_transfer;		/* (kgN/m2) livecroot carbon transfer pool  */
	double deadcrootn;				/* (kgN/m2) deadcroot carbon actual pool  */
	double deadcrootn_storage;		/* (kgN/m2) deadcroot carbon storage pool  */
	double deadcrootn_transfer;		/* (kgN/m2) deadcroot carbon transfer pool  */
	double cwdn[N_SOILLAYERS];		/* (kgN/m2) coarse woody debris carbon pool  */
	double litr1n[N_SOILLAYERS];	/* (kgN/m2) labile litter carbon pool  */
	double litr2n[N_SOILLAYERS];	/* (kgN/m2) litter unshielded cellulose carbon pool  */
	double litr3n[N_SOILLAYERS];	/* (kgN/m2) litter shielded cellulose carbon pool  */
	double litr4n[N_SOILLAYERS];	/* (kgN/m2) litter lignin carbon pool  */
	double litrc_above;				/* (kgC/m2) estimated aboveground carbon pool  */
	double cwdc_above;				/* (kgN/m2) estimated aboveground coarse woody debris pool   */
	double STDBn_leaf;				/*  wilted leaf biomass  */
	double STDBn_froot;				/*  wilted froot biomass  */
	double STDBn_yield;				/*  wilted yield biomass  */
	double STDBn_softstem;			/*  wilted sofstem biomass  */
	double CTDBn_leaf;				/*  cut-down leaf biomass  */
	double CTDBn_froot;				/*  cut-down froot biomass  */
	double CTDBn_yield;				/*  cut-down yield biomass  */
	double CTDBn_softstem;			/*  cut-down sofstem biomass  */
	double CTDBn_cstem;			    /*  cut-down coarse stem biomass  */
	double CTDBn_croot;			    /*  cut-down coarse root biomass  */
	double soil1n[N_SOILLAYERS];	/* (kgN/m2) Nitrogen content of labile SOM */
	double soil2n[N_SOILLAYERS];	/* (kgN/m2) Nitrogen content of fast decomposing SOM  */
	double soil3n[N_SOILLAYERS];	/* (kgN/m2) Nitrogen content of slowly decomposing SOM  */ 
	double soil4n[N_SOILLAYERS];	/* (kgN/m2) Nitrogen content of stable SOM  */
	double retransn;				/* (kgN/m2) retranslocated N */
	double npool;					/* (kgN/m2) temporary plant N pool */
	double sminNH4[N_SOILLAYERS];	/* (kgN/m2) mineralized NH4 pool*/
	double sminNO3[N_SOILLAYERS];	/* (kgN/m2) mineralized NO3 pool */
	double annmax_leafc;			/* (kgC/m2) annual maximum daily leaf carbon */
	double annmax_frootc;			/* (kgC/m2) annual maximum daily froot carbon */
	double annmax_yieldc;			/* (kgC/m2) annual maximum daily yield carbon  */
	double annmax_softstemc;		/* (kgC/m2) annual maximum daily softstem carbon  */
	double annmax_livestemc;		/* (kgC/m2) annual maximum daily livestem carbon */
	double annmax_livecrootc;		/* (kgC/m2) annual maximum daily livecroot carbon */

/* endVAR*/

} restart_data_struct;
