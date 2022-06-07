# Microsoft Developer Studio Generated NMAKE File, Based on bgc411.dsp
!IF "$(CFG)" == ""
CFG=bgc411 - Win32 Release
!MESSAGE No configuration specified. Defaulting to bgc411 - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "bgc411 - Win32 Release" && "$(CFG)" != "bgc411 - Win32 DebugDojka"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "bgc411.mak" CFG="bgc411 - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "bgc411 - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "bgc411 - Win32 DebugDojka" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "bgc411 - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "..\..\bgc411.exe" "$(OUTDIR)\bgc411.bsc"


CLEAN :
	-@erase "$(INTDIR)\annual_rates.obj"
	-@erase "$(INTDIR)\annual_rates.sbr"
	-@erase "$(INTDIR)\atm_pres.obj"
	-@erase "$(INTDIR)\atm_pres.sbr"
	-@erase "$(INTDIR)\baresoil_evap.obj"
	-@erase "$(INTDIR)\baresoil_evap.sbr"
	-@erase "$(INTDIR)\bgc.obj"
	-@erase "$(INTDIR)\bgc.sbr"
	-@erase "$(INTDIR)\canopy_et.obj"
	-@erase "$(INTDIR)\canopy_et.sbr"
	-@erase "$(INTDIR)\check_balance.obj"
	-@erase "$(INTDIR)\check_balance.sbr"
	-@erase "$(INTDIR)\co2_init.obj"
	-@erase "$(INTDIR)\co2_init.sbr"
	-@erase "$(INTDIR)\conduct_calc.obj"
	-@erase "$(INTDIR)\conduct_calc.sbr"
	-@erase "$(INTDIR)\conduct_limit_factors.obj"
	-@erase "$(INTDIR)\conduct_limit_factors.sbr"
	-@erase "$(INTDIR)\daily_allocation.obj"
	-@erase "$(INTDIR)\daily_allocation.sbr"
	-@erase "$(INTDIR)\daymet.obj"
	-@erase "$(INTDIR)\daymet.sbr"
	-@erase "$(INTDIR)\dayphen.obj"
	-@erase "$(INTDIR)\dayphen.sbr"
	-@erase "$(INTDIR)\decomp.obj"
	-@erase "$(INTDIR)\decomp.sbr"
	-@erase "$(INTDIR)\end_init.obj"
	-@erase "$(INTDIR)\end_init.sbr"
	-@erase "$(INTDIR)\epc_init.obj"
	-@erase "$(INTDIR)\epc_init.sbr"
	-@erase "$(INTDIR)\fertilizing.obj"
	-@erase "$(INTDIR)\fertilizing.sbr"
	-@erase "$(INTDIR)\fertilizing_init.obj"
	-@erase "$(INTDIR)\fertilizing_init.sbr"
	-@erase "$(INTDIR)\firstday.obj"
	-@erase "$(INTDIR)\firstday.sbr"
	-@erase "$(INTDIR)\grazing.obj"
	-@erase "$(INTDIR)\grazing.sbr"
	-@erase "$(INTDIR)\grazing_init.obj"
	-@erase "$(INTDIR)\grazing_init.sbr"
	-@erase "$(INTDIR)\groundwater.obj"
	-@erase "$(INTDIR)\groundwater.sbr"
	-@erase "$(INTDIR)\groundwater_init.obj"
	-@erase "$(INTDIR)\groundwater_init.sbr"
	-@erase "$(INTDIR)\growth_resp.obj"
	-@erase "$(INTDIR)\growth_resp.sbr"
	-@erase "$(INTDIR)\GSI_calculation.obj"
	-@erase "$(INTDIR)\GSI_calculation.sbr"
	-@erase "$(INTDIR)\GSI_init.obj"
	-@erase "$(INTDIR)\GSI_init.sbr"
	-@erase "$(INTDIR)\harvesting.obj"
	-@erase "$(INTDIR)\harvesting.sbr"
	-@erase "$(INTDIR)\harvesting_init.obj"
	-@erase "$(INTDIR)\harvesting_init.sbr"
	-@erase "$(INTDIR)\ini.obj"
	-@erase "$(INTDIR)\ini.sbr"
	-@erase "$(INTDIR)\irrigation.obj"
	-@erase "$(INTDIR)\irrigation.sbr"
	-@erase "$(INTDIR)\irrigation_init.obj"
	-@erase "$(INTDIR)\irrigation_init.sbr"
	-@erase "$(INTDIR)\maint_resp.obj"
	-@erase "$(INTDIR)\maint_resp.sbr"
	-@erase "$(INTDIR)\make_zero_flux_struct.obj"
	-@erase "$(INTDIR)\make_zero_flux_struct.sbr"
	-@erase "$(INTDIR)\management.obj"
	-@erase "$(INTDIR)\management.sbr"
	-@erase "$(INTDIR)\met_init.obj"
	-@erase "$(INTDIR)\met_init.sbr"
	-@erase "$(INTDIR)\metarr_init.obj"
	-@erase "$(INTDIR)\metarr_init.sbr"
	-@erase "$(INTDIR)\mortality.obj"
	-@erase "$(INTDIR)\mortality.sbr"
	-@erase "$(INTDIR)\mowing.obj"
	-@erase "$(INTDIR)\mowing.sbr"
	-@erase "$(INTDIR)\mowing_init.obj"
	-@erase "$(INTDIR)\mowing_init.sbr"
	-@erase "$(INTDIR)\multilayer_hydrolparams.obj"
	-@erase "$(INTDIR)\multilayer_hydrolparams.sbr"
	-@erase "$(INTDIR)\multilayer_hydrolprocess.obj"
	-@erase "$(INTDIR)\multilayer_hydrolprocess.sbr"
	-@erase "$(INTDIR)\multilayer_rootdepth.obj"
	-@erase "$(INTDIR)\multilayer_rootdepth.sbr"
	-@erase "$(INTDIR)\multilayer_sminn.obj"
	-@erase "$(INTDIR)\multilayer_sminn.sbr"
	-@erase "$(INTDIR)\multilayer_transpiration.obj"
	-@erase "$(INTDIR)\multilayer_transpiration.sbr"
	-@erase "$(INTDIR)\multilayer_tsoil.obj"
	-@erase "$(INTDIR)\multilayer_tsoil.sbr"
	-@erase "$(INTDIR)\ndep_init.obj"
	-@erase "$(INTDIR)\ndep_init.sbr"
	-@erase "$(INTDIR)\otherGHGflux_estimation.obj"
	-@erase "$(INTDIR)\otherGHGflux_estimation.sbr"
	-@erase "$(INTDIR)\output_init.obj"
	-@erase "$(INTDIR)\output_init.sbr"
	-@erase "$(INTDIR)\output_map_init.obj"
	-@erase "$(INTDIR)\output_map_init.sbr"
	-@erase "$(INTDIR)\phenology.obj"
	-@erase "$(INTDIR)\phenology.sbr"
	-@erase "$(INTDIR)\photosynthesis.obj"
	-@erase "$(INTDIR)\photosynthesis.sbr"
	-@erase "$(INTDIR)\planting.obj"
	-@erase "$(INTDIR)\planting.sbr"
	-@erase "$(INTDIR)\planting_init.obj"
	-@erase "$(INTDIR)\planting_init.sbr"
	-@erase "$(INTDIR)\ploughing.obj"
	-@erase "$(INTDIR)\ploughing.sbr"
	-@erase "$(INTDIR)\ploughing_init.obj"
	-@erase "$(INTDIR)\ploughing_init.sbr"
	-@erase "$(INTDIR)\pointbgc.obj"
	-@erase "$(INTDIR)\pointbgc.sbr"
	-@erase "$(INTDIR)\prcp_route.obj"
	-@erase "$(INTDIR)\prcp_route.sbr"
	-@erase "$(INTDIR)\precision_control.obj"
	-@erase "$(INTDIR)\precision_control.sbr"
	-@erase "$(INTDIR)\prephenology.obj"
	-@erase "$(INTDIR)\prephenology.sbr"
	-@erase "$(INTDIR)\presim_state_init.obj"
	-@erase "$(INTDIR)\presim_state_init.sbr"
	-@erase "$(INTDIR)\radtrans.obj"
	-@erase "$(INTDIR)\radtrans.sbr"
	-@erase "$(INTDIR)\read_mgmarray.obj"
	-@erase "$(INTDIR)\read_mgmarray.sbr"
	-@erase "$(INTDIR)\restart_init.obj"
	-@erase "$(INTDIR)\restart_init.sbr"
	-@erase "$(INTDIR)\restart_io.obj"
	-@erase "$(INTDIR)\restart_io.sbr"
	-@erase "$(INTDIR)\richards.obj"
	-@erase "$(INTDIR)\richards.sbr"
	-@erase "$(INTDIR)\scc_init.obj"
	-@erase "$(INTDIR)\scc_init.sbr"
	-@erase "$(INTDIR)\senescence.obj"
	-@erase "$(INTDIR)\senescence.sbr"
	-@erase "$(INTDIR)\sitec_init.obj"
	-@erase "$(INTDIR)\sitec_init.sbr"
	-@erase "$(INTDIR)\smooth.obj"
	-@erase "$(INTDIR)\smooth.sbr"
	-@erase "$(INTDIR)\snowmelt.obj"
	-@erase "$(INTDIR)\snowmelt.sbr"
	-@erase "$(INTDIR)\spinup_bgc.obj"
	-@erase "$(INTDIR)\spinup_bgc.sbr"
	-@erase "$(INTDIR)\spinup_daily_allocation.obj"
	-@erase "$(INTDIR)\spinup_daily_allocation.sbr"
	-@erase "$(INTDIR)\state_init.obj"
	-@erase "$(INTDIR)\state_init.sbr"
	-@erase "$(INTDIR)\state_update.obj"
	-@erase "$(INTDIR)\state_update.sbr"
	-@erase "$(INTDIR)\summary.obj"
	-@erase "$(INTDIR)\summary.sbr"
	-@erase "$(INTDIR)\thinning.obj"
	-@erase "$(INTDIR)\thinning.sbr"
	-@erase "$(INTDIR)\thinning_init.obj"
	-@erase "$(INTDIR)\thinning_init.sbr"
	-@erase "$(INTDIR)\time_init.obj"
	-@erase "$(INTDIR)\time_init.sbr"
	-@erase "$(INTDIR)\tipping.obj"
	-@erase "$(INTDIR)\tipping.sbr"
	-@erase "$(INTDIR)\transient_bgc.obj"
	-@erase "$(INTDIR)\transient_bgc.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\waterstress_days.obj"
	-@erase "$(INTDIR)\waterstress_days.sbr"
	-@erase "$(INTDIR)\zero_srcsnk.obj"
	-@erase "$(INTDIR)\zero_srcsnk.sbr"
	-@erase "$(OUTDIR)\bgc411.bsc"
	-@erase "..\..\bgc411.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /Zi /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\bgc411.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\bgc411.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\annual_rates.sbr" \
	"$(INTDIR)\atm_pres.sbr" \
	"$(INTDIR)\baresoil_evap.sbr" \
	"$(INTDIR)\bgc.sbr" \
	"$(INTDIR)\canopy_et.sbr" \
	"$(INTDIR)\check_balance.sbr" \
	"$(INTDIR)\co2_init.sbr" \
	"$(INTDIR)\conduct_calc.sbr" \
	"$(INTDIR)\conduct_limit_factors.sbr" \
	"$(INTDIR)\daily_allocation.sbr" \
	"$(INTDIR)\daymet.sbr" \
	"$(INTDIR)\dayphen.sbr" \
	"$(INTDIR)\decomp.sbr" \
	"$(INTDIR)\end_init.sbr" \
	"$(INTDIR)\epc_init.sbr" \
	"$(INTDIR)\fertilizing.sbr" \
	"$(INTDIR)\fertilizing_init.sbr" \
	"$(INTDIR)\firstday.sbr" \
	"$(INTDIR)\grazing.sbr" \
	"$(INTDIR)\grazing_init.sbr" \
	"$(INTDIR)\groundwater.sbr" \
	"$(INTDIR)\groundwater_init.sbr" \
	"$(INTDIR)\growth_resp.sbr" \
	"$(INTDIR)\GSI_calculation.sbr" \
	"$(INTDIR)\GSI_init.sbr" \
	"$(INTDIR)\harvesting.sbr" \
	"$(INTDIR)\harvesting_init.sbr" \
	"$(INTDIR)\ini.sbr" \
	"$(INTDIR)\irrigation.sbr" \
	"$(INTDIR)\irrigation_init.sbr" \
	"$(INTDIR)\maint_resp.sbr" \
	"$(INTDIR)\make_zero_flux_struct.sbr" \
	"$(INTDIR)\management.sbr" \
	"$(INTDIR)\met_init.sbr" \
	"$(INTDIR)\metarr_init.sbr" \
	"$(INTDIR)\mortality.sbr" \
	"$(INTDIR)\mowing.sbr" \
	"$(INTDIR)\mowing_init.sbr" \
	"$(INTDIR)\multilayer_hydrolparams.sbr" \
	"$(INTDIR)\multilayer_hydrolprocess.sbr" \
	"$(INTDIR)\multilayer_rootdepth.sbr" \
	"$(INTDIR)\multilayer_sminn.sbr" \
	"$(INTDIR)\multilayer_transpiration.sbr" \
	"$(INTDIR)\multilayer_tsoil.sbr" \
	"$(INTDIR)\ndep_init.sbr" \
	"$(INTDIR)\otherGHGflux_estimation.sbr" \
	"$(INTDIR)\output_init.sbr" \
	"$(INTDIR)\output_map_init.sbr" \
	"$(INTDIR)\phenology.sbr" \
	"$(INTDIR)\photosynthesis.sbr" \
	"$(INTDIR)\planting.sbr" \
	"$(INTDIR)\planting_init.sbr" \
	"$(INTDIR)\ploughing.sbr" \
	"$(INTDIR)\ploughing_init.sbr" \
	"$(INTDIR)\pointbgc.sbr" \
	"$(INTDIR)\prcp_route.sbr" \
	"$(INTDIR)\precision_control.sbr" \
	"$(INTDIR)\prephenology.sbr" \
	"$(INTDIR)\presim_state_init.sbr" \
	"$(INTDIR)\radtrans.sbr" \
	"$(INTDIR)\read_mgmarray.sbr" \
	"$(INTDIR)\restart_init.sbr" \
	"$(INTDIR)\restart_io.sbr" \
	"$(INTDIR)\richards.sbr" \
	"$(INTDIR)\scc_init.sbr" \
	"$(INTDIR)\senescence.sbr" \
	"$(INTDIR)\sitec_init.sbr" \
	"$(INTDIR)\smooth.sbr" \
	"$(INTDIR)\snowmelt.sbr" \
	"$(INTDIR)\spinup_bgc.sbr" \
	"$(INTDIR)\spinup_daily_allocation.sbr" \
	"$(INTDIR)\state_init.sbr" \
	"$(INTDIR)\state_update.sbr" \
	"$(INTDIR)\summary.sbr" \
	"$(INTDIR)\thinning.sbr" \
	"$(INTDIR)\thinning_init.sbr" \
	"$(INTDIR)\time_init.sbr" \
	"$(INTDIR)\tipping.sbr" \
	"$(INTDIR)\transient_bgc.sbr" \
	"$(INTDIR)\waterstress_days.sbr" \
	"$(INTDIR)\zero_srcsnk.sbr"

