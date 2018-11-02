/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2018 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   |          Stefan Röhrich <sr@linux.de>                                |
   |          Zeev Suraski <zeev@php.net>                                 |
   |          Jade Nicoletti <nicoletti@nns.ch>                           |
   |          Michael Wallner <mike@php.net>                              |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "SAPI.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "ext/standard/file.h"
#include "ext/standard/php_string.h"
#include "php_zlib.h"

/*
 * zlib include files can define the following preprocessor defines which rename
 * the corresponding PHP functions to gzopen64, gzseek64 and gztell64 and thereby
 * breaking some software, most notably PEAR's Archive_Tar, which halts execution
 * without error message on gzip compressed archivesa.
 *
 * This only seems to happen on 32bit systems with large file support.
 */
#undef gzopen
#undef gzseek
#undef gztell

int le_deflate;
int le_inflate;

ZEND_DECLARE_MODULE_GLOBALS(zlib);

/* {{{ Memory management wrappers */

static voidpf php_zlib_alloc(voidpf opaque, uInt items, uInt size)
{
	return (voidpf)safe_emalloc(items, size, 0);
}

static void php_zlib_free(voidpf opaque, voidpf address)
{
	efree((void*)address);
}
/* }}} */

/* {{{ Incremental deflate/inflate resource destructors */

void deflate_rsrc_dtor(zend_resource *res)
{
	z_stream *ctx = zend_fetch_resource(res, NULL, le_deflate);
	deflateEnd(ctx);
	efree(ctx);
}

void inflate_rsrc_dtor(zend_resource *res)
{
	z_stream *ctx = zend_fetch_resource(res, NULL, le_inflate);
	if (((php_zlib_context *) ctx)->inflateDict) {
		efree(((php_zlib_context *) ctx)->inflateDict);
	}
	inflateEnd(ctx);
	efree(ctx);
}

/* }}} */

/* {{{ php_zlib_output_conflict_check() */
static int php_zlib_output_conflict_check(const char *handler_name, size_t handler_name_len)
{
	if (php_output_get_level() > 0) {
		if (php_output_handler_conflict(handler_name, handler_name_len, ZEND_STRL(PHP_ZLIB_OUTPUT_HANDLER_NAME))
		||	php_output_handler_conflict(handler_name, handler_name_len, ZEND_STRL("ob_gzhandler"))
		||  php_output_handler_conflict(handler_name, handler_name_len, ZEND_STRL("mb_output_handler"))
		||	php_output_handler_conflict(handler_name, handler_name_len, ZEND_STRL("URL-Rewriter"))) {
			return FAILURE;
		}
	}
	return SUCCESS;
}
/* }}} */

/* {{{ php_zlib_output_encoding() */
static int php_zlib_output_encoding(void)
{
	zval *enc;

	if (!ZLIBG(compression_coding)) {
		if ((Z_TYPE(PG(http_globals)[TRACK_VARS_SERVER]) == IS_ARRAY || zend_is_auto_global_str(ZEND_STRL("_SERVER"))) &&
			(enc = zend_hash_str_find(Z_ARRVAL(PG(http_globals)[TRACK_VARS_SERVER]), "HTTP_ACCEPT_ENCODING", sizeof("HTTP_ACCEPT_ENCODING") - 1))) {
			convert_to_string(enc);
			if (strstr(Z_STRVAL_P(enc), "gzip")) {
				ZLIBG(compression_coding) = PHP_ZLIB_ENCODING_GZIP;
			} else if (strstr(Z_STRVAL_P(enc), "deflate")) {
				ZLIBG(compression_coding) = PHP_ZLIB_ENCODING_DEFLATE;
			}
		}
	}
	return ZLIBG(compression_coding);
}
/* }}} */

/* {{{ php_zlib_output_handler_ex() */
static int php_zlib_output_handler_ex(php_zlib_context *ctx, php_output_context *output_context)
{
	int flags = Z_SYNC_FLUSH;

	if (output_context->op & PHP_OUTPUT_HANDLER_START) {
		/* start up */
		if (Z_OK != deflateInit2(&ctx->Z, ZLIBG(output_compression_level), Z_DEFLATED, ZLIBG(compression_coding), MAX_MEM_LEVEL, Z_DEFAULT_STRATEGY)) {
			return FAILURE;
		}
	}

	if (output_context->op & PHP_OUTPUT_HANDLER_CLEAN) {
		/* free buffers */
		deflateEnd(&ctx->Z);

		if (output_context->op & PHP_OUTPUT_HANDLER_FINAL) {
			/* discard */
			return SUCCESS;
		} else {
			/* restart */
			if (Z_OK != deflateInit2(&ctx->Z, ZLIBG(output_compression_level), Z_DEFLATED, ZLIBG(compression_coding), MAX_MEM_LEVEL, Z_DEFAULT_STRATEGY)) {
				return FAILURE;
			}
			ctx->buffer.used = 0;
		}
	} else {
		if (output_context->in.used) {
			/* append input */
			if (ctx->buffer.free < output_context->in.used) {
				if (!(ctx->buffer.aptr = erealloc_recoverable(ctx->buffer.data, ctx->buffer.used + ctx->buffer.free + output_context->in.used))) {
					deflateEnd(&ctx->Z);
					return FAILURE;
				}
				ctx->buffer.data = ctx->buffer.aptr;
				ctx->buffer.free += output_context->in.used;
			}
			memcpy(ctx->buffer.data + ctx->buffer.used, output_context->in.data, output_context->in.used);
			ctx->buffer.free -= output_context->in.used;
			ctx->buffer.used += output_context->in.used;
		}
		output_context->out.size = PHP_ZLIB_BUFFER_SIZE_GUESS(output_context->in.used);
		output_context->out.data = emalloc(output_context->out.size);
		output_context->out.free = 1;
		output_context->out.used = 0;

		ctx->Z.avail_in = ctx->buffer.used;
		ctx->Z.next_in = (Bytef *) ctx->buffer.data;
		ctx->Z.avail_out = output_context->out.size;
		ctx->Z.next_out = (Bytef *) output_context->out.data;

		if (output_context->op & PHP_OUTPUT_HANDLER_FINAL) {
			flags = Z_FINISH;
		} else if (output_context->op & PHP_OUTPUT_HANDLER_FLUSH) {
			flags = Z_FULL_FLUSH;
		}

		switch (deflate(&ctx->Z, flags)) {
			case Z_OK:
				if (flags == Z_FINISH) {
					deflateEnd(&ctx->Z);
					return FAILURE;
				}
			case Z_STREAM_END:
				if (ctx->Z.avail_in) {
					memmove(ctx->buffer.data, ctx->buffer.data + ctx->buffer.used - ctx->Z.avail_in, ctx->Z.avail_in);
				}
				ctx->buffer.free += ctx->buffer.used - ctx->Z.avail_in;
				ctx->buffer.used = ctx->Z.avail_in;
				output_context->out.used = output_context->out.size - ctx->Z.avail_out;
				break;
			default:
				deflateEnd(&ctx->Z);
				return FAILURE;
		}

		if (output_context->op & PHP_OUTPUT_HANDLER_FINAL) {
			deflateEnd(&ctx->Z);
		}
	}

	return SUCCESS;
}
/* }}} */

