/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000, 2001 The PHP Group             |
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

/* TODO: move to config.m4 when support for old versions is ready or just
 * don't support rather old vpopmail. current version must bail out if
 * incompat option is specified and work for minimal params
 */

#undef VPOPMAIL_IS_REALLY_OLD

#include <errno.h>
#include <signal.h>
#include "php.h"
#include "php_ini.h"
#include "php_vpopmail.h"

#include "vpopmail.h"

#include "ext/standard/php_string.h"

ZEND_DECLARE_MODULE_GLOBALS(vpopmail)

/* keep this as the last include in order to destroy VERSION/PACKAGE only for the rest of the code */

#undef VERSION
#undef PACKAGE
#include "vpopmail_config.h"
#undef PACKAGE
#include "vpopmail.h"

/* Function table */

function_entry vpopmail_functions[] = {
	/* domain management */
	PHP_FE(vpopmail_adddomain, NULL)
	PHP_FE(vpopmail_deldomain, NULL)
	/* user management */
	PHP_FE(vpopmail_adduser, NULL)
	PHP_FE(vpopmail_deluser, NULL)
	PHP_FE(vpopmail_passwd, NULL)
	PHP_FE(vpopmail_setuserquota, NULL)
	PHP_FE(vpopmail_auth_user, NULL)
	{NULL, NULL, NULL}
};

