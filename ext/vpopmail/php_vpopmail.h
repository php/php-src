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
   | Author: David Croft <david@infotrek.co.uk>                           |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_VPOPMAIL_H
#define PHP_VPOPMAIL_H

#if HAVE_VPOPMAIL

extern zend_module_entry vpopmail_module_entry;
#define phpext_vpopmail_ptr &vpopmail_module_entry

#ifdef PHP_WIN32
#define PHP_VPOPMAIL_API __declspec(dllexport)
#else
#define PHP_VPOPMAIL_API
#endif

PHP_MINIT_FUNCTION(vpopmail);
PHP_MSHUTDOWN_FUNCTION(vpopmail);
PHP_MINFO_FUNCTION(vpopmail);

PHP_FUNCTION(vpopmail_adddomain);
PHP_FUNCTION(vpopmail_deldomain);
PHP_FUNCTION(vpopmail_adduser);
PHP_FUNCTION(vpopmail_deluser);
PHP_FUNCTION(vpopmail_passwd);
PHP_FUNCTION(vpopmail_setuserquota);
PHP_FUNCTION(vpopmail_auth_user);

typedef struct {
	int le_vpopmail;
	int initialised;
	char *defaulthost;
	int defaultport;
	int defaulttimeout;
	char *proxyaddress;
	int proxyport;
	char *proxylogon;
	char *proxypassword;
} php_vpopmail_globals;

#ifdef ZTS
#define VPOPMAILG(v) (vpopmail_globals->v)
#define VPOPMAILLS_FETCH() php_vpopmail_globals *vpopmail_globals = ts_resource(gd_vpopmail_id)
#else
#define VPOPMAILG(v) (vpopmail_globals.v)
#define VPOPMAILLS_FETCH()
#endif

#else

#define phpext_vpopmail_ptr NULL

#endif

#endif	/* PHP_VPOPMAIL_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
