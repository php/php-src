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
   | Author: David Croft <david@infotrek.co.uk>                           |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#undef VPOPMAIL_IS_REALLY_OLD

#include "php.h"
#include "php_ini.h"
#include "php_vpopmail.h"

#include "vpopmail.h"

#include "ext/standard/php_string.h"

#ifdef ZTS
int vpopmail_globals_id;
#else
php_vpopmail_globals vpopmail_globals;
#endif

/* Function table */

function_entry vpopmail_functions[] = {
	PHP_FE(vpopmail_auth_user, NULL)
	PHP_FE(vpopmail_adddomain, NULL)
	PHP_FE(vpopmail_deldomain, NULL)
	PHP_FE(vpopmail_adduser, NULL)
	PHP_FE(vpopmail_deluser, NULL)
	PHP_FE(vpopmail_passwd, NULL)
	PHP_FE(vpopmail_setuserquota, NULL)
	{NULL, NULL, NULL}
};

zend_module_entry vpopmail_module_entry = {
	"vpopmail",
	vpopmail_functions,
	PHP_MINIT(vpopmail),
	PHP_MSHUTDOWN(vpopmail),
	NULL,
	NULL,
	PHP_MINFO(vpopmail),
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_VPOPMAIL
ZEND_GET_MODULE(vpopmail)
#endif


PHP_INI_BEGIN()
	/*	STD_PHP_INI_ENTRY("pfpro.proxypassword",		"",				PHP_INI_ALL, OnUpdateString,	proxypassword,			php_pfpro_globals,	pfpro_globals) */
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

PHP_MINFO_FUNCTION(vpopmail)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "vpopmail support", "enabled");
	/*	php_info_print_table_row(2, "vpopmail version", "Who knows"); */
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}



/* {{{ proto void vpopmail_adddomain($domain, $dir, $uid, $gid)
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

	retval = vadddomain(Z_STRVAL_PP(domain),
#ifdef VPOPMAIL_IS_REALLY_OLD
						Z_STRVAL_PP(dir),
						Z_LVAL_PP(uid),
						Z_LVAL_PP(gid)
#else
						0
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

/* {{{ proto void vpopmail_deldomain($domain)
   Delete a virtual domain */
PHP_FUNCTION(vpopmail_deldomain)
{
	zval **domain;
	int retval;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &domain) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_string_ex(domain);

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

/* {{{ proto void vpopmail_adduser($user, $domain, $password[, $gecos[, $apop]])
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

/* {{{ proto void vpopmail_deluser($user, $domain)
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

/* {{{ proto void vpopmail_passwd($user, $domain, $password)
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

/* {{{ proto void vpopmail_setuserquota($user, $domain, $quota)
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

/* {{{ proto void vpopmail_auth_user($user, $domain, $password)
   Attempt to validate a username/domain/password. Returns true/false */
PHP_FUNCTION(vpopmail_auth_user)
{
	zval **user;
	zval **domain;
	zval **password;
	struct passwd *retval;

	if (ZEND_NUM_ARGS() != 3
			|| zend_get_parameters_ex(3, &user, &domain, &password) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_string_ex(user);
	convert_to_string_ex(domain);
	convert_to_string_ex(password);

	retval = vauth_user(Z_STRVAL_PP(user),
						Z_STRVAL_PP(domain),
						Z_STRVAL_PP(password));

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
