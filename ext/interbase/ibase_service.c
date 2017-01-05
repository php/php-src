/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2017 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Ard Biesheuvel <a.k.biesheuvel@its.tudelft.nl>              |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#if HAVE_IBASE

#include "php_interbase.h"
#include "php_ibase_includes.h"

typedef struct {
	isc_svc_handle handle;
	char *hostname;
	char *username;
	zend_resource *res;
} ibase_service;

static int le_service;

static void _php_ibase_free_service(zend_resource *rsrc) /* {{{ */
{
	ibase_service *sv = (ibase_service *) rsrc->ptr;

	if (isc_service_detach(IB_STATUS, &sv->handle)) {
		_php_ibase_error();
	}

	if (sv->hostname) {
		efree(sv->hostname);
	}
	if (sv->username) {
		efree(sv->username);
	}

	efree(sv);
}
/* }}} */

/* the svc api seems to get confused after an error has occurred,
   so invalidate the handle on errors */
#define IBASE_SVC_ERROR(svm) \
	do { zend_list_delete(svm->res); _php_ibase_error(); } while (0)


void php_ibase_service_minit(INIT_FUNC_ARGS) /* {{{ */
{
	le_service = zend_register_list_destructors_ex(_php_ibase_free_service, NULL,
	    "interbase service manager handle", module_number);

	/* backup options */
	REGISTER_LONG_CONSTANT("IBASE_BKP_IGNORE_CHECKSUMS", isc_spb_bkp_ignore_checksums, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IBASE_BKP_IGNORE_LIMBO", isc_spb_bkp_ignore_limbo, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IBASE_BKP_METADATA_ONLY", isc_spb_bkp_metadata_only, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IBASE_BKP_NO_GARBAGE_COLLECT", isc_spb_bkp_no_garbage_collect, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IBASE_BKP_OLD_DESCRIPTIONS", isc_spb_bkp_old_descriptions, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IBASE_BKP_NON_TRANSPORTABLE", isc_spb_bkp_non_transportable, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IBASE_BKP_CONVERT", isc_spb_bkp_convert, CONST_PERSISTENT);

	/* restore options */
	REGISTER_LONG_CONSTANT("IBASE_RES_DEACTIVATE_IDX", isc_spb_res_deactivate_idx, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IBASE_RES_NO_SHADOW", isc_spb_res_no_shadow, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IBASE_RES_NO_VALIDITY", isc_spb_res_no_validity, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IBASE_RES_ONE_AT_A_TIME", isc_spb_res_one_at_a_time, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IBASE_RES_REPLACE", isc_spb_res_replace, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IBASE_RES_CREATE", isc_spb_res_create, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IBASE_RES_USE_ALL_SPACE", isc_spb_res_use_all_space, CONST_PERSISTENT);

	/* manage options */
	REGISTER_LONG_CONSTANT("IBASE_PRP_PAGE_BUFFERS", isc_spb_prp_page_buffers, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IBASE_PRP_SWEEP_INTERVAL", isc_spb_prp_sweep_interval, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IBASE_PRP_SHUTDOWN_DB", isc_spb_prp_shutdown_db, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IBASE_PRP_DENY_NEW_TRANSACTIONS", isc_spb_prp_deny_new_transactions, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IBASE_PRP_DENY_NEW_ATTACHMENTS", isc_spb_prp_deny_new_attachments, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IBASE_PRP_RESERVE_SPACE", isc_spb_prp_reserve_space, CONST_PERSISTENT);
	  REGISTER_LONG_CONSTANT("IBASE_PRP_RES_USE_FULL", isc_spb_prp_res_use_full, CONST_PERSISTENT);
	  REGISTER_LONG_CONSTANT("IBASE_PRP_RES", isc_spb_prp_res, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IBASE_PRP_WRITE_MODE", isc_spb_prp_write_mode, CONST_PERSISTENT);
	  REGISTER_LONG_CONSTANT("IBASE_PRP_WM_ASYNC", isc_spb_prp_wm_async, CONST_PERSISTENT);
	  REGISTER_LONG_CONSTANT("IBASE_PRP_WM_SYNC", isc_spb_prp_wm_sync, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IBASE_PRP_ACCESS_MODE", isc_spb_prp_access_mode, CONST_PERSISTENT);
	  REGISTER_LONG_CONSTANT("IBASE_PRP_AM_READONLY", isc_spb_prp_am_readonly, CONST_PERSISTENT);
	  REGISTER_LONG_CONSTANT("IBASE_PRP_AM_READWRITE", isc_spb_prp_am_readwrite, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IBASE_PRP_SET_SQL_DIALECT", isc_spb_prp_set_sql_dialect, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IBASE_PRP_ACTIVATE", isc_spb_prp_activate, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IBASE_PRP_DB_ONLINE", isc_spb_prp_db_online, CONST_PERSISTENT);

	/* repair options */
	REGISTER_LONG_CONSTANT("IBASE_RPR_CHECK_DB", isc_spb_rpr_check_db, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IBASE_RPR_IGNORE_CHECKSUM", isc_spb_rpr_ignore_checksum, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IBASE_RPR_KILL_SHADOWS", isc_spb_rpr_kill_shadows, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IBASE_RPR_MEND_DB", isc_spb_rpr_mend_db, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IBASE_RPR_VALIDATE_DB", isc_spb_rpr_validate_db, CONST_PERSISTENT);
	  REGISTER_LONG_CONSTANT("IBASE_RPR_FULL", isc_spb_rpr_full, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IBASE_RPR_SWEEP_DB", isc_spb_rpr_sweep_db, CONST_PERSISTENT);

	/* db info arguments */
	REGISTER_LONG_CONSTANT("IBASE_STS_DATA_PAGES", isc_spb_sts_data_pages, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IBASE_STS_DB_LOG", isc_spb_sts_db_log, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IBASE_STS_HDR_PAGES", isc_spb_sts_hdr_pages, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IBASE_STS_IDX_PAGES", isc_spb_sts_idx_pages, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IBASE_STS_SYS_RELATIONS", isc_spb_sts_sys_relations, CONST_PERSISTENT);

	/* server info arguments */
	REGISTER_LONG_CONSTANT("IBASE_SVC_SERVER_VERSION", isc_info_svc_server_version, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IBASE_SVC_IMPLEMENTATION", isc_info_svc_implementation, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IBASE_SVC_GET_ENV", isc_info_svc_get_env, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IBASE_SVC_GET_ENV_LOCK", isc_info_svc_get_env_lock, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IBASE_SVC_GET_ENV_MSG", isc_info_svc_get_env_msg, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IBASE_SVC_USER_DBPATH", isc_info_svc_user_dbpath, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IBASE_SVC_SVR_DB_INFO", isc_info_svc_svr_db_info, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IBASE_SVC_GET_USERS", isc_info_svc_get_users, CONST_PERSISTENT);
}
/* }}} */

static void _php_ibase_user(INTERNAL_FUNCTION_PARAMETERS, char operation) /* {{{ */
{
	/* user = 0, password = 1, first_name = 2, middle_name = 3, last_name = 4 */
	static char const user_flags[] = { isc_spb_sec_username, isc_spb_sec_password,
	    isc_spb_sec_firstname, isc_spb_sec_middlename, isc_spb_sec_lastname };
	char buf[128], *args[] = { NULL, NULL, NULL, NULL, NULL };
	int i, args_len[] = { 0, 0, 0, 0, 0 };
	unsigned short spb_len = 1;
	zval *res;
	ibase_service *svm;

	RESET_ERRMSG;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(),
			(operation == isc_action_svc_delete_user) ? "rs" : "rss|sss",
			&res, &args[0], &args_len[0], &args[1], &args_len[1], &args[2], &args_len[2],
			&args[3], &args_len[3], &args[4], &args_len[4])) {
		RETURN_FALSE;
	}

	svm = (ibase_service *)zend_fetch_resource_ex(res, "Interbase service manager handle",
		le_service);

	buf[0] = operation;

	for (i = 0; i < sizeof(user_flags); ++i) {
		if (args[i] != NULL) {
			int chunk = slprintf(&buf[spb_len], sizeof(buf) - spb_len, "%c%c%c%s",
				user_flags[i], (char)args_len[i], (char)(args_len[i] >> 8), args[i]);

			if ((spb_len + chunk) > sizeof(buf) || chunk <= 0) {
				_php_ibase_module_error("Internal error: insufficient buffer space for SPB (%d)", spb_len);
				RETURN_FALSE;
			}
			spb_len += chunk;
		}
	}

	/* now start the job */
	if (isc_service_start(IB_STATUS, &svm->handle, NULL, spb_len, buf)) {
		IBASE_SVC_ERROR(svm);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool ibase_add_user(resource service_handle, string user_name, string password [, string first_name [, string middle_name [, string last_name]]])
   Add a user to security database */
PHP_FUNCTION(ibase_add_user)
{
	_php_ibase_user(INTERNAL_FUNCTION_PARAM_PASSTHRU, isc_action_svc_add_user);
}
/* }}} */

/* {{{ proto bool ibase_modify_user(resource service_handle, string user_name, string password [, string first_name [, string middle_name [, string last_name]]])
   Modify a user in security database */
PHP_FUNCTION(ibase_modify_user)
{
	_php_ibase_user(INTERNAL_FUNCTION_PARAM_PASSTHRU, isc_action_svc_modify_user);
}
/* }}} */

/* {{{ proto bool ibase_delete_user(resource service_handle, string user_name, string password [, string first_name [, string middle_name [, string last_name]]])
   Delete a user from security database */
PHP_FUNCTION(ibase_delete_user)
{
	_php_ibase_user(INTERNAL_FUNCTION_PARAM_PASSTHRU, isc_action_svc_delete_user);
}
/* }}} */

/* {{{ proto resource ibase_service_attach(string host, string dba_username, string dba_password)
   Connect to the service manager */
PHP_FUNCTION(ibase_service_attach)
{
	size_t hlen, ulen, plen, spb_len;
	ibase_service *svm;
	char buf[128], *host, *user, *pass, *loc;
	isc_svc_handle handle = 0;

	RESET_ERRMSG;

	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS(), "sss",
			&host, &hlen, &user, &ulen, &pass, &plen)) {

		RETURN_FALSE;
	}

	/* construct the spb, hack the service name into it as well */
	spb_len = slprintf(buf, sizeof(buf), "%c%c%c%c%s%c%c%s" "%s:service_mgr",
		isc_spb_version, isc_spb_current_version, isc_spb_user_name, (char)ulen,
		user, isc_spb_password, (char)plen, pass, host);

	if (spb_len > sizeof(buf) || spb_len == -1) {
		_php_ibase_module_error("Internal error: insufficient buffer space for SPB (%d)", spb_len);
		RETURN_FALSE;
	}

	spb_len -= hlen + 12;
	loc = buf + spb_len; /* points to %s:service_mgr part */

	/* attach to the service manager */
	if (isc_service_attach(IB_STATUS, 0, loc, &handle, (unsigned short)spb_len, buf)) {
		_php_ibase_error();
		RETURN_FALSE;
	}

	svm = (ibase_service*)emalloc(sizeof(ibase_service));
	svm->handle = handle;
	svm->hostname = estrdup(host);
	svm->username = estrdup(user);

	RETVAL_RES(zend_register_resource(svm, le_service));
	Z_TRY_ADDREF_P(return_value);
	svm->res = Z_RES_P(return_value);
}
/* }}} */

/* {{{ proto bool ibase_service_detach(resource service_handle)
   Disconnect from the service manager */
PHP_FUNCTION(ibase_service_detach)
{
	zval *res;

	RESET_ERRMSG;

	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS(), "r", &res)) {
		RETURN_FALSE;
	}

	zend_list_delete(Z_RES_P(res));

	RETURN_TRUE;
}
/* }}} */

static void _php_ibase_service_query(INTERNAL_FUNCTION_PARAMETERS, /* {{{ */
	ibase_service *svm, char info_action)
{
	static char spb[] = { isc_info_svc_timeout, 10, 0, 0, 0 };

	char res_buf[400], *result, *heap_buf = NULL, *heap_p;
	zend_long heap_buf_size = 200, line_len;

	/* info about users requires an action first */
	if (info_action == isc_info_svc_get_users) {
		static char action[] = { isc_action_svc_display_user };

		if (isc_service_start(IB_STATUS, &svm->handle, NULL, sizeof(action), action)) {
			IBASE_SVC_ERROR(svm);
			RETURN_FALSE;
		}
	}

query_loop:
	result = res_buf;

	if (isc_service_query(IB_STATUS, &svm->handle, NULL, sizeof(spb), spb,
			1, &info_action, sizeof(res_buf), res_buf)) {

		IBASE_SVC_ERROR(svm);
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
						RETVAL_STRING(heap_buf);
						efree(heap_buf);
						return;
					} else {
						RETURN_TRUE;
					}
				}
				if (!heap_buf || (heap_p - heap_buf + line_len +2) > heap_buf_size) {
					zend_long res_size = heap_buf ? heap_p - heap_buf : 0;

					while (heap_buf_size < (res_size + line_len +2)) {
						heap_buf_size *= 2;
					}
					heap_buf = (char*) erealloc(heap_buf, heap_buf_size);
					heap_p = heap_buf + res_size;
				}
				result += 2;
				*(result+line_len) = 0;
				snprintf(heap_p, heap_buf_size - (heap_p - heap_buf), "%s\n", result);
				heap_p += line_len +1;
				goto query_loop; /* repeat until result is exhausted */

			case isc_info_svc_server_version:
			case isc_info_svc_implementation:
			case isc_info_svc_get_env:
			case isc_info_svc_get_env_lock:
			case isc_info_svc_get_env_msg:
			case isc_info_svc_user_dbpath:
				RETURN_STRINGL(result + 2, isc_vax_integer(result, 2));

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
							add_next_index_stringl(return_value, result +2, len);
							result += len+2;
					}
				} while (*result != isc_info_flag_end);
				return;

			case isc_info_svc_get_users: {
				zval user;
				array_init(return_value);

				while (*result != isc_info_end) {

					switch (*result++) {
						int len;

						case isc_spb_sec_username:
							/* it appears that the username is always first */
							array_init(&user);
							add_next_index_zval(return_value, &user);

							len = isc_vax_integer(result,2);
							add_assoc_stringl(&user, "user_name", result +2, len);
							result += len+2;
							break;

						case isc_spb_sec_firstname:
							len = isc_vax_integer(result,2);
							add_assoc_stringl(&user, "first_name", result +2, len);
							result += len+2;
							break;

						case isc_spb_sec_middlename:
							len = isc_vax_integer(result,2);
							add_assoc_stringl(&user, "middle_name", result +2, len);
							result += len+2;
							break;

						case isc_spb_sec_lastname:
							len = isc_vax_integer(result,2);
							add_assoc_stringl(&user, "last_name", result +2, len);
							result += len+2;
							break;

						case isc_spb_sec_userid:
							add_assoc_long(&user, "user_id", isc_vax_integer(result, 4));
							result += 4;
							break;

						case isc_spb_sec_groupid:
							add_assoc_long(&user, "group_id", isc_vax_integer(result, 4));
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
	size_t dblen, bklen, spb_len;
	zend_long opts = 0;
	zend_bool verbose = 0;
	ibase_service *svm;

	RESET_ERRMSG;

	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS(), "rss|lb",
			&res, &db, &dblen, &bk, &bklen, &opts, &verbose)) {
		RETURN_FALSE;
	}

	svm = (ibase_service *)zend_fetch_resource_ex(res,
		"Interbase service manager handle", le_service);

	/* fill the param buffer */
	spb_len = slprintf(buf, sizeof(buf), "%c%c%c%c%s%c%c%c%s%c%c%c%c%c",
		operation, isc_spb_dbname, (char)dblen, (char)(dblen >> 8), db,
		isc_spb_bkp_file, (char)bklen, (char)(bklen >> 8), bk, isc_spb_options,
		(char)opts,(char)(opts >> 8), (char)(opts >> 16), (char)(opts >> 24));

	if (verbose) {
		buf[spb_len++] = isc_spb_verbose;
	}

	if (spb_len > sizeof(buf) || spb_len <= 0) {
		_php_ibase_module_error("Internal error: insufficient buffer space for SPB (%d)", spb_len);
		RETURN_FALSE;
	}

	/* now start the backup/restore job */
	if (isc_service_start(IB_STATUS, &svm->handle, NULL, (unsigned short)spb_len, buf)) {
		IBASE_SVC_ERROR(svm);
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
	size_t dblen;
	int spb_len;
	zend_long action, argument = 0;
	ibase_service *svm;

	RESET_ERRMSG;

	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS(), "rsl|l",
			&res, &db, &dblen, &action, &argument)) {
		RETURN_FALSE;
	}

	svm = (ibase_service *)zend_fetch_resource_ex(res,
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
				_php_ibase_module_error("Unrecognised option (" ZEND_LONG_FMT ")", action);
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
				spb_len = slprintf(buf, sizeof(buf), "%c%c%c%c%s%c%c%c%c%c",
					svc_action, isc_spb_dbname, (char)dblen, (char)(dblen >> 8), db,
					(char)action, (char)argument, (char)(argument >> 8), (char)(argument >> 16),
					(char)(argument >> 24));
				break;

			case isc_spb_prp_reserve_space:
			case isc_spb_prp_write_mode:
			case isc_spb_prp_access_mode:
				spb_len = slprintf(buf, sizeof(buf), "%c%c%c%c%s%c%c",
					isc_action_svc_properties, isc_spb_dbname, (char)dblen, (char)(dblen >> 8),
					db, (char)action, (char)argument);
		}
	}

	if (spb_len > sizeof(buf) || spb_len == -1) {
		_php_ibase_module_error("Internal error: insufficient buffer space for SPB (%d)", spb_len);
		RETURN_FALSE;
	}

	if (isc_service_start(IB_STATUS, &svm->handle, NULL, (unsigned short)spb_len, buf)) {
		IBASE_SVC_ERROR(svm);
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
   Request information about a database server */
PHP_FUNCTION(ibase_server_info)
{
	zval *res;
	zend_long action;
	ibase_service *svm;

	RESET_ERRMSG;

	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS(), "rl", &res, &action)) {
		RETURN_FALSE;
	}

	svm = (ibase_service *)zend_fetch_resource_ex(res,
		"Interbase service manager handle", le_service);

	_php_ibase_service_query(INTERNAL_FUNCTION_PARAM_PASSTHRU, svm, (char)action);
}
/* }}} */

#else

void php_ibase_register_service_constants(INIT_FUNC_ARGS) { /* nop */ }

#endif /* HAVE_IBASE */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
