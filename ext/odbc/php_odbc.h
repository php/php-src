/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Stig Sæther Bakken <ssb@php.net>                            |
   |          Andreas Karajannis <Andreas.Karajannis@gmd.de>              |
   |	      Kevin N. Shallow <kshallow@tampabay.rr.com>                 |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_ODBC_H
#define PHP_ODBC_H

#ifdef HAVE_UODBC

#ifdef ZTS
#include "TSRM.h"
#endif

extern zend_module_entry odbc_module_entry;
#define odbc_module_ptr &odbc_module_entry

#include "php_version.h"
#define PHP_ODBC_VERSION PHP_VERSION

#if defined(HAVE_DBMAKER) || defined(PHP_WIN32) || defined(HAVE_IBMDB2) || defined(HAVE_UNIXODBC) || defined(HAVE_IODBC)
# define PHP_ODBC_HAVE_FETCH_HASH 1
#endif

/* user functions */
PHP_MINIT_FUNCTION(odbc);
PHP_MSHUTDOWN_FUNCTION(odbc);
PHP_RINIT_FUNCTION(odbc);
PHP_RSHUTDOWN_FUNCTION(odbc);
PHP_MINFO_FUNCTION(odbc);

#ifdef PHP_WIN32
# define PHP_ODBC_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
# define PHP_ODBC_API __attribute__ ((visibility("default")))
#else
# define PHP_ODBC_API
#endif

#else

#define odbc_module_ptr NULL

#endif /* HAVE_UODBC */

#define phpext_odbc_ptr odbc_module_ptr

#endif /* PHP_ODBC_H */
