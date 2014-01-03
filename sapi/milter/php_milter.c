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
   | Author: Harald Radi <phanto@php.net>                                 |
   |         Parts based on CGI SAPI Module by                            |
   |         Rasmus Lerdorf, Stig Bakken and Zeev Suraski                 |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "php.h"
#include "php_globals.h"
#include "php_variables.h"
#include "zend_modules.h"

#ifndef ZTS
#error SRM sapi module is only useable in thread-safe mode
#endif

#include "SAPI.h"

#include <stdio.h>
#include "php.h"
#if HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#if HAVE_SIGNAL_H
#include <signal.h>
#endif
#if HAVE_SETLOCALE
#include <locale.h>
#endif
#include "zend.h"
#include "zend_extensions.h"
#include "php_ini.h"
#include "php_globals.h"
#include "php_main.h"
#include "fopen_wrappers.h"
#include "ext/standard/php_standard.h"

#ifdef __riscos__
#include <unixlib/local.h>
#endif

#include "zend_compile.h"
#include "zend_execute.h"
#include "zend_highlight.h"
#include "zend_indent.h"

#include "libmilter/mfapi.h"

#include "php_getopt.h"

#define OPTSTRING "ac:d:Def:hnp:vVz:?"
#define MG(v)  TSRMG(milter_globals_id, zend_milter_globals *, v)

#define IS_NONE "%s(): This function must not be called outside of a milter callback function's scope"
#define NOT_EOM "%s(): This function can only be used inside the milter_eom callback's scope"
#define NOT_INIT "%s(): This function can only be used inside the milter_init callback's scope"

#define MLFI_NONE		0
#define MLFI_CONNECT	1
#define MLFI_HELO		2
#define MLFI_ENVFROM	3
#define MLFI_ENVRCPT	4
#define MLFI_HEADER		5
#define MLFI_EOH		6
#define MLFI_BODY		7
#define MLFI_EOM		8
#define MLFI_ABORT		9
#define MLFI_CLOSE		10
#define MLFI_INIT		11

/* {{{ globals
 */
extern char *ap_php_optarg;
extern int ap_php_optind;

static int flag_debug=0;
static char *filename = NULL;

/* per thread */
ZEND_BEGIN_MODULE_GLOBALS(milter)
	SMFICTX *ctx;
	int state;
	int initialized;
ZEND_END_MODULE_GLOBALS(milter)

ZEND_DECLARE_MODULE_GLOBALS(milter)
/* }}} */

/* this method is called only once when the milter starts */
/* {{{ Init Milter
*/
static int mlfi_init()
{
	int ret = 0;
	zend_file_handle file_handle;
	zval function_name, retval;
	int status;
	TSRMLS_FETCH();

	/* request startup */
	if (php_request_startup(TSRMLS_C)==FAILURE) {
		SG(headers_sent) = 1;
		SG(request_info).no_headers = 1;
		php_request_shutdown((void *) 0);

		return -1;
	}
	
	/* disable headers */
	SG(headers_sent) = 1;
	SG(request_info).no_headers = 1;
	 
	if (filename == NULL) {
		php_printf("No input file specified");
		return SMFIS_TEMPFAIL;
	}

	if (!(file_handle.handle.fp = VCWD_FOPEN(filename, "rb"))) {
		php_printf("Could not open input file: %s\n", filename);
		return SMFIS_TEMPFAIL;
	}

	file_handle.type = ZEND_HANDLE_FP;
	file_handle.filename = filename;
	file_handle.free_filename = 0;
	file_handle.opened_path = NULL;

	php_execute_script(&file_handle TSRMLS_CC);
	
	/* call userland */
	INIT_ZVAL(function_name);

	ZVAL_STRING(&function_name, "milter_init", 0);

	/* set the milter context for possible use in API functions */
	MG(state) = MLFI_INIT;

	status = call_user_function(CG(function_table), NULL, &function_name, &retval, 0, NULL TSRMLS_CC);

	MG(state) = MLFI_NONE;
	MG(initialized) = 1;

	if (status == SUCCESS && Z_TYPE(retval) == IS_LONG) {
		ret = Z_LVAL(retval);
	}
	
	php_request_shutdown((void *) 0);
	
	return ret;
}
/* }}} */

/* {{{ Milter callback functions
 */

