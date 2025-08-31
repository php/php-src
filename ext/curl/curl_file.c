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
   | Author: Stanislav Malyshev <stas@php.net>                            |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "Zend/zend_exceptions.h"
#include "curl_private.h"
#include "curl_file_arginfo.h"

PHP_CURL_API zend_class_entry *curl_CURLFile_class;
PHP_CURL_API zend_class_entry *curl_CURLStringFile_class;

static void curlfile_ctor(INTERNAL_FUNCTION_PARAMETERS)
{
	zend_string *fname, *mime = NULL, *postname = NULL;
	zval *cf = return_value;

	ZEND_PARSE_PARAMETERS_START(1,3)
		Z_PARAM_PATH_STR(fname)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR_OR_NULL(mime)
		Z_PARAM_STR_OR_NULL(postname)
	ZEND_PARSE_PARAMETERS_END();

	zend_update_property_str(curl_CURLFile_class, Z_OBJ_P(cf), "name", sizeof("name")-1, fname);

	if (mime) {
		zend_update_property_str(curl_CURLFile_class, Z_OBJ_P(cf), "mime", sizeof("mime")-1, mime);
	}

	if (postname) {
		zend_update_property_str(curl_CURLFile_class, Z_OBJ_P(cf), "postname", sizeof("postname")-1, postname);
	}
}

/* {{{ Create the CURLFile object */
ZEND_METHOD(CURLFile, __construct)
{
	return_value = ZEND_THIS;
	curlfile_ctor(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ Create the CURLFile object */
PHP_FUNCTION(curl_file_create)
{
	object_init_ex( return_value, curl_CURLFile_class );
	curlfile_ctor(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

static void curlfile_get_property(const char *name, size_t name_len, INTERNAL_FUNCTION_PARAMETERS)
{
	zval *res, rv;

	ZEND_PARSE_PARAMETERS_NONE();
	res = zend_read_property(curl_CURLFile_class, Z_OBJ_P(ZEND_THIS), name, name_len, 1, &rv);
	RETURN_COPY_DEREF(res);
}

static void curlfile_set_property(const char *name, size_t name_len, INTERNAL_FUNCTION_PARAMETERS)
{
	zend_string *arg;

	ZEND_PARSE_PARAMETERS_START(1,1)
		Z_PARAM_STR(arg)
	ZEND_PARSE_PARAMETERS_END();

	zend_update_property_str(curl_CURLFile_class, Z_OBJ_P(ZEND_THIS), name, name_len, arg);
}

/* {{{ Get file name */
ZEND_METHOD(CURLFile, getFilename)
{
	curlfile_get_property("name", sizeof("name")-1, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ Get MIME type */
ZEND_METHOD(CURLFile, getMimeType)
{
	curlfile_get_property("mime", sizeof("mime")-1, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ Get file name for POST */
ZEND_METHOD(CURLFile, getPostFilename)
{
	curlfile_get_property("postname", sizeof("postname")-1, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ Set MIME type */
ZEND_METHOD(CURLFile, setMimeType)
{
	curlfile_set_property("mime", sizeof("mime")-1, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ Set file name for POST */
ZEND_METHOD(CURLFile, setPostFilename)
{
	curlfile_set_property("postname", sizeof("postname")-1, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

ZEND_METHOD(CURLStringFile, __construct)
{
	zend_string *data, *postname, *mime = NULL;
	zval *object;

	object = ZEND_THIS;

	ZEND_PARSE_PARAMETERS_START(2,3)
		Z_PARAM_STR(data)
		Z_PARAM_STR(postname)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR(mime)
	ZEND_PARSE_PARAMETERS_END();

	zend_update_property_str(curl_CURLStringFile_class, Z_OBJ_P(object), "data", sizeof("data") - 1, data);
	zend_update_property_str(curl_CURLStringFile_class, Z_OBJ_P(object), "postname", sizeof("postname")-1, postname);
	if (mime) {
		zend_update_property_str(curl_CURLStringFile_class, Z_OBJ_P(object), "mime", sizeof("mime")-1, mime);
	} else {
		zend_update_property_string(curl_CURLStringFile_class, Z_OBJ_P(object), "mime", sizeof("mime")-1, "application/octet-stream");
	}
}

void curlfile_register_class(void)
{
	curl_CURLFile_class = register_class_CURLFile();

	curl_CURLStringFile_class = register_class_CURLStringFile();
}
