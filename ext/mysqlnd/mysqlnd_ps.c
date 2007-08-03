/*
  +----------------------------------------------------------------------+
  | PHP Version 6                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2006-2007 The PHP Group                                |
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
												  zend_bool *free_buffer);


MYSQLND_RES * _mysqlnd_stmt_use_result(MYSQLND_STMT *stmt TSRMLS_DC);

enum_func_status mysqlnd_fetch_stmt_row_buffered(MYSQLND_RES *result, void *param,
												unsigned int flags,
												zend_bool *fetched_anything TSRMLS_DC);

enum_func_status mysqlnd_fetch_stmt_row_cursor(MYSQLND_RES *result, void *param,
											   unsigned int flags,
											   zend_bool *fetched_anything TSRMLS_DC);

void mysqlnd_stmt_separate_result_bind(MYSQLND_STMT * const stmt);


/* {{{ mysqlnd_stmt::store_result */
static MYSQLND_RES *
MYSQLND_METHOD(mysqlnd_stmt, store_result)(MYSQLND_STMT * const stmt TSRMLS_DC)
{
	enum_func_status ret;
	MYSQLND *conn = stmt->conn;
	MYSQLND_RES *result;
	zend_bool to_cache = FALSE;

	/* be compliant with libmysql - NULL will turn */
	if (!stmt->field_count) {
		return NULL;
	}

	if (stmt->cursor_exists) {
		/* Silently convert buffered to unbuffered, for now */
		return stmt->m->use_result(stmt TSRMLS_CC);
	}


	/* Nothing to store for UPSERT/LOAD DATA*/
	if (conn->state != CONN_FETCHING_DATA ||
		stmt->state != MYSQLND_STMT_WAITING_USE_OR_STORE)
	{
		SET_CLIENT_ERROR(conn->error_info, CR_COMMANDS_OUT_OF_SYNC,
						 UNKNOWN_SQLSTATE, mysqlnd_out_of_sync);
		return NULL;
	}

	stmt->default_rset_handler = stmt->m->store_result;

	SET_EMPTY_ERROR(stmt->error_info);
	SET_EMPTY_ERROR(stmt->conn->error_info);
	MYSQLND_INC_CONN_STATISTIC(&conn->stats, STAT_PS_BUFFERED_SETS);

	result = stmt->result;
	result->type			= MYSQLND_RES_PS_BUF;
	result->m.fetch_row		= mysqlnd_fetch_stmt_row_buffered;
	result->m.fetch_lengths	= NULL;/* makes no sense */
	result->zval_cache		= mysqlnd_palloc_get_thd_cache_reference(conn->zval_cache);

	/* Create room for 'next_extend' rows */

	/* Not set for SHOW statements at PREPARE stage */
	if (result->conn) {
		result->conn->m->free_reference(result->conn TSRMLS_CC);
		result->conn = NULL;	/* store result does not reference  the connection */
	}

	ret = mysqlnd_store_result_fetch_data(conn, result, result->meta,
										  TRUE, stmt->update_max_length,
										  to_cache TSRMLS_CC);

	if (PASS == ret) {
		/* libmysql API docs say it should be so for SELECT statements */
		stmt->upsert_status.affected_rows = stmt->result->data->row_count;

		stmt->state = MYSQLND_STMT_USE_OR_STORE_CALLED;
	} else {
		conn->error_info = result->data->error_info;
		stmt->result->m.free_result_contents(stmt->result TSRMLS_CC);
		efree(stmt->result);
		stmt->result = NULL;
		stmt->state = MYSQLND_STMT_PREPARED;
	}

	return result;
}
/* }}} */


/* {{{ mysqlnd_stmt::get_result */
static MYSQLND_RES *
MYSQLND_METHOD(mysqlnd_stmt, get_result)(MYSQLND_STMT * const stmt TSRMLS_DC)
{
	MYSQLND *conn = stmt->conn;
	MYSQLND_RES *result;

	/* be compliant with libmysql - NULL will turn */
	if (!stmt->field_count) {
		return NULL;
	}

	if (stmt->cursor_exists) {
		/* Silently convert buffered to unbuffered, for now */
		return stmt->m->use_result(stmt TSRMLS_CC);
	}

	/* Nothing to store for UPSERT/LOAD DATA*/
	if (conn->state != CONN_FETCHING_DATA ||
		stmt->state != MYSQLND_STMT_WAITING_USE_OR_STORE)
	{
		SET_CLIENT_ERROR(conn->error_info, CR_COMMANDS_OUT_OF_SYNC,
						 UNKNOWN_SQLSTATE, mysqlnd_out_of_sync);
		return NULL;
	}

	SET_EMPTY_ERROR(stmt->error_info);
	SET_EMPTY_ERROR(stmt->conn->error_info);
	MYSQLND_INC_CONN_STATISTIC(&conn->stats, STAT_BUFFERED_SETS);

	result = mysqlnd_result_init(stmt->result->field_count,
								 mysqlnd_palloc_get_thd_cache_reference(conn->zval_cache));	

	result->meta = stmt->result->meta->m->clone_metadata(stmt->result->meta, FALSE);

	/* Not set for SHOW statements at PREPARE stage */
	if (stmt->result->conn) {
		stmt->result->conn->m->free_reference(stmt->result->conn TSRMLS_CC);
		stmt->result->conn = NULL;	/* store result does not reference  the connection */
	}

	if ((result = result->m.store_result(result, conn, TRUE TSRMLS_CC))) {
		stmt->upsert_status.affected_rows = result->data->row_count;	
		stmt->state = MYSQLND_STMT_PREPARED;
		result->type = MYSQLND_RES_PS_BUF;
	} else {
		stmt->error_info = conn->error_info;
		stmt->state = MYSQLND_STMT_PREPARED;
	}

	return result;
}
/* }}} */


