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
   | Author: Nikolay P. Romanyuk <mag@redcom.ru>                          |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_VELOCIS_H
#define PHP_VELOCIS_H

#if defined(HAVE_VELOCIS) && !HAVE_UODBC
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

extern zend_module_entry velocis_module_entry;
#define velocis_module_ptr &velocis_module_entry

/* velocis.c functions */
PHP_MINIT_FUNCTION(velocis);
PHP_RINIT_FUNCTION(velocis);
PHP_MINFO_FUNCTION(velocis);
PHP_MSHUTDOWN_FUNCTION(velocis);

PHP_FUNCTION(velocis_connect);
PHP_FUNCTION(velocis_close);
PHP_FUNCTION(velocis_exec);
PHP_FUNCTION(velocis_fetch);
PHP_FUNCTION(velocis_result);
PHP_FUNCTION(velocis_freeresult);
PHP_FUNCTION(velocis_autocommit);
PHP_FUNCTION(velocis_off_autocommit);
PHP_FUNCTION(velocis_commit);
PHP_FUNCTION(velocis_rollback);
PHP_FUNCTION(velocis_fieldnum);
PHP_FUNCTION(velocis_fieldname);

extern velocis_module php_velocis_module;

#else

#define velocis_module_ptr NULL

#endif /* HAVE_VELOCIS */
#endif /* PHP_VELOCIS_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
