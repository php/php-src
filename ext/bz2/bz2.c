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
  | Author: Sterling Hughes <sterling@php.net>                           |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_bz2.h"
#include "bz2_arginfo.h"

#ifdef HAVE_BZ2

/* PHP Includes */
#include "ext/standard/info.h"
#include "ext/standard/php_string.h"
#include "main/php_network.h"

/* for fileno() */
#include <stdio.h>

/* Internal error constants */
#define PHP_BZ_ERRNO   0
#define PHP_BZ_ERRSTR  1
#define PHP_BZ_ERRBOTH 2

static PHP_MINIT_FUNCTION(bz2);
static PHP_MSHUTDOWN_FUNCTION(bz2);
static PHP_MINFO_FUNCTION(bz2);

zend_module_entry bz2_module_entry = {
	STANDARD_MODULE_HEADER,
	"bz2",
	ext_functions,
	PHP_MINIT(bz2),
	PHP_MSHUTDOWN(bz2),
	NULL,
	NULL,
	PHP_MINFO(bz2),
	PHP_BZ2_VERSION,
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

static ssize_t php_bz2iop_read(php_stream *stream, char *buf, size_t count)
{
	struct php_bz2_stream_data_t *self = (struct php_bz2_stream_data_t *)stream->abstract;
	size_t ret = 0;

	do {
		int just_read;
		size_t remain = count - ret;
		int to_read = (int)(remain <= INT_MAX ? remain : INT_MAX);

		just_read = BZ2_bzread(self->bz_file, buf, to_read);

		if (just_read < 1) {
			/* it is not safe to keep reading after an error, see #72613 */
			stream->eof = 1;
			if (just_read < 0) {
				if (ret) {
					return ret;
				}
				return -1;
			}
			break;
		}

		ret += just_read;
	} while (ret < count);

	return ret;
}

static ssize_t php_bz2iop_write(php_stream *stream, const char *buf, size_t count)
{
	ssize_t wrote = 0;
	struct php_bz2_stream_data_t *self = (struct php_bz2_stream_data_t *)stream->abstract;

	do {
		int just_wrote;
		size_t remain = count - wrote;
		int to_write = (int)(remain <= INT_MAX ? remain : INT_MAX);

		just_wrote = BZ2_bzwrite(self->bz_file, (char*)buf, to_write);
		if (just_wrote < 0) {
			if (wrote == 0) {
				return just_wrote;
			}
			return wrote;
		}
		if (just_wrote == 0) {
			break;
		}

		wrote += just_wrote;

	} while (wrote < count);

	return wrote;
}

static int php_bz2iop_close(php_stream *stream, int close_handle)
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

static int php_bz2iop_flush(php_stream *stream)
{
	struct php_bz2_stream_data_t *self = (struct php_bz2_stream_data_t *)stream->abstract;
	return BZ2_bzflush(self->bz_file);
}
/* }}} */

const php_stream_ops php_stream_bz2io_ops = {
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
														const char *mode, php_stream *innerstream STREAMS_DC)
{
	struct php_bz2_stream_data_t *self;

	self = emalloc(sizeof(*self));

	self->stream = innerstream;
	if (innerstream) {
		GC_ADDREF(innerstream->res);
	}
	self->bz_file = bz;

	return php_stream_alloc_rel(&php_stream_bz2io_ops, self, 0, mode);
}

PHP_BZ2_API php_stream *_php_stream_bz2open(php_stream_wrapper *wrapper,
											const char *path,
											const char *mode,
											int options,
											zend_string **opened_path,
											php_stream_context *context STREAMS_DC)
{
	php_stream *retstream = NULL, *stream = NULL;
	char *path_copy = NULL;
	BZFILE *bz_file = NULL;

	if (strncasecmp("compress.bzip2://", path, 17) == 0) {
		path += 17;
	}
	if (mode[0] == '\0' || (mode[0] != 'w' && mode[0] != 'r' && mode[1] != '\0')) {
		return NULL;
	}

#ifdef VIRTUAL_DIR
	virtual_filepath_ex(path, &path_copy, NULL);
#else
	path_copy = (char *)path;
#endif

	if (php_check_open_basedir(path_copy)) {
#ifdef VIRTUAL_DIR
		efree(path_copy);
#endif
		return NULL;
	}

	/* try and open it directly first */
	bz_file = BZ2_bzopen(path_copy, mode);

	if (opened_path && bz_file) {
		*opened_path = zend_string_init(path_copy, strlen(path_copy), 0);
	}

#ifdef VIRTUAL_DIR
	efree(path_copy);
#endif

	if (bz_file == NULL) {
		/* that didn't work, so try and get something from the network/wrapper */
		stream = php_stream_open_wrapper(path, mode, options | STREAM_WILL_CAST, opened_path);

		if (stream) {
			php_socket_t fd;
			if (SUCCESS == php_stream_cast(stream, PHP_STREAM_AS_FD, (void **) &fd, REPORT_ERRORS)) {
				bz_file = BZ2_bzdopen((int)fd, mode);
			}
		}

		/* remove the file created by php_stream_open_wrapper(), it is not needed since BZ2 functions
		 * failed.
		 */
		if (opened_path && !bz_file && mode[0] == 'w') {
			VCWD_UNLINK(ZSTR_VAL(*opened_path));
		}
	}

	if (bz_file) {
		retstream = _php_stream_bz2open_from_BZFILE(bz_file, mode, stream STREAMS_REL_CC);
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

static const php_stream_wrapper_ops bzip2_stream_wops = {
	_php_stream_bz2open,
	NULL, /* close */
	NULL, /* fstat */
	NULL, /* stat */
	NULL, /* opendir */
	"BZip2",
	NULL, /* unlink */
	NULL, /* rename */
	NULL, /* mkdir */
	NULL, /* rmdir */
	NULL
};

static const php_stream_wrapper php_stream_bzip2_wrapper = {
	&bzip2_stream_wops,
	NULL,
	0 /* is_url */
};

static void php_bz2_error(INTERNAL_FUNCTION_PARAMETERS, int);

static PHP_MINIT_FUNCTION(bz2)
{
	php_register_url_stream_wrapper("compress.bzip2", &php_stream_bzip2_wrapper);
	php_stream_filter_register_factory("bzip2.*", &php_bz2_filter_factory);
	return SUCCESS;
}

static PHP_MSHUTDOWN_FUNCTION(bz2)
{
	php_unregister_url_stream_wrapper("compress.bzip2");
	php_stream_filter_unregister_factory("bzip2.*");

	return SUCCESS;
}

static PHP_MINFO_FUNCTION(bz2)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "BZip2 Support", "Enabled");
	php_info_print_table_row(2, "Stream Wrapper support", "compress.bzip2://");
	php_info_print_table_row(2, "Stream Filter support", "bzip2.decompress, bzip2.compress");
	php_info_print_table_row(2, "BZip2 Version", (char *) BZ2_bzlibVersion());
	php_info_print_table_end();
}

/* {{{ Reads up to length bytes from a BZip2 stream, or 1024 bytes if length is not specified */
PHP_FUNCTION(bzread)
{
	zval *bz;
	zend_long len = 1024;
	php_stream *stream;
	zend_string *data;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(), "r|l", &bz, &len)) {
		RETURN_THROWS();
	}

	php_stream_from_zval(stream, bz);

	if (len  < 0) {
		zend_argument_value_error(2, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	data = php_stream_read_to_str(stream, len);
	if (!data) {
		RETURN_FALSE;
	}
	RETURN_STR(data);
}
/* }}} */

/* {{{ Opens a new BZip2 stream */
PHP_FUNCTION(bzopen)
{
	zval     *file;   /* The file to open */
	char     *mode;   /* The mode to open the stream with */
	size_t      mode_len;

	BZFILE   *bz;     /* The compressed file stream */
	php_stream *stream = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zs", &file, &mode, &mode_len) == FAILURE) {
		RETURN_THROWS();
	}

	if (mode_len != 1 || (mode[0] != 'r' && mode[0] != 'w')) {
		zend_argument_value_error(2, "must be either \"r\" or \"w\"");
		RETURN_THROWS();
	}

	/* If it's not a resource its a string containing the filename to open */
	if (Z_TYPE_P(file) == IS_STRING) {
		if (Z_STRLEN_P(file) == 0) {
			zend_argument_value_error(1, "cannot be empty");
			RETURN_THROWS();
		}

		if (CHECK_ZVAL_NULL_PATH(file)) {
			zend_argument_type_error(1, "must not contain null bytes");
			RETURN_THROWS();
		}

		stream = php_stream_bz2open(NULL, Z_STRVAL_P(file), mode, REPORT_ERRORS, NULL);
	} else if (Z_TYPE_P(file) == IS_RESOURCE) {
		/* If it is a resource, than its a stream resource */
		php_socket_t fd;
		size_t stream_mode_len;

		php_stream_from_zval(stream, file);
		stream_mode_len = strlen(stream->mode);

		if (stream_mode_len != 1 && !(stream_mode_len == 2 && memchr(stream->mode, 'b', 2))) {
			php_error_docref(NULL, E_WARNING, "Cannot use stream opened in mode '%s'", stream->mode);
			RETURN_FALSE;
		} else if (stream_mode_len == 1 && stream->mode[0] != 'r' && stream->mode[0] != 'w' && stream->mode[0] != 'a' && stream->mode[0] != 'x') {
			php_error_docref(NULL, E_WARNING, "Cannot use stream opened in mode '%s'", stream->mode);
			RETURN_FALSE;
		}

		switch(mode[0]) {
			case 'r':
				/* only "r" and "rb" are supported */
				if (stream->mode[0] != mode[0] && !(stream_mode_len == 2 && stream->mode[1] != mode[0])) {
					php_error_docref(NULL, E_WARNING, "Cannot read from a stream opened in write only mode");
					RETURN_FALSE;
				}
				break;
			case 'w':
				/* support only "w"(b), "a"(b), "x"(b) */
				if (stream->mode[0] != mode[0] && !(stream_mode_len == 2 && stream->mode[1] != mode[0])
					&& stream->mode[0] != 'a' && !(stream_mode_len == 2 && stream->mode[1] != 'a')
					&& stream->mode[0] != 'x' && !(stream_mode_len == 2 && stream->mode[1] != 'x')) {
					php_error_docref(NULL, E_WARNING, "cannot write to a stream opened in read only mode");
					RETURN_FALSE;
				}
				break;
			EMPTY_SWITCH_DEFAULT_CASE();
		}

		if (FAILURE == php_stream_cast(stream, PHP_STREAM_AS_FD, (void *) &fd, REPORT_ERRORS)) {
			RETURN_FALSE;
		}

		bz = BZ2_bzdopen((int)fd, mode);

		stream = php_stream_bz2open_from_BZFILE(bz, mode, stream);
	} else {
		zend_argument_type_error(1, "must be of type string or file-resource, %s given", zend_zval_value_name(file));
		RETURN_THROWS();
	}

	if (stream) {
		php_stream_to_zval(stream, return_value);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ Returns the error number */
PHP_FUNCTION(bzerrno)
{
	php_bz2_error(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_BZ_ERRNO);
}
/* }}} */

/* {{{ Returns the error string */
PHP_FUNCTION(bzerrstr)
{
	php_bz2_error(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_BZ_ERRSTR);
}
/* }}} */

/* {{{ Returns the error number and error string in an associative array */
PHP_FUNCTION(bzerror)
{
	php_bz2_error(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_BZ_ERRBOTH);
}
/* }}} */

/* {{{ Compresses a string into BZip2 encoded data */
PHP_FUNCTION(bzcompress)
{
	char             *source;          /* Source data to compress */
	zend_long              zblock_size = 0; /* Optional block size to use */
	zend_long              zwork_factor = 0;/* Optional work factor to use */
	zend_string      *dest = NULL;     /* Destination to place the compressed data into */
	int               error,           /* Error Container */
					  block_size  = 4, /* Block size for compression algorithm */
					  work_factor = 0, /* Work factor for compression algorithm */
					  argc = ZEND_NUM_ARGS(); /* Argument count */
	size_t               source_len;      /* Length of the source data */
	unsigned int      dest_len;        /* Length of the destination buffer */

	if (zend_parse_parameters(argc, "s|ll", &source, &source_len, &zblock_size, &zwork_factor) == FAILURE) {
		RETURN_THROWS();
	}

	/* Assign them to easy to use variables, dest_len is initially the length of the data
	   + .01 x length of data + 600 which is the largest size the results of the compression
	   could possibly be, at least that's what the libbz2 docs say (thanks to jeremy@nirvani.net
	   for pointing this out).  */
	dest_len = (unsigned int) (source_len + (0.01 * source_len) + 600);

	/* Allocate the destination buffer */
	dest = zend_string_alloc(dest_len, 0);

	/* Handle the optional arguments */
	if (argc > 1) {
		block_size = zblock_size;
	}

	if (argc > 2) {
		work_factor = zwork_factor;
	}

	error = BZ2_bzBuffToBuffCompress(ZSTR_VAL(dest), &dest_len, source, source_len, block_size, 0, work_factor);
	if (error != BZ_OK) {
		zend_string_efree(dest);
		RETURN_LONG(error);
	} else {
		/* Copy the buffer, we have perhaps allocate a lot more than we need,
		   so we erealloc() the buffer to the proper size */
		ZSTR_LEN(dest) = dest_len;
		ZSTR_VAL(dest)[ZSTR_LEN(dest)] = '\0';
		RETURN_NEW_STR(dest);
	}
}
/* }}} */

/* {{{ Decompresses BZip2 compressed data */
PHP_FUNCTION(bzdecompress)
{
	char *source;
	zend_string *dest;
	size_t source_len;
	int error;
	bool small = 0;
#ifdef PHP_WIN32
	unsigned __int64 size = 0;
#else
	unsigned long long size = 0;
#endif
	bz_stream bzs;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(), "s|b", &source, &source_len, &small)) {
		RETURN_THROWS();
	}

	bzs.bzalloc = NULL;
	bzs.bzfree = NULL;

	if (BZ2_bzDecompressInit(&bzs, 0, (int)small) != BZ_OK) {
		RETURN_FALSE;
	}

	bzs.next_in = source;
	bzs.avail_in = source_len;

	/* in most cases bz2 offers at least 2:1 compression, so we use that as our base */
	dest = zend_string_safe_alloc(source_len, 2, 1, 0);
	bzs.avail_out = source_len * 2;
	bzs.next_out = ZSTR_VAL(dest);

	while ((error = BZ2_bzDecompress(&bzs)) == BZ_OK && bzs.avail_in > 0) {
		/* compression is better then 2:1, need to allocate more memory */
		bzs.avail_out = source_len;
		size = (bzs.total_out_hi32 * (unsigned int) -1) + bzs.total_out_lo32;
#ifndef ZEND_ENABLE_ZVAL_LONG64
		if (size > SIZE_MAX) {
			/* no reason to continue if we're going to drop it anyway */
			break;
		}
#endif
		dest = zend_string_safe_realloc(dest, 1, bzs.avail_out+1, (size_t) size, 0);
		bzs.next_out = ZSTR_VAL(dest) + size;
	}

	if (error == BZ_STREAM_END || error == BZ_OK) {
		size = (bzs.total_out_hi32 * (unsigned int) -1) + bzs.total_out_lo32;
#ifndef ZEND_ENABLE_ZVAL_LONG64
		if (UNEXPECTED(size > SIZE_MAX)) {
			php_error_docref(NULL, E_WARNING, "Decompressed size too big, max is %zd", SIZE_MAX);
			zend_string_efree(dest);
			RETVAL_LONG(BZ_MEM_ERROR);
		} else
#endif
		{
			dest = zend_string_safe_realloc(dest, 1, (size_t)size, 1, 0);
			ZSTR_LEN(dest) = (size_t)size;
			ZSTR_VAL(dest)[(size_t)size] = '\0';
			RETVAL_STR(dest);
		}
	} else { /* real error */
		zend_string_efree(dest);
		RETVAL_LONG(error);
	}

	BZ2_bzDecompressEnd(&bzs);
}
/* }}} */

/* {{{ php_bz2_error()
   The central error handling interface, does the work for bzerrno, bzerrstr and bzerror */
static void php_bz2_error(INTERNAL_FUNCTION_PARAMETERS, int opt)
{
	zval         *bzp;     /* BZip2 Resource Pointer */
	php_stream   *stream;
	const char   *errstr;  /* Error string */
	int           errnum;  /* Error number */
	struct php_bz2_stream_data_t *self;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &bzp) == FAILURE) {
		RETURN_THROWS();
	}

	php_stream_from_zval(stream, bzp);

	if (!php_stream_is(stream, PHP_STREAM_IS_BZIP2)) {
		zend_argument_type_error(1, "must be a bz2 stream");
		RETURN_THROWS();
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
			RETURN_STRING((char*)errstr);
			break;
		case PHP_BZ_ERRBOTH:
			array_init(return_value);

			add_assoc_long  (return_value, "errno",  errnum);
			add_assoc_string(return_value, "errstr", (char*)errstr);
			break;
	}
}
/* }}} */

#endif