/* {{{ php_zlib_output_handler() */
static int php_zlib_output_handler(void **handler_context, php_output_context *output_context)
{
	php_zlib_context *ctx = *(php_zlib_context **) handler_context;

	if (!php_zlib_output_encoding()) {
		/* "Vary: Accept-Encoding" header sent along uncompressed content breaks caching in MSIE,
			so let's just send it with successfully compressed content or unless the complete
			buffer gets discarded, see http://bugs.php.net/40325;

			Test as follows:
			+Vary: $ HTTP_ACCEPT_ENCODING=gzip ./sapi/cgi/php <<<'<?php ob_start("ob_gzhandler"); echo "foo\n";'
			+Vary: $ HTTP_ACCEPT_ENCODING= ./sapi/cgi/php <<<'<?php ob_start("ob_gzhandler"); echo "foo\n";'
			-Vary: $ HTTP_ACCEPT_ENCODING=gzip ./sapi/cgi/php <<<'<?php ob_start("ob_gzhandler"); echo "foo\n"; ob_end_clean();'
			-Vary: $ HTTP_ACCEPT_ENCODING= ./sapi/cgi/php <<<'<?php ob_start("ob_gzhandler"); echo "foo\n"; ob_end_clean();'
		*/
		if ((output_context->op & PHP_OUTPUT_HANDLER_START)
		&&	(output_context->op != (PHP_OUTPUT_HANDLER_START|PHP_OUTPUT_HANDLER_CLEAN|PHP_OUTPUT_HANDLER_FINAL))
		) {
			sapi_add_header_ex(ZEND_STRL("Vary: Accept-Encoding"), 1, 0);
		}
		return FAILURE;
	}

	if (SUCCESS != php_zlib_output_handler_ex(ctx, output_context)) {
		return FAILURE;
	}

	if (!(output_context->op & PHP_OUTPUT_HANDLER_CLEAN)) {
		int flags;

		if (SUCCESS == php_output_handler_hook(PHP_OUTPUT_HANDLER_HOOK_GET_FLAGS, &flags)) {
			/* only run this once */
			if (!(flags & PHP_OUTPUT_HANDLER_STARTED)) {
				if (SG(headers_sent) || !ZLIBG(output_compression)) {
					deflateEnd(&ctx->Z);
					return FAILURE;
				}
				switch (ZLIBG(compression_coding)) {
					case PHP_ZLIB_ENCODING_GZIP:
						sapi_add_header_ex(ZEND_STRL("Content-Encoding: gzip"), 1, 1);
						break;
					case PHP_ZLIB_ENCODING_DEFLATE:
						sapi_add_header_ex(ZEND_STRL("Content-Encoding: deflate"), 1, 1);
						break;
					default:
						deflateEnd(&ctx->Z);
						return FAILURE;
				}
				sapi_add_header_ex(ZEND_STRL("Vary: Accept-Encoding"), 1, 0);
				php_output_handler_hook(PHP_OUTPUT_HANDLER_HOOK_IMMUTABLE, NULL);
			}
		}
	}

	return SUCCESS;
}
/* }}} */

/* {{{ php_zlib_output_handler_context_init() */
static php_zlib_context *php_zlib_output_handler_context_init(void)
{
	php_zlib_context *ctx = (php_zlib_context *) ecalloc(1, sizeof(php_zlib_context));
	ctx->Z.zalloc = php_zlib_alloc;
	ctx->Z.zfree = php_zlib_free;
	return ctx;
}
/* }}} */

/* {{{ php_zlib_output_handler_context_dtor() */
static void php_zlib_output_handler_context_dtor(void *opaq)
{
	php_zlib_context *ctx = (php_zlib_context *) opaq;

	if (ctx) {
		if (ctx->buffer.data) {
			efree(ctx->buffer.data);
		}
		efree(ctx);
	}
}
/* }}} */

/* {{{ php_zlib_output_handler_init() */
static php_output_handler *php_zlib_output_handler_init(const char *handler_name, size_t handler_name_len, size_t chunk_size, int flags)
{
	php_output_handler *h = NULL;

	if (!ZLIBG(output_compression)) {
		ZLIBG(output_compression) = chunk_size ? chunk_size : PHP_OUTPUT_HANDLER_DEFAULT_SIZE;
	}

    ZLIBG(handler_registered) = 1;

	if ((h = php_output_handler_create_internal(handler_name, handler_name_len, php_zlib_output_handler, chunk_size, flags))) {
		php_output_handler_set_context(h, php_zlib_output_handler_context_init(), php_zlib_output_handler_context_dtor);
	}

	return h;
}
/* }}} */

/* {{{ php_zlib_output_compression_start() */
static void php_zlib_output_compression_start(void)
{
	zval zoh;
	php_output_handler *h;

	switch (ZLIBG(output_compression)) {
		case 0:
			break;
		case 1:
			ZLIBG(output_compression) = PHP_OUTPUT_HANDLER_DEFAULT_SIZE;
			/* break omitted intentionally */
		default:
			if (	php_zlib_output_encoding() &&
					(h = php_zlib_output_handler_init(ZEND_STRL(PHP_ZLIB_OUTPUT_HANDLER_NAME), ZLIBG(output_compression), PHP_OUTPUT_HANDLER_STDFLAGS)) &&
					(SUCCESS == php_output_handler_start(h))) {
				if (ZLIBG(output_handler) && *ZLIBG(output_handler)) {
					ZVAL_STRING(&zoh, ZLIBG(output_handler));
					php_output_start_user(&zoh, ZLIBG(output_compression), PHP_OUTPUT_HANDLER_STDFLAGS);
					zval_ptr_dtor(&zoh);
				}
			}
			break;
	}
}
/* }}} */

/* {{{ php_zlib_encode() */
static zend_string *php_zlib_encode(const char *in_buf, size_t in_len, int encoding, int level)
{
	int status;
	z_stream Z;
	zend_string *out;

	memset(&Z, 0, sizeof(z_stream));
	Z.zalloc = php_zlib_alloc;
	Z.zfree = php_zlib_free;

	if (Z_OK == (status = deflateInit2(&Z, level, Z_DEFLATED, encoding, MAX_MEM_LEVEL, Z_DEFAULT_STRATEGY))) {
		out = zend_string_alloc(PHP_ZLIB_BUFFER_SIZE_GUESS(in_len), 0);

		Z.next_in = (Bytef *) in_buf;
		Z.next_out = (Bytef *) ZSTR_VAL(out);
		Z.avail_in = in_len;
		Z.avail_out = ZSTR_LEN(out);

		status = deflate(&Z, Z_FINISH);
		deflateEnd(&Z);

		if (Z_STREAM_END == status) {
			/* size buffer down to actual length */
			out = zend_string_truncate(out, Z.total_out, 0);
			ZSTR_VAL(out)[ZSTR_LEN(out)] = '\0';
			return out;
		} else {
			zend_string_efree(out);
		}
	}

	php_error_docref(NULL, E_WARNING, "%s", zError(status));
	return NULL;
}
/* }}} */

