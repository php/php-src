
/*****************************************************************************
 *                                                                           *
 * sys/param.c                                                               *
 *                                                                           *
 * Freely redistributable and modifiable.  Use at your own risk.             *
 *                                                                           *
 * Copyright 1994 The Downhill Project                                       *
 *                                                                           *
 *****************************************************************************/
#ifndef MAXPATHLEN
/*#define MAXPATHLEN     _MAX_PATH*/
#define MAXPATHLEN     260  /* _MAX_PATH defined in STDLIB.H for Windows; picked up the value from there */
#define _MAX_PATH     260  /* Defined since it is used in os.c while compiling apache */
#endif
#define MAXHOSTNAMELEN 64
#define howmany(x,y)   (((x)+((y)-1))/(y))
#define roundup(x,y)   ((((x)+((y)-1))/(y))*(y))

#define _MAX_VOLUME 31  /* Defined since it is used in os.c while compiling apache. The value picked up from LibC */
