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

/* TODO: move to config.m4 when support for old versions is ready or just
 * don't support rather old vpopmail. current version must bail out if
 * incompat option is specified and work for minimal params
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#undef VPOPMAIL_IS_REALLY_OLD

#include <errno.h>
#include <signal.h>
#include "php.h"
#include "php_ini.h"
#include "php_vpopmail.h"

#if HAVE_VPOPMAIL

#include "ext/standard/exec.h"
#include "ext/standard/info.h"
#include "ext/standard/php_string.h"

ZEND_DECLARE_MODULE_GLOBALS(vpopmail)

/* keep this as the last include in order to destroy VERSION/PACKAGE only for the rest of the code */

#undef VERSION
#undef PACKAGE
#include "vpopmail_config.h"
#include "vpopmail.h"
#if HAVE_VPOPMAIL_VAUTH
#include "vauth.h"
#endif


/* vpopmail does not export this, argh! */
#define MAX_BUFF 500

/* Function table */

function_entry vpopmail_functions[] = {
	/* domain management - lib call */
	PHP_FE(vpopmail_add_domain, NULL)
	PHP_FE(vpopmail_del_domain, NULL)
	PHP_FE(vpopmail_add_alias_domain, NULL)
	/* domain management - exec */
	PHP_FE(vpopmail_add_domain_ex, NULL)
	PHP_FE(vpopmail_del_domain_ex, NULL)
	PHP_FE(vpopmail_add_alias_domain_ex, NULL)
	/* user management */
	PHP_FE(vpopmail_add_user, NULL)
	PHP_FE(vpopmail_del_user, NULL)
	PHP_FE(vpopmail_passwd, NULL)
	PHP_FE(vpopmail_set_user_quota, NULL)
	PHP_FE(vpopmail_auth_user, NULL)
	/* alias management */
#if HAVE_VPOPMAIL_VALIAS
	PHP_FE(vpopmail_alias_add, NULL)
	PHP_FE(vpopmail_alias_del, NULL)
#ifdef VALIAS
	PHP_FE(vpopmail_alias_del_domain, NULL)
#endif
	PHP_FE(vpopmail_alias_get, NULL)
	PHP_FE(vpopmail_alias_get_all, NULL)
#endif
	/* error handling */
	PHP_FE(vpopmail_error, NULL)
	{NULL, NULL, NULL}
};

zend_module_entry vpopmail_module_entry = {
	STANDARD_MODULE_HEADER,
	"vpopmail",
	vpopmail_functions,
	PHP_MINIT(vpopmail),
	PHP_MSHUTDOWN(vpopmail),
	PHP_RINIT(vpopmail),
	PHP_RSHUTDOWN(vpopmail),
	PHP_MINFO(vpopmail),
	NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_VPOPMAIL
ZEND_GET_MODULE(vpopmail)
#endif


PHP_INI_BEGIN()
	/*STD_PHP_INI_ENTRY("vpopmail.directory",		"",				PHP_INI_ALL, OnUpdateString,	directory,			php_vpopmail_globals,	vpopmail_globals)*/
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
	VPOPMAILG(vpopmail_open) = 0;
	VPOPMAILG(vpopmail_errno) = 0;

	return SUCCESS;
}

/* nasty but... */
void vclose();
/* ...we need this */

PHP_RSHUTDOWN_FUNCTION(vpopmail)
{
	if (VPOPMAILG(vpopmail_open) != 0) {
		vclose();
	}
	return SUCCESS;
}

PHP_MINFO_FUNCTION(vpopmail)
{
	char ids[64];

	sprintf(ids, "%d/%d %d/%d/%d/%d", VPOPMAILUID, VPOPMAILGID, getuid(), getgid(), geteuid(), getegid());

	php_info_print_table_start();
	php_info_print_table_header(2, "vpopmail support", "enabled");
	php_info_print_table_row(2, "vpopmail version", VERSION);
	php_info_print_table_row(2, "vpopmail uid/gid php uid/gid/euid/egid", ids);
	php_info_print_table_row(2, "vpopmail dir", VPOPMAILDIR);
	php_info_print_table_row(2, "vpopmail vadddomain", VPOPMAIL_BIN_DIR VPOPMAIL_ADDD);
	php_info_print_table_row(2, "vpopmail vdeldomain", VPOPMAIL_BIN_DIR VPOPMAIL_DELD);
	php_info_print_table_row(2, "vpopmail vaddaliasdomain", VPOPMAIL_BIN_DIR VPOPMAIL_ADAD);
#if HAVE_VPOPMAIL_VALIAS
	php_info_print_table_row(2, "vpopmail valias support", "Enabled");
#else
	php_info_print_table_row(2, "vpopmail valias support", "Not supported by vpopmail");
#endif
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}

