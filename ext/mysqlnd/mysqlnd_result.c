/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2006-2014 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Andrey Hristov <andrey@mysql.com>                           |
  |          Ulf Wendel <uwendel@mysql.com>                              |
  |          Georg Richter <georg@mysql.com>                             |
  +----------------------------------------------------------------------+
*/

/* $Id$ */
#include "php.h"
#include "mysqlnd.h"
#include "mysqlnd_wireprotocol.h"
#include "mysqlnd_block_alloc.h"
#include "mysqlnd_priv.h"
#include "mysqlnd_result.h"
#include "mysqlnd_result_meta.h"
#include "mysqlnd_statistics.h"
#include "mysqlnd_debug.h"
#include "mysqlnd_ext_plugin.h"

#define MYSQLND_SILENT


/* {{{ mysqlnd_res::initialize_result_set_rest */
static enum_func_status
MYSQLND_METHOD(mysqlnd_res, initialize_result_set_rest)(MYSQLND_RES * const result TSRMLS_DC)
{
	unsigned int i;
	zval **data_cursor = result->stored_data? result->stored_data->data:NULL;
	zval **data_begin = result->stored_data? result->stored_data->data:NULL;
	unsigned int field_count = result->meta? result->meta->field_count : 0;
	uint64_t row_count = result->stored_data? result->stored_data->row_count:0;
	enum_func_status ret = PASS;
	DBG_ENTER("mysqlnd_res::initialize_result_set_rest");

	if (!data_cursor || row_count == result->stored_data->initialized_rows) {
		DBG_RETURN(ret);
	}
	while ((data_cursor - data_begin) < (int)(row_count * field_count)) {
		if (NULL == data_cursor[0]) {
			enum_func_status rc = result->m.row_decoder(
									result->stored_data->row_buffers[(data_cursor - data_begin) / field_count],
									data_cursor,
									result->meta->field_count,
									result->meta->fields,
									result->conn->options->int_and_float_native,
									result->conn->stats TSRMLS_CC);
			if (rc != PASS) {
				ret = FAIL;
				break;
			}
			result->stored_data->initialized_rows++;
			for (i = 0; i < result->field_count; i++) {
				/*
				  NULL fields are 0 length, 0 is not more than 0
				  String of zero size, definitely can't be the next max_length.
				  Thus for NULL and zero-length we are quite efficient.
				*/
				if (Z_TYPE_P(data_cursor[i]) >= IS_STRING) {
					unsigned long len = Z_STRLEN_P(data_cursor[i]);
					if (result->meta->fields[i].max_length < len) {
						result->meta->fields[i].max_length = len;
					}
				}
			}
		}
		data_cursor += field_count;
	}
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_rset_zval_ptr_dtor */
static void
mysqlnd_rset_zval_ptr_dtor(zval **zv, enum_mysqlnd_res_type type, zend_bool * copy_ctor_called TSRMLS_DC)
{
	DBG_ENTER("mysqlnd_rset_zval_ptr_dtor");
	if (!zv || !*zv) {
		*copy_ctor_called = FALSE;
		DBG_ERR_FMT("zv was NULL");
		DBG_VOID_RETURN;
	}
	/*
	  This zval is not from the cache block.
	  Thus the refcount is -1 than of a zval from the cache,
	  because the zvals from the cache are owned by it.
	*/
	if (type == MYSQLND_RES_PS_BUF || type == MYSQLND_RES_PS_UNBUF) {
		*copy_ctor_called = FALSE;
		; /* do nothing, zval_ptr_dtor will do the job*/
	} else if (Z_REFCOUNT_PP(zv) > 1) {
		/*
		  Not a prepared statement, then we have to
		  call copy_ctor and then zval_ptr_dtor()
		*/
		if (Z_TYPE_PP(zv) == IS_STRING) {
			zval_copy_ctor(*zv);
		}
		*copy_ctor_called = TRUE;
	} else {
		/*
		  noone but us point to this, so we can safely ZVAL_NULL the zval,
		  so Zend does not try to free what the zval points to - which is
		  in result set buffers
		*/
		*copy_ctor_called = FALSE;
		if (Z_TYPE_PP(zv) == IS_STRING) {
			ZVAL_NULL(*zv);
		}
	}
	zval_ptr_dtor(zv);
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_res::unbuffered_free_last_data */
static void
MYSQLND_METHOD(mysqlnd_res, unbuffered_free_last_data)(MYSQLND_RES * result TSRMLS_DC)
{
	MYSQLND_RES_UNBUFFERED *unbuf = result->unbuf;

	DBG_ENTER("mysqlnd_res::unbuffered_free_last_data");

	if (!unbuf) {
		DBG_VOID_RETURN;
	}

	if (unbuf->last_row_data) {
		unsigned int i, ctor_called_count = 0;
		zend_bool copy_ctor_called;
		MYSQLND_STATS *global_stats = result->conn? result->conn->stats:NULL;

		for (i = 0; i < result->field_count; i++) {
			mysqlnd_rset_zval_ptr_dtor(&(unbuf->last_row_data[i]), result->type, &copy_ctor_called TSRMLS_CC);
			if (copy_ctor_called) {
				++ctor_called_count;
			}
		}
		DBG_INF_FMT("copy_ctor_called_count=%u", ctor_called_count);
		/* By using value3 macros we hold a mutex only once, there is no value2 */
		MYSQLND_INC_CONN_STATISTIC_W_VALUE2(global_stats,
											STAT_COPY_ON_WRITE_PERFORMED,
											ctor_called_count,
											STAT_COPY_ON_WRITE_SAVED,
											result->field_count - ctor_called_count);
		/* Free last row's zvals */
		mnd_efree(unbuf->last_row_data);
		unbuf->last_row_data = NULL;
	}
	if (unbuf->last_row_buffer) {
		DBG_INF("Freeing last row buffer");
		/* Nothing points to this buffer now, free it */
		unbuf->last_row_buffer->free_chunk(unbuf->last_row_buffer TSRMLS_CC);
		unbuf->last_row_buffer = NULL;
	}

	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_res::free_buffered_data */
static void
MYSQLND_METHOD(mysqlnd_res, free_buffered_data)(MYSQLND_RES * result TSRMLS_DC)
{
	MYSQLND_RES_BUFFERED *set = result->stored_data;
	unsigned int field_count = result->field_count;
	int64_t row;

	DBG_ENTER("mysqlnd_res::free_buffered_data");
	DBG_INF_FMT("Freeing "MYSQLND_LLU_SPEC" row(s)", set->row_count);

	if (set->data) {
		unsigned int copy_on_write_performed = 0;
		unsigned int copy_on_write_saved = 0;
		zval **data = set->data;
		set->data = NULL; /* prevent double free if following loop is interrupted */

		for (row = set->row_count - 1; row >= 0; row--) {
			zval **current_row = data + row * field_count;
			MYSQLND_MEMORY_POOL_CHUNK *current_buffer = set->row_buffers[row];
			int64_t col;

			if (current_row != NULL) {
				for (col = field_count - 1; col >= 0; --col) {
					if (current_row[col]) {
						zend_bool copy_ctor_called;
						mysqlnd_rset_zval_ptr_dtor(&(current_row[col]), result->type, &copy_ctor_called TSRMLS_CC);
						if (copy_ctor_called) {
							++copy_on_write_performed;
						} else {
							++copy_on_write_saved;
						}
					}
				}
			}
			current_buffer->free_chunk(current_buffer TSRMLS_CC);
		}

		MYSQLND_INC_GLOBAL_STATISTIC_W_VALUE2(STAT_COPY_ON_WRITE_PERFORMED, copy_on_write_performed,
											  STAT_COPY_ON_WRITE_SAVED, copy_on_write_saved);
		mnd_efree(data);
	}

	if (set->row_buffers) {
		mnd_efree(set->row_buffers);
		set->row_buffers	= NULL;
	}
	set->data_cursor = NULL;
	set->row_count	= 0;

	mnd_efree(set);

	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_res::free_result_buffers */
static void
MYSQLND_METHOD(mysqlnd_res, free_result_buffers)(MYSQLND_RES * result TSRMLS_DC)
{
	DBG_ENTER("mysqlnd_res::free_result_buffers");
	DBG_INF_FMT("%s", result->unbuf? "unbuffered":(result->stored_data? "buffered":"unknown"));

	if (result->unbuf) {
		result->m.unbuffered_free_last_data(result TSRMLS_CC);
		mnd_efree(result->unbuf);
		result->unbuf = NULL;
	} else if (result->stored_data) {
		result->m.free_buffered_data(result TSRMLS_CC);
		result->stored_data = NULL;
	}

	if (result->lengths) {
		mnd_efree(result->lengths);
		result->lengths = NULL;
	}

	if (result->row_packet) {
		PACKET_FREE(result->row_packet);
		result->row_packet = NULL;
	}

	if (result->result_set_memory_pool) {
		mysqlnd_mempool_destroy(result->result_set_memory_pool TSRMLS_CC);
		result->result_set_memory_pool = NULL;
	}

	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_internal_free_result_contents */
static
void mysqlnd_internal_free_result_contents(MYSQLND_RES * result TSRMLS_DC)
{
	DBG_ENTER("mysqlnd_internal_free_result_contents");

	result->m.free_result_buffers(result TSRMLS_CC);

	if (result->meta) {
		result->meta->m->free_metadata(result->meta TSRMLS_CC);
		result->meta = NULL;
	}

	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_internal_free_result */
static
void mysqlnd_internal_free_result(MYSQLND_RES * result TSRMLS_DC)
{
	DBG_ENTER("mysqlnd_internal_free_result");
	result->m.free_result_contents(result TSRMLS_CC);

	if (result->conn) {
		result->conn->m->free_reference(result->conn TSRMLS_CC);
		result->conn = NULL;
	}

	mnd_pefree(result, result->persistent);

	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_res::read_result_metadata */
static enum_func_status
MYSQLND_METHOD(mysqlnd_res, read_result_metadata)(MYSQLND_RES * result, MYSQLND_CONN_DATA * conn TSRMLS_DC)
{
	DBG_ENTER("mysqlnd_res::read_result_metadata");

	/*
	  Make it safe to call it repeatedly for PS -
	  better free and allocate a new because the number of field might change 
	  (select *) with altered table. Also for statements which skip the PS
	  infrastructure!
	*/
	if (result->meta) {
		result->meta->m->free_metadata(result->meta TSRMLS_CC);
		result->meta = NULL;
	}

	result->meta = result->m.result_meta_init(result->field_count, result->persistent TSRMLS_CC);
	if (!result->meta) {
		SET_OOM_ERROR(*conn->error_info);
		DBG_RETURN(FAIL);
	}

	/* 1. Read all fields metadata */

	/* It's safe to reread without freeing */
	if (FAIL == result->meta->m->read_metadata(result->meta, conn TSRMLS_CC)) {
		result->m.free_result_contents(result TSRMLS_CC);
		DBG_RETURN(FAIL);
	}
	/* COM_FIELD_LIST is broken and has premature EOF, thus we need to hack here and in mysqlnd_res_meta.c */
	result->field_count = result->meta->field_count;

	/*
	  2. Follows an EOF packet, which the client of mysqlnd_read_result_metadata()
	     should consume.
	  3. If there is a result set, it follows. The last packet will have 'eof' set
	     If PS, then no result set follows.
	*/

	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ mysqlnd_query_read_result_set_header */
enum_func_status
mysqlnd_query_read_result_set_header(MYSQLND_CONN_DATA * conn, MYSQLND_STMT * s TSRMLS_DC)
{
	MYSQLND_STMT_DATA * stmt = s ? s->data:NULL;
	enum_func_status ret;
	MYSQLND_PACKET_RSET_HEADER * rset_header = NULL;
	MYSQLND_PACKET_EOF * fields_eof = NULL;

	DBG_ENTER("mysqlnd_query_read_result_set_header");
	DBG_INF_FMT("stmt=%lu", stmt? stmt->stmt_id:0);

	ret = FAIL;
	do {
		rset_header = conn->protocol->m.get_rset_header_packet(conn->protocol, FALSE TSRMLS_CC);
		if (!rset_header) {
			SET_OOM_ERROR(*conn->error_info);
			ret = FAIL;
			break;
		}

		SET_ERROR_AFF_ROWS(conn);

		if (FAIL == (ret = PACKET_READ(rset_header, conn))) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Error reading result set's header");
			break;
		}

		if (rset_header->error_info.error_no) {
			/*
			  Cover a protocol design error: error packet does not
			  contain the server status. Therefore, the client has no way
			  to find out whether there are more result sets of
			  a multiple-result-set statement pending. Luckily, in 5.0 an
			  error always aborts execution of a statement, wherever it is
			  a multi-statement or a stored procedure, so it should be
			  safe to unconditionally turn off the flag here.
			*/
			conn->upsert_status->server_status &= ~SERVER_MORE_RESULTS_EXISTS;
			/*
			  This will copy the error code and the messages, as they
			  are buffers in the struct
			*/
			COPY_CLIENT_ERROR(*conn->error_info, rset_header->error_info);
			ret = FAIL;
			DBG_ERR_FMT("error=%s", rset_header->error_info.error);
			/* Return back from CONN_QUERY_SENT */
			CONN_SET_STATE(conn, CONN_READY);
			break;
		}
		conn->error_info->error_no = 0;

		switch (rset_header->field_count) {
			case MYSQLND_NULL_LENGTH: {	/* LOAD DATA LOCAL INFILE */
				zend_bool is_warning;
				DBG_INF("LOAD DATA");
				conn->last_query_type = QUERY_LOAD_LOCAL;
				conn->field_count = 0; /* overwrite previous value, or the last value could be used and lead to bug#53503 */
				CONN_SET_STATE(conn, CONN_SENDING_LOAD_DATA);
				ret = mysqlnd_handle_local_infile(conn, rset_header->info_or_local_file, &is_warning TSRMLS_CC);
				CONN_SET_STATE(conn,  (ret == PASS || is_warning == TRUE)? CONN_READY:CONN_QUIT_SENT);
				MYSQLND_INC_CONN_STATISTIC(conn->stats, STAT_NON_RSET_QUERY);
				break;
			}
			case 0:				/* UPSERT */
				DBG_INF("UPSERT");
				conn->last_query_type = QUERY_UPSERT;
				conn->field_count = rset_header->field_count;
				memset(conn->upsert_status, 0, sizeof(*conn->upsert_status));
				conn->upsert_status->warning_count = rset_header->warning_count;
				conn->upsert_status->server_status = rset_header->server_status;
				conn->upsert_status->affected_rows = rset_header->affected_rows;
				conn->upsert_status->last_insert_id = rset_header->last_insert_id;
				SET_NEW_MESSAGE(conn->last_message, conn->last_message_len,
								rset_header->info_or_local_file, rset_header->info_or_local_file_len,
								conn->persistent);
				/* Result set can follow UPSERT statement, check server_status */
				if (conn->upsert_status->server_status & SERVER_MORE_RESULTS_EXISTS) {
					CONN_SET_STATE(conn, CONN_NEXT_RESULT_PENDING);
				} else {
					CONN_SET_STATE(conn, CONN_READY);
				}
				ret = PASS;
				MYSQLND_INC_CONN_STATISTIC(conn->stats, STAT_NON_RSET_QUERY);
				break;
			default: do {			/* Result set */
				MYSQLND_RES * result;
				enum_mysqlnd_collected_stats statistic = STAT_LAST;

				DBG_INF("Result set pending");
				SET_EMPTY_MESSAGE(conn->last_message, conn->last_message_len, conn->persistent);

				MYSQLND_INC_CONN_STATISTIC(conn->stats, STAT_RSET_QUERY);
				memset(conn->upsert_status, 0, sizeof(*conn->upsert_status));
				/* restore after zeroing */
				SET_ERROR_AFF_ROWS(conn);

				conn->last_query_type = QUERY_SELECT;
				CONN_SET_STATE(conn, CONN_FETCHING_DATA);
				/* PS has already allocated it */
				conn->field_count = rset_header->field_count;
				if (!stmt) {
					result = conn->current_result = conn->m->result_init(rset_header->field_count, conn->persistent TSRMLS_CC);
				} else {
					if (!stmt->result) {
						DBG_INF("This is 'SHOW'/'EXPLAIN'-like query.");
						/*
						  This is 'SHOW'/'EXPLAIN'-like query. Current implementation of
						  prepared statements can't send result set metadata for these queries
						  on prepare stage. Read it now.
						*/
						result = stmt->result = conn->m->result_init(rset_header->field_count, stmt->persistent TSRMLS_CC);
					} else {
						/*
						  Update result set metadata if it for some reason changed between
						  prepare and execute, i.e.:
						  - in case of 'SELECT ?' we don't know column type unless data was
							supplied to mysql_stmt_execute, so updated column type is sent
							now.
						  - if data dictionary changed between prepare and execute, for
							example a table used in the query was altered.
						  Note, that now (4.1.3) we always send metadata in reply to
						  COM_STMT_EXECUTE (even if it is not necessary), so either this or
						  previous branch always works.
						*/
					}
					result = stmt->result;
				}
				if (!result) {
					SET_OOM_ERROR(*conn->error_info);
					ret = FAIL;
					break;
				}

				if (FAIL == (ret = result->m.read_result_metadata(result, conn TSRMLS_CC))) {
					/* For PS, we leave them in Prepared state */
					if (!stmt && conn->current_result) {
						mnd_efree(conn->current_result);
						conn->current_result = NULL;
					}
					DBG_ERR("Error occurred while reading metadata");
					break;
				}

				/* Check for SERVER_STATUS_MORE_RESULTS if needed */
				fields_eof = conn->protocol->m.get_eof_packet(conn->protocol, FALSE TSRMLS_CC);
				if (!fields_eof) {
					SET_OOM_ERROR(*conn->error_info);
					ret = FAIL;
					break;
				}
				if (FAIL == (ret = PACKET_READ(fields_eof, conn))) {
					DBG_ERR("Error occurred while reading the EOF packet");
					result->m.free_result_contents(result TSRMLS_CC);
					mnd_efree(result);
					if (!stmt) {
						conn->current_result = NULL;
					} else {
						stmt->result = NULL;
						memset(stmt, 0, sizeof(MYSQLND_STMT));
						stmt->state = MYSQLND_STMT_INITTED;
					}
				} else {
					unsigned int to_log = MYSQLND_G(log_mask);
					to_log &= fields_eof->server_status;
					DBG_INF_FMT("warnings=%u server_status=%u", fields_eof->warning_count, fields_eof->server_status);
					conn->upsert_status->warning_count = fields_eof->warning_count;
					/*
					  If SERVER_MORE_RESULTS_EXISTS is set then this is either MULTI_QUERY or a CALL()
					  The first packet after sending the query/com_execute has the bit set only
					  in this cases. Not sure why it's a needed but it marks that the whole stream
					  will include many result sets. What actually matters are the bits set at the end
					  of every result set (the EOF packet).
					*/
					conn->upsert_status->server_status = fields_eof->server_status;
					if (fields_eof->server_status & SERVER_QUERY_NO_GOOD_INDEX_USED) {
						statistic = STAT_BAD_INDEX_USED;
					} else if (fields_eof->server_status & SERVER_QUERY_NO_INDEX_USED) {
						statistic = STAT_NO_INDEX_USED;
					} else if (fields_eof->server_status & SERVER_QUERY_WAS_SLOW) {
						statistic = STAT_QUERY_WAS_SLOW;
					}
					if (to_log) {
#if A0
						char *backtrace = mysqlnd_get_backtrace(TSRMLS_C);
						php_log_err(backtrace TSRMLS_CC);
						efree(backtrace);
#endif
					}
					MYSQLND_INC_CONN_STATISTIC(conn->stats, statistic);
				}
			} while (0);
			PACKET_FREE(fields_eof);
			break; /* switch break */
		}
	} while (0);
	PACKET_FREE(rset_header);

	DBG_INF(ret == PASS? "PASS":"FAIL");
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_fetch_lengths_buffered */
/*
  Do lazy initialization for buffered results. As PHP strings have
  length inside, this function makes not much sense in the context
  of PHP, to be called as separate function. But let's have it for
  completeness.
*/
static unsigned long *
mysqlnd_fetch_lengths_buffered(MYSQLND_RES * const result TSRMLS_DC)
{
	unsigned int i;
	zval **previous_row;
	MYSQLND_RES_BUFFERED *set = result->stored_data;

	/*
	  If:
	  - unbuffered result
	  - first row has not been read
	  - last_row has been read
	*/
	if (set->data_cursor == NULL ||
		set->data_cursor == set->data ||
		((set->data_cursor - set->data) > (set->row_count * result->meta->field_count) ))
	{
		return NULL;/* No rows or no more rows */
	}

	previous_row = set->data_cursor - result->meta->field_count;
	for (i = 0; i < result->meta->field_count; i++) {
		result->lengths[i] = (Z_TYPE_P(previous_row[i]) == IS_NULL)? 0:Z_STRLEN_P(previous_row[i]);
	}

	return result->lengths;
}
/* }}} */


/* {{{ mysqlnd_fetch_lengths_unbuffered */
static unsigned long *
mysqlnd_fetch_lengths_unbuffered(MYSQLND_RES * const result TSRMLS_DC)
{
	/* simulate output of libmysql */
	return (!result->unbuf || result->unbuf->last_row_data || result->unbuf->eof_reached)? result->lengths:NULL;
}
/* }}} */


/* {{{ mysqlnd_res::fetch_lengths */
PHPAPI unsigned long * _mysqlnd_fetch_lengths(MYSQLND_RES * const result TSRMLS_DC)
{
	return result->m.fetch_lengths? result->m.fetch_lengths(result TSRMLS_CC) : NULL;
}
/* }}} */


/* {{{ mysqlnd_fetch_row_unbuffered_c */
static MYSQLND_ROW_C
mysqlnd_fetch_row_unbuffered_c(MYSQLND_RES * result TSRMLS_DC)
{
	enum_func_status	ret;
	MYSQLND_ROW_C		retrow = NULL;
	unsigned int		i,
						field_count = result->field_count;
	MYSQLND_PACKET_ROW	*row_packet = result->row_packet;
	unsigned long		*lengths = result->lengths;

	DBG_ENTER("mysqlnd_fetch_row_unbuffered_c");

	if (result->unbuf->eof_reached) {
		/* No more rows obviously */
		DBG_RETURN(retrow);
	}
	if (CONN_GET_STATE(result->conn) != CONN_FETCHING_DATA) {
		SET_CLIENT_ERROR(*result->conn->error_info, CR_COMMANDS_OUT_OF_SYNC,
						 UNKNOWN_SQLSTATE, mysqlnd_out_of_sync); 
		DBG_RETURN(retrow);
	}
	if (!row_packet) {
		/* Not fully initialized object that is being cleaned up */
		DBG_RETURN(retrow);
	}
	/* Let the row packet fill our buffer and skip additional mnd_malloc + memcpy */
	row_packet->skip_extraction = FALSE;

	/*
	  If we skip rows (row == NULL) we have to
	  result->m.unbuffered_free_last_data() before it. The function returns always true.
	*/
	if (PASS == (ret = PACKET_READ(row_packet, result->conn)) && !row_packet->eof) {
		result->unbuf->row_count++;

		result->m.unbuffered_free_last_data(result TSRMLS_CC);

		result->unbuf->last_row_data = row_packet->fields;
		result->unbuf->last_row_buffer = row_packet->row_buffer;
		row_packet->fields = NULL;
		row_packet->row_buffer = NULL;

		MYSQLND_INC_CONN_STATISTIC(result->conn->stats, STAT_ROWS_FETCHED_FROM_CLIENT_NORMAL_UNBUF);

		if (!row_packet->skip_extraction) {
			MYSQLND_FIELD *field = result->meta->fields;
			struct mysqlnd_field_hash_key * hash_key = result->meta->zend_hash_keys;

			enum_func_status rc = result->m.row_decoder(result->unbuf->last_row_buffer,
										  result->unbuf->last_row_data,
										  row_packet->field_count,
										  row_packet->fields_metadata,
										  result->conn->options->int_and_float_native,
										  result->conn->stats TSRMLS_CC);
			if (PASS != rc) {
				DBG_RETURN(retrow);
			}

			retrow = mnd_malloc(result->field_count * sizeof(char *));
			if (retrow) {
				for (i = 0; i < field_count; i++, field++, hash_key++) {
					zval *data = result->unbuf->last_row_data[i];
					unsigned int len;

					if (Z_TYPE_P(data) != IS_NULL) {
						convert_to_string(data);
						retrow[i] = Z_STRVAL_P(data);
						len = Z_STRLEN_P(data);
					} else {
						retrow[i] = NULL;
						len = 0;
					}

					if (lengths) {
						lengths[i] = len;
					}

					if (field->max_length < len) {
						field->max_length = len;
					}
				}
			} else {
				SET_OOM_ERROR(*result->conn->error_info);
			}
		}
	} else if (ret == FAIL) {
		if (row_packet->error_info.error_no) {
			COPY_CLIENT_ERROR(*result->conn->error_info, row_packet->error_info);
			DBG_ERR_FMT("errorno=%u error=%s", row_packet->error_info.error_no, row_packet->error_info.error);
		}
		CONN_SET_STATE(result->conn, CONN_READY);
		result->unbuf->eof_reached = TRUE; /* so next time we won't get an error */
	} else if (row_packet->eof) {
		/* Mark the connection as usable again */
		DBG_INF_FMT("warnings=%u server_status=%u", row_packet->warning_count, row_packet->server_status);
		result->unbuf->eof_reached = TRUE;
		memset(result->conn->upsert_status, 0, sizeof(*result->conn->upsert_status));
		result->conn->upsert_status->warning_count = row_packet->warning_count;
		result->conn->upsert_status->server_status = row_packet->server_status;
		/*
		  result->row_packet will be cleaned when
		  destroying the result object
		*/
		if (result->conn->upsert_status->server_status & SERVER_MORE_RESULTS_EXISTS) {
			CONN_SET_STATE(result->conn, CONN_NEXT_RESULT_PENDING);
		} else {
			CONN_SET_STATE(result->conn, CONN_READY);
		}
		result->m.unbuffered_free_last_data(result TSRMLS_CC);
	}

	DBG_RETURN(retrow);
}
/* }}} */


/* {{{ mysqlnd_fetch_row_unbuffered */
static enum_func_status
mysqlnd_fetch_row_unbuffered(MYSQLND_RES * result, void *param, unsigned int flags, zend_bool *fetched_anything TSRMLS_DC)
{
	enum_func_status	ret;
	zval				*row = (zval *) param;
	MYSQLND_PACKET_ROW	*row_packet = result->row_packet;

	DBG_ENTER("mysqlnd_fetch_row_unbuffered");

	*fetched_anything = FALSE;
	if (result->unbuf->eof_reached) {
		/* No more rows obviously */
		DBG_RETURN(PASS);
	}
	if (CONN_GET_STATE(result->conn) != CONN_FETCHING_DATA) {
		SET_CLIENT_ERROR(*result->conn->error_info, CR_COMMANDS_OUT_OF_SYNC, UNKNOWN_SQLSTATE, mysqlnd_out_of_sync);
		DBG_RETURN(FAIL);
	}
	if (!row_packet) {
		/* Not fully initialized object that is being cleaned up */
		DBG_RETURN(FAIL);
	}
	/* Let the row packet fill our buffer and skip additional mnd_malloc + memcpy */
	row_packet->skip_extraction = row? FALSE:TRUE;

	/*
	  If we skip rows (row == NULL) we have to
	  result->m.unbuffered_free_last_data() before it. The function returns always true.
	*/
	if (PASS == (ret = PACKET_READ(row_packet, result->conn)) && !row_packet->eof) {
		result->m.unbuffered_free_last_data(result TSRMLS_CC);

		result->unbuf->last_row_data = row_packet->fields;
		result->unbuf->last_row_buffer = row_packet->row_buffer;
		row_packet->fields = NULL;
		row_packet->row_buffer = NULL;

		MYSQLND_INC_CONN_STATISTIC(result->conn->stats, STAT_ROWS_FETCHED_FROM_CLIENT_NORMAL_UNBUF);

		if (!row_packet->skip_extraction) {
			HashTable *row_ht = Z_ARRVAL_P(row);
			MYSQLND_FIELD *field = result->meta->fields;
			struct mysqlnd_field_hash_key * hash_key = result->meta->zend_hash_keys;
			unsigned int i, field_count = result->field_count;
			unsigned long *lengths = result->lengths;

			enum_func_status rc = result->m.row_decoder(result->unbuf->last_row_buffer,
											result->unbuf->last_row_data,
											field_count,
											row_packet->fields_metadata,
											result->conn->options->int_and_float_native,
											result->conn->stats TSRMLS_CC);
			if (PASS != rc) {
				DBG_RETURN(FAIL);
			}
			for (i = 0; i < field_count; i++, field++, hash_key++) {
				zval *data = result->unbuf->last_row_data[i];
				unsigned int len = (Z_TYPE_P(data) == IS_NULL)? 0:Z_STRLEN_P(data);

				if (lengths) {
					lengths[i] = len;
				}

				if (flags & MYSQLND_FETCH_NUM) {
					Z_ADDREF_P(data);
					zend_hash_next_index_insert(row_ht, &data, sizeof(zval *), NULL);
				}
				if (flags & MYSQLND_FETCH_ASSOC) {
					/* zend_hash_quick_update needs length + trailing zero */
					/* QQ: Error handling ? */
					/*
					  zend_hash_quick_update does not check, as add_assoc_zval_ex do, whether
					  the index is a numeric and convert it to it. This however means constant
					  hashing of the column name, which is not needed as it can be precomputed.
					*/
					Z_ADDREF_P(data);
					if (hash_key->is_numeric == FALSE) {
						zend_hash_quick_update(Z_ARRVAL_P(row),
											   field->name,
											   field->name_length + 1,
											   hash_key->key,
											   (void *) &data, sizeof(zval *), NULL);
					} else {
						zend_hash_index_update(Z_ARRVAL_P(row),
											   hash_key->key,
											   (void *) &data, sizeof(zval *), NULL);
					}
				}
				if (field->max_length < len) {
					field->max_length = len;
				}
			}
		}
		*fetched_anything = TRUE;
		result->unbuf->row_count++;
	} else if (ret == FAIL) {
		if (row_packet->error_info.error_no) {
			COPY_CLIENT_ERROR(*result->conn->error_info, row_packet->error_info);
			DBG_ERR_FMT("errorno=%u error=%s", row_packet->error_info.error_no, row_packet->error_info.error);
		}
		CONN_SET_STATE(result->conn, CONN_READY);
		result->unbuf->eof_reached = TRUE; /* so next time we won't get an error */
	} else if (row_packet->eof) {
		/* Mark the connection as usable again */
		DBG_INF_FMT("warnings=%u server_status=%u", row_packet->warning_count, row_packet->server_status);
		result->unbuf->eof_reached = TRUE;
		memset(result->conn->upsert_status, 0, sizeof(*result->conn->upsert_status));
		result->conn->upsert_status->warning_count = row_packet->warning_count;
		result->conn->upsert_status->server_status = row_packet->server_status;
		/*
		  result->row_packet will be cleaned when
		  destroying the result object
		*/
		if (result->conn->upsert_status->server_status & SERVER_MORE_RESULTS_EXISTS) {
			CONN_SET_STATE(result->conn, CONN_NEXT_RESULT_PENDING);
		} else {
			CONN_SET_STATE(result->conn, CONN_READY);
		}
		result->m.unbuffered_free_last_data(result TSRMLS_CC);
	}

	DBG_INF_FMT("ret=%s fetched=%u", ret == PASS? "PASS":"FAIL", *fetched_anything);
	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ mysqlnd_res::use_result */
static MYSQLND_RES *
MYSQLND_METHOD(mysqlnd_res, use_result)(MYSQLND_RES * const result, zend_bool ps TSRMLS_DC)
{
	DBG_ENTER("mysqlnd_res::use_result");

	SET_EMPTY_ERROR(*result->conn->error_info);

	if (ps == FALSE) {
		result->type			= MYSQLND_RES_NORMAL;
		result->m.fetch_row		= result->m.fetch_row_normal_unbuffered;
		result->m.fetch_lengths	= mysqlnd_fetch_lengths_unbuffered;
		result->m.row_decoder	= php_mysqlnd_rowp_read_text_protocol;
		result->lengths			= mnd_ecalloc(result->field_count, sizeof(unsigned long));
		if (!result->lengths) {
			goto oom;
		}
	} else {
		result->type			= MYSQLND_RES_PS_UNBUF;
		result->m.fetch_row		= NULL;
		/* result->m.fetch_row() will be set in mysqlnd_ps.c */
		result->m.fetch_lengths	= NULL; /* makes no sense */
		result->m.row_decoder	= php_mysqlnd_rowp_read_binary_protocol;
		result->lengths 		= NULL;
	}

	result->result_set_memory_pool = mysqlnd_mempool_create(MYSQLND_G(mempool_default_size) TSRMLS_CC);
	result->unbuf = mnd_ecalloc(1, sizeof(MYSQLND_RES_UNBUFFERED));
	if (!result->result_set_memory_pool || !result->unbuf) {
		goto oom;
	}

	/*
	  Will be freed in the mysqlnd_internal_free_result_contents() called
	  by the resource destructor. mysqlnd_fetch_row_unbuffered() expects
	  this to be not NULL.
	*/
	/* FALSE = non-persistent */
	result->row_packet = result->conn->protocol->m.get_row_packet(result->conn->protocol, FALSE TSRMLS_CC);
	if (!result->row_packet) {
		goto oom;
	}
	result->row_packet->result_set_memory_pool = result->result_set_memory_pool;
	result->row_packet->field_count = result->field_count;
	result->row_packet->binary_protocol = ps;
	result->row_packet->fields_metadata = result->meta->fields;
	result->row_packet->bit_fields_count = result->meta->bit_fields_count;
	result->row_packet->bit_fields_total_len = result->meta->bit_fields_total_len;

	DBG_RETURN(result);
oom:
	SET_OOM_ERROR(*result->conn->error_info);
	DBG_RETURN(NULL);
}
/* }}} */


/* {{{ mysqlnd_fetch_row_buffered_c */
static MYSQLND_ROW_C
mysqlnd_fetch_row_buffered_c(MYSQLND_RES * result TSRMLS_DC)
{
	MYSQLND_ROW_C ret = NULL;
	MYSQLND_RES_BUFFERED *set = result->stored_data;

	DBG_ENTER("mysqlnd_fetch_row_buffered_c");

	/* If we haven't read everything */
	if (set->data_cursor &&
		(set->data_cursor - set->data) < (set->row_count * result->meta->field_count))
	{
		zval **current_row = set->data_cursor;
		MYSQLND_FIELD *field = result->meta->fields;
		struct mysqlnd_field_hash_key * hash_key = result->meta->zend_hash_keys;
		unsigned int i;

		if (NULL == current_row[0]) {
			uint64_t row_num = (set->data_cursor - set->data) / result->meta->field_count;
			enum_func_status rc = result->m.row_decoder(set->row_buffers[row_num],
											current_row,
											result->meta->field_count,
											result->meta->fields,
											result->conn->options->int_and_float_native,
											result->conn->stats TSRMLS_CC);
			if (rc != PASS) {
				DBG_RETURN(ret);
			}
			set->initialized_rows++;
			for (i = 0; i < result->field_count; i++) {
				/*
				  NULL fields are 0 length, 0 is not more than 0
				  String of zero size, definitely can't be the next max_length.
				  Thus for NULL and zero-length we are quite efficient.
				*/
				if (Z_TYPE_P(current_row[i]) >= IS_STRING) {
					unsigned long len = Z_STRLEN_P(current_row[i]);
					if (field->max_length < len) {
						field->max_length = len;
					}
				}
			}
		}

		set->data_cursor += result->meta->field_count;
		MYSQLND_INC_GLOBAL_STATISTIC(STAT_ROWS_FETCHED_FROM_CLIENT_NORMAL_BUF);

		ret = mnd_malloc(result->field_count * sizeof(char *));
		if (ret) {
			for (i = 0; i < result->field_count; i++, field++, hash_key++) {
				zval *data = current_row[i];

				if (Z_TYPE_P(data) != IS_NULL) {
					convert_to_string(data);
					ret[i] = Z_STRVAL_P(data);
				} else {
					ret[i] = NULL;
				}
			}
		}
		/* there is no conn handle in this function thus we can't set OOM in error_info */
	} else {
		set->data_cursor = NULL;
		DBG_INF("EOF reached");
	}
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_fetch_row_buffered */
static enum_func_status
mysqlnd_fetch_row_buffered(MYSQLND_RES * result, void *param, unsigned int flags, zend_bool *fetched_anything TSRMLS_DC)
{
	unsigned int i;
	zval *row = (zval *) param;
	MYSQLND_RES_BUFFERED *set = result->stored_data;
	enum_func_status ret = FAIL;

	DBG_ENTER("mysqlnd_fetch_row_buffered");

	/* If we haven't read everything */
	if (set->data_cursor &&
		(set->data_cursor - set->data) < (set->row_count * result->meta->field_count))
	{
		zval **current_row = set->data_cursor;
		MYSQLND_FIELD *field = result->meta->fields;
		struct mysqlnd_field_hash_key * hash_key = result->meta->zend_hash_keys;

		if (NULL == current_row[0]) {
			uint64_t row_num = (set->data_cursor - set->data) / result->meta->field_count;
			enum_func_status rc = result->m.row_decoder(set->row_buffers[row_num],
											current_row,
											result->meta->field_count,
											result->meta->fields,
											result->conn->options->int_and_float_native,
											result->conn->stats TSRMLS_CC);
			if (rc != PASS) {
				DBG_RETURN(FAIL);
			}
			set->initialized_rows++;
			for (i = 0; i < result->field_count; i++) {
				/*
				  NULL fields are 0 length, 0 is not more than 0
				  String of zero size, definitely can't be the next max_length.
				  Thus for NULL and zero-length we are quite efficient.
				*/
				if (Z_TYPE_P(current_row[i]) >= IS_STRING) {
					unsigned long len = Z_STRLEN_P(current_row[i]);
					if (field->max_length < len) {
						field->max_length = len;
					}
				}
			}
		}

		for (i = 0; i < result->field_count; i++, field++, hash_key++) {
			zval *data = current_row[i];

			if (flags & MYSQLND_FETCH_NUM) {
				Z_ADDREF_P(data);
				zend_hash_next_index_insert(Z_ARRVAL_P(row), &data, sizeof(zval *), NULL);
			}
			if (flags & MYSQLND_FETCH_ASSOC) {
				/* zend_hash_quick_update needs length + trailing zero */
				/* QQ: Error handling ? */
				/*
				  zend_hash_quick_update does not check, as add_assoc_zval_ex do, whether
				  the index is a numeric and convert it to it. This however means constant
				  hashing of the column name, which is not needed as it can be precomputed.
				*/
				Z_ADDREF_P(data);
				if (hash_key->is_numeric == FALSE) {
					zend_hash_quick_update(Z_ARRVAL_P(row),
										   field->name,
										   field->name_length + 1,
										   hash_key->key,
										   (void *) &data, sizeof(zval *), NULL);
				} else {
					zend_hash_index_update(Z_ARRVAL_P(row),
										   hash_key->key,
										   (void *) &data, sizeof(zval *), NULL);
				}
			}
		}
		set->data_cursor += result->meta->field_count;
		*fetched_anything = TRUE;
		MYSQLND_INC_GLOBAL_STATISTIC(STAT_ROWS_FETCHED_FROM_CLIENT_NORMAL_BUF);
		ret = PASS;
	} else {
		set->data_cursor = NULL;
		*fetched_anything = FALSE;
		ret = PASS;
		DBG_INF("EOF reached");
	}
	DBG_INF_FMT("ret=PASS fetched=%u", *fetched_anything);
	DBG_RETURN(ret);
}
/* }}} */


#define STORE_RESULT_PREALLOCATED_SET_IF_NOT_EMPTY 2

/* {{{ mysqlnd_res::store_result_fetch_data */
enum_func_status
MYSQLND_METHOD(mysqlnd_res, store_result_fetch_data)(MYSQLND_CONN_DATA * const conn, MYSQLND_RES * result,
													MYSQLND_RES_METADATA *meta,
													zend_bool binary_protocol TSRMLS_DC)
{
	enum_func_status ret;
	MYSQLND_PACKET_ROW *row_packet = NULL;
	unsigned int next_extend = STORE_RESULT_PREALLOCATED_SET_IF_NOT_EMPTY, free_rows = 1;
	MYSQLND_RES_BUFFERED *set;

	DBG_ENTER("mysqlnd_res::store_result_fetch_data");

	result->stored_data	= set = mnd_ecalloc(1, sizeof(MYSQLND_RES_BUFFERED));
	if (!set) {
		SET_OOM_ERROR(*conn->error_info);
		ret = FAIL;
		goto end;
	}
	if (free_rows) {
		set->row_buffers = mnd_emalloc((size_t)(free_rows * sizeof(MYSQLND_MEMORY_POOL_CHUNK *)));
		if (!set->row_buffers) {
			SET_OOM_ERROR(*conn->error_info);
			ret = FAIL;
			goto end;
		}
	}
	set->references	= 1;

	/* non-persistent */
	row_packet = conn->protocol->m.get_row_packet(conn->protocol, FALSE TSRMLS_CC);
	if (!row_packet) {
		SET_OOM_ERROR(*conn->error_info);
		ret = FAIL;
		goto end;
	}
	row_packet->result_set_memory_pool = result->result_set_memory_pool;
	row_packet->field_count = meta->field_count;
	row_packet->binary_protocol = binary_protocol;
	row_packet->fields_metadata = meta->fields;
	row_packet->bit_fields_count	= meta->bit_fields_count;
	row_packet->bit_fields_total_len = meta->bit_fields_total_len;

	row_packet->skip_extraction = TRUE; /* let php_mysqlnd_rowp_read() not allocate row_packet->fields, we will do it */

	while (FAIL != (ret = PACKET_READ(row_packet, conn)) && !row_packet->eof) {
		if (!free_rows) {
			uint64_t total_allocated_rows = free_rows = next_extend = next_extend * 11 / 10; /* extend with 10% */
			MYSQLND_MEMORY_POOL_CHUNK ** new_row_buffers;
			total_allocated_rows += set->row_count;

			/* don't try to allocate more than possible - mnd_XXalloc expects size_t, and it can have narrower range than uint64_t */
			if (total_allocated_rows * sizeof(MYSQLND_MEMORY_POOL_CHUNK *) > SIZE_MAX) {
				SET_OOM_ERROR(*conn->error_info);
				ret = FAIL;
				goto end;
			}
			new_row_buffers = mnd_erealloc(set->row_buffers, (size_t)(total_allocated_rows * sizeof(MYSQLND_MEMORY_POOL_CHUNK *)));
			if (!new_row_buffers) {
				SET_OOM_ERROR(*conn->error_info);
				ret = FAIL;
				goto end;
			}
			set->row_buffers = new_row_buffers;
		}
		free_rows--;
		set->row_buffers[set->row_count] = row_packet->row_buffer;

		set->row_count++;

		/* So row_packet's destructor function won't efree() it */
		row_packet->fields = NULL;
		row_packet->row_buffer = NULL;

		/*
		  No need to FREE_ALLOCA as we can reuse the
		  'lengths' and 'fields' arrays. For lengths its absolutely safe.
		  'fields' is reused because the ownership of the strings has been
		  transfered above. 
		*/
	}
	/* Overflow ? */
	if (set->row_count) {
		/* don't try to allocate more than possible - mnd_XXalloc expects size_t, and it can have narrower range than uint64_t */
		if (set->row_count * meta->field_count * sizeof(zval *) > SIZE_MAX) {
			SET_OOM_ERROR(*conn->error_info);
			ret = FAIL;
			goto end;
		}
		/* if pecalloc is used valgrind barks gcc version 4.3.1 20080507 (prerelease) [gcc-4_3-branch revision 135036] (SUSE Linux) */
		set->data = mnd_emalloc((size_t)(set->row_count * meta->field_count * sizeof(zval *)));
		if (!set->data) {
			SET_OOM_ERROR(*conn->error_info);
			ret = FAIL;
			goto end;
		}
		memset(set->data, 0, (size_t)(set->row_count * meta->field_count * sizeof(zval *)));
	}

	MYSQLND_INC_CONN_STATISTIC_W_VALUE(conn->stats,
									   binary_protocol? STAT_ROWS_BUFFERED_FROM_CLIENT_PS:
														STAT_ROWS_BUFFERED_FROM_CLIENT_NORMAL,
									   set->row_count);

	/* Finally clean */
	if (row_packet->eof) { 
		memset(conn->upsert_status, 0, sizeof(*conn->upsert_status));
		conn->upsert_status->warning_count = row_packet->warning_count;
		conn->upsert_status->server_status = row_packet->server_status;
	}
	/* save some memory */
	if (free_rows) {
		/* don't try to allocate more than possible - mnd_XXalloc expects size_t, and it can have narrower range than uint64_t */
		if (set->row_count * sizeof(MYSQLND_MEMORY_POOL_CHUNK *) > SIZE_MAX) {
			SET_OOM_ERROR(*conn->error_info);
			ret = FAIL;
			goto end;
		}
		set->row_buffers = mnd_erealloc(set->row_buffers, (size_t) (set->row_count * sizeof(MYSQLND_MEMORY_POOL_CHUNK *)));
	}

	if (conn->upsert_status->server_status & SERVER_MORE_RESULTS_EXISTS) {
		CONN_SET_STATE(conn, CONN_NEXT_RESULT_PENDING);
	} else {
		CONN_SET_STATE(conn, CONN_READY);
	}

	if (ret == FAIL) {
		COPY_CLIENT_ERROR(set->error_info, row_packet->error_info);
	} else {
		/* Position at the first row */
		set->data_cursor = set->data;

		/* libmysql's documentation says it should be so for SELECT statements */
		conn->upsert_status->affected_rows = set->row_count;
	}
	DBG_INF_FMT("ret=%s row_count=%u warnings=%u server_status=%u",
				ret == PASS? "PASS":"FAIL", (uint) set->row_count, conn->upsert_status->warning_count, conn->upsert_status->server_status);
end:
	PACKET_FREE(row_packet);

	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_res::store_result */
static MYSQLND_RES *
MYSQLND_METHOD(mysqlnd_res, store_result)(MYSQLND_RES * result,
										  MYSQLND_CONN_DATA * const conn,
										  zend_bool ps_protocol TSRMLS_DC)
{
	enum_func_status ret;

	DBG_ENTER("mysqlnd_res::store_result");

	/* We need the conn because we are doing lazy zval initialization in buffered_fetch_row */
	result->conn 			= conn->m->get_reference(conn TSRMLS_CC);
	result->type			= MYSQLND_RES_NORMAL;
	result->m.fetch_row		= result->m.fetch_row_normal_buffered;
	result->m.fetch_lengths	= mysqlnd_fetch_lengths_buffered;
	result->m.row_decoder = ps_protocol? php_mysqlnd_rowp_read_binary_protocol:
										 php_mysqlnd_rowp_read_text_protocol;

	result->result_set_memory_pool = mysqlnd_mempool_create(MYSQLND_G(mempool_default_size) TSRMLS_CC);
	result->lengths = mnd_ecalloc(result->field_count, sizeof(unsigned long));

	if (!result->result_set_memory_pool || !result->lengths) {
		SET_OOM_ERROR(*conn->error_info);
		DBG_RETURN(NULL);
	}

	CONN_SET_STATE(conn, CONN_FETCHING_DATA);

	ret = result->m.store_result_fetch_data(conn, result, result->meta, ps_protocol TSRMLS_CC);
	if (FAIL == ret) {
		if (result->stored_data) {
			COPY_CLIENT_ERROR(*conn->error_info, result->stored_data->error_info);
		} else {
			SET_OOM_ERROR(*conn->error_info);
		}
		DBG_RETURN(NULL);
	}
	/* libmysql's documentation says it should be so for SELECT statements */
	conn->upsert_status->affected_rows = result->stored_data->row_count;

	DBG_RETURN(result);
}
/* }}} */


/* {{{ mysqlnd_res::skip_result */
static enum_func_status
MYSQLND_METHOD(mysqlnd_res, skip_result)(MYSQLND_RES * const result TSRMLS_DC)
{
	zend_bool fetched_anything;

	DBG_ENTER("mysqlnd_res::skip_result");
	/*
	  Unbuffered sets
	  A PS could be prepared - there is metadata and thus a stmt->result but the
	  fetch_row function isn't actually set (NULL), thus we have to skip these.
	*/
	if (!result->stored_data && result->unbuf &&
		!result->unbuf->eof_reached && result->m.fetch_row)
	{
		DBG_INF("skipping result");
		/* We have to fetch all data to clean the line */
		MYSQLND_INC_CONN_STATISTIC(result->conn->stats,
									result->type == MYSQLND_RES_NORMAL? STAT_FLUSHED_NORMAL_SETS:
																		STAT_FLUSHED_PS_SETS);

		while ((PASS == result->m.fetch_row(result, NULL, 0, &fetched_anything TSRMLS_CC)) && fetched_anything == TRUE) {
			/* do nothing */;
		}
	}
	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ mysqlnd_res::free_result */
static enum_func_status
MYSQLND_METHOD(mysqlnd_res, free_result)(MYSQLND_RES * result, zend_bool implicit TSRMLS_DC)
{
	DBG_ENTER("mysqlnd_res::free_result");

	result->m.skip_result(result TSRMLS_CC);
	MYSQLND_INC_CONN_STATISTIC(result->conn? result->conn->stats : NULL,
							   implicit == TRUE?	STAT_FREE_RESULT_IMPLICIT:
							   						STAT_FREE_RESULT_EXPLICIT);

	result->m.free_result_internal(result TSRMLS_CC);
	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ mysqlnd_res::data_seek */
static enum_func_status
MYSQLND_METHOD(mysqlnd_res, data_seek)(MYSQLND_RES * result, uint64_t row TSRMLS_DC)
{
	DBG_ENTER("mysqlnd_res::data_seek");
	DBG_INF_FMT("row=%lu", row);

	if (!result->stored_data) {
		return FAIL;
	}

	/* libmysql just moves to the end, it does traversing of a linked list */
	if (row >= result->stored_data->row_count) {
		result->stored_data->data_cursor = NULL;
	} else {
		result->stored_data->data_cursor = result->stored_data->data + row * result->meta->field_count;
	}

	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ mysqlnd_res::num_rows */
static uint64_t
MYSQLND_METHOD(mysqlnd_res, num_rows)(const MYSQLND_RES * const result TSRMLS_DC)
{
	/* Be compatible with libmysql. We count row_count, but will return 0 */
	return result->stored_data? result->stored_data->row_count:(result->unbuf && result->unbuf->eof_reached? result->unbuf->row_count:0);
}
/* }}} */


/* {{{ mysqlnd_res::num_fields */
static unsigned int
MYSQLND_METHOD(mysqlnd_res, num_fields)(const MYSQLND_RES * const result TSRMLS_DC)
{
	return result->field_count;
}
/* }}} */


/* {{{ mysqlnd_res::fetch_field */
static const MYSQLND_FIELD *
MYSQLND_METHOD(mysqlnd_res, fetch_field)(MYSQLND_RES * const result TSRMLS_DC)
{
	DBG_ENTER("mysqlnd_res::fetch_field");
	do {
		if (result->meta) {
			/*
			  We optimize the result set, so we don't convert all the data from raw buffer format to
			  zval arrays during store. In the case someone doesn't read all the lines this will
			  save time. However, when a metadata call is done, we need to calculate max_length.
			  We don't have control whether max_length will be used, unfortunately. Otherwise we
			  could have been able to skip that step.
			  Well, if the mysqli API switches from returning stdClass to class like mysqli_field_metadata,
			  then we can have max_length as dynamic property, which will be calculated during runtime and
			  not during mysqli_fetch_field() time.
			*/
			if (result->stored_data && (result->stored_data->initialized_rows < result->stored_data->row_count)) {
				DBG_INF_FMT("We have decode the whole result set to be able to satisfy this meta request");
				/* we have to initialize the rest to get the updated max length */
				if (PASS != result->m.initialize_result_set_rest(result TSRMLS_CC)) {
					break;
				}
			}
			DBG_RETURN(result->meta->m->fetch_field(result->meta TSRMLS_CC));
		}
	} while (0);
	DBG_RETURN(NULL);
}
/* }}} */


/* {{{ mysqlnd_res::fetch_field_direct */
static const MYSQLND_FIELD *
MYSQLND_METHOD(mysqlnd_res, fetch_field_direct)(MYSQLND_RES * const result, MYSQLND_FIELD_OFFSET fieldnr TSRMLS_DC)
{
	DBG_ENTER("mysqlnd_res::fetch_field_direct");
	do {
		if (result->meta) {
			/*
			  We optimize the result set, so we don't convert all the data from raw buffer format to
			  zval arrays during store. In the case someone doesn't read all the lines this will
			  save time. However, when a metadata call is done, we need to calculate max_length.
			  We don't have control whether max_length will be used, unfortunately. Otherwise we
			  could have been able to skip that step.
			  Well, if the mysqli API switches from returning stdClass to class like mysqli_field_metadata,
			  then we can have max_length as dynamic property, which will be calculated during runtime and
			  not during mysqli_fetch_field_direct() time.
			*/
			if (result->stored_data && (result->stored_data->initialized_rows < result->stored_data->row_count)) {
				DBG_INF_FMT("We have decode the whole result set to be able to satisfy this meta request");
				/* we have to initialized the rest to get the updated max length */
				if (PASS != result->m.initialize_result_set_rest(result TSRMLS_CC)) {
					break;
				}
			}
			DBG_RETURN(result->meta->m->fetch_field_direct(result->meta, fieldnr TSRMLS_CC));
		}
	} while (0);

	DBG_RETURN(NULL);
}
/* }}} */


/* {{{ mysqlnd_res::fetch_field */
static const MYSQLND_FIELD *
MYSQLND_METHOD(mysqlnd_res, fetch_fields)(MYSQLND_RES * const result TSRMLS_DC)
{
	DBG_ENTER("mysqlnd_res::fetch_fields");
	do {
		if (result->meta) {
			if (result->stored_data && (result->stored_data->initialized_rows < result->stored_data->row_count)) {
				/* we have to initialize the rest to get the updated max length */
				if (PASS != result->m.initialize_result_set_rest(result TSRMLS_CC)) {
					break;
				}
			}
			DBG_RETURN(result->meta->m->fetch_fields(result->meta TSRMLS_CC));
		}
	} while (0);
	DBG_RETURN(NULL);
}
/* }}} */



/* {{{ mysqlnd_res::field_seek */
static MYSQLND_FIELD_OFFSET
MYSQLND_METHOD(mysqlnd_res, field_seek)(MYSQLND_RES * const result, MYSQLND_FIELD_OFFSET field_offset TSRMLS_DC)
{
	MYSQLND_FIELD_OFFSET return_value = 0;
	if (result->meta) {
		return_value = result->meta->current_field;
		result->meta->current_field = field_offset;
	}
	return return_value;
}
/* }}} */


/* {{{ mysqlnd_res::field_tell */
static MYSQLND_FIELD_OFFSET
MYSQLND_METHOD(mysqlnd_res, field_tell)(const MYSQLND_RES * const result TSRMLS_DC)
{
	return result->meta? result->meta->m->field_tell(result->meta TSRMLS_CC) : 0;
}
/* }}} */


/* {{{ mysqlnd_res::fetch_into */
static void
MYSQLND_METHOD(mysqlnd_res, fetch_into)(MYSQLND_RES * result, unsigned int flags,
										zval *return_value,
										enum_mysqlnd_extension extension TSRMLS_DC ZEND_FILE_LINE_DC)
{
	zend_bool fetched_anything;

	DBG_ENTER("mysqlnd_res::fetch_into");

	if (!result->m.fetch_row) {
		RETVAL_NULL();
		DBG_VOID_RETURN;
	}
	/*
	  Hint Zend how many elements we will have in the hash. Thus it won't
	  extend and rehash the hash constantly.
	*/
	mysqlnd_array_init(return_value, mysqlnd_num_fields(result) * 2);
	if (FAIL == result->m.fetch_row(result, (void *)return_value, flags, &fetched_anything TSRMLS_CC)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Error while reading a row");
		zval_dtor(return_value);
		RETVAL_FALSE;
	} else if (fetched_anything == FALSE) {
		zval_dtor(return_value);
		switch (extension) {
			case MYSQLND_MYSQLI:
				RETVAL_NULL();
				break;
			case MYSQLND_MYSQL:
				RETVAL_FALSE;
				break;
			default:exit(0);
		}
	}
	/*
	  return_value is IS_NULL for no more data and an array for data. Thus it's ok
	  to return here.
	*/
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_res::fetch_row_c */
static MYSQLND_ROW_C
MYSQLND_METHOD(mysqlnd_res, fetch_row_c)(MYSQLND_RES * result TSRMLS_DC)
{
	MYSQLND_ROW_C ret = NULL;
	DBG_ENTER("mysqlnd_res::fetch_row_c");

	if (result->m.fetch_row) {
		if (result->m.fetch_row == result->m.fetch_row_normal_buffered) {
			DBG_RETURN(mysqlnd_fetch_row_buffered_c(result TSRMLS_CC));
		} else if (result->m.fetch_row == result->m.fetch_row_normal_unbuffered) {
			DBG_RETURN(mysqlnd_fetch_row_unbuffered_c(result TSRMLS_CC));
		} else {
			php_error_docref(NULL TSRMLS_CC, E_ERROR, "result->m.fetch_row has invalid value. Report to the developers");
		}
	}
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_res::fetch_all */
static void
MYSQLND_METHOD(mysqlnd_res, fetch_all)(MYSQLND_RES * result, unsigned int flags, zval *return_value TSRMLS_DC ZEND_FILE_LINE_DC)
{
	zval  *row;
	ulong i = 0;
	MYSQLND_RES_BUFFERED *set = result->stored_data;

	DBG_ENTER("mysqlnd_res::fetch_all");

	if ((!result->unbuf && !set)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "fetch_all can be used only with buffered sets");
		if (result->conn) {
			SET_CLIENT_ERROR(*result->conn->error_info, CR_NOT_IMPLEMENTED, UNKNOWN_SQLSTATE, "fetch_all can be used only with buffered sets");
		}
		RETVAL_NULL();
		DBG_VOID_RETURN;
	}

	/* 4 is a magic value. The cast is safe, if larger then the array will be later extended - no big deal :) */
	mysqlnd_array_init(return_value, set? (unsigned int) set->row_count : 4); 

	do {
		MAKE_STD_ZVAL(row);
		mysqlnd_fetch_into(result, flags, row, MYSQLND_MYSQLI);
		if (Z_TYPE_P(row) != IS_ARRAY) {
			zval_ptr_dtor(&row);
			break;
		}
		add_index_zval(return_value, i++, row);
	} while (1);

	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_res::fetch_field_data */
static void
MYSQLND_METHOD(mysqlnd_res, fetch_field_data)(MYSQLND_RES * result, unsigned int offset, zval *return_value TSRMLS_DC)
{
	zval row;
	zval **entry;
	unsigned int i = 0;

	DBG_ENTER("mysqlnd_res::fetch_field_data");
	DBG_INF_FMT("offset=%u", offset);

	if (!result->m.fetch_row) {
		RETVAL_NULL();
		DBG_VOID_RETURN;
	}
	/*
	  Hint Zend how many elements we will have in the hash. Thus it won't
	  extend and rehash the hash constantly.
	*/
	INIT_PZVAL(&row);
	mysqlnd_fetch_into(result, MYSQLND_FETCH_NUM, &row, MYSQLND_MYSQL);
	if (Z_TYPE(row) != IS_ARRAY) {
		zval_dtor(&row);
		RETVAL_NULL();
		DBG_VOID_RETURN;
	}
	zend_hash_internal_pointer_reset(Z_ARRVAL(row));
	while (i++ < offset) {
		zend_hash_move_forward(Z_ARRVAL(row));
		zend_hash_get_current_data(Z_ARRVAL(row), (void **)&entry);
	}

	zend_hash_get_current_data(Z_ARRVAL(row), (void **)&entry);

	*return_value = **entry;
	zval_copy_ctor(return_value);
	Z_SET_REFCOUNT_P(return_value, 1);
	zval_dtor(&row);

	DBG_VOID_RETURN;
}
/* }}} */


MYSQLND_CLASS_METHODS_START(mysqlnd_res)
	NULL, /* fetch_row */
	mysqlnd_fetch_row_buffered,
	mysqlnd_fetch_row_unbuffered,
	MYSQLND_METHOD(mysqlnd_res, use_result),
	MYSQLND_METHOD(mysqlnd_res, store_result),
	MYSQLND_METHOD(mysqlnd_res, fetch_into),
	MYSQLND_METHOD(mysqlnd_res, fetch_row_c),
	MYSQLND_METHOD(mysqlnd_res, fetch_all),
	MYSQLND_METHOD(mysqlnd_res, fetch_field_data),
	MYSQLND_METHOD(mysqlnd_res, num_rows),
	MYSQLND_METHOD(mysqlnd_res, num_fields),
	MYSQLND_METHOD(mysqlnd_res, skip_result),
	MYSQLND_METHOD(mysqlnd_res, data_seek),
	MYSQLND_METHOD(mysqlnd_res, field_seek),
	MYSQLND_METHOD(mysqlnd_res, field_tell),
	MYSQLND_METHOD(mysqlnd_res, fetch_field),
	MYSQLND_METHOD(mysqlnd_res, fetch_field_direct),
	MYSQLND_METHOD(mysqlnd_res, fetch_fields),
	MYSQLND_METHOD(mysqlnd_res, read_result_metadata),
	NULL, /* fetch_lengths */
	MYSQLND_METHOD(mysqlnd_res, store_result_fetch_data),
	MYSQLND_METHOD(mysqlnd_res, initialize_result_set_rest),
	MYSQLND_METHOD(mysqlnd_res, free_result_buffers),
	MYSQLND_METHOD(mysqlnd_res, free_result),

	mysqlnd_internal_free_result, /* free_result_internal */
	mysqlnd_internal_free_result_contents, /* free_result_contents */
	MYSQLND_METHOD(mysqlnd_res, free_buffered_data),
	MYSQLND_METHOD(mysqlnd_res, unbuffered_free_last_data),

	NULL /* row_decoder */,
	mysqlnd_result_meta_init
MYSQLND_CLASS_METHODS_END;


/* {{{ mysqlnd_result_init */
PHPAPI MYSQLND_RES *
mysqlnd_result_init(unsigned int field_count, zend_bool persistent TSRMLS_DC)
{
	size_t alloc_size = sizeof(MYSQLND_RES) + mysqlnd_plugin_count() * sizeof(void *);
	MYSQLND_RES *ret = mnd_pecalloc(1, alloc_size, persistent);

	DBG_ENTER("mysqlnd_result_init");

	if (!ret) {
		DBG_RETURN(NULL);
	}

	ret->persistent		= persistent;
	ret->field_count	= field_count;
	ret->m = *mysqlnd_result_get_methods();

	DBG_RETURN(ret);
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
