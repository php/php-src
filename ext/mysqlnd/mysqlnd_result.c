/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2006-2016 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Andrey Hristov <andrey@php.net>                             |
  |          Ulf Wendel <uw@php.net>                                     |
  |          Georg Richter <georg@php.net>                               |
  +----------------------------------------------------------------------+
*/

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

/* {{{ mysqlnd_result_buffered_zval::initialize_result_set_rest */
static enum_func_status
MYSQLND_METHOD(mysqlnd_result_buffered_zval, initialize_result_set_rest)(MYSQLND_RES_BUFFERED * const result, MYSQLND_RES_METADATA * const meta,
																		 MYSQLND_STATS * stats, zend_bool int_and_float_native)
{
	unsigned int i;
	enum_func_status ret = PASS;
	const unsigned int field_count = meta->field_count;
	const uint64_t row_count = result->row_count;
	enum_func_status rc;

	zval *data_begin = ((MYSQLND_RES_BUFFERED_ZVAL *) result)->data;
	zval *data_cursor = data_begin;

	DBG_ENTER("mysqlnd_result_buffered_zval::initialize_result_set_rest");

	if (!data_cursor || row_count == result->initialized_rows) {
		DBG_RETURN(ret);
	}
	while ((data_cursor - data_begin) < (int)(row_count * field_count)) {
		if (Z_ISUNDEF(data_cursor[0])) {
			rc = result->m.row_decoder(result->row_buffers[(data_cursor - data_begin) / field_count],
									data_cursor,
									field_count,
									meta->fields,
									int_and_float_native,
									stats);
			if (rc != PASS) {
				ret = FAIL;
				break;
			}
			result->initialized_rows++;
			for (i = 0; i < field_count; i++) {
				/*
				  NULL fields are 0 length, 0 is not more than 0
				  String of zero size, definitely can't be the next max_length.
				  Thus for NULL and zero-length we are quite efficient.
				*/
				if (Z_TYPE(data_cursor[i]) == IS_STRING) {
					zend_ulong len = Z_STRLEN(data_cursor[i]);
					if (meta->fields[i].max_length < len) {
						meta->fields[i].max_length = len;
					}
				}
			}
		}
		data_cursor += field_count;
	}
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_result_buffered_c::initialize_result_set_rest */
static enum_func_status
MYSQLND_METHOD(mysqlnd_result_buffered_c, initialize_result_set_rest)(MYSQLND_RES_BUFFERED * const result, MYSQLND_RES_METADATA * const meta,
																	  MYSQLND_STATS * stats, zend_bool int_and_float_native)
{
	unsigned int i;
	enum_func_status ret = PASS;
	const unsigned int field_count = meta->field_count;
	const uint64_t row_count = result->row_count;
	enum_func_status rc;
	DBG_ENTER("mysqlnd_result_buffered_c::initialize_result_set_rest");

	if (result->initialized_rows < row_count) {
		zend_uchar * initialized = ((MYSQLND_RES_BUFFERED_C *) result)->initialized;
		zval * current_row = mnd_emalloc(field_count * sizeof(zval));

		if (!current_row) {
			DBG_RETURN(FAIL);
		}

		for (i = 0; i < result->row_count; i++) {
			/* (i / 8) & the_bit_for_i*/
			if (initialized[i >> 3] & (1 << (i & 7))) {
				continue;
			}

			rc = result->m.row_decoder(result->row_buffers[i], current_row, field_count, meta->fields, int_and_float_native, stats);

			if (rc != PASS) {
				ret = FAIL;
				break;
			}
			result->initialized_rows++;
			initialized[i >> 3] |= (1 << (i & 7));
			for (i = 0; i < field_count; i++) {
				/*
				  NULL fields are 0 length, 0 is not more than 0
				  String of zero size, definitely can't be the next max_length.
				  Thus for NULL and zero-length we are quite efficient.
				*/
				if (Z_TYPE(current_row[i]) == IS_STRING) {
					zend_ulong len = Z_STRLEN(current_row[i]);
					if (meta->fields[i].max_length < len) {
						meta->fields[i].max_length = len;
					}
				}
				zval_ptr_dtor(&current_row[i]);
			}
		}
		mnd_efree(current_row);
	}
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_result_unbuffered::free_last_data */
static void
MYSQLND_METHOD(mysqlnd_result_unbuffered, free_last_data)(MYSQLND_RES_UNBUFFERED * unbuf, MYSQLND_STATS * const global_stats)
{
	DBG_ENTER("mysqlnd_res::unbuffered_free_last_data");

	if (!unbuf) {
		DBG_VOID_RETURN;
	}

	DBG_INF_FMT("field_count=%u", unbuf->field_count);
	if (unbuf->last_row_data) {
		unsigned int i;
		for (i = 0; i < unbuf->field_count; i++) {
			zval_ptr_dtor(&(unbuf->last_row_data[i]));
		}

		/* Free last row's zvals */
		mnd_efree(unbuf->last_row_data);
		unbuf->last_row_data = NULL;
	}
	if (unbuf->last_row_buffer) {
		DBG_INF("Freeing last row buffer");
		/* Nothing points to this buffer now, free it */
		unbuf->last_row_buffer->free_chunk(unbuf->last_row_buffer);
		unbuf->last_row_buffer = NULL;
	}

	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_result_unbuffered::free_result */
static void
MYSQLND_METHOD(mysqlnd_result_unbuffered, free_result)(MYSQLND_RES_UNBUFFERED * const result, MYSQLND_STATS * const global_stats)
{
	DBG_ENTER("mysqlnd_result_unbuffered, free_result");
	result->m.free_last_data(result, global_stats);

	if (result->lengths) {
		mnd_pefree(result->lengths, result->persistent);
		result->lengths = NULL;
	}

	/* must be free before because references the memory pool */
	if (result->row_packet) {
		PACKET_FREE(result->row_packet);
		result->row_packet = NULL;
	}

	if (result->result_set_memory_pool) {
		mysqlnd_mempool_destroy(result->result_set_memory_pool);
		result->result_set_memory_pool = NULL;
	}


	mnd_pefree(result, result->persistent);
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_result_buffered_zval::free_result */
static void
MYSQLND_METHOD(mysqlnd_result_buffered_zval, free_result)(MYSQLND_RES_BUFFERED_ZVAL * const set)
{
	zval * data = set->data;

	DBG_ENTER("mysqlnd_result_buffered_zval::free_result");

	set->data = NULL; /* prevent double free if following loop is interrupted */
	if (data) {
		unsigned int field_count = set->field_count;
		int64_t row;

		for (row = set->row_count - 1; row >= 0; row--) {
			zval *current_row = data + row * field_count;
			int64_t col;

			if (current_row != NULL) {
				for (col = field_count - 1; col >= 0; --col) {
					zval_ptr_dtor(&(current_row[col]));
				}
			}
		}
		mnd_efree(data);
	}
	set->data_cursor = NULL;
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_result_buffered_c::free_result */
static void
MYSQLND_METHOD(mysqlnd_result_buffered_c, free_result)(MYSQLND_RES_BUFFERED_C * const set)
{
	DBG_ENTER("mysqlnd_result_buffered_c::free_result");
	mnd_pefree(set->initialized, set->persistent);
	set->initialized = NULL;
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_result_buffered::free_result */
static void
MYSQLND_METHOD(mysqlnd_result_buffered, free_result)(MYSQLND_RES_BUFFERED * const set)
{
	int64_t row;

	DBG_ENTER("mysqlnd_result_buffered::free_result");
	DBG_INF_FMT("Freeing "MYSQLND_LLU_SPEC" row(s)", set->row_count);

	if (set->type == MYSQLND_BUFFERED_TYPE_ZVAL) {
		MYSQLND_METHOD(mysqlnd_result_buffered_zval, free_result)((MYSQLND_RES_BUFFERED_ZVAL *) set);
	} if (set->type == MYSQLND_BUFFERED_TYPE_C) {
		MYSQLND_METHOD(mysqlnd_result_buffered_c, free_result)((MYSQLND_RES_BUFFERED_C *) set);
	}

	for (row = set->row_count - 1; row >= 0; row--) {
		MYSQLND_MEMORY_POOL_CHUNK *current_buffer = set->row_buffers[row];
		current_buffer->free_chunk(current_buffer);
	}

	if (set->lengths) {
		mnd_pefree(set->lengths, set->persistent);
		set->lengths = NULL;
	}

	if (set->row_buffers) {
		mnd_pefree(set->row_buffers, 0);
		set->row_buffers = NULL;
	}

	if (set->result_set_memory_pool) {
		mysqlnd_mempool_destroy(set->result_set_memory_pool);
		set->result_set_memory_pool = NULL;
	}


	set->row_count	= 0;

	mnd_pefree(set, set->persistent);

	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_res::free_result_buffers */
static void
MYSQLND_METHOD(mysqlnd_res, free_result_buffers)(MYSQLND_RES * result)
{
	DBG_ENTER("mysqlnd_res::free_result_buffers");
	DBG_INF_FMT("%s", result->unbuf? "unbuffered":(result->stored_data? "buffered":"unknown"));

	if (result->unbuf) {
		result->unbuf->m.free_result(result->unbuf, result->conn? result->conn->stats : NULL);
		result->unbuf = NULL;
	} else if (result->stored_data) {
		result->stored_data->m.free_result(result->stored_data);
		result->stored_data = NULL;
	}


	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_res::free_result_contents_internal */
static
void MYSQLND_METHOD(mysqlnd_res, free_result_contents_internal)(MYSQLND_RES * result)
{
	DBG_ENTER("mysqlnd_res::free_result_contents_internal");

	result->m.free_result_buffers(result);

	if (result->meta) {
		result->meta->m->free_metadata(result->meta);
		result->meta = NULL;
	}

	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_res::free_result_internal */
static
void MYSQLND_METHOD(mysqlnd_res, free_result_internal)(MYSQLND_RES * result)
{
	DBG_ENTER("mysqlnd_res::free_result_internal");
	result->m.skip_result(result);

	result->m.free_result_contents(result);

	if (result->conn) {
		result->conn->m->free_reference(result->conn);
		result->conn = NULL;
	}

	mnd_pefree(result, result->persistent);

	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_res::read_result_metadata */
static enum_func_status
MYSQLND_METHOD(mysqlnd_res, read_result_metadata)(MYSQLND_RES * result, MYSQLND_CONN_DATA * conn)
{
	DBG_ENTER("mysqlnd_res::read_result_metadata");

	/*
	  Make it safe to call it repeatedly for PS -
	  better free and allocate a new because the number of field might change
	  (select *) with altered table. Also for statements which skip the PS
	  infrastructure!
	*/
	if (result->meta) {
		result->meta->m->free_metadata(result->meta);
		result->meta = NULL;
	}

	result->meta = result->m.result_meta_init(result->field_count, result->persistent);
	if (!result->meta) {
		SET_OOM_ERROR(*conn->error_info);
		DBG_RETURN(FAIL);
	}

	/* 1. Read all fields metadata */

	/* It's safe to reread without freeing */
	if (FAIL == result->meta->m->read_metadata(result->meta, conn)) {
		result->m.free_result_contents(result);
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
mysqlnd_query_read_result_set_header(MYSQLND_CONN_DATA * conn, MYSQLND_STMT * s)
{
	MYSQLND_STMT_DATA * stmt = s ? s->data:NULL;
	enum_func_status ret;
	MYSQLND_PACKET_RSET_HEADER * rset_header = NULL;
	MYSQLND_PACKET_EOF * fields_eof = NULL;

	DBG_ENTER("mysqlnd_query_read_result_set_header");
	DBG_INF_FMT("stmt=%lu", stmt? stmt->stmt_id:0);

	ret = FAIL;
	do {
		rset_header = conn->protocol->m.get_rset_header_packet(conn->protocol, FALSE);
		if (!rset_header) {
			SET_OOM_ERROR(*conn->error_info);
			ret = FAIL;
			break;
		}

		SET_ERROR_AFF_ROWS(conn);

		if (FAIL == (ret = PACKET_READ(rset_header, conn))) {
			php_error_docref(NULL, E_WARNING, "Error reading result set's header");
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
				ret = mysqlnd_handle_local_infile(conn, rset_header->info_or_local_file, &is_warning);
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
					result = conn->current_result = conn->m->result_init(rset_header->field_count, conn->persistent);
				} else {
					if (!stmt->result) {
						DBG_INF("This is 'SHOW'/'EXPLAIN'-like query.");
						/*
						  This is 'SHOW'/'EXPLAIN'-like query. Current implementation of
						  prepared statements can't send result set metadata for these queries
						  on prepare stage. Read it now.
						*/
						result = stmt->result = conn->m->result_init(rset_header->field_count, stmt->persistent);
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

				if (FAIL == (ret = result->m.read_result_metadata(result, conn))) {
					/* For PS, we leave them in Prepared state */
					if (!stmt && conn->current_result) {
						mnd_efree(conn->current_result);
						conn->current_result = NULL;
					}
					DBG_ERR("Error occurred while reading metadata");
					break;
				}

				/* Check for SERVER_STATUS_MORE_RESULTS if needed */
				fields_eof = conn->protocol->m.get_eof_packet(conn->protocol, FALSE);
				if (!fields_eof) {
					SET_OOM_ERROR(*conn->error_info);
					ret = FAIL;
					break;
				}
				if (FAIL == (ret = PACKET_READ(fields_eof, conn))) {
					DBG_ERR("Error occurred while reading the EOF packet");
					result->m.free_result_contents(result);
					mnd_efree(result);
					if (!stmt) {
						conn->current_result = NULL;
					} else {
						stmt->result = NULL;
						memset(stmt, 0, sizeof(*stmt));
						stmt->state = MYSQLND_STMT_INITTED;
					}
				} else {
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


/* {{{ mysqlnd_result_buffered::fetch_lengths */
/*
  Do lazy initialization for buffered results. As PHP strings have
  length inside, this function makes not much sense in the context
  of PHP, to be called as separate function. But let's have it for
  completeness.
*/
static zend_ulong *
MYSQLND_METHOD(mysqlnd_result_buffered_zval, fetch_lengths)(MYSQLND_RES_BUFFERED * const result)
{
	const MYSQLND_RES_BUFFERED_ZVAL * set = (MYSQLND_RES_BUFFERED_ZVAL *) result;
	/*
	  If:
	  - unbuffered result
	  - first row has not been read
	  - last_row has been read
	*/
	DBG_ENTER("mysqlnd_result_buffered_zval::fetch_lengths");

	if (set->data_cursor == NULL ||
		set->data_cursor == set->data ||
		((set->data_cursor - set->data) > (result->row_count * result->field_count) ))
	{
		DBG_INF("EOF");
		DBG_RETURN(NULL);/* No rows or no more rows */
	}
	DBG_INF("non NULL");
	DBG_RETURN(result->lengths);
}
/* }}} */


/* {{{ mysqlnd_result_buffered_c::fetch_lengths */
/*
  Do lazy initialization for buffered results. As PHP strings have
  length inside, this function makes not much sense in the context
  of PHP, to be called as separate function. But let's have it for
  completeness.
*/
static zend_ulong *
MYSQLND_METHOD(mysqlnd_result_buffered_c, fetch_lengths)(MYSQLND_RES_BUFFERED * const result)
{
	const MYSQLND_RES_BUFFERED_C * set = (MYSQLND_RES_BUFFERED_C *) result;
	DBG_ENTER("mysqlnd_result_buffered_c::fetch_lengths");

	if (set->current_row > set->row_count || set->current_row == 0) {
		DBG_INF("EOF");
		DBG_RETURN(NULL); /* No more rows, or no fetched row */
	}
	DBG_INF("non NULL");
	DBG_RETURN(result->lengths);
}
/* }}} */


/* {{{ mysqlnd_result_unbuffered::fetch_lengths */
static zend_ulong *
MYSQLND_METHOD(mysqlnd_result_unbuffered, fetch_lengths)(MYSQLND_RES_UNBUFFERED * const result)
{
	/* simulate output of libmysql */
	return (result->last_row_data || result->eof_reached)? result->lengths : NULL;
}
/* }}} */


/* {{{ mysqlnd_res::fetch_lengths */
static zend_ulong *
MYSQLND_METHOD(mysqlnd_res, fetch_lengths)(MYSQLND_RES * const result)
{
	zend_ulong * ret;
	DBG_ENTER("mysqlnd_res::fetch_lengths");
	ret = result->stored_data && result->stored_data->m.fetch_lengths ?
					result->stored_data->m.fetch_lengths(result->stored_data) :
					(result->unbuf && result->unbuf->m.fetch_lengths ?
						result->unbuf->m.fetch_lengths(result->unbuf) :
						NULL
					);
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_result_unbuffered::fetch_row_c */
static enum_func_status
MYSQLND_METHOD(mysqlnd_result_unbuffered, fetch_row_c)(MYSQLND_RES * result, void * param, unsigned int flags, zend_bool * fetched_anything)
{
	enum_func_status	ret;
	MYSQLND_ROW_C		*row = (MYSQLND_ROW_C *) param;
	MYSQLND_PACKET_ROW	*row_packet = result->unbuf->row_packet;
	const MYSQLND_RES_METADATA * const meta = result->meta;

	DBG_ENTER("mysqlnd_result_unbuffered::fetch_row_c");

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
	row_packet->skip_extraction = FALSE;

	/*
	  If we skip rows (row == NULL) we have to
	  result->m.unbuffered_free_last_data() before it. The function returns always true.
	*/
	if (PASS == (ret = PACKET_READ(row_packet, result->conn)) && !row_packet->eof) {
		result->unbuf->m.free_last_data(result->unbuf, result->conn? result->conn->stats : NULL);

		result->unbuf->last_row_data = row_packet->fields;
		result->unbuf->last_row_buffer = row_packet->row_buffer;
		row_packet->fields = NULL;
		row_packet->row_buffer = NULL;

		MYSQLND_INC_CONN_STATISTIC(result->conn->stats, STAT_ROWS_FETCHED_FROM_CLIENT_NORMAL_UNBUF);

		if (!row_packet->skip_extraction) {
			unsigned int i, field_count = meta->field_count;

			enum_func_status rc = result->unbuf->m.row_decoder(result->unbuf->last_row_buffer,
											result->unbuf->last_row_data,
											field_count,
											row_packet->fields_metadata,
											result->conn->options->int_and_float_native,
											result->conn->stats);
			if (PASS != rc) {
				DBG_RETURN(FAIL);
			}
			{
				*row = mnd_malloc(field_count * sizeof(char *));
				if (*row) {
					MYSQLND_FIELD * field = meta->fields;
					zend_ulong * lengths = result->unbuf->lengths;

					for (i = 0; i < field_count; i++, field++) {
						zval * data = &result->unbuf->last_row_data[i];
						unsigned int len = (Z_TYPE_P(data) == IS_STRING)? Z_STRLEN_P(data) : 0;

/* BEGIN difference between normal normal fetch and _c */
						if (Z_TYPE_P(data) != IS_NULL) {
							convert_to_string(data);
							(*row)[i] = Z_STRVAL_P(data);
						} else {
							(*row)[i] = NULL;
						}
/* END difference between normal normal fetch and _c */

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
		}
		result->unbuf->row_count++;
		*fetched_anything = TRUE;
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
		result->unbuf->m.free_last_data(result->unbuf, result->conn? result->conn->stats : NULL);
	}

	DBG_INF_FMT("ret=%s fetched=%u", ret == PASS? "PASS":"FAIL", *fetched_anything);
	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ mysqlnd_result_unbuffered::fetch_row */
static enum_func_status
MYSQLND_METHOD(mysqlnd_result_unbuffered, fetch_row)(MYSQLND_RES * result, void * param, const unsigned int flags, zend_bool * fetched_anything)
{
	enum_func_status	ret;
	zval				*row = (zval *) param;
	MYSQLND_PACKET_ROW	*row_packet = result->unbuf->row_packet;
	const MYSQLND_RES_METADATA * const meta = result->meta;

	DBG_ENTER("mysqlnd_result_unbuffered::fetch_row");

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
		result->unbuf->m.free_last_data(result->unbuf, result->conn? result->conn->stats : NULL);

		result->unbuf->last_row_data = row_packet->fields;
		result->unbuf->last_row_buffer = row_packet->row_buffer;
		row_packet->fields = NULL;
		row_packet->row_buffer = NULL;

		MYSQLND_INC_CONN_STATISTIC(result->conn->stats, STAT_ROWS_FETCHED_FROM_CLIENT_NORMAL_UNBUF);

		if (!row_packet->skip_extraction) {
			unsigned int i, field_count = meta->field_count;

			enum_func_status rc = result->unbuf->m.row_decoder(result->unbuf->last_row_buffer,
											result->unbuf->last_row_data,
											field_count,
											row_packet->fields_metadata,
											result->conn->options->int_and_float_native,
											result->conn->stats);
			if (PASS != rc) {
				DBG_RETURN(FAIL);
			}
			{
				HashTable * row_ht = Z_ARRVAL_P(row);
				MYSQLND_FIELD * field = meta->fields;
				zend_ulong * lengths = result->unbuf->lengths;

				for (i = 0; i < field_count; i++, field++) {
					zval * data = &result->unbuf->last_row_data[i];
					unsigned int len = (Z_TYPE_P(data) == IS_STRING)? Z_STRLEN_P(data) : 0;

					if (flags & MYSQLND_FETCH_NUM) {
						Z_TRY_ADDREF_P(data);
						zend_hash_next_index_insert(row_ht, data);
					}
					if (flags & MYSQLND_FETCH_ASSOC) {
						/* zend_hash_quick_update needs length + trailing zero */
						/* QQ: Error handling ? */
						/*
						  zend_hash_quick_update does not check, as add_assoc_zval_ex do, whether
						  the index is a numeric and convert it to it. This however means constant
						  hashing of the column name, which is not needed as it can be precomputed.
						*/
						Z_TRY_ADDREF_P(data);
						if (meta->zend_hash_keys[i].is_numeric == FALSE) {
							zend_hash_update(Z_ARRVAL_P(row), meta->fields[i].sname, data);
						} else {
							zend_hash_index_update(Z_ARRVAL_P(row), meta->zend_hash_keys[i].key, data);
						}
					}

					if (lengths) {
						lengths[i] = len;
					}

					if (field->max_length < len) {
						field->max_length = len;
					}
				}
			}
		}
		result->unbuf->row_count++;
		*fetched_anything = TRUE;
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
		result->unbuf->m.free_last_data(result->unbuf, result->conn? result->conn->stats : NULL);
	}

	DBG_INF_FMT("ret=%s fetched=%u", ret == PASS? "PASS":"FAIL", *fetched_anything);
	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ mysqlnd_res::use_result */
static MYSQLND_RES *
MYSQLND_METHOD(mysqlnd_res, use_result)(MYSQLND_RES * const result, zend_bool ps)
{
	DBG_ENTER("mysqlnd_res::use_result");

	SET_EMPTY_ERROR(*result->conn->error_info);

	if (ps == FALSE) {
		result->type			= MYSQLND_RES_NORMAL;
	} else {
		result->type			= MYSQLND_RES_PS_UNBUF;
	}

	result->unbuf = mysqlnd_result_unbuffered_init(result->field_count, ps, result->persistent);
	if (!result->unbuf) {
		goto oom;
	}

	/*
	  Will be freed in the mysqlnd_internal_free_result_contents() called
	  by the resource destructor. mysqlnd_result_unbuffered::fetch_row() expects
	  this to be not NULL.
	*/
	/* FALSE = non-persistent */
	result->unbuf->row_packet = result->conn->protocol->m.get_row_packet(result->conn->protocol, FALSE);
	if (!result->unbuf->row_packet) {
		goto oom;
	}
	result->unbuf->row_packet->result_set_memory_pool = result->unbuf->result_set_memory_pool;
	result->unbuf->row_packet->field_count = result->field_count;
	result->unbuf->row_packet->binary_protocol = ps;
	result->unbuf->row_packet->fields_metadata = result->meta->fields;
	result->unbuf->row_packet->bit_fields_count = result->meta->bit_fields_count;
	result->unbuf->row_packet->bit_fields_total_len = result->meta->bit_fields_total_len;

	DBG_RETURN(result);
oom:
	SET_OOM_ERROR(*result->conn->error_info);
	DBG_RETURN(NULL);
}
/* }}} */


/* {{{ mysqlnd_result_buffered::fetch_row_c */
static enum_func_status
MYSQLND_METHOD(mysqlnd_result_buffered, fetch_row_c)(MYSQLND_RES * result, void * param, unsigned int flags, zend_bool * fetched_anything)
{
	MYSQLND_ROW_C * row = (MYSQLND_ROW_C *) param;
	const MYSQLND_RES_METADATA * const meta = result->meta;
	unsigned int field_count = meta->field_count;
	enum_func_status ret = FAIL;
	DBG_ENTER("mysqlnd_result_buffered::fetch_row_c");

	if (result->stored_data->type == MYSQLND_BUFFERED_TYPE_ZVAL) {
		MYSQLND_RES_BUFFERED_ZVAL * set = (MYSQLND_RES_BUFFERED_ZVAL *) result->stored_data;

		/* If we haven't read everything */
		if (set->data_cursor &&
			(set->data_cursor - set->data) < (result->stored_data->row_count * field_count))
		{
			zval *current_row = set->data_cursor;
			unsigned int i;

			if (Z_ISUNDEF(current_row[0])) {
				uint64_t row_num = (set->data_cursor - set->data) / field_count;
				enum_func_status rc = set->m.row_decoder(set->row_buffers[row_num],
												current_row,
												field_count,
												meta->fields,
												result->conn->options->int_and_float_native,
												result->conn->stats);
				if (rc != PASS) {
					DBG_RETURN(FAIL);
				}
				set->initialized_rows++;
				for (i = 0; i < field_count; i++) {
					/*
					  NULL fields are 0 length, 0 is not more than 0
					  String of zero size, definitely can't be the next max_length.
					  Thus for NULL and zero-length we are quite efficient.
					*/
					if (Z_TYPE(current_row[i]) == IS_STRING) {
						zend_ulong len = Z_STRLEN(current_row[i]);
						if (meta->fields[i].max_length < len) {
							meta->fields[i].max_length = len;
						}
					}
				}
			}

/* BEGIN difference between normal normal fetch and _c */
			/* there is no conn handle in this function thus we can't set OOM in error_info */
			*row = mnd_malloc(field_count * sizeof(char *));
			if (*row) {
				for (i = 0; i < field_count; i++) {
					zval * data = &current_row[i];

					set->lengths[i] = (Z_TYPE_P(data) == IS_STRING)? Z_STRLEN_P(data) : 0;

					if (Z_TYPE_P(data) != IS_NULL) {
						convert_to_string(data);
						(*row)[i] = Z_STRVAL_P(data);
					} else {
						(*row)[i] = NULL;
					}
				}
				set->data_cursor += field_count;
				MYSQLND_INC_GLOBAL_STATISTIC(STAT_ROWS_FETCHED_FROM_CLIENT_NORMAL_BUF);
			} else {
				SET_OOM_ERROR(*result->conn->error_info);
			}
/* END difference between normal normal fetch and _c */

			*fetched_anything = *row? TRUE:FALSE;
			ret = *row? PASS:FAIL;
		} else {
			set->data_cursor = NULL;
			DBG_INF("EOF reached");
			*fetched_anything = FALSE;
			ret = PASS;
		}
	} else if (result->stored_data->type == MYSQLND_BUFFERED_TYPE_C) {
		/*
			We don't support _C with pdo because it uses the data in a different way - just references it.
			We will either leak or give nirvana pointers
		*/
		*fetched_anything = FALSE;
		DBG_RETURN(FAIL);
	}
	DBG_INF_FMT("ret=PASS fetched=%u", *fetched_anything);
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_result_buffered_zval::fetch_row */
static enum_func_status
MYSQLND_METHOD(mysqlnd_result_buffered_zval, fetch_row)(MYSQLND_RES * result, void * param, const unsigned int flags, zend_bool * fetched_anything)
{
	zval * row = (zval *) param;
	const MYSQLND_RES_METADATA * const meta = result->meta;
	unsigned int field_count = meta->field_count;
	enum_func_status ret = FAIL;
	MYSQLND_RES_BUFFERED_ZVAL * set = (MYSQLND_RES_BUFFERED_ZVAL *) result->stored_data;

	DBG_ENTER("mysqlnd_result_buffered_zval::fetch_row");

	/* If we haven't read everything */
	if (set->data_cursor &&
		(set->data_cursor - set->data) < (set->row_count * field_count))
	{
		unsigned int i;
		zval *current_row = set->data_cursor;

		if (Z_ISUNDEF(current_row[0])) {
			uint64_t row_num = (set->data_cursor - set->data) / field_count;
			enum_func_status rc = set->m.row_decoder(set->row_buffers[row_num],
											current_row,
											field_count,
											meta->fields,
											result->conn->options->int_and_float_native,
											result->conn->stats);
			if (rc != PASS) {
				DBG_RETURN(FAIL);
			}
			set->initialized_rows++;
			for (i = 0; i < field_count; i++) {
				/*
				  NULL fields are 0 length, 0 is not more than 0
				  String of zero size, definitely can't be the next max_length.
				  Thus for NULL and zero-length we are quite efficient.
				*/
				if (Z_TYPE(current_row[i]) == IS_STRING) {
					zend_ulong len = Z_STRLEN(current_row[i]);
					if (meta->fields[i].max_length < len) {
						meta->fields[i].max_length = len;
					}
				}
			}
		}

		for (i = 0; i < field_count; i++) {
			zval * data = &current_row[i];

			set->lengths[i] = (Z_TYPE_P(data) == IS_STRING)? Z_STRLEN_P(data) : 0;

			if (flags & MYSQLND_FETCH_NUM) {
				Z_TRY_ADDREF_P(data);
				zend_hash_next_index_insert(Z_ARRVAL_P(row), data);
			}
			if (flags & MYSQLND_FETCH_ASSOC) {
				/* zend_hash_quick_update needs length + trailing zero */
				/* QQ: Error handling ? */
				/*
				  zend_hash_quick_update does not check, as add_assoc_zval_ex do, whether
				  the index is a numeric and convert it to it. This however means constant
				  hashing of the column name, which is not needed as it can be precomputed.
				*/
				Z_TRY_ADDREF_P(data);
				if (meta->zend_hash_keys[i].is_numeric == FALSE) {
					zend_hash_update(Z_ARRVAL_P(row), meta->fields[i].sname, data);
				} else {
					zend_hash_index_update(Z_ARRVAL_P(row), meta->zend_hash_keys[i].key, data);
				}
			}
		}
		set->data_cursor += field_count;
		MYSQLND_INC_GLOBAL_STATISTIC(STAT_ROWS_FETCHED_FROM_CLIENT_NORMAL_BUF);
		*fetched_anything = TRUE;
		ret = PASS;
	} else {
		set->data_cursor = NULL;
		DBG_INF("EOF reached");
		*fetched_anything = FALSE;
		ret = PASS;
	}
	DBG_INF_FMT("ret=PASS fetched=%u", *fetched_anything);
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_result_buffered_c::fetch_row */
static enum_func_status
MYSQLND_METHOD(mysqlnd_result_buffered_c, fetch_row)(MYSQLND_RES * result, void * param, const unsigned int flags, zend_bool * fetched_anything)
{
	zval * row = (zval *) param;
	const MYSQLND_RES_METADATA * const meta = result->meta;
	unsigned int field_count = meta->field_count;
	enum_func_status ret = FAIL;

	MYSQLND_RES_BUFFERED_C * set = (MYSQLND_RES_BUFFERED_C *) result->stored_data;

	DBG_ENTER("mysqlnd_result_buffered_c::fetch_row");

	/* If we haven't read everything */
	if (set->current_row < set->row_count) {
		zval *current_row;
		enum_func_status rc;
		unsigned int i;

		current_row = mnd_emalloc(field_count * sizeof(zval));
		if (!current_row) {
			SET_OOM_ERROR(*result->conn->error_info);
			DBG_RETURN(FAIL);
		}

		rc = result->stored_data->m.row_decoder(result->stored_data->row_buffers[set->current_row],
										current_row,
										field_count,
										meta->fields,
										result->conn->options->int_and_float_native,
										result->conn->stats);
		if (rc != PASS) {
			DBG_RETURN(FAIL);
		}
		if (!(set->initialized[set->current_row >> 3] & (1 << (set->current_row & 7)))) {
			set->initialized[set->current_row >> 3] |= (1 << (set->current_row & 7)); /* mark initialized */

			set->initialized_rows++;

			for (i = 0; i < field_count; i++) {
				/*
				  NULL fields are 0 length, 0 is not more than 0
				  String of zero size, definitely can't be the next max_length.
				  Thus for NULL and zero-length we are quite efficient.
				*/
				if (Z_TYPE(current_row[i]) == IS_STRING) {
					zend_ulong len = Z_STRLEN(current_row[i]);
					if (meta->fields[i].max_length < len) {
						meta->fields[i].max_length = len;
					}
				}
			}
		}

		for (i = 0; i < field_count; i++) {
			zval * data = &current_row[i];

			set->lengths[i] = (Z_TYPE_P(data) == IS_STRING)? Z_STRLEN_P(data) : 0;

			if (flags & MYSQLND_FETCH_NUM) {
				Z_TRY_ADDREF_P(data);
				zend_hash_next_index_insert(Z_ARRVAL_P(row), data);
			}
			if (flags & MYSQLND_FETCH_ASSOC) {
				/* zend_hash_quick_update needs length + trailing zero */
				/* QQ: Error handling ? */
				/*
				  zend_hash_quick_update does not check, as add_assoc_zval_ex do, whether
				  the index is a numeric and convert it to it. This however means constant
				  hashing of the column name, which is not needed as it can be precomputed.
				*/
				Z_TRY_ADDREF_P(data);
				if (meta->zend_hash_keys[i].is_numeric == FALSE) {
					zend_hash_update(Z_ARRVAL_P(row), meta->fields[i].sname, data);
				} else {
					zend_hash_index_update(Z_ARRVAL_P(row), meta->zend_hash_keys[i].key, data);
				}
			}
			/*
				This will usually not destroy anything but decref.
				However, if neither NUM nor ASSOC is set we will free memory cleanly and won't leak.
				It also simplifies the handling of Z_ADDREF_P because we don't need to check if only
				either NUM or ASSOC is set but not both.
			*/
			zval_ptr_dtor(data);
		}
		mnd_efree(current_row);
		set->current_row++;
		MYSQLND_INC_GLOBAL_STATISTIC(STAT_ROWS_FETCHED_FROM_CLIENT_NORMAL_BUF);
		*fetched_anything = TRUE;
		ret = PASS;
	} else {
		if (set->current_row == set->row_count) {
			set->current_row = set->row_count + 1;
		}
		DBG_INF_FMT("EOF reached. current_row=%llu", (unsigned long long) set->current_row);
		*fetched_anything = FALSE;
		ret = PASS;
	}

	DBG_INF_FMT("ret=PASS fetched=%u", *fetched_anything);
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_res::fetch_row */
static enum_func_status
MYSQLND_METHOD(mysqlnd_res, fetch_row)(MYSQLND_RES * result, void * param, const unsigned int flags, zend_bool *fetched_anything)
{
	const mysqlnd_fetch_row_func f = result->stored_data? result->stored_data->m.fetch_row:(result->unbuf? result->unbuf->m.fetch_row:NULL);
	if (f) {
		return f(result, param, flags, fetched_anything);
	}
	*fetched_anything = FALSE;
	return PASS;
}
/* }}} */


#define STORE_RESULT_PREALLOCATED_SET_IF_NOT_EMPTY 2

/* {{{ mysqlnd_res::store_result_fetch_data */
enum_func_status
MYSQLND_METHOD(mysqlnd_res, store_result_fetch_data)(MYSQLND_CONN_DATA * const conn, MYSQLND_RES * result,
													MYSQLND_RES_METADATA * meta,
													MYSQLND_MEMORY_POOL_CHUNK ***row_buffers,
													zend_bool binary_protocol)
{
	enum_func_status ret;
	MYSQLND_PACKET_ROW * row_packet = NULL;
	unsigned int next_extend = STORE_RESULT_PREALLOCATED_SET_IF_NOT_EMPTY, free_rows = 1;
	MYSQLND_RES_BUFFERED *set;

	DBG_ENTER("mysqlnd_res::store_result_fetch_data");

	set = result->stored_data;

	if (!set || !row_buffers) {
		ret = FAIL;
		goto end;
	}
	if (free_rows) {
		*row_buffers = mnd_pemalloc((size_t)(free_rows * sizeof(MYSQLND_MEMORY_POOL_CHUNK *)), 0);
		if (!*row_buffers) {
			SET_OOM_ERROR(*conn->error_info);
			ret = FAIL;
			goto end;
		}
	}
	set->references	= 1;

	/* non-persistent */
	row_packet = conn->protocol->m.get_row_packet(conn->protocol, FALSE);
	if (!row_packet) {
		SET_OOM_ERROR(*conn->error_info);
		ret = FAIL;
		goto end;
	}
	row_packet->result_set_memory_pool = result->stored_data->result_set_memory_pool;
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
			new_row_buffers = mnd_perealloc(*row_buffers, (size_t)(total_allocated_rows * sizeof(MYSQLND_MEMORY_POOL_CHUNK *)), 0);
			if (!new_row_buffers) {
				SET_OOM_ERROR(*conn->error_info);
				ret = FAIL;
				goto end;
			}
			*row_buffers = new_row_buffers;
		}
		free_rows--;
		(*row_buffers)[set->row_count] = row_packet->row_buffer;

		set->row_count++;

		/* So row_packet's destructor function won't efree() it */
		row_packet->fields = NULL;
		row_packet->row_buffer = NULL;

		/*
		  No need to FREE_ALLOCA as we can reuse the
		  'lengths' and 'fields' arrays. For lengths its absolutely safe.
		  'fields' is reused because the ownership of the strings has been
		  transferred above.
		*/
	}
	/* Overflow ? */
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
		*row_buffers = mnd_perealloc(*row_buffers, (size_t) (set->row_count * sizeof(MYSQLND_MEMORY_POOL_CHUNK *)), 0);
	}

	if (conn->upsert_status->server_status & SERVER_MORE_RESULTS_EXISTS) {
		CONN_SET_STATE(conn, CONN_NEXT_RESULT_PENDING);
	} else {
		CONN_SET_STATE(conn, CONN_READY);
	}

	if (ret == FAIL) {
		COPY_CLIENT_ERROR(set->error_info, row_packet->error_info);
	} else {
		/* libmysql's documentation says it should be so for SELECT statements */
		conn->upsert_status->affected_rows = set->row_count;
	}
	DBG_INF_FMT("ret=%s row_count=%u warnings=%u server_status=%u",
				ret == PASS? "PASS":"FAIL", (uint) set->row_count, conn->upsert_status->warning_count, conn->upsert_status->server_status);
end:
	PACKET_FREE(row_packet);
	DBG_INF_FMT("rows=%llu", (unsigned long long)result->stored_data->row_count);
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_res::store_result */
static MYSQLND_RES *
MYSQLND_METHOD(mysqlnd_res, store_result)(MYSQLND_RES * result,
										  MYSQLND_CONN_DATA * const conn,
										  const unsigned int flags)
{
	enum_func_status ret;
	MYSQLND_MEMORY_POOL_CHUNK ***row_buffers = NULL;

	DBG_ENTER("mysqlnd_res::store_result");

	/* We need the conn because we are doing lazy zval initialization in buffered_fetch_row */
	/* In case of error the reference will be released in free_result_internal() called indirectly by our caller */
	result->conn = conn->m->get_reference(conn);
	result->type = MYSQLND_RES_NORMAL;

	CONN_SET_STATE(conn, CONN_FETCHING_DATA);

	if (flags & MYSQLND_STORE_NO_COPY) {
		result->stored_data	= (MYSQLND_RES_BUFFERED *) mysqlnd_result_buffered_zval_init(result->field_count, flags & MYSQLND_STORE_PS, result->persistent);
		if (!result->stored_data) {
			SET_OOM_ERROR(*conn->error_info);
			DBG_RETURN(NULL);
		}
		row_buffers = &result->stored_data->row_buffers;
	} else if (flags & MYSQLND_STORE_COPY) {
		result->stored_data	= (MYSQLND_RES_BUFFERED *) mysqlnd_result_buffered_c_init(result->field_count, flags & MYSQLND_STORE_PS, result->persistent);
		if (!result->stored_data) {
			SET_OOM_ERROR(*conn->error_info);
			DBG_RETURN(NULL);
		}
		row_buffers = &result->stored_data->row_buffers;
	}
	ret = result->m.store_result_fetch_data(conn, result, result->meta, row_buffers, flags & MYSQLND_STORE_PS);

	if (FAIL == ret) {
		if (result->stored_data) {
			COPY_CLIENT_ERROR(*conn->error_info, result->stored_data->error_info);
		} else {
			SET_OOM_ERROR(*conn->error_info);
		}
		DBG_RETURN(NULL);
	} else {
	/* Overflow ? */
		if (flags & MYSQLND_STORE_NO_COPY) {
			MYSQLND_RES_METADATA * meta = result->meta;
			MYSQLND_RES_BUFFERED_ZVAL * set = (MYSQLND_RES_BUFFERED_ZVAL *) result->stored_data;
			if (set->row_count) {
				/* don't try to allocate more than possible - mnd_XXalloc expects size_t, and it can have narrower range than uint64_t */
				if (set->row_count * meta->field_count * sizeof(zval *) > SIZE_MAX) {
					SET_OOM_ERROR(*conn->error_info);
					DBG_RETURN(NULL);
				}
				/* if pecalloc is used valgrind barks gcc version 4.3.1 20080507 (prerelease) [gcc-4_3-branch revision 135036] (SUSE Linux) */
				set->data = mnd_emalloc((size_t)(set->row_count * meta->field_count * sizeof(zval)));
				if (!set->data) {
					SET_OOM_ERROR(*conn->error_info);
					DBG_RETURN(NULL);
				}
				memset(set->data, 0, (size_t)(set->row_count * meta->field_count * sizeof(zval)));
			}
			/* Position at the first row */
			set->data_cursor = set->data;
		} else if (flags & MYSQLND_STORE_COPY) {
			MYSQLND_RES_BUFFERED_C * set = (MYSQLND_RES_BUFFERED_C *) result->stored_data;
			set->current_row = 0;
			set->initialized = mnd_pecalloc((set->row_count / 8) + 1, sizeof(zend_uchar), set->persistent); /* +1 for safety */
		}
	}

	/* libmysql's documentation says it should be so for SELECT statements */
	conn->upsert_status->affected_rows = result->stored_data->row_count;

	DBG_RETURN(result);
}
/* }}} */


/* {{{ mysqlnd_res::skip_result */
static enum_func_status
MYSQLND_METHOD(mysqlnd_res, skip_result)(MYSQLND_RES * const result)
{
	zend_bool fetched_anything;

	DBG_ENTER("mysqlnd_res::skip_result");
	/*
	  Unbuffered sets
	  A PS could be prepared - there is metadata and thus a stmt->result but the
	  fetch_row function isn't actually set (NULL), thus we have to skip these.
	*/
	if (result->unbuf && !result->unbuf->eof_reached) {
		DBG_INF("skipping result");
		/* We have to fetch all data to clean the line */
		MYSQLND_INC_CONN_STATISTIC(result->conn->stats,
									result->type == MYSQLND_RES_NORMAL? STAT_FLUSHED_NORMAL_SETS:
																		STAT_FLUSHED_PS_SETS);

		while ((PASS == result->m.fetch_row(result, NULL, 0, &fetched_anything)) && fetched_anything == TRUE) {
			/* do nothing */;
		}
	}
	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ mysqlnd_res::free_result */
static enum_func_status
MYSQLND_METHOD(mysqlnd_res, free_result)(MYSQLND_RES * result, zend_bool implicit)
{
	DBG_ENTER("mysqlnd_res::free_result");

	MYSQLND_INC_CONN_STATISTIC(result->conn? result->conn->stats : NULL,
							   implicit == TRUE?	STAT_FREE_RESULT_IMPLICIT:
							   						STAT_FREE_RESULT_EXPLICIT);

	result->m.free_result_internal(result);
	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ mysqlnd_res::data_seek */
static enum_func_status
MYSQLND_METHOD(mysqlnd_res, data_seek)(MYSQLND_RES * const result, const uint64_t row)
{
	DBG_ENTER("mysqlnd_res::data_seek");
	DBG_INF_FMT("row=%lu", row);

	DBG_RETURN(result->stored_data? result->stored_data->m.data_seek(result->stored_data, row) : FAIL);
}
/* }}} */


/* {{{ mysqlnd_result_buffered_zval::data_seek */
static enum_func_status
MYSQLND_METHOD(mysqlnd_result_buffered_zval, data_seek)(MYSQLND_RES_BUFFERED * const result, const uint64_t row)
{
	MYSQLND_RES_BUFFERED_ZVAL * set = (MYSQLND_RES_BUFFERED_ZVAL *) result;
	DBG_ENTER("mysqlnd_result_buffered_zval::data_seek");

	/* libmysql just moves to the end, it does traversing of a linked list */
	if (row >= set->row_count) {
		set->data_cursor = NULL;
	} else {
		set->data_cursor = set->data + row * result->field_count;
	}
	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ mysqlnd_result_buffered_c::data_seek */
static enum_func_status
MYSQLND_METHOD(mysqlnd_result_buffered_c, data_seek)(MYSQLND_RES_BUFFERED * const result, const uint64_t row)
{
	MYSQLND_RES_BUFFERED_C * set = (MYSQLND_RES_BUFFERED_C *) result;
	DBG_ENTER("mysqlnd_result_buffered_c::data_seek");

	/* libmysql just moves to the end, it does traversing of a linked list */
	if (row >= set->row_count) {
		set->current_row = set->row_count;
	} else {
		set->current_row = row;
	}
	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ mysqlnd_result_unbuffered::num_rows */
static uint64_t
MYSQLND_METHOD(mysqlnd_result_unbuffered, num_rows)(const MYSQLND_RES_UNBUFFERED * const result)
{
	/* Be compatible with libmysql. We count row_count, but will return 0 */
	return result->eof_reached? result->row_count:0;
}
/* }}} */


/* {{{ mysqlnd_result_buffered::num_rows */
static uint64_t
MYSQLND_METHOD(mysqlnd_result_buffered, num_rows)(const MYSQLND_RES_BUFFERED * const result)
{
	return result->row_count;
}
/* }}} */


/* {{{ mysqlnd_res::num_rows */
static uint64_t
MYSQLND_METHOD(mysqlnd_res, num_rows)(const MYSQLND_RES * const result)
{
	return result->stored_data?
			result->stored_data->m.num_rows(result->stored_data) :
			(result->unbuf? result->unbuf->m.num_rows(result->unbuf) : 0);
}
/* }}} */


/* {{{ mysqlnd_res::num_fields */
static unsigned int
MYSQLND_METHOD(mysqlnd_res, num_fields)(const MYSQLND_RES * const result)
{
	return result->field_count;
}
/* }}} */


/* {{{ mysqlnd_res::fetch_field */
static const MYSQLND_FIELD *
MYSQLND_METHOD(mysqlnd_res, fetch_field)(MYSQLND_RES * const result)
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
				if (PASS != result->stored_data->m.initialize_result_set_rest(result->stored_data, result->meta, result->conn->stats,
																			  result->conn->options->int_and_float_native))
				{
					break;
				}
			}
			DBG_RETURN(result->meta->m->fetch_field(result->meta));
		}
	} while (0);
	DBG_RETURN(NULL);
}
/* }}} */


/* {{{ mysqlnd_res::fetch_field_direct */
static const MYSQLND_FIELD *
MYSQLND_METHOD(mysqlnd_res, fetch_field_direct)(MYSQLND_RES * const result, const MYSQLND_FIELD_OFFSET fieldnr)
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
				if (PASS != result->stored_data->m.initialize_result_set_rest(result->stored_data, result->meta, result->conn->stats,
																			  result->conn->options->int_and_float_native))
				{
					break;
				}
			}
			DBG_RETURN(result->meta->m->fetch_field_direct(result->meta, fieldnr));
		}
	} while (0);

	DBG_RETURN(NULL);
}
/* }}} */


/* {{{ mysqlnd_res::fetch_field */
static const MYSQLND_FIELD *
MYSQLND_METHOD(mysqlnd_res, fetch_fields)(MYSQLND_RES * const result)
{
	DBG_ENTER("mysqlnd_res::fetch_fields");
	do {
		if (result->meta) {
			if (result->stored_data && (result->stored_data->initialized_rows < result->stored_data->row_count)) {
				/* we have to initialize the rest to get the updated max length */
				if (PASS != result->stored_data->m.initialize_result_set_rest(result->stored_data, result->meta, result->conn->stats,
																			  result->conn->options->int_and_float_native))
				{
					break;
				}
			}
			DBG_RETURN(result->meta->m->fetch_fields(result->meta));
		}
	} while (0);
	DBG_RETURN(NULL);
}
/* }}} */


/* {{{ mysqlnd_res::field_seek */
static MYSQLND_FIELD_OFFSET
MYSQLND_METHOD(mysqlnd_res, field_seek)(MYSQLND_RES * const result, const MYSQLND_FIELD_OFFSET field_offset)
{
	return result->meta? result->meta->m->field_seek(result->meta, field_offset) : 0;
}
/* }}} */


/* {{{ mysqlnd_res::field_tell */
static MYSQLND_FIELD_OFFSET
MYSQLND_METHOD(mysqlnd_res, field_tell)(const MYSQLND_RES * const result)
{
	return result->meta? result->meta->m->field_tell(result->meta) : 0;
}
/* }}} */


/* {{{ mysqlnd_res::fetch_into */
static void
MYSQLND_METHOD(mysqlnd_res, fetch_into)(MYSQLND_RES * result, const unsigned int flags,
										zval *return_value,
										enum_mysqlnd_extension extension ZEND_FILE_LINE_DC)
{
	zend_bool fetched_anything;

	DBG_ENTER("mysqlnd_res::fetch_into");

	/*
	  Hint Zend how many elements we will have in the hash. Thus it won't
	  extend and rehash the hash constantly.
	*/
	array_init_size(return_value, mysqlnd_num_fields(result) * 2);
	if (FAIL == result->m.fetch_row(result, (void *)return_value, flags, &fetched_anything)) {
		php_error_docref(NULL, E_WARNING, "Error while reading a row");
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
MYSQLND_METHOD(mysqlnd_res, fetch_row_c)(MYSQLND_RES * result)
{
	zend_bool fetched_anything;
	MYSQLND_ROW_C ret = NULL;
	DBG_ENTER("mysqlnd_res::fetch_row_c");

	if (result->stored_data && result->stored_data->m.fetch_row == MYSQLND_METHOD(mysqlnd_result_buffered_zval, fetch_row)) {
		MYSQLND_METHOD(mysqlnd_result_buffered, fetch_row_c)(result, (void *) &ret, 0, &fetched_anything);
	} else if (result->unbuf && result->unbuf->m.fetch_row == MYSQLND_METHOD(mysqlnd_result_unbuffered, fetch_row)) {
		MYSQLND_METHOD(mysqlnd_result_unbuffered, fetch_row_c)(result, (void *) &ret, 0, &fetched_anything);
	} else {
		ret = NULL;
		php_error_docref(NULL, E_ERROR, "result->m.fetch_row has invalid value. Report to the developers");
	}
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_res::fetch_all */
static void
MYSQLND_METHOD(mysqlnd_res, fetch_all)(MYSQLND_RES * result, const unsigned int flags, zval *return_value ZEND_FILE_LINE_DC)
{
	zval  row;
	zend_ulong i = 0;
	MYSQLND_RES_BUFFERED *set = result->stored_data;

	DBG_ENTER("mysqlnd_res::fetch_all");

	if ((!result->unbuf && !set)) {
		php_error_docref(NULL, E_WARNING, "fetch_all can be used only with buffered sets");
		if (result->conn) {
			SET_CLIENT_ERROR(*result->conn->error_info, CR_NOT_IMPLEMENTED, UNKNOWN_SQLSTATE, "fetch_all can be used only with buffered sets");
		}
		RETVAL_NULL();
		DBG_VOID_RETURN;
	}

	/* 4 is a magic value. The cast is safe, if larger then the array will be later extended - no big deal :) */
	array_init_size(return_value, set? (unsigned int) set->row_count : 4);

	do {
		mysqlnd_fetch_into(result, flags, &row, MYSQLND_MYSQLI);
		if (Z_TYPE(row) != IS_ARRAY) {
			zval_ptr_dtor(&row);
			break;
		}
		add_index_zval(return_value, i++, &row);
	} while (1);

	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_res::fetch_field_data */
static void
MYSQLND_METHOD(mysqlnd_res, fetch_field_data)(MYSQLND_RES * result, unsigned int offset, zval *return_value)
{
	zval row;
	zval *entry;
	unsigned int i = 0;

	DBG_ENTER("mysqlnd_res::fetch_field_data");
	DBG_INF_FMT("offset=%u", offset);
	/*
	  Hint Zend how many elements we will have in the hash. Thus it won't
	  extend and rehash the hash constantly.
	*/
	mysqlnd_fetch_into(result, MYSQLND_FETCH_NUM, &row, MYSQLND_MYSQL);
	if (Z_TYPE(row) != IS_ARRAY) {
		zval_dtor(&row);
		RETVAL_NULL();
		DBG_VOID_RETURN;
	}

	zend_hash_internal_pointer_reset(Z_ARRVAL(row));
	while (i++ < offset) {
		zend_hash_move_forward(Z_ARRVAL(row));
	}

	entry = zend_hash_get_current_data(Z_ARRVAL(row));

	ZVAL_COPY(return_value, entry);
	zval_dtor(&row);

	DBG_VOID_RETURN;
}
/* }}} */


MYSQLND_CLASS_METHODS_START(mysqlnd_res)
	MYSQLND_METHOD(mysqlnd_res, fetch_row),
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
	MYSQLND_METHOD(mysqlnd_res, fetch_lengths),
	MYSQLND_METHOD(mysqlnd_res, store_result_fetch_data),
	MYSQLND_METHOD(mysqlnd_res, free_result_buffers),
	MYSQLND_METHOD(mysqlnd_res, free_result),
	MYSQLND_METHOD(mysqlnd_res, free_result_internal),
	MYSQLND_METHOD(mysqlnd_res, free_result_contents_internal),
	mysqlnd_result_meta_init
MYSQLND_CLASS_METHODS_END;


MYSQLND_CLASS_METHODS_START(mysqlnd_result_unbuffered)
	MYSQLND_METHOD(mysqlnd_result_unbuffered, fetch_row),
	NULL, /* row_decoder */
	MYSQLND_METHOD(mysqlnd_result_unbuffered, num_rows),
	MYSQLND_METHOD(mysqlnd_result_unbuffered, fetch_lengths),
	MYSQLND_METHOD(mysqlnd_result_unbuffered, free_last_data),
	MYSQLND_METHOD(mysqlnd_result_unbuffered, free_result)
MYSQLND_CLASS_METHODS_END;


MYSQLND_CLASS_METHODS_START(mysqlnd_result_buffered)
	NULL, /* fetch_row   */
	NULL, /* row_decoder */
	MYSQLND_METHOD(mysqlnd_result_buffered, num_rows),
	NULL, /* fetch_lengths */
	NULL, /* data_seek */
	NULL, /* initialize_result_set_rest */
	MYSQLND_METHOD(mysqlnd_result_buffered, free_result)
MYSQLND_CLASS_METHODS_END;


/* {{{ mysqlnd_result_init */
PHPAPI MYSQLND_RES *
mysqlnd_result_init(unsigned int field_count, zend_bool persistent)
{
	size_t alloc_size = sizeof(MYSQLND_RES) + mysqlnd_plugin_count() * sizeof(void *);
	MYSQLND_RES * ret = mnd_pecalloc(1, alloc_size, persistent);

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


/* {{{ mysqlnd_result_unbuffered_init */
PHPAPI MYSQLND_RES_UNBUFFERED *
mysqlnd_result_unbuffered_init(unsigned int field_count, zend_bool ps, zend_bool persistent)
{
	size_t alloc_size = sizeof(MYSQLND_RES_UNBUFFERED) + mysqlnd_plugin_count() * sizeof(void *);
	MYSQLND_RES_UNBUFFERED * ret = mnd_pecalloc(1, alloc_size, persistent);

	DBG_ENTER("mysqlnd_result_unbuffered_init");

	if (!ret) {
		DBG_RETURN(NULL);
	}

	if (!(ret->lengths = mnd_pecalloc(field_count, sizeof(zend_ulong), persistent))) {
		mnd_pefree(ret, persistent);
		DBG_RETURN(NULL);
	}
	if (!(ret->result_set_memory_pool = mysqlnd_mempool_create(MYSQLND_G(mempool_default_size)))) {
		mnd_efree(ret->lengths);
		mnd_pefree(ret, persistent);
		DBG_RETURN(NULL);
	}

	ret->persistent	= persistent;
	ret->field_count= field_count;
	ret->ps = ps;

	ret->m = *mysqlnd_result_unbuffered_get_methods();

	if (ps) {
		ret->m.fetch_lengths = NULL; /* makes no sense */
		ret->m.row_decoder	= php_mysqlnd_rowp_read_binary_protocol;
	} else {
		ret->m.row_decoder	= php_mysqlnd_rowp_read_text_protocol_zval;
	}

	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_result_buffered_zval_init */
PHPAPI MYSQLND_RES_BUFFERED_ZVAL *
mysqlnd_result_buffered_zval_init(unsigned int field_count, zend_bool ps, zend_bool persistent)
{
	size_t alloc_size = sizeof(MYSQLND_RES_BUFFERED_ZVAL) + mysqlnd_plugin_count() * sizeof(void *);
	MYSQLND_RES_BUFFERED_ZVAL * ret = mnd_pecalloc(1, alloc_size, persistent);

	DBG_ENTER("mysqlnd_result_buffered_zval_init");

	if (!ret) {
		DBG_RETURN(NULL);
	}
	if (!(ret->lengths = mnd_pecalloc(field_count, sizeof(zend_ulong), persistent))) {
		mnd_pefree(ret, persistent);
		DBG_RETURN(NULL);
	}
	if (!(ret->result_set_memory_pool = mysqlnd_mempool_create(MYSQLND_G(mempool_default_size)))) {
		mnd_efree(ret->lengths);
		mnd_pefree(ret, persistent);
		DBG_RETURN(NULL);
	}

	ret->persistent	= persistent;
	ret->field_count= field_count;
	ret->ps = ps;
	ret->m = *mysqlnd_result_buffered_get_methods();
	ret->type = MYSQLND_BUFFERED_TYPE_ZVAL;

	if (ps) {
		ret->m.fetch_lengths = NULL; /* makes no sense */
		ret->m.row_decoder	= php_mysqlnd_rowp_read_binary_protocol;
	} else {
		ret->m.row_decoder	= php_mysqlnd_rowp_read_text_protocol_zval;
	}
	ret->m.fetch_row		= MYSQLND_METHOD(mysqlnd_result_buffered_zval, fetch_row);
	ret->m.fetch_lengths 	= MYSQLND_METHOD(mysqlnd_result_buffered_zval, fetch_lengths);
	ret->m.data_seek		= MYSQLND_METHOD(mysqlnd_result_buffered_zval, data_seek);
	ret->m.initialize_result_set_rest = MYSQLND_METHOD(mysqlnd_result_buffered_zval, initialize_result_set_rest);
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_result_buffered_c_init */
PHPAPI MYSQLND_RES_BUFFERED_C *
mysqlnd_result_buffered_c_init(unsigned int field_count, zend_bool ps, zend_bool persistent)
{
	size_t alloc_size = sizeof(MYSQLND_RES_BUFFERED_C) + mysqlnd_plugin_count() * sizeof(void *);
	MYSQLND_RES_BUFFERED_C * ret = mnd_pecalloc(1, alloc_size, persistent);

	DBG_ENTER("mysqlnd_result_buffered_c_init");

	if (!ret) {
		DBG_RETURN(NULL);
	}
	if (!(ret->lengths = mnd_pecalloc(field_count, sizeof(zend_ulong), persistent))) {
		mnd_pefree(ret, persistent);
		DBG_RETURN(NULL);
	}
	if (!(ret->result_set_memory_pool = mysqlnd_mempool_create(MYSQLND_G(mempool_default_size)))) {
		mnd_efree(ret->lengths);
		mnd_pefree(ret, persistent);
		DBG_RETURN(NULL);
	}

	ret->persistent	= persistent;
	ret->field_count= field_count;
	ret->ps = ps;
	ret->m = *mysqlnd_result_buffered_get_methods();
	ret->type = MYSQLND_BUFFERED_TYPE_C;

	if (ps) {
		ret->m.fetch_lengths = NULL; /* makes no sense */
		ret->m.row_decoder	= php_mysqlnd_rowp_read_binary_protocol;
	} else {
		ret->m.row_decoder	= php_mysqlnd_rowp_read_text_protocol_c;
	}
	ret->m.fetch_row		= MYSQLND_METHOD(mysqlnd_result_buffered_c, fetch_row);
	ret->m.fetch_lengths 	= MYSQLND_METHOD(mysqlnd_result_buffered_c, fetch_lengths);
	ret->m.data_seek		= MYSQLND_METHOD(mysqlnd_result_buffered_c, data_seek);
	ret->m.initialize_result_set_rest = MYSQLND_METHOD(mysqlnd_result_buffered_c, initialize_result_set_rest);

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
