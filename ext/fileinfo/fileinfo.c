/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2004 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.0 of the PHP license,       |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_0.txt.                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Ilia Alshanetsky <ilia@php.net>                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <magic.h>

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "ext/standard/file.h" /* needed for context stuff */
#include "php_fileinfo.h"
#include "fopen_wrappers.h" /* needed for is_url */

struct php_fileinfo {
	long options;
	struct magic_set *magic;
};

#ifndef PHP_DEFAULT_MAGIC_FILE
#define PHP_DEFAULT_MAGIC_FILE NULL
#endif

#ifdef ZEND_ENGINE_2

static zend_object_handlers finfo_object_handlers;
zend_class_entry *finfo_class_entry;

struct finfo_object {
	zend_object zo;
	struct php_fileinfo *ptr;
};

#define FILEINFO_DECLARE_INIT_OBJECT(object) \
	zval *object = getThis();

#define FILEINFO_REGISTER_OBJECT(_object, _ptr) \
{ \
	struct finfo_object *obj; \
        obj = (struct finfo_object*)zend_object_store_get_object(_object TSRMLS_CC); \
        obj->ptr = _ptr; \
}

#define FILEINFO_FROM_OBJECT(finfo, object) \
{ \
	struct finfo_object *obj = zend_object_store_get_object(object TSRMLS_CC); \
	finfo = obj->ptr; \
	if (!finfo) { \
        	php_error_docref(NULL TSRMLS_CC, E_WARNING, "The invalid fileinfo object."); \
		RETURN_FALSE; \
	} \
}

/* {{{ finfo_objects_dtor
 */
static void finfo_objects_dtor(void *object, zend_object_handle handle TSRMLS_DC)
{
	struct finfo_object *intern = (struct finfo_object *) object;

	if (intern->ptr) {
		magic_close(intern->ptr->magic);
		efree(intern->ptr);
	}

	efree(intern);
}
/* }}} */

/* {{{ finfo_objects_new
 */
PHP_FILEINFO_API zend_object_value finfo_objects_new(zend_class_entry *class_type TSRMLS_DC)
{
	zend_object_value retval;
	struct finfo_object *intern;

	intern = emalloc(sizeof(struct finfo_object));
	intern->zo.ce = class_type;
	intern->zo.in_get = 0;
	intern->zo.in_set = 0;
	intern->zo.properties = NULL;
	intern->ptr = NULL;

	retval.handle = zend_objects_store_put(intern, finfo_objects_dtor, NULL, NULL TSRMLS_CC);
	retval.handlers = (zend_object_handlers *) &finfo_object_handlers;

	return retval;
}
/* }}} */

/* {{{ finfo_class_functions
 */
function_entry finfo_class_functions[] = {
	ZEND_ME_MAPPING(finfo,		finfo_open,	NULL)
	ZEND_ME_MAPPING(set_flags,	finfo_set_flags,NULL)
	ZEND_ME_MAPPING(file,		finfo_file,	NULL)
	ZEND_ME_MAPPING(buffer,		finfo_buffer,	NULL)
	{NULL, NULL, NULL}
};
/* }}} */

#else 

#define FILEINFO_REGISTER_OBJECT(_object, _ptr) {}
#define FILEINFO_FROM_OBJECT(socket_id, object) {}

#define FILEINFO_DECLARE_INIT_OBJECT(object)
#define object 0

#endif

