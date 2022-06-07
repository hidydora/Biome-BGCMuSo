/*
bgc_struct.h 
header file for structure definitions

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Biome-BGC version 4.1.1
Copyright 2000, Peter E. Thornton
Numerical Terradynamics Simulation Group (NTSG)
School of Forestry, University of Montana
Missoula, MT 59812
*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Modified:
4/17/2000 (PET): Added new nf structure element (sminn_to_denitrif). This is
part of a larger modification that increases denitrification in the presence
of excess soil mineral N.
Modified:
13/07/2000: Added input of Ndep from file => added new ramp_ndep structure
element ndep_array (array). Changes are made by Galina Churkina.
*/

#define N_POOLS 3			/* Hidy 2010 - number of type of pools: water, carbon, nitrogen */
/* simulation control variables */
typedef struct
{
	int metyears;          /* # years of met data */
    int simyears;          /* # years of simulation */
    int simstartyear;      /* first year of simulation */
	int spinup;            /* (flag) 1=spinup run, 0=normal run */
	int maxspinyears;      /* maximum number of years for spinup run */
	int dodaily;           /* flag for daily output */
	int domonavg;          /* flag for monthly average of daily outputs */
	int doannavg;          /* flag for annual average of daily outputs */
	int doannual;          /* flag for annual output */
	int ndayout;           /* number of daily outputs */
	int nannout;           /* number of annual outputs */
	int* daycodes;         /* array of indices for daily outputs */
	int* anncodes;         /* array of indices for annual outputs */
	int read_restart;      /* flag to read restart file */
	int write_restart;     /* flag to write restart file */
	int keep_metyr;        /* (flag) 1=retain restart metyr, 0=reset metyr */
	int onscreen;          /* (flag) 1=show progress on-screen 0=don't */

	/*  !!!!!!!!!!!!!!!!!! Hidy 2009 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */	
	int GSI_flag;		/*(flag) 1=if onday and offday calc. is not user-defined (epc file), use GSI_calculation to determine onday&offday*/
	int THN_flag;
	int MOW_flag;
	int GRZ_flag;
	int HRV_flag;
	int PLG_flag;
	int PLT_flag;
	int FRZ_flag;
	int simyr;			     /* Hidy 2010 - counter */
	int yday;			     /* Hidy 2010 - counter */
	int spinyears;		     /* Hidy 2010 - counter */
	int n_limitation;	     /* Hidy 2010 - flag for nitrogen limitation */
	int varWPM_flag;	     /* Hidy 2012 - changing WPM value */
	int varMSC_flag;	     /* Hidy 2012 - changing MSC value */
	double vegperday;        /* Hidy 2013 - counter */
	double vegperday_pre;    /* Hidy 2013 - counter */
	int vegper_flag;         /* Hidy 2013 - flag: start and end of VP */
} control_struct;

/* a structure to hold information about ramped N-deposition scenario */
typedef struct
{
	int doramp;         	/* (flag) 1=ramped Ndep, 0=constant Ndep */
	int ind_year;          	/* (int)  reference year for indust. Ndep */
	double preind_ndep;    	/* (double)  (kgN/m2/yr) preindustrial Ndep (at first metyear) */
	double ind_ndep;       	/* (double)  (kgN/m2/yr) industrial Ndep at ref yr */
	double* ndep_array;	/* (kgN/m2/yr) annual Ndep array*/
} ramp_ndep_struct;

/* a structure to hold information on the annual co2 concentration */
typedef struct
{
	int varco2;             /* (flag) 0=const 1=use file 2=const,file for Ndep */
    double co2ppm;          /* (ppm)     constant CO2 concentration */
	double* co2ppm_array;   /* (ppm)     annual CO2 concentration array */
} co2control_struct;	

/* meteorological variable arrays */
/* inputs from mtclim, except tavg and tavg_ra
which are used for an 11-day running average of daily average air T,
computed for the whole length of the met array prior to the 
daily model loop */
typedef struct
{
    double* tmax;          /* (deg C) daily maximum air temperature */
    double* tmin;          /* (deg C) daily minimum air temperature */
    double* prcp;          /* (cm)    precipitation */
    double* vpd;           /* (Pa)    vapor pressure deficit */
    double* swavgfd;       /* (W/m2)  daylight avg shortwave flux density */
    double* par;           /* (W/m2)  photosynthetically active radiation */
    double* dayl;          /* (s)     daylength */
	double* tday;          /* (deg C) daylight average temperature */
	double* tavg;          /* (deg C) daily average temperature */
    double* tavg_ra;       /* (deg C) 11-day running avg of daily avg temp */
} metarr_struct;


#define N_SOILLAYERS 5
/* daily values that are passed to daily model subroutines */
typedef struct
{
	double prcp;							 /* (kg/m2) precipitation */
	double tmax;							 /* (deg C) daily maximum air temperature */
	double tmin;							 /* (deg C) daily minimum air temperature */
	double tavg;							 /* (deg C) daily average air temperature */
	double tday;							 /* (deg C) daylight average air temperature */
	double tnight;							 /* (deg C) nightime average air temperature */
	double tavg_ra;						 /* (deg C) Hidy 2010 - 11-day running average air temperature */
	double tday_pre;						 /* (deg C) Hidy 2010 - daily average air temperature on the previous day */
	double tsoil[N_SOILLAYERS];				 /* (deg C) Hidy 2010 - daily soil layer temperature */
	double tsoil_change[N_SOILLAYERS];		 /* (deg C) Hidy 2010 - daily soil temperature change */
	double tsoil_avg;						 /* (deg C) Hidy 2010 - average soil temperature */
	double tsoil_top_pre;					 /* (deg C) Hidy 2010 -  daily soil surface temperature of the previous day */
	double vpd;								 /* (Pa)    vapor pressure deficit */
	double swavgfd;							 /* (W/m2)  daylight average shortwave flux */
	double swabs;							 /* (W/m2)  canopy absorbed shortwave flux */ 
	double swtrans;							 /* (W/m2)  transmitted shortwave flux */
	double swabs_per_plaisun;				 /* (W/m2) swabs per unit sunlit proj LAI */
	double swabs_per_plaishade;				 /* (W/m2) swabs per unit shaded proj LAI */
	double ppfd_per_plaisun;				 /* (umol/m2/s) ppfd per unit sunlit proj LAI */
	double ppfd_per_plaishade;				 /* (umol/m2/s) ppfd per unit shaded proj LAI */
	double par;								 /* (W/m2)  photosynthetically active radiation */
	double parabs;							 /* (W/m2)  PAR absorbed by canopy */
	double pa;								 /* (Pa)    atmospheric pressure */
	double co2;								 /* (ppm)   atmospheric concentration of CO2 */
	double dayl;							 /* (s)     daylength */
} metvar_struct;

/* water state variables (including sums for sources and sinks) */
typedef struct
{
    double soilw[N_SOILLAYERS];		 /* (kgH2O/m2) water stored in the soil layers; Hidy 2010 - Hidy 2010: multilayer soil */
	double soilw_SUM;				 /* (kgH2O/m2) water stored in soil; Hidy 2010 - Hidy 2010: multilayer soil */
	double pond_water;				 /* (kgH2O/m2) water stored on surface which can not infiltrated because of saturation - Hidy 2010 - Hidy 2010: multilayer soil */
    double snoww;					 /* (kgH2O/m2) water stored in snowpack */
    double canopyw;					 /* (kgH2O/m2) water stored on canopy */
    double prcp_src;				 /* (kgH2O/m2) SUM of precipitation */
    double soilevap_snk;			 /* (kgH2O/m2) SUM of soil water evaporation */
    double snowsubl_snk;			 /* (kgH2O/m2) SUM of snow water sublimation */
    double canopyevap_snk;			 /* (kgH2O/m2) SUM of canopy water evaporation */
    double trans_snk;				 /* (kgH2O/m2) SUM of transpiration */
	/* soil-water submodel - by Hidy 2008. */
	double runoff_snk;			 	 /* (kgH2O/m2) SUM of runoff */
	double deeppercolation_snk;		 /* (kgH2O/m2) SUM of percolated water out of the system */
	double deepdiffusion_snk;		 /* (kgH2O/m2) SUM of percolated water out of the system */
	/* thinning - Hidy 2012. */
	double canopyw_THNsnk;				/* (kgH2O/m2) water stored on canopy is disappered because of thinning*/
	/* mowing - Hidy 2008. */
	double canopyw_MOWsnk;				/* (kgH2O/m2) water stored on canopy is disappered because of mowing*/
	/* harvesting - Hidy 2012. */
	double canopyw_HRVsnk;
	/* ploughing - Hidy 2012. */
	double canopyw_PLGsnk;				/* (kgH2O/m2) water stored on canopy is disappered because of harvesting*/
	/* grazing - Hidy 2009. */
	double canopyw_GRZsnk;				/* (kgH2O/m2) water stored on canopy is disappered because of grazing*/
	
} wstate_struct;