"$(OUTDIR)\bgc411.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\bgc411.pdb" /machine:I386 /out:"../../bgc411.exe" 
LINK32_OBJS= \
	"$(INTDIR)\annual_rates.obj" \
	"$(INTDIR)\atm_pres.obj" \
	"$(INTDIR)\baresoil_evap.obj" \
	"$(INTDIR)\bgc.obj" \
	"$(INTDIR)\canopy_et.obj" \
	"$(INTDIR)\check_balance.obj" \
	"$(INTDIR)\co2_init.obj" \
	"$(INTDIR)\conduct_calc.obj" \
	"$(INTDIR)\conduct_limit_factors.obj" \
	"$(INTDIR)\daily_allocation.obj" \
	"$(INTDIR)\daymet.obj" \
	"$(INTDIR)\dayphen.obj" \
	"$(INTDIR)\decomp.obj" \
	"$(INTDIR)\end_init.obj" \
	"$(INTDIR)\epc_init.obj" \
	"$(INTDIR)\fertilizing.obj" \
	"$(INTDIR)\fertilizing_init.obj" \
	"$(INTDIR)\firstday.obj" \
	"$(INTDIR)\grazing.obj" \
	"$(INTDIR)\grazing_init.obj" \
	"$(INTDIR)\groundwater.obj" \
	"$(INTDIR)\groundwater_init.obj" \
	"$(INTDIR)\growth_resp.obj" \
	"$(INTDIR)\GSI_calculation.obj" \
	"$(INTDIR)\GSI_init.obj" \
	"$(INTDIR)\harvesting.obj" \
	"$(INTDIR)\harvesting_init.obj" \
	"$(INTDIR)\ini.obj" \
	"$(INTDIR)\irrigation.obj" \
	"$(INTDIR)\irrigation_init.obj" \
	"$(INTDIR)\maint_resp.obj" \
	"$(INTDIR)\make_zero_flux_struct.obj" \
	"$(INTDIR)\management.obj" \
	"$(INTDIR)\met_init.obj" \
	"$(INTDIR)\metarr_init.obj" \
	"$(INTDIR)\mortality.obj" \
	"$(INTDIR)\mowing.obj" \
	"$(INTDIR)\mowing_init.obj" \
	"$(INTDIR)\multilayer_hydrolparams.obj" \
	"$(INTDIR)\multilayer_hydrolprocess.obj" \
	"$(INTDIR)\multilayer_rootdepth.obj" \
	"$(INTDIR)\multilayer_sminn.obj" \
	"$(INTDIR)\multilayer_transpiration.obj" \
	"$(INTDIR)\multilayer_tsoil.obj" \
	"$(INTDIR)\ndep_init.obj" \
	"$(INTDIR)\otherGHGflux_estimation.obj" \
	"$(INTDIR)\output_init.obj" \
	"$(INTDIR)\output_map_init.obj" \
	"$(INTDIR)\phenology.obj" \
	"$(INTDIR)\photosynthesis.obj" \
	"$(INTDIR)\planting.obj" \
	"$(INTDIR)\planting_init.obj" \
	"$(INTDIR)\ploughing.obj" \
	"$(INTDIR)\ploughing_init.obj" \
	"$(INTDIR)\pointbgc.obj" \
	"$(INTDIR)\prcp_route.obj" \
	"$(INTDIR)\precision_control.obj" \
	"$(INTDIR)\prephenology.obj" \
	"$(INTDIR)\presim_state_init.obj" \
	"$(INTDIR)\radtrans.obj" \
	"$(INTDIR)\read_mgmarray.obj" \
	"$(INTDIR)\restart_init.obj" \
	"$(INTDIR)\restart_io.obj" \
	"$(INTDIR)\richards.obj" \
	"$(INTDIR)\scc_init.obj" \
	"$(INTDIR)\senescence.obj" \
	"$(INTDIR)\sitec_init.obj" \
	"$(INTDIR)\smooth.obj" \
	"$(INTDIR)\snowmelt.obj" \
	"$(INTDIR)\spinup_bgc.obj" \
	"$(INTDIR)\spinup_daily_allocation.obj" \
	"$(INTDIR)\state_init.obj" \
	"$(INTDIR)\state_update.obj" \
	"$(INTDIR)\summary.obj" \
	"$(INTDIR)\thinning.obj" \
	"$(INTDIR)\thinning_init.obj" \
	"$(INTDIR)\time_init.obj" \
	"$(INTDIR)\tipping.obj" \
	"$(INTDIR)\transient_bgc.obj" \
	"$(INTDIR)\waterstress_days.obj" \
	"$(INTDIR)\zero_srcsnk.obj"