/* connection info filter, is called whenever sendmail connects to the milter */
/* {{{ mlfi_connect()
*/
static sfsistat	mlfi_connect(SMFICTX *ctx, char *hostname, _SOCK_ADDR *hostaddr)
{
	zend_file_handle file_handle;
	zval function_name, retval, *param[1];
	int status;
	TSRMLS_FETCH();

	/* request startup */
	if (php_request_startup(TSRMLS_C)==FAILURE) {
		SG(headers_sent) = 1;
		SG(request_info).no_headers = 1;
		php_request_shutdown((void *) 0);

		return SMFIS_TEMPFAIL;
	}
	
	/* disable headers */
	SG(headers_sent) = 1;
	SG(request_info).no_headers = 1;
	
	if (filename == NULL) {
		php_printf("No input file specified");
		return SMFIS_TEMPFAIL;
	}
	
	if (!(file_handle.handle.fp = VCWD_FOPEN(filename, "rb"))) {
		php_printf("Could not open input file: %s\n", filename);
		return SMFIS_TEMPFAIL;
	}

	file_handle.type = ZEND_HANDLE_FP;
	file_handle.filename = filename;
	file_handle.free_filename = 0;
	file_handle.opened_path = NULL;

	php_execute_script(&file_handle TSRMLS_CC);
	
	/* call userland */
	INIT_ZVAL(function_name);

	ALLOC_ZVAL(param[0]);
	INIT_PZVAL(param[0]);

	ZVAL_STRING(&function_name, "milter_connect", 0);
	ZVAL_STRING(param[0], hostname, 1);

	/* set the milter context for possible use in API functions */
	MG(ctx) = ctx;
	MG(state) = MLFI_CONNECT;

	status = call_user_function(CG(function_table), NULL, &function_name, &retval, 1, param TSRMLS_CC);

	MG(state) = MLFI_NONE;
	zval_ptr_dtor(param);
	if (status == SUCCESS && Z_TYPE(retval) == IS_LONG) {
		return Z_LVAL(retval);
	}
	
	return SMFIS_CONTINUE;
}
/* }}} */

/* SMTP HELO command filter */
/* {{{ mlfi_helo()
*/
static sfsistat mlfi_helo(SMFICTX *ctx, char *helohost)
{
	zval function_name, retval, *param[1];
	int status;
	TSRMLS_FETCH();

	/* call userland */
	INIT_ZVAL(function_name);
	
	ALLOC_ZVAL(param[0]);
	INIT_PZVAL(param[0]);

	ZVAL_STRING(&function_name, "milter_helo", 0);
	ZVAL_STRING(param[0], helohost, 1);

	/* set the milter context for possible use in API functions */
	MG(ctx) = ctx;
	MG(state) = MLFI_HELO;
	
	status = call_user_function(CG(function_table), NULL, &function_name, &retval, 1, param TSRMLS_CC);

	MG(state) = MLFI_NONE;
	zval_ptr_dtor(param);
	
	if (status == SUCCESS && Z_TYPE(retval) == IS_LONG) {
		return Z_LVAL(retval);
	}
	
	return SMFIS_CONTINUE;
}
/* }}} */

/* envelope sender filter */
/* {{{ mlfi_envform()
*/
static sfsistat mlfi_envfrom(SMFICTX *ctx, char **argv)
{
	zval function_name, retval, *param[1];
	int status;
	TSRMLS_FETCH();

	/* call userland */
	INIT_ZVAL(function_name);
	
	ALLOC_ZVAL(param[0]);
	INIT_PZVAL(param[0]);

	ZVAL_STRING(&function_name, "milter_envfrom", 0);
	array_init(param[0]);

	while (*argv) {
		add_next_index_string(param[0], *argv, 1);
		argv++;
	}

	/* set the milter context for possible use in API functions */
	MG(ctx) = ctx;
	MG(state) = MLFI_ENVFROM;
	
	status = call_user_function(CG(function_table), NULL, &function_name, &retval, 1, param TSRMLS_CC);

	MG(state) = MLFI_NONE;
	zval_ptr_dtor(param);
	
	if (status == SUCCESS && Z_TYPE(retval) == IS_LONG) {
		return Z_LVAL(retval);
	}

	return SMFIS_CONTINUE;
}
/* }}} */

