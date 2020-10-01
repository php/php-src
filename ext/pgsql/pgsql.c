/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Zeev Suraski <zeev@php.net>                                 |
   |          Jouni Ahto <jouni.ahto@exdec.fi>                            |
   |          Yasuo Ohgaki <yohgaki@php.net>                              |
   |          Youichi Iwakiri <yiwakiri@st.rim.or.jp> (pg_copy_*)         |
   |          Chris Kings-Lynne <chriskl@php.net> (v3 protocol)           |
   +----------------------------------------------------------------------+
 */

#include <stdlib.h>

#define PHP_PGSQL_PRIVATE 1

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define SMART_STR_PREALLOC 512

#include "php.h"
#include "php_ini.h"
#include "ext/standard/php_standard.h"
#include "zend_smart_str.h"
#include "ext/pcre/php_pcre.h"
#ifdef PHP_WIN32
# include "win32/time.h"
#endif
#include "php_pgsql.h"
#include "php_globals.h"
#include "zend_exceptions.h"
#include "pgsql_arginfo.h"

#ifdef HAVE_PGSQL

#ifndef InvalidOid
#define InvalidOid ((Oid) 0)
#endif

#define PGSQL_ASSOC           1<<0
#define PGSQL_NUM             1<<1
#define PGSQL_BOTH            (PGSQL_ASSOC|PGSQL_NUM)

#define PGSQL_NOTICE_LAST     1  /* Get the last notice */
#define PGSQL_NOTICE_ALL      2  /* Get all notices */
#define PGSQL_NOTICE_CLEAR    3  /* Remove notices */

#define PGSQL_STATUS_LONG     1
#define PGSQL_STATUS_STRING   2

#define PGSQL_MAX_LENGTH_OF_LONG   30
#define PGSQL_MAX_LENGTH_OF_DOUBLE 60

#if ZEND_LONG_MAX < UINT_MAX
#define PGSQL_RETURN_OID(oid) do { \
	if (oid > ZEND_LONG_MAX) { \
		smart_str s = {0}; \
		smart_str_append_unsigned(&s, oid); \
		smart_str_0(&s); \
		RETURN_NEW_STR(s.s); \
	} \
	RETURN_LONG((zend_long)oid); \
} while(0)
#else
#define PGSQL_RETURN_OID(oid) RETURN_LONG((zend_long)oid)
#endif

#define CHECK_DEFAULT_LINK(x) \
	if ((x) == NULL) { \
		zend_throw_error(NULL, "No PostgreSQL link opened yet"); \
		RETURN_THROWS(); \
	}
#define FETCH_DEFAULT_LINK()  PGG(default_link)

#ifndef HAVE_PQFREEMEM
#define PQfreemem free
#endif

ZEND_DECLARE_MODULE_GLOBALS(pgsql)
static PHP_GINIT_FUNCTION(pgsql);

/* {{{ pgsql_module_entry */
zend_module_entry pgsql_module_entry = {
	STANDARD_MODULE_HEADER,
	"pgsql",
	ext_functions,
	PHP_MINIT(pgsql),
	PHP_MSHUTDOWN(pgsql),
	PHP_RINIT(pgsql),
	PHP_RSHUTDOWN(pgsql),
	PHP_MINFO(pgsql),
	PHP_PGSQL_VERSION,
	PHP_MODULE_GLOBALS(pgsql),
	PHP_GINIT(pgsql),
	NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};
/* }}} */

#ifdef COMPILE_DL_PGSQL
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(pgsql)
#endif

static int le_link, le_plink, le_result, le_lofp, le_string;

/* Compatibility definitions */

#ifndef HAVE_PGSQL_WITH_MULTIBYTE_SUPPORT
#define pg_encoding_to_char(x) "SQL_ASCII"
#endif

/* {{{ _php_pgsql_trim_message */
static char * _php_pgsql_trim_message(const char *message, size_t *len)
{
	register size_t i = strlen(message);

	if (i>2 && (message[i-2] == '\r' || message[i-2] == '\n') && message[i-1] == '.') {
		--i;
	}
	while (i>1 && (message[i-1] == '\r' || message[i-1] == '\n')) {
		--i;
	}
	if (len) {
		*len = i;
	}
	return estrndup(message, i);
}
/* }}} */

/* {{{ _php_pgsql_trim_result */
static inline char * _php_pgsql_trim_result(PGconn * pgsql, char **buf)
{
	return *buf = _php_pgsql_trim_message(PQerrorMessage(pgsql), NULL);
}
/* }}} */

#define PQErrorMessageTrim(pgsql, buf) _php_pgsql_trim_result(pgsql, buf)

#define PHP_PQ_ERROR(text, pgsql) {										\
		char *msgbuf = _php_pgsql_trim_message(PQerrorMessage(pgsql), NULL); \
		php_error_docref(NULL, E_WARNING, text, msgbuf);		\
		efree(msgbuf);													\
} \

/* {{{ php_pgsql_set_default_link */
static void php_pgsql_set_default_link(zend_resource *res)
{
	GC_ADDREF(res);

	if (PGG(default_link) != NULL) {
		zend_list_delete(PGG(default_link));
	}

	PGG(default_link) = res;
}
/* }}} */

/* {{{ _close_pgsql_link */
static void _close_pgsql_link(zend_resource *rsrc)
{
	PGconn *link = (PGconn *)rsrc->ptr;
	PGresult *res;
	zval *hash;

	while ((res = PQgetResult(link))) {
		PQclear(res);
	}
	PQfinish(link);
	PGG(num_links)--;

	/* Remove connection hash for this link */
	hash = zend_hash_index_find(&PGG(hashes), (uintptr_t) link);
	if (hash) {
		zend_hash_index_del(&PGG(hashes), (uintptr_t) link);
		zend_hash_del(&EG(regular_list), Z_STR_P(hash));
	}
}
/* }}} */

/* {{{ _close_pgsql_plink */
static void _close_pgsql_plink(zend_resource *rsrc)
{
	PGconn *link = (PGconn *)rsrc->ptr;
	PGresult *res;

	while ((res = PQgetResult(link))) {
		PQclear(res);
	}
	PQfinish(link);
	PGG(num_persistent)--;
	PGG(num_links)--;
}
/* }}} */

/* {{{ _php_pgsql_notice_handler */
static void _php_pgsql_notice_handler(void *resource_id, const char *message)
{
	zval *notices;
	zval tmp;
	char *trimed_message;
	size_t trimed_message_len;

	if (! PGG(ignore_notices)) {
		notices = zend_hash_index_find(&PGG(notices), (zend_ulong)resource_id);
		if (!notices) {
			array_init(&tmp);
			notices = &tmp;
			zend_hash_index_update(&PGG(notices), (zend_ulong)resource_id, notices);
		}
		trimed_message = _php_pgsql_trim_message(message, &trimed_message_len);
		if (PGG(log_notices)) {
			php_error_docref(NULL, E_NOTICE, "%s", trimed_message);
		}
		add_next_index_stringl(notices, trimed_message, trimed_message_len);
		efree(trimed_message);
	}
}
/* }}} */

/* {{{ _rollback_transactions */
static int _rollback_transactions(zval *el)
{
	PGconn *link;
	PGresult *res;
	zend_resource *rsrc = Z_RES_P(el);

	if (rsrc->type != le_plink)
		return 0;

	link = (PGconn *) rsrc->ptr;

	if (PQsetnonblocking(link, 0)) {
		php_error_docref("ref.pgsql", E_NOTICE, "Cannot set connection to blocking mode");
		return -1;
	}

	while ((res = PQgetResult(link))) {
		PQclear(res);
	}
	if ((PQprotocolVersion(link) >= 3 && PQtransactionStatus(link) != PQTRANS_IDLE) || PQprotocolVersion(link) < 3) {
		int orig = PGG(ignore_notices);
		PGG(ignore_notices) = 1;
		res = PQexec(link,"ROLLBACK;");
		PQclear(res);
		PGG(ignore_notices) = orig;
	}

	return 0;
}
/* }}} */

/* {{{ _free_ptr */
static void _free_ptr(zend_resource *rsrc)
{
	pgLofp *lofp = (pgLofp *)rsrc->ptr;
	efree(lofp);
}
/* }}} */

/* {{{ _free_result */
static void _free_result(zend_resource *rsrc)
{
	pgsql_result_handle *pg_result = (pgsql_result_handle *)rsrc->ptr;

	PQclear(pg_result->result);
	efree(pg_result);
}
/* }}} */

