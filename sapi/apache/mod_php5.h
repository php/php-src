/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2014 The PHP Group                                |
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
	zend_bool in_request;
} php_apache_info_struct;

extern zend_module_entry apache_module_entry;

#ifdef ZTS
extern int php_apache_info_id;
#define AP(v) TSRMG(php_apache_info_id, php_apache_info_struct *, v)
#else
extern php_apache_info_struct php_apache_info;
#define AP(v) (php_apache_info.v)
#endif

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
