/* Copyright Abandoned 1996 TCX DataKonsult AB & Monty Program KB & Detron HB 
This file is public domain and comes with NO WARRANTY of any kind */

#ifndef _mysys_err_h
#define _mysys_err_h

#include <errno.h>

#ifdef	__cplusplus
extern "C" {
#endif

#define GLOB		0	/* Error maps */
#define GLOBERRS	27	/* Max number of error messages in map's */
#define EE(X)	globerrs[ X ]	/* Defines to add error to right map */

extern const char * NEAR globerrs[];	/* my_error_messages is here */

/* Error message numbers in global map */
#define EE_FILENOTFOUND		0
#define EE_CANTCREATEFILE	1
#define EE_READ			2
#define EE_WRITE		3
#define EE_BADCLOSE		4
#define EE_OUTOFMEMORY		5
#define EE_DELETE		6
#define EE_LINK			7
#define EE_EOFERR		9
#define EE_CANTLOCK		10
#define EE_CANTUNLOCK		11
#define EE_DIR			12
#define EE_STAT			13
#define EE_CANT_CHSIZE		14
#define EE_CANT_OPEN_STREAM	15
#define EE_GETWD		16
#define EE_SETWD		17
#define EE_LINK_WARNING		18
#define EE_OPEN_WARNING		19
#define EE_DISK_FULL		20
#define EE_CANT_MKDIR		21
#define EE_UNKNOWN_CHARSET	22
#define EE_OUT_OF_FILERESOURCES	23
#define EE_CANT_READLINK	24
#define EE_CANT_SYMLINK		25
#define EE_REALPATH		26

#ifdef	__cplusplus
}
#endif
#endif
