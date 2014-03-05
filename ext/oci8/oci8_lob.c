/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2014 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Stig Sæther Bakken <ssb@php.net>                            |
   |          Thies C. Arntzen <thies@thieso.net>                         |
   |                                                                      |
   | Collection support by Andy Sautins <asautins@veripost.net>           |
   | Temporary LOB support by David Benson <dbenson@mancala.com>          |
   | ZTS per process OCIPLogon by Harald Radi <harald.radi@nme.at>        |
   |                                                                      |
   | Redesigned by: Antony Dovgal <antony@zend.com>                       |
   |                Andi Gutmans <andi@zend.com>                          |
   |                Wez Furlong <wez@omniti.com>                          |
   +----------------------------------------------------------------------+
*/

/* $Id$ */



#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "php_ini.h"

#if HAVE_OCI8

#include "php_oci8.h"
#include "php_oci8_int.h"

/* for import/export functions */
#include <fcntl.h>

#ifndef O_BINARY
#define O_BINARY 0
#endif

/* {{{ php_oci_lob_create()
 Create LOB descriptor and allocate all the resources needed */
php_oci_descriptor *php_oci_lob_create (php_oci_connection *connection, long type TSRMLS_DC)
{
	php_oci_descriptor *descriptor;

	switch (type) {
		case OCI_DTYPE_FILE:
		case OCI_DTYPE_LOB:
		case OCI_DTYPE_ROWID:
			/* these three are allowed */
			break;
		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown descriptor type %ld", type);
			return NULL;
			break;
	}

	descriptor = ecalloc(1, sizeof(php_oci_descriptor));
	descriptor->type = type;
	descriptor->connection = connection;
	zend_list_addref(descriptor->connection->rsrc_id);

	PHP_OCI_CALL_RETURN(OCI_G(errcode), OCIDescriptorAlloc, (connection->env, (dvoid*)&(descriptor->descriptor), descriptor->type, (size_t) 0, (dvoid **) 0));

	if (OCI_G(errcode) != OCI_SUCCESS) {
		OCI_G(errcode) = php_oci_error(OCI_G(err), OCI_G(errcode) TSRMLS_CC);
		PHP_OCI_HANDLE_ERROR(connection, OCI_G(errcode));
		efree(descriptor);
		return NULL;
	}

	PHP_OCI_REGISTER_RESOURCE(descriptor, le_descriptor);
	
	descriptor->lob_current_position = 0;
	descriptor->lob_size = -1;				/* we should set it to -1 to know, that it's just not initialized */
	descriptor->buffering = PHP_OCI_LOB_BUFFER_DISABLED;				/* buffering is off by default */
	descriptor->charset_form = SQLCS_IMPLICIT;	/* default value */
	descriptor->charset_id = connection->charset;
	descriptor->is_open = 0;

	if (descriptor->type == OCI_DTYPE_LOB || descriptor->type == OCI_DTYPE_FILE) {
		/* add Lobs & Files to hash. we'll flush them at the end */
		if (!connection->descriptors) {
			ALLOC_HASHTABLE(connection->descriptors);
			zend_hash_init(connection->descriptors, 0, NULL, php_oci_descriptor_flush_hash_dtor, 0);
			connection->descriptor_count = 0;
		}
		
		descriptor->index = (connection->descriptor_count)++;
		if (connection->descriptor_count == LONG_MAX) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Internal descriptor counter has reached limit");
			php_oci_connection_descriptors_free(connection TSRMLS_CC);
			return NULL;
		}

		zend_hash_index_update(connection->descriptors,descriptor->index,&descriptor,sizeof(php_oci_descriptor *),NULL);
	}
	return descriptor;

} /* }}} */

/* {{{ php_oci_lob_get_length()
 Get length of the LOB. The length is cached so we don't need to ask Oracle every time */
