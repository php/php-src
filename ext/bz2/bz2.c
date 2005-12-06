/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Sterling Hughes <sterling@php.net>                           |
   +----------------------------------------------------------------------+
 */
 
/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_bz2.h"

#if HAVE_BZ2

/* PHP Includes */
#include "ext/standard/file.h"
#include "ext/standard/info.h"
#include "ext/standard/php_string.h"

/* for fileno() */
#include <stdio.h>

/* Internal error constants */
#define PHP_BZ_ERRNO   0
#define PHP_BZ_ERRSTR  1
#define PHP_BZ_ERRBOTH 2

/* Blocksize of the decompression buffer */
#define PHP_BZ_DECOMPRESS_SIZE 4096

function_entry bz2_functions[] = {
	PHP_FE(bzopen,       NULL)
	PHP_FE(bzread,       NULL)
	PHP_FALIAS(bzwrite,   fwrite,		NULL)
	PHP_FALIAS(bzflush,   fflush,		NULL)
	PHP_FALIAS(bzclose,   fclose,		NULL)
	PHP_FE(bzerrno,      NULL)
	PHP_FE(bzerrstr,     NULL)
	PHP_FE(bzerror,      NULL)
	PHP_FE(bzcompress,   NULL)
	PHP_FE(bzdecompress, NULL)
	{NULL, NULL, NULL}
};