/* envelope recipient filter */
/* {{{ mlfi_envrcpt()
*/
static sfsistat mlfi_envrcpt(SMFICTX *ctx, char **argv)
{
	zval function_name, retval, *param[1];
	int status;
	TSRMLS_FETCH();

	/* call userland */
	INIT_ZVAL(function_name);
	
	ALLOC_ZVAL(param[0]);
	INIT_PZVAL(param[0]);

	ZVAL_STRING(&function_name, "milter_envrcpt", 0);
	array_init(param[0]);

	while (*argv) {
		add_next_index_string(param[0], *argv, 1);
		argv++;
	}

	/* set the milter context for possible use in API functions */
	MG(ctx) = ctx;
	MG(state) = MLFI_ENVRCPT;
	
	status = call_user_function(CG(function_table), NULL, &function_name, &retval, 1, param TSRMLS_CC);

	MG(state) = MLFI_NONE;
	
	zval_ptr_dtor(param);
	
	if (status == SUCCESS && Z_TYPE(retval) == IS_LONG) {
		return Z_LVAL(retval);
	}

	return SMFIS_CONTINUE;
}
/* }}} */

/* header filter */
/* {{{ mlfi_header()
*/
static sfsistat mlfi_header(SMFICTX *ctx, char *headerf, char *headerv)
{
	zval function_name, retval, *param[2];
	int status;
	TSRMLS_FETCH();

	/* call userland */
	INIT_ZVAL(function_name);
	
	ALLOC_ZVAL(param[0]);
	ALLOC_ZVAL(param[1]);
	INIT_PZVAL(param[0]);
	INIT_PZVAL(param[1]);
	
	ZVAL_STRING(&function_name, "milter_header", 0);
	ZVAL_STRING(param[0], headerf, 1);
	ZVAL_STRING(param[1], headerv, 1);

	/* set the milter context for possible use in API functions */
	MG(ctx) = ctx;
	MG(state) = MLFI_HEADER;
	
	status = call_user_function(CG(function_table), NULL, &function_name, &retval, 2, param TSRMLS_CC);

	MG(state) = MLFI_NONE;
	
	zval_ptr_dtor(&param[0]);
	zval_ptr_dtor(&param[1]);	
	
	if (status == SUCCESS && Z_TYPE(retval) == IS_LONG) {
		return Z_LVAL(retval);
	}
	
	return SMFIS_CONTINUE;
}
/* }}} */

/* end of header */
/* {{{ mlfi_eoh()
*/
static sfsistat mlfi_eoh(SMFICTX *ctx)
{
	zval function_name, retval;
	int status;
	TSRMLS_FETCH();

	/* call userland */
	INIT_ZVAL(function_name);
	ZVAL_STRING(&function_name, "milter_eoh", 0);

	/* set the milter context for possible use in API functions */
	MG(ctx) = ctx;
	MG(state) = MLFI_EOH;
	
	status = call_user_function(CG(function_table), NULL, &function_name, &retval, 0, NULL TSRMLS_CC);

	MG(state) = MLFI_NONE;
	
	if (status == SUCCESS && Z_TYPE(retval) == IS_LONG) {
		return Z_LVAL(retval);
	}

	return SMFIS_CONTINUE;
}
/* }}} */

/* body block */
/* {{{ mlfi_body()
*/
static sfsistat mlfi_body(SMFICTX *ctx, u_char *bodyp, size_t len)
{
	zval function_name, retval, *param[1];
	int status;
	TSRMLS_FETCH();

	/* call userland */
	INIT_ZVAL(function_name);
	
	ALLOC_ZVAL(param[0]);
	INIT_PZVAL(param[0]);

	ZVAL_STRING(&function_name, "milter_body", 0);
	ZVAL_STRINGL(param[0], (char*)bodyp, len, 1); /*alex*/
	
	/* set the milter context for possible use in API functions */
	MG(ctx) = ctx;
	MG(state) = MLFI_BODY;
	
	status = call_user_function(CG(function_table), NULL, &function_name, &retval, 1, param TSRMLS_CC);

	MG(state) = MLFI_NONE;
	
	zval_ptr_dtor(param);	

	if (status == SUCCESS && Z_TYPE(retval) == IS_LONG) {
		return Z_LVAL(retval);
	}

	return SMFIS_CONTINUE;
}
/* }}} */

/* end of message */
/* {{{ mlfi_eom()
*/
static sfsistat mlfi_eom(SMFICTX *ctx)
{
	zval function_name, retval;
	int status;
	TSRMLS_FETCH();

	/* call userland */
	INIT_ZVAL(function_name);
	ZVAL_STRING(&function_name, "milter_eom", 0);
	
	/* set the milter context for possible use in API functions */
	MG(ctx) = ctx;
	MG(state) = MLFI_EOM;

	status = call_user_function(CG(function_table), NULL, &function_name, &retval, 0, NULL TSRMLS_CC);

	MG(state) = MLFI_NONE;
	
	if (status == SUCCESS && Z_TYPE(retval) == IS_LONG) {
		return Z_LVAL(retval);
	}

	return SMFIS_CONTINUE;
}
/* }}} */

