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
   | Author: Rasmus Lerdorf <rasmus@php.net>                              |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#ifndef MOD_PHP4_H
#define MOD_PHP4_H

#if !defined(WIN32) && !defined(WINNT)
#ifndef MODULE_VAR_EXPORT
#define MODULE_VAR_EXPORT
#endif
#endif

typedef struct {
	long engine;
	long last_modified;
	long xbithack;
	long terminate_child;
	long setup_env;
	zend_bool in_request;
	zend_bool apache_config_loaded;
	char *uri_handler;
	char *auth_handler;
	char *access_handler;
	char *type_handler;
	char *fixup_handler;
	char *logger_handler;
} php_apache_info_struct;

extern zend_module_entry apache_module_entry;

#ifdef ZTS
extern int php_apache_info_id;
#define AP(v) TSRMG(php_apache_info_id, php_apache_info_struct *, v)
#else
extern php_apache_info_struct php_apache_info;
#define AP(v) (php_apache_info.v)
#endif

#endif							/* MOD_PHP4_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
