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
   | Authors: David Croft <david@infotrek.co.uk>                          |
   |          Boian Bonev <boian@bonev.com>                               |
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
PHP_RINIT_FUNCTION(vpopmail);
PHP_RSHUTDOWN_FUNCTION(vpopmail);
PHP_MINFO_FUNCTION(vpopmail);

/* domain management - lib call */
PHP_FUNCTION(vpopmail_add_domain);
PHP_FUNCTION(vpopmail_del_domain);
PHP_FUNCTION(vpopmail_add_alias_domain);
/* domain management - exec */
PHP_FUNCTION(vpopmail_add_domain_ex);
PHP_FUNCTION(vpopmail_del_domain_ex);
PHP_FUNCTION(vpopmail_add_alias_domain_ex);
/* user management */
PHP_FUNCTION(vpopmail_add_user);
PHP_FUNCTION(vpopmail_del_user);
PHP_FUNCTION(vpopmail_passwd);
PHP_FUNCTION(vpopmail_set_user_quota);
PHP_FUNCTION(vpopmail_auth_user);
/* alias management */
#if HAVE_VPOPMAIL_VALIAS
PHP_FUNCTION(vpopmail_alias_add);
PHP_FUNCTION(vpopmail_alias_del);
PHP_FUNCTION(vpopmail_alias_del_domain);
PHP_FUNCTION(vpopmail_alias_get);
PHP_FUNCTION(vpopmail_alias_get_all);
#endif
/* error handling */
PHP_FUNCTION(vpopmail_error);

/* defines for vpopmail command line tool names */
#define VPOPMAIL_ADDD "/vadddomain "
#define VPOPMAIL_DELD "/vdeldomain "
#define VPOPMAIL_ADAD "/vaddaliasdomain "

ZEND_BEGIN_MODULE_GLOBALS(vpopmail)
	int vpopmail_open;
	int vpopmail_errno;
ZEND_END_MODULE_GLOBALS(vpopmail)

#ifdef ZTS
#define VPOPMAILG(v) TSRMG(vpopmail_globals_id, zend_vpopmail_globals *, v)
#else
#define VPOPMAILG(v) (vpopmail_globals.v)
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
