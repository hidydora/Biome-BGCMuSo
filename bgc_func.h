/*
bgc_func.h
header file for function prototypes

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Biome-BGCMuSo v7.0.
Original code: Copyright 2000, Peter E. Thornton
Numerical Terradynamic Simulation Group, The University of Montana, USA
Modified code: Copyright 2022, D. Hidy [dori.hidy@gmail.com]
Hungarian Academy of Sciences, Hungary
See the website of Biome-BGCMuSo at http://nimbus.elte.hu/bbgc/ for documentation, model executable and example input files.
*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
*/

int leapControl(int year, int* enddays, int* mondays, int* leap);

int soilb_estimation(double sand, double silt, double* soilB, double* VWCsat,double* VWCfc, double* VWCwp,  
	                 double* BD, double* RCN, int* soiltype);
int multilayer_soilcalc(control_struct* ctrl,  soilprop_struct* sprop);


int output_map_init(double** output_map, phenology_struct* phen, metvar_struct* metv, wstate_struct* ws,
	wflux_struct* wf, cstate_struct* cs, cflux_struct* cf, nstate_struct* ns, nflux_struct* nf, 
	soilprop_struct* sprop, epvar_struct* epv, psn_struct* psn_sun, psn_struct* psn_shade, summary_struct* summary);

int make_zero_flux_struct(wflux_struct* wf, cflux_struct* cf, nflux_struct* nf, GWcalc_struct* gwc);

int annVARinit(summary_struct* summary, epvar_struct* epv, cstate_struct* cs, wstate_struct* ws, cflux_struct* cf, nflux_struct* nf);

int atm_pres(double elev, double* pa);

int restart_input(const control_struct* ctrl, const epconst_struct* epc, const soilprop_struct* sprop, const siteconst_struct* sitec, 
	              wstate_struct* ws, cstate_struct* cs, nstate_struct* ns, epvar_struct* epv, restart_data_struct* restart);

int firstday(const control_struct* ctrl, const soilprop_struct* sprop, const epconst_struct* epc, const planting_struct *PLT, 
	         siteconst_struct* sitec, cinit_struct* cinit, phenology_struct* phen, epvar_struct* epv, 
			 cstate_struct* cs, nstate_struct* ns, psn_struct* psn_sun, psn_struct* psn_shade);

int zero_srcsnk(cstate_struct* cs, nstate_struct* ns, wstate_struct* ws, summary_struct* summary);

int dayphen(control_struct* ctrl, const epconst_struct* epc, const phenarray_struct* phenarr, const planting_struct* PLT, phenology_struct* phen);

int management(control_struct* ctrl, fertilizing_struct* FRZ, grazing_struct* GRZ, harvesting_struct* HRV, mowing_struct* MOW, 
			   planting_struct* PLT, ploughing_struct* PLG, thinning_struct* THN, irrigating_struct* IRG, 
			   mulching_struct* MUL, CWDextract_struct* CWE, flooding_struct* FLD, groundwater_struct* GWS, int* mondays);

int multilayer_hydrolparams(const siteconst_struct* sitec, const soilprop_struct* sprop, wstate_struct* ws, epvar_struct* epv);

int daymet(const control_struct* ctrl,const metarr_struct* metarr, const epconst_struct* epc, metvar_struct* metv, double snoww);

int phenphase(file logfile, const control_struct* ctrl, const epconst_struct* epc, const soilprop_struct* sprop, const planting_struct* PLT, 
	          phenology_struct *phen, metvar_struct *metv, epvar_struct* epv, cstate_struct* cs);
	int vernalization(const epconst_struct* epc, const metvar_struct* metv, phenology_struct *phen);
	int photoslow(const epconst_struct* epc, const metvar_struct* metv, phenology_struct *phen);

int multilayer_tsoil(const epconst_struct* epc, const siteconst_struct* sitec, const soilprop_struct* sprop, const epvar_struct* epv, int yday, double snoww,
					 metvar_struct* metv);
int soilCover(siteconst_struct* sitec, soilprop_struct* sprop, metvar_struct* metv,  epvar_struct* epv, cstate_struct* cs);

