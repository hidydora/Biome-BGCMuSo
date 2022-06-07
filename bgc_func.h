/*
bgc_func.h
header file for function prototypes

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Biome-BGC version 4.1.1
Copyright 2000, Peter E. Thornton
Numerical Terradynamics Simulation Group (NTSG)
School of Forestry, University of Montana
Missoula, MT 59812
*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
*/

int output_map_init(double** output_map, metvar_struct* metv, wstate_struct* ws,
	wflux_struct* wf, cstate_struct* cs, cflux_struct* cf, nstate_struct* ns,
	nflux_struct* nf, phenology_struct* phen, epvar_struct* epv,
	psn_struct* psn_sun, psn_struct* psn_shade, summary_struct* summary);
int make_zero_flux_struct(wflux_struct* wf, cflux_struct* cf,
	nflux_struct* nf);
int atm_pres(double elev, double* pa);

/* Hidy 2010 - plus/new input variables */
int restart_input(const siteconst_struct* sitec, control_struct* ctrl, wstate_struct* ws, cstate_struct* cs,
	nstate_struct* ns, epvar_struct* epv, int* metyr, restart_data_struct* restart);
int restart_output(const siteconst_struct* sitec,  control_struct* ctrl, wstate_struct* ws,cstate_struct* cs,
	nstate_struct* ns, epvar_struct* epv, int metyr, restart_data_struct* restart);

int free_phenmem(phenarray_struct* phen);

/* Hidy 2010 - plus/new input variables */
int firstday(const siteconst_struct* sitec, const epconst_struct* epc, const cinit_struct* cinit,
	epvar_struct* epv, phenarray_struct* phen, cstate_struct* cs, nstate_struct* ns, metvar_struct* metv);

int precision_control(const siteconst_struct* sitec, wstate_struct* ws, cstate_struct* cs, nstate_struct* ns);
int zero_srcsnk(cstate_struct* cs, nstate_struct* ns, wstate_struct* ws,
	summary_struct* summary);

/* Hidy 2010 - plus/new input variables */
int daymet(const control_struct* ctrl, const metarr_struct* metarr, const siteconst_struct* sitec,
    const wstate_struct* ws, metvar_struct* metv, double* tair_annavg_ptr, int metday);
int dayphen(const phenarray_struct* phenarr, phenology_struct* phen, int metday, int metyr);

int phenology(const epconst_struct* epc, const phenology_struct* phen,
	epvar_struct* epv, cstate_struct* cs, cflux_struct* cf, nstate_struct* ns,
	nflux_struct* nf);
int leaf_litfall(const epconst_struct* epc, double litfallc,
	cflux_struct* cf, nflux_struct* nf);
int froot_litfall(const epconst_struct* epc, double litfallc, 
	cflux_struct* cf, nflux_struct* nf);
int radtrans(const cstate_struct* cs, const epconst_struct* epc, 
	metvar_struct* metv, epvar_struct* epv, double albedo);

int snowmelt(const metvar_struct* metv, wflux_struct* wf, double snoww);

int baresoil_evap(const metvar_struct* metv, wflux_struct* wf, double* dsr_ptr);

/* Hidy 2010 - plus/new input variables */
int prcp_route(const metvar_struct* metv, double precip_int_coef,double all_lai, wflux_struct* wf);

int maint_resp(const cstate_struct* cs, const nstate_struct* ns, const epconst_struct* epc,  const metvar_struct* metv, 
	const siteconst_struct* sitec, cflux_struct* cf, epvar_struct* epv);

/* Hidy 2010 - plus/new input variables */
int canopy_et(const metvar_struct* metv, epvar_struct* epv, wflux_struct* wf);
int penmon(const pmet_struct* in, int out_flag,	double* et);
int photosynthesis(psn_struct* psn);
int decomp(const metvar_struct* metv, const epconst_struct* epc, epvar_struct* epv, 
	const siteconst_struct* sitec, cstate_struct* cs, cflux_struct* cf,
nstate_struct* ns, nflux_struct* nf, ntemp_struct* nt);

/* Hidy 2010 - plus/new input variables */
int daily_allocation(cflux_struct* cf, cstate_struct* cs,
	nflux_struct* nf, nstate_struct* ns, epconst_struct* epc, epvar_struct* epv,
	ntemp_struct* nt, int* n_limitation_ptr);
int spinup_daily_allocation(cflux_struct* cf, cstate_struct* cs,
	nflux_struct* nf, nstate_struct* ns, epconst_struct* epc, epvar_struct* epv,
	ntemp_struct* nt, double naddfrac);
int annual_rates(const epconst_struct* epc, epvar_struct* epv);
int growth_resp(epconst_struct* epc, cflux_struct* cf);
/* Hidy 2010 - plus/new input variables */
int daily_water_state_update(const control_struct* ctrl, const siteconst_struct* sitec, wflux_struct* wf, wstate_struct* ws);
/* Hidy 2010 - plus/new input variables */
int daily_carbon_state_update(const control_struct* ctrl, cflux_struct* cf, cstate_struct* cs, int alloc, int woody, int evergreen);
/* Hidy 2010 - plus/new input variables */
int daily_nitrogen_state_update(const control_struct* ctrl,nflux_struct* nf, nstate_struct* ns, int alloc, int woody, int evergreen);


