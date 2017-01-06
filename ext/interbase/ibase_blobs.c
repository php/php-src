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

#define BLOB_CLOSE		1
#define BLOB_CANCEL		2

static int le_blob;

static void _php_ibase_free_blob(zend_resource *rsrc) /* {{{ */
{
	ibase_blob *ib_blob = (ibase_blob *)rsrc->ptr;

	if (ib_blob->bl_handle != 0) { /* blob open*/
		if (isc_cancel_blob(IB_STATUS, &ib_blob->bl_handle)) {
			_php_ibase_module_error("You can lose data. Close any blob after reading from or "
				"writing to it. Use ibase_blob_close() before calling ibase_close()");
		}
	}
	efree(ib_blob);
}
/* }}} */

void php_ibase_blobs_minit(INIT_FUNC_ARGS) /* {{{ */
{
	le_blob = zend_register_list_destructors_ex(_php_ibase_free_blob, NULL,
	    "interbase blob", module_number);
}
/* }}} */

int _php_ibase_string_to_quad(char const *id, ISC_QUAD *qd) /* {{{ */
{
	/* shortcut for most common case */
	if (sizeof(ISC_QUAD) == sizeof(ISC_UINT64)) {
		return sscanf(id, BLOB_ID_MASK, (ISC_UINT64 *) qd);
	} else {
		ISC_UINT64 res;
		if (sscanf(id, BLOB_ID_MASK, &res)) {
			qd->gds_quad_high = (ISC_LONG) (res >> 0x20);
			qd->gds_quad_low = (ISC_LONG) (res & 0xFFFFFFFF);
			return 1;
		}
		return 0;
	}
}
/* }}} */

zend_string *_php_ibase_quad_to_string(ISC_QUAD const qd) /* {{{ */
{
	/* shortcut for most common case */
	if (sizeof(ISC_QUAD) == sizeof(ISC_UINT64)) {
		return strpprintf(BLOB_ID_LEN+1, "0x%0*" LL_MASK "x", 16, *(ISC_UINT64*)(void *) &qd);
	} else {
		ISC_UINT64 res = ((ISC_UINT64) qd.gds_quad_high << 0x20) | qd.gds_quad_low;
		return strpprintf(BLOB_ID_LEN+1, "0x%0*" LL_MASK "x", 16, res);
	}
}
/* }}} */

typedef struct { /* {{{ */
	ISC_LONG  max_segment;		/* Length of longest segment */
	ISC_LONG  num_segments;		/* Total number of segments */
	ISC_LONG  total_length;		/* Total length of blob */
	int		  bl_stream;		/* blob is stream ? */
/* }}} */
} IBASE_BLOBINFO;

int _php_ibase_blob_get(zval *return_value, ibase_blob *ib_blob, zend_ulong max_len) /* {{{ */
{
	if (ib_blob->bl_qd.gds_quad_high || ib_blob->bl_qd.gds_quad_low) { /*not null ?*/

		ISC_STATUS stat;
		zend_string *bl_data;
		zend_ulong cur_len;
		unsigned short seg_len;

		bl_data = zend_string_safe_alloc(1, max_len, 0, 0);

		for (cur_len = stat = 0; (stat == 0 || stat == isc_segment) && cur_len < max_len; cur_len += seg_len) {

			unsigned short chunk_size = (max_len-cur_len) > USHRT_MAX ? USHRT_MAX
				: (unsigned short)(max_len-cur_len);

			stat = isc_get_segment(IB_STATUS, &ib_blob->bl_handle, &seg_len, chunk_size, &ZSTR_VAL(bl_data)[cur_len]);
		}

		if (IB_STATUS[0] == 1 && (stat != 0 && stat != isc_segstr_eof && stat != isc_segment)) {
			zend_string_free(bl_data);
			_php_ibase_error();
			return FAILURE;
		}
		ZSTR_VAL(bl_data)[cur_len] = '\0';
		ZSTR_LEN(bl_data) = cur_len;
		RETVAL_NEW_STR(bl_data);
	} else { /* null blob */
		RETVAL_EMPTY_STRING(); /* empty string */
	}
	return SUCCESS;
}
/* }}} */