/* {{{ php_zlib_inflate_rounds() */
static inline int php_zlib_inflate_rounds(z_stream *Z, size_t max, char **buf, size_t *len)
{
	int status, round = 0;
	php_zlib_buffer buffer = {NULL, NULL, 0, 0, 0};

	*buf = NULL;
	*len = 0;

	buffer.size = (max && (max < Z->avail_in)) ? max : Z->avail_in;

	do {
		if ((max && (max <= buffer.used)) || !(buffer.aptr = erealloc_recoverable(buffer.data, buffer.size))) {
			status = Z_MEM_ERROR;
		} else {
			buffer.data = buffer.aptr;
			Z->avail_out = buffer.free = buffer.size - buffer.used;
			Z->next_out = (Bytef *) buffer.data + buffer.used;
#if 0
			fprintf(stderr, "\n%3d: %3d PRIOR: size=%7lu,\tfree=%7lu,\tused=%7lu,\tavail_in=%7lu,\tavail_out=%7lu\n", round, status, buffer.size, buffer.free, buffer.used, Z->avail_in, Z->avail_out);
#endif
			status = inflate(Z, Z_NO_FLUSH);

			buffer.used += buffer.free - Z->avail_out;
			buffer.free = Z->avail_out;
#if 0
			fprintf(stderr, "%3d: %3d AFTER: size=%7lu,\tfree=%7lu,\tused=%7lu,\tavail_in=%7lu,\tavail_out=%7lu\n", round, status, buffer.size, buffer.free, buffer.used, Z->avail_in, Z->avail_out);
#endif
			buffer.size += (buffer.size >> 3) + 1;
		}
	} while ((Z_BUF_ERROR == status || (Z_OK == status && Z->avail_in)) && ++round < 100);

	if (status == Z_STREAM_END) {
		buffer.data = erealloc(buffer.data, buffer.used + 1);
		buffer.data[buffer.used] = '\0';
		*buf = buffer.data;
		*len = buffer.used;
	} else {
		if (buffer.data) {
			efree(buffer.data);
		}
		/* HACK: See zlib/examples/zpipe.c inf() function for explanation. */
		/* This works as long as this function is not used for streaming. Required to catch very short invalid data. */
		status = (status == Z_OK) ? Z_DATA_ERROR : status;
	}
	return status;
}
/* }}} */

/* {{{ php_zlib_decode() */
static int php_zlib_decode(const char *in_buf, size_t in_len, char **out_buf, size_t *out_len, int encoding, size_t max_len)
{
	int status = Z_DATA_ERROR;
	z_stream Z;

	memset(&Z, 0, sizeof(z_stream));
	Z.zalloc = php_zlib_alloc;
	Z.zfree = php_zlib_free;

	if (in_len) {
retry_raw_inflate:
		status = inflateInit2(&Z, encoding);
		if (Z_OK == status) {
			Z.next_in = (Bytef *) in_buf;
			Z.avail_in = in_len + 1; /* NOTE: data must be zero terminated */

			switch (status = php_zlib_inflate_rounds(&Z, max_len, out_buf, out_len)) {
				case Z_STREAM_END:
					inflateEnd(&Z);
					return SUCCESS;

				case Z_DATA_ERROR:
					/* raw deflated data? */
					if (PHP_ZLIB_ENCODING_ANY == encoding) {
						inflateEnd(&Z);
						encoding = PHP_ZLIB_ENCODING_RAW;
						goto retry_raw_inflate;
					}
			}
			inflateEnd(&Z);
		}
	}

	*out_buf = NULL;
	*out_len = 0;

	php_error_docref(NULL, E_WARNING, "%s", zError(status));
	return FAILURE;
}
/* }}} */

/* {{{ php_zlib_cleanup_ob_gzhandler_mess() */
static void php_zlib_cleanup_ob_gzhandler_mess(void)
{
	if (ZLIBG(ob_gzhandler)) {
		deflateEnd(&(ZLIBG(ob_gzhandler)->Z));
		php_zlib_output_handler_context_dtor(ZLIBG(ob_gzhandler));
		ZLIBG(ob_gzhandler) = NULL;
	}
}
/* }}} */

/* {{{ proto string ob_gzhandler(string data, int flags)
   Legacy hack */
static PHP_FUNCTION(ob_gzhandler)
{
	char *in_str;
	size_t in_len;
	zend_long flags = 0;
	php_output_context ctx = {0};
	int encoding, rv;

	/*
	 * NOTE that the real ob_gzhandler is an alias to "zlib output compression".
	 * This is a really bad hack, because
	 * - we have to initialize a php_zlib_context on demand
	 * - we have to clean it up in RSHUTDOWN
	 * - OG(running) is not set or set to any other output handler
	 * - we have to mess around with php_output_context */

	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS(), "sl", &in_str, &in_len, &flags)) {
		RETURN_FALSE;
	}

	if (!(encoding = php_zlib_output_encoding())) {
		RETURN_FALSE;
	}

	if (flags & PHP_OUTPUT_HANDLER_START) {
		switch (encoding) {
			case PHP_ZLIB_ENCODING_GZIP:
				sapi_add_header_ex(ZEND_STRL("Content-Encoding: gzip"), 1, 1);
				break;
			case PHP_ZLIB_ENCODING_DEFLATE:
				sapi_add_header_ex(ZEND_STRL("Content-Encoding: deflate"), 1, 1);
				break;
		}
		sapi_add_header_ex(ZEND_STRL("Vary: Accept-Encoding"), 1, 0);
	}

	if (!ZLIBG(ob_gzhandler)) {
		ZLIBG(ob_gzhandler) = php_zlib_output_handler_context_init();
	}

	ctx.op = flags;
	ctx.in.data = in_str;
	ctx.in.used = in_len;

	rv = php_zlib_output_handler_ex(ZLIBG(ob_gzhandler), &ctx);

	if (SUCCESS != rv) {
		if (ctx.out.data && ctx.out.free) {
			efree(ctx.out.data);
		}
		php_zlib_cleanup_ob_gzhandler_mess();
		RETURN_FALSE;
	}

	if (ctx.out.data) {
		RETVAL_STRINGL(ctx.out.data, ctx.out.used);
		if (ctx.out.free) {
			efree(ctx.out.data);
		}
	} else {
		RETVAL_EMPTY_STRING();
	}
}
/* }}} */

/* {{{ proto string zlib_get_coding_type(void)
   Returns the coding type used for output compression */
static PHP_FUNCTION(zlib_get_coding_type)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	switch (ZLIBG(compression_coding)) {
		case PHP_ZLIB_ENCODING_GZIP:
			RETURN_STRINGL("gzip", sizeof("gzip") - 1);
		case PHP_ZLIB_ENCODING_DEFLATE:
			RETURN_STRINGL("deflate", sizeof("deflate") - 1);
		default:
			RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto array gzfile(string filename [, int use_include_path])
   Read and uncompress entire .gz-file into an array */
static PHP_FUNCTION(gzfile)
{
	char *filename;
	size_t filename_len;
	int flags = REPORT_ERRORS;
	char buf[8192] = {0};
	register int i = 0;
	zend_long use_include_path = 0;
	php_stream *stream;

	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS(), "p|l", &filename, &filename_len, &use_include_path)) {
		return;
	}

	if (use_include_path) {
		flags |= USE_PATH;
	}

	/* using a stream here is a bit more efficient (resource wise) than php_gzopen_wrapper */
	stream = php_stream_gzopen(NULL, filename, "rb", flags, NULL, NULL STREAMS_CC);

	if (!stream) {
		/* Error reporting is already done by stream code */
		RETURN_FALSE;
	}

	/* Initialize return array */
	array_init(return_value);

	/* Now loop through the file and do the magic quotes thing if needed */
	memset(buf, 0, sizeof(buf));

	while (php_stream_gets(stream, buf, sizeof(buf) - 1) != NULL) {
		add_index_string(return_value, i++, buf);
	}
	php_stream_close(stream);
}
/* }}} */

/* {{{ proto resource gzopen(string filename, string mode [, int use_include_path])
   Open a .gz-file and return a .gz-file pointer */
static PHP_FUNCTION(gzopen)
{
	char *filename;
	char *mode;
	size_t filename_len, mode_len;
	int flags = REPORT_ERRORS;
	php_stream *stream;
	zend_long use_include_path = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ps|l", &filename, &filename_len, &mode, &mode_len, &use_include_path) == FAILURE) {
		return;
	}

	if (use_include_path) {
		flags |= USE_PATH;
	}

	stream = php_stream_gzopen(NULL, filename, mode, flags, NULL, NULL STREAMS_CC);

	if (!stream) {
		RETURN_FALSE;
	}
	php_stream_to_zval(stream, return_value);
}
/* }}} */

