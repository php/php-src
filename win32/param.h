
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
#define MAXPATHLEN     _MAX_PATH
#endif
#define MAXHOSTNAMELEN 64
#define howmany(x,y)   (((x)+((y)-1))/(y))
#define roundup(x,y)   ((((x)+((y)-1))/(y))*(y))
