/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2009 The PHP Group                                |
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
   |          Zeev Suraski <zeev@zend.com>                                |
   |          Jade Nicoletti <nicoletti@nns.ch>                           |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "SAPI.h"
#include "php_ini.h"

#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifdef PHP_WIN32
#define O_RDONLY _O_RDONLY
#include "win32/param.h"
#else
#include <sys/param.h>
/* #include <sys/uio.h> */
#endif
#include "ext/standard/head.h"
#include "safe_mode.h"
#include "ext/standard/php_standard.h"
#include "ext/standard/info.h"
#include "php_zlib.h"
#include "fopen_wrappers.h"
#if HAVE_PWD_H
#ifdef PHP_WIN32
#include "win32/pwd.h"
#else
#include <pwd.h>
#endif
#endif
#if defined(HAVE_UNISTD_H) && defined(PHP_WIN32)
#undef HAVE_UNISTD_H
#endif

#ifdef COMPILE_DL_ZLIB
#ifndef PUTS
#define PUTS(a) php_printf("%s",a)
#endif
#ifndef PUTC
#define PUTC(a) PUTS(a)
#endif
#ifndef PHPWRITE
#define PHPWRITE(a,n) php_write((a),(n) TSRMLS_CC)
#endif
#endif

/* Win32 needs some more memory */
#ifdef PHP_WIN32
#define PHP_ZLIB_MODIFIER 100
#else
#define PHP_ZLIB_MODIFIER 1000
#endif

#define OS_CODE			0x03 /* FIXME */
#define GZIP_HEADER_LENGTH		10
#define GZIP_FOOTER_LENGTH		8

/* True globals, no need for thread safety */
static const int gz_magic[2] = {0x1f, 0x8b};	/* gzip magic header */

static int php_enable_output_compression(int buffer_size TSRMLS_DC);

static PHP_MINIT_FUNCTION(zlib);
static PHP_MSHUTDOWN_FUNCTION(zlib);
static PHP_RINIT_FUNCTION(zlib);
static PHP_MINFO_FUNCTION(zlib);
static PHP_FUNCTION(gzopen);
static PHP_FUNCTION(readgzfile);
static PHP_FUNCTION(gzfile);
static PHP_FUNCTION(gzcompress);
static PHP_FUNCTION(gzuncompress);
static PHP_FUNCTION(gzdeflate);
static PHP_FUNCTION(gzinflate);
static PHP_FUNCTION(gzencode);
static PHP_FUNCTION(ob_gzhandler);
static PHP_FUNCTION(zlib_get_coding_type);

/* {{{ php_zlib_functions[]
 */
static zend_function_entry php_zlib_functions[] = {
	PHP_FE(readgzfile,						NULL)
	PHP_FALIAS(gzrewind,	rewind,			NULL)
	PHP_FALIAS(gzclose,		fclose,			NULL)
	PHP_FALIAS(gzeof,		feof,			NULL)
	PHP_FALIAS(gzgetc,		fgetc,			NULL)
	PHP_FALIAS(gzgets,		fgets,			NULL)
	PHP_FALIAS(gzgetss,		fgetss,			NULL)
	PHP_FALIAS(gzread,		fread,			NULL)
	PHP_FE(gzopen,							NULL)
	PHP_FALIAS(gzpassthru,	fpassthru,		NULL)
	PHP_FALIAS(gzseek,		fseek,			NULL)
	PHP_FALIAS(gztell,		ftell,			NULL)
	PHP_FALIAS(gzwrite,		fwrite,			NULL)
	PHP_FALIAS(gzputs,		fwrite,			NULL)
	PHP_FE(gzfile,							NULL)
	PHP_FE(gzcompress,            			NULL)
	PHP_FE(gzuncompress,           			NULL)
	PHP_FE(gzdeflate,             			NULL)
	PHP_FE(gzinflate,              			NULL)
	PHP_FE(gzencode,						NULL)
	PHP_FE(ob_gzhandler,					NULL)
	PHP_FE(zlib_get_coding_type,			NULL)
	{NULL, NULL, NULL}
};
/* }}} */

ZEND_DECLARE_MODULE_GLOBALS(zlib)

/* {{{ php_zlib_module_entry
 */
zend_module_entry php_zlib_module_entry = {
	STANDARD_MODULE_HEADER,
	"zlib",
	php_zlib_functions,
	PHP_MINIT(zlib),
	PHP_MSHUTDOWN(zlib),
	PHP_RINIT(zlib),
	NULL,
	PHP_MINFO(zlib),
	"1.1",
	PHP_MODULE_GLOBALS(zlib),
	NULL,
	NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};