static int _php_pgsql_detect_identifier_escape(const char *identifier, size_t len) /* {{{ */
{
	/* Handle edge case. Cannot be a escaped string */
	if (len <= 2) {
		return FAILURE;
	}
	/* Detect double quotes */
	if (identifier[0] == '"' && identifier[len-1] == '"') {
		size_t i;

		/* Detect wrong format of " inside of escaped string */
		for (i = 1; i < len-1; i++) {
			if (identifier[i] == '"' && (identifier[++i] != '"' || i == len-1)) {
				return FAILURE;
			}
		}
	} else {
		return FAILURE;
	}
	/* Escaped properly */
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_INI */
PHP_INI_BEGIN()
STD_PHP_INI_BOOLEAN( "pgsql.allow_persistent",      "1",  PHP_INI_SYSTEM, OnUpdateBool, allow_persistent,      zend_pgsql_globals, pgsql_globals)
STD_PHP_INI_ENTRY_EX("pgsql.max_persistent",       "-1",  PHP_INI_SYSTEM, OnUpdateLong, max_persistent,        zend_pgsql_globals, pgsql_globals, display_link_numbers)
STD_PHP_INI_ENTRY_EX("pgsql.max_links",            "-1",  PHP_INI_SYSTEM, OnUpdateLong, max_links,             zend_pgsql_globals, pgsql_globals, display_link_numbers)
STD_PHP_INI_BOOLEAN( "pgsql.auto_reset_persistent", "0",  PHP_INI_SYSTEM, OnUpdateBool, auto_reset_persistent, zend_pgsql_globals, pgsql_globals)
STD_PHP_INI_BOOLEAN( "pgsql.ignore_notice",         "0",  PHP_INI_ALL,    OnUpdateBool, ignore_notices,        zend_pgsql_globals, pgsql_globals)
STD_PHP_INI_BOOLEAN( "pgsql.log_notice",            "0",  PHP_INI_ALL,    OnUpdateBool, log_notices,           zend_pgsql_globals, pgsql_globals)
PHP_INI_END()
/* }}} */

/* {{{ PHP_GINIT_FUNCTION */
static PHP_GINIT_FUNCTION(pgsql)
{
#if defined(COMPILE_DL_PGSQL) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	memset(pgsql_globals, 0, sizeof(zend_pgsql_globals));
	/* Initialize notice message hash at MINIT only */
	zend_hash_init(&pgsql_globals->notices, 0, NULL, ZVAL_PTR_DTOR, 1);
	zend_hash_init(&pgsql_globals->hashes, 0, NULL, ZVAL_PTR_DTOR, 1);
}
/* }}} */

static void php_libpq_version(char *buf, size_t len)
{
	int version = PQlibVersion();
	int major = version / 10000;
	if (major >= 10) {
		int minor = version % 10000;
		snprintf(buf, len, "%d.%d", major, minor);
	} else {
		int minor = version / 100 % 100;
		int revision = version % 100;
		snprintf(buf, len, "%d.%d.%d", major, minor, revision);
	}
}

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(pgsql)
{
	char buf[16];

	REGISTER_INI_ENTRIES();

	le_link = zend_register_list_destructors_ex(_close_pgsql_link, NULL, "pgsql link", module_number);
	le_plink = zend_register_list_destructors_ex(NULL, _close_pgsql_plink, "pgsql link persistent", module_number);
	le_result = zend_register_list_destructors_ex(_free_result, NULL, "pgsql result", module_number);
	le_lofp = zend_register_list_destructors_ex(_free_ptr, NULL, "pgsql large object", module_number);
	le_string = zend_register_list_destructors_ex(_free_ptr, NULL, "pgsql string", module_number);
	/* libpq version */
	php_libpq_version(buf, sizeof(buf));
	REGISTER_STRING_CONSTANT("PGSQL_LIBPQ_VERSION", buf, CONST_CS | CONST_PERSISTENT);
	REGISTER_STRING_CONSTANT("PGSQL_LIBPQ_VERSION_STR", buf, CONST_CS | CONST_PERSISTENT | CONST_DEPRECATED);
	/* For connection option */
	REGISTER_LONG_CONSTANT("PGSQL_CONNECT_FORCE_NEW", PGSQL_CONNECT_FORCE_NEW, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_CONNECT_ASYNC", PGSQL_CONNECT_ASYNC, CONST_CS | CONST_PERSISTENT);
	/* For pg_fetch_array() */
	REGISTER_LONG_CONSTANT("PGSQL_ASSOC", PGSQL_ASSOC, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_NUM", PGSQL_NUM, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_BOTH", PGSQL_BOTH, CONST_CS | CONST_PERSISTENT);
	/* For pg_last_notice() */
	REGISTER_LONG_CONSTANT("PGSQL_NOTICE_LAST", PGSQL_NOTICE_LAST, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_NOTICE_ALL", PGSQL_NOTICE_ALL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_NOTICE_CLEAR", PGSQL_NOTICE_CLEAR, CONST_CS | CONST_PERSISTENT);
	/* For pg_connection_status() */
	REGISTER_LONG_CONSTANT("PGSQL_CONNECTION_BAD", CONNECTION_BAD, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_CONNECTION_OK", CONNECTION_OK, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_CONNECTION_STARTED", CONNECTION_STARTED, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_CONNECTION_MADE", CONNECTION_MADE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_CONNECTION_AWAITING_RESPONSE", CONNECTION_AWAITING_RESPONSE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_CONNECTION_AUTH_OK", CONNECTION_AUTH_OK, CONST_CS | CONST_PERSISTENT);
#ifdef CONNECTION_SSL_STARTUP
	REGISTER_LONG_CONSTANT("PGSQL_CONNECTION_SSL_STARTUP", CONNECTION_SSL_STARTUP, CONST_CS | CONST_PERSISTENT);
#endif
	REGISTER_LONG_CONSTANT("PGSQL_CONNECTION_SETENV", CONNECTION_SETENV, CONST_CS | CONST_PERSISTENT);
	/* For pg_connect_poll() */
	REGISTER_LONG_CONSTANT("PGSQL_POLLING_FAILED", PGRES_POLLING_FAILED, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_POLLING_READING", PGRES_POLLING_READING, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_POLLING_WRITING", PGRES_POLLING_WRITING, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_POLLING_OK", PGRES_POLLING_OK, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_POLLING_ACTIVE", PGRES_POLLING_ACTIVE, CONST_CS | CONST_PERSISTENT);
	/* For pg_transaction_status() */
	REGISTER_LONG_CONSTANT("PGSQL_TRANSACTION_IDLE", PQTRANS_IDLE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_TRANSACTION_ACTIVE", PQTRANS_ACTIVE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_TRANSACTION_INTRANS", PQTRANS_INTRANS, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_TRANSACTION_INERROR", PQTRANS_INERROR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_TRANSACTION_UNKNOWN", PQTRANS_UNKNOWN, CONST_CS | CONST_PERSISTENT);
	/* For pg_set_error_verbosity() */
	REGISTER_LONG_CONSTANT("PGSQL_ERRORS_TERSE", PQERRORS_TERSE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_ERRORS_DEFAULT", PQERRORS_DEFAULT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_ERRORS_VERBOSE", PQERRORS_VERBOSE, CONST_CS | CONST_PERSISTENT);
	/* For lo_seek() */
	REGISTER_LONG_CONSTANT("PGSQL_SEEK_SET", SEEK_SET, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_SEEK_CUR", SEEK_CUR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_SEEK_END", SEEK_END, CONST_CS | CONST_PERSISTENT);
	/* For pg_result_status() return value type */
	REGISTER_LONG_CONSTANT("PGSQL_STATUS_LONG", PGSQL_STATUS_LONG, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_STATUS_STRING", PGSQL_STATUS_STRING, CONST_CS | CONST_PERSISTENT);
	/* For pg_result_status() return value */
	REGISTER_LONG_CONSTANT("PGSQL_EMPTY_QUERY", PGRES_EMPTY_QUERY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_COMMAND_OK", PGRES_COMMAND_OK, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_TUPLES_OK", PGRES_TUPLES_OK, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_COPY_OUT", PGRES_COPY_OUT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_COPY_IN", PGRES_COPY_IN, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_BAD_RESPONSE", PGRES_BAD_RESPONSE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_NONFATAL_ERROR", PGRES_NONFATAL_ERROR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_FATAL_ERROR", PGRES_FATAL_ERROR, CONST_CS | CONST_PERSISTENT);
	/* For pg_result_error_field() field codes */
	REGISTER_LONG_CONSTANT("PGSQL_DIAG_SEVERITY", PG_DIAG_SEVERITY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_DIAG_SQLSTATE", PG_DIAG_SQLSTATE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_DIAG_MESSAGE_PRIMARY", PG_DIAG_MESSAGE_PRIMARY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_DIAG_MESSAGE_DETAIL", PG_DIAG_MESSAGE_DETAIL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_DIAG_MESSAGE_HINT", PG_DIAG_MESSAGE_HINT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_DIAG_STATEMENT_POSITION", PG_DIAG_STATEMENT_POSITION, CONST_CS | CONST_PERSISTENT);
#ifdef PG_DIAG_INTERNAL_POSITION
	REGISTER_LONG_CONSTANT("PGSQL_DIAG_INTERNAL_POSITION", PG_DIAG_INTERNAL_POSITION, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef PG_DIAG_INTERNAL_QUERY
	REGISTER_LONG_CONSTANT("PGSQL_DIAG_INTERNAL_QUERY", PG_DIAG_INTERNAL_QUERY, CONST_CS | CONST_PERSISTENT);
#endif
	REGISTER_LONG_CONSTANT("PGSQL_DIAG_CONTEXT", PG_DIAG_CONTEXT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_DIAG_SOURCE_FILE", PG_DIAG_SOURCE_FILE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_DIAG_SOURCE_LINE", PG_DIAG_SOURCE_LINE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_DIAG_SOURCE_FUNCTION", PG_DIAG_SOURCE_FUNCTION, CONST_CS | CONST_PERSISTENT);
#ifdef PG_DIAG_SCHEMA_NAME
	REGISTER_LONG_CONSTANT("PGSQL_DIAG_SCHEMA_NAME", PG_DIAG_SCHEMA_NAME, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef PG_DIAG_TABLE_NAME
	REGISTER_LONG_CONSTANT("PGSQL_DIAG_TABLE_NAME", PG_DIAG_TABLE_NAME, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef PG_DIAG_COLUMN_NAME
	REGISTER_LONG_CONSTANT("PGSQL_DIAG_COLUMN_NAME", PG_DIAG_COLUMN_NAME, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef PG_DIAG_DATATYPE_NAME
	REGISTER_LONG_CONSTANT("PGSQL_DIAG_DATATYPE_NAME", PG_DIAG_DATATYPE_NAME, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef PG_DIAG_CONSTRAINT_NAME
	REGISTER_LONG_CONSTANT("PGSQL_DIAG_CONSTRAINT_NAME", PG_DIAG_CONSTRAINT_NAME, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef PG_DIAG_SEVERITY_NONLOCALIZED
	REGISTER_LONG_CONSTANT("PGSQL_DIAG_SEVERITY_NONLOCALIZED", PG_DIAG_SEVERITY_NONLOCALIZED, CONST_CS | CONST_PERSISTENT);
#endif
	/* pg_convert options */
	REGISTER_LONG_CONSTANT("PGSQL_CONV_IGNORE_DEFAULT", PGSQL_CONV_IGNORE_DEFAULT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_CONV_FORCE_NULL", PGSQL_CONV_FORCE_NULL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_CONV_IGNORE_NOT_NULL", PGSQL_CONV_IGNORE_NOT_NULL, CONST_CS | CONST_PERSISTENT);
	/* pg_insert/update/delete/select options */
	REGISTER_LONG_CONSTANT("PGSQL_DML_ESCAPE", PGSQL_DML_ESCAPE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_DML_NO_CONV", PGSQL_DML_NO_CONV, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_DML_EXEC", PGSQL_DML_EXEC, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_DML_ASYNC", PGSQL_DML_ASYNC, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_DML_STRING", PGSQL_DML_STRING, CONST_CS | CONST_PERSISTENT);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION */
PHP_MSHUTDOWN_FUNCTION(pgsql)
{
	UNREGISTER_INI_ENTRIES();
	zend_hash_destroy(&PGG(notices));
	zend_hash_destroy(&PGG(hashes));

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION */
PHP_RINIT_FUNCTION(pgsql)
{
	PGG(default_link) = NULL;
	PGG(num_links) = PGG(num_persistent);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION */
PHP_RSHUTDOWN_FUNCTION(pgsql)
{
	/* clean up notice messages */
	zend_hash_clean(&PGG(notices));
	zend_hash_clean(&PGG(hashes));
	/* clean up persistent connection */
	zend_hash_apply(&EG(persistent_list), (apply_func_t) _rollback_transactions);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(pgsql)
{
	char buf[256];

	php_info_print_table_start();
	php_info_print_table_header(2, "PostgreSQL Support", "enabled");
	php_libpq_version(buf, sizeof(buf));
	php_info_print_table_row(2, "PostgreSQL (libpq) Version", buf);
#ifdef HAVE_PGSQL_WITH_MULTIBYTE_SUPPORT
	php_info_print_table_row(2, "Multibyte character support", "enabled");
#else
	php_info_print_table_row(2, "Multibyte character support", "disabled");
#endif
	snprintf(buf, sizeof(buf), ZEND_LONG_FMT, PGG(num_persistent));
	php_info_print_table_row(2, "Active Persistent Links", buf);
	snprintf(buf, sizeof(buf), ZEND_LONG_FMT, PGG(num_links));
	php_info_print_table_row(2, "Active Links", buf);
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}
/* }}} */

/* {{{ php_pgsql_do_connect */
static void php_pgsql_do_connect(INTERNAL_FUNCTION_PARAMETERS, int persistent)
{
	char *connstring;
	size_t connstring_len;
	PGconn *pgsql;
	smart_str str = {0};
	zend_long connect_type = 0;
	PGresult *pg_result;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|l", &connstring, &connstring_len, &connect_type) == FAILURE) {
		RETURN_THROWS();
	}

	smart_str_appends(&str, "pgsql");
	smart_str_appendl(&str, connstring, connstring_len);
	smart_str_appendc(&str, '_');
	/* make sure that the PGSQL_CONNECT_FORCE_NEW bit is not part of the hash so that subsequent
	 * connections can re-use this connection. See bug #39979. */
	smart_str_append_long(&str, connect_type & ~PGSQL_CONNECT_FORCE_NEW);
	smart_str_0(&str);

	if (persistent && PGG(allow_persistent)) {
		zend_resource *le;

		/* try to find if we already have this link in our persistent list */
		if ((le = zend_hash_find_ptr(&EG(persistent_list), str.s)) == NULL) {  /* we don't */
			if (PGG(max_links) != -1 && PGG(num_links) >= PGG(max_links)) {
				php_error_docref(NULL, E_WARNING,
								 "Cannot create new link. Too many open links (" ZEND_LONG_FMT ")", PGG(num_links));
				goto err;
			}
			if (PGG(max_persistent) != -1 && PGG(num_persistent) >= PGG(max_persistent)) {
				php_error_docref(NULL, E_WARNING,
								 "Cannot create new link. Too many open persistent links (" ZEND_LONG_FMT ")", PGG(num_persistent));
				goto err;
			}

			/* create the link */
			pgsql = PQconnectdb(connstring);
			if (pgsql == NULL || PQstatus(pgsql) == CONNECTION_BAD) {
				PHP_PQ_ERROR("Unable to connect to PostgreSQL server: %s", pgsql)
				if (pgsql) {
					PQfinish(pgsql);
				}
				goto err;
			}

			/* hash it up */
			if (zend_register_persistent_resource(ZSTR_VAL(str.s), ZSTR_LEN(str.s), pgsql, le_plink) == NULL) {
				goto err;
			}
			PGG(num_links)++;
			PGG(num_persistent)++;
		} else {  /* we do */
			if (le->type != le_plink) {
				goto err;
			}
			/* ensure that the link did not die */
			if (PGG(auto_reset_persistent) & 1) {
				/* need to send & get something from backend to
				   make sure we catch CONNECTION_BAD every time */
				PGresult *pg_result;
				pg_result = PQexec(le->ptr, "select 1");
				PQclear(pg_result);
			}
			if (PQstatus(le->ptr) == CONNECTION_BAD) { /* the link died */
				if (le->ptr == NULL) {
					le->ptr = PQconnectdb(connstring);
				}
				else {
					PQreset(le->ptr);
				}
				if (le->ptr == NULL || PQstatus(le->ptr) == CONNECTION_BAD) {
					php_error_docref(NULL, E_WARNING,"PostgreSQL link lost, unable to reconnect");
					zend_hash_del(&EG(persistent_list), str.s);
					goto err;
				}
			}
			pgsql = (PGconn *) le->ptr;
			if (PQprotocolVersion(pgsql) >= 3 && atof(PQparameterStatus(pgsql, "server_version")) >= 7.2) {
				pg_result = PQexec(pgsql, "RESET ALL;");
				PQclear(pg_result);
			}
		}
		RETVAL_RES(zend_register_resource(pgsql, le_plink));
	} else { /* Non persistent connection */
		zend_resource *index_ptr, new_index_ptr;

		/* first we check the hash for the hashed_details key.  if it exists,
		 * it should point us to the right offset where the actual pgsql link sits.
		 * if it doesn't, open a new pgsql link, add it to the resource list,
		 * and add a pointer to it with hashed_details as the key.
		 */
		if (!(connect_type & PGSQL_CONNECT_FORCE_NEW)
			&& (index_ptr = zend_hash_find_ptr(&EG(regular_list), str.s)) != NULL) {
			zend_resource *link;

			if (index_ptr->type != le_index_ptr) {
				goto err;
			}

			link = (zend_resource *)index_ptr->ptr;
			ZEND_ASSERT(link->ptr && (link->type == le_link || link->type == le_plink));
			php_pgsql_set_default_link(link);
			GC_ADDREF(link);
			RETVAL_RES(link);
			goto cleanup;
		}
		if (PGG(max_links) != -1 && PGG(num_links) >= PGG(max_links)) {
			php_error_docref(NULL, E_WARNING, "Cannot create new link. Too many open links (" ZEND_LONG_FMT ")", PGG(num_links));
			goto err;
		}

		/* Non-blocking connect */
		if (connect_type & PGSQL_CONNECT_ASYNC) {
			pgsql = PQconnectStart(connstring);
			if (pgsql==NULL || PQstatus(pgsql)==CONNECTION_BAD) {
				PHP_PQ_ERROR("Unable to connect to PostgreSQL server: %s", pgsql);
				if (pgsql) {
					PQfinish(pgsql);
				}
				goto err;
			}
		} else {
			pgsql = PQconnectdb(connstring);
			if (pgsql==NULL || PQstatus(pgsql)==CONNECTION_BAD) {
				PHP_PQ_ERROR("Unable to connect to PostgreSQL server: %s", pgsql);
				if (pgsql) {
					PQfinish(pgsql);
				}
				goto err;
			}
		}

		/* add it to the list */
		RETVAL_RES(zend_register_resource(pgsql, le_link));

		/* add it to the hash */
		new_index_ptr.ptr = (void *) Z_RES_P(return_value);
		new_index_ptr.type = le_index_ptr;
		zend_hash_update_mem(&EG(regular_list), str.s, (void *) &new_index_ptr, sizeof(zend_resource));

		/* Keep track of link => hash mapping, so we can remove the hash entry from regular_list
		 * when the connection is closed. This uses the address of the connection rather than the
		 * zend_resource, because the resource destructor is passed a stack copy of the resource
		 * structure. */
		{
			zval tmp;
			ZVAL_STR_COPY(&tmp, str.s);
			zend_hash_index_update(&PGG(hashes), (uintptr_t) pgsql, &tmp);
		}
		PGG(num_links)++;
	}
	/* set notice processor */
	if (! PGG(ignore_notices) && Z_TYPE_P(return_value) == IS_RESOURCE) {
		PQsetNoticeProcessor(pgsql, _php_pgsql_notice_handler, (void*)(zend_uintptr_t)Z_RES_HANDLE_P(return_value));
	}
	php_pgsql_set_default_link(Z_RES_P(return_value));

cleanup:
	smart_str_free(&str);
	return;

err:
	smart_str_free(&str);
	RETURN_FALSE;
}
/* }}} */

/* {{{ Open a PostgreSQL connection */
PHP_FUNCTION(pg_connect)
{
	php_pgsql_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU,0);
}
/* }}} */

/* {{{ Poll the status of an in-progress async PostgreSQL connection attempt*/
PHP_FUNCTION(pg_connect_poll)
{
	zval *pgsql_link;
	PGconn *pgsql;
	int ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &pgsql_link) == FAILURE) {
		RETURN_THROWS();
	}

	if ((pgsql = (PGconn *)zend_fetch_resource2(Z_RES_P(pgsql_link), "PostgreSQL link", le_link, le_plink)) == NULL) {
		RETURN_THROWS();
	}

	ret = PQconnectPoll(pgsql);

	RETURN_LONG(ret);
}
/* }}} */

/* {{{ Open a persistent PostgreSQL connection */
PHP_FUNCTION(pg_pconnect)
{
	php_pgsql_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU,1);
}
/* }}} */

/* {{{ Close a PostgreSQL connection */
PHP_FUNCTION(pg_close)
{
	zval *pgsql_link = NULL;
	zend_resource *link;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|r!", &pgsql_link) == FAILURE) {
		RETURN_THROWS();
	}

	if (!pgsql_link) {
		link = PGG(default_link);
		CHECK_DEFAULT_LINK(link);
		zend_list_delete(link);
		PGG(default_link) = NULL;
		RETURN_TRUE;
	}

	link = Z_RES_P(pgsql_link);
	if (zend_fetch_resource2(link, "PostgreSQL link", le_link, le_plink) == NULL) {
		RETURN_THROWS();
	}

	if (link == PGG(default_link)) {
		zend_list_delete(link);
		PGG(default_link) = NULL;
	}
	zend_list_close(link);

	RETURN_TRUE;
}
/* }}} */

#define PHP_PG_DBNAME 1
#define PHP_PG_ERROR_MESSAGE 2
#define PHP_PG_OPTIONS 3
#define PHP_PG_PORT 4
#define PHP_PG_TTY 5
#define PHP_PG_HOST 6
#define PHP_PG_VERSION 7

/* {{{ php_pgsql_get_link_info */
static void php_pgsql_get_link_info(INTERNAL_FUNCTION_PARAMETERS, int entry_type)
{
	zend_resource *link;
	zval *pgsql_link = NULL;
	PGconn *pgsql;
	char *msgbuf;
	char *result;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|r!", &pgsql_link) == FAILURE) {
		RETURN_THROWS();
	}

	if (!pgsql_link) {
		link = FETCH_DEFAULT_LINK();
		CHECK_DEFAULT_LINK(link);
	} else {
		link = Z_RES_P(pgsql_link);
	}

	if ((pgsql = (PGconn *)zend_fetch_resource2(link, "PostgreSQL link", le_link, le_plink)) == NULL) {
		RETURN_THROWS();
	}

	switch(entry_type) {
		case PHP_PG_DBNAME:
			result = PQdb(pgsql);
			break;
		case PHP_PG_ERROR_MESSAGE:
			result = PQErrorMessageTrim(pgsql, &msgbuf);
			RETVAL_STRING(result);
			efree(result);
			return;
		case PHP_PG_OPTIONS:
			result = PQoptions(pgsql);
			break;
		case PHP_PG_PORT:
			result = PQport(pgsql);
			break;
		case PHP_PG_TTY:
			result = PQtty(pgsql);
			break;
		case PHP_PG_HOST:
			result = PQhost(pgsql);
			break;
		case PHP_PG_VERSION:
			array_init(return_value);
			char buf[16];
			php_libpq_version(buf, sizeof(buf));
			add_assoc_string(return_value, "client", buf);
			add_assoc_long(return_value, "protocol", PQprotocolVersion(pgsql));
			if (PQprotocolVersion(pgsql) >= 3) {
				/* 8.0 or grater supports protorol version 3 */
				char *tmp;
				add_assoc_string(return_value, "server", (char*)PQparameterStatus(pgsql, "server_version"));

#define PHP_PQ_COPY_PARAM(_x) tmp = (char*)PQparameterStatus(pgsql, _x); \
	if(tmp) add_assoc_string(return_value, _x, tmp); \
	else add_assoc_null(return_value, _x);

				PHP_PQ_COPY_PARAM("server_encoding");
				PHP_PQ_COPY_PARAM("client_encoding");
				PHP_PQ_COPY_PARAM("is_superuser");
				PHP_PQ_COPY_PARAM("session_authorization");
				PHP_PQ_COPY_PARAM("DateStyle");
				PHP_PQ_COPY_PARAM("IntervalStyle");
				PHP_PQ_COPY_PARAM("TimeZone");
				PHP_PQ_COPY_PARAM("integer_datetimes");
				PHP_PQ_COPY_PARAM("standard_conforming_strings");
				PHP_PQ_COPY_PARAM("application_name");
			}
			return;
		EMPTY_SWITCH_DEFAULT_CASE()
	}
	if (result) {
		RETURN_STRING(result);
	} else {
		RETURN_EMPTY_STRING();
	}
}
/* }}} */

/* {{{ Get the database name */
PHP_FUNCTION(pg_dbname)
{
	php_pgsql_get_link_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP_PG_DBNAME);
}
/* }}} */

/* {{{ Get the error message string */
PHP_FUNCTION(pg_last_error)
{
	php_pgsql_get_link_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP_PG_ERROR_MESSAGE);
}
/* }}} */

/* {{{ Get the options associated with the connection */
PHP_FUNCTION(pg_options)
{
	php_pgsql_get_link_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP_PG_OPTIONS);
}
/* }}} */

/* {{{ Return the port number associated with the connection */
PHP_FUNCTION(pg_port)
{
	php_pgsql_get_link_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP_PG_PORT);
}
/* }}} */

/* {{{ Return the tty name associated with the connection */
PHP_FUNCTION(pg_tty)
{
	php_pgsql_get_link_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP_PG_TTY);
}
/* }}} */

/* {{{ Returns the host name associated with the connection */
PHP_FUNCTION(pg_host)
{
	php_pgsql_get_link_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP_PG_HOST);
}
/* }}} */

/* {{{ Returns an array with client, protocol and server version (when available) */
PHP_FUNCTION(pg_version)
{
	php_pgsql_get_link_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP_PG_VERSION);
}
/* }}} */

/* {{{ Returns the value of a server parameter */
PHP_FUNCTION(pg_parameter_status)
{
	zval *pgsql_link = NULL;
	zend_resource *link;
	PGconn *pgsql;
	char *param;
	size_t len;

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS(), "rs", &pgsql_link, &param, &len) == FAILURE) {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &param, &len) == FAILURE) {
			RETURN_THROWS();
		}

		link = FETCH_DEFAULT_LINK();
		CHECK_DEFAULT_LINK(link);
	} else {
		link = Z_RES_P(pgsql_link);
	}

	if ((pgsql = (PGconn *)zend_fetch_resource2(link, "PostgreSQL link", le_link, le_plink)) == NULL) {
		RETURN_THROWS();
	}

	param = (char*)PQparameterStatus(pgsql, param);
	if (param) {
		RETURN_STRING(param);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ Ping database. If connection is bad, try to reconnect. */
PHP_FUNCTION(pg_ping)
{
	zval *pgsql_link = NULL;
	PGconn *pgsql;
	PGresult *res;
	zend_resource *link;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|r!", &pgsql_link) == FAILURE) {
		RETURN_THROWS();
	}

	if (pgsql_link == NULL) {
		link = FETCH_DEFAULT_LINK();
		CHECK_DEFAULT_LINK(link);
	} else {
		link = Z_RES_P(pgsql_link);
	}

	if ((pgsql = (PGconn *)zend_fetch_resource2(link, "PostgreSQL link", le_link, le_plink)) == NULL) {
		RETURN_THROWS();
	}

	/* ping connection */
	res = PQexec(pgsql, "SELECT 1;");
	PQclear(res);

	/* check status. */
	if (PQstatus(pgsql) == CONNECTION_OK)
		RETURN_TRUE;

	/* reset connection if it's broken */
	PQreset(pgsql);
	if (PQstatus(pgsql) == CONNECTION_OK) {
		RETURN_TRUE;
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ Execute a query */
PHP_FUNCTION(pg_query)
{
	zval *pgsql_link = NULL;
	char *query;
	int  argc = ZEND_NUM_ARGS();
	size_t query_len;
	int leftover = 0;
	zend_resource *link;
	PGconn *pgsql;
	PGresult *pgsql_result;
	ExecStatusType status;

	if (argc == 1) {
		if (zend_parse_parameters(argc, "s", &query, &query_len) == FAILURE) {
			RETURN_THROWS();
		}
		link = FETCH_DEFAULT_LINK();
		CHECK_DEFAULT_LINK(link);
	} else {
		if (zend_parse_parameters(argc, "rs", &pgsql_link, &query, &query_len) == FAILURE) {
			RETURN_THROWS();
		}
		link = Z_RES_P(pgsql_link);
	}

	if ((pgsql = (PGconn *)zend_fetch_resource2(link, "PostgreSQL link", le_link, le_plink)) == NULL) {
		RETURN_THROWS();
	}

	if (PQsetnonblocking(pgsql, 0)) {
		php_error_docref(NULL, E_NOTICE,"Cannot set connection to blocking mode");
		RETURN_FALSE;
	}
	while ((pgsql_result = PQgetResult(pgsql))) {
		PQclear(pgsql_result);
		leftover = 1;
	}
	if (leftover) {
		php_error_docref(NULL, E_NOTICE, "Found results on this connection. Use pg_get_result() to get these results first");
	}
	pgsql_result = PQexec(pgsql, query);
	if ((PGG(auto_reset_persistent) & 2) && PQstatus(pgsql) != CONNECTION_OK) {
		PQclear(pgsql_result);
		PQreset(pgsql);
		pgsql_result = PQexec(pgsql, query);
	}

	if (pgsql_result) {
		status = PQresultStatus(pgsql_result);
	} else {
		status = (ExecStatusType) PQstatus(pgsql);
	}

	switch (status) {
		case PGRES_EMPTY_QUERY:
		case PGRES_BAD_RESPONSE:
		case PGRES_NONFATAL_ERROR:
		case PGRES_FATAL_ERROR:
			PHP_PQ_ERROR("Query failed: %s", pgsql);
			PQclear(pgsql_result);
			RETURN_FALSE;
			break;
		case PGRES_COMMAND_OK: /* successful command that did not return rows */
		default:
			if (pgsql_result) {
				pgsql_result_handle *pg_result = (pgsql_result_handle *) emalloc(sizeof(pgsql_result_handle));
				pg_result->conn = pgsql;
				pg_result->result = pgsql_result;
				pg_result->row = 0;
				RETURN_RES(zend_register_resource(pg_result, le_result));
			} else {
				PQclear(pgsql_result);
				RETURN_FALSE;
			}
			break;
	}
}
/* }}} */

/* {{{ _php_pgsql_free_params */
static void _php_pgsql_free_params(char **params, int num_params)
{
	if (num_params > 0) {
		int i;
		for (i = 0; i < num_params; i++) {
			if (params[i]) {
				efree(params[i]);
			}
		}
		efree(params);
	}
}
/* }}} */

/* {{{ Execute a query */
PHP_FUNCTION(pg_query_params)
{
	zval *pgsql_link = NULL;
	zval *pv_param_arr, *tmp;
	char *query;
	size_t query_len;
	int argc = ZEND_NUM_ARGS();
	int leftover = 0;
	int num_params = 0;
	char **params = NULL;
	zend_resource *link;
	PGconn *pgsql;
	PGresult *pgsql_result;
	ExecStatusType status;
	pgsql_result_handle *pg_result;

	if (argc == 2) {
		if (zend_parse_parameters(argc, "sa", &query, &query_len, &pv_param_arr) == FAILURE) {
			RETURN_THROWS();
		}
		link = FETCH_DEFAULT_LINK();
		CHECK_DEFAULT_LINK(link);
	} else {
		if (zend_parse_parameters(argc, "rsa", &pgsql_link, &query, &query_len, &pv_param_arr) == FAILURE) {
			RETURN_THROWS();
		}
		link = Z_RES_P(pgsql_link);
	}

	if ((pgsql = (PGconn *)zend_fetch_resource2(link, "PostgreSQL link", le_link, le_plink)) == NULL) {
		RETURN_THROWS();
	}

	if (PQsetnonblocking(pgsql, 0)) {
		php_error_docref(NULL, E_NOTICE,"Cannot set connection to blocking mode");
		RETURN_FALSE;
	}
	while ((pgsql_result = PQgetResult(pgsql))) {
		PQclear(pgsql_result);
		leftover = 1;
	}
	if (leftover) {
		php_error_docref(NULL, E_NOTICE, "Found results on this connection. Use pg_get_result() to get these results first");
	}

	num_params = zend_hash_num_elements(Z_ARRVAL_P(pv_param_arr));
	if (num_params > 0) {
		int i = 0;
		params = (char **)safe_emalloc(sizeof(char *), num_params, 0);

		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(pv_param_arr), tmp) {
			ZVAL_DEREF(tmp);
			if (Z_TYPE_P(tmp) == IS_NULL) {
				params[i] = NULL;
			} else {
				zend_string *param_str = zval_try_get_string(tmp);
				if (!param_str) {
					_php_pgsql_free_params(params, num_params);
					RETURN_THROWS();
				}
				params[i] = estrndup(ZSTR_VAL(param_str), ZSTR_LEN(param_str));
				zend_string_release(param_str);
			}
			i++;
		} ZEND_HASH_FOREACH_END();
	}

	pgsql_result = PQexecParams(pgsql, query, num_params,
					NULL, (const char * const *)params, NULL, NULL, 0);
	if ((PGG(auto_reset_persistent) & 2) && PQstatus(pgsql) != CONNECTION_OK) {
		PQclear(pgsql_result);
		PQreset(pgsql);
		pgsql_result = PQexecParams(pgsql, query, num_params,
						NULL, (const char * const *)params, NULL, NULL, 0);
	}

	if (pgsql_result) {
		status = PQresultStatus(pgsql_result);
	} else {
		status = (ExecStatusType) PQstatus(pgsql);
	}

	_php_pgsql_free_params(params, num_params);

	switch (status) {
		case PGRES_EMPTY_QUERY:
		case PGRES_BAD_RESPONSE:
		case PGRES_NONFATAL_ERROR:
		case PGRES_FATAL_ERROR:
			PHP_PQ_ERROR("Query failed: %s", pgsql);
			PQclear(pgsql_result);
			RETURN_FALSE;
			break;
		case PGRES_COMMAND_OK: /* successful command that did not return rows */
		default:
			if (pgsql_result) {
				pg_result = (pgsql_result_handle *) emalloc(sizeof(pgsql_result_handle));
				pg_result->conn = pgsql;
				pg_result->result = pgsql_result;
				pg_result->row = 0;
				RETURN_RES(zend_register_resource(pg_result, le_result));
			} else {
				PQclear(pgsql_result);
				RETURN_FALSE;
			}
			break;
	}
}
/* }}} */

/* {{{ Prepare a query for future execution */
PHP_FUNCTION(pg_prepare)
{
	zval *pgsql_link = NULL;
	char *query, *stmtname;
	size_t query_len, stmtname_len;
	int argc = ZEND_NUM_ARGS();
	int leftover = 0;
	PGconn *pgsql;
	zend_resource *link;
	PGresult *pgsql_result;
	ExecStatusType status;
	pgsql_result_handle *pg_result;

	if (argc == 2) {
		if (zend_parse_parameters(argc, "ss", &stmtname, &stmtname_len, &query, &query_len) == FAILURE) {
			RETURN_THROWS();
		}
		link = FETCH_DEFAULT_LINK();
		CHECK_DEFAULT_LINK(link);
	} else {
		if (zend_parse_parameters(argc, "rss", &pgsql_link, &stmtname, &stmtname_len, &query, &query_len) == FAILURE) {
			RETURN_THROWS();
		}
		link = Z_RES_P(pgsql_link);
	}

	if ((pgsql = (PGconn *)zend_fetch_resource2(link, "PostgreSQL link", le_link, le_plink)) == NULL) {
		RETURN_THROWS();
	}

	if (PQsetnonblocking(pgsql, 0)) {
		php_error_docref(NULL, E_NOTICE,"Cannot set connection to blocking mode");
		RETURN_FALSE;
	}
	while ((pgsql_result = PQgetResult(pgsql))) {
		PQclear(pgsql_result);
		leftover = 1;
	}
	if (leftover) {
		php_error_docref(NULL, E_NOTICE, "Found results on this connection. Use pg_get_result() to get these results first");
	}
	pgsql_result = PQprepare(pgsql, stmtname, query, 0, NULL);
	if ((PGG(auto_reset_persistent) & 2) && PQstatus(pgsql) != CONNECTION_OK) {
		PQclear(pgsql_result);
		PQreset(pgsql);
		pgsql_result = PQprepare(pgsql, stmtname, query, 0, NULL);
	}

	if (pgsql_result) {
		status = PQresultStatus(pgsql_result);
	} else {
		status = (ExecStatusType) PQstatus(pgsql);
	}

	switch (status) {
		case PGRES_EMPTY_QUERY:
		case PGRES_BAD_RESPONSE:
		case PGRES_NONFATAL_ERROR:
		case PGRES_FATAL_ERROR:
			PHP_PQ_ERROR("Query failed: %s", pgsql);
			PQclear(pgsql_result);
			RETURN_FALSE;
			break;
		case PGRES_COMMAND_OK: /* successful command that did not return rows */
		default:
			if (pgsql_result) {
				pg_result = (pgsql_result_handle *) emalloc(sizeof(pgsql_result_handle));
				pg_result->conn = pgsql;
				pg_result->result = pgsql_result;
				pg_result->row = 0;
				RETURN_RES(zend_register_resource(pg_result, le_result));
			} else {
				PQclear(pgsql_result);
				RETURN_FALSE;
			}
			break;
	}
}
/* }}} */

/* {{{ Execute a prepared query  */
PHP_FUNCTION(pg_execute)
{
	zval *pgsql_link = NULL;
	zval *pv_param_arr, *tmp;
	char *stmtname;
	size_t stmtname_len;
	int argc = ZEND_NUM_ARGS();
	int leftover = 0;
	int num_params = 0;
	char **params = NULL;
	PGconn *pgsql;
	zend_resource *link;
	PGresult *pgsql_result;
	ExecStatusType status;
	pgsql_result_handle *pg_result;

	if (argc == 2) {
		if (zend_parse_parameters(argc, "sa", &stmtname, &stmtname_len, &pv_param_arr)==FAILURE) {
			RETURN_THROWS();
		}
		link = FETCH_DEFAULT_LINK();
		CHECK_DEFAULT_LINK(link);
	} else {
		if (zend_parse_parameters(argc, "rsa", &pgsql_link, &stmtname, &stmtname_len, &pv_param_arr) == FAILURE) {
			RETURN_THROWS();
		}
		link = Z_RES_P(pgsql_link);
	}

	if ((pgsql = (PGconn *)zend_fetch_resource2(link, "PostgreSQL link", le_link, le_plink)) == NULL) {
		RETURN_THROWS();
	}

	if (PQsetnonblocking(pgsql, 0)) {
		php_error_docref(NULL, E_NOTICE,"Cannot set connection to blocking mode");
		RETURN_FALSE;
	}
	while ((pgsql_result = PQgetResult(pgsql))) {
		PQclear(pgsql_result);
		leftover = 1;
	}
	if (leftover) {
		php_error_docref(NULL, E_NOTICE, "Found results on this connection. Use pg_get_result() to get these results first");
	}

	num_params = zend_hash_num_elements(Z_ARRVAL_P(pv_param_arr));
	if (num_params > 0) {
		int i = 0;
		params = (char **)safe_emalloc(sizeof(char *), num_params, 0);

		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(pv_param_arr), tmp) {

			if (Z_TYPE_P(tmp) == IS_NULL) {
				params[i] = NULL;
			} else {
				zend_string *tmp_str;
				zend_string *str = zval_get_tmp_string(tmp, &tmp_str);

				params[i] = estrndup(ZSTR_VAL(str), ZSTR_LEN(str));
				zend_tmp_string_release(tmp_str);
			}

			i++;
		} ZEND_HASH_FOREACH_END();
	}

	pgsql_result = PQexecPrepared(pgsql, stmtname, num_params,
					(const char * const *)params, NULL, NULL, 0);
	if ((PGG(auto_reset_persistent) & 2) && PQstatus(pgsql) != CONNECTION_OK) {
		PQclear(pgsql_result);
		PQreset(pgsql);
		pgsql_result = PQexecPrepared(pgsql, stmtname, num_params,
						(const char * const *)params, NULL, NULL, 0);
	}

	if (pgsql_result) {
		status = PQresultStatus(pgsql_result);
	} else {
		status = (ExecStatusType) PQstatus(pgsql);
	}

	_php_pgsql_free_params(params, num_params);

	switch (status) {
		case PGRES_EMPTY_QUERY:
		case PGRES_BAD_RESPONSE:
		case PGRES_NONFATAL_ERROR:
		case PGRES_FATAL_ERROR:
			PHP_PQ_ERROR("Query failed: %s", pgsql);
			PQclear(pgsql_result);
			RETURN_FALSE;
			break;
		case PGRES_COMMAND_OK: /* successful command that did not return rows */
		default:
			if (pgsql_result) {
				pg_result = (pgsql_result_handle *) emalloc(sizeof(pgsql_result_handle));
				pg_result->conn = pgsql;
				pg_result->result = pgsql_result;
				pg_result->row = 0;
				RETURN_RES(zend_register_resource(pg_result, le_result));
			} else {
				PQclear(pgsql_result);
				RETURN_FALSE;
			}
			break;
	}
}
/* }}} */

#define PHP_PG_NUM_ROWS 1
#define PHP_PG_NUM_FIELDS 2
#define PHP_PG_CMD_TUPLES 3

/* {{{ php_pgsql_get_result_info */
static void php_pgsql_get_result_info(INTERNAL_FUNCTION_PARAMETERS, int entry_type)
{
	zval *result;
	PGresult *pgsql_result;
	pgsql_result_handle *pg_result;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &result) == FAILURE) {
		RETURN_THROWS();
	}

	if ((pg_result = (pgsql_result_handle *)zend_fetch_resource(Z_RES_P(result), "PostgreSQL result", le_result)) == NULL) {
		RETURN_THROWS();
	}

	pgsql_result = pg_result->result;

	switch (entry_type) {
		case PHP_PG_NUM_ROWS:
			RETVAL_LONG(PQntuples(pgsql_result));
			break;
		case PHP_PG_NUM_FIELDS:
			RETVAL_LONG(PQnfields(pgsql_result));
			break;
		case PHP_PG_CMD_TUPLES:
			RETVAL_LONG(atoi(PQcmdTuples(pgsql_result)));
			break;
		EMPTY_SWITCH_DEFAULT_CASE()
	}
}
/* }}} */

/* {{{ Return the number of rows in the result */
PHP_FUNCTION(pg_num_rows)
{
	php_pgsql_get_result_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP_PG_NUM_ROWS);
}
/* }}} */

/* {{{ Return the number of fields in the result */
PHP_FUNCTION(pg_num_fields)
{
	php_pgsql_get_result_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP_PG_NUM_FIELDS);
}
/* }}} */

/* {{{ Returns the number of affected tuples */
PHP_FUNCTION(pg_affected_rows)
{
	php_pgsql_get_result_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP_PG_CMD_TUPLES);
}
/* }}} */