"..\..\bgc411.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\bgc411.exe"


CLEAN :
	-@erase "$(INTDIR)\annual_rates.obj"
	-@erase "$(INTDIR)\atm_pres.obj"
	-@erase "$(INTDIR)\baresoil_evap.obj"
	-@erase "$(INTDIR)\bgc.obj"
	-@erase "$(INTDIR)\canopy_et.obj"
	-@erase "$(INTDIR)\check_balance.obj"
	-@erase "$(INTDIR)\co2_init.obj"
	-@erase "$(INTDIR)\conduct_calc.obj"
	-@erase "$(INTDIR)\conduct_limit_factors.obj"
	-@erase "$(INTDIR)\daily_allocation.obj"
	-@erase "$(INTDIR)\daymet.obj"
	-@erase "$(INTDIR)\dayphen.obj"
	-@erase "$(INTDIR)\decomp.obj"
	-@erase "$(INTDIR)\end_init.obj"
	-@erase "$(INTDIR)\epc_init.obj"
	-@erase "$(INTDIR)\fertilizing.obj"
	-@erase "$(INTDIR)\fertilizing_init.obj"
	-@erase "$(INTDIR)\firstday.obj"
	-@erase "$(INTDIR)\grazing.obj"
	-@erase "$(INTDIR)\grazing_init.obj"
	-@erase "$(INTDIR)\groundwater.obj"
	-@erase "$(INTDIR)\groundwater_init.obj"
	-@erase "$(INTDIR)\growth_resp.obj"
	-@erase "$(INTDIR)\GSI_calculation.obj"
	-@erase "$(INTDIR)\GSI_init.obj"
	-@erase "$(INTDIR)\harvesting.obj"
	-@erase "$(INTDIR)\harvesting_init.obj"
	-@erase "$(INTDIR)\ini.obj"
	-@erase "$(INTDIR)\irrigation.obj"
	-@erase "$(INTDIR)\irrigation_init.obj"
	-@erase "$(INTDIR)\maint_resp.obj"
	-@erase "$(INTDIR)\make_zero_flux_struct.obj"
	-@erase "$(INTDIR)\management.obj"
	-@erase "$(INTDIR)\met_init.obj"
	-@erase "$(INTDIR)\metarr_init.obj"
	-@erase "$(INTDIR)\mortality.obj"
	-@erase "$(INTDIR)\mowing.obj"
	-@erase "$(INTDIR)\mowing_init.obj"
	-@erase "$(INTDIR)\multilayer_hydrolparams.obj"
	-@erase "$(INTDIR)\multilayer_hydrolprocess.obj"
	-@erase "$(INTDIR)\multilayer_rootdepth.obj"
	-@erase "$(INTDIR)\multilayer_sminn.obj"
	-@erase "$(INTDIR)\multilayer_transpiration.obj"
	-@erase "$(INTDIR)\multilayer_tsoil.obj"
	-@erase "$(INTDIR)\ndep_init.obj"
	-@erase "$(INTDIR)\otherGHGflux_estimation.obj"
	-@erase "$(INTDIR)\output_init.obj"
	-@erase "$(INTDIR)\output_map_init.obj"
	-@erase "$(INTDIR)\phenology.obj"
	-@erase "$(INTDIR)\photosynthesis.obj"
	-@erase "$(INTDIR)\planting.obj"
	-@erase "$(INTDIR)\planting_init.obj"
	-@erase "$(INTDIR)\ploughing.obj"
	-@erase "$(INTDIR)\ploughing_init.obj"
	-@erase "$(INTDIR)\pointbgc.obj"
	-@erase "$(INTDIR)\prcp_route.obj"
	-@erase "$(INTDIR)\precision_control.obj"
	-@erase "$(INTDIR)\prephenology.obj"
	-@erase "$(INTDIR)\presim_state_init.obj"
	-@erase "$(INTDIR)\radtrans.obj"
	-@erase "$(INTDIR)\read_mgmarray.obj"
	-@erase "$(INTDIR)\restart_init.obj"
	-@erase "$(INTDIR)\restart_io.obj"
	-@erase "$(INTDIR)\richards.obj"
	-@erase "$(INTDIR)\scc_init.obj"
	-@erase "$(INTDIR)\senescence.obj"
	-@erase "$(INTDIR)\sitec_init.obj"
	-@erase "$(INTDIR)\smooth.obj"
	-@erase "$(INTDIR)\snowmelt.obj"
	-@erase "$(INTDIR)\spinup_bgc.obj"
	-@erase "$(INTDIR)\spinup_daily_allocation.obj"
	-@erase "$(INTDIR)\state_init.obj"
	-@erase "$(INTDIR)\state_update.obj"
	-@erase "$(INTDIR)\summary.obj"
	-@erase "$(INTDIR)\thinning.obj"
	-@erase "$(INTDIR)\thinning_init.obj"
	-@erase "$(INTDIR)\time_init.obj"
	-@erase "$(INTDIR)\tipping.obj"
	-@erase "$(INTDIR)\transient_bgc.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\waterstress_days.obj"
	-@erase "$(INTDIR)\zero_srcsnk.obj"
	-@erase "$(OUTDIR)\bgc411.map"
	-@erase "..\..\bgc411.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W4 /GX /ZI /Od /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FAcs /Fa"$(INTDIR)\\" /Fp"$(INTDIR)\bgc411.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\bgc411.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /profile /map:"$(INTDIR)\bgc411.map" /debug /machine:I386 /out:"../../bgc411.exe" 