int php_oci_lob_get_length (php_oci_descriptor *descriptor, ub4 *length TSRMLS_DC)
{
	php_oci_connection *connection = descriptor->connection;

	*length = 0;
	
	if (descriptor->lob_size >= 0) {
		*length = descriptor->lob_size;
		return 0;
	} else {
		if (descriptor->type == OCI_DTYPE_FILE) {
			PHP_OCI_CALL_RETURN(connection->errcode, OCILobFileOpen, (connection->svc, connection->err, descriptor->descriptor, OCI_FILE_READONLY));
			if (connection->errcode != OCI_SUCCESS) {
				connection->errcode = php_oci_error(connection->err, connection->errcode TSRMLS_CC);
				PHP_OCI_HANDLE_ERROR(connection, connection->errcode);
				return 1;
			}
		}
		
		PHP_OCI_CALL_RETURN(connection->errcode, OCILobGetLength, (connection->svc, connection->err, descriptor->descriptor, (ub4 *)length));

		if (connection->errcode != OCI_SUCCESS) {
			connection->errcode = php_oci_error(connection->err, connection->errcode TSRMLS_CC);
			PHP_OCI_HANDLE_ERROR(connection, connection->errcode);
			return 1;
		}

		descriptor->lob_size = *length;

		if (descriptor->type == OCI_DTYPE_FILE) {
			PHP_OCI_CALL_RETURN(connection->errcode, OCILobFileClose, (connection->svc, connection->err, descriptor->descriptor));

			if (connection->errcode != OCI_SUCCESS) {
				connection->errcode = php_oci_error(connection->err, connection->errcode TSRMLS_CC);
				PHP_OCI_HANDLE_ERROR(connection, connection->errcode);
				return 1;
			}
		}
	}
	return 0;	
} /* }}} */

/* {{{ php_oci_lob_callback()
   Append LOB portion to a memory buffer */
#if defined(HAVE_OCI_LOB_READ2)
sb4 php_oci_lob_callback (dvoid *ctxp, CONST dvoid *bufxp, oraub8 len, ub1 piece, dvoid **changed_bufpp, oraub8 *changed_lenp)
#else
sb4 php_oci_lob_callback (dvoid *ctxp, CONST dvoid *bufxp, ub4 len, ub1 piece)
#endif
{
	ub4 lenp = (ub4) len;
	php_oci_lob_ctx *ctx = (php_oci_lob_ctx *)ctxp;

	switch (piece)
	{
		case OCI_LAST_PIECE:
			if ((*(ctx->lob_len) + lenp) > (ctx->alloc_len)) {
				/* this should not happen ever */
				*(ctx->lob_data) = NULL;
				*(ctx->lob_len) = 0;
				return OCI_ERROR;
			}
			memcpy(*(ctx->lob_data) + *(ctx->lob_len), bufxp, (size_t) lenp);
			*(ctx->lob_len) += lenp;
			*(*(ctx->lob_data) + *(ctx->lob_len)) = 0x00;
			return OCI_CONTINUE;

		case OCI_FIRST_PIECE:
		case OCI_NEXT_PIECE:
			if ((*(ctx->lob_len) + lenp) > ctx->alloc_len) {
				/* this should not happen ever */
				*(ctx->lob_data) = NULL;
				*(ctx->lob_len) = 0;
				return OCI_ERROR;
			}
			memcpy(*(ctx->lob_data) + *(ctx->lob_len), bufxp, (size_t) lenp);
			*(ctx->lob_len) += lenp;
			return OCI_CONTINUE;

		default: {
			TSRMLS_FETCH();
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unexpected LOB piece id received (value:%d)", piece);
			*(ctx->lob_data) = NULL;
			*(ctx->lob_len) = 0;
			return OCI_ERROR;
		}
	}
}
/* }}} */

/* {{{ php_oci_lob_calculate_buffer() */
static inline int php_oci_lob_calculate_buffer(php_oci_descriptor *descriptor, long read_length TSRMLS_DC)
{
	php_oci_connection *connection = descriptor->connection;
	ub4 chunk_size;

	if (descriptor->type == OCI_DTYPE_FILE) {
		return read_length;
	}

	if (!descriptor->chunk_size) {
		PHP_OCI_CALL_RETURN(connection->errcode, OCILobGetChunkSize, (connection->svc, connection->err, descriptor->descriptor, &chunk_size));

		if (connection->errcode != OCI_SUCCESS) {
			connection->errcode = php_oci_error(connection->err, connection->errcode TSRMLS_CC);
			PHP_OCI_HANDLE_ERROR(connection, connection->errcode);
			return read_length; /* we have to return original length here */
		}
		descriptor->chunk_size = chunk_size;
	}
	
	if ((read_length % descriptor->chunk_size) != 0) {
		return descriptor->chunk_size * ((read_length / descriptor->chunk_size) + 1);
	}
	return read_length;
}
/* }}} */