/* {{{ Returns the last notice set by the backend */
PHP_FUNCTION(pg_last_notice)
{
	zval *pgsql_link = NULL;
	zval *notice, *notices;
	PGconn *pg_link;
	zend_long option = PGSQL_NOTICE_LAST;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r|l", &pgsql_link, &option) == FAILURE) {
		RETURN_THROWS();
	}

	/* Just to check if user passed valid resoruce */
	if ((pg_link = (PGconn *)zend_fetch_resource2(Z_RES_P(pgsql_link), "PostgreSQL link", le_link, le_plink)) == NULL) {
		RETURN_THROWS();
	}

	notices = zend_hash_index_find(&PGG(notices), (zend_ulong)Z_RES_HANDLE_P(pgsql_link));
	switch (option) {
		case PGSQL_NOTICE_LAST:
			if (notices) {
				zend_hash_internal_pointer_end(Z_ARRVAL_P(notices));
				if ((notice = zend_hash_get_current_data(Z_ARRVAL_P(notices))) == NULL) {
					RETURN_EMPTY_STRING();
				}
				RETURN_COPY(notice);
			} else {
				RETURN_EMPTY_STRING();
			}
			break;
		case PGSQL_NOTICE_ALL:
			if (notices) {
				RETURN_COPY(notices);
			} else {
				array_init(return_value);
				return;
			}
			break;
		case PGSQL_NOTICE_CLEAR:
			if (notices) {
				zend_hash_clean(&PGG(notices));
			}
			RETURN_TRUE;
			break;
		default:
			zend_argument_value_error(2, "must be one of PGSQL_NOTICE_LAST, PGSQL_NOTICE_ALL, or PGSQL_NOTICE_CLEAR");
			RETURN_THROWS();
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ get_field_name */
static char *get_field_name(PGconn *pgsql, Oid oid, HashTable *list)
{
	smart_str str = {0};
	zend_resource *field_type;
	char *ret=NULL;

	/* try to lookup the type in the resource list */
	smart_str_appends(&str, "pgsql_oid_");
	smart_str_append_unsigned(&str, oid);
	smart_str_0(&str);

	if ((field_type = zend_hash_find_ptr(list, str.s)) != NULL) {
		ret = estrdup((char *)field_type->ptr);
	} else { /* hash all oid's */
		int i, num_rows;
		int oid_offset,name_offset;
		char *tmp_oid, *end_ptr, *tmp_name;
		zend_resource new_oid_entry;
		PGresult *result;

		if ((result = PQexec(pgsql, "select oid,typname from pg_type")) == NULL || PQresultStatus(result) != PGRES_TUPLES_OK) {
			if (result) {
				PQclear(result);
			}
			smart_str_free(&str);
			return estrndup("", sizeof("") - 1);
		}
		num_rows = PQntuples(result);
		oid_offset = PQfnumber(result,"oid");
		name_offset = PQfnumber(result,"typname");

		for (i=0; i<num_rows; i++) {
			if ((tmp_oid = PQgetvalue(result,i,oid_offset))==NULL) {
				continue;
			}

			smart_str_free(&str);
			smart_str_appends(&str, "pgsql_oid_");
			smart_str_appends(&str, tmp_oid);
			smart_str_0(&str);

			if ((tmp_name = PQgetvalue(result,i,name_offset))==NULL) {
				continue;
			}
			new_oid_entry.type = le_string;
			new_oid_entry.ptr = estrdup(tmp_name);
			zend_hash_update_mem(list, str.s, (void *) &new_oid_entry, sizeof(zend_resource));
			if (!ret && strtoul(tmp_oid, &end_ptr, 10)==oid) {
				ret = estrdup(tmp_name);
			}
		}
		PQclear(result);
	}

	smart_str_free(&str);
	return ret;
}
/* }}} */

/* {{{ Returns the name of the table field belongs to, or table's oid if oid_only is true */
PHP_FUNCTION(pg_field_table)
{
	zval *result;
	pgsql_result_handle *pg_result;
	zend_long fnum = -1;
	zend_bool return_oid = 0;
	Oid oid;
	smart_str hash_key = {0};
	char *table_name;
	zend_resource *field_table;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rl|b", &result, &fnum, &return_oid) == FAILURE) {
		RETURN_THROWS();
	}

	if ((pg_result = (pgsql_result_handle *)zend_fetch_resource(Z_RES_P(result), "PostgreSQL result", le_result)) == NULL) {
		RETURN_THROWS();
	}

	if (fnum < 0) {
		zend_argument_value_error(2, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	if (fnum >= PQnfields(pg_result->result)) {
		zend_argument_value_error(2, "must be less than the number of fields for this result set");
		RETURN_THROWS();
	}

	oid = PQftable(pg_result->result, (int)fnum);

	if (InvalidOid == oid) {
		RETURN_FALSE;
	}

	if (return_oid) {
		PGSQL_RETURN_OID(oid);
	}

	/* try to lookup the table name in the resource list */
	smart_str_appends(&hash_key, "pgsql_table_oid_");
	smart_str_append_unsigned(&hash_key, oid);
	smart_str_0(&hash_key);

	if ((field_table = zend_hash_find_ptr(&EG(regular_list), hash_key.s)) != NULL) {
		smart_str_free(&hash_key);
		RETURN_STRING((char *)field_table->ptr);
	} else { /* Not found, lookup by querying PostgreSQL system tables */
		PGresult *tmp_res;
		smart_str querystr = {0};
		zend_resource new_field_table;

		smart_str_appends(&querystr, "select relname from pg_class where oid=");
		smart_str_append_unsigned(&querystr, oid);
		smart_str_0(&querystr);

		if ((tmp_res = PQexec(pg_result->conn, ZSTR_VAL(querystr.s))) == NULL || PQresultStatus(tmp_res) != PGRES_TUPLES_OK) {
			if (tmp_res) {
				PQclear(tmp_res);
			}
			smart_str_free(&querystr);
			smart_str_free(&hash_key);
			RETURN_FALSE;
		}

		smart_str_free(&querystr);

		if ((table_name = PQgetvalue(tmp_res, 0, 0)) == NULL) {
			PQclear(tmp_res);
			smart_str_free(&hash_key);
			RETURN_FALSE;
		}

		new_field_table.type = le_string;
		new_field_table.ptr = estrdup(table_name);
		zend_hash_update_mem(&EG(regular_list), hash_key.s, (void *)&new_field_table, sizeof(zend_resource));

		smart_str_free(&hash_key);
		PQclear(tmp_res);
		RETURN_STRING(table_name);
	}

}
/* }}} */

#define PHP_PG_FIELD_NAME 1
#define PHP_PG_FIELD_SIZE 2
#define PHP_PG_FIELD_TYPE 3
#define PHP_PG_FIELD_TYPE_OID 4

/* {{{ php_pgsql_get_field_info */
static void php_pgsql_get_field_info(INTERNAL_FUNCTION_PARAMETERS, int entry_type)
{
	zval *result;
	zend_long field;
	PGresult *pgsql_result;
	pgsql_result_handle *pg_result;
	Oid oid;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rl", &result, &field) == FAILURE) {
		RETURN_THROWS();
	}

	if ((pg_result = (pgsql_result_handle *)zend_fetch_resource(Z_RES_P(result), "PostgreSQL result", le_result)) == NULL) {
		RETURN_THROWS();
	}

	if (field < 0) {
		zend_argument_value_error(2, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	pgsql_result = pg_result->result;

	if (field >= PQnfields(pgsql_result)) {
		zend_argument_value_error(2, "must be less than the number of fields for this result set");
		RETURN_THROWS();
	}

	switch (entry_type) {
		case PHP_PG_FIELD_NAME:
			RETURN_STRING(PQfname(pgsql_result, (int)field));
			break;
		case PHP_PG_FIELD_SIZE:
			RETURN_LONG(PQfsize(pgsql_result, (int)field));
			break;
		case PHP_PG_FIELD_TYPE: {
				char *name = get_field_name(pg_result->conn, PQftype(pgsql_result, (int)field), &EG(regular_list));
				RETVAL_STRING(name);
				efree(name);
			}
			break;
		case PHP_PG_FIELD_TYPE_OID:

			oid = PQftype(pgsql_result, (int)field);
			PGSQL_RETURN_OID(oid);
			break;
		EMPTY_SWITCH_DEFAULT_CASE()
	}
}
/* }}} */

/* {{{ Returns the name of the field */
PHP_FUNCTION(pg_field_name)
{
	php_pgsql_get_field_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP_PG_FIELD_NAME);
}
/* }}} */

/* {{{ Returns the internal size of the field */
PHP_FUNCTION(pg_field_size)
{
	php_pgsql_get_field_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP_PG_FIELD_SIZE);
}
/* }}} */

/* {{{ Returns the type name for the given field */
PHP_FUNCTION(pg_field_type)
{
	php_pgsql_get_field_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP_PG_FIELD_TYPE);
}
/* }}} */

/* {{{ Returns the type oid for the given field */
PHP_FUNCTION(pg_field_type_oid)
{
	php_pgsql_get_field_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP_PG_FIELD_TYPE_OID);
}
/* }}} */

/* {{{ Returns the field number of the named field */
PHP_FUNCTION(pg_field_num)
{
	zval *result;
	char *field;
	size_t field_len;
	PGresult *pgsql_result;
	pgsql_result_handle *pg_result;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rs", &result, &field, &field_len) == FAILURE) {
		RETURN_THROWS();
	}

	if ((pg_result = (pgsql_result_handle *)zend_fetch_resource(Z_RES_P(result), "PostgreSQL result", le_result)) == NULL) {
		RETURN_THROWS();
	}

	pgsql_result = pg_result->result;

	RETURN_LONG(PQfnumber(pgsql_result, field));
}
/* }}} */

static zend_long field_arg_to_offset(
		PGresult *result, zend_string *field_name, zend_long field_offset, int arg_num) {
	if (field_name) {
		field_offset = PQfnumber(result, ZSTR_VAL(field_name));
		if (field_offset < 0) {
			/* Avoid displaying the argument name, as the signature is overloaded and the name
			 * might not line up. */
			zend_value_error("Argument #%d must be a field name from this result set", arg_num);
			return -1;
		}
	} else {
		if (field_offset < 0) {
			zend_value_error("Argument #%d must be greater than or equal to 0", arg_num);
			return -1;
		}
		if (field_offset >= PQnfields(result)) {
			zend_value_error("Argument #%d must be less than the number of fields for this result set", arg_num);
			return -1;
		}
	}
	return field_offset;
}

/* {{{ Returns values from a result identifier */
PHP_FUNCTION(pg_fetch_result)
{
	zval *result;
	zend_string *field_name;
	zend_long row, field_offset;
	PGresult *pgsql_result;
	pgsql_result_handle *pg_result;
	int pgsql_row, argc = ZEND_NUM_ARGS();

	if (argc == 2) {
		ZEND_PARSE_PARAMETERS_START(2, 2)
			Z_PARAM_RESOURCE(result)
			Z_PARAM_STR_OR_LONG(field_name, field_offset)
		ZEND_PARSE_PARAMETERS_END();
	} else {
		ZEND_PARSE_PARAMETERS_START(3, 3)
			Z_PARAM_RESOURCE(result)
			Z_PARAM_LONG(row)
			Z_PARAM_STR_OR_LONG(field_name, field_offset)
		ZEND_PARSE_PARAMETERS_END();
	}

	if ((pg_result = (pgsql_result_handle *)zend_fetch_resource(Z_RES_P(result), "PostgreSQL result", le_result)) == NULL) {
		RETURN_THROWS();
	}

	pgsql_result = pg_result->result;
	if (argc == 2) {
		if (pg_result->row < 0) {
			pg_result->row = 0;
		}
		pgsql_row = pg_result->row;
		if (pgsql_row >= PQntuples(pgsql_result)) {
			RETURN_FALSE;
		}
		pg_result->row++;
	} else {
		if (row < 0) {
			zend_argument_value_error(2, "must be greater than or equal to 0");
			RETURN_THROWS();
		}
		if (row >= PQntuples(pgsql_result)) {
			php_error_docref(NULL, E_WARNING, "Unable to jump to row " ZEND_LONG_FMT " on PostgreSQL result index " ZEND_LONG_FMT,
							row, Z_LVAL_P(result));
			RETURN_FALSE;
		}
		pgsql_row = (int)row;
	}

	field_offset = field_arg_to_offset(pgsql_result, field_name, field_offset, argc);
	if (field_offset < 0) {
		RETURN_THROWS();
	}

	if (PQgetisnull(pgsql_result, pgsql_row, field_offset)) {
		RETVAL_NULL();
	} else {
		RETVAL_STRINGL(PQgetvalue(pgsql_result, pgsql_row, field_offset),
				PQgetlength(pgsql_result, pgsql_row, field_offset));
	}
}
/* }}} */

/* {{{ void php_pgsql_fetch_hash */
static void php_pgsql_fetch_hash(INTERNAL_FUNCTION_PARAMETERS, zend_long result_type, int into_object)
{
	zval                *result;
	PGresult            *pgsql_result;
	pgsql_result_handle *pg_result;
	int             i, num_fields, pgsql_row;
	zend_long            row;
	bool row_is_null = 1;
	char            *field_name;
	zval            *ctor_params = NULL;
	zend_class_entry *ce = NULL;

	if (into_object) {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "r|l!Ca!", &result, &row, &row_is_null, &ce, &ctor_params) == FAILURE) {
			RETURN_THROWS();
		}
		if (!ce) {
			ce = zend_standard_class_def;
		}
		result_type = PGSQL_ASSOC;
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "r|l!l", &result, &row, &row_is_null, &result_type) == FAILURE) {
			RETURN_THROWS();
		}
	}

	if (!row_is_null && row < 0) {
		zend_argument_value_error(2, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	if (!(result_type & PGSQL_BOTH)) {
		zend_argument_value_error(3, "must be one of PGSQL_ASSOC, PGSQL_NUM, or PGSQL_BOTH");
		RETURN_THROWS();
	}

	if ((pg_result = (pgsql_result_handle *)zend_fetch_resource(Z_RES_P(result), "PostgreSQL result", le_result)) == NULL) {
		RETURN_THROWS();
	}

	pgsql_result = pg_result->result;

	if (!row_is_null) {
		if (row >= PQntuples(pgsql_result)) {
			php_error_docref(NULL, E_WARNING, "Unable to jump to row " ZEND_LONG_FMT " on PostgreSQL result index " ZEND_LONG_FMT,
							row, Z_LVAL_P(result));
			RETURN_FALSE;
		}
		pgsql_row = (int)row;
		pg_result->row = pgsql_row;
	} else {
		/* If 2nd param is NULL, use internal row counter to access next row */
		pgsql_row = pg_result->row;
		if (pgsql_row < 0 || pgsql_row >= PQntuples(pgsql_result)) {
			RETURN_FALSE;
		}
		pg_result->row++;
	}

	array_init(return_value);
	for (i = 0, num_fields = PQnfields(pgsql_result); i < num_fields; i++) {
		if (PQgetisnull(pgsql_result, pgsql_row, i)) {
			if (result_type & PGSQL_NUM) {
				add_index_null(return_value, i);
			}
			if (result_type & PGSQL_ASSOC) {
				field_name = PQfname(pgsql_result, i);
				add_assoc_null(return_value, field_name);
			}
		} else {
			char *element = PQgetvalue(pgsql_result, pgsql_row, i);
			if (element) {
				const size_t element_len = strlen(element);

				if (result_type & PGSQL_NUM) {
					add_index_stringl(return_value, i, element, element_len);
				}

				if (result_type & PGSQL_ASSOC) {
					field_name = PQfname(pgsql_result, i);
					add_assoc_stringl(return_value, field_name, element, element_len);
				}
			}
		}
	}

	if (into_object) {
		zval dataset;
		zend_fcall_info fci;
		zend_fcall_info_cache fcc;
		zval retval;

		ZVAL_COPY_VALUE(&dataset, return_value);
		object_init_ex(return_value, ce);
		if (!ce->default_properties_count && !ce->__set) {
			Z_OBJ_P(return_value)->properties = Z_ARR(dataset);
		} else {
			zend_merge_properties(return_value, Z_ARRVAL(dataset));
			zval_ptr_dtor(&dataset);
		}

		if (ce->constructor) {
			fci.size = sizeof(fci);
			ZVAL_UNDEF(&fci.function_name);
			fci.object = Z_OBJ_P(return_value);
			fci.retval = &retval;
			fci.params = NULL;
			fci.param_count = 0;
			fci.named_params = NULL;

			if (ctor_params) {
				if (zend_fcall_info_args(&fci, ctor_params) == FAILURE) {
					/* Two problems why we throw exceptions here: PHP is typeless
					 * and hence passing one argument that's not an array could be
					 * by mistake and the other way round is possible, too. The
					 * single value is an array. Also we'd have to make that one
					 * argument passed by reference.
					 */
					zend_throw_exception(zend_ce_exception, "Parameter ctor_params must be an array", 0);
					RETURN_THROWS();
				}
			}

			fcc.function_handler = ce->constructor;
			fcc.called_scope = Z_OBJCE_P(return_value);
			fcc.object = Z_OBJ_P(return_value);

			if (zend_call_function(&fci, &fcc) == FAILURE) {
				zend_throw_exception_ex(zend_ce_exception, 0, "Could not execute %s::%s()", ZSTR_VAL(ce->name), ZSTR_VAL(ce->constructor->common.function_name));
			} else {
				zval_ptr_dtor(&retval);
			}
			if (fci.params) {
				efree(fci.params);
			}
		} else if (ctor_params) {
			zend_throw_exception_ex(zend_ce_exception, 0, "Class %s does not have a constructor hence you cannot use ctor_params", ZSTR_VAL(ce->name));
		}
	}
}
/* }}} */

/* {{{ Get a row as an enumerated array */
PHP_FUNCTION(pg_fetch_row)
{
	php_pgsql_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, PGSQL_NUM, 0);
}
/* }}} */

/* {{{ Fetch a row as an assoc array */
PHP_FUNCTION(pg_fetch_assoc)
{
	/* pg_fetch_assoc() is added from PHP 4.3.0. It should raise error, when
	   there is 3rd parameter */
	if (ZEND_NUM_ARGS() > 2)
		WRONG_PARAM_COUNT;
	php_pgsql_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, PGSQL_ASSOC, 0);
}
/* }}} */

/* {{{ Fetch a row as an array */
PHP_FUNCTION(pg_fetch_array)
{
	php_pgsql_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, PGSQL_BOTH, 0);
}
/* }}} */

/* {{{ Fetch a row as an object */
PHP_FUNCTION(pg_fetch_object)
{
	/* pg_fetch_object() allowed result_type used to be. 3rd parameter
	   must be allowed for compatibility */
	php_pgsql_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, PGSQL_ASSOC, 1);
}
/* }}} */

/* {{{ Fetch all rows into array */
PHP_FUNCTION(pg_fetch_all)
{
	zval *result;
	long result_type = PGSQL_ASSOC;
	PGresult *pgsql_result;
	pgsql_result_handle *pg_result;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r|l", &result, &result_type) == FAILURE) {
		RETURN_THROWS();
	}

	if (!(result_type & PGSQL_BOTH)) {
		zend_argument_value_error(2, "must be one of PGSQL_ASSOC, PGSQL_NUM, or PGSQL_BOTH");
		RETURN_THROWS();
	}

	if ((pg_result = (pgsql_result_handle *)zend_fetch_resource(Z_RES_P(result), "PostgreSQL result", le_result)) == NULL) {
		RETURN_THROWS();
	}

	pgsql_result = pg_result->result;
	array_init(return_value);
	php_pgsql_result2array(pgsql_result, return_value, result_type);
}
/* }}} */

/* {{{ Fetch all rows into array */
PHP_FUNCTION(pg_fetch_all_columns)
{
	zval *result;
	PGresult *pgsql_result;
	pgsql_result_handle *pg_result;
	zend_long colno=0;
	int pg_numrows, pg_row;
	size_t num_fields;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r|l", &result, &colno) == FAILURE) {
		RETURN_THROWS();
	}

	if ((pg_result = (pgsql_result_handle *)zend_fetch_resource(Z_RES_P(result), "PostgreSQL result", le_result)) == NULL) {
		RETURN_THROWS();
	}

	if (colno < 0) {
		zend_argument_value_error(2, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	pgsql_result = pg_result->result;

	num_fields = PQnfields(pgsql_result);
	if (colno >= (zend_long)num_fields) {
		zend_argument_value_error(2, "must be less than the number of fields for this result set");
		RETURN_THROWS();
	}

	array_init(return_value);

	if ((pg_numrows = PQntuples(pgsql_result)) <= 0) {
		return;
	}

	for (pg_row = 0; pg_row < pg_numrows; pg_row++) {
		if (PQgetisnull(pgsql_result, pg_row, (int)colno)) {
			add_next_index_null(return_value);
		} else {
			add_next_index_string(return_value, PQgetvalue(pgsql_result, pg_row, (int)colno));
		}
	}
}
/* }}} */

/* {{{ Set internal row offset */
PHP_FUNCTION(pg_result_seek)
{
	zval *result;
	zend_long row;
	pgsql_result_handle *pg_result;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rl", &result, &row) == FAILURE) {
		RETURN_THROWS();
	}

	if ((pg_result = (pgsql_result_handle *)zend_fetch_resource(Z_RES_P(result), "PostgreSQL result", le_result)) == NULL) {
		RETURN_THROWS();
	}

	if (row < 0 || row >= PQntuples(pg_result->result)) {
		RETURN_FALSE;
	}

	/* seek to offset */
	pg_result->row = (int)row;
	RETURN_TRUE;
}
/* }}} */

#define PHP_PG_DATA_LENGTH 1
#define PHP_PG_DATA_ISNULL 2

/* {{{ php_pgsql_data_info */
static void php_pgsql_data_info(INTERNAL_FUNCTION_PARAMETERS, int entry_type)
{
	zval *result;
	zend_string *field_name;
	zend_long row, field_offset;
	PGresult *pgsql_result;
	pgsql_result_handle *pg_result;
	int pgsql_row, argc = ZEND_NUM_ARGS();

	if (argc == 2) {
		ZEND_PARSE_PARAMETERS_START(2, 2)
			Z_PARAM_RESOURCE(result)
			Z_PARAM_STR_OR_LONG(field_name, field_offset)
		ZEND_PARSE_PARAMETERS_END();
	} else {
		ZEND_PARSE_PARAMETERS_START(3, 3)
			Z_PARAM_RESOURCE(result)
			Z_PARAM_LONG(row)
			Z_PARAM_STR_OR_LONG(field_name, field_offset)
		ZEND_PARSE_PARAMETERS_END();
	}

	if ((pg_result = (pgsql_result_handle *)zend_fetch_resource(Z_RES_P(result), "PostgreSQL result", le_result)) == NULL) {
		RETURN_THROWS();
	}

	pgsql_result = pg_result->result;
	if (argc == 2) {
		if (pg_result->row < 0) {
			pg_result->row = 0;
		}
		pgsql_row = pg_result->row;
		if (pgsql_row < 0 || pgsql_row >= PQntuples(pgsql_result)) {
			RETURN_FALSE;
		}
	} else {
		if (row < 0) {
			zend_argument_value_error(2, "must be greater than or equal to 0");
			RETURN_THROWS();
		}
		if (row >= PQntuples(pgsql_result)) {
			php_error_docref(NULL, E_WARNING, "Unable to jump to row " ZEND_LONG_FMT " on PostgreSQL result index " ZEND_LONG_FMT,
							row, Z_LVAL_P(result));
			RETURN_FALSE;
		}
		pgsql_row = (int)row;
	}

	field_offset = field_arg_to_offset(pgsql_result, field_name, field_offset, argc);
	if (field_offset < 0) {
		RETURN_THROWS();
	}

	switch (entry_type) {
		case PHP_PG_DATA_LENGTH:
			RETVAL_LONG(PQgetlength(pgsql_result, pgsql_row, field_offset));
			break;
		case PHP_PG_DATA_ISNULL:
			RETVAL_LONG(PQgetisnull(pgsql_result, pgsql_row, field_offset));
			break;
		EMPTY_SWITCH_DEFAULT_CASE()
	}
}
/* }}} */

/* {{{ Returns the printed length */
PHP_FUNCTION(pg_field_prtlen)
{
	php_pgsql_data_info(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_PG_DATA_LENGTH);
}
/* }}} */

/* {{{ Test if a field is NULL */
PHP_FUNCTION(pg_field_is_null)
{
	php_pgsql_data_info(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_PG_DATA_ISNULL);
}
/* }}} */

/* {{{ Free result memory */
PHP_FUNCTION(pg_free_result)
{
	zval *result;
	pgsql_result_handle *pg_result;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &result) == FAILURE) {
		RETURN_THROWS();
	}

	if ((pg_result = (pgsql_result_handle *)zend_fetch_resource(Z_RES_P(result), "PostgreSQL result", le_result)) == NULL) {
		RETURN_THROWS();
	}

	zend_list_close(Z_RES_P(result));
	RETURN_TRUE;
}
/* }}} */

/* {{{ Returns the last object identifier */
PHP_FUNCTION(pg_last_oid)
{
	zval *result;
	PGresult *pgsql_result;
	pgsql_result_handle *pg_result;
	Oid oid;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &result) == FAILURE) {
		RETURN_THROWS();
	}

	if ((pg_result = (pgsql_result_handle *)zend_fetch_resource(Z_RES_P(result), "PostgreSQL result", le_result)) == NULL) {
		RETURN_THROWS();
	}

	pgsql_result = pg_result->result;
	oid = PQoidValue(pgsql_result);
	if (oid == InvalidOid) {
		RETURN_FALSE;
	}
	PGSQL_RETURN_OID(oid);
}
/* }}} */

/* {{{ Enable tracing a PostgreSQL connection */
PHP_FUNCTION(pg_trace)
{
	char *z_filename, *mode = "w";
	size_t z_filename_len, mode_len;
	zval *pgsql_link = NULL;
	PGconn *pgsql;
	FILE *fp = NULL;
	php_stream *stream;
	zend_resource *link;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "p|sr!", &z_filename, &z_filename_len, &mode, &mode_len, &pgsql_link) == FAILURE) {
		RETURN_THROWS();
	}

	if (!pgsql_link) {
		link = FETCH_DEFAULT_LINK();
		CHECK_DEFAULT_LINK(link);
	} else {
		link = Z_RES_P(pgsql_link);
	}

	if ((pgsql = (PGconn *)zend_fetch_resource2(link, "PostgreSQL link", le_link, le_plink)) == NULL) {
		RETURN_THROWS();
	}

	stream = php_stream_open_wrapper(z_filename, mode, REPORT_ERRORS, NULL);

	if (!stream) {
		RETURN_FALSE;
	}

	if (FAILURE == php_stream_cast(stream, PHP_STREAM_AS_STDIO, (void**)&fp, REPORT_ERRORS))	{
		php_stream_close(stream);
		RETURN_FALSE;
	}
	php_stream_auto_cleanup(stream);
	PQtrace(pgsql, fp);
	RETURN_TRUE;
}
/* }}} */

/* {{{ Disable tracing of a PostgreSQL connection */
PHP_FUNCTION(pg_untrace)
{
	zval *pgsql_link = NULL;
	PGconn *pgsql;
	zend_resource *link;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|r!", &pgsql_link) == FAILURE) {
		RETURN_THROWS();
	}

	if (pgsql_link == NULL) {
		link = FETCH_DEFAULT_LINK();
		CHECK_DEFAULT_LINK(link);
	} else {
		link = Z_RES_P(pgsql_link);
	}

	if ((pgsql = (PGconn *)zend_fetch_resource2(link, "PostgreSQL link", le_link, le_plink)) == NULL) {
		RETURN_THROWS();
	}

	PQuntrace(pgsql);
	RETURN_TRUE;
}
/* }}} */

