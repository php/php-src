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
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "Zend/zend_exceptions.h"
#include "php_curl.h"
#if HAVE_CURL

PHP_CURL_API zend_class_entry *curl_CURLBufferFile_class;

static void curlbufferfile_ctor(INTERNAL_FUNCTION_PARAMETERS)
{
	zend_string *postname, *buffer, *mime = NULL;
	zval *cf = return_value;

	ZEND_PARSE_PARAMETERS_START(2,3)
		Z_PARAM_STR(postname)
		Z_PARAM_STR(buffer)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR(mime)
	ZEND_PARSE_PARAMETERS_END();

	zend_update_property_string(curl_CURLBufferFile_class, cf, "postname", sizeof("postname")-1, ZSTR_VAL(postname));
	zend_update_property_stringl(curl_CURLBufferFile_class, cf, "buffer", sizeof("buffer")-1, ZSTR_VAL(buffer), ZSTR_LEN(buffer));

	if (mime) {
		zend_update_property_string(curl_CURLBufferFile_class, cf, "mime", sizeof("mime")-1, ZSTR_VAL(mime));
	}
}

/* {{{ proto CURLBufferFile::__construct(string $postfilename, string $buffer [, string $mimetype])
   Create the CURLBufferFile object */
ZEND_METHOD(CURLBufferFile, __construct)
{
	return_value = getThis();
	curlbufferfile_ctor(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto CURLBufferFile curl_buffer_file_create(string $postfilename, string $buffer [, string $mimetype])
   Create the CURLBufferFile object */
PHP_FUNCTION(curl_buffer_file_create)
{
    object_init_ex( return_value, curl_CURLBufferFile_class );
    curlbufferfile_ctor(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

static void curlbufferfile_get_property(char *name, size_t name_len, INTERNAL_FUNCTION_PARAMETERS)
{
	zval *res, rv;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	res = zend_read_property(curl_CURLBufferFile_class, getThis(), name, name_len, 1, &rv);
	ZVAL_COPY_DEREF(return_value, res);
}

static void curlbufferfile_set_property(char *name, size_t name_len, INTERNAL_FUNCTION_PARAMETERS)
{
	zend_string *arg;

	ZEND_PARSE_PARAMETERS_START(1,1)
		Z_PARAM_STR(arg)
	ZEND_PARSE_PARAMETERS_END();

	zend_update_property_string(curl_CURLBufferFile_class, getThis(), name, name_len, ZSTR_VAL(arg));
}

/* {{{ proto string CURLBufferFile::getBuffer()
   Get file name */
ZEND_METHOD(CURLBufferFile, getBuffer)
{
	curlbufferfile_get_property("buffer", sizeof("buffer")-1, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto string CURLBufferFile::getMimeType()
   Get MIME type */
ZEND_METHOD(CURLBufferFile, getMimeType)
{
	curlbufferfile_get_property("mime", sizeof("mime")-1, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto string CURLBufferFile::getPostFilename()
   Get file name for POST */
ZEND_METHOD(CURLBufferFile, getPostFilename)
{
	curlbufferfile_get_property("postname", sizeof("postname")-1, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto void CURLBufferFile::setMimeType(string $mime)
   Set MIME type */
ZEND_METHOD(CURLBufferFile, setMimeType)
{
	curlbufferfile_set_property("mime", sizeof("mime")-1, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

ZEND_BEGIN_ARG_INFO_EX(arginfo_curlbufferfile_create, 0, 0, 2)
	ZEND_ARG_INFO(0, postname)
	ZEND_ARG_INFO(0, buffer)
	ZEND_ARG_INFO(0, mimetype)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_curlbufferfile_name, 0)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

static const zend_function_entry curlbufferfile_funcs[] = {
	PHP_ME(CURLBufferFile,			__construct,        arginfo_curlbufferfile_create, ZEND_ACC_CTOR|ZEND_ACC_PUBLIC)
	PHP_ME(CURLBufferFile,			getPostFilename,    NULL, ZEND_ACC_PUBLIC)
	PHP_ME(CURLBufferFile,			getBuffer,          NULL, ZEND_ACC_PUBLIC)
	PHP_ME(CURLBufferFile,			getMimeType,        NULL, ZEND_ACC_PUBLIC)
	PHP_ME(CURLBufferFile,			setMimeType,        arginfo_curlbufferfile_name, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

void curlbufferfile_register_class(void)
{
	zend_class_entry ce;
	INIT_CLASS_ENTRY( ce, "CURLBufferFile", curlbufferfile_funcs );
	curl_CURLBufferFile_class = zend_register_internal_class(&ce);
	zend_declare_property_string(curl_CURLBufferFile_class, "buffer", sizeof("buffer")-1, "", ZEND_ACC_PUBLIC);
	zend_declare_property_string(curl_CURLBufferFile_class, "mime", sizeof("mime")-1, "", ZEND_ACC_PUBLIC);
	zend_declare_property_string(curl_CURLBufferFile_class, "postname", sizeof("postname")-1, "", ZEND_ACC_PUBLIC);
}

#endif