/* message aborted */
/* {{{ mlfi_abort()
*/
static sfsistat mlfi_abort(SMFICTX *ctx)
{
	zval function_name, retval;
	int status;
	TSRMLS_FETCH();

	/* call userland */
	INIT_ZVAL(function_name);
	ZVAL_STRING(&function_name, "milter_abort", 0);
	
	/* set the milter context for possible use in API functions */
	MG(ctx) = ctx;
	MG(state) = MLFI_ABORT;
	
	status = call_user_function(CG(function_table), NULL, &function_name, &retval, 0, NULL TSRMLS_CC);

	MG(state) = MLFI_NONE;
	
	if (status == SUCCESS && Z_TYPE(retval) == IS_LONG) {
		return Z_LVAL(retval);
	}

	return SMFIS_CONTINUE;
}
/* }}} */

/* connection cleanup */
/* {{{ mlfi_close()
*/
static sfsistat mlfi_close(SMFICTX *ctx)
{
	int ret = SMFIS_CONTINUE;
	zval function_name, retval;
	int status;
	TSRMLS_FETCH();

	/* call userland */
	INIT_ZVAL(function_name);
	ZVAL_STRING(&function_name, "milter_close", 0);
	
	/* set the milter context for possible use in API functions */
	MG(ctx) = ctx;
	MG(state) = MLFI_CLOSE;
	
	status = call_user_function(CG(function_table), NULL, &function_name, &retval, 0, NULL TSRMLS_CC);

	MG(state) = MLFI_NONE;
	
	if (status == SUCCESS && Z_TYPE(retval) == IS_LONG) {
		ret = Z_LVAL(retval);
	}
	
	php_request_shutdown((void *) 0);

	return ret;
}
/* }}} */
/* }}} */

/* {{{ Milter entry struct
 */
struct smfiDesc smfilter = {
    "php-milter",	/* filter name */
    SMFI_VERSION,   /* version code -- leave untouched */
    0,				/* flags */
    mlfi_connect,	/* info filter callback */
    mlfi_helo,		/* HELO filter callback */
    mlfi_envfrom,	/* envelope filter callback */
    mlfi_envrcpt,	/* envelope recipient filter callback */
    mlfi_header,	/* header filter callback */
    mlfi_eoh,		/* end of header callback */
    mlfi_body,		/* body filter callback */
    mlfi_eom,		/* end of message callback */
    mlfi_abort,		/* message aborted callback */
    mlfi_close,		/* connection cleanup callback */
};
/* }}} */

/* {{{ PHP Milter API
 */

/* {{{ proto void smfi_setflags(long flags)
   Sets the flags describing the actions the filter may take. */	
PHP_FUNCTION(smfi_setflags)
{
	long flags;
	
	/* valid only in the init callback */
	if (MG(state) != MLFI_INIT) {
		php_error(E_WARNING, NOT_INIT, get_active_function_name(TSRMLS_C));
	} else if (zend_parse_parameters(1 TSRMLS_CC, "l", &flags) == SUCCESS) {
		flags = flags & (SMFIF_ADDHDRS|SMFIF_CHGHDRS|SMFIF_CHGBODY|SMFIF_ADDRCPT|SMFIF_DELRCPT);
		smfilter.xxfi_flags = flags;
	}	
}
/* }}} */

/* {{{ proto void smfi_settimeout(long timeout)
   Sets the number of seconds libmilter will wait for an MTA connection before timing out a socket. */	
PHP_FUNCTION(smfi_settimeout)
{
	long timeout;
	
	/* valid only in the init callback */
	if (MG(state) != MLFI_INIT) {
		php_error(E_WARNING, NOT_INIT, get_active_function_name(TSRMLS_C));
	} else if (zend_parse_parameters(1 TSRMLS_CC, "l", &timeout) == SUCCESS) {
		smfi_settimeout(timeout);
	}	
}
/* }}} */

/* {{{ proto string smfi_getsymval(string macro)
   Returns the value of the given macro or NULL if the macro is not defined. */	