/* {{{ Create a large object */
PHP_FUNCTION(pg_lo_create)
{
	zval *pgsql_link = NULL, *oid = NULL;
	PGconn *pgsql;
	Oid pgsql_oid, wanted_oid = InvalidOid;
	int argc = ZEND_NUM_ARGS();
	zend_resource *link;

	if (zend_parse_parameters(argc, "|zz", &pgsql_link, &oid) == FAILURE) {
		RETURN_THROWS();
	}

	/* Overloaded method uses default link if arg 1 is not a resource, set oid pointer */
	if ((argc == 1) && (Z_TYPE_P(pgsql_link) != IS_RESOURCE)) {
		oid = pgsql_link;
		pgsql_link = NULL;
	}

	if (pgsql_link == NULL) {
		link = FETCH_DEFAULT_LINK();
		CHECK_DEFAULT_LINK(link);
	} else if ((Z_TYPE_P(pgsql_link) == IS_RESOURCE)) {
		link = Z_RES_P(pgsql_link);
	} else {
		link = NULL;
	}

	if ((pgsql = (PGconn *)zend_fetch_resource2(link, "PostgreSQL link", le_link, le_plink)) == NULL) {
		RETURN_THROWS();
	}

	if (oid) {
		switch (Z_TYPE_P(oid)) {
		case IS_STRING:
			{
				/* TODO: Use subroutine? */
				char *end_ptr;
				wanted_oid = (Oid)strtoul(Z_STRVAL_P(oid), &end_ptr, 10);
				if ((Z_STRVAL_P(oid)+Z_STRLEN_P(oid)) != end_ptr) {
					/* wrong integer format */
					zend_value_error("Invalid OID value passed");
					RETURN_THROWS();
				}
			}
			break;
		case IS_LONG:
			if (Z_LVAL_P(oid) < (zend_long)InvalidOid) {
				zend_value_error("Invalid OID value passed");
				RETURN_THROWS();
			}
			wanted_oid = (Oid)Z_LVAL_P(oid);
			break;
		default:
			zend_type_error("OID value must be of type string|int, %s given", zend_zval_type_name(oid));
			RETURN_THROWS();
		}
		if ((pgsql_oid = lo_create(pgsql, wanted_oid)) == InvalidOid) {
			php_error_docref(NULL, E_WARNING, "Unable to create PostgreSQL large object");
			RETURN_FALSE;
		}

		PGSQL_RETURN_OID(pgsql_oid);
	}

	if ((pgsql_oid = lo_creat(pgsql, INV_READ|INV_WRITE)) == InvalidOid) {
		php_error_docref(NULL, E_WARNING, "Unable to create PostgreSQL large object");
		RETURN_FALSE;
	}

	PGSQL_RETURN_OID(pgsql_oid);
}
/* }}} */

/* {{{ Delete a large object */
PHP_FUNCTION(pg_lo_unlink)
{
	zval *pgsql_link = NULL;
	zend_long oid_long;
	char *oid_string, *end_ptr;
	size_t oid_strlen;
	PGconn *pgsql;
	Oid oid;
	zend_resource *link;
	int argc = ZEND_NUM_ARGS();

	/* accept string type since Oid type is unsigned int */
	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc,
								 "rs", &pgsql_link, &oid_string, &oid_strlen) == SUCCESS) {
		/* TODO: Use subroutine? */
		oid = (Oid)strtoul(oid_string, &end_ptr, 10);
		if ((oid_string+oid_strlen) != end_ptr) {
			/* wrong integer format */
			zend_value_error("Invalid OID value passed");
			RETURN_THROWS();
		}
		link = Z_RES_P(pgsql_link);
	}
	else if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc,
								 "rl", &pgsql_link, &oid_long) == SUCCESS) {
		if (oid_long <= (zend_long)InvalidOid) {
			zend_value_error("Invalid OID value passed");
			RETURN_THROWS();
		}
		oid = (Oid)oid_long;
		link = Z_RES_P(pgsql_link);
	}
	else if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc,
								 "s", &oid_string, &oid_strlen) == SUCCESS) {
		/* TODO: subroutine? */
		oid = (Oid)strtoul(oid_string, &end_ptr, 10);
		if ((oid_string+oid_strlen) != end_ptr) {
			/* wrong integer format */
			zend_value_error("Invalid OID value passed");
			RETURN_THROWS();
		}
		link = FETCH_DEFAULT_LINK();
		CHECK_DEFAULT_LINK(link);
	}
	else if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc,
								 "l", &oid_long) == SUCCESS) {
		if (oid_long <= (zend_long)InvalidOid) {
			zend_value_error("Invalid OID value passed");
			RETURN_THROWS();
		}
		oid = (Oid)oid_long;
		link = FETCH_DEFAULT_LINK();
		CHECK_DEFAULT_LINK(link);
	}
	else {
		zend_argument_count_error("Requires 1 or 2 arguments, %d given", ZEND_NUM_ARGS());
		RETURN_THROWS();
	}

	if ((pgsql = (PGconn *)zend_fetch_resource2(link, "PostgreSQL link", le_link, le_plink)) == NULL) {
		RETURN_THROWS();
	}

	if (lo_unlink(pgsql, oid) == -1) {
		php_error_docref(NULL, E_WARNING, "Unable to delete PostgreSQL large object %u", oid);
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ Open a large object and return fd */
PHP_FUNCTION(pg_lo_open)
{
	zval *pgsql_link = NULL;
	zend_long oid_long;
	char *oid_string, *end_ptr, *mode_string;
	size_t oid_strlen, mode_strlen;
	PGconn *pgsql;
	Oid oid;
	int pgsql_mode=0, pgsql_lofd;
	int create = 0;
	pgLofp *pgsql_lofp;
	int argc = ZEND_NUM_ARGS();
	zend_resource *link;

	/* accept string type since Oid is unsigned int */
	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc,
								 "rss", &pgsql_link, &oid_string, &oid_strlen, &mode_string, &mode_strlen) == SUCCESS) {
		/* TODO: Use subroutine? */
		oid = (Oid)strtoul(oid_string, &end_ptr, 10);
		if ((oid_string+oid_strlen) != end_ptr) {
			/* wrong integer format */
			zend_value_error("Invalid OID value passed");
			RETURN_THROWS();
		}
		link = Z_RES_P(pgsql_link);
	}
	else if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc,
								 "rls", &pgsql_link, &oid_long, &mode_string, &mode_strlen) == SUCCESS) {
		if (oid_long <= (zend_long)InvalidOid) {
			zend_value_error("Invalid OID value passed");
			RETURN_THROWS();
		}
		oid = (Oid)oid_long;
		link = Z_RES_P(pgsql_link);
	}
	else if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc,
								 "ss", &oid_string, &oid_strlen, &mode_string, &mode_strlen) == SUCCESS) {
		/* TODO: Use subroutine? */
		oid = (Oid)strtoul(oid_string, &end_ptr, 10);
		if ((oid_string+oid_strlen) != end_ptr) {
			/* wrong integer format */
			zend_value_error("Invalid OID value passed");
			RETURN_THROWS();
		}
		link = FETCH_DEFAULT_LINK();
		CHECK_DEFAULT_LINK(link);
	}
	else if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc,
								 "ls", &oid_long, &mode_string, &mode_strlen) == SUCCESS) {
		if (oid_long <= (zend_long)InvalidOid) {
			zend_value_error("Invalid OID value passed");
			RETURN_THROWS();
		}
		oid = (Oid)oid_long;
		link = FETCH_DEFAULT_LINK();
		CHECK_DEFAULT_LINK(link);
	}
	else {
		zend_argument_count_error("Requires 1 or 2 arguments, %d given", ZEND_NUM_ARGS());
		RETURN_THROWS();
	}

	if ((pgsql = (PGconn *)zend_fetch_resource2(link, "PostgreSQL link", le_link, le_plink)) == NULL) {
		RETURN_THROWS();
	}

	/* r/w/+ is little bit more PHP-like than INV_READ/INV_WRITE and a lot of
	   faster to type. Unfortunately, doesn't behave the same way as fopen()...
	   (Jouni)
	*/

	if (strchr(mode_string, 'r') == mode_string) {
		pgsql_mode |= INV_READ;
		if (strchr(mode_string, '+') == mode_string+1) {
			pgsql_mode |= INV_WRITE;
		}
	}
	if (strchr(mode_string, 'w') == mode_string) {
		pgsql_mode |= INV_WRITE;
		create = 1;
		if (strchr(mode_string, '+') == mode_string+1) {
			pgsql_mode |= INV_READ;
		}
	}

	pgsql_lofp = (pgLofp *) emalloc(sizeof(pgLofp));

	if ((pgsql_lofd = lo_open(pgsql, oid, pgsql_mode)) == -1) {
		if (create) {
			if ((oid = lo_creat(pgsql, INV_READ|INV_WRITE)) == 0) {
				efree(pgsql_lofp);
				php_error_docref(NULL, E_WARNING, "Unable to create PostgreSQL large object");
				RETURN_FALSE;
			} else {
				if ((pgsql_lofd = lo_open(pgsql, oid, pgsql_mode)) == -1) {
					if (lo_unlink(pgsql, oid) == -1) {
						efree(pgsql_lofp);
						php_error_docref(NULL, E_WARNING, "Something is really messed up! Your database is badly corrupted in a way NOT related to PHP");
						RETURN_FALSE;
					}
					efree(pgsql_lofp);
					php_error_docref(NULL, E_WARNING, "Unable to open PostgreSQL large object");
					RETURN_FALSE;
				} else {
					pgsql_lofp->conn = pgsql;
					pgsql_lofp->lofd = pgsql_lofd;
					RETURN_RES(zend_register_resource(pgsql_lofp, le_lofp));
				}
			}
		} else {
			efree(pgsql_lofp);
			php_error_docref(NULL, E_WARNING, "Unable to open PostgreSQL large object");
			RETURN_FALSE;
		}
	} else {
		pgsql_lofp->conn = pgsql;
		pgsql_lofp->lofd = pgsql_lofd;
		RETURN_RES(zend_register_resource(pgsql_lofp, le_lofp));
	}
}
/* }}} */

/* {{{ Close a large object */
PHP_FUNCTION(pg_lo_close)
{
	zval *pgsql_lofp;
	pgLofp *pgsql;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &pgsql_lofp) == FAILURE) {
		RETURN_THROWS();
	}

	if ((pgsql = (pgLofp *)zend_fetch_resource(Z_RES_P(pgsql_lofp), "PostgreSQL large object", le_lofp)) == NULL) {
		RETURN_THROWS();
	}

	if (lo_close((PGconn *)pgsql->conn, pgsql->lofd) < 0) {
		php_error_docref(NULL, E_WARNING, "Unable to close PostgreSQL large object descriptor %d", pgsql->lofd);
		RETVAL_FALSE;
	} else {
		RETVAL_TRUE;
	}

	zend_list_close(Z_RES_P(pgsql_lofp));
	return;
}
/* }}} */

#define PGSQL_LO_READ_BUF_SIZE  8192

/* {{{ Read a large object */
PHP_FUNCTION(pg_lo_read)
{
	zval *pgsql_id;
	zend_long len;
	size_t buf_len = PGSQL_LO_READ_BUF_SIZE;
	int nbytes, argc = ZEND_NUM_ARGS();
	zend_string *buf;
	pgLofp *pgsql;

	if (zend_parse_parameters(argc, "r|l", &pgsql_id, &len) == FAILURE) {
		RETURN_THROWS();
	}

	if ((pgsql = (pgLofp *)zend_fetch_resource(Z_RES_P(pgsql_id), "PostgreSQL large object", le_lofp)) == NULL) {
		RETURN_THROWS();
	}

	if (argc > 1) {
		buf_len = len < 0 ? 0 : len;
	}

	buf = zend_string_alloc(buf_len, 0);
	if ((nbytes = lo_read((PGconn *)pgsql->conn, pgsql->lofd, ZSTR_VAL(buf), ZSTR_LEN(buf)))<0) {
		zend_string_efree(buf);
		RETURN_FALSE;
	}

	ZSTR_LEN(buf) = nbytes;
	ZSTR_VAL(buf)[ZSTR_LEN(buf)] = '\0';
	RETURN_NEW_STR(buf);
}
/* }}} */

/* {{{ Write a large object */
PHP_FUNCTION(pg_lo_write)
{
  	zval *pgsql_id;
  	char *str;
  	zend_long z_len;
  	zend_bool z_len_is_null = 1;
	size_t str_len, nbytes;
	size_t len;
	pgLofp *pgsql;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rs|l!", &pgsql_id, &str, &str_len, &z_len, &z_len_is_null) == FAILURE) {
		RETURN_THROWS();
	}

	if (!z_len_is_null) {
		if (z_len < 0) {
			zend_argument_value_error(3, "must be greater than or equal to 0");
			RETURN_THROWS();
		}
		if (z_len > (zend_long)str_len) {
			zend_argument_value_error(3, "must be less than or equal to the length of argument #2 ($buf)");
			RETURN_THROWS();
		}
		len = z_len;
	}
	else {
		len = str_len;
	}

	if ((pgsql = (pgLofp *)zend_fetch_resource(Z_RES_P(pgsql_id), "PostgreSQL large object", le_lofp)) == NULL) {
		RETURN_THROWS();
	}

	if ((nbytes = lo_write((PGconn *)pgsql->conn, pgsql->lofd, str, len)) == (size_t)-1) {
		RETURN_FALSE;
	}

	RETURN_LONG(nbytes);
}
/* }}} */

/* {{{ Read a large object and send straight to browser */
PHP_FUNCTION(pg_lo_read_all)
{
  	zval *pgsql_id;
	int tbytes;
	volatile int nbytes;
	char buf[PGSQL_LO_READ_BUF_SIZE];
	pgLofp *pgsql;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &pgsql_id) == FAILURE) {
		RETURN_THROWS();
	}

	if ((pgsql = (pgLofp *)zend_fetch_resource(Z_RES_P(pgsql_id), "PostgreSQL large object", le_lofp)) == NULL) {
		RETURN_THROWS();
	}

	tbytes = 0;
	while ((nbytes = lo_read((PGconn *)pgsql->conn, pgsql->lofd, buf, PGSQL_LO_READ_BUF_SIZE))>0) {
		PHPWRITE(buf, nbytes);
		tbytes += nbytes;
	}
	RETURN_LONG(tbytes);
}
/* }}} */

/* {{{ Import large object direct from filesystem */
PHP_FUNCTION(pg_lo_import)
{
	zval *pgsql_link = NULL, *oid = NULL;
	char *file_in;
	size_t name_len;
	int argc = ZEND_NUM_ARGS();
	PGconn *pgsql;
	Oid returned_oid;
	zend_resource *link;

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc,
								 "rp|z", &pgsql_link, &file_in, &name_len, &oid) == SUCCESS) {
		link = Z_RES_P(pgsql_link);
	}
	else if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc,
									  "p|z", &file_in, &name_len, &oid) == SUCCESS) {
		link = FETCH_DEFAULT_LINK();
		CHECK_DEFAULT_LINK(link);
	}
	else {
		WRONG_PARAM_COUNT;
	}

	if (php_check_open_basedir(file_in)) {
		RETURN_FALSE;
	}

	if ((pgsql = (PGconn *)zend_fetch_resource2(link, "PostgreSQL link", le_link, le_plink)) == NULL) {
		RETURN_THROWS();
	}

	if (oid) {
		Oid wanted_oid;
		switch (Z_TYPE_P(oid)) {
		case IS_STRING:
			{
				/* TODO: Use subroutine? */
				char *end_ptr;
				wanted_oid = (Oid)strtoul(Z_STRVAL_P(oid), &end_ptr, 10);
				if ((Z_STRVAL_P(oid)+Z_STRLEN_P(oid)) != end_ptr) {
					/* wrong integer format */
					zend_value_error("Invalid OID value passed");
					RETURN_THROWS();
				}
			}
			break;
		case IS_LONG:
			if (Z_LVAL_P(oid) < (zend_long)InvalidOid) {
				zend_value_error("Invalid OID value passed");
				RETURN_THROWS();
			}
			wanted_oid = (Oid)Z_LVAL_P(oid);
			break;
		default:
			zend_type_error("OID value must be of type string|int, %s given", zend_zval_type_name(oid));
			RETURN_THROWS();
		}

		returned_oid = lo_import_with_oid(pgsql, file_in, wanted_oid);

		if (returned_oid == InvalidOid) {
			RETURN_FALSE;
		}

		PGSQL_RETURN_OID(returned_oid);
	}

	returned_oid = lo_import(pgsql, file_in);

	if (returned_oid == InvalidOid) {
		RETURN_FALSE;
	}
	PGSQL_RETURN_OID(returned_oid);
}
/* }}} */

/* {{{ Export large object direct to filesystem */
PHP_FUNCTION(pg_lo_export)
{
	zval *pgsql_link = NULL;
	char *file_out, *oid_string, *end_ptr;
	size_t oid_strlen;
	size_t name_len;
	zend_long oid_long;
	Oid oid;
	PGconn *pgsql;
	int argc = ZEND_NUM_ARGS();
	zend_resource *link;

	/* allow string to handle large OID value correctly */
	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc,
								 "rlp", &pgsql_link, &oid_long, &file_out, &name_len) == SUCCESS) {
		if (oid_long <= (zend_long)InvalidOid) {
			zend_value_error("Invalid OID value passed");
			RETURN_THROWS();
		}
		oid = (Oid)oid_long;
		link = Z_RES_P(pgsql_link);
	}
	else if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc,
								 "rsp", &pgsql_link, &oid_string, &oid_strlen, &file_out, &name_len) == SUCCESS) {
		/* TODO: Use subroutine? */
		oid = (Oid)strtoul(oid_string, &end_ptr, 10);
		if ((oid_string+oid_strlen) != end_ptr) {
			/* wrong integer format */
			zend_value_error("Invalid OID value passed");
			RETURN_THROWS();
		}
		link = Z_RES_P(pgsql_link);
	}
	else if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc,
									  "lp",  &oid_long, &file_out, &name_len) == SUCCESS) {
		if (oid_long <= (zend_long)InvalidOid) {
			zend_value_error("Invalid OID value passed");
			RETURN_THROWS();
		}
		oid = (Oid)oid_long;
		link = FETCH_DEFAULT_LINK();
		CHECK_DEFAULT_LINK(link);
	}
	else if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc,
								 "sp", &oid_string, &oid_strlen, &file_out, &name_len) == SUCCESS) {
		/* TODO: Use subroutine? */
		oid = (Oid)strtoul(oid_string, &end_ptr, 10);
		if ((oid_string+oid_strlen) != end_ptr) {
			/* wrong integer format */
			zend_value_error("Invalid OID value passed");
			RETURN_THROWS();
		}
		link = FETCH_DEFAULT_LINK();
		CHECK_DEFAULT_LINK(link);
	}
	else {
		zend_argument_count_error("Requires 2 or 3 arguments, %d given", ZEND_NUM_ARGS());
		RETURN_THROWS();
	}

	if (php_check_open_basedir(file_out)) {
		RETURN_FALSE;
	}

	if ((pgsql = (PGconn *)zend_fetch_resource2(link, "PostgreSQL link", le_link, le_plink)) == NULL) {
		RETURN_THROWS();
	}

	if (lo_export(pgsql, oid, file_out) == -1) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ Seeks position of large object */
PHP_FUNCTION(pg_lo_seek)
{
	zval *pgsql_id = NULL;
	zend_long result, offset = 0, whence = SEEK_CUR;
	pgLofp *pgsql;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rl|l", &pgsql_id, &offset, &whence) == FAILURE) {
		RETURN_THROWS();
	}
	if (whence != SEEK_SET && whence != SEEK_CUR && whence != SEEK_END) {
		zend_argument_value_error(3, "must be one of PGSQL_SEEK_SET, PGSQL_SEEK_CUR, or PGSQL_SEEK_END");
		RETURN_THROWS();
	}

	if ((pgsql = (pgLofp *)zend_fetch_resource(Z_RES_P(pgsql_id), "PostgreSQL large object", le_lofp)) == NULL) {
		RETURN_THROWS();
	}

#ifdef HAVE_PG_LO64
	if (PQserverVersion((PGconn *)pgsql->conn) >= 90300) {
		result = lo_lseek64((PGconn *)pgsql->conn, pgsql->lofd, offset, (int)whence);
	} else {
		result = lo_lseek((PGconn *)pgsql->conn, pgsql->lofd, (int)offset, (int)whence);
	}
#else
	result = lo_lseek((PGconn *)pgsql->conn, pgsql->lofd, offset, whence);
#endif
	if (result > -1) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ Returns current position of large object */
PHP_FUNCTION(pg_lo_tell)
{
	zval *pgsql_id = NULL;
	zend_long offset = 0;
	pgLofp *pgsql;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &pgsql_id) == FAILURE) {
		RETURN_THROWS();
	}

	if ((pgsql = (pgLofp *)zend_fetch_resource(Z_RES_P(pgsql_id), "PostgreSQL large object", le_lofp)) == NULL) {
		RETURN_THROWS();
	}

#ifdef VE_PG_LO64
	if (PQserverVersion((PGconn *)pgsql->conn) >= 90300) {
		offset = lo_tell64((PGconn *)pgsql->conn, pgsql->lofd);
	} else {
		offset = lo_tell((PGconn *)pgsql->conn, pgsql->lofd);
	}
#else
	offset = lo_tell((PGconn *)pgsql->conn, pgsql->lofd);
#endif
	RETURN_LONG(offset);
}
/* }}} */

/* {{{ Truncate large object to size */
PHP_FUNCTION(pg_lo_truncate)
{
	zval *pgsql_id = NULL;
	size_t size;
	pgLofp *pgsql;
	int result;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rl", &pgsql_id, &size) == FAILURE) {
		RETURN_THROWS();
	}

	if ((pgsql = (pgLofp *)zend_fetch_resource(Z_RES_P(pgsql_id), "PostgreSQL large object", le_lofp)) == NULL) {
		RETURN_THROWS();
	}

#ifdef VE_PG_LO64
	if (PQserverVersion((PGconn *)pgsql->conn) >= 90300) {
		result = lo_truncate64((PGconn *)pgsql->conn, pgsql->lofd, size);
	} else {
		result = lo_truncate((PGconn *)pgsql->conn, pgsql->lofd, size);
	}
#else
	result = lo_truncate((PGconn *)pgsql->conn, pgsql->lofd, size);
#endif
	if (!result) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ Set error verbosity */