/* }}} */

#ifdef COMPILE_DL_ZLIB
ZEND_GET_MODULE(php_zlib)
#endif

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

/* {{{ OnUpdate_zlib_output_compression */
static PHP_INI_MH(OnUpdate_zlib_output_compression)
{
	char *ini_value;

	if (new_value == NULL) {
		return FAILURE;
	}
	
	if (!strncasecmp(new_value, "off", sizeof("off"))) {
		new_value = "0";
		new_value_length = sizeof("0");
	} else if (!strncasecmp(new_value, "on", sizeof("on"))) {
		new_value = "1";
		new_value_length = sizeof("1");
	}

	ini_value = zend_ini_string("output_handler", sizeof("output_handler"), 0); 
	if (ini_value != NULL && strlen(ini_value) != 0 && zend_atoi(new_value, new_value_length) != 0) {
		php_error_docref("ref.outcontrol" TSRMLS_CC, E_CORE_ERROR, "Cannot use both zlib.output_compression and output_handler together!!");
		return FAILURE;
	}

	if (stage == PHP_INI_STAGE_RUNTIME && SG(headers_sent) && !SG(request_info).no_headers) {
		php_error_docref("ref.outcontrol" TSRMLS_CC, E_WARNING, "Cannot change zlib.output_compression - headers already sent");
		return FAILURE;
	}

	OnUpdateLong(entry, new_value, new_value_length, mh_arg1, mh_arg2, mh_arg3, stage TSRMLS_CC);

	return SUCCESS;
}
/* }}} */

/* {{{ OnUpdate_zlib_output_compression_level */
static PHP_INI_MH(OnUpdate_zlib_output_compression_level)
{
	OnUpdateLong(entry, new_value, new_value_length, mh_arg1, mh_arg2, mh_arg3, stage TSRMLS_CC);

	return SUCCESS;
}
/* }}} */

/* {{{ OnUpdate_zlib_output_handler */
static PHP_INI_MH(OnUpdate_zlib_output_handler)
{
	if (stage == PHP_INI_STAGE_RUNTIME && SG(headers_sent) && !SG(request_info).no_headers) {
		php_error_docref("ref.outcontrol" TSRMLS_CC, E_WARNING, "Cannot change zlib.output_handler - headers already sent");
		return FAILURE;
	}

	OnUpdateString(entry, new_value, new_value_length, mh_arg1, mh_arg2, mh_arg3, stage TSRMLS_CC);

	return SUCCESS;
}
/* }}} */


PHP_INI_BEGIN()
	STD_PHP_INI_BOOLEAN("zlib.output_compression",      "0", PHP_INI_ALL, OnUpdate_zlib_output_compression,       output_compression,       zend_zlib_globals, zlib_globals)
	STD_PHP_INI_ENTRY("zlib.output_compression_level", "-1", PHP_INI_ALL, OnUpdate_zlib_output_compression_level, output_compression_level, zend_zlib_globals, zlib_globals)
	STD_PHP_INI_ENTRY("zlib.output_handler",             "", PHP_INI_ALL, OnUpdate_zlib_output_handler,           output_handler,           zend_zlib_globals, zlib_globals)
PHP_INI_END()

/* {{{ PHP_MINIT_FUNCTION
 */