#define FINFO_SET_OPTION(magic, options) \
	if (magic_setflags(magic, options) == -1) { \
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Failed to set option '%ld' %d:%s", \
				options, magic_errno(magic), magic_error(magic)); \
		RETURN_FALSE; \
	}

/* True global resources - no need for thread safety here */
static int le_fileinfo;

void finfo_resource_destructor(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	if (rsrc->ptr) {
		struct php_fileinfo *finfo = (struct php_fileinfo *) rsrc->ptr;
		magic_close(finfo->magic);
		efree(rsrc->ptr);
		rsrc->ptr = NULL;
	}
}

/* {{{ fileinfo_functions[]
 */
function_entry fileinfo_functions[] = {
	PHP_FE(finfo_open,	NULL)
	PHP_FE(finfo_close,	NULL)
	PHP_FE(finfo_set_flags,	NULL)
	PHP_FE(finfo_file,	NULL)
	PHP_FE(finfo_buffer,	NULL)
	{NULL, NULL, NULL}
};
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(finfo)
{
#ifdef ZEND_ENGINE_2
	zend_class_entry _finfo_class_entry;
	INIT_CLASS_ENTRY(_finfo_class_entry, "finfo", finfo_class_functions);
	_finfo_class_entry.create_object = finfo_objects_new;
	finfo_class_entry = zend_register_internal_class(&_finfo_class_entry TSRMLS_CC);

	/* copy the standard object handlers to you handler table */
	memcpy(&finfo_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
#endif /* ZEND_ENGINE_2 */

	le_fileinfo = zend_register_list_destructors_ex(finfo_resource_destructor, NULL, "file_info", module_number);

	REGISTER_LONG_CONSTANT("FILEINFO_NONE",			MAGIC_NONE, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILEINFO_SYMLINK",		MAGIC_SYMLINK, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILEINFO_MIME",			MAGIC_MIME, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILEINFO_COMPRESS",		MAGIC_COMPRESS, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILEINFO_DEVICES",		MAGIC_DEVICES, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILEINFO_CONTINUE",		MAGIC_CONTINUE, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILEINFO_PRESERVE_ATIME",	MAGIC_PRESERVE_ATIME, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILEINFO_RAW",			MAGIC_RAW, CONST_CS|CONST_PERSISTENT);

	return SUCCESS;
}
/* }}} */

/* {{{ fileinfo_module_entry
 */
zend_module_entry fileinfo_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"fileinfo",
	fileinfo_functions,
	PHP_MINIT(finfo),
	NULL,
	NULL,	
	NULL,
	PHP_MINFO(fileinfo),
#if ZEND_MODULE_API_NO >= 20010901
	"0.1", /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_FILENINFO
ZEND_GET_MODULE(fileinfo)
#endif

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(fileinfo)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "fileinfo support", "enabled");
	php_info_print_table_end();
}
/* }}} */

/* {{{ proto resource finfo_open([int options [, string arg]])
   Create a new fileinfo resource. */
PHP_FUNCTION(finfo_open)
{
	long options = MAGIC_NONE;
	char *file = PHP_DEFAULT_MAGIC_FILE;
	int file_len = 0;
	struct php_fileinfo *finfo;
	FILEINFO_DECLARE_INIT_OBJECT(object)
	char resolved_path[MAXPATHLEN];

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|ls", &options, &file, &file_len) == FAILURE) {
		RETURN_FALSE;
	}

	if (file_len) { /* user specified filed, perform open_basedir checks */
		if (!VCWD_REALPATH(file, resolved_path)) {
			RETURN_FALSE;
		}
		file = resolved_path;

		if ((PG(safe_mode) && (!php_checkuid(file, NULL, CHECKUID_CHECK_FILE_AND_DIR))) || php_check_open_basedir(file TSRMLS_CC)) {
			RETURN_FALSE;
		}
	}

	finfo = emalloc(sizeof(struct php_fileinfo));

	finfo->options = options;
	finfo->magic = magic_open(options);

	if (finfo->magic == NULL) {
		efree(finfo);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid mode '%ld'.", options);
		RETURN_FALSE;	
	}

	if (magic_load(finfo->magic, file) == -1) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Failed to load magic database at '%s'.", file);
		efree(finfo);
		magic_close(finfo->magic);
		RETURN_FALSE;
	}	

	if (object) {
		FILEINFO_REGISTER_OBJECT(object, finfo);
	} else {
		ZEND_REGISTER_RESOURCE(return_value, finfo, le_fileinfo);
	}	
}
/* }}} */

/* {{{ proto resource finfo_close(resource finfo)
   Close fileinfo resource. */
