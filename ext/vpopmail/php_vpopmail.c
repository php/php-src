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

// TODO: move to config.m4 when support for old versions is ready or just don't support rather old vpopmail
// current version must bail out if incompat option is specified and work for minimal params
//#undef VPOPMAIL_IS_REALLY_OLD

#include "php.h"
#include "php_ini.h"
#include "php_vpopmail.h"

#if HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif

#if HAVE_VPOPMAIL

#include "vpopmail.h"

#include "ext/standard/exec.h"
#include "ext/standard/php_string.h"

// keep this as the last include in order to destroy VERSION/PACKAGE only for the rest of the code
#undef VERSION
#undef PACKAGE
#include "vpopmail_config.h"
#undef PACKAGE
#include "vpopmail.h"

// vpopmail does not export this, argh!
#define MAX_BUFF 500

#define VPOPSTR(x) #x

/* removed by intent - will add when need to
ZEND_DECLARE_MODULE_GLOBALS(vpop)
*/

/* Function table */

function_entry vpopmail_functions[] = {
	// domain management
	PHP_FE(vpopmail_adddomain, NULL)
	PHP_FE(vpopmail_deldomain, NULL)
	PHP_FE(vpopmail_addaliasdomain,	NULL)
	// user management
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
	NULL, //PHP_MINIT(vpopmail),		// do nothing upon
	NULL, //PHP_MSHUTDOWN(vpopmail),	// 	these
	NULL, //PHP_RINIT(vpopmail),		// 	events
	PHP_RSHUTDOWN(vpopmail),		// close vpopmail lib upon request shutdown
	PHP_MINFO(vpopmail),
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_VPOPMAIL
ZEND_GET_MODULE(vpopmail)
#endif

// removed by intent - will add when needed
//PHP_INI_BEGIN()
/*	STD_PHP_INI_ENTRY("pfpro.proxypassword",		"",				PHP_INI_ALL, OnUpdateString,	proxypassword,			php_pfpro_globals,	pfpro_globals) */
//PHP_INI_END()


// removed by intent - will add when needed
/*PHP_MINIT_FUNCTION(vpopmail) {
	REGISTER_INI_ENTRIES();
	return SUCCESS;
}*/

// removed by intent - will add when needed
/*PHP_MSHUTDOWN_FUNCTION(vpopmail)
{
	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}*/

/* at request end we close the connection to any databases open by the lib */
PHP_RSHUTDOWN_FUNCTION(vpopmail) {
	vclose();
	return SUCCESS;
}

PHP_MINFO_FUNCTION(vpopmail) {
	char g[64],u[64];

	sprintf(g,"%d",VPOPMAILGID);
	sprintf(u,"%d",VPOPMAILUID);
	php_info_print_table_start();
	php_info_print_table_row(2, "vpopmail support", "enabled");
	php_info_print_table_row(2, "vpopmail version", VERSION);
	php_info_print_table_row(2, "vpopmail uid", u);
	php_info_print_table_row(2, "vpopmail gid", g);
	php_info_print_table_row(2, "vpopmail dir", VPOPMAILDIR);
	php_info_print_table_row(2, "vpopmail vadddomain", VPOPMAIL_BIN_DIR VPOPMAIL_ADDD);
	php_info_print_table_row(2, "vpopmail vdeldomain", VPOPMAIL_BIN_DIR VPOPMAIL_DELD);
	php_info_print_table_row(2, "vpopmail vaddaliasdomain", VPOPMAIL_BIN_DIR VPOPMAIL_ADAD);
	php_info_print_table_end();

	// will add when needed
	//DISPLAY_INI_ENTRIES();
}

/*
 * shall not copy/paste this stuff but rather make _Exec from ext/standard/exec.c public as php_exec or something
 */
static int vpopmail_exec(char *cmd) {
	FILE *fp;
	int t, l, ret, output=1;
	PLS_FETCH();

#ifdef PHP_WIN32
	fp = V_POPEN(cmd, "rb");
#else
	fp = V_POPEN(cmd, "r");
#endif
	if (!fp) {
		php_error(E_WARNING, "Unable to fork [%s]", cmd);
		return -1;
	}
	ret = pclose(fp);
#if HAVE_SYS_WAIT_H
	if (WIFEXITED(ret)) {
		ret = WEXITSTATUS(ret);
	}
#endif
	return ret;
}

//////////////////////////////////////////////////////////////////////////
// domain management functions (these are slow - implementation executes external binary

/* {{{ proto int vpopmail_adddomain(string domain, string passwd [, string quota [, string bounce [, bool apop]]])
   Add a new virtual domain */
PHP_FUNCTION(vpopmail_adddomain) {	// this code cannot work if not suid/sgid
	zval **domain, **passwd, **quota, **bounce, **apop;
	int retval,len=0,argc=ZEND_NUM_ARGS(),Uid=VPOPMAILUID,Gid=VPOPMAILGID,is_bounce_email;
	int fr_bounce=0,fr_quota=0;
	char *q,*bo,*cmd,*escdomain="",*escpasswd="",*escquota="",*escbounce="",*escapop="";

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
				RETURN_LONG(-1);
			}
			/* Fall-through. */
		case 3:
			fr_quota=1;
			convert_to_string_ex(quota);
			escquota=php_escape_shell_cmd(Z_STRVAL_PP(quota));
			if (!escquota) {
				php_error(E_WARNING,"vpopmail_adddomain error: cannot alloc");
				RETURN_LONG(-1);
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
		RETURN_LONG(-1);
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
		RETURN_LONG(-1);
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
	retval=vpopmail_exec(cmd);
	efree(cmd);
	efree(escdomain);
	efree(escpasswd);
	if (fr_quota)
		efree(escquota);
	if (fr_bounce)
		efree(escbounce);

	if (retval!=VA_SUCCESS)
		php_error(E_WARNING,"vpopmail_adddomain error: %d", retval);

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto int vpopmail_deldomain(string domain)
   Delete a virtual domain */
PHP_FUNCTION(vpopmail_deldomain) {
	zval **domain;
	int retval=-1;
	char *cmd,*escdomain;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &domain) == FAILURE){
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(domain);

	escdomain=php_escape_shell_cmd(Z_STRVAL_PP(domain));
	if (!escdomain) {
		php_error(E_WARNING,"vpopmail_deldomain error: cannot alloc");
		RETURN_LONG(-1);
	}
	cmd=emalloc(strlen(VPOPMAIL_BIN_DIR)+strlen(VPOPMAIL_DELD)+strlen(escdomain)+1);
	if (!cmd) {
		efree(escdomain);
		php_error(E_WARNING,"vpopmail_deldomain error: cannot alloc");
		RETURN_LONG(-1);
	}
	sprintf(cmd,VPOPMAIL_BIN_DIR VPOPMAIL_DELD"%s",escdomain);
	retval=vpopmail_exec(cmd);
	efree(escdomain);
	efree(cmd);

	if (retval!=VA_SUCCESS)
		php_error(E_WARNING,"vpopmail_deldomain error: %d", retval);

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto int vpopmail_addaliasdomain(string olddomain, string newdomain)
   Add alias to an existing virtual domain */
PHP_FUNCTION(vpopmail_addaliasdomain) {
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
		RETURN_LONG(-1);
	}
	escolddomain=php_escape_shell_cmd(Z_STRVAL_PP(olddomain));
	if (!escolddomain) {
		efree(escnewdomain);
		php_error(E_WARNING,"vpopmail_addaliasdomain error: cannot alloc");
		RETURN_LONG(-1);
	}

	cmd=emalloc(strlen(VPOPMAIL_BIN_DIR VPOPMAIL_ADAD)+strlen(escolddomain)+strlen(" ")+strlen(escnewdomain)+1);
	if (!cmd) {
		efree(escnewdomain);
		efree(escolddomain);
		php_error(E_WARNING,"vpopmail_addaliasdomain error: cannot alloc");
		RETURN_LONG(-1);
	}
	sprintf(cmd,"%s%s %s",VPOPMAIL_BIN_DIR VPOPMAIL_ADAD,escolddomain,escnewdomain);
	retval=vpopmail_exec(cmd);
	efree(cmd);
	efree(escnewdomain);
	efree(escolddomain);

	if (retval!=VA_SUCCESS)
		php_error(E_WARNING,"vpopmail_addaliasdomain error: %d", retval);

	RETURN_LONG(retval);
}
/* }}} */

//////////////////////////////////////////////////////////////////////////
// user management functions - native implementation - fast

/* {{{ proto int vpopmail_adduser(string username, string domain, string password [, string quota [, string gecos [, int apop]]])
   Add a new user to the specified virtual domain */
PHP_FUNCTION(vpopmail_adduser) {
	zval **username, **domain, **password, **quota, **gecos, **apop;
	int argc;
	int retval,hv_apop=0,hv_gecos=0,hv_quota=0;

	argc = ZEND_NUM_ARGS();
	if (argc < 3 || argc > 6 || zend_get_parameters_ex(argc, &username, &domain, &password, &quota, &gecos, &apop) == FAILURE){
		WRONG_PARAM_COUNT;
	}

	switch (argc) {
		case 6:
			hv_apop=1;
			convert_to_long_ex(apop);
			/* Fall-through. */
		case 5:
			hv_gecos=1;
			convert_to_string_ex(gecos);
			/* Fall-through. */
		case 4:
			hv_quota=1;
			convert_to_string_ex(quota);
			/* Fall-through. */
		case 3:
			convert_to_string_ex(password);
			convert_to_string_ex(domain);
			convert_to_string_ex(username);
			break;
	}

	retval=vadduser(Z_STRVAL_PP(username),Z_STRVAL_PP(domain),Z_STRVAL_PP(password),hv_gecos?Z_STRVAL_PP(gecos):Z_STRVAL_PP(username),hv_apop?Z_LVAL_PP(apop):0);
	if (retval!=VA_SUCCESS) {
        	php_error(E_WARNING, "vpopmail_adduser error: %s", verror(retval));
		RETURN_LONG(retval);
	}
	if (hv_quota)
		retval=vsetuserquota(Z_STRVAL_PP(username),Z_STRVAL_PP(domain),Z_STRVAL_PP(quota));
	if (retval != VA_SUCCESS)
        	php_error(E_WARNING, "vpopmail_adduser error: %s", verror(retval));
	RETURN_LONG(VA_SUCCESS);
}
/* }}} */

/* {{{ proto int vpopmail_deluser(string username, string domain)
   Delete a user from a virtual domain */
PHP_FUNCTION(vpopmail_deluser) {
	zval **user;
	zval **domain;
	int retval;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &user, &domain) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_string_ex(user);
	convert_to_string_ex(domain);

	retval = vdeluser(Z_STRVAL_PP(user), Z_STRVAL_PP(domain));

	if (retval != VA_SUCCESS)
        	php_error(E_WARNING, "vpopmail_deluser error: %s", verror(retval));
	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto int vpopmail_passwd(string username, string domain, string password[, int apop])
   Change a virtual user's password */
PHP_FUNCTION(vpopmail_passwd) {
	zval **username, **domain, **password, **apop;
	int argc,retval,is_apop=0;

	argc=ZEND_NUM_ARGS();
	if (argc < 3 || argc > 4 || zend_get_parameters_ex(argc, &username, &domain, &password, &apop) == FAILURE)
		WRONG_PARAM_COUNT;
	if (argc==4) {
		convert_to_long_ex(apop);
		is_apop=(Z_BVAL_PP(apop) ? 1 : 0);
	}

	convert_to_string_ex(username);
	convert_to_string_ex(domain);
	convert_to_string_ex(password);

	retval=vpasswd(Z_STRVAL_PP(username),Z_STRVAL_PP(domain),Z_STRVAL_PP(password),is_apop);
	if (retval != VA_SUCCESS)
	        php_error(E_WARNING, "vpopmail_passwd error: %s", verror(retval));
	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto int vpopmail_setuserquota(string username, string domain, string quota)
   Sets a virtual user's quota */
PHP_FUNCTION(vpopmail_setuserquota) {
	zval **user;
	zval **domain;
	zval **quota;
	int retval;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &user, &domain, &quota) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_string_ex(user);
	convert_to_string_ex(domain);
	convert_to_string_ex(quota);

	retval = vsetuserquota(Z_STRVAL_PP(user), Z_STRVAL_PP(domain), Z_STRVAL_PP(quota));
	if (retval != VA_SUCCESS)
	        php_error(E_WARNING, "vpopmail_setuserquota error: %s", verror(retval));
	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto bool vpopmail_auth_user(string user, string domain, string password [, int apop])
   Attempt to validate a username/domain/password. Returns true/false */
PHP_FUNCTION(vpopmail_auth_user) {
	zval **user;
	zval **domain;
	zval **password;
	zval **apop;
	struct passwd *retval;
	int argc=ZEND_NUM_ARGS();
	int hv_apop=0;

	if (argc < 3 || argc > 4 || zend_get_parameters_ex(argc, &user, &domain, &password, &apop) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_string_ex(user);
	convert_to_string_ex(domain);
	convert_to_string_ex(password);

	if (argc==4) {
		convert_to_string_ex(apop);
		hv_apop=1;
	}

	// warning: retval is a ref to a static member; this is safe - will not leak
	retval = vauth_user(Z_STRVAL_PP(user), Z_STRVAL_PP(domain), Z_STRVAL_PP(password), hv_apop?Z_STRVAL_PP(apop):"");

	if (retval == NULL) {
		RETURN_FALSE;
	} else {
		RETURN_TRUE;
	}
}
/* }}} */


#endif	/* HAVE_VPOPMAIL */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
