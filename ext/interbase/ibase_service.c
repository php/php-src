/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2004 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_0.txt.                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Ard Biesheuvel <a.k.biesheuvel@its.tudelft.nl>              |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#if HAVE_IBASE6_API

#include "php_interbase.h"
#include "interbase.h"

static void _php_ibase_user(INTERNAL_FUNCTION_PARAMETERS, int operation) /* {{{ */
{
	zval **args[8];
	char *ib_server, *dba_user_name, *dba_password, *user_name, *user_password = NULL;
	char *first_name = NULL, *middle_name = NULL, *last_name = NULL;
	char service_name_buffer[128], *service_name = service_name_buffer;
	char spb_buffer[128], *spb = spb_buffer;
	unsigned short spb_length;
	isc_svc_handle service_handle = NULL;
	
	RESET_ERRMSG;

	switch (operation) {
		case isc_action_svc_add_user:
		case isc_action_svc_modify_user:
			/* 5 to 8 parameters for ADD or MODIFY operation */
			if (ZEND_NUM_ARGS() < 5 || ZEND_NUM_ARGS() > 8) {
				WRONG_PARAM_COUNT;
			}
			break;
	
		case isc_action_svc_delete_user:
			/* 4 parameters for DELETE operation */
			if (ZEND_NUM_ARGS() != 4) {
				WRONG_PARAM_COUNT;
			}
	}

	if (zend_get_parameters_array_ex(ZEND_NUM_ARGS(), args) == FAILURE) {
		RETURN_FALSE;
	}

	switch (ZEND_NUM_ARGS()) {
		case 8:
			convert_to_string_ex(args[7]);
			last_name = Z_STRVAL_PP(args[7]);
			/* fallout */
		case 7:
			convert_to_string_ex(args[6]);
			middle_name = Z_STRVAL_PP(args[6]);
			/* fallout */
		case 6:
			convert_to_string_ex(args[5]);
			first_name = Z_STRVAL_PP(args[5]);
			/* fallout */
	}

	if (operation != isc_action_svc_delete_user) {
		/* Parameter not available for DELETE operation */
		convert_to_string_ex(args[4]);
		user_password = Z_STRVAL_PP(args[4]);
	}

	convert_to_string_ex(args[3]);
	user_name = Z_STRVAL_PP(args[3]);

	convert_to_string_ex(args[2]);
	dba_password = Z_STRVAL_PP(args[2]);

	convert_to_string_ex(args[1]);
	dba_user_name = Z_STRVAL_PP(args[1]);

	convert_to_string_ex(args[0]);
	ib_server = Z_STRVAL_PP(args[0]);

	/* Build buffer for isc_service_attach() */
	*spb++ = isc_spb_version;
	*spb++ = isc_spb_current_version;
	*spb++ = isc_spb_user_name;
	*spb++ = strlen(dba_user_name);
	strcpy(spb, dba_user_name);
	spb += strlen(dba_user_name);
	*spb++ = isc_spb_password;
	*spb++ = strlen(dba_password);
	strcpy(spb, dba_password);
	spb += strlen(dba_password);
	spb_length = spb - spb_buffer;

	/* Attach to the Service Manager */
	sprintf(service_name, "%s:service_mgr", ib_server);
	if (isc_service_attach(IB_STATUS, 0, service_name, &service_handle, spb_length, spb_buffer)) {
		_php_ibase_error(TSRMLS_C);
		RETURN_FALSE;
	} else {
		char request[128], *x, *p = request;

		/* Identify cluster (here, isc_action_svc_*_user) */
		*p++ = operation;

		/* Argument for username */
		*p++ = isc_spb_sec_username;
		ADD_SPB_LENGTH(p, strlen(user_name));
		for (x = user_name ; *x;) *p++ = *x++;

		/* Argument for password */
		if (user_password) {
			*p++ = isc_spb_sec_password;
			ADD_SPB_LENGTH(p, strlen(user_password));
			for (x = user_password ; *x;) *p++ = *x++;
		}

		/* Argument for first name */
		if (first_name) {
			*p++ = isc_spb_sec_firstname;
			ADD_SPB_LENGTH(p, strlen(first_name));
			for (x = first_name ; *x;) *p++ = *x++;
		}

		/* Argument for middle name */
		if (middle_name) {
			*p++ = isc_spb_sec_middlename;
			ADD_SPB_LENGTH(p, strlen(middle_name));
			for (x = middle_name ; *x;) *p++ = *x++;
		}

		/* Argument for last name */
		if (last_name) {
			*p++ = isc_spb_sec_lastname;
			ADD_SPB_LENGTH(p, strlen(last_name));
			for (x = last_name ; *x;) *p++ = *x++;
		}

		/* Let's go update: start Service Manager */
		if (isc_service_start(IB_STATUS, &service_handle, NULL, (unsigned short) (p - request), request)) {
			_php_ibase_error(TSRMLS_C);
			isc_service_detach(IB_STATUS, &service_handle);
			RETURN_FALSE;
		} else {
			/* Detach from Service Manager */
			isc_service_detach(IB_STATUS, &service_handle);
		}
	}
	
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool ibase_add_user(string server, string dba_user_name, string dba_password, string user_name, string password [, string first_name [, string middle_name [, string last_name]]])
   Add an user to security database (only for IB6 or later) */
PHP_FUNCTION(ibase_add_user)
{
	_php_ibase_user(INTERNAL_FUNCTION_PARAM_PASSTHRU, isc_action_svc_add_user);
}
/* }}} */

/* {{{ proto bool ibase_modify_user(string server, string dba_user_name, string dba_password, string user_name, string password [, string first_name [, string middle_name [, string last_name]]])
   Modify an user in security database (only for IB6 or later) */
PHP_FUNCTION(ibase_modify_user)
{
	_php_ibase_user(INTERNAL_FUNCTION_PARAM_PASSTHRU, isc_action_svc_modify_user);
}
/* }}} */

/* {{{ proto bool ibase_delete_user(string server, string dba_user_name, string dba_password, string username)
   Delete an user from security database (only for IB6 or later) */
PHP_FUNCTION(ibase_delete_user)
{
	_php_ibase_user(INTERNAL_FUNCTION_PARAM_PASSTHRU, isc_action_svc_delete_user);
}
/* }}} */

/* {{{ proto resource ibase_service_attach(string host, string dba_username, string dba_password) 
   Connect to the service manager */
PHP_FUNCTION(ibase_service_attach)
{
	long hlen, ulen, plen, spb_len;
	ibase_service *svm;
	char buf[128], *host, *user, *pass, *loc;
	isc_svc_handle handle = NULL;

	RESET_ERRMSG;
	
	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sss", 
			&host, &hlen, &user, &ulen, &pass, &plen)) {
		
		RETURN_FALSE;
	}

	/* construct the spb, hack the service name into it as well */
	spb_len = snprintf(buf, sizeof(buf), "%c%c%c%c%s%c%c%s" "%s:service_mgr", 
		isc_spb_version, isc_spb_current_version, isc_spb_user_name, (char)ulen,
		user, isc_spb_password, (char)plen, pass, host);

	if (spb_len > sizeof(buf) || spb_len == -1) {
		_php_ibase_module_error("Internal error: insufficient buffer space for SPB (%ld)" 
			TSRMLS_CC, spb_len);
		RETURN_FALSE;
	}
	
	spb_len -= hlen + 12;
	loc = buf + spb_len; /* points to %s:service_mgr part */
	
	/* attach to the service manager */
	if (isc_service_attach(IB_STATUS, 0, loc, &handle, (unsigned short)spb_len, buf)) {
		_php_ibase_error(TSRMLS_C);
		RETURN_FALSE;
	}
	
	svm = (ibase_service*)emalloc(sizeof(ibase_service));
	svm->handle = handle;
	svm->hostname = estrdup(host);
	svm->username = estrdup(user);

	ZEND_REGISTER_RESOURCE(return_value, svm, le_service);
	zend_list_addref(Z_LVAL_P(return_value));
}
/* }}} */