/* {{{ php_oci_lob_read()
 Read specified portion of the LOB into the buffer */
int php_oci_lob_read (php_oci_descriptor *descriptor, long read_length, long initial_offset, char **data, ub4 *data_len TSRMLS_DC)
{
	php_oci_connection *connection = descriptor->connection;
	ub4 length = 0;
	int buffer_size = PHP_OCI_LOB_BUFFER_SIZE;
	php_oci_lob_ctx ctx;
	ub1 *bufp;
#if defined(HAVE_OCI_LOB_READ2)
	oraub8 bytes_read, offset = 0;
	oraub8 requested_len = read_length; /* this is by default */
	oraub8 chars_read = 0;
#else
	int bytes_read, offset = 0;
	int requested_len = read_length; /* this is by default */
#endif
	int is_clob = 0;
	sb4 bytes_per_char = 1;

	*data_len = 0;
	*data = NULL;

	ctx.lob_len = data_len;
	ctx.lob_data = data;
	ctx.alloc_len = 0;

	if (php_oci_lob_get_length(descriptor, &length TSRMLS_CC)) {
		return 1;
	}

	if (length <= 0) {
		return 0;
	}
	
	if (initial_offset > length) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Offset must be less than size of the LOB");
		return 1;
	}
		
	if (read_length == -1) {
		requested_len = length;
	}
	
	if (requested_len > (length - initial_offset)) {
		requested_len = length - initial_offset;
	}
	
	if (requested_len <= 0) {
		return 0;
	}
	
	offset = initial_offset;

	if (descriptor->type == OCI_DTYPE_FILE) {
		PHP_OCI_CALL_RETURN(connection->errcode, OCILobFileOpen, (connection->svc, connection->err, descriptor->descriptor, OCI_FILE_READONLY));

		if (connection->errcode != OCI_SUCCESS) {
			connection->errcode = php_oci_error(connection->err, connection->errcode TSRMLS_CC);
			PHP_OCI_HANDLE_ERROR(connection, connection->errcode);
			return 1;
		}
	} else {
		ub2 charset_id = 0;

		PHP_OCI_CALL_RETURN(connection->errcode, OCILobCharSetId, (connection->env, connection->err, descriptor->descriptor, &charset_id));

		if (connection->errcode != OCI_SUCCESS) {
			connection->errcode = php_oci_error(connection->err, connection->errcode TSRMLS_CC);
			PHP_OCI_HANDLE_ERROR(connection, connection->errcode);
			return 1;
		}

		if (charset_id > 0) { /* charset_id is always > 0 for [N]CLOBs */
			is_clob = 1;
		}
	}

	if (is_clob) {
		PHP_OCI_CALL_RETURN(connection->errcode, OCINlsNumericInfoGet, (connection->env, connection->err, &bytes_per_char, OCI_NLS_CHARSET_MAXBYTESZ));

		if (connection->errcode != OCI_SUCCESS) {
			connection->errcode = php_oci_error(connection->err, connection->errcode TSRMLS_CC);
			PHP_OCI_HANDLE_ERROR(connection, connection->errcode);
			return 1;
		}
	} else {
		/* BLOBs don't have encoding, so bytes_per_char == 1 */
	}

	ctx.alloc_len = (requested_len + 1) * bytes_per_char;
	*data = ecalloc(bytes_per_char, requested_len + 1);

#ifdef HAVE_OCI_LOB_READ2
	if (is_clob) {
		chars_read = requested_len;
		bytes_read = 0;
	} else {
		chars_read = 0;
		bytes_read = requested_len;
	}

	buffer_size = (requested_len < buffer_size ) ? requested_len : buffer_size;		/* optimize buffer size */
	buffer_size = php_oci_lob_calculate_buffer(descriptor, buffer_size TSRMLS_CC);	/* use chunk size */

	bufp = (ub1 *) ecalloc(1, buffer_size);
	PHP_OCI_CALL_RETURN(connection->errcode, OCILobRead2,
		(
			connection->svc,
			connection->err,
			descriptor->descriptor,
			(oraub8 *)&bytes_read,							/* IN/OUT bytes toread/read */
			(oraub8 *)&chars_read,							/* IN/OUT chars toread/read */
			(oraub8) offset + 1,							/* offset (starts with 1) */
			(dvoid *) bufp,
			(oraub8) buffer_size,							/* size of buffer */
			OCI_FIRST_PIECE,
			(dvoid *)&ctx,
			(OCICallbackLobRead2) php_oci_lob_callback,				/* callback... */
			(ub2) descriptor->charset_id,			   /* The character set ID of the buffer data. */
			(ub1) descriptor->charset_form					  /* The character set form of the buffer data. */
		)
	);
	
	efree(bufp);
	
	if (is_clob) {
		offset = descriptor->lob_current_position + chars_read;
	} else {
		offset = descriptor->lob_current_position + bytes_read;
	}