PHP_FUNCTION(smfi_getsymval)
{
	char *symname, *ret;
	int len;

	/* valid in any callback */
	if (MG(state) == MLFI_NONE) {
		php_error(E_WARNING, IS_NONE, get_active_function_name(TSRMLS_C));
	} else if (zend_parse_parameters(1 TSRMLS_CC, "s", &symname, &len) == SUCCESS) {
		if ((ret = smfi_getsymval(MG(ctx), symname)) != NULL) {
			RETURN_STRING(ret, 1);
		}
	}

	RETURN_NULL();
}
/* }}} */

/* {{{ proto bool smfi_setreply(string rcode, string xcode, string message)
   Directly set the SMTP error reply code for this connection.
   This code will be used on subsequent error replies resulting from actions taken by this filter. */	
PHP_FUNCTION(smfi_setreply)
{
	char *rcode, *xcode, *message;
	int len;
	
	/* valid in any callback */
	if (MG(state) == MLFI_NONE) {
		php_error(E_WARNING, IS_NONE, get_active_function_name(TSRMLS_C));
	} else if (zend_parse_parameters(3 TSRMLS_CC, "sss", &rcode, &len, &xcode, &len, &message, &len) == SUCCESS) {
		if (smfi_setreply(MG(ctx), rcode, xcode, message) == MI_SUCCESS) {
			RETURN_TRUE;
		}
	}
	
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool smfi_addheader(string headerf, string headerv)
   Adds a header to the current message. */	
PHP_FUNCTION(smfi_addheader)
{
	char *f, *v;
	int len;
	
	/* valid only in milter_eom */
	if (MG(state) != MLFI_EOM) {
		php_error(E_WARNING, NOT_EOM, get_active_function_name(TSRMLS_C));
	} else if (zend_parse_parameters(2 TSRMLS_CC, "ss", &f, &len, &v, &len) == SUCCESS) {
		if (smfi_addheader(MG(ctx), f, v) == MI_SUCCESS) {
			RETURN_TRUE;
		}
	}
	
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool smfi_chgheader(string headerf, string headerv)
   Changes a header's value for the current message. */	
PHP_FUNCTION(smfi_chgheader)
{
	char *f, *v;
	long idx;
	int len;
	
	/* valid only in milter_eom */
	if (MG(state) != MLFI_EOM) {
		php_error(E_WARNING, NOT_EOM, get_active_function_name(TSRMLS_C));
	} else if (zend_parse_parameters(3 TSRMLS_CC, "sls", &f, &len, &idx, &v, &len) == SUCCESS) {
		if (smfi_chgheader(MG(ctx), f, idx, v) == MI_SUCCESS) {
			RETURN_TRUE;
		}
	}
	
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool smfi_addrcpt(string rcpt)
   Add a recipient to the message envelope. */	
PHP_FUNCTION(smfi_addrcpt)
{
	char *rcpt;
	int len;
	
	/* valid only in milter_eom */
	if (MG(state) != MLFI_EOM) {
		php_error(E_WARNING, NOT_EOM, get_active_function_name(TSRMLS_C));
	} else if (zend_parse_parameters(1 TSRMLS_CC, "s", &rcpt, &len) == SUCCESS) {
		if (smfi_addrcpt(MG(ctx), rcpt) == MI_SUCCESS) {
			RETURN_TRUE;
		}
	}
	
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool smfi_delrcpt(string rcpt)
   Removes the named recipient from the current message's envelope. */	
PHP_FUNCTION(smfi_delrcpt)
{
	char *rcpt;
	int len;
	
	/* valid only in milter_eom */
	if (MG(state) != MLFI_EOM) {
		php_error(E_WARNING, NOT_EOM, get_active_function_name(TSRMLS_C));
	} else if (zend_parse_parameters(1 TSRMLS_CC, "s", &rcpt, &len) == SUCCESS) {
		if (smfi_delrcpt(MG(ctx), rcpt) == MI_SUCCESS) {
			RETURN_TRUE;
		}
	}
	
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool smfi_replacebody(string body)
   Replaces the body of the current message. If called more than once,
   subsequent calls result in data being appended to the new body. */	
PHP_FUNCTION(smfi_replacebody)
{
	char *body;
	int len;
	
	/* valid only in milter_eom */
	if (MG(state) != MLFI_EOM) {
		php_error(E_WARNING, NOT_EOM, get_active_function_name(TSRMLS_C));
	} else if (zend_parse_parameters(1 TSRMLS_CC, "s", &body, &len) == SUCCESS) {
		if (smfi_replacebody(MG(ctx), (u_char*)body, len) == MI_SUCCESS) {
			RETURN_TRUE;
		}
	}
	
	RETURN_FALSE;
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(milter)
{
	REGISTER_LONG_CONSTANT("SMFIS_CONTINUE",	SMFIS_CONTINUE,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SMFIS_REJECT",		SMFIS_REJECT,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SMFIS_DISCARD",		SMFIS_DISCARD,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SMFIS_ACCEPT",		SMFIS_ACCEPT,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SMFIS_TEMPFAIL",	SMFIS_TEMPFAIL,	CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("SMFIF_ADDHDRS",		SMFIF_ADDHDRS,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SMFIF_CHGHDRS",		SMFIF_CHGHDRS,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SMFIF_CHGBODY",		SMFIF_CHGBODY,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SMFIF_ADDRCPT",		SMFIF_ADDRCPT,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SMFIF_DELRCPT",		SMFIF_DELRCPT,	CONST_CS | CONST_PERSISTENT);

	ZEND_INIT_MODULE_GLOBALS(milter, NULL, NULL);

	MG(state) = MLFI_NONE;
	MG(initialized) = 0;
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(milter)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "Milter support", "enabled");
	php_info_print_table_end();
}
/* }}} */
/* }}} */

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_smfi_setflags, 0, 0, 1)
	ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_smfi_settimeout, 0, 0, 1)
	ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_smfi_getsymval, 0, 0, 1)
	ZEND_ARG_INFO(0, macro)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_smfi_setreply, 0, 0, 3)
	ZEND_ARG_INFO(0, rcode)
	ZEND_ARG_INFO(0, xcode)
	ZEND_ARG_INFO(0, message)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_smfi_addheader, 0, 0, 2)
	ZEND_ARG_INFO(0, headerf)
	ZEND_ARG_INFO(0, headerv)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_smfi_chgheader, 0, 0, 2)
	ZEND_ARG_INFO(0, headerf)
	ZEND_ARG_INFO(0, headerv)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_smfi_addrcpt, 0, 0, 1)
	ZEND_ARG_INFO(0, rcpt)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_smfi_delrcpt, 0, 0, 1)
	ZEND_ARG_INFO(0, rcpt)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_smfi_replacebody, 0, 0, 1)
	ZEND_ARG_INFO(0, body)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ milter_functions[]
