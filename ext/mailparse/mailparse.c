/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Wez Furlong <wez@thebrainroom.com>                           |
   | Credit also given to Double Precision Inc. who wrote the code that   |
   | the support routines for this extension were based upon.             |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/file.h"
#include "php_mailparse.h"
#include "mailparse_rfc822.h"
#include "ext/standard/info.h"
#include "main/php_output.h"
#include "php_open_temporary_file.h"

/* just in case the config check doesn't enable mbstring automatically */
#if !HAVE_MBSTRING
#error The mailparse extension requires the mbstring extension!
#endif

#include "ext/mbstring/mbfilter.h"

static int le_rfc2045;
/* this is for sections we "found": we mustn't free them, as this will cause
 * a SEGFAULT when the parent is freed */
static int le_rfc2045_nofree;


function_entry mailparse_functions[] = {
	PHP_FE(mailparse_msg_parse_file,			NULL)
	PHP_FE(mailparse_msg_get_part,				NULL)
	PHP_FE(mailparse_msg_get_structure,			NULL)
	PHP_FE(mailparse_msg_get_part_data,			NULL)
	PHP_FE(mailparse_msg_extract_part,			NULL)
	PHP_FE(mailparse_msg_extract_part_file,		NULL)
	
	PHP_FE(mailparse_msg_create,				NULL)
	PHP_FE(mailparse_msg_free,				NULL)
	PHP_FE(mailparse_msg_parse,				NULL)
	PHP_FE(mailparse_rfc822_parse_addresses,		NULL)
	PHP_FE(mailparse_determine_best_xfer_encoding, NULL)
	PHP_FE(mailparse_stream_encode,						NULL)
	PHP_FE(mailparse_uudecode_all,					NULL)

	{NULL, NULL, NULL}
};

zend_module_entry mailparse_module_entry = {
	STANDARD_MODULE_HEADER,
	"mailparse",
	mailparse_functions,
	PHP_MINIT(mailparse),
	PHP_MSHUTDOWN(mailparse),
	PHP_RINIT(mailparse),
	PHP_RSHUTDOWN(mailparse),
	PHP_MINFO(mailparse),
    NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES
};

ZEND_DECLARE_MODULE_GLOBALS(mailparse)

#ifdef COMPILE_DL_MAILPARSE
ZEND_GET_MODULE(mailparse)
#endif


ZEND_RSRC_DTOR_FUNC(rfc2045_dtor)
{
	rfc2045_free(rsrc->ptr);
}

PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY("mailparse.def_charset", RFC2045CHARSET, PHP_INI_ALL, OnUpdateString, def_charset, zend_mailparse_globals, mailparse_globals)
PHP_INI_END()

#define mailparse_msg_name	"mailparse_mail_structure"

#define mailparse_fetch_rfc2045_resource(rfcvar, zvalarg)	ZEND_FETCH_RESOURCE2(rfcvar, struct rfc2045 *, zvalarg, -1, mailparse_msg_name, le_rfc2045, le_rfc2045_nofree)

PHP_MINIT_FUNCTION(mailparse)
{
#ifdef ZTS
	zend_mailparse_globals *mailparse_globals;

	ts_allocate_id(&mailparse_globals_id, sizeof(zend_mailparse_globals), NULL, NULL);
	mailparse_globals = ts_resource(mailparse_globals_id);
#endif

	le_rfc2045 = 		zend_register_list_destructors_ex(rfc2045_dtor, NULL, mailparse_msg_name, module_number);
	le_rfc2045_nofree = zend_register_list_destructors_ex(NULL, NULL, mailparse_msg_name, module_number);

	REGISTER_INI_ENTRIES();

	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(mailparse)
{
	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}

PHP_MINFO_FUNCTION(mailparse)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "mailparse support", "enabled");
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}


PHP_RINIT_FUNCTION(mailparse)
{
	return SUCCESS;
}


PHP_RSHUTDOWN_FUNCTION(mailparse)
{
	return SUCCESS;
}

static void mailparse_rfc822t_errfunc(const char * msg, int num)
{
	TSRMLS_FETCH();

	php_error(E_WARNING, "%s(): %s %d", get_active_function_name(TSRMLS_C), msg, num);
}

