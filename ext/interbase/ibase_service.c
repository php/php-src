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
#include "php_ibase_includes.h"

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

static void _php_ibase_service_query(INTERNAL_FUNCTION_PARAMETERS, /* {{{ */
	ibase_service *svm, char info_action)
{
	static char spb[] = { isc_info_svc_timeout, 10, 0, 0, 0 };
		
	char res_buf[400], *result, *heap_buf = NULL, *heap_p;
	long heap_buf_size = 200, line_len;

	/* info about users requires an action first */
	if (info_action == isc_info_svc_get_users) {
		static char action[] = { isc_action_svc_display_user };

		if (isc_service_start(IB_STATUS, &svm->handle, NULL, sizeof(action), action)) {
			_php_ibase_error(TSRMLS_C);
			RETURN_FALSE;
		}
	}
	
query_loop:		
	result = res_buf;

	if (isc_service_query(IB_STATUS, &svm->handle, NULL, sizeof(spb), spb,
			1, &info_action, sizeof(res_buf), res_buf)) {
		
		_php_ibase_error(TSRMLS_C);
		RETURN_FALSE;
	}
	while (*result != isc_info_end) {
		switch (*result++) {
			default:
				RETURN_FALSE;
				
			case isc_info_svc_line: 
				if (! (line_len = isc_vax_integer(result, 2))) {
					/* done */
					if (heap_buf) {
						RETURN_STRING(heap_buf,0);
					} else {
						RETURN_TRUE;
					}
				}
				if (!heap_buf || (heap_p - heap_buf + line_len +2) > heap_buf_size) {
					long res_size = heap_buf ? heap_p - heap_buf : 0;
					
					while (heap_buf_size < (res_size + line_len +2)) {
						heap_buf_size *= 2;					
					}
					heap_buf = (char*) erealloc(heap_buf, heap_buf_size);
					heap_p = heap_buf + res_size;
				}
				result += 2;
				sprintf(heap_p, "%s\n", result);
				heap_p += line_len +2;
				goto query_loop; /* repeat until result is exhausted */

			case isc_info_svc_server_version:
			case isc_info_svc_implementation:
			case isc_info_svc_get_env:
			case isc_info_svc_get_env_lock:
			case isc_info_svc_get_env_msg:
			case isc_info_svc_user_dbpath:
				RETURN_STRINGL(result + 2, isc_vax_integer(result, 2), 1);				

			case isc_info_svc_svr_db_info:
				array_init(return_value);

				do {
					switch (*result++) {
						int len;

						case isc_spb_num_att:
							add_assoc_long(return_value, "attachments", isc_vax_integer(result,4));
							result += 4;
							break;
						
						case isc_spb_num_db:
							add_assoc_long(return_value, "databases", isc_vax_integer(result,4));
							result += 4;
							break;

						case isc_spb_dbname:
							len = isc_vax_integer(result,2);
							add_next_index_stringl(return_value, result +2, len, 1);
							result += len+2;
					}
				} while (*result != isc_info_flag_end);
				return;
				
			case isc_info_svc_get_users: {
				zval *user;
				array_init(return_value);
				
				while (*result != isc_info_end) {
					
					switch (*result++) {
						int len;
						
						case isc_spb_sec_username:
							/* it appears that the username is always first */
							ALLOC_INIT_ZVAL(user);
							array_init(user);
							add_next_index_zval(return_value, user);

							len = isc_vax_integer(result,2);
							add_assoc_stringl(user, "user_name", result +2, len, 1);
							result += len+2;
							break;
						
						case isc_spb_sec_firstname:
							len = isc_vax_integer(result,2);
							add_assoc_stringl(user, "first_name", result +2, len, 1);
							result += len+2;
							break;

						case isc_spb_sec_middlename:
							len = isc_vax_integer(result,2);
							add_assoc_stringl(user, "middle_name", result +2, len, 1);
							result += len+2;
							break;

						case isc_spb_sec_lastname:
							len = isc_vax_integer(result,2);
							add_assoc_stringl(user, "last_name", result +2, len, 1);
							result += len+2;
							break;

						case isc_spb_sec_userid:
							add_assoc_long(user, "user_id", isc_vax_integer(result, 4));
							result += 4;
							break;
							
						case isc_spb_sec_groupid:
							add_assoc_long(user, "group_id", isc_vax_integer(result, 4));
							result += 4;
							break;
					}
				}
				return;	
			}
		}
	}
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
	zend_bool verbose = 0;
	ibase_service *svm;
	
	RESET_ERRMSG;
	
	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rss|lb",
			&res, &db, &dblen, &bk, &bklen, &opts, &verbose)) { 
		RETURN_FALSE; 
	}
	
	ZEND_FETCH_RESOURCE(svm, ibase_service *, &res, -1, 
		"Interbase service manager handle", le_service);
	
	/* fill the param buffer */
	spb_len = snprintf(buf, sizeof(buf), "%c%c%c%c%s%c%c%c%s%c%c%c%c%c", 
		operation, isc_spb_dbname, (char)dblen, (char)(dblen >> 8), db, 
		isc_spb_bkp_file, (char)bklen, (char)(bklen >> 8), bk, isc_spb_options,
		(char)opts,(char)(opts >> 8), (char)(opts >> 16), (char)(opts >> 24));
		
	if (verbose) {
		buf[spb_len++] = isc_spb_verbose;
	}

	if (spb_len > sizeof(buf) || spb_len <= 0) {
		_php_ibase_module_error("Internal error: insufficient buffer space for SPB (%ld)" 
			TSRMLS_CC, spb_len);
		RETURN_FALSE;
	}
	
	/* now start the backup/restore job */
	if (isc_service_start(IB_STATUS, &svm->handle, NULL, (unsigned short)spb_len, buf)) {
		_php_ibase_error(TSRMLS_C);
		RETURN_FALSE;
	}
	
	if (!verbose) {
		RETURN_TRUE;
	} else {
		_php_ibase_service_query(INTERNAL_FUNCTION_PARAM_PASSTHRU, svm, isc_info_svc_line);
	}
}
/* }}} */