*/
const static zend_function_entry milter_functions[] = {
	PHP_FE(smfi_setflags, 		arginfo_smfi_setflags)
	PHP_FE(smfi_settimeout, 	arginfo_smfi_settimeout)
	PHP_FE(smfi_getsymval, 		arginfo_smfi_getsymval)
	PHP_FE(smfi_setreply, 		arginfo_smfi_setreply)
	PHP_FE(smfi_addheader, 		arginfo_smfi_addheader)
	PHP_FE(smfi_chgheader, 		arginfo_smfi_chgheader)
	PHP_FE(smfi_addrcpt, 		arginfo_smfi_addrcpt)
	PHP_FE(smfi_delrcpt, 		arginfo_smfi_delrcpt)
	PHP_FE(smfi_replacebody, 	arginfo_smfi_replacebody)
	PHP_FE_END
};
/* }}} */

/* {{{ Zend module entry
*/
static zend_module_entry php_milter_module = {
	STANDARD_MODULE_HEADER,
	"Milter",
	milter_functions,
	PHP_MINIT(milter),
	NULL,
	NULL,
	NULL,
	PHP_MINFO(milter),
	"0.1.0",
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

/* {{{ Milter SAPI
*/
static int sapi_milter_ub_write(const char *str, uint str_length TSRMLS_DC)
{
	return str_length;
}

static void sapi_milter_flush(void *server_context)
{
}

static void sapi_milter_register_variables(zval *track_vars_array TSRMLS_DC)
{
	php_register_variable ("SERVER_SOFTWARE", "Sendmail Milter", track_vars_array TSRMLS_CC);
}

static int sapi_milter_post_read(char *buf, uint count_bytes TSRMLS_DC)
{
	return 0;
}

static char* sapi_milter_read_cookies(TSRMLS_D)
{
	return NULL;
}

static int sapi_milter_send_headers(sapi_headers_struct *sapi_headers TSRMLS_DC)
{
	return SAPI_HEADER_SENT_SUCCESSFULLY;
}

static int php_milter_startup(sapi_module_struct *sapi_module)
{
	if (php_module_startup(sapi_module, &php_milter_module, 1) == FAILURE) {
		return FAILURE;
	}
	return SUCCESS;
}
/* }}} */

/* {{{ sapi_module_struct milter_sapi_module
*/
static sapi_module_struct milter_sapi_module = {
	"milter",						/* name */
	"Sendmail Milter SAPI",			/* pretty name */

	php_milter_startup,				/* startup */
	php_module_shutdown_wrapper,	/* shutdown */

	NULL,							/* activate */
	NULL,							/* deactivate */

	sapi_milter_ub_write,			/* unbuffered write */
	sapi_milter_flush,				/* flush */
	NULL,							/* get uid */
	NULL,							/* getenv */

	php_error,						/* error handler */

	NULL,							/* header handler */
	sapi_milter_send_headers,		/* send headers handler */
	NULL,							/* send header handler */

	sapi_milter_post_read,			/* read POST data */
	sapi_milter_read_cookies,		/* read Cookies */

	sapi_milter_register_variables,	/* register server variables */
	NULL,							/* Log message */
	NULL,							/* Get request time */
	NULL,							/* Child terminate */

	NULL,							/* Block interruptions */
	NULL,							/* Unblock interruptions */

	STANDARD_SAPI_MODULE_PROPERTIES
};
/* }}} */

/****
* ripped from cli, has to be cleaned up !
*/

/* {{{ php_milter_usage
*/
static void php_milter_usage(char *argv0)
{
	char *prog;

	prog = strrchr(argv0, '/');
	if (prog) {
		prog++;
	} else {
		prog = "php-milter";
	}

	printf(     "Usage: %s [options] [-f] <file> [args...]\n"
	            "       %s [options] [-- args...]\n"
				"  -a               Run interactively\n"
				"  -c <path>|<file> Look for php.ini file in this directory\n"
				"  -n               No php.ini file will be used\n"
				"  -d foo[=bar]     Define INI entry foo with value 'bar'\n"
				"  -D               run as daemon\n"
				"  -e               Generate extended information for debugger/profiler\n"
				"  -f <file>        Parse <file>.\n"
				"  -h               This help\n"
				"  -p <socket>      path to create socket\n"
				"  -v               Version number\n"
				"  -V <n>           set debug level to n (1 or 2).\n"
				"  -z <file>        Load Zend extension <file>.\n"
				"  args...          Arguments passed to script. Use -- args when first argument \n"
				"                   starts with - or script is read from stdin\n"
				, prog, prog);
}
/* }}} */

static void define_command_line_ini_entry(char *arg) /* {{{ */
{
	char *name, *value;

	name = arg;
	value = strchr(arg, '=');
	if (value) {
		*value = 0;
		value++;
	} else {
		value = "1";
	}
	zend_alter_ini_entry(name, strlen(name)+1, value, strlen(value), PHP_INI_SYSTEM, PHP_INI_STAGE_ACTIVATE);
}
/* }}} */

/* {{{ main
*/
int main(int argc, char *argv[])
{
    char *sock = NULL;
	int dofork = 0;

	int exit_status = SUCCESS;
	int c;
/* temporary locals */
	int orig_optind=ap_php_optind;
	char *orig_optarg=ap_php_optarg;
	int interactive=0;
	char *param_error=NULL;
/* end of temporary locals */

	void ***tsrm_ls;

#ifdef HAVE_SIGNAL_H
#if defined(SIGPIPE) && defined(SIG_IGN)
	signal(SIGPIPE, SIG_IGN); /* ignore SIGPIPE in standalone mode so
								that sockets created via fsockopen()
								don't kill PHP if the remote site
								closes it.  in apache|apxs mode apache
								does that for us!  thies@thieso.net
								20000419 */
#endif
#endif


	tsrm_startup(1, 1, 0, NULL);
	sapi_startup(&milter_sapi_module);
	
	while ((c=ap_php_getopt(argc, argv, OPTSTRING))!=-1) {
		switch (c) {
		case 'c':
			milter_sapi_module.php_ini_path_override = strdup(ap_php_optarg);
			break;
		case 'n':
			milter_sapi_module.php_ini_ignore = 1;
			break;
		}
	}
	ap_php_optind = orig_optind;
	ap_php_optarg = orig_optarg;

	milter_sapi_module.executable_location = argv[0];

	tsrm_ls = ts_resource(0);

	sapi_module.startup(&milter_sapi_module);

	zend_first_try {
		while ((c=ap_php_getopt(argc, argv, OPTSTRING))!=-1) {
			switch (c) {
			case '?':
				php_output_tearup();
				SG(headers_sent) = 1;
				php_milter_usage(argv[0]);
				php_output_teardown();
				exit(1);
				break;
			}
		}
		ap_php_optind = orig_optind;
		ap_php_optarg = orig_optarg;

        /* Set some CLI defaults */
		SG(options) |= SAPI_OPTION_NO_CHDIR;
		zend_alter_ini_entry("html_errors", 12, "0", 1, PHP_INI_SYSTEM, PHP_INI_STAGE_ACTIVATE);
		zend_alter_ini_entry("max_execution_time", 19, "0", 1, PHP_INI_SYSTEM, PHP_INI_STAGE_ACTIVATE);

		zend_uv.html_errors = 0; /* tell the engine we're in non-html mode */

		while ((c = ap_php_getopt(argc, argv, OPTSTRING)) != -1) {
			switch (c) {

			case 'a':	/* interactive mode */
				printf("Interactive mode enabled\n\n");
				interactive=1;
				break;

			case 'C': /* don't chdir to the script directory */
				/* This is default so NOP */
				break;
			case 'd': /* define ini entries on command line */
				define_command_line_ini_entry(ap_php_optarg);
				break;

			case 'D': /* daemon */
				dofork = 1;
				break;

			case 'e': /* enable extended info output */
				CG(compiler_options) |= ZEND_COMPILE_EXTENDED_INFO;
				break;

			case 'f': /* parse file */
				filename = ap_php_optarg;
				break;

			case 'h': /* help & quit */
			case '?':
				php_output_tearup();
				SG(headers_sent) = 1;
				php_milter_usage(argv[0]);
				php_output_teardown();
				exit(1);
				break;

			case 'p': /* socket */
				sock = strdup(ap_php_optarg);
				break;

			case 'v': /* show php version & quit */
				if (php_request_startup(TSRMLS_C)==FAILURE) {
					zend_ini_deactivate(TSRMLS_C);
					php_module_shutdown(TSRMLS_C);
					sapi_shutdown();
					tsrm_shutdown();

					exit(1);
				}
				SG(headers_sent) = 1;
				SG(request_info).no_headers = 1;
				php_printf("PHP %s (%s) (built: %s %s)\nCopyright (c) 1997-2014 The PHP Group\n%s", PHP_VERSION, sapi_module.name, __DATE__, __TIME__, get_zend_version());
				php_output_teardown();
				exit(1);
				break;

			case 'V': /* verbose */
				flag_debug = atoi(ap_php_optarg);
				break;

			case 'z': /* load extension file */
				zend_load_extension(ap_php_optarg);
				break;

			default:
				break;
			}
		}

		if (param_error) {
			SG(headers_sent) = 1;
			SG(request_info).no_headers = 1;
			PUTS(param_error);
			exit(1);
		}

		CG(interactive) = interactive;

		/* only set script_file if not set already and not in direct mode and not at end of parameter list */
		if (argc > ap_php_optind && !filename) {
			filename=argv[ap_php_optind];
			ap_php_optind++;
		}

		/* check if file exists, exit else */
		
		if (dofork) {
			switch(fork()) {
				case -1: /* Uh-oh, we have a problem forking. */
					fprintf(stderr, "Uh-oh, couldn't fork!\n");
					exit(errno);
					break;
				case 0: /* Child */
					break;
				default: /* Parent */
					exit(0);
			}
		}
			
		if (sock) {
			struct stat junk;
			if (stat(sock,&junk) == 0) unlink(sock);
		}

		openlog("php-milter", LOG_PID, LOG_MAIL);
		
		if ((exit_status = mlfi_init())) {
			syslog(1, "mlfi_init failed.");
			closelog();
			goto err;
		}

		smfi_setconn(sock);
		if (smfi_register(smfilter) == MI_FAILURE) {
			syslog(1, "smfi_register failed.");
			fprintf(stderr, "smfi_register failed\n");
			closelog();
			goto err;
		} else {
			exit_status = smfi_main();
		}			

		closelog();

		if (milter_sapi_module.php_ini_path_override) {
			free(milter_sapi_module.php_ini_path_override);
		}

	} zend_catch {
		exit_status = EG(exit_status);
	} zend_end_try();

err:
	php_module_shutdown(TSRMLS_C);
	sapi_shutdown();
	tsrm_shutdown();

	exit(exit_status);
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