PHP_FUNCTION(pg_set_error_verbosity)
{
	zval *pgsql_link = NULL;
	zend_long verbosity;
	int argc = ZEND_NUM_ARGS();
	PGconn *pgsql;
	zend_resource *link;

	if (argc == 1) {
		if (zend_parse_parameters(argc, "l", &verbosity) == FAILURE) {
			RETURN_THROWS();
		}
		link = FETCH_DEFAULT_LINK();
		CHECK_DEFAULT_LINK(link);
	} else {
		if (zend_parse_parameters(argc, "rl", &pgsql_link, &verbosity) == FAILURE) {
			RETURN_THROWS();
		}
		link = Z_RES_P(pgsql_link);
	}

	if ((pgsql = (PGconn *)zend_fetch_resource2(link, "PostgreSQL link", le_link, le_plink)) == NULL) {
		RETURN_THROWS();
	}

	if (verbosity & (PQERRORS_TERSE|PQERRORS_DEFAULT|PQERRORS_VERBOSE)) {
		RETURN_LONG(PQsetErrorVerbosity(pgsql, verbosity));
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ Set client encoding */
PHP_FUNCTION(pg_set_client_encoding)
{
	char *encoding;
	size_t encoding_len;
	zval *pgsql_link = NULL;
	int argc = ZEND_NUM_ARGS();
	PGconn *pgsql;
	zend_resource *link;

	if (argc == 1) {
		if (zend_parse_parameters(argc, "s", &encoding, &encoding_len) == FAILURE) {
			RETURN_THROWS();
		}
		link = FETCH_DEFAULT_LINK();
		CHECK_DEFAULT_LINK(link);
	} else {
		if (zend_parse_parameters(argc, "rs", &pgsql_link, &encoding, &encoding_len) == FAILURE) {
			RETURN_THROWS();
		}
		link = Z_RES_P(pgsql_link);
	}

	if ((pgsql = (PGconn *)zend_fetch_resource2(link, "PostgreSQL link", le_link, le_plink)) == NULL) {
		RETURN_THROWS();
	}

	RETURN_LONG(PQsetClientEncoding(pgsql, encoding));
}
/* }}} */

/* {{{ Get the current client encoding */
PHP_FUNCTION(pg_client_encoding)
{
	zval *pgsql_link = NULL;
	PGconn *pgsql;
	zend_resource *link;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|r!", &pgsql_link) == FAILURE) {
		RETURN_THROWS();
	}

	if (pgsql_link == NULL) {
		link = FETCH_DEFAULT_LINK();
		CHECK_DEFAULT_LINK(link);
	} else {
		link = Z_RES_P(pgsql_link);
	}

	if ((pgsql = (PGconn *)zend_fetch_resource2(link, "PostgreSQL link", le_link, le_plink)) == NULL) {
		RETURN_THROWS();
	}

	/* Just do the same as found in PostgreSQL sources... */

	RETURN_STRING((char *) pg_encoding_to_char(PQclientEncoding(pgsql)));
}
/* }}} */

/* {{{ Sync with backend. Completes the Copy command */
PHP_FUNCTION(pg_end_copy)
{
	zval *pgsql_link = NULL;
	PGconn *pgsql;
	int result = 0;
	zend_resource *link;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|r!", &pgsql_link) == FAILURE) {
		RETURN_THROWS();
	}

	if (pgsql_link == NULL) {
		link = FETCH_DEFAULT_LINK();
		CHECK_DEFAULT_LINK(link);
	} else {
		link = Z_RES_P(pgsql_link);
	}

	if ((pgsql = (PGconn *)zend_fetch_resource2(link, "PostgreSQL link", le_link, le_plink)) == NULL) {
		RETURN_THROWS();
	}

	result = PQendcopy(pgsql);

	if (result!=0) {
		PHP_PQ_ERROR("Query failed: %s", pgsql);
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ Send null-terminated string to backend server*/
PHP_FUNCTION(pg_put_line)
{
	char *query;
	zval *pgsql_link = NULL;
	size_t query_len;
	PGconn *pgsql;
	zend_resource *link;
	int result = 0, argc = ZEND_NUM_ARGS();

	if (argc == 1) {
		if (zend_parse_parameters(argc, "s", &query, &query_len) == FAILURE) {
			RETURN_THROWS();
		}
		link = FETCH_DEFAULT_LINK();
		CHECK_DEFAULT_LINK(link);
	} else {
		if (zend_parse_parameters(argc, "rs", &pgsql_link, &query, &query_len) == FAILURE) {
			RETURN_THROWS();
		}
		link = Z_RES_P(pgsql_link);
	}

	if ((pgsql = (PGconn *)zend_fetch_resource2(link, "PostgreSQL link", le_link, le_plink)) == NULL) {
		RETURN_THROWS();
	}

	result = PQputline(pgsql, query);
	if (result==EOF) {
		PHP_PQ_ERROR("Query failed: %s", pgsql);
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ Copy table to array */
PHP_FUNCTION(pg_copy_to)
{
	zval *pgsql_link;
	char *table_name, *pg_delim = NULL, *pg_null_as = NULL;
	size_t table_name_len, pg_delim_len, pg_null_as_len, free_pg_null = 0;
	char *query;
	PGconn *pgsql;
	PGresult *pgsql_result;
	ExecStatusType status;
	char *csv = (char *)NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rs|ss",
							  &pgsql_link, &table_name, &table_name_len,
							  &pg_delim, &pg_delim_len, &pg_null_as, &pg_null_as_len) == FAILURE) {
		RETURN_THROWS();
	}

	if ((pgsql = (PGconn *)zend_fetch_resource2(Z_RES_P(pgsql_link), "PostgreSQL link", le_link, le_plink)) == NULL) {
		RETURN_THROWS();
	}

	if (!pg_delim) {
		pg_delim = "\t";
	}
	if (!pg_null_as) {
		pg_null_as = estrdup("\\\\N");
		free_pg_null = 1;
	}

	spprintf(&query, 0, "COPY %s TO STDOUT DELIMITER E'%c' NULL AS E'%s'", table_name, *pg_delim, pg_null_as);

	while ((pgsql_result = PQgetResult(pgsql))) {
		PQclear(pgsql_result);
	}
	pgsql_result = PQexec(pgsql, query);
	if (free_pg_null) {
		efree(pg_null_as);
	}
	efree(query);

	if (pgsql_result) {
		status = PQresultStatus(pgsql_result);
	} else {
		status = (ExecStatusType) PQstatus(pgsql);
	}

	switch (status) {
		case PGRES_COPY_OUT:
			if (pgsql_result) {
				int copydone = 0;

				PQclear(pgsql_result);
				array_init(return_value);
				while (!copydone)
				{
					int ret = PQgetCopyData(pgsql, &csv, 0);
					switch (ret) {
						case -1:
							copydone = 1;
							break;
						case 0:
						case -2:
							PHP_PQ_ERROR("getline failed: %s", pgsql);
							RETURN_FALSE;
							break;
						default:
							add_next_index_string(return_value, csv);
							PQfreemem(csv);
							break;
					}
				}
				while ((pgsql_result = PQgetResult(pgsql))) {
					PQclear(pgsql_result);
				}
			} else {
				PQclear(pgsql_result);
				RETURN_FALSE;
			}
			break;
		default:
			PQclear(pgsql_result);
			PHP_PQ_ERROR("Copy command failed: %s", pgsql);
			RETURN_FALSE;
			break;
	}
}
/* }}} */

/* {{{ Copy table from array */
PHP_FUNCTION(pg_copy_from)
{
	zval *pgsql_link = NULL, *pg_rows;
	zval *value;
	char *table_name, *pg_delim = NULL, *pg_null_as = NULL;
	size_t  table_name_len, pg_delim_len, pg_null_as_len;
	int  pg_null_as_free = 0;
	char *query;
	PGconn *pgsql;
	PGresult *pgsql_result;
	ExecStatusType status;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rsa|ss",
							  &pgsql_link, &table_name, &table_name_len, &pg_rows,
							  &pg_delim, &pg_delim_len, &pg_null_as, &pg_null_as_len) == FAILURE) {
		RETURN_THROWS();
	}

	if ((pgsql = (PGconn *)zend_fetch_resource2(Z_RES_P(pgsql_link), "PostgreSQL link", le_link, le_plink)) == NULL) {
		RETURN_THROWS();
	}

	if (!pg_delim) {
		pg_delim = "\t";
	}
	if (!pg_null_as) {
		pg_null_as = estrdup("\\\\N");
		pg_null_as_free = 1;
	}

	spprintf(&query, 0, "COPY %s FROM STDIN DELIMITER E'%c' NULL AS E'%s'", table_name, *pg_delim, pg_null_as);
	while ((pgsql_result = PQgetResult(pgsql))) {
		PQclear(pgsql_result);
	}
	pgsql_result = PQexec(pgsql, query);

	if (pg_null_as_free) {
		efree(pg_null_as);
	}
	efree(query);

	if (pgsql_result) {
		status = PQresultStatus(pgsql_result);
	} else {
		status = (ExecStatusType) PQstatus(pgsql);
	}

	switch (status) {
		case PGRES_COPY_IN:
			if (pgsql_result) {
				int command_failed = 0;
				PQclear(pgsql_result);
				ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(pg_rows), value) {
					zend_string *tmp = zval_try_get_string(value);
					if (UNEXPECTED(!tmp)) {
						return;
					}
					query = (char *)emalloc(ZSTR_LEN(tmp) + 2);
					strlcpy(query, ZSTR_VAL(tmp), ZSTR_LEN(tmp) + 2);
					if (ZSTR_LEN(tmp) > 0 && *(query + ZSTR_LEN(tmp) - 1) != '\n') {
						strlcat(query, "\n", ZSTR_LEN(tmp) + 2);
					}
					if (PQputCopyData(pgsql, query, (int)strlen(query)) != 1) {
						efree(query);
						zend_string_release(tmp);
						PHP_PQ_ERROR("copy failed: %s", pgsql);
						RETURN_FALSE;
					}
					efree(query);
					zend_string_release(tmp);
				} ZEND_HASH_FOREACH_END();

				if (PQputCopyEnd(pgsql, NULL) != 1) {
					PHP_PQ_ERROR("putcopyend failed: %s", pgsql);
					RETURN_FALSE;
				}
				while ((pgsql_result = PQgetResult(pgsql))) {
					if (PGRES_COMMAND_OK != PQresultStatus(pgsql_result)) {
						PHP_PQ_ERROR("Copy command failed: %s", pgsql);
						command_failed = 1;
					}
					PQclear(pgsql_result);
				}
				if (command_failed) {
					RETURN_FALSE;
				}
			} else {
				PQclear(pgsql_result);
				RETURN_FALSE;
			}
			RETURN_TRUE;
			break;
		default:
			PQclear(pgsql_result);
			PHP_PQ_ERROR("Copy command failed: %s", pgsql);
			RETURN_FALSE;
			break;
	}
}
/* }}} */

/* {{{ Escape string for text/char type */
PHP_FUNCTION(pg_escape_string)
{
	zend_string *from = NULL, *to = NULL;
	zval *pgsql_link;
	zend_resource *link;
	PGconn *pgsql;

	switch (ZEND_NUM_ARGS()) {
		case 1:
			if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &from) == FAILURE) {
				RETURN_THROWS();
			}
			link = FETCH_DEFAULT_LINK();
			break;
		default:
			if (zend_parse_parameters(ZEND_NUM_ARGS(), "rS", &pgsql_link, &from) == FAILURE) {
				RETURN_THROWS();
			}
			link = Z_RES_P(pgsql_link);
			break;
	}

	to = zend_string_safe_alloc(ZSTR_LEN(from), 2, 0, 0);
	if (link) {
		if ((pgsql = (PGconn *)zend_fetch_resource2(link, "PostgreSQL link", le_link, le_plink)) == NULL) {
			RETURN_THROWS();
		}
		ZSTR_LEN(to) = PQescapeStringConn(pgsql, ZSTR_VAL(to), ZSTR_VAL(from), ZSTR_LEN(from), NULL);
	} else
	{
		ZSTR_LEN(to) = PQescapeString(ZSTR_VAL(to), ZSTR_VAL(from), ZSTR_LEN(from));
	}

	to = zend_string_truncate(to, ZSTR_LEN(to), 0);
	RETURN_NEW_STR(to);
}
/* }}} */

/* {{{ Escape binary for bytea type  */
PHP_FUNCTION(pg_escape_bytea)
{
	char *from = NULL, *to = NULL;
	size_t to_len;
	size_t from_len;
	PGconn *pgsql;
	zval *pgsql_link;
	zend_resource *link;

	switch (ZEND_NUM_ARGS()) {
		case 1:
			if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &from, &from_len) == FAILURE) {
				RETURN_THROWS();
			}
			link = FETCH_DEFAULT_LINK();
			break;
		default:
			if (zend_parse_parameters(ZEND_NUM_ARGS(), "rs", &pgsql_link, &from, &from_len) == FAILURE) {
				RETURN_THROWS();
			}
			link = Z_RES_P(pgsql_link);
			break;
	}

	if (link) {
		if ((pgsql = (PGconn *)zend_fetch_resource2(link, "PostgreSQL link", le_link, le_plink)) == NULL) {
			RETURN_THROWS();
		}
		to = (char *)PQescapeByteaConn(pgsql, (unsigned char *)from, (size_t)from_len, &to_len);
	} else
		to = (char *)PQescapeBytea((unsigned char*)from, from_len, &to_len);

	RETVAL_STRINGL(to, to_len-1); /* to_len includes additional '\0' */
	PQfreemem(to);
}
/* }}} */

/* {{{ Unescape binary for bytea type  */
PHP_FUNCTION(pg_unescape_bytea)
{
	char *from, *tmp;
	size_t to_len;
	size_t from_len;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s!",
							  &from, &from_len) == FAILURE) {
		RETURN_THROWS();
	}

	tmp = (char *)PQunescapeBytea((unsigned char*)from, &to_len);
	if (!tmp) {
		zend_error(E_ERROR, "Out of memory");
		return;
	}

	RETVAL_STRINGL(tmp, to_len);
	PQfreemem(tmp);
}
/* }}} */

static void php_pgsql_escape_internal(INTERNAL_FUNCTION_PARAMETERS, int escape_literal) /* {{{ */ {
	char *from = NULL;
	zval *pgsql_link = NULL;
	PGconn *pgsql;
	size_t from_len;
	char *tmp;
	zend_resource *link;

	switch (ZEND_NUM_ARGS()) {
		case 1:
			if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &from, &from_len) == FAILURE) {
				RETURN_THROWS();
			}
			link = FETCH_DEFAULT_LINK();
			CHECK_DEFAULT_LINK(link);
			break;

		default:
			if (zend_parse_parameters(ZEND_NUM_ARGS(), "rs", &pgsql_link, &from, &from_len) == FAILURE) {
				RETURN_THROWS();
			}
			link = Z_RES_P(pgsql_link);
			break;
	}

	if ((pgsql = (PGconn *)zend_fetch_resource2(link, "PostgreSQL link", le_link, le_plink)) == NULL) {
		RETURN_THROWS();
	}

	if (escape_literal) {
		tmp = PQescapeLiteral(pgsql, from, (size_t)from_len);
	} else {
		tmp = PQescapeIdentifier(pgsql, from, (size_t)from_len);
	}
	if (!tmp) {
		php_error_docref(NULL, E_WARNING,"Failed to escape");
		RETURN_FALSE;
	}

	RETVAL_STRING(tmp);
	PQfreemem(tmp);
}
/* }}} */

/* {{{ Escape parameter as string literal (i.e. parameter)	*/
PHP_FUNCTION(pg_escape_literal)
{
	php_pgsql_escape_internal(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ Escape identifier (i.e. table name, field name)	*/
PHP_FUNCTION(pg_escape_identifier)
{
	php_pgsql_escape_internal(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ Get error message associated with result */
PHP_FUNCTION(pg_result_error)
{
	zval *result;
	PGresult *pgsql_result;
	pgsql_result_handle *pg_result;
	char *err = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &result) == FAILURE) {
		RETURN_THROWS();
	}

	if ((pg_result = (pgsql_result_handle *)zend_fetch_resource(Z_RES_P(result), "PostgreSQL result", le_result)) == NULL) {
		RETURN_THROWS();
	}

	pgsql_result = pg_result->result;
	if (!pgsql_result) {
		RETURN_FALSE;
	}
	err = (char *)PQresultErrorMessage(pgsql_result);
	RETURN_STRING(err);
}
/* }}} */

/* {{{ Get error message field associated with result */
PHP_FUNCTION(pg_result_error_field)
{
	zval *result;
	zend_long fieldcode;
	PGresult *pgsql_result;
	pgsql_result_handle *pg_result;
	char *field = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rl", &result, &fieldcode) == FAILURE) {
		RETURN_THROWS();
	}

	if ((pg_result = (pgsql_result_handle *)zend_fetch_resource(Z_RES_P(result), "PostgreSQL result", le_result)) == NULL) {
		RETURN_THROWS();
	}

	pgsql_result = pg_result->result;
	if (!pgsql_result) {
		RETURN_FALSE;
	}
	if (fieldcode & (PG_DIAG_SEVERITY|PG_DIAG_SQLSTATE|PG_DIAG_MESSAGE_PRIMARY|PG_DIAG_MESSAGE_DETAIL
				|PG_DIAG_MESSAGE_HINT|PG_DIAG_STATEMENT_POSITION
#ifdef PG_DIAG_INTERNAL_POSITION
				|PG_DIAG_INTERNAL_POSITION
#endif
#ifdef PG_DIAG_INTERNAL_QUERY
				|PG_DIAG_INTERNAL_QUERY
#endif
				|PG_DIAG_CONTEXT|PG_DIAG_SOURCE_FILE|PG_DIAG_SOURCE_LINE
				|PG_DIAG_SOURCE_FUNCTION)) {
		field = (char *)PQresultErrorField(pgsql_result, (int)fieldcode);
		if (field == NULL) {
			RETURN_NULL();
		} else {
			RETURN_STRING(field);
		}
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ Get connection status */
PHP_FUNCTION(pg_connection_status)
{
	zval *pgsql_link = NULL;
	PGconn *pgsql;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &pgsql_link) == FAILURE) {
		RETURN_THROWS();
	}

	if ((pgsql = (PGconn *)zend_fetch_resource2(Z_RES_P(pgsql_link), "PostgreSQL link", le_link, le_plink)) == NULL) {
		RETURN_THROWS();
	}

	RETURN_LONG(PQstatus(pgsql));
}

/* }}} */

/* {{{ Get transaction status */
PHP_FUNCTION(pg_transaction_status)
{
	zval *pgsql_link = NULL;
	PGconn *pgsql;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &pgsql_link) == FAILURE) {
		RETURN_THROWS();
	}

	if ((pgsql = (PGconn *)zend_fetch_resource2(Z_RES_P(pgsql_link), "PostgreSQL link", le_link, le_plink)) == NULL) {
		RETURN_THROWS();
	}

	RETURN_LONG(PQtransactionStatus(pgsql));
}

/* }}} */

/* {{{ Reset connection (reconnect) */
PHP_FUNCTION(pg_connection_reset)
{
	zval *pgsql_link;
	PGconn *pgsql;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &pgsql_link) == FAILURE) {
		RETURN_THROWS();
	}

	if ((pgsql = (PGconn *)zend_fetch_resource2(Z_RES_P(pgsql_link), "PostgreSQL link", le_link, le_plink)) == NULL) {
		RETURN_THROWS();
	}

	PQreset(pgsql);
	if (PQstatus(pgsql) == CONNECTION_BAD) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

#define PHP_PG_ASYNC_IS_BUSY		1
#define PHP_PG_ASYNC_REQUEST_CANCEL 2

/* {{{ php_pgsql_flush_query */
static int php_pgsql_flush_query(PGconn *pgsql)
{
	PGresult *res;
	int leftover = 0;

	if (PQsetnonblocking(pgsql, 1)) {
		php_error_docref(NULL, E_NOTICE,"Cannot set connection to nonblocking mode");
		return -1;
	}
	while ((res = PQgetResult(pgsql))) {
		PQclear(res);
		leftover++;
	}
	PQsetnonblocking(pgsql, 0);
	return leftover;
}
/* }}} */

/* {{{ php_pgsql_do_async */
static void php_pgsql_do_async(INTERNAL_FUNCTION_PARAMETERS, int entry_type)
{
	zval *pgsql_link;
	PGconn *pgsql;
	PGresult *pgsql_result;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &pgsql_link) == FAILURE) {
		RETURN_THROWS();
	}

	if ((pgsql = (PGconn *)zend_fetch_resource2(Z_RES_P(pgsql_link), "PostgreSQL link", le_link, le_plink)) == NULL) {
		RETURN_THROWS();
	}

	if (PQsetnonblocking(pgsql, 1)) {
		php_error_docref(NULL, E_NOTICE, "Cannot set connection to nonblocking mode");
		RETURN_FALSE;
	}
	switch(entry_type) {
		case PHP_PG_ASYNC_IS_BUSY:
			PQconsumeInput(pgsql);
			RETVAL_LONG(PQisBusy(pgsql));
			break;
		case PHP_PG_ASYNC_REQUEST_CANCEL:
			RETVAL_LONG(PQrequestCancel(pgsql));
			while ((pgsql_result = PQgetResult(pgsql))) {
				PQclear(pgsql_result);
			}
			break;
		EMPTY_SWITCH_DEFAULT_CASE()
	}
	if (PQsetnonblocking(pgsql, 0)) {
		php_error_docref(NULL, E_NOTICE, "Cannot set connection to blocking mode");
	}
	convert_to_boolean_ex(return_value);
}
/* }}} */

/* {{{ Cancel request */
PHP_FUNCTION(pg_cancel_query)
{
	php_pgsql_do_async(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_PG_ASYNC_REQUEST_CANCEL);
}
/* }}} */

/* {{{ Get connection is busy or not */
PHP_FUNCTION(pg_connection_busy)
{
	php_pgsql_do_async(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_PG_ASYNC_IS_BUSY);
}
/* }}} */

static int _php_pgsql_link_has_results(PGconn *pgsql) /* {{{ */
{
	PGresult *result;
	while ((result = PQgetResult(pgsql))) {
		PQclear(result);
		return 1;
	}
	return 0;
}
/* }}} */

/* {{{ Send asynchronous query */
PHP_FUNCTION(pg_send_query)
{
	zval *pgsql_link;
	char *query;
	size_t len;
	PGconn *pgsql;
	int is_non_blocking;
	int ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rs", &pgsql_link, &query, &len) == FAILURE) {
		RETURN_THROWS();
	}

	if ((pgsql = (PGconn *)zend_fetch_resource2(Z_RES_P(pgsql_link), "PostgreSQL link", le_link, le_plink)) == NULL) {
		RETURN_THROWS();
	}

	is_non_blocking = PQisnonblocking(pgsql);

	if (is_non_blocking == 0 && PQsetnonblocking(pgsql, 1) == -1) {
		php_error_docref(NULL, E_NOTICE, "Cannot set connection to nonblocking mode");
		RETURN_FALSE;
	}

	if (_php_pgsql_link_has_results(pgsql)) {
		php_error_docref(NULL, E_NOTICE,
			"There are results on this connection. Call pg_get_result() until it returns FALSE");
	}

	if (is_non_blocking) {
		if (!PQsendQuery(pgsql, query)) {
			RETURN_FALSE;
		}
		ret = PQflush(pgsql);
	} else {
		if (!PQsendQuery(pgsql, query)) {
			if ((PGG(auto_reset_persistent) & 2) && PQstatus(pgsql) != CONNECTION_OK) {
				PQreset(pgsql);
			}
			if (!PQsendQuery(pgsql, query)) {
				RETURN_FALSE;
			}
		}

		/* Wait to finish sending buffer */
		while ((ret = PQflush(pgsql))) {
			if (ret == -1) {
				php_error_docref(NULL, E_NOTICE, "Could not empty PostgreSQL send buffer");
				break;
			}
			usleep(10000);
		}

		if (PQsetnonblocking(pgsql, 0)) {
			php_error_docref(NULL, E_NOTICE, "Cannot set connection to blocking mode");
		}
	}

	if (ret == 0) {
		RETURN_TRUE;
	} else if (ret == -1) {
		RETURN_FALSE;
	} else {
		RETURN_LONG(0);
	}
}
/* }}} */

/* {{{ Send asynchronous parameterized query */
PHP_FUNCTION(pg_send_query_params)
{
	zval *pgsql_link, *pv_param_arr, *tmp;
	int num_params = 0;
	char **params = NULL;
	char *query;
	size_t query_len;
	PGconn *pgsql;
	int is_non_blocking;
	int ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rsa", &pgsql_link, &query, &query_len, &pv_param_arr) == FAILURE) {
		RETURN_THROWS();
	}

	if ((pgsql = (PGconn *)zend_fetch_resource2(Z_RES_P(pgsql_link), "PostgreSQL link", le_link, le_plink)) == NULL) {
		RETURN_THROWS();
	}

	is_non_blocking = PQisnonblocking(pgsql);

	if (is_non_blocking == 0 && PQsetnonblocking(pgsql, 1) == -1) {
		php_error_docref(NULL, E_NOTICE, "Cannot set connection to nonblocking mode");
		RETURN_FALSE;
	}

	if (_php_pgsql_link_has_results(pgsql)) {
		php_error_docref(NULL, E_NOTICE,
			"There are results on this connection. Call pg_get_result() until it returns FALSE");
	}

	num_params = zend_hash_num_elements(Z_ARRVAL_P(pv_param_arr));
	if (num_params > 0) {
		int i = 0;
		params = (char **)safe_emalloc(sizeof(char *), num_params, 0);

		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(pv_param_arr), tmp) {

			if (Z_TYPE_P(tmp) == IS_NULL) {
				params[i] = NULL;
			} else {
				zend_string *tmp_str;
				zend_string *str = zval_get_tmp_string(tmp, &tmp_str);

				params[i] = estrndup(ZSTR_VAL(str), ZSTR_LEN(str));
				zend_tmp_string_release(tmp_str);
			}

			i++;
		} ZEND_HASH_FOREACH_END();
	}

	if (PQsendQueryParams(pgsql, query, num_params, NULL, (const char * const *)params, NULL, NULL, 0)) {
		_php_pgsql_free_params(params, num_params);
	} else if (is_non_blocking) {
		_php_pgsql_free_params(params, num_params);
		RETURN_FALSE;
	} else {
		if ((PGG(auto_reset_persistent) & 2) && PQstatus(pgsql) != CONNECTION_OK) {
			PQreset(pgsql);
		}
		if (!PQsendQueryParams(pgsql, query, num_params, NULL, (const char * const *)params, NULL, NULL, 0)) {
			_php_pgsql_free_params(params, num_params);
			RETURN_FALSE;
		}
	}

	if (is_non_blocking) {
		ret = PQflush(pgsql);
	} else {
		/* Wait to finish sending buffer */
		while ((ret = PQflush(pgsql))) {
			if (ret == -1) {
				php_error_docref(NULL, E_NOTICE, "Could not empty PostgreSQL send buffer");
				break;
			}
			usleep(10000);
		}

		if (PQsetnonblocking(pgsql, 0) != 0) {
			php_error_docref(NULL, E_NOTICE, "Cannot set connection to blocking mode");
		}
	}

	if (ret == 0) {
		RETURN_TRUE;
	} else if (ret == -1) {
		RETURN_FALSE;
	} else {
		RETURN_LONG(0);
	}
}
/* }}} */

/* {{{ Asynchronously prepare a query for future execution */
PHP_FUNCTION(pg_send_prepare)
{
	zval *pgsql_link;
	char *query, *stmtname;
	size_t stmtname_len, query_len;
	PGconn *pgsql;
	int is_non_blocking;
	int ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rss", &pgsql_link, &stmtname, &stmtname_len, &query, &query_len) == FAILURE) {
		RETURN_THROWS();
	}

	if ((pgsql = (PGconn *)zend_fetch_resource2(Z_RES_P(pgsql_link), "PostgreSQL link", le_link, le_plink)) == NULL) {
		RETURN_THROWS();
	}

	is_non_blocking = PQisnonblocking(pgsql);

	if (is_non_blocking == 0 && PQsetnonblocking(pgsql, 1) == -1) {
		php_error_docref(NULL, E_NOTICE, "Cannot set connection to nonblocking mode");
		RETURN_FALSE;
	}

	if (_php_pgsql_link_has_results(pgsql)) {
		php_error_docref(NULL, E_NOTICE,
			"There are results on this connection. Call pg_get_result() until it returns FALSE");
	}

	if (!PQsendPrepare(pgsql, stmtname, query, 0, NULL)) {
		if (is_non_blocking) {
			RETURN_FALSE;
		} else {
			if ((PGG(auto_reset_persistent) & 2) && PQstatus(pgsql) != CONNECTION_OK) {
				PQreset(pgsql);
			}
			if (!PQsendPrepare(pgsql, stmtname, query, 0, NULL)) {
				RETURN_FALSE;
			}
		}
	}

	if (is_non_blocking) {
		ret = PQflush(pgsql);
	} else {
		/* Wait to finish sending buffer */
		while ((ret = PQflush(pgsql))) {
			if (ret == -1) {
				php_error_docref(NULL, E_NOTICE, "Could not empty PostgreSQL send buffer");
				break;
			}
			usleep(10000);
		}
		if (PQsetnonblocking(pgsql, 0) != 0) {
			php_error_docref(NULL, E_NOTICE, "Cannot set connection to blocking mode");
		}
	}

	if (ret == 0) {
		RETURN_TRUE;
	} else if (ret == -1) {
		RETURN_FALSE;
	} else {
		RETURN_LONG(0);
	}
}
/* }}} */

