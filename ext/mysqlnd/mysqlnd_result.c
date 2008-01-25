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
#include "mysqlnd_charset.h"
#include "mysqlnd_debug.h"
#include "ext/standard/basic_functions.h"

#define MYSQLND_SILENT


/* {{{ mysqlnd_unbuffered_free_last_data */
void mysqlnd_unbuffered_free_last_data(MYSQLND_RES *result TSRMLS_DC)
{
	MYSQLND_RES_UNBUFFERED *unbuf = result->unbuf;

	DBG_ENTER("mysqlnd_unbuffered_free_last_data");

	if (!unbuf) {
		DBG_VOID_RETURN;
	}

	if (unbuf->last_row_data) {
		unsigned int i, ctor_called_count = 0;
		zend_bool copy_ctor_called;
		MYSQLND_STATS *global_stats = result->conn? &result->conn->stats:NULL;
		for (i = 0; i < result->field_count; i++) {
			mysqlnd_palloc_zval_ptr_dtor(&(unbuf->last_row_data[i]),
										 result->zval_cache, result->type,
										 &copy_ctor_called TSRMLS_CC);
			if (copy_ctor_called) {
				ctor_called_count++;
			}
		}
		/* By using value3 macros we hold a mutex only once, there is no value2 */
		MYSQLND_INC_CONN_STATISTIC_W_VALUE3(global_stats,
											STAT_COPY_ON_WRITE_PERFORMED,
											ctor_called_count,
											STAT_COPY_ON_WRITE_SAVED,
											result->field_count - ctor_called_count,
											STAT_COPY_ON_WRITE_PERFORMED, 0);
		
		/* Free last row's zvals */
		efree(unbuf->last_row_data);
		unbuf->last_row_data = NULL;
	}
	if (unbuf->last_row_buffer) {
		/* Nothing points to this buffer now, free it */
		efree(unbuf->last_row_buffer);
		unbuf->last_row_buffer = NULL;
	}

	DBG_VOID_RETURN;
}
/* }}} */