#define UUDEC(c)	(char)(((c)-' ')&077)
#define UU_NEXT(v)	v = php_stream_getc(instream); if (v == EOF) break; v = UUDEC(v)
static void mailparse_do_uudecode(php_stream * instream, php_stream * outstream)
{
	int A, B, C, D, n;

	while(!php_stream_eof(instream))	{
		UU_NEXT(n);

		while(n != 0)	{
			UU_NEXT(A);
			UU_NEXT(B);
			UU_NEXT(C);
			UU_NEXT(D);

			if (n-- > 0)
				php_stream_putc(outstream, (A << 2) | (B >> 4));
			if (n-- > 0)
				php_stream_putc(outstream, (B << 4) | (C >> 2));
			if (n-- > 0)
				php_stream_putc(outstream, (C << 6) | D);
		}
		/* skip newline */
		php_stream_getc(instream);
	}
}


/* {{{ proto array mailparse_uudecode_all(resource fp)
   Scans the data from fp and extract each embedded uuencoded file. Returns an array listing filename information */
PHP_FUNCTION(mailparse_uudecode_all)
{
	zval * file, * item;
	int type;
	char * buffer = NULL;
	char * outpath = NULL;
	int nparts = 0;
	php_stream * instream, *outstream = NULL, *partstream = NULL;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(), "r", &file))
		return;

	instream = (php_stream*)zend_fetch_resource(&file TSRMLS_CC, -1, "File-Handle", &type, 1, php_file_le_stream());
	ZEND_VERIFY_RESOURCE(instream);

	outstream = php_stream_fopen_temporary_file(NULL, "mailparse", &outpath TSRMLS_CC);
	if (outstream == NULL)	{
		zend_error(E_WARNING, "%s(): unable to open temp file", get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
	}

	php_stream_rewind(instream);
	
	buffer = emalloc(4096);
	while(php_stream_gets(instream, buffer, 4096))	{
		/* Look for the "begin " sequence that identifies a uuencoded file */
		if (strncmp(buffer, "begin ", 6) == 0)	{
			char * origfilename;
			int len;
			/* parse out the file name.
			 * The next 4 bytes are an octal number for perms; ignore it */
			origfilename = &buffer[10];
			/* NUL terminate the filename */
			len = strlen(origfilename);
			while(isspace(origfilename[len-1]))
				origfilename[--len] = '\0';
		
			/* make the return an array */
			if (nparts == 0)	{
				array_init(return_value);
				/* create an initial item representing the file with all uuencoded parts
				 * removed */
				MAKE_STD_ZVAL(item);
				array_init(item);
				add_assoc_string(item, "filename", outpath, 0);
				add_next_index_zval(return_value, item);
			}
			
			/* add an item */
			MAKE_STD_ZVAL(item);
			array_init(item);
			add_assoc_string(item, "origfilename", origfilename, 1);

			/* create a temp file for the data */
			partstream = php_stream_fopen_temporary_file(NULL, "mailparse", &outpath TSRMLS_CC);
			if (partstream)	{
				nparts++;
				add_assoc_string(item, "filename", outpath, 0);
				add_next_index_zval(return_value, item);

				/* decode it */
				mailparse_do_uudecode(instream, partstream);
				php_stream_close(partstream);
			}
		}
		else	{
			/* write to the output file */
			php_stream_puts(outstream, buffer);
		}
	}
	php_stream_close(outstream);
	php_stream_rewind(instream);
	efree(buffer);

	if (nparts == 0)	{
		/* delete temporary file */
		unlink(outpath);
		efree(outpath);
		RETURN_FALSE;
	}
}
/* }}} */




/* {{{ proto array mailparse_rfc822_parse_addresses(string addresses)
   Parse addresses and returns a hash containing that data */