/* {{{ Executes prevriously prepared stmtname asynchronously */
PHP_FUNCTION(pg_send_execute)
{
	zval *pgsql_link;
	zval *pv_param_arr, *tmp;
	int num_params = 0;
	char **params = NULL;
	char *stmtname;
	size_t stmtname_len;
	PGconn *pgsql;
	int is_non_blocking;
	int ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rsa", &pgsql_link, &stmtname, &stmtname_len, &pv_param_arr) == FAILURE) {
		RETURN_THROWS();
	}

	if ((pgsql = (PGconn *)zend_fetch_resource2(Z_RES_P(pgsql_link), "PostgreSQL link", le_link, le_plink)) == NULL) {
		RETURN_THROWS();
	}

	is_non_blocking = PQisnonblocking(pgsql);

	if (is_non_blocking == 0 && PQsetnonblocking(pgsql, 1) == -1) {
		php_error_docref(NULL, E_NOTICE, "Cannot set connection to nonblocking mode");
		RETURN_FALSE;
	}

	if (_php_pgsql_link_has_results(pgsql)) {
		php_error_docref(NULL, E_NOTICE,
			"There are results on this connection. Call pg_get_result() until it returns FALSE");
	}

	num_params = zend_hash_num_elements(Z_ARRVAL_P(pv_param_arr));
	if (num_params > 0) {
		int i = 0;
		params = (char **)safe_emalloc(sizeof(char *), num_params, 0);

		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(pv_param_arr), tmp) {

			if (Z_TYPE_P(tmp) == IS_NULL) {
				params[i] = NULL;
			} else {
				zend_string *tmp_str = zval_try_get_string(tmp);
				if (UNEXPECTED(!tmp)) {
					_php_pgsql_free_params(params, num_params);
					return;
				}
				params[i] = estrndup(ZSTR_VAL(tmp_str), ZSTR_LEN(tmp_str));
				zend_string_release(tmp_str);
			}

			i++;
		} ZEND_HASH_FOREACH_END();
	}

	if (PQsendQueryPrepared(pgsql, stmtname, num_params, (const char * const *)params, NULL, NULL, 0)) {
		_php_pgsql_free_params(params, num_params);
	} else if (is_non_blocking) {
		_php_pgsql_free_params(params, num_params);
		RETURN_FALSE;
	} else {
		if ((PGG(auto_reset_persistent) & 2) && PQstatus(pgsql) != CONNECTION_OK) {
			PQreset(pgsql);
		}
		if (!PQsendQueryPrepared(pgsql, stmtname, num_params, (const char * const *)params, NULL, NULL, 0)) {
			_php_pgsql_free_params(params, num_params);
			RETURN_FALSE;
		}
	}

	if (is_non_blocking) {
		ret = PQflush(pgsql);
	} else {
		/* Wait to finish sending buffer */
		while ((ret = PQflush(pgsql))) {
			if (ret == -1) {
				php_error_docref(NULL, E_NOTICE, "Could not empty PostgreSQL send buffer");
				break;
			}
			usleep(10000);
		}
		if (PQsetnonblocking(pgsql, 0) != 0) {
			php_error_docref(NULL, E_NOTICE, "Cannot set connection to blocking mode");
		}
	}

	if (ret == 0) {
		RETURN_TRUE;
	} else if (ret == -1) {
		RETURN_FALSE;
	} else {
		RETURN_LONG(0);
	}
}
/* }}} */

/* {{{ Get asynchronous query result */
PHP_FUNCTION(pg_get_result)
{
	zval *pgsql_link;
	PGconn *pgsql;
	PGresult *pgsql_result;
	pgsql_result_handle *pg_result;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &pgsql_link) == FAILURE) {
		RETURN_THROWS();
	}

	if ((pgsql = (PGconn *)zend_fetch_resource2(Z_RES_P(pgsql_link), "PostgreSQL link", le_link, le_plink)) == NULL) {
		RETURN_THROWS();
	}

	pgsql_result = PQgetResult(pgsql);
	if (!pgsql_result) {
		/* no result */
		RETURN_FALSE;
	}
	pg_result = (pgsql_result_handle *) emalloc(sizeof(pgsql_result_handle));
	pg_result->conn = pgsql;
	pg_result->result = pgsql_result;
	pg_result->row = 0;
	RETURN_RES(zend_register_resource(pg_result, le_result));
}
/* }}} */

/* {{{ Get status of query result */
PHP_FUNCTION(pg_result_status)
{
	zval *result;
	zend_long result_type = PGSQL_STATUS_LONG;
	ExecStatusType status;
	PGresult *pgsql_result;
	pgsql_result_handle *pg_result;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r|l", &result, &result_type) == FAILURE) {
		RETURN_THROWS();
	}

	if ((pg_result = (pgsql_result_handle *)zend_fetch_resource(Z_RES_P(result), "PostgreSQL result", le_result)) == NULL) {
		RETURN_THROWS();
	}

	pgsql_result = pg_result->result;
	if (result_type == PGSQL_STATUS_LONG) {
		status = PQresultStatus(pgsql_result);
		RETURN_LONG((int)status);
	}
	else if (result_type == PGSQL_STATUS_STRING) {
		RETURN_STRING(PQcmdStatus(pgsql_result));
	} else {
		zend_argument_value_error(2, "must be either PGSQL_STATUS_LONG or PGSQL_STATUS_STRING");
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ Get asynchronous notification */
PHP_FUNCTION(pg_get_notify)
{
	zval *pgsql_link;
	zend_long result_type = PGSQL_ASSOC;
	PGconn *pgsql;
	PGnotify *pgsql_notify;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r|l", &pgsql_link, &result_type) == FAILURE) {
		RETURN_THROWS();
	}

	if ((pgsql = (PGconn *)zend_fetch_resource2(Z_RES_P(pgsql_link), "PostgreSQL link", le_link, le_plink)) == NULL) {
		RETURN_THROWS();
	}

	if (!(result_type & PGSQL_BOTH)) {
		zend_argument_value_error(2, "must be one of PGSQL_ASSOC, PGSQL_NUM, or PGSQL_BOTH");
		RETURN_THROWS();
	}

	PQconsumeInput(pgsql);
	pgsql_notify = PQnotifies(pgsql);
	if (!pgsql_notify) {
		/* no notify message */
		RETURN_FALSE;
	}
	array_init(return_value);
	if (result_type & PGSQL_NUM) {
		add_index_string(return_value, 0, pgsql_notify->relname);
		add_index_long(return_value, 1, pgsql_notify->be_pid);
		if (PQprotocolVersion(pgsql) >= 3 && atof(PQparameterStatus(pgsql, "server_version")) >= 9.0) {
			add_index_string(return_value, 2, pgsql_notify->extra);
		}
	}
	if (result_type & PGSQL_ASSOC) {
		add_assoc_string(return_value, "message", pgsql_notify->relname);
		add_assoc_long(return_value, "pid", pgsql_notify->be_pid);
		if (PQprotocolVersion(pgsql) >= 3 && atof(PQparameterStatus(pgsql, "server_version")) >= 9.0) {
			add_assoc_string(return_value, "payload", pgsql_notify->extra);
		}
	}
	PQfreemem(pgsql_notify);
}
/* }}} */

/* {{{ Get backend(server) pid */
PHP_FUNCTION(pg_get_pid)
{
	zval *pgsql_link;
	PGconn *pgsql;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &pgsql_link) == FAILURE) {
		RETURN_THROWS();
	}

	if ((pgsql = (PGconn *)zend_fetch_resource2(Z_RES_P(pgsql_link), "PostgreSQL link", le_link, le_plink)) == NULL) {
		RETURN_THROWS();
	}

	RETURN_LONG(PQbackendPID(pgsql));
}
/* }}} */

static ssize_t php_pgsql_fd_write(php_stream *stream, const char *buf, size_t count) /* {{{ */
{
	return -1;
}
/* }}} */

static ssize_t php_pgsql_fd_read(php_stream *stream, char *buf, size_t count) /* {{{ */
{
	return -1;
}
/* }}} */

static int php_pgsql_fd_close(php_stream *stream, int close_handle) /* {{{ */
{
	return EOF;
}
/* }}} */

static int php_pgsql_fd_flush(php_stream *stream) /* {{{ */
{
	return FAILURE;
}
/* }}} */

static int php_pgsql_fd_set_option(php_stream *stream, int option, int value, void *ptrparam) /* {{{ */
{
	PGconn *pgsql = (PGconn *) stream->abstract;
	switch (option) {
		case PHP_STREAM_OPTION_BLOCKING:
			return PQsetnonblocking(pgsql, value);
		default:
			return FAILURE;
	}
}
/* }}} */

static int php_pgsql_fd_cast(php_stream *stream, int cast_as, void **ret) /* {{{ */
{
	PGconn *pgsql = (PGconn *) stream->abstract;

	switch (cast_as)	{
		case PHP_STREAM_AS_FD_FOR_SELECT:
		case PHP_STREAM_AS_FD:
		case PHP_STREAM_AS_SOCKETD:
			if (ret) {
				int fd_number = PQsocket(pgsql);
				if (fd_number == -1) {
					return FAILURE;
				}

				*(php_socket_t *)ret = fd_number;
				return SUCCESS;
			}
		default:
			return FAILURE;
	}
}
/* }}} */

/* {{{ Get a read-only handle to the socket underlying the pgsql connection */
PHP_FUNCTION(pg_socket)
{
	zval *pgsql_link;
	php_stream *stream;
	PGconn *pgsql;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &pgsql_link) == FAILURE) {
		RETURN_THROWS();
	}

	if ((pgsql = (PGconn *)zend_fetch_resource2(Z_RES_P(pgsql_link), "PostgreSQL link", le_link, le_plink)) == NULL) {
		RETURN_THROWS();
	}

	stream = php_stream_alloc(&php_stream_pgsql_fd_ops, pgsql, NULL, "r");

	if (stream) {
		php_stream_to_zval(stream, return_value);
		return;
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ Reads input on the connection */
PHP_FUNCTION(pg_consume_input)
{
	zval *pgsql_link;
	PGconn *pgsql;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &pgsql_link) == FAILURE) {
		RETURN_THROWS();
	}

	if ((pgsql = (PGconn *)zend_fetch_resource2(Z_RES_P(pgsql_link), "PostgreSQL link", le_link, le_plink)) == NULL) {
		RETURN_THROWS();
	}

	RETURN_BOOL(PQconsumeInput(pgsql));
}
/* }}} */

/* {{{ Flush outbound query data on the connection */
PHP_FUNCTION(pg_flush)
{
	zval *pgsql_link;
	PGconn *pgsql;
	int ret;
	int is_non_blocking;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &pgsql_link) == FAILURE) {
		RETURN_THROWS();
	}

	if ((pgsql = (PGconn *)zend_fetch_resource2(Z_RES_P(pgsql_link), "PostgreSQL link", le_link, le_plink)) == NULL) {
		RETURN_THROWS();
	}

	is_non_blocking = PQisnonblocking(pgsql);

	if (is_non_blocking == 0 && PQsetnonblocking(pgsql, 1) == -1) {
		php_error_docref(NULL, E_NOTICE, "Cannot set connection to nonblocking mode");
		RETURN_FALSE;
	}

	ret = PQflush(pgsql);

	if (is_non_blocking == 0 && PQsetnonblocking(pgsql, 0) == -1) {
		php_error_docref(NULL, E_NOTICE, "Failed resetting connection to blocking mode");
	}

	switch (ret) {
		case 0: RETURN_TRUE; break;
		case 1: RETURN_LONG(0); break;
		default: RETURN_FALSE;
	}
}
/* }}} */

/* {{{ php_pgsql_meta_data
 * table_name must not be empty
 * TODO: Add meta_data cache for better performance
 */
PHP_PGSQL_API int php_pgsql_meta_data(PGconn *pg_link, const char *table_name, zval *meta, zend_bool extended)
{
	PGresult *pg_result;
	char *src, *tmp_name, *tmp_name2 = NULL;
	char *escaped;
	smart_str querystr = {0};
	size_t new_len;
	int i, num_rows;
	zval elem;

	ZEND_ASSERT(*table_name);

	src = estrdup(table_name);
	tmp_name = php_strtok_r(src, ".", &tmp_name2);
	if (!tmp_name) {
		// TODO ValueError (empty table name)?
		efree(src);
		php_error_docref(NULL, E_WARNING, "The table name must be specified");
		return FAILURE;
	}
	if (!tmp_name2 || !*tmp_name2) {
		/* Default schema */
		tmp_name2 = tmp_name;
		tmp_name = "public";
	}

	if (extended) {
		smart_str_appends(&querystr,
						  "SELECT a.attname, a.attnum, t.typname, a.attlen, a.attnotNULL, a.atthasdef, a.attndims, t.typtype, "
						  "d.description "
						  "FROM pg_class as c "
						  " JOIN pg_attribute a ON (a.attrelid = c.oid) "
						  " JOIN pg_type t ON (a.atttypid = t.oid) "
						  " JOIN pg_namespace n ON (c.relnamespace = n.oid) "
						  " LEFT JOIN pg_description d ON (d.objoid=a.attrelid AND d.objsubid=a.attnum AND c.oid=d.objoid) "
						  "WHERE a.attnum > 0  AND c.relname = '");
	} else {
		smart_str_appends(&querystr,
						  "SELECT a.attname, a.attnum, t.typname, a.attlen, a.attnotnull, a.atthasdef, a.attndims, t.typtype "
						  "FROM pg_class as c "
						  " JOIN pg_attribute a ON (a.attrelid = c.oid) "
						  " JOIN pg_type t ON (a.atttypid = t.oid) "
						  " JOIN pg_namespace n ON (c.relnamespace = n.oid) "
						  "WHERE a.attnum > 0 AND c.relname = '");
	}
	escaped = (char *)safe_emalloc(strlen(tmp_name2), 2, 1);
	new_len = PQescapeStringConn(pg_link, escaped, tmp_name2, strlen(tmp_name2), NULL);
	if (new_len) {
		smart_str_appendl(&querystr, escaped, new_len);
	}
	efree(escaped);

	smart_str_appends(&querystr, "' AND n.nspname = '");
	escaped = (char *)safe_emalloc(strlen(tmp_name), 2, 1);
	new_len = PQescapeStringConn(pg_link, escaped, tmp_name, strlen(tmp_name), NULL);
	if (new_len) {
		smart_str_appendl(&querystr, escaped, new_len);
	}
	efree(escaped);

	smart_str_appends(&querystr, "' ORDER BY a.attnum;");
	smart_str_0(&querystr);
	efree(src);

	pg_result = PQexec(pg_link, ZSTR_VAL(querystr.s));
	if (PQresultStatus(pg_result) != PGRES_TUPLES_OK || (num_rows = PQntuples(pg_result)) == 0) {
		php_error_docref(NULL, E_WARNING, "Table '%s' doesn't exists", table_name);
		smart_str_free(&querystr);
		PQclear(pg_result);
		return FAILURE;
	}
	smart_str_free(&querystr);

	for (i = 0; i < num_rows; i++) {
		char *name;
		array_init(&elem);
		/* pg_attribute.attnum */
		add_assoc_long_ex(&elem, "num", sizeof("num") - 1, atoi(PQgetvalue(pg_result, i, 1)));
		/* pg_type.typname */
		add_assoc_string_ex(&elem, "type", sizeof("type") - 1, PQgetvalue(pg_result, i, 2));
		/* pg_attribute.attlen */
		add_assoc_long_ex(&elem, "len", sizeof("len") - 1, atoi(PQgetvalue(pg_result,i,3)));
		/* pg_attribute.attnonull */
		add_assoc_bool_ex(&elem, "not null", sizeof("not null") - 1, !strcmp(PQgetvalue(pg_result, i, 4), "t"));
		/* pg_attribute.atthasdef */
		add_assoc_bool_ex(&elem, "has default", sizeof("has default") - 1, !strcmp(PQgetvalue(pg_result,i,5), "t"));
		/* pg_attribute.attndims */
		add_assoc_long_ex(&elem, "array dims", sizeof("array dims") - 1, atoi(PQgetvalue(pg_result, i, 6)));
		/* pg_type.typtype */
		add_assoc_bool_ex(&elem, "is enum", sizeof("is enum") - 1, !strcmp(PQgetvalue(pg_result, i, 7), "e"));
		if (extended) {
			/* pg_type.typtype */
			add_assoc_bool_ex(&elem, "is base", sizeof("is base") - 1, !strcmp(PQgetvalue(pg_result, i, 7), "b"));
			add_assoc_bool_ex(&elem, "is composite", sizeof("is composite") - 1, !strcmp(PQgetvalue(pg_result, i, 7), "c"));
			add_assoc_bool_ex(&elem, "is pesudo", sizeof("is pesudo") - 1, !strcmp(PQgetvalue(pg_result, i, 7), "p"));
			/* pg_description.description */
			add_assoc_string_ex(&elem, "description", sizeof("description") - 1, PQgetvalue(pg_result, i, 8));
		}
		/* pg_attribute.attname */
		name = PQgetvalue(pg_result,i,0);
		add_assoc_zval(meta, name, &elem);
	}
	PQclear(pg_result);

	return SUCCESS;
}

/* }}} */