#else

	bytes_read = requested_len;
	buffer_size = (requested_len < buffer_size ) ? requested_len : buffer_size;		/* optimize buffer size */
	buffer_size = php_oci_lob_calculate_buffer(descriptor, buffer_size TSRMLS_CC);	/* use chunk size */

	bufp = (ub1 *) ecalloc(1, buffer_size);
	PHP_OCI_CALL_RETURN(connection->errcode, OCILobRead,
		(
			 connection->svc,
			 connection->err,
			 descriptor->descriptor,
			 &bytes_read,								 /* IN/OUT bytes toread/read */
			 offset + 1,							 /* offset (starts with 1) */
			 (dvoid *) bufp,
			 (ub4) buffer_size,							 /* size of buffer */
			 (dvoid *)&ctx,
			 (OCICallbackLobRead) php_oci_lob_callback,				 /* callback... */
			 (ub2) descriptor->charset_id,				/* The character set ID of the buffer data. */
			 (ub1) descriptor->charset_form					   /* The character set form of the buffer data. */
		)
	);
	
	efree(bufp);
	offset = descriptor->lob_current_position + bytes_read;

#endif
	
	if (connection->errcode != OCI_SUCCESS) {
		connection->errcode = php_oci_error(connection->err, connection->errcode TSRMLS_CC);
		PHP_OCI_HANDLE_ERROR(connection, connection->errcode);
		if (*data) {
			efree(*data);
			*data = NULL;
		}
		*data_len = 0;
		return 1;
	}
	
	descriptor->lob_current_position = (int)offset;

	if (descriptor->type == OCI_DTYPE_FILE) {
		PHP_OCI_CALL_RETURN(connection->errcode, OCILobFileClose, (connection->svc, connection->err, descriptor->descriptor));

		if (connection->errcode != OCI_SUCCESS) {
			connection->errcode = php_oci_error(connection->err, connection->errcode TSRMLS_CC);
			PHP_OCI_HANDLE_ERROR(connection, connection->errcode);
			if (*data) {
				efree(*data);
				*data = NULL;
			}
			*data_len = 0;
			return 1;
		}
	}

	return 0;
} /* }}} */

/* {{{ php_oci_lob_write()
 Write data to the LOB */
int php_oci_lob_write (php_oci_descriptor *descriptor, ub4 offset, char *data, int data_len, ub4 *bytes_written TSRMLS_DC)
{
	OCILobLocator *lob		   = (OCILobLocator *) descriptor->descriptor;
	php_oci_connection *connection = (php_oci_connection *) descriptor->connection;
	ub4 lob_length;
	
	*bytes_written = 0;
	if (php_oci_lob_get_length(descriptor, &lob_length TSRMLS_CC)) {
		return 1;
	}
	
	if (!data || data_len <= 0) {
		return 0;
	}
	
	if (offset < 0) {
		offset = 0;
	}
	
	if (offset > descriptor->lob_current_position) {
		offset = descriptor->lob_current_position;
	}
	
	PHP_OCI_CALL_RETURN(connection->errcode, OCILobWrite,
			(
				connection->svc,
				connection->err,
				lob,
				(ub4 *)&data_len,
				(ub4) offset + 1,
				(dvoid *) data,
				(ub4) data_len,
				OCI_ONE_PIECE,
				(dvoid *)0,
				(OCICallbackLobWrite) 0,
				(ub2) descriptor->charset_id,
				(ub1) descriptor->charset_form
			)
		);

	if (connection->errcode) {
		connection->errcode = php_oci_error(connection->err, connection->errcode TSRMLS_CC);
		PHP_OCI_HANDLE_ERROR(connection, connection->errcode);
		*bytes_written = 0;
		return 1;
	}
	*bytes_written = data_len;
	descriptor->lob_current_position += data_len;
	
	if (descriptor->lob_current_position > descriptor->lob_size) {
		descriptor->lob_size = descriptor->lob_current_position;
	}
	
	/* marking buffer as used */
	if (descriptor->buffering == PHP_OCI_LOB_BUFFER_ENABLED) {
		descriptor->buffering = PHP_OCI_LOB_BUFFER_USED;
	}
	
	return 0;
} /* }}} */