/* water flux variables */
typedef struct
{
    double prcp_to_canopyw;							/* (kgH2O/m2/d) interception on canopy */
    double prcp_to_soilw;							/* (kgH2O/m2/d) precip entering soilwater pool  -  Hidy 2010: on the top soil layer */
    double prcp_to_snoww;							/* (kgH2O/m2/d) snowpack accumulation */
	double prcp_to_runoff;						    /* (kgH2O/m2/d) Hidy 2010 - runoff flux */
    double canopyw_evap;							/* (kgH2O/m2/d) evaporation from canopy */
    double canopyw_to_soilw;						/* (kgH2O/m2/d) canopy drip and stemflow -  Hidy 2010: on the top soil layer */
    double snoww_subl;								/* (kgH2O/m2/d) sublimation from snowpack */
    double snoww_to_soilw;							/* (kgH2O/m2/d) melt from snowpack -  Hidy 2010: on the top soil layer */
    double soilw_evap;								/* (kgH2O/m2/d) evaporation from soil */
    double soilw_trans[N_SOILLAYERS];				/* (kgH2O/m2/d) Hidy 2010 - transpiration from the soil layers */
    double soilw_trans_SUM;							/* (kgH2O/m2/d) Hidy 2010 - sum of transpiration */  
	double soilw_percolated[N_SOILLAYERS];		/* (kgH2O/m2/d) Hidy 2010 - percolation fluxes between soil layers */
	double soilw_diffused[N_SOILLAYERS];			/* (kgH2O/m2/d) Hidy 2010 - diffusion flux between the soil layers (positive: downward) */
	/* thinning - by Hidy 2008. */
	double canopyw_to_THN;							/* (kgH2O/m2/d) water stored on canopy is disappered because of thinning*/
	/* mowing - by Hidy 2008. */
	double canopyw_to_MOW;							/* (kgH2O/m2/d) water stored on canopy is disappered because of mowing*/
	/* harvesting - by Hidy 2008.*/
	double canopyw_to_HRV;							/* (kgH2O/m2/d) water stored on canopy is disappered because of harvesting */
	/* ploughing - Hidy 2012. */
	double canopyw_to_PLG;							/* (kgH2O/m2/d) water stored on canopy is disappered because of ploughing */
	/* grazing - by Hidy 2009. */
	double canopyw_to_GRZ;							/* (kgH2O/m2/d) water stored on canopy is disappered because of grazing*/

} wflux_struct;

/* carbon state initialization structure */
typedef struct
{
	double max_leafc;      /* (kgC/m2) first-year displayed + stored leafc */
	double max_stemc;      /* (kgC/m2) first-year total stem carbon */
} cinit_struct;

/* carbon state variables (including sums for sources and sinks) */
typedef struct 
{
    double leafc;            /* (kgC/m2) leaf C */
    double leafc_storage;    /* (kgC/m2) leaf C storage */
    double leafc_transfer;   /* (kgC/m2) leaf C transfer */
    double frootc;           /* (kgC/m2) fine root C */
    double frootc_storage;   /* (kgC/m2) fine root C storage */
    double frootc_transfer;  /* (kgC/m2) fine root C transfer */
    double livestemc;        /* (kgC/m2) live stem C */
    double livestemc_storage;  /* (kgC/m2) live stem C storage */
    double livestemc_transfer; /* (kgC/m2) live stem C transfer */
    double deadstemc;          /* (kgC/m2) dead stem C */
    double deadstemc_storage;  /* (kgC/m2) dead stem C storage */
    double deadstemc_transfer; /* (kgC/m2) dead stem C transfer */
    double livecrootc;         /* (kgC/m2) live coarse root C */
    double livecrootc_storage; /* (kgC/m2) live coarse root C storage */
    double livecrootc_transfer;/* (kgC/m2) live coarse root C transfer */
    double deadcrootc;         /* (kgC/m2) dead coarse root C */
    double deadcrootc_storage; /* (kgC/m2) dead coarse root C storage */
    double deadcrootc_transfer;/* (kgC/m2) dead coarse root C transfer */
	double gresp_storage;    /* (kgC/m2) growth respiration storage */
	double gresp_transfer;   /* (kgC/m2) growth respiration transfer */
    double cwdc;             /* (kgC/m2) coarse woody debris C */
    double litr1c;           /* (kgC/m2) litter labile C */
    double litr2c;           /* (kgC/m2) litter unshielded cellulose C */
    double litr3c;           /* (kgC/m2) litter shielded cellulose C */
    double litr4c;           /* (kgC/m2) litter lignin C */
	/* Hidy 2013: senescence */
	double litr1c_strg_SNSC;
	double litr2c_strg_SNSC;
	double litr3c_strg_SNSC;
	double litr4c_strg_SNSC;
    double soil1c;           /* (kgC/m2) microbial recycling pool C (fast) */
    double soil2c;           /* (kgC/m2) microbial recycling pool C (medium) */
    double soil3c;           /* (kgC/m2) microbial recycling pool C (slow) */
    double soil4c;           /* (kgC/m2) recalcitrant SOM C (humus, slowest) */
	double cpool;            /* (kgC/m2) temporary photosynthate C pool */
    double psnsun_src;       /* (kgC/m2) SUM of gross PSN from sulit canopy */
    double psnshade_src;     /* (kgC/m2) SUM of gross PSN from shaded canopy */
    double leaf_mr_snk;      /* (kgC/m2) SUM of leaf maint resp */
    double leaf_gr_snk;      /* (kgC/m2) SUM of leaf growth resp */
    double froot_mr_snk;     /* (kgC/m2) SUM of fine root maint resp */
    double froot_gr_snk;     /* (kgC/m2) SUM of fine root growth resp */
    double livestem_mr_snk;  /* (kgC/m2) SUM of live stem maint resp */
    double livestem_gr_snk;  /* (kgC/m2) SUM of live stem growth resp */
    double deadstem_gr_snk;  /* (kgC/m2) SUM of dead stem growth resp */
    double livecroot_mr_snk; /* (kgC/m2) SUM of live coarse root maint resp */
    double livecroot_gr_snk; /* (kgC/m2) SUM of live coarse root growth resp */
    double deadcroot_gr_snk; /* (kgC/m2) SUM of dead coarse root growth resp */
    double litr1_hr_snk;     /* (kgC/m2) SUM of labile litr microbial resp */
    double litr2_hr_snk;     /* (kgC/m2) SUM of cellulose litr microbial resp */
    double litr4_hr_snk;     /* (kgC/m2) SUM of lignin litr microbial resp */
    double soil1_hr_snk;     /* (kgC/m2) SUM of fast microbial respiration */
    double soil2_hr_snk;     /* (kgC/m2) SUM of medium microbial respiration */
    double soil3_hr_snk;     /* (kgC/m2) SUM of slow microbial respiration */
    double soil4_hr_snk;     /* (kgC/m2) SUM of recalcitrant SOM respiration */
	double fire_snk;         /* (kgC/m2) SUM of fire losses */
	double SNSC_snk;		 /* (kgC/m2) SUM of senescence losses */
	double SNSC_src;		 /* (kgC/m2) SUM of wilted plant material which turns into the litter pool */
	/* planting - by Hidy 2012. */
    double PLTsrc;     /* (kgN/m2) planted N */
	/* thinning - by Hidy 2012.  */
	double THNsnk;              /* (kgC/m2) thinned leaf C */
	double THNsrc;			  /* (kgC/m2) thinned plant material (C content) returns to the litter  */
	/* mowing - by Hidy 2008.  */
	double MOWsnk;              /* (kgC/m2) mowed leaf C */
	double MOWsrc;			  /* (kgC/m2) mowed plant material (C content) returns to the litter */
	/* harvesting - Hidy 2012.  */
	double HRVsnk;             /* (kgC/m2) harvested leaf C */
	double HRVsrc;			   /* (kgC/m2) harvested plant material (C content) returns to the litter */
	/* ploughing - Hidy 2012. */
	double PLGsnk;              /* (kgC/m2) ploughed leaf C */
	double PLGsrc;			  /* (kgC/m2) ploughed plant material (C content)to the soil (labile litter) */
	/* grazing - by Hidy 2009.  */
	double GRZsnk;              /* (kgC/m2) grazed leaf C */
	double GRZsrc;              /* (kgC/m2) added carbon from fertilizer */
	/* fertilizing - Hidy 2009. */
	double FRZsrc;			  /* (kgC/m2) carbon content of fertilizer return to the litter pool */
} cstate_struct;

