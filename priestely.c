/* 
priestley.c
Priestley-Taylor calculation - combined equation for determining evaporation and transpiration. 
fluxes.  

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Biome-BGCMuSo v6.0.
Copyright 2019, D. Hidy [dori.hidy@gmail.com]
Hungarian Academy of Sciences, Hungary
See the website of Biome-BGCMuSo at http://nimbus.elte.hu/bbgc/ for documentation, model executable and example input files.
*-*-*-*-*-*-*-*-*-*-*-*-*-*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <malloc.h>
#include "ini.h"
#include "bgc_struct.h"
#include "bgc_func.h"
#include "bgc_constants.h"


int priestley(const metvar_struct* metv, const epvar_struct *epv, wflux_struct* wf)
{
 

    int errflag=0;

   
 EEQ = metv->swabs * (4.88 * 10-3 - 4.37 *10-3 * ALBEDO) * (TD + 29),
    return (errflag);
}
