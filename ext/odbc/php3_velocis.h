/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997,1998 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of one of the following licenses:                 |
   |                                                                      |
   |  A) the GNU General Public License as published by the Free Software |
   |     Foundation; either version 2 of the License, or (at your option) |
   |     any later version.                                               |
   |                                                                      |
   |  B) the PHP License as published by the PHP Development Team and     |
   |     included in the distribution in the file: LICENSE                |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of both licenses referred to here.   |
   | If you did not, or have any questions about PHP licensing, please    |
   | contact core@php.net.                                                |
   +----------------------------------------------------------------------+
   | Authors: Nikolay P. Romanyuk <mag@redcom.ru>                         |
   |                                                                      |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifndef _PHP3_VELOCIS_H
#define _PHP3_VELOCIS_H

#if COMPILE_DL
#undef HAVE_VELOCIS
#define HAVE_VELOCIS 1
#endif

#if HAVE_VELOCIS && !HAVE_UODBC
#define UNIX
#include <sql.h>
#include <sqlext.h>

typedef struct VConn {
	HDBC    hdbc;
	long    index;
} VConn;

typedef struct {
	char name[32];
	char *value;
	long vallen;
	SDWORD valtype;
} VResVal;

typedef struct Vresult {
	HSTMT   hstmt;
	VConn   *conn; 
	long    index;
	VResVal *values;
	long    numcols;
	int     fetched;
} Vresult;

typedef struct {
	long num_links;
	long max_links;
	int le_link,le_result;
} velocis_module;

extern php3_module_entry velocis_module_entry;
#define velocis_module_ptr &velocis_module_entry

/* velocis.c functions */
extern int php3_minit_velocis(INIT_FUNC_ARGS);
extern int php3_rinit_velocis(INIT_FUNC_ARGS);
extern void php3_info_velocis(ZEND_MODULE_INFO_FUNC_ARGS);
extern int php3_shutdown_velocis(SHUTDOWN_FUNC_ARGS);
extern void php3_velocis_connect(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_velocis_close(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_velocis_exec(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_velocis_fetch(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_velocis_result(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_velocis_freeresult(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_velocis_autocommit(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_velocis_off_autocommit(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_velocis_commit(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_velocis_rollback(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_velocis_fieldnum(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_velocis_fieldname(INTERNAL_FUNCTION_PARAMETERS);

extern velocis_module php3_velocis_module;

#else

#define velocis_module_ptr NULL

#endif /* HAVE_VELOCIS */
#endif /* _PHP3_VELOCIS_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
