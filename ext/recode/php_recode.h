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
   | If you did not receive a copy of the PHP license and are unable to	  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Kristian Koehntopp <kris@koehntopp.de>                       |
   +----------------------------------------------------------------------+
 */


/* $Id$ */

#ifndef PHP_RECODE_H
#define PHP_RECODE_H

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

#ifdef HAVE_STDBOOL_H
# include <stdbool.h>
#else
  typedef enum {false = 0, true = 1} bool;
#endif

#include <sys/types.h>
#include <stdio.h>
#include <recode.h>
#include <unistd.h>

extern zend_module_entry recode_module_entry;
#define phpext_recode_ptr &recode_module_entry

PHP_MINIT_FUNCTION(recode);
PHP_MSHUTDOWN_FUNCTION(recode);
PHP_MINFO_FUNCTION(recode);

PHP_FUNCTION(recode_string);
PHP_FUNCTION(recode_file);

#ifdef ZTS
#include "TSRM.h"
#endif

ZEND_BEGIN_MODULE_GLOBALS(recode)
	RECODE_OUTER   outer;
ZEND_END_MODULE_GLOBALS(recode)

#ifdef ZTS
#define ReSG(v) TSRMG(recode_globals_id, zend_recode_globals *, v)
#else
#define ReSG(v) (recode_globals.v)
#endif

#endif
	
#endif /* PHP_RECODE_H */