/* {{{ mysqlnd_free_buffered_data */
void mysqlnd_free_buffered_data(MYSQLND_RES *result TSRMLS_DC)
{
	MYSQLND_THD_ZVAL_PCACHE  *zval_cache = result->zval_cache;
	MYSQLND_RES_BUFFERED *set = result->data;
	unsigned int field_count = result->field_count;
	unsigned int row;

	DBG_ENTER("mysqlnd_free_buffered_data");
	DBG_INF_FMT("Freeing "MYSQLND_LLU_SPEC" row(s)", result->data->row_count);

	DBG_INF_FMT("before: real_usage=%lu  usage=%lu", zend_memory_usage(TRUE TSRMLS_CC), zend_memory_usage(FALSE TSRMLS_CC));
	for (row = 0; row < result->data->row_count; row++) {
		unsigned int col;
		zval **current_row = current_row = set->data[row];
		zend_uchar *current_buffer = set->row_buffers[row];

		for (col = 0; col < field_count; col++) {
			zend_bool copy_ctor_called;
			mysqlnd_palloc_zval_ptr_dtor(&(current_row[col]), zval_cache,
										 result->type, &copy_ctor_called TSRMLS_CC);
#if MYSQLND_DEBUG_MEMORY
			DBG_INF_FMT("Copy_ctor_called=%d", copy_ctor_called);
#endif
			MYSQLND_INC_GLOBAL_STATISTIC(copy_ctor_called? STAT_COPY_ON_WRITE_PERFORMED:
														   STAT_COPY_ON_WRITE_SAVED);
		}
#if MYSQLND_DEBUG_MEMORY
		DBG_INF("Freeing current_row & current_buffer");
#endif
		pefree(current_row, set->persistent);
		pefree(current_buffer, set->persistent);
	}
	DBG_INF("Freeing data & row_buffer");
	pefree(set->data, set->persistent);
	pefree(set->row_buffers, set->persistent);
	set->data			= NULL;
	set->row_buffers	= NULL;
	set->data_cursor	= NULL;
	set->row_count	= 0;
	if (set->qcache) {
		mysqlnd_qcache_free_cache_reference(&set->qcache);
	}
	DBG_INF("Freeing set");
	pefree(set, set->persistent);

	DBG_INF_FMT("after: real_usage=%lu  usage=%lu", zend_memory_usage(TRUE TSRMLS_CC), zend_memory_usage(FALSE TSRMLS_CC));
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_res::free_result_buffers */
void
MYSQLND_METHOD(mysqlnd_res, free_result_buffers)(MYSQLND_RES *result TSRMLS_DC)
{
	DBG_ENTER("mysqlnd_res::free_result_buffers");
	DBG_INF_FMT("%s", result->unbuf? "unbuffered":(result->data? "buffered":"unknown"));

	if (result->unbuf) {
		mysqlnd_unbuffered_free_last_data(result TSRMLS_CC);
		efree(result->unbuf);
		result->unbuf = NULL;
	} else if (result->data) {
		mysqlnd_free_buffered_data(result TSRMLS_CC);
		result->data = NULL;
	}

	if (result->lengths) {
		efree(result->lengths);
		result->lengths = NULL;
	}

	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_internal_free_result_contents */
static
void mysqlnd_internal_free_result_contents(MYSQLND_RES *result TSRMLS_DC)
{
	DBG_ENTER("mysqlnd_internal_free_result_contents");

	result->m.free_result_buffers(result TSRMLS_CC);

	if (result->row_packet) {
		DBG_INF("Freeing packet");
		PACKET_FREE(result->row_packet);
		result->row_packet = NULL;
	}

	result->conn = NULL;

	if (result->meta) {
		result->meta->m->free_metadata(result->meta, FALSE TSRMLS_CC);
		result->meta = NULL;
	}

	if (result->zval_cache) {
		DBG_INF("Freeing zval cache reference");
		mysqlnd_palloc_free_thd_cache_reference(&result->zval_cache);
		result->zval_cache = NULL;
	}

	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_internal_free_result */
static
void mysqlnd_internal_free_result(MYSQLND_RES *result TSRMLS_DC)
{
	DBG_ENTER("mysqlnd_internal_free_result");
	/*
	  result->conn is an address if this is an unbuffered query.
	  In this case, decrement the reference counter in the connection
	  object and if needed free the latter.
	*/
	if (result->conn) {
		result->conn->m->free_reference(result->conn TSRMLS_CC);
		result->conn = NULL;
	}

	result->m.free_result_contents(result TSRMLS_CC);
	efree(result);

	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_res::read_result_metadata */
static enum_func_status
MYSQLND_METHOD(mysqlnd_res, read_result_metadata)(MYSQLND_RES *result, MYSQLND *conn TSRMLS_DC)
{
	DBG_ENTER("mysqlnd_res::read_result_metadata");

	/*
	  Make it safe to call it repeatedly for PS -
	  better free and allocate a new because the number of field might change 
	  (select *) with altered table. Also for statements which skip the PS
	  infrastructure!
	*/
	if (result->meta) {
		result->meta->m->free_metadata(result->meta, FALSE TSRMLS_CC);
		result->meta = NULL;
	}

	result->meta = mysqlnd_result_meta_init(result->field_count TSRMLS_CC);

	/* 1. Read all fields metadata */

	/* It's safe to reread without freeing */
	if (FAIL == result->meta->m->read_metadata(result->meta, conn TSRMLS_CC)) {
		result->m.free_result_contents(result TSRMLS_CC);
		DBG_RETURN(FAIL);
	}

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
mysqlnd_query_read_result_set_header(MYSQLND *conn, MYSQLND_STMT *stmt TSRMLS_DC)
{
	enum_func_status ret;
	php_mysql_packet_rset_header rset_header;

	DBG_ENTER("mysqlnd_query_read_result_set_header");
	DBG_INF_FMT("stmt=%d", stmt? stmt->stmt_id:0);

	ret = FAIL;
	PACKET_INIT_ALLOCA(rset_header, PROT_RSET_HEADER_PACKET);
	do {
		if (FAIL == (ret = PACKET_READ_ALLOCA(rset_header, conn))) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Error reading result set's header");
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
			conn->upsert_status.server_status &= ~SERVER_MORE_RESULTS_EXISTS;
			conn->upsert_status.affected_rows = -1;
			/*
			  This will copy the error code and the messages, as they
			  are buffers in the struct
			*/
			conn->error_info = rset_header.error_info;
			ret = FAIL;
			break;
		}
		conn->error_info.error_no = 0;

		switch (rset_header.field_count) {
			case MYSQLND_NULL_LENGTH: {	/* LOAD DATA LOCAL INFILE */
				zend_bool is_warning;
				DBG_INF("LOAD DATA");
				conn->last_query_type = QUERY_LOAD_LOCAL;
				conn->state = CONN_SENDING_LOAD_DATA;
				ret = mysqlnd_handle_local_infile(conn, rset_header.info_or_local_file, &is_warning TSRMLS_CC);
				conn->state = (ret == PASS || is_warning == TRUE)? CONN_READY:CONN_QUIT_SENT;
				MYSQLND_INC_CONN_STATISTIC(&conn->stats, STAT_NON_RSET_QUERY);
				break;
			}
			case 0:				/* UPSERT */
				DBG_INF("UPSERT");
				conn->last_query_type = QUERY_UPSERT;
				conn->field_count = rset_header.field_count;
				conn->upsert_status.warning_count = rset_header.warning_count;
				conn->upsert_status.server_status = rset_header.server_status;
				conn->upsert_status.affected_rows = rset_header.affected_rows;
				conn->upsert_status.last_insert_id = rset_header.last_insert_id;
				SET_NEW_MESSAGE(conn->last_message, conn->last_message_len,
								rset_header.info_or_local_file, rset_header.info_or_local_file_len,
								conn->persistent);
				/* Result set can follow UPSERT statement, check server_status */
				if (conn->upsert_status.server_status & SERVER_MORE_RESULTS_EXISTS) {
					conn->state = CONN_NEXT_RESULT_PENDING;
				} else {
					conn->state = CONN_READY;
				}
				ret = PASS;
				MYSQLND_INC_CONN_STATISTIC(&conn->stats, STAT_NON_RSET_QUERY);
				break;
			default:{			/* Result set	*/
				php_mysql_packet_eof fields_eof;
				MYSQLND_RES *result;
				enum_mysqlnd_collected_stats stat = STAT_LAST;

				DBG_INF("Result set pending");
				SET_EMPTY_MESSAGE(conn->last_message, conn->last_message_len, conn->persistent);

				MYSQLND_INC_CONN_STATISTIC(&conn->stats, STAT_RSET_QUERY);
				memset(&conn->upsert_status, 0, sizeof(conn->upsert_status));
				conn->last_query_type = QUERY_SELECT;
				conn->state = CONN_FETCHING_DATA;
				/* PS has already allocated it */
				if (!stmt) {
					conn->field_count = rset_header.field_count;
					result =
						conn->current_result=
							mysqlnd_result_init(rset_header.field_count,
												mysqlnd_palloc_get_thd_cache_reference(conn->zval_cache)
												TSRMLS_CC);
				} else {
					if (!stmt->result) {
						DBG_INF("This is 'SHOW'/'EXPLAIN'-like query.");
						/*
						  This is 'SHOW'/'EXPLAIN'-like query. Current implementation of
						  prepared statements can't send result set metadata for these queries
						  on prepare stage. Read it now.
						*/
						conn->field_count = rset_header.field_count;
						result =
							stmt->result =
								mysqlnd_result_init(rset_header.field_count,
													mysqlnd_palloc_get_thd_cache_reference(conn->zval_cache)
													TSRMLS_CC);
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

				if (FAIL == (ret = result->m.read_result_metadata(result, conn TSRMLS_CC))) {
					/* For PS, we leave them in Prepared state */
					if (!stmt) {
						efree(conn->current_result);
						conn->current_result = NULL;
					}
					DBG_ERR("Error ocurred while reading metadata");
					break;
				}

				/* Check for SERVER_STATUS_MORE_RESULTS if needed */
				PACKET_INIT_ALLOCA(fields_eof, PROT_EOF_PACKET);
				if (FAIL == (ret = PACKET_READ_ALLOCA(fields_eof, conn))) {
					DBG_ERR("Error ocurred while reading the EOF packet");
					result->m.free_result_contents(result TSRMLS_CC);
					efree(result);
					if (!stmt) {
						conn->current_result = NULL;
					} else {
						stmt->result = NULL;
						memset(stmt, 0, sizeof(MYSQLND_STMT));
						stmt->state = MYSQLND_STMT_INITTED;
					}
				} else {
					DBG_INF_FMT("warns=%u status=%u", fields_eof.warning_count, fields_eof.server_status);
					conn->upsert_status.warning_count = fields_eof.warning_count;
					conn->upsert_status.server_status = fields_eof.server_status;
					if (fields_eof.server_status & MYSQLND_SERVER_QUERY_NO_GOOD_INDEX_USED) {
						stat = STAT_BAD_INDEX_USED;
					} else if (fields_eof.server_status & MYSQLND_SERVER_QUERY_NO_INDEX_USED) {
						stat = STAT_NO_INDEX_USED;
					}
					if (stat != STAT_LAST) {
						char *backtrace = mysqlnd_get_backtrace(TSRMLS_C);
#if A0
						php_log_err(backtrace TSRMLS_CC);
#endif
						efree(backtrace);
						MYSQLND_INC_CONN_STATISTIC(&conn->stats, stat);
					}
				}

				PACKET_FREE_ALLOCA(fields_eof);

				break;
			}
		}
	} while (0);
	PACKET_FREE_ALLOCA(rset_header);

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
static 
unsigned long * mysqlnd_fetch_lengths_buffered(MYSQLND_RES * const result)
{
	unsigned int i;
	zval **previous_row;

	/*
	  If:
	  - unbuffered result
	  - first row has not been read
	  - last_row has been read
	*/
	if (result->data->data_cursor == NULL ||
		result->data->data_cursor == result->data->data ||
		((result->data->data_cursor - result->data->data) > result->data->row_count))
	{
		return NULL;/* No rows or no more rows */
	}

	previous_row = *(result->data->data_cursor - 1);
	for (i = 0; i < result->field_count; i++) {
		result->lengths[i] = (Z_TYPE_P(previous_row[i]) == IS_NULL)? 0:Z_STRLEN_P(previous_row[i]);
	}

	return result->lengths;
}
/* }}} */


/* {{{ mysqlnd_fetch_lengths_unbuffered */
static
unsigned long * mysqlnd_fetch_lengths_unbuffered(MYSQLND_RES * const result)
{
	return result->lengths;
}
/* }}} */


/* {{{ mysqlnd_res::fetch_lengths */
PHPAPI unsigned long * mysqlnd_fetch_lengths(MYSQLND_RES * const result)
{
	return result->m.fetch_lengths? result->m.fetch_lengths(result):NULL;
}
/* }}} */


/* {{{ mysqlnd_fetch_row_unbuffered_c */
static MYSQLND_ROW_C
mysqlnd_fetch_row_unbuffered_c(MYSQLND_RES *result TSRMLS_DC)
{
	enum_func_status		ret;
	MYSQLND_ROW_C 			retrow = NULL;
	unsigned int			i,
							field_count = result->field_count;
	php_mysql_packet_row	*row_packet = result->row_packet;
	unsigned long			*lengths = result->lengths;

	DBG_ENTER("mysqlnd_fetch_row_unbuffered");

	if (result->unbuf->eof_reached) {
		/* No more rows obviously */
		DBG_RETURN(retrow);
	}
	if (result->conn->state != CONN_FETCHING_DATA) {
		SET_CLIENT_ERROR(result->conn->error_info, CR_COMMANDS_OUT_OF_SYNC,
						 UNKNOWN_SQLSTATE, mysqlnd_out_of_sync); 
		DBG_RETURN(retrow);
	}
	/* Let the row packet fill our buffer and skip additional mnd_malloc + memcpy */
	row_packet->skip_extraction = FALSE;

	/*
	  If we skip rows (row == NULL) we have to
	  mysqlnd_unbuffered_free_last_data() before it. The function returns always true.
	*/
	if (PASS == (ret = PACKET_READ(row_packet, result->conn)) && !row_packet->eof) {
		MYSQLND_FIELD *field = result->meta->fields;

		result->unbuf->row_count++;

		retrow = mnd_malloc(result->field_count * sizeof(char *));

		mysqlnd_unbuffered_free_last_data(result TSRMLS_CC);

		result->unbuf->last_row_data = row_packet->fields;
		result->unbuf->last_row_buffer = row_packet->row_buffer;
		row_packet->fields = NULL;
		row_packet->row_buffer = NULL;

		MYSQLND_INC_CONN_STATISTIC(&result->conn->stats, STAT_ROWS_FETCHED_FROM_CLIENT_NORMAL_UNBUF);

		for (i = 0; i < field_count; i++, field++) {
			zval *data = result->unbuf->last_row_data[i];
			int len;

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
	} else if (ret == FAIL) {
		if (row_packet->error_info.error_no) {
			result->conn->error_info = row_packet->error_info;
			DBG_ERR_FMT("errorno=%d error=%s", row_packet->error_info.error_no, row_packet->error_info.error); 
		}
		result->conn->state = CONN_READY;
		result->unbuf->eof_reached = TRUE; /* so next time we won't get an error */
	} else if (row_packet->eof) {
		/* Mark the connection as usable again */
		DBG_INF_FMT("warns=%u status=%u", row_packet->warning_count, row_packet->server_status);
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
		mysqlnd_unbuffered_free_last_data(result TSRMLS_CC);
	}

	DBG_RETURN(retrow);
}
/* }}} */


/* {{{ mysqlnd_fetch_row_unbuffered */
static enum_func_status
mysqlnd_fetch_row_unbuffered(MYSQLND_RES *result, void *param, unsigned int flags,
							zend_bool *fetched_anything TSRMLS_DC)
{
	enum_func_status		ret;
	zval 					*row = (zval *) param;
	unsigned int			i,
							field_count = result->field_count;
	php_mysql_packet_row	*row_packet = result->row_packet;
	unsigned long			*lengths = result->lengths;

	DBG_ENTER("mysqlnd_fetch_row_unbuffered");
	DBG_INF_FMT("flags=%d", flags);

	if (result->unbuf->eof_reached) {
		/* No more rows obviously */
		*fetched_anything = FALSE;
		DBG_RETURN(PASS);
	}
	if (result->conn->state != CONN_FETCHING_DATA) {
		SET_CLIENT_ERROR(result->conn->error_info, CR_COMMANDS_OUT_OF_SYNC,
						 UNKNOWN_SQLSTATE, mysqlnd_out_of_sync); 
		DBG_RETURN(FAIL);
	}
	/* Let the row packet fill our buffer and skip additional mnd_malloc + memcpy */
	row_packet->skip_extraction = row? FALSE:TRUE;

	/*
	  If we skip rows (row == NULL) we have to
	  mysqlnd_unbuffered_free_last_data() before it. The function returns always true.
	*/
	if (PASS == (ret = PACKET_READ(row_packet, result->conn)) && !row_packet->eof) {
		result->unbuf->row_count++;
		*fetched_anything = TRUE;

		mysqlnd_unbuffered_free_last_data(result TSRMLS_CC);

		result->unbuf->last_row_data = row_packet->fields;
		result->unbuf->last_row_buffer = row_packet->row_buffer;
		row_packet->fields = NULL;
		row_packet->row_buffer = NULL;

		MYSQLND_INC_CONN_STATISTIC(&result->conn->stats, STAT_ROWS_FETCHED_FROM_CLIENT_NORMAL_UNBUF);

		if (!row_packet->skip_extraction) {
			HashTable *row_ht = Z_ARRVAL_P(row);
			MYSQLND_FIELD *field = result->meta->fields;
			struct mysqlnd_field_hash_key *zend_hash_key = result->meta->zend_hash_keys;

			for (i = 0; i < field_count; i++, field++, zend_hash_key++) {
				zval *data = result->unbuf->last_row_data[i];
				int len = (Z_TYPE_P(data) == IS_NULL)? 0:Z_STRLEN_P(data);

				if (lengths) {
					lengths[i] = len;
				}

				/* Forbid ZE to free it, we will clean it */
				Z_ADDREF_P(data);

				if ((flags & MYSQLND_FETCH_BOTH) == MYSQLND_FETCH_BOTH) {
					Z_ADDREF_P(data);
				}
				if (flags & MYSQLND_FETCH_NUM) {
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
					if (zend_hash_key->is_numeric == FALSE) {
#if PHP_MAJOR_VERSION >= 6
						if (UG(unicode)) {
							zend_u_hash_quick_update(Z_ARRVAL_P(row), IS_UNICODE,
													 zend_hash_key->ustr,
													 zend_hash_key->ulen + 1,
													 zend_hash_key->key,
													 (void *) &data, sizeof(zval *), NULL);
						} else
#endif
						{
							zend_hash_quick_update(Z_ARRVAL_P(row),
												   field->name,
												   field->name_length + 1,
												   zend_hash_key->key,
												   (void *) &data, sizeof(zval *), NULL);
						}
					} else {
						zend_hash_index_update(Z_ARRVAL_P(row),
											   zend_hash_key->key,
											   (void *) &data, sizeof(zval *), NULL);
					}
				}
				if (field->max_length < len) {
					field->max_length = len;
				}
			}
		}
	} else if (ret == FAIL) {
		if (row_packet->error_info.error_no) {
			result->conn->error_info = row_packet->error_info;
			DBG_ERR_FMT("errorno=%d error=%s", row_packet->error_info.error_no, row_packet->error_info.error); 
		}
		*fetched_anything = FALSE;
		result->conn->state = CONN_READY;
		result->unbuf->eof_reached = TRUE; /* so next time we won't get an error */
	} else if (row_packet->eof) {
		/* Mark the connection as usable again */
		DBG_INF_FMT("warns=%u status=%u", row_packet->warning_count, row_packet->server_status);
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
		mysqlnd_unbuffered_free_last_data(result TSRMLS_CC);
		*fetched_anything = FALSE;
	}

	DBG_INF_FMT("ret=%s fetched=%d", ret == PASS? "PASS":"FAIL", *fetched_anything);
	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ mysqlnd_res::use_result */
static MYSQLND_RES *
MYSQLND_METHOD(mysqlnd_res, use_result)(MYSQLND_RES * const result, zend_bool ps TSRMLS_DC)
{
	DBG_ENTER("mysqlnd_res::use_result");
	DBG_INF_FMT("ps=%d", ps);

	SET_EMPTY_ERROR(result->conn->error_info);

	if (ps == FALSE) {
		result->type			= MYSQLND_RES_NORMAL;
		result->m.fetch_row		= result->m.fetch_row_normal_unbuffered;
		result->m.fetch_lengths	= mysqlnd_fetch_lengths_unbuffered;
		result->lengths			= mnd_ecalloc(result->field_count, sizeof(unsigned long));
	} else {
		result->type			= MYSQLND_RES_PS_UNBUF;
		/* result->m.fetch_row() will be set in mysqlnd_ps.c */
		result->m.fetch_lengths	= NULL; /* makes no sense */
		result->lengths 		= NULL;
	}

	result->unbuf = mnd_ecalloc(1, sizeof(MYSQLND_RES_UNBUFFERED));

	/*
	  Will be freed in the mysqlnd_internal_free_result_contents() called
	  by the resource destructor. mysqlnd_fetch_row_unbuffered() expects
	  this to be not NULL.
	*/
	PACKET_INIT(result->row_packet, PROT_ROW_PACKET, php_mysql_packet_row *);
	result->row_packet->field_count = result->field_count;
	result->row_packet->binary_protocol = ps;
	result->row_packet->fields_metadata = result->meta->fields;
	result->row_packet->bit_fields_count = result->meta->bit_fields_count;
	result->row_packet->bit_fields_total_len = result->meta->bit_fields_total_len;

	DBG_RETURN(result);
}
/* }}} */


/* {{{ mysqlnd_fetch_row_buffered_c */
static MYSQLND_ROW_C
mysqlnd_fetch_row_buffered_c(MYSQLND_RES *result TSRMLS_DC)
{
	MYSQLND_ROW_C ret = NULL;
	unsigned int i;

	DBG_ENTER("mysqlnd_fetch_row_buffered_c");

	/* If we haven't read everything */
	if (result->data->data_cursor &&
		(result->data->data_cursor - result->data->data) < result->data->row_count)
	{
		zval **current_row = *result->data->data_cursor;

		ret = mnd_malloc(result->field_count * sizeof(char *));
		for (i = 0; i < result->field_count; i++) {
			zval *data = current_row[i];
			if (Z_TYPE_P(data) != IS_NULL) {
				convert_to_string(data);
				ret[i] = Z_STRVAL_P(data);
			} else {
				ret[i] = NULL;
			}
		}
		result->data->data_cursor++;
		MYSQLND_INC_GLOBAL_STATISTIC(STAT_ROWS_FETCHED_FROM_CLIENT_NORMAL_BUF);
	} else {
		result->data->data_cursor = NULL;
		DBG_INF("EOF reached");
	}
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_fetch_row_buffered */
static enum_func_status
mysqlnd_fetch_row_buffered(MYSQLND_RES *result, void *param, unsigned int flags,
						   zend_bool *fetched_anything TSRMLS_DC)
{
	unsigned int i;
	zval *row = (zval *) param;

	DBG_ENTER("mysqlnd_fetch_row_buffered");
	DBG_INF_FMT("flags=%u row=%p", flags, row);

	/* If we haven't read everything */
	if (result->data->data_cursor &&
		(result->data->data_cursor - result->data->data) < result->data->row_count)
	{
		zval **current_row = *result->data->data_cursor;
		MYSQLND_FIELD *field = result->meta->fields;
		struct mysqlnd_field_hash_key *zend_hash_key = result->meta->zend_hash_keys;

		for (i = 0; i < result->field_count; i++, field++, zend_hash_key++) {
			zval *data = current_row[i];

			/*
			  Let us later know what to do with this zval. If ref_count > 1, we will just
			  decrease it, otherwise free it. zval_ptr_dtor() make this very easy job.
			*/
			Z_ADDREF_P(data);
			
			if ((flags & MYSQLND_FETCH_BOTH) == MYSQLND_FETCH_BOTH) {
				Z_ADDREF_P(data);
			}
			if (flags & MYSQLND_FETCH_NUM) {
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
				if (zend_hash_key->is_numeric == FALSE) {
#if PHP_MAJOR_VERSION >= 6
					if (UG(unicode)) {
						zend_u_hash_quick_update(Z_ARRVAL_P(row), IS_UNICODE,
												 zend_hash_key->ustr,
												 zend_hash_key->ulen + 1,
												 zend_hash_key->key,
												 (void *) &data, sizeof(zval *), NULL);
					} else
#endif
					{
						zend_hash_quick_update(Z_ARRVAL_P(row),
											   field->name,
											   field->name_length + 1,
											   zend_hash_key->key,
											   (void *) &data, sizeof(zval *), NULL);
					}
				} else {
					zend_hash_index_update(Z_ARRVAL_P(row),
										   zend_hash_key->key,
										   (void *) &data, sizeof(zval *), NULL);
				}
			}
		}
		result->data->data_cursor++;
		*fetched_anything = TRUE;
		MYSQLND_INC_GLOBAL_STATISTIC(STAT_ROWS_FETCHED_FROM_CLIENT_NORMAL_BUF);
	} else {
		result->data->data_cursor = NULL;
		*fetched_anything = FALSE;
		DBG_INF("EOF reached");
	}
	DBG_INF_FMT("ret=PASS fetched=%d", *fetched_anything);
	DBG_RETURN(PASS);
}
/* }}} */


#define STORE_RESULT_PREALLOCATED_SET 32

/* {{{ mysqlnd_store_result_fetch_data */
enum_func_status
mysqlnd_store_result_fetch_data(MYSQLND * const conn, MYSQLND_RES *result,
								MYSQLND_RES_METADATA *meta,
								zend_bool binary_protocol,
								zend_bool update_max_length,
								zend_bool to_cache TSRMLS_DC)
{
	enum_func_status ret;
	php_mysql_packet_row row_packet;
	unsigned int next_extend = STORE_RESULT_PREALLOCATED_SET, free_rows;
	MYSQLND_RES_BUFFERED *set;

	DBG_ENTER("mysqlnd_store_result_fetch_data");
	DBG_INF_FMT("conn=%llu binary_proto=%d update_max_len=%d to_cache=%d",
				conn->thread_id, binary_protocol, update_max_length, to_cache);

	free_rows = next_extend;

	result->data	= set = mnd_pecalloc(1, sizeof(MYSQLND_RES_BUFFERED), to_cache);
	set->data		= mnd_pemalloc(STORE_RESULT_PREALLOCATED_SET * sizeof(zval **), to_cache);
	set->row_buffers= mnd_pemalloc(STORE_RESULT_PREALLOCATED_SET * sizeof(zend_uchar *), to_cache);
	set->persistent	= to_cache;
	set->qcache		= to_cache? mysqlnd_qcache_get_cache_reference(conn->qcache):NULL;
	set->references	= 1;

	PACKET_INIT_ALLOCA(row_packet, PROT_ROW_PACKET);
	row_packet.field_count = meta->field_count;
	row_packet.binary_protocol = binary_protocol;
	row_packet.fields_metadata = meta->fields;
	row_packet.bit_fields_count		= meta->bit_fields_count;
	row_packet.bit_fields_total_len = meta->bit_fields_total_len;
	/* Let the row packet fill our buffer and skip additional malloc + memcpy */
	while (FAIL != (ret = PACKET_READ_ALLOCA(row_packet, conn)) && !row_packet.eof) {
		unsigned int i;
		zval **current_row;

		if (!free_rows) {
			uint64 total_rows = free_rows = next_extend = next_extend * 5 / 3; /* extend with 33% */
			total_rows += set->row_count;
			set->data = mnd_perealloc(set->data, total_rows * sizeof(zval **), set->persistent);

			set->row_buffers = mnd_perealloc(set->row_buffers,
										 total_rows * sizeof(zend_uchar *), set->persistent);
		}
		free_rows--;
		current_row = set->data[set->row_count] = row_packet.fields;
		set->row_buffers[set->row_count] = row_packet.row_buffer;
		set->row_count++;

		/* So row_packet's destructor function won't efree() it */
		row_packet.fields = NULL;
		row_packet.row_buffer = NULL;


		if (update_max_length == TRUE) {
			for (i = 0; i < row_packet.field_count; i++) {
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
		/*
		  No need to FREE_ALLOCA as we can reuse the
		  'lengths' and 'fields' arrays. For lengths its absolutely safe.
		  'fields' is reused because the ownership of the strings has been
		  transfered above. 
		*/
	}
	MYSQLND_INC_CONN_STATISTIC_W_VALUE(&conn->stats,
									   binary_protocol? STAT_ROWS_BUFFERED_FROM_CLIENT_PS:
														STAT_ROWS_BUFFERED_FROM_CLIENT_NORMAL,
									   set->row_count);

	/* Finally clean */
	if (row_packet.eof) { 
		conn->upsert_status.warning_count = row_packet.warning_count;
		conn->upsert_status.server_status = row_packet.server_status;
	}
	/* save some memory */
	if (free_rows) {
		set->data = mnd_perealloc(set->data,
							  (size_t) set->row_count * sizeof(zval **),
							  set->persistent);
		set->row_buffers = mnd_perealloc(set->row_buffers,
									 (size_t) set->row_count * sizeof(zend_uchar *),
									 set->persistent);
	}

	if (conn->upsert_status.server_status & SERVER_MORE_RESULTS_EXISTS) {
		conn->state = CONN_NEXT_RESULT_PENDING;
	} else {
		conn->state = CONN_READY;
	}

	if (ret == FAIL) {
		set->error_info = row_packet.error_info;
	} else {
		/* Position at the first row */
		set->data_cursor = set->data;	

		/* libmysql's documentation says it should be so for SELECT statements */
		conn->upsert_status.affected_rows = result->data->row_count;
	}
	PACKET_FREE_ALLOCA(row_packet);

	DBG_INF_FMT("ret=%s row_count=%u warns=%u status=%u", ret == PASS? "PASS":"FAIL",
				set->row_count, conn->upsert_status.warning_count, conn->upsert_status.server_status);
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_res::store_result */
static MYSQLND_RES *
MYSQLND_METHOD(mysqlnd_res, store_result)(MYSQLND_RES * result,
										  MYSQLND * const conn,
										  zend_bool ps_protocol TSRMLS_DC)
{
	enum_func_status ret;
	zend_bool to_cache = FALSE;

	DBG_ENTER("mysqlnd_res::store_result");
	DBG_INF_FMT("conn=%d ps_protocol=%d", conn->thread_id, ps_protocol);

	result->conn			= NULL;	/* store result does not reference  the connection */
	result->type			= MYSQLND_RES_NORMAL;
	result->m.fetch_row		= result->m.fetch_row_normal_buffered;
	result->m.fetch_lengths	= mysqlnd_fetch_lengths_buffered;

	conn->state = CONN_FETCHING_DATA;

	result->lengths = mnd_ecalloc(result->field_count, sizeof(unsigned long));

	ret = mysqlnd_store_result_fetch_data(conn, result, result->meta,
										  ps_protocol, TRUE, to_cache TSRMLS_CC);
	if (PASS == ret) {
		/* libmysql's documentation says it should be so for SELECT statements */
		conn->upsert_status.affected_rows = result->data->row_count;
	} else {
		conn->error_info = result->data->error_info;
		result->m.free_result_internal(result TSRMLS_CC);
		result = NULL;
	}

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
	if (!result->data && result->conn && result->unbuf &&
		!result->unbuf->eof_reached && result->m.fetch_row)
	{
		DBG_INF("skipping result");
		/* We have to fetch all data to clean the line */
		MYSQLND_INC_CONN_STATISTIC(&result->conn->stats,
									result->type == MYSQLND_RES_NORMAL? STAT_FLUSHED_NORMAL_SETS:
																		STAT_FLUSHED_PS_SETS);

		while ((PASS == result->m.fetch_row(result, NULL, 0, &fetched_anything TSRMLS_CC)) &&
			   fetched_anything == TRUE)
		{
			/* do nothing */;
		}
	}
	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ mysqlnd_res::free_result */
static enum_func_status
MYSQLND_METHOD(mysqlnd_res, free_result)(MYSQLND_RES *result, zend_bool implicit TSRMLS_DC)
{
	DBG_ENTER("mysqlnd_res::free_result");
	DBG_INF_FMT("implicit=%d", implicit);

	result->m.skip_result(result TSRMLS_CC);
	MYSQLND_INC_CONN_STATISTIC(result->conn? &result->conn->stats : NULL,
							   implicit == TRUE?	STAT_FREE_RESULT_IMPLICIT:
							   						STAT_FREE_RESULT_EXPLICIT);

	result->m.free_result_internal(result TSRMLS_CC);
	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ mysqlnd_res::data_seek */
static enum_func_status
MYSQLND_METHOD(mysqlnd_res, data_seek)(MYSQLND_RES *result, uint64 row TSRMLS_DC)
{
	DBG_ENTER("mysqlnd_res::data_seek");
	DBG_INF_FMT("row=%lu", row);

	if (!result->data) {
		return FAIL;
	}

	/* libmysql just moves to the end, it does traversing of a linked list */
	if (row >= result->data->row_count) {
		result->data->data_cursor = NULL;
	} else {
		result->data->data_cursor = result->data->data + row;
	}

	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ mysqlnd_res::num_fields */
static uint64
MYSQLND_METHOD(mysqlnd_res, num_rows)(const MYSQLND_RES * const res)
{
	/* Be compatible with libmysql. We count row_count, but will return 0 */
	return res->data? res->data->row_count:0;
}
/* }}} */


/* {{{ mysqlnd_res::num_fields */
static unsigned int
MYSQLND_METHOD(mysqlnd_res, num_fields)(const MYSQLND_RES * const res)
{
	return res->field_count;
}
/* }}} */


/* {{{ mysqlnd_res::fetch_field */
static MYSQLND_FIELD *
MYSQLND_METHOD(mysqlnd_res, fetch_field)(MYSQLND_RES * const result TSRMLS_DC)
{
	DBG_ENTER("mysqlnd_res::fetch_field");
	DBG_RETURN(result->meta? result->meta->m->fetch_field(result->meta TSRMLS_CC):NULL);
}
/* }}} */


/* {{{ mysqlnd_res::fetch_field_direct */
static MYSQLND_FIELD *
MYSQLND_METHOD(mysqlnd_res, fetch_field_direct)(const MYSQLND_RES * const result,
												MYSQLND_FIELD_OFFSET fieldnr TSRMLS_DC)
{
	DBG_ENTER("mysqlnd_res::fetch_field_direct");
	DBG_RETURN(result->meta? result->meta->m->fetch_field_direct(result->meta, fieldnr TSRMLS_CC):NULL);
}
/* }}} */


/* {{{ mysqlnd_res::field_seek */
static MYSQLND_FIELD_OFFSET
MYSQLND_METHOD(mysqlnd_res, field_seek)(MYSQLND_RES * const result,
										MYSQLND_FIELD_OFFSET field_offset)
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
MYSQLND_METHOD(mysqlnd_res, field_tell)(const MYSQLND_RES * const result)
{
	return result->meta? result->meta->m->field_tell(result->meta):0;
}
/* }}} */


/* {{{ mysqlnd_res::fetch_into */
static void
MYSQLND_METHOD(mysqlnd_res, fetch_into)(MYSQLND_RES *result, unsigned int flags,
										zval *return_value,
										enum_mysqlnd_extension extension TSRMLS_DC ZEND_FILE_LINE_DC)
{
	zend_bool fetched_anything;

	DBG_ENTER("mysqlnd_res::fetch_into");
	DBG_INF_FMT("flags=%u mysqlnd_extension=%d", flags, extension);

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
MYSQLND_METHOD(mysqlnd_res, fetch_row_c)(MYSQLND_RES *result TSRMLS_DC)
{
	MYSQLND_ROW_C ret = NULL;
	DBG_ENTER("mysqlnd_res::fetch_row_c");

	if (result->m.fetch_row) {
		if (result->m.fetch_row == result->m.fetch_row_normal_buffered) {
			return mysqlnd_fetch_row_buffered_c(result TSRMLS_CC);
		} else if (result->m.fetch_row == result->m.fetch_row_normal_unbuffered) {
			return mysqlnd_fetch_row_unbuffered_c(result TSRMLS_CC);
		} else {
			*((int*)NULL) = 1;
		}
	}
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_res::fetch_all */
static void
MYSQLND_METHOD(mysqlnd_res, fetch_all)(MYSQLND_RES *result, unsigned int flags,
									   zval *return_value TSRMLS_DC ZEND_FILE_LINE_DC)
{
	zval  *row;
	ulong i = 0;

	DBG_ENTER("mysqlnd_res::fetch_all");
	DBG_INF_FMT("flags=%u", flags);

	/* mysqlnd_res::fetch_all works with buffered resultsets only */
	if (result->conn || !result->data ||
		!result->data->row_count || !result->data->data_cursor ||
		result->data->data_cursor >= result->data->data + result->data->row_count)
	{
		RETVAL_NULL();
		DBG_VOID_RETURN;
	}	

	mysqlnd_array_init(return_value, (uint) result->data->row_count);

	while (result->data->data_cursor &&
		   (result->data->data_cursor - result->data->data) < result->data->row_count)
	{
		MAKE_STD_ZVAL(row);
		mysqlnd_fetch_into(result, flags, row, MYSQLND_MYSQLI);
		add_index_zval(return_value, i++, row);
	}

	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_res::fetch_into */
static void
MYSQLND_METHOD(mysqlnd_res, fetch_field_data)(MYSQLND_RES *result, unsigned int offset,
											  zval *return_value TSRMLS_DC)
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


/* {{{ mysqlnd_result_init */
MYSQLND_RES *mysqlnd_result_init(unsigned int field_count, MYSQLND_THD_ZVAL_PCACHE *cache TSRMLS_DC)
{
	MYSQLND_RES *ret = mnd_ecalloc(1, sizeof(MYSQLND_RES));

	DBG_ENTER("mysqlnd_result_init");
	DBG_INF_FMT("field_count=%u cache=%p", field_count, cache);

	ret->field_count	= field_count;
	ret->zval_cache		= cache;

	ret->m.use_result	= MYSQLND_METHOD(mysqlnd_res, use_result);
	ret->m.store_result	= MYSQLND_METHOD(mysqlnd_res, store_result);
	ret->m.free_result	= MYSQLND_METHOD(mysqlnd_res, free_result);
	ret->m.seek_data	= MYSQLND_METHOD(mysqlnd_res, data_seek);
	ret->m.num_rows		= MYSQLND_METHOD(mysqlnd_res, num_rows);
	ret->m.num_fields	= MYSQLND_METHOD(mysqlnd_res, num_fields);
	ret->m.fetch_into	= MYSQLND_METHOD(mysqlnd_res, fetch_into);
	ret->m.fetch_row_c	= MYSQLND_METHOD(mysqlnd_res, fetch_row_c);
	ret->m.fetch_all	= MYSQLND_METHOD(mysqlnd_res, fetch_all);
	ret->m.fetch_field_data	= MYSQLND_METHOD(mysqlnd_res, fetch_field_data);
	ret->m.seek_field	= MYSQLND_METHOD(mysqlnd_res, field_seek);
	ret->m.field_tell	= MYSQLND_METHOD(mysqlnd_res, field_tell);
	ret->m.fetch_field	= MYSQLND_METHOD(mysqlnd_res, fetch_field);
	ret->m.fetch_field_direct = MYSQLND_METHOD(mysqlnd_res, fetch_field_direct);

	ret->m.skip_result	= MYSQLND_METHOD(mysqlnd_res, skip_result);
	ret->m.free_result_buffers	= MYSQLND_METHOD(mysqlnd_res, free_result_buffers);
	ret->m.free_result_internal = mysqlnd_internal_free_result;
	ret->m.free_result_contents = mysqlnd_internal_free_result_contents;

	ret->m.read_result_metadata = MYSQLND_METHOD(mysqlnd_res, read_result_metadata);
	ret->m.fetch_row_normal_buffered	= mysqlnd_fetch_row_buffered;
	ret->m.fetch_row_normal_unbuffered	= mysqlnd_fetch_row_unbuffered;

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