/* {{{ php_oci_lob_set_buffering()
 Turn buffering off/onn for this particular LOB */
int php_oci_lob_set_buffering (php_oci_descriptor *descriptor, int on_off TSRMLS_DC)
{
	php_oci_connection *connection = descriptor->connection;

	if (!on_off && descriptor->buffering == PHP_OCI_LOB_BUFFER_DISABLED) {
		/* disabling when it's already off */
		return 0;
	}
	
	if (on_off && descriptor->buffering != PHP_OCI_LOB_BUFFER_DISABLED) {
		/* enabling when it's already on */
		return 0;
	}
	
	if (on_off) {
		PHP_OCI_CALL_RETURN(connection->errcode, OCILobEnableBuffering, (connection->svc, connection->err, descriptor->descriptor));
	} else {
		PHP_OCI_CALL_RETURN(connection->errcode, OCILobDisableBuffering, (connection->svc, connection->err, descriptor->descriptor));
	}

	if (connection->errcode != OCI_SUCCESS) {
		connection->errcode = php_oci_error(connection->err, connection->errcode TSRMLS_CC);
		PHP_OCI_HANDLE_ERROR(connection, connection->errcode);
		return 1;
	}
	descriptor->buffering = on_off ? PHP_OCI_LOB_BUFFER_ENABLED : PHP_OCI_LOB_BUFFER_DISABLED;
	return 0;
} /* }}} */

/* {{{ php_oci_lob_get_buffering()
 Return current buffering state for the LOB */
int php_oci_lob_get_buffering (php_oci_descriptor *descriptor)
{
	if (descriptor->buffering != PHP_OCI_LOB_BUFFER_DISABLED) {
		return 1;
	} else {
		return 0;
	}
} /* }}} */

/* {{{ php_oci_lob_copy()
 Copy one LOB (or its part) to another one */
int php_oci_lob_copy (php_oci_descriptor *descriptor_dest, php_oci_descriptor *descriptor_from, long length TSRMLS_DC)
{
	php_oci_connection *connection = descriptor_dest->connection;
	ub4 length_dest, length_from, copy_len;
	
	if (php_oci_lob_get_length(descriptor_dest, &length_dest TSRMLS_CC)) {
		return 1;
	}
	
	if (php_oci_lob_get_length(descriptor_from, &length_from TSRMLS_CC)) {
		return 1;
	}

	if (length == -1) {
		copy_len = length_from - descriptor_from->lob_current_position;
	} else {
		copy_len = length;
	}

	if ((int)copy_len <= 0) {
		/* silently fail, there is nothing to copy */
		return 1;
	}

	PHP_OCI_CALL_RETURN(connection->errcode, OCILobCopy,
			(
			 connection->svc,
			 connection->err,
			 descriptor_dest->descriptor,
			 descriptor_from->descriptor,
			 copy_len,
			 descriptor_dest->lob_current_position+1,
			 descriptor_from->lob_current_position+1
			)
	);

	if (connection->errcode != OCI_SUCCESS) {
		connection->errcode = php_oci_error(connection->err, connection->errcode TSRMLS_CC);
		PHP_OCI_HANDLE_ERROR(connection, connection->errcode);
		return 1;
	}
	
	return 0;
} /* }}} */

/* {{{ php_oci_lob_close()
 Close LOB */
int php_oci_lob_close (php_oci_descriptor *descriptor TSRMLS_DC)
{
	php_oci_connection *connection = descriptor->connection;
	
	if (descriptor->is_open) {
		PHP_OCI_CALL_RETURN(connection->errcode, OCILobClose, (connection->svc, connection->err, descriptor->descriptor));
	}

	if (connection->errcode != OCI_SUCCESS) {
		connection->errcode = php_oci_error(connection->err, connection->errcode TSRMLS_CC);
		PHP_OCI_HANDLE_ERROR(connection, connection->errcode);
		return 1;
	}

	if (php_oci_temp_lob_close(descriptor TSRMLS_CC)) {
		return 1;
	}
	
	return 0;
} /* }}} */