/* Hidy 2010 - plus/new input variables */
int mortality(const control_struct* ctrl, const epconst_struct* epc, cstate_struct* cs, cflux_struct* cf,
	nstate_struct* ns, nflux_struct* nf, epvar_struct* epv, int simyr);	

/* Hidy 2010 - plus/new input variables */
int check_water_balance(wstate_struct* ws, control_struct* ctrl, int first_balance);
int check_carbon_balance(cstate_struct* cs, control_struct* ctrl, int first_balance);
int check_nitrogen_balance(nstate_struct* ns, control_struct* ctrl, int first_balance);

/* Hidy 2010 - plus/new input variables */
int csummary(cflux_struct* cf, cstate_struct* cs, nflux_struct* nf, summary_struct* summary);

/*****************************/
/* Hidy 2011 - NEW FUNCTIONS: water stress, multilayer soil and management */

/* calculating conductance values based on limitation factors (original: included into canopy_et.c) */
int conduct_calc(const metvar_struct* metv, const epconst_struct* epc, const siteconst_struct* sitec, epvar_struct* epv);

/* calculating the water stress days */
int waterstress_days(int yday, phenology_struct* phen, epvar_struct* epv);

/* change of mineralized N in multilayer soil */
int multilayer_sminn(const siteconst_struct* sitec, const epvar_struct* epv, nstate_struct* ns, nflux_struct* nf, 
					 wstate_struct* ws, wflux_struct* wf);

/* calculate the hydrological parameters in the multilayer soil */
int multilayer_hydrolparams(const control_struct* ctrl, const siteconst_struct* sitec,  wstate_struct* ws, epvar_struct* epv);

/* calculate the hydrological processes in the multilayer soil */
int multilayer_hydrolprocess(const control_struct* ctrl, const siteconst_struct* sitec, const epconst_struct* epc, epvar_struct* epv,
	wstate_struct* ws, wflux_struct* wf);

/* calculate the transpiration processes in the multilayer soil */
int multilayer_transpiration(const control_struct* ctrl, const siteconst_struct* sitec,const epconst_struct* epc, epvar_struct* epv,
							 wstate_struct* ws, wflux_struct* wf);

/* calculating tsoil in the multilayer soil  */
int multilayer_tsoil(const epconst_struct* epc, const siteconst_struct* sitec, const wstate_struct* ws, 
					 metvar_struct* metv, epvar_struct* epv);

/* calculating rooting depth in the multilayer soil  */
int multilayer_rootdepth(const control_struct* ctrl, const epconst_struct* epc, const siteconst_struct* sitec, 
						 const phenology_struct* phen, epvar_struct* epv, nstate_struct* ns);

/* planting - Hidy 2012 */
int planting(int yday, const control_struct* ctrl,const epconst_struct* epc, planting_struct* PLT, 
            cstate_struct* cs, nstate_struct*ns, cflux_struct* cf, nflux_struct* nf,  phenology_struct* phen);

/* thinning - Hidy 2012 */
int thinning(int yday, phenology_struct* phen, const control_struct* ctrl, const epconst_struct* epc, 
		   thinning_struct* THN, cflux_struct* cf, nflux_struct* nf, wflux_struct* wf, 
	       cstate_struct* cs, nstate_struct* ns, wstate_struct* ws);

/* mowing - Hidy 2012 */
int mowing(int yday, phenology_struct* phen, const control_struct* ctrl, const epconst_struct* epc, 
		   mowing_struct* MOW, cflux_struct* cf, nflux_struct* nf, wflux_struct* wf, 
	       cstate_struct* cs, nstate_struct* ns, wstate_struct* ws);

/* harvesting - Hidy 2012 */
int harvesting(int yday, phenology_struct* phen, const control_struct* ctrl, const epconst_struct* epc, 
			   harvesting_struct* HRV, cflux_struct* cf, nflux_struct* nf, wflux_struct* wf, 
	           cstate_struct* cs, nstate_struct* ns, wstate_struct* ws);

/* ploughing - Hidy 2012 */
int ploughing(int yday, phenology_struct* phen, const control_struct* ctrl, const epconst_struct* epc, 
			   ploughing_struct* PLG, cflux_struct* cf, nflux_struct* nf, wflux_struct* wf, 
	           cstate_struct* cs, nstate_struct* ns, wstate_struct* ws);

/* grazing - Hidy 2012 */
int grazing(int yday, phenology_struct* phen, const control_struct* ctrl, const epconst_struct* epc, 
			grazing_struct* GRZ, cflux_struct* cf, nflux_struct* nf, wflux_struct* wf, 
	        cstate_struct* cs, nstate_struct* ns, wstate_struct* ws);

/* fertilizing - Hidy 2012 */
int fertilizing(int yday, const control_struct* ctrl, fertilizing_struct* FRZ, 
				cstate_struct* cs, nstate_struct*ns, cflux_struct* cf, nflux_struct* nf);

/* limitation factors of conductance calculation - Hidy 2012. */
int conduct_limit_factors(const siteconst_struct* sitec, const epconst_struct* epc, epvar_struct* epv);