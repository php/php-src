/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2013 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Rasmus Lerdorf <rasmus@php.net>                              |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#ifndef MOD_PHP5_H
#define MOD_PHP5_H

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
	long current_hook;
	zend_bool in_request;
	zend_bool apache_config_loaded;
	zend_bool headers_sent;
} php_apache_info_struct;

typedef struct _php_handler {
    long type;
    long stage;
    char *name;
} php_handler;

#define AP_HANDLER_TYPE_FILE 0
#define AP_HANDLER_TYPE_METHOD 1

extern zend_module_entry apache_module_entry;

#ifdef ZTS
extern int php_apache_info_id;
#define AP(v) TSRMG(php_apache_info_id, php_apache_info_struct *, v)
#else
extern php_apache_info_struct php_apache_info;
#define AP(v) (php_apache_info.v)
#endif

/* defines for the various stages of the apache request */
#define AP_WAITING_FOR_REQUEST 0
#define AP_POST_READ 1
#define AP_URI_TRANS 2
#define AP_HEADER_PARSE 3
#define AP_ACCESS_CONTROL 4
#define AP_AUTHENTICATION 5
#define AP_AUTHORIZATION 6
#define AP_TYPE_CHECKING 7
#define AP_FIXUP 8
#define AP_RESPONSE 9
#define AP_LOGGING 10
#define AP_CLEANUP 11


/* fix for gcc4 visibility patch */
#ifndef PHP_WIN32
# undef MODULE_VAR_EXPORT
# define MODULE_VAR_EXPORT PHPAPI
#endif

#endif							/* MOD_PHP5_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