LINK32_OBJS= \
	"$(INTDIR)\annual_rates.obj" \
	"$(INTDIR)\atm_pres.obj" \
	"$(INTDIR)\baresoil_evap.obj" \
	"$(INTDIR)\bgc.obj" \
	"$(INTDIR)\canopy_et.obj" \
	"$(INTDIR)\check_balance.obj" \
	"$(INTDIR)\co2_init.obj" \
	"$(INTDIR)\conduct_calc.obj" \
	"$(INTDIR)\conduct_limit_factors.obj" \
	"$(INTDIR)\daily_allocation.obj" \
	"$(INTDIR)\daymet.obj" \
	"$(INTDIR)\dayphen.obj" \
	"$(INTDIR)\decomp.obj" \
	"$(INTDIR)\end_init.obj" \
	"$(INTDIR)\epc_init.obj" \
	"$(INTDIR)\fertilizing.obj" \
	"$(INTDIR)\fertilizing_init.obj" \
	"$(INTDIR)\firstday.obj" \
	"$(INTDIR)\grazing.obj" \
	"$(INTDIR)\grazing_init.obj" \
	"$(INTDIR)\groundwater.obj" \
	"$(INTDIR)\groundwater_init.obj" \
	"$(INTDIR)\growth_resp.obj" \
	"$(INTDIR)\GSI_calculation.obj" \
	"$(INTDIR)\GSI_init.obj" \
	"$(INTDIR)\harvesting.obj" \
	"$(INTDIR)\harvesting_init.obj" \
	"$(INTDIR)\ini.obj" \
	"$(INTDIR)\irrigation.obj" \
	"$(INTDIR)\irrigation_init.obj" \
	"$(INTDIR)\maint_resp.obj" \
	"$(INTDIR)\make_zero_flux_struct.obj" \
	"$(INTDIR)\management.obj" \
	"$(INTDIR)\met_init.obj" \
	"$(INTDIR)\metarr_init.obj" \
	"$(INTDIR)\mortality.obj" \
	"$(INTDIR)\mowing.obj" \
	"$(INTDIR)\mowing_init.obj" \
	"$(INTDIR)\multilayer_hydrolparams.obj" \
	"$(INTDIR)\multilayer_hydrolprocess.obj" \
	"$(INTDIR)\multilayer_rootdepth.obj" \
	"$(INTDIR)\multilayer_sminn.obj" \
	"$(INTDIR)\multilayer_transpiration.obj" \
	"$(INTDIR)\multilayer_tsoil.obj" \
	"$(INTDIR)\ndep_init.obj" \
	"$(INTDIR)\otherGHGflux_estimation.obj" \
	"$(INTDIR)\output_init.obj" \
	"$(INTDIR)\output_map_init.obj" \
	"$(INTDIR)\phenology.obj" \
	"$(INTDIR)\photosynthesis.obj" \
	"$(INTDIR)\planting.obj" \
	"$(INTDIR)\planting_init.obj" \
	"$(INTDIR)\ploughing.obj" \
	"$(INTDIR)\ploughing_init.obj" \
	"$(INTDIR)\pointbgc.obj" \
	"$(INTDIR)\prcp_route.obj" \
	"$(INTDIR)\precision_control.obj" \
	"$(INTDIR)\prephenology.obj" \
	"$(INTDIR)\presim_state_init.obj" \
	"$(INTDIR)\radtrans.obj" \
	"$(INTDIR)\read_mgmarray.obj" \
	"$(INTDIR)\restart_init.obj" \
	"$(INTDIR)\restart_io.obj" \
	"$(INTDIR)\richards.obj" \
	"$(INTDIR)\scc_init.obj" \
	"$(INTDIR)\senescence.obj" \
	"$(INTDIR)\sitec_init.obj" \
	"$(INTDIR)\smooth.obj" \
	"$(INTDIR)\snowmelt.obj" \
	"$(INTDIR)\spinup_bgc.obj" \
	"$(INTDIR)\spinup_daily_allocation.obj" \
	"$(INTDIR)\state_init.obj" \
	"$(INTDIR)\state_update.obj" \
	"$(INTDIR)\summary.obj" \
	"$(INTDIR)\thinning.obj" \
	"$(INTDIR)\thinning_init.obj" \
	"$(INTDIR)\time_init.obj" \
	"$(INTDIR)\tipping.obj" \
	"$(INTDIR)\transient_bgc.obj" \
	"$(INTDIR)\waterstress_days.obj" \
	"$(INTDIR)\zero_srcsnk.obj"