/* daily carbon flux variables */
typedef struct
{
	/* mortality fluxes */ 
	double m_leafc_to_litr1c;              /* (kgC/m2/d) */            
	double m_leafc_to_litr2c;              /* (kgC/m2/d) */
	double m_leafc_to_litr3c;              /* (kgC/m2/d) */
	double m_leafc_to_litr4c;              /* (kgC/m2/d) */
	double m_frootc_to_litr1c;             /* (kgC/m2/d) */
	double m_frootc_to_litr2c;             /* (kgC/m2/d) */
	double m_frootc_to_litr3c;             /* (kgC/m2/d) */
	double m_frootc_to_litr4c;             /* (kgC/m2/d) */
	double m_leafc_storage_to_litr1c;      /* (kgC/m2/d) */
	double m_frootc_storage_to_litr1c;     /* (kgC/m2/d) */
	double m_livestemc_storage_to_litr1c;  /* (kgC/m2/d) */
	double m_deadstemc_storage_to_litr1c;  /* (kgC/m2/d) */
	double m_livecrootc_storage_to_litr1c; /* (kgC/m2/d) */
	double m_deadcrootc_storage_to_litr1c; /* (kgC/m2/d) */
	double m_leafc_transfer_to_litr1c;     /* (kgC/m2/d) */
	double m_frootc_transfer_to_litr1c;    /* (kgC/m2/d) */
	double m_livestemc_transfer_to_litr1c; /* (kgC/m2/d) */
	double m_deadstemc_transfer_to_litr1c; /* (kgC/m2/d) */
	double m_livecrootc_transfer_to_litr1c;/* (kgC/m2/d) */
	double m_deadcrootc_transfer_to_litr1c;/* (kgC/m2/d) */
	double m_livestemc_to_cwdc;            /* (kgC/m2/d) */
	double m_deadstemc_to_cwdc;            /* (kgC/m2/d) */
	double m_livecrootc_to_cwdc;           /* (kgC/m2/d) */
	double m_deadcrootc_to_cwdc;           /* (kgC/m2/d) */
	double m_gresp_storage_to_litr1c;      /* (kgC/m2/d) */
	double m_gresp_transfer_to_litr1c;     /* (kgC/m2/d) */
	/* fire fluxes */
	double m_leafc_to_fire;                /* (kgC/m2/d) */
	double m_frootc_to_fire;               /* (kgC/m2/d) */
	double m_leafc_storage_to_fire;        /* (kgC/m2/d) */
	double m_frootc_storage_to_fire;       /* (kgC/m2/d) */
	double m_livestemc_storage_to_fire;    /* (kgC/m2/d) */
	double m_deadstemc_storage_to_fire;    /* (kgC/m2/d) */
	double m_livecrootc_storage_to_fire;   /* (kgC/m2/d) */
	double m_deadcrootc_storage_to_fire;   /* (kgC/m2/d) */
	double m_leafc_transfer_to_fire;       /* (kgC/m2/d) */
	double m_frootc_transfer_to_fire;      /* (kgC/m2/d) */
	double m_livestemc_transfer_to_fire;   /* (kgC/m2/d) */
	double m_deadstemc_transfer_to_fire;   /* (kgC/m2/d) */
	double m_livecrootc_transfer_to_fire;  /* (kgC/m2/d) */
	double m_deadcrootc_transfer_to_fire;  /* (kgC/m2/d) */
	double m_livestemc_to_fire;            /* (kgC/m2/d) */
	double m_deadstemc_to_fire;            /* (kgC/m2/d) */
	double m_livecrootc_to_fire;           /* (kgC/m2/d) */
	double m_deadcrootc_to_fire;           /* (kgC/m2/d) */
	double m_gresp_storage_to_fire;        /* (kgC/m2/d) */
	double m_gresp_transfer_to_fire;       /* (kgC/m2/d) */
	double m_litr1c_to_fire;               /* (kgC/m2/d) */
	double m_litr2c_to_fire;               /* (kgC/m2/d) */
	double m_litr3c_to_fire;               /* (kgC/m2/d) */
	double m_litr4c_to_fire;               /* (kgC/m2/d) */
	double m_cwdc_to_fire;                 /* (kgC/m2/d) */
	/* Hidy 2010 - senescence fluxes */
	double m_leafc_to_SNSC;                /* (kgC/m2/d) */
	double m_frootc_to_SNSC;               /* (kgC/m2/d) */
	double m_leafc_storage_to_SNSC;                /* (kgC/m2/d) */
	double m_frootc_storage_to_SNSC;               /* (kgC/m2/d) */
	double m_leafc_transfer_to_SNSC;                /* (kgC/m2/d) */
	double m_frootc_transfer_to_SNSC;               /* (kgC/m2/d) */
	double m_gresp_storage_to_SNSC;
	double m_gresp_transfer_to_SNSC;
	double SNSC_to_litr1c;
	double SNSC_to_litr2c;
	double SNSC_to_litr3c;
	double SNSC_to_litr4c;
	/* phenology fluxes from transfer pool */
	double leafc_transfer_to_leafc;          /* (kgC/m2/d) */
	double frootc_transfer_to_frootc;        /* (kgC/m2/d) */
	double livestemc_transfer_to_livestemc;  /* (kgC/m2/d) */
	double deadstemc_transfer_to_deadstemc;  /* (kgC/m2/d) */
	double livecrootc_transfer_to_livecrootc;/* (kgC/m2/d) */
	double deadcrootc_transfer_to_deadcrootc;/* (kgC/m2/d) */
	/* leaf and fine root litterfall */
	double leafc_to_litr1c;              /* (kgC/m2/d) */
	double leafc_to_litr2c;              /* (kgC/m2/d) */
	double leafc_to_litr3c;              /* (kgC/m2/d) */
	double leafc_to_litr4c;              /* (kgC/m2/d) */
	double frootc_to_litr1c;             /* (kgC/m2/d) */
	double frootc_to_litr2c;             /* (kgC/m2/d) */
	double frootc_to_litr3c;             /* (kgC/m2/d) */
	double frootc_to_litr4c;             /* (kgC/m2/d) */
	/* maintenance respiration fluxes */
	double leaf_day_mr;                  /* (kgC/m2/d) */
	double leaf_night_mr;                /* (kgC/m2/d) */
	double froot_mr;                     /* (kgC/m2/d) */
	double livestem_mr;                  /* (kgC/m2/d) */
	double livecroot_mr;                 /* (kgC/m2/d) */
	/* photosynthesis flux */
	double psnsun_to_cpool;              /* (kgC/m2/d) */
	double psnshade_to_cpool;            /* (kgC/m2/d) */
	/* litter decomposition fluxes */
	double cwdc_to_litr2c;               /* (kgC/m2/d) */
	double cwdc_to_litr3c;               /* (kgC/m2/d) */
	double cwdc_to_litr4c;               /* (kgC/m2/d) */
	double litr1_hr;                     /* (kgC/m2/d) */
	double litr1c_to_soil1c;             /* (kgC/m2/d) */
	double litr2_hr;                     /* (kgC/m2/d) */
	double litr2c_to_soil2c;             /* (kgC/m2/d) */
	double litr3c_to_litr2c;             /* (kgC/m2/d) */
	double litr4_hr;                     /* (kgC/m2/d) */
	double litr4c_to_soil3c;             /* (kgC/m2/d) */
	double soil1_hr;                     /* (kgC/m2/d) */
	double soil1c_to_soil2c;             /* (kgC/m2/d) */
	double soil2_hr;                     /* (kgC/m2/d) */
	double soil2c_to_soil3c;             /* (kgC/m2/d) */
	double soil3_hr;                     /* (kgC/m2/d) */
	double soil3c_to_soil4c;             /* (kgC/m2/d) */
	double soil4_hr;                     /* (kgC/m2/d) */
	/* daily allocation fluxes from current GPP */
	double cpool_to_leafc;               /* (kgC/m2/d) */
	double cpool_to_leafc_storage;       /* (kgC/m2/d) */
	double cpool_to_frootc;              /* (kgC/m2/d) */
	double cpool_to_frootc_storage;      /* (kgC/m2/d) */
	double cpool_to_livestemc;           /* (kgC/m2/d) */
	double cpool_to_livestemc_storage;   /* (kgC/m2/d) */
	double cpool_to_deadstemc;           /* (kgC/m2/d) */
	double cpool_to_deadstemc_storage;   /* (kgC/m2/d) */
	double cpool_to_livecrootc;          /* (kgC/m2/d) */
	double cpool_to_livecrootc_storage;  /* (kgC/m2/d) */
	double cpool_to_deadcrootc;          /* (kgC/m2/d) */
	double cpool_to_deadcrootc_storage;  /* (kgC/m2/d) */
	double cpool_to_gresp_storage;       /* (kgC/m2/d) */
	/* daily growth respiration fluxes */
	double cpool_leaf_gr;                /* (kgC/m2/d) */
	double cpool_leaf_storage_gr;        /* (kgC/m2/d) */
	double transfer_leaf_gr;             /* (kgC/m2/d) */
	double cpool_froot_gr;               /* (kgC/m2/d) */
	double cpool_froot_storage_gr;       /* (kgC/m2/d) */
	double transfer_froot_gr;            /* (kgC/m2/d) */
	double cpool_livestem_gr;            /* (kgC/m2/d) */
	double cpool_livestem_storage_gr;    /* (kgC/m2/d) */
	double transfer_livestem_gr;         /* (kgC/m2/d) */
	double cpool_deadstem_gr;            /* (kgC/m2/d) */
	double cpool_deadstem_storage_gr;    /* (kgC/m2/d) */
	double transfer_deadstem_gr;         /* (kgC/m2/d) */
	double cpool_livecroot_gr;           /* (kgC/m2/d) */
	double cpool_livecroot_storage_gr;   /* (kgC/m2/d) */
	double transfer_livecroot_gr;        /* (kgC/m2/d) */
	double cpool_deadcroot_gr;           /* (kgC/m2/d) */
	double cpool_deadcroot_storage_gr;   /* (kgC/m2/d) */
	double transfer_deadcroot_gr;        /* (kgC/m2/d) */
	/* annual turnover of storage to transfer pools */
	double leafc_storage_to_leafc_transfer;           /* (kgC/m2/d) */
	double frootc_storage_to_frootc_transfer;         /* (kgC/m2/d) */
	double livestemc_storage_to_livestemc_transfer;    /* (kgC/m2/d) */
	double deadstemc_storage_to_deadstemc_transfer;    /* (kgC/m2/d) */
	double livecrootc_storage_to_livecrootc_transfer; /* (kgC/m2/d) */
	double deadcrootc_storage_to_deadcrootc_transfer; /* (kgC/m2/d) */
	double gresp_storage_to_gresp_transfer;           /* (kgC/m2/d) */
	/* turnover of live wood to dead wood */
	double livestemc_to_deadstemc;        /* (kgC/m2/d) */
	double livecrootc_to_deadcrootc;      /* (kgC/m2/d) */
	/* planting - Hidy 2012. */
	double leafc_transfer_from_PLT;		/* (kgC/m2/d) */
	double frootc_transfer_from_PLT;	/* (kgC/m2/d) */
	/* thinning - Hidy 2012. */
	double leafc_to_THN;				 /* (kgC/m2/d) */
	double leafc_storage_to_THN;         /* (kgC/m2/d) */
	double leafc_transfer_to_THN;        /* (kgC/m2/d) */
	double frootc_to_THN;				 /* (kgC/m2/d) */
	double frootc_storage_to_THN;         /* (kgC/m2/d) */
	double frootc_transfer_to_THN;        /* (kgC/m2/d) */
	double livecrootc_to_THN;				 /* (kgC/m2/d) */
	double livecrootc_storage_to_THN;         /* (kgC/m2/d) */
	double livecrootc_transfer_to_THN;        /* (kgC/m2/d) */
	double deadcrootc_to_THN;				 /* (kgC/m2/d) */
	double deadcrootc_storage_to_THN;         /* (kgC/m2/d) */
	double deadcrootc_transfer_to_THN;        /* (kgC/m2/d) */
	double livestemc_to_THN;				 /* (kgC/m2/d) */
	double livestemc_storage_to_THN;         /* (kgC/m2/d) */
	double livestemc_transfer_to_THN;        /* (kgC/m2/d) */
	double deadstemc_to_THN;				 /* (kgC/m2/d) */
	double deadstemc_storage_to_THN;         /* (kgC/m2/d) */
	double deadstemc_transfer_to_THN;        /* (kgC/m2/d) */
	double gresp_storage_to_THN;         /* (kgC/m2/d) */
	double gresp_transfer_to_THN;        /* (kgC/m2/d) */
	double THN_to_litr1c;				 /* (kgC/m2/d) */
	double THN_to_litr2c;				 /* (kgC/m2/d) */
	double THN_to_litr3c;				 /* (kgC/m2/d) */
	double THN_to_litr4c;				 /* (kgC/m2/d) */
	double THN_to_cwdc;			    	 /* (kgC/m2/d) */
	/* mowing - Hidy 2008. */
	double leafc_to_MOW;				 /* (kgC/m2/d) */
	double leafc_storage_to_MOW;         /* (kgC/m2/d) */
	double leafc_transfer_to_MOW;        /* (kgC/m2/d) */
	double gresp_storage_to_MOW;         /* (kgC/m2/d) */
	double gresp_transfer_to_MOW;        /* (kgC/m2/d) */
	double MOW_to_litr1c;				 /* (kgC/m2/d) */
	double MOW_to_litr2c;				 /* (kgC/m2/d) */
	double MOW_to_litr3c;				 /* (kgC/m2/d) */
	double MOW_to_litr4c;				 /* (kgC/m2/d) */
	/* harvesting - Hidy 2012. */
	double leafc_to_HRV;				 /* (kgC/m2/d) */
	double leafc_storage_to_HRV;         /* (kgC/m2/d) */
	double leafc_transfer_to_HRV;        /* (kgC/m2/d) */
	double gresp_storage_to_HRV;         /* (kgC/m2/d) */
	double gresp_transfer_to_HRV;        /* (kgC/m2/d) */
	double HRV_to_litr1c;				 /* (kgC/m2/d) */
	double HRV_to_litr2c;				 /* (kgC/m2/d) */
	double HRV_to_litr3c;				 /* (kgC/m2/d) */
	double HRV_to_litr4c;				 /* (kgC/m2/d) */
	/* ploughing - Hidy 2012. */
	double leafc_to_PLG;					 /* (kgC/m2/d) */
	double leafc_storage_to_PLG;          /* (kgC/m2/d) */
	double leafc_transfer_to_PLG;         /* (kgC/m2/d) */
	double gresp_storage_to_PLG;          /* (kgC/m2/d) */
	double gresp_transfer_to_PLG;         /* (kgC/m2/d) */
	double frootc_to_PLG;					/* (kgC/m2/d) */
	double frootc_storage_to_PLG;         /* (kgC/m2/d) */
	double frootc_transfer_to_PLG;        /* (kgC/m2/d) */
	double PLG_to_litr1c;
	double PLG_to_litr2c;
	double PLG_to_litr3c;
	double PLG_to_litr4c;
	/* grazing - Hidy 2009. */
	double leafc_to_GRZ;					/* (kgC/m2/d) */
	double leafc_storage_to_GRZ;         /* (kgC/m2/d) */
	double leafc_transfer_to_GRZ;        /* (kgC/m2/d) */
	double gresp_storage_to_GRZ;         /* (kgC/m2/d) */
	double gresp_transfer_to_GRZ;        /* (kgC/m2/d) */
	double GRZ_to_litr1c;				 /* (kgC/m2/d) */
	double GRZ_to_litr2c;				 /* (kgC/m2/d) */
	double GRZ_to_litr3c;				 /* (kgC/m2/d) */
	double GRZ_to_litr4c;				 /* (kgC/m2/d) */       
	/* fertiliziation -  by Hidy 2008 */
	double FRZ_to_litr1c;				 /* (kgC/m2/d) */
	double FRZ_to_litr2c;				 /* (kgC/m2/d) */
	double FRZ_to_litr3c;				 /* (kgC/m2/d) */
	double FRZ_to_litr4c;				 /* (kgC/m2/d) */ 
} cflux_struct;