int _php_ibase_blob_add(zval *string_arg, ibase_blob *ib_blob) /* {{{ */
{
	zend_ulong put_cnt = 0, rem_cnt;
	unsigned short chunk_size;

	convert_to_string_ex(string_arg);

	for (rem_cnt = Z_STRLEN_P(string_arg); rem_cnt > 0; rem_cnt -= chunk_size)  {

		chunk_size = rem_cnt > USHRT_MAX ? USHRT_MAX : (unsigned short)rem_cnt;

		if (isc_put_segment(IB_STATUS, &ib_blob->bl_handle, chunk_size, &Z_STRVAL_P(string_arg)[put_cnt] )) {
			_php_ibase_error();
			return FAILURE;
		}
		put_cnt += chunk_size;
	}
	return SUCCESS;
}
/* }}} */

static int _php_ibase_blob_info(isc_blob_handle bl_handle, IBASE_BLOBINFO *bl_info) /* {{{ */
{
	static char bl_items[] = {
		isc_info_blob_num_segments,
		isc_info_blob_max_segment,
		isc_info_blob_total_length,
		isc_info_blob_type
	};

	char bl_inf[sizeof(zend_long)*8], *p;

	bl_info->max_segment = 0;
	bl_info->num_segments = 0;
	bl_info->total_length = 0;
	bl_info->bl_stream = 0;

	if (isc_blob_info(IB_STATUS, &bl_handle, sizeof(bl_items), bl_items, sizeof(bl_inf), bl_inf)) {
		_php_ibase_error();
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
				_php_ibase_module_error("PHP module internal error");
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
	zval *link = NULL;
	ibase_db_link *ib_link;
	ibase_trans *trans = NULL;
	ibase_blob *ib_blob;

	RESET_ERRMSG;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(), "|r", &link)) {
		RETURN_FALSE;
	}

	PHP_IBASE_LINK_TRANS(link, ib_link, trans);

	ib_blob = (ibase_blob *) emalloc(sizeof(ibase_blob));
	ib_blob->bl_handle = 0;
	ib_blob->type = BLOB_INPUT;

	if (isc_create_blob(IB_STATUS, &ib_link->handle, &trans->handle, &ib_blob->bl_handle, &ib_blob->bl_qd)) {
		_php_ibase_error();
		efree(ib_blob);
		RETURN_FALSE;
	}

	RETVAL_RES(zend_register_resource(ib_blob, le_blob));
	Z_TRY_ADDREF_P(return_value);
}
/* }}} */

/* {{{ proto resource ibase_blob_open([ resource link_identifier, ] string blob_id)
   Open blob for retrieving data parts */