/* {{{ proto mixed ibase_backup(resource service_handle, string source_db, string dest_file [, int options [, bool verbose]])
   Initiates a backup task in the service manager and returns immediately */
PHP_FUNCTION(ibase_backup)
{
	_php_ibase_backup_restore(INTERNAL_FUNCTION_PARAM_PASSTHRU, isc_action_svc_backup);
}
/* }}} */

/* {{{ proto mixed ibase_restore(resource service_handle, string source_file, string dest_db [, int options [, bool verbose]])
   Initiates a restore task in the service manager and returns immediately */
PHP_FUNCTION(ibase_restore)
{
	_php_ibase_backup_restore(INTERNAL_FUNCTION_PARAM_PASSTHRU, isc_action_svc_restore);
}
/* }}} */

static void _php_ibase_service_action(INTERNAL_FUNCTION_PARAMETERS, char svc_action) /* {{{ */
{
	zval *res;
	char buf[128], *db;
	long action, spb_len, dblen, argument = 0;
	ibase_service *svm;
	
	RESET_ERRMSG;
	
	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsl|l",
			&res, &db, &dblen, &action, &argument)) { 
		RETURN_FALSE; 
	}
	
	ZEND_FETCH_RESOURCE(svm, ibase_service *, &res, -1, 
		"Interbase service manager handle", le_service);
	
	if (svc_action == isc_action_svc_db_stats) {
		switch (action) {
			default:
				goto unknown_option;
				
			case isc_spb_sts_data_pages:
			case isc_spb_sts_db_log:
			case isc_spb_sts_hdr_pages:
			case isc_spb_sts_idx_pages:
			case isc_spb_sts_sys_relations:
				goto options_argument;
		}
	} else {
		/* these actions all expect different types of arguments */
		switch (action) {
			default:
unknown_option:			
				_php_ibase_module_error("Unrecognised option (%ld)" TSRMLS_CC, action);
				RETURN_FALSE;
				
			case isc_spb_rpr_check_db:
			case isc_spb_rpr_ignore_checksum:
			case isc_spb_rpr_kill_shadows:
			case isc_spb_rpr_mend_db:
			case isc_spb_rpr_validate_db:
			case isc_spb_rpr_sweep_db:
				svc_action = isc_action_svc_repair;
				
			case isc_spb_prp_activate:
			case isc_spb_prp_db_online:
options_argument:	
				argument |= action;
				action = isc_spb_options;
	
			case isc_spb_prp_page_buffers:
			case isc_spb_prp_sweep_interval:
			case isc_spb_prp_shutdown_db:
			case isc_spb_prp_deny_new_transactions:
			case isc_spb_prp_deny_new_attachments:
			case isc_spb_prp_set_sql_dialect:
				spb_len = snprintf(buf, sizeof(buf), "%c%c%c%c%s%c%c%c%c%c",
					svc_action, isc_spb_dbname, (char)dblen, (char)(dblen >> 8), db, 
					(char)action, (char)argument, (char)(argument >> 8), (char)(argument >> 16),
					(char)(argument >> 24));
				break;
				
			case isc_spb_prp_reserve_space:
			case isc_spb_prp_write_mode:
			case isc_spb_prp_access_mode:
				spb_len = snprintf(buf, sizeof(buf), "%c%c%c%c%s%c%c",
					isc_action_svc_properties, isc_spb_dbname, (char)dblen, (char)(dblen >> 8), 
					db, (char)action, (char)argument);
		}
	}
	
	if (spb_len > sizeof(buf) || spb_len == -1) {
		_php_ibase_module_error("Internal error: insufficient buffer space for SPB (%ld)" 
			TSRMLS_CC, spb_len);
		RETURN_FALSE;
	}

	if (isc_service_start(IB_STATUS, &svm->handle, NULL, (unsigned short)spb_len, buf)) {
		_php_ibase_error(TSRMLS_C);
		RETURN_FALSE;
	}
	
	if (svc_action == isc_action_svc_db_stats) {
		_php_ibase_service_query(INTERNAL_FUNCTION_PARAM_PASSTHRU, svm, isc_info_svc_line);
	} else {
		RETURN_TRUE;
	}
}	
/* }}} */	    