"..\..\bgc411.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("bgc411.dep")
!INCLUDE "bgc411.dep"
!ELSE 
!MESSAGE Warning: cannot find "bgc411.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "bgc411 - Win32 Release" || "$(CFG)" == "bgc411 - Win32 DebugDojka"
SOURCE=..\annual_rates.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\annual_rates.obj"	"$(INTDIR)\annual_rates.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\annual_rates.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\atm_pres.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\atm_pres.obj"	"$(INTDIR)\atm_pres.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\atm_pres.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\baresoil_evap.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\baresoil_evap.obj"	"$(INTDIR)\baresoil_evap.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\baresoil_evap.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\bgc.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\bgc.obj"	"$(INTDIR)\bgc.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\bgc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\canopy_et.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\canopy_et.obj"	"$(INTDIR)\canopy_et.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\canopy_et.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\check_balance.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\check_balance.obj"	"$(INTDIR)\check_balance.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\check_balance.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\co2_init.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\co2_init.obj"	"$(INTDIR)\co2_init.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\co2_init.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\conduct_calc.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\conduct_calc.obj"	"$(INTDIR)\conduct_calc.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\conduct_calc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\conduct_limit_factors.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\conduct_limit_factors.obj"	"$(INTDIR)\conduct_limit_factors.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\conduct_limit_factors.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\daily_allocation.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\daily_allocation.obj"	"$(INTDIR)\daily_allocation.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\daily_allocation.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\daymet.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\daymet.obj"	"$(INTDIR)\daymet.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\daymet.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\dayphen.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\dayphen.obj"	"$(INTDIR)\dayphen.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\dayphen.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\decomp.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\decomp.obj"	"$(INTDIR)\decomp.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\decomp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\end_init.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\end_init.obj"	"$(INTDIR)\end_init.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\end_init.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\epc_init.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\epc_init.obj"	"$(INTDIR)\epc_init.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\epc_init.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\fertilizing.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\fertilizing.obj"	"$(INTDIR)\fertilizing.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\fertilizing.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\fertilizing_init.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\fertilizing_init.obj"	"$(INTDIR)\fertilizing_init.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\fertilizing_init.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\firstday.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\firstday.obj"	"$(INTDIR)\firstday.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\firstday.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\grazing.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\grazing.obj"	"$(INTDIR)\grazing.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\grazing.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\grazing_init.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\grazing_init.obj"	"$(INTDIR)\grazing_init.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\grazing_init.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\groundwater.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\groundwater.obj"	"$(INTDIR)\groundwater.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\groundwater.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\groundwater_init.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\groundwater_init.obj"	"$(INTDIR)\groundwater_init.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\groundwater_init.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\growth_resp.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\growth_resp.obj"	"$(INTDIR)\growth_resp.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\growth_resp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\GSI_calculation.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\GSI_calculation.obj"	"$(INTDIR)\GSI_calculation.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\GSI_calculation.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\GSI_init.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\GSI_init.obj"	"$(INTDIR)\GSI_init.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\GSI_init.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\harvesting.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\harvesting.obj"	"$(INTDIR)\harvesting.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\harvesting.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\harvesting_init.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\harvesting_init.obj"	"$(INTDIR)\harvesting_init.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\harvesting_init.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\ini.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\ini.obj"	"$(INTDIR)\ini.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\ini.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\irrigation.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\irrigation.obj"	"$(INTDIR)\irrigation.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\irrigation.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\irrigation_init.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\irrigation_init.obj"	"$(INTDIR)\irrigation_init.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\irrigation_init.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\maint_resp.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\maint_resp.obj"	"$(INTDIR)\maint_resp.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\maint_resp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\make_zero_flux_struct.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\make_zero_flux_struct.obj"	"$(INTDIR)\make_zero_flux_struct.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\make_zero_flux_struct.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\management.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\management.obj"	"$(INTDIR)\management.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\management.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\met_init.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\met_init.obj"	"$(INTDIR)\met_init.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\met_init.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\metarr_init.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\metarr_init.obj"	"$(INTDIR)\metarr_init.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\metarr_init.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\mortality.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\mortality.obj"	"$(INTDIR)\mortality.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\mortality.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\mowing.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\mowing.obj"	"$(INTDIR)\mowing.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\mowing.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\mowing_init.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\mowing_init.obj"	"$(INTDIR)\mowing_init.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\mowing_init.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\multilayer_hydrolparams.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\multilayer_hydrolparams.obj"	"$(INTDIR)\multilayer_hydrolparams.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\multilayer_hydrolparams.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\multilayer_hydrolprocess.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\multilayer_hydrolprocess.obj"	"$(INTDIR)\multilayer_hydrolprocess.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\multilayer_hydrolprocess.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\multilayer_rootdepth.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\multilayer_rootdepth.obj"	"$(INTDIR)\multilayer_rootdepth.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\multilayer_rootdepth.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\multilayer_sminn.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\multilayer_sminn.obj"	"$(INTDIR)\multilayer_sminn.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\multilayer_sminn.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\multilayer_transpiration.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\multilayer_transpiration.obj"	"$(INTDIR)\multilayer_transpiration.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\multilayer_transpiration.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\multilayer_tsoil.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\multilayer_tsoil.obj"	"$(INTDIR)\multilayer_tsoil.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\multilayer_tsoil.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\ndep_init.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\ndep_init.obj"	"$(INTDIR)\ndep_init.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\ndep_init.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\otherGHGflux_estimation.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\otherGHGflux_estimation.obj"	"$(INTDIR)\otherGHGflux_estimation.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\otherGHGflux_estimation.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\output_init.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\output_init.obj"	"$(INTDIR)\output_init.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\output_init.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\output_map_init.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\output_map_init.obj"	"$(INTDIR)\output_map_init.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\output_map_init.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\phenology.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\phenology.obj"	"$(INTDIR)\phenology.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\phenology.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\photosynthesis.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\photosynthesis.obj"	"$(INTDIR)\photosynthesis.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\photosynthesis.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\planting.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\planting.obj"	"$(INTDIR)\planting.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\planting.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\planting_init.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\planting_init.obj"	"$(INTDIR)\planting_init.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\planting_init.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\ploughing.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\ploughing.obj"	"$(INTDIR)\ploughing.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\ploughing.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\ploughing_init.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\ploughing_init.obj"	"$(INTDIR)\ploughing_init.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\ploughing_init.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\pointbgc.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\pointbgc.obj"	"$(INTDIR)\pointbgc.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\pointbgc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\prcp_route.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\prcp_route.obj"	"$(INTDIR)\prcp_route.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\prcp_route.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\precision_control.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\precision_control.obj"	"$(INTDIR)\precision_control.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\precision_control.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\prephenology.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\prephenology.obj"	"$(INTDIR)\prephenology.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\prephenology.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\presim_state_init.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\presim_state_init.obj"	"$(INTDIR)\presim_state_init.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\presim_state_init.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\radtrans.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\radtrans.obj"	"$(INTDIR)\radtrans.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\radtrans.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\read_mgmarray.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\read_mgmarray.obj"	"$(INTDIR)\read_mgmarray.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\read_mgmarray.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\restart_init.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\restart_init.obj"	"$(INTDIR)\restart_init.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\restart_init.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\restart_io.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\restart_io.obj"	"$(INTDIR)\restart_io.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\restart_io.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\richards.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\richards.obj"	"$(INTDIR)\richards.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\richards.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\scc_init.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\scc_init.obj"	"$(INTDIR)\scc_init.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\scc_init.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\senescence.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\senescence.obj"	"$(INTDIR)\senescence.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\senescence.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\sitec_init.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\sitec_init.obj"	"$(INTDIR)\sitec_init.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\sitec_init.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\smooth.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\smooth.obj"	"$(INTDIR)\smooth.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\smooth.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\snowmelt.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\snowmelt.obj"	"$(INTDIR)\snowmelt.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\snowmelt.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\spinup_bgc.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\spinup_bgc.obj"	"$(INTDIR)\spinup_bgc.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\spinup_bgc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\spinup_daily_allocation.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\spinup_daily_allocation.obj"	"$(INTDIR)\spinup_daily_allocation.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\spinup_daily_allocation.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\state_init.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\state_init.obj"	"$(INTDIR)\state_init.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\state_init.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\state_update.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\state_update.obj"	"$(INTDIR)\state_update.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\state_update.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\summary.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\summary.obj"	"$(INTDIR)\summary.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\summary.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\thinning.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\thinning.obj"	"$(INTDIR)\thinning.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\thinning.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\thinning_init.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\thinning_init.obj"	"$(INTDIR)\thinning_init.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\thinning_init.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\time_init.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\time_init.obj"	"$(INTDIR)\time_init.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\time_init.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\tipping.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\tipping.obj"	"$(INTDIR)\tipping.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\tipping.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\transient_bgc.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\transient_bgc.obj"	"$(INTDIR)\transient_bgc.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\transient_bgc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\waterstress_days.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\waterstress_days.obj"	"$(INTDIR)\waterstress_days.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\waterstress_days.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\zero_srcsnk.c

!IF  "$(CFG)" == "bgc411 - Win32 Release"


"$(INTDIR)\zero_srcsnk.obj"	"$(INTDIR)\zero_srcsnk.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "bgc411 - Win32 DebugDojka"


"$(INTDIR)\zero_srcsnk.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

