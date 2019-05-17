/*****************************************************************************
 *                                                                           *
 * sys/param.c                                                               *
 *                                                                           *
 * Freely redistributable and modifiable.  Use at your own risk.             *
 *                                                                           *
 * Copyright 1994 The Downhill Project                                       *
 *                                                                           *
 *****************************************************************************/
#ifndef PHP_WIN32_PARAM_H
#define PHP_WIN32_PARAM_H

#ifndef MAXPATHLEN
#include "win32/ioutil.h"
#define MAXPATHLEN PHP_WIN32_IOUTIL_MAXPATHLEN
#endif
#define MAXHOSTNAMELEN 64
#define howmany(x,y)   (((x)+((y)-1))/(y))
#define roundup(x,y)   ((((x)+((y)-1))/(y))*(y))

#endif
