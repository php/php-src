/* 
   +----------------------------------------------------------------------+
   | PHP version 4.0													  |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group					  |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.01 of the PHP license,	  |
   | that is bundled with this package in the file LICENSE, and is		  |
   | available at through the world-wide-web at							  |
   | http://www.php.net/license/2_01.txt.								  |
   | If you did not receive a copy of the PHP license and are unable to	  |
   | obtain it through the world-wide-web, please send a note to		  |
   | license@php.net so we can mail you a copy immediately.				  |
   +----------------------------------------------------------------------+
   | Authors: Kristian Koehntopp (kris@koehntopp.de)					  |
   +----------------------------------------------------------------------+
 */


/* $Id$ */

#ifndef _PHP_RECODE_H
#define _PHP_RECODE_H

#ifdef COMPILE_DL_RECODE
#undef HAVE_LIBRECODE
#define HAVE_LIBRECODE 1
#endif

#ifdef PHP_WIN32
#define PHP_MYSQL_API __declspec(dllexport)
#else
#define PHP_MYSQL_API
#endif

#if HAVE_LIBRECODE

/* Checking for thread safety and issue warning if necessary. */
#ifdef ZTS
#warning Recode module has not been tested for thread-safety.
#endif

/* Recode 3.5 is broken in the sense that it requires the definition
 * of a symbol "program_name" in order to link.
 */
#define HAVE_BROKEN_RECODE

#include <stdbool.h>
#include <sys/types.h>
#include <stdio.h>
#include <recode.h>
#include <unistd.h>

extern zend_module_entry recode_module_entry;
#define phpext_recode_ptr &recode_module_entry

extern PHP_MINIT_FUNCTION(recode);
extern PHP_MSHUTDOWN_FUNCTION(recode);
extern PHP_MINFO_FUNCTION(recode);

extern PHP_FUNCTION(recode_string);
extern PHP_FUNCTION(recode_file);

typedef struct {
	RECODE_OUTER   outer;
} php_recode_globals;

#ifdef ZTS
# define ReSLS_D	php_recode_globals *recode_globals
# define ReSLS_DC	, ReSLS_D
# define ReSLS_C	recode_globals
# define ReSLS_CC , ReSLS_C
# define ReSG(v) (recode_globals->v)
# define ReSLS_FETCH()	php_recode_globals *recode_globals = ts_resource(recode_globals_id)
#else
# define ReSLS_D
# define ReSLS_DC
# define ReSLS_C
# define ReSLS_CC
# define ReSG(v) (recode_globals.v)
# define ReSLS_FETCH()
extern PHP_MYSQL_API php_recode_globals recode_globals;
#endif

#else
#define phpext_recode_ptr NULL
#endif

#endif /* _PHP_RECODE_H */
