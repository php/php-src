/*
  +----------------------------------------------------------------------+
  | PHP Version 6                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2006-2008 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Georg Richter <georg@mysql.com>                             |
  |          Andrey Hristov <andrey@mysql.com>                           |
  |          Ulf Wendel <uwendel@mysql.com>                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */
#include "php.h"
#include "mysqlnd.h"
#include "mysqlnd_wireprotocol.h"
#include "mysqlnd_priv.h"
#include "mysqlnd_result.h"
#include "mysqlnd_result_meta.h"
#include "mysqlnd_statistics.h"
#include "mysqlnd_debug.h"


#define MYSQLND_SILENT


const char * const mysqlnd_not_bound_as_blob = "Can't send long data for non-string/non-binary data types";
const char * const mysqlnd_stmt_not_prepared = "Statement not prepared";

/* Exported by mysqlnd.c */
enum_func_status mysqlnd_simple_command(MYSQLND *conn, enum php_mysqlnd_server_command command,
										const char * const arg, size_t arg_len,
										enum php_mysql_packet_type ok_packet,
										zend_bool silent TSRMLS_DC);

/* Exported by mysqlnd_ps_codec.c */
zend_uchar* mysqlnd_stmt_execute_generate_request(MYSQLND_STMT *stmt, size_t *request_len,
												  zend_bool *free_buffer TSRMLS_DC);


MYSQLND_RES * _mysqlnd_stmt_use_result(MYSQLND_STMT *stmt TSRMLS_DC);

enum_func_status mysqlnd_fetch_stmt_row_buffered(MYSQLND_RES *result, void *param,
												unsigned int flags,
												zend_bool *fetched_anything TSRMLS_DC);

enum_func_status mysqlnd_fetch_stmt_row_cursor(MYSQLND_RES *result, void *param,
											   unsigned int flags,
											   zend_bool *fetched_anything TSRMLS_DC);

static void mysqlnd_stmt_separate_result_bind(MYSQLND_STMT * const stmt TSRMLS_DC);
static void mysqlnd_stmt_separate_one_result_bind(MYSQLND_STMT * const stmt, uint param_no TSRMLS_DC);

static void mysqlnd_internal_free_stmt_content(MYSQLND_STMT * const stmt TSRMLS_DC);
static enum_func_status mysqlnd_stmt_execute_parse_response(MYSQLND_STMT * const stmt TSRMLS_DC);

/* {{{ mysqlnd_stmt::store_result */
static MYSQLND_RES *
MYSQLND_METHOD(mysqlnd_stmt, store_result)(MYSQLND_STMT * const stmt TSRMLS_DC)
{
	enum_func_status ret;
	MYSQLND *conn = stmt->conn;
	MYSQLND_RES *result;
	zend_bool to_cache = FALSE;

	DBG_ENTER("mysqlnd_stmt::store_result");
	DBG_INF_FMT("stmt=%lu", stmt->stmt_id);

	/* be compliant with libmysql - NULL will turn */
	if (!stmt->field_count) {
		DBG_RETURN(NULL);
	}

	if (stmt->cursor_exists) {
		/* Silently convert buffered to unbuffered, for now */
		DBG_RETURN(stmt->m->use_result(stmt TSRMLS_CC));
	}

	/* Nothing to store for UPSERT/LOAD DATA*/
	if (CONN_GET_STATE(conn) != CONN_FETCHING_DATA ||
		stmt->state != MYSQLND_STMT_WAITING_USE_OR_STORE)
	{
		SET_CLIENT_ERROR(conn->error_info, CR_COMMANDS_OUT_OF_SYNC,
						 UNKNOWN_SQLSTATE, mysqlnd_out_of_sync);
		DBG_RETURN(NULL);
	}

	stmt->default_rset_handler = stmt->m->store_result;

	SET_EMPTY_ERROR(stmt->error_info);
	SET_EMPTY_ERROR(stmt->conn->error_info);
	MYSQLND_INC_CONN_STATISTIC(&conn->stats, STAT_PS_BUFFERED_SETS);

	result = stmt->result;
	result->type			= MYSQLND_RES_PS_BUF;
	result->m.fetch_row		= mysqlnd_fetch_stmt_row_buffered;
	result->m.fetch_lengths	= NULL;/* makes no sense */
	if (!result->zval_cache) {
		result->zval_cache = mysqlnd_palloc_get_thd_cache_reference(conn->zval_cache);
	}

	/* Create room for 'next_extend' rows */

	ret = mysqlnd_store_result_fetch_data(conn, result, result->meta,
										  TRUE, to_cache TSRMLS_CC);

	if (PASS == ret) {
		/* libmysql API docs say it should be so for SELECT statements */
		stmt->upsert_status.affected_rows = stmt->result->stored_data->row_count;

		stmt->state = MYSQLND_STMT_USE_OR_STORE_CALLED;
	} else {
		conn->error_info = result->stored_data->error_info;
		stmt->result->m.free_result_contents(stmt->result TSRMLS_CC);
		mnd_efree(stmt->result);
		stmt->result = NULL;
		stmt->state = MYSQLND_STMT_PREPARED;
	}

	DBG_RETURN(result);
}
/* }}} */


/* {{{ mysqlnd_stmt::background_store_result */
static MYSQLND_RES *
MYSQLND_METHOD(mysqlnd_stmt, background_store_result)(MYSQLND_STMT * const stmt TSRMLS_DC)
{
	enum_func_status ret;
	MYSQLND *conn = stmt->conn;
	MYSQLND_RES *result;
	zend_bool to_cache = FALSE;

	DBG_ENTER("mysqlnd_stmt::background_store_result");
	DBG_INF_FMT("stmt=%lu", stmt->stmt_id);

	/* be compliant with libmysql - NULL will turn */
	if (!stmt->field_count) {
		DBG_RETURN(NULL);
	}

	if (stmt->cursor_exists) {
		/* Silently convert buffered to unbuffered, for now */
		MYSQLND_RES * res = stmt->m->use_result(stmt TSRMLS_CC);
		DBG_RETURN(res);
	}

	/* Nothing to store for UPSERT/LOAD DATA*/
	if (CONN_GET_STATE(conn) != CONN_FETCHING_DATA ||
		stmt->state != MYSQLND_STMT_WAITING_USE_OR_STORE)
	{
		SET_CLIENT_ERROR(conn->error_info, CR_COMMANDS_OUT_OF_SYNC,
						 UNKNOWN_SQLSTATE, mysqlnd_out_of_sync);
		DBG_RETURN(NULL);
	}

	stmt->default_rset_handler = stmt->m->store_result;

	SET_EMPTY_ERROR(stmt->error_info);
	SET_EMPTY_ERROR(stmt->conn->error_info);
	MYSQLND_INC_CONN_STATISTIC(&conn->stats, STAT_PS_BUFFERED_SETS);

	result = stmt->result;
	result->type			= MYSQLND_RES_PS_BUF;
	result->m.fetch_row		= mysqlnd_fetch_stmt_row_buffered;
	result->m.fetch_lengths	= NULL;/* makes no sense */
	if (!result->zval_cache) {
		result->zval_cache = mysqlnd_palloc_get_thd_cache_reference(conn->zval_cache);
	}

	/* Create room for 'next_extend' rows */

	/* Not set for SHOW statements at PREPARE stage */
	if (result->conn) {
		result->conn->m->free_reference(result->conn TSRMLS_CC);
		result->conn = NULL;	/* store result does not reference  the connection */
	}

	ret = mysqlnd_store_result_fetch_data(conn, result, result->meta,
										  TRUE, to_cache TSRMLS_CC);

	if (PASS == ret) {
		/* libmysql API docs say it should be so for SELECT statements */
		stmt->upsert_status.affected_rows = stmt->result->stored_data->row_count;

		stmt->state = MYSQLND_STMT_USE_OR_STORE_CALLED;
	} else {
		conn->error_info = result->stored_data->error_info;
		stmt->result->m.free_result_contents(stmt->result TSRMLS_CC);
		mnd_efree(stmt->result);
		stmt->result = NULL;
		stmt->state = MYSQLND_STMT_PREPARED;
	}

	DBG_RETURN(result);
}
/* }}} */


/* {{{ mysqlnd_stmt::get_result */
static MYSQLND_RES *
MYSQLND_METHOD(mysqlnd_stmt, get_result)(MYSQLND_STMT * const stmt TSRMLS_DC)
{
	MYSQLND *conn = stmt->conn;
	MYSQLND_RES *result;

	DBG_ENTER("mysqlnd_stmt::get_result");
	DBG_INF_FMT("stmt=%lu", stmt->stmt_id);

	/* be compliant with libmysql - NULL will turn */
	if (!stmt->field_count) {
		DBG_RETURN(NULL);
	}

	if (stmt->cursor_exists) {
		/* Silently convert buffered to unbuffered, for now */
		DBG_RETURN(stmt->m->use_result(stmt TSRMLS_CC));
	}

	/* Nothing to store for UPSERT/LOAD DATA*/
	if (CONN_GET_STATE(conn) != CONN_FETCHING_DATA || stmt->state != MYSQLND_STMT_WAITING_USE_OR_STORE) {
		SET_CLIENT_ERROR(conn->error_info, CR_COMMANDS_OUT_OF_SYNC,
						 UNKNOWN_SQLSTATE, mysqlnd_out_of_sync);
		DBG_RETURN(NULL);
	}

	SET_EMPTY_ERROR(stmt->error_info);
	SET_EMPTY_ERROR(stmt->conn->error_info);
	MYSQLND_INC_CONN_STATISTIC(&conn->stats, STAT_BUFFERED_SETS);

	result = mysqlnd_result_init(stmt->result->field_count,
								 mysqlnd_palloc_get_thd_cache_reference(conn->zval_cache) TSRMLS_CC);	

	result->meta = stmt->result->meta->m->clone_metadata(stmt->result->meta, FALSE TSRMLS_CC);

	if ((result = result->m.store_result(result, conn, TRUE TSRMLS_CC))) {
		stmt->upsert_status.affected_rows = result->stored_data->row_count;	
		stmt->state = MYSQLND_STMT_PREPARED;
		result->type = MYSQLND_RES_PS_BUF;
	} else {
		stmt->error_info = conn->error_info;
		stmt->state = MYSQLND_STMT_PREPARED;
	}

	DBG_RETURN(result);
}
/* }}} */


/* {{{ mysqlnd_stmt::more_results */
static zend_bool
MYSQLND_METHOD(mysqlnd_stmt, more_results)(const MYSQLND_STMT * stmt TSRMLS_DC)
{
	DBG_ENTER("mysqlnd_stmt::more_results");
	/* (conn->state == CONN_NEXT_RESULT_PENDING) too */
	DBG_RETURN((stmt->conn && (stmt->conn->upsert_status.server_status &
							   SERVER_MORE_RESULTS_EXISTS))?
									TRUE:
									FALSE);
}
/* }}} */


