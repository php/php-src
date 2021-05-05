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
#include "php.h"

#include <magic.h>
/* 
 * HOWMANY specifies the maximum offset libmagic will look at
 * this is currently hardcoded in the libmagic source but not exported
 */
#ifndef HOWMANY
#define HOWMANY 65536
#endif

#include "php_ini.h"
#include "ext/standard/info.h"
#include "ext/standard/file.h" /* needed for context stuff */
#include "php_fileinfo.h"
#include "fopen_wrappers.h" /* needed for is_url */

#ifndef _S_IFDIR
# define _S_IFDIR		S_IFDIR
#endif

/* {{{ macros and type definitions */
struct php_fileinfo {
	long options;
	struct magic_set *magic;
};

#ifdef ZEND_ENGINE_2
/* {{{ */
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

	zend_object_std_dtor(&intern->zo TSRMLS_CC);
	efree(intern);
}
/* }}} */

/* {{{ finfo_objects_new
 */
PHP_FILEINFO_API zend_object_value finfo_objects_new(zend_class_entry *class_type TSRMLS_DC)
{
	zend_object_value retval;
	struct finfo_object *intern;
	zval *tmp;

	intern = emalloc(sizeof(struct finfo_object));

	zend_object_std_init(&intern->zo, class_type TSRMLS_CC);
	zend_hash_copy(intern->zo.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref,(void *) &tmp, sizeof(zval *));

	intern->ptr = NULL;

	retval.handle = zend_objects_store_put(intern, finfo_objects_dtor, NULL, NULL TSRMLS_CC);
	retval.handlers = (zend_object_handlers *) &finfo_object_handlers;

	return retval;
}
/* }}} */

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_finfo_open, 0, 0, 0)
	ZEND_ARG_INFO(0, options)
	ZEND_ARG_INFO(0, arg)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_finfo_close, 0, 0, 1)
	ZEND_ARG_INFO(0, finfo)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_finfo_set_flags, 0, 0, 2)
	ZEND_ARG_INFO(0, finfo)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_finfo_method_set_flags, 0, 0, 1)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_finfo_file, 0, 0, 2)
	ZEND_ARG_INFO(0, finfo)
	ZEND_ARG_INFO(0, filename)
	ZEND_ARG_INFO(0, options)
	ZEND_ARG_INFO(0, context)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_finfo_method_file, 0, 0, 1)
	ZEND_ARG_INFO(0, filename)
	ZEND_ARG_INFO(0, options)
	ZEND_ARG_INFO(0, context)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_finfo_buffer, 0, 0, 2)
	ZEND_ARG_INFO(0, finfo)
	ZEND_ARG_INFO(0, string)
	ZEND_ARG_INFO(0, options)
	ZEND_ARG_INFO(0, context)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_finfo_method_buffer, 0, 0, 1)
	ZEND_ARG_INFO(0, string)
	ZEND_ARG_INFO(0, options)
	ZEND_ARG_INFO(0, context)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mime_content_type, 0, 0, 1)
	ZEND_ARG_INFO(0, string)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ finfo_class_functions
 */
