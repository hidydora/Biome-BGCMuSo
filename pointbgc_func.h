/*
pointbgc_func.h
function prototypes for pointbgc
for use with pointbgc front-end to BIOME-BGC library v4.1

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Biome-BGC version 4.1.1
Copyright 2000, Peter E. Thornton
Numerical Terradynamics Simulation Group (NTSG)
School of Forestry, University of Montana
Missoula, MT 59812
*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Modified:
13/07/2000: Added input of Ndep from file=> added simyears function to
ramp_ndep_init function. Changes are made by Galina Churkina.
*/

int met_init(file init, point_struct* point);
int restart_init(file init, restart_ctrl_struct* restart);
int time_init(file init, control_struct *ctrl);
int scc_init(file init, climchange_struct* scc);
int co2_init(file init, co2control_struct* co2, int simyears);
int sitec_init(file init, siteconst_struct* sitec);
int ramp_ndep_init(file init, ramp_ndep_struct* ramp_ndep, int simyears);
/* Hidy 2011: new parameter of epc_init function sitec to contol new EPC parameters */
int epc_init(file init, const siteconst_struct* sitec, epconst_struct* epc, control_struct* ctrl);
int wstate_init(file init, const siteconst_struct* sitec, wstate_struct* ws);
int cnstate_init(file init, const epconst_struct* epc, cstate_struct* cs,
	cinit_struct* cinit, nstate_struct* ns);
int output_init(file init, output_struct* output);
int end_init(file init);
int metarr_init(file metf, metarr_struct* metarr, const climchange_struct* scc,
int nyears);
int presim_state_init(wstate_struct* ws, cstate_struct* cs, nstate_struct* ns,
	cinit_struct* cinit);
int prephenology(const control_struct* ctrl, const epconst_struct* epc, 
	const siteconst_struct* sitec, const metarr_struct* metarr, phenarray_struct* phenarr);

/* new init functions - modified by Hidy 2008.*/
int thinning_init(file init, int max_THNdays, thinning_struct* THN);
int mowing_init(file init, int max_MOWdays, mowing_struct* MOW);
int grazing_init(file init, grazing_struct* GRZ);
int harvesting_init(file init, int max_HRVdays, harvesting_struct* HRV);
int ploughing_init(file init, int max_PLGdays, ploughing_struct* PLG);
int fertilizing_init(file init, int max_FRZdays, fertilizing_struct* FRZ);
int planting_init(file init, int max_PLTdays, planting_struct* PLT);
int GSI_init(file init, GSI_struct* GSI);

/* model corrections - Hidy 2009. */
int GSI_calculation(const metarr_struct* metarr, const control_struct* ctrl, const siteconst_struct* sitec, 
	GSI_struct* GSI, phenarray_struct* phenarr);


