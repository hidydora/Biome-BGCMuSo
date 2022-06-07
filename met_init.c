/* 
met_init.c
open met file for input, scan through header lines

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

int met_init(file init, point_struct* point)
{
	int errflag=0;
	int nhead = 0;
	int i;
	char key1[] = "MET_INPUT";
	char keyword[STRINGSIZE];
	char junk_head[1024];

	/********************************************************************
	**                                                                 **
	** Begin reading initialization file block starting with keyword:  **
	** MET_INPUT                                                       ** 
	**                                                                 **
	********************************************************************/
	
	/* scan for the input metfile keyword, exit if not next */
	if (!errflag && scan_value(init, keyword, 's'))
	{
		printf("ERROR reading keyword for met file: met_init()\n");
		errflag=202;
	}
	if (!errflag && strcmp(keyword, key1))
	{
		printf("Expecting keyword --> %s in file %s\n",key1,init.name);
		errflag=202;
	}
	
	/* get the met data filename and open file for ascii read */
    if (!errflag && scan_open(init,&point->metf,'i',1)) 
	{
		printf("ERROR opening met data file: met_init()\n");
		errflag=20201;
	}
	
	/* get number of metfile header lines */
	if (!errflag && scan_value(init, &nhead, 'i'))
	{
		printf("ERROR reading number of met file header lines: met_init()\n");
		errflag=20202;
	}
	
	/* read header lines from input met data file and discard */
	for (i=0 ; !errflag && i<nhead ; i++)
	{
		if (scan_value(point->metf, junk_head, 's'))
		{
			printf("ERROR reading met file header line #%d\n",i+1);
			errflag=20202;
		}
	}

	/* number of simdays in last simyear (truncated year: < 365) */
	if (!errflag && scan_value(init, &point->nday_lastsimyear, 'i'))
	{
		printf("ERROR reading number of simdays in last simyear: met_init()\n");
		errflag=20203;
	}

	return (errflag);
}