function_entry finfo_class_functions[] = {
#if PHP_VERSION_ID >= 50200
	ZEND_ME_MAPPING(finfo,          finfo_open,     arginfo_finfo_open, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(set_flags,      finfo_set_flags,arginfo_finfo_method_set_flags, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(file,           finfo_file,     arginfo_finfo_method_file, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(buffer,         finfo_buffer,   arginfo_finfo_method_buffer, ZEND_ACC_PUBLIC)
#else
	ZEND_ME_MAPPING(finfo,          finfo_open,     arginfo_finfo_open)
	ZEND_ME_MAPPING(set_flags,      finfo_set_flags,arginfo_finfo_method_set_flags)
	ZEND_ME_MAPPING(file,           finfo_file,     arginfo_finfo_method_file)
	ZEND_ME_MAPPING(buffer,         finfo_buffer,   arginfo_finfo_method_buffer)
#endif
	{NULL, NULL, NULL}
};
/* }}} */

/* }}} */
#else 
/* {{{ */
#define FILEINFO_REGISTER_OBJECT(_object, _ptr) {}
#define FILEINFO_FROM_OBJECT(socket_id, object) {}

#define FILEINFO_DECLARE_INIT_OBJECT(object)
#define object 0

/* }}} */
#endif /* ZEND_ENGINE_2 */

#define FINFO_SET_OPTION(magic, options) \
	if (magic_setflags(magic, options) == -1) { \
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Failed to set option '%ld' %d:%s", \
				options, magic_errno(magic), magic_error(magic)); \
		RETURN_FALSE; \
	}

/* True global resources - no need for thread safety here */
static int le_fileinfo;
/* }}} */

void finfo_resource_destructor(zend_rsrc_list_entry *rsrc TSRMLS_DC) /* {{{ */
{
	if (rsrc->ptr) {
		struct php_fileinfo *finfo = (struct php_fileinfo *) rsrc->ptr;
		magic_close(finfo->magic);
		efree(rsrc->ptr);
		rsrc->ptr = NULL;
	}
}
/* }}} */


/* {{{ fileinfo_functions[]
 */
function_entry fileinfo_functions[] = {
	PHP_FE(finfo_open,		arginfo_finfo_open)
	PHP_FE(finfo_close,		arginfo_finfo_close)
	PHP_FE(finfo_set_flags,	arginfo_finfo_set_flags)
	PHP_FE(finfo_file,		arginfo_finfo_file)
	PHP_FE(finfo_buffer,	arginfo_finfo_buffer)
	PHP_FE(mime_content_type, arginfo_mime_content_type)
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
/*	REGISTER_LONG_CONSTANT("FILEINFO_COMPRESS",		MAGIC_COMPRESS, CONST_CS|CONST_PERSISTENT); disabled, as it does fork now */
	REGISTER_LONG_CONSTANT("FILEINFO_DEVICES",		MAGIC_DEVICES, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILEINFO_CONTINUE",		MAGIC_CONTINUE, CONST_CS|CONST_PERSISTENT);
#ifdef MAGIC_PRESERVE_ATIME
	REGISTER_LONG_CONSTANT("FILEINFO_PRESERVE_ATIME",	MAGIC_PRESERVE_ATIME, CONST_CS|CONST_PERSISTENT);
#endif
#ifdef MAGIC_RAW
	REGISTER_LONG_CONSTANT("FILEINFO_RAW",			MAGIC_RAW, CONST_CS|CONST_PERSISTENT);
#endif

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
	PHP_FILEINFO_VERSION,
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_FILEINFO
ZEND_GET_MODULE(fileinfo)
#endif

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(fileinfo)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "fileinfo support", "enabled");
	php_info_print_table_row(2, "version", PHP_FILEINFO_VERSION);
	php_info_print_table_end();
}
/* }}} */

/* {{{ proto resource finfo_open([int options [, string arg]])
   Create a new fileinfo resource. */
PHP_FUNCTION(finfo_open)
{
	long options = MAGIC_NONE;
	char *file = NULL;
	int file_len = 0;
	struct php_fileinfo *finfo;
	FILEINFO_DECLARE_INIT_OBJECT(object)
	char resolved_path[MAXPATHLEN];

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|ls", &options, &file, &file_len) == FAILURE) {
		RETURN_FALSE;
	}

	if (file && *file) { /* user specified file, perform open_basedir checks */
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
		magic_close(finfo->magic);
		efree(finfo);
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

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zfinfo) == FAILURE) {
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
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &zfinfo, &options) == FAILURE) {
			RETURN_FALSE;
		}
		ZEND_FETCH_RESOURCE(finfo, struct php_fileinfo *, &zfinfo, -1, "file_info", le_fileinfo);
	}

	FINFO_SET_OPTION(finfo->magic, options)
	finfo->options = options;

	RETURN_TRUE;
}
/* }}} */

#define FILEINFO_MODE_BUFFER 0
#define FILEINFO_MODE_STREAM 1
#define FILEINFO_MODE_FILE 2