PHP_FUNCTION(mailparse_rfc822_parse_addresses)
{
	zval ** addresses;
	struct rfc822t * tokens;
	struct rfc822a * addrs;
	int i;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &addresses) == FAILURE)	{
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(addresses);

	tokens = mailparse_rfc822t_alloc(Z_STRVAL_PP(addresses), mailparse_rfc822t_errfunc);

	if (tokens)	{
		addrs = mailparse_rfc822a_alloc(tokens);
		if (addrs)	{

			array_init(return_value);

			for (i = 0; i < addrs->naddrs; i++)	{
				char * p;
				zval * item;

				MAKE_STD_ZVAL(item);

				if (array_init(item) == FAILURE)
					break;

			  	p = mailparse_rfc822_getname(addrs, i);
				add_assoc_string(item, "display", p, 0); /* don't duplicate - getname allocated the memory for us */
				p = mailparse_rfc822_getaddr(addrs, i);
				add_assoc_string(item, "address", p, 0); /* don't duplicate - getaddr allocated the memory for us */

				/* add this to the result */
				zend_hash_next_index_insert(HASH_OF(return_value), &item, sizeof(item), NULL);
			}

			mailparse_rfc822a_free(addrs);
		}

		mailparse_rfc822t_free(tokens);
	}
}
/* }}} */

/* {{{ proto int mailparse_determine_best_xfer_encoding(resource fp)
   Figures out the best way of encoding the content read from the file pointer fp, which must be seek-able */
PHP_FUNCTION(mailparse_determine_best_xfer_encoding)
{
	zval ** file;
	int longline = 0;
	int linelen = 0;
	int c;
	enum mbfl_no_encoding bestenc = mbfl_no_encoding_7bit;
	void * what;
	int type;
	char * name;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &file) == FAILURE)	{
		WRONG_PARAM_COUNT;
	}

	what = zend_fetch_resource(file TSRMLS_CC, -1, "File-Handle", &type, 1, php_file_le_stream());
	ZEND_VERIFY_RESOURCE(what);

	if (type == php_file_le_stream())	{
		php_stream * stream = (php_stream*)what;

		php_stream_rewind(stream);
		while(!php_stream_eof(stream))	{
			c = php_stream_getc(stream);
			if (c == EOF)
				break;
			if (c > 0x80)
				bestenc = mbfl_no_encoding_8bit;
			else if (c == 0)	{
				bestenc = mbfl_no_encoding_base64;
				longline = 0;
				break;
			}
			if (c == '\n')
				linelen = 0;
			else if (++linelen > 200)
				longline = 1;
		}
		if (longline)
			bestenc = mbfl_no_encoding_qprint;
		php_stream_rewind(stream);
	}

	name = (char *)mbfl_no2preferred_mime_name(bestenc);
	if (name)
	{
		RETVAL_STRING(name, 1);
	}
	else
	{
		RETVAL_FALSE;
	}
}
/* }}} */

/* {{{ proto boolean mailparse_stream_encode(resource sourcefp, resource destfp, string encoding)
   Streams data from source file pointer, apply encoding and write to destfp */

static int mailparse_stream_output(int c, void * stream)
{
	char buf = c;
	return php_stream_write((php_stream*)stream, &buf, 1);
}
static int mailparse_stream_flush(void * stream)
{
	return php_stream_flush((php_stream*)stream);
}

PHP_FUNCTION(mailparse_stream_encode)
{
	zval ** srcfile, ** destfile, ** encod;
	void * what;
	int type;
	php_stream * srcstream, * deststream;
	char * buf;
	size_t len;
	size_t bufsize = 2048;
	enum mbfl_no_encoding enc;
	mbfl_convert_filter * conv = NULL;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &srcfile, &destfile, &encod) == FAILURE)	{
		WRONG_PARAM_COUNT;
	}

	if (Z_TYPE_PP(srcfile) == IS_RESOURCE && Z_LVAL_PP(srcfile) == 0)	{
		RETURN_FALSE;
	}
	if (Z_TYPE_PP(destfile) == IS_RESOURCE && Z_LVAL_PP(destfile) == 0)	{
		RETURN_FALSE;
	}

	what = zend_fetch_resource(srcfile TSRMLS_CC, -1, "File-Handle", &type, 1, php_file_le_stream());
	ZEND_VERIFY_RESOURCE(what);

	srcstream = (php_stream*)what;

	what = zend_fetch_resource(destfile TSRMLS_CC, -1, "File-Handle", &type, 1, php_file_le_stream());
	ZEND_VERIFY_RESOURCE(what);
	deststream = (php_stream*)what;

	convert_to_string_ex(encod);
	enc = mbfl_name2no_encoding(Z_STRVAL_PP(encod));
	if (enc == mbfl_no_encoding_invalid)	{
		zend_error(E_WARNING, "%s(): unknown encoding \"%s\"",
				get_active_function_name(TSRMLS_C),
				Z_STRVAL_PP(encod)
				);
		RETURN_FALSE;
	}

	buf = emalloc(bufsize);
	RETVAL_TRUE;

	conv = mbfl_convert_filter_new(mbfl_no_encoding_8bit,
			enc,
			mailparse_stream_output,
			mailparse_stream_flush,
			deststream
			);

	while(!php_stream_eof(srcstream))	{
		len = php_stream_read(srcstream, buf, bufsize);
		if (len > 0)
		{
			int i;
			for (i=0; i<len; i++)
				mbfl_convert_filter_feed(buf[i], conv);
		}
	}

	mbfl_convert_filter_flush(conv);
	mbfl_convert_filter_delete(conv);
	efree(buf);
}
/* }}} */

