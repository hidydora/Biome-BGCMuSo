/* 
restart_init.c
Initialize the simulation restart parameters

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
Biome-BGCMuSo v6.0.
Copyright 2000, Peter E. Thornton
Numerical Terradynamic Simulation Group (NTSG)
School of Forestry, University of Montana
Missoula, MT 59812
*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "ini.h"
#include "bgc_struct.h"
#include "pointbgc_struct.h"
#include "pointbgc_func.h"

int restart_init(file init, restart_ctrl_struct* restart)
{
	int errflag=0;
	char key1[] = "RESTART";
	char keyword[STRINGSIZE];
	char junk[STRINGSIZE];

	/********************************************************************
	**                                                                 **
	** Begin reading initialization file block starting with keyword:  **
	** RESTART                                                         ** 
	**                                                                 **
	********************************************************************/
	
	/* scan for the restart block keyword, exit if not next */
	if (!errflag && scan_value(init, keyword, 's'))
	{
		printf("ERROR reading keyword for restart data\n");
		errflag=203;
	}
	if (!errflag && strcmp(keyword, key1))
	{
		printf("Expecting keyword --> %s in file %s\n",key1,init.name);
		errflag=203;
	}
	
	/* check for input restart file */
	if (!errflag && scan_value(init, &restart->read_restart, 'i'))
	{
		printf("ERROR reading input restart flag\n");
		errflag=20301;
	}
	/* check for output restart file */
	if (!errflag && scan_value(init, &restart->write_restart, 'i'))
	{
		printf("ERROR reading output restart flag\n");
		errflag=20302;
	}
	
	/* if using an input restart file, open it, otherwise
	discard the next line of the ini file */
	if (!errflag && restart->read_restart)
	{
    	if (scan_open(init,&(restart->in_restart),'r',1)) 
		{
			printf("ERROR opening input restart file\n");
			errflag=20303;
		}
	}
	else
	{
		if (scan_value(init, junk, 's'))
		{
			printf("ERROR scanning input restart filename\n");
			errflag=20304;
		}
	}
	/* if using an output restart file, open it, otherwise
	discard the next line of the ini file */
	if (!errflag && restart->write_restart)
	{
    	if (scan_open(init,&(restart->out_restart),'w',1)) 
		{
			printf("ERROR opening output restart file\n");
			errflag=20305;
		}
	}
	else
	{
		if (scan_value(init, junk, 's'))
		{
			printf("ERROR scanning output restart filename\n");
			errflag=20306;
		}
	}
	
	return (errflag);
}