zend_module_entry bz2_module_entry = {
	STANDARD_MODULE_HEADER,
	"bz2",
	bz2_functions,
	PHP_MINIT(bz2),
	PHP_MSHUTDOWN(bz2),
	NULL,
	NULL,
	PHP_MINFO(bz2),
	NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_BZ2
ZEND_GET_MODULE(bz2)
#endif

struct php_bz2_stream_data_t {
	BZFILE *bz_file;
	php_stream *stream;
};

/* {{{ BZip2 stream implementation */

static size_t php_bz2iop_read(php_stream *stream, char *buf, size_t count TSRMLS_DC)
{
	struct php_bz2_stream_data_t *self = (struct php_bz2_stream_data_t *) stream->abstract;
	size_t ret;
	
	ret = BZ2_bzread(self->bz_file, buf, count);

	if (ret == 0) {
		stream->eof = 1;
	}

	return ret;
}

static size_t php_bz2iop_write(php_stream *stream, const char *buf, size_t count TSRMLS_DC)
{
	struct php_bz2_stream_data_t *self = (struct php_bz2_stream_data_t *) stream->abstract;

	return BZ2_bzwrite(self->bz_file, (char*)buf, count); 
}

static int php_bz2iop_close(php_stream *stream, int close_handle TSRMLS_DC)
{
	struct php_bz2_stream_data_t *self = (struct php_bz2_stream_data_t *)stream->abstract;
	int ret = EOF;
	
	if (close_handle) {
		BZ2_bzclose(self->bz_file);
	}

	if (self->stream) {
		php_stream_free(self->stream, PHP_STREAM_FREE_CLOSE | (close_handle == 0 ? PHP_STREAM_FREE_PRESERVE_HANDLE : 0));
	}

	efree(self);

	return ret;
}

static int php_bz2iop_flush(php_stream *stream TSRMLS_DC)
{
	struct php_bz2_stream_data_t *self = (struct php_bz2_stream_data_t *)stream->abstract;
	return BZ2_bzflush(self->bz_file);
}
/* }}} */

php_stream_ops php_stream_bz2io_ops = {
	php_bz2iop_write, php_bz2iop_read,
	php_bz2iop_close, php_bz2iop_flush,
	"BZip2",
	NULL, /* seek */
	NULL, /* cast */
	NULL, /* stat */
	NULL  /* set_option */
};

/* {{{ Bzip2 stream openers */
PHP_BZ2_API php_stream *_php_stream_bz2open_from_BZFILE(BZFILE *bz, 
														char *mode, php_stream *innerstream STREAMS_DC TSRMLS_DC)
{
	struct php_bz2_stream_data_t *self;
	
	self = emalloc(sizeof(*self));

	self->stream = innerstream;
	self->bz_file = bz;

	return php_stream_alloc_rel(&php_stream_bz2io_ops, self, 0, mode);
}

PHP_BZ2_API php_stream *_php_stream_bz2open(php_stream_wrapper *wrapper,
											char *path,
											char *mode,
											int options,
											char **opened_path,
											php_stream_context *context STREAMS_DC TSRMLS_DC)
{
	php_stream *retstream = NULL, *stream = NULL;
	char *path_copy = NULL;
	BZFILE *bz_file = NULL;

	if (strncasecmp("compress.bzip2://", path, 17) == 0) {
		path += 17;
	}

#ifdef VIRTUAL_DIR
	virtual_filepath(path, &path_copy TSRMLS_CC);
#else
	path_copy = path;
#endif  
	
	/* try and open it directly first */
	bz_file = BZ2_bzopen(path_copy, mode);

	if (opened_path && bz_file) {
		*opened_path = estrdup(path_copy);
	}
	path_copy = NULL;
	
	if (bz_file == NULL) {
		/* that didn't work, so try and get something from the network/wrapper */
		stream = php_stream_open_wrapper(path, mode, options, opened_path);
	
		if (stream) {
			int fd;
			if (SUCCESS == php_stream_cast(stream, PHP_STREAM_AS_FD, (void **) &fd, REPORT_ERRORS)) {
				bz_file = BZ2_bzdopen(fd, mode);
			}
		}
	}
	
	if (bz_file) {
		retstream = _php_stream_bz2open_from_BZFILE(bz_file, mode, stream STREAMS_REL_CC TSRMLS_CC);
		if (retstream) {
			return retstream;
		}

		BZ2_bzclose(bz_file);
	}

	if (stream) {
		php_stream_close(stream);
	}

	return NULL;
}

/* }}} */

static php_stream_wrapper_ops bzip2_stream_wops = {
	_php_stream_bz2open,
	NULL, /* close */
	NULL, /* fstat */
	NULL, /* stat */
	NULL, /* opendir */
	"BZip2",
	NULL  /* unlink */
};

php_stream_wrapper php_stream_bzip2_wrapper = {
	&bzip2_stream_wops,
	NULL,
	0 /* is_url */
};

static void php_bz2_error(INTERNAL_FUNCTION_PARAMETERS, int);

PHP_MINIT_FUNCTION(bz2)
{
	php_register_url_stream_wrapper("compress.bzip2", &php_stream_bzip2_wrapper TSRMLS_CC);

	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(bz2)
{
	php_unregister_url_stream_wrapper("compress.bzip2" TSRMLS_CC);

	return SUCCESS;
}

PHP_MINFO_FUNCTION(bz2)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "BZip2 Support", "Enabled");
	php_info_print_table_row(2, "BZip2 Version", (char *) BZ2_bzlibVersion());
	php_info_print_table_end();
}

/* {{{ proto string bzread(int bz[, int length])
   Reads up to length bytes from a BZip2 stream, or 1024 bytes if length is not specified */
PHP_FUNCTION(bzread)
{
	zval *bz;
	long len = 1024;
	php_stream *stream;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r|l", &bz, &len)) {
		RETURN_FALSE;
	}
	
	php_stream_from_zval(stream, &bz);

	if (len < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "length may not be negative");
		RETURN_FALSE;
	}

	Z_STRVAL_P(return_value) = emalloc(len + 1);
	Z_STRLEN_P(return_value) = php_stream_read(stream, Z_STRVAL_P(return_value), len);
	
	if (Z_STRLEN_P(return_value) < 0) {
		efree(Z_STRVAL_P(return_value));
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "could not read valid bz2 data from stream");
		RETURN_FALSE;		
	}
	
	Z_STRVAL_P(return_value)[Z_STRLEN_P(return_value)] = 0;

	if (PG(magic_quotes_runtime)) {
		Z_STRVAL_P(return_value) = php_addslashes(	Z_STRVAL_P(return_value),
													Z_STRLEN_P(return_value),
													&Z_STRLEN_P(return_value), 1 TSRMLS_CC);
	}

	Z_TYPE_P(return_value) = IS_STRING;
}
/* }}} */