int phenology(const epconst_struct* epc, const cstate_struct* cs, const nstate_struct* ns, 
	          phenology_struct* phen, metvar_struct* metv,epvar_struct* epv, cflux_struct* cf, nflux_struct* nf);
	int leaf_litfall(const epconst_struct* epc, double litfallc, cflux_struct* cf, nflux_struct* nf);
	int froot_litfall(const epconst_struct* epc, double litfallc, cflux_struct* cf, nflux_struct* nf);
	int yield_litfall(const epconst_struct* epc, double litfallc, cflux_struct* cf, nflux_struct* nf);
	int softstem_litfall(const epconst_struct* epc, double litfallc, cflux_struct* cf, nflux_struct* nf);
	int transfer_fromGDD(const epconst_struct* epc, const cstate_struct* cs, const nstate_struct* ns, 
	                     phenology_struct *phen, metvar_struct *metv, epvar_struct* epv, cflux_struct* cf, nflux_struct* nf);

int multilayer_rootDepth(const epconst_struct* epc, const soilprop_struct* sprop, const cstate_struct* cs, siteconst_struct* sitec,  epvar_struct* epv);
	int calc_nrootlayers(int flag, double maxRD, double frootc, siteconst_struct* sitec, epvar_struct* epv);

int radtrans(const control_struct* ctrl, const phenology_struct* phen, const cstate_struct* cs, const epconst_struct* epc, const siteconst_struct *sitec,
	         metvar_struct* metv, epvar_struct* epv);

int prcpANDrunoffH(const metvar_struct* metv, const soilprop_struct* sprop,  const epconst_struct* epc, epvar_struct *epv, wflux_struct* wf);

int snowmelt(const metvar_struct* metv, wflux_struct* wf, double snoww);

int Elimit_and_PET(const epconst_struct* epc, const soilprop_struct* sprop, const metvar_struct* metv, epvar_struct *epv, wflux_struct* wf);

int soilEVP_calc(control_struct* ctrl, const siteconst_struct* sitec,const soilprop_struct* sprop, epvar_struct* epv, wstate_struct* ws, wflux_struct* wf);

int conduct_calc(const control_struct* ctrl, const metvar_struct* metv, const epconst_struct* epc, epvar_struct* epv, int simyr);

int maint_resp(const planting_struct* PLT, const cstate_struct* cs, const nstate_struct* ns, const epconst_struct* epc, const metvar_struct* metv, 
	           epvar_struct* epv, cflux_struct* cf);

int canopy_et(const epconst_struct* epc, const metvar_struct* metv, epvar_struct* epv, wflux_struct* wf);

int penmon(const pmet_struct* in, int out_flag,	double* et);

int priestley(const metvar_struct* metv, const epvar_struct *epv, wflux_struct* wf);

int photosynthesis(const epconst_struct* epc, const metvar_struct* metv, const cstate_struct* cs, const wstate_struct* ws, const phenology_struct* phen, 
	               epvar_struct* epv, psn_struct* psn_sun, psn_struct* psn_shade, cflux_struct *cf); 
	int farquhar(const epconst_struct* epc, const metvar_struct* metv, psn_struct* psn);

int decomp(const metvar_struct* metv, const epconst_struct* epc, const soilprop_struct* sprop, const siteconst_struct* sitec, const cstate_struct* cs, const nstate_struct* ns, 
	       epvar_struct* epv, cflux_struct* cf, nflux_struct* nf, ntemp_struct* nt);
	int CH4flux_estimation(const soilprop_struct* sprop, int layer, double VWC, double T, double* CH4flux);

int daily_allocation(const epconst_struct* epc, const soilprop_struct* sprop, const metvar_struct* metv, const NdepControl_struct* ndep,
	                 cstate_struct*cs,  nstate_struct* ns, cflux_struct* cf, nflux_struct* nf, epvar_struct* epv, ntemp_struct* nt, double naddfrac);


int flowering_heatstress(const epconst_struct* epc, const metvar_struct *metv, cstate_struct* cs, epvar_struct* epv, cflux_struct* cf, nflux_struct* nf);

int annual_rates(const epconst_struct* epc, epvar_struct* epv);

int growth_resp(const epconst_struct* epc, cflux_struct* cf);

int multilayer_transpiration(control_struct* ctrl, const siteconst_struct* sitec, const soilprop_struct* sprop, 
	                          epvar_struct* epv, wstate_struct* ws, wflux_struct* wf);

int irrigating(const control_struct* ctrl, const irrigating_struct* IRG, const siteconst_struct* sitec, const soilprop_struct* sprop,
	           epvar_struct* epv, wstate_struct* ws, wflux_struct* wf);