/*
 * Domain management functions - library call
 */

/* {{{ proto bool vpopmail_add_domain(string domain, string dir, int uid, int gid)
   Adds a new virtual domain */
PHP_FUNCTION(vpopmail_add_domain)
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
	VPOPMAILG(vpopmail_errno)=retval;

	if (retval == VA_SUCCESS) {
		RETURN_TRUE;
	} else {
	        php_error(E_WARNING, "vpopmail error: %s", verror(retval));
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool vpopmail_del_domain(string domain)
   Deletes a virtual domain */
PHP_FUNCTION(vpopmail_del_domain)
{
	zval **domain;
	int retval;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &domain) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_string_ex(domain);

	VPOPMAILG(vpopmail_open) = 1;

	retval = vdeldomain(Z_STRVAL_PP(domain));
	
	VPOPMAILG(vpopmail_errno)=retval;

	if (retval == VA_SUCCESS) {
		RETURN_TRUE;
	} else {
	        php_error(E_WARNING, "vpopmail error: %s", verror(retval));
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool vpopmail_add_alias_domain(string domain, string aliasdomain)
   Adds an alias for a virtual domain */
PHP_FUNCTION(vpopmail_add_alias_domain)
{
	zval **domain;
	zval **aliasdomain;
	char *tmpstr;
	char Dir[156];
	char TmpBuf1[300];
	char TmpBuf2[300];
	int uid, gid;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &domain, &aliasdomain) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_string_ex(domain);
	convert_to_string_ex(aliasdomain);

	php_strtolower(Z_STRVAL_PP(domain), Z_STRLEN_PP(domain));
	php_strtolower(Z_STRVAL_PP(aliasdomain), Z_STRLEN_PP(aliasdomain));

	VPOPMAILG(vpopmail_open) = 1;

	tmpstr = vget_assign(Z_STRVAL_PP(domain), Dir, 156, &uid, &gid);

	if (tmpstr == NULL) {
		php_error(E_WARNING, "vpopmail_add_alias_domain error: existing domain %s was not found", Z_STRVAL_PP(domain));
		VPOPMAILG(vpopmail_errno) = 1;
		RETURN_FALSE;
	}

	tmpstr = strstr(Dir, "/domains");
	*tmpstr = 0;

	sprintf(TmpBuf1, "%s/domains/%s", Dir, Z_STRVAL_PP(aliasdomain));
	sprintf(TmpBuf2, "%s/domains/%s", Dir, Z_STRVAL_PP(domain));

	if (symlink(TmpBuf2, TmpBuf1) != 0) {
	        php_error(E_WARNING, "vpopmail_add_alias_domain error: could not symlink domains: %s", strerror(errno));
		VPOPMAILG(vpopmail_errno) = 1;
		RETURN_FALSE;
	}

#if (!defined(HAVE_VPOPMAIL_API) || HAVE_VPOPMAIL_API == 1)
	if (add_domain_assign(Z_STRVAL_PP(aliasdomain), Dir, uid, gid) != 1) {
#else
	if (add_domain_assign(Z_STRVAL_PP(aliasdomain), Z_STRVAL_PP(domain), Dir, uid, gid) != 1) {
#endif
		php_error(E_WARNING, "vpopmail_addaliasdomain could not add domain to control files");
		VPOPMAILG(vpopmail_errno) = 1;
		RETURN_FALSE;
	}

	signal_process("qmail-send", SIGHUP);

	VPOPMAILG(vpopmail_errno) = 0;
	RETURN_TRUE;
}
/* }}} */

/*
 * Domain management functions - exec
 */

/* {{{ proto bool vpopmail_add_domain_ex(string domain, string passwd [, string quota [, string bounce [, bool apop]]])
   Adds a new virtual domain */
PHP_FUNCTION(vpopmail_add_domain_ex)
{
	zval **domain, **passwd, **quota, **bounce, **apop;
	int retval,len=0,argc=ZEND_NUM_ARGS(),is_bounce_email;
	int fr_bounce=0,fr_quota=0;
	char *cmd,*escdomain="",*escpasswd="",*escquota="",*escbounce="",*escapop="";

	if (argc < 2 || argc > 5 || zend_get_parameters_ex(argc, &domain, &passwd, &quota, &bounce, &apop) == FAILURE){
		WRONG_PARAM_COUNT;
	}
	
	switch (argc) {
		case 5:
			convert_to_long_ex(apop);
			escapop=Z_BVAL_PP(apop)?"1":"0";
			/* Fall-through. */
		case 4:
			fr_bounce=1;
			convert_to_string_ex(bounce);
			escbounce=php_escape_shell_cmd(Z_STRVAL_PP(bounce));
			if (!escbounce) {
				php_error(E_WARNING,"vpopmail_adddomain error: cannot alloc");
				VPOPMAILG(vpopmail_errno)=1;
				RETURN_FALSE;
			}
			/* Fall-through. */
		case 3:
			fr_quota=1;
			convert_to_string_ex(quota);
			escquota=php_escape_shell_cmd(Z_STRVAL_PP(quota));
			if (!escquota) {
				php_error(E_WARNING,"vpopmail_adddomain error: cannot alloc");
				VPOPMAILG(vpopmail_errno)=1;
				RETURN_FALSE;
			}
			/* Fall-through. */
		case 2:
			convert_to_string_ex(passwd);
			convert_to_string_ex(domain);
			break;
	}

	escdomain=php_escape_shell_cmd(Z_STRVAL_PP(domain));
	escpasswd=php_escape_shell_cmd(Z_STRVAL_PP(passwd));
	if (!escdomain||!escpasswd) {
		if (fr_quota)
			efree(escquota);
		if (fr_bounce)
			efree(escbounce);
		php_error(E_WARNING,"vpopmail_adddomain error: cannot alloc");
		VPOPMAILG(vpopmail_errno)=1;
		RETURN_FALSE;
	}
	len+=strlen(VPOPMAIL_BIN_DIR);
	len+=strlen(VPOPMAIL_ADDD);
	if (*escquota)
		len+=strlen("-q ")+strlen(escquota)+strlen(" ");
	if (*escbounce) {
		if (strchr(Z_STRVAL_PP(bounce),'@')) {
			is_bounce_email=1;
			len+=strlen("-e ")+strlen(escbounce)+strlen(" ");
		} else {
			is_bounce_email=0;
			len+=strlen("-b ");
		}
	}
	if (*escapop)
		len+=strlen("-a ");
	len+=strlen(escdomain)+strlen(" ");
	len+=strlen(escpasswd)+strlen(" ");
	len++;
	cmd=emalloc(len);
	if (!cmd) {
		if (fr_quota)
			efree(escquota);
		if (fr_bounce)
			efree(escbounce);
		efree(escdomain);
		efree(escpasswd);
		php_error(E_WARNING,"vpopmail_adddomain error: cannot alloc");
		VPOPMAILG(vpopmail_errno)=1;
		RETURN_FALSE;
	}
	strcpy(cmd,VPOPMAIL_BIN_DIR VPOPMAIL_ADDD);
	if (*escquota) {
		strcat(cmd,"-q ");
		strcat(cmd,escquota);
		strcat(cmd," ");
	}
	if (*escbounce) {
		if (is_bounce_email) {
			strcat(cmd,"-e ");
			strcat(cmd,escbounce);
			strcat(cmd," ");
		} else {
			strcat(cmd,"-b ");
		}
	}
	if (*escapop)
		strcat(cmd,"-a ");
	strcat(cmd,escdomain);
	strcat(cmd," ");
	strcat(cmd,escpasswd);
	retval=php_Exec(0,cmd,NULL,return_value TSRMLS_CC);
	efree(cmd);
	efree(escdomain);
	efree(escpasswd);
	if (fr_quota)
		efree(escquota);
	if (fr_bounce)
		efree(escbounce);

	if (retval!=VA_SUCCESS) {
		php_error(E_WARNING,"vpopmail_add_domain_ex error: %d", retval);
		VPOPMAILG(vpopmail_errno)=1;
		RETURN_FALSE;
	} else {
		VPOPMAILG(vpopmail_errno)=0;
		RETURN_TRUE;
	}
}
/* }}} */

/* {{{ proto bool vpopmail_del_domain_ex(string domain)
   Deletes a virtual domain */
PHP_FUNCTION(vpopmail_del_domain_ex)
{
	zval **domain;
	int retval=-1;
	char *cmd,*escdomain;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &domain) == FAILURE){
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(domain);

	escdomain=php_escape_shell_cmd(Z_STRVAL_PP(domain));
	if (!escdomain) {
		php_error(E_WARNING,"vpopmail_del_domain_ex error: cannot alloc");
		VPOPMAILG(vpopmail_errno)=1;
		RETURN_FALSE;
	}
	cmd=emalloc(strlen(VPOPMAIL_BIN_DIR)+strlen(VPOPMAIL_DELD)+strlen(escdomain)+1);
	if (!cmd) {
		efree(escdomain);
		php_error(E_WARNING,"vpopmail_del_domain_ex error: cannot alloc");
		VPOPMAILG(vpopmail_errno)=1;
		RETURN_FALSE;
	}
	sprintf(cmd,VPOPMAIL_BIN_DIR VPOPMAIL_DELD"%s",escdomain);
	retval=php_Exec(0,cmd,NULL,return_value TSRMLS_CC);
	efree(escdomain);
	efree(cmd);

	if (retval!=VA_SUCCESS) {
		php_error(E_WARNING,"vpopmail_del_domain_ex error: %d", retval);
		VPOPMAILG(vpopmail_errno)=1;
		RETURN_FALSE;
	} else {
		VPOPMAILG(vpopmail_errno)=0;
		RETURN_TRUE;
	}
}
/* }}} */

/* {{{ proto bool vpopmail_add_alias_domain_ex(string olddomain, string newdomain)
   Adds an alias to an existing virtual domain */
PHP_FUNCTION(vpopmail_add_alias_domain_ex)
{
	zval **olddomain, **newdomain;
	int retval;
	char *cmd,*escolddomain,*escnewdomain;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &olddomain, &newdomain) == FAILURE){
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(olddomain);
	convert_to_string_ex(newdomain);
	escnewdomain=php_escape_shell_cmd(Z_STRVAL_PP(newdomain));
	if (!escnewdomain) {
		php_error(E_WARNING,"vpopmail_addaliasdomain error: cannot alloc");
		VPOPMAILG(vpopmail_errno)=1;
		RETURN_FALSE;
	}
	escolddomain=php_escape_shell_cmd(Z_STRVAL_PP(olddomain));
	if (!escolddomain) {
		efree(escnewdomain);
		php_error(E_WARNING,"vpopmail_addaliasdomain error: cannot alloc");
		VPOPMAILG(vpopmail_errno)=1;
		RETURN_FALSE;
	}

	cmd=emalloc(strlen(VPOPMAIL_BIN_DIR VPOPMAIL_ADAD)+strlen(escolddomain)+strlen(" ")+strlen(escnewdomain)+1);
	if (!cmd) {
		efree(escnewdomain);
		efree(escolddomain);
		php_error(E_WARNING,"vpopmail_addaliasdomain error: cannot alloc");
		VPOPMAILG(vpopmail_errno)=1;
		RETURN_FALSE;
	}
	sprintf(cmd,"%s%s %s",VPOPMAIL_BIN_DIR VPOPMAIL_ADAD,escolddomain,escnewdomain);
	retval=php_Exec(0,cmd,NULL,return_value TSRMLS_CC);
	efree(cmd);
	efree(escnewdomain);
	efree(escolddomain);

	if (retval!=VA_SUCCESS) {
		php_error(E_WARNING,"vpopmail_addaliasdomain error: %d", retval);
		VPOPMAILG(vpopmail_errno)=1;
		RETURN_FALSE;
	} else {
		VPOPMAILG(vpopmail_errno)=0;
		RETURN_TRUE;
	}
}
/* }}} */