/* {{{ proto void mailparse_msg_parse(resource rfc2045buf, string data)
   Incrementally parse data into buffer */
PHP_FUNCTION(mailparse_msg_parse)
{
	zval ** arg, ** data;
	struct rfc2045 * rfcbuf;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg, &data) == FAILURE)	{
		WRONG_PARAM_COUNT;
	}

	if (Z_TYPE_PP(arg) == IS_RESOURCE && Z_LVAL_PP(arg) == 0)	{
		RETURN_FALSE;
	}

	mailparse_fetch_rfc2045_resource(rfcbuf, arg);

	convert_to_string_ex(data);

	rfc2045_parse(rfcbuf, Z_STRVAL_PP(data), Z_STRLEN_PP(data));
}
/* }}} */

/* {{{ proto resource mailparse_msg_parse_file(string filename)
   Parse file and return a resource representing the structure */
PHP_FUNCTION(mailparse_msg_parse_file)
{
	zval ** filename;
	struct rfc2045 * rfcbuf;
	char * filebuf;
	FILE * fp;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &filename) == FAILURE)	{
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(filename);

	/* open file and read it in */
	fp = VCWD_FOPEN(Z_STRVAL_PP(filename), "r");
	if (fp == NULL)	{
		zend_error(E_WARNING, "%s(): unable to open file %s", get_active_function_name(TSRMLS_C), Z_STRVAL_PP(filename));
		RETURN_FALSE;
	}

	filebuf = emalloc(MAILPARSE_BUFSIZ);

	rfcbuf = rfc2045_alloc_ac();
	if (rfcbuf)	{
		ZEND_REGISTER_RESOURCE(return_value, rfcbuf, le_rfc2045);

		while(!feof(fp))	{
			int got = fread(filebuf, sizeof(char), MAILPARSE_BUFSIZ, fp);
			if (got > 0)	{
				rfc2045_parse(rfcbuf, filebuf, got);
			}
		}
		fclose(fp);
	}
	efree(filebuf);
}
/* }}} */