/* {{{ proto bool ibase_service_detach(resource service_handle) 
   Disconnect from the service manager */
PHP_FUNCTION(ibase_service_detach)
{
	zval *res;
	
	RESET_ERRMSG;
	
	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &res)) { 
		RETURN_FALSE; 
	}
	
	zend_list_delete(Z_LVAL_P(res));
	
	RETURN_TRUE;
}
/* }}} */

static void _php_ibase_backup_restore(INTERNAL_FUNCTION_PARAMETERS, char operation) /* {{{ */
{
	/**
	 * It appears that the service API is a little bit confused about which flag
	 * to use for the source and destination in the case of a restore operation.
	 * When passing the backup file as isc_spb_dbname and the destination db as 
	 * bpk_file, things work well.
	 */
	zval *res;
	char *db, *bk, buf[200]; 
	long dblen, bklen, spb_len, opts = 0;
	zend_bool replace = 0;
	ibase_service *svm;
	
	RESET_ERRMSG;
	
	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, 
			operation == isc_action_svc_backup ? "rss" : "rss|b", 
			&res, &db, &dblen, &bk, &bklen, &replace)) { 
		RETURN_FALSE; 
	}
	
	ZEND_FETCH_RESOURCE(svm, ibase_service *, &res, -1, 
		"Interbase service manager handle", le_service);
	
	switch (operation) {
		default:
			break;
		case isc_action_svc_restore:
			opts |= replace ? isc_spb_res_replace : isc_spb_res_create;
	}
	
	/* fill the param buffer */
	spb_len = snprintf(buf, sizeof(buf), "%c%c%c%c%s%c%c%c%s%c%c%c%c%c", 
		operation, isc_spb_dbname, (char)dblen, (char)(dblen >> 8), db, 
		isc_spb_bkp_file, (char)bklen, (char)(bklen >> 8), bk, isc_spb_options,
		(char)opts,(char)(opts >> 8), (char)(opts >> 16), (char)(opts >> 24));

	if (spb_len > sizeof(buf) || spb_len == -1) {
		_php_ibase_module_error("Internal error: insufficient buffer space for SPB (%ld)" 
			TSRMLS_CC, spb_len);
		RETURN_FALSE;
	}
	
	/* now start the backup/restore job */
	if (isc_service_start(IB_STATUS, &svm->handle, NULL, (unsigned short)spb_len, buf)) {
		_php_ibase_error(TSRMLS_C);
		RETURN_FALSE;
	}
	
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool ibase_backup(resource service_handle, string source_db, string dest_file) 
   Initiates a backup task in the service manager and returns immediately */
PHP_FUNCTION(ibase_backup)
{
	_php_ibase_backup_restore(INTERNAL_FUNCTION_PARAM_PASSTHRU, isc_action_svc_backup);
}

/* {{{ proto bool ibase_restore(resource service_handle, string source_file, string dest_db [, bool replace]) 
   Initiates a restore task in the service manager and returns immediately */
PHP_FUNCTION(ibase_restore)
{
	_php_ibase_backup_restore(INTERNAL_FUNCTION_PARAM_PASSTHRU, isc_action_svc_restore);
}
/* }}} */
#endif /* HAVE_IBASE */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