/* nitrogen state variables (including sums for sources and sinks) */ 
typedef struct
{
    double leafn;              /* (kgN/m2) leaf N */
    double leafn_storage;      /* (kgN/m2) leaf N */
    double leafn_transfer;     /* (kgN/m2) leaf N */
    double frootn;             /* (kgN/m2) fine root N */ 
    double frootn_storage;     /* (kgN/m2) fine root N */ 
    double frootn_transfer;    /* (kgN/m2) fine root N */ 
    double livestemn;          /* (kgN/m2) live stem N */
    double livestemn_storage;  /* (kgN/m2) live stem N */
    double livestemn_transfer; /* (kgN/m2) live stem N */
    double deadstemn;          /* (kgN/m2) dead stem N */
    double deadstemn_storage;  /* (kgN/m2) dead stem N */
    double deadstemn_transfer; /* (kgN/m2) dead stem N */
    double livecrootn;         /* (kgN/m2) live coarse root N */
    double livecrootn_storage; /* (kgN/m2) live coarse root N */
    double livecrootn_transfer;/* (kgN/m2) live coarse root N */
    double deadcrootn;         /* (kgN/m2) dead coarse root N */
    double deadcrootn_storage; /* (kgN/m2) dead coarse root N */
    double deadcrootn_transfer;/* (kgN/m2) dead coarse root N */
    double cwdn;               /* (kgN/m2) coarse woody debris N */
    double litr1n;             /* (kgN/m2) litter labile N */
    double litr2n;             /* (kgN/m2) litter unshielded cellulose N */
    double litr3n;             /* (kgN/m2) litter shielded cellulose N */
    double litr4n;             /* (kgN/m2) litter lignin N */
	/* Hidy 2013: senescence */
	double litr1n_strg_SNSC;
	double litr2n_strg_SNSC;
	double litr3n_strg_SNSC;
	double litr4n_strg_SNSC;
    double soil1n;             /* (kgN/m2) microbial recycling pool N (fast) */
    double soil2n;             /* (kgN/m2) microbial recycling pool N (medium) */
    double soil3n;             /* (kgN/m2) microbial recycling pool N (slow) */
    double soil4n;             /* (kgN/m2) recalcitrant SOM N (humus, slowest) */
	/* multilayer soil - Hidy 2011 */
    double sminn[N_SOILLAYERS];		    /* (kgN/m2) soil mineral N in multilayer soil */
	double sminn_RZ;					/* (kgN/m2) sum of the soil mineral N in the rootzone on actual day */
	double sminn_RZ_befsoilproc;		/* (kgN/m2) sum of the soil mineral N in the rootzone before taking acoount soil processes  */
	double sminn_SUM;					/* (kgN/m2) sum of the soil mineral N in the soillayers */	

	double retransn;					/* (kgN/m2) plant pool of retranslocated N */
	double npool;						/* (kgN/m2) temporary plant N pool */
    double nfix_src;					/* (kgN/m2) SUM of biological N fixation */
    double ndep_src;					/* (kgN/m2) SUM of N deposition inputs */
    double nleached_snk;				/* (kgN/m2) SUM of N leached */
    double nvol_snk;					/* (kgN/m2) SUM of N lost to volatilization */
	double fire_snk;					/* (kgN/m2) SUM of N lost to fire */	
	/* sensescence simulation - Hidy 2011 */
	double SNSC_snk;				/* (kgN/m2) SUM of senescence losses */
	double SNSC_src;				/* (kgN/m2) SUM of wilted plant material which turns into the litter pool */
	/* fertilization - by Hidy 2008. */
	double FRZsrc;				 /*(kgN/m2) SUM of N fertilization inputs */	
	/* planting - by Hidy 2012. */
    double PLTsrc;				 /* (kgN/m2) planted leaf N */
	/* thinning - by Hidy 2008. */
	double THNsnk;				 /* (kgN/m2) thinned leaf N */
	double THNsrc;				 /* (kgC/m2) thinned plant material (N content) returns to the soil (labile litter) */
	/* mowing - by Hidy 2008. */
	double MOWsnk;              /* (kgN/m2) mowed leaf N */
	double MOWsrc;				/* (kgC/m2) mowed plant material (N content) returns to the soil (labile litter) */
	/* harvesting - by Hidy 2012. */
	double HRVsnk;
	double HRVsrc;			/* (kgN/m2) harvested leaf N */
	/* ploughing - Hidy 2012. */
	double PLGsnk;				/* (kgN/m2) plouhged leaf N */
	double PLGsrc;				/* (kgC/m2) ploughed plant material (N content) returns to the soil (labile litter) */
	/* grazing - by Hidy 2009. */
	double GRZsnk;              /* (kgN/m2) grazed leaf N */
	double GRZsrc;          /* (kgN/m2) leaf N from fertilizer*/
} nstate_struct;