/* {{{ proto int readgzfile(string filename [, int use_include_path])
   Output a .gz-file */
static PHP_FUNCTION(readgzfile)
{
	char *filename;
	size_t filename_len;
	int flags = REPORT_ERRORS;
	php_stream *stream;
	size_t size;
	zend_long use_include_path = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "p|l", &filename, &filename_len, &use_include_path) == FAILURE) {
		return;
	}

	if (use_include_path) {
		flags |= USE_PATH;
	}

	stream = php_stream_gzopen(NULL, filename, "rb", flags, NULL, NULL STREAMS_CC);

	if (!stream) {
		RETURN_FALSE;
	}
	size = php_stream_passthru(stream);
	php_stream_close(stream);
	RETURN_LONG(size);
}
/* }}} */

#define PHP_ZLIB_ENCODE_FUNC(name, default_encoding) \
static PHP_FUNCTION(name) \
{ \
	zend_string *in, *out; \
	zend_long level = -1; \
	zend_long encoding = default_encoding; \
	if (default_encoding) { \
		if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS(), "S|ll", &in, &level, &encoding)) { \
			return; \
		} \
	} else { \
		if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS(), "Sl|l", &in, &encoding, &level)) { \
			return; \
		} \
	} \
	if (level < -1 || level > 9) { \
		php_error_docref(NULL, E_WARNING, "compression level (" ZEND_LONG_FMT ") must be within -1..9", level); \
		RETURN_FALSE; \
	} \
	switch (encoding) { \
		case PHP_ZLIB_ENCODING_RAW: \
		case PHP_ZLIB_ENCODING_GZIP: \
		case PHP_ZLIB_ENCODING_DEFLATE: \
			break; \
		default: \
			php_error_docref(NULL, E_WARNING, "encoding mode must be either ZLIB_ENCODING_RAW, ZLIB_ENCODING_GZIP or ZLIB_ENCODING_DEFLATE"); \
			RETURN_FALSE; \
	} \
	if ((out = php_zlib_encode(ZSTR_VAL(in), ZSTR_LEN(in), encoding, level)) == NULL) { \
		RETURN_FALSE; \
	} \
	RETURN_STR(out); \
}

#define PHP_ZLIB_DECODE_FUNC(name, encoding) \
static PHP_FUNCTION(name) \
{ \
	char *in_buf, *out_buf; \
	size_t in_len; \
	size_t out_len; \
	zend_long max_len = 0; \
	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS(), "s|l", &in_buf, &in_len, &max_len)) { \
		return; \
	} \
	if (max_len < 0) { \
		php_error_docref(NULL, E_WARNING, "length (" ZEND_LONG_FMT ") must be greater or equal zero", max_len); \
		RETURN_FALSE; \
	} \
	if (SUCCESS != php_zlib_decode(in_buf, in_len, &out_buf, &out_len, encoding, max_len)) { \
		RETURN_FALSE; \
	} \
	RETVAL_STRINGL(out_buf, out_len); \
	efree(out_buf); \
}

/* {{{ proto binary zlib_encode(binary data, int encoding[, int level = -1])
   Compress data with the specified encoding */
PHP_ZLIB_ENCODE_FUNC(zlib_encode, 0);
/* }}} */

/* {{{ proto binary zlib_decode(binary data[, int max_decoded_len])
   Uncompress any raw/gzip/zlib encoded data */
PHP_ZLIB_DECODE_FUNC(zlib_decode, PHP_ZLIB_ENCODING_ANY);
/* }}} */

/* NOTE: The naming of these userland functions was quite unlucky */
/* {{{ proto binary gzdeflate(binary data[, int level = -1[, int encoding = ZLIB_ENCODING_RAW])
   Encode data with the raw deflate encoding */
PHP_ZLIB_ENCODE_FUNC(gzdeflate, PHP_ZLIB_ENCODING_RAW);
/* }}} */

/* {{{ proto binary gzencode(binary data[, int level = -1[, int encoding = ZLIB_ENCODING_GZIP])
   Encode data with the gzip encoding */
PHP_ZLIB_ENCODE_FUNC(gzencode, PHP_ZLIB_ENCODING_GZIP);
/* }}} */

/* {{{ proto binary gzcompress(binary data[, int level = -1[, int encoding = ZLIB_ENCODING_DEFLATE])
   Encode data with the zlib encoding */
PHP_ZLIB_ENCODE_FUNC(gzcompress, PHP_ZLIB_ENCODING_DEFLATE);
/* }}} */

/* {{{ proto binary gzinflate(binary data[, int max_decoded_len])
   Decode raw deflate encoded data */
PHP_ZLIB_DECODE_FUNC(gzinflate, PHP_ZLIB_ENCODING_RAW);
/* }}} */

/* {{{ proto binary gzdecode(binary data[, int max_decoded_len])
   Decode gzip encoded data */
PHP_ZLIB_DECODE_FUNC(gzdecode, PHP_ZLIB_ENCODING_GZIP);
/* }}} */

/* {{{ proto binary gzuncompress(binary data[, int max_decoded_len])
   Decode zlib encoded data */
PHP_ZLIB_DECODE_FUNC(gzuncompress, PHP_ZLIB_ENCODING_DEFLATE);
/* }}} */

static zend_bool zlib_create_dictionary_string(HashTable *options, char **dict, size_t *dictlen) {
	zval *option_buffer;

	if (options && (option_buffer = zend_hash_str_find(options, ZEND_STRL("dictionary"))) != NULL) {
		ZVAL_DEREF(option_buffer);
		switch (Z_TYPE_P(option_buffer)) {
			case IS_STRING: {
				zend_string *str = Z_STR_P(option_buffer);
				*dict = emalloc(ZSTR_LEN(str));
				memcpy(*dict, ZSTR_VAL(str), ZSTR_LEN(str));
				*dictlen = ZSTR_LEN(str);
			} break;

			case IS_ARRAY: {
				HashTable *dictionary = Z_ARR_P(option_buffer);

				if (zend_hash_num_elements(dictionary) > 0) {
					char *dictptr;
					zval *cur;
					zend_string **strings = emalloc(sizeof(zend_string *) * zend_hash_num_elements(dictionary));
					zend_string **end, **ptr = strings - 1;

					ZEND_HASH_FOREACH_VAL(dictionary, cur) {
						size_t i;

						*++ptr = zval_get_string(cur);
						if (!*ptr || ZSTR_LEN(*ptr) == 0) {
							if (*ptr) {
								efree(*ptr);
							}
							while (--ptr >= strings) {
								efree(ptr);
							}
							efree(strings);
							php_error_docref(NULL, E_WARNING, "dictionary entries must be non-empty strings");
							return 0;
						}
						for (i = 0; i < ZSTR_LEN(*ptr); i++) {
							if (ZSTR_VAL(*ptr)[i] == 0) {
								do {
									efree(ptr);
								} while (--ptr >= strings);
								efree(strings);
								php_error_docref(NULL, E_WARNING, "dictionary entries must not contain a NULL-byte");
								return 0;
							}
						}

						*dictlen += ZSTR_LEN(*ptr) + 1;
					} ZEND_HASH_FOREACH_END();

					dictptr = *dict = emalloc(*dictlen);
					ptr = strings;
					end = strings + zend_hash_num_elements(dictionary);
					do {
						memcpy(dictptr, ZSTR_VAL(*ptr), ZSTR_LEN(*ptr));
						dictptr += ZSTR_LEN(*ptr);
						*dictptr++ = 0;
						zend_string_release_ex(*ptr, 0);
					} while (++ptr != end);
					efree(strings);
				}
			} break;

			default:
				php_error_docref(NULL, E_WARNING, "dictionary must be of type zero-terminated string or array, got %s", zend_get_type_by_const(Z_TYPE_P(option_buffer)));
				return 0;
		}
	}

	return 1;
}