/* {{{ Get meta_data */
PHP_FUNCTION(pg_meta_data)
{
	zval *pgsql_link;
	char *table_name;
	size_t table_name_len;
	zend_bool extended=0;
	PGconn *pgsql;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rs|b",
							  &pgsql_link, &table_name, &table_name_len, &extended) == FAILURE) {
		RETURN_THROWS();
	}

	if ((pgsql = (PGconn *)zend_fetch_resource2(Z_RES_P(pgsql_link), "PostgreSQL link", le_link, le_plink)) == NULL) {
		RETURN_THROWS();
	}

	/* php_pgsql_meta_data() asserts that table_name is not empty */
	if (table_name_len == 0) {
		zend_argument_value_error(2, "cannot be empty");
		RETURN_THROWS();
	}

	array_init(return_value);
	if (php_pgsql_meta_data(pgsql, table_name, return_value, extended) == FAILURE) {
		zend_array_destroy(Z_ARR_P(return_value)); /* destroy array */
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ php_pgsql_get_data_type */
static php_pgsql_data_type php_pgsql_get_data_type(const char *type_name, size_t len)
{
	/* This is stupid way to do. I'll fix it when I decied how to support
	   user defined types. (Yasuo) */

	/* boolean */
	if (!strcmp(type_name, "bool")|| !strcmp(type_name, "boolean"))
		return PG_BOOL;
	/* object id */
	if (!strcmp(type_name, "oid"))
		return PG_OID;
	/* integer */
	if (!strcmp(type_name, "int2") || !strcmp(type_name, "smallint"))
		return PG_INT2;
	if (!strcmp(type_name, "int4") || !strcmp(type_name, "integer"))
		return PG_INT4;
	if (!strcmp(type_name, "int8") || !strcmp(type_name, "bigint"))
		return PG_INT8;
	/* real and other */
	if (!strcmp(type_name, "float4") || !strcmp(type_name, "real"))
		return PG_FLOAT4;
	if (!strcmp(type_name, "float8") || !strcmp(type_name, "double precision"))
		return PG_FLOAT8;
	if (!strcmp(type_name, "numeric"))
		return PG_NUMERIC;
	if (!strcmp(type_name, "money"))
		return PG_MONEY;
	/* character */
	if (!strcmp(type_name, "text"))
		return PG_TEXT;
	if (!strcmp(type_name, "bpchar") || !strcmp(type_name, "character"))
		return PG_CHAR;
	if (!strcmp(type_name, "varchar") || !strcmp(type_name, "character varying"))
		return PG_VARCHAR;
	/* time and interval */
	if (!strcmp(type_name, "abstime"))
		return PG_UNIX_TIME;
	if (!strcmp(type_name, "reltime"))
		return PG_UNIX_TIME_INTERVAL;
	if (!strcmp(type_name, "tinterval"))
		return PG_UNIX_TIME_INTERVAL;
	if (!strcmp(type_name, "date"))
		return PG_DATE;
	if (!strcmp(type_name, "time"))
		return PG_TIME;
	if (!strcmp(type_name, "time with time zone") || !strcmp(type_name, "timetz"))
		return PG_TIME_WITH_TIMEZONE;
	if (!strcmp(type_name, "timestamp without time zone") || !strcmp(type_name, "timestamp"))
		return PG_TIMESTAMP;
	if (!strcmp(type_name, "timestamp with time zone") || !strcmp(type_name, "timestamptz"))
		return PG_TIMESTAMP_WITH_TIMEZONE;
	if (!strcmp(type_name, "interval"))
		return PG_INTERVAL;
	/* binary */
	if (!strcmp(type_name, "bytea"))
		return PG_BYTEA;
	/* network */
	if (!strcmp(type_name, "cidr"))
		return PG_CIDR;
	if (!strcmp(type_name, "inet"))
		return PG_INET;
	if (!strcmp(type_name, "macaddr"))
		return PG_MACADDR;
	/* bit */
	if (!strcmp(type_name, "bit"))
		return PG_BIT;
	if (!strcmp(type_name, "bit varying"))
		return PG_VARBIT;
	/* geometric */
	if (!strcmp(type_name, "line"))
		return PG_LINE;
	if (!strcmp(type_name, "lseg"))
		return PG_LSEG;
	if (!strcmp(type_name, "box"))
		return PG_BOX;
	if (!strcmp(type_name, "path"))
		return PG_PATH;
	if (!strcmp(type_name, "point"))
		return PG_POINT;
	if (!strcmp(type_name, "polygon"))
		return PG_POLYGON;
	if (!strcmp(type_name, "circle"))
		return PG_CIRCLE;

	return PG_UNKNOWN;
}
/* }}} */

/* {{{ php_pgsql_convert_match
 * test field value with regular expression specified.
 */
static int php_pgsql_convert_match(const char *str, size_t str_len, const char *regex , size_t regex_len, int icase)
{
	pcre2_code *re;
	PCRE2_SIZE err_offset;
	int res, errnumber;
	uint32_t options = PCRE2_NO_AUTO_CAPTURE;
	size_t i;
	pcre2_match_data *match_data;

	/* Check invalid chars for POSIX regex */
	for (i = 0; i < str_len; i++) {
		if (str[i] == '\n' ||
			str[i] == '\r' ||
			str[i] == '\0' ) {
			return FAILURE;
		}
	}

	if (icase) {
		options |= PCRE2_CASELESS;
	}

	re = pcre2_compile((PCRE2_SPTR)regex, regex_len, options, &errnumber, &err_offset, php_pcre_cctx());
	if (NULL == re) {
		PCRE2_UCHAR err_msg[128];
		pcre2_get_error_message(errnumber, err_msg, sizeof(err_msg));
		php_error_docref(NULL, E_WARNING, "Cannot compile regex: '%s'", err_msg);
		return FAILURE;
	}

	match_data = php_pcre_create_match_data(0, re);
	if (NULL == match_data) {
		pcre2_code_free(re);
		php_error_docref(NULL, E_WARNING, "Cannot allocate match data");
		return FAILURE;
	}
	res = pcre2_match(re, (PCRE2_SPTR)str, str_len, 0, 0, match_data, php_pcre_mctx());
	php_pcre_free_match_data(match_data);
	pcre2_code_free(re);

	if (res == PCRE2_ERROR_NOMATCH) {
		return FAILURE;
	} else if (res < 0) {
		php_error_docref(NULL, E_WARNING, "Cannot exec regex");
		return FAILURE;
	}
	return SUCCESS;
}

/* }}} */

/* {{{ php_pgsql_add_quote
 * add quotes around string.
 */
static int php_pgsql_add_quotes(zval *src, zend_bool should_free)
{
	smart_str str = {0};

	assert(Z_TYPE_P(src) == IS_STRING);
	assert(should_free == 1 || should_free == 0);

	smart_str_appendc(&str, 'E');
	smart_str_appendc(&str, '\'');
	smart_str_appendl(&str, Z_STRVAL_P(src), Z_STRLEN_P(src));
	smart_str_appendc(&str, '\'');
	smart_str_0(&str);

	if (should_free) {
		zval_ptr_dtor(src);
	}
	ZVAL_NEW_STR(src, str.s);

	return SUCCESS;
}
/* }}} */

/* Raise E_NOTICE to E_WARNING or Error? */
#define PGSQL_CONV_CHECK_IGNORE() \
	if (!err && Z_TYPE(new_val) == IS_STRING && !strcmp(Z_STRVAL(new_val), "NULL")) { \
		/* if new_value is string "NULL" and field has default value, remove element to use default value */ \
		if (!(opt & PGSQL_CONV_IGNORE_DEFAULT) && Z_TYPE_P(has_default) == IS_TRUE) { \
			zval_ptr_dtor(&new_val); \
			skip_field = 1; \
		} \
		/* raise error if it's not null and cannot be ignored */ \
		else if (!(opt & PGSQL_CONV_IGNORE_NOT_NULL) && Z_TYPE_P(not_null) == IS_TRUE) { \
			php_error_docref(NULL, E_NOTICE, "Detected NULL for 'NOT NULL' field '%s'", ZSTR_VAL(field)); \
			err = 1; \
		} \
	}

/* {{{ php_pgsql_convert
 * check and convert array values (fieldname=>value pair) for sql
 */
PHP_PGSQL_API int php_pgsql_convert(PGconn *pg_link, const char *table_name, const zval *values, zval *result, zend_ulong opt)
{
	zend_string *field = NULL;
	zval meta, *def, *type, *not_null, *has_default, *is_enum, *val, new_val;
	int err = 0, skip_field;
	php_pgsql_data_type data_type;

	ZEND_ASSERT(pg_link != NULL);
	ZEND_ASSERT(Z_TYPE_P(values) == IS_ARRAY);
	ZEND_ASSERT(Z_TYPE_P(result) == IS_ARRAY);
	ZEND_ASSERT(!(opt & ~PGSQL_CONV_OPTS));
	ZEND_ASSERT(table_name);
	/* Table name cannot be empty for php_pgsql_meta_data() */
	ZEND_ASSERT(*table_name);

	array_init(&meta);
/* table_name is escaped by php_pgsql_meta_data */
	if (php_pgsql_meta_data(pg_link, table_name, &meta, 0) == FAILURE) {
		zval_ptr_dtor(&meta);
		return FAILURE;
	}

	ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(values), field, val) {
		skip_field = 0;
		ZVAL_NULL(&new_val);

		/* TODO: Check when meta data can be broken and see if can use assertions instead */

		if (!err && field == NULL) {
			zend_value_error("Array of values must be an associative array with string keys");
			err = 1;
		}

		if (!err && (def = zend_hash_find(Z_ARRVAL(meta), field)) == NULL) {
			php_error_docref(NULL, E_NOTICE, "Invalid field name (%s) in values", ZSTR_VAL(field));
			err = 1;
		}
		if (!err && (type = zend_hash_str_find(Z_ARRVAL_P(def), "type", sizeof("type") - 1)) == NULL) {
			php_error_docref(NULL, E_NOTICE, "Detected broken meta data. Missing 'type'");
			err = 1;
		}
		if (!err && (not_null = zend_hash_str_find(Z_ARRVAL_P(def), "not null", sizeof("not null") - 1)) == NULL) {
			php_error_docref(NULL, E_NOTICE, "Detected broken meta data. Missing 'not null'");
			err = 1;
		}
		if (!err && (has_default = zend_hash_str_find(Z_ARRVAL_P(def), "has default", sizeof("has default") - 1)) == NULL) {
			php_error_docref(NULL, E_NOTICE, "Detected broken meta data. Missing 'has default'");
			err = 1;
		}
		if (!err && (is_enum = zend_hash_str_find(Z_ARRVAL_P(def), "is enum", sizeof("is enum") - 1)) == NULL) {
			php_error_docref(NULL, E_NOTICE, "Detected broken meta data. Missing 'is enum'");
			err = 1;
		}
		if (!err && (Z_TYPE_P(val) == IS_ARRAY || Z_TYPE_P(val) == IS_OBJECT || Z_TYPE_P(val) == IS_RESOURCE)) {
			zend_type_error("Values must be of type string|int|float|bool|null, %s given", zend_zval_type_name(val));
			err = 1;
		}
		if (err) {
			break; /* break out for() */
		}

		convert_to_boolean(is_enum);
		if (Z_TYPE_P(is_enum) == IS_TRUE) {
			/* enums need to be treated like strings */
			data_type = PG_TEXT;
		} else {
			data_type = php_pgsql_get_data_type(Z_STRVAL_P(type), Z_STRLEN_P(type));
		}

		/* TODO: Should E_NOTICE be converted to type error if PHP type cannot be converted to field type? */
		switch(data_type)
		{
			case PG_BOOL:
				switch (Z_TYPE_P(val)) {
					case IS_STRING:
						if (Z_STRLEN_P(val) == 0) {
							ZVAL_STRING(&new_val, "NULL");
						}
						else {
							if (!strcmp(Z_STRVAL_P(val), "t") || !strcmp(Z_STRVAL_P(val), "T") ||
								!strcmp(Z_STRVAL_P(val), "y") || !strcmp(Z_STRVAL_P(val), "Y") ||
								!strcmp(Z_STRVAL_P(val), "true") || !strcmp(Z_STRVAL_P(val), "True") ||
								!strcmp(Z_STRVAL_P(val), "yes") || !strcmp(Z_STRVAL_P(val), "Yes") ||
								!strcmp(Z_STRVAL_P(val), "1")) {
								ZVAL_STRINGL(&new_val, "'t'", sizeof("'t'")-1);
							}
							else if (!strcmp(Z_STRVAL_P(val), "f") || !strcmp(Z_STRVAL_P(val), "F") ||
									 !strcmp(Z_STRVAL_P(val), "n") || !strcmp(Z_STRVAL_P(val), "N") ||
									 !strcmp(Z_STRVAL_P(val), "false") ||  !strcmp(Z_STRVAL_P(val), "False") ||
									 !strcmp(Z_STRVAL_P(val), "no") ||  !strcmp(Z_STRVAL_P(val), "No") ||
									 !strcmp(Z_STRVAL_P(val), "0")) {
								ZVAL_STRINGL(&new_val, "'f'", sizeof("'f'")-1);
							}
							else {
								php_error_docref(NULL, E_NOTICE, "Detected invalid value (%s) for PostgreSQL %s field (%s)", Z_STRVAL_P(val), Z_STRVAL_P(type), ZSTR_VAL(field));
								err = 1;
							}
						}
						break;

					case IS_LONG:
						if (Z_LVAL_P(val)) {
							ZVAL_STRINGL(&new_val, "'t'", sizeof("'t'")-1);
						}
						else {
							ZVAL_STRINGL(&new_val, "'f'", sizeof("'f'")-1);
						}
						break;

					case IS_TRUE:
						ZVAL_STRINGL(&new_val, "'t'", sizeof("'t'")-1);
						break;

					case IS_FALSE:
						ZVAL_STRINGL(&new_val, "'f'", sizeof("'f'")-1);
						break;

					case IS_NULL:
						ZVAL_STRINGL(&new_val, "NULL", sizeof("NULL")-1);
						break;

					default:
						err = 1;
				}
				PGSQL_CONV_CHECK_IGNORE();
				if (err) {
					php_error_docref(NULL, E_NOTICE, "Expects string, null, long or boolelan value for PostgreSQL '%s' (%s)", Z_STRVAL_P(type), ZSTR_VAL(field));
				}
				break;

			case PG_OID:
			case PG_INT2:
			case PG_INT4:
			case PG_INT8:
				switch (Z_TYPE_P(val)) {
					case IS_STRING:
						if (Z_STRLEN_P(val) == 0) {
							ZVAL_STRINGL(&new_val, "NULL", sizeof("NULL")-1);
						}
						else {
							/* FIXME: better regex must be used */
#define REGEX0 "^([+-]{0,1}[0-9]+)$"
							if (php_pgsql_convert_match(Z_STRVAL_P(val), Z_STRLEN_P(val), REGEX0, sizeof(REGEX0)-1, 0) == FAILURE) {
								err = 1;
							}
							else {
								ZVAL_STRINGL(&new_val, Z_STRVAL_P(val), Z_STRLEN_P(val));
							}
#undef REGEX0
						}
						break;

					case IS_DOUBLE:
						ZVAL_DOUBLE(&new_val, Z_DVAL_P(val));
						convert_to_long_ex(&new_val);
						break;

					case IS_LONG:
						ZVAL_LONG(&new_val, Z_LVAL_P(val));
						break;

					case IS_NULL:
						ZVAL_STRINGL(&new_val, "NULL", sizeof("NULL")-1);
						break;

					default:
						err = 1;
				}
				PGSQL_CONV_CHECK_IGNORE();
				if (err) {
					php_error_docref(NULL, E_NOTICE, "Expects NULL, string, long or double value for pgsql '%s' (%s)", Z_STRVAL_P(type), ZSTR_VAL(field));
				}
				break;

			case PG_NUMERIC:
			case PG_MONEY:
			case PG_FLOAT4:
			case PG_FLOAT8:
				switch (Z_TYPE_P(val)) {
					case IS_STRING:
						if (Z_STRLEN_P(val) == 0) {
							ZVAL_STRINGL(&new_val, "NULL", sizeof("NULL")-1);
						}
						else {
#define REGEX0 "^[-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?$"
#define REGEX1 "^[+-]{0,1}(inf)(inity){0,1}$"
							/* better regex? */
							if (php_pgsql_convert_match(Z_STRVAL_P(val), Z_STRLEN_P(val), REGEX0, sizeof(REGEX0)-1, 0) == FAILURE) {
								if (php_pgsql_convert_match(Z_STRVAL_P(val), Z_STRLEN_P(val), REGEX1, sizeof(REGEX1)-1, 1) == FAILURE) {
									err = 1;
								} else {
									ZVAL_STRING(&new_val, Z_STRVAL_P(val));
									php_pgsql_add_quotes(&new_val, 1);
								}
							}
							else {
								ZVAL_STRING(&new_val, Z_STRVAL_P(val));
							}
#undef REGEX0
#undef REGEX1
						}
						break;

					case IS_LONG:
						ZVAL_LONG(&new_val, Z_LVAL_P(val));
						break;

					case IS_DOUBLE:
						ZVAL_DOUBLE(&new_val, Z_DVAL_P(val));
						break;

					case IS_NULL:
						ZVAL_STRINGL(&new_val, "NULL", sizeof("NULL")-1);
						break;

					default:
						err = 1;
				}
				PGSQL_CONV_CHECK_IGNORE();
				if (err) {
					php_error_docref(NULL, E_NOTICE, "Expects NULL, string, long or double value for PostgreSQL '%s' (%s)", Z_STRVAL_P(type), ZSTR_VAL(field));
				}
				break;

				/* Exotic types are handled as string also.
				   Please feel free to add more valitions. Invalid query fails
				   at execution anyway. */
			case PG_TEXT:
			case PG_CHAR:
			case PG_VARCHAR:
				/* bit */
			case PG_BIT:
			case PG_VARBIT:
				/* geometric */
			case PG_LINE:
			case PG_LSEG:
			case PG_POINT:
			case PG_BOX:
			case PG_PATH:
			case PG_POLYGON:
			case PG_CIRCLE:
				/* unknown. JSON, Array etc */
			case PG_UNKNOWN:
				switch (Z_TYPE_P(val)) {
					case IS_STRING:
						if (Z_STRLEN_P(val) == 0) {
							if (opt & PGSQL_CONV_FORCE_NULL) {
								ZVAL_STRINGL(&new_val, "NULL", sizeof("NULL")-1);
							} else {
								ZVAL_STRINGL(&new_val, "''", sizeof("''")-1);
							}
						}
						else {
							zend_string *str;
							/* PostgreSQL ignores \0 */
							str = zend_string_alloc(Z_STRLEN_P(val) * 2, 0);
							/* better to use PGSQLescapeLiteral since PGescapeStringConn does not handle special \ */
							ZSTR_LEN(str) = PQescapeStringConn(pg_link, ZSTR_VAL(str), Z_STRVAL_P(val), Z_STRLEN_P(val), NULL);
							str = zend_string_truncate(str, ZSTR_LEN(str), 0);
							ZVAL_NEW_STR(&new_val, str);
							php_pgsql_add_quotes(&new_val, 1);
						}
						break;

					case IS_LONG:
						ZVAL_STR(&new_val, zend_long_to_str(Z_LVAL_P(val)));
						break;

					case IS_DOUBLE:
						ZVAL_DOUBLE(&new_val, Z_DVAL_P(val));
						convert_to_string_ex(&new_val);
						break;

					case IS_NULL:
						ZVAL_STRINGL(&new_val, "NULL", sizeof("NULL")-1);
						break;

					default:
						err = 1;
				}
				PGSQL_CONV_CHECK_IGNORE();
				if (err) {
					php_error_docref(NULL, E_NOTICE, "Expects NULL, string, long or double value for PostgreSQL '%s' (%s)", Z_STRVAL_P(type), ZSTR_VAL(field));
				}
				break;

			case PG_UNIX_TIME:
			case PG_UNIX_TIME_INTERVAL:
				/* these are the actallay a integer */
				switch (Z_TYPE_P(val)) {
					case IS_STRING:
						if (Z_STRLEN_P(val) == 0) {
							ZVAL_STRINGL(&new_val, "NULL", sizeof("NULL")-1);
						}
						else {
							/* better regex? */
							if (php_pgsql_convert_match(Z_STRVAL_P(val), Z_STRLEN_P(val), "^[0-9]+$", sizeof("^[0-9]+$")-1, 0) == FAILURE) {
								err = 1;
							}
							else {
								ZVAL_STRINGL(&new_val, Z_STRVAL_P(val), Z_STRLEN_P(val));
								convert_to_long_ex(&new_val);
							}
						}
						break;

					case IS_DOUBLE:
						ZVAL_DOUBLE(&new_val, Z_DVAL_P(val));
						convert_to_long_ex(&new_val);
						break;

					case IS_LONG:
						ZVAL_LONG(&new_val, Z_LVAL_P(val));
						break;

					case IS_NULL:
						ZVAL_STRINGL(&new_val, "NULL", sizeof("NULL")-1);
						break;

					default:
						err = 1;
				}
				PGSQL_CONV_CHECK_IGNORE();
				if (err) {
					php_error_docref(NULL, E_NOTICE, "Expects NULL, string, long or double value for '%s' (%s)", Z_STRVAL_P(type), ZSTR_VAL(field));
				}
				break;

			case PG_CIDR:
			case PG_INET:
				switch (Z_TYPE_P(val)) {
					case IS_STRING:
						if (Z_STRLEN_P(val) == 0) {
							ZVAL_STRINGL(&new_val, "NULL", sizeof("NULL")-1);
						}
						else {
#define REGEX0 "^((25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])\\.){3,3}(25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])(\\/[0-9]{1,3})?$"
#define REGEX1 "^(([0-9a-fA-F]{1,4}:){7,7}[0-9a-fA-F]{1,4}|([0-9a-fA-F]{1,4}:){1,7}:|([0-9a-fA-F]{1,4}:){1,6}:[0-9a-fA-F]{1,4}|([0-9a-fA-F]{1,4}:){1,5}(:[0-9a-fA-F]{1,4}){1,2}|([0-9a-fA-F]{1,4}:){1,4}(:[0-9a-fA-F]{1,4}){1,3}|([0-9a-fA-F]{1,4}:){1,3}(:[0-9a-fA-F]{1,4}){1,4}|([0-9a-fA-F]{1,4}:){1,2}(:[0-9a-fA-F]{1,4}){1,5}|[0-9a-fA-F]{1,4}:((:[0-9a-fA-F]{1,4}){1,6})|:((:[0-9a-fA-F]{1,4}){1,7}|:)|fe80:(:[0-9a-fA-F]{0,4}){0,4}%[0-9a-zA-Z]{1,}|::(ffff(:0{1,4}){0,1}:){0,1}((25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])\\.){3,3}(25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])|([0-9a-fA-F]{1,4}:){1,4}:((25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])\\.){3,3}(25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9]))(\\/[0-9]{1,3})?$"
							/* The inet type holds an IPv4 or IPv6 host address, and optionally its subnet, all in one field. See more in the doc.
							 	The regex might still be not perfect, but catches the most of IP variants. We might decide to remove the regex
								at all though and let the server side to handle it.*/
							if (php_pgsql_convert_match(Z_STRVAL_P(val), Z_STRLEN_P(val), REGEX0, sizeof(REGEX0)-1, 0) == FAILURE
								&& php_pgsql_convert_match(Z_STRVAL_P(val), Z_STRLEN_P(val), REGEX1, sizeof(REGEX1)-1, 0) == FAILURE) {
								err = 1;
							}
							else {
								ZVAL_STRINGL(&new_val, Z_STRVAL_P(val), Z_STRLEN_P(val));
								php_pgsql_add_quotes(&new_val, 1);
							}
#undef REGEX0
#undef REGEX1
						}
						break;

					case IS_NULL:
						ZVAL_STRINGL(&new_val, "NULL", sizeof("NULL")-1);
						break;

					default:
						err = 1;
				}
				PGSQL_CONV_CHECK_IGNORE();
				if (err) {
					php_error_docref(NULL, E_NOTICE, "Expects NULL or IPv4 or IPv6 address string for '%s' (%s)", Z_STRVAL_P(type), ZSTR_VAL(field));
				}
				break;

			case PG_TIME_WITH_TIMEZONE:
			case PG_TIMESTAMP:
			case PG_TIMESTAMP_WITH_TIMEZONE:
				switch(Z_TYPE_P(val)) {
					case IS_STRING:
						if (Z_STRLEN_P(val) == 0) {
							ZVAL_STRINGL(&new_val, "NULL", sizeof("NULL")-1);
						} else if (!strcasecmp(Z_STRVAL_P(val), "now()")) {
							ZVAL_STRINGL(&new_val, "NOW()", sizeof("NOW()")-1);
						} else {
#define REGEX0 "^([0-9]{4}[/-][0-9]{1,2}[/-][0-9]{1,2})(([ \\t]+|T)(([0-9]{1,2}:[0-9]{1,2}){1}(:[0-9]{1,2}){0,1}(\\.[0-9]+){0,1}([ \\t]*([+-][0-9]{1,4}(:[0-9]{1,2}){0,1}|[-a-zA-Z_/+]{1,50})){0,1})){0,1}$"
							/* better regex? */
							if (php_pgsql_convert_match(Z_STRVAL_P(val), Z_STRLEN_P(val), REGEX0, sizeof(REGEX0)-1, 1) == FAILURE) {
								err = 1;
							} else {
								ZVAL_STRING(&new_val, Z_STRVAL_P(val));
								php_pgsql_add_quotes(&new_val, 1);
							}
#undef REGEX0
						}
						break;

					case IS_NULL:
						ZVAL_STRINGL(&new_val, "NULL", sizeof("NULL")-1);
						break;

					default:
						err = 1;
				}
				PGSQL_CONV_CHECK_IGNORE();
				if (err) {
					php_error_docref(NULL, E_NOTICE, "Expects NULL or string for PostgreSQL %s field (%s)", Z_STRVAL_P(type), ZSTR_VAL(field));
				}
				break;

			case PG_DATE:
				switch(Z_TYPE_P(val)) {
					case IS_STRING:
						if (Z_STRLEN_P(val) == 0) {
							ZVAL_STRINGL(&new_val, "NULL", sizeof("NULL")-1);
						}
						else {
#define REGEX0 "^([0-9]{4}[/-][0-9]{1,2}[/-][0-9]{1,2})$"
							/* FIXME: better regex must be used */
							if (php_pgsql_convert_match(Z_STRVAL_P(val), Z_STRLEN_P(val), REGEX0, sizeof(REGEX0)-1, 1) == FAILURE) {
								err = 1;
							}
							else {
								ZVAL_STRINGL(&new_val, Z_STRVAL_P(val), Z_STRLEN_P(val));
								php_pgsql_add_quotes(&new_val, 1);
							}
#undef REGEX0
						}
						break;

					case IS_NULL:
						ZVAL_STRINGL(&new_val, "NULL", sizeof("NULL")-1);
						break;

					default:
						err = 1;
				}
				PGSQL_CONV_CHECK_IGNORE();
				if (err) {
					php_error_docref(NULL, E_NOTICE, "Expects NULL or string for PostgreSQL %s field (%s)", Z_STRVAL_P(type), ZSTR_VAL(field));
				}
				break;

			case PG_TIME:
				switch(Z_TYPE_P(val)) {
					case IS_STRING:
						if (Z_STRLEN_P(val) == 0) {
							ZVAL_STRINGL(&new_val, "NULL", sizeof("NULL")-1);
						}
						else {
#define REGEX0 "^(([0-9]{1,2}:[0-9]{1,2}){1}(:[0-9]{1,2}){0,1}){0,1}$"
							/* FIXME: better regex must be used */
							if (php_pgsql_convert_match(Z_STRVAL_P(val), Z_STRLEN_P(val), REGEX0, sizeof(REGEX0)-1, 1) == FAILURE) {
								err = 1;
							}
							else {
								ZVAL_STRINGL(&new_val, Z_STRVAL_P(val), Z_STRLEN_P(val));
								php_pgsql_add_quotes(&new_val, 1);
							}
#undef REGEX0
						}
						break;

					case IS_NULL:
						ZVAL_STRINGL(&new_val, "NULL", sizeof("NULL")-1);
						break;

					default:
						err = 1;
				}
				PGSQL_CONV_CHECK_IGNORE();
				if (err) {
					php_error_docref(NULL, E_NOTICE, "Expects NULL or string for PostgreSQL %s field (%s)", Z_STRVAL_P(type), ZSTR_VAL(field));
				}
				break;

			case PG_INTERVAL:
				switch(Z_TYPE_P(val)) {
					case IS_STRING:
						if (Z_STRLEN_P(val) == 0) {
							ZVAL_STRING(&new_val, "NULL");
						}
						else {

							/* From the Postgres docs:

							   interval values can be written with the following syntax:
							   [@] quantity unit [quantity unit...] [direction]

							   Where: quantity is a number (possibly signed); unit is second, minute, hour,
							   day, week, month, year, decade, century, millennium, or abbreviations or
							   plurals of these units [note not *all* abbreviations] ; direction can be
							   ago or empty. The at sign (@) is optional noise.

							   ...

							   Quantities of days, hours, minutes, and seconds can be specified without explicit
							   unit markings. For example, '1 12:59:10' is read the same as '1 day 12 hours 59 min 10
							   sec'.
							*/
#define REGEX0 \
	"^(@?[ \\t]+)?(" \
	/* Textual time units and their abbreviations: */ \
	"(([-+]?[ \\t]+)?" \
	"[0-9]+(\\.[0-9]*)?[ \\t]*" \
	"(millenniums|millennia|millennium|mil|mils|" \
	"centuries|century|cent|c|" \
	"decades|decade|dec|decs|" \
	"years|year|y|" \
	"months|month|mon|" \
	"weeks|week|w|" \
	"days|day|d|" \
	"hours|hour|hr|hrs|h|" \
	"minutes|minute|mins|min|m|" \
	"seconds|second|secs|sec|s))+|" \
	/* Textual time units plus (dd)* hh[:mm[:ss]] */ \
	"((([-+]?[ \\t]+)?" \
	"[0-9]+(\\.[0-9]*)?[ \\t]*" \
	"(millenniums|millennia|millennium|mil|mils|" \
	"centuries|century|cent|c|" \
	"decades|decade|dec|decs|" \
	"years|year|y|" \
	"months|month|mon|" \
	"weeks|week|w|" \
	"days|day|d))+" \
	"([-+]?[ \\t]+" \
	"([0-9]+[ \\t]+)+"				 /* dd */ \
	"(([0-9]{1,2}:){0,2}[0-9]{0,2})" /* hh:[mm:[ss]] */ \
	")?))" \
	"([ \\t]+ago)?$"

							if (php_pgsql_convert_match(Z_STRVAL_P(val), Z_STRLEN_P(val), REGEX0, sizeof(REGEX0)-1, 1) == FAILURE) {
								err = 1;
							}
							else {
								ZVAL_STRING(&new_val, Z_STRVAL_P(val));
								php_pgsql_add_quotes(&new_val, 1);
							}
#undef REGEX0
						}
						break;

					case IS_NULL:
						ZVAL_STRING(&new_val, "NULL");
						break;

					default:
						err = 1;
				}
				PGSQL_CONV_CHECK_IGNORE();
				if (err) {
					php_error_docref(NULL, E_NOTICE, "Expects NULL or string for PostgreSQL %s field (%s)", Z_STRVAL_P(type), ZSTR_VAL(field));
				}
				break;
			case PG_BYTEA:
				switch (Z_TYPE_P(val)) {
					case IS_STRING:
						if (Z_STRLEN_P(val) == 0) {
							ZVAL_STRING(&new_val, "NULL");
						}
						else {
							unsigned char *tmp;
							size_t to_len;
							smart_str s = {0};
							tmp = PQescapeByteaConn(pg_link, (unsigned char *)Z_STRVAL_P(val), Z_STRLEN_P(val), &to_len);
							ZVAL_STRINGL(&new_val, (char *)tmp, to_len - 1); /* PQescapeBytea's to_len includes additional '\0' */
							PQfreemem(tmp);
							php_pgsql_add_quotes(&new_val, 1);
							smart_str_appendl(&s, Z_STRVAL(new_val), Z_STRLEN(new_val));
							smart_str_0(&s);
							zval_ptr_dtor(&new_val);
							ZVAL_NEW_STR(&new_val, s.s);
						}
						break;

					case IS_LONG:
						ZVAL_STR(&new_val, zend_long_to_str(Z_LVAL_P(val)));
						break;

					case IS_DOUBLE:
						ZVAL_DOUBLE(&new_val, Z_DVAL_P(val));
						convert_to_string_ex(&new_val);
						break;

					case IS_NULL:
						ZVAL_STRINGL(&new_val, "NULL", sizeof("NULL")-1);
						break;

					default:
						err = 1;
				}
				PGSQL_CONV_CHECK_IGNORE();
				if (err) {
					php_error_docref(NULL, E_NOTICE, "Expects NULL, string, long or double value for PostgreSQL '%s' (%s)", Z_STRVAL_P(type), ZSTR_VAL(field));
				}
				break;

			case PG_MACADDR:
				switch(Z_TYPE_P(val)) {
					case IS_STRING:
						if (Z_STRLEN_P(val) == 0) {
							ZVAL_STRINGL(&new_val, "NULL", sizeof("NULL")-1);
						}
						else {
#define REGEX0 "^([0-9a-f]{2,2}:){5,5}[0-9a-f]{2,2}$"
							if (php_pgsql_convert_match(Z_STRVAL_P(val), Z_STRLEN_P(val), REGEX0, sizeof(REGEX0)-1, 1) == FAILURE) {
								err = 1;
							}
							else {
								ZVAL_STRINGL(&new_val, Z_STRVAL_P(val), Z_STRLEN_P(val));
								php_pgsql_add_quotes(&new_val, 1);
							}
#undef REGEX0
						}
						break;

					case IS_NULL:
						ZVAL_STRINGL(&new_val, "NULL", sizeof("NULL")-1);
						break;

					default:
						err = 1;
				}
				PGSQL_CONV_CHECK_IGNORE();
				if (err) {
					php_error_docref(NULL, E_NOTICE, "Expects NULL or string for PostgreSQL %s field (%s)", Z_STRVAL_P(type), ZSTR_VAL(field));
				}
				break;

			default:
				/* should not happen */
				php_error_docref(NULL, E_NOTICE, "Unknown or system data type '%s' for '%s'. Report error", Z_STRVAL_P(type), ZSTR_VAL(field));
				err = 1;
				break;
		} /* switch */

		if (err) {
			zval_ptr_dtor(&new_val);
			break; /* break out for() */
		}
		/* If field is NULL and HAS DEFAULT, should be skipped */
		if (!skip_field) {
			if (_php_pgsql_detect_identifier_escape(ZSTR_VAL(field), ZSTR_LEN(field)) == SUCCESS) {
				zend_hash_update(Z_ARRVAL_P(result), field, &new_val);
			} else {
				char *escaped = PQescapeIdentifier(pg_link, ZSTR_VAL(field), ZSTR_LEN(field));
				add_assoc_zval(result, escaped, &new_val);
				PQfreemem(escaped);
			}
		}
	} ZEND_HASH_FOREACH_END(); /* for */

	zval_ptr_dtor(&meta);

	if (err) {
		/* shouldn't destroy & free zval here */
		return FAILURE;
	}
	return SUCCESS;
}
/* }}} */

/* {{{ Check and convert values for PostgreSQL SQL statement */
PHP_FUNCTION(pg_convert)
{
	zval *pgsql_link, *values;
	char *table_name;
	size_t table_name_len;
	zend_ulong option = 0;
	PGconn *pg_link;

	if (zend_parse_parameters(ZEND_NUM_ARGS(),
							  "rsa|l", &pgsql_link, &table_name, &table_name_len, &values, &option) == FAILURE) {
		RETURN_THROWS();
	}

	if (table_name_len == 0) {
		zend_argument_value_error(2, "cannot be empty");
		RETURN_THROWS();
	}

	if (option & ~PGSQL_CONV_OPTS) {
		zend_argument_value_error(4, "must be a valid bit mask of PGSQL_CONV_IGNORE_DEFAULT, "
			"PGSQL_CONV_FORCE_NULL, and PGSQL_CONV_IGNORE_NOT_NULL");
		RETURN_THROWS();
	}

	if ((pg_link = (PGconn *)zend_fetch_resource2(Z_RES_P(pgsql_link), "PostgreSQL link", le_link, le_plink)) == NULL) {
		RETURN_THROWS();
	}

	if (php_pgsql_flush_query(pg_link)) {
		php_error_docref(NULL, E_NOTICE, "Detected unhandled result(s) in connection");
	}
	array_init(return_value);
	if (php_pgsql_convert(pg_link, table_name, values, return_value, option) == FAILURE) {
		zend_array_destroy(Z_ARR_P(return_value));
		RETURN_FALSE;
	}
}
/* }}} */

