/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
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

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "Zend/zend_exceptions.h"
#include "Zend/zend_interfaces.h"
#include "php_curl.h"
#if HAVE_CURL

PHP_CURL_API zend_class_entry *curl_CURLFile_class;

static void curlfile_ctor(INTERNAL_FUNCTION_PARAMETERS)
{
	zend_string *fname, *mime = NULL, *postname = NULL;
	zval *cf = return_value;

	ZEND_PARSE_PARAMETERS_START(1,3)
		Z_PARAM_PATH_STR(fname)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR(mime)
		Z_PARAM_STR(postname)
	ZEND_PARSE_PARAMETERS_END();

	zend_update_property_string(curl_CURLFile_class, cf, "name", sizeof("name")-1, ZSTR_VAL(fname));

	if (mime) {
		zend_update_property_string(curl_CURLFile_class, cf, "mime", sizeof("mime")-1, ZSTR_VAL(mime));
	}

	if (postname) {
		zend_update_property_string(curl_CURLFile_class, cf, "postname", sizeof("postname")-1, ZSTR_VAL(postname));
	}
}

/* {{{ proto CURLFile::__construct(string $name, [string $mimetype [, string $postfilename]])
   Create the CURLFile object */
ZEND_METHOD(CURLFile, __construct)
{
	return_value = ZEND_THIS;
	curlfile_ctor(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto CURLFile curl_file_create(string $name, [string $mimetype [, string $postfilename]])
   Create the CURLFile object */
PHP_FUNCTION(curl_file_create)
{
    object_init_ex( return_value, curl_CURLFile_class );
    curlfile_ctor(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

static void curlfile_get_property(char *name, size_t name_len, INTERNAL_FUNCTION_PARAMETERS)
{
	zval *res, rv;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	res = zend_read_property(curl_CURLFile_class, ZEND_THIS, name, name_len, 1, &rv);
	ZVAL_COPY_DEREF(return_value, res);
}

static void curlfile_set_property(char *name, size_t name_len, INTERNAL_FUNCTION_PARAMETERS)
{
	zend_string *arg;

	ZEND_PARSE_PARAMETERS_START(1,1)
		Z_PARAM_STR(arg)
	ZEND_PARSE_PARAMETERS_END();

	zend_update_property_string(curl_CURLFile_class, ZEND_THIS, name, name_len, ZSTR_VAL(arg));
}

/* {{{ proto string CURLFile::getFilename()
   Get file name */
ZEND_METHOD(CURLFile, getFilename)
{
	curlfile_get_property("name", sizeof("name")-1, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto string CURLFile::getMimeType()
   Get MIME type */
ZEND_METHOD(CURLFile, getMimeType)
{
	curlfile_get_property("mime", sizeof("mime")-1, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto string CURLFile::getPostFilename()
   Get file name for POST */
ZEND_METHOD(CURLFile, getPostFilename)
{
	curlfile_get_property("postname", sizeof("postname")-1, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto void CURLFile::setMimeType(string $mime)
   Set MIME type */
ZEND_METHOD(CURLFile, setMimeType)
{
	curlfile_set_property("mime", sizeof("mime")-1, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto void CURLFile::setPostFilename(string $name)
   Set file name for POST */
ZEND_METHOD(CURLFile, setPostFilename)
{
	curlfile_set_property("postname", sizeof("postname")-1, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

ZEND_BEGIN_ARG_INFO_EX(arginfo_curlfile_create, 0, 0, 1)
	ZEND_ARG_INFO(0, filename)
	ZEND_ARG_INFO(0, mimetype)
	ZEND_ARG_INFO(0, postname)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_curlfile_name, 0)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()


static const zend_function_entry curlfile_funcs[] = {
	PHP_ME(CURLFile,			__construct,        arginfo_curlfile_create, ZEND_ACC_PUBLIC)
	PHP_ME(CURLFile,			getFilename,        NULL, ZEND_ACC_PUBLIC)
	PHP_ME(CURLFile,			getMimeType,        NULL, ZEND_ACC_PUBLIC)
	PHP_ME(CURLFile,			setMimeType,        arginfo_curlfile_name, ZEND_ACC_PUBLIC)
	PHP_ME(CURLFile,			getPostFilename,    NULL, ZEND_ACC_PUBLIC)
	PHP_ME(CURLFile,			setPostFilename,    arginfo_curlfile_name, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

void curlfile_register_class(void)
{
	zend_class_entry ce;
	INIT_CLASS_ENTRY( ce, "CURLFile", curlfile_funcs );
	curl_CURLFile_class = zend_register_internal_class(&ce);
	curl_CURLFile_class->serialize = zend_class_serialize_deny;
	curl_CURLFile_class->unserialize = zend_class_unserialize_deny;
	zend_declare_property_string(curl_CURLFile_class, "name", sizeof("name")-1, "", ZEND_ACC_PUBLIC);
	zend_declare_property_string(curl_CURLFile_class, "mime", sizeof("mime")-1, "", ZEND_ACC_PUBLIC);
	zend_declare_property_string(curl_CURLFile_class, "postname", sizeof("postname")-1, "", ZEND_ACC_PUBLIC);
}

#endif