/* {{{ php_oci_temp_lob_close()
   Close Temporary LOB */
int php_oci_temp_lob_close (php_oci_descriptor *descriptor TSRMLS_DC)
{
	php_oci_connection *connection = descriptor->connection;
	int is_temporary;

	PHP_OCI_CALL_RETURN(connection->errcode, OCILobIsTemporary, (connection->env,connection->err, descriptor->descriptor, &is_temporary));
	
	if (connection->errcode != OCI_SUCCESS) {
		connection->errcode = php_oci_error(connection->err, connection->errcode TSRMLS_CC);
		PHP_OCI_HANDLE_ERROR(connection, connection->errcode);
		return 1;
	}
	
	if (is_temporary) {
		PHP_OCI_CALL_RETURN(connection->errcode, OCILobFreeTemporary, (connection->svc, connection->err, descriptor->descriptor));
		
		if (connection->errcode != OCI_SUCCESS) {
			connection->errcode = php_oci_error(connection->err, connection->errcode TSRMLS_CC);
			PHP_OCI_HANDLE_ERROR(connection, connection->errcode);
			return 1;
		}
	}
	return 0;
} /* }}} */


/* {{{ php_oci_lob_flush()
 Flush buffers for the LOB (only if they have been used) */
int php_oci_lob_flush(php_oci_descriptor *descriptor, long flush_flag TSRMLS_DC)
{
	OCILobLocator *lob = descriptor->descriptor;
	php_oci_connection *connection = descriptor->connection;
	
	if (!lob) {
		return 1;
	}

	switch (flush_flag) {
		case 0:
		case OCI_LOB_BUFFER_FREE:
			/* only these two are allowed */
			break;
		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid flag value: %ld", flush_flag);
			return 1;
			break;
	}
	
	/* do not really flush buffer, but report success
	 * to suppress OCI error when flushing not used buffer
	 * */
	if (descriptor->buffering != PHP_OCI_LOB_BUFFER_USED) {
		return 0;
	}

	PHP_OCI_CALL_RETURN(connection->errcode, OCILobFlushBuffer, (connection->svc, connection->err, lob, flush_flag));

	if (connection->errcode != OCI_SUCCESS) {
		connection->errcode = php_oci_error(connection->err, connection->errcode TSRMLS_CC);
		PHP_OCI_HANDLE_ERROR(connection, connection->errcode);
		return 1;
	}

	/* marking buffer as enabled and not used */
	descriptor->buffering = PHP_OCI_LOB_BUFFER_ENABLED;
	return 0;
} /* }}} */

/* {{{ php_oci_lob_free()
 Close LOB descriptor and free associated resources */
void php_oci_lob_free (php_oci_descriptor *descriptor TSRMLS_DC)
{
	if (!descriptor || !descriptor->connection) {
		return;
	}

	if (descriptor->connection->descriptors) {
		/* delete descriptor from the hash */
		zend_hash_index_del(descriptor->connection->descriptors, descriptor->index);
		if (zend_hash_num_elements(descriptor->connection->descriptors) == 0) {
			descriptor->connection->descriptor_count = 0;
		} else {
			if (descriptor->index + 1 == descriptor->connection->descriptor_count) {
				/* If the descriptor being freed is the end-most one
				 * allocated, then the descriptor_count is reduced so
				 * a future descriptor can reuse the hash table index.
				 * This can prevent the hash index range increasing in
				 * the common case that each descriptor is
				 * allocated/used/freed before another descriptor is
				 * needed.  However it is possible that a script frees
				 * descriptors in arbitrary order which would prevent
				 * descriptor_count ever being reduced to zero until
				 * zend_hash_num_elements() returns 0.
				 */
				descriptor->connection->descriptor_count--;
			}
		}
	}
	
	/* flushing Lobs & Files with buffering enabled */
	if ((descriptor->type == OCI_DTYPE_FILE || descriptor->type == OCI_DTYPE_LOB) && descriptor->buffering == PHP_OCI_LOB_BUFFER_USED) {
		php_oci_lob_flush(descriptor, OCI_LOB_BUFFER_FREE TSRMLS_CC);
	}

	if (descriptor->type == OCI_DTYPE_LOB) {
		php_oci_temp_lob_close(descriptor TSRMLS_CC);
	}

	PHP_OCI_CALL(OCIDescriptorFree, (descriptor->descriptor, descriptor->type));

	zend_list_delete(descriptor->connection->rsrc_id);
	efree(descriptor);
} /* }}} */