/* {{{ proto bool ibase_maintain_db(resource service_handle, string db, int action [, int argument])
   Execute a maintenance command on the database server */
PHP_FUNCTION(ibase_maintain_db)
{
	_php_ibase_service_action(INTERNAL_FUNCTION_PARAM_PASSTHRU, isc_action_svc_properties);
}
/* }}} */

/* {{{ proto string ibase_db_info(resource service_handle, string db, int action [, int argument])
   Request statistics about a database */
PHP_FUNCTION(ibase_db_info)
{
	_php_ibase_service_action(INTERNAL_FUNCTION_PARAM_PASSTHRU, isc_action_svc_db_stats);
}
/* }}} */

/* {{{ proto string ibase_server_info(resource service_handle, int action)
   Request information about a database */
PHP_FUNCTION(ibase_server_info)
{
	zval *res;
	long action;
	ibase_service *svm;
	
	RESET_ERRMSG;
	
	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &res, &action)) { 
		RETURN_FALSE; 
	}
	
	ZEND_FETCH_RESOURCE(svm, ibase_service *, &res, -1, 
		"Interbase service manager handle", le_service);
	
	_php_ibase_service_query(INTERNAL_FUNCTION_PARAM_PASSTHRU, svm, (char)action);
}
/* }}} */

#endif /* HAVE_IBASE6_API */
        
/*      
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
