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
   | Authors: Jouni Ahto <jah@cultnet.fi>                                 |
   |                                                                      |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifndef _PHP3_IBASE_H
#define _PHP3_IBASE_H

#if COMPILE_DL
#undef HAVE_IBASE
#define HAVE_IBASE 1
#endif

#if HAVE_IBASE
#include <ibase.h>

extern zend_module_entry ibase_module_entry;
#define php3_ibase_module_ptr &ibase_module_entry

extern PHP_MINIT_FUNCTION(ibase);
extern PHP_RINIT_FUNCTION(ibase);
extern PHP_MSHUTDOWN_FUNCTION(ibase);
PHP_MINFO_FUNCTION(ibase);

PHP_FUNCTION(ibase_connect);
PHP_FUNCTION(ibase_pconnect);
PHP_FUNCTION(ibase_close);
PHP_FUNCTION(ibase_query);
PHP_FUNCTION(ibase_fetch_row);
PHP_FUNCTION(ibase_free_result);
PHP_FUNCTION(ibase_prepare);
PHP_FUNCTION(ibase_bind);
PHP_FUNCTION(ibase_execute);
PHP_FUNCTION(ibase_free_query);
PHP_FUNCTION(ibase_timefmt);

typedef struct {
	long default_link;
	long num_links, num_persistent;
	long max_links, max_persistent;
	long allow_persistent;
	int le_link, le_plink, le_result, le_query;
	char *default_user, *default_password;
	long manualtransactions;
	char *timeformat;
} ibase_module;

typedef struct _php3_ibase_result {
	isc_stmt_handle result;
	isc_tr_handle trans;
	XSQLDA *sqlda;
	int commitok;
} ibase_result_handle;

typedef struct _php3_ibase_query {
	isc_stmt_handle query;
	isc_tr_handle trans;
	XSQLDA *sqlda;
	int alloced;
} ibase_query_handle;

typedef struct _php3_ibase_varchar {
    short var_len;
    char var_str[1];
} IBASE_VCHAR;

extern ibase_module php3_ibase_module;

#else

#define php3_ibase_module_ptr NULL

#endif /* HAVE_IBASE */

#define phpext_interbase_ptr php3_ibase_module_ptr

#endif /* _PHP3_IBASE_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
