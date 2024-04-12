/*
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | https://www.php.net/license/3_01.txt                                 |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Andrey Hristov <andrey@php.net>                             |
  |          Ulf Wendel <uw@php.net>                                     |
  +----------------------------------------------------------------------+
*/

#include "php.h"
#include "mysqlnd.h"
#include "mysqlnd_wireprotocol.h"
#include "mysqlnd_block_alloc.h"
#include "mysqlnd_connection.h"
#include "mysqlnd_priv.h"
#include "mysqlnd_result.h"
#include "mysqlnd_result_meta.h"
#include "mysqlnd_statistics.h"
#include "mysqlnd_debug.h"
#include "mysqlnd_ext_plugin.h"

/* {{{ mysqlnd_result_unbuffered::free_result */
static void
MYSQLND_METHOD(mysqlnd_result_unbuffered, free_result)(MYSQLND_RES_UNBUFFERED * const result, MYSQLND_STATS * const global_stats)
{
	DBG_ENTER("mysqlnd_result_unbuffered, free_result");

	/* must be free before because references the memory pool */
	if (result->row_packet) {
		PACKET_FREE(result->row_packet);
		mnd_efree(result->row_packet);
		result->row_packet = NULL;
	}

	DBG_VOID_RETURN;
}
/* }}} */

static void mysqlnd_result_free_prev_data(MYSQLND_RES *result)
{
	if (result->free_row_data) {
		for (unsigned i = 0; i < result->field_count; ++i) {
			zval_ptr_dtor_nogc(&result->row_data[i]);
		}
		result->free_row_data = 0;
	}
}

