/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_0.txt.                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Jouni Ahto <jouni.ahto@exdec.fi>                            |
   |          Andrew Avdeev <andy@rsc.mv.ru>                              |
   |          Ard Biesheuvel <a.k.biesheuvel@its.tudelft.nl>              |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#if HAVE_IBASE

#include "php_ini.h"
#include "ext/standard/php_standard.h"
#include "php_interbase.h"

#include <time.h>

#ifndef SQLDA_CURRENT_VERSION
#define SQLDA_CURRENT_VERSION SQLDA_VERSION1
#endif

#ifndef SQL_DIALECT_CURRENT
#define SQL_DIALECT_CURRENT 1 /* == SQL_DIALECT_V5 */
#endif

#ifdef ZEND_DEBUG_
#define IBDEBUG(a) php_printf("::: %s (%d)\n", a, __LINE__);
#endif

#ifndef IBDEBUG
#define IBDEBUG(a)
#endif

#define SAFE_STRING(s) ((s)?(s):"")

#define ISC_LONG_MIN (-ISC_LONG_MAX - 1)
#define ISC_LONG_MAX 2147483647

#ifdef PHP_WIN32
# ifndef ISC_INT64
#  define ISC_INT64 __int64
# endif
# define LL_MASK "I64"
# define LL_LIT(lit) lit ## I64
#else
# ifndef ISC_INT64
#  define ISC_INT64 long long int
# endif
# define LL_MASK "ll"
# define LL_LIT(lit) lit ## ll
#endif

#define QUERY_RESULT	1
#define EXECUTE_RESULT	2

#define ROLLBACK		0
#define COMMIT			1
#define RETAIN			2

#define FETCH_ROW		1
#define FETCH_ARRAY		2

#define BLOB_ID_LEN		18
#define BLOB_ID_MASK	"0x%" LL_MASK "x"

#define BLOB_INPUT		1
#define BLOB_OUTPUT		2

#define BLOB_CLOSE		1
#define BLOB_CANCEL		2

/* {{{ extension definition structures */
function_entry ibase_functions[] = {
	PHP_FE(ibase_connect, NULL)
	PHP_FE(ibase_pconnect, NULL)
	PHP_FE(ibase_close, NULL)
	PHP_FE(ibase_drop_db, NULL)
	PHP_FE(ibase_query, NULL)
	PHP_FE(ibase_fetch_row, NULL)
	PHP_FE(ibase_fetch_assoc, NULL)
	PHP_FE(ibase_fetch_object, NULL)
	PHP_FE(ibase_free_result, NULL)
	PHP_FE(ibase_name_result, NULL)
	PHP_FE(ibase_prepare, NULL)
	PHP_FE(ibase_execute, NULL)
	PHP_FE(ibase_free_query, NULL)
#if HAVE_STRFTIME
	PHP_FE(ibase_timefmt, NULL)
#endif

	PHP_FE(ibase_num_fields, NULL)
	PHP_FE(ibase_num_params, NULL)
#if abies_0
	PHP_FE(ibase_num_rows, NULL)
#endif
	PHP_FE(ibase_affected_rows, NULL)
	PHP_FE(ibase_field_info, NULL)
	PHP_FE(ibase_param_info, NULL)

	PHP_FE(ibase_trans, NULL)
	PHP_FE(ibase_commit, NULL)
	PHP_FE(ibase_rollback, NULL)
	PHP_FE(ibase_commit_ret, NULL)

	PHP_FE(ibase_blob_info, NULL)
	PHP_FE(ibase_blob_create, NULL)
	PHP_FE(ibase_blob_add, NULL)
	PHP_FE(ibase_blob_cancel, NULL)
	PHP_FE(ibase_blob_close, NULL)
	PHP_FE(ibase_blob_open, NULL)
	PHP_FE(ibase_blob_get, NULL)
	PHP_FE(ibase_blob_echo, NULL)
	PHP_FE(ibase_blob_import, NULL)
	PHP_FE(ibase_errmsg, NULL)
	PHP_FE(ibase_errcode, NULL)

#ifdef SQL_DIALECT_V6
	PHP_FE(ibase_add_user, NULL)
	PHP_FE(ibase_modify_user, NULL)
	PHP_FE(ibase_delete_user, NULL)

	PHP_FE(ibase_rollback_ret, NULL)
#endif
	PHP_FE(ibase_wait_event, NULL)
	PHP_FE(ibase_set_event_handler, NULL)
	PHP_FE(ibase_free_event_handler, NULL)
	{NULL, NULL, NULL}
};

