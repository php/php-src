/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2004 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_0.txt.                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Contributed by ECL IP'S Software & Services                          |
   |                http://www.eclips-software.com                        |
   |                mailto://idev@eclips-software.com                     |
   | Author: David Hénot <henot@php.net>                                  |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifndef PHP_II_H
#define PHP_II_H

#if HAVE_II

extern zend_module_entry ingres_module_entry;
#define phpext_ingres_ptr &ingres_module_entry

#ifdef PHP_WIN32
#define PHP_II_API __declspec(dllexport)
#else
#define PHP_II_API
#endif

PHP_MINIT_FUNCTION(ii);
PHP_MSHUTDOWN_FUNCTION(ii);
PHP_RINIT_FUNCTION(ii);
PHP_RSHUTDOWN_FUNCTION(ii);
PHP_MINFO_FUNCTION(ii);

PHP_FUNCTION(ingres_connect);
PHP_FUNCTION(ingres_pconnect);
PHP_FUNCTION(ingres_close);
PHP_FUNCTION(ingres_query);
PHP_FUNCTION(ingres_num_rows);
PHP_FUNCTION(ingres_num_fields);
PHP_FUNCTION(ingres_field_name);
PHP_FUNCTION(ingres_field_type);
PHP_FUNCTION(ingres_field_nullable);
PHP_FUNCTION(ingres_field_length);
PHP_FUNCTION(ingres_field_precision);
PHP_FUNCTION(ingres_field_scale);
PHP_FUNCTION(ingres_fetch_array);
PHP_FUNCTION(ingres_fetch_row);
PHP_FUNCTION(ingres_fetch_object);
PHP_FUNCTION(ingres_rollback);
PHP_FUNCTION(ingres_commit);
PHP_FUNCTION(ingres_autocommit);

ZEND_BEGIN_MODULE_GLOBALS(ii)
	long allow_persistent;
	long max_persistent;
	long max_links;
	char *default_database;
	char *default_user;
	char *default_password;

	long num_persistent;
	long num_links;
	long default_link;
ZEND_END_MODULE_GLOBALS(ii)

#define II_ASSOC (1<<0)
#define II_NUM   (1<<1)
#define II_BOTH  (II_ASSOC|II_NUM)

#ifdef ZTS
#define IIG(v) TSRMG(ii_globals_id, zend_ii_globals *, v)
#else
#define IIG(v) (ii_globals.v)
#endif

#else

#define phpext_ingres_ptr NULL

#endif

#endif	/* PHP_II_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
