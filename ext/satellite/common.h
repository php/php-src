/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: David Eriksson <david@2good.com>                            |
   +----------------------------------------------------------------------+
 */

/*
 * $Id$
 * vim: syntax=c tabstop=2 shiftwidth=2
 */

/* -----------------------------------------------------------------------
 * 
 * Some common stuff, now only memory allocation macros
 *
 * -----------------------------------------------------------------------
 */
#ifndef __common_h__
#define __common_h__

#include <zend.h>

#define DEBUG_MEMORY	0

#if DEBUG_MEMORY

void * satellite_debug_calloc(int count, int length);
void satellite_debug_free(void * pBuffer);
char * satellite_debug_strdup(const char * pString);

/* default memory allocation */
#define satellite_new_n(type,n)	(type*)satellite_debug_calloc((n), sizeof(type))
#define satellite_delete(p)			satellite_debug_free(p)
#define satellite_strdup(s)			satellite_debug_strdup(s)

#else

/* default memory allocation */
#define satellite_new_n(type,n)	(type*)ecalloc((n), sizeof(type))
#define satellite_delete(p)			if(p)efree(p)
#define satellite_strdup(s)			estrdup(s)

#endif

#define satellite_new_1(type)		satellite_new_n(type, 1)
#define satellite_new(type)			satellite_new_1(type)


/* macros for old prefix "orbit_" */

#define orbit_new_n(type,n)		satellite_new_n(type,n)
#define orbit_delete(p)				satellite_delete(p)
#define orbit_strdup(s)				satellite_strdup(s)
#define orbit_new_1(type)			satellite_new_1(type)
#define orbit_new(type)				satellite_new(type)

#endif /* __common_h__ */

