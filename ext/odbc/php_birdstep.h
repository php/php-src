/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2012 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Nikolay P. Romanyuk <mag@redcom.ru>                         |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_BIRDSTEP_H
#define PHP_BIRDSTEP_H

#if defined(HAVE_BIRDSTEP) && !HAVE_UODBC
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
} birdstep_module;

extern zend_module_entry birdstep_module_entry;
#define birdstep_module_ptr &birdstep_module_entry

/* birdstep.c functions */
PHP_MINIT_FUNCTION(birdstep);
PHP_RINIT_FUNCTION(birdstep);
PHP_MINFO_FUNCTION(birdstep);
PHP_MSHUTDOWN_FUNCTION(birdstep);

PHP_FUNCTION(birdstep_connect);
PHP_FUNCTION(birdstep_close);
PHP_FUNCTION(birdstep_exec);
PHP_FUNCTION(birdstep_fetch);
PHP_FUNCTION(birdstep_result);
PHP_FUNCTION(birdstep_freeresult);
PHP_FUNCTION(birdstep_autocommit);
PHP_FUNCTION(birdstep_off_autocommit);
PHP_FUNCTION(birdstep_commit);
PHP_FUNCTION(birdstep_rollback);
PHP_FUNCTION(birdstep_fieldnum);
PHP_FUNCTION(birdstep_fieldname);

extern birdstep_module php_birdstep_module;

#else

#define birdstep_module_ptr NULL

#endif /* HAVE_BIRDSTEP */
#endif /* PHP_BIRDSTEP_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