/* daily nitrogen flux variables */
typedef struct
{
	/* mortality fluxes */
	double m_leafn_to_litr1n;              /* (kgN/m2/d) */
	double m_leafn_to_litr2n;              /* (kgN/m2/d) */
	double m_leafn_to_litr3n;              /* (kgN/m2/d) */
	double m_leafn_to_litr4n;              /* (kgN/m2/d) */
	double m_frootn_to_litr1n;             /* (kgN/m2/d) */
	double m_frootn_to_litr2n;             /* (kgN/m2/d) */
	double m_frootn_to_litr3n;             /* (kgN/m2/d) */
	double m_frootn_to_litr4n;             /* (kgN/m2/d) */
	double m_leafn_storage_to_litr1n;      /* (kgN/m2/d) */
	double m_frootn_storage_to_litr1n;     /* (kgN/m2/d) */
	double m_livestemn_storage_to_litr1n;  /* (kgN/m2/d) */
	double m_deadstemn_storage_to_litr1n;  /* (kgN/m2/d) */
	double m_livecrootn_storage_to_litr1n; /* (kgN/m2/d) */
	double m_deadcrootn_storage_to_litr1n; /* (kgN/m2/d) */
	double m_leafn_transfer_to_litr1n;     /* (kgN/m2/d) */
	double m_frootn_transfer_to_litr1n;    /* (kgN/m2/d) */
	double m_livestemn_transfer_to_litr1n; /* (kgN/m2/d) */
	double m_deadstemn_transfer_to_litr1n; /* (kgN/m2/d) */
	double m_livecrootn_transfer_to_litr1n;/* (kgN/m2/d) */
	double m_deadcrootn_transfer_to_litr1n;/* (kgN/m2/d) */
	double m_livestemn_to_litr1n;          /* (kgN/m2/d) */
	double m_livestemn_to_cwdn;            /* (kgN/m2/d) */
	double m_deadstemn_to_cwdn;            /* (kgN/m2/d) */
	double m_livecrootn_to_litr1n;         /* (kgN/m2/d) */
	double m_livecrootn_to_cwdn;           /* (kgN/m2/d) */
	double m_deadcrootn_to_cwdn;           /* (kgN/m2/d) */
	double m_retransn_to_litr1n;           /* (kgN/m2/d) */
	/* 2010 Hidy - senescence fluxes */
	double m_leafn_to_SNSC;                /* (kgN/m2/d) */
	double m_frootn_to_SNSC;               /* (kgN/m2/d) */
	double m_leafn_storage_to_SNSC;		   /* (kgN/m2/d) */
	double m_frootn_storage_to_SNSC;       /* (kgN/m2/d) */
	double m_leafn_transfer_to_SNSC;       /* (kgN/m2/d) */
	double m_frootn_transfer_to_SNSC;      /* (kgN/m2/d) */
	double m_retransn_to_SNSC;
	double SNSC_to_litr1n;
	double SNSC_to_litr2n;
	double SNSC_to_litr3n;
	double SNSC_to_litr4n;
	/* fire fluxes */
	double m_leafn_to_fire;                /* (kgN/m2/d) */
	double m_frootn_to_fire;               /* (kgN/m2/d) */
	double m_leafn_storage_to_fire;        /* (kgN/m2/d) */
	double m_frootn_storage_to_fire;       /* (kgN/m2/d) */
	double m_livestemn_storage_to_fire;    /* (kgN/m2/d) */
	double m_deadstemn_storage_to_fire;    /* (kgN/m2/d) */
	double m_livecrootn_storage_to_fire;   /* (kgN/m2/d) */
	double m_deadcrootn_storage_to_fire;   /* (kgN/m2/d) */
	double m_leafn_transfer_to_fire;       /* (kgN/m2/d) */
	double m_frootn_transfer_to_fire;      /* (kgN/m2/d) */
	double m_livestemn_transfer_to_fire;   /* (kgN/m2/d) */
	double m_deadstemn_transfer_to_fire;   /* (kgN/m2/d) */
	double m_livecrootn_transfer_to_fire;  /* (kgN/m2/d) */
	double m_deadcrootn_transfer_to_fire;  /* (kgN/m2/d) */
	double m_livestemn_to_fire;            /* (kgN/m2/d) */
	double m_deadstemn_to_fire;            /* (kgN/m2/d) */
	double m_livecrootn_to_fire;           /* (kgN/m2/d) */
	double m_deadcrootn_to_fire;           /* (kgN/m2/d) */
	double m_retransn_to_fire;             /* (kgN/m2/d) */
	double m_litr1n_to_fire;               /* (kgN/m2/d) */
	double m_litr2n_to_fire;               /* (kgN/m2/d) */
	double m_litr3n_to_fire;               /* (kgN/m2/d) */
	double m_litr4n_to_fire;               /* (kgN/m2/d) */
	double m_cwdn_to_fire;                 /* (kgN/m2/d) */
	/* phenology fluxes from transfer pool */
	double leafn_transfer_to_leafn;           /* (kgN/m2/d) */
	double frootn_transfer_to_frootn;         /* (kgN/m2/d) */
	double livestemn_transfer_to_livestemn;   /* (kgN/m2/d) */
	double deadstemn_transfer_to_deadstemn;   /* (kgN/m2/d) */
	double livecrootn_transfer_to_livecrootn; /* (kgN/m2/d) */
	double deadcrootn_transfer_to_deadcrootn; /* (kgN/m2/d) */
	/* litterfall fluxes */
	double leafn_to_litr1n;               /* (kgN/m2/d) */
	double leafn_to_litr2n;               /* (kgN/m2/d) */ 
	double leafn_to_litr3n;               /* (kgN/m2/d) */
	double leafn_to_litr4n;               /* (kgN/m2/d) */
	double leafn_to_retransn;             /* (kgN/m2/d) */
	double frootn_to_litr1n;              /* (kgN/m2/d) */
	double frootn_to_litr2n;              /* (kgN/m2/d) */
	double frootn_to_litr3n;              /* (kgN/m2/d) */
	double frootn_to_litr4n;              /* (kgN/m2/d) */
	/* deposition flux */
	double ndep_to_sminn;                 /* (kgN/m2/d) */
	double nfix_to_sminn;                 /* (kgN/m2/d) */
	/* litter and soil decomposition fluxes */
	double cwdn_to_litr2n;                /* (kgN/m2/d) */
	double cwdn_to_litr3n;                /* (kgN/m2/d) */
	double cwdn_to_litr4n;                /* (kgN/m2/d) */
	double litr1n_to_soil1n;              /* (kgN/m2/d) */
	double sminn_to_soil1n_l1;            /* (kgN/m2/d) */
	double litr2n_to_soil2n;              /* (kgN/m2/d) */
	double sminn_to_soil2n_l2;            /* (kgN/m2/d) */
	double litr3n_to_litr2n;              /* (kgN/m2/d) */
	double litr4n_to_soil3n;              /* (kgN/m2/d) */
	double sminn_to_soil3n_l4;            /* (kgN/m2/d) */
	double soil1n_to_soil2n;              /* (kgN/m2/d) */
	double sminn_to_soil2n_s1;            /* (kgN/m2/d) */
	double soil2n_to_soil3n;              /* (kgN/m2/d) */
	double sminn_to_soil3n_s2;            /* (kgN/m2/d) */
	double soil3n_to_soil4n;              /* (kgN/m2/d) */
	double sminn_to_soil4n_s3;            /* (kgN/m2/d) */
	double soil4n_to_sminn;               /* (kgN/m2/d) */
	/* denitrification (volatilization) fluxes */
	double sminn_to_nvol_l1s1;            /* (kgN/m2/d) */
	double sminn_to_nvol_l2s2;            /* (kgN/m2/d) */
	double sminn_to_nvol_l4s3;            /* (kgN/m2/d) */
	double sminn_to_nvol_s1s2;            /* (kgN/m2/d) */
	double sminn_to_nvol_s2s3;            /* (kgN/m2/d) */
	double sminn_to_nvol_s3s4;            /* (kgN/m2/d) */
	double sminn_to_nvol_s4;              /* (kgN/m2/d) */
	double sminn_to_denitrif;             /* (kgN/m2/d) */
	/* SMINN change caused by soil process - Hidy 2011 */
	double sminn_soilproc[N_SOILLAYERS];    /* (kgN/m2/d) */
	double sminn_leached[N_SOILLAYERS];    /* (kgN/m2/d) */
	/* daily allocation fluxes */
	double retransn_to_npool;             /* (kgN/m2/d) */
	double sminn_to_npool;                /* (kgN/m2/d) */
	double npool_to_leafn;                /* (kgN/m2/d) */
	double npool_to_leafn_storage;        /* (kgN/m2/d) */
	double npool_to_frootn;               /* (kgN/m2/d) */
	double npool_to_frootn_storage;       /* (kgN/m2/d) */
	double npool_to_livestemn;            /* (kgN/m2/d) */
	double npool_to_livestemn_storage;    /* (kgN/m2/d) */
	double npool_to_deadstemn;            /* (kgN/m2/d) */
	double npool_to_deadstemn_storage;    /* (kgN/m2/d) */
	double npool_to_livecrootn;           /* (kgN/m2/d) */
	double npool_to_livecrootn_storage;   /* (kgN/m2/d) */
	double npool_to_deadcrootn;           /* (kgN/m2/d) */
	double npool_to_deadcrootn_storage;   /* (kgN/m2/d) */
	/* annual turnover of storage to transfer */
	double leafn_storage_to_leafn_transfer;           /* (kgN/m2/d) */
	double frootn_storage_to_frootn_transfer;         /* (kgN/m2/d) */
	double livestemn_storage_to_livestemn_transfer;   /* (kgN/m2/d) */
	double deadstemn_storage_to_deadstemn_transfer;   /* (kgN/m2/d) */
	double livecrootn_storage_to_livecrootn_transfer; /* (kgN/m2/d) */
	double deadcrootn_storage_to_deadcrootn_transfer; /* (kgN/m2/d) */
	/* turnover of live wood to dead wood, with retranslocation */
	double livestemn_to_deadstemn;        /* (kgN/m2/d) */
	double livestemn_to_retransn;         /* (kgN/m2/d) */
	double livecrootn_to_deadcrootn;      /* (kgN/m2/d) */
	double livecrootn_to_retransn;        /* (kgN/m2/d) */
	/* planting - Hidy 2012. */
	double leafn_transfer_from_PLT;		/* (kgN/m2/d) */
	double frootn_transfer_from_PLT;		/* (kgN/m2/d) */
	/* thinning - by Hidy 2012. */
	double leafn_to_THN;				 /* (kgN/m2/d) */
	double leafn_storage_to_THN;         /* (kgN/m2/d) */
	double leafn_transfer_to_THN;        /* (kgN/m2/d) */
	double frootn_to_THN;				 /* (kgN/m2/d) */
	double frootn_storage_to_THN;         /* (kgN/m2/d) */
	double frootn_transfer_to_THN;        /* (kgN/m2/d) */
	double livecrootn_to_THN;				 /* (kgN/m2/d) */
	double livecrootn_storage_to_THN;         /* (kgN/m2/d) */
	double livecrootn_transfer_to_THN;        /* (kgN/m2/d) */
	double deadcrootn_to_THN;				 /* (kgN/m2/d) */
	double deadcrootn_storage_to_THN;         /* (kgN/m2/d) */
	double deadcrootn_transfer_to_THN;        /* (kgN/m2/d) */
	double livestemn_to_THN;				 /* (kgN/m2/d) */
	double livestemn_storage_to_THN;         /* (kgN/m2/d) */
	double livestemn_transfer_to_THN;        /* (kgN/m2/d) */
	double deadstemn_to_THN;				 /* (kgN/m2/d) */
	double deadstemn_storage_to_THN;         /* (kgN/m2/d) */
	double deadstemn_transfer_to_THN;        /* (kgN/m2/d) */
	double restransn_to_THN;				  /* (kgN/m2/d) */
	double THN_to_litr1n;				 /* (kgN/m2/d) */
	double THN_to_litr2n;				 /* (kgN/m2/d) */
	double THN_to_litr3n;				 /* (kgN/m2/d) */
	double THN_to_litr4n;				 /* (kgN/m2/d) */
	double THN_to_cwdn;
	/* mowing - by Hidy 2008. */
	double leafn_to_MOW;                 /* (kgN/m2/d) */
	double leafn_storage_to_MOW;         /* (kgN/m2/d) */
	double leafn_transfer_to_MOW;        /* (kgN/m2/d) */
	double MOW_to_litr1n;				 /* (kgN/m2/d) */
	double MOW_to_litr2n;				 /* (kgN/m2/d) */
	double MOW_to_litr3n;				 /* (kgN/m2/d) */
	double MOW_to_litr4n;				 /* (kgN/m2/d) */
	/* harvesting - by Hidy 2012. */
	double leafn_to_HRV;                 /* (kgN/m2/d) */
	double leafn_storage_to_HRV;         /* (kgN/m2/d) */
	double leafn_transfer_to_HRV;        /* (kgN/m2/d) */
	double HRV_to_litr1n;				 /* (kgN/m2/d) */
	double HRV_to_litr2n;				 /* (kgN/m2/d) */
	double HRV_to_litr3n;				 /* (kgN/m2/d) */
	double HRV_to_litr4n;				 /* (kgN/m2/d) */
	/* ploughing - Hidy 2012. */
	double leafn_to_PLG;                 /* (kgN/m2/d) */
	double leafn_storage_to_PLG;         /* (kgN/m2/d) */
	double leafn_transfer_to_PLG;        /* (kgN/m2/d) */
	double frootn_to_PLG;				/* (kgN/m2/d) */
	double frootn_storage_to_PLG;         /* (kgN/m2/d) */
	double frootn_transfer_to_PLG;        /* (kgN/m2/d) */
	double PLG_to_litr1n;
	double PLG_to_litr2n;
	double PLG_to_litr3n;
	double PLG_to_litr4n;
	/* grazing - by Hidy 2008. */
	double leafn_to_GRZ;                 /* (kgN/m2/d) */
	double leafn_storage_to_GRZ;         /* (kgN/m2/d) */
	double leafn_transfer_to_GRZ;        /* (kgN/m2/d) */
	double GRZ_to_litr1n;				 /* (kgN/m2/d) */
	double GRZ_to_litr2n;				 /* (kgN/m2/d) */
	double GRZ_to_litr3n;				 /* (kgN/m2/d) */
	double GRZ_to_litr4n;				 /* (kgN/m2/d) */
	/* fertiliziation -  by Hidy 2008 */
	double FRZ_to_sminn;      /* (kgN/m2/d) */
	double FRZ_to_litr1n;				 /* (kgN/m2/d) */
	double FRZ_to_litr2n;				 /* (kgN/m2/d) */
	double FRZ_to_litr3n;				 /* (kgN/m2/d) */
	double FRZ_to_litr4n;				 /* (kgN/m2/d) */
} nflux_struct;