static PHP_MINIT_FUNCTION(zlib)
{
	php_register_url_stream_wrapper("compress.zlib", &php_stream_gzip_wrapper TSRMLS_CC);
	php_stream_filter_register_factory("zlib.*", &php_zlib_filter_factory TSRMLS_CC);

	REGISTER_LONG_CONSTANT("FORCE_GZIP", CODING_GZIP, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FORCE_DEFLATE", CODING_DEFLATE, CONST_CS | CONST_PERSISTENT);

	REGISTER_INI_ENTRIES();

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION
 */
static PHP_RINIT_FUNCTION(zlib)
{
	uint chunk_size = ZLIBG(output_compression);

	ZLIBG(ob_gzhandler_status) = 0;
	ZLIBG(compression_coding) = 0;
	if (chunk_size) {
		if (chunk_size == 1) {
			chunk_size = 4096; /* use the default size */
			ZLIBG(output_compression) = chunk_size;
		}
		php_enable_output_compression(chunk_size TSRMLS_CC);
	}
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
static PHP_MSHUTDOWN_FUNCTION(zlib)
{
	php_unregister_url_stream_wrapper("zlib" TSRMLS_CC);
	php_stream_filter_unregister_factory("zlib.*" TSRMLS_CC);
	
	UNREGISTER_INI_ENTRIES();

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
static PHP_MINFO_FUNCTION(zlib)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "ZLib Support", "enabled");
	php_info_print_table_row(2, "Stream Wrapper support", "compress.zlib://");
	php_info_print_table_row(2, "Stream Filter support", "zlib.inflate, zlib.deflate");
	php_info_print_table_row(2, "Compiled Version", ZLIB_VERSION);
	php_info_print_table_row(2, "Linked Version", (char *) zlibVersion());
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}
/* }}} */

/* {{{ proto array gzfile(string filename [, int use_include_path])
   Read und uncompress entire .gz-file into an array */
static PHP_FUNCTION(gzfile)
{
	char *filename;
	int filename_len;
	long flags = 0;
	char *slashed, buf[8192];
	register int i = 0;
	int use_include_path = 0;
	php_stream *stream;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &filename, &filename_len, &flags) == FAILURE) {
		return;
	}

	use_include_path = flags ? USE_PATH : 0;

	/* using a stream here is a bit more efficient (resource wise) than php_gzopen_wrapper */
	stream = php_stream_gzopen(NULL, filename, "rb", use_include_path | ENFORCE_SAFE_MODE | REPORT_ERRORS, NULL, NULL STREAMS_CC TSRMLS_CC);
	if (stream == NULL) {
		/* Error reporting is already done by stream code */
		RETURN_FALSE;
	}

	/* Initialize return array */
	array_init(return_value);

	/* Now loop through the file and do the magic quotes thing if needed */
	memset(buf,0,sizeof(buf));

	while (php_stream_gets(stream, buf, sizeof(buf) - 1) != NULL) {
		if (PG(magic_quotes_runtime)) {
			int len;
			
			slashed = php_addslashes(buf, 0, &len, 0 TSRMLS_CC); /* 0 = don't free source string */
			add_index_stringl(return_value, i++, slashed, len, 0);
		} else {
			add_index_string(return_value, i++, buf, 1);
		}
	}
	php_stream_close(stream);
}
/* }}} */

/* {{{ proto resource gzopen(string filename, string mode [, int use_include_path])
   Open a .gz-file and return a .gz-file pointer */
static PHP_FUNCTION(gzopen)
{
	char *filename, *mode;
	int filename_len, mode_len;
	long flags = 0;
	php_stream *stream;
	int use_include_path = 0;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|l", &filename, &filename_len, &mode, &mode_len, &flags) == FAILURE) {
		return;
	}

	use_include_path = flags ? USE_PATH : 0;

	stream = php_stream_gzopen(NULL, filename, mode, use_include_path | ENFORCE_SAFE_MODE | REPORT_ERRORS, NULL, NULL STREAMS_CC TSRMLS_CC);

	if (!stream) {
		RETURN_FALSE;
	}
	php_stream_to_zval(stream, return_value);
}	
/* }}} */

/*
 * Read a file and write the ouput to stdout
 */
/* {{{ proto int readgzfile(string filename [, int use_include_path])
   Output a .gz-file */
static PHP_FUNCTION(readgzfile)
{
	char *filename;
	int filename_len;
	long flags = 0;
	php_stream *stream;
	int size;
	int use_include_path = 0;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &filename, &filename_len, &flags) == FAILURE) {
		return;
	}

	use_include_path = flags ? USE_PATH : 0;

	stream = php_stream_gzopen(NULL, filename, "rb", use_include_path | ENFORCE_SAFE_MODE, NULL, NULL STREAMS_CC TSRMLS_CC);
	if (!stream) {
		RETURN_FALSE;
	}
	size = php_stream_passthru(stream);
   	php_stream_close(stream);
	RETURN_LONG(size);
}
/* }}} */

/* {{{ proto string gzcompress(string data [, int level]) 
   Gzip-compress a string */