static void _php_finfo_get_type(INTERNAL_FUNCTION_PARAMETERS, int mode, int mimetype_emu) /* {{{ */
{
	long options = 0;
	char *ret_val = NULL, *buffer = NULL;
	int buffer_len;
	struct php_fileinfo *finfo;
	zval *zfinfo, *zcontext = NULL;
	zval *what;
	char mime_directory[] = "directory";

	struct magic_set *magic = NULL;
	FILEINFO_DECLARE_INIT_OBJECT(object)

	if (mimetype_emu) {

		/* mime_content_type(..) emulation */
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &what) == FAILURE) {
			return;
		}

		switch (Z_TYPE_P(what)) {
			case IS_STRING:
				buffer = Z_STRVAL_P(what);
				buffer_len = Z_STRLEN_P(what);
				mode = FILEINFO_MODE_FILE;
				break;

			case IS_RESOURCE:
				mode = FILEINFO_MODE_STREAM;
				break;

			default:
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Can only process string or stream arguments");
				RETURN_FALSE;
		}

		magic = magic_open(MAGIC_MIME);
		if (magic_load(magic, NULL) == -1) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Failed to load magic database.");
			goto common;
		}
	} else if (object) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|lr", &buffer, &buffer_len, &options, &zcontext) == FAILURE) {
			RETURN_FALSE;
		}
		FILEINFO_FROM_OBJECT(finfo, object);
		magic = finfo->magic;
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs|lr", &zfinfo, &buffer, &buffer_len, &options, &zcontext) == FAILURE) {
			RETURN_FALSE;
		}
		ZEND_FETCH_RESOURCE(finfo, struct php_fileinfo *, &zfinfo, -1, "file_info", le_fileinfo);
		magic = finfo->magic;
	}	

	/* Set options for the current file/buffer. */
	if (options) {
		FINFO_SET_OPTION(magic, options)
	}

	switch (mode) {
		case FILEINFO_MODE_BUFFER:
		{
			ret_val = (char *) magic_buffer(magic, buffer, buffer_len);
			break;
		}

		case FILEINFO_MODE_STREAM:
		{
				php_stream *stream;
				off_t streampos;

				php_stream_from_zval_no_verify(stream, &what);
				if (!stream) {
					goto common;
				}

				streampos = php_stream_tell(stream); /* remember stream position for restoration */
				php_stream_seek(stream, 0, SEEK_SET);

				ret_val = (char *) magic_stream(magic, stream);

				php_stream_seek(stream, streampos, SEEK_SET);
				break;
		}

		case FILEINFO_MODE_FILE:
		{
			/* determine if the file is a local file or remote URL */
			char *tmp2;
			php_stream_wrapper *wrap;
			struct stat sb;

			if (buffer == NULL || !*buffer) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Empty filename or path");
				RETVAL_FALSE;
				goto clean;
			}

			if (php_sys_stat(buffer, &sb) == 0) {
					  if (sb.st_mode & _S_IFDIR) {
								 ret_val = mime_directory;
								 goto common;
					  }
			} else {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "File or path not found '%s'", buffer);
				RETVAL_FALSE;
				goto clean;
			}

			wrap = php_stream_locate_url_wrapper(buffer, &tmp2, 0 TSRMLS_CC);

			if (wrap) {
				php_stream_context *context = php_stream_context_from_zval(zcontext, 0);

				php_stream *stream = php_stream_open_wrapper_ex(buffer, "rb", ENFORCE_SAFE_MODE | REPORT_ERRORS, NULL, context);

				if (!stream) {
					RETVAL_FALSE;
					goto clean;
				}

				ret_val = (char *)magic_stream(magic, stream);
				php_stream_close(stream);
			}
			break;
		}

		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Can only process string or stream arguments");
	}

common:
	if (ret_val) {
		RETVAL_STRING(ret_val, 1);
	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Failed identify data %d:%s", magic_errno(magic), magic_error(magic));
		RETVAL_FALSE;
	}

clean:
	if (mimetype_emu) {
		magic_close(magic);
	}

	/* Restore options */
	if (options) {
		FINFO_SET_OPTION(magic, finfo->options)
	}
	return;
}
/* }}} */

/* {{{ proto string finfo_file(resource finfo, char *file_name [, int options [, resource context]])
   Return information about a file. */
PHP_FUNCTION(finfo_file)
{
	_php_finfo_get_type(INTERNAL_FUNCTION_PARAM_PASSTHRU, FILEINFO_MODE_FILE, 0);
}
/* }}} */

/* {{{ proto string finfo_buffer(resource finfo, char *string [, int options [, resource context]])
   Return infromation about a string buffer. */
PHP_FUNCTION(finfo_buffer)
{
	_php_finfo_get_type(INTERNAL_FUNCTION_PARAM_PASSTHRU, FILEINFO_MODE_BUFFER, 0);
}
/* }}} */

/* {{{ proto string mime_content_type(string filename|resource stream)
   Return content-type for file */
PHP_FUNCTION(mime_content_type)
{
	_php_finfo_get_type(INTERNAL_FUNCTION_PARAM_PASSTHRU, -1, 1);
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
