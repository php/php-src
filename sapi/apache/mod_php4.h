/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Rasmus Lerdorf <rasmus@php.net>                             |
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
} php_apache_info_struct;

extern zend_module_entry apache_module_entry;

#ifdef ZTS
extern int php_apache_info_id;
#define APLS_D php_apache_info_struct *apache_globals
#define AP(v) (apache_globals->v)
#define APLS_FETCH() APLS_D = ts_resource(php_apache_info_id)
#else
extern php_apache_info_struct php_apache_info;
#define APLS_D
#define AP(v) (php_apache_info.v)
#define APLS_FETCH()
#endif


#ifdef WIN32
#define S_IXUSR _S_IEXEC
#endif

#endif							/* MOD_PHP4_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