/* {{{ mysqlnd_stmt_skip_metadata */
static enum_func_status
mysqlnd_stmt_skip_metadata(MYSQLND_STMT *stmt TSRMLS_DC)
{
	/* Follows parameter metadata, we have just to skip it, as libmysql does */
	unsigned int i = 0;
	php_mysql_packet_res_field field_packet;

	PACKET_INIT_ALLOCA(field_packet, PROT_RSET_FLD_PACKET);
	field_packet.skip_parsing = TRUE;
	for (;i < stmt->param_count; i++) {
		if (FAIL == PACKET_READ_ALLOCA(field_packet, stmt->conn)) {
			PACKET_FREE_ALLOCA(field_packet);
			return FAIL;
		}
	}
	PACKET_FREE_ALLOCA(field_packet);
	return PASS;
}
/* }}} */


/* {{{ mysqlnd_stmt_read_prepare_response */
static enum_func_status
mysqlnd_stmt_read_prepare_response(MYSQLND_STMT *stmt TSRMLS_DC)
{
	php_mysql_packet_prepare_response prepare_resp;

	PACKET_INIT_ALLOCA(prepare_resp, PROT_PREPARE_RESP_PACKET);
	if (FAIL == PACKET_READ_ALLOCA(prepare_resp, stmt->conn)) {
		PACKET_FREE_ALLOCA(prepare_resp);
		return FAIL;
	}

	if (0xFF == prepare_resp.error_code) {
		stmt->error_info = stmt->conn->error_info = prepare_resp.error_info;
		return FAIL;
	}

	stmt->stmt_id = prepare_resp.stmt_id;
	stmt->warning_count = stmt->conn->upsert_status.warning_count = prepare_resp.warning_count;
	stmt->field_count = stmt->conn->field_count = prepare_resp.field_count;
	stmt->param_count = prepare_resp.param_count;
	PACKET_FREE_ALLOCA(prepare_resp);

	return PASS;
}
/* }}} */


/* {{{ mysqlnd_stmt_prepare_read_eof */
static enum_func_status
mysqlnd_stmt_prepare_read_eof(MYSQLND_STMT *stmt TSRMLS_DC)
{
	php_mysql_packet_eof fields_eof;
	enum_func_status ret;

	PACKET_INIT_ALLOCA(fields_eof, PROT_EOF_PACKET);
	if (FAIL == (ret = PACKET_READ_ALLOCA(fields_eof, stmt->conn))) {
		if (stmt->result) {
			stmt->result->m.free_result_contents(stmt->result TSRMLS_CC);
			efree(stmt->result);
			memset(stmt, 0, sizeof(MYSQLND_STMT));
			stmt->state = MYSQLND_STMT_INITTED;
		}
	} else {
		stmt->upsert_status.server_status = fields_eof.server_status;
		stmt->upsert_status.warning_count = fields_eof.warning_count;
		stmt->state = MYSQLND_STMT_PREPARED;
	}
	PACKET_FREE_ALLOCA(fields_eof);

	return ret;
}
/* }}} */


/* {{{ mysqlnd_stmt::prepare */
static enum_func_status
MYSQLND_METHOD(mysqlnd_stmt, prepare)(MYSQLND_STMT * const stmt, const char * const query,
									  unsigned int query_len TSRMLS_DC)
{
	MYSQLND_STMT *stmt_to_prepare = stmt;

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
		MYSQLND_RES *result = mysqlnd_result_init(stmt_to_prepare->field_count, NULL);
		/* Allocate the result now as it is needed for the reading of metadata */
		stmt_to_prepare->result = result; 

		result->conn = stmt_to_prepare->conn->m->get_reference(stmt_to_prepare->conn);

		result->type = MYSQLND_RES_PS_BUF;

		if (FAIL == result->m.read_result_metadata(result, stmt_to_prepare->conn TSRMLS_CC) ||
			FAIL == mysqlnd_stmt_prepare_read_eof(stmt_to_prepare TSRMLS_CC)) {
			goto fail;
		}
	}

	if (stmt_to_prepare != stmt) {
		/* Free old buffers, binding and resources on server */
		stmt->m->close(stmt, TRUE TSRMLS_CC);

		memcpy(stmt, stmt_to_prepare, sizeof(MYSQLND_STMT));

		/* Now we will have a clean new statement object */
		efree(stmt_to_prepare);
	}
	stmt->state = MYSQLND_STMT_PREPARED;
	return PASS;

