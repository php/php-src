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

#if HAVE_UODBC

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

PHP_FUNCTION(odbc_error);
PHP_FUNCTION(odbc_errormsg);
PHP_FUNCTION(odbc_setoption);
PHP_FUNCTION(odbc_autocommit);
PHP_FUNCTION(odbc_close);
PHP_FUNCTION(odbc_close_all);
PHP_FUNCTION(odbc_commit);
PHP_FUNCTION(odbc_connect);
PHP_FUNCTION(odbc_pconnect);
PHP_FUNCTION(odbc_cursor);
#ifdef HAVE_SQLDATASOURCES
PHP_FUNCTION(odbc_data_source);
#endif
PHP_FUNCTION(odbc_do);
PHP_FUNCTION(odbc_exec);
PHP_FUNCTION(odbc_execute);
#ifdef PHP_ODBC_HAVE_FETCH_HASH
PHP_FUNCTION(odbc_fetch_array);
PHP_FUNCTION(odbc_fetch_object);
#endif
PHP_FUNCTION(odbc_fetch_into);
PHP_FUNCTION(odbc_fetch_row);
PHP_FUNCTION(odbc_field_len);
PHP_FUNCTION(odbc_field_scale);
PHP_FUNCTION(odbc_field_name);
PHP_FUNCTION(odbc_field_type);
PHP_FUNCTION(odbc_field_num);
PHP_FUNCTION(odbc_free_result);
#if !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30)
PHP_FUNCTION(odbc_next_result);
#endif
PHP_FUNCTION(odbc_num_fields);
PHP_FUNCTION(odbc_num_rows);
PHP_FUNCTION(odbc_prepare);
PHP_FUNCTION(odbc_result);
PHP_FUNCTION(odbc_result_all);
PHP_FUNCTION(odbc_rollback);
PHP_FUNCTION(odbc_binmode);
PHP_FUNCTION(odbc_longreadlen);
PHP_FUNCTION(odbc_tables);
PHP_FUNCTION(odbc_columns);
#if !defined(HAVE_DBMAKER) && !defined(HAVE_SOLID) && !defined(HAVE_SOLID_35)    /* not supported now */
PHP_FUNCTION(odbc_columnprivileges);
PHP_FUNCTION(odbc_tableprivileges);
#endif
#if !defined(HAVE_SOLID) || !defined(HAVE_SOLID_35)    /* not supported */
PHP_FUNCTION(odbc_foreignkeys);
PHP_FUNCTION(odbc_procedures);
PHP_FUNCTION(odbc_procedurecolumns);
#endif
PHP_FUNCTION(odbc_gettypeinfo);
PHP_FUNCTION(odbc_primarykeys);
PHP_FUNCTION(odbc_specialcolumns);
PHP_FUNCTION(odbc_statistics);

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