/* {{{ mysqlnd_result_buffered::free_result */
static void
MYSQLND_METHOD(mysqlnd_result_buffered, free_result)(MYSQLND_RES_BUFFERED * const set)
{

	DBG_ENTER("mysqlnd_result_buffered::free_result");
	DBG_INF_FMT("Freeing %" PRIu64 " row(s)", set->row_count);

	mysqlnd_error_info_free_contents(&set->error_info);

	if (set->row_buffers) {
		mnd_efree(set->row_buffers);
		set->row_buffers = NULL;
	}

	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_res::free_result_buffers */
static void
MYSQLND_METHOD(mysqlnd_res, free_result_buffers)(MYSQLND_RES * result)
{
	DBG_ENTER("mysqlnd_res::free_result_buffers");
	DBG_INF_FMT("%s", result->unbuf? "unbuffered":(result->stored_data? "buffered":"unknown"));

	mysqlnd_result_free_prev_data(result);

	if (result->meta) {
		ZEND_ASSERT(zend_arena_contains(result->memory_pool->arena, result->meta));
		result->meta->m->free_metadata(result->meta);
		result->meta = NULL;
	}

	if (result->unbuf) {
		result->unbuf->m.free_result(result->unbuf, result->conn? result->conn->stats : NULL);
		result->unbuf = NULL;
	} else if (result->stored_data) {
		result->stored_data->m.free_result(result->stored_data);
		result->stored_data = NULL;
	}

	mysqlnd_mempool_restore_state(result->memory_pool);
	mysqlnd_mempool_save_state(result->memory_pool);

	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_res::free_result_contents_internal */
static
void MYSQLND_METHOD(mysqlnd_res, free_result_contents_internal)(MYSQLND_RES * result)
{
	DBG_ENTER("mysqlnd_res::free_result_contents_internal");

	result->m.free_result_buffers(result);

	if (result->conn) {
		result->conn->m->free_reference(result->conn);
		result->conn = NULL;
	}

	mysqlnd_mempool_destroy(result->memory_pool);

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

	result->meta = result->m.result_meta_init(result, result->field_count);

	/* 1. Read all fields metadata */

	/* It's safe to reread without freeing */
	if (FAIL == result->meta->m->read_metadata(result->meta, conn, result)) {
		result->meta->m->free_metadata(result->meta);
		result->meta = NULL;
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
	enum_func_status ret;
	MYSQLND_STMT_DATA * stmt = s ? s->data : NULL;
	MYSQLND_PACKET_RSET_HEADER rset_header;
	MYSQLND_PACKET_EOF fields_eof;

	DBG_ENTER("mysqlnd_query_read_result_set_header");
	DBG_INF_FMT("stmt=" ZEND_ULONG_FMT, stmt? stmt->stmt_id:0);

	ret = FAIL;
	do {
		conn->payload_decoder_factory->m.init_rset_header_packet(&rset_header);
		UPSERT_STATUS_SET_AFFECTED_ROWS_TO_ERROR(conn->upsert_status);

		if (FAIL == (ret = PACKET_READ(conn, &rset_header))) {
			if (conn->error_info->error_no != CR_SERVER_GONE_ERROR) {
				php_error_docref(NULL, E_WARNING, "Error reading result set's header");
			}
			break;
		}

		if (rset_header.error_info.error_no) {
			/*
			  Cover a protocol design error: error packet does not
			  contain the server status. Therefore, the client has no way
			  to find out whether there are more result sets of
			  a multiple-result-set statement pending. Luckily, in 5.0 an
			  error always aborts execution of a statement, wherever it is
			  a multi-statement or a stored procedure, so it should be
			  safe to unconditionally turn off the flag here.
			*/
			UPSERT_STATUS_SET_SERVER_STATUS(conn->upsert_status, UPSERT_STATUS_GET_SERVER_STATUS(conn->upsert_status) & ~SERVER_MORE_RESULTS_EXISTS);
			/*
			  This will copy the error code and the messages, as they
			  are buffers in the struct
			*/
			COPY_CLIENT_ERROR(conn->error_info, rset_header.error_info);
			ret = FAIL;
			DBG_ERR_FMT("error=%s", rset_header.error_info.error);
			/* Return back from CONN_QUERY_SENT */
			SET_CONNECTION_STATE(&conn->state, CONN_READY);
			break;
		}
		conn->error_info->error_no = 0;

		switch (rset_header.field_count) {
			case MYSQLND_NULL_LENGTH: {	/* LOAD DATA LOCAL INFILE */
				bool is_warning;
				DBG_INF("LOAD DATA");
				conn->last_query_type = QUERY_LOAD_LOCAL;
				conn->field_count = 0; /* overwrite previous value, or the last value could be used and lead to bug#53503 */
				SET_CONNECTION_STATE(&conn->state, CONN_SENDING_LOAD_DATA);
				ret = mysqlnd_handle_local_infile(conn, rset_header.info_or_local_file.s, &is_warning);
				SET_CONNECTION_STATE(&conn->state,  (ret == PASS || is_warning == TRUE)? CONN_READY:CONN_QUIT_SENT);
				MYSQLND_INC_CONN_STATISTIC(conn->stats, STAT_NON_RSET_QUERY);
				break;
			}
			case 0:				/* UPSERT */
				DBG_INF("UPSERT");
				conn->last_query_type = QUERY_UPSERT;
				conn->field_count = rset_header.field_count;
				UPSERT_STATUS_RESET(conn->upsert_status);
				UPSERT_STATUS_SET_WARNINGS(conn->upsert_status, rset_header.warning_count);
				UPSERT_STATUS_SET_SERVER_STATUS(conn->upsert_status, rset_header.server_status);
				UPSERT_STATUS_SET_AFFECTED_ROWS(conn->upsert_status, rset_header.affected_rows);
				UPSERT_STATUS_SET_LAST_INSERT_ID(conn->upsert_status, rset_header.last_insert_id);
				mysqlnd_set_string(&conn->last_message, rset_header.info_or_local_file.s, rset_header.info_or_local_file.l);
				/* Result set can follow UPSERT statement, check server_status */
				if (UPSERT_STATUS_GET_SERVER_STATUS(conn->upsert_status) & SERVER_MORE_RESULTS_EXISTS) {
					SET_CONNECTION_STATE(&conn->state, CONN_NEXT_RESULT_PENDING);
				} else {
					SET_CONNECTION_STATE(&conn->state, CONN_READY);
				}
				ret = PASS;
				MYSQLND_INC_CONN_STATISTIC(conn->stats, STAT_NON_RSET_QUERY);
				break;
			default: do {			/* Result set */
				MYSQLND_RES * result;
				enum_mysqlnd_collected_stats statistic = STAT_LAST;

				DBG_INF("Result set pending");
				mysqlnd_set_string(&conn->last_message, NULL, 0);

				MYSQLND_INC_CONN_STATISTIC(conn->stats, STAT_RSET_QUERY);
				UPSERT_STATUS_RESET(conn->upsert_status);
				/* restore after zeroing */
				UPSERT_STATUS_SET_AFFECTED_ROWS_TO_ERROR(conn->upsert_status);

				conn->last_query_type = QUERY_SELECT;
				SET_CONNECTION_STATE(&conn->state, CONN_FETCHING_DATA);
				/* PS has already allocated it */
				conn->field_count = rset_header.field_count;
				if (!stmt) {
					result = conn->current_result = conn->m->result_init(rset_header.field_count);
				} else {
					if (!stmt->result) {
						DBG_INF("This is 'SHOW'/'EXPLAIN'-like query.");
						/*
						  This is 'SHOW'/'EXPLAIN'-like query. Current implementation of
						  prepared statements can't send result set metadata for these queries
						  on prepare stage. Read it now.
						*/
						result = stmt->result = conn->m->result_init(rset_header.field_count);
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
						if (rset_header.field_count != stmt->result->field_count) {
							stmt->result->m.free_result(stmt->result, TRUE);
							stmt->result = conn->m->result_init(rset_header.field_count);
						}
						result = stmt->result;
					}
				}
				if (!result) {
					SET_OOM_ERROR(conn->error_info);
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
				conn->payload_decoder_factory->m.init_eof_packet(&fields_eof);
				if (FAIL == (ret = PACKET_READ(conn, &fields_eof))) {
					DBG_ERR("Error occurred while reading the EOF packet");
					result->m.free_result_contents(result);
					if (!stmt) {
						conn->current_result = NULL;
					} else {
						stmt->result = NULL;
						/* XXX: This will crash, because we will null also the methods.
							But seems it happens in extreme cases or doesn't. Should be fixed by exporting a function
							(from mysqlnd_driver.c?) to do the reset.
							This is done also in mysqlnd_ps.c
						*/
						memset(stmt, 0, sizeof(*stmt));
						stmt->state = MYSQLND_STMT_INITTED;
					}
				} else {
					DBG_INF_FMT("warnings=%u server_status=%u", fields_eof.warning_count, fields_eof.server_status);
					UPSERT_STATUS_SET_WARNINGS(conn->upsert_status, fields_eof.warning_count);
					/*
					  If SERVER_MORE_RESULTS_EXISTS is set then this is either MULTI_QUERY or a CALL()
					  The first packet after sending the query/com_execute has the bit set only
					  in this cases. Not sure why it's a needed but it marks that the whole stream
					  will include many result sets. What actually matters are the bits set at the end
					  of every result set (the EOF packet).
					*/
					UPSERT_STATUS_SET_SERVER_STATUS(conn->upsert_status, fields_eof.server_status);
					if (fields_eof.server_status & SERVER_QUERY_NO_GOOD_INDEX_USED) {
						statistic = STAT_BAD_INDEX_USED;
					} else if (fields_eof.server_status & SERVER_QUERY_NO_INDEX_USED) {
						statistic = STAT_NO_INDEX_USED;
					} else if (fields_eof.server_status & SERVER_QUERY_WAS_SLOW) {
						statistic = STAT_QUERY_WAS_SLOW;
					}
					MYSQLND_INC_CONN_STATISTIC(conn->stats, statistic);
				}
				PACKET_FREE(&fields_eof);
			} while (0);
			break; /* switch break */
		}
	} while (0);
	PACKET_FREE(&rset_header);

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
static const size_t *
MYSQLND_METHOD(mysqlnd_result_buffered, fetch_lengths)(const MYSQLND_RES_BUFFERED * const result)
{
	DBG_ENTER("mysqlnd_result_buffered::fetch_lengths");

	if (result->current_row > result->row_count || result->current_row == 0) {
		DBG_INF("EOF");
		DBG_RETURN(NULL); /* No more rows, or no fetched row */
	}
	DBG_INF("non NULL");
	DBG_RETURN(result->lengths);
}
/* }}} */


/* {{{ mysqlnd_result_unbuffered::fetch_lengths */
static const size_t *
MYSQLND_METHOD(mysqlnd_result_unbuffered, fetch_lengths)(const MYSQLND_RES_UNBUFFERED * const result)
{
	/* simulate output of libmysql */
	return (result->last_row_buffer.ptr || result->eof_reached)? result->lengths : NULL;
}
/* }}} */


/* {{{ mysqlnd_res::fetch_lengths */
static const size_t *
MYSQLND_METHOD(mysqlnd_res, fetch_lengths)(const MYSQLND_RES * const result)
{
	const size_t * ret;
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


/* {{{ mysqlnd_result_unbuffered::fetch_row */
static enum_func_status
MYSQLND_METHOD(mysqlnd_result_unbuffered, fetch_row)(MYSQLND_RES * result, zval **row_ptr, const unsigned int flags, bool * fetched_anything)
{
	enum_func_status ret;
	MYSQLND_PACKET_ROW	*row_packet = result->unbuf->row_packet;
	const MYSQLND_RES_METADATA * const meta = result->meta;
	MYSQLND_RES_UNBUFFERED *set = result->unbuf;
	MYSQLND_CONN_DATA * const conn = result->conn;
	void *checkpoint;

	DBG_ENTER("mysqlnd_result_unbuffered::fetch_row");

	*fetched_anything = FALSE;
	if (set->eof_reached) {
		/* No more rows obviously */
		DBG_RETURN(PASS);
	}
	if (GET_CONNECTION_STATE(&conn->state) != CONN_FETCHING_DATA) {
		SET_CLIENT_ERROR(conn->error_info, CR_COMMANDS_OUT_OF_SYNC, UNKNOWN_SQLSTATE, mysqlnd_out_of_sync);
		DBG_RETURN(FAIL);
	}
	if (!row_packet) {
		/* Not fully initialized object that is being cleaned up */
		DBG_RETURN(FAIL);
	}

	checkpoint = result->memory_pool->checkpoint;
	mysqlnd_mempool_save_state(result->memory_pool);

	if (PASS == (ret = PACKET_READ(conn, row_packet)) && !row_packet->eof) {
		set->last_row_buffer = row_packet->row_buffer;
		row_packet->row_buffer.ptr = NULL;

		MYSQLND_INC_CONN_STATISTIC(conn->stats, set->stmt
			? STAT_ROWS_FETCHED_FROM_CLIENT_PS_UNBUF
			: STAT_ROWS_FETCHED_FROM_CLIENT_NORMAL_UNBUF);

		if (row_ptr) {
			unsigned int field_count = meta->field_count;

			*row_ptr = result->row_data;
			enum_func_status rc = set->m.row_decoder(
				&set->last_row_buffer, result->row_data, field_count,
				row_packet->fields_metadata, conn->options->int_and_float_native, conn->stats);
			if (PASS != rc) {
				mysqlnd_mempool_restore_state(result->memory_pool);
				result->memory_pool->checkpoint = checkpoint;
				DBG_RETURN(FAIL);
			}

			size_t *lengths = set->lengths;
			if (lengths) {
				for (unsigned i = 0; i < field_count; i++) {
					zval *data = &result->row_data[i];
					lengths[i] = Z_TYPE_P(data) == IS_STRING ? Z_STRLEN_P(data) : 0;
				}
			}
		}
		set->row_count++;
		*fetched_anything = TRUE;
	} else if (ret == FAIL) {
		if (row_packet->error_info.error_no) {
			COPY_CLIENT_ERROR(conn->error_info, row_packet->error_info);
			if (set->stmt) {
				COPY_CLIENT_ERROR(set->stmt->error_info, row_packet->error_info);
			}
			DBG_ERR_FMT("errorno=%u error=%s", row_packet->error_info.error_no, row_packet->error_info.error);
		}
		if (GET_CONNECTION_STATE(&conn->state) != CONN_QUIT_SENT) {
			SET_CONNECTION_STATE(&conn->state, CONN_READY);
		}
		set->eof_reached = TRUE; /* so next time we won't get an error */
	} else if (row_packet->eof) {
		/* Mark the connection as usable again */
		DBG_INF_FMT("warnings=%u server_status=%u", row_packet->warning_count, row_packet->server_status);
		set->eof_reached = TRUE;

		UPSERT_STATUS_RESET(conn->upsert_status);
		UPSERT_STATUS_SET_WARNINGS(conn->upsert_status, row_packet->warning_count);
		UPSERT_STATUS_SET_SERVER_STATUS(conn->upsert_status, row_packet->server_status);
		/*
		  result->row_packet will be cleaned when
		  destroying the result object
		*/
		if (UPSERT_STATUS_GET_SERVER_STATUS(conn->upsert_status) & SERVER_MORE_RESULTS_EXISTS) {
			SET_CONNECTION_STATE(&conn->state, CONN_NEXT_RESULT_PENDING);
		} else {
			SET_CONNECTION_STATE(&conn->state, CONN_READY);
		}
	}

	mysqlnd_mempool_restore_state(result->memory_pool);
	result->memory_pool->checkpoint = checkpoint;

	DBG_INF_FMT("ret=%s fetched=%u", ret == PASS? "PASS":"FAIL", *fetched_anything);
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_res::use_result */
static MYSQLND_RES *
MYSQLND_METHOD(mysqlnd_res, use_result)(MYSQLND_RES * const result, MYSQLND_STMT_DATA *stmt)
{
	MYSQLND_CONN_DATA * const conn = result->conn;
	DBG_ENTER("mysqlnd_res::use_result");

	SET_EMPTY_ERROR(conn->error_info);

	if (!stmt) {
		result->type = MYSQLND_RES_NORMAL;
	} else {
		result->type = MYSQLND_RES_PS_UNBUF;
	}

	result->unbuf = mysqlnd_result_unbuffered_init(result, result->field_count, stmt);

	/*
	  Will be freed in the mysqlnd_internal_free_result_contents() called
	  by the resource destructor. mysqlnd_result_unbuffered::fetch_row() expects
	  this to be not NULL.
	*/
	/* FALSE = non-persistent */
	{
		struct st_mysqlnd_packet_row *row_packet = mnd_emalloc(sizeof(struct st_mysqlnd_packet_row));

		conn->payload_decoder_factory->m.init_row_packet(row_packet);
		row_packet->result_set_memory_pool = result->unbuf->result_set_memory_pool;
		row_packet->field_count = result->field_count;
		row_packet->binary_protocol = stmt != NULL;
		row_packet->fields_metadata = result->meta->fields;

		result->unbuf->row_packet = row_packet;
	}

	DBG_RETURN(result);
}
/* }}} */


/* {{{ mysqlnd_result_buffered::fetch_row */
static enum_func_status
MYSQLND_METHOD(mysqlnd_result_buffered, fetch_row)(MYSQLND_RES * result, zval **row_ptr, const unsigned int flags, bool * fetched_anything)
{
	MYSQLND_RES_BUFFERED *set = result->stored_data;

	DBG_ENTER("mysqlnd_result_buffered::fetch_row");

	/* If we haven't read everything */
	if (set->current_row < set->row_count) {
		if (row_ptr) {
			const MYSQLND_RES_METADATA * const meta = result->meta;
			const unsigned int field_count = meta->field_count;
			MYSQLND_CONN_DATA * const conn = result->conn;
			enum_func_status rc;
			zval *current_row = result->row_data;
			*row_ptr = result->row_data;
			rc = result->stored_data->m.row_decoder(&set->row_buffers[set->current_row],
													current_row,
													field_count,
													meta->fields,
													conn->options->int_and_float_native,
													conn->stats);
			if (rc != PASS) {
				DBG_RETURN(FAIL);
			}

			if (set->lengths) {
				for (unsigned i = 0; i < field_count; ++i) {
					zval *data = &current_row[i];
					set->lengths[i] = Z_TYPE_P(data) == IS_STRING ? Z_STRLEN_P(data) : 0;
				}
			}
		}

		++set->current_row;
		MYSQLND_INC_GLOBAL_STATISTIC(set->stmt
			? STAT_ROWS_FETCHED_FROM_CLIENT_PS_BUF : STAT_ROWS_FETCHED_FROM_CLIENT_NORMAL_BUF);
		*fetched_anything = TRUE;
	} else {
		if (set->current_row == set->row_count) {
			set->current_row = set->row_count + 1;
		}
		DBG_INF_FMT("EOF reached. current_row=%llu", (unsigned long long) set->current_row);
		*fetched_anything = FALSE;
	}

	DBG_INF_FMT("ret=PASS fetched=%u", *fetched_anything);
	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ mysqlnd_res::fetch_row */
static enum_func_status
MYSQLND_METHOD(mysqlnd_res, fetch_row)(MYSQLND_RES *result, zval **row_ptr, const unsigned int flags, bool *fetched_anything)
{
	const mysqlnd_fetch_row_func f =
		result->stored_data ? result->stored_data->m.fetch_row :
		result->unbuf ? result->unbuf->m.fetch_row : NULL;
	if (f) {
		return f(result, row_ptr, flags, fetched_anything);
	}
	*fetched_anything = FALSE;
	return PASS;
}
/* }}} */


/* {{{ mysqlnd_res::store_result_fetch_data */
enum_func_status
MYSQLND_METHOD(mysqlnd_res, store_result_fetch_data)(MYSQLND_CONN_DATA * const conn, MYSQLND_RES * result,
													MYSQLND_RES_METADATA * meta,
													MYSQLND_ROW_BUFFER **row_buffers,
													bool binary_protocol)
{
	enum_func_status ret;
	uint64_t total_allocated_rows = 0;
	unsigned int free_rows = 0;
	MYSQLND_RES_BUFFERED * set = result->stored_data;
	MYSQLND_PACKET_ROW row_packet;

	DBG_ENTER("mysqlnd_res::store_result_fetch_data");
	if (!set || !row_buffers) {
		ret = FAIL;
		goto end;
	}

	*row_buffers = NULL;

	conn->payload_decoder_factory->m.init_row_packet(&row_packet);
	set->references	= 1;

	row_packet.result_set_memory_pool = result->stored_data->result_set_memory_pool;
	row_packet.field_count = meta->field_count;
	row_packet.binary_protocol = binary_protocol;
	row_packet.fields_metadata = meta->fields;

	while (FAIL != (ret = PACKET_READ(conn, &row_packet)) && !row_packet.eof) {
		if (!free_rows) {
			MYSQLND_ROW_BUFFER * new_row_buffers;

			if (total_allocated_rows < 1024) {
				if (total_allocated_rows == 0) {
					free_rows = 1;
					total_allocated_rows = 1;
				} else {
					free_rows = total_allocated_rows;
					total_allocated_rows *= 2;
				}
			} else {
				free_rows = 1024;
				total_allocated_rows += 1024;
			}

			/* don't try to allocate more than possible - mnd_XXalloc expects size_t, and it can have narrower range than uint64_t */
			if (total_allocated_rows * sizeof(MYSQLND_ROW_BUFFER) > SIZE_MAX) {
				SET_OOM_ERROR(conn->error_info);
				ret = FAIL;
				goto free_end;
			}
			if (*row_buffers) {
				new_row_buffers = mnd_erealloc(*row_buffers, (size_t)(total_allocated_rows * sizeof(MYSQLND_ROW_BUFFER)));
			} else {
				new_row_buffers = mnd_emalloc((size_t)(total_allocated_rows * sizeof(MYSQLND_ROW_BUFFER)));
			}
			*row_buffers = new_row_buffers;
		}
		free_rows--;
		(*row_buffers)[set->row_count] = row_packet.row_buffer;

		set->row_count++;

		/* So row_packet's destructor function won't efree() it */
		row_packet.row_buffer.ptr = NULL;
	}
	/* Overflow ? */
	MYSQLND_INC_CONN_STATISTIC_W_VALUE(conn->stats,
									   binary_protocol? STAT_ROWS_BUFFERED_FROM_CLIENT_PS:
														STAT_ROWS_BUFFERED_FROM_CLIENT_NORMAL,
									   set->row_count);

	/* Finally clean */
	if (row_packet.eof) {
		UPSERT_STATUS_RESET(conn->upsert_status);
		UPSERT_STATUS_SET_WARNINGS(conn->upsert_status, row_packet.warning_count);
		UPSERT_STATUS_SET_SERVER_STATUS(conn->upsert_status, row_packet.server_status);
	}

	if (ret == FAIL) {
		/* Error packets do not contain server status information. However, we know that after
		 * an error there will be no further result sets. */
		UPSERT_STATUS_SET_SERVER_STATUS(conn->upsert_status,
			UPSERT_STATUS_GET_SERVER_STATUS(conn->upsert_status) & ~SERVER_MORE_RESULTS_EXISTS);
	}

	/* save some memory */
	if (free_rows) {
		/* don't try to allocate more than possible - mnd_XXalloc expects size_t, and it can have narrower range than uint64_t */
		if (set->row_count * sizeof(MYSQLND_ROW_BUFFER) > SIZE_MAX) {
			SET_OOM_ERROR(conn->error_info);
			ret = FAIL;
			goto free_end;
		}
		*row_buffers = mnd_erealloc(*row_buffers, (size_t) (set->row_count * sizeof(MYSQLND_ROW_BUFFER)));
	}

	if (UPSERT_STATUS_GET_SERVER_STATUS(conn->upsert_status) & SERVER_MORE_RESULTS_EXISTS) {
		SET_CONNECTION_STATE(&conn->state, CONN_NEXT_RESULT_PENDING);
	} else {
		SET_CONNECTION_STATE(&conn->state, CONN_READY);
	}

	if (ret == FAIL) {
		COPY_CLIENT_ERROR(&set->error_info, row_packet.error_info);
	} else {
		/* libmysql's documentation says it should be so for SELECT statements */
		UPSERT_STATUS_SET_AFFECTED_ROWS(conn->upsert_status, set->row_count);
	}
	DBG_INF_FMT("ret=%s row_count=%u warnings=%u server_status=%u",
				ret == PASS? "PASS":"FAIL",
				(uint32_t) set->row_count,
				UPSERT_STATUS_GET_WARNINGS(conn->upsert_status),
				UPSERT_STATUS_GET_SERVER_STATUS(conn->upsert_status));
free_end:
	PACKET_FREE(&row_packet);
	DBG_INF_FMT("rows=%llu", (unsigned long long)set->row_count);
end:
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_res::store_result */
static MYSQLND_RES *
MYSQLND_METHOD(mysqlnd_res, store_result)(MYSQLND_RES * result,
										  MYSQLND_CONN_DATA * const conn,
										  MYSQLND_STMT_DATA *stmt)
{
	enum_func_status ret;
	MYSQLND_ROW_BUFFER **row_buffers = NULL;

	DBG_ENTER("mysqlnd_res::store_result");

	/* We need the conn because we are doing lazy zval initialization in buffered_fetch_row */
	/* In case of error the reference will be released in free_result() called indirectly by our caller */
	result->conn = conn->m->get_reference(conn);
	result->type = MYSQLND_RES_NORMAL;

	SET_CONNECTION_STATE(&conn->state, CONN_FETCHING_DATA);

	result->stored_data	= (MYSQLND_RES_BUFFERED *) mysqlnd_result_buffered_init(result, result->field_count, stmt);
	row_buffers = &result->stored_data->row_buffers;

	ret = result->m.store_result_fetch_data(conn, result, result->meta, row_buffers, stmt != NULL);

	if (FAIL == ret) {
		if (result->stored_data) {
			COPY_CLIENT_ERROR(conn->error_info, result->stored_data->error_info);
		} else {
			SET_OOM_ERROR(conn->error_info);
		}
		DBG_RETURN(NULL);
	} else {
		result->stored_data->current_row = 0;
	}

	/* libmysql's documentation says it should be so for SELECT statements */
	UPSERT_STATUS_SET_AFFECTED_ROWS(conn->upsert_status, result->stored_data->row_count);

	DBG_RETURN(result);
}
/* }}} */


/* {{{ mysqlnd_res::skip_result */
static void
MYSQLND_METHOD(mysqlnd_res, skip_result)(MYSQLND_RES * const result)
{
	bool fetched_anything;

	DBG_ENTER("mysqlnd_res::skip_result");
	/*
	  Unbuffered sets
	  A PS could be prepared - there is metadata and thus a stmt->result but the
	  fetch_row function isn't actually set (NULL), thus we have to skip these.
	*/
	if (result->unbuf && !result->unbuf->eof_reached) {
		MYSQLND_CONN_DATA * const conn = result->conn;
		DBG_INF("skipping result");
		/* We have to fetch all data to clean the line */
		MYSQLND_INC_CONN_STATISTIC(conn->stats,
									result->type == MYSQLND_RES_NORMAL? STAT_FLUSHED_NORMAL_SETS:
																		STAT_FLUSHED_PS_SETS);

		while ((PASS == result->m.fetch_row(result, NULL, 0, &fetched_anything)) && fetched_anything == TRUE) {
			MYSQLND_INC_CONN_STATISTIC(conn->stats,
				result->type == MYSQLND_RES_NORMAL
					? STAT_ROWS_SKIPPED_NORMAL : STAT_ROWS_SKIPPED_PS);
		}
	}
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_res::free_result */
static enum_func_status
MYSQLND_METHOD(mysqlnd_res, free_result)(MYSQLND_RES * result, const bool implicit)
{
	DBG_ENTER("mysqlnd_res::free_result");

	MYSQLND_INC_CONN_STATISTIC(result->conn? result->conn->stats : NULL,
							   implicit == TRUE?	STAT_FREE_RESULT_IMPLICIT:
							   						STAT_FREE_RESULT_EXPLICIT);

	result->m.skip_result(result);
	result->m.free_result_contents(result);
	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ mysqlnd_res::data_seek */
static enum_func_status
MYSQLND_METHOD(mysqlnd_res, data_seek)(MYSQLND_RES * const result, const uint64_t row)
{
	DBG_ENTER("mysqlnd_res::data_seek");
	DBG_INF_FMT("row=%" PRIu64, row);

	DBG_RETURN(result->stored_data? result->stored_data->m.data_seek(result->stored_data, row) : FAIL);
}
/* }}} */


/* {{{ mysqlnd_result_buffered::data_seek */
static enum_func_status
MYSQLND_METHOD(mysqlnd_result_buffered, data_seek)(MYSQLND_RES_BUFFERED * const result, const uint64_t row)
{
	DBG_ENTER("mysqlnd_result_buffered::data_seek");

	/* libmysql just moves to the end, it does traversing of a linked list */
	if (row >= result->row_count) {
		result->current_row = result->row_count;
	} else {
		result->current_row = row;
	}
	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ mysqlnd_result_unbuffered::num_rows */
static uint64_t
MYSQLND_METHOD(mysqlnd_result_unbuffered, num_rows)(const MYSQLND_RES_UNBUFFERED * const result)
{
	/* Be compatible with libmysql. We count row_count, but will return 0 */
	return result->eof_reached? result->row_count : 0;
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
										zval *return_value ZEND_FILE_LINE_DC)
{
	bool fetched_anything;
	zval *row_data;

	DBG_ENTER("mysqlnd_res::fetch_into");
	if (FAIL == result->m.fetch_row(result, &row_data, flags, &fetched_anything)) {
		RETVAL_FALSE;
		DBG_VOID_RETURN;
	} else if (fetched_anything == FALSE) {
		RETVAL_NULL();
		DBG_VOID_RETURN;
	}

	const MYSQLND_RES_METADATA * const meta = result->meta;
	unsigned int array_size = meta->field_count;
	if ((flags & (MYSQLND_FETCH_NUM|MYSQLND_FETCH_ASSOC)) == (MYSQLND_FETCH_NUM|MYSQLND_FETCH_ASSOC)) {
		array_size *= 2;
	}
	array_init_size(return_value, array_size);

	HashTable *row_ht = Z_ARRVAL_P(return_value);
	MYSQLND_FIELD *field = meta->fields;
	for (unsigned i = 0; i < meta->field_count; i++, field++) {
		zval *data = &row_data[i];

		if (flags & MYSQLND_FETCH_NUM) {
			if (zend_hash_index_add(row_ht, i, data) != NULL) {
				Z_TRY_ADDREF_P(data);
			}
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
			if (meta->fields[i].is_numeric == FALSE) {
				zend_hash_update(row_ht, meta->fields[i].sname, data);
			} else {
				zend_hash_index_update(row_ht, meta->fields[i].num_key, data);
			}
		}

		zval_ptr_dtor_nogc(data);
	}
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_res::fetch_row_c */
static MYSQLND_ROW_C
MYSQLND_METHOD(mysqlnd_res, fetch_row_c)(MYSQLND_RES * result)
{
	bool fetched_anything;
	zval *row_data;
	MYSQLND_ROW_C ret = NULL;
	DBG_ENTER("mysqlnd_res::fetch_row_c");

	mysqlnd_result_free_prev_data(result);
	if (result->m.fetch_row(result, &row_data, 0, &fetched_anything) == PASS && fetched_anything) {
		unsigned field_count = result->field_count;
		MYSQLND_FIELD *field = result->meta->fields;

		ret = mnd_emalloc(field_count * sizeof(char *));
		for (unsigned i = 0; i < field_count; i++, field++) {
			zval *data = &row_data[i];
			if (Z_TYPE_P(data) != IS_NULL) {
				convert_to_string(data);
				ret[i] = Z_STRVAL_P(data);
			} else {
				ret[i] = NULL;
			}
		}
		result->free_row_data = 1;
	}
	DBG_RETURN(ret);
}
/* }}} */


MYSQLND_CLASS_METHODS_START(mysqlnd_res)
	MYSQLND_METHOD(mysqlnd_res, fetch_row),
	MYSQLND_METHOD(mysqlnd_res, use_result),
	MYSQLND_METHOD(mysqlnd_res, store_result),
	MYSQLND_METHOD(mysqlnd_res, fetch_into),
	MYSQLND_METHOD(mysqlnd_res, fetch_row_c),
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
	MYSQLND_METHOD(mysqlnd_res, free_result_contents_internal),
	mysqlnd_result_meta_init,
	NULL, /* unused1 */
	NULL, /* unused2 */
	NULL, /* unused3 */
	NULL, /* unused4 */
	NULL  /* unused5 */
MYSQLND_CLASS_METHODS_END;


MYSQLND_CLASS_METHODS_START(mysqlnd_result_unbuffered)
	MYSQLND_METHOD(mysqlnd_result_unbuffered, fetch_row),
	NULL, /* row_decoder */
	MYSQLND_METHOD(mysqlnd_result_unbuffered, num_rows),
	MYSQLND_METHOD(mysqlnd_result_unbuffered, fetch_lengths),
	MYSQLND_METHOD(mysqlnd_result_unbuffered, free_result)
MYSQLND_CLASS_METHODS_END;


MYSQLND_CLASS_METHODS_START(mysqlnd_result_buffered)
	MYSQLND_METHOD(mysqlnd_result_buffered, fetch_row),
	NULL, /* row_decoder */
	MYSQLND_METHOD(mysqlnd_result_buffered, num_rows),
	MYSQLND_METHOD(mysqlnd_result_buffered, fetch_lengths),
	MYSQLND_METHOD(mysqlnd_result_buffered, data_seek),
	MYSQLND_METHOD(mysqlnd_result_buffered, free_result)
MYSQLND_CLASS_METHODS_END;


/* {{{ mysqlnd_result_init */
PHPAPI MYSQLND_RES *
mysqlnd_result_init(const unsigned int field_count)
{
	const size_t alloc_size = sizeof(MYSQLND_RES) + mysqlnd_plugin_count() * sizeof(void *);
	MYSQLND_MEMORY_POOL * pool;
	MYSQLND_RES * ret;

	DBG_ENTER("mysqlnd_result_init");

	pool = mysqlnd_mempool_create(MYSQLND_G(mempool_default_size));
	if (!pool) {
		DBG_RETURN(NULL);
	}

	ret = pool->get_chunk(pool, alloc_size);
	memset(ret, 0, alloc_size);

	ret->row_data = pool->get_chunk(pool, field_count * sizeof(zval));
	ret->free_row_data = 0;

	ret->memory_pool	= pool;
	ret->field_count	= field_count;
	ret->m = *mysqlnd_result_get_methods();

	mysqlnd_mempool_save_state(pool);

	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_result_unbuffered_init */
PHPAPI MYSQLND_RES_UNBUFFERED *
mysqlnd_result_unbuffered_init(MYSQLND_RES *result, const unsigned int field_count, MYSQLND_STMT_DATA *stmt)
{
	const size_t alloc_size = sizeof(MYSQLND_RES_UNBUFFERED) + mysqlnd_plugin_count() * sizeof(void *);
	MYSQLND_MEMORY_POOL * pool = result->memory_pool;
	MYSQLND_RES_UNBUFFERED * ret;

	DBG_ENTER("mysqlnd_result_unbuffered_init");

	ret = pool->get_chunk(pool, alloc_size);
	memset(ret, 0, alloc_size);

	ret->result_set_memory_pool = pool;
	ret->field_count = field_count;
	ret->stmt = stmt;

	ret->m = *mysqlnd_result_unbuffered_get_methods();

	if (stmt) {
		ret->m.row_decoder = php_mysqlnd_rowp_read_binary_protocol;
		ret->m.fetch_lengths = NULL; /* makes no sense */
		ret->lengths = NULL;
	} else {
		ret->m.row_decoder = php_mysqlnd_rowp_read_text_protocol;

		ret->lengths = pool->get_chunk(pool, field_count * sizeof(size_t));
		memset(ret->lengths, 0, field_count * sizeof(size_t));
	}

	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_result_buffered_init */
PHPAPI MYSQLND_RES_BUFFERED *
mysqlnd_result_buffered_init(MYSQLND_RES * result, const unsigned int field_count, MYSQLND_STMT_DATA *stmt)
{
	const size_t alloc_size = sizeof(MYSQLND_RES_BUFFERED) + mysqlnd_plugin_count() * sizeof(void *);
	MYSQLND_MEMORY_POOL * pool = result->memory_pool;
	MYSQLND_RES_BUFFERED * ret;

	DBG_ENTER("mysqlnd_result_buffered_init");

	ret = pool->get_chunk(pool, alloc_size);
	memset(ret, 0, alloc_size);

	mysqlnd_error_info_init(&ret->error_info, /* persistent */ 0);

	ret->result_set_memory_pool = pool;
	ret->field_count= field_count;
	ret->stmt = stmt;
	ret->m = *mysqlnd_result_buffered_get_methods();

	if (stmt) {
		ret->m.row_decoder = php_mysqlnd_rowp_read_binary_protocol;
		ret->m.fetch_lengths = NULL; /* makes no sense */
		ret->lengths = NULL;
	} else {
		ret->m.row_decoder = php_mysqlnd_rowp_read_text_protocol;

		ret->lengths = pool->get_chunk(pool, field_count * sizeof(size_t));
		memset(ret->lengths, 0, field_count * sizeof(size_t));
	}

	DBG_RETURN(ret);
}
/* }}} */