static int do_exec(smart_str *querystr, ExecStatusType expect, PGconn *pg_link, zend_ulong opt) /* {{{ */
{
	if (opt & PGSQL_DML_ASYNC) {
		if (PQsendQuery(pg_link, ZSTR_VAL(querystr->s))) {
			return 0;
		}
	}
	else {
		PGresult *pg_result;

		pg_result = PQexec(pg_link, ZSTR_VAL(querystr->s));
		if (PQresultStatus(pg_result) == expect) {
			PQclear(pg_result);
			return 0;
		} else {
			php_error_docref(NULL, E_WARNING, "%s", PQresultErrorMessage(pg_result));
			PQclear(pg_result);
		}
	}

	return -1;
}
/* }}} */

static inline void build_tablename(smart_str *querystr, PGconn *pg_link, const char *table) /* {{{ */
{
	size_t table_len = strlen(table);

	/* schema.table should be "schema"."table" */
	const char *dot = memchr(table, '.', table_len);
	size_t len = dot ? dot - table : table_len;
	if (_php_pgsql_detect_identifier_escape(table, len) == SUCCESS) {
		smart_str_appendl(querystr, table, len);
	} else {
		char *escaped = PQescapeIdentifier(pg_link, table, len);
		smart_str_appends(querystr, escaped);
		PQfreemem(escaped);
	}
	if (dot) {
		const char *after_dot = dot + 1;
		len = table_len - len - 1;
		/* "schema"."table" format */
		if (_php_pgsql_detect_identifier_escape(after_dot, len) == SUCCESS) {
			smart_str_appendc(querystr, '.');
			smart_str_appendl(querystr, after_dot, len);
		} else {
			char *escaped = PQescapeIdentifier(pg_link, after_dot, len);
			smart_str_appendc(querystr, '.');
			smart_str_appends(querystr, escaped);
			PQfreemem(escaped);
		}
	}
}
/* }}} */

/* {{{ php_pgsql_insert */
PHP_PGSQL_API int php_pgsql_insert(PGconn *pg_link, const char *table, zval *var_array, zend_ulong opt, zend_string **sql)
{
	zval *val, converted;
	char buf[256];
	char *tmp;
	smart_str querystr = {0};
	int ret = FAILURE;
	zend_string *fld;

	assert(pg_link != NULL);
	assert(table != NULL);
	assert(Z_TYPE_P(var_array) == IS_ARRAY);

	ZVAL_UNDEF(&converted);
	if (zend_hash_num_elements(Z_ARRVAL_P(var_array)) == 0) {
		smart_str_appends(&querystr, "INSERT INTO ");
		build_tablename(&querystr, pg_link, table);
		smart_str_appends(&querystr, " DEFAULT VALUES");

		goto no_values;
	}

	/* convert input array if needed */
	if (!(opt & (PGSQL_DML_NO_CONV|PGSQL_DML_ESCAPE))) {
		array_init(&converted);
		if (php_pgsql_convert(pg_link, table, var_array, &converted, (opt & PGSQL_CONV_OPTS)) == FAILURE) {
			goto cleanup;
		}
		var_array = &converted;
	}

	smart_str_appends(&querystr, "INSERT INTO ");
	build_tablename(&querystr, pg_link, table);
	smart_str_appends(&querystr, " (");

	ZEND_HASH_FOREACH_STR_KEY(Z_ARRVAL_P(var_array), fld) {
		if (fld == NULL) {
			zend_value_error("Array of values must be an associative array with string keys");
			goto cleanup;
		}
		if (opt & PGSQL_DML_ESCAPE) {
			tmp = PQescapeIdentifier(pg_link, ZSTR_VAL(fld), ZSTR_LEN(fld) + 1);
			smart_str_appends(&querystr, tmp);
			PQfreemem(tmp);
		} else {
			smart_str_appendl(&querystr, ZSTR_VAL(fld), ZSTR_LEN(fld));
		}
		smart_str_appendc(&querystr, ',');
	} ZEND_HASH_FOREACH_END();
	ZSTR_LEN(querystr.s)--;
	smart_str_appends(&querystr, ") VALUES (");

	/* make values string */
	ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(var_array), val) {
		/* we can avoid the key_type check here, because we tested it in the other loop */
		switch (Z_TYPE_P(val)) {
			case IS_STRING:
				if (opt & PGSQL_DML_ESCAPE) {
					size_t new_len;
					char *tmp;
					tmp = (char *)safe_emalloc(Z_STRLEN_P(val), 2, 1);
					new_len = PQescapeStringConn(pg_link, tmp, Z_STRVAL_P(val), Z_STRLEN_P(val), NULL);
					smart_str_appendc(&querystr, '\'');
					smart_str_appendl(&querystr, tmp, new_len);
					smart_str_appendc(&querystr, '\'');
					efree(tmp);
				} else {
					smart_str_appendl(&querystr, Z_STRVAL_P(val), Z_STRLEN_P(val));
				}
				break;
			case IS_LONG:
				smart_str_append_long(&querystr, Z_LVAL_P(val));
				break;
			case IS_DOUBLE:
				smart_str_appendl(&querystr, buf, snprintf(buf, sizeof(buf), "%F", Z_DVAL_P(val)));
				break;
			case IS_NULL:
				smart_str_appendl(&querystr, "NULL", sizeof("NULL")-1);
				break;
			default:
				zend_type_error("Value must be of type string|int|float|null, %s given", zend_zval_type_name(val));
				goto cleanup;
		}
		smart_str_appendc(&querystr, ',');
	} ZEND_HASH_FOREACH_END();
	/* Remove the trailing "," */
	ZSTR_LEN(querystr.s)--;
	smart_str_appends(&querystr, ");");

no_values:

	smart_str_0(&querystr);

	if ((opt & (PGSQL_DML_EXEC|PGSQL_DML_ASYNC)) &&
		do_exec(&querystr, PGRES_COMMAND_OK, pg_link, (opt & PGSQL_CONV_OPTS)) == 0) {
		ret = SUCCESS;
	}
	else if (opt & PGSQL_DML_STRING) {
		ret = SUCCESS;
	}

cleanup:
	zval_ptr_dtor(&converted);
	if (ret == SUCCESS && (opt & PGSQL_DML_STRING)) {
		*sql = querystr.s;
	}
	else {
		smart_str_free(&querystr);
	}
	return ret;
}
/* }}} */

/* {{{ Insert values (filed=>value) to table */
PHP_FUNCTION(pg_insert)
{
	zval *pgsql_link, *values;
	char *table;
	size_t table_len;
	zend_ulong option = PGSQL_DML_EXEC, return_sql;
	PGconn *pg_link;
	PGresult *pg_result;
	ExecStatusType status;
	zend_string *sql = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rsa|l",
							  &pgsql_link, &table, &table_len, &values, &option) == FAILURE) {
		RETURN_THROWS();
	}

	if (table_len == 0) {
		zend_argument_value_error(2, "cannot be empty");
		RETURN_THROWS();
	}

	if (option & ~(PGSQL_CONV_OPTS|PGSQL_DML_NO_CONV|PGSQL_DML_EXEC|PGSQL_DML_ASYNC|PGSQL_DML_STRING|PGSQL_DML_ESCAPE)) {
		zend_argument_value_error(4, "must be a valid bit mask of PGSQL_CONV_FORCE_NULL, PGSQL_DML_NO_CONV, "
			"PGSQL_DML_ESCAPE, PGSQL_DML_EXEC, PGSQL_DML_ASYNC, and PGSQL_DML_STRING");
		RETURN_THROWS();
	}

	if ((pg_link = (PGconn *)zend_fetch_resource2(Z_RES_P(pgsql_link), "PostgreSQL link", le_link, le_plink)) == NULL) {
		RETURN_THROWS();
	}

	if (php_pgsql_flush_query(pg_link)) {
		php_error_docref(NULL, E_NOTICE, "Detected unhandled result(s) in connection");
	}
	return_sql = option & PGSQL_DML_STRING;
	if (option & PGSQL_DML_EXEC) {
		/* return resource when executed */
		option = option & ~PGSQL_DML_EXEC;
		if (php_pgsql_insert(pg_link, table, values, option|PGSQL_DML_STRING, &sql) == FAILURE) {
			RETURN_FALSE;
		}
		pg_result = PQexec(pg_link, ZSTR_VAL(sql));
		if ((PGG(auto_reset_persistent) & 2) && PQstatus(pg_link) != CONNECTION_OK) {
			PQclear(pg_result);
			PQreset(pg_link);
			pg_result = PQexec(pg_link, ZSTR_VAL(sql));
		}
		efree(sql);

		if (pg_result) {
			status = PQresultStatus(pg_result);
		} else {
			status = (ExecStatusType) PQstatus(pg_link);
		}

		switch (status) {
			case PGRES_EMPTY_QUERY:
			case PGRES_BAD_RESPONSE:
			case PGRES_NONFATAL_ERROR:
			case PGRES_FATAL_ERROR:
				PHP_PQ_ERROR("Query failed: %s", pg_link);
				PQclear(pg_result);
				RETURN_FALSE;
				break;
			case PGRES_COMMAND_OK: /* successful command that did not return rows */
			default:
				if (pg_result) {
					pgsql_result_handle *pgsql_handle = (pgsql_result_handle *) emalloc(sizeof(pgsql_result_handle));
					pgsql_handle->conn = pg_link;
					pgsql_handle->result = pg_result;
					pgsql_handle->row = 0;
					RETURN_RES(zend_register_resource(pgsql_handle, le_result));
				} else {
					PQclear(pg_result);
					RETURN_FALSE;
				}
			break;
		}
	} else if (php_pgsql_insert(pg_link, table, values, option, &sql) == FAILURE) {
		RETURN_FALSE;
	}
	if (return_sql) {
		RETURN_STR(sql);
		return;
	}
	RETURN_TRUE;
}
/* }}} */

static inline int build_assignment_string(PGconn *pg_link, smart_str *querystr, HashTable *ht, int where_cond, const char *pad, int pad_len, zend_ulong opt) /* {{{ */
{
	zend_string *fld;
	zval *val;

	ZEND_HASH_FOREACH_STR_KEY_VAL(ht, fld, val) {
		if (fld == NULL) {
			zend_value_error("Array of values must be an associative array with string keys");
			return -1;
		}
		if (opt & PGSQL_DML_ESCAPE) {
			char *tmp = PQescapeIdentifier(pg_link, ZSTR_VAL(fld), ZSTR_LEN(fld) + 1);
			smart_str_appends(querystr, tmp);
			PQfreemem(tmp);
		} else {
			smart_str_appendl(querystr, ZSTR_VAL(fld), ZSTR_LEN(fld));
		}
		if (where_cond && (Z_TYPE_P(val) == IS_TRUE || Z_TYPE_P(val) == IS_FALSE || (Z_TYPE_P(val) == IS_STRING && !strcmp(Z_STRVAL_P(val), "NULL")))) {
			smart_str_appends(querystr, " IS ");
		} else {
			smart_str_appendc(querystr, '=');
		}

		switch (Z_TYPE_P(val)) {
			case IS_STRING:
				if (opt & PGSQL_DML_ESCAPE) {
					char *tmp = (char *)safe_emalloc(Z_STRLEN_P(val), 2, 1);
					size_t new_len = PQescapeStringConn(pg_link, tmp, Z_STRVAL_P(val), Z_STRLEN_P(val), NULL);
					smart_str_appendc(querystr, '\'');
					smart_str_appendl(querystr, tmp, new_len);
					smart_str_appendc(querystr, '\'');
					efree(tmp);
				} else {
					smart_str_appendl(querystr, Z_STRVAL_P(val), Z_STRLEN_P(val));
				}
				break;
			case IS_LONG:
				smart_str_append_long(querystr, Z_LVAL_P(val));
				break;
			case IS_DOUBLE: {
				char buf[256];
				smart_str_appendl(querystr, buf, MIN(snprintf(buf, sizeof(buf), "%F", Z_DVAL_P(val)), sizeof(buf) - 1));
				}
				break;
			case IS_NULL:
				smart_str_appendl(querystr, "NULL", sizeof("NULL")-1);
				break;
			default:
				zend_type_error("Value must be of type string|int|float|null, %s given", zend_zval_type_name(val));
				return -1;
		}
		smart_str_appendl(querystr, pad, pad_len);
	} ZEND_HASH_FOREACH_END();
	if (querystr->s) {
		ZSTR_LEN(querystr->s) -= pad_len;
	}

	return 0;
}
/* }}} */

/* {{{ php_pgsql_update */
PHP_PGSQL_API int php_pgsql_update(PGconn *pg_link, const char *table, zval *var_array, zval *ids_array, zend_ulong opt, zend_string **sql)
{
	zval var_converted, ids_converted;
	smart_str querystr = {0};
	int ret = FAILURE;

	assert(pg_link != NULL);
	assert(table != NULL);
	assert(Z_TYPE_P(var_array) == IS_ARRAY);
	assert(Z_TYPE_P(ids_array) == IS_ARRAY);
	assert(!(opt & ~(PGSQL_CONV_OPTS|PGSQL_DML_NO_CONV|PGSQL_DML_EXEC|PGSQL_DML_STRING|PGSQL_DML_ESCAPE)));

	if (zend_hash_num_elements(Z_ARRVAL_P(var_array)) == 0
			|| zend_hash_num_elements(Z_ARRVAL_P(ids_array)) == 0) {
		return FAILURE;
	}

	ZVAL_UNDEF(&var_converted);
	ZVAL_UNDEF(&ids_converted);
	if (!(opt & (PGSQL_DML_NO_CONV|PGSQL_DML_ESCAPE))) {
		array_init(&var_converted);
		if (php_pgsql_convert(pg_link, table, var_array, &var_converted, (opt & PGSQL_CONV_OPTS)) == FAILURE) {
			goto cleanup;
		}
		var_array = &var_converted;
		array_init(&ids_converted);
		if (php_pgsql_convert(pg_link, table, ids_array, &ids_converted, (opt & PGSQL_CONV_OPTS)) == FAILURE) {
			goto cleanup;
		}
		ids_array = &ids_converted;
	}

	smart_str_appends(&querystr, "UPDATE ");
	build_tablename(&querystr, pg_link, table);
	smart_str_appends(&querystr, " SET ");

	if (build_assignment_string(pg_link, &querystr, Z_ARRVAL_P(var_array), 0, ",", 1, opt))
		goto cleanup;

	smart_str_appends(&querystr, " WHERE ");

	if (build_assignment_string(pg_link, &querystr, Z_ARRVAL_P(ids_array), 1, " AND ", sizeof(" AND ")-1, opt))
		goto cleanup;

	smart_str_appendc(&querystr, ';');
	smart_str_0(&querystr);

	if ((opt & PGSQL_DML_EXEC) && do_exec(&querystr, PGRES_COMMAND_OK, pg_link, opt) == 0) {
		ret = SUCCESS;
	} else if (opt & PGSQL_DML_STRING) {
		ret = SUCCESS;
	}

cleanup:
	zval_ptr_dtor(&var_converted);
	zval_ptr_dtor(&ids_converted);
	if (ret == SUCCESS && (opt & PGSQL_DML_STRING)) {
		*sql = querystr.s;
	}
	else {
		smart_str_free(&querystr);
	}
	return ret;
}
/* }}} */

/* {{{ Update table using values (field=>value) and ids (id=>value) */
PHP_FUNCTION(pg_update)
{
	zval *pgsql_link, *values, *ids;
	char *table;
	size_t table_len;
	zend_ulong option =  PGSQL_DML_EXEC;
	PGconn *pg_link;
	zend_string *sql = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rsaa|l",
							  &pgsql_link, &table, &table_len, &values, &ids, &option) == FAILURE) {
		RETURN_THROWS();
	}

	if (table_len == 0) {
		zend_argument_value_error(2, "cannot be empty");
		RETURN_THROWS();
	}

	if (option & ~(PGSQL_CONV_OPTS|PGSQL_DML_NO_CONV|PGSQL_DML_EXEC|PGSQL_DML_STRING|PGSQL_DML_ESCAPE)) {
		zend_argument_value_error(5, "must be a valid bit mask of PGSQL_CONV_FORCE_NULL, PGSQL_DML_NO_CONV, "
			"PGSQL_DML_ESCAPE, PGSQL_DML_EXEC, PGSQL_DML_ASYNC, and PGSQL_DML_STRING");
		RETURN_THROWS();
	}

	if ((pg_link = (PGconn *)zend_fetch_resource2(Z_RES_P(pgsql_link), "PostgreSQL link", le_link, le_plink)) == NULL) {
		RETURN_THROWS();
	}

	if (php_pgsql_flush_query(pg_link)) {
		php_error_docref(NULL, E_NOTICE, "Detected unhandled result(s) in connection");
	}
	if (php_pgsql_update(pg_link, table, values, ids, option, &sql) == FAILURE) {
		RETURN_FALSE;
	}
	if (option & PGSQL_DML_STRING) {
		RETURN_STR(sql);
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ php_pgsql_delete */
PHP_PGSQL_API int php_pgsql_delete(PGconn *pg_link, const char *table, zval *ids_array, zend_ulong opt, zend_string **sql)
{
	zval ids_converted;
	smart_str querystr = {0};
	int ret = FAILURE;

	assert(pg_link != NULL);
	assert(table != NULL);
	assert(Z_TYPE_P(ids_array) == IS_ARRAY);
	assert(!(opt & ~(PGSQL_CONV_FORCE_NULL|PGSQL_DML_EXEC|PGSQL_DML_STRING|PGSQL_DML_ESCAPE)));

	if (zend_hash_num_elements(Z_ARRVAL_P(ids_array)) == 0) {
		return FAILURE;
	}

	ZVAL_UNDEF(&ids_converted);
	if (!(opt & (PGSQL_DML_NO_CONV|PGSQL_DML_ESCAPE))) {
		array_init(&ids_converted);
		if (php_pgsql_convert(pg_link, table, ids_array, &ids_converted, (opt & PGSQL_CONV_OPTS)) == FAILURE) {
			goto cleanup;
		}
		ids_array = &ids_converted;
	}

	smart_str_appends(&querystr, "DELETE FROM ");
	build_tablename(&querystr, pg_link, table);
	smart_str_appends(&querystr, " WHERE ");

	if (build_assignment_string(pg_link, &querystr, Z_ARRVAL_P(ids_array), 1, " AND ", sizeof(" AND ")-1, opt))
		goto cleanup;

	smart_str_appendc(&querystr, ';');
	smart_str_0(&querystr);

	if ((opt & PGSQL_DML_EXEC) && do_exec(&querystr, PGRES_COMMAND_OK, pg_link, opt) == 0) {
		ret = SUCCESS;
	} else if (opt & PGSQL_DML_STRING) {
		ret = SUCCESS;
	}

cleanup:
	zval_ptr_dtor(&ids_converted);
	if (ret == SUCCESS && (opt & PGSQL_DML_STRING)) {
		*sql = querystr.s;
	}
	else {
		smart_str_free(&querystr);
	}
	return ret;
}
/* }}} */

/* {{{ Delete records has ids (id=>value) */
PHP_FUNCTION(pg_delete)
{
	zval *pgsql_link, *ids;
	char *table;
	size_t table_len;
	zend_ulong option = PGSQL_DML_EXEC;
	PGconn *pg_link;
	zend_string *sql;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rsa|l",
							  &pgsql_link, &table, &table_len, &ids, &option) == FAILURE) {
		RETURN_THROWS();
	}

	if (table_len == 0) {
		zend_argument_value_error(2, "cannot be empty");
		RETURN_THROWS();
	}

	if (option & ~(PGSQL_CONV_FORCE_NULL|PGSQL_DML_NO_CONV|PGSQL_DML_EXEC|PGSQL_DML_STRING|PGSQL_DML_ESCAPE)) {
		zend_argument_value_error(4, "must be a valid bit mask of PGSQL_CONV_FORCE_NULL, PGSQL_DML_NO_CONV, "
			"PGSQL_DML_ESCAPE, PGSQL_DML_EXEC, PGSQL_DML_ASYNC, and PGSQL_DML_STRING");
		RETURN_THROWS();
	}

	if ((pg_link = (PGconn *)zend_fetch_resource2(Z_RES_P(pgsql_link), "PostgreSQL link", le_link, le_plink)) == NULL) {
		RETURN_THROWS();
	}

	if (php_pgsql_flush_query(pg_link)) {
		php_error_docref(NULL, E_NOTICE, "Detected unhandled result(s) in connection");
	}
	if (php_pgsql_delete(pg_link, table, ids, option, &sql) == FAILURE) {
		RETURN_FALSE;
	}
	if (option & PGSQL_DML_STRING) {
		RETURN_STR(sql);
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ php_pgsql_result2array */
PHP_PGSQL_API void php_pgsql_result2array(PGresult *pg_result, zval *ret_array, long result_type)
{
	zval row;
	char *field_name;
	size_t num_fields;
	int pg_numrows, pg_row;
	uint32_t i;
	assert(Z_TYPE_P(ret_array) == IS_ARRAY);

	pg_numrows = PQntuples(pg_result);
	for (pg_row = 0; pg_row < pg_numrows; pg_row++) {
		array_init(&row);
		for (i = 0, num_fields = PQnfields(pg_result); i < num_fields; i++) {
			field_name = PQfname(pg_result, i);
			if (PQgetisnull(pg_result, pg_row, i)) {
				if (result_type & PGSQL_ASSOC) {
					add_assoc_null(&row, field_name);
				}
				if (result_type & PGSQL_NUM) {
					add_next_index_null(&row);
				}
			} else {
				char *element = PQgetvalue(pg_result, pg_row, i);
				if (element) {
					const size_t element_len = strlen(element);
					if (result_type & PGSQL_ASSOC) {
						add_assoc_stringl(&row, field_name, element, element_len);
					}
					if (result_type & PGSQL_NUM) {
						add_next_index_stringl(&row, element, element_len);
					}
				}
			}
		}
		add_index_zval(ret_array, pg_row, &row);
	}
}
/* }}} */

/* {{{ php_pgsql_select */
 PHP_PGSQL_API int php_pgsql_select(PGconn *pg_link, const char *table, zval *ids_array, zval *ret_array, zend_ulong opt, long result_type, zend_string **sql)
{
	zval ids_converted;
	smart_str querystr = {0};
	int ret = FAILURE;
	PGresult *pg_result;

	assert(pg_link != NULL);
	assert(table != NULL);
	assert(Z_TYPE_P(ids_array) == IS_ARRAY);
	assert(Z_TYPE_P(ret_array) == IS_ARRAY);
	assert(!(opt & ~(PGSQL_CONV_OPTS|PGSQL_DML_NO_CONV|PGSQL_DML_EXEC|PGSQL_DML_ASYNC|PGSQL_DML_STRING|PGSQL_DML_ESCAPE)));

	if (zend_hash_num_elements(Z_ARRVAL_P(ids_array)) == 0) {
		return FAILURE;
	}

	ZVAL_UNDEF(&ids_converted);
	if (!(opt & (PGSQL_DML_NO_CONV|PGSQL_DML_ESCAPE))) {
		array_init(&ids_converted);
		if (php_pgsql_convert(pg_link, table, ids_array, &ids_converted, (opt & PGSQL_CONV_OPTS)) == FAILURE) {
			goto cleanup;
		}
		ids_array = &ids_converted;
	}

	smart_str_appends(&querystr, "SELECT * FROM ");
	build_tablename(&querystr, pg_link, table);
	smart_str_appends(&querystr, " WHERE ");

	if (build_assignment_string(pg_link, &querystr, Z_ARRVAL_P(ids_array), 1, " AND ", sizeof(" AND ")-1, opt))
		goto cleanup;

	smart_str_appendc(&querystr, ';');
	smart_str_0(&querystr);

	pg_result = PQexec(pg_link, ZSTR_VAL(querystr.s));
	if (PQresultStatus(pg_result) == PGRES_TUPLES_OK) {
		php_pgsql_result2array(pg_result, ret_array, result_type);
		ret = SUCCESS;
	} else {
		php_error_docref(NULL, E_NOTICE, "Failed to execute '%s'", ZSTR_VAL(querystr.s));
	}
	PQclear(pg_result);

cleanup:
	zval_ptr_dtor(&ids_converted);
	if (ret == SUCCESS && (opt & PGSQL_DML_STRING)) {
		*sql = querystr.s;
	}
	else {
		smart_str_free(&querystr);
	}
	return ret;
}
/* }}} */

/* {{{ Select records that has ids (id=>value) */
PHP_FUNCTION(pg_select)
{
	zval *pgsql_link, *ids;
	char *table;
	size_t table_len;
	zend_ulong option = PGSQL_DML_EXEC;
	long result_type = PGSQL_ASSOC;
	PGconn *pg_link;
	zend_string *sql = NULL;

	/* TODO Document result_type param on php.net (apparently it was added in PHP 7.1) */
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rsa|ll",
							  &pgsql_link, &table, &table_len, &ids, &option, &result_type) == FAILURE) {
		RETURN_THROWS();
	}

	if (table_len == 0) {
		zend_argument_value_error(2, "cannot be empty");
		RETURN_THROWS();
	}

	if (option & ~(PGSQL_CONV_FORCE_NULL|PGSQL_DML_NO_CONV|PGSQL_DML_EXEC|PGSQL_DML_ASYNC|PGSQL_DML_STRING|PGSQL_DML_ESCAPE)) {
		zend_argument_value_error(4, "must be a valid bit mask of PGSQL_CONV_FORCE_NULL, PGSQL_DML_NO_CONV, "
			"PGSQL_DML_ESCAPE, PGSQL_DML_EXEC, PGSQL_DML_ASYNC, and PGSQL_DML_STRING");
		RETURN_THROWS();
	}
	if (!(result_type & PGSQL_BOTH)) {
		zend_argument_value_error(5, "must be one of PGSQL_ASSOC, PGSQL_NUM, or PGSQL_BOTH");
		RETURN_THROWS();
	}

	if ((pg_link = (PGconn *)zend_fetch_resource2(Z_RES_P(pgsql_link), "PostgreSQL link", le_link, le_plink)) == NULL) {
		RETURN_THROWS();
	}

	if (php_pgsql_flush_query(pg_link)) {
		php_error_docref(NULL, E_NOTICE, "Detected unhandled result(s) in connection");
	}
	array_init(return_value);
	if (php_pgsql_select(pg_link, table, ids, return_value, option, result_type, &sql) == FAILURE) {
		zval_ptr_dtor(return_value);
		RETURN_FALSE;
	}
	if (option & PGSQL_DML_STRING) {
		zval_ptr_dtor(return_value);
		RETURN_STR(sql);
	}
	return;
}
/* }}} */

#endif