/* {{{ php_oci_lob_import()
 Import LOB contents from the given file */
int php_oci_lob_import (php_oci_descriptor *descriptor, char *filename TSRMLS_DC)
{
	int fp;
	ub4 loblen;
	OCILobLocator *lob = (OCILobLocator *)descriptor->descriptor;
	php_oci_connection *connection = descriptor->connection;
	char buf[8192];
	ub4 offset = 1;
	
#if (PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION > 3) || (PHP_MAJOR_VERSION > 5)
	/* Safe mode has been removed in PHP 5.4 */
	if (php_check_open_basedir(filename TSRMLS_CC)) {
#else
	if ((PG(safe_mode) && (!php_checkuid(filename, NULL, CHECKUID_CHECK_FILE_AND_DIR))) || php_check_open_basedir(filename TSRMLS_CC)) {
#endif
		return 1;
	}
	
	if ((fp = VCWD_OPEN(filename, O_RDONLY|O_BINARY)) == -1) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Can't open file %s", filename);
		return 1;
	}

	while ((loblen = read(fp, &buf, sizeof(buf))) > 0) {	
		PHP_OCI_CALL_RETURN(connection->errcode,
				OCILobWrite,
				(
					connection->svc,
					connection->err,
					lob,
					&loblen,
					offset,
					(dvoid *) &buf,
					loblen,
					OCI_ONE_PIECE,
					(dvoid *)0,
					(OCICallbackLobWrite) 0,
					(ub2) descriptor->charset_id,
					(ub1) descriptor->charset_form
				)
		);

		if (connection->errcode != OCI_SUCCESS) {
			connection->errcode = php_oci_error(connection->err, connection->errcode TSRMLS_CC);
			PHP_OCI_HANDLE_ERROR(connection, connection->errcode);
			close(fp);
			return 1;
		}
		offset += loblen;
	}
	close(fp);
	
	return 0;
} /* }}} */

/* {{{ php_oci_lob_append()
 Append data to the end of the LOB */
int php_oci_lob_append (php_oci_descriptor *descriptor_dest, php_oci_descriptor *descriptor_from TSRMLS_DC)
{
	php_oci_connection *connection = descriptor_dest->connection;
	OCILobLocator *lob_dest = descriptor_dest->descriptor;
	OCILobLocator *lob_from = descriptor_from->descriptor;
	ub4 dest_len, from_len;

	if (php_oci_lob_get_length(descriptor_dest, &dest_len TSRMLS_CC)) {
		return 1;
	}
	
	if (php_oci_lob_get_length(descriptor_from, &from_len TSRMLS_CC)) {
		return 1;
	}

	if (from_len <= 0) {
		return 0;
	}

	PHP_OCI_CALL_RETURN(connection->errcode, OCILobAppend, (connection->svc, connection->err, lob_dest, lob_from));

	if (connection->errcode != OCI_SUCCESS) {
		connection->errcode = php_oci_error(connection->err, connection->errcode TSRMLS_CC);
		PHP_OCI_HANDLE_ERROR(connection, connection->errcode);
		return 1;
	}
	return 0;
} /* }}} */

/* {{{ php_oci_lob_truncate()
 Truncate LOB to the given length */
int php_oci_lob_truncate (php_oci_descriptor *descriptor, long new_lob_length TSRMLS_DC)
{
	php_oci_connection *connection = descriptor->connection;
	OCILobLocator *lob = descriptor->descriptor;
	ub4 lob_length;
	
	if (php_oci_lob_get_length(descriptor, &lob_length TSRMLS_CC)) {
		return 1;
	}
	
	if (lob_length <= 0) {
		return 0;
	}

	if (new_lob_length < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Size must be greater than or equal to 0");
		return 1;
	}

	if (new_lob_length > lob_length) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Size must be less than or equal to the current LOB size");
		return 1;
	}
	
	PHP_OCI_CALL_RETURN(connection->errcode, OCILobTrim, (connection->svc, connection->err, lob, new_lob_length));

	if (connection->errcode != OCI_SUCCESS) {
		connection->errcode = php_oci_error(connection->err, connection->errcode TSRMLS_CC);
		PHP_OCI_HANDLE_ERROR(connection, connection->errcode);
		return 1;
	}
	
	descriptor->lob_size = new_lob_length;
	return 0;
} /* }}} */