/*
 * User management functions
 */

/* {{{ proto bool vpopmail_add_user(string user, string domain, string password[, string gecos[, bool apop]])
   Adds a new user to the specified virtual domain */
PHP_FUNCTION(vpopmail_add_user)
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

	VPOPMAILG(vpopmail_open) = 1;

	retval = vadduser(Z_STRVAL_PP(user),
					  Z_STRVAL_PP(domain),
					  Z_STRVAL_PP(password),
					  the_gecos,
					  is_apop);
	
	VPOPMAILG(vpopmail_errno)=retval;

	if (retval == VA_SUCCESS) {
		RETURN_TRUE;
	} else {
        	php_error(E_WARNING, "vpopmail error: %s", verror(retval));
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool vpopmail_del_user(string user, string domain)
   Deletes a user from a virtual domain */
PHP_FUNCTION(vpopmail_del_user)
{
	zval **user;
	zval **domain;
	int retval;

	if (ZEND_NUM_ARGS() != 2
			|| zend_get_parameters_ex(2, &user, &domain) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_string_ex(user);
	convert_to_string_ex(domain);

	VPOPMAILG(vpopmail_open) = 1;

	retval = vdeluser(Z_STRVAL_PP(user),
					  Z_STRVAL_PP(domain));

	VPOPMAILG(vpopmail_errno)=retval;

	if (retval == VA_SUCCESS) {
		RETURN_TRUE;
	} else {
	        php_error(E_WARNING, "vpopmail error: %s", verror(retval));
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool vpopmail_passwd(string user, string domain, string password)
   Changes user's password */
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

	VPOPMAILG(vpopmail_open) = 1;

	retval = vpasswd(Z_STRVAL_PP(user),
					 Z_STRVAL_PP(domain),
					 Z_STRVAL_PP(password),
					 is_apop);
	
	VPOPMAILG(vpopmail_errno)=retval;

	if (retval == VA_SUCCESS) {
		RETURN_TRUE;
	} else {
	        php_error(E_WARNING, "vpopmail error: %s", verror(retval));
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool vpopmail_set_user_quota(string user, string domain, string quota)
   Sets user's quota */
PHP_FUNCTION(vpopmail_set_user_quota)
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

	VPOPMAILG(vpopmail_open) = 1;

	retval = vsetuserquota(Z_STRVAL_PP(user),
						   Z_STRVAL_PP(domain),
						   Z_STRVAL_PP(quota));
	
	VPOPMAILG(vpopmail_errno)=retval;

	if (retval == VA_SUCCESS) {
		RETURN_TRUE;
	} else {
	        php_error(E_WARNING, "vpopmail error: %s", verror(retval));
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool vpopmail_auth_user(string user, string domain, string password[, string apop])
   Attempts to validate a username/domain/password. Returns true/false */
PHP_FUNCTION(vpopmail_auth_user)
{
	zval **user;
	zval **domain;
	zval **password;
	zval **apop;
	/* indeed we do not care of contents; newer vpopmail versions use struct vpasswd instead passwd */
	void *retval;
	int argc=ZEND_NUM_ARGS();

	if (argc < 3 || argc > 4
			|| zend_get_parameters_ex(argc, &user, &domain, &password, &apop) == FAILURE)
		WRONG_PARAM_COUNT;

	if (argc > 3)
		convert_to_string_ex(apop);

	convert_to_string_ex(user);
	convert_to_string_ex(domain);
	convert_to_string_ex(password);

	VPOPMAILG(vpopmail_open) = 1;
	VPOPMAILG(vpopmail_errno) = 0;

	retval = vauth_user(Z_STRVAL_PP(user),
				Z_STRVAL_PP(domain),
				Z_STRVAL_PP(password),
				(argc>3)?Z_STRVAL_PP(apop):"");

	/* 
	 * we do not set vpopmail_errno here - it is considered auth_user cannot fail; insted it does not auth
	 * this is a vpopmail's api limitation - there is no error return form vauth_user
	 */

	if (retval == NULL) {
		RETURN_FALSE;
	} else {
		RETURN_TRUE;
	}
}
/* }}} */


#if HAVE_VPOPMAIL_VALIAS

/*
 * Alias management functions
 */


/* {{{ proto bool vpopmail_alias_add(string user, string domain, string alias)
   Inserts a virtual alias */
PHP_FUNCTION(vpopmail_alias_add)
{
	zval **user;
	zval **domain;
	zval **alias;
	int retval;

	if (ZEND_NUM_ARGS() != 3
			|| zend_get_parameters_ex(ZEND_NUM_ARGS(), &user, &domain, &alias) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_string_ex(user);
	convert_to_string_ex(domain);
	convert_to_string_ex(alias);

	VPOPMAILG(vpopmail_open) = 1;

	retval = valias_insert(Z_STRVAL_PP(user),
						   Z_STRVAL_PP(domain),
						   Z_STRVAL_PP(alias));
	
	VPOPMAILG(vpopmail_errno)=retval;

	if (retval == VA_SUCCESS) {
		RETURN_TRUE;
	} else {
	        php_error(E_WARNING, "vpopmail error: %s", verror(retval));
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool vpopmail_alias_del(string user, string domain)
   Deletes all virtual aliases of a given user */
PHP_FUNCTION(vpopmail_alias_del)
{
	zval **user;
	zval **domain;
	int retval;

	if (ZEND_NUM_ARGS() != 2
			|| zend_get_parameters_ex(ZEND_NUM_ARGS(), &user, &domain) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_string_ex(user);
	convert_to_string_ex(domain);

	VPOPMAILG(vpopmail_open) = 1;

	retval = valias_delete(Z_STRVAL_PP(user), Z_STRVAL_PP(domain));
	
	VPOPMAILG(vpopmail_errno)=retval;

	if (retval == VA_SUCCESS) {
		RETURN_TRUE;
	} else {
	        php_error(E_WARNING, "vpopmail error: %s", verror(retval));
		RETURN_FALSE;
	}
}
/* }}} */

#ifdef VALIAS
/* {{{ proto bool vpopmail_alias_del_domain(string domain)
   Deletes all virtual aliases of a given domain */
PHP_FUNCTION(vpopmail_alias_del_domain)
{
	zval **domain;
	int retval;

	if (ZEND_NUM_ARGS() != 1
			|| zend_get_parameters_ex(ZEND_NUM_ARGS(), &domain) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_string_ex(domain);

	VPOPMAILG(vpopmail_open) = 1;

	retval = valias_delete_domain(Z_STRVAL_PP(domain));
	
	VPOPMAILG(vpopmail_errno)=retval;

	if (retval == VA_SUCCESS) {
		RETURN_TRUE;
	} else {
	        php_error(E_WARNING, "vpopmail error: %s", verror(retval));
		RETURN_FALSE;
	}
}
/* }}} */
#endif

/* {{{ proto array vpopmail_alias_get(string alias, string domain)
   Returns all lines of an alias for a given domain */
PHP_FUNCTION(vpopmail_alias_get)
{
	zval **alias;
	zval **domain;
	int retval;
	char *talias;

	if (ZEND_NUM_ARGS() != 2
			|| zend_get_parameters_ex(ZEND_NUM_ARGS(), &alias, &domain) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_string_ex(alias);
	convert_to_string_ex(domain);

	VPOPMAILG(vpopmail_open) = 1;

	if (array_init(return_value)!=SUCCESS) {
		zend_error(E_ERROR,"unable to create array");
		RETURN_FALSE;
	}

	talias=valias_select(Z_STRVAL_PP(alias), Z_STRVAL_PP(domain));
	while (talias) {
		add_next_index_string(return_value,talias,1);
		talias=valias_select_next();
	}
}
/* }}} */

/* {{{ proto array vpopmail_alias_get_all(string domain)
   Returns all lines of all aliases for a given domain */
PHP_FUNCTION(vpopmail_alias_get_all)
{
	zval **domain;
	int retval;
	char *talias;
	char tpath[1024];
	char ppath[1024]="";
	zval *tarr;

	if (ZEND_NUM_ARGS() != 1
			|| zend_get_parameters_ex(ZEND_NUM_ARGS(), &domain) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_string_ex(domain);

	VPOPMAILG(vpopmail_open) = 1;

	if (array_init(return_value)!=SUCCESS) {
		zend_error(E_ERROR,"unable to create array");
		RETURN_FALSE;
	}

	talias=valias_select_all(tpath, Z_STRVAL_PP(domain));
	while (talias) {
		if (strcmp(ppath,tpath)) {
			MAKE_STD_ZVAL(tarr);
			if (array_init(tarr)!=SUCCESS) {
				zend_error(E_ERROR,"unable to create array");
				RETURN_FALSE;
			}
			add_assoc_zval(return_value,tpath,tarr);
			strcpy(ppath,tpath);
		}
		add_next_index_string(tarr,talias,1);
		talias=valias_select_all_next(tpath);
	}
}
/* }}} */

#endif

/*
 * Error handling helper
 */

/* {{{ proto string vpopmail_error(void)
   Returns text message for last vpopmail error */
PHP_FUNCTION(vpopmail_error)
{
	if (ZEND_NUM_ARGS() != 0)
		WRONG_PARAM_COUNT;
	
	RETURN_STRING(verror(VPOPMAILG(vpopmail_errno)),1);
}
/* }}} */

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
