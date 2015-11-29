/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2015 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Stanislav Malyshev <stas@php.net>                            |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "Zend/zend_exceptions.h"
#include "php_curl.h"
#if HAVE_CURL

PHP_CURL_API zend_class_entry *curl_CURLFile_class;

static void curlfile_ctor(char *fname, size_t fname_len, char *mime, size_t mime_len, char *postname, size_t postname_len, char *buffer, size_t buffer_len, INTERNAL_FUNCTION_PARAMETERS)
{
	zval *cf = return_value;

	if (fname) {
		zend_update_property_stringl(curl_CURLFile_class, cf, "name", sizeof("name")-1, fname, fname_len);
	}

	if (mime) {
		zend_update_property_stringl(curl_CURLFile_class, cf, "mime", sizeof("mime")-1, mime, mime_len);
	}

	if (postname) {
		zend_update_property_stringl(curl_CURLFile_class, cf, "postname", sizeof("postname")-1, postname, postname_len);
	}

	if (buffer) {
		zend_update_property_stringl(curl_CURLFile_class, cf, "buffer", sizeof("buffer")-1, buffer, buffer_len);
	}
}

/* {{{ proto void CURLFile::__construct([string $name, [string $mimetype [, string $postfilename]]])
   Create the CURLFile object */
ZEND_METHOD(CURLFile, __construct)
{
	char *fname = NULL, *mime = NULL, *postname = NULL, *buffer = NULL;
	size_t fname_len, mime_len, postname_len, buffer_len;

	return_value = getThis();

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|sss", &fname, &fname_len, &mime, &mime_len, &postname, &postname_len) == FAILURE) {
		return;
	}

	curlfile_ctor(fname, fname_len, mime, mime_len, postname, postname_len, buffer, buffer_len, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto CURLFile curl_file_create(string $name, [string $mimetype [, string $postfilename]])
   Create the CURLFile object */
PHP_FUNCTION(curl_file_create)
{
	char *fname = NULL, *mime = NULL, *postname = NULL, *buffer = NULL;
	size_t fname_len, mime_len, postname_len, buffer_len;

	object_init_ex( return_value, curl_CURLFile_class );

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|ss", &fname, &fname_len, &mime, &mime_len, &postname, &postname_len) == FAILURE) {
		return;
	}

	curlfile_ctor(fname, fname_len, mime, mime_len, postname, postname_len, buffer, buffer_len, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto CURLBufferFile curl_buffer_file_create(string $buffer, string $postfilename, [string $mimetype])
   Create the CURLBufferFile object */
PHP_FUNCTION(curl_buffer_file_create)
{
	char *fname = NULL, *mime = NULL, *postname = NULL, *buffer = NULL;
	size_t fname_len, mime_len, postname_len, buffer_len;

	object_init_ex( return_value, curl_CURLFile_class );

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss|s", &buffer, &buffer_len, &postname, &postname_len, &mime, &mime_len) == FAILURE) {
		return;
	}

	curlfile_ctor(fname, fname_len, mime, mime_len, postname, postname_len, buffer, buffer_len, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

static void curlfile_get_property(char *name, INTERNAL_FUNCTION_PARAMETERS)
{
	zval *res, rv;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	res = zend_read_property(curl_CURLFile_class, getThis(), name, strlen(name), 1, &rv);
	ZVAL_DEREF(res);
	ZVAL_COPY(return_value, res);
}

static void curlfile_set_property(char *name, INTERNAL_FUNCTION_PARAMETERS)
{
	char *arg = NULL;
	size_t arg_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &arg, &arg_len) == FAILURE) {
		return;
	}
	zend_update_property_stringl(curl_CURLFile_class, getThis(), name, strlen(name), arg, arg_len);
}

/* {{{ proto string CURLFile::getFilename()
   Get file name */
ZEND_METHOD(CURLFile, getFilename)
{
	curlfile_get_property("name", INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto string CURLFile::getBuffer()
   Get buffer */
ZEND_METHOD(CURLFile, getBuffer)
{
	curlfile_get_property("buffer", INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto string CURLFile::getMimeType()
   Get MIME type */
ZEND_METHOD(CURLFile, getMimeType)
{
	curlfile_get_property("mime", INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto string CURLFile::getPostFilename()
   Get file name for POST */
ZEND_METHOD(CURLFile, getPostFilename)
{
	curlfile_get_property("postname", INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto void CURLFile::setFilename(string $name)
   Set file name */
ZEND_METHOD(CURLFile, setFilename)
{
	curlfile_set_property("name", INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto void CURLFile::setBuffer(string $buffer)
   Set buffer */
ZEND_METHOD(CURLFile, setBuffer)
{
	curlfile_set_property("buffer", INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto void CURLFile::setMimeType(string $mime)
   Set MIME type */
ZEND_METHOD(CURLFile, setMimeType)
{
	curlfile_set_property("mime", INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto void CURLFile::setPostFilename(string $name)
   Set file name for POST */
ZEND_METHOD(CURLFile, setPostFilename)
{
	curlfile_set_property("postname", INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto void CURLFile::__wakeup()
   Unserialization handler */
ZEND_METHOD(CURLFile, __wakeup)
{
	zval *fname, rv;

	fname = zend_read_property(curl_CURLFile_class, getThis(), "name", strlen("name"), 1, &rv);
	if (zval_is_true(fname)) {
		zend_update_property_string(curl_CURLFile_class, getThis(), "name", sizeof("name")-1, "");
		zend_throw_exception(NULL, "Unserialization of CURLFile instances with file name is not allowed", 0);
	}
}
/* }}} */

ZEND_BEGIN_ARG_INFO_EX(arginfo_curlfile_ctor, 0, 0, 0)
	ZEND_ARG_INFO(0, filename)
	ZEND_ARG_INFO(0, mimetype)
	ZEND_ARG_INFO(0, postname)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_curlfile_name, 0)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()


static const zend_function_entry curlfile_funcs[] = {
	PHP_ME(CURLFile,			__construct,        arginfo_curlfile_ctor, ZEND_ACC_CTOR|ZEND_ACC_PUBLIC)
	PHP_ME(CURLFile,			getFilename,        NULL, ZEND_ACC_PUBLIC)
	PHP_ME(CURLFile,			setFilename,        arginfo_curlfile_name, ZEND_ACC_PUBLIC)
	PHP_ME(CURLFile,			getBuffer,          NULL, ZEND_ACC_PUBLIC)
	PHP_ME(CURLFile,			setBuffer,          arginfo_curlfile_name, ZEND_ACC_PUBLIC)
	PHP_ME(CURLFile,			getMimeType,        NULL, ZEND_ACC_PUBLIC)
	PHP_ME(CURLFile,			setMimeType,        arginfo_curlfile_name, ZEND_ACC_PUBLIC)
	PHP_ME(CURLFile,			getPostFilename,    NULL, ZEND_ACC_PUBLIC)
	PHP_ME(CURLFile,			setPostFilename,    arginfo_curlfile_name, ZEND_ACC_PUBLIC)
	PHP_ME(CURLFile,            __wakeup,           NULL, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

void curlfile_register_class(void)
{
	zend_class_entry ce;
	INIT_CLASS_ENTRY( ce, "CURLFile", curlfile_funcs );
	curl_CURLFile_class = zend_register_internal_class(&ce);
	zend_declare_property_string(curl_CURLFile_class, "name", sizeof("name")-1, "", ZEND_ACC_PUBLIC);
	zend_declare_property_string(curl_CURLFile_class, "buffer", sizeof("buffer")-1, "", ZEND_ACC_PUBLIC);
	zend_declare_property_string(curl_CURLFile_class, "mime", sizeof("mime")-1, "", ZEND_ACC_PUBLIC);
	zend_declare_property_string(curl_CURLFile_class, "postname", sizeof("postname")-1, "", ZEND_ACC_PUBLIC);
}

#endif