/* {{{ proto void mailparse_msg_free(resource rfc2045buf)
   Frees a handle allocated by mailparse_msg_create
*/
PHP_FUNCTION(mailparse_msg_free)
{
	zval **arg;
	struct rfc2045 * rfcbuf;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if (Z_TYPE_PP(arg) == IS_RESOURCE && Z_LVAL_PP(arg) == 0)	{
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE(rfcbuf, struct rfc2045 *, arg, -1, mailparse_msg_name, le_rfc2045);

	zend_list_delete(Z_LVAL_PP(arg));
	RETURN_TRUE;
}
/* }}} */


/* {{{ proto int mailparse_msg_create(void)
   Returns a handle that can be used to parse a message */
PHP_FUNCTION(mailparse_msg_create)
{
	struct rfc2045 * rfcbuf;

	rfcbuf = rfc2045_alloc_ac();

	ZEND_REGISTER_RESOURCE(return_value, rfcbuf, le_rfc2045);
}
/* }}} */

static void get_structure_callback(struct rfc2045 *p, struct rfc2045id *id, void *ptr)
{
	zval * return_value = (zval *)ptr;
	char intbuf[16];
	char buf[256];
	int len, i = 0;
	TSRMLS_FETCH();

	while(id && i < sizeof(buf))	{
		sprintf(intbuf, "%d", id->idnum);
		len = strlen(intbuf);
		if (len > (sizeof(buf)-i))	{
			/* too many sections: bail */
			zend_error(E_WARNING, "%s(): too many nested sections in message", get_active_function_name(TSRMLS_C));
			return;
		}
		sprintf(&buf[i], "%s%c", intbuf, id->next ? '.' : '\0');
		i += len + (id->next ? 1 : 0);
		id = id->next;
	}
	add_next_index_string(return_value, buf,1);
}

/* {{{ proto array mailparse_msg_get_structure(resource rfc2045)
   Returns an array of mime section names in the supplied message */
PHP_FUNCTION(mailparse_msg_get_structure)
{
	zval **arg;
	struct rfc2045 * rfcbuf;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if (Z_TYPE_PP(arg) == IS_RESOURCE && Z_LVAL_PP(arg) == 0)	{
		RETURN_FALSE;
	}

	mailparse_fetch_rfc2045_resource(rfcbuf, arg);

	if (array_init(return_value) == FAILURE)	{
		RETURN_FALSE;
	}

	rfc2045_decode(rfcbuf, &get_structure_callback, return_value);
}
/* }}} */

/* callback for decoding using a "userdefined" php function */
static int extract_callback_user_func(const char *p, size_t n, zval *userfunc)
{
	zval * retval;
	zval * arg;
	TSRMLS_FETCH();

	MAKE_STD_ZVAL(retval);
	Z_TYPE_P(retval) = IS_BOOL;
	Z_LVAL_P(retval) = 0;

	MAKE_STD_ZVAL(arg);
	ZVAL_STRINGL(arg, (char*)p, (int)n, 1);

	/* TODO: use zend_is_callable */

	if (call_user_function(EG(function_table), NULL, userfunc, retval, 1, &arg TSRMLS_CC) == FAILURE)
		zend_error(E_WARNING, "%s(): unable to call user function", get_active_function_name(TSRMLS_C));

	zval_dtor(retval);
	zval_dtor(arg);
	efree(retval);
	efree(arg);

	return 0;
}

/* callback for decoding to the current output buffer */
static int extract_callback_stdout(const char *p, size_t n, void *ptr)
{
	ZEND_WRITE(p, n);
	return 0;
}

/* {{{ proto void mailparse_msg_extract_part(resource rfc2045, string msgbody[, string callbackfunc])
   Extracts/decodes a message section.  If callbackfunc is not specified, the contents will be sent to "stdout" */
PHP_FUNCTION(mailparse_msg_extract_part)
{
	zval **arg, **bodystr, **cbfunc;
	struct rfc2045 * rfcbuf;
	off_t start, end, body;
	off_t nlines;
	off_t nbodylines;

	switch(ZEND_NUM_ARGS())	{
		case 3:
			if (zend_get_parameters_ex(3, &arg, &bodystr, &cbfunc) == FAILURE)	{
				WRONG_PARAM_COUNT;
			}
			if (Z_TYPE_PP(cbfunc) != IS_ARRAY)
				convert_to_string_ex(cbfunc);
			break;
		case 2:
			if (zend_get_parameters_ex(2, &arg, &bodystr) == FAILURE)	{
				WRONG_PARAM_COUNT;
			}
			cbfunc = NULL;
			break;
	}
	convert_to_string_ex(bodystr);

	if (Z_TYPE_PP(arg) == IS_RESOURCE && Z_LVAL_PP(arg) == 0)	{
		RETURN_FALSE;
	}
	mailparse_fetch_rfc2045_resource(rfcbuf, arg);


	rfc2045_mimepos(rfcbuf, &start, &end, &body, &nlines, &nbodylines);

	if (cbfunc)
		rfc2045_cdecode_start(rfcbuf, (rfc2045_decode_user_func_t)&extract_callback_user_func, *cbfunc);
	else
		rfc2045_cdecode_start(rfcbuf, &extract_callback_stdout, NULL);

	if (Z_STRLEN_PP(bodystr) < end)
		end = Z_STRLEN_PP(bodystr);
	else
		end = end-body;

	rfc2045_cdecode(rfcbuf, Z_STRVAL_PP(bodystr) + body, end);
	rfc2045_cdecode_end(rfcbuf);

	RETURN_TRUE;

}
/* }}} */

/* {{{ proto string mailparse_msg_extract_part_file(resource rfc2045, string filename [, string callbackfunc])
   Extracts/decodes a message section, decoding the transfer encoding */
PHP_FUNCTION(mailparse_msg_extract_part_file)
{
	zval **arg, **filename, **cbfunc;
	struct rfc2045 * rfcbuf;
	char * filebuf = NULL;
	FILE * fp = NULL;
	off_t start, end, body;
	off_t nlines;
	off_t nbodylines;

	switch(ZEND_NUM_ARGS())	{
		case 3:
			if (zend_get_parameters_ex(3, &arg, &filename, &cbfunc) == FAILURE)	{
				WRONG_PARAM_COUNT;
			}
			if (Z_TYPE_PP(cbfunc) != IS_ARRAY)
				convert_to_string_ex(cbfunc);
			break;
		case 2:
			if (zend_get_parameters_ex(2, &arg, &filename) == FAILURE)	{
				WRONG_PARAM_COUNT;
			}
			cbfunc = NULL;
			break;
	}
	convert_to_string_ex(filename);

	if (Z_TYPE_PP(arg) == IS_RESOURCE && Z_LVAL_PP(arg) == 0)	{
		RETURN_FALSE;
	}
	mailparse_fetch_rfc2045_resource(rfcbuf, arg);

	/* figure out where the message part starts/ends */
	rfc2045_mimepos(rfcbuf, &start, &end, &body, &nlines, &nbodylines);

	if (cbfunc)
		rfc2045_cdecode_start(rfcbuf, (rfc2045_decode_user_func_t)&extract_callback_user_func, *cbfunc);
	else
		rfc2045_cdecode_start(rfcbuf, &extract_callback_stdout, NULL);

	/* open file and read it in */
	fp = VCWD_FOPEN(Z_STRVAL_PP(filename), "rb");
	if (fp == NULL)	{
		zend_error(E_WARNING, "%s(): unable to open file %s", get_active_function_name(TSRMLS_C), Z_STRVAL_PP(filename));
		RETURN_FALSE;
	}
	if (fseek(fp, body, SEEK_SET) == -1)
	{
		zend_error(E_WARNING, "%s(): unable to seek to section start", get_active_function_name(TSRMLS_C));
		RETVAL_FALSE;
		goto cleanup;
	}
	filebuf = emalloc(MAILPARSE_BUFSIZ);


	while (body < end)
	{
		size_t n = MAILPARSE_BUFSIZ;

		if ((off_t)n > end-body)
			n=end-body;
		n = fread(filebuf, sizeof(char), n, fp);
		if (n == 0)
		{
			zend_error(E_WARNING, "%s(): error reading from file \"%s\", offset %d", get_active_function_name(TSRMLS_C), Z_STRVAL_PP(filename), body);
			RETVAL_FALSE;
			goto cleanup;
		}
		rfc2045_cdecode(rfcbuf, filebuf, n);
		body += n;
	}
	RETVAL_TRUE;

cleanup:
	rfc2045_cdecode_end(rfcbuf);
	if (fp)
		fclose(fp);
	if (filebuf)
		efree(filebuf);
}
/* }}} */

/* {{{ proto array mailparse_msg_get_part_data(resource rfc2045)
   Returns an associative array of info about the message */
/* NOTE: you may add keys to the array, but PLEASE do not remove the key/value pairs
	that are emitted here - it will break my PHP scripts if you do! */
PHP_FUNCTION(mailparse_msg_get_part_data)
{
	zval ** arg;
	struct rfc2045 * rfcbuf;
	char * content_type, *transfer_encoding, *charset;
	off_t start, end, body, nlines, nbodylines;
	char * disposition, * disposition_name, *disposition_filename;
	char *p;
	struct rfc2045attr * attr;
	zval * headers;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg) == FAILURE)	{
		WRONG_PARAM_COUNT;
	}

	if (Z_TYPE_PP(arg) == IS_RESOURCE && Z_LVAL_PP(arg) == 0)	{
		RETURN_FALSE;
	}


	mailparse_fetch_rfc2045_resource(rfcbuf, arg);

	if (array_init(return_value) == FAILURE)	{
		RETURN_FALSE;
	}


	rfc2045_mimeinfo(rfcbuf, (const char**)&content_type, (const char**)&transfer_encoding, (const char**)&charset);
	rfc2045_mimepos(rfcbuf, &start, &end, &body, &nlines, &nbodylines);

	if (content_type && *content_type)
		add_assoc_string(return_value, "content-type", content_type, 1);

	/* get attributes for content-type */
	attr = rfcbuf->content_type_attr;
	while (attr != NULL)	{
		char buf[80];
		strcpy(buf, "content-");
		strcat(buf, attr->name);
		add_assoc_string(return_value, buf, attr->value, 1);
		attr = attr->next;
	}
	/* get attributes for content-disposition */
	attr = rfcbuf->content_disposition_attr;
	while (attr != NULL)	{
		char buf[80];
		strcpy(buf, "disposition-");
		strcat(buf, attr->name);
		add_assoc_string(return_value, buf, attr->value, 1);
		attr = attr->next;
	}
	/* get headers for this section */
	MAKE_STD_ZVAL(headers);
	*headers = *rfcbuf->headerhash;
	INIT_PZVAL(headers);
	zval_copy_ctor(headers);
	/* add to result */
	zend_hash_update(HASH_OF(return_value), "headers", sizeof("headers"), &headers, sizeof(headers), NULL);

	add_assoc_string(return_value, "transfer-encoding", transfer_encoding, 1);
	add_assoc_string(return_value, "charset", charset, 1);

	rfc2045_dispositioninfo(rfcbuf, (const char**)&disposition, (const char**)&disposition_name, (const char**)&disposition_filename);
	if (disposition && *disposition)
		add_assoc_string(return_value, "content-disposition", disposition, 1);

	if (*(p=(char*)rfc2045_content_id(rfcbuf)))
		add_assoc_string(return_value, "content-id", p, 1);
	if (*(p=(char*)rfc2045_content_description(rfcbuf)))
		add_assoc_string(return_value, "content-description", p, 1);
	if (*(p=(char*)rfc2045_content_language(rfcbuf)))
		add_assoc_string(return_value, "content-language", p, 1);
	if (*(p=(char*)rfc2045_content_md5(rfcbuf)))
		add_assoc_string(return_value, "content-md5", p, 1);
	if (*(p=(char*)rfc2045_content_base(rfcbuf)))	{
		add_assoc_string(return_value, "content-base", p, 1);
		/* content base allocates mem */
		efree(p);
	}


	add_assoc_long(return_value, "starting-pos", start);
	add_assoc_long(return_value, "starting-pos-body", body);
	add_assoc_long(return_value, "ending-pos", end);
	add_assoc_long(return_value, "line-count", nlines);
	add_assoc_long(return_value, "body-line-count", nbodylines);
}
/* }}} */

/* {{{ proto int mailparse_msg_get_part(resource rfc2045, string mimesection)
   Returns a handle on a given section in a mimemessage */
PHP_FUNCTION(mailparse_msg_get_part)
{
	zval ** arg, ** mimesection;
	struct rfc2045 * rfcbuf, * newsection;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg, &mimesection) == FAILURE)	{
		WRONG_PARAM_COUNT;
	}

	if (Z_TYPE_PP(arg) == IS_RESOURCE && Z_LVAL_PP(arg) == 0)	{
		RETURN_FALSE;
	}

	mailparse_fetch_rfc2045_resource(rfcbuf, arg);

	convert_to_string_ex(mimesection);

	newsection = rfc2045_find(rfcbuf, Z_STRVAL_PP(mimesection));

	if (!newsection)	{
		php_error(E_WARNING, "%s(): cannot find section %s in message", get_active_function_name(TSRMLS_C), Z_STRVAL_PP(mimesection));
		RETURN_FALSE;
	}
	ZEND_REGISTER_RESOURCE(return_value, newsection, le_rfc2045_nofree);
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