/* {{{ mysqlnd_stmt::next_result */
static enum_func_status
MYSQLND_METHOD(mysqlnd_stmt, next_result)(MYSQLND_STMT * stmt TSRMLS_DC)
{
	MYSQLND *conn = stmt->conn;

	DBG_ENTER("mysqlnd_stmt::next_result");
	DBG_INF_FMT("stmt=%lu", stmt->stmt_id);

	if (!conn ||
		CONN_GET_STATE(conn) != CONN_NEXT_RESULT_PENDING ||
		!(conn->upsert_status.server_status & SERVER_MORE_RESULTS_EXISTS) ||
		!stmt->result)
	{
		DBG_RETURN(FAIL);
	}

	/* Free space for next result */
	mysqlnd_internal_free_stmt_content(stmt TSRMLS_CC);

	DBG_RETURN(mysqlnd_stmt_execute_parse_response(stmt TSRMLS_CC));
}
/* }}} */


/* {{{ mysqlnd_stmt_skip_metadata */
static enum_func_status
mysqlnd_stmt_skip_metadata(MYSQLND_STMT *stmt TSRMLS_DC)
{
	/* Follows parameter metadata, we have just to skip it, as libmysql does */
	unsigned int i = 0;
	enum_func_status ret = PASS;
	php_mysql_packet_res_field field_packet;

	DBG_ENTER("mysqlnd_stmt_skip_metadata");
	DBG_INF_FMT("stmt=%lu", stmt->stmt_id);

	PACKET_INIT_ALLOCA(field_packet, PROT_RSET_FLD_PACKET);
	field_packet.skip_parsing = TRUE;
	for (;i < stmt->param_count; i++) {
		if (FAIL == PACKET_READ_ALLOCA(field_packet, stmt->conn)) {
			ret = FAIL;
			break;
		}
	}
	PACKET_FREE_ALLOCA(field_packet);

	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_stmt_read_prepare_response */
static enum_func_status
mysqlnd_stmt_read_prepare_response(MYSQLND_STMT *stmt TSRMLS_DC)
{
	php_mysql_packet_prepare_response prepare_resp;
	enum_func_status ret = PASS;

	DBG_ENTER("mysqlnd_stmt_read_prepare_response");
	DBG_INF_FMT("stmt=%lu", stmt->stmt_id);

	PACKET_INIT_ALLOCA(prepare_resp, PROT_PREPARE_RESP_PACKET);
	if (FAIL == PACKET_READ_ALLOCA(prepare_resp, stmt->conn)) {
		ret = FAIL;
		goto done;
	}

	if (0xFF == prepare_resp.error_code) {
		stmt->error_info = stmt->conn->error_info = prepare_resp.error_info;
		ret = FAIL;
		goto done;
	}

	stmt->stmt_id = prepare_resp.stmt_id;
	stmt->warning_count = stmt->conn->upsert_status.warning_count = prepare_resp.warning_count;
	stmt->field_count = stmt->conn->field_count = prepare_resp.field_count;
	stmt->param_count = prepare_resp.param_count;
	PACKET_FREE_ALLOCA(prepare_resp);

done:
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_stmt_prepare_read_eof */
static enum_func_status
mysqlnd_stmt_prepare_read_eof(MYSQLND_STMT *stmt TSRMLS_DC)
{
	php_mysql_packet_eof fields_eof;
	enum_func_status ret;

	DBG_ENTER("mysqlnd_stmt_prepare_read_eof");
	DBG_INF_FMT("stmt=%lu", stmt->stmt_id);

	PACKET_INIT_ALLOCA(fields_eof, PROT_EOF_PACKET);
	if (FAIL == (ret = PACKET_READ_ALLOCA(fields_eof, stmt->conn))) {
		if (stmt->result) {
			stmt->result->m.free_result_contents(stmt->result TSRMLS_CC);
			mnd_efree(stmt->result);
			memset(stmt, 0, sizeof(MYSQLND_STMT));
			stmt->state = MYSQLND_STMT_INITTED;
		}
	} else {
		stmt->upsert_status.server_status = fields_eof.server_status;
		stmt->upsert_status.warning_count = fields_eof.warning_count;
		stmt->state = MYSQLND_STMT_PREPARED;
	}
	PACKET_FREE_ALLOCA(fields_eof);

	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_stmt::prepare */
static enum_func_status
MYSQLND_METHOD(mysqlnd_stmt, prepare)(MYSQLND_STMT * const stmt, const char * const query,
									  unsigned int query_len TSRMLS_DC)
{
	MYSQLND_STMT *stmt_to_prepare = stmt;

	DBG_ENTER("mysqlnd_stmt::prepare");
	DBG_INF_FMT("stmt=%lu", stmt->stmt_id);

	SET_ERROR_AFF_ROWS(stmt);
	SET_ERROR_AFF_ROWS(stmt->conn);

	SET_EMPTY_ERROR(stmt->error_info);
	SET_EMPTY_ERROR(stmt->conn->error_info);

	if (stmt->state > MYSQLND_STMT_INITTED) {
		/* See if we have to clean the wire */
		if (stmt->state == MYSQLND_STMT_WAITING_USE_OR_STORE) {
			/* Do implicit use_result and then flush the result */
			stmt->default_rset_handler = stmt->m->use_result;
			stmt->default_rset_handler(stmt TSRMLS_CC);
		}
		/* No 'else' here please :) */
		if (stmt->state > MYSQLND_STMT_WAITING_USE_OR_STORE) {
			stmt->result->m.skip_result(stmt->result TSRMLS_CC);
		}
		/*
		  Create a new test statement, which we will prepare, but if anything
		  fails, we will scrap it.
		*/
		stmt_to_prepare = mysqlnd_stmt_init(stmt->conn);
	}

	if (FAIL == mysqlnd_simple_command(stmt_to_prepare->conn, COM_STMT_PREPARE, query,
									   query_len, PROT_LAST, FALSE TSRMLS_CC) ||
		FAIL == mysqlnd_stmt_read_prepare_response(stmt_to_prepare TSRMLS_CC)) {
		goto fail;
	}

	if (stmt_to_prepare->param_count) {
		if (FAIL == mysqlnd_stmt_skip_metadata(stmt_to_prepare TSRMLS_CC) ||
			FAIL == mysqlnd_stmt_prepare_read_eof(stmt_to_prepare TSRMLS_CC))
		{
			goto fail;
		}
	}

	/*
	  Read metadata only if there is actual result set.
	  Beware that SHOW statements bypass the PS framework and thus they send
	  no metadata at prepare.
	*/
	if (stmt_to_prepare->field_count) {
		MYSQLND_RES *result = mysqlnd_result_init(stmt_to_prepare->field_count,
												  mysqlnd_palloc_get_thd_cache_reference(stmt->conn->zval_cache)
												  TSRMLS_CC);
		/* Allocate the result now as it is needed for the reading of metadata */
		stmt_to_prepare->result = result; 

		result->conn = stmt_to_prepare->conn->m->get_reference(stmt_to_prepare->conn TSRMLS_CC);

		result->type = MYSQLND_RES_PS_BUF;

		if (FAIL == result->m.read_result_metadata(result, stmt_to_prepare->conn TSRMLS_CC) ||
			FAIL == mysqlnd_stmt_prepare_read_eof(stmt_to_prepare TSRMLS_CC)) {
			goto fail;
		}
	}

	if (stmt_to_prepare != stmt) {
		/* Free old buffers, binding and resources on server */
		stmt->m->net_close(stmt, TRUE TSRMLS_CC);

		memcpy(stmt, stmt_to_prepare, sizeof(MYSQLND_STMT));

		/* Now we will have a clean new statement object */
		mnd_efree(stmt_to_prepare);
	}
	stmt->state = MYSQLND_STMT_PREPARED;
	DBG_INF("PASS");
	DBG_RETURN(PASS);

fail:
	if (stmt_to_prepare != stmt) {
		stmt_to_prepare->m->dtor(stmt_to_prepare, TRUE TSRMLS_CC);
	}
	stmt->state = MYSQLND_STMT_INITTED;

	DBG_INF("FAIL");
	DBG_RETURN(FAIL);
}
/* }}} */


/* {{{ mysqlnd_stmt_execute_parse_response */
static enum_func_status
mysqlnd_stmt_execute_parse_response(MYSQLND_STMT * const stmt TSRMLS_DC)
{
	enum_func_status ret;
	MYSQLND	*conn = stmt->conn;

	DBG_ENTER("mysqlnd_stmt_execute_parse_response");

	CONN_SET_STATE(conn, CONN_QUERY_SENT);

	ret = mysqlnd_query_read_result_set_header(stmt->conn, stmt TSRMLS_CC);
	if (ret == FAIL) {
		stmt->error_info = conn->error_info;
		stmt->upsert_status.affected_rows = conn->upsert_status.affected_rows;
		if (CONN_GET_STATE(conn) == CONN_QUIT_SENT) {
			/* close the statement here, the connection has been closed */
		}
		stmt->state = MYSQLND_STMT_PREPARED;
	} else {
		SET_EMPTY_ERROR(stmt->error_info);
		SET_EMPTY_ERROR(stmt->conn->error_info);
		stmt->send_types_to_server = 0;
		stmt->upsert_status = conn->upsert_status;
		stmt->state = MYSQLND_STMT_EXECUTED;
		if (conn->last_query_type == QUERY_UPSERT || conn->last_query_type == QUERY_LOAD_LOCAL) {
			DBG_INF("PASS");
			DBG_RETURN(PASS);
		}

		stmt->result->type = MYSQLND_RES_PS_BUF;
		if (!stmt->result->conn) {
			/*
			  For SHOW we don't create (bypasses PS in server)
			  a result set at prepare and thus a connection was missing
			*/
			stmt->result->conn = stmt->conn->m->get_reference(stmt->conn TSRMLS_CC);
		}

		/* Update stmt->field_count as SHOW sets it to 0 at prepare */
		stmt->field_count = stmt->result->field_count = conn->field_count;
		stmt->result->lengths = NULL;
		if (stmt->field_count) {
			stmt->state = MYSQLND_STMT_WAITING_USE_OR_STORE;
			/*
			  We need to set this because the user might not call
			  use_result() or store_result() and we should be able to scrap the
			  data on the line, if he just decides to close the statement.
			*/
			DBG_INF_FMT("server_status=%d cursor=%d", stmt->upsert_status.server_status,
						stmt->upsert_status.server_status & SERVER_STATUS_CURSOR_EXISTS);

			if (stmt->upsert_status.server_status & SERVER_STATUS_CURSOR_EXISTS) {
				stmt->cursor_exists = TRUE;
				CONN_SET_STATE(conn, CONN_READY);
				/* Only cursor read */
				stmt->default_rset_handler = stmt->m->use_result;
				DBG_INF("use_result");
			} else if (stmt->flags & CURSOR_TYPE_READ_ONLY) {
				/*
				  We have asked for CURSOR but got no cursor, because the condition
				  above is not fulfilled. Then...

				  This is a single-row result set, a result set with no rows, EXPLAIN,
				  SHOW VARIABLES, or some other command which either a) bypasses the
				  cursors framework in the server and writes rows directly to the
				  network or b) is more efficient if all (few) result set rows are
				  precached on client and server's resources are freed.
				*/
				/* preferred is buffered read */
				stmt->default_rset_handler = stmt->m->store_result;
				DBG_INF("store_result");
			} else {
				/* preferred is unbuffered read */
				stmt->default_rset_handler = stmt->m->use_result;
				DBG_INF("use_result");
			}
		}
	}

	DBG_INF(ret == PASS? "PASS":"FAIL");
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_stmt::execute */
static enum_func_status
MYSQLND_METHOD(mysqlnd_stmt, execute)(MYSQLND_STMT * const stmt TSRMLS_DC)
{
	enum_func_status ret;
	MYSQLND		*conn = stmt->conn;
	zend_uchar	*request;
	size_t		request_len;
	zend_bool	free_request;

	DBG_ENTER("mysqlnd_stmt::execute");
	DBG_INF_FMT("stmt=%lu", stmt->stmt_id);

	SET_ERROR_AFF_ROWS(stmt);
	SET_ERROR_AFF_ROWS(stmt->conn);
	
	if (stmt->result && stmt->state > MYSQLND_STMT_PREPARED && stmt->field_count) {
		/*
		  We don need to copy the data from the buffers which we will clean.
		  Because it has already been copied. See
		  #ifndef WE_DONT_COPY_IN_BUFFERED_AND_UNBUFFERED_BECAUSEOF_IS_REF
		*/
#ifdef WE_DONT_COPY_IN_BUFFERED_AND_UNBUFFERED_BECAUSEOF_IS_REF
		if (stmt->result_bind &&
			stmt->result_zvals_separated_once == TRUE && 
			stmt->state >= MYSQLND_STMT_USER_FETCHING)
		{
			/*
			  We need to copy the data from the buffers which we will clean.
			  The bound variables point to them only if the user has started
			  to fetch data (MYSQLND_STMT_USER_FETCHING).
			  We need to check 'result_zvals_separated_once' or we will leak
			  in the following scenario
			  prepare("select 1 from dual");
			  execute();
			  fetch(); <-- no binding, but that's not a problem
			  bind_result();
			  execute(); <-- here we will leak because we separate without need
			*/
			unsigned int i;
			for (i = 0; i < stmt->field_count; i++) {
				if (stmt->result_bind[i].bound == TRUE) {
					zval_copy_ctor(stmt->result_bind[i].zv);
				}
			}
		}
#endif

		/*
		  If right after execute() we have to call the appropriate
		  use_result() or store_result() and clean.
		*/
		if (stmt->state == MYSQLND_STMT_WAITING_USE_OR_STORE) {
			DBG_INF("fetching result set header");
			/* Do implicit use_result and then flush the result */
			stmt->default_rset_handler = stmt->m->use_result;
			stmt->default_rset_handler(stmt TSRMLS_CC);
		}

		if (stmt->state > MYSQLND_STMT_WAITING_USE_OR_STORE) {
			DBG_INF("skipping result");
			/* Flush if anything is left and unbuffered set */
			stmt->result->m.skip_result(stmt->result TSRMLS_CC);
		}

		if (stmt->state > MYSQLND_STMT_PREPARED) {
			/* As the buffers have been freed, we should go back to PREPARED */
			stmt->state = MYSQLND_STMT_PREPARED;
		}

		/*
		  Executed, but the user hasn't started to fetch
		  This will clean also the metadata, but after the EXECUTE call we will
		  have it again.
		*/
		stmt->result->m.free_result_buffers(stmt->result TSRMLS_CC);
	} else if (stmt->state < MYSQLND_STMT_PREPARED) {
		/* Only initted - error */
		SET_CLIENT_ERROR(conn->error_info, CR_COMMANDS_OUT_OF_SYNC, UNKNOWN_SQLSTATE,
						 mysqlnd_out_of_sync);
		SET_STMT_ERROR(stmt, CR_COMMANDS_OUT_OF_SYNC, UNKNOWN_SQLSTATE, mysqlnd_out_of_sync);
		DBG_INF("FAIL");
		DBG_RETURN(FAIL);
	}

	if (stmt->param_count) {
		uint i, not_bound = 0;
		if (!stmt->param_bind) {
			SET_STMT_ERROR(stmt, CR_PARAMS_NOT_BOUND, UNKNOWN_SQLSTATE,
							 "No data supplied for parameters in prepared statement");
			DBG_INF("FAIL");
			DBG_RETURN(FAIL);
		}
		for (i = 0; i < stmt->param_count; i++) {
			if (stmt->param_bind[i].zv == NULL) {
				not_bound++;
			}
		}
		if (not_bound) {
			char * msg;
			spprintf(&msg, 0, "No data supplied for %d parameter%s in prepared statement",
					 not_bound, not_bound>1 ?"s":"");
			SET_STMT_ERROR(stmt, CR_PARAMS_NOT_BOUND, UNKNOWN_SQLSTATE, msg);
			if (msg) {
				efree(msg);
			}
			DBG_INF("FAIL");
			DBG_RETURN(FAIL);
		}
	}
	request = mysqlnd_stmt_execute_generate_request(stmt, &request_len, &free_request TSRMLS_CC);
	
	/* support for buffer types should be added here ! */

	ret = mysqlnd_simple_command(stmt->conn, COM_STMT_EXECUTE, (char *)request, request_len,
								 PROT_LAST /* we will handle the response packet*/,
								 FALSE TSRMLS_CC);

	if (free_request) {
		mnd_efree(request);
	}

	if (ret == FAIL) {
		stmt->error_info = conn->error_info;
		DBG_INF("FAIL");
		DBG_RETURN(FAIL);
	}
	stmt->execute_count++;

	DBG_RETURN(mysqlnd_stmt_execute_parse_response(stmt TSRMLS_CC));
}
/* }}} */


/* {{{ mysqlnd_fetch_stmt_row_buffered */
enum_func_status
mysqlnd_fetch_stmt_row_buffered(MYSQLND_RES *result, void *param, unsigned int flags,
								zend_bool *fetched_anything TSRMLS_DC)
{
	MYSQLND_STMT *stmt = (MYSQLND_STMT *) param;
	MYSQLND_RES_BUFFERED *set = result->stored_data;
	uint field_count = result->meta->field_count;

	DBG_ENTER("mysqlnd_fetch_stmt_row_buffered");
	DBG_INF_FMT("stmt=%lu", stmt->stmt_id);

	/* If we haven't read everything */
	if (set->data_cursor &&
		(set->data_cursor - set->data) < (set->row_count * field_count))
	{
		/* The user could have skipped binding - don't crash*/
		if (stmt->result_bind) {
			unsigned int i;
			MYSQLND_RES_METADATA * meta = result->meta;
			zval **current_row = set->data_cursor;

			if (NULL == current_row[0]) {
				uint64 row_num = (set->data_cursor - set->data) / field_count;
				set->initialized_rows++;
				result->m.row_decoder(set->row_buffers[row_num],
									  current_row,
									  meta->field_count,
									  meta->fields,
									  result->conn TSRMLS_CC);
				if (stmt->update_max_length) {
					for (i = 0; i < result->field_count; i++) {
						/*
						  NULL fields are 0 length, 0 is not more than 0
						  String of zero size, definitely can't be the next max_length.
						  Thus for NULL and zero-length we are quite efficient.
						*/
						if (Z_TYPE_P(current_row[i]) >= IS_STRING) {
							unsigned long len = Z_STRLEN_P(current_row[i]);
							if (meta->fields[i].max_length < len) {
								meta->fields[i].max_length = len;
							}
						}
					}
				}
			}

			for (i = 0; i < result->field_count; i++) {
				/* Clean what we copied last time */
#ifndef WE_DONT_COPY_IN_BUFFERED_AND_UNBUFFERED_BECAUSEOF_IS_REF
				if (stmt->result_bind[i].zv) {
					zval_dtor(stmt->result_bind[i].zv);
				}
#endif
				/* copy the type */
				if (stmt->result_bind[i].bound == TRUE) {
					DBG_INF_FMT("i=%d type=%d", i, Z_TYPE_P(current_row[i]));
					if (Z_TYPE_P(current_row[i]) != IS_NULL) {
						/*
						  Copy the value.
						  Pre-condition is that the zvals in the result_bind buffer
						  have been  ZVAL_NULL()-ed or to another simple type
						  (int, double, bool but not string). Because of the reference
						  counting the user can't delete the strings the variables point to.
						*/

						Z_TYPE_P(stmt->result_bind[i].zv) = Z_TYPE_P(current_row[i]);
						stmt->result_bind[i].zv->value = current_row[i]->value;
#ifndef WE_DONT_COPY_IN_BUFFERED_AND_UNBUFFERED_BECAUSEOF_IS_REF
						zval_copy_ctor(stmt->result_bind[i].zv);
#endif
					} else {
						ZVAL_NULL(stmt->result_bind[i].zv);
					}
				}
			}
		}
		set->data_cursor += field_count;
		*fetched_anything = TRUE;
		/* buffered result sets don't have a connection */
		MYSQLND_INC_GLOBAL_STATISTIC(STAT_ROWS_FETCHED_FROM_CLIENT_PS_BUF);
		DBG_INF("row fetched");
	} else {
		set->data_cursor = NULL;
		*fetched_anything = FALSE;
		DBG_INF("no more data");
	}
	DBG_INF("PASS");
	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ mysqlnd_stmt_fetch_row_unbuffered */
static enum_func_status
mysqlnd_stmt_fetch_row_unbuffered(MYSQLND_RES *result, void *param, unsigned int flags,
								zend_bool *fetched_anything TSRMLS_DC)
{
	enum_func_status ret;
	MYSQLND_STMT *stmt = (MYSQLND_STMT *) param;
	php_mysql_packet_row *row_packet = result->row_packet;

	DBG_ENTER("mysqlnd_stmt_fetch_row_unbuffered");

	if (result->unbuf->eof_reached) {
		/* No more rows obviously */
		*fetched_anything = FALSE;
		DBG_INF("eof reached");
		DBG_RETURN(PASS);
	}
	if (CONN_GET_STATE(result->conn) != CONN_FETCHING_DATA) {
		SET_CLIENT_ERROR(result->conn->error_info, CR_COMMANDS_OUT_OF_SYNC,
						 UNKNOWN_SQLSTATE, mysqlnd_out_of_sync);
		DBG_ERR("command out of sync");
		DBG_RETURN(FAIL);
	}
	/* Let the row packet fill our buffer and skip additional malloc + memcpy */
	row_packet->skip_extraction = stmt && stmt->result_bind? FALSE:TRUE;

	/*
	  If we skip rows (stmt == NULL || stmt->result_bind == NULL) we have to
	  mysqlnd_unbuffered_free_last_data() before it. The function returns always true.
	*/
	if (PASS == (ret = PACKET_READ(row_packet, result->conn)) && !row_packet->eof) {
		unsigned int i, field_count = result->field_count;
		result->unbuf->row_count++;
		*fetched_anything = TRUE;

		if (!row_packet->skip_extraction) {
			mysqlnd_unbuffered_free_last_data(result TSRMLS_CC);

			DBG_INF("extracting data");
			result->unbuf->last_row_data = row_packet->fields;
			result->unbuf->last_row_buffer = row_packet->row_buffer;
			row_packet->fields = NULL;
			row_packet->row_buffer = NULL;

			result->m.row_decoder(result->unbuf->last_row_buffer,
								  result->unbuf->last_row_data,
								  row_packet->field_count,
								  row_packet->fields_metadata,
								  result->conn TSRMLS_CC);

			for (i = 0; i < field_count; i++) {
				if (stmt->result_bind[i].bound == TRUE) {
					zval *data = result->unbuf->last_row_data[i];
					/*
					  stmt->result_bind[i].zv has been already destructed
					  in mysqlnd_unbuffered_free_last_data()
					*/

#ifndef WE_DONT_COPY_IN_BUFFERED_AND_UNBUFFERED_BECAUSEOF_IS_REF
					zval_dtor(stmt->result_bind[i].zv);
#endif
					if (IS_NULL != (Z_TYPE_P(stmt->result_bind[i].zv) = Z_TYPE_P(data)) ) {
						stmt->result_bind[i].zv->value = data->value;
#ifndef WE_DONT_COPY_IN_BUFFERED_AND_UNBUFFERED_BECAUSEOF_IS_REF
						zval_copy_ctor(stmt->result_bind[i].zv);
#endif						
						if (
							(Z_TYPE_P(data) == IS_STRING
#if PHP_MAJOR_VERSION >= 6
							|| Z_TYPE_P(data) == IS_UNICODE
#endif
							)
							 && (result->meta->fields[i].max_length < Z_STRLEN_P(data)))
						{
							result->meta->fields[i].max_length = Z_STRLEN_P(data);
						}
					}
				}
			}
			MYSQLND_INC_CONN_STATISTIC(&stmt->conn->stats, STAT_ROWS_FETCHED_FROM_CLIENT_PS_UNBUF);
		} else {
			DBG_INF("skipping extraction");
			/*
			  Data has been allocated and usually mysqlnd_unbuffered_free_last_data()
			  frees it but we can't call this function as it will cause problems with
			  the bound variables. Thus we need to do part of what it does or Zend will
			  report leaks.
			*/
			row_packet->row_buffer->free_chunk(row_packet->row_buffer, TRUE TSRMLS_CC);
			row_packet->row_buffer = NULL;
		}
	} else if (ret == FAIL) {
		if (row_packet->error_info.error_no) {
			stmt->conn->error_info = row_packet->error_info; 
			stmt->error_info = row_packet->error_info; 
		}
		*fetched_anything = FALSE;
		CONN_SET_STATE(result->conn, CONN_READY);
		result->unbuf->eof_reached = TRUE; /* so next time we won't get an error */	
	} else if (row_packet->eof) {
		DBG_INF("EOF");
		/* Mark the connection as usable again */
		result->unbuf->eof_reached = TRUE;
		result->conn->upsert_status.warning_count = row_packet->warning_count;
		result->conn->upsert_status.server_status = row_packet->server_status;
		/*
		  result->row_packet will be cleaned when
		  destroying the result object
		*/
		if (result->conn->upsert_status.server_status & SERVER_MORE_RESULTS_EXISTS) {
			CONN_SET_STATE(result->conn, CONN_NEXT_RESULT_PENDING);
		} else {
			CONN_SET_STATE(result->conn, CONN_READY);
		}
		*fetched_anything = FALSE;
	}

	DBG_INF_FMT("ret=%s fetched_anything=%d", ret == PASS? "PASS":"FAIL", *fetched_anything);
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_stmt::use_result */
static MYSQLND_RES *
MYSQLND_METHOD(mysqlnd_stmt, use_result)(MYSQLND_STMT *stmt TSRMLS_DC)
{
	MYSQLND_RES *result;
	MYSQLND *conn = stmt->conn;

	DBG_ENTER("mysqlnd_stmt::use_result");
	DBG_INF_FMT("stmt=%lu", stmt->stmt_id);

	if (!stmt->field_count ||
		(!stmt->cursor_exists && CONN_GET_STATE(conn) != CONN_FETCHING_DATA) ||
		(stmt->cursor_exists && CONN_GET_STATE(conn) != CONN_READY) ||
		(stmt->state != MYSQLND_STMT_WAITING_USE_OR_STORE))
	{
		SET_CLIENT_ERROR(conn->error_info, CR_COMMANDS_OUT_OF_SYNC,
						 UNKNOWN_SQLSTATE, mysqlnd_out_of_sync);
		DBG_ERR("command out of sync");
		DBG_RETURN(NULL);
	}

	SET_EMPTY_ERROR(stmt->error_info);

	MYSQLND_INC_CONN_STATISTIC(&stmt->conn->stats, STAT_PS_UNBUFFERED_SETS);
	result = stmt->result;
	
	result->m.use_result(stmt->result, TRUE TSRMLS_CC);
	result->m.fetch_row	= stmt->cursor_exists? mysqlnd_fetch_stmt_row_cursor:
											   mysqlnd_stmt_fetch_row_unbuffered;
	stmt->state = MYSQLND_STMT_USE_OR_STORE_CALLED;

	DBG_INF_FMT("%p", result);
	DBG_RETURN(result);
}
/* }}} */


#define STMT_ID_LENGTH 4

/* {{{ mysqlnd_fetch_row_cursor */
enum_func_status
mysqlnd_fetch_stmt_row_cursor(MYSQLND_RES *result, void *param, unsigned int flags,
							  zend_bool *fetched_anything TSRMLS_DC)
{
	enum_func_status ret;
	MYSQLND_STMT *stmt = (MYSQLND_STMT *) param;
	zend_uchar buf[STMT_ID_LENGTH /* statement id */ + 4 /* number of rows to fetch */];
	php_mysql_packet_row *row_packet = result->row_packet;

	DBG_ENTER("mysqlnd_fetch_stmt_row_cursor");
	DBG_INF_FMT("stmt=%lu flags=%u", stmt->stmt_id, flags);

	if (!stmt) {
		DBG_ERR("no statement");
		DBG_RETURN(FAIL);
	}

	if (stmt->state < MYSQLND_STMT_USER_FETCHING) {
		/* Only initted - error */
		SET_CLIENT_ERROR(stmt->conn->error_info, CR_COMMANDS_OUT_OF_SYNC, UNKNOWN_SQLSTATE,
						mysqlnd_out_of_sync);
		DBG_ERR("command out of sync");
		DBG_RETURN(FAIL);
	}

	SET_EMPTY_ERROR(stmt->error_info);
	SET_EMPTY_ERROR(stmt->conn->error_info);

	int4store(buf, stmt->stmt_id);
	int4store(buf + STMT_ID_LENGTH, 1); /* for now fetch only one row */

	if (FAIL == mysqlnd_simple_command(stmt->conn, COM_STMT_FETCH, (char *)buf, sizeof(buf),
									   PROT_LAST /* we will handle the response packet*/,
									   FALSE TSRMLS_CC)) {
		stmt->error_info = stmt->conn->error_info;
		DBG_RETURN(FAIL);
	}

	row_packet->skip_extraction = stmt->result_bind? FALSE:TRUE;

	if (PASS == (ret = PACKET_READ(row_packet, result->conn)) && !row_packet->eof) {
		unsigned int i, field_count = result->field_count;
		mysqlnd_unbuffered_free_last_data(result TSRMLS_CC);

		result->unbuf->last_row_data = row_packet->fields;
		result->unbuf->last_row_buffer = row_packet->row_buffer;


		row_packet->fields = NULL;
		row_packet->row_buffer = NULL;
		if (!row_packet->skip_extraction) {
			result->m.row_decoder(result->unbuf->last_row_buffer,
								  result->unbuf->last_row_data,
								  row_packet->field_count,
								  row_packet->fields_metadata,
								  result->conn TSRMLS_CC);

			/* If no result bind, do nothing. We consumed the data */
			for (i = 0; i < field_count; i++) {
				if (stmt->result_bind[i].bound == TRUE) {
					zval *data = result->unbuf->last_row_data[i];
					/*
					  stmt->result_bind[i].zv has been already destructed
					  in mysqlnd_unbuffered_free_last_data()
					*/
					DBG_INF_FMT("i=%d type=%d", i, Z_TYPE_P(stmt->result_bind[i].zv));
					if (IS_NULL != (Z_TYPE_P(stmt->result_bind[i].zv) = Z_TYPE_P(data)) ) {
						stmt->result_bind[i].zv->value = data->value;
#ifdef WE_DONT_COPY_IN_BUFFERED_AND_UNBUFFERED_BECAUSEOF_IS_REF
						zval_copy_ctor(stmt->result_bind[i].zv);
#endif
						if ((Z_TYPE_P(data) == IS_STRING
#if PHP_MAJOR_VERSION >= 6
							|| Z_TYPE_P(data) == IS_UNICODE
#endif
							)
							 && (result->meta->fields[i].max_length < Z_STRLEN_P(data)))
						{
							result->meta->fields[i].max_length = Z_STRLEN_P(data);
						}
					}
				}
			}
		}
		result->unbuf->row_count++;
		*fetched_anything = TRUE;
		/* We asked for one row, the next one should be EOF, eat it */
		ret = PACKET_READ(row_packet, result->conn);
		if (row_packet->row_buffer) {
			row_packet->row_buffer->free_chunk(row_packet->row_buffer, TRUE TSRMLS_CC);
			row_packet->row_buffer = NULL;
		}
		MYSQLND_INC_CONN_STATISTIC(&stmt->conn->stats, STAT_ROWS_FETCHED_FROM_CLIENT_PS_CURSOR);
	} else {
		*fetched_anything = FALSE;

		stmt->upsert_status.warning_count =
			stmt->conn->upsert_status.warning_count =
				row_packet->warning_count;

		stmt->upsert_status.server_status = 
			stmt->conn->upsert_status.server_status =
				row_packet->server_status;

		result->unbuf->eof_reached = row_packet->eof;
	}
	stmt->upsert_status.warning_count =
		stmt->conn->upsert_status.warning_count =
			row_packet->warning_count;
	stmt->upsert_status.server_status = 
		stmt->conn->upsert_status.server_status =
			row_packet->server_status;

	DBG_INF_FMT("ret=%s fetched=%d server_status=%d warnings=%d eof=%d",
				ret == PASS? "PASS":"FAIL", *fetched_anything,
				row_packet->server_status, row_packet->warning_count,
				result->unbuf->eof_reached);
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_stmt::fetch */
static enum_func_status
MYSQLND_METHOD(mysqlnd_stmt, fetch)(MYSQLND_STMT * const stmt,
									zend_bool * const fetched_anything TSRMLS_DC)
{
	enum_func_status ret;
	DBG_ENTER("mysqlnd_stmt::fetch");
	DBG_INF_FMT("stmt=%lu", stmt->stmt_id);

	if (!stmt->result ||
		stmt->state < MYSQLND_STMT_WAITING_USE_OR_STORE) {
		SET_STMT_ERROR(stmt, CR_COMMANDS_OUT_OF_SYNC, UNKNOWN_SQLSTATE, mysqlnd_out_of_sync);

		DBG_ERR("command out of sync");
		DBG_RETURN(FAIL);
	} else if (stmt->state == MYSQLND_STMT_WAITING_USE_OR_STORE) {
		/* Execute only once. We have to free the previous contents of user's bound vars */

		stmt->default_rset_handler(stmt TSRMLS_CC);
	}
	stmt->state = MYSQLND_STMT_USER_FETCHING;

	SET_EMPTY_ERROR(stmt->error_info);
	SET_EMPTY_ERROR(stmt->conn->error_info);

	DBG_INF_FMT("result_bind=%p separated_once=%d", stmt->result_bind, stmt->result_zvals_separated_once);
	/*
	  The user might have not bound any variables for result.
	  Do the binding once she does it.
	*/
	if (stmt->result_bind && !stmt->result_zvals_separated_once) {
		unsigned int i;
		/*
		  mysqlnd_stmt_store_result() has been called free the bind
		  variables to prevent leaking of their previous content.
		*/
		for (i = 0; i < stmt->result->field_count; i++) {
			if (stmt->result_bind[i].bound == TRUE) {
				zval_dtor(stmt->result_bind[i].zv);
				ZVAL_NULL(stmt->result_bind[i].zv);
			}
		}
		stmt->result_zvals_separated_once = TRUE;
	}

	ret = stmt->result->m.fetch_row(stmt->result, (void*)stmt, 0, fetched_anything TSRMLS_CC);
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_stmt::reset */
static enum_func_status
MYSQLND_METHOD(mysqlnd_stmt, reset)(MYSQLND_STMT * const stmt TSRMLS_DC)
{
	enum_func_status ret = PASS;
	zend_uchar cmd_buf[STMT_ID_LENGTH /* statement id */];

	DBG_ENTER("mysqlnd_stmt::reset");
	DBG_INF_FMT("stmt=%lu", stmt->stmt_id);

	SET_EMPTY_ERROR(stmt->error_info);
	SET_EMPTY_ERROR(stmt->conn->error_info);

	if (stmt->stmt_id) {
		MYSQLND * conn = stmt->conn;
		if (stmt->param_bind) {
			unsigned int i;
			DBG_INF("resetting long data");
			/* Reset Long Data */
			for (i = 0; i < stmt->param_count; i++) {
				if (stmt->param_bind[i].flags & MYSQLND_PARAM_BIND_BLOB_USED) {
					stmt->param_bind[i].flags &= ~MYSQLND_PARAM_BIND_BLOB_USED;
				}
			}
		}

		/*
		  If the user decided to close the statement right after execute()
		  We have to call the appropriate use_result() or store_result() and
		  clean.
		*/
		if (stmt->state == MYSQLND_STMT_WAITING_USE_OR_STORE) {
			DBG_INF("fetching result set header");
			stmt->default_rset_handler(stmt TSRMLS_CC);
			stmt->state = MYSQLND_STMT_USER_FETCHING;
		}

		if (stmt->result) {
			DBG_INF("skipping result");
			stmt->result->m.skip_result(stmt->result TSRMLS_CC);
		}

		/* Now the line should be free, if it wasn't */

		DBG_INF("freeing result");
		/* free_result() doesn't actually free stmt->result but only the buffers */
		stmt->m->free_result(stmt TSRMLS_CC);

		int4store(cmd_buf, stmt->stmt_id);
		if (CONN_GET_STATE(conn) == CONN_READY &&
			FAIL == (ret = mysqlnd_simple_command(conn, COM_STMT_RESET, (char *)cmd_buf,
												  sizeof(cmd_buf), PROT_OK_PACKET,
												  FALSE TSRMLS_CC))) {
			stmt->error_info = conn->error_info;
		}
		stmt->upsert_status = conn->upsert_status;

		stmt->state = MYSQLND_STMT_PREPARED;
	}
	DBG_INF(ret == PASS? "PASS":"FAIL");
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_stmt::send_long_data */
static enum_func_status
MYSQLND_METHOD(mysqlnd_stmt, send_long_data)(MYSQLND_STMT * const stmt, unsigned int param_no,
							 				 const char * const data, unsigned long length TSRMLS_DC)
{
	enum_func_status ret = FAIL;
	MYSQLND * conn = stmt->conn;
	zend_uchar *cmd_buf;
	enum php_mysqlnd_server_command cmd = COM_STMT_SEND_LONG_DATA;
	
	DBG_ENTER("mysqlnd_stmt::send_long_data");
	DBG_INF_FMT("stmt=%lu param_no=%d data_len=%lu", stmt->stmt_id, param_no, length);

	SET_EMPTY_ERROR(stmt->error_info);
	SET_EMPTY_ERROR(stmt->conn->error_info);

	if (stmt->state < MYSQLND_STMT_PREPARED) {
		SET_STMT_ERROR(stmt, CR_NO_PREPARE_STMT, UNKNOWN_SQLSTATE, mysqlnd_stmt_not_prepared);
		DBG_ERR("not prepared");
		DBG_RETURN(FAIL);
	}
	if (!stmt->param_bind) {
		SET_STMT_ERROR(stmt, CR_COMMANDS_OUT_OF_SYNC, UNKNOWN_SQLSTATE, mysqlnd_out_of_sync);
		DBG_ERR("command out of sync");
		DBG_RETURN(FAIL);
	}

	if (param_no >= stmt->param_count) {
		SET_STMT_ERROR(stmt, CR_INVALID_PARAMETER_NO, UNKNOWN_SQLSTATE, "Invalid parameter number");
		DBG_ERR("invalid param_no");
		DBG_RETURN(FAIL);
	}
	if (stmt->param_bind[param_no].type != MYSQL_TYPE_LONG_BLOB) {
		SET_STMT_ERROR(stmt, CR_INVALID_BUFFER_USE, UNKNOWN_SQLSTATE, mysqlnd_not_bound_as_blob);
		DBG_ERR("param_no is not of a blob type");
		DBG_RETURN(FAIL);
	}

	/*
	  XXX:	Unfortunately we have to allocate additional buffer to be able the
			additional data, which is like a header inside the payload.
			This should be optimised, but it will be a pervasive change, so
			mysqlnd_simple_command() will accept not a buffer, but actually MYSQLND_STRING*
			terminated by NULL, to send. If the strings are not big, we can collapse them
			on the buffer every connection has, but otherwise we will just send them
			one by one to the wire.
	*/

	if (CONN_GET_STATE(conn) == CONN_READY) {
		size_t packet_len;
		stmt->param_bind[param_no].flags |= MYSQLND_PARAM_BIND_BLOB_USED;
		cmd_buf = mnd_emalloc(packet_len = STMT_ID_LENGTH + 2 + length);

		int4store(cmd_buf, stmt->stmt_id);
		int2store(cmd_buf + STMT_ID_LENGTH, param_no);
		memcpy(cmd_buf + STMT_ID_LENGTH + 2, data, length);

		/* COM_STMT_SEND_LONG_DATA doesn't send an OK packet*/
		ret = mysqlnd_simple_command(conn, cmd, (char *)cmd_buf, packet_len,
									 PROT_LAST , FALSE TSRMLS_CC);
		mnd_efree(cmd_buf);
		if (FAIL == ret) {
			stmt->error_info = conn->error_info;
		}
		/*
		  Cover protocol error: COM_STMT_SEND_LONG_DATA was designed to be quick and not
		  sent response packets. According to documentation the only way to get an error
		  is to have out-of-memory on the server-side. However, that's not true, as if
		  max_allowed_packet_size is smaller than the chunk being sent to the server, the
		  latter will complain with an error message. However, normally we don't expect
		  an error message, thus we continue. When sending the next command, which expects
		  response we will read the unexpected data and error message will look weird.
		  Therefore we do non-blocking read to clean the line, if there is a need.
		  Nevertheless, there is a built-in protection when sending a command packet, that
		  checks if the line is clear - useful for debug purposes and to be switched off
		  in release builds.

		  Maybe we can make it automatic by checking what's the value of
		  max_allowed_packet_size on the server and resending the data.
		*/
#ifdef MYSQLND_DO_WIRE_CHECK_BEFORE_COMMAND
#if HAVE_USLEEP && !defined(PHP_WIN32)
		usleep(120000);
#endif
		if ((packet_len = php_mysqlnd_consume_uneaten_data(conn, cmd TSRMLS_CC))) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "There was an error "
							 "while sending long data. Probably max_allowed_packet_size "
							 "is smaller than the data. You have to increase it or send "
							 "smaller chunks of data. Answer was %u bytes long.", packet_len);
			SET_STMT_ERROR(stmt, CR_CONNECTION_ERROR, UNKNOWN_SQLSTATE,
							"Server responded to COM_STMT_SEND_LONG_DATA.");
			ret = FAIL;
		}
#endif
	}

	DBG_INF(ret == PASS? "PASS":"FAIL");
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_stmt::bind_parameters */
static enum_func_status
MYSQLND_METHOD(mysqlnd_stmt, bind_parameters)(MYSQLND_STMT * const stmt,
											  MYSQLND_PARAM_BIND * const param_bind TSRMLS_DC)
{
	DBG_ENTER("mysqlnd_stmt::bind_param");
	DBG_INF_FMT("stmt=%lu param_count=%u", stmt->stmt_id, stmt->param_count);

	if (stmt->state < MYSQLND_STMT_PREPARED) {
		SET_STMT_ERROR(stmt, CR_NO_PREPARE_STMT, UNKNOWN_SQLSTATE, mysqlnd_stmt_not_prepared);
		DBG_ERR("not prepared");
		if (param_bind && stmt->param_bind_dtor) {
			stmt->param_bind_dtor(param_bind);
		}
		DBG_RETURN(FAIL);
	}

	SET_EMPTY_ERROR(stmt->error_info);
	SET_EMPTY_ERROR(stmt->conn->error_info);

	if (stmt->param_count) {
		unsigned int i = 0;

		if (!param_bind) {
			SET_STMT_ERROR(stmt, CR_COMMANDS_OUT_OF_SYNC, UNKNOWN_SQLSTATE,
							 "Re-binding (still) not supported");
			DBG_ERR("Re-binding (still) not supported");
			DBG_RETURN(FAIL);
		} else if (stmt->param_bind) {
			DBG_INF("Binding");
			/*
			  There is already result bound.
			  Forbid for now re-binding!!
			*/
			for (i = 0; i < stmt->param_count; i++) {
				/*
				  We may have the last reference, then call zval_ptr_dtor()
				  or we may leak memory.
				  Switching from bind_one_parameter to bind_parameters may result in zv being NULL
				*/
				if (stmt->param_bind[i].zv) {
					zval_ptr_dtor(&stmt->param_bind[i].zv);
				}
			}
			if (stmt->param_bind != param_bind && stmt->param_bind_dtor) {
				stmt->param_bind_dtor(stmt->param_bind);
			}
		}

		stmt->param_bind = param_bind;
		for (i = 0; i < stmt->param_count; i++) {
			/* The client will use stmt_send_long_data */
			DBG_INF_FMT("%d is of type %d", i, stmt->param_bind[i].type);
			/* Prevent from freeing */
			/* Don't update is_ref, or we will leak during conversion */
			Z_ADDREF_P(stmt->param_bind[i].zv);
			if (stmt->param_bind[i].type == MYSQL_TYPE_LONG_BLOB) {
				stmt->param_bind[i].flags &= ~MYSQLND_PARAM_BIND_BLOB_USED;
			}
		}
		stmt->send_types_to_server = 1;
	}
	DBG_INF("PASS");
	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ mysqlnd_stmt::bind_one_parameter */
static enum_func_status
MYSQLND_METHOD(mysqlnd_stmt, bind_one_parameter)(MYSQLND_STMT * const stmt, unsigned int param_no,
												 zval * const zv, zend_uchar type TSRMLS_DC)
{
	DBG_ENTER("mysqlnd_stmt::bind_one_parameter");
	DBG_INF_FMT("stmt=%lu param_no=%d param_count=%u type=%d",
				stmt->stmt_id, param_no, stmt->param_count, type);

	if (stmt->state < MYSQLND_STMT_PREPARED) {
		SET_STMT_ERROR(stmt, CR_NO_PREPARE_STMT, UNKNOWN_SQLSTATE, mysqlnd_stmt_not_prepared);
		DBG_ERR("not prepared");
		DBG_RETURN(FAIL);
	}

	if (param_no < 0 || param_no >= stmt->param_count) {
		SET_STMT_ERROR(stmt, CR_INVALID_PARAMETER_NO, UNKNOWN_SQLSTATE, "Invalid parameter number");
		DBG_ERR("invalid param_no");
		DBG_RETURN(FAIL);
	}
	SET_EMPTY_ERROR(stmt->error_info);
	SET_EMPTY_ERROR(stmt->conn->error_info);

	if (stmt->param_count) {
		if (!stmt->param_bind) {
			stmt->param_bind = ecalloc(stmt->param_count, sizeof(MYSQLND_PARAM_BIND));
		}
		
		/* Prevent from freeing */
		/* Don't update is_ref, or we will leak during conversion */
		Z_ADDREF_P(zv);
		DBG_INF("Binding");
		/* Release what we had, if we had */
		if (stmt->param_bind[param_no].zv) {
			zval_ptr_dtor(&stmt->param_bind[param_no].zv);
		}
		if (type == MYSQL_TYPE_LONG_BLOB) {
			/* The client will use stmt_send_long_data */
			stmt->param_bind[param_no].flags &= ~MYSQLND_PARAM_BIND_BLOB_USED;
		}
		stmt->param_bind[param_no].zv = zv;
		stmt->param_bind[param_no].type = type;
		
		stmt->send_types_to_server = 1;
	}
	DBG_INF("PASS");
	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ mysqlnd_stmt::refresh_bind_param */
static enum_func_status
MYSQLND_METHOD(mysqlnd_stmt, refresh_bind_param)(MYSQLND_STMT * const stmt TSRMLS_DC)
{
	DBG_ENTER("mysqlnd_stmt::refresh_bind_param");
	DBG_INF_FMT("stmt=%lu param_count=%u", stmt->stmt_id, stmt->param_count);

	if (stmt->state < MYSQLND_STMT_PREPARED) {
		SET_STMT_ERROR(stmt, CR_NO_PREPARE_STMT, UNKNOWN_SQLSTATE, mysqlnd_stmt_not_prepared);
		DBG_ERR("not prepared");
		DBG_RETURN(FAIL);
	}

	SET_EMPTY_ERROR(stmt->error_info);
	SET_EMPTY_ERROR(stmt->conn->error_info);

	if (stmt->param_count) {
		stmt->send_types_to_server = 1;
	}
	DBG_INF("PASS");
	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ mysqlnd_stmt::set_bind_param_dtor */
static void
MYSQLND_METHOD(mysqlnd_stmt, set_param_bind_dtor)(MYSQLND_STMT * const stmt,
												  void (*param_bind_dtor)(MYSQLND_PARAM_BIND *dtor) TSRMLS_DC)
{
	DBG_ENTER("mysqlnd_stmt::set_bind_param_dtor");
	DBG_INF_FMT("stmt=%p", param_bind_dtor);
	stmt->param_bind_dtor = param_bind_dtor;
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_stmt::bind_result */
static enum_func_status
MYSQLND_METHOD(mysqlnd_stmt, bind_result)(MYSQLND_STMT * const stmt,
										  MYSQLND_RESULT_BIND * const result_bind TSRMLS_DC)
{
	DBG_ENTER("mysqlnd_stmt::bind_result");
	DBG_INF_FMT("stmt=%lu field_count=%u", stmt->stmt_id, stmt->field_count);


	if (stmt->state < MYSQLND_STMT_PREPARED) {
		SET_STMT_ERROR(stmt, CR_NO_PREPARE_STMT, UNKNOWN_SQLSTATE, mysqlnd_stmt_not_prepared);
		if (result_bind && stmt->result_bind_dtor) {
			stmt->result_bind_dtor(result_bind);
		}
		DBG_ERR("not prepared");
		DBG_RETURN(FAIL);
	}

	SET_EMPTY_ERROR(stmt->error_info);
	SET_EMPTY_ERROR(stmt->conn->error_info);

	if (stmt->field_count) {
		uint i = 0;

		if (!result_bind) {
			DBG_ERR("no result bind passed");
			DBG_RETURN(FAIL);
		}

		mysqlnd_stmt_separate_result_bind(stmt TSRMLS_CC);
		stmt->result_zvals_separated_once = FALSE;
		stmt->result_bind = result_bind;
		for (i = 0; i < stmt->field_count; i++) {
			/* Prevent from freeing */
			Z_ADDREF_P(stmt->result_bind[i].zv);		
			/*
			  Don't update is_ref !!! it's not our job
			  Otherwise either 009.phpt or mysqli_stmt_bind_result.phpt
			  will fail.
			*/
			stmt->result_bind[i].bound = TRUE;
		}
	} else if (result_bind && stmt->result_bind_dtor) {
		stmt->result_bind_dtor(result_bind);
	}
	DBG_INF("PASS");
	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ mysqlnd_stmt::bind_result */
static enum_func_status
MYSQLND_METHOD(mysqlnd_stmt, bind_one_result)(MYSQLND_STMT * const stmt, uint param_no TSRMLS_DC)
{
	DBG_ENTER("mysqlnd_stmt::bind_result");
	DBG_INF_FMT("stmt=%lu field_count=%u", stmt->stmt_id, stmt->field_count);

	if (stmt->state < MYSQLND_STMT_PREPARED) {
		SET_STMT_ERROR(stmt, CR_NO_PREPARE_STMT, UNKNOWN_SQLSTATE, mysqlnd_stmt_not_prepared);
		DBG_ERR("not prepared");
		DBG_RETURN(FAIL);
	}

	if (param_no < 0 || param_no >= stmt->field_count) {
		SET_STMT_ERROR(stmt, CR_INVALID_PARAMETER_NO, UNKNOWN_SQLSTATE, "Invalid parameter number");
		DBG_ERR("invalid param_no");
		DBG_RETURN(FAIL);
	}

	SET_EMPTY_ERROR(stmt->error_info);
	SET_EMPTY_ERROR(stmt->conn->error_info);

	if (stmt->field_count) {
		mysqlnd_stmt_separate_one_result_bind(stmt, param_no TSRMLS_CC);
		/* Guaranteed is that stmt->result_bind is NULL */
		if (!stmt->result_bind) {
			stmt->result_bind = ecalloc(stmt->field_count, sizeof(MYSQLND_RESULT_BIND));
		} else {
			stmt->result_bind = erealloc(stmt->result_bind, stmt->field_count * sizeof(MYSQLND_RESULT_BIND));
		}
		ALLOC_INIT_ZVAL(stmt->result_bind[param_no].zv);
		/*
		  Don't update is_ref !!! it's not our job
		  Otherwise either 009.phpt or mysqli_stmt_bind_result.phpt
		  will fail.
		*/	
		stmt->result_bind[param_no].bound = TRUE;
	}
	DBG_INF("PASS");
	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ mysqlnd_stmt::set_bind_result_dtor */
static void
MYSQLND_METHOD(mysqlnd_stmt, set_result_bind_dtor)(MYSQLND_STMT * const stmt,
												   void (*result_bind_dtor)(MYSQLND_RESULT_BIND *dtor) TSRMLS_DC)
{
	DBG_ENTER("mysqlnd_stmt::set_bind_param_dtor");
	DBG_INF_FMT("stmt=%p", result_bind_dtor);
	stmt->result_bind_dtor = result_bind_dtor;
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_stmt::insert_id */
static uint64
MYSQLND_METHOD(mysqlnd_stmt, insert_id)(const MYSQLND_STMT * const stmt)
{
	return stmt->upsert_status.last_insert_id;
}
/* }}} */


/* {{{ mysqlnd_stmt::affected_rows */
static uint64
MYSQLND_METHOD(mysqlnd_stmt, affected_rows)(const MYSQLND_STMT * const stmt)
{
	return stmt->upsert_status.affected_rows;
}
/* }}} */


/* {{{ mysqlnd_stmt::num_rows */
static uint64
MYSQLND_METHOD(mysqlnd_stmt, num_rows)(const MYSQLND_STMT * const stmt)
{
	return stmt->result? mysqlnd_num_rows(stmt->result):0;
}
/* }}} */


/* {{{ mysqlnd_stmt::warning_count */
static unsigned int
MYSQLND_METHOD(mysqlnd_stmt, warning_count)(const MYSQLND_STMT * const stmt)
{
	return stmt->upsert_status.warning_count;
}
/* }}} */


/* {{{ mysqlnd_stmt::field_count */
static unsigned int
MYSQLND_METHOD(mysqlnd_stmt, field_count)(const MYSQLND_STMT * const stmt)
{
	return stmt->field_count;
}
/* }}} */


/* {{{ mysqlnd_stmt::param_count */
static unsigned int
MYSQLND_METHOD(mysqlnd_stmt, param_count)(const MYSQLND_STMT * const stmt)
{
	return stmt->param_count;
}
/* }}} */


/* {{{ mysqlnd_stmt::errno */
static unsigned int
MYSQLND_METHOD(mysqlnd_stmt, errno)(const MYSQLND_STMT * const stmt)
{
	return stmt->error_info.error_no;
}
/* }}} */


/* {{{ mysqlnd_stmt::error */
static const char *
MYSQLND_METHOD(mysqlnd_stmt, error)(const MYSQLND_STMT * const stmt)
{
	return stmt->error_info.error;
}
/* }}} */


/* {{{ mysqlnd_stmt::sqlstate */
static const char *
MYSQLND_METHOD(mysqlnd_stmt, sqlstate)(const MYSQLND_STMT * const stmt)
{
	return stmt->error_info.sqlstate[0] ? stmt->error_info.sqlstate:MYSQLND_SQLSTATE_NULL;
}
/* }}} */


/* {{{ mysqlnd_stmt::data_seek */
static enum_func_status
MYSQLND_METHOD(mysqlnd_stmt, data_seek)(const MYSQLND_STMT * const stmt, uint64 row TSRMLS_DC)
{
	return stmt->result? stmt->result->m.seek_data(stmt->result, row TSRMLS_CC) : FAIL;
}
/* }}} */


/* {{{ mysqlnd_stmt::param_metadata */
static MYSQLND_RES *
MYSQLND_METHOD(mysqlnd_stmt, param_metadata)(MYSQLND_STMT * const stmt)
{
	if (!stmt->param_count) {
		return NULL;
	}

	return NULL;
}
/* }}} */


/* {{{ mysqlnd_stmt::result_metadata */
static MYSQLND_RES *
MYSQLND_METHOD(mysqlnd_stmt, result_metadata)(MYSQLND_STMT * const stmt TSRMLS_DC)
{
	MYSQLND_RES *result;

	DBG_ENTER("mysqlnd_stmt::result_metadata");
	DBG_INF_FMT("stmt=%u field_count=%u", stmt->stmt_id, stmt->field_count);

	if (!stmt->field_count || !stmt->conn || !stmt->result || !stmt->result->meta) {
		DBG_INF("NULL");
		DBG_RETURN(NULL);
	}

	if (stmt->update_max_length && stmt->result->stored_data) {
		/* stored result, we have to update the max_length before we clone the meta data :( */
		mysqlnd_res_initialize_result_set_rest(stmt->result TSRMLS_CC);
	}
	/*
	  TODO: This implementation is kind of a hack,
			find a better way to do it. In different functions I have put
			fuses to check for result->m.fetch_row() being NULL. This should
			be handled in a better way.

	  In the meantime we don't need a zval cache reference for this fake
	  result set, so we don't get one.
	*/
	result = mysqlnd_result_init(stmt->field_count, NULL TSRMLS_CC);
	result->type = MYSQLND_RES_NORMAL;
	result->m.fetch_row = result->m.fetch_row_normal_unbuffered;
	result->unbuf = mnd_ecalloc(1, sizeof(MYSQLND_RES_UNBUFFERED));
	result->unbuf->eof_reached = TRUE;
	result->meta = stmt->result->meta->m->clone_metadata(stmt->result->meta, FALSE TSRMLS_CC);

	DBG_INF_FMT("result=%p", result);
	DBG_RETURN(result);
}
/* }}} */


/* {{{ mysqlnd_stmt::attr_set */
static enum_func_status
MYSQLND_METHOD(mysqlnd_stmt, attr_set)(MYSQLND_STMT * const stmt,
									   enum mysqlnd_stmt_attr attr_type,
									   const void * const value TSRMLS_DC)
{
	unsigned long val = *(unsigned long *) value;
	DBG_ENTER("mysqlnd_stmt::attr_set");
	DBG_INF_FMT("stmt=%lu attr_type=%u value=%lu", stmt->stmt_id, attr_type, val);

	switch (attr_type) {
		case STMT_ATTR_UPDATE_MAX_LENGTH:
			/*
			  XXX : libmysql uses my_bool, but mysqli uses ulong as storage on the stack
			  and mysqlnd won't be used out of the scope of PHP -> use ulong.
			*/
			stmt->update_max_length = val? TRUE:FALSE;
			break;
		case STMT_ATTR_CURSOR_TYPE: {
			if (val > (unsigned long) CURSOR_TYPE_READ_ONLY) {
				SET_STMT_ERROR(stmt, CR_NOT_IMPLEMENTED, UNKNOWN_SQLSTATE, "Not implemented");
				DBG_INF("FAIL");
				DBG_RETURN(FAIL);
			}
			stmt->flags = val;
			break;
		}
		case STMT_ATTR_PREFETCH_ROWS: {
			if (val == 0) {
				val = MYSQLND_DEFAULT_PREFETCH_ROWS;
			} else if (val > 1) {
				SET_STMT_ERROR(stmt, CR_NOT_IMPLEMENTED, UNKNOWN_SQLSTATE, "Not implemented");
				DBG_INF("FAIL");
				DBG_RETURN(FAIL);
			}
			stmt->prefetch_rows = val;
			break;
		}
		default:
			SET_STMT_ERROR(stmt, CR_NOT_IMPLEMENTED, UNKNOWN_SQLSTATE, "Not implemented");
			DBG_RETURN(FAIL);
	}
	DBG_INF("PASS");
	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ mysqlnd_stmt::attr_get */
static enum_func_status
MYSQLND_METHOD(mysqlnd_stmt, attr_get)(const MYSQLND_STMT * const stmt,
									   enum mysqlnd_stmt_attr attr_type,
									   void * const value TSRMLS_DC)
{
	DBG_ENTER("mysqlnd_stmt::attr_set");
	DBG_INF_FMT("stmt=%lu attr_type=%u", stmt->stmt_id, attr_type);

	switch (attr_type) {
		case STMT_ATTR_UPDATE_MAX_LENGTH:
			*(zend_bool *) value= stmt->update_max_length;
			break;
		case STMT_ATTR_CURSOR_TYPE:
			*(unsigned long *) value= stmt->flags;
			break;
		case STMT_ATTR_PREFETCH_ROWS:
			*(unsigned long *) value= stmt->prefetch_rows;
			break;
		default:
			DBG_RETURN(FAIL);
	}
	DBG_INF_FMT("value=%lu", value);
	DBG_RETURN(PASS);
}
/* }}} */

/* free_result() doesn't actually free stmt->result but only the buffers */
/* {{{ mysqlnd_stmt::free_result */
static enum_func_status
MYSQLND_METHOD(mysqlnd_stmt, free_result)(MYSQLND_STMT * const stmt TSRMLS_DC)
{
	DBG_ENTER("mysqlnd_stmt::free_result");
	DBG_INF_FMT("stmt=%lu", stmt->stmt_id);

	if (!stmt->result) {
		DBG_INF("no result");
		DBG_RETURN(PASS);
	}

	/*
	  If right after execute() we have to call the appropriate
	  use_result() or store_result() and clean.
	*/
	if (stmt->state == MYSQLND_STMT_WAITING_USE_OR_STORE) {
		DBG_INF("fetching result set header");
		/* Do implicit use_result and then flush the result */
		stmt->default_rset_handler = stmt->m->use_result;
		stmt->default_rset_handler(stmt TSRMLS_CC);
	}

	if (stmt->state > MYSQLND_STMT_WAITING_USE_OR_STORE) {
		DBG_INF("skipping result");
		/* Flush if anything is left and unbuffered set */
		stmt->result->m.skip_result(stmt->result TSRMLS_CC);
		/*
		  Separate the bound variables, which point to the result set, then
		  destroy the set.
		*/
		mysqlnd_stmt_separate_result_bind(stmt TSRMLS_CC);

		/* Now we can destroy the result set */
		stmt->result->m.free_result_buffers(stmt->result TSRMLS_CC);
	}

	if (stmt->state > MYSQLND_STMT_PREPARED) {
		/* As the buffers have been freed, we should go back to PREPARED */
		stmt->state = MYSQLND_STMT_PREPARED;
	}

	/* Line is free! */
	CONN_SET_STATE(stmt->conn, CONN_READY);

	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ mysqlnd_stmt_separate_result_bind */
void mysqlnd_stmt_separate_result_bind(MYSQLND_STMT * const stmt TSRMLS_DC)
{
	unsigned int i;

	DBG_ENTER("mysqlnd_stmt_separate_result_bind");
	DBG_INF_FMT("stmt=%lu result_bind=%p field_count=%u",
				stmt->stmt_id, stmt->result_bind, stmt->field_count);

	if (!stmt->result_bind) {
		DBG_VOID_RETURN;
	}

	/*
	  Because only the bound variables can point to our internal buffers, then
	  separate or free only them. Free is possible because the user could have
	  lost reference.
	*/
	for (i = 0; i < stmt->field_count; i++) {
		/* Let's try with no cache */
		if (stmt->result_bind[i].bound == TRUE) {
			DBG_INF_FMT("%d has refcount=%u", i, Z_REFCOUNT_P(stmt->result_bind[i].zv));
			/*
			  We have to separate the actual zval value of the bound
			  variable from our allocated zvals or we will face double-free
			*/
			if (Z_REFCOUNT_P(stmt->result_bind[i].zv) > 1) {
#ifdef WE_DONT_COPY_IN_BUFFERED_AND_UNBUFFERED_BECAUSEOF_IS_REF
				zval_copy_ctor(stmt->result_bind[i].zv);
#endif
				zval_ptr_dtor(&stmt->result_bind[i].zv);
			} else {
				/*
				  If it is a string, what is pointed will be freed
				  later in free_result(). We need to remove the variable to
				  which the user has lost reference.
				*/
#ifdef WE_DONT_COPY_IN_BUFFERED_AND_UNBUFFERED_BECAUSEOF_IS_REF
				ZVAL_NULL(stmt->result_bind[i].zv);
#endif
				zval_ptr_dtor(&stmt->result_bind[i].zv);
			}
		}
	}
	if (stmt->result_bind_dtor) {
		stmt->result_bind_dtor(stmt->result_bind);
	}
	stmt->result_bind = NULL;

	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_stmt_separate_one_result_bind */
void mysqlnd_stmt_separate_one_result_bind(MYSQLND_STMT * const stmt, uint param_no TSRMLS_DC)
{
	DBG_ENTER("mysqlnd_stmt_separate_one_result_bind");
	DBG_INF_FMT("stmt=%lu result_bind=%p field_count=%u param_no=%d",
				stmt->stmt_id, stmt->result_bind, stmt->field_count, param_no);

	if (!stmt->result_bind) {
		DBG_VOID_RETURN;
	}

	/*
	  Because only the bound variables can point to our internal buffers, then
	  separate or free only them. Free is possible because the user could have
	  lost reference.
	*/
	/* Let's try with no cache */
	if (stmt->result_bind[param_no].bound == TRUE) {
		DBG_INF_FMT("%d has refcount=%u", param_no, Z_REFCOUNT_P(stmt->result_bind[param_no].zv));
		/*
		  We have to separate the actual zval value of the bound
		  variable from our allocated zvals or we will face double-free
		*/
		if (Z_REFCOUNT_P(stmt->result_bind[param_no].zv) > 1) {
#ifdef WE_DONT_COPY_IN_BUFFERED_AND_UNBUFFERED_BECAUSEOF_IS_REF
			zval_copy_ctor(stmt->result_bind[param_no].zv);
#endif
			zval_ptr_dtor(&stmt->result_bind[param_no].zv);
		} else {
			/*
			  If it is a string, what is pointed will be freed
			  later in free_result(). We need to remove the variable to
			  which the user has lost reference.
			*/
#ifdef WE_DONT_COPY_IN_BUFFERED_AND_UNBUFFERED_BECAUSEOF_IS_REF
			ZVAL_NULL(stmt->result_bind[param_no].zv);
#endif
			zval_ptr_dtor(&stmt->result_bind[param_no].zv);
		}
	}

	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_internal_free_stmt_content */
static
void mysqlnd_internal_free_stmt_content(MYSQLND_STMT * const stmt TSRMLS_DC)
{
	DBG_ENTER("mysqlnd_internal_free_stmt_content");
	DBG_INF_FMT("stmt=%lu param_bind=%p param_count=%u",
				stmt->stmt_id, stmt->param_bind, stmt->param_count);

	/* Destroy the input bind */
	if (stmt->param_bind) {
		unsigned int i;
		/*
		  Because only the bound variables can point to our internal buffers, then
		  separate or free only them. Free is possible because the user could have
		  lost reference.
		*/
		for (i = 0; i < stmt->param_count; i++) {
			/*
			  If bind_one_parameter was used, but not everything was
			  bound and nothing was fetched, then some `zv` could be NULL
			*/
			if (stmt->param_bind[i].zv) {
				zval_ptr_dtor(&stmt->param_bind[i].zv);
			}
		}
		if (stmt->param_bind_dtor) {
			stmt->param_bind_dtor(stmt->param_bind);
		}
		stmt->param_bind = NULL;
	}

	/*
	  First separate the bound variables, which point to the result set, then
	  destroy the set.
	*/
	mysqlnd_stmt_separate_result_bind(stmt TSRMLS_CC);
	/* Not every statement has a result set attached */
	if (stmt->result) {
		stmt->result->m.free_result_internal(stmt->result TSRMLS_CC);
		stmt->result = NULL;
	}

	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_stmt::net_close */
static enum_func_status
MYSQLND_METHOD_PRIVATE(mysqlnd_stmt, net_close)(MYSQLND_STMT * const stmt, zend_bool implicit TSRMLS_DC)
{
	MYSQLND * conn = stmt->conn;
	zend_uchar cmd_buf[STMT_ID_LENGTH /* statement id */];
	enum_mysqlnd_collected_stats stat = STAT_LAST;

	DBG_ENTER("mysqlnd_stmt::net_close");
	DBG_INF_FMT("stmt=%lu", stmt->stmt_id);

	SET_EMPTY_ERROR(stmt->error_info);
	SET_EMPTY_ERROR(stmt->conn->error_info);

	/*
	  If the user decided to close the statement right after execute()
	  We have to call the appropriate use_result() or store_result() and
	  clean.
	*/
	do {
		DBG_INF_FMT("stmt->state=%d", stmt->state);
		if (stmt->state == MYSQLND_STMT_WAITING_USE_OR_STORE) {
			DBG_INF("fetching result set header");
			stmt->default_rset_handler(stmt TSRMLS_CC);
			stmt->state = MYSQLND_STMT_USER_FETCHING;
		}

		/* unbuffered set not fetched to the end ? Clean the line */
		if (stmt->result) {
			DBG_INF("skipping result");
			stmt->result->m.skip_result(stmt->result TSRMLS_CC);
		}
	} while (mysqlnd_stmt_more_results(stmt) && mysqlnd_stmt_next_result(stmt));
	/*
	  After this point we are allowed to free the result set,
	  as we have cleaned the line
	*/
	if (stmt->stmt_id) {
		MYSQLND_INC_GLOBAL_STATISTIC(implicit == TRUE?	STAT_FREE_RESULT_IMPLICIT:
														STAT_FREE_RESULT_EXPLICIT);

		int4store(cmd_buf, stmt->stmt_id);
		if (CONN_GET_STATE(conn) == CONN_READY &&
			FAIL == mysqlnd_simple_command(conn, COM_STMT_CLOSE, (char *)cmd_buf, sizeof(cmd_buf),
										   PROT_LAST /* COM_STMT_CLOSE doesn't send an OK packet*/,
										   FALSE TSRMLS_CC)) {
			stmt->error_info = conn->error_info;
			DBG_RETURN(FAIL);
		}
	}
	switch (stmt->execute_count) {
		case 0:
			stat = STAT_PS_PREPARED_NEVER_EXECUTED;
			break;
		case 1:
			stat = STAT_PS_PREPARED_ONCE_USED;
			break;
		default:
			break;
	}
	if (stat != STAT_LAST) {
		MYSQLND_INC_CONN_STATISTIC(&conn->stats, stat);
	}

	if (stmt->execute_cmd_buffer.buffer) {
		mnd_efree(stmt->execute_cmd_buffer.buffer);
		stmt->execute_cmd_buffer.buffer = NULL;
	}

	mysqlnd_internal_free_stmt_content(stmt TSRMLS_CC);

	if (stmt->conn) {
		stmt->conn->m->free_reference(stmt->conn TSRMLS_CC);
		stmt->conn = NULL;
	}

	DBG_RETURN(PASS);
}
/* }}} */

/* {{{ mysqlnd_stmt::dtor */
static enum_func_status
MYSQLND_METHOD(mysqlnd_stmt, dtor)(MYSQLND_STMT * const stmt, zend_bool implicit TSRMLS_DC)
{
	enum_func_status ret;

	DBG_ENTER("mysqlnd_stmt::dtor");
	DBG_INF_FMT("stmt=%p", stmt);

	MYSQLND_INC_GLOBAL_STATISTIC(implicit == TRUE?	STAT_STMT_CLOSE_IMPLICIT:
													STAT_STMT_CLOSE_EXPLICIT);

	ret = stmt->m->net_close(stmt, implicit TSRMLS_CC);
	mnd_efree(stmt);

	DBG_INF(ret == PASS? "PASS":"FAIL");
	DBG_RETURN(ret);
}
/* }}} */


static
struct st_mysqlnd_stmt_methods mysqlnd_stmt_methods = {
	MYSQLND_METHOD(mysqlnd_stmt, prepare),
	MYSQLND_METHOD(mysqlnd_stmt, execute),
	MYSQLND_METHOD(mysqlnd_stmt, use_result),
	MYSQLND_METHOD(mysqlnd_stmt, store_result),
	MYSQLND_METHOD(mysqlnd_stmt, background_store_result),
	MYSQLND_METHOD(mysqlnd_stmt, get_result),
	MYSQLND_METHOD(mysqlnd_stmt, more_results),
	MYSQLND_METHOD(mysqlnd_stmt, next_result),
	MYSQLND_METHOD(mysqlnd_stmt, free_result),
	MYSQLND_METHOD(mysqlnd_stmt, data_seek),
	MYSQLND_METHOD(mysqlnd_stmt, reset),
	MYSQLND_METHOD_PRIVATE(mysqlnd_stmt, net_close),
	MYSQLND_METHOD(mysqlnd_stmt, dtor),

	MYSQLND_METHOD(mysqlnd_stmt, fetch),

	MYSQLND_METHOD(mysqlnd_stmt, bind_parameters),
	MYSQLND_METHOD(mysqlnd_stmt, bind_one_parameter),
	MYSQLND_METHOD(mysqlnd_stmt, refresh_bind_param),
	MYSQLND_METHOD(mysqlnd_stmt, set_param_bind_dtor),
	MYSQLND_METHOD(mysqlnd_stmt, bind_result),
	MYSQLND_METHOD(mysqlnd_stmt, bind_one_result),
	MYSQLND_METHOD(mysqlnd_stmt, set_result_bind_dtor),
	MYSQLND_METHOD(mysqlnd_stmt, send_long_data),
	MYSQLND_METHOD(mysqlnd_stmt, param_metadata),
	MYSQLND_METHOD(mysqlnd_stmt, result_metadata),

	MYSQLND_METHOD(mysqlnd_stmt, insert_id),
	MYSQLND_METHOD(mysqlnd_stmt, affected_rows),
	MYSQLND_METHOD(mysqlnd_stmt, num_rows),

	MYSQLND_METHOD(mysqlnd_stmt, param_count),
	MYSQLND_METHOD(mysqlnd_stmt, field_count),
	MYSQLND_METHOD(mysqlnd_stmt, warning_count),

	MYSQLND_METHOD(mysqlnd_stmt, errno),
	MYSQLND_METHOD(mysqlnd_stmt, error),
	MYSQLND_METHOD(mysqlnd_stmt, sqlstate),

	MYSQLND_METHOD(mysqlnd_stmt, attr_get),
	MYSQLND_METHOD(mysqlnd_stmt, attr_set),	
};


/* {{{ _mysqlnd_stmt_init */
MYSQLND_STMT * _mysqlnd_stmt_init(MYSQLND * const conn TSRMLS_DC)
{
	MYSQLND_STMT *stmt = mnd_ecalloc(1, sizeof(MYSQLND_STMT));

	DBG_ENTER("_mysqlnd_stmt_init");
	DBG_INF_FMT("stmt=%p", stmt); 

	stmt->m = &mysqlnd_stmt_methods;
	stmt->state = MYSQLND_STMT_INITTED;
	stmt->execute_cmd_buffer.length = 4096;
	stmt->execute_cmd_buffer.buffer = mnd_emalloc(stmt->execute_cmd_buffer.length);

	stmt->prefetch_rows = MYSQLND_DEFAULT_PREFETCH_ROWS;
	/*
	  Mark that we reference the connection, thus it won't be
	  be destructed till there is open statements. The last statement
	  or normal query result will close it then.
	*/
	stmt->conn = conn->m->get_reference(conn TSRMLS_CC);

	stmt->m->set_param_bind_dtor(stmt, mysqlnd_efree_param_bind_dtor TSRMLS_CC); 
	stmt->m->set_result_bind_dtor(stmt, mysqlnd_efree_result_bind_dtor TSRMLS_CC); 

	DBG_RETURN(stmt);
}
/* }}} */


/* {{{ mysqlnd_efree_param_bind_dtor */
PHPAPI void
mysqlnd_efree_param_bind_dtor(MYSQLND_PARAM_BIND * param_bind)
{
	efree(param_bind);
}
/* }}} */


/* {{{ mysqlnd_efree_result_bind_dtor */
PHPAPI void
mysqlnd_efree_result_bind_dtor(MYSQLND_RESULT_BIND * result_bind)
{
	efree(result_bind);
}
/* }}} */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