zend_module_entry vpopmail_module_entry = {
	"vpopmail",
	vpopmail_functions,
	PHP_MINIT(vpopmail),
	PHP_MSHUTDOWN(vpopmail),
	PHP_RINIT(vpopmail),
	PHP_RSHUTDOWN(vpopmail),
	PHP_MINFO(vpopmail),
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_VPOPMAIL
ZEND_GET_MODULE(vpopmail)
#endif


PHP_INI_BEGIN()
	/*	STD_PHP_INI_ENTRY("vpopmail.directory",		"",				PHP_INI_ALL, OnUpdateString,	directory,			php_vpopmail_globals,	vpopmail_globals) */
PHP_INI_END()


PHP_MINIT_FUNCTION(vpopmail)
{
	REGISTER_INI_ENTRIES();
	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(vpopmail)
{
	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}

PHP_RINIT_FUNCTION(vpopmail)
{
	VPOPMAILLS_FETCH();

	VPOPMAILG(vpopmail_open) = 0;

	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(vpopmail)
{
	VPOPMAILLS_FETCH();

	if (VPOPMAILG(vpopmail_open) != 0) {
		vclose();
	}

	return SUCCESS;
}

PHP_MINFO_FUNCTION(vpopmail)
{
	char ids[64];

	sprintf(ids, "%d/%d", VPOPMAILUID, VPOPMAILGID);

	php_info_print_table_start();
	php_info_print_table_header(2, "vpopmail support", "enabled");
	php_info_print_table_row(2, "vpopmail version", VERSION);
	php_info_print_table_row(2, "vpopmail uid/gid", ids);
	php_info_print_table_row(2, "vpopmail dir", VPOPMAILDIR);
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}



/* {{{ proto void vpopmail_adddomain(string domain, string dir, int uid, int gid)
   Add a new virtual domain */
PHP_FUNCTION(vpopmail_adddomain)
{
	zval **domain;
	zval **dir;
	zval **uid;
	zval **gid;
	int retval;

	if (ZEND_NUM_ARGS() != 4
			|| zend_get_parameters_ex(4, &domain, &dir, &uid, &gid) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_string_ex(domain);
	convert_to_string_ex(dir);
	convert_to_long_ex(uid);
	convert_to_long_ex(gid);

	VPOPMAILLS_FETCH();
	VPOPMAILG(vpopmail_open) = 1;

	retval = vadddomain(Z_STRVAL_PP(domain),
#ifdef VPOPMAIL_IS_REALLY_OLD
						0
#else
						Z_STRVAL_PP(dir),
						Z_LVAL_PP(uid),
						Z_LVAL_PP(gid)
#endif
						);

	if (retval == VA_SUCCESS) {
		RETURN_TRUE;
	}
	else {
        php_error(E_WARNING, "vpopmail error: %s", verror(retval));
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto void vpopmail_deldomain(string domain)
   Delete a virtual domain */
PHP_FUNCTION(vpopmail_deldomain)
{
	zval **domain;
	int retval;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &domain) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_string_ex(domain);

	VPOPMAILLS_FETCH();
	VPOPMAILG(vpopmail_open) = 1;

	retval = vdeldomain(Z_STRVAL_PP(domain));

	if (retval == VA_SUCCESS) {
		RETURN_TRUE;
	}
	else {
        php_error(E_WARNING, "vpopmail error: %s", verror(retval));
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto void vpopmail_addaliasdomain(string domain, string aliasdomain)
   Add an alias for a virtual domain */
PHP_FUNCTION(vpopmail_addaliasdomain)
{
	zval **domain;
	zval **aliasdomain;
	char *tmpstr;
	char Dir[156];
	char TmpBuf1[300];
	char TmpBuf2[300];
	int uid, gid;
	int retval;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &domain, &aliasdomain) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_string_ex(domain);
	convert_to_string_ex(aliasdomain);

	php_strtolower(Z_STRVAL_PP(domain), Z_STRLEN_PP(domain));
	php_strtolower(Z_STRVAL_PP(aliasdomain), Z_STRLEN_PP(aliasdomain));

	VPOPMAILLS_FETCH();
	VPOPMAILG(vpopmail_open) = 1;

	tmpstr = vget_assign(Z_STRVAL_PP(domain), Dir, 156, &uid, &gid);

	if (tmpstr == NULL) {
        php_error(E_WARNING, "existing domain %s was not found", Z_STRVAL_PP(domain));
		RETURN_FALSE;
	}

	tmpstr = strstr(Dir, "/domains");
	*tmpstr = 0;

	sprintf(TmpBuf1, "%s/domains/%s", Dir, Z_STRVAL_PP(aliasdomain));
	sprintf(TmpBuf2, "%s/domains/%s", Dir, Z_STRVAL_PP(domain));

	if (symlink(TmpBuf2, TmpBuf1) != 0) {
        php_error(E_WARNING, "vpopmail_addaliasdomain could not symlink domains: %s", strerror(errno));
		RETURN_FALSE;
	}

	if (add_domain_assign(Z_STRVAL_PP(aliasdomain), Dir, uid, gid) != 0) {
		php_error(E_WARNING, "vpopmail_addaliasdomain could not add domain to control files");
		RETURN_FALSE;
	}

	signal_process("qmail-send", SIGHUP);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void vpopmail_adduser(string user, string domain, string password[, string gecos[, bool apop]])
   Add a new user to the specified virtual domain */
PHP_FUNCTION(vpopmail_adduser)
{
	zval **user;
	zval **domain;
	zval **password;
	zval **gecos;
	zval **apop;
	int is_apop = 0;
	char *the_gecos = "";
	int retval;

	if (ZEND_NUM_ARGS() < 3 || ZEND_NUM_ARGS() > 5
			|| zend_get_parameters_ex(ZEND_NUM_ARGS(), &user, &domain, &password, &gecos, &apop) == FAILURE)
		WRONG_PARAM_COUNT;

	switch (ZEND_NUM_ARGS()) {
	case 5:
		convert_to_boolean_ex(apop);
		is_apop = (Z_BVAL_PP(apop) ? 1 : 0);
		/* fall through */

	case 4:
		convert_to_string_ex(gecos);
		the_gecos = Z_STRVAL_PP(gecos);
		/* fall through */

	default:
		convert_to_string_ex(user);
		convert_to_string_ex(domain);
		convert_to_string_ex(password);
	}

	VPOPMAILLS_FETCH();
	VPOPMAILG(vpopmail_open) = 1;

	retval = vadduser(Z_STRVAL_PP(user),
					  Z_STRVAL_PP(domain),
					  Z_STRVAL_PP(password),
					  the_gecos,
					  is_apop);

	if (retval == VA_SUCCESS) {
		RETURN_TRUE;
	}
	else {
        php_error(E_WARNING, "vpopmail error: %s", verror(retval));
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto void vpopmail_deluser(string user, string domain)
   Delete a user from a virtual domain */
PHP_FUNCTION(vpopmail_deluser)
{
	zval **user;
	zval **domain;
	int retval;

	if (ZEND_NUM_ARGS() != 2
			|| zend_get_parameters_ex(2, &user, &domain) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_string_ex(user);
	convert_to_string_ex(domain);

	VPOPMAILLS_FETCH();
	VPOPMAILG(vpopmail_open) = 1;

	retval = vdeluser(Z_STRVAL_PP(user),
					  Z_STRVAL_PP(domain));

	if (retval == VA_SUCCESS) {
		RETURN_TRUE;
	}
	else {
        php_error(E_WARNING, "vpopmail error: %s", verror(retval));
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto void vpopmail_passwd(string user, string domain, string password)
   Change a virtual user's password */
PHP_FUNCTION(vpopmail_passwd)
{
	zval **user;
	zval **domain;
	zval **password;
	zval **apop;
	int is_apop = 0;
	int retval;

	if (ZEND_NUM_ARGS() < 3 || ZEND_NUM_ARGS() > 4
			|| zend_get_parameters_ex(ZEND_NUM_ARGS(), &user, &domain, &password, &apop) == FAILURE)
		WRONG_PARAM_COUNT;

	if (ZEND_NUM_ARGS() > 3) {
		convert_to_boolean_ex(apop);
		is_apop = (Z_BVAL_PP(apop) ? 1 : 0);
	}

	convert_to_string_ex(user);
	convert_to_string_ex(domain);
	convert_to_string_ex(password);

	VPOPMAILLS_FETCH();
	VPOPMAILG(vpopmail_open) = 1;

	retval = vpasswd(Z_STRVAL_PP(user),
					 Z_STRVAL_PP(domain),
					 Z_STRVAL_PP(password),
					 is_apop);

	if (retval == VA_SUCCESS) {
		RETURN_TRUE;
	}
	else {
        php_error(E_WARNING, "vpopmail error: %s", verror(retval));
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto void vpopmail_setuserquota(string user, string domain, string quota)
   Sets a virtual user's quota */
PHP_FUNCTION(vpopmail_setuserquota)
{
	zval **user;
	zval **domain;
	zval **quota;
	int retval;

	if (ZEND_NUM_ARGS() != 3
			|| zend_get_parameters_ex(3, &user, &domain, &quota) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_string_ex(user);
	convert_to_string_ex(domain);
	convert_to_string_ex(quota);

	VPOPMAILLS_FETCH();
	VPOPMAILG(vpopmail_open) = 1;

	retval = vsetuserquota(Z_STRVAL_PP(user),
						   Z_STRVAL_PP(domain),
						   Z_STRVAL_PP(quota));

	if (retval == VA_SUCCESS) {
		RETURN_TRUE;
	}
	else {
        php_error(E_WARNING, "vpopmail error: %s", verror(retval));
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto void vpopmail_auth_user(string user, string domain, string password[, bool apop])
   Attempt to validate a username/domain/password. Returns true/false */
PHP_FUNCTION(vpopmail_auth_user)
{
	zval **user;
	zval **domain;
	zval **password;
	zval **apop;
	int is_apop = 0;
	struct passwd *retval;

	if (ZEND_NUM_ARGS() < 3 || ZEND_NUM_ARGS() > 4
			|| zend_get_parameters_ex(ZEND_NUM_ARGS(), &user, &domain, &password, &apop) == FAILURE)
		WRONG_PARAM_COUNT;

	if (ZEND_NUM_ARGS() > 3) {
		convert_to_boolean_ex(apop);
		is_apop = (Z_BVAL_PP(apop) ? 1 : 0);
	}

	convert_to_string_ex(user);
	convert_to_string_ex(domain);
	convert_to_string_ex(password);

	VPOPMAILLS_FETCH();
	VPOPMAILG(vpopmail_open) = 1;

	retval = vauth_user(Z_STRVAL_PP(user),
						Z_STRVAL_PP(domain),
						Z_STRVAL_PP(password),
						is_apop);

	if (retval == NULL) {
		RETURN_FALSE;
	}
	else {
		RETURN_TRUE;
	}
}
/* }}} */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
