/*
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
#include "curl_private.h"
#include "curl_file_arginfo.h"

PHP_CURL_API zend_class_entry *curl_CURLFile_class;

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

	zend_update_property_string(curl_CURLFile_class, Z_OBJ_P(cf), "name", sizeof("name")-1, ZSTR_VAL(fname));

	if (mime) {
		zend_update_property_string(curl_CURLFile_class, Z_OBJ_P(cf), "mime", sizeof("mime")-1, ZSTR_VAL(mime));
	}

	if (postname) {
		zend_update_property_string(curl_CURLFile_class, Z_OBJ_P(cf), "postname", sizeof("postname")-1, ZSTR_VAL(postname));
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

static void curlfile_get_property(char *name, size_t name_len, INTERNAL_FUNCTION_PARAMETERS)
{
	zval *res, rv;

	ZEND_PARSE_PARAMETERS_NONE();
	res = zend_read_property(curl_CURLFile_class, Z_OBJ_P(ZEND_THIS), name, name_len, 1, &rv);
	ZVAL_COPY_DEREF(return_value, res);
}

static void curlfile_set_property(char *name, size_t name_len, INTERNAL_FUNCTION_PARAMETERS)
{
	zend_string *arg;

	ZEND_PARSE_PARAMETERS_START(1,1)
		Z_PARAM_STR(arg)
	ZEND_PARSE_PARAMETERS_END();

	zend_update_property_string(curl_CURLFile_class, Z_OBJ_P(ZEND_THIS), name, name_len, ZSTR_VAL(arg));
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

void curlfile_register_class(void)
{
	zend_class_entry ce;
	INIT_CLASS_ENTRY( ce, "CURLFile", class_CURLFile_methods );
	curl_CURLFile_class = zend_register_internal_class(&ce);
	curl_CURLFile_class->serialize = zend_class_serialize_deny;
	curl_CURLFile_class->unserialize = zend_class_unserialize_deny;
	zend_declare_property_string(curl_CURLFile_class, "name", sizeof("name")-1, "", ZEND_ACC_PUBLIC);
	zend_declare_property_string(curl_CURLFile_class, "mime", sizeof("mime")-1, "", ZEND_ACC_PUBLIC);
	zend_declare_property_string(curl_CURLFile_class, "postname", sizeof("postname")-1, "", ZEND_ACC_PUBLIC);
}