PHP_FUNCTION(ibase_blob_open)
{
	char *blob_id;
	size_t blob_id_len;
	zval *link = NULL;
	ibase_db_link *ib_link;
	ibase_trans *trans = NULL;
	ibase_blob *ib_blob;

	RESET_ERRMSG;

	switch (ZEND_NUM_ARGS()) {
		default:
			WRONG_PARAM_COUNT;
		case 1:
			if (FAILURE == zend_parse_parameters(1, "s", &blob_id, &blob_id_len)) {
				RETURN_FALSE;
			}
			break;
		case 2:
			if (FAILURE == zend_parse_parameters(2, "rs", &link, &blob_id, &blob_id_len)) {
				RETURN_FALSE;
			}
			break;
	}

	PHP_IBASE_LINK_TRANS(link, ib_link, trans);

	ib_blob = (ibase_blob *) emalloc(sizeof(ibase_blob));
	ib_blob->bl_handle = 0;
	ib_blob->type = BLOB_OUTPUT;

	do {
		if (! _php_ibase_string_to_quad(blob_id, &ib_blob->bl_qd)) {
			_php_ibase_module_error("String is not a BLOB ID");
			break;
		}

		if (isc_open_blob(IB_STATUS, &ib_link->handle, &trans->handle, &ib_blob->bl_handle,
				&ib_blob->bl_qd)) {
			_php_ibase_error();
			break;
		}

		RETVAL_RES(zend_register_resource(ib_blob, le_blob));
		Z_TRY_ADDREF_P(return_value);
		return;

	} while (0);

	efree(ib_blob);
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool ibase_blob_add(resource blob_handle, string data)
   Add data into created blob */
PHP_FUNCTION(ibase_blob_add)
{
	zval *blob_arg, *string_arg;
	ibase_blob *ib_blob;

	RESET_ERRMSG;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(), "rz", &blob_arg, &string_arg)) {
		return;
	}

	ib_blob = (ibase_blob *)zend_fetch_resource_ex(blob_arg, "Interbase blob", le_blob);

	if (ib_blob->type != BLOB_INPUT) {
		_php_ibase_module_error("BLOB is not open for input");
		RETURN_FALSE;
	}

	if (_php_ibase_blob_add(string_arg, ib_blob) != SUCCESS) {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto string ibase_blob_get(resource blob_handle, int len)
   Get len bytes data from open blob */
PHP_FUNCTION(ibase_blob_get)
{
	zval *blob_arg;
	zend_ulong len_arg;
	ibase_blob *ib_blob;

	RESET_ERRMSG;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(), "rl", &blob_arg, &len_arg)) {
		return;
	}

	ib_blob = (ibase_blob *)zend_fetch_resource_ex(blob_arg, "Interbase blob", le_blob);

	if (ib_blob->type != BLOB_OUTPUT) {
		_php_ibase_module_error("BLOB is not open for output");
		RETURN_FALSE;
	}

	if (_php_ibase_blob_get(return_value, ib_blob, len_arg) != SUCCESS) {
		RETURN_FALSE;
	}
}
/* }}} */