static PHP_FUNCTION(gzcompress)
{
	int data_len, status;
	long level = Z_DEFAULT_COMPRESSION;
	unsigned long l2;
	char *data, *s2;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &data, &data_len, &level) == FAILURE) {
		return;
	}

	if ((level < -1) || (level > 9)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "compression level (%ld) must be within -1..9", level);
		RETURN_FALSE;
	}

	l2 = data_len + (data_len / PHP_ZLIB_MODIFIER) + 15 + 1; /* room for \0 */
	s2 = (char *) emalloc(l2);
	if (!s2) {
		RETURN_FALSE;
	}
		
	if (level >= 0) {
		status = compress2(s2, &l2, data, data_len, level);
	} else {
		status = compress(s2, &l2, data, data_len);
	}
	
	if (status == Z_OK) {
		s2 = erealloc(s2, l2 + 1);
		s2[l2] = '\0';
		RETURN_STRINGL(s2, l2, 0);
	} else {
		efree(s2);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", zError(status));
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto string gzuncompress(string data [, int length]) 
   Unzip a gzip-compressed string */
static PHP_FUNCTION(gzuncompress)
{
	int data_len, status;
	unsigned int factor=1, maxfactor=16;
	long limit = 0;
	unsigned long plength=0, length;
	char *data, *s1=NULL, *s2=NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &data, &data_len, &limit) == FAILURE) {
		return;
	}

	if (limit < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "length (%ld) must be greater or equal zero", limit);
		RETURN_FALSE;
	}
	plength = limit;

	/*
	 zlib::uncompress() wants to know the output data length
	 if none was given as a parameter
	 we try from input length * 2 up to input length * 2^15
	 doubling it whenever it wasn't big enough
	 that should be eneugh for all real life cases	
	*/
	do {
		length = plength ? plength : (unsigned long)data_len * (1 << factor++);
		s2 = (char *) erealloc(s1, length);
		status = uncompress(s2, &length, data, data_len);
		s1 = s2;
	} while ((status == Z_BUF_ERROR) && (!plength) && (factor < maxfactor));

	if (status == Z_OK) {
		s2 = erealloc(s2, length + 1); /* space for \0 */
		s2[ length ] = '\0';
		RETURN_STRINGL(s2, length, 0);
	} else {
		efree(s2);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", zError(status));
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto string gzdeflate(string data [, int level]) 
   Gzip-compress a string */
static PHP_FUNCTION(gzdeflate)
{
	int data_len,status;
	long level = Z_DEFAULT_COMPRESSION;
	z_stream stream;
	char *data, *s2;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &data, &data_len, &level) == FAILURE) {
		return;
	}

	if ((level < -1) || (level > 9)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "compression level (%ld) must be within -1..9", level);
		RETURN_FALSE;
	}

	stream.data_type = Z_ASCII;
	stream.zalloc = php_zlib_alloc;
	stream.zfree  = php_zlib_free;
	stream.opaque = (voidpf) Z_NULL;

	stream.next_in = (Bytef *) data;
	stream.avail_in = data_len;

	stream.avail_out = stream.avail_in + (stream.avail_in / PHP_ZLIB_MODIFIER) + 15 + 1; /* room for \0 */

	s2 = (char *) emalloc(stream.avail_out);
	if (!s2) {
		RETURN_FALSE;
	}
	
	stream.next_out = s2;

	/* init with -MAX_WBITS disables the zlib internal headers */
	status = deflateInit2(&stream, level, Z_DEFLATED, -MAX_WBITS, MAX_MEM_LEVEL, 0);
	if (status == Z_OK) {
		status = deflate(&stream, Z_FINISH);
		if (status != Z_STREAM_END) {
			deflateEnd(&stream);
			if (status == Z_OK) {
				status = Z_BUF_ERROR;
			}
		} else {
			status = deflateEnd(&stream);
		}
	}

	if (status == Z_OK) {
		s2 = erealloc(s2,stream.total_out + 1); /* resize to buffer to the "right" size */
		s2[ stream.total_out ] = '\0';
		RETURN_STRINGL(s2, stream.total_out, 0);
	} else {
		efree(s2);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", zError(status));
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto string gzinflate(string data [, int length]) 
   Unzip a gzip-compressed string */
static PHP_FUNCTION(gzinflate)
{
	int data_len, status;
	unsigned int factor=1, maxfactor=16;
	long limit = 0;
	unsigned long plength=0, length;
	char *data, *s1=NULL, *s2=NULL;
	z_stream stream;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &data, &data_len, &limit) == FAILURE) {
		return;
	}

	if (!data_len) {
		RETURN_FALSE;
	}

	if (limit < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "length (%ld) must be greater or equal zero", limit);
		RETURN_FALSE;
	}
	plength = limit;

	/*
	  stream.avail_out wants to know the output data length
	  if none was given as a parameter
	  we try from input length * 2 up to input length * 2^15
	  doubling it whenever it wasn't big enough
	  that should be enaugh for all real life cases	
	*/

	stream.zalloc = php_zlib_alloc;
	stream.zfree = php_zlib_free;

	do {
		length = plength ? plength : (unsigned long)data_len * (1 << factor++);
		s2 = (char *) erealloc(s1, length);

		if (!s2 && s1) {
			efree(s1);
			RETURN_FALSE;
		}

		stream.next_in = (Bytef *) data;
		stream.avail_in = (uInt) data_len + 1; /* there is room for \0 */

		stream.next_out = s2;
		stream.avail_out = (uInt) length;

		/* init with -MAX_WBITS disables the zlib internal headers */
		status = inflateInit2(&stream, -MAX_WBITS);
		if (status == Z_OK) {
			status = inflate(&stream, Z_FINISH);
			if (status != Z_STREAM_END) {
				inflateEnd(&stream);
				if (status == Z_OK) {
					status = Z_BUF_ERROR;
				}
			} else {
				status = inflateEnd(&stream);
			}
		}
		s1 = s2;
		
	} while ((status == Z_BUF_ERROR) && (!plength) && (factor < maxfactor));

	if (status == Z_OK) {
		s2 = erealloc(s2, stream.total_out + 1); /* room for \0 */
		s2[ stream.total_out ] = '\0';
		RETURN_STRINGL(s2, stream.total_out, 0);
	} else {
		efree(s2);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", zError(status));
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto string zlib_get_coding_type(void)
   Returns the coding type used for output compression */
static PHP_FUNCTION(zlib_get_coding_type)
{
	switch (ZLIBG(compression_coding)) {
		case CODING_GZIP:
			RETURN_STRINGL("gzip", sizeof("gzip") - 1, 1);

		case CODING_DEFLATE:
			RETURN_STRINGL("deflate", sizeof("deflate") - 1, 1);
	}

	RETURN_FALSE;
}

/* {{{ php_do_deflate
 */
static int php_do_deflate(uint str_length, Bytef **p_buffer, uint *p_buffer_len, zend_bool do_start, zend_bool do_end TSRMLS_DC)
{
	Bytef *buffer;
	uInt prev_outlen, outlen;
	int err;
	int start_offset = ((do_start && ZLIBG(compression_coding) == CODING_GZIP) ? 10 : 0);
	int end_offset = (do_end ? 8 : 0);

	outlen = (uint) (str_length + (str_length / PHP_ZLIB_MODIFIER) + 12 + 1); /* leave some room for a trailing \0 */
	if ((outlen + start_offset + end_offset) > *p_buffer_len) {
		buffer = (Bytef *) emalloc(outlen + start_offset + end_offset);
	} else {
		buffer = *p_buffer;
	}
	
	ZLIBG(stream).next_out = buffer + start_offset;
	ZLIBG(stream).avail_out = outlen;

	err = deflate(&ZLIBG(stream), Z_SYNC_FLUSH);
	while (err == Z_OK && !ZLIBG(stream).avail_out) {
		prev_outlen = outlen;
		outlen *= 3;
		if ((outlen + start_offset + end_offset) > *p_buffer_len) {
			buffer = erealloc(buffer, outlen + start_offset + end_offset);
		}
		
		ZLIBG(stream).next_out = buffer + start_offset + prev_outlen;
		ZLIBG(stream).avail_out = prev_outlen * 2;

		err = deflate(&ZLIBG(stream), Z_SYNC_FLUSH);
	}

	if (do_end) {
		err = deflate(&ZLIBG(stream), Z_FINISH);
		buffer[outlen + start_offset - ZLIBG(stream).avail_out] = '\0';
	}

	*p_buffer = buffer;
	*p_buffer_len = outlen - ZLIBG(stream).avail_out;

	return err;
}
/* }}} */

/* {{{ php_deflate_string
 */
static int php_deflate_string(const char *str, uint str_length, char **newstr, uint *new_length, zend_bool do_start, zend_bool do_end TSRMLS_DC)
{
	int err;

	if (do_start) {
		ZLIBG(stream).zalloc = php_zlib_alloc;
		ZLIBG(stream).zfree = php_zlib_free;
		ZLIBG(stream).opaque = Z_NULL;

		switch (ZLIBG(compression_coding)) {
			case CODING_GZIP:
				/* windowBits is passed < 0 to suppress zlib header & trailer */
				if (deflateInit2(&ZLIBG(stream), ZLIBG(output_compression_level), Z_DEFLATED,	-MAX_WBITS, MAX_MEM_LEVEL, Z_DEFAULT_STRATEGY) != Z_OK) {
					/* TODO: print out error */
					return FAILURE;
				}
		
				ZLIBG(crc) = crc32(0L, Z_NULL, 0);
				break;

			case CODING_DEFLATE:
				if (deflateInit(&ZLIBG(stream), ZLIBG(output_compression_level)) != Z_OK) {
					/* TODO: print out error */
					return FAILURE;
				}
				break;		
		}
	}

	ZLIBG(stream).next_in = (Bytef *) str;
	ZLIBG(stream).avail_in = (uInt) str_length;

	if (ZLIBG(compression_coding) == CODING_GZIP) {
		ZLIBG(crc) = crc32(ZLIBG(crc), (const Bytef *) str, str_length);
	}

	err = php_do_deflate(str_length, (Bytef **) newstr, new_length, do_start, do_end TSRMLS_CC);
	/* TODO: error handling (err may be Z_STREAM_ERROR, Z_BUF_ERROR, ?) */

	if (do_start && ZLIBG(compression_coding) == CODING_GZIP) {
		/* Write a very simple .gz header: */
		(*newstr)[0] = gz_magic[0];
		(*newstr)[1] = gz_magic[1];
		(*newstr)[2] = Z_DEFLATED;
		(*newstr)[3] = (*newstr)[4] = (*newstr)[5] = (*newstr)[6] = (*newstr)[7] = (*newstr)[8] = 0;
		(*newstr)[9] = OS_CODE;
		*new_length += 10;
	}
	if (do_end) {
		if (ZLIBG(compression_coding) == CODING_GZIP) {
			char *trailer = (*newstr) + (*new_length);

			/* write crc & stream.total_in in LSB order */
			trailer[0] = (char) ZLIBG(crc) & 0xFF;
			trailer[1] = (char) (ZLIBG(crc) >> 8) & 0xFF;
			trailer[2] = (char) (ZLIBG(crc) >> 16) & 0xFF;
			trailer[3] = (char) (ZLIBG(crc) >> 24) & 0xFF;
			trailer[4] = (char) ZLIBG(stream).total_in & 0xFF;
			trailer[5] = (char) (ZLIBG(stream).total_in >> 8) & 0xFF;
			trailer[6] = (char) (ZLIBG(stream).total_in >> 16) & 0xFF;
			trailer[7] = (char) (ZLIBG(stream).total_in >> 24) & 0xFF;
			trailer[8] = '\0';
			*new_length += 8;
		}
		deflateEnd(&ZLIBG(stream));
	}

	return SUCCESS;
}
/* }}} */

/* {{{ proto string gzencode(string data [, int level [, int encoding_mode]])
   GZ encode a string */
static PHP_FUNCTION(gzencode)
{
	char *data, *s2;
	int data_len;
	long level = Z_DEFAULT_COMPRESSION, coding = CODING_GZIP;
	int status;
	z_stream stream;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|ll", &data, &data_len, &level, &coding) == FAILURE) {
		return;
	}

	if ((level < -1) || (level > 9)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "compression level(%ld) must be within -1..9", level);
		RETURN_FALSE;
	}

	if ((coding != CODING_GZIP) && (coding != CODING_DEFLATE)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "encoding mode must be FORCE_GZIP or FORCE_DEFLATE");
		RETURN_FALSE;
	}

	stream.zalloc = php_zlib_alloc;
	stream.zfree = php_zlib_free;
	stream.opaque = Z_NULL;

	stream.next_in = (Bytef *) data;
	stream.avail_in = data_len;

	stream.avail_out = stream.avail_in + (stream.avail_in / PHP_ZLIB_MODIFIER) + 15 + 1; /* room for \0 */
	s2 = (char *) emalloc(stream.avail_out + GZIP_HEADER_LENGTH + (coding == CODING_GZIP ? GZIP_FOOTER_LENGTH : 0));

	/* add gzip file header */
	s2[0] = gz_magic[0];
	s2[1] = gz_magic[1];
	s2[2] = Z_DEFLATED;
	s2[3] = s2[4] = s2[5] = s2[6] = s2[7] = s2[8] = 0; /* time set to 0 */
	s2[9] = OS_CODE;

	stream.next_out = &(s2[GZIP_HEADER_LENGTH]);

	switch (coding) {
		case CODING_GZIP:
			/* windowBits is passed < 0 to suppress zlib header & trailer */
			if ((status = deflateInit2(&stream, level, Z_DEFLATED, -MAX_WBITS, MAX_MEM_LEVEL, Z_DEFAULT_STRATEGY)) != Z_OK) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", zError(status));
				RETURN_FALSE;
			}
		
			break;
		case CODING_DEFLATE:
			if ((status = deflateInit(&stream, level)) != Z_OK) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", zError(status));
				RETURN_FALSE;
			}
			break;		
	}

	status = deflate(&stream, Z_FINISH);
	if (status != Z_STREAM_END) {
		deflateEnd(&stream);
		if (status == Z_OK) {
			status = Z_BUF_ERROR;
		}
	} else {
		status = deflateEnd(&stream);
	}

	if (status == Z_OK) {
		/* resize to buffer to the "right" size */
		s2 = erealloc(s2, stream.total_out + GZIP_HEADER_LENGTH + (coding == CODING_GZIP ? GZIP_FOOTER_LENGTH : 0) + 1);

		if (coding == CODING_GZIP) {
			char *trailer = s2 + (stream.total_out + GZIP_HEADER_LENGTH);
			uLong crc = crc32(0L, Z_NULL, 0);

			crc = crc32(crc, (const Bytef *) data, data_len);

			/* write crc & stream.total_in in LSB order */
			trailer[0] = (char) crc & 0xFF;
			trailer[1] = (char) (crc >> 8) & 0xFF;
			trailer[2] = (char) (crc >> 16) & 0xFF;
			trailer[3] = (char) (crc >> 24) & 0xFF;
			trailer[4] = (char) stream.total_in & 0xFF;
			trailer[5] = (char) (stream.total_in >> 8) & 0xFF;
			trailer[6] = (char) (stream.total_in >> 16) & 0xFF;
			trailer[7] = (char) (stream.total_in >> 24) & 0xFF;
			trailer[8] = '\0';
		} else {
			s2[stream.total_out + GZIP_HEADER_LENGTH + (coding == CODING_GZIP ? GZIP_FOOTER_LENGTH : 0)] = '\0'; 
		}
		RETURN_STRINGL(s2, stream.total_out + GZIP_HEADER_LENGTH + (coding == CODING_GZIP ? GZIP_FOOTER_LENGTH : 0), 0);
	} else {
		efree(s2);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", zError(status));
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ php_ob_gzhandler_check
 */
int php_ob_gzhandler_check(TSRMLS_D)
{
	/* check for wrong usages */
	if (OG(ob_nesting_level > 0)) {
		if (php_ob_handler_used("ob_gzhandler" TSRMLS_CC)) {
			php_error_docref("ref.outcontrol" TSRMLS_CC, E_WARNING, "output handler 'ob_gzhandler' cannot be used twice");
			return FAILURE;
		}
		if (php_ob_handler_used("mb_output_handler" TSRMLS_CC)) {
			php_error_docref("ref.outcontrol" TSRMLS_CC, E_WARNING, "output handler 'ob_gzhandler' cannot be used after 'mb_output_handler'");
			return FAILURE;
		}
		if (php_ob_handler_used("URL-Rewriter" TSRMLS_CC)) {
			php_error_docref("ref.outcontrol" TSRMLS_CC, E_WARNING, "output handler 'ob_gzhandler' cannot be used after 'URL-Rewriter'");
			return FAILURE;
		}
		if (php_ob_init_conflict("ob_gzhandler", "zlib output compression" TSRMLS_CC)) {
			return FAILURE;
		}
	}

	return SUCCESS;
}

/* }}} */

/* {{{ proto string ob_gzhandler(string str, int mode)
   Encode str based on accept-encoding setting - designed to be called from ob_start() */
static PHP_FUNCTION(ob_gzhandler)
{
	char *string;
	int string_len;
	long mode;
	zval **a_encoding;
	zend_bool return_original = 0;
	zend_bool do_start, do_end;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl", &string, &string_len, &mode) == FAILURE) {
		return;
	}

	if(ZLIBG(ob_gzhandler_status) == -1)
		RETURN_FALSE;

	zend_is_auto_global("_SERVER", sizeof("_SERVER")-1 TSRMLS_CC);

	if (!PG(http_globals)[TRACK_VARS_SERVER]
		|| zend_hash_find(PG(http_globals)[TRACK_VARS_SERVER]->value.ht, "HTTP_ACCEPT_ENCODING", sizeof("HTTP_ACCEPT_ENCODING"), (void **) &a_encoding) == FAILURE
	) {
		ZLIBG(ob_gzhandler_status) = -1;
		RETURN_FALSE;
	}

	convert_to_string_ex(a_encoding);
	if (php_memnstr(Z_STRVAL_PP(a_encoding), "gzip", 4, Z_STRVAL_PP(a_encoding) + Z_STRLEN_PP(a_encoding))) {
		ZLIBG(compression_coding) = CODING_GZIP;
	} else if (php_memnstr(Z_STRVAL_PP(a_encoding), "deflate", 7, Z_STRVAL_PP(a_encoding) + Z_STRLEN_PP(a_encoding))) {
		ZLIBG(compression_coding) = CODING_DEFLATE;
	} else {
		ZLIBG(ob_gzhandler_status) = -1;
		RETURN_FALSE;
	}
	
	do_start = ((mode & PHP_OUTPUT_HANDLER_START) ? 1 : 0);
	do_end = ((mode & PHP_OUTPUT_HANDLER_END) ? 1 : 0);
	Z_STRVAL_P(return_value) = NULL;
	Z_STRLEN_P(return_value) = 0;

	if (php_deflate_string(string, string_len, &Z_STRVAL_P(return_value), &Z_STRLEN_P(return_value), do_start, do_end TSRMLS_CC) == SUCCESS) {
		Z_TYPE_P(return_value) = IS_STRING;
		if (do_start) {
			switch (ZLIBG(compression_coding)) {
				case CODING_GZIP:
					if (sapi_add_header("Content-Encoding: gzip", sizeof("Content-Encoding: gzip") - 1, 1) == FAILURE) {
						return_original = 1;
					}
					if (sapi_add_header_ex("Vary: Accept-Encoding", sizeof("Vary: Accept-Encoding") - 1, 1, 0 TSRMLS_CC)==FAILURE) {
						return_original = 1;
					}
					break;
				case CODING_DEFLATE:
					if (sapi_add_header("Content-Encoding: deflate", sizeof("Content-Encoding: deflate") - 1, 1) == FAILURE) {
						return_original = 1;
					}
					if (sapi_add_header_ex("Vary: Accept-Encoding", sizeof("Vary: Accept-Encoding") - 1, 1, 0 TSRMLS_CC)==FAILURE) {
						return_original = 1;
					}
					break;
				default:
					return_original = 1;
					break;
			}
		}

		if (return_original) {
			zval_dtor(return_value);
		}

	} else {
		return_original = 1;
	}

	if (return_original) {
		/* return the original string */
		RETURN_STRINGL(string, string_len, 1);
	}
}
/* }}} */