int multilayer_hydrolprocess(control_struct* ctrl, siteconst_struct* sitec, soilprop_struct* sprop, const epconst_struct* epc, epvar_struct* epv, 
	                         wstate_struct* ws, wflux_struct* wf, groundwater_struct* GWS, GWcalc_struct* gwc, flooding_struct* FLD, int* mondays);
	int infiltANDpond(siteconst_struct* sitec, soilprop_struct* sprop, epvar_struct* epv, wstate_struct* ws, wflux_struct* wf);
	int pondANDrunoffD(control_struct* ctrl, siteconst_struct* sitec, soilprop_struct* sprop, epvar_struct* epv, wstate_struct* ws, wflux_struct* wf);
	int richards(const epconst_struct* epc, soilprop_struct* sprop, wstate_struct* ws, wflux_struct* wf, GWcalc_struct* gwc);
	int tipping(siteconst_struct* sitec, soilprop_struct* sprop, const epconst_struct* epc, epvar_struct* epv, wstate_struct* ws, wflux_struct* wf);
	int diffusCalc(const soilprop_struct* sprop, double dz0, double VWC0, double rVWC0, double VWC0_sat, double VWC0_fc, double VWC0_wp, 
		                                         double dz1, double VWC1, double rVWC1, double VWC1_sat, double VWC1_fc, double VWC1_wp, double* soilwDiffus);
	int soilstress_calculation(soilprop_struct* sprop, const epconst_struct* epc, 
		                       epvar_struct* epv, wstate_struct* ws, wflux_struct* wf);
	int flooding(control_struct* ctrl, const siteconst_struct* sitec, const flooding_struct* FLD, soilprop_struct* sprop, epvar_struct* epv, 
		         wstate_struct* ws, wflux_struct* wf, int* mondays);
	int groundwaterT_preproc(const control_struct* ctrl, const epconst_struct* epc, const siteconst_struct* sitec, soilprop_struct* sprop, epvar_struct* epv, 
	            wstate_struct* ws, wflux_struct* wf, groundwater_struct* GWS);
	int groundwaterT_CFcharge(const siteconst_struct* sitec, soilprop_struct* sprop, epvar_struct* epv, 
	            wstate_struct* ws, wflux_struct* wf, groundwater_struct* GWS);
	int groundwaterR_preproc(const control_struct* ctrl, const siteconst_struct* sitec, soilprop_struct* sprop, epvar_struct* epv, 
	            wstate_struct* ws, wflux_struct* wf, groundwater_struct* GWS, GWcalc_struct* gwc);
	int groundwaterR_postproc(const siteconst_struct* sitec, epvar_struct* epv, wstate_struct* ws, wflux_struct* wf, GWcalc_struct* gwc);
	int potEVPsurface_to_actEVPsurface(control_struct* ctrl, const siteconst_struct* sitec, soilprop_struct* sprop, 
	                   epvar_struct* epv, wstate_struct* ws, wflux_struct* wf);
		int EVPphase1TOphase2(const soilprop_struct* sprop, epvar_struct* epv, wstate_struct* ws, wflux_struct* wf);

int water_state_update(const wflux_struct* wf, wstate_struct* ws);

int CN_state_update(const siteconst_struct* sitec, const epconst_struct* epc, control_struct* ctrl, epvar_struct* epv, 
	                      cflux_struct* cf, nflux_struct* nf, cstate_struct* cs, nstate_struct* ns, int alloc, int evergreen);
	int nsc_maintresp(const epconst_struct* epc, control_struct *ctrl, epvar_struct* epv, cflux_struct* cf, nflux_struct* nf, cstate_struct* cs, nstate_struct* ns);
	int CNratio_control(cstate_struct* cs, double CNratio, double cpool, double npool, double cflux, double nflux, double CNratio_flux);

int senescence(const siteconst_struct *sitec, const epconst_struct* epc, const grazing_struct* GRZ, const metvar_struct* metv, 
			   control_struct* ctrl, cstate_struct* cs, cflux_struct* cf, nstate_struct* ns, nflux_struct* nf, epvar_struct* epv);
	int genprog_senescence(const epconst_struct* epc, const metvar_struct* metv, epvar_struct* epv, cflux_struct* cf, nflux_struct* nf);

int mortality(const control_struct* ctrl, const siteconst_struct* sitec, const epconst_struct* epc, 
	          epvar_struct* epv, cstate_struct* cs, cflux_struct* cf, nstate_struct* ns, nflux_struct* nf, int simyr);	