static void _php_ibase_blob_end(INTERNAL_FUNCTION_PARAMETERS, int bl_end) /* {{{ */
{
	zval *blob_arg;
	ibase_blob *ib_blob;

	RESET_ERRMSG;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(), "r", &blob_arg)) {
		return;
	}

	ib_blob = (ibase_blob *)zend_fetch_resource_ex(blob_arg, "Interbase blob", le_blob);

	if (bl_end == BLOB_CLOSE) { /* return id here */

		if (ib_blob->bl_qd.gds_quad_high || ib_blob->bl_qd.gds_quad_low) { /*not null ?*/
			if (isc_close_blob(IB_STATUS, &ib_blob->bl_handle)) {
				_php_ibase_error();
				RETURN_FALSE;
			}
		}
		ib_blob->bl_handle = 0;

		RETVAL_NEW_STR(_php_ibase_quad_to_string(ib_blob->bl_qd));
	} else { /* discard created blob */
		if (isc_cancel_blob(IB_STATUS, &ib_blob->bl_handle)) {
			_php_ibase_error();
			RETURN_FALSE;
		}
		ib_blob->bl_handle = 0;
		RETVAL_TRUE;
	}
	zend_list_delete(Z_RES_P(blob_arg));
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
	char *blob_id;
	size_t blob_id_len;
	zval *link = NULL;
	ibase_db_link *ib_link;
	ibase_trans *trans = NULL;
	ibase_blob ib_blob = { 0, BLOB_INPUT };
	IBASE_BLOBINFO bl_info;

	RESET_ERRMSG;

	switch (ZEND_NUM_ARGS()) {
		default:
			WRONG_PARAM_COUNT;
		case 1:
			if (FAILURE == zend_parse_parameters(1, "s", &blob_id, &blob_id_len)) {
				RETURN_FALSE;
			}
			break;
		case 2:
			if (FAILURE == zend_parse_parameters(2, "rs", &link, &blob_id, &blob_id_len)) {
				RETURN_FALSE;
			}
			break;
	}

	PHP_IBASE_LINK_TRANS(link, ib_link, trans);

	if (! _php_ibase_string_to_quad(blob_id, &ib_blob.bl_qd)) {
		_php_ibase_module_error("Unrecognized BLOB ID");
		RETURN_FALSE;
	}

	if (ib_blob.bl_qd.gds_quad_high || ib_blob.bl_qd.gds_quad_low) { /* not null ? */
		if (isc_open_blob(IB_STATUS, &ib_link->handle, &trans->handle, &ib_blob.bl_handle,
				&ib_blob.bl_qd)) {
			_php_ibase_error();
			RETURN_FALSE;
		}

		if (_php_ibase_blob_info(ib_blob.bl_handle, &bl_info)) {
			RETURN_FALSE;
		}
		if (isc_close_blob(IB_STATUS, &ib_blob.bl_handle)) {
			_php_ibase_error();
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
	char *blob_id;
	size_t blob_id_len;
	zval *link = NULL;
	ibase_db_link *ib_link;
	ibase_trans *trans = NULL;
	ibase_blob ib_blob_id = { 0, BLOB_OUTPUT  };
	char bl_data[IBASE_BLOB_SEG];
	unsigned short seg_len;

	RESET_ERRMSG;

	switch (ZEND_NUM_ARGS()) {
		default:
			WRONG_PARAM_COUNT;
		case 1:
			if (FAILURE == zend_parse_parameters(1, "s", &blob_id, &blob_id_len)) {
				RETURN_FALSE;
			}
			break;
		case 2:
			if (FAILURE == zend_parse_parameters(2, "rs", &link, &blob_id, &blob_id_len)) {
				RETURN_FALSE;
			}
			break;
	}

	PHP_IBASE_LINK_TRANS(link, ib_link, trans);

	if (! _php_ibase_string_to_quad(blob_id, &ib_blob_id.bl_qd)) {
		_php_ibase_module_error("Unrecognized BLOB ID");
		RETURN_FALSE;
	}

	do {
		if (isc_open_blob(IB_STATUS, &ib_link->handle, &trans->handle, &ib_blob_id.bl_handle,
				&ib_blob_id.bl_qd)) {
			break;
		}

		while (!isc_get_segment(IB_STATUS, &ib_blob_id.bl_handle, &seg_len, sizeof(bl_data), bl_data)
				|| IB_STATUS[1] == isc_segment) {
			PHPWRITE(bl_data, seg_len);
		}

		if (IB_STATUS[0] && (IB_STATUS[1] != isc_segstr_eof)) {
			break;
		}

		if (isc_close_blob(IB_STATUS, &ib_blob_id.bl_handle)) {
			break;
		}
		RETURN_TRUE;
	} while (0);

	_php_ibase_error();
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto string ibase_blob_import([ resource link_identifier, ] resource file)
   Create blob, copy file in it, and close it */
PHP_FUNCTION(ibase_blob_import)
{
	zval *link = NULL, *file;
	int size;
	unsigned short b;
	ibase_blob ib_blob = { 0, 0 };
	ibase_db_link *ib_link;
	ibase_trans *trans = NULL;
	char bl_data[IBASE_BLOB_SEG];
	php_stream *stream;

	RESET_ERRMSG;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(), "r|r",
			(ZEND_NUM_ARGS()-1) ? &link : &file, &file)) {
		RETURN_FALSE;
	}

	PHP_IBASE_LINK_TRANS(link, ib_link, trans);

	php_stream_from_zval(stream, file);

	do {
		if (isc_create_blob(IB_STATUS, &ib_link->handle, &trans->handle, &ib_blob.bl_handle,
				&ib_blob.bl_qd)) {
			break;
		}

		for (size = 0; (b = php_stream_read(stream, bl_data, sizeof(bl_data))); size += b) {
			if (isc_put_segment(IB_STATUS, &ib_blob.bl_handle, b, bl_data)) {
				break;
			}
		}

		if (isc_close_blob(IB_STATUS, &ib_blob.bl_handle)) {
			break;
		}
		RETURN_NEW_STR(_php_ibase_quad_to_string(ib_blob.bl_qd));
	} while (0);

	_php_ibase_error();
	RETURN_FALSE;
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