fail:
	if (stmt_to_prepare != stmt) {
		stmt_to_prepare->m->dtor(stmt_to_prepare, TRUE TSRMLS_CC);
	}
	stmt->state = MYSQLND_STMT_INITTED;

	return FAIL;
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

	SET_ERROR_AFF_ROWS(stmt);
	SET_ERROR_AFF_ROWS(stmt->conn);
	
	if (stmt->state > MYSQLND_STMT_PREPARED && stmt->field_count) {
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
		return FAIL;
	}

	if (stmt->param_count && !stmt->param_bind) {
		SET_STMT_ERROR(stmt, CR_PARAMS_NOT_BOUND, UNKNOWN_SQLSTATE,
						 "No data supplied for parameters in prepared statement");
		return FAIL;
	}

	request = mysqlnd_stmt_execute_generate_request(stmt, &request_len, &free_request);
	
	/* support for buffer types should be added here ! */

	ret = mysqlnd_simple_command(stmt->conn, COM_STMT_EXECUTE, (char *)request, request_len,
								 PROT_LAST /* we will handle the response packet*/,
								 FALSE TSRMLS_CC);

	if (free_request) {
		efree(request);
	}

	if (ret == FAIL) {
		stmt->error_info = conn->error_info;
		return FAIL;
	}

	ret = mysqlnd_query_read_result_set_header(stmt->conn, stmt TSRMLS_CC);
	if (ret == FAIL) {
		stmt->error_info = conn->error_info;
		stmt->upsert_status.affected_rows = conn->upsert_status.affected_rows;
		if (conn->state == CONN_QUIT_SENT) {
			/* close the statement here, the connection has been closed */
		}
	} else {
		SET_EMPTY_ERROR(stmt->error_info);
		SET_EMPTY_ERROR(stmt->conn->error_info);
		stmt->send_types_to_server = 0;
		stmt->upsert_status = conn->upsert_status;
		stmt->state = MYSQLND_STMT_EXECUTED;
		if (conn->last_query_type == QUERY_UPSERT) {
			stmt->upsert_status = conn->upsert_status;
			return PASS;
		} else if (conn->last_query_type == QUERY_LOAD_LOCAL) {
			return PASS;
		}

		stmt->result->type = MYSQLND_RES_PS_BUF;
		if (!stmt->result->conn) {
			/*
			  For SHOW we don't create (bypasses PS in server)
			  a result set at prepare and thus a connection was missing
			*/
			stmt->result->conn = stmt->conn->m->get_reference(stmt->conn);
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
#ifndef MYSQLND_SILENT
			php_printf("server_status=%d cursor=%d\n", stmt->upsert_status.server_status, stmt->upsert_status.server_status & SERVER_STATUS_CURSOR_EXISTS);
#endif
			if (stmt->upsert_status.server_status & SERVER_STATUS_CURSOR_EXISTS) {
				stmt->cursor_exists = TRUE;
				conn->state = CONN_READY;
				/* Only cursor read */
				stmt->default_rset_handler = stmt->m->use_result;
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
			} else {
				/* preferred is unbuffered read */
				stmt->default_rset_handler = stmt->m->use_result;
			}
		}
	}

	return ret;
}
/* }}} */