/* temporary nitrogen variables for reconciliation of decomposition
immobilization fluxes and plant growth N demands */
typedef struct 
{
	double mineralized;
	double potential_immob;
	double plant_ndemand;		// Hidy 2009.
	double excess_c;
	double day_gpp;
	double plitr1c_loss;
	double pmnf_l1s1;
	double plitr2c_loss;
	double pmnf_l2s2;
	double plitr4c_loss;
	double pmnf_l4s3;
	double psoil1c_loss;
	double pmnf_s1s2;
	double psoil2c_loss;
	double pmnf_s2s3;
	double psoil3c_loss;
	double pmnf_s3s4;
	double psoil4c_loss;
	double kl4;
} ntemp_struct;
	
/* phenological control arrays */
typedef struct
{
	int* remdays_curgrowth; /* (nmetdays) days left in current growth season */
	int* remdays_transfer;  /* (nmetdays) number of transfer days remaining */
	int* remdays_litfall;   /* (nmetdays) number of litfall days remaining */
	int* predays_transfer;  /* (nmetdays) number of transfer days previous */
	int* predays_litfall;   /* (nmetdays) number of litfall days previous */
	int* onday_arr;			/* Hidy 2009 - (nmetyears) first day of transfer period */
	int* offday_arr;		/* Hidy 2009 - (nmetyears) last day of transfer period */
} phenarray_struct;

/* daily phenological data array */
typedef struct
{  
	double remdays_curgrowth; /* days left in current growth season */
	double remdays_transfer;  /* number of transfer days remaining */
	double remdays_litfall;   /* number of litfall days remaining */
	double predays_transfer;  /* number of transfer days previous */
	double predays_litfall;   /* number of litfall days previous */
	int onday;				  /* Hidy 2012 - first day of vegetation period */
	int offday;				  /* Hidy 2012 - last day of vegetation period */
} phenology_struct;


/* ecophysiological variables */
typedef struct
{
	double day_leafc_litfall_increment;      /* (kgC/m2/d) rate leaf litfall */
	double day_frootc_litfall_increment;     /* (kgC/m2/d) rate froot litfall */
	double day_livestemc_turnover_increment; /* (kgC/m2/d) rate livestem turnover */
	double day_livecrootc_turnover_increment;/* (kgC/m2/d) rate livecroot turnover */
	double annmax_leafc;					 /* (kgC/m2) annual maximum daily leaf C */
	double annmax_frootc;					 /* (kgC/m2) annual maximum daily froot C */
	double annmax_livestemc;				 /* (kgC/m2) annual maximum daily livestem C */
	double annmax_livecrootc;				 /* (kgC/m2) annual maximum daily livecroot C */
	double dsr;							    /* (days) number of days since rain, for soil evap */
	double dsws;							/* Hidy 2010 - (days) number of days since soil water stress */
	double dsws_FULL;							/* Hidy 2010 - (days) number of days since total soil water stress (below WP)*/
    double proj_lai;						/* (DIM) live projected leaf area index */
    double all_lai;							/* (DIM) live all-sided leaf area index */
    double plaisun;							/* (DIM) sunlit projected leaf area index */
    double plaishade;						/* (DIM) shaded projected leaf area index */
    double sun_proj_sla;					/* (m2/kgC) sunlit projected SLA */
    double shade_proj_sla;					/* (m2/kgC) shaded projected SLA */
    
	/* Hidy 2010 - new variables */
	double soillayer_RZportion[N_SOILLAYERS];		/* (ratio) Hidy 2010 - array contains the portion of rootzone in the given soil layer */
	double psi[N_SOILLAYERS];						/* (MPa) water potential of soil and leaves - Hidy 2010: multilayer soil  */
	double pF[N_SOILLAYERS];						/* (cm) soil water suction derived from log(soil water potential) - Hidy 2010  */
	double hydr_conduct[N_SOILLAYERS];				/* (m/s) hydraulic conductivity - Hidy 2010: multilayer soil  */
	double hydr_diffus[N_SOILLAYERS];				/* (m2/s) hydraulic diffusivity - Hidy 2010: multilayer soil  */
    double vwc[N_SOILLAYERS];						/* (DIM) volumetric water content - Hidy 2010: multilayer soil */
	int n_rootlayers;								/* (number) number of of the soil layers in which root can be find  */
	double rooting_depth;							/* (m) actual depth of the rooting zone */
	double psi_avg;									/* (MPa) average water potential of soil and leaves  - Hidy 2010 */
	double hydr_conduct_avg;						/* (m/s) average hydraulic conductivity (1 meter deep layer) - Hidy 2010  */
	double hydr_diffus_avg;			    			/* (m2/s) average hydraulic diffusivity (1 meter deep layer) - Hidy 2010  */
    double vwc_avg;									/* (m3/m3) average volumetric water content (1 meter deep layer) - Hidy 2010 */
    double vwc_ratio_open;							/* (DIM) volumetric water content ratio at start of conductance reduction - Hidy 2012*/
	double vwc_ratio_close;							/* (DIM) volumetric water content ratio at stomatal closure - Hidy 2012*/
	double vwc_open;								/* (m3/m3) volumetric water content at start of conductance reduction - Hidy 2012*/
	double vwc_close;								/* (m3/m3) volumetric water content at stomatal closure - Hidy 2012*/
	double psi_open;								/* (MPa) soil water potential at start of conductance reduction - Hidy 2012*/
	double psi_close;								/* (MPa) soil water potential at stomatal closure  - Hidy 2012*/

	double dlmr_area_sun;					/* (umolC/m2projected leaf area/s) sunlit leaf MR */
    double dlmr_area_shade;					/* (umolC/m2projected leaf area/s) shaded leaf MR */
    double gl_t_wv_sun;						/* (m/s) leaf-scale conductance to transpired water */
    double gl_t_wv_shade;					/* (m/s) leaf-scale conductance to transpired water */
    double assim_sun;						/* (umol/m2/s) sunlit assimilation per unit pLAI */
    double assim_shade;						/* (umol/m2/s) shaded assimilation per unit pLAI */
    /* decomp variables */
    double t_scalar;						/* (DIM) decomp temperature scalar */
    double w_scalar;						/* (DIM) decomp water scalar */
    double rate_scalar;						/* (DIM) decomp combined scalar */
	double daily_gross_nmin;				/* (kgN/m2/d) daily gross N mineralization */
	double daily_gross_nimmob;				/* (kgN/m2/d) daily gross N immobilization */ 
	double daily_net_nmin;					/* (kgN/m2/d) daily net N mineralization */

    /* the following are optional outputs, usually set if the appropriate
    functions are called with the flag verbose = 1 */
	double m_tmin;							   /* (DIM) freezing night temperature multiplier */
	double m_soilprop_layer[N_SOILLAYERS]; /* (DIM) Hidy 2010: CONTROL - soil water content multiplier */
	double m_soilprop;					      /* (DIM) soil water properties (water content and depth) multiplier */
	double m_co2;						   	   /* (DIM) atmospheric [CO2] multiplier */
	double m_ppfd_sun;						 /* (DIM) PAR flux density multiplier */
	double m_ppfd_shade;					 /* (DIM) PAR flux density multiplier */
	double m_vpd;							 /* (DIM) vapor pressure deficit multiplier */
	double m_final_sun;						 /* (DIM) product of all other multipliers */
	double m_final_shade;        /* (DIM) product of all other multipliers */
	double gl_bl;          /* (m/s) leaf boundary layer conductance */
    double gl_c;           /* (m/s) leaf cuticular conductance */
    double gl_s_sun;           /* (m/s) leaf-scale stomatal conductance */
    double gl_s_shade;           /* (m/s) leaf-scale stomatal conductance */
	double gl_e_wv;        /* (m/s) leaf conductance to evaporated water */
	double gl_sh;          /* (m/s) leaf conductance to sensible heat */
    double gc_e_wv;        /* (m/s) canopy conductance to evaporated water */
    double gc_sh;          /* (m/s) canopy conductance to sensible heat */
	
	/* diagnostic variables for ouput purposes only */
	double ytd_maxplai;    /* (DIM) year-to-date maximum projected LAI */
} epvar_struct;