/* {{{ proto resource inflate_init(int encoding)
   Initialize an incremental inflate context with the specified encoding */
PHP_FUNCTION(inflate_init)
{
	z_stream *ctx;
	zend_long encoding, window = 15;
	char *dict = NULL;
	size_t dictlen = 0;
	HashTable *options = NULL;
	zval *option_buffer;

	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS(), "l|H", &encoding, &options)) {
		return;
	}

	if (options && (option_buffer = zend_hash_str_find(options, ZEND_STRL("window"))) != NULL) {
		window = zval_get_long(option_buffer);
	}
	if (window < 8 || window > 15) {
		php_error_docref(NULL, E_WARNING, "zlib window size (lograithm) (" ZEND_LONG_FMT ") must be within 8..15", window);
		RETURN_FALSE;
	}

	if (!zlib_create_dictionary_string(options, &dict, &dictlen)) {
		RETURN_FALSE;
	}

	switch (encoding) {
		case PHP_ZLIB_ENCODING_RAW:
		case PHP_ZLIB_ENCODING_GZIP:
		case PHP_ZLIB_ENCODING_DEFLATE:
			break;
		default:
			php_error_docref(NULL, E_WARNING, "encoding mode must be ZLIB_ENCODING_RAW, ZLIB_ENCODING_GZIP or ZLIB_ENCODING_DEFLATE");
			RETURN_FALSE;
	}

	ctx = ecalloc(1, sizeof(php_zlib_context));
	ctx->zalloc = php_zlib_alloc;
	ctx->zfree = php_zlib_free;
	((php_zlib_context *) ctx)->inflateDict = dict;
	((php_zlib_context *) ctx)->inflateDictlen = dictlen;
	((php_zlib_context *) ctx)->status = Z_OK;

	if (encoding < 0) {
		encoding += 15 - window;
	} else {
		encoding -= 15 - window;
	}

	if (Z_OK == inflateInit2(ctx, encoding)) {
		if (encoding == PHP_ZLIB_ENCODING_RAW && dictlen > 0) {
			php_zlib_context *php_ctx = (php_zlib_context *) ctx;
			switch (inflateSetDictionary(ctx, (Bytef *) php_ctx->inflateDict, php_ctx->inflateDictlen)) {
				case Z_OK:
					efree(php_ctx->inflateDict);
					php_ctx->inflateDict = NULL;
					break;
				case Z_DATA_ERROR:
					php_error_docref(NULL, E_WARNING, "dictionary does not match expected dictionary (incorrect adler32 hash)");
					efree(php_ctx->inflateDict);
					php_ctx->inflateDict = NULL;
					RETURN_FALSE;
				EMPTY_SWITCH_DEFAULT_CASE()
			}
		}
		RETURN_RES(zend_register_resource(ctx, le_inflate));
	} else {
		efree(ctx);
		php_error_docref(NULL, E_WARNING, "failed allocating zlib.inflate context");
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto string inflate_add(resource context, string encoded_data[, int flush_mode = ZLIB_SYNC_FLUSH])
   Incrementally inflate encoded data in the specified context */
PHP_FUNCTION(inflate_add)
{
	zend_string *out;
	char *in_buf;
	size_t in_len, buffer_used = 0, CHUNK_SIZE = 8192;
	zval *res;
	z_stream *ctx;
	zend_long flush_type = Z_SYNC_FLUSH;
	int status;

	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS(), "rs|l", &res, &in_buf, &in_len, &flush_type)) {
		return;
	}

	if (!(ctx = zend_fetch_resource_ex(res, NULL, le_inflate))) {
		php_error_docref(NULL, E_WARNING, "Invalid zlib.inflate resource");
		RETURN_FALSE;
	}

	switch (flush_type) {
		case Z_NO_FLUSH:
		case Z_PARTIAL_FLUSH:
		case Z_SYNC_FLUSH:
		case Z_FULL_FLUSH:
		case Z_BLOCK:
		case Z_FINISH:
			break;

		default:
			php_error_docref(NULL, E_WARNING,
				"flush mode must be ZLIB_NO_FLUSH, ZLIB_PARTIAL_FLUSH, ZLIB_SYNC_FLUSH, ZLIB_FULL_FLUSH, ZLIB_BLOCK or ZLIB_FINISH");
			RETURN_FALSE;
	}

	/* Lazy-resetting the zlib stream so ctx->total_in remains available until the next inflate_add() call. */
	if (((php_zlib_context *) ctx)->status == Z_STREAM_END)
	{
		((php_zlib_context *) ctx)->status = Z_OK;
		inflateReset(ctx);
	}

	if (in_len <= 0 && flush_type != Z_FINISH) {
		RETURN_EMPTY_STRING();
	}

	out = zend_string_alloc((in_len > CHUNK_SIZE) ? in_len : CHUNK_SIZE, 0);
	ctx->next_in = (Bytef *) in_buf;
	ctx->next_out = (Bytef *) ZSTR_VAL(out);
	ctx->avail_in = in_len;
	ctx->avail_out = ZSTR_LEN(out);

	do {
		status = inflate(ctx, flush_type);
		buffer_used = ZSTR_LEN(out) - ctx->avail_out;

		((php_zlib_context *) ctx)->status = status; /* Save status for exposing to userspace */

		switch (status) {
			case Z_OK:
				if (ctx->avail_out == 0) {
					/* more output buffer space needed; realloc and try again */
					out = zend_string_realloc(out, ZSTR_LEN(out) + CHUNK_SIZE, 0);
					ctx->avail_out = CHUNK_SIZE;
					ctx->next_out = (Bytef *) ZSTR_VAL(out) + buffer_used;
					break;
				} else {
					goto complete;
				}
			case Z_STREAM_END:
				goto complete;
			case Z_BUF_ERROR:
				if (flush_type == Z_FINISH && ctx->avail_out == 0) {
					/* more output buffer space needed; realloc and try again */
					out = zend_string_realloc(out, ZSTR_LEN(out) + CHUNK_SIZE, 0);
					ctx->avail_out = CHUNK_SIZE;
					ctx->next_out = (Bytef *) ZSTR_VAL(out) + buffer_used;
					break;
				} else {
					/* No more input data; we're finished */
					goto complete;
				}
			case Z_NEED_DICT:
				if (((php_zlib_context *) ctx)->inflateDict) {
					php_zlib_context *php_ctx = (php_zlib_context *) ctx;
					switch (inflateSetDictionary(ctx, (Bytef *) php_ctx->inflateDict, php_ctx->inflateDictlen)) {
						case Z_OK:
							efree(php_ctx->inflateDict);
							php_ctx->inflateDict = NULL;
							break;
						case Z_DATA_ERROR:
							php_error_docref(NULL, E_WARNING, "dictionary does not match expected dictionary (incorrect adler32 hash)");
							efree(php_ctx->inflateDict);
							zend_string_release_ex(out, 0);
							php_ctx->inflateDict = NULL;
							RETURN_FALSE;
						EMPTY_SWITCH_DEFAULT_CASE()
					}
					break;
				} else {
					php_error_docref(NULL, E_WARNING, "inflating this data requires a preset dictionary, please specify it in the options array of inflate_init()");
					RETURN_FALSE;
				}
			default:
				zend_string_release_ex(out, 0);
				php_error_docref(NULL, E_WARNING, "%s", zError(status));
				RETURN_FALSE;
		}
	} while (1);

	complete: {
		out = zend_string_realloc(out, buffer_used, 0);
		ZSTR_VAL(out)[buffer_used] = 0;
		RETURN_STR(out);
	}
}
/* }}} */