/* {{{ mysqlnd_fetch_stmt_row_buffered */
enum_func_status
mysqlnd_fetch_stmt_row_buffered(MYSQLND_RES *result, void *param, unsigned int flags,
								zend_bool *fetched_anything TSRMLS_DC)
{
	unsigned int i;
	MYSQLND_STMT *stmt = (MYSQLND_STMT *) param;

	/* If we haven't read everything */
	if (result->data->data_cursor &&
		(result->data->data_cursor - result->data->data) < result->data->row_count)
	{
		/* The user could have skipped binding - don't crash*/
		if (stmt->result_bind) {
			zval **current_row = *result->data->data_cursor;
			for (i = 0; i < result->field_count; i++) {
				/* Clean what we copied last time */
#ifndef WE_DONT_COPY_IN_BUFFERED_AND_UNBUFFERED_BECAUSEOF_IS_REF
				zval_dtor(stmt->result_bind[i].zv);
#endif
				/* copy the type */
				if (stmt->result_bind[i].bound == TRUE) {
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
		result->data->data_cursor++;
		*fetched_anything = TRUE;
	} else {
		result->data->data_cursor = NULL;
		*fetched_anything = FALSE;
#ifndef MYSQLND_SILENT
		php_printf("NO MORE DATA\n ");
#endif
	}
	return PASS;
}
/* }}} */


/* {{{ mysqlnd_stmt_fetch_row_unbuffered */
enum_func_status
mysqlnd_stmt_fetch_row_unbuffered(MYSQLND_RES *result, void *param, unsigned int flags,
								zend_bool *fetched_anything TSRMLS_DC)
{
	enum_func_status ret;
	MYSQLND_STMT *stmt = (MYSQLND_STMT *) param;
	unsigned int i, field_count = result->field_count;
	php_mysql_packet_row *row_packet = result->row_packet;

	if (result->unbuf->eof_reached) {
		/* No more rows obviously */
		*fetched_anything = FALSE;
		return PASS;
	}
	if (result->conn->state != CONN_FETCHING_DATA) {
		SET_CLIENT_ERROR(result->conn->error_info, CR_COMMANDS_OUT_OF_SYNC,
						 UNKNOWN_SQLSTATE, mysqlnd_out_of_sync); 
		return FAIL;
	}
	/* Let the row packet fill our buffer and skip additional malloc + memcpy */
	row_packet->skip_extraction = stmt && stmt->result_bind? FALSE:TRUE;

	/*
	  If we skip rows (stmt == NULL || stmt->result_bind == NULL) we have to
	  mysqlnd_unbuffered_free_last_data() before it. The function returns always true.
	*/
	if (PASS == (ret = PACKET_READ(row_packet, result->conn)) && !row_packet->eof) {
		result->unbuf->row_count++;
		*fetched_anything = TRUE;

		if (!row_packet->skip_extraction) {
			mysqlnd_unbuffered_free_last_data(result TSRMLS_CC);

			result->unbuf->last_row_data = row_packet->fields;
			result->unbuf->last_row_buffer = row_packet->row_buffer;
			row_packet->fields = NULL;
			row_packet->row_buffer = NULL;

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
		} else {
			/*
			  Data has been allocated and usually mysqlnd_unbuffered_free_last_data()
			  frees it but we can't call this function as it will cause problems with
			  the bound variables. Thus we need to do part of what it does or Zend will
			  report leaks.
			*/
			efree(row_packet->row_buffer);
			row_packet->row_buffer = NULL;
		}
	} else if (ret == FAIL) {
		if (row_packet->error_info.error_no) {
			stmt->conn->error_info = row_packet->error_info; 
			stmt->error_info = row_packet->error_info; 
		}
		*fetched_anything = FALSE;
		result->conn->state = CONN_READY;
		result->unbuf->eof_reached = TRUE; /* so next time we won't get an error */	
	} else if (row_packet->eof) {
		/* Mark the connection as usable again */
		result->unbuf->eof_reached = TRUE;
		result->conn->upsert_status.warning_count = row_packet->warning_count;
		result->conn->upsert_status.server_status = row_packet->server_status;
		/*
		  result->row_packet will be cleaned when
		  destroying the result object
		*/
		if (result->conn->upsert_status.server_status & SERVER_MORE_RESULTS_EXISTS) {
			result->conn->state = CONN_NEXT_RESULT_PENDING;
		} else {
			result->conn->state = CONN_READY;
		}
		*fetched_anything = FALSE;
	}

	return ret;
}
/* }}} */


/* {{{ mysqlnd_stmt::use_result */
MYSQLND_RES *
MYSQLND_METHOD(mysqlnd_stmt, use_result)(MYSQLND_STMT *stmt TSRMLS_DC)
{
	MYSQLND_RES *result;
	MYSQLND *conn = stmt->conn;

	if (!stmt->field_count ||
		(!stmt->cursor_exists && conn->state != CONN_FETCHING_DATA) ||
		(stmt->cursor_exists && conn->state != CONN_READY) ||
		(stmt->state != MYSQLND_STMT_WAITING_USE_OR_STORE))
	{
		SET_CLIENT_ERROR(conn->error_info, CR_COMMANDS_OUT_OF_SYNC,
						 UNKNOWN_SQLSTATE, mysqlnd_out_of_sync); 
		return NULL;
	}

	SET_EMPTY_ERROR(stmt->error_info);
	SET_EMPTY_ERROR(stmt->conn->error_info);

	MYSQLND_INC_CONN_STATISTIC(&stmt->conn->stats, STAT_PS_UNBUFFERED_SETS);

	result					= stmt->result;
	result->type 			= MYSQLND_RES_PS_UNBUF;
	result->m.fetch_row		= stmt->cursor_exists? mysqlnd_fetch_stmt_row_cursor:
													mysqlnd_stmt_fetch_row_unbuffered;
	result->m.fetch_lengths	= NULL; /* makes no sense */
	result->zval_cache		= mysqlnd_palloc_get_thd_cache_reference(conn->zval_cache);

	result->unbuf	= ecalloc(1, sizeof(MYSQLND_RES_UNBUFFERED));

	/*
	  Will be freed in the mysqlnd_internal_free_result_contents() called
	  by the resource destructor. mysqlnd_fetch_row_unbuffered() expects
	  this to be not NULL.
	*/
	PACKET_INIT(result->row_packet, PROT_ROW_PACKET, php_mysql_packet_row *);
	result->row_packet->field_count = result->field_count;
	result->row_packet->binary_protocol = TRUE;
	result->row_packet->fields_metadata = stmt->result->meta->fields;
	result->row_packet->bit_fields_count = result->meta->bit_fields_count;
	result->row_packet->bit_fields_total_len = result->meta->bit_fields_total_len;
	result->lengths = NULL;

	stmt->state = MYSQLND_STMT_USE_OR_STORE_CALLED;

	/* No multithreading issues as we don't share the connection :) */

	return result;
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

	if (!stmt) {
		return FAIL;
	}

	if (stmt->state < MYSQLND_STMT_USER_FETCHING) {
		/* Only initted - error */
		SET_CLIENT_ERROR(stmt->conn->error_info, CR_COMMANDS_OUT_OF_SYNC, UNKNOWN_SQLSTATE,
						mysqlnd_out_of_sync);
		return FAIL;
	}

	SET_EMPTY_ERROR(stmt->error_info);
	SET_EMPTY_ERROR(stmt->conn->error_info);

	int4store(buf, stmt->stmt_id);
	int4store(buf + STMT_ID_LENGTH, 1); /* for now fetch only one row */

	if (FAIL == mysqlnd_simple_command(stmt->conn, COM_STMT_FETCH, (char *)buf, sizeof(buf),
									   PROT_LAST /* we will handle the response packet*/,
									   FALSE TSRMLS_CC)) {
		stmt->error_info = stmt->conn->error_info;
		return FAIL;
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
			/* If no result bind, do nothing. We consumed the data */
			for (i = 0; i < field_count; i++) {
				if (stmt->result_bind[i].bound == TRUE) {
					zval *data = result->unbuf->last_row_data[i];
					/*
					  stmt->result_bind[i].zv has been already destructed
					  in mysqlnd_unbuffered_free_last_data()
					*/

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
			efree(row_packet->row_buffer);
			row_packet->row_buffer = NULL;
		}
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

	return ret;
}
/* }}} */


/* {{{ mysqlnd_stmt_fetch */
PHPAPI enum_func_status
MYSQLND_METHOD(mysqlnd_stmt, fetch)(MYSQLND_STMT * const stmt,
									zend_bool * const fetched_anything TSRMLS_DC)
{
	if (!stmt->result ||
		stmt->state < MYSQLND_STMT_WAITING_USE_OR_STORE) {
		SET_STMT_ERROR(stmt, CR_COMMANDS_OUT_OF_SYNC, UNKNOWN_SQLSTATE, mysqlnd_out_of_sync);
		
		return FAIL;
	} else if (stmt->state == MYSQLND_STMT_WAITING_USE_OR_STORE) {
		/* Execute only once. We have to free the previous contents of user's bound vars */

		stmt->default_rset_handler(stmt TSRMLS_CC);
	}
	stmt->state = MYSQLND_STMT_USER_FETCHING;

	SET_EMPTY_ERROR(stmt->error_info);
	SET_EMPTY_ERROR(stmt->conn->error_info);

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

	MYSQLND_INC_CONN_STATISTIC(&stmt->conn->stats, STAT_ROWS_FETCHED_FROM_CLIENT);

	return stmt->result->m.fetch_row(stmt->result, (void*)stmt, 0, fetched_anything TSRMLS_CC);
}
/* }}} */


/* {{{ mysqlnd_stmt::reset */
static enum_func_status
MYSQLND_METHOD(mysqlnd_stmt, reset)(MYSQLND_STMT * const stmt TSRMLS_DC)
{
	enum_func_status ret = PASS;
	MYSQLND * conn = stmt->conn;
	zend_uchar cmd_buf[STMT_ID_LENGTH /* statement id */];

	SET_EMPTY_ERROR(stmt->error_info);
	SET_EMPTY_ERROR(stmt->conn->error_info);

	if (stmt->stmt_id) {
		if (stmt->param_bind) {
			unsigned int i;
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
			stmt->default_rset_handler(stmt TSRMLS_CC);
			stmt->state = MYSQLND_STMT_USER_FETCHING;
		}

		if (stmt->result) {
			stmt->result->m.skip_result(stmt->result TSRMLS_CC);
		}
		/* Now the line should be free, if it wasn't */

		int4store(cmd_buf, stmt->stmt_id);
		if (conn->state == CONN_READY &&
			FAIL == (ret = mysqlnd_simple_command(conn, COM_STMT_RESET, (char *)cmd_buf,
												  sizeof(cmd_buf), PROT_OK_PACKET,
												  FALSE TSRMLS_CC))) {
			stmt->error_info = conn->error_info;
		}
		stmt->upsert_status = conn->upsert_status;

		stmt->state = MYSQLND_STMT_PREPARED;
	}
	return ret;
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
	size_t packet_len;
	enum php_mysqlnd_server_command cmd = COM_STMT_SEND_LONG_DATA;
	
	SET_EMPTY_ERROR(stmt->error_info);
	SET_EMPTY_ERROR(stmt->conn->error_info);

	if (stmt->state < MYSQLND_STMT_PREPARED) {
		SET_STMT_ERROR(stmt, CR_NO_PREPARE_STMT, UNKNOWN_SQLSTATE, mysqlnd_stmt_not_prepared);
		return FAIL;
	}
	if (!stmt->param_bind) {
		SET_STMT_ERROR(stmt, CR_COMMANDS_OUT_OF_SYNC, UNKNOWN_SQLSTATE, mysqlnd_out_of_sync);
		return FAIL;
	}

	if (param_no >= stmt->param_count) {
		SET_STMT_ERROR(stmt, CR_INVALID_PARAMETER_NO, UNKNOWN_SQLSTATE, "Invalid parameter number");
		return FAIL;
	}
	if (stmt->param_bind[param_no].type != MYSQL_TYPE_LONG_BLOB) {
		SET_STMT_ERROR(stmt, CR_INVALID_BUFFER_USE, UNKNOWN_SQLSTATE, mysqlnd_not_bound_as_blob);
		return FAIL;
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

	if (conn->state == CONN_READY) {
		stmt->param_bind[param_no].flags |= MYSQLND_PARAM_BIND_BLOB_USED;
		cmd_buf = emalloc(packet_len = STMT_ID_LENGTH + 2 + length);

		int4store(cmd_buf, stmt->stmt_id);
		int2store(cmd_buf + STMT_ID_LENGTH, param_no);
		memcpy(cmd_buf + STMT_ID_LENGTH + 2, data, length);

		/* COM_STMT_SEND_LONG_DATA doesn't send an OK packet*/
		ret = mysqlnd_simple_command(conn, cmd, (char *)cmd_buf, packet_len,
									 PROT_LAST , FALSE TSRMLS_CC);
		efree(cmd_buf);
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

	return ret;
}
/* }}} */


/* {{{ _mysqlnd_stmt_bind_param */
static enum_func_status
MYSQLND_METHOD(mysqlnd_stmt, bind_param)(MYSQLND_STMT * const stmt,
										 MYSQLND_PARAM_BIND * const param_bind)
{
	unsigned int i = 0;

	if (stmt->state < MYSQLND_STMT_PREPARED) {
		SET_STMT_ERROR(stmt, CR_NO_PREPARE_STMT, UNKNOWN_SQLSTATE, mysqlnd_stmt_not_prepared);
		return FAIL;
	}

	SET_EMPTY_ERROR(stmt->error_info);
	SET_EMPTY_ERROR(stmt->conn->error_info);

	if (stmt->param_count) {
		if (!param_bind) {
			SET_STMT_ERROR(stmt, CR_COMMANDS_OUT_OF_SYNC, UNKNOWN_SQLSTATE,
							 "Re-binding (still) not supported");
			return FAIL;
		} else if (stmt->param_bind) {
			/*
			  There is already result bound.
			  Forbid for now re-binding!!
			*/
			for (i = 0; i < stmt->param_count; i++) {
				/* For BLOBS zv is NULL */
				if (stmt->param_bind[i].zv) {
					/*
					  We may have the last reference, then call zval_ptr_dtor()
					  or we may leak memory.
					*/
					zval_ptr_dtor(&stmt->param_bind[i].zv);
					stmt->param_bind[i].zv = NULL;
				}
			}
			efree(stmt->param_bind);
		}

		stmt->param_bind = param_bind;
		for (i = 0; i < stmt->param_count; i++) {
			/* The client will use stmt_send_long_data */
			if (stmt->param_bind[i].type != MYSQL_TYPE_LONG_BLOB) {
				/* Prevent from freeing */
				ZVAL_ADDREF(stmt->param_bind[i].zv);
				/* Don't update is_ref, or we will leak during conversion */
			} else {
				stmt->param_bind[i].zv = NULL;
				stmt->param_bind[i].flags &= ~MYSQLND_PARAM_BIND_BLOB_USED;
			}
		}
		stmt->send_types_to_server = 1;
	}
	return PASS;
}
/* }}} */


/* {{{ mysqlnd_stmt::bind_result */
static enum_func_status
MYSQLND_METHOD(mysqlnd_stmt, bind_result)(MYSQLND_STMT * const stmt,
										  MYSQLND_RESULT_BIND * const result_bind)
{
	uint i = 0;

	SET_EMPTY_ERROR(stmt->error_info);
	SET_EMPTY_ERROR(stmt->conn->error_info);

	if (stmt->state < MYSQLND_STMT_PREPARED) {
		SET_STMT_ERROR(stmt, CR_NO_PREPARE_STMT, UNKNOWN_SQLSTATE, mysqlnd_stmt_not_prepared);
		if (result_bind) {
			efree(result_bind);
		}
		return FAIL;
	}

	if (stmt->field_count) {
		if (!result_bind) {
			return FAIL;
		}

		mysqlnd_stmt_separate_result_bind(stmt);

		stmt->result_bind = result_bind;
		for (i = 0; i < stmt->field_count; i++) {
			/* Prevent from freeing */
			ZVAL_ADDREF(stmt->result_bind[i].zv);		
			/*
			  Don't update is_ref !!! it's not our job
			  Otherwise either 009.phpt or mysqli_stmt_bind_result.phpt
			  will fail.
			*/
			stmt->result_bind[i].bound = TRUE;
		}
	} else if (result_bind) {
		efree(result_bind);
	}
	return PASS;
}
/* }}} */


/* {{{ mysqlnd_stmt::insert_id */
static mynd_ulonglong
MYSQLND_METHOD(mysqlnd_stmt, insert_id)(const MYSQLND_STMT * const stmt)
{
	return stmt->upsert_status.last_insert_id;
}
/* }}} */


/* {{{ mysqlnd_stmt::affected_rows */
static mynd_ulonglong
MYSQLND_METHOD(mysqlnd_stmt, affected_rows)(const MYSQLND_STMT * const stmt)
{
	return stmt->upsert_status.affected_rows;
}
/* }}} */


/* {{{ mysqlnd_stmt::num_rows */
static mynd_ulonglong
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
MYSQLND_METHOD(mysqlnd_stmt, data_seek)(const MYSQLND_STMT * const stmt, mynd_ulonglong row)
{
	return stmt->result? stmt->result->m.seek_data(stmt->result, row) : FAIL;
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
MYSQLND_METHOD(mysqlnd_stmt, result_metadata)(MYSQLND_STMT * const stmt)
{
	MYSQLND_RES *result;

	if (!stmt->field_count || !stmt->conn || !stmt->result ||
		!stmt->result->meta)
	{
		return NULL;
	}

	/*
	  TODO: This implementation is kind of a hack,
			find a better way to do it. In different functions I have put
			fuses to check for result->m.fetch_row() being NULL. This should
			be handled in a better way.

	  In the meantime we don't need a zval cache reference for this fake
	  result set, so we don't get one.
	*/
	result = mysqlnd_result_init(stmt->field_count, NULL);
	result->type = MYSQLND_RES_NORMAL;
	result->m.fetch_row = result->m.fetch_row_normal_unbuffered;
	result->unbuf = ecalloc(1, sizeof(MYSQLND_RES_UNBUFFERED));
	result->unbuf->eof_reached = TRUE;
	result->meta = stmt->result->meta->m->clone_metadata(stmt->result->meta, FALSE);

	return result;
}
/* }}} */


/* {{{ mysqlnd_stmt::attr_set */
static enum_func_status
MYSQLND_METHOD(mysqlnd_stmt, attr_set)(MYSQLND_STMT * const stmt,
									   enum mysqlnd_stmt_attr attr_type,
									   const void * const value)
{
	unsigned long val = *(unsigned long *) value;
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
				return FAIL;
			}
			stmt->flags = val;
			break;
		}
		case STMT_ATTR_PREFETCH_ROWS: {
			if (val == 0) {
				val = MYSQLND_DEFAULT_PREFETCH_ROWS;
			} else if (val > 1) {
				SET_STMT_ERROR(stmt, CR_NOT_IMPLEMENTED, UNKNOWN_SQLSTATE, "Not implemented");
				return FAIL;
			}
			stmt->prefetch_rows = val;
			break;
		}
		default:
			SET_STMT_ERROR(stmt, CR_NOT_IMPLEMENTED, UNKNOWN_SQLSTATE, "Not implemented");
			return FAIL;
	}
	return PASS;
}
/* }}} */


/* {{{ _mysqlnd_stmt_attr_get */
static enum_func_status
MYSQLND_METHOD(mysqlnd_stmt, attr_get)(MYSQLND_STMT * const stmt,
									   enum mysqlnd_stmt_attr attr_type,
									   void * const value)
{
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
			return FAIL;
	}
	return PASS;
}
/* }}} */


/* {{{ mysqlnd_stmt::free_result */
static enum_func_status
MYSQLND_METHOD(mysqlnd_stmt, free_result)(MYSQLND_STMT * const stmt TSRMLS_DC)
{
	if (!stmt->result) {
		return PASS;
	}

	if (stmt->state == MYSQLND_STMT_WAITING_USE_OR_STORE) {
		/* Do implicit use_result and then flush the result */
		stmt->default_rset_handler = stmt->m->use_result;
		stmt->default_rset_handler(stmt TSRMLS_CC);
	}

	if (stmt->state > MYSQLND_STMT_WAITING_USE_OR_STORE) {
		/* Flush if anything is left and unbuffered set */
		stmt->result->m.skip_result(stmt->result TSRMLS_CC);
		/*
		  Separate the bound variables, which point to the result set, then
		  destroy the set.
		*/
		mysqlnd_stmt_separate_result_bind(stmt);

		/* Now we can destroy the result set */
		stmt->result->m.free_result_buffers(stmt->result TSRMLS_CC);
	}

	/* As the buffers have been freed, we should go back to PREPARED */
	stmt->state = MYSQLND_STMT_PREPARED;

	/* Line is free! */
	stmt->conn->state = CONN_READY;

	return PASS;
}
/* }}} */


/* {{{ mysqlnd_stmt_separate_result_bind */
void mysqlnd_stmt_separate_result_bind(MYSQLND_STMT * const stmt)
{
	int i;

	if (!stmt->result_bind) {
		return;
	}

	/*
	  Because only the bound variables can point to our internal buffers, then
	  separate or free only them. Free is possible because the user could have
	  lost reference.
	*/
	for (i = 0; i < stmt->field_count; i++) {
		/* Let's try with no cache */
		if (stmt->result_bind[i].bound == TRUE) {
			/*
			  We have to separate the actual zval value of the bound
			  variable from our allocated zvals or we will face double-free
			*/
			if (ZVAL_REFCOUNT(stmt->result_bind[i].zv) > 1) {
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
	efree(stmt->result_bind);
	stmt->result_bind = NULL;
}
/* }}} */


/* {{{ mysqlnd_internal_free_stmt_content */
static
void mysqlnd_internal_free_stmt_content(MYSQLND_STMT *stmt TSRMLS_DC)
{
	/* Destroy the input bind */
	if (stmt->param_bind) {
		int i;
		/*
		  Because only the bound variables can point to our internal buffers, then
		  separate or free only them. Free is possible because the user could have
		  lost reference.
		*/
		for (i = 0; i < stmt->param_count; i++) {
			/* For BLOBs zv is NULL */
			if (stmt->param_bind[i].zv) {
				zval_ptr_dtor(&stmt->param_bind[i].zv);
			}
		}

		efree(stmt->param_bind);
		stmt->param_bind = NULL;
	}

	/*
	  First separate the bound variables, which point to the result set, then
	  destroy the set.
	*/
	mysqlnd_stmt_separate_result_bind(stmt);
	/* Not every statement has a result set attached */
	if (stmt->result) {
		stmt->result->m.free_result_internal(stmt->result TSRMLS_CC);
		stmt->result = NULL;
	}
	if (stmt->cmd_buffer.buffer) {
		efree(stmt->cmd_buffer.buffer);
		stmt->cmd_buffer.buffer = NULL;
	}

	if (stmt->conn) {
		stmt->conn->m->free_reference(stmt->conn TSRMLS_CC);
		stmt->conn = NULL;
	}
}
/* }}} */


/* {{{ mysqlnd_stmt::close */
static enum_func_status
MYSQLND_METHOD(mysqlnd_stmt, close)(MYSQLND_STMT * const stmt, zend_bool implicit TSRMLS_DC)
{
	MYSQLND * conn = stmt->conn;
	zend_uchar cmd_buf[STMT_ID_LENGTH /* statement id */];

	SET_EMPTY_ERROR(stmt->error_info);
	SET_EMPTY_ERROR(stmt->conn->error_info);

	/*
	  If the user decided to close the statement right after execute()
	  We have to call the appropriate use_result() or store_result() and
	  clean.
	*/
	if (stmt->state == MYSQLND_STMT_WAITING_USE_OR_STORE) {
		stmt->default_rset_handler(stmt TSRMLS_CC);
		stmt->state = MYSQLND_STMT_USER_FETCHING;
	}

	/* unbuffered set not fetched to the end ? Clean the line */
	if (stmt->result) {
		stmt->result->m.skip_result(stmt->result TSRMLS_CC);
	}
	/*
	  After this point we are allowed to free the result set,
	  as we have cleaned the line
	*/
	if (stmt->stmt_id) {	
		MYSQLND_INC_CONN_STATISTIC(NULL, implicit == TRUE?	STAT_FREE_RESULT_IMPLICIT:
															STAT_FREE_RESULT_EXPLICIT);

		int4store(cmd_buf, stmt->stmt_id);
		if (conn->state == CONN_READY &&
			FAIL == mysqlnd_simple_command(conn, COM_STMT_CLOSE, (char *)cmd_buf, sizeof(cmd_buf),
										   PROT_LAST /* COM_STMT_CLOSE doesn't send an OK packet*/,
										   FALSE TSRMLS_CC)) {
			stmt->error_info = conn->error_info;
			return FAIL;
		}
	}

	mysqlnd_internal_free_stmt_content(stmt TSRMLS_CC);

	return PASS;
}
/* }}} */


/* {{{ mysqlnd_stmt::dtor */
static enum_func_status
MYSQLND_METHOD_PRIVATE(mysqlnd_stmt, dtor)(MYSQLND_STMT * const stmt, zend_bool implicit TSRMLS_DC)
{
	enum_func_status ret;

	MYSQLND_INC_CONN_STATISTIC(NULL, implicit == TRUE?	STAT_STMT_CLOSE_IMPLICIT:
														STAT_STMT_CLOSE_EXPLICIT);

	if (PASS == (ret = stmt->m->close(stmt, implicit TSRMLS_CC))) {
		efree(stmt);
	}

	return ret;
}
/* }}} */


static
struct st_mysqlnd_stmt_methods mysqlnd_stmt_methods = {
	MYSQLND_METHOD(mysqlnd_stmt, prepare),
	MYSQLND_METHOD(mysqlnd_stmt, execute),
	MYSQLND_METHOD(mysqlnd_stmt, use_result),
	MYSQLND_METHOD(mysqlnd_stmt, store_result),
	MYSQLND_METHOD(mysqlnd_stmt, get_result),
	MYSQLND_METHOD(mysqlnd_stmt, free_result),
	MYSQLND_METHOD(mysqlnd_stmt, data_seek),
	MYSQLND_METHOD(mysqlnd_stmt, reset),
	MYSQLND_METHOD(mysqlnd_stmt, close),
	MYSQLND_METHOD_PRIVATE(mysqlnd_stmt, dtor),

	MYSQLND_METHOD(mysqlnd_stmt, fetch),

	MYSQLND_METHOD(mysqlnd_stmt, bind_param),
	MYSQLND_METHOD(mysqlnd_stmt, bind_result),
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
MYSQLND_STMT * _mysqlnd_stmt_init(MYSQLND * const conn)
{
	MYSQLND_STMT *stmt = ecalloc(1, sizeof(MYSQLND_STMT));

	stmt->m = &mysqlnd_stmt_methods;
	stmt->state = MYSQLND_STMT_INITTED;
	stmt->cmd_buffer.length = 4096;
	stmt->cmd_buffer.buffer = emalloc(stmt->cmd_buffer.length);

	stmt->prefetch_rows = MYSQLND_DEFAULT_PREFETCH_ROWS;
	/*
	  Mark that we reference the connection, thus it won't be
	  be destructed till there is open statements. The last statement
	  or normal query result will close it then.
	*/
	stmt->conn = conn->m->get_reference(conn);

	return stmt;
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