/* soil and site constants */
typedef struct
{
    double max_rootzone_depth;							/* (m)   Hidy 2010 - maximum depth of rooting zone */
    double soillayer_depths[N_SOILLAYERS];			/*  (m) Hidy 2010 - array contains the soil layer depths (positive values)*/
	double soillayer_thickness[N_SOILLAYERS];		/*  (m) Hidy 2010 - array contains the soil layer thickness (positive values) */
	double rel_soillayer_thickness[N_SOILLAYERS];	/* (ratio) Hidy 2010 - array contains the ratio of soil layer thickness and total soil depth(positive values) */
	double delta_z[N_SOILLAYERS];					/*  (m) Hidy 2010 - array contains the depth of the middle layers (negative values)*/
    double soil_b;										/* (DIM) Clapp-Hornberger "b" parameter */
    double vwc_sat;										/* (DIM) volumetric water content at saturation */
    double vwc_fc;								/* (DIM) VWC at field capacity ( = -0.033 MPa) */
	double vwc_wp;								/* (DIM) VWC at wilting point ( = -2.0 MPa) */
    double psi_sat;								/* (MPa) soil matric potential at saturation */
	double psi_fc;								/* (MPa) soil matric potential at field capacity */
	double psi_wp;								/* (MPa) soil matric potential at wilting point */
	double hydr_conduct_sat;					/* (m/s) hidraulic conductivity at saturation  - Hidy 2010 */
	double hydr_diffus_sat;				    	/* (m2/s) hidraulic diffusivity at saturation  - Hidy 2010 */
	double hydr_conduct_fc;				    	/* (m/s) hidraulic conductivity at field capacity  - Hidy 2010 */
	double hydr_diffus_fc;				    	/* (m2/s) hidraulic diffusivity at field capacity  - Hidy 2010 */
	double mean_surf_air_temp;					/* (K/m) mean surface air temperature - Hidy 2010 */
    double elev;								/* (m) site elevation */
    double lat;				 					/* (degrees) site latitude (negative for south) */
    double sw_alb;								/* (DIM) surface shortwave albedo */
    double ndep;								/* (kgN/m2/yr) wet+dry atmospheric deposition of N */
    double nfix;								/* (kgN/m2/yr) symbiotic+asymbiotic fixation of N */
	double runoff_param;						/* (m) runoff parameter - Campbell and Diaz, 1988 */
	double vwc_sat_mes;							/* (m3/m3) Hidy 2010 - measured soil water content at saturation*/
	double vwc_fc_mes;							/* (m3/m3) Hidy 2010 - measured soil water content at field capacity*/
	double vwc_wp_mes;							/* (m3/m3) Hidy 2010 - measured soil water content at wilting point*/
} siteconst_struct;								

/* canopy ecophysiological constants */
typedef struct
{
	int woody;             /* (flag) 1=woody, 0=non-woody */
	int evergreen;         /* (flag) 1=evergreen, 0=deciduous */
	int c3_flag;           /* (flag) 1 = C3,  0 = C4 */
	int phenology_flag;    /* (flag) 1=phenology model, 0=user defined */
	int onday;             /* (yday) yearday leaves on */
	int offday;            /* (yday) yearday leaves off */
	double transfer_pdays;    /* (prop.) fraction of growth period for transfer */
	double litfall_pdays;     /* (prop.) fraction of growth period for litfall */
    double leaf_turnover;     /* (1/yr) annual leaf turnover fraction */
    double froot_turnover;    /* (1/yr) annual fine root turnover fraction */
    double livewood_turnover; /* (1/yr) annual live wood turnover fraction */
    double daily_mortality_turnover; /* (1/day) daily mortality turnover */
    double daily_fire_turnover;      /* (1/day) daily fire turnover */
    double alloc_frootc_leafc; /* (ratio) new fine root C to new leaf C */
	double alloc_newstemc_newleafc; /* (ratio) new stem C to new leaf C */
	double alloc_newlivewoodc_newwoodc; /* (ratio) new livewood C:new wood C */
    double alloc_crootc_stemc; /* (ratio) new live croot C to new live stem C */
    double alloc_prop_curgrowth; /* (prop.) daily allocation to current growth */
    double avg_proj_sla;   /* (m2/kgC) canopy average proj. SLA */
    double sla_ratio;      /* (DIM) ratio of shaded to sunlit projected SLA */
    double lai_ratio;      /* (DIM) ratio of (all-sided LA / one-sided LA) */
    double int_coef;       /* (kg/kg/LAI/d) canopy precip interception coef */
    double ext_coef;       /* (DIM) canopy light extinction coefficient */
    double flnr;           /* (kg NRub/kg Nleaf) leaf N in Rubisco */
	double vwc_ratio_open;			/* (%) Hidy 2011 - vwc ratio at start of conductance reduction */
    double vwc_ratio_close;			/* (%) Hidy 2011 - vwc at complete conductance */
	double psi_open;			/* (%) Hidy 2011 - soil water potential at conductance reduction */
    double psi_close;			/* (%) Hidy 2011 - soil water potential at complete conductance */
	double vpd_open;       /* (Pa)  vpd at start of conductance reduction */
	double vpd_close;      /* (Pa)  vpd at complete conductance reduction */
    double gl_smax;        /* (m/s) maximum leaf-scale stomatal conductance */
    double gl_c;           /* (m/s) leaf-scale cuticular conductance */
	double gl_bl;          /* (m/s) leaf-scale boundary layer conductance */
    double froot_cn;     /* (kgC/kgN) C:N for fine roots */
    double leaf_cn;      /* (kgC/kgN) C:N for leaves */
    double livewood_cn;  /* (kgC/kgN) C:N for live wood */
	double deadwood_cn;  /* (kgC/kgN) C:N for dead wood */
    double leaflitr_cn;      /* (kgC/kgN) constant C:N for leaf litter */
    double leaflitr_flab;    /* (DIM) leaf litter labile fraction */
    double leaflitr_fucel;   /* (DIM) leaf litter unshielded cellulose fract. */
    double leaflitr_fscel;   /* (DIM) leaf litter shielded cellulose fract. */
    double leaflitr_flig;    /* (DIM) leaf litter lignin fraction */
    double frootlitr_flab;   /* (DIM) froot litter labile fraction */
    double frootlitr_fucel;  /* (DIM) froot litter unshielded cellulose fract */
    double frootlitr_fscel;  /* (DIM) froot litter shielded cellulose fract */
    double frootlitr_flig;   /* (DIM) froot litter lignin fraction */
    double deadwood_fucel;   /* (DIM) dead wood unshileded cellulose fraction */
    double deadwood_fscel;   /* (DIM) dead wood shielded cellulose fraction */
    double deadwood_flig;    /* (DIM) dead wood lignin fraction */
	double mort_SNSC_displayed;	 /* Hidy 2011 - mortality parameter of senescence reagrding to displayed plant material */
	double mort_SNSC_storaged;	 /* Hidy 2011 - mortality parameter of senescence regarding to storaged plant material */
    double mort_SNSC_to_litter; /* Hidy 2013 - turnover rate of wilted standing biomass to litter*/
	double q10_value;				/* (DIM) temperature coefficient for calculating maint.resp. */
	double* wpm_array;		 /* Hidy 2011 - changingx WPM values */
	double* msc_array;		 /* Hidy 2011 - changingx WPM values */
} epconst_struct;

/* strucure for thinning paramteres - by Hidy 2012. */
#define MAX_THNDAYS 5
typedef struct
{
	int THN_flag;					/* (flag) 1=do thinning , 0=no thinning */	
	int n_THNdays;					/* number of of the thinning days in 1 year */
	int THNdays[MAX_THNDAYS];		/* array contains the thinning days in 1 year*/
	double thinning_rate;			/* (prop.) rate of the thinned trees */
	double transp_stem_rate;		/* (prop.) rate of the transported stem */
	double transp_leaf_rate;		/* (prop.) rate of the transported leaf */
} thinning_struct;

