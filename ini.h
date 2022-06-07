/*
ini.h
header file for file I/O handling and initialization file reading
for use with BBGC MuSo v3.0.8 library

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
BBGC MuSo v3.0.8
Copyright 2000, Peter E. Thornton
Numerical Terradynamics Simulation Group
*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
*/

/* structure definition for filename handling */
typedef struct
{
	char name[128];
	FILE *ptr;
} file;

/* function prototypes */
int file_open (file *target, char mode);
int scan_value (file ini, void *var, char mode);
int scan_array (file ini, void *var, char mode, int nl);
int scan_open (file ini,file *target,char mode);