/* {{{ proto bool inflate_get_status(resource context)
   Get decompression status, usually returns either ZLIB_OK or ZLIB_STREAM_END. */
PHP_FUNCTION(inflate_get_status)
{
	zval *res;
	z_stream *ctx;

	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS(), "r", &res))
	{
		RETURN_NULL();
	}

	if (!(ctx = zend_fetch_resource_ex(res, NULL, le_inflate))) {
		php_error_docref(NULL, E_WARNING, "Invalid zlib.inflate resource");
		RETURN_FALSE;
	}

	RETURN_LONG(((php_zlib_context *) ctx)->status);
}
/* }}} */

/* {{{ proto bool inflate_get_read_len(resource context)
   Get number of bytes read so far. */
PHP_FUNCTION(inflate_get_read_len)
{
	zval *res;
	z_stream *ctx;

	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS(), "r", &res))
	{
		RETURN_NULL();
	}

	if (!(ctx = zend_fetch_resource_ex(res, NULL, le_inflate))) {
		php_error_docref(NULL, E_WARNING, "Invalid zlib.inflate resource");
		RETURN_FALSE;
	}

	RETURN_LONG(ctx->total_in);
}
/* }}} */

/* {{{ proto resource deflate_init(int encoding[, array options])
   Initialize an incremental deflate context using the specified encoding */
PHP_FUNCTION(deflate_init)
{
	z_stream *ctx;
	zend_long encoding, level = -1, memory = 8, window = 15, strategy = Z_DEFAULT_STRATEGY;
	char *dict = NULL;
	size_t dictlen = 0;
	HashTable *options = NULL;
	zval *option_buffer;

	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS(), "l|H", &encoding, &options)) {
		return;
	}

	if (options && (option_buffer = zend_hash_str_find(options, ZEND_STRL("level"))) != NULL) {
		level = zval_get_long(option_buffer);
	}
	if (level < -1 || level > 9) {
		php_error_docref(NULL, E_WARNING, "compression level (" ZEND_LONG_FMT ") must be within -1..9", level);
		RETURN_FALSE;
	}

	if (options && (option_buffer = zend_hash_str_find(options, ZEND_STRL("memory"))) != NULL) {
		memory = zval_get_long(option_buffer);
	}
	if (memory < 1 || memory > 9) {
		php_error_docref(NULL, E_WARNING, "compression memory level (" ZEND_LONG_FMT ") must be within 1..9", memory);
		RETURN_FALSE;
	}

	if (options && (option_buffer = zend_hash_str_find(options, ZEND_STRL("window"))) != NULL) {
		window = zval_get_long(option_buffer);
	}
	if (window < 8 || window > 15) {
		php_error_docref(NULL, E_WARNING, "zlib window size (logarithm) (" ZEND_LONG_FMT ") must be within 8..15", window);
		RETURN_FALSE;
	}

	if (options && (option_buffer = zend_hash_str_find(options, ZEND_STRL("strategy"))) != NULL) {
		strategy = zval_get_long(option_buffer);
	}
	switch (strategy) {
		case Z_FILTERED:
		case Z_HUFFMAN_ONLY:
		case Z_RLE:
		case Z_FIXED:
		case Z_DEFAULT_STRATEGY:
			break;
		default:
			php_error_docref(NULL, E_WARNING, "strategy must be one of ZLIB_FILTERED, ZLIB_HUFFMAN_ONLY, ZLIB_RLE, ZLIB_FIXED or ZLIB_DEFAULT_STRATEGY");
			RETURN_FALSE;
	}

	if (!zlib_create_dictionary_string(options, &dict, &dictlen)) {
		RETURN_FALSE;
	}

	switch (encoding) {
		case PHP_ZLIB_ENCODING_RAW:
		case PHP_ZLIB_ENCODING_GZIP:
		case PHP_ZLIB_ENCODING_DEFLATE:
			break;
		default:
			php_error_docref(NULL, E_WARNING,
				"encoding mode must be ZLIB_ENCODING_RAW, ZLIB_ENCODING_GZIP or ZLIB_ENCODING_DEFLATE");
			RETURN_FALSE;
	}

	ctx = ecalloc(1, sizeof(php_zlib_context));
	ctx->zalloc = php_zlib_alloc;
	ctx->zfree = php_zlib_free;

	if (encoding < 0) {
		encoding += 15 - window;
	} else {
		encoding -= 15 - window;
	}

	if (Z_OK == deflateInit2(ctx, level, Z_DEFLATED, encoding, memory, strategy)) {
		if (dict) {
			int success = deflateSetDictionary(ctx, (Bytef *) dict, dictlen);
			ZEND_ASSERT(success == Z_OK);
			efree(dict);
		}

		RETURN_RES(zend_register_resource(ctx, le_deflate));
	} else {
		efree(ctx);
		php_error_docref(NULL, E_WARNING, "failed allocating zlib.deflate context");
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto string deflate_add(resource context, string data[, int flush_mode = ZLIB_SYNC_FLUSH])
   Incrementally deflate data in the specified context */
PHP_FUNCTION(deflate_add)
{
	zend_string *out;
	char *in_buf;
	size_t in_len, out_size, buffer_used;
	zval *res;
	z_stream *ctx;
	zend_long flush_type = Z_SYNC_FLUSH;
	int status;

	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS(), "rs|l", &res, &in_buf, &in_len, &flush_type)) {
		return;
	}

	if (!(ctx = zend_fetch_resource_ex(res, NULL, le_deflate))) {
		php_error_docref(NULL, E_WARNING, "Invalid deflate resource");
		RETURN_FALSE;
	}

	switch (flush_type) {
		case Z_BLOCK:
#if ZLIB_VERNUM < 0x1240L
			php_error_docref(NULL, E_WARNING,
				"zlib >= 1.2.4 required for BLOCK deflate; current version: %s", ZLIB_VERSION);
			RETURN_FALSE;
#endif
		case Z_NO_FLUSH:
		case Z_PARTIAL_FLUSH:
		case Z_SYNC_FLUSH:
		case Z_FULL_FLUSH:
		case Z_FINISH:
			break;

		default:
			php_error_docref(NULL, E_WARNING,
				"flush mode must be ZLIB_NO_FLUSH, ZLIB_PARTIAL_FLUSH, ZLIB_SYNC_FLUSH, ZLIB_FULL_FLUSH, ZLIB_BLOCK or ZLIB_FINISH");
			RETURN_FALSE;
	}

	if (in_len <= 0 && flush_type != Z_FINISH) {
		RETURN_EMPTY_STRING();
	}

	out_size = PHP_ZLIB_BUFFER_SIZE_GUESS(in_len);
	out_size = (out_size < 64) ? 64 : out_size;
	out = zend_string_alloc(out_size, 0);

	ctx->next_in = (Bytef *) in_buf;
	ctx->next_out = (Bytef *) ZSTR_VAL(out);
	ctx->avail_in = in_len;
	ctx->avail_out = ZSTR_LEN(out);

	buffer_used = 0;

	do {
		if (ctx->avail_out == 0) {
			/* more output buffer space needed; realloc and try again */
			/* adding 64 more bytes solved every issue I have seen    */
			out = zend_string_realloc(out, ZSTR_LEN(out) + 64, 0);
			ctx->avail_out = 64;
			ctx->next_out = (Bytef *) ZSTR_VAL(out) + buffer_used;
		}
		status = deflate(ctx, flush_type);
		buffer_used = ZSTR_LEN(out) - ctx->avail_out;
	} while (status == Z_OK && ctx->avail_out == 0);

	switch (status) {
		case Z_OK:
			ZSTR_LEN(out) = (char *) ctx->next_out - ZSTR_VAL(out);
			ZSTR_VAL(out)[ZSTR_LEN(out)] = 0;
			RETURN_STR(out);
			break;
		case Z_STREAM_END:
			ZSTR_LEN(out) = (char *) ctx->next_out - ZSTR_VAL(out);
			ZSTR_VAL(out)[ZSTR_LEN(out)] = 0;
			deflateReset(ctx);
			RETURN_STR(out);
			break;
		default:
			zend_string_release_ex(out, 0);
			php_error_docref(NULL, E_WARNING, "zlib error (%s)", zError(status));
			RETURN_FALSE;
	}
}
/* }}} */