zend_module_entry ibase_module_entry = {
	STANDARD_MODULE_HEADER,
	"interbase",
	ibase_functions,
	PHP_MINIT(ibase),
	PHP_MSHUTDOWN(ibase),
	PHP_RINIT(ibase),
	PHP_RSHUTDOWN(ibase),
	PHP_MINFO(ibase),
	NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_INTERBASE
ZEND_GET_MODULE(ibase)
#define DL_MALLOC(size) malloc(size)
#define DL_STRDUP(str) strdup(str)
#define DL_FREE(ptr) free(ptr)
#else
#define DL_MALLOC(size) emalloc(size)
#define DL_STRDUP(str) estrdup(str)
#define DL_FREE(ptr) efree(ptr)
#endif

/* True globals, no need for thread safety */
static int le_blob, le_link, le_plink, le_result, le_query, le_trans, le_event;

ZEND_DECLARE_MODULE_GLOBALS(ibase)

/* }}} */

/* error handling ---------------------------- */

#define IB_STATUS (IBG(status))

/* {{{ proto string ibase_errmsg(void) 
   Return error message */
PHP_FUNCTION(ibase_errmsg)
{
	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	if (IBG(sql_code) != 0) {
		RETURN_STRING(IBG(errmsg), 1);
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto int ibase_errcode(void) 
   Return error code */
PHP_FUNCTION(ibase_errcode)
{
	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	if (IBG(sql_code) != 0) {
		RETURN_LONG(IBG(sql_code));
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ _php_ibase_error(TSRMLS_D) 
   print interbase error and save it for ibase_errmsg() */
static void _php_ibase_error(TSRMLS_D)
{
	char *s = IBG(errmsg);
	ISC_STATUS *statusp = IB_STATUS;

	IBG(sql_code) = isc_sqlcode(IB_STATUS);
	
	while ((s - IBG(errmsg)) < MAX_ERRMSG - (IBASE_MSGSIZE + 2) && isc_interprete(s, &statusp)) {
		strcat(IBG(errmsg), " ");
		s = IBG(errmsg) + strlen(IBG(errmsg));
	}

	php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", IBG(errmsg));
}
/* }}} */

/* {{{ _php_ibase_module_error()
   print php interbase module error and save it for ibase_errmsg() */
static void _php_ibase_module_error(char * TSRMLS_DC, ...) PHP_ATTRIBUTE_FORMAT(printf,1,PHP_ATTR_FMT_OFFSET +2);

static void _php_ibase_module_error(char *msg TSRMLS_DC, ...)
{
	va_list ap;

#ifdef ZTS
	va_start(ap, TSRMLS_C);
#else
	va_start(ap, msg);
#endif

	/* vsnprintf NUL terminates the buf and writes at most n-1 chars+NUL */
	vsnprintf(IBG(errmsg), MAX_ERRMSG, msg, ap);
	va_end(ap);

	IBG(sql_code) = -999; /* no SQL error */

	php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", IBG(errmsg));
}
/* }}} */

/* {{{ internal macros, functions and structures */

typedef struct {
    isc_db_handle *db_ptr;
    long tpb_len;
    char *tpb_ptr;
} ISC_TEB;

typedef struct {
	unsigned short vary_length;
	char vary_string[1];
} IBVARY;

/* Fill ib_link and trans with the correct database link and transaction.
 */
static void _php_ibase_get_link_trans(INTERNAL_FUNCTION_PARAMETERS, zval **link_id, ibase_db_link **ib_link, ibase_trans **trans)
{
	int type;

	IBDEBUG("Transaction or database link?");
	if (zend_list_find(Z_LVAL_PP(link_id), &type)) {
	 	if (type == le_trans) {
			/* Transaction resource: make sure it refers to one link only, then 
			   fetch it; database link is stored in ib_trans->db_link[]. */
			IBDEBUG("Type is le_trans");
			ZEND_FETCH_RESOURCE(*trans, ibase_trans *, link_id, -1, "InterBase transaction", le_trans);
			if ((*trans)->link_cnt > 1) {
				_php_ibase_module_error("Link id is ambiguous: transaction spans multiple connections." TSRMLS_CC);
				return;
			}				
			*ib_link = (*trans)->db_link[0];
			return;
		}
	} 
	IBDEBUG("Type is le_[p]link or id not found");
	/* Database link resource, use default transaction. */
	*trans = NULL;
	ZEND_FETCH_RESOURCE2(*ib_link, ibase_db_link *, link_id, -1, "InterBase link", le_link, le_plink);
}
	
#define RESET_ERRMSG { IBG(errmsg)[0] = '\0'; IBG(sql_code) = 0; }

/* sql variables union
 * used for convert and binding input variables
 */
typedef struct {
	union {
		short sval;
		float fval;
		ISC_LONG lval;
		ISC_QUAD qval;
#ifdef ISC_TIMESTAMP
		ISC_TIMESTAMP tsval;
		ISC_DATE dtval;
		ISC_TIME tmval;
#endif
	} val;
	short sqlind;
} BIND_BUF;

static inline int _php_ibase_string_to_quad(char const *id, ISC_QUAD *qd)
{
	/* shortcut for most common case */
	if (sizeof(ISC_QUAD) == sizeof(unsigned ISC_INT64)) {
		return sscanf(id, BLOB_ID_MASK, (unsigned ISC_INT64 *) qd);
	} else {
		unsigned ISC_INT64 res;
		if (sscanf(id, BLOB_ID_MASK, &res)) {
			qd->gds_quad_high = (ISC_LONG) (res >> 0x20);
			qd->gds_quad_low = (unsigned ISC_LONG) (res & 0xFFFFFFFF);
			return 1;
		}
		return 0;
	}
}

static inline char *_php_ibase_quad_to_string(ISC_QUAD const qd)
{
	char *result = (char *) emalloc(BLOB_ID_LEN+1);

	/* shortcut for most common case */
	if (sizeof(ISC_QUAD) == sizeof(unsigned ISC_INT64)) {
		sprintf(result, BLOB_ID_MASK, *(unsigned ISC_INT64*)(void *) &qd); 
	} else {
		unsigned ISC_INT64 res = ((unsigned ISC_INT64) qd.gds_quad_high << 0x20) | qd.gds_quad_low;
		sprintf(result, BLOB_ID_MASK, res);
	}
	result[BLOB_ID_LEN] = '\0';
	return result;
}

/* blob information struct */
typedef struct {
	ISC_LONG  max_segment;		/* Length of longest segment */
	ISC_LONG  num_segments;		/* Total number of segments */
	ISC_LONG  total_length;		/* Total length of blob */
	int		  bl_stream;		/* blob is stream ? */
} IBASE_BLOBINFO;

/* }}} */


/* destructors ---------------------- */

/* {{{ _php_ibase_free_xsqlda() (not actual destructor)
 */
static void _php_ibase_free_xsqlda(XSQLDA *sqlda)
{
	int i;
	XSQLVAR *var;

	IBDEBUG("Free XSQLDA?");
	if (sqlda) {
		IBDEBUG("Freeing XSQLDA...");
		var = sqlda->sqlvar;
		for (i = 0; i < sqlda->sqld; i++, var++) {
			efree(var->sqldata);
			if (var->sqlind) {
				efree(var->sqlind);
			}
		}
		efree(sqlda);
	}
}
/* }}} */

/* {{{ _php_ibase_commit_link()
 */
static void _php_ibase_commit_link(ibase_db_link *link TSRMLS_DC)
{
	unsigned short i = 0, j;
	ibase_tr_list *l;
	IBDEBUG("Checking transactions to close...");

	for (l = link->tr_list; l != NULL; ++i) {
		ibase_tr_list *p = l;
		if (p->trans != NULL) {
			if (i == 0) {
				if (p->trans->handle != NULL) {
					IBDEBUG("Committing default transaction...");
					if (isc_commit_transaction(IB_STATUS, &p->trans->handle)) {
						_php_ibase_error(TSRMLS_C);
					}
				}
				efree(p->trans); /* default transaction is not a registered resource: clean up */
			}
			else {
				if (p->trans->handle != NULL) { 
					/* non-default transaction might have been rolled back by other call of this dtor */
					IBDEBUG("Rolling back other transactions...");
					if (isc_rollback_transaction(IB_STATUS, &p->trans->handle)) {
						_php_ibase_error(TSRMLS_C);
					}
				}
				/* set this link pointer to NULL in the transaction */
				for (j = 0; j < p->trans->link_cnt; ++j) {
					if (p->trans->db_link[j] == link) {
						p->trans->db_link[j] = NULL;
						break;
					}
				}
			}
		}
		l = l->next;
		efree(p);
	}
	link->tr_list = NULL;
}
/* }}} */

static void php_ibase_commit_link_rsrc(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	ibase_db_link *link = (ibase_db_link *) rsrc->ptr;

	_php_ibase_commit_link(link TSRMLS_CC);
}

/* {{{ _php_ibase_close_link()
 */
static void _php_ibase_close_link(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	ibase_db_link *link = (ibase_db_link *) rsrc->ptr;

	_php_ibase_commit_link(link TSRMLS_CC);
	if (link->handle != NULL) {
		IBDEBUG("Closing normal link...");
		isc_detach_database(IB_STATUS, &link->handle);
	}
	IBG(num_links)--;
	efree(link);
}
/* }}} */

/* {{{ _php_ibase_close_plink()
 */
static void _php_ibase_close_plink(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	ibase_db_link *link = (ibase_db_link *) rsrc->ptr;

	_php_ibase_commit_link(link TSRMLS_CC);
	IBDEBUG("Closing permanent link...");
	if (link->handle != NULL) {
		isc_detach_database(IB_STATUS, &link->handle);
	}
	IBG(num_persistent)--;
	IBG(num_links)--;
	free(link);
}
/* }}} */

/* {{{ _php_ibase_free_result()
 */
static void _php_ibase_free_result(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	ibase_result *ib_result = (ibase_result *) rsrc->ptr;

	IBDEBUG("Freeing result by dtor...");
	if (ib_result) {
		_php_ibase_free_xsqlda(ib_result->out_sqlda);
		if (ib_result->stmt && ib_result->type != EXECUTE_RESULT) {
			IBDEBUG("Dropping statement handle (free_result dtor)...");
			isc_dsql_free_statement(IB_STATUS, &ib_result->stmt, DSQL_drop);
		}
		efree(ib_result);
	}
}
/* }}} */

/* {{{ _php_ibase_free_query() */
static void _php_ibase_free_query(ibase_query *ib_query TSRMLS_DC)
{
	IBDEBUG("Freeing query...");

	if (ib_query->in_sqlda) {
		efree(ib_query->in_sqlda);
	}
	if (ib_query->out_sqlda) {
		efree(ib_query->out_sqlda);
	}
	if (ib_query->stmt) {
		IBDEBUG("Dropping statement handle (free_query)...");
		if (isc_dsql_free_statement(IB_STATUS, &ib_query->stmt, DSQL_drop)) {
			_php_ibase_error(TSRMLS_C);
		}
	}
	if (ib_query->in_array) {
		efree(ib_query->in_array);
	}
	if (ib_query->out_array) {
		efree(ib_query->out_array);
	}
	if (ib_query->query) {
		efree(ib_query->query);
	}
}
/* }}} */

/* {{{ php_ibase_free_query_rsrc() */
static void php_ibase_free_query_rsrc(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	ibase_query *ib_query = (ibase_query *)rsrc->ptr;

	if (ib_query != NULL) {
		IBDEBUG("Preparing to free query by dtor...");

		_php_ibase_free_query(ib_query TSRMLS_CC);
		if (ib_query->statement_type != isc_info_sql_stmt_exec_procedure) {
			zend_list_delete(ib_query->result_res_id);
		}
		efree(ib_query);
	}
}
/* }}} */

/* {{{ _php_ibase_free_blob()	*/
static void _php_ibase_free_blob(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	ibase_blob *ib_blob = (ibase_blob *)rsrc->ptr;

	if (ib_blob->bl_handle != NULL) { /* blob open*/
		if (isc_cancel_blob(IB_STATUS, &ib_blob->bl_handle)) {
			_php_ibase_module_error("You can lose data. Close any blob after " 
				"reading from or writing to it. Use ibase_blob_close() before calling ibase_close()" TSRMLS_CC);
		}
	}
	efree(ib_blob);
}
/* }}} */

/* {{{ _php_ibase_free_trans()	*/
static void _php_ibase_free_trans(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	ibase_trans *trans = (ibase_trans *)rsrc->ptr;
	unsigned short i;
	
	IBDEBUG("Cleaning up transaction resource...");
	if (trans->handle != NULL) {
		IBDEBUG("Rolling back unhandled transaction...");
		if (isc_rollback_transaction(IB_STATUS, &trans->handle)) {
			_php_ibase_error(TSRMLS_C);
		}
	}

	/* now remove this transaction from all the connection-transaction lists */
	for (i = 0; i < trans->link_cnt; ++i) {
		if (trans->db_link[i] != NULL) {
			ibase_tr_list **l;
			for (l = &trans->db_link[i]->tr_list; *l != NULL; l = &(*l)->next) {
				if ( (*l)->trans == trans) {
					ibase_tr_list *p = *l;
					*l = p->next;
					efree(p);
					break;
				}
			}
		}
	}
	efree(trans);
}
/* }}} */

/* {{{	_php_ibase_event_free() */
static void _php_ibase_event_free(char *event_buf, char *result_buf)
{
	isc_free(event_buf);
	isc_free(result_buf);
}
/* }}} */

/* {{{ _php_ibase_free_event */
static void _php_ibase_free_event(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	ibase_event *event = (ibase_event *) rsrc->ptr;
	
	IBDEBUG("Cleaning up event resource");

	zval_dtor(event->callback);
	event->callback = NULL;
	
	if (event->link->handle != NULL) {
		if (isc_cancel_events(IB_STATUS, &event->link->handle, &event->event_id)) {
			_php_ibase_error(TSRMLS_C);
		}
	}
	_php_ibase_event_free(event->event_buffer,event->result_buffer);
}
/* }}} */

/* {{{ startup, shutdown and info functions */
PHP_INI_BEGIN()
	STD_PHP_INI_BOOLEAN("ibase.allow_persistent", "1", PHP_INI_SYSTEM, OnUpdateLong, allow_persistent, zend_ibase_globals, ibase_globals)
	STD_PHP_INI_ENTRY_EX("ibase.max_persistent", "-1", PHP_INI_SYSTEM, OnUpdateLong, max_persistent, zend_ibase_globals, ibase_globals, display_link_numbers)
	STD_PHP_INI_ENTRY_EX("ibase.max_links", "-1", PHP_INI_SYSTEM, OnUpdateLong, max_links, zend_ibase_globals, ibase_globals, display_link_numbers)
	STD_PHP_INI_ENTRY("ibase.default_user", NULL, PHP_INI_ALL, OnUpdateString, default_user, zend_ibase_globals, ibase_globals)
	STD_PHP_INI_ENTRY("ibase.default_password", NULL, PHP_INI_ALL, OnUpdateString, default_password, zend_ibase_globals, ibase_globals)
	STD_PHP_INI_ENTRY("ibase.timestampformat", "%m/%d/%Y %H:%M:%S", PHP_INI_ALL, OnUpdateString, cfg_timestampformat, zend_ibase_globals, ibase_globals)
	STD_PHP_INI_ENTRY("ibase.dateformat", "%m/%d/%Y", PHP_INI_ALL, OnUpdateString, cfg_dateformat, zend_ibase_globals, ibase_globals)
	STD_PHP_INI_ENTRY("ibase.timeformat", "%H:%M:%S", PHP_INI_ALL, OnUpdateString, cfg_timeformat, zend_ibase_globals, ibase_globals)
PHP_INI_END()

static void php_ibase_init_globals(zend_ibase_globals *ibase_globals)
{
	ibase_globals->timestampformat = NULL;
	ibase_globals->dateformat = NULL;
	ibase_globals->timeformat = NULL;
	ibase_globals->num_persistent = 0;
	ibase_globals->sql_code = 0;
}

PHP_MINIT_FUNCTION(ibase)
{
	ZEND_INIT_MODULE_GLOBALS(ibase, php_ibase_init_globals, NULL);

	REGISTER_INI_ENTRIES();

	le_result = zend_register_list_destructors_ex(_php_ibase_free_result, NULL, "interbase result", module_number);
	le_query = zend_register_list_destructors_ex(php_ibase_free_query_rsrc, NULL, "interbase query", module_number);
	le_blob = zend_register_list_destructors_ex(_php_ibase_free_blob, NULL, "interbase blob", module_number);
	le_link = zend_register_list_destructors_ex(_php_ibase_close_link, NULL, "interbase link", module_number);
	le_plink = zend_register_list_destructors_ex(php_ibase_commit_link_rsrc, _php_ibase_close_plink, "interbase link persistent", module_number);
	le_trans = zend_register_list_destructors_ex(_php_ibase_free_trans, NULL, "interbase transaction", module_number);
	le_event = zend_register_list_destructors_ex(_php_ibase_free_event, NULL, "interbase event", module_number);

	REGISTER_LONG_CONSTANT("IBASE_DEFAULT", PHP_IBASE_DEFAULT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IBASE_TEXT", PHP_IBASE_FETCH_BLOBS, CONST_PERSISTENT); /* deprecated, for BC only */
	REGISTER_LONG_CONSTANT("IBASE_FETCH_BLOBS", PHP_IBASE_FETCH_BLOBS, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IBASE_FETCH_ARRAYS", PHP_IBASE_FETCH_ARRAYS, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IBASE_UNIXTIME", PHP_IBASE_UNIXTIME, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IBASE_TIMESTAMP", PHP_IBASE_TIMESTAMP, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IBASE_DATE", PHP_IBASE_DATE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IBASE_TIME", PHP_IBASE_TIME, CONST_PERSISTENT);
	/* transactions */
	REGISTER_LONG_CONSTANT("IBASE_WRITE", PHP_IBASE_WRITE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IBASE_READ", PHP_IBASE_READ, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IBASE_COMMITTED", PHP_IBASE_COMMITTED, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IBASE_CONSISTENCY", PHP_IBASE_CONSISTENCY, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IBASE_CONCURRENCY", PHP_IBASE_CONCURRENCY, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IBASE_REC_VERSION", PHP_IBASE_REC_VERSION, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IBASE_REC_NO_VERSION", PHP_IBASE_REC_NO_VERSION, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IBASE_NOWAIT", PHP_IBASE_NOWAIT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IBASE_WAIT", PHP_IBASE_WAIT, CONST_PERSISTENT);
	
	return SUCCESS;
}

PHP_RINIT_FUNCTION(ibase)
{
	IBG(default_link)= -1;
	IBG(num_links) = IBG(num_persistent);

	if (IBG(timestampformat)) {
		DL_FREE(IBG(timestampformat));
	}
	IBG(timestampformat) = DL_STRDUP(IBG(cfg_timestampformat));

	if (IBG(dateformat)) {
		DL_FREE(IBG(dateformat));
	}
	IBG(dateformat) = DL_STRDUP(IBG(cfg_dateformat));

	if (IBG(timeformat)) {
		DL_FREE(IBG(timeformat));
	}
	IBG(timeformat) = DL_STRDUP(IBG(cfg_timeformat));

	RESET_ERRMSG;

	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(ibase)
{
	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(ibase)
{
	if (IBG(timestampformat)) {
		DL_FREE(IBG(timestampformat));
	}
	IBG(timestampformat) = NULL;

	if (IBG(dateformat)) {
		DL_FREE(IBG(dateformat));
	}
	IBG(dateformat) = NULL;

	if (IBG(timeformat)) {
		DL_FREE(IBG(timeformat));
	}
	IBG(timeformat) = NULL;

	return SUCCESS;
} 
 
PHP_MINFO_FUNCTION(ibase)
{
	char tmp[32];

	php_info_print_table_start();
	php_info_print_table_row(2, "Interbase Support", "enabled");
	php_info_print_table_row(2, "Revision", "$Revision$");
#ifdef COMPILE_DL_INTERBASE
	php_info_print_table_row(2, "Dynamic Module", "Yes");
#endif
	php_info_print_table_row(2, "Allow Persistent Links", (IBG(allow_persistent) ? "Yes" : "No"));

	if (IBG(max_persistent) == -1) {
		snprintf(tmp, 31, "%ld/unlimited", IBG(num_persistent));
	} else {
		snprintf(tmp, 31, "%ld/%ld", IBG(num_persistent), IBG(max_persistent));
	}
	tmp[31] = 0;
	php_info_print_table_row(2, "Persistent Links", tmp);

	if (IBG(max_links) == -1) {
		snprintf(tmp, 31, "%ld/unlimited", IBG(num_links));
	} else {
		snprintf(tmp, 31, "%ld/%ld", IBG(num_links), IBG(max_links));
	}
	tmp[31] = 0;
	php_info_print_table_row(2, "Total Links", tmp);

	php_info_print_table_row(2, "Timestamp Format", IBG(timestampformat));
	php_info_print_table_row(2, "Date Format", IBG(dateformat));
	php_info_print_table_row(2, "Time Format", IBG(timeformat));

	php_info_print_table_end();
}
/* }}} */

/* {{{ _php_ibase_attach_db() */
static int _php_ibase_attach_db(char *server, char *uname, char *passwd, char *charset, int buffers, char *role, isc_db_handle *db TSRMLS_DC)
{
	char dpb_buffer[256], *dpb, *p;
	int dpb_length, len;

	dpb = dpb_buffer;

	*dpb++ = isc_dpb_version1;

	if (uname != NULL && (len = strlen(uname))) {
		*dpb++ = isc_dpb_user_name;
		*dpb++ = len;
		for (p = uname; *p;) {
			*dpb++ = *p++;
		}
	}

	if (passwd != NULL && (len = strlen(passwd))) {
		*dpb++ = isc_dpb_password;
		*dpb++ = strlen(passwd);
		for (p = passwd; *p;) {
			*dpb++ = *p++;
		}
	}

	if (charset != NULL && (len = strlen(charset))) {
		*dpb++ = isc_dpb_lc_ctype;
		*dpb++ = strlen(charset);
		for (p = charset; *p;) {
			*dpb++ = *p++;
		}
	}

	if (buffers) {
		*dpb++ = isc_dpb_num_buffers;
		*dpb++ = 1;
		*dpb++ = buffers;
	}

#ifdef isc_dpb_sql_role_name
	if (role != NULL && (len = strlen(role))) {
		*dpb++ = isc_dpb_sql_role_name;
		*dpb++ = strlen(role);
		for (p = role; *p;) {
			*dpb++ = *p++;
		}
	}
#endif

	dpb_length = dpb - dpb_buffer;

	if (isc_attach_database(IB_STATUS, (short) strlen(server), server, db, (short) dpb_length, dpb_buffer)) {
		_php_ibase_error(TSRMLS_C);
		return FAILURE;
	}
	return SUCCESS;
}
/* }}} */

/* {{{ _php_ibase_connect() */
static void _php_ibase_connect(INTERNAL_FUNCTION_PARAMETERS, int persistent)
{
	zval ***args;
	char *ib_server = NULL, *ib_uname, *ib_passwd, *ib_charset = NULL, *ib_buffers = NULL, *ib_dialect = NULL, *ib_role = NULL;
	int ib_uname_len, ib_passwd_len;
	isc_db_handle db_handle = NULL;
	char *hashed_details;
	int hashed_details_length = 0;
	ibase_db_link *ib_link = NULL;
	
	RESET_ERRMSG;
		
	ib_uname = IBG(default_user);
	ib_passwd = IBG(default_password);
	ib_uname_len = ib_uname ? strlen(ib_uname) : 0;
	ib_passwd_len = ib_passwd ? strlen(ib_passwd) : 0;
	
	if (ZEND_NUM_ARGS() < 1 || ZEND_NUM_ARGS() > 7) {
		WRONG_PARAM_COUNT;
	}
	
	args = (zval ***) safe_emalloc(sizeof(zval **), ZEND_NUM_ARGS(), 0);
	if (zend_get_parameters_array_ex(ZEND_NUM_ARGS(), args) == FAILURE) {
		efree(args);
		RETURN_FALSE;
	}

	switch (ZEND_NUM_ARGS()) {
		case 7:
			convert_to_string_ex(args[6]);
			ib_role = Z_STRVAL_PP(args[6]);
			hashed_details_length += Z_STRLEN_PP(args[6]);
			/* fallout */
		case 6:
			convert_to_string_ex(args[5]);
			ib_dialect = Z_STRVAL_PP(args[5]);
			hashed_details_length += Z_STRLEN_PP(args[5]);
			/* fallout */
		case 5:
			convert_to_string_ex(args[4]);
			ib_buffers = Z_STRVAL_PP(args[4]);
			hashed_details_length += Z_STRLEN_PP(args[4]);
			/* fallout */
		case 4:
			convert_to_string_ex(args[3]);
			ib_charset = Z_STRVAL_PP(args[3]);
			hashed_details_length += Z_STRLEN_PP(args[3]);
			/* fallout */
		case 3:
			convert_to_string_ex(args[2]);
			ib_passwd = Z_STRVAL_PP(args[2]);
			hashed_details_length += Z_STRLEN_PP(args[2]);
			/* fallout */
		case 2:
			convert_to_string_ex(args[1]);
			ib_uname = Z_STRVAL_PP(args[1]);
			hashed_details_length += Z_STRLEN_PP(args[1]);
			/* fallout */
		case 1:
			convert_to_string_ex(args[0]);
			ib_server = Z_STRVAL_PP(args[0]);
			hashed_details_length += Z_STRLEN_PP(args[0]);
	} /* case */
	
	efree(args);
	
	hashed_details = (char *) emalloc(hashed_details_length + strlen("ibase_%s_%s_%s_%s_%s_%s_%s") + 1);
	sprintf(hashed_details, "ibase_%s_%s_%s_%s_%s_%s_%s", SAFE_STRING(ib_server), SAFE_STRING(ib_uname), SAFE_STRING(ib_passwd), SAFE_STRING(ib_charset), SAFE_STRING(ib_buffers), SAFE_STRING(ib_dialect), SAFE_STRING(ib_role));

	if (persistent) {
		list_entry *le;
		int open_new_connection = 1;
		
		if ( (zend_hash_find(&EG(persistent_list), hashed_details, hashed_details_length + 1, (void *) &le) != FAILURE) ) {
			static char info[] = {isc_info_base_level, isc_info_end};
			char result[8]; /* Enough? Hope so... */ 

			if (Z_TYPE_P(le) != le_plink) {
				RETURN_FALSE;
			}
			/* Check if connection has timed out */
			ib_link = (ibase_db_link *) le->ptr;
			if (!isc_database_info(IB_STATUS, &ib_link->handle, sizeof(info), info, sizeof(result), result)) {
				open_new_connection = 0;
			}
		}
		
		/* There was no previous connection to use or it has timed out */
		if (open_new_connection) {
			list_entry new_le;
			
			if ((IBG(max_links) != -1) && (IBG(num_links) >= IBG(max_links))) {
				_php_ibase_module_error("Too many open links (%ld)" TSRMLS_CC, IBG(num_links));
				efree(hashed_details);
				RETURN_FALSE;
			}
			if ((IBG(max_persistent) != -1) && (IBG(num_persistent) >= IBG(max_persistent))) {
				_php_ibase_module_error("Too many open persistent links (%ld)" TSRMLS_CC, IBG(num_persistent));
				efree(hashed_details);
				RETURN_FALSE;
			}

			/* create the ib_link */

			if (_php_ibase_attach_db(ib_server, ib_uname, ib_passwd, ib_charset,
									 (ib_buffers ? strtoul(ib_buffers, NULL, 0) : 0),
									 ib_role, &db_handle TSRMLS_CC) == FAILURE) {
				efree(hashed_details);
				RETURN_FALSE;
			}

			ib_link = (ibase_db_link *) malloc(sizeof(ibase_db_link));
			ib_link->handle = db_handle;
			ib_link->dialect = (ib_dialect ? (unsigned short) strtoul(ib_dialect, NULL, 10) : SQL_DIALECT_CURRENT);
			ib_link->tr_list = NULL;

			/* hash it up */
			Z_TYPE(new_le) = le_plink;
			new_le.ptr = ib_link;
			if (zend_hash_update(&EG(persistent_list), hashed_details, hashed_details_length + 1, (void *) &new_le, sizeof(list_entry), NULL) == FAILURE) {
				efree(hashed_details);
				free(ib_link);
				RETURN_FALSE;
			}
			IBG(num_links)++;
			IBG(num_persistent)++;
		}

		ZEND_REGISTER_RESOURCE(return_value, ib_link, le_plink);

	} else {
		list_entry *index_ptr, new_index_ptr;
		
		/* first we check the hash for the hashed_details key.	if it exists,
		 * it should point us to the right offset where the actual ib_link sits.
		 * if it doesn't, open a new ib_link, add it to the resource list,
		 * and add a pointer to it with hashed_details as the key.
		 */
		if ( (zend_hash_find(&EG(regular_list), hashed_details, hashed_details_length + 1, (void *) &index_ptr) == SUCCESS) ) {
			int type, xlink;
			void *ptr;
			if (Z_TYPE_P(index_ptr) != le_index_ptr) {
				RETURN_FALSE;
			}
			xlink = (int) index_ptr->ptr;
			ptr = zend_list_find(xlink, &type);	 /* check if the xlink is still there */
			if (ptr && (type == le_link || type == le_plink)) {
				zend_list_addref(xlink);
				Z_LVAL_P(return_value) = xlink;
				Z_TYPE_P(return_value) = IS_RESOURCE;
				IBG(default_link) = Z_LVAL_P(return_value);
				efree(hashed_details);
				return;
			} else {
				zend_hash_del(&EG(regular_list), hashed_details, hashed_details_length + 1);
			}
		}
		if ((IBG(max_links) != -1) && (IBG(num_links) >= IBG(max_links))) {
			_php_ibase_module_error("Too many open links (%ld)" TSRMLS_CC, IBG(num_links));
			efree(hashed_details);
			RETURN_FALSE;
		}
		/* create the ib_link */

		if (_php_ibase_attach_db(ib_server, ib_uname, ib_passwd, ib_charset,
								 (ib_buffers ? strtoul(ib_buffers, NULL, 0) : 0),
								 ib_role, &db_handle TSRMLS_CC) == FAILURE) {
			efree(hashed_details);
			RETURN_FALSE;
		}

		ib_link = (ibase_db_link *) emalloc(sizeof(ibase_db_link));
		ib_link->handle = db_handle;
		ib_link->dialect = (ib_dialect ? (unsigned short) strtoul(ib_dialect, NULL, 10) : SQL_DIALECT_CURRENT);
		ib_link->tr_list = NULL;
		
		ZEND_REGISTER_RESOURCE(return_value, ib_link, le_link);

		/* add it to the hash */
		new_index_ptr.ptr = (void *) Z_LVAL_P(return_value);
		Z_TYPE(new_index_ptr) = le_index_ptr;
		if (zend_hash_update(&EG(regular_list), hashed_details, hashed_details_length + 1, (void *) &new_index_ptr, sizeof(list_entry), NULL) == FAILURE) {
			efree(hashed_details);
			RETURN_FALSE;
		}
		IBG(num_links)++;
	}
	efree(hashed_details);
	zend_list_addref(Z_LVAL_P(return_value));
	IBG(default_link) = Z_LVAL_P(return_value);
}
/* }}} */

/* {{{ proto resource ibase_connect(string database [, string username [, string password [, string charset [, int buffers [, int dialect [, string role]]]]]])
   Open a connection to an InterBase database */
PHP_FUNCTION(ibase_connect)
{
	_php_ibase_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto resource ibase_pconnect(string database [, string username [, string password [, string charset [, int buffers [, int dialect [, string role]]]]]])
   Open a persistent connection to an InterBase database */
PHP_FUNCTION(ibase_pconnect)
{
	_php_ibase_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto bool ibase_close([resource link_identifier])
   Close an InterBase connection */
PHP_FUNCTION(ibase_close)
{
	zval **link_arg = NULL;
	ibase_db_link *ib_link;
	int link_id;
	
	RESET_ERRMSG;
	
	switch (ZEND_NUM_ARGS()) {
		case 0:
			link_id = IBG(default_link);
			IBG(default_link) = -1;
			break;
		case 1:
			if (zend_get_parameters_ex(1, &link_arg) == FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long_ex(link_arg);
			link_id = Z_LVAL_PP(link_arg);
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}

	ZEND_FETCH_RESOURCE2(ib_link, ibase_db_link *, link_arg, link_id, "InterBase link", le_link, le_plink);
	zend_list_delete(link_id);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool ibase_drop_db([resource link_identifier])
   Drop an InterBase database */
PHP_FUNCTION(ibase_drop_db)
{
	zval **link_arg = NULL;
	ibase_db_link *ib_link;
	ibase_tr_list *l;
	int link_id;
	
	RESET_ERRMSG;
	
	switch (ZEND_NUM_ARGS()) {
		case 0:
			link_id = IBG(default_link);
			IBG(default_link) = -1;
			break;
		case 1:
			if (zend_get_parameters_ex(1, &link_arg) == FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long_ex(link_arg);
			link_id = Z_LVAL_PP(link_arg);
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	ZEND_FETCH_RESOURCE2(ib_link, ibase_db_link *, link_arg, link_id, "InterBase link", le_link, le_plink);
	if (isc_drop_database(IB_STATUS, &ib_link->handle)) {
		_php_ibase_error(TSRMLS_C);
		RETURN_FALSE;
	}

	/* isc_drop_database() doesn't invalidate the transaction handles */
	for (l = ib_link->tr_list; l != NULL; l = l->next) {
		if (l->trans != NULL) l->trans->handle = NULL;
	}

	zend_list_delete(link_id);
	RETURN_TRUE;
}
/* }}} */

/* {{{ _php_ibase_alloc_array() */
static int _php_ibase_alloc_array(ibase_array **ib_arrayp, XSQLDA *sqlda, isc_db_handle link, isc_tr_handle trans TSRMLS_DC)
{
#define IB_ARRAY (*ib_arrayp)

	unsigned short i;
	XSQLVAR *var = sqlda->sqlvar;

	IB_ARRAY = safe_emalloc(sizeof(ibase_array), sqlda->sqld, 0);
	
	for (i = 0; i < sqlda->sqld; i++, var++) {
		unsigned short dim;
		unsigned long ar_size = 1;
		
		if ((var->sqltype & ~1) == SQL_ARRAY) {
			ISC_ARRAY_DESC *ar_desc = &IB_ARRAY[i].ar_desc;
			
			if (isc_array_lookup_bounds(IB_STATUS, &link, &trans, var->relname, var->sqlname, ar_desc)) {
				_php_ibase_error(TSRMLS_C);
				efree(IB_ARRAY);
				IB_ARRAY = NULL;
				return FAILURE;
			}

			switch (ar_desc->array_desc_dtype) {
				case blr_text:
				case blr_text2:
					IB_ARRAY[i].el_type = SQL_TEXT;
					IB_ARRAY[i].el_size = ar_desc->array_desc_length;
					break;
				case blr_short:
					IB_ARRAY[i].el_type = SQL_SHORT;
					IB_ARRAY[i].el_size = sizeof(short);
					break;
				case blr_long:
					IB_ARRAY[i].el_type = SQL_LONG;
					IB_ARRAY[i].el_size = sizeof(ISC_LONG);
					break;
				case blr_float:
					IB_ARRAY[i].el_type = SQL_FLOAT;
					IB_ARRAY[i].el_size = sizeof(float);
					break;
				case blr_double:
					IB_ARRAY[i].el_type = SQL_DOUBLE;
					IB_ARRAY[i].el_size = sizeof(double);
					break;
#ifdef blr_int64
				case blr_int64:
					IB_ARRAY[i].el_type = SQL_INT64;
					IB_ARRAY[i].el_size = sizeof(ISC_INT64);
					break;
#endif
#ifndef blr_timestamp
				case blr_date:
					IB_ARRAY[i].el_type = SQL_DATE;
					IB_ARRAY[i].el_size = sizeof(ISC_QUAD);
					break;
#else
				case blr_timestamp:
					IB_ARRAY[i].el_type = SQL_TIMESTAMP;
					IB_ARRAY[i].el_size = sizeof(ISC_TIMESTAMP);
					break;
				case blr_sql_date:
					IB_ARRAY[i].el_type = SQL_TYPE_DATE;
					IB_ARRAY[i].el_size = sizeof(ISC_DATE);
					break;
				case blr_sql_time:
					IB_ARRAY[i].el_type = SQL_TYPE_TIME;
					IB_ARRAY[i].el_size = sizeof(ISC_TIME);
					break;
#endif						
				case blr_varying:
				case blr_varying2:
					/**
					 * IB has a strange way of handling VARCHAR arrays. It doesn't store
					 * the length in the first short, as with VARCHAR fields. It does, 
					 * however, expect the extra short to be allocated for each element.
					 */
					IB_ARRAY[i].el_type = SQL_TEXT;
					IB_ARRAY[i].el_size = ar_desc->array_desc_length + sizeof(short);
					break;
				case blr_quad:
				case blr_blob_id:
				case blr_cstring:
				case blr_cstring2:
					/**
					 * These types are mentioned as array types in the manual, but I 
					 * wouldn't know how to create an array field with any of these
					 * types. I assume these types are not applicable to arrays, and
					 * were mentioned erroneously.
					 */
				default:
					_php_ibase_module_error("Unsupported array type %d in relation '%s' column '%s'" TSRMLS_CC, ar_desc->array_desc_dtype, var->relname, var->sqlname);
					efree(IB_ARRAY);
					IB_ARRAY = NULL;
					return FAILURE;
			} /* switch array_desc_type */
			
			/* calculate elements count */
			for (dim = 0; dim < ar_desc->array_desc_dimensions; dim++) {
				ar_size *= 1 + ar_desc->array_desc_bounds[dim].array_bound_upper - ar_desc->array_desc_bounds[dim].array_bound_lower;
			}
			IB_ARRAY[i].ar_size = IB_ARRAY[i].el_size * ar_size;
		} /* if SQL_ARRAY */
	} /* for column */
	return SUCCESS;
#undef IB_ARRAY
}
/* }}} */

/* {{{ _php_ibase_alloc_query() */
/* allocate and prepare query */
static int _php_ibase_alloc_query(ibase_query *ib_query, ibase_db_link *link, ibase_trans *trans, char *query, unsigned short dialect, int trans_res_id TSRMLS_DC)
{
	static char info_type[] = {isc_info_sql_stmt_type};
	char result[8];
	
	ib_query->link = link;
	ib_query->trans = trans;
	ib_query->result_res_id = 0;
	ib_query->stmt = NULL;
	ib_query->in_array = NULL;
	ib_query->out_array = NULL;
	ib_query->dialect = dialect;
	ib_query->query = estrdup(query);
	ib_query->trans_res_id = trans_res_id;
	
	if (isc_dsql_allocate_statement(IB_STATUS, &link->handle, &ib_query->stmt)) {
		_php_ibase_error(TSRMLS_C);
		goto _php_ibase_alloc_query_error;
	}

	ib_query->out_sqlda = (XSQLDA *) emalloc(XSQLDA_LENGTH(1));
	ib_query->out_sqlda->sqln = 1;
	ib_query->out_sqlda->version = SQLDA_CURRENT_VERSION;

	if (isc_dsql_prepare(IB_STATUS, &ib_query->trans->handle, &ib_query->stmt, 0, query, dialect, ib_query->out_sqlda)) {
		_php_ibase_error(TSRMLS_C);
		goto _php_ibase_alloc_query_error;
	}

	/* find out what kind of statement was prepared */
	if (isc_dsql_sql_info(IB_STATUS, &ib_query->stmt, sizeof(info_type), info_type, sizeof(result), result)) {
		_php_ibase_error(TSRMLS_C);
		goto _php_ibase_alloc_query_error;
	}
	ib_query->statement_type = result[3];	
	
	/* not enough output variables ? */
	if (ib_query->out_sqlda->sqld > ib_query->out_sqlda->sqln) {
		ib_query->out_sqlda = erealloc(ib_query->out_sqlda, XSQLDA_LENGTH(ib_query->out_sqlda->sqld));
		ib_query->out_sqlda->sqln = ib_query->out_sqlda->sqld;
		ib_query->out_sqlda->version = SQLDA_CURRENT_VERSION;
		if (isc_dsql_describe(IB_STATUS, &ib_query->stmt, SQLDA_CURRENT_VERSION, ib_query->out_sqlda)) {
			_php_ibase_error(TSRMLS_C);
			goto _php_ibase_alloc_query_error;
		}
	}

	/* maybe have input placeholders? */
	ib_query->in_sqlda = emalloc(XSQLDA_LENGTH(1));
	ib_query->in_sqlda->sqln = 1;
	ib_query->in_sqlda->version = SQLDA_CURRENT_VERSION;
	if (isc_dsql_describe_bind(IB_STATUS, &ib_query->stmt, SQLDA_CURRENT_VERSION, ib_query->in_sqlda)) {
		_php_ibase_error(TSRMLS_C);
		goto _php_ibase_alloc_query_error;
	}
	
	/* not enough input variables ? */
	if (ib_query->in_sqlda->sqln < ib_query->in_sqlda->sqld) {
		ib_query->in_sqlda = erealloc(ib_query->in_sqlda, XSQLDA_LENGTH(ib_query->in_sqlda->sqld));
		ib_query->in_sqlda->sqln = ib_query->in_sqlda->sqld;
		ib_query->in_sqlda->version = SQLDA_CURRENT_VERSION;
		if (isc_dsql_describe_bind(IB_STATUS, &ib_query->stmt, SQLDA_CURRENT_VERSION, ib_query->in_sqlda)) {
			_php_ibase_error(TSRMLS_C);
			goto _php_ibase_alloc_query_error;
		}
	}
	
	/* no, haven't placeholders at all */
	if (ib_query->in_sqlda->sqld == 0) {
		efree(ib_query->in_sqlda);
		ib_query->in_sqlda = NULL;
	} else if (_php_ibase_alloc_array(&ib_query->in_array, ib_query->in_sqlda, link->handle, trans->handle TSRMLS_CC) == FAILURE) {
		goto _php_ibase_alloc_query_error;
	}

	if (ib_query->out_sqlda->sqld == 0) {
		efree(ib_query->out_sqlda);
		ib_query->out_sqlda = NULL;
	} else 	if (_php_ibase_alloc_array(&ib_query->out_array, ib_query->out_sqlda, link->handle, trans->handle TSRMLS_CC) == FAILURE) {
		goto _php_ibase_alloc_query_error;
	}

	return SUCCESS;
	
_php_ibase_alloc_query_error:
	
	if (ib_query->out_sqlda) {
		efree(ib_query->out_sqlda);
	}
	if (ib_query->in_sqlda) {
		efree(ib_query->in_sqlda);
	}
	if (ib_query->out_array) {
		efree(ib_query->out_array);
	}
	if (ib_query->query) {
		efree(ib_query->query);
	}
	return FAILURE;
}
/* }}} */

/* ((( _php_ibase_blob_add */
static int _php_ibase_blob_add(zval **string_arg, ibase_blob *ib_blob TSRMLS_DC)
{
	unsigned long put_cnt = 0, rem_cnt;
	unsigned short chunk_size;

	convert_to_string_ex(string_arg);

	for (rem_cnt = Z_STRLEN_PP(string_arg); rem_cnt > 0; rem_cnt -= chunk_size)  {
		
		chunk_size = rem_cnt > USHRT_MAX ? USHRT_MAX : (unsigned short)rem_cnt;

		if (isc_put_segment(IB_STATUS, &ib_blob->bl_handle, chunk_size, &Z_STRVAL_PP(string_arg)[put_cnt] )) {
			_php_ibase_error(TSRMLS_C);
			return FAILURE;
		}
		put_cnt += chunk_size;
	}
	return SUCCESS;
}	
/* }}} */

/* {{{ _php_ibase_bind_array() */
static int _php_ibase_bind_array(zval *val, char *buf, unsigned long buf_size, ibase_array *array, int dim TSRMLS_DC)
{
	zval null_val, *pnull_val = &null_val;
	int u_bound = array->ar_desc.array_desc_bounds[dim].array_bound_upper,
		l_bound = array->ar_desc.array_desc_bounds[dim].array_bound_lower,
		dim_len = 1 + u_bound - l_bound;

	ZVAL_NULL(pnull_val);

	if (dim < array->ar_desc.array_desc_dimensions) {
		unsigned long slice_size = buf_size / dim_len;
		unsigned short i;
		zval **subval = &val;
				
		if (Z_TYPE_P(val) == IS_ARRAY) {
			zend_hash_internal_pointer_reset(Z_ARRVAL_P(val));
		}

		for (i = 0; i < dim_len; ++i) { 

			if (Z_TYPE_P(val) == IS_ARRAY &&
				zend_hash_get_current_data(Z_ARRVAL_P(val), (void *) &subval) == FAILURE)
			{
				subval = &pnull_val;
			}
				
			if (_php_ibase_bind_array(*subval, buf, slice_size, array, dim + 1 TSRMLS_CC) == FAILURE) 
			{
				return FAILURE;
			}
			buf += slice_size;

			if (Z_TYPE_P(val) == IS_ARRAY) {
				zend_hash_move_forward(Z_ARRVAL_P(val));
			}
		}

		if (Z_TYPE_P(val) == IS_ARRAY) {
			zend_hash_internal_pointer_reset(Z_ARRVAL_P(val));
		}

	} else {
		/* expect a single value */
		if (Z_TYPE_P(val) == IS_NULL) {
			memset(buf, 0, buf_size);
		} else if (array->ar_desc.array_desc_scale < 0) {

			/* no coercion for array types */
			double l;
			
			convert_to_double(val);

			if (Z_DVAL_P(val) > 0) {
				l = Z_DVAL_P(val) * pow(10, -array->ar_desc.array_desc_scale) + .5;
			} else {
				l = Z_DVAL_P(val) * pow(10, -array->ar_desc.array_desc_scale) - .5;
			}

			switch (array->el_type) {
				case SQL_SHORT:
					if (l > SHRT_MAX || l < SHRT_MIN) {
						_php_ibase_module_error("Array parameter exceeds field width" TSRMLS_CC);
						return FAILURE;
					}
					*(short*) buf = (short) l;
					break;
				case SQL_LONG:
					if (l > ISC_LONG_MAX || l < ISC_LONG_MIN) {
						_php_ibase_module_error("Array parameter exceeds field width" TSRMLS_CC);
						return FAILURE;
					}
					*(ISC_LONG*) buf = (ISC_LONG) l;
					break;
#ifdef SQL_INT64
				case SQL_INT64:
					{
						long double l;
						
						convert_to_string(val);
						
						if (!sscanf(Z_STRVAL_P(val), "%" LL_MASK "f", &l)) {
							_php_ibase_module_error("Cannot convert '%s' to long double" TSRMLS_CC, Z_STRVAL_P(val));
							return FAILURE;
						}
						
						if (l > 0) {
							*(ISC_INT64 *) buf = (ISC_INT64) (l * pow(10, -array->ar_desc.array_desc_scale) + .5);
						} else {
							*(ISC_INT64 *) buf = (ISC_INT64) (l * pow(10, -array->ar_desc.array_desc_scale) - .5);
						}
					}
					break;
#endif
			}			
		} else {
			struct tm t = { 0, 0, 0, 0, 0, 0 };

			switch (array->el_type) {
				unsigned short n;
				
				case SQL_SHORT:
					convert_to_long(val);
					if (Z_LVAL_P(val) > SHRT_MAX || Z_LVAL_P(val) < SHRT_MIN) {
						_php_ibase_module_error("Array parameter exceeds field width" TSRMLS_CC);
						return FAILURE;
					}
					*(short *) buf = (short) Z_LVAL_P(val);
					break;
				case SQL_LONG:
					convert_to_long(val);
#if (SIZEOF_LONG > 4)
					if (Z_LVAL_P(val) > ISC_LONG_MAX || Z_LVAL_P(val) < ISC_LONG_MIN) {
						_php_ibase_module_error("Array parameter exceeds field width" TSRMLS_CC);
						return FAILURE;
					}
#endif
					*(ISC_LONG *) buf = (ISC_LONG) Z_LVAL_P(val);
					break;
#ifdef SQL_INT64
				case SQL_INT64:
#if (SIZEOF_LONG >= 8)
					convert_to_long(val);
					*(long *) buf = Z_LVAL_P(val);
#else
					{
						ISC_INT64 l;

						convert_to_string(val);
						if (!sscanf(Z_STRVAL_P(val), "%" LL_MASK "d", &l)) {
							_php_ibase_module_error("Cannot convert '%s' to long integer" TSRMLS_CC, Z_STRVAL_P(val));
							return FAILURE;
						} else {
							*(ISC_INT64 *) buf = l;
						}
					}
#endif
					break;
#endif
				case SQL_FLOAT:
					convert_to_double(val);
					*(float*) buf = (float) Z_DVAL_P(val);
					break;
				case SQL_DOUBLE:
					convert_to_double(val);
					*(double*) buf = Z_DVAL_P(val);
					break;
#ifndef SQL_TIMESTAMP
				case SQL_DATE:
#else
				case SQL_TIMESTAMP:
#endif
					convert_to_string(val);
#ifdef HAVE_STRPTIME
					strptime(Z_STRVAL_P(val), IBG(timestampformat), &t);
#else
					n = sscanf(Z_STRVAL_P(val), "%d%*[/]%d%*[/]%d %d%*[:]%d%*[:]%d", &t.tm_mon, &t.tm_mday, &t.tm_year, &t.tm_hour, &t.tm_min, &t.tm_sec);
	
					if (n != 3 && n != 6) {
						_php_ibase_module_error("Invalid date/time format (expected 3 or 6 fields, got %d. Use format 'm/d/Y H:i:s'. You gave '%s')" TSRMLS_CC, n, Z_STRVAL_P(val));
						return FAILURE;
					}
					t.tm_year -= 1900;
					t.tm_mon--;
#endif
#ifndef SQL_TIMESTAMP
					isc_encode_date(&t, (ISC_QUAD *) buf);
					break;
#else
					isc_encode_timestamp(&t, (ISC_TIMESTAMP * ) buf);
					break;
				case SQL_TYPE_DATE:
					convert_to_string(val);
#ifdef HAVE_STRPTIME
					strptime(Z_STRVAL_P(val), IBG(dateformat), &t);
#else
					n = sscanf(Z_STRVAL_P(val), "%d%*[/]%d%*[/]%d", &t.tm_mon, &t.tm_mday, &t.tm_year);
	
					if (n != 3) {
						_php_ibase_module_error("Invalid date format (expected 3 fields, got %d. Use format 'm/d/Y' You gave '%s')" TSRMLS_CC, n, Z_STRVAL_P(val));
						return FAILURE;
					}
					t.tm_year -= 1900;
					t.tm_mon--;
#endif
					isc_encode_sql_date(&t, (ISC_DATE *) buf);
					break;
				case SQL_TYPE_TIME:
					convert_to_string(val);
#ifdef HAVE_STRPTIME
					strptime(Z_STRVAL_P(val), IBG(timeformat), &t);
#else
					n = sscanf(Z_STRVAL_P(val), "%d%*[:]%d%*[:]%d", &t.tm_hour, &t.tm_min, &t.tm_sec);
	
					if (n != 3) {
						_php_ibase_module_error("Invalid time format (expected 3 fields, got %d. Use format 'H:i:s'. You gave '%s')" TSRMLS_CC, n, Z_STRVAL_P(val));
						return FAILURE;
					}
#endif
					isc_encode_sql_time(&t, (ISC_TIME *) buf);
					break;
#endif
				default:
					convert_to_string(val);
					strncpy(buf, Z_STRVAL_P(val), array->el_size);
					buf[array->el_size-1] = '\0';
			}	
		}
	}
	return SUCCESS;
}		
/* }}} */

/* {{{ _php_ibase_bind()
   Bind parameter placeholders in a previously prepared query */
static int _php_ibase_bind(XSQLDA *sqlda, zval **b_vars, BIND_BUF *buf, ibase_query *ib_query TSRMLS_DC)
{
	int i, rv = SUCCESS;
	XSQLVAR *var = sqlda->sqlvar;

	for (i = 0; i < sqlda->sqld; ++var, ++i) { /* bound vars */
		
		zval *b_var = b_vars[i];

		var->sqlind = &buf[i].sqlind;
		
		if (Z_TYPE_P(b_var) == IS_NULL) {

			if ((var->sqltype & 1) != 1) {
				_php_ibase_module_error("Parameter %d must have a value" TSRMLS_CC, i+1);
				rv = FAILURE;
			}

			buf[i].sqlind = -1;
		} else {
			buf[i].sqlind = 0;

			if (var->sqlscale < 0) {
				/*
				  DECIMAL or NUMERIC field are stored internally as scaled integers.
				  Coerce it to string and let InterBase's internal routines handle it.
				*/
				var->sqltype = SQL_TEXT;
			}
		
			switch (var->sqltype & ~1) {
				case SQL_SHORT:
					convert_to_long(b_var);
					if (Z_LVAL_P(b_var) > SHRT_MAX || Z_LVAL_P(b_var) < SHRT_MIN) {
						_php_ibase_module_error("Parameter %d exceeds field width" TSRMLS_CC, i+1);
						rv = FAILURE;
					}
					buf[i].val.sval = (short) Z_LVAL_P(b_var);
					var->sqldata = (void *) &buf[i].val.sval;
					break;
				case SQL_LONG:
					convert_to_long(b_var);
#if (SIZEOF_LONG > 4)
					/* ISC_LONG is always 32-bit */
					if (Z_LVAL_P(b_var) > ISC_LONG_MAX || Z_LVAL_P(b_var) < ISC_LONG_MIN) {
						_php_ibase_module_error("Parameter %d exceeds field width" TSRMLS_CC, i+1);
						rv = FAILURE;
					}
#endif
					buf[i].val.lval = (ISC_LONG) Z_LVAL_P(b_var);
					var->sqldata = (void *) &buf[i].val.lval;
					break;
#if defined(SQL_INT64) && (SIZEOF_LONG == 8)
				case SQL_INT64:
					convert_to_long(b_var);
					var->sqldata = (void *) &Z_LVAL_P(b_var);
					break;
#endif
				case SQL_FLOAT:
	 				convert_to_double(b_var);
					buf[i].val.fval = (float) Z_DVAL_P(b_var);
					var->sqldata = (void *) &buf[i].val.fval;
					break;
				case SQL_DOUBLE:
					convert_to_double(b_var);
					var->sqldata = (void *) &Z_DVAL_P(b_var);
					break;
#ifndef SQL_TIMESTAMP
				case SQL_DATE:
					convert_to_string(b_var);
					{
						struct tm t;
#ifdef HAVE_STRPTIME
						strptime(Z_STRVAL_P(b_var), IBG(timestampformat), &t);
#else
						/* Parsing doesn't seem to happen with older versions... */
						int n;
						
						t.tm_year = t.tm_mon = t.tm_mday = t.tm_hour = t.tm_min = t.tm_sec = 0;
						
						n = sscanf(Z_STRVAL_P(b_var), "%d%*[/]%d%*[/]%d %d%*[:]%d%*[:]%d", &t.tm_mon, &t.tm_mday, &t.tm_year, &t.tm_hour, &t.tm_min, &t.tm_sec);
		
						if (n != 3 && n != 6) {
							_php_ibase_module_error("Parameter %d: invalid date/time format (expected 3 or 6 fields, got %d. Use format m/d/Y H:i:s. You gave '%s')" TSRMLS_CC, i+1, n, Z_STRVAL_P(b_var));
							rv = FAILURE;
						}
						t.tm_year -= 1900;
						t.tm_mon--;
#endif
						isc_encode_date(&t, &buf[i].val.qval);
						var->sqldata = (void *) (&buf[i].val.qval);
					}
#else
#ifdef HAVE_STRPTIME
				case SQL_TIMESTAMP:
				case SQL_TYPE_DATE:
				case SQL_TYPE_TIME:
					{
						struct tm t;
	
						convert_to_string(b_var);
	
						switch (var->sqltype & ~1) {
							case SQL_TIMESTAMP:
								strptime(Z_STRVAL_P(b_var), IBG(timestampformat), &t);
								isc_encode_timestamp(&t, &buf[i].val.tsval);
								var->sqldata = (void *) (&buf[i].val.tsval);
								break;
							case SQL_TYPE_DATE:
								strptime(Z_STRVAL_P(b_var), IBG(dateformat), &t);
								isc_encode_sql_date(&t, &buf[i].val.dtval);
								var->sqldata = (void *) (&buf[i].val.dtval);
								break;
							case SQL_TYPE_TIME:
								strptime(Z_STRVAL_P(b_var), IBG(timeformat), &t);
								isc_encode_sql_time(&t, &buf[i].val.tmval);
								var->sqldata = (void *) (&buf[i].val.tmval);
								break;
						}
					}
#endif
#endif
					break;
				case SQL_BLOB:
						
					convert_to_string(b_var);
	
					if (Z_STRLEN_P(b_var) != BLOB_ID_LEN ||
						!_php_ibase_string_to_quad(Z_STRVAL_P(b_var), &buf[i].val.qval)) {
	
						ibase_blob ib_blob = { NULL, {0, 0}, BLOB_INPUT };
	
						if (isc_create_blob(IB_STATUS, &ib_query->link->handle, &ib_query->trans->handle, &ib_blob.bl_handle, &ib_blob.bl_qd)) {
							_php_ibase_error(TSRMLS_C);
							return FAILURE;
						}
	
						if (_php_ibase_blob_add(&b_var, &ib_blob TSRMLS_CC) != SUCCESS) {
							return FAILURE;
						}
							
						if (isc_close_blob(IB_STATUS, &ib_blob.bl_handle)) {
							_php_ibase_error(TSRMLS_C);
							return FAILURE;
						}
						buf[i].val.qval = ib_blob.bl_qd;
					}
					var->sqldata = (void *) &buf[i].val.qval;
					break;
				case SQL_ARRAY:
					if (Z_TYPE_P(b_var) != IS_ARRAY) {
						convert_to_string(b_var);
		
						if (Z_STRLEN_P(b_var) != BLOB_ID_LEN ||
							!_php_ibase_string_to_quad(Z_STRVAL_P(b_var), &buf[i].val.qval)) {

							_php_ibase_module_error("Parameter %d: invalid array ID" TSRMLS_CC,i+1);
							rv = FAILURE;
						}
					} else {
						/* convert the array data into something IB can understand */
						void *array_data = emalloc(ib_query->in_array[i].ar_size);
						ISC_QUAD array_id = { 0, 0 };

						if (_php_ibase_bind_array(b_var, array_data, ib_query->in_array[i].ar_size, 
							&ib_query->in_array[i], 0 TSRMLS_CC) == FAILURE) 
						{
							_php_ibase_module_error("Parameter %d: failed to bind array argument" TSRMLS_CC,i+1);
							efree(array_data);
							rv = FAILURE;
							break;
						}
							
						if (isc_array_put_slice(IB_STATUS, 
												&ib_query->link->handle, 
												&ib_query->trans->handle, 
												&array_id, 
												&ib_query->in_array[i].ar_desc, 
												array_data, 
												&ib_query->in_array[i].ar_size))
						{
							_php_ibase_error(TSRMLS_C);
							efree(array_data);
							return FAILURE;
						}
						buf[i].val.qval = array_id;
						efree(array_data);
					}				
					var->sqldata = (void *) &buf[i].val.qval;
					break;
				default:
					convert_to_string(b_var);
					var->sqldata = Z_STRVAL_P(b_var);
					var->sqllen	 = Z_STRLEN_P(b_var);
					var->sqltype = SQL_TEXT;
			} /* switch */
		} /* if */
	} /* for */
	return rv;
}
/* }}} */

/* {{{ _php_ibase_alloc_xsqlda() */
static void _php_ibase_alloc_xsqlda(XSQLDA *sqlda)
{
	int i;
	XSQLVAR *var = sqlda->sqlvar;

	for (i = 0; i < sqlda->sqld; i++, var++) {
		switch (var->sqltype & ~1) {
			case SQL_TEXT:
				var->sqldata = safe_emalloc(sizeof(char), (var->sqllen), 0);
				break;
			case SQL_VARYING:
				var->sqldata = safe_emalloc(sizeof(char), (var->sqllen + sizeof(short)), 0);
				break;
			case SQL_SHORT:
				var->sqldata = emalloc(sizeof(short));
				break;
			case SQL_LONG:
				var->sqldata = emalloc(sizeof(ISC_LONG));
				break;
			case SQL_FLOAT:
				var->sqldata = emalloc(sizeof(float));
					break;
			case SQL_DOUBLE:
				var->sqldata = emalloc(sizeof(double));
				break;
#ifdef SQL_INT64
			case SQL_INT64:
				var->sqldata = emalloc(sizeof(ISC_INT64));
				break;
#endif
#ifdef SQL_TIMESTAMP
			case SQL_TIMESTAMP:
				var->sqldata = emalloc(sizeof(ISC_TIMESTAMP));
				break;
			case SQL_TYPE_DATE:
				var->sqldata = emalloc(sizeof(ISC_DATE));
				break;
			case SQL_TYPE_TIME:
				var->sqldata = emalloc(sizeof(ISC_TIME));
				break;
#else
			case SQL_DATE:
#endif
			case SQL_BLOB:
			case SQL_ARRAY:
				var->sqldata = emalloc(sizeof(ISC_QUAD));
				break;
		} /* switch */

		if (var->sqltype & 1) { /* sql NULL flag */
			var->sqlind = emalloc(sizeof(short));
		} else {
			var->sqlind = NULL;
		}
	} /* for */
}
/* }}} */

/* {{{ _php_ibase_exec() */
static int _php_ibase_exec(INTERNAL_FUNCTION_PARAMETERS, ibase_result **ib_resultp, ibase_query *ib_query, int argc, zval **args)
{
#define IB_RESULT (*ib_resultp)
	XSQLDA *in_sqlda = NULL, *out_sqlda = NULL;
	BIND_BUF *bind_buf = NULL;
	int rv = FAILURE;
	static char info_count[] = {isc_info_sql_records};
	char result[64];
	ISC_STATUS isc_result;
	
	RESET_ERRMSG;

	if (argc > 0 && args != NULL) {
		SEPARATE_ZVAL(args);
	}
	
	switch (ib_query->statement_type) {
		isc_tr_handle tr;
		ibase_tr_list **l;
		ibase_trans *trans;
		
		case isc_info_sql_stmt_start_trans:
		
			/* a SET TRANSACTION statement should be executed with a NULL trans handle */
			tr = NULL;
			
			if (isc_dsql_execute_immediate(IB_STATUS, &ib_query->link->handle, &tr, 0, ib_query->query, ib_query->dialect, NULL)) {
				_php_ibase_error(TSRMLS_C);
				goto _php_ibase_exec_error;
			}
			
			trans = (ibase_trans *) emalloc(sizeof(ibase_trans));
			trans->handle = tr;
			trans->link_cnt = 1;
			trans->affected_rows = 0;
			trans->db_link[0] = ib_query->link;
	
			if (ib_query->link->tr_list == NULL) {
				ib_query->link->tr_list = (ibase_tr_list *) emalloc(sizeof(ibase_tr_list));
				ib_query->link->tr_list->trans = NULL;
				ib_query->link->tr_list->next = NULL;
			}
			
			/* link the transaction into the connection-transaction list */
			for (l = &ib_query->link->tr_list; *l != NULL; l = &(*l)->next);
			*l = (ibase_tr_list *) emalloc(sizeof(ibase_tr_list));
			(*l)->trans = trans;
			(*l)->next = NULL;
	
			ZEND_REGISTER_RESOURCE(return_value, trans, le_trans);

			return SUCCESS;

		case isc_info_sql_stmt_commit:
		case isc_info_sql_stmt_rollback:
		
			if (isc_dsql_execute_immediate(IB_STATUS, &ib_query->link->handle, &ib_query->trans->handle, 0, ib_query->query, ib_query->dialect, NULL)) {
				_php_ibase_error(TSRMLS_C);
				goto _php_ibase_exec_error;
			}

			if (ib_query->trans->handle == NULL && ib_query->trans_res_id != 0) {
				/* transaction was released by the query and was a registered resource, 
				   so we have to release it */
				zend_list_delete(ib_query->trans_res_id);
			}

			return SUCCESS;
		
		default:
			
			RETVAL_BOOL(1);
	}

	/* allocate sqlda and output buffers */
	if (ib_query->out_sqlda) { /* output variables in select, select for update */
		IBDEBUG("Query wants XSQLDA for output");
		IB_RESULT = emalloc(sizeof(ibase_result) + sizeof(ibase_array) * (ib_query->out_sqlda->sqld-1));
		IB_RESULT->link = ib_query->link;
		IB_RESULT->trans = ib_query->trans;
		IB_RESULT->stmt = ib_query->stmt; 
		IB_RESULT->statement_type = ib_query->statement_type;
		IB_RESULT->out_sqlda = NULL;
		IB_RESULT->has_more_rows = 1;

		out_sqlda = IB_RESULT->out_sqlda = emalloc(XSQLDA_LENGTH(ib_query->out_sqlda->sqld));
		memcpy(out_sqlda, ib_query->out_sqlda, XSQLDA_LENGTH(ib_query->out_sqlda->sqld));
		_php_ibase_alloc_xsqlda(out_sqlda);

		if (ib_query->out_array) {
			memcpy(&IB_RESULT->out_array, ib_query->out_array, sizeof(ibase_array) * out_sqlda->sqld);
		}
	}

	if (ib_query->in_sqlda) { /* has placeholders */
		IBDEBUG("Query wants XSQLDA for input");
		if (ib_query->in_sqlda->sqld != argc) {
			_php_ibase_module_error("Placeholders (%d) and variables (%d) mismatch" TSRMLS_CC, ib_query->in_sqlda->sqld, argc);
			goto _php_ibase_exec_error;
		}
		in_sqlda = emalloc(XSQLDA_LENGTH(ib_query->in_sqlda->sqld));
		memcpy(in_sqlda, ib_query->in_sqlda, XSQLDA_LENGTH(ib_query->in_sqlda->sqld));
		bind_buf = safe_emalloc(sizeof(BIND_BUF), ib_query->in_sqlda->sqld, 0);
		if (_php_ibase_bind(in_sqlda, args, bind_buf, ib_query TSRMLS_CC) == FAILURE) {
			IBDEBUG("Could not bind input XSQLDA");
			goto _php_ibase_exec_error;
		}
	}

	if (ib_query->statement_type == isc_info_sql_stmt_exec_procedure) {
		isc_result = isc_dsql_execute2(IB_STATUS, &ib_query->trans->handle, &ib_query->stmt, SQLDA_CURRENT_VERSION, in_sqlda, out_sqlda);
	} else {
		isc_result = isc_dsql_execute(IB_STATUS, &ib_query->trans->handle, &ib_query->stmt, SQLDA_CURRENT_VERSION, in_sqlda);
	}
	if (isc_result) {
		IBDEBUG("Could not execute query");
		_php_ibase_error(TSRMLS_C);
		goto _php_ibase_exec_error;
	}
	ib_query->trans->affected_rows = 0;
	
	switch (ib_query->statement_type) {

		unsigned long affected_rows;

		case isc_info_sql_stmt_insert:
		case isc_info_sql_stmt_update:
		case isc_info_sql_stmt_delete:
		case isc_info_sql_stmt_exec_procedure:
		
			if (isc_dsql_sql_info(IB_STATUS, &ib_query->stmt, sizeof(info_count), info_count, sizeof(result), result)) {
				_php_ibase_error(TSRMLS_C);
				goto _php_ibase_exec_error;
			}

			affected_rows = 0;
			
			if (result[0] == isc_info_sql_records) {
				unsigned i = 3, result_size = isc_vax_integer(&result[1],2);
	
				while (result[i] != isc_info_end && i < result_size) {
					short len = (short)isc_vax_integer(&result[i+1],2);
					if (result[i] != isc_info_req_select_count) {
						affected_rows += isc_vax_integer(&result[i+3],len);
					}
					i += len+3;
				}
			}
			if (affected_rows > 0) {
				ib_query->trans->affected_rows = affected_rows;
				RETVAL_LONG(affected_rows);
			}
	}

	rv = SUCCESS;
	
_php_ibase_exec_error:		 /* I'm a bad boy... */
	
	if (in_sqlda) {
		efree(in_sqlda);
	}
	if (bind_buf)
		efree(bind_buf);

	if (rv == FAILURE) {
		if (IB_RESULT) {
			efree(IB_RESULT);
			IB_RESULT = NULL;
		}
		if (out_sqlda) {
			_php_ibase_free_xsqlda(out_sqlda);
		}
	}
	
	return rv;
#undef IB_RESULT
}
/* }}} */

/* {{{ proto resource ibase_trans([int trans_args [, resource link_identifier [, ... ], int trans_args [, resource link_identifier [, ... ]] [, ...]]])
   Start a transaction over one or several databases */

#define TPB_MAX_SIZE (8*sizeof(char))

PHP_FUNCTION(ibase_trans)
{
	unsigned short i, argn, link_cnt = 0, tpb_len = 0;
	char last_tpb[TPB_MAX_SIZE];
	ibase_db_link **ib_link = NULL;
	ibase_trans *ib_trans;
	isc_tr_handle tr_handle = NULL;
	ISC_STATUS result;
	
	RESET_ERRMSG;

	argn = ZEND_NUM_ARGS();

	/* (1+argn) is an upper bound for the number of links this trans connects to */
	ib_link = (ibase_db_link **) do_alloca(sizeof(ibase_db_link *) * (1+argn));
	
	if (argn > 0) {
		long trans_argl = 0;
		char *tpb;
		ISC_TEB *teb;
		zval ***args = (zval ***) do_alloca(sizeof(zval **) * argn);

		if (zend_get_parameters_array_ex(argn, args) == FAILURE) {
			free_alloca(args);
			free_alloca(ib_link);
			RETURN_FALSE;
		}

		teb = (ISC_TEB *) do_alloca(sizeof(ISC_TEB) * argn);
		tpb = (char *) do_alloca(TPB_MAX_SIZE * argn);

		/* enumerate all the arguments: assume every non-resource argument 
		   specifies modifiers for the link ids that follow it */
		for (i = 0; i < argn; ++i) {
			
			if (Z_TYPE_PP(args[i]) == IS_RESOURCE) {
				
				ZEND_FETCH_RESOURCE2(ib_link[link_cnt], ibase_db_link *, args[i], -1, "InterBase link", le_link, le_plink);
	
				/* copy the most recent modifier string into tbp[] */
				memcpy(&tpb[TPB_MAX_SIZE * link_cnt], last_tpb, TPB_MAX_SIZE);

				/* add a database handle to the TEB with the most recently specified set of modifiers */
				teb[link_cnt].db_ptr = &ib_link[link_cnt]->handle;
				teb[link_cnt].tpb_len = tpb_len;
				teb[link_cnt].tpb_ptr = &tpb[TPB_MAX_SIZE * link_cnt];
				
				++link_cnt;
				
			} else {
				
				tpb_len = 0;

				convert_to_long_ex(args[i]);
				trans_argl = Z_LVAL_PP(args[i]);

				if (trans_argl) {
					last_tpb[tpb_len++] = isc_tpb_version3;

					/* access mode */
					if (PHP_IBASE_READ == (trans_argl & PHP_IBASE_READ)) {
						last_tpb[tpb_len++] = isc_tpb_read;
					} else if (PHP_IBASE_WRITE == (trans_argl & PHP_IBASE_WRITE)) {
						last_tpb[tpb_len++] = isc_tpb_write;
					}

					/* isolation level */
					if (PHP_IBASE_COMMITTED == (trans_argl & PHP_IBASE_COMMITTED)) {
						last_tpb[tpb_len++] = isc_tpb_read_committed;
						if (PHP_IBASE_REC_VERSION == (trans_argl & PHP_IBASE_REC_VERSION)) {
							last_tpb[tpb_len++] = isc_tpb_rec_version;
						} else if (PHP_IBASE_REC_NO_VERSION == (trans_argl & PHP_IBASE_REC_NO_VERSION)) {
							last_tpb[tpb_len++] = isc_tpb_no_rec_version; 
						}	
					} else if (PHP_IBASE_CONSISTENCY == (trans_argl & PHP_IBASE_CONSISTENCY)) {
						last_tpb[tpb_len++] = isc_tpb_consistency;
					} else if (PHP_IBASE_CONCURRENCY == (trans_argl & PHP_IBASE_CONCURRENCY)) {
						last_tpb[tpb_len++] = isc_tpb_concurrency;
					}
					
					/* lock resolution */
					if (PHP_IBASE_NOWAIT == (trans_argl & PHP_IBASE_NOWAIT)) {
						last_tpb[tpb_len++] = isc_tpb_nowait;
					} else if (PHP_IBASE_WAIT == (trans_argl & PHP_IBASE_WAIT)) {
						last_tpb[tpb_len++] = isc_tpb_wait;
					}
				}
			}
		}	
					
		if (link_cnt > 0) {
			result = isc_start_multiple(IB_STATUS, &tr_handle, link_cnt, teb);
		}

		free_alloca(args);
		free_alloca(tpb);
		free_alloca(teb);
	}

	if (link_cnt == 0) {
		link_cnt = 1;
		ZEND_FETCH_RESOURCE2(ib_link[0], ibase_db_link *, NULL, IBG(default_link), "InterBase link", le_link, le_plink);
		result = isc_start_transaction(IB_STATUS, &tr_handle, 1, &ib_link[0]->handle, tpb_len, last_tpb);
	}
	
	/* start the transaction */
	if (result) {
		_php_ibase_error(TSRMLS_C);
		free_alloca(ib_link);
		RETURN_FALSE;
	}

	/* register the transaction in our own data structures */
	ib_trans = (ibase_trans *) safe_emalloc((link_cnt-1), sizeof(ibase_db_link *), sizeof(ibase_trans));
	ib_trans->handle = tr_handle;
	ib_trans->link_cnt = link_cnt;
	ib_trans->affected_rows = 0;
	for (i = 0; i < link_cnt; ++i) {
		ibase_tr_list **l;
		ib_trans->db_link[i] = ib_link[i];
		
		/* the first item in the connection-transaction list is reserved for the default transaction */
		if (ib_link[i]->tr_list == NULL) {
			ib_link[i]->tr_list = (ibase_tr_list *) emalloc(sizeof(ibase_tr_list));
			ib_link[i]->tr_list->trans = NULL;
			ib_link[i]->tr_list->next = NULL;
		}

		/* link the transaction into the connection-transaction list */
		for (l = &ib_link[i]->tr_list; *l != NULL; l = &(*l)->next);
		*l = (ibase_tr_list *) emalloc(sizeof(ibase_tr_list));
		(*l)->trans = ib_trans;
		(*l)->next = NULL;
	}
	free_alloca(ib_link);
	ZEND_REGISTER_RESOURCE(return_value, ib_trans, le_trans);
}
/* }}} */

/* {{{ _php_ibase_def_trans() */
/* open default transaction */
static int _php_ibase_def_trans(ibase_db_link *ib_link, ibase_trans **trans TSRMLS_DC)
{
	if (ib_link == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid database link");
		return FAILURE;
	}

	/* the first item in the connection-transaction list is reserved for the default transaction */
	if (ib_link->tr_list == NULL) {
		ib_link->tr_list = (ibase_tr_list *) emalloc(sizeof(ibase_tr_list));
		ib_link->tr_list->trans = NULL;
		ib_link->tr_list->next = NULL;
	}

	if (*trans == NULL) {
		ibase_trans *tr = ib_link->tr_list->trans;

		if (tr == NULL) {
			tr = (ibase_trans *) emalloc(sizeof(ibase_trans));
			tr->handle = NULL;
			tr->link_cnt = 1;
			tr->affected_rows = 0;
			tr->db_link[0] = ib_link;
			ib_link->tr_list->trans = tr;
		}
		if (tr->handle == NULL) {
			if (isc_start_transaction(IB_STATUS, &tr->handle, 1, &ib_link->handle, 0, NULL)) {
				_php_ibase_error(TSRMLS_C);
				return FAILURE;
			}
		}
		*trans = tr;
	}
	return SUCCESS;
}
/* }}} */

/* {{{ _php_ibase_trans_end() */
static void _php_ibase_trans_end(INTERNAL_FUNCTION_PARAMETERS, int commit)
{
	ibase_trans *trans = NULL;
	int res_id = 0;
	ISC_STATUS result;

	RESET_ERRMSG;

	switch (ZEND_NUM_ARGS()) {

		ibase_db_link *ib_link;
		zval **arg;

		case 0:
			ZEND_FETCH_RESOURCE2(ib_link, ibase_db_link *, NULL, IBG(default_link), "InterBase link", le_link, le_plink);
			if (ib_link->tr_list == NULL || ib_link->tr_list->trans == NULL) {
				/* this link doesn't have a default transaction */
				_php_ibase_module_error("Default link has no default transaction" TSRMLS_CC);
				RETURN_FALSE;
			}
			trans = ib_link->tr_list->trans;
			break;

		case 1: 
			if (zend_get_parameters_ex(1, &arg) == FAILURE) {
				RETURN_FALSE;
			}
			/* one id was passed, could be db or trans id */
			_php_ibase_get_link_trans(INTERNAL_FUNCTION_PARAM_PASSTHRU, arg, &ib_link, &trans);
			if (trans != NULL) {			
				convert_to_long_ex(arg);
				res_id = Z_LVAL_PP(arg);

			} else {
				ZEND_FETCH_RESOURCE2(ib_link, ibase_db_link *, arg, -1, "InterBase link", le_link, le_plink);

				if (ib_link->tr_list == NULL || ib_link->tr_list->trans == NULL) {
					/* this link doesn't have a default transaction */
					_php_ibase_module_error("Link has no default transaction" TSRMLS_CC);
					RETURN_FALSE;
				}
				trans = ib_link->tr_list->trans;
			}
			break;

		default:
			WRONG_PARAM_COUNT;
			break;
	}

	switch (commit) {
		
		default: /* == case ROLLBACK: */
			result = isc_rollback_transaction(IB_STATUS, &trans->handle);
			break;
		case COMMIT:
			result = isc_commit_transaction(IB_STATUS, &trans->handle);
			break;
#ifdef SQL_DIALECT_V6
		case (ROLLBACK | RETAIN):
			result = isc_rollback_retaining(IB_STATUS, &trans->handle);
			break;
#endif
		case (COMMIT | RETAIN):
			result = isc_commit_retaining(IB_STATUS, &trans->handle);
			break;
	}
	
	if (result) {
		_php_ibase_error(TSRMLS_C);
		RETURN_FALSE;
	}

	/* Don't try to destroy implicitly opened transaction from list... */
	if ( (commit & RETAIN) == 0 && res_id != 0) {
		zend_list_delete(res_id);
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool ibase_commit( resource link_identifier )
   Commit transaction */
PHP_FUNCTION(ibase_commit)
{
	_php_ibase_trans_end(INTERNAL_FUNCTION_PARAM_PASSTHRU, COMMIT);
}
/* }}} */

/* {{{ proto bool ibase_rollback( resource link_identifier )
   Rollback transaction */
PHP_FUNCTION(ibase_rollback)
{
	_php_ibase_trans_end(INTERNAL_FUNCTION_PARAM_PASSTHRU, ROLLBACK);
}
/* }}} */

/* {{{ proto bool ibase_commit_ret( resource link_identifier )
   Commit transaction and retain the transaction context */
PHP_FUNCTION(ibase_commit_ret)
{
	_php_ibase_trans_end(INTERNAL_FUNCTION_PARAM_PASSTHRU, COMMIT | RETAIN);
}
/* }}} */

/* {{{ proto bool ibase_rollback_ret( resource link_identifier )
   Rollback transaction and retain the transaction context */
#ifdef SQL_DIALECT_V6

PHP_FUNCTION(ibase_rollback_ret)
{
	_php_ibase_trans_end(INTERNAL_FUNCTION_PARAM_PASSTHRU, ROLLBACK | RETAIN);
}

#endif
/* }}} */

/* {{{ proto mixed ibase_query([resource link_identifier, [ resource link_identifier, ]] string query [, mixed bind_arg [, mixed bind_arg [, ...]]])
   Execute a query */
PHP_FUNCTION(ibase_query)
{
	zval ***args, **bind_args = NULL;
	int i, bind_n = 0, trans_res_id = 0;
	ibase_db_link *ib_link = NULL;
	ibase_trans *trans = NULL;
	ibase_query ib_query = { NULL, NULL, 0, 0 };
	ibase_result *result = NULL;
	char *query;

	RESET_ERRMSG;

	if (ZEND_NUM_ARGS() < 1) {
		WRONG_PARAM_COUNT;
	}

	/* use stack to avoid leaks */
	args = (zval ***) do_alloca(sizeof(zval **) * ZEND_NUM_ARGS());
	if (zend_get_parameters_array_ex(ZEND_NUM_ARGS(), args) == FAILURE) {
		free_alloca(args);
		RETURN_FALSE;
	}
	
	i = 0;
	while (Z_TYPE_PP(args[i++]) != IS_STRING) {
		if (i >= ZEND_NUM_ARGS()) {
			_php_ibase_module_error("Query argument missing" TSRMLS_CC);
			free_alloca(args);
			RETURN_FALSE;
		}
	}

	convert_to_string_ex(args[i-1]);
	query = Z_STRVAL_PP(args[i-1]);

	/* find out if the first one or two arguments refer to either a link id, 
	   a trans id or both */
	switch (i) {
		case 1:

			/* no link ids were passed: if there's no default link, use exec_immediate() with
			   a NULL handle; this will enable the use of CREATE DATABASE statements. */
			if (IBG(default_link) == -1) {
				isc_db_handle db = NULL;
				isc_tr_handle trans = NULL;

				if (isc_dsql_execute_immediate(IB_STATUS, &db, &trans, 0, query, SQL_DIALECT_CURRENT, NULL)) {
					_php_ibase_error(TSRMLS_C);
					free_alloca(args);
					RETURN_FALSE;
				}
				
				/* has a new database been created ? */
				if (db != NULL) {

					if ((IBG(max_links) != -1) && (IBG(num_links) >= IBG(max_links))) {					

						/* too many links already ? => close it up immediately */
						if (isc_detach_database(IB_STATUS, &db)) {
							_php_ibase_error(TSRMLS_C);
							free_alloca(args);
							RETURN_FALSE;
						}
					} else {
							
						/* register the link as a resource; unfortunately, we cannot register 
						   it in the hash table, because we don't know the connection params */
						ib_link = (ibase_db_link *) emalloc(sizeof(ibase_db_link));
						ib_link->handle = db;
						ib_link->dialect = SQL_DIALECT_CURRENT;
						ib_link->tr_list = NULL;
						
						ZEND_REGISTER_RESOURCE(return_value, ib_link, le_link);
						zend_list_addref(Z_LVAL_P(return_value));
						IBG(default_link) = Z_LVAL_P(return_value);
						IBG(num_links)++;

						free_alloca(args);
						return;
					}
				}
				RETURN_TRUE;
			}					
				
			ZEND_FETCH_RESOURCE2(ib_link, ibase_db_link *, NULL, IBG(default_link), "InterBase link", le_link, le_plink);
			break;			
		case 2:
			/* one id was passed, could be db or trans id */
			_php_ibase_get_link_trans(INTERNAL_FUNCTION_PARAM_PASSTHRU, args[0], &ib_link, &trans);

			if (trans != NULL) {
				/* argument was a trans id */
				convert_to_long_ex(args[0]);
				trans_res_id = Z_LVAL_PP(args[0]);
			}
			break;	
		case 3:
			/* two ids were passed, first should be link and second should be trans; */
			ZEND_FETCH_RESOURCE2(ib_link, ibase_db_link*, args[0], -1, "InterBase link", le_link, le_plink);
			ZEND_FETCH_RESOURCE(trans, ibase_trans*, args[1], -1, "InterBase transaction", le_trans);

			convert_to_long_ex(args[1]);
			trans_res_id = Z_LVAL_PP(args[1]);

			break;
		default:
			/* more than two arguments preceed the SQL string */
			_php_ibase_module_error("Invalid arguments" TSRMLS_CC);
			free_alloca(args);
			RETURN_FALSE;
	}
			
	if (ZEND_NUM_ARGS() > i) { /* have variables to bind */
		/* Using variables in a query without preparing it can be
		   useful, because it allows you to use (among other things) 
		   SQL-queries as consts and the passing of string arguments 
		   without the horror of [un]slashing them. */
		bind_n = ZEND_NUM_ARGS() - i;
		bind_args = args[i];
	}
	
	/* open default transaction */
	if (ib_link == NULL || _php_ibase_def_trans(ib_link, &trans TSRMLS_CC) == FAILURE) {
		free_alloca(args);
		RETURN_FALSE;
	}

	if (_php_ibase_alloc_query(&ib_query, ib_link, trans, query, ib_link->dialect, trans_res_id TSRMLS_CC) == FAILURE) {
		free_alloca(args);
		RETURN_FALSE;
	}

	if (_php_ibase_exec(INTERNAL_FUNCTION_PARAM_PASSTHRU, &result, &ib_query, bind_n, bind_args) == FAILURE) {
		_php_ibase_free_query(&ib_query TSRMLS_CC);
		free_alloca(args);
		RETURN_FALSE;
	}

	free_alloca(args);
	
	if (result != NULL) { /* statement returns a result */
		result->type = QUERY_RESULT;	

		/* EXECUTE PROCEDURE returns only one row => statement can be released immediately */
		if (ib_query.statement_type != isc_info_sql_stmt_exec_procedure) {
			ib_query.stmt = NULL; /* keep stmt when free query */
		}
		ZEND_REGISTER_RESOURCE(return_value, result, le_result);
	}
	_php_ibase_free_query(&ib_query TSRMLS_CC);
}
/* }}} */

/* {{{ proto int ibase_affected_rows( [ resource link_identifier ] )
   Returns the number of rows affected by the previous INSERT, UPDATE or DELETE statement */
PHP_FUNCTION(ibase_affected_rows)
{
	ibase_trans *trans = NULL;

	RESET_ERRMSG;

	switch (ZEND_NUM_ARGS()) {

		ibase_db_link *ib_link;
		zval **arg;

		case 0:
			ZEND_FETCH_RESOURCE2(ib_link, ibase_db_link *, NULL, IBG(default_link), "InterBase link", le_link, le_plink);
			if (ib_link->tr_list == NULL || ib_link->tr_list->trans == NULL) {
				RETURN_FALSE;
			}
			trans = ib_link->tr_list->trans;
			break;

		case 1: 
			if (zend_get_parameters_ex(1, &arg) == FAILURE) {
				RETURN_FALSE;
			}
			/* one id was passed, could be db or trans id */
			_php_ibase_get_link_trans(INTERNAL_FUNCTION_PARAM_PASSTHRU, arg, &ib_link, &trans);
			if (trans == NULL) {			
				ZEND_FETCH_RESOURCE2(ib_link, ibase_db_link *, arg, -1, "InterBase link", le_link, le_plink);

				if (ib_link->tr_list == NULL || ib_link->tr_list->trans == NULL) {
					RETURN_FALSE;
				}
				trans = ib_link->tr_list->trans;
			}
			break;

		default:
			WRONG_PARAM_COUNT;
			break;
	}
	RETURN_LONG(trans->affected_rows);
}
/* }}} */

/* {{{ ibase_num_rows( resource result_identifier ) */
#if abies_0
PHP_FUNCTION(ibase_num_rows) 
{
	/**
	 * PLEASE READ THIS FIRST before adding this function to the main PHP
	 * documentation:
	 * 
	 * As this function relies on the InterBase API function isc_dsql_sql_info()
	 * which has a couple of limitations (which I hope will be fixed in future 
	 * releases of Firebird), this function is fairly useless. I'm leaving it
	 * in place for people who can live with the limitations, which I only 
	 * found out about after I had implemented it anyway.
	 *
	 * Currently, there's no way to determine how many rows can be fetched from
	 * a cursor. The only number that _can_ be determined is the number of rows
	 * that have already been pre-fetched by the client library. 
	 * This implies the following:
	 * - num_rows() always returns zero before the first fetch;
	 * - num_rows() for SELECT ... FOR UPDATE is broken -> never returns a
	 *   higher number than the number of records fetched so far (no pre-fetch);
	 * - the result of num_rows() for other statements is merely a lower bound 
	 * on the number of records => calling ibase_num_rows() again after a couple
	 * of fetches will most likely return a new (higher) figure for large result 
	 * sets.
	 *
	 * 12-aug-2003 Ard Biesheuvel
	 */
	
	zval **result_arg;
	ibase_result *ib_result;
	static char info_count[] = {isc_info_sql_records};
	char result[64];

	RESET_ERRMSG;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &result_arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ib_result, ibase_result *, result_arg, -1, "InterBase result", le_result);
	
	if (isc_dsql_sql_info(IB_STATUS, &ib_result->stmt, sizeof(info_count), info_count, sizeof(result), result)) {
		_php_ibase_error(TSRMLS_C);
		RETURN_FALSE;
	}
	
	if (result[0] == isc_info_sql_records) {
		unsigned i = 3, result_size = isc_vax_integer(&result[1],2);

		while (result[i] != isc_info_end && i < result_size) {
			short len = (short)isc_vax_integer(&result[i+1],2);
			if (result[i] == isc_info_req_select_count) {
				RETURN_LONG(isc_vax_integer(&result[i+3],len));
			}
			i += len+3;
		}
	}					
}
#endif
/* }}} */

/* {{{ _php_ibase_var_zval() */
static int _php_ibase_var_zval(zval *val, void *data, int type, int len, int scale, int flag TSRMLS_DC)
{
#ifdef SQL_INT64
	static ISC_INT64 const scales[] = { 1, 10, 100, 1000, 10000, 100000, 1000000, 100000000, 1000000000, 
		1000000000, LL_LIT(10000000000),LL_LIT(100000000000),LL_LIT(10000000000000),LL_LIT(100000000000000),
		LL_LIT(1000000000000000),LL_LIT(1000000000000000),LL_LIT(1000000000000000000) };
#else 
	static long const scales[] = { 1, 10, 100, 1000, 10000, 100000, 1000000, 100000000, 1000000000, 1000000000 };
#endif		

	switch (type & ~1) {
		unsigned short l;
		long n;
		char string_data[255];

		case SQL_VARYING:
			len = ((IBVARY *) data)->vary_length;
			data = ((IBVARY *) data)->vary_string;
			/* no break */
		case SQL_TEXT:
			if (PG(magic_quotes_runtime)) {
				Z_STRVAL_P(val) = php_addslashes(data, len, &Z_STRLEN_P(val), 0 TSRMLS_CC);
				Z_TYPE_P(val) = IS_STRING;
			} else {
				ZVAL_STRINGL(val,(char *) data,len,1);
			}
			break;
		case SQL_SHORT:
			n = *(short *) data;
			goto _sql_long;
#ifdef SQL_INT64
		case SQL_INT64:
#if (SIZEOF_LONG >= 8)
			n = *(long *) data;
			goto _sql_long;
#else
			if (scale == 0) {
				l = sprintf(string_data, "%" LL_MASK "d", *(ISC_INT64 *) data);
				ZVAL_STRINGL(val,string_data,l,1);
			} else {
				ISC_INT64 n = *(ISC_INT64 *) data, f = scales[-scale];

				if (n >= 0) {
					l = sprintf(string_data, "%" LL_MASK "d.%0*" LL_MASK "d", n / f, -scale, n % f);
				} else if (n < -f) {
					l = sprintf(string_data, "%" LL_MASK "d.%0*" LL_MASK "d", n / f, -scale, -n % f);				
 				} else {
					l = sprintf(string_data, "-0.%0*" LL_MASK "d", -scale, -n % f);
				}
				ZVAL_STRINGL(val,string_data,l,1);
			}
			break;
#endif
#endif
		case SQL_LONG:
			n = *(ISC_LONG *) data; 
		_sql_long:
			if (scale == 0) {
				ZVAL_LONG(val,n);
			} else {
				long f = (long) scales[-scale];
				
				if (n >= 0) {
					l = sprintf(string_data, "%ld.%0*ld", n / f, -scale,  n % f);
				} else if (n < -f) {
					l = sprintf(string_data, "%ld.%0*ld", n / f, -scale,  -n % f);
				} else {
					l = sprintf(string_data, "-0.%0*ld", -scale, -n % f);
				}
				ZVAL_STRINGL(val,string_data,l,1);
			}
			break;
		case SQL_FLOAT:
			ZVAL_DOUBLE(val, *(float *) data);
			break;
		case SQL_DOUBLE:
			ZVAL_DOUBLE(val, *(double *) data);
			break;
		default: /* == any date/time type */
		{
			struct tm t;
			char *format = NULL;

#ifndef SQL_TIMESTAMP
			isc_decode_date((ISC_QUAD *) data, &t);
			format = IBG(timestampformat);
#else
			switch (type & ~1) {
				default: /* == case SQL_TIMESTAMP: */
					isc_decode_timestamp((ISC_TIMESTAMP *) data, &t);
					format = IBG(timestampformat);
					break;
				case SQL_TYPE_DATE:
					isc_decode_sql_date((ISC_DATE *) data, &t);
					format = IBG(dateformat);
					break;
				case SQL_TYPE_TIME:
					isc_decode_sql_time((ISC_TIME *) data, &t);
					format = IBG(timeformat);
					break;
			}
#endif
			/*
			  XXX - Might have to remove this later - seems that isc_decode_date()
			   always sets tm_isdst to 0, sometimes incorrectly (InterBase 6 bug?)
			*/
			t.tm_isdst = -1;
#if HAVE_TM_ZONE
			t.tm_zone = tzname[0];
#endif
			if (flag & PHP_IBASE_UNIXTIME) {
				ZVAL_LONG(val, mktime(&t));
			} else {
#if HAVE_STRFTIME
				l = strftime(string_data, sizeof(string_data), format, &t);
#else
				switch (type & ~1) {
					default:
						l = sprintf(string_data, "%02d/%02d/%4d %02d:%02d:%02d", t.tm_mon+1, t.tm_mday, t.tm_year + 1900, t.tm_hour, t.tm_min, t.tm_sec);
						break;
#ifdef SQL_TIMESTAMP
					case SQL_TYPE_DATE:
						l = sprintf(string_data, "%02d/%02d/%4d", t.tm_mon + 1, t.tm_mday, t.tm_year + 1900);
						break;
					case SQL_TYPE_TIME:
						l = sprintf(string_data, "%02d:%02d:%02d", t.tm_hour, t.tm_min, t.tm_sec);
						break;
#endif
				}
#endif
				ZVAL_STRINGL(val,string_data,l,1);
				break;
			}
		}
	} /* switch (type) */
	return SUCCESS;
}
/* }}}	*/

/* {{{ _php_ibase_arr_zval() */
/* create multidimension array - recursion function
 * (*datap) argument changed 
 */
static int _php_ibase_arr_zval(zval *ar_zval, char *data, unsigned long data_size, ibase_array *ib_array, int dim, int flag TSRMLS_DC)
{
	int 
		u_bound = ib_array->ar_desc.array_desc_bounds[dim].array_bound_upper,
		l_bound = ib_array->ar_desc.array_desc_bounds[dim].array_bound_lower,
		dim_len = 1 + u_bound - l_bound;
	unsigned short i;
		
	if (dim < ib_array->ar_desc.array_desc_dimensions) { /* array again */
		unsigned long slice_size = data_size / dim_len;
		
		array_init(ar_zval);

		for (i = 0; i < dim_len; ++i) {
			zval *slice_zval;
			ALLOC_INIT_ZVAL(slice_zval);

			/* recursion here */
			if (_php_ibase_arr_zval(slice_zval, data, slice_size, ib_array, dim + 1, flag TSRMLS_CC) == FAILURE) {
				return FAILURE;
			}
			data += slice_size;
			
			add_index_zval(ar_zval,l_bound+i,slice_zval);
		}
	} else { /* data at last */
		
		if (_php_ibase_var_zval(ar_zval, data, 
								ib_array->el_type,
								ib_array->ar_desc.array_desc_length,
								ib_array->ar_desc.array_desc_scale,
								flag TSRMLS_CC) == FAILURE) {
			return FAILURE;
		}
		
		/* fix for peculiar handling of VARCHAR arrays;
		   truncate the field to the cstring length */
		if (ib_array->ar_desc.array_desc_dtype == blr_varying ||
			ib_array->ar_desc.array_desc_dtype == blr_varying2) {
				
			Z_STRLEN_P(ar_zval) = strlen(Z_STRVAL_P(ar_zval));
		}
	}
	return SUCCESS;
}
/* }}} */


/* {{{ _php_ibase_blob_get */
static int _php_ibase_blob_get(zval *return_value, ibase_blob *ib_blob, unsigned long max_len TSRMLS_DC)
{
	if (ib_blob->bl_qd.gds_quad_high || ib_blob->bl_qd.gds_quad_low) { /*not null ?*/
		
		ISC_STATUS stat;
		char *bl_data;
		unsigned long cur_len;
		unsigned short seg_len;
	
		bl_data = emalloc(max_len + 1);
	
		for (cur_len = stat = 0; (stat == 0 || stat == isc_segment) && cur_len < max_len; cur_len += seg_len) {
	
			unsigned short chunk_size = (max_len-cur_len) > USHRT_MAX ? USHRT_MAX : (unsigned short)(max_len-cur_len);
	
			stat = isc_get_segment(IB_STATUS, &ib_blob->bl_handle, &seg_len, chunk_size, &bl_data[cur_len]); 
		}
	
		bl_data[cur_len] = '\0';
		if (IB_STATUS[0] == 1 && (stat != 0 && stat != isc_segstr_eof && stat != isc_segment)) {
			efree(bl_data);
			_php_ibase_error(TSRMLS_C);
			return FAILURE;
		}
		RETVAL_STRINGL(bl_data, cur_len, 0);
	} else { /* null blob */
		RETVAL_STRING("", 1); /* empty string */
	}
	return SUCCESS;
}

/* }}} */

/* {{{ _php_ibase_fetch_hash() */
static void _php_ibase_fetch_hash(INTERNAL_FUNCTION_PARAMETERS, int fetch_type)
{
	zval **result_arg, **flag_arg;
	long i, flag = 0;
	ibase_result *ib_result;
	XSQLVAR *var;
	
	RESET_ERRMSG;
	
	switch (ZEND_NUM_ARGS()) {
		case 1:
			if (ZEND_NUM_ARGS() == 1 && zend_get_parameters_ex(1, &result_arg) == FAILURE) {
				RETURN_FALSE;
			}
			break;
		case 2:
			if (ZEND_NUM_ARGS() == 2 && zend_get_parameters_ex(2, &result_arg, &flag_arg) == FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long_ex(flag_arg);
			flag = Z_LVAL_PP(flag_arg);
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}

	ZEND_FETCH_RESOURCE(ib_result, ibase_result *, result_arg, -1, "InterBase result", le_result);

	if (ib_result->out_sqlda == NULL || !ib_result->has_more_rows) {
		RETURN_FALSE;
	}

	if (ib_result->statement_type != isc_info_sql_stmt_exec_procedure) {

		if (isc_dsql_fetch(IB_STATUS, &ib_result->stmt, 1, ib_result->out_sqlda)) {
	
			ib_result->has_more_rows = 0;
			if (IB_STATUS[0] && IB_STATUS[1]) { /* error in fetch */
				_php_ibase_error(TSRMLS_C);
			}
			RETURN_FALSE;
		}
	} else {
		ib_result->has_more_rows = 0;
	}	
	
	array_init(return_value);
	
	var = ib_result->out_sqlda->sqlvar;
	for (i = 0; i < ib_result->out_sqlda->sqld; i++, var++) {
		if (((var->sqltype & 1) == 0) || *var->sqlind != -1) {
			zval *result;
			ALLOC_INIT_ZVAL(result);

			switch (var->sqltype & ~1) {

				default:
					_php_ibase_var_zval(result, var->sqldata, var->sqltype, var->sqllen, var->sqlscale, flag TSRMLS_CC);
					break;
				case SQL_BLOB:
					if (flag & PHP_IBASE_FETCH_BLOBS) { /* fetch blob contents into hash */
	
						ibase_blob blob_handle;
						unsigned long max_len = 0;
						static char bl_items[] = {isc_info_blob_total_length};
						char bl_info[20];
						unsigned short i;
	
						blob_handle.bl_handle = NULL;
						blob_handle.bl_qd = *(ISC_QUAD *) var->sqldata;
			
						if (isc_open_blob(IB_STATUS, &ib_result->link->handle, &ib_result->trans->handle, &blob_handle.bl_handle, &blob_handle.bl_qd)) {
							_php_ibase_error(TSRMLS_C);
							goto _php_ibase_fetch_error;
						}
						
						if (isc_blob_info(IB_STATUS, &blob_handle.bl_handle, sizeof(bl_items), bl_items, sizeof(bl_info), bl_info)) {
							_php_ibase_error(TSRMLS_C);
							goto _php_ibase_fetch_error;
						}
						
						/* find total length of blob's data */
						for (i = 0; i < sizeof(bl_info); ) {
							unsigned short item_len;
							char item = bl_info[i++];
	
							if (item == isc_info_end || item == isc_info_truncated || 
								item == isc_info_error || i >= sizeof(bl_info)) {

								_php_ibase_module_error("Could not determine BLOB size (internal error)" TSRMLS_CC);
								goto _php_ibase_fetch_error;
							}								

							item_len = (unsigned short) isc_vax_integer(&bl_info[i], 2);

							if (item == isc_info_blob_total_length) {
								max_len = isc_vax_integer(&bl_info[i+2], item_len);
								break;
							}
							i += item_len+2;
						}
						
						if (max_len == 0) {
							ZVAL_STRING(result, "", 1);
						} else if (_php_ibase_blob_get(result, &blob_handle, max_len TSRMLS_CC) != SUCCESS) {
							goto _php_ibase_fetch_error;
						}
						
						if (isc_close_blob(IB_STATUS, &blob_handle.bl_handle)) {
							_php_ibase_error(TSRMLS_C);
							goto _php_ibase_fetch_error;
						}
	
					} else { /* blob id only */
						ISC_QUAD bl_qd = *(ISC_QUAD *) var->sqldata;
						ZVAL_STRINGL(result,_php_ibase_quad_to_string(bl_qd), BLOB_ID_LEN, 0);
					}
					break;
				case SQL_ARRAY:
					if (flag & PHP_IBASE_FETCH_ARRAYS) { /* array can be *huge* so only fetch if asked */
						ISC_QUAD ar_qd = *(ISC_QUAD *) var->sqldata;
						ibase_array *ib_array = &ib_result->out_array[i];
						void *ar_data = emalloc(ib_array->ar_size);
						
						if (isc_array_get_slice(IB_STATUS, &ib_result->link->handle, &ib_result->trans->handle, &ar_qd, &ib_array->ar_desc, ar_data, &ib_array->ar_size)) {
							_php_ibase_error(TSRMLS_C);
							efree(ar_data);
							goto _php_ibase_fetch_error;
						}

						if (_php_ibase_arr_zval(result, ar_data, ib_array->ar_size, ib_array, 0, flag TSRMLS_CC) == FAILURE) {
							efree(ar_data);
							goto _php_ibase_fetch_error;
						}
						efree(ar_data);

					} else { /* blob id only */
						ISC_QUAD ar_qd = *(ISC_QUAD *) var->sqldata;
						ZVAL_STRINGL(result,_php_ibase_quad_to_string(ar_qd), BLOB_ID_LEN, 0);
					}
					break;
				_php_ibase_fetch_error:
					zval_dtor(result);
					FREE_ZVAL(result);
					RETURN_FALSE;
			} /* switch */

			if (fetch_type & FETCH_ROW) {
				add_index_zval(return_value, i, result);
			} else {
				add_assoc_zval(return_value, var->aliasname, result);
			}
		} else {
			if (fetch_type & FETCH_ROW) {
				add_index_null(return_value, i);
			} else {
				add_assoc_null(return_value, var->aliasname);
			}
		}
	} /* for field */
}
/* }}} */

/* {{{ proto array ibase_fetch_row(resource result [, int fetch_flags])
   Fetch a row  from the results of a query */
PHP_FUNCTION(ibase_fetch_row)
{
	_php_ibase_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, FETCH_ROW);
}
/* }}} */

/* {{{ proto array ibase_fetch_assoc(resource result [, int fetch_flags])
   Fetch a row  from the results of a query */
PHP_FUNCTION(ibase_fetch_assoc)
{
	_php_ibase_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, FETCH_ARRAY);
}
/* }}} */

/* {{{ proto object ibase_fetch_object(resource result [, int fetch_flags])
   Fetch a object from the results of a query */
PHP_FUNCTION(ibase_fetch_object)
{
	_php_ibase_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, FETCH_ARRAY);

	if (Z_TYPE_P(return_value) == IS_ARRAY) {
		object_and_properties_init(return_value, ZEND_STANDARD_CLASS_DEF_PTR, Z_ARRVAL_P(return_value));
	}
}
/* }}} */


/* {{{ proto bool ibase_name_result(resource result, string name)
   Assign a name to a result for use with ... WHERE CURRENT OF <name> statements */
PHP_FUNCTION(ibase_name_result)
{
	zval **result_arg, **name_arg;
	ibase_result *ib_result;

	RESET_ERRMSG;
	
	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &result_arg, &name_arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ib_result, ibase_result *, result_arg, -1, "InterBase result", le_result);
	convert_to_string_ex(name_arg);
	
	if (isc_dsql_set_cursor_name(IB_STATUS, &ib_result->stmt, Z_STRVAL_PP(name_arg), 0)) {
		_php_ibase_error(TSRMLS_C);
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */


/* {{{ proto bool ibase_free_result(resource result)
   Free the memory used by a result */
PHP_FUNCTION(ibase_free_result)
{
	zval **result_arg;
	ibase_result *ib_result;

	RESET_ERRMSG;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &result_arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ib_result, ibase_result *, result_arg, -1, "InterBase result", le_result);
	zend_list_delete(Z_LVAL_PP(result_arg));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto resource ibase_prepare([resource link_identifier, ] string query)
   Prepare a query for later execution */
PHP_FUNCTION(ibase_prepare)
{
	zval **link_arg, **trans_arg, **query_arg;
	ibase_db_link *ib_link;
	ibase_trans *trans = NULL;
	int trans_res_id = 0;
	ibase_query *ib_query;
	char *query;

	RESET_ERRMSG;

	switch (ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &query_arg) == FAILURE) {
				RETURN_FALSE;
			}
			ZEND_FETCH_RESOURCE2(ib_link, ibase_db_link *, NULL, IBG(default_link), "InterBase link", le_link, le_plink);
			break;
		case 2:
			if (zend_get_parameters_ex(2, &link_arg, &query_arg) == FAILURE) {
				RETURN_FALSE;
			}
			_php_ibase_get_link_trans(INTERNAL_FUNCTION_PARAM_PASSTHRU, link_arg, &ib_link, &trans);
			
			if (trans != NULL) {
				convert_to_long_ex(link_arg);
				trans_res_id = Z_LVAL_PP(link_arg);
			}
				
			break;
		case 3:
			if (zend_get_parameters_ex(3, &link_arg, &trans_arg, &query_arg) == FAILURE) {
				RETURN_FALSE;
			}
			ZEND_FETCH_RESOURCE2(ib_link, ibase_db_link*, link_arg, -1, "InterBase link", le_link, le_plink);
			ZEND_FETCH_RESOURCE(trans, ibase_trans*, trans_arg, -1, "InterBase transaction", le_trans);
			
			convert_to_long_ex(trans_arg);
			trans_res_id = Z_LVAL_PP(trans_arg);
			
			break;			
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	convert_to_string_ex(query_arg);
	query = Z_STRVAL_PP(query_arg);

	/* open default transaction */
	if (_php_ibase_def_trans(ib_link, &trans TSRMLS_CC) == FAILURE) {
		RETURN_FALSE;
	}
	
	ib_query = (ibase_query *) emalloc(sizeof(ibase_query));

	if (_php_ibase_alloc_query(ib_query, ib_link, trans, query, ib_link->dialect, trans_res_id TSRMLS_CC) == FAILURE) {
		efree(ib_query);
		RETURN_FALSE;
	}
	ZEND_REGISTER_RESOURCE(return_value, ib_query, le_query);
}
/* }}} */

/* {{{ proto mixed ibase_execute(resource query [, mixed bind_arg [, mixed bind_arg [, ...]]])
   Execute a previously prepared query */
PHP_FUNCTION(ibase_execute)
{
	zval ***args, **bind_args = NULL;
	ibase_query *ib_query;
	ibase_result *result = NULL;

	RESET_ERRMSG;

	if (ZEND_NUM_ARGS() < 1) {
		WRONG_PARAM_COUNT;
	}

	/* use stack to avoid leaks */
	args = (zval ***) do_alloca(ZEND_NUM_ARGS() * sizeof(zval **));
	if (zend_get_parameters_array_ex(ZEND_NUM_ARGS(), args) == FAILURE) {
		free_alloca(args);
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE(ib_query, ibase_query *, args[0], -1, "InterBase query", le_query);

	if (ZEND_NUM_ARGS() > 1) { /* have variables to bind */
		bind_args = args[1];
	}
	
	/* Have we used this cursor before and it's still open (exec proc has no cursor) ? */
	if (ib_query->result_res_id != 0 && ib_query->statement_type != isc_info_sql_stmt_exec_procedure) {
		IBDEBUG("Implicitly closing a cursor");
		if (isc_dsql_free_statement(IB_STATUS, &ib_query->stmt, DSQL_close)) {
			_php_ibase_error(TSRMLS_C);
		}
		/* invalidate previous results returned by this query (not necessary for exec proc) */
		zend_list_delete(ib_query->result_res_id);	
	}
		
	if (_php_ibase_exec(INTERNAL_FUNCTION_PARAM_PASSTHRU, &result, ib_query, ZEND_NUM_ARGS() - 1, bind_args) == FAILURE) {
		free_alloca(args);
		RETURN_FALSE;
	}
	
	/* free the query if trans handle was released */
	if (ib_query->trans->handle == NULL) {
		zend_list_delete(Z_LVAL_PP(args[0]));
	}

	free_alloca(args);
	
	if (result != NULL) {
		result->type = EXECUTE_RESULT;
		if (ib_query->statement_type == isc_info_sql_stmt_exec_procedure) {
			result->stmt = NULL;
		}
		ib_query->result_res_id = zend_list_insert(result, le_result);
		RETURN_RESOURCE(ib_query->result_res_id);
	}
}
/* }}} */

/* {{{ proto bool ibase_free_query(resource query)
   Free memory used by a query */
PHP_FUNCTION(ibase_free_query)
{
	zval **query_arg;
	ibase_query *ib_query;

	RESET_ERRMSG;

	if (ZEND_NUM_ARGS()!=1 || zend_get_parameters_ex(1, &query_arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ib_query, ibase_query *, query_arg, -1, "InterBase query", le_query);
	zend_list_delete(Z_LVAL_PP(query_arg));
	RETURN_TRUE;
}
/* }}} */

#if HAVE_STRFTIME
/* {{{ proto bool ibase_timefmt(string format)
   Sets the format of timestamp, date and time columns returned from queries */
PHP_FUNCTION(ibase_timefmt)
{
	zval ***args;
	char *fmt = NULL;
	int type = PHP_IBASE_TIMESTAMP;
	
	RESET_ERRMSG; /* ??? */

	if (ZEND_NUM_ARGS() < 1 || ZEND_NUM_ARGS() > 2) {
		WRONG_PARAM_COUNT;
	}
	
	args = (zval ***) safe_emalloc(sizeof(zval **), ZEND_NUM_ARGS(), 0);
	if (zend_get_parameters_array_ex(ZEND_NUM_ARGS(), args) == FAILURE) {
		efree(args);
		RETURN_FALSE;
	}

	switch (ZEND_NUM_ARGS()) {
		case 2:
			convert_to_long_ex(args[1]);
			type = Z_LVAL_PP(args[1]);
		case 1:
			convert_to_string_ex(args[0]);
			fmt = Z_STRVAL_PP(args[0]);
	}

	switch (type) {
		case PHP_IBASE_TIMESTAMP:
			if (IBG(timestampformat)) {
				DL_FREE(IBG(timestampformat));
			}
			IBG(timestampformat) = DL_STRDUP(fmt);
			break;
		case PHP_IBASE_DATE:
			if (IBG(dateformat)) {
				DL_FREE(IBG(dateformat));
			}
			IBG(dateformat) = DL_STRDUP(fmt);
			break;
		case PHP_IBASE_TIME:
			if (IBG(timeformat)) {
				DL_FREE(IBG(timeformat));
			}
			IBG(timeformat) = DL_STRDUP(fmt);
			break;
	}

	efree(args);
	RETURN_TRUE;
}
/* }}} */
#endif

/* {{{ proto int ibase_num_fields(resource query_result)
   Get the number of fields in result */
PHP_FUNCTION(ibase_num_fields)
{
	zval **result;
	int type;
	XSQLDA *sqlda;

	RESET_ERRMSG;

	if (ZEND_NUM_ARGS()!=1 || zend_get_parameters_ex(1, &result)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	zend_list_find(Z_LVAL_PP(result), &type);
	
	if (type == le_query) {
		ibase_query *ib_query;

		ZEND_FETCH_RESOURCE(ib_query, ibase_query *, result, -1, "InterBase query", le_query);
		sqlda = ib_query->out_sqlda;
	} else {
		ibase_result *ib_result;
		
		ZEND_FETCH_RESOURCE(ib_result, ibase_result *, result, -1, "InterBase result", le_result);
		sqlda = ib_result->out_sqlda;
	}					

	if (sqlda == NULL) {
		RETURN_LONG(0);
	} else {
		RETURN_LONG(sqlda->sqld);
	}
}
/* }}} */

/* {{{ _php_ibase_field_info() */
static void _php_ibase_field_info(zval *return_value, XSQLVAR *var)
{
	unsigned short len;
	char buf[16], *s = buf;
	
	array_init(return_value);

	add_index_stringl(return_value, 0, var->sqlname, var->sqlname_length, 1);
	add_assoc_stringl(return_value, "name", var->sqlname, var->sqlname_length, 1);

	add_index_stringl(return_value, 1, var->aliasname, var->aliasname_length, 1);
	add_assoc_stringl(return_value, "alias", var->aliasname, var->aliasname_length, 1);

	add_index_stringl(return_value, 2, var->relname, var->relname_length, 1);
	add_assoc_stringl(return_value, "relation", var->relname, var->relname_length, 1);

	len = sprintf(buf, "%d", var->sqllen);
	add_index_stringl(return_value, 3, buf, len, 1);
	add_assoc_stringl(return_value, "length", buf, len, 1);

	if (var->sqlscale < 0) {
		unsigned short precision;

		switch (var->sqltype & ~1) {

			case SQL_SHORT:
				precision = 4;
				break;
			case SQL_LONG:
				precision = 9;
				break;
#ifdef SQL_INT64
			case SQL_INT64:     
				precision = 18;
				break;
#endif
		}
		len = sprintf(buf, "NUMERIC(%d,%d)", precision, -var->sqlscale);
		add_index_stringl(return_value, 4, s, len, 1);
		add_assoc_stringl(return_value, "type", s, len, 1);
	} else {
		switch (var->sqltype & ~1) {
			case SQL_TEXT:
			    s = "CHAR"; 
			    break;
			case SQL_VARYING:
				s = "VARCHAR"; 
				break;
			case SQL_SHORT:
				s = "SMALLINT"; 
				break;
			case SQL_LONG:
				s = "INTEGER"; 
				break;
			case SQL_FLOAT:	    
				s = "FLOAT"; break;
			case SQL_DOUBLE:    
			case SQL_D_FLOAT:   
				s = "DOUBLE PRECISION"; break;
#ifdef SQL_INT64
			case SQL_INT64:     
				s = "BIGINT"; 
				break;
#endif
#ifdef SQL_TIMESTAMP
			case SQL_TIMESTAMP:	
				s = "TIMESTAMP"; 
				break;
			case SQL_TYPE_DATE:
				s = "DATE";
				break;
			case SQL_TYPE_TIME:
				s = "TIME"; 
				break;
#else
			case SQL_DATE:
				s = "DATE"; 
				break;
#endif
			case SQL_BLOB:
				s = "BLOB"; 
				break;
			case SQL_ARRAY:
				s = "ARRAY";
				break;
				/* FIXME: provide more detailed information about the field type, field size
				 * and array dimensions */
			case SQL_QUAD:
				s = "QUAD";
				break;
		}
		add_index_string(return_value, 4, s, 1);
		add_assoc_string(return_value, "type", s, 1);
	}
}
/* }}} */

/* {{{ proto array ibase_field_info(resource query_result, int field_number)
   Get information about a field */
PHP_FUNCTION(ibase_field_info)
{
	zval **result_arg, **field_arg;
	int type;
	XSQLDA *sqlda;

	RESET_ERRMSG;

	if (ZEND_NUM_ARGS()!=2 || zend_get_parameters_ex(2, &result_arg, &field_arg)==FAILURE) {
		WRONG_PARAM_COUNT;
	}

	zend_list_find(Z_LVAL_PP(result_arg), &type);
	
	if (type == le_query) {
		ibase_query *ib_query;

		ZEND_FETCH_RESOURCE(ib_query, ibase_query *, result_arg, -1, "InterBase query", le_query);
		sqlda = ib_query->out_sqlda;
	} else {
		ibase_result *ib_result;
		
		ZEND_FETCH_RESOURCE(ib_result, ibase_result *, result_arg, -1, "InterBase result", le_result);
		sqlda = ib_result->out_sqlda;
	}					

	if (sqlda == NULL) {
		_php_ibase_module_error("Trying to get field info from a non-select query" TSRMLS_CC);
		RETURN_FALSE;
	}

	convert_to_long_ex(field_arg);

	if (Z_LVAL_PP(field_arg) < 0 || Z_LVAL_PP(field_arg) >= sqlda->sqld) {
		RETURN_FALSE;
	}
	_php_ibase_field_info(return_value,sqlda->sqlvar + Z_LVAL_PP(field_arg));
}
/* }}} */

/* {{{ proto int ibase_num_params(resource query)
   Get the number of params in a prepared query */
PHP_FUNCTION(ibase_num_params)
{
	zval **result;
	ibase_query *ib_query;

	RESET_ERRMSG;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &result) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(ib_query, ibase_query *, result, -1, "InterBase query", le_query);

	if (ib_query->in_sqlda == NULL) {
		RETURN_LONG(0);
	} else {
		RETURN_LONG(ib_query->in_sqlda->sqld);
	}
}
/* }}} */

/* {{{ proto array ibase_param_info(resource query, int field_number)
   Get information about a parameter */
PHP_FUNCTION(ibase_param_info)
{
	zval **result_arg, **field_arg;
	ibase_query *ib_query;

	RESET_ERRMSG;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &result_arg, &field_arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ib_query, ibase_query *, result_arg, -1, "InterBase query", le_query);

	if (ib_query->in_sqlda == NULL) {
		RETURN_FALSE;
	}

	convert_to_long_ex(field_arg);

	if (Z_LVAL_PP(field_arg) < 0 || Z_LVAL_PP(field_arg) >= ib_query->in_sqlda->sqld) {
		RETURN_FALSE;
	}
	
	_php_ibase_field_info(return_value,ib_query->in_sqlda->sqlvar + Z_LVAL_PP(field_arg));
}
/* }}} */

/* blobs ----------------------------------- */

/* {{{ _php_ibase_blob_info(isc_blob_handle bl_handle, IBASE_BLOBINFO *bl_info) */
static int _php_ibase_blob_info(isc_blob_handle bl_handle, IBASE_BLOBINFO *bl_info TSRMLS_DC)
{
	static char bl_items[] = {
		isc_info_blob_num_segments,
		isc_info_blob_max_segment,
		isc_info_blob_total_length,
		isc_info_blob_type
	};
	
	char bl_inf[sizeof(long)*8], *p;

	bl_info->max_segment = 0;
	bl_info->num_segments = 0;
	bl_info->total_length = 0;
	bl_info->bl_stream = 0;

	if (isc_blob_info(IB_STATUS, &bl_handle, sizeof(bl_items), bl_items, sizeof(bl_inf), bl_inf)) {
		_php_ibase_error(TSRMLS_C);
		return FAILURE;
	}

	for (p = bl_inf; *p != isc_info_end && p < bl_inf + sizeof(bl_inf);) {
		unsigned short item_len;
		int item = *p++;

		item_len = (short) isc_vax_integer(p, 2);
		p += 2;
		switch (item) {
			case isc_info_blob_num_segments:
				bl_info->num_segments = isc_vax_integer(p, item_len);
				break;
			case isc_info_blob_max_segment:
				bl_info->max_segment = isc_vax_integer(p, item_len);
				break;
			case isc_info_blob_total_length:
				bl_info->total_length = isc_vax_integer(p, item_len);
				break;
			case isc_info_blob_type:
				bl_info->bl_stream = isc_vax_integer(p, item_len);
				break;
			case isc_info_end:
				break;
			case isc_info_truncated:
			case isc_info_error:  /* hmm. don't think so...*/
				_php_ibase_module_error("PHP module internal error" TSRMLS_CC);
				return FAILURE;
		} /* switch */
		p += item_len;
	} /* for */
	return SUCCESS;
}
/* }}} */

/* {{{ proto resource ibase_blob_create([resource link_identifier])
   Create blob for adding data */
PHP_FUNCTION(ibase_blob_create)
{
	zval **link_arg;
	ibase_db_link *ib_link;
	ibase_trans *trans = NULL;
	ibase_blob *ib_blob;

	RESET_ERRMSG;

	switch (ZEND_NUM_ARGS()) {
		case 0:
			ZEND_FETCH_RESOURCE2(ib_link, ibase_db_link *, NULL, IBG(default_link), "InterBase link", le_link, le_plink);
			break;
		case 1:
			if (zend_get_parameters_ex(1, &link_arg) == FAILURE) {
				RETURN_FALSE;
			}
			_php_ibase_get_link_trans(INTERNAL_FUNCTION_PARAM_PASSTHRU, link_arg, &ib_link, &trans);
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	/* open default transaction */
	if (_php_ibase_def_trans(ib_link, &trans TSRMLS_CC) == FAILURE) {
		RETURN_FALSE;
	}
	
	ib_blob = (ibase_blob *) emalloc(sizeof(ibase_blob));
	ib_blob->bl_handle = NULL;
	ib_blob->type = BLOB_INPUT;
	
	if (isc_create_blob(IB_STATUS, &ib_link->handle, &trans->handle, &ib_blob->bl_handle, &ib_blob->bl_qd)) {
		_php_ibase_error(TSRMLS_C);
		efree(ib_blob);
		RETURN_FALSE;
	}
		
	ZEND_REGISTER_RESOURCE(return_value, ib_blob, le_blob);
}
/* }}} */

/* {{{ proto resource ibase_blob_open([ resource link_identifier, ] string blob_id)
   Open blob for retrieving data parts */
PHP_FUNCTION(ibase_blob_open)
{
	zval **blob_arg, **link_arg;
	ibase_db_link *link;
	ibase_trans *trans = NULL;
	ibase_blob *ib_blob;

	RESET_ERRMSG;

	switch (ZEND_NUM_ARGS()) {
		
		case 1:
			if (zend_get_parameters_ex(1, &blob_arg) != SUCCESS) {
				WRONG_PARAM_COUNT;
			}
			ZEND_FETCH_RESOURCE2(link, ibase_db_link *, NULL, IBG(default_link), "InterBase link", le_link, le_plink);
			break;
		case 2:
			if (zend_get_parameters_ex(2, &link_arg, &blob_arg) != SUCCESS) {
				WRONG_PARAM_COUNT;
			}
			_php_ibase_get_link_trans(INTERNAL_FUNCTION_PARAM_PASSTHRU, link_arg, &link, &trans);	
			break;
		default:
			WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(blob_arg);

	ib_blob = (ibase_blob *) emalloc(sizeof(ibase_blob));
	ib_blob->bl_handle = NULL;
	ib_blob->type = BLOB_OUTPUT;
	
	if (! _php_ibase_string_to_quad(Z_STRVAL_PP(blob_arg), &(ib_blob->bl_qd))) {
		_php_ibase_module_error("String is not a BLOB ID" TSRMLS_CC);
		efree(ib_blob);
		RETURN_FALSE;
	}

	/* open default transaction */
	if (_php_ibase_def_trans(link, &trans TSRMLS_CC) == FAILURE) {
		efree(ib_blob);
		RETURN_FALSE;
	}

	if (isc_open_blob(IB_STATUS, &link->handle, &trans->handle, &ib_blob->bl_handle, &ib_blob->bl_qd)) {
		_php_ibase_error(TSRMLS_C);
		efree(ib_blob);
		RETURN_FALSE;
	}
	
	ZEND_REGISTER_RESOURCE(return_value, ib_blob, le_blob);
}
/* }}} */

/* {{{ proto bool ibase_blob_add(resource blob_handle, string data)
   Add data into created blob */
PHP_FUNCTION(ibase_blob_add)
{
	zval **blob_arg, **string_arg;
	ibase_blob *ib_blob;

	RESET_ERRMSG;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &blob_arg, &string_arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ib_blob, ibase_blob *, blob_arg, -1, "Interbase blob", le_blob);

	if (ib_blob->type != BLOB_INPUT) {
		_php_ibase_module_error("BLOB is not open for input" TSRMLS_CC);
		RETURN_FALSE;
	}

	if (_php_ibase_blob_add(string_arg, ib_blob TSRMLS_CC) != SUCCESS) {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto string ibase_blob_get(resource blob_handle, int len)
   Get len bytes data from open blob */
PHP_FUNCTION(ibase_blob_get)
{
	zval **blob_arg, **len_arg;
	ibase_blob *ib_blob;

	RESET_ERRMSG;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &blob_arg, &len_arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ib_blob, ibase_blob *, blob_arg, -1, "Interbase blob", le_blob);

	if (ib_blob->type != BLOB_OUTPUT) {
		_php_ibase_module_error("BLOB is not open for output" TSRMLS_CC);
		RETURN_FALSE;
	}

	convert_to_long_ex(len_arg);

	if (_php_ibase_blob_get(return_value, ib_blob, Z_LVAL_PP(len_arg) TSRMLS_CC) != SUCCESS) {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ _php_ibase_blob_end() */
/* Close or Cancel created or Close open blob */
static void _php_ibase_blob_end(INTERNAL_FUNCTION_PARAMETERS, int bl_end)
{
	zval **blob_arg;
	ibase_blob *ib_blob;

	RESET_ERRMSG;
	
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &blob_arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ib_blob, ibase_blob *, blob_arg, -1, "Interbase blob", le_blob);	
	
	if (bl_end == BLOB_CLOSE) { /* return id here */

		if (ib_blob->bl_qd.gds_quad_high || ib_blob->bl_qd.gds_quad_low) { /*not null ?*/
			if (isc_close_blob(IB_STATUS, &ib_blob->bl_handle)) {
				_php_ibase_error(TSRMLS_C);
				RETURN_FALSE;
			}
		}
		ib_blob->bl_handle = NULL;

		RETVAL_STRINGL(_php_ibase_quad_to_string(ib_blob->bl_qd), BLOB_ID_LEN, 0);
	} else { /* discard created blob */
		if (isc_cancel_blob(IB_STATUS, &ib_blob->bl_handle)) {
			_php_ibase_error(TSRMLS_C);
			RETURN_FALSE;
		}
		ib_blob->bl_handle = NULL;
		RETVAL_BOOL(1);
	}
	zend_list_delete(Z_LVAL_PP(blob_arg));
}
/* }}} */

/* {{{ proto string ibase_blob_close(resource blob_handle)
   Close blob */
PHP_FUNCTION(ibase_blob_close)
{
	_php_ibase_blob_end(INTERNAL_FUNCTION_PARAM_PASSTHRU, BLOB_CLOSE);
}
/* }}} */

/* {{{ proto bool ibase_blob_cancel(resource blob_handle)
   Cancel creating blob */
PHP_FUNCTION(ibase_blob_cancel)
{
	_php_ibase_blob_end(INTERNAL_FUNCTION_PARAM_PASSTHRU, BLOB_CANCEL);
}
/* }}} */

/* {{{ proto array ibase_blob_info([ resource link_identifier, ] string blob_id)
   Return blob length and other useful info */
PHP_FUNCTION(ibase_blob_info)
{
	zval **blob_arg, **link_arg;
	ibase_db_link *link;
	ibase_trans *trans = NULL;
	ibase_blob ib_blob = { NULL, { 0, 0 }, BLOB_INPUT };
	IBASE_BLOBINFO bl_info;

	RESET_ERRMSG;

	switch (ZEND_NUM_ARGS()) {
		
		case 1:
			if (zend_get_parameters_ex(1, &blob_arg) != SUCCESS) {
				WRONG_PARAM_COUNT;
			}
			ZEND_FETCH_RESOURCE2(link, ibase_db_link *, NULL, IBG(default_link), "InterBase link", le_link, le_plink);
			break;
		case 2:
			if (zend_get_parameters_ex(2, &link_arg, &blob_arg) != SUCCESS) {
				WRONG_PARAM_COUNT;
			}
			_php_ibase_get_link_trans(INTERNAL_FUNCTION_PARAM_PASSTHRU, link_arg, &link, &trans);	
			break;
		default:
			WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(blob_arg);

	if (! _php_ibase_string_to_quad(Z_STRVAL_PP(blob_arg), &(ib_blob.bl_qd))) {
		_php_ibase_module_error("Unrecognized BLOB ID" TSRMLS_CC);
		RETURN_FALSE;
	}

	/* open default transaction */
	if (_php_ibase_def_trans(link, &trans TSRMLS_CC) == FAILURE) {
		RETURN_FALSE;
	}
	
	if (ib_blob.bl_qd.gds_quad_high || ib_blob.bl_qd.gds_quad_low) { /* not null ? */
		if (isc_open_blob(IB_STATUS, &link->handle, &trans->handle, &ib_blob.bl_handle, &ib_blob.bl_qd)) {
			_php_ibase_error(TSRMLS_C);
			RETURN_FALSE;
		}

		if (_php_ibase_blob_info(ib_blob.bl_handle, &bl_info TSRMLS_CC)) {
			RETURN_FALSE;
		}
		if (isc_close_blob(IB_STATUS, &ib_blob.bl_handle)) {
			_php_ibase_error(TSRMLS_C);
			RETURN_FALSE;
		}
	} else { /* null blob, all values to zero */
		bl_info.max_segment = 0;
		bl_info.num_segments = 0;
		bl_info.total_length = 0;
		bl_info.bl_stream = 0;
	}

	array_init(return_value);
	
	add_index_long(return_value, 0, bl_info.total_length);
 	add_assoc_long(return_value, "length", bl_info.total_length);

	add_index_long(return_value, 1, bl_info.num_segments);
 	add_assoc_long(return_value, "numseg", bl_info.num_segments);

	add_index_long(return_value, 2, bl_info.max_segment);
 	add_assoc_long(return_value, "maxseg", bl_info.max_segment);

	add_index_bool(return_value, 3, bl_info.bl_stream);
 	add_assoc_bool(return_value, "stream", bl_info.bl_stream);

	add_index_bool(return_value, 4, (!ib_blob.bl_qd.gds_quad_high && !ib_blob.bl_qd.gds_quad_low));
 	add_assoc_bool(return_value, "isnull", (!ib_blob.bl_qd.gds_quad_high && !ib_blob.bl_qd.gds_quad_low));
}
/* }}} */

/* {{{ proto bool ibase_blob_echo([ resource link_identifier, ] string blob_id)
   Output blob contents to browser */
PHP_FUNCTION(ibase_blob_echo)
{
	zval **blob_arg, **link_arg;
	ibase_db_link *link;
	ibase_trans *trans = NULL;		
	ibase_blob ib_blob_id = { NULL, { 0, 0 }, BLOB_OUTPUT };
	char bl_data[IBASE_BLOB_SEG]; 
	unsigned short seg_len;

	RESET_ERRMSG;

	switch (ZEND_NUM_ARGS()) {
		
		case 1:
			if (zend_get_parameters_ex(1, &blob_arg) != SUCCESS) {
				WRONG_PARAM_COUNT;
			}
			ZEND_FETCH_RESOURCE2(link, ibase_db_link *, NULL, IBG(default_link), "InterBase link", le_link, le_plink);
			break;
		case 2:
			if (zend_get_parameters_ex(2, &link_arg, &blob_arg) != SUCCESS) {
				WRONG_PARAM_COUNT;
			}
			_php_ibase_get_link_trans(INTERNAL_FUNCTION_PARAM_PASSTHRU, link_arg, &link, &trans);	
			break;
		default:
			WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(blob_arg);

	if (! _php_ibase_string_to_quad(Z_STRVAL_PP(blob_arg), &(ib_blob_id.bl_qd))) {
		_php_ibase_module_error("Unrecognized BLOB ID" TSRMLS_CC);
		RETURN_FALSE;
	}

	/* open default transaction */
	if (_php_ibase_def_trans(link, &trans TSRMLS_CC) == FAILURE) {
		RETURN_FALSE;
	}

	if (isc_open_blob(IB_STATUS, &link->handle, &trans->handle, &ib_blob_id.bl_handle, &ib_blob_id.bl_qd)) {
		_php_ibase_error(TSRMLS_C);
		RETURN_FALSE;
	}

	while (!isc_get_segment(IB_STATUS, &ib_blob_id.bl_handle, &seg_len, sizeof(bl_data), bl_data) || IB_STATUS[1] == isc_segment) {
		PHPWRITE(bl_data, seg_len);
	}

	if (IB_STATUS[0] && (IB_STATUS[1] != isc_segstr_eof)) {
		_php_ibase_error(TSRMLS_C);
		RETURN_FALSE;
	}

	if (isc_close_blob(IB_STATUS, &ib_blob_id.bl_handle)) {
		_php_ibase_error(TSRMLS_C);
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string ibase_blob_import([resource link_identifier, ] int file_id)
   Create blob, copy file in it, and close it */
PHP_FUNCTION(ibase_blob_import)
{
	zval **link_arg, **file_arg;
	int link_id = 0, size;
	unsigned short b;
	ibase_blob ib_blob = { NULL, { 0, 0 }, 0 };
	ibase_db_link *ib_link;
	ibase_trans *trans = NULL;
	char bl_data[IBASE_BLOB_SEG];
	php_stream *stream;

	RESET_ERRMSG;

	switch (ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &file_arg) == FAILURE) {
				RETURN_FALSE;
			}
			link_id = IBG(default_link);
			ZEND_FETCH_RESOURCE2(ib_link, ibase_db_link *, NULL, link_id, "InterBase link", le_link, le_plink);
			break;
		case 2:
			if (zend_get_parameters_ex(2, &link_arg, &file_arg) == FAILURE) {
				RETURN_FALSE;
			}
			_php_ibase_get_link_trans(INTERNAL_FUNCTION_PARAM_PASSTHRU, link_arg, &ib_link, &trans);
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	/* open default transaction */
	if (_php_ibase_def_trans(ib_link, &trans TSRMLS_CC) == FAILURE) {
		RETURN_FALSE;
	}

	php_stream_from_zval(stream, file_arg);
	
	if (isc_create_blob(IB_STATUS, &ib_link->handle, &trans->handle, &ib_blob.bl_handle, &ib_blob.bl_qd)) {
		_php_ibase_error(TSRMLS_C);
		RETURN_FALSE;
	}

	size = 0;

	while ((b = php_stream_read(stream, bl_data, sizeof(bl_data))) > 0) {
		if (isc_put_segment(IB_STATUS, &ib_blob.bl_handle, b, bl_data)) {
			_php_ibase_error(TSRMLS_C);
			RETURN_FALSE;
		}
		size += b;
	}
	
	if (isc_close_blob(IB_STATUS, &ib_blob.bl_handle)) {
		_php_ibase_error(TSRMLS_C);
		RETURN_FALSE;
	}

	RETURN_STRINGL( _php_ibase_quad_to_string(ib_blob.bl_qd), BLOB_ID_LEN, 0);
}
/* }}} */

#ifdef SQL_DIALECT_V6
/* {{{ _php_ibase_user() */
static void _php_ibase_user(INTERNAL_FUNCTION_PARAMETERS, int operation)
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

/*
	zend_printf("server   : %s<br>", ib_server);
	zend_printf("admin    : %s<br>", dba_user_name);
	zend_printf("admin pwd: %s<br>", dba_password);
	zend_printf("user     : %s<br>", user_name);
	zend_printf("user pwd : %s<br>", user_password);
	zend_printf("fname    : %s<br>", first_name);
	zend_printf("mname    : %s<br>", middle_name);
	zend_printf("lname    : %s<br>", last_name);
*/

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
#endif /* SQL_DIALECT_V6 */

/* {{{ _php_ibase_event_block() */
static void _php_ibase_event_block(ibase_db_link *ib_link, unsigned short count, char **events, 
	unsigned short *l, char **event_buf, char **result_buf)
{
	ISC_STATUS dummy_result[20];
	unsigned long dummy_count[15];

	/**
	 * Unfortunately, there's no clean and portable way in C to pass arguments to 
	 * a variadic function if you don't know the number of arguments at compile time.
	 * (And even if there were a way, the Interbase API doesn't provide a version of
	 * this function that takes a va_list as an argument)
	 *
	 * In this case, the number of arguments is limited to 18 by the underlying API,
	 * so we can work around it.
	 */

	*l = (unsigned short) isc_event_block(event_buf, result_buf, count, events[0], 
		events[1], events[2], events[3], events[4], events[5], events[6], events[7], 
		events[8], events[9], events[10], events[11], events[12], events[13], events[14]);

	/**
	 * Currently, this is the only way to correctly initialize an event buffer.
	 * This is clearly something that should be fixed, cause the semantics of
	 * isc_wait_for_event() indicate that it blocks until an event occurs.
	 * If the Firebird people ever fix this, these lines should be removed,
	 * otherwise, events will have to fire twice before ibase_wait_event() returns.
	 */

	isc_wait_for_event(dummy_result, &ib_link->handle, *l, *event_buf, *result_buf);
	isc_event_counts(dummy_count, *l, *event_buf, *result_buf);
}
/* }}} */

/* {{{ proto string ibase_wait_event([resource link_identifier,] string event [, string event [, ...]])
   Waits for any one of the passed Interbase events to be posted by the database, and returns its name */
PHP_FUNCTION(ibase_wait_event)
{
	zval ***args;
	ibase_db_link *ib_link;
	char *event_buffer, *result_buffer, *events[15];
	unsigned short i = 0, event_count = 0, buffer_size;
	unsigned long occurred_event[15];
	
	RESET_ERRMSG;

	/* no more than 15 events */
	if (ZEND_NUM_ARGS() < 1 || ZEND_NUM_ARGS() > 16) {
		WRONG_PARAM_COUNT;
	}

	args = (zval ***) safe_emalloc(sizeof(zval **), ZEND_NUM_ARGS(), 0);
	if (zend_get_parameters_array_ex(ZEND_NUM_ARGS(), args) == FAILURE) {
		efree(args);
		RETURN_FALSE;
	}

	if (Z_TYPE_PP(args[0]) == IS_RESOURCE) {

		ZEND_FETCH_RESOURCE2(ib_link, ibase_db_link *, args[0], -1, "InterBase link", le_link, le_plink);
		i = 1;

	} else {

		if (ZEND_NUM_ARGS() > 15) {
			efree(args);
			WRONG_PARAM_COUNT;
		}

		ZEND_FETCH_RESOURCE2(ib_link, ibase_db_link *, NULL, IBG(default_link), "InterBase link", le_link, le_plink);
	}				
		
	for (; i < ZEND_NUM_ARGS(); ++i) {
		convert_to_string_ex(args[i]);
		events[event_count++] = Z_STRVAL_PP(args[i]);
	}

	/* fills the required data structure with information about the events */
	_php_ibase_event_block(ib_link, event_count, events, &buffer_size, &event_buffer, &result_buffer);

	/* now block until an event occurs */
	if (isc_wait_for_event(IB_STATUS, &ib_link->handle, buffer_size, event_buffer, result_buffer)) {
		_php_ibase_error(TSRMLS_C);
		_php_ibase_event_free(event_buffer,result_buffer);
		efree(args);
		RETURN_FALSE;
	}
	
	/* find out which event occurred */
	isc_event_counts(occurred_event, buffer_size, event_buffer, result_buffer);
	for (i = 0; i < event_count; ++i) {
		if (occurred_event[i]) {
			char *result = estrdup(events[i]);
			_php_ibase_event_free(event_buffer,result_buffer);
			efree(args);
			RETURN_STRING(result,0);
		}
	}
	
	/* If we reach this line, isc_wait_for_event() did return, but we don't know
	   which event fired. */
	_php_ibase_event_free(event_buffer,result_buffer);
	efree(args);
	RETURN_FALSE;
}
/* }}} */

/* {{{ _php_ibase_callback() */
static isc_callback _php_ibase_callback(ibase_event *event, unsigned short buffer_size, char *result_buf)
{
	/* this function is called asynchronously by the Interbase client library. */
	TSRMLS_FETCH_FROM_CTX(event->thread_ctx);
	
	/**
	 * The callback function is called when the event is first registered and when the event
	 * is cancelled. I consider this is a bug. By clearing event->callback, we make sure 
	 * nothing happens if no event was actually posted.
	 */
	if (event->callback != NULL) {

		unsigned short i;
		unsigned long occurred_event[15];
		zval event_name, link_id, return_value, *args[2];

		/* initialize at runtime to satisify picky compilers */
		args[0] = &event_name; 
		args[1] = &link_id; 
		
		/* copy the updated results into the result buffer */
		memcpy(event->result_buffer, result_buf, buffer_size);
		
		INIT_ZVAL(event_name);
		INIT_ZVAL(link_id);
		ZVAL_RESOURCE(&link_id, event->link_res_id);

		/* find out which event occurred */
		isc_event_counts(occurred_event, buffer_size, event->event_buffer, event->result_buffer);
		for (i = 0; i < event->event_count; ++i) {
			if (occurred_event[i]) {
				ZVAL_STRING(&event_name,event->events[i],0);
				break;
			}
		}	

		/* call the callback provided by the user */
		if (SUCCESS != call_user_function(EG(function_table), NULL, event->callback, &return_value, 2, args TSRMLS_CC)) {
			_php_ibase_module_error("Error calling callback %s" TSRMLS_CC, Z_STRVAL_P(event->callback));
			return 0;
		}

		if (Z_TYPE(return_value) == IS_BOOL && !Z_BVAL(return_value)) {
			return 0;
		}			
	}
		
	/* re-register the event */
	if (isc_que_events(IB_STATUS, &event->link->handle, &event->event_id, buffer_size, 
		event->event_buffer,(isc_callback)_php_ibase_callback, (void *)event)) {

		_php_ibase_error(TSRMLS_C);
	}
	return 0;
}
/* }}} */

/* {{{ proto resource ibase_set_event_handler([resource link_identifier,] callback handler, string event [, string event [, ...]])
   Register the callback for handling each of the named events */
PHP_FUNCTION(ibase_set_event_handler)
{
	/**
	 * The callback passed to this function should take an event name (string) and a 
	 * link resource id (int) as arguments. The value returned from the function is 
	 * used to determine if the event handler should remain set.
	 */

	zval ***args, **cb_arg;
	ibase_db_link *ib_link;
	ibase_event *event;
	char *callback_name;
	unsigned short i = 1, buffer_size;
	int link_res_id;
		
	RESET_ERRMSG;

	/* no more than 15 events */
	if (ZEND_NUM_ARGS() < 1 || ZEND_NUM_ARGS() > 17) {
		WRONG_PARAM_COUNT;
	}

	args = (zval ***) safe_emalloc(sizeof(zval **), ZEND_NUM_ARGS(), 0);
	if (zend_get_parameters_array_ex(ZEND_NUM_ARGS(), args) == FAILURE) {
		efree(args);
		RETURN_FALSE;
	}

	/* get a working link */
	if (Z_TYPE_PP(args[0]) != IS_STRING) {

		cb_arg = args[1];
		i = 2;

		ZEND_FETCH_RESOURCE2(ib_link, ibase_db_link *, args[0], -1, "InterBase link", le_link, le_plink);

		convert_to_long_ex(args[0]);
		link_res_id = Z_LVAL_PP(args[0]);

	} else {

		if (ZEND_NUM_ARGS() > 16) {
			efree(args);
			WRONG_PARAM_COUNT;
		}

		cb_arg = args[0];

		ZEND_FETCH_RESOURCE2(ib_link, ibase_db_link *, NULL, IBG(default_link), "InterBase link", le_link, le_plink);
		link_res_id = IBG(default_link);
	}				
		
	/* get the callback */
	if (!zend_is_callable(*cb_arg, 0, &callback_name)) {
		_php_ibase_module_error("Callback argument %s is not a callable function" TSRMLS_CC, callback_name);
		efree(callback_name);
		efree(args);
		RETURN_FALSE;
	}
	efree(callback_name);
		
	/* allocate the event resource */
	event = (ibase_event *) safe_emalloc(sizeof(ibase_event), 1, 0);
	TSRMLS_SET_CTX(event->thread_ctx);
	event->link_res_id = link_res_id;
	event->link = ib_link;
	event->event_count = 0;
	event->callback = NULL;
	event->events = (char **) safe_emalloc(sizeof(char *),ZEND_NUM_ARGS()-i,0);
		
	for (; i < ZEND_NUM_ARGS(); ++i) {
		convert_to_string_ex(args[i]);
		event->events[event->event_count++] = estrdup(Z_STRVAL_PP(args[i]));
	}

	/* fills the required data structure with information about the events */
	_php_ibase_event_block(ib_link, event->event_count, event->events, 
		&buffer_size, &event->event_buffer, &event->result_buffer);
		
	/* now register the events with the Interbase API */
	if (isc_que_events(IB_STATUS, &ib_link->handle, &event->event_id, buffer_size, 
		event->event_buffer,(isc_callback)_php_ibase_callback, (void *)event)) {
			
		_php_ibase_error(TSRMLS_C);
		efree(args);
		efree(event);
		RETURN_FALSE;
	}

	ALLOC_ZVAL(event->callback);
	*event->callback = **cb_arg;
	INIT_PZVAL(event->callback);
	zval_copy_ctor(event->callback);
	
	efree(args);

	ZEND_REGISTER_RESOURCE(return_value, event, le_event);
	zend_list_addref(Z_LVAL_P(return_value));
}

/* {{{ proto bool ibase_free_event_handler(resource event)
   Frees the event handler set by ibase_set_event_handler() */
PHP_FUNCTION(ibase_free_event_handler)
{
	zval **event_arg;
	ibase_event *event;
	
	RESET_ERRMSG;
	
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &event_arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(event, ibase_event *, event_arg, -1, "Interbase event", le_event);
	zend_list_delete(Z_LVAL_PP(event_arg));
	RETURN_TRUE;
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