/* {{{ php_gzip_output_handler
 */
static void php_gzip_output_handler(char *output, uint output_len, char **handled_output, uint *handled_output_len, int mode TSRMLS_DC)
{
	zend_bool do_start, do_end;

	if (!ZLIBG(output_compression)) {
		*handled_output = NULL;
	} else {
		do_start = (mode & PHP_OUTPUT_HANDLER_START ? 1 : 0);
		do_end = (mode & PHP_OUTPUT_HANDLER_END ? 1 : 0);
		if (php_deflate_string(output, output_len, handled_output, handled_output_len, do_start, do_end TSRMLS_CC) != SUCCESS) {
			zend_error(E_ERROR, "Compression failed");
		}
	}
}
/* }}} */

/* {{{ php_enable_output_compression
 */
static int php_enable_output_compression(int buffer_size TSRMLS_DC)
{
	zval **a_encoding;

	zend_is_auto_global("_SERVER", sizeof("_SERVER")-1 TSRMLS_CC);

	if (!PG(http_globals)[TRACK_VARS_SERVER]
		|| zend_hash_find(PG(http_globals)[TRACK_VARS_SERVER]->value.ht, "HTTP_ACCEPT_ENCODING", sizeof("HTTP_ACCEPT_ENCODING"), (void **) &a_encoding) == FAILURE
	) {
		return FAILURE;
	}

	convert_to_string_ex(a_encoding);

	if (php_memnstr(Z_STRVAL_PP(a_encoding), "gzip", 4, Z_STRVAL_PP(a_encoding) + Z_STRLEN_PP(a_encoding))) {
		ZLIBG(compression_coding) = CODING_GZIP;
	} else if (php_memnstr(Z_STRVAL_PP(a_encoding), "deflate", 7, Z_STRVAL_PP(a_encoding) + Z_STRLEN_PP(a_encoding))) {
		ZLIBG(compression_coding) = CODING_DEFLATE;
	} else {
		return FAILURE;
	}

	php_ob_set_internal_handler(php_gzip_output_handler, (uint)buffer_size, "zlib output compression", 0 TSRMLS_CC);

	if (ZLIBG(output_handler) && strlen(ZLIBG(output_handler))) {
		php_start_ob_buffer_named(ZLIBG(output_handler), 0, 1 TSRMLS_CC);
	}
	return SUCCESS;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