#ifdef COMPILE_DL_ZLIB
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(php_zlib)
#endif

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_ob_gzhandler, 0, 0, 2)
	ZEND_ARG_INFO(0, data)
	ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_zlib_get_coding_type, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gzfile, 0, 0, 1)
	ZEND_ARG_INFO(0, filename)
	ZEND_ARG_INFO(0, use_include_path)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gzopen, 0, 0, 2)
	ZEND_ARG_INFO(0, filename)
	ZEND_ARG_INFO(0, mode)
	ZEND_ARG_INFO(0, use_include_path)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_readgzfile, 0, 0, 1)
	ZEND_ARG_INFO(0, filename)
	ZEND_ARG_INFO(0, use_include_path)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_zlib_encode, 0, 0, 2)
	ZEND_ARG_INFO(0, data)
	ZEND_ARG_INFO(0, encoding)
	ZEND_ARG_INFO(0, level)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_zlib_decode, 0, 0, 1)
	ZEND_ARG_INFO(0, data)
	ZEND_ARG_INFO(0, max_decoded_len)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gzdeflate, 0, 0, 1)
	ZEND_ARG_INFO(0, data)
	ZEND_ARG_INFO(0, level)
	ZEND_ARG_INFO(0, encoding)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gzencode, 0, 0, 1)
	ZEND_ARG_INFO(0, data)
	ZEND_ARG_INFO(0, level)
	ZEND_ARG_INFO(0, encoding)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gzcompress, 0, 0, 1)
	ZEND_ARG_INFO(0, data)
	ZEND_ARG_INFO(0, level)
	ZEND_ARG_INFO(0, encoding)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gzinflate, 0, 0, 1)
	ZEND_ARG_INFO(0, data)
	ZEND_ARG_INFO(0, max_decoded_len)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gzdecode, 0, 0, 1)
	ZEND_ARG_INFO(0, data)
	ZEND_ARG_INFO(0, max_decoded_len)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gzuncompress, 0, 0, 1)
	ZEND_ARG_INFO(0, data)
	ZEND_ARG_INFO(0, max_decoded_len)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gzputs, 0, 0, 2)
	ZEND_ARG_INFO(0, fp)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_gzpassthru, 0)
	ZEND_ARG_INFO(0, fp)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gzseek, 0, 0, 2)
	ZEND_ARG_INFO(0, fp)
	ZEND_ARG_INFO(0, offset)
	ZEND_ARG_INFO(0, whence)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_gzread, 0)
	ZEND_ARG_INFO(0, fp)
	ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gzgetss, 0, 0, 1)
	ZEND_ARG_INFO(0, fp)
	ZEND_ARG_INFO(0, length)
	ZEND_ARG_INFO(0, allowable_tags)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gzgets, 0, 0, 1)
	ZEND_ARG_INFO(0, fp)
	ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_deflate_init, 0, 0, 1)
	ZEND_ARG_INFO(0, encoding)
	ZEND_ARG_INFO(0, level)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_deflate_add, 0, 0, 2)
	ZEND_ARG_INFO(0, resource)
	ZEND_ARG_INFO(0, add)
	ZEND_ARG_INFO(0, flush_behavior)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_inflate_init, 0, 0, 1)
	ZEND_ARG_INFO(0, encoding)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_inflate_add, 0, 0, 2)
	ZEND_ARG_INFO(0, context)
	ZEND_ARG_INFO(0, encoded_data)
	ZEND_ARG_INFO(0, flush_mode)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_inflate_get_status, 0, 0, 1)
	ZEND_ARG_INFO(0, resource)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_inflate_get_read_len, 0, 0, 1)
	ZEND_ARG_INFO(0, resource)
ZEND_END_ARG_INFO()

/* }}} */

/* {{{ php_zlib_functions[] */
static const zend_function_entry php_zlib_functions[] = {
	PHP_FE(readgzfile,						arginfo_readgzfile)
	PHP_FALIAS(gzrewind,	rewind,			arginfo_gzpassthru)
	PHP_FALIAS(gzclose,		fclose,			arginfo_gzpassthru)
	PHP_FALIAS(gzeof,		feof,			arginfo_gzpassthru)
	PHP_FALIAS(gzgetc,		fgetc,			arginfo_gzpassthru)
	PHP_FALIAS(gzgets,		fgets,			arginfo_gzgets)
	PHP_DEP_FALIAS(gzgetss,	fgetss,			arginfo_gzgetss)
	PHP_FALIAS(gzread,		fread,			arginfo_gzread)
	PHP_FE(gzopen,							arginfo_gzopen)
	PHP_FALIAS(gzpassthru,	fpassthru,		arginfo_gzpassthru)
	PHP_FALIAS(gzseek,		fseek,			arginfo_gzseek)
	PHP_FALIAS(gztell,		ftell,			arginfo_gzpassthru)
	PHP_FALIAS(gzwrite,		fwrite,			arginfo_gzputs)
	PHP_FALIAS(gzputs,		fwrite,			arginfo_gzputs)
	PHP_FE(gzfile,							arginfo_gzfile)
	PHP_FE(gzcompress,						arginfo_gzcompress)
	PHP_FE(gzuncompress,					arginfo_gzuncompress)
	PHP_FE(gzdeflate,						arginfo_gzdeflate)
	PHP_FE(gzinflate,						arginfo_gzinflate)
	PHP_FE(gzencode,						arginfo_gzencode)
	PHP_FE(gzdecode,						arginfo_gzdecode)
	PHP_FE(zlib_encode,						arginfo_zlib_encode)
	PHP_FE(zlib_decode,						arginfo_zlib_decode)
	PHP_FE(zlib_get_coding_type,			arginfo_zlib_get_coding_type)
	PHP_FE(deflate_init,					arginfo_deflate_init)
	PHP_FE(deflate_add,						arginfo_deflate_add)
	PHP_FE(inflate_init,					arginfo_inflate_init)
	PHP_FE(inflate_add,						arginfo_inflate_add)
	PHP_FE(inflate_get_status,				arginfo_inflate_get_status)
	PHP_FE(inflate_get_read_len,				arginfo_inflate_get_read_len)
	PHP_FE(ob_gzhandler,					arginfo_ob_gzhandler)
	PHP_FE_END
};
/* }}} */