int multilayer_sminn(const control_struct* ctrl, const metvar_struct* metv,  const soilprop_struct* sprop, const siteconst_struct* sitec, const cflux_struct* cf, const NdepControl_struct *ndep,
	                 epvar_struct* epv, nstate_struct* ns, nflux_struct* nf);
	int nitrification(int layer, const soilprop_struct* sprop, double net_miner, double tsoil, double pH, double WFPS, double sminNH4avail, 
	                  epvar_struct* epv, double* N2O_flux_NITRIF, double* sminNH4_to_nitrif);
	int denitrification(double soiltype, double sminNO3avail_ppm, double pH, double WFPS, double SR_total, 
		                double* sminNO3_to_denitr, double* ratioN2_N2O);

int multilayer_leaching(const soilprop_struct* sprop, const epvar_struct* epv,
					    control_struct* ctrl, cstate_struct* cs, cflux_struct* cf, nstate_struct* ns, nflux_struct* nf, wstate_struct* ws, wflux_struct* wf);

int planting(control_struct* ctrl, const siteconst_struct* sitec, const planting_struct* PLT, epconst_struct* epc, 
	         epvar_struct* epv, phenology_struct* phen, cstate_struct* cs, nstate_struct*ns, cflux_struct* cf, nflux_struct* nf);
	int planttype_determination(control_struct* ctrl, phenology_struct* phen);

int thinning(const control_struct* ctrl, const epconst_struct* epc, const thinning_struct* THN, 
	         cstate_struct* cs, nstate_struct* ns, wstate_struct* ws, cflux_struct* cf, nflux_struct* nf, wflux_struct* wf);

int mowing(const control_struct* ctrl, const epconst_struct* epc, const mowing_struct* MOW, const epvar_struct* epv, 
		   cstate_struct* cs, nstate_struct* ns, wstate_struct* ws, cflux_struct* cf, nflux_struct* nf, wflux_struct* wf);

int grazing(control_struct* ctrl, const epconst_struct* epc, const siteconst_struct* sitec, grazing_struct* GRZ, epvar_struct* epv,
	        cstate_struct* cs, nstate_struct* ns, wstate_struct* ws, cflux_struct* cf, nflux_struct* nf, wflux_struct* wf, int* mondays);

int harvesting(file econout, control_struct* ctrl, phenology_struct* phen, const epconst_struct* epc, const harvesting_struct* HRV, const irrigating_struct* IRG,
	           epvar_struct* epv, cstate_struct* cs, nstate_struct* ns, wstate_struct* ws, cflux_struct* cf, nflux_struct* nf, wflux_struct* wf);

int ploughing(const control_struct* ctrl, const epconst_struct* epc, siteconst_struct* sitec, soilprop_struct* sprop, metvar_struct* metv, epvar_struct* epv, 
	          ploughing_struct* PLG, cstate_struct* cs, nstate_struct* ns, wstate_struct* ws, cflux_struct* cf, nflux_struct* nf, wflux_struct* wf);

int fertilizing(const control_struct* ctrl, const siteconst_struct* sitec, const soilprop_struct* sprop, fertilizing_struct* FRZ, 
				cstate_struct* cs, nstate_struct* ns, wstate_struct* ws, cflux_struct* cf, nflux_struct* nf, wflux_struct* wf);

int mulching(control_struct* ctrl, const mulching_struct* MUL,  cstate_struct* cs, nstate_struct*ns, cflux_struct* cf, nflux_struct* nf);

int CWDextract(control_struct* ctrl, const CWDextract_struct* CWE, cstate_struct* cs, nstate_struct*ns, cflux_struct* cf, nflux_struct* nf);

int cutdown2litter(const siteconst_struct *sitec, const epconst_struct* epc, const epvar_struct* epv, cstate_struct* cs, cflux_struct* cf, nstate_struct* ns, nflux_struct* nf);

int precision_control(wstate_struct* ws, cstate_struct* cs, nstate_struct* ns);

int check_water_balance(wstate_struct* ws, int first_balance);
int check_carbon_balance(cstate_struct* cs, int first_balance);
int check_nitrogen_balance(nstate_struct* ns, int first_balance);

int cnw_summary(const epconst_struct* epc, const siteconst_struct* sitec, const soilprop_struct* sprop, const metvar_struct* metv, 
	            const cstate_struct* cs, const cflux_struct* cf, const nstate_struct* ns, const nflux_struct* nf, const wflux_struct* wf, 
				epvar_struct* epv, summary_struct* summary);

int restart_output(const wstate_struct* ws, const cstate_struct* cs, const nstate_struct* ns, const epvar_struct* epv, restart_data_struct* restart);