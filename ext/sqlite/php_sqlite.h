/*
  +----------------------------------------------------------------------+
  | PHP Version 4                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2003 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 2.02 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available at through the world-wide-web at                           |
  | http://www.php.net/license/2_02.txt.                                 |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Wez Furlong <wez@thebrainroom.com>                          |
  |          Tal Peer <tal@php.net>                                      |
  +----------------------------------------------------------------------+

  $Id$ 
*/

#ifndef PHP_SQLITE_H
#define PHP_SQLITE_H

extern zend_module_entry sqlite_module_entry;
#define phpext_sqlite_ptr &sqlite_module_entry

#ifdef PHP_WIN32
#define PHP_SQLITE_API __declspec(dllexport)
#else
#define PHP_SQLITE_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(sqlite);
PHP_MSHUTDOWN_FUNCTION(sqlite);
PHP_RINIT_FUNCTION(sqlite);
PHP_RSHUTDOWN_FUNCTION(sqlite);
PHP_MINFO_FUNCTION(sqlite);

PHP_FUNCTION(sqlite_open);
PHP_FUNCTION(sqlite_popen);
PHP_FUNCTION(sqlite_close);
PHP_FUNCTION(sqlite_query);
PHP_FUNCTION(sqlite_unbuffered_query);
PHP_FUNCTION(sqlite_fetch_array);

PHP_FUNCTION(sqlite_num_rows);
PHP_FUNCTION(sqlite_num_fields);
PHP_FUNCTION(sqlite_field_name);
PHP_FUNCTION(sqlite_seek);

PHP_FUNCTION(sqlite_libversion);
PHP_FUNCTION(sqlite_libencoding);

PHP_FUNCTION(sqlite_changes);
PHP_FUNCTION(sqlite_last_insert_rowid);

PHP_FUNCTION(sqlite_escape_string);

PHP_FUNCTION(sqlite_busy_timeout);

PHP_FUNCTION(sqlite_last_error);
PHP_FUNCTION(sqlite_error_string);

PHP_FUNCTION(sqlite_create_aggregate);
PHP_FUNCTION(sqlite_create_function);

#ifdef ZTS
#define SQLITE_G(v) TSRMG(sqlite_globals_id, zend_sqlite_globals *, v)
#else
#define SQLITE_G(v) (sqlite_globals.v)
#endif

#endif


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