/* {{{ OnUpdate_zlib_output_compression */
static PHP_INI_MH(OnUpdate_zlib_output_compression)
{
	int int_value;
	char *ini_value;
	zend_long *p;
#ifndef ZTS
	char *base = (char *) mh_arg2;
#else
	char *base;

	base = (char *) ts_resource(*((int *) mh_arg2));
#endif

	if (new_value == NULL) {
		return FAILURE;
	}

	if (!strncasecmp(ZSTR_VAL(new_value), "off", sizeof("off"))) {
		int_value = 0;
	} else if (!strncasecmp(ZSTR_VAL(new_value), "on", sizeof("on"))) {
		int_value = 1;
	} else {
		int_value = zend_atoi(ZSTR_VAL(new_value), ZSTR_LEN(new_value));
	}
	ini_value = zend_ini_string("output_handler", sizeof("output_handler"), 0);

	if (ini_value && *ini_value && int_value) {
		php_error_docref("ref.outcontrol", E_CORE_ERROR, "Cannot use both zlib.output_compression and output_handler together!!");
		return FAILURE;
	}
	if (stage == PHP_INI_STAGE_RUNTIME) {
		int status = php_output_get_status();
		if (status & PHP_OUTPUT_SENT) {
			php_error_docref("ref.outcontrol", E_WARNING, "Cannot change zlib.output_compression - headers already sent");
			return FAILURE;
		}
	}

	p = (zend_long *) (base+(size_t) mh_arg1);
	*p = int_value;

	ZLIBG(output_compression) = ZLIBG(output_compression_default);
	if (stage == PHP_INI_STAGE_RUNTIME && int_value) {
		if (!php_output_handler_started(ZEND_STRL(PHP_ZLIB_OUTPUT_HANDLER_NAME))) {
			php_zlib_output_compression_start();
		}
	}

	return SUCCESS;
}
/* }}} */

/* {{{ OnUpdate_zlib_output_handler */
static PHP_INI_MH(OnUpdate_zlib_output_handler)
{
	if (stage == PHP_INI_STAGE_RUNTIME && (php_output_get_status() & PHP_OUTPUT_SENT)) {
		php_error_docref("ref.outcontrol", E_WARNING, "Cannot change zlib.output_handler - headers already sent");
		return FAILURE;
	}

	return OnUpdateString(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage);
}
/* }}} */

/* {{{ INI */
PHP_INI_BEGIN()
	STD_PHP_INI_BOOLEAN("zlib.output_compression",      "0", PHP_INI_ALL, OnUpdate_zlib_output_compression,       output_compression_default,       zend_zlib_globals, zlib_globals)
	STD_PHP_INI_ENTRY("zlib.output_compression_level", "-1", PHP_INI_ALL, OnUpdateLong,                           output_compression_level, zend_zlib_globals, zlib_globals)
	STD_PHP_INI_ENTRY("zlib.output_handler",             "", PHP_INI_ALL, OnUpdate_zlib_output_handler,           output_handler,           zend_zlib_globals, zlib_globals)
PHP_INI_END()

/* }}} */

/* {{{ PHP_MINIT_FUNCTION */
static PHP_MINIT_FUNCTION(zlib)
{
	php_register_url_stream_wrapper("compress.zlib", &php_stream_gzip_wrapper);
	php_stream_filter_register_factory("zlib.*", &php_zlib_filter_factory);

	php_output_handler_alias_register(ZEND_STRL("ob_gzhandler"), php_zlib_output_handler_init);
	php_output_handler_conflict_register(ZEND_STRL("ob_gzhandler"), php_zlib_output_conflict_check);
	php_output_handler_conflict_register(ZEND_STRL(PHP_ZLIB_OUTPUT_HANDLER_NAME), php_zlib_output_conflict_check);

	le_deflate = zend_register_list_destructors_ex(deflate_rsrc_dtor, NULL, "zlib.deflate", module_number);
	le_inflate = zend_register_list_destructors_ex(inflate_rsrc_dtor, NULL, "zlib.inflate", module_number);

	REGISTER_LONG_CONSTANT("FORCE_GZIP", PHP_ZLIB_ENCODING_GZIP, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FORCE_DEFLATE", PHP_ZLIB_ENCODING_DEFLATE, CONST_CS|CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("ZLIB_ENCODING_RAW", PHP_ZLIB_ENCODING_RAW, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ZLIB_ENCODING_GZIP", PHP_ZLIB_ENCODING_GZIP, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ZLIB_ENCODING_DEFLATE", PHP_ZLIB_ENCODING_DEFLATE, CONST_CS|CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("ZLIB_NO_FLUSH", Z_NO_FLUSH, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ZLIB_PARTIAL_FLUSH", Z_PARTIAL_FLUSH, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ZLIB_SYNC_FLUSH", Z_SYNC_FLUSH, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ZLIB_FULL_FLUSH", Z_FULL_FLUSH, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ZLIB_BLOCK", Z_BLOCK, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ZLIB_FINISH", Z_FINISH, CONST_CS|CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("ZLIB_FILTERED", Z_FILTERED, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ZLIB_HUFFMAN_ONLY", Z_HUFFMAN_ONLY, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ZLIB_RLE", Z_RLE, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ZLIB_FIXED", Z_FIXED, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ZLIB_DEFAULT_STRATEGY", Z_DEFAULT_STRATEGY, CONST_CS|CONST_PERSISTENT);

	REGISTER_STRING_CONSTANT("ZLIB_VERSION", ZLIB_VERSION, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ZLIB_VERNUM", ZLIB_VERNUM, CONST_CS|CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("ZLIB_OK", Z_OK, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ZLIB_STREAM_END", Z_STREAM_END, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ZLIB_NEED_DICT", Z_NEED_DICT, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ZLIB_ERRNO", Z_ERRNO, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ZLIB_STREAM_ERROR", Z_STREAM_ERROR, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ZLIB_DATA_ERROR", Z_DATA_ERROR, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ZLIB_MEM_ERROR", Z_MEM_ERROR, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ZLIB_BUF_ERROR", Z_BUF_ERROR, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ZLIB_VERSION_ERROR", Z_VERSION_ERROR, CONST_CS|CONST_PERSISTENT);

	REGISTER_INI_ENTRIES();
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION */
static PHP_MSHUTDOWN_FUNCTION(zlib)
{
	php_unregister_url_stream_wrapper("zlib");
	php_stream_filter_unregister_factory("zlib.*");

	UNREGISTER_INI_ENTRIES();

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION */
static PHP_RINIT_FUNCTION(zlib)
{
	ZLIBG(compression_coding) = 0;
    if (!ZLIBG(handler_registered)) {
        ZLIBG(output_compression) = ZLIBG(output_compression_default);
        php_zlib_output_compression_start();
    }

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION */
static PHP_RSHUTDOWN_FUNCTION(zlib)
{
	php_zlib_cleanup_ob_gzhandler_mess();
    ZLIBG(handler_registered) = 0;

    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
static PHP_MINFO_FUNCTION(zlib)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "ZLib Support", "enabled");
	php_info_print_table_row(2, "Stream Wrapper", "compress.zlib://");
	php_info_print_table_row(2, "Stream Filter", "zlib.inflate, zlib.deflate");
	php_info_print_table_row(2, "Compiled Version", ZLIB_VERSION);
	php_info_print_table_row(2, "Linked Version", (char *) zlibVersion());
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}
/* }}} */

/* {{{ ZEND_MODULE_GLOBALS_CTOR */
static PHP_GINIT_FUNCTION(zlib)
{
#if defined(COMPILE_DL_ZLIB) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	zlib_globals->ob_gzhandler = NULL;
    zlib_globals->handler_registered = 0;
}
/* }}} */

/* {{{ php_zlib_module_entry */
zend_module_entry php_zlib_module_entry = {
	STANDARD_MODULE_HEADER,
	"zlib",
	php_zlib_functions,
	PHP_MINIT(zlib),
	PHP_MSHUTDOWN(zlib),
	PHP_RINIT(zlib),
	PHP_RSHUTDOWN(zlib),
	PHP_MINFO(zlib),
	PHP_ZLIB_VERSION,
	PHP_MODULE_GLOBALS(zlib),
	PHP_GINIT(zlib),
	NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