PHP_FUNCTION(finfo_close)
{
	struct php_fileinfo *finfo;
	zval *zfinfo;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &zfinfo) == FAILURE) {
		RETURN_FALSE;
	}
	ZEND_FETCH_RESOURCE(finfo, struct php_fileinfo *, &zfinfo, -1, "file_info", le_fileinfo);

	zend_list_delete(Z_RESVAL_P(zfinfo));

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool finfo_set_flags(resource finfo, int options)
   Set libmagic configuration options. */
PHP_FUNCTION(finfo_set_flags)
{
	long options;
	struct php_fileinfo *finfo;
	zval *zfinfo;
	FILEINFO_DECLARE_INIT_OBJECT(object)

	if (object) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &options) == FAILURE) {
			RETURN_FALSE;
		}
		FILEINFO_FROM_OBJECT(finfo, object);
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zl", &zfinfo, &options) == FAILURE) {
			RETURN_FALSE;
		}
		ZEND_FETCH_RESOURCE(finfo, struct php_fileinfo *, &zfinfo, -1, "file_info", le_fileinfo);
	}

	FINFO_SET_OPTION(finfo->magic, options)
	finfo->options = options;

	RETURN_TRUE;
}
/* }}} */

static void _php_finfo_get_type(INTERNAL_FUNCTION_PARAMETERS, int mode)
{
	long options = 0;
	char *buffer, *tmp, *ret_val;
	int buffer_len;
	struct php_fileinfo *finfo;
	zval *zfinfo, *zcontext = NULL;
	FILEINFO_DECLARE_INIT_OBJECT(object)

	if (object) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|lbz", &buffer, &buffer_len, &options, &zcontext) == FAILURE) {
			RETURN_FALSE;
		}
		FILEINFO_FROM_OBJECT(finfo, object);
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zs|lbz", &zfinfo, &buffer, &buffer_len, &options, &zcontext) == FAILURE) {
			RETURN_FALSE;
		}
		ZEND_FETCH_RESOURCE(finfo, struct php_fileinfo *, &zfinfo, -1, "file_info", le_fileinfo);
	}	

	/* Set options for the current file/buffer. */
	if (options) {
		FINFO_SET_OPTION(finfo->magic, options)
	}

	if (mode) { /* file */
		/* determine if the file is a local file or remote URL */
		char *tmp2;
		php_stream_wrapper *wrap = php_stream_locate_url_wrapper(buffer, &tmp2, 0 TSRMLS_DC);
		if (wrap && wrap->is_url) {
#ifdef ZEND_ENGINE_2
			php_stream_context *context = php_stream_context_from_zval(zcontext, 0);
#else 
			php_stream_context *context = NULL;
#endif			
			php_stream *stream = php_stream_open_wrapper_ex(buffer, "rb", 
					ENFORCE_SAFE_MODE | REPORT_ERRORS, NULL, context);
			if (!stream) {
				RETURN_FALSE;
			}
			buffer_len = php_stream_copy_to_mem(stream, &tmp, PHP_STREAM_COPY_ALL, 0);
			php_stream_close(stream);

			if (buffer_len == 0) {
				RETURN_FALSE;
			}
		} else { /* local file */
			char resolved_path[MAXPATHLEN];
			if (!VCWD_REALPATH(buffer, resolved_path)) {
				RETURN_FALSE;
			}

			ret_val = (char *) magic_file(finfo->magic, buffer);
			goto common;
		}
	} else { /* buffer */
		tmp = buffer;
	}

	ret_val = (char *) magic_buffer(finfo->magic, tmp, buffer_len);
	if (mode) {
		efree(tmp);
	}
common:
	/* Restore options */
	if (options) {
		FINFO_SET_OPTION(finfo->magic, finfo->options)
	}

	if (!ret_val) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Failed identify data %d:%s",
				magic_errno(finfo->magic), magic_error(finfo->magic));
		RETURN_FALSE;
	} else {
		RETURN_STRING(ret_val, 1);
	}
}

/* {{{ proto string finfo_file(resource finfo, char *file_name [, int options [, resource context]])
   Return information about a file. */
PHP_FUNCTION(finfo_file)
{
	_php_finfo_get_type(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto string finfo_buffer(resource finfo, char *string [, int options])
   Return infromation about a string buffer. */
PHP_FUNCTION(finfo_buffer)
{
	_php_finfo_get_type(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
