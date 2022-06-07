/* 
determine vegetation period (0: before, 1: in, -1: after) 
*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Biome-BGC version 4.1.1
Copyright Hidy 2013.
*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <malloc.h>
#include "bgc_struct.h"
#include "bgc_func.h"
#include "bgc_constants.h"

int vegetation_period_determ(control_struct* ctrl, phenology_struct* phen)
{
	int ok=1;


	/* determine vegetation period */
	ctrl->vegperday_pre = ctrl->vegperday;
	ctrl->vegperday = phen->remdays_curgrowth;

	/* start of vegetation period */
	if (ctrl->vegperday_pre == 0)
	{
		if (ctrl->vegperday == 0)
		{
				ctrl->vegper_flag = 0; 
		}
		else 
		{
			ctrl->vegper_flag = 1; 
		}
	}
	else
	{
		if (ctrl->vegperday == 0)
		{
			/* end of vegetation period */
			ctrl->vegper_flag = -1;
		}
		else 
		{
			ctrl->vegper_flag = 0; 
		}
	}

	return(!ok);
}