/* strucure for mowing paramteres - by Hidy 2008 */
#define MAX_MOWDAYS 5
typedef struct
{
	int MOW_flag;					/* (flag) 1=do mowing , 0=no mowing */	
	int fixday_or_fixLAI_flag;		/* (flag) 0=mowing is occured on fix days, 1=mowing is occured is LAI greater than a limit */
	int MOWdays[MAX_MOWDAYS];		/* array contains the mowing days in 1 year*/
	int n_MOWdays;					/* number of of the mowing days in 1 year */
	double fixLAI_befMOW;			/*  value of the LAI before mowing*/
	double LAI_limit;				/*  value of the LAI after mowing*/
	double transport_coeff;			/* define the proportion of plant material transported away */
} mowing_struct;

/* strucure for harvesting paramteres - by Hidy 2009 */
#define MAX_HRVDAYS 5
typedef struct
{
	int HRV_flag;						/* (flag) 1=do harvesting , 0=no harvesting */
	int n_HRVdays;						/* number of the harvesting days in 1 year */
    int HRVdays[MAX_HRVDAYS];			/* array contains the harvesting days in 1 year*/
	double LAI_snag;					/*  value of the LAI after harvesting*/
	double transport_coeff;				/* define the proportion of plant material transported away */
} harvesting_struct;

/* strucure for ploughing paramteres - by Hidy 2012 */
#define MAX_PLGDAYS 5
typedef struct
{
	int PLG_flag;						/* (flag) 1=do plough , 0=no plough */
	int n_PLGdays;					/* number of the plough days in 1 year */
	int PLGdays[MAX_PLGDAYS];	/* array contains the plough days in 1 year*/
} ploughing_struct;

/* strucure for grazing paramteres - by Hidy 2009 */
typedef struct
{
	int GRZ_flag;							/* (flag) 1=do grazin , 0=no grazing */	
	int first_day_of_GRZ;					/* yday - beginning of grazing */
	int last_day_of_GRZ;						/* yday - end of grazing */
	double drymatter_intake;        		/*  (kg dry matter/LSU) pasture forgage dry matter intake */
	double stocking_rate;					/* animal stocking rate: Livestock Units per hectare */
	double prop_DMintake_formed_excrement;	/* prop of the dry matter intake formed excrement */
	double prop_excrement_return2litter;	/* prop. of excrement return to litter */
	double C_content_of_drymatter;			/* carbon content of dry matter*/
	double N_content_of_excrement;		/* ratio - nitrogen content of the fertilizer */
	double C_content_of_excrement;	    	/* ratio - carbon content of the fertilizer */
} grazing_struct;

/* strucure for planting paramteres - by Hidy 2008 */
#define MAX_PLTDAYS 5
typedef struct
{
	int PLT_flag;					/* (flag) 1=do planting , 0=no planting */	
    int n_PLTdays;					/* number of of the planting days in 1 year */
    int PLTdays[MAX_PLTDAYS];       /* array contains the planting days in 1 year*/
	double seed_quantity;			/*  quantity of seed*/
	double seed_carbon;				/*  carbon content of seed*/
	double prop_leaf_product;	/* (%) proportion of material of seed which produces leaf */
} planting_struct;

/* strucure for fertilizing paramteres - by Hidy 2008 */
#define MAX_FRZDAYS 5
typedef struct
{
	int FRZ_flag;					    /* (flag) 1=fertilizing , 0=no fertilizing */
	int n_FRZdays;					/* number of of the fertilizing days in 1 year */
    int FRZdays[MAX_FRZDAYS];       /* array contains the fertilizing days in 1 year*/
	double fertilizer[MAX_FRZDAYS];   /* array contains the amount of fertilizer on the fertilizing days*/
	double FRZ_pool_act;				/* actual  nitrogen content of fertilization above the ground */
	double nitrate_content;				/*(%)  nitrate content of fertilizer*/
 	double ammonium_content;			/*(%)  ammonium content of fertilizer*/
	double carbon_content;				/*(%)  carbon content of fertilizer*/
	double litr_flab;					/*(%)  labile fraction of fertilizer*/
	double litr_fucel;					/*(%)  unshielded cellulose fraction of fertilizer*/
	double litr_fscel;					/*(%)  shielded cellulose fraction of fertilizer*/
	double litr_flig;					/*(%)  lignin fraction of fertilizer*/
    double dissolv_coeff;				/*(%)  define the quantity of the N from fertilization (ratio) get into the mineralized N pool on a given day*/
    double utilization_coeff;			/*(%)  useful part of the fertilizer */
} fertilizing_struct;

/* structure for the photosynthesis routine */
typedef struct
{
	int c3;                 /* (flag) set to 1 for C3 model, 0 for C4 model */
	double pa;              /* (Pa) atmospheric pressure */
	double co2;             /* (ppm) atmospheric [CO2] */
	double t;               /* (deg C) temperature */
	double lnc;             /* (kg Nleaf/m2) leaf N per unit sunlit leaf area */
	double flnr;            /* (kg NRub/kg Nleaf) fract. of leaf N in Rubisco */
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

typedef struct
{
	double ta;              /* (degC) air temperature */
	double pa;              /* (Pa)   air pressure */
	double vpd;             /* (Pa)   vapor pressure deficit */
	double irad;            /* (W/m2) incident shortwave flux density */
	double rv;              /* (s/m)  resistance to water vapor flux */
	double rh;              /* (s/m)  resistance to sensible heat flux */
} pmet_struct;

typedef struct
{
	double daily_npp;      /* kgC/m2/day = GPP - Rmaint - Rgrowth */
	double daily_nee;      /* kgC/m2/day = GPP - Rmaint - Rgrowth - Rheretotrop - fire losses */
	double daily_nbp;      /* kgC/m2/day = GPP - Rmaint - Rgrowth - Rheretotrop - disturb_emission - fire losses*/
	double daily_gpp;      /* kgC/m2/day  gross PSN source */
	double daily_mr;       /* kgC/m2/day  maintenance respiration */
	double daily_gr;       /* kgC/m2/day  growth respiration */
	double daily_hr;       /* kgC/m2/day  heterotrophic respiration */
	double daily_sr;       /* kgC/m2/day  Hidy 2012 - soil respiration */
	double daily_fire;     /* kgC/m2/day  fire losses */
	double daily_litfallc; /* kgC/m2/day  total litterfall */
	double cum_npp;        /* kgC/m2  Summed over entire simulation */
	double cum_nep;        /* kgC/m2  Summed over entire simulation */
	double cum_nee;        /* kgC/m2  Summed over entire simulation */
	double cum_gpp;        /* kgC/m2  Summed over entire simulation */
	double cum_mr;         /* kgC/m2  Summed over entire simulation */
	double cum_gr;         /* kgC/m2  Summed over entire simulation */
	double cum_hr;         /* kgC/m2  Summed over entire simulation */
	double cum_fire;       /* kgC/m2  Summed over entire simulation */
	double vegc;           /* kgC/m2  total vegetation C */
	double litrc;          /* kgC/m2  total litter C */
	double soilc;          /* kgC/m2  total soil C */
	double totalc;         /* kgC/m2  total of vegc, litrc, and soilc */
	/*effect of planting, thinning, mowing, grazing, harvesting, ploughing and fertilizing   - Hidy 2012.*/
	double carbonchange_THN;   /* kgC/m2  total of thinning carbon change   */
	double carbonchange_MOW;   /* kgC/m2  total of mowing carbon change   */
	double carbonchange_HRV;   /* kgC/m2  total of harvesting carbon change   */
	double carbonchange_PLG;	   /* kgC/m2  total of plouging carbon change   */
	double carbonchange_GRZ;	   /* kgC/m2  total of grazing carbon change   */
	double carbonchange_PLT;   /* kgC/m2  total of planting carbon change   */
	double carbonchange_FRZ;  /* kgC/m2  total of fertilizing carbon change   */
} summary_struct;

/* restart data structure */
typedef struct
{
	double soilw[N_SOILLAYERS];
	double snoww;
	double canopyw;
	double leafc;
	double leafc_storage;
	double leafc_transfer;
	double frootc;
	double frootc_storage;
	double frootc_transfer;
	double livestemc;
	double livestemc_storage;
	double livestemc_transfer;
	double deadstemc;
	double deadstemc_storage;
	double deadstemc_transfer;
	double livecrootc;
	double livecrootc_storage;
	double livecrootc_transfer;
	double deadcrootc;
	double deadcrootc_storage;
	double deadcrootc_transfer;
	double gresp_storage;
	double gresp_transfer;
	double cwdc;
	double litr1c;
	double litr2c;
	double litr3c;
	double litr4c;
	/* Hidy 2013: senescence */
//	double litr1c_strg_SNSC;
//	double litr2c_strg_SNSC;
//	double litr3c_strg_SNSC;
//	double litr4c_strg_SNSC;
	double soil1c;
	double soil2c;
	double soil3c;
	double soil4c;
	double cpool;
	double leafn;
	double leafn_storage;
	double leafn_transfer;
	double frootn;
	double frootn_storage;
	double frootn_transfer;
	double livestemn;
	double livestemn_storage;
	double livestemn_transfer;
	double deadstemn;
	double deadstemn_storage;
	double deadstemn_transfer;
	double livecrootn;
	double livecrootn_storage;
	double livecrootn_transfer;
	double deadcrootn;
	double deadcrootn_storage;
	double deadcrootn_transfer;
	double cwdn;
	double litr1n;
	double litr2n;
	double litr3n;
	double litr4n;
	/* Hidy 2013: senescence */
//  double litr1n_strg_SNSC;
//	double litr2n_strg_SNSC;
//	double litr3n_strg_SNSC;
//	double litr4n_strg_SNSC;
	double soil1n;
	double soil2n;
	double soil3n;
	double soil4n;
	/* Hidy 2011 - multilayer soil */
	double sminn_RZ;
	double sminn_RZ_befsoilproc;
	double sminn[N_SOILLAYERS];
	double retransn;
	double npool;
	double day_leafc_litfall_increment;
	double day_frootc_litfall_increment;
	double day_livestemc_turnover_increment;
	double day_livecrootc_turnover_increment;
	double annmax_leafc;
	double annmax_frootc;
	double annmax_livestemc;
	double annmax_livecrootc;
	double dsr;
	int metyr;
} restart_data_struct;