/* {{{ php_oci_lob_erase()
 Erase (or fill with whitespaces, depending on LOB type) the LOB (or its part) */
int php_oci_lob_erase (php_oci_descriptor *descriptor, long offset, ub4 length, ub4 *bytes_erased TSRMLS_DC)
{
	php_oci_connection *connection = descriptor->connection;
	OCILobLocator *lob = descriptor->descriptor;
	ub4 lob_length;

	*bytes_erased = 0;
	
	if (php_oci_lob_get_length(descriptor, &lob_length TSRMLS_CC)) {
		return 1;
	}
	
	if (offset == -1) {
		offset = descriptor->lob_current_position;
	}

	if (length == -1) {
		length = lob_length;
	}
	
	PHP_OCI_CALL_RETURN(connection->errcode, OCILobErase, (connection->svc, connection->err, lob, (ub4 *)&length, offset+1));

	if (connection->errcode != OCI_SUCCESS) {
		connection->errcode = php_oci_error(connection->err, connection->errcode TSRMLS_CC);
		PHP_OCI_HANDLE_ERROR(connection, connection->errcode);
		return 1;
	}
	
	*bytes_erased = length;
	return 0;
} /* }}} */

/* {{{ php_oci_lob_is_equal()
 Compare two LOB descriptors and figure out if they are pointing to the same LOB */
int php_oci_lob_is_equal (php_oci_descriptor *descriptor_first, php_oci_descriptor *descriptor_second, boolean *result TSRMLS_DC)
{
	php_oci_connection *connection = descriptor_first->connection;
	OCILobLocator *first_lob   = descriptor_first->descriptor;
	OCILobLocator *second_lob  = descriptor_second->descriptor;

	PHP_OCI_CALL_RETURN(connection->errcode, OCILobIsEqual, (connection->env, first_lob, second_lob, result));

	if (connection->errcode) {
		connection->errcode = php_oci_error(connection->err, connection->errcode TSRMLS_CC);
		PHP_OCI_HANDLE_ERROR(connection, connection->errcode);
		return 1;
	}
	return 0;
} /* }}} */

/* {{{ php_oci_lob_write_tmp()
 Create temporary LOB and write data to it */
int php_oci_lob_write_tmp (php_oci_descriptor *descriptor, long type, char *data, int data_len TSRMLS_DC)
{
	php_oci_connection *connection = descriptor->connection;
	OCILobLocator *lob		   = descriptor->descriptor;
	ub4 bytes_written = 0;
	
	switch (type) {
		case OCI_TEMP_BLOB:
		case OCI_TEMP_CLOB:
			/* only these two are allowed */
			break;
		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid temporary lob type: %ld", type);
			return 1;
			break;
	}

	if (data_len < 0) {
		return 1;
	}

	PHP_OCI_CALL_RETURN(connection->errcode, OCILobCreateTemporary,
			(
			 connection->svc,
			 connection->err,
			 lob,
			 OCI_DEFAULT,
			 OCI_DEFAULT,
			 (ub1)type,
			 OCI_ATTR_NOCACHE,
			 OCI_DURATION_SESSION
			)
	);

	if (connection->errcode) {
		connection->errcode = php_oci_error(connection->err, connection->errcode TSRMLS_CC);
		PHP_OCI_HANDLE_ERROR(connection, connection->errcode);
		return 1;
	}

	PHP_OCI_CALL_RETURN(connection->errcode, OCILobOpen, (connection->svc, connection->err, lob, OCI_LOB_READWRITE));

	if (connection->errcode) {
		connection->errcode = php_oci_error(connection->err, connection->errcode TSRMLS_CC);
		PHP_OCI_HANDLE_ERROR(connection, connection->errcode);
		return 1;
	}

	descriptor->is_open = 1;

	return php_oci_lob_write(descriptor, 0, data, data_len, &bytes_written TSRMLS_CC);
} /* }}} */

#endif /* HAVE_OCI8 */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