/* {{{ proto resource bzopen(string|int file|fp, string mode)
   Opens a new BZip2 stream */
PHP_FUNCTION(bzopen)
{
	zval    **file,   /* The file to open */
	        **mode;   /* The mode to open the stream with */
	BZFILE   *bz;     /* The compressed file stream */
	php_stream *stream = NULL;
	
	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &file, &mode) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(mode);
	
	/* If it's not a resource its a string containing the filename to open */
	if (Z_TYPE_PP(file) != IS_RESOURCE) {
		convert_to_string_ex(file);
		stream = php_stream_bz2open(NULL,
									Z_STRVAL_PP(file), 
									Z_STRVAL_PP(mode), 
									ENFORCE_SAFE_MODE | REPORT_ERRORS, 
									NULL);
	} else {
		/* If it is a resource, than its a stream resource */
		int fd;

		php_stream_from_zval(stream, file);

		if (FAILURE == php_stream_cast(stream, PHP_STREAM_AS_FD, (void *) &fd, REPORT_ERRORS)) {
			RETURN_FALSE;
		}
		
		bz = BZ2_bzdopen(fd, Z_STRVAL_PP(mode));

		stream = php_stream_bz2open_from_BZFILE(bz, Z_STRVAL_PP(mode), stream);
	}

	if (stream) {
		php_stream_to_zval(stream, return_value);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto int bzerrno(resource bz)
   Returns the error number */
PHP_FUNCTION(bzerrno)
{
	php_bz2_error(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_BZ_ERRNO);
}
/* }}} */

/* {{{ proto string bzerrstr(resource bz)
   Returns the error string */
PHP_FUNCTION(bzerrstr)
{
	php_bz2_error(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_BZ_ERRSTR);
}
/* }}} */

/* {{{ proto array bzerror(resource bz)
   Returns the error number and error string in an associative array */
PHP_FUNCTION(bzerror)
{
	php_bz2_error(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_BZ_ERRBOTH);
}
/* }}} */

/* {{{ proto string bzcompress(string source [, int blocksize100k [, int workfactor]])
   Compresses a string into BZip2 encoded data */
PHP_FUNCTION(bzcompress)
{
	zval            **source,          /* Source data to compress */
	                **zblock_size,     /* Optional block size to use */
					**zwork_factor;    /* Optional work factor to use */
	char             *dest = NULL;     /* Destination to place the compressed data into */
	int               error,           /* Error Container */
					  block_size  = 4, /* Block size for compression algorithm */
					  work_factor = 0, /* Work factor for compression algorithm */
					  argc;            /* Argument count */
	unsigned int      source_len,      /* Length of the source data */
					  dest_len;        /* Length of the destination buffer */ 
	
	argc = ZEND_NUM_ARGS();

	if (argc < 1 || argc > 3 || zend_get_parameters_ex(argc, &source, &zblock_size, &zwork_factor) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(source);
	
	/* Assign them to easy to use variables, dest_len is initially the length of the data
	   + .01 x length of data + 600 which is the largest size the results of the compression 
	   could possibly be, at least that's what the libbz2 docs say (thanks to jeremy@nirvani.net 
	   for pointing this out).  */
	source_len = Z_STRLEN_PP(source);
	dest_len   = Z_STRLEN_PP(source) + (0.01 * Z_STRLEN_PP(source)) + 600;
	
	/* Allocate the destination buffer */
	dest = emalloc(dest_len + 1);
	
	/* Handle the optional arguments */
	if (argc > 1) {
		convert_to_long_ex(zblock_size);
		block_size = Z_LVAL_PP(zblock_size);
	}
	
	if (argc > 2) {
		convert_to_long_ex(zwork_factor);
		work_factor = Z_LVAL_PP(zwork_factor);
	}

	error = BZ2_bzBuffToBuffCompress(dest, &dest_len, Z_STRVAL_PP(source), source_len, block_size, 0, work_factor);
	if (error != BZ_OK) {
		efree(dest);
		RETURN_LONG(error);
	} else {
		/* Copy the buffer, we have perhaps allocate alot more than we need,
		   so we erealloc() the buffer to the proper size */
		dest = erealloc(dest, dest_len + 1);
		dest[dest_len] = 0;
		RETURN_STRINGL(dest, dest_len, 0);
	}
}
/* }}} */

/* {{{ proto string bzdecompress(string source [, int small])
   Decompresses BZip2 compressed data */
PHP_FUNCTION(bzdecompress)
{
	zval    **source,                             /* Source data to decompress */
	        **zsmall;                             /* (Optional) user specified small */
	char     *dest;                               /* Destination buffer, initially allocated */
	int       error,                              /* Error container */
	          iter = 1,                           /* Iteration count for the compression loop */
			  size,                               /* Current size to realloc the dest buffer to */
			  dest_len = PHP_BZ_DECOMPRESS_SIZE,  /* Size of the destination length */
			  small    = 0,                       /* The actual small */
			  argc     = ZEND_NUM_ARGS();         /* Argument count */
	
	if (argc < 1 || argc > 2 || zend_get_parameters_ex(argc, &source, &zsmall) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(source);
	
	/* optional small argument handling */
	if (argc > 1) {
		convert_to_long_ex(zsmall);
		small = Z_LVAL_PP(zsmall);
	}

	/* Depending on the size of the source buffer, either allocate
	  the length of the source buffer or the a default decompression
	  size */
	dest = emalloc(PHP_BZ_DECOMPRESS_SIZE > Z_STRLEN_PP(source) ? PHP_BZ_DECOMPRESS_SIZE : Z_STRLEN_PP(source));

	/* (de)Compression Loop */	
	do {
		/* Handle the (re)allocation of the buffer */
		size = dest_len * iter;
		if (iter > 1) {
			dest = erealloc(dest, size);
		}
		++iter;

		/* Perform the decompression */
		error = BZ2_bzBuffToBuffDecompress(dest, &size, Z_STRVAL_PP(source), Z_STRLEN_PP(source), small, 0);
	} while (error == BZ_OUTBUFF_FULL);
	
	if (error != BZ_OK) {
		efree(dest);
		RETURN_LONG(error);
	} else {
		/* we might have allocated a little to much, so erealloc the buffer 
		 down to size, before returning it */
		dest = erealloc(dest, size + 1);
		dest[size] = 0;
		RETURN_STRINGL(dest, size, 0);
	}
}
/* }}} */

/* {{{ php_bz2_error()
   The central error handling interface, does the work for bzerrno, bzerrstr and bzerror */
static void php_bz2_error(INTERNAL_FUNCTION_PARAMETERS, int opt)
{ 
	zval        **bzp;     /* BZip2 Resource Pointer */
	php_stream   *stream;
	const char   *errstr;  /* Error string */
	int           errnum;  /* Error number */
	struct php_bz2_stream_data_t *self;
	
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &bzp) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	php_stream_from_zval(stream, bzp);

	if (!php_stream_is(stream, PHP_STREAM_IS_BZIP2)) {
		RETURN_FALSE;
	}

	self = (struct php_bz2_stream_data_t *) stream->abstract;
	
	/* Fetch the error information */
	errstr = BZ2_bzerror(self->bz_file, &errnum);
	
	/* Determine what to return */
	switch (opt) {
		case PHP_BZ_ERRNO:
			RETURN_LONG(errnum);
			break;
		case PHP_BZ_ERRSTR:
			RETURN_STRING((char*)errstr, 1);
			break;
		case PHP_BZ_ERRBOTH:
			array_init(return_value);
		
			add_assoc_long  (return_value, "errno",  errnum);
			add_assoc_string(return_value, "errstr", (char*)errstr, 1);
			break;
	}
}
/* }}} */

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */
