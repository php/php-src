/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2017 The PHP Group                                |
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
#include "Zend/zend_exceptions.h"

/* {{{ macros and type definitions */
typedef struct _php_fileinfo {
	zend_long options;
	struct magic_set *magic;
} php_fileinfo;

static zend_object_handlers finfo_object_handlers;
zend_class_entry *finfo_class_entry;

typedef struct _finfo_object {
	php_fileinfo *ptr;
	zend_object zo;
} finfo_object;

#define FILEINFO_DECLARE_INIT_OBJECT(object) \
	zval *object = ZEND_IS_METHOD_CALL() ? getThis() : NULL;

static inline finfo_object *php_finfo_fetch_object(zend_object *obj) {
	return (finfo_object *)((char*)(obj) - XtOffsetOf(finfo_object, zo));
}

#define Z_FINFO_P(zv) php_finfo_fetch_object(Z_OBJ_P((zv)))

#define FILEINFO_REGISTER_OBJECT(_object, _ptr) \
{ \
	finfo_object *obj; \
    obj = Z_FINFO_P(_object); \
    obj->ptr = _ptr; \
}

#define FILEINFO_FROM_OBJECT(finfo, object) \
{ \
	finfo_object *obj = Z_FINFO_P(object); \
	finfo = obj->ptr; \
	if (!finfo) { \
        	php_error_docref(NULL, E_WARNING, "The invalid fileinfo object."); \
		RETURN_FALSE; \
	} \
}

/* {{{ finfo_objects_free
 */
static void finfo_objects_free(zend_object *object)
{
	finfo_object *intern = php_finfo_fetch_object(object);

	if (intern->ptr) {
		magic_close(intern->ptr->magic);
		efree(intern->ptr);
	}

	zend_object_std_dtor(&intern->zo);
}
/* }}} */

/* {{{ finfo_objects_new
 */
PHP_FILEINFO_API zend_object *finfo_objects_new(zend_class_entry *class_type)
{
	finfo_object *intern;

	intern = ecalloc(1, sizeof(finfo_object) + zend_object_properties_size(class_type));

	zend_object_std_init(&intern->zo, class_type);
	object_properties_init(&intern->zo, class_type);
	intern->zo.handlers = &finfo_object_handlers;

	return &intern->zo;
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
zend_function_entry finfo_class_functions[] = {
	ZEND_ME_MAPPING(finfo,          finfo_open,     arginfo_finfo_open, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(set_flags,      finfo_set_flags,arginfo_finfo_method_set_flags, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(file,           finfo_file,     arginfo_finfo_method_file, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(buffer,         finfo_buffer,   arginfo_finfo_method_buffer, ZEND_ACC_PUBLIC)
	PHP_FE_END
};
/* }}} */

#define FINFO_SET_OPTION(magic, options) \
	if (magic_setflags(magic, options) == -1) { \
		php_error_docref(NULL, E_WARNING, "Failed to set option '%pd' %d:%s", \
				options, magic_errno(magic), magic_error(magic)); \
		RETURN_FALSE; \
	}

/* True global resources - no need for thread safety here */
static int le_fileinfo;
/* }}} */

void finfo_resource_destructor(zend_resource *rsrc) /* {{{ */
{
	if (rsrc->ptr) {
		php_fileinfo *finfo = (php_fileinfo *) rsrc->ptr;
		magic_close(finfo->magic);
		efree(rsrc->ptr);
		rsrc->ptr = NULL;
	}
}
/* }}} */


/* {{{ fileinfo_functions[]
 */
zend_function_entry fileinfo_functions[] = {
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
	zend_class_entry _finfo_class_entry;
	INIT_CLASS_ENTRY(_finfo_class_entry, "finfo", finfo_class_functions);
	_finfo_class_entry.create_object = finfo_objects_new;
	finfo_class_entry = zend_register_internal_class(&_finfo_class_entry);

	/* copy the standard object handlers to you handler table */
	memcpy(&finfo_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	finfo_object_handlers.offset = XtOffsetOf(finfo_object, zo);
	finfo_object_handlers.free_obj = finfo_objects_free;

	le_fileinfo = zend_register_list_destructors_ex(finfo_resource_destructor, NULL, "file_info", module_number);

	REGISTER_LONG_CONSTANT("FILEINFO_NONE",			MAGIC_NONE, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILEINFO_SYMLINK",		MAGIC_SYMLINK, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILEINFO_MIME",			MAGIC_MIME, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILEINFO_MIME_TYPE",	MAGIC_MIME_TYPE, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILEINFO_MIME_ENCODING",MAGIC_MIME_ENCODING, CONST_CS|CONST_PERSISTENT);
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
	STANDARD_MODULE_HEADER,
	"fileinfo",
	fileinfo_functions,
	PHP_MINIT(finfo),
	NULL,
	NULL,
	NULL,
	PHP_MINFO(fileinfo),
	PHP_FILEINFO_VERSION,
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
	char magic_ver[5];

	(void)snprintf(magic_ver, 4, "%d", magic_version());
	magic_ver[4] = '\0';

	php_info_print_table_start();
	php_info_print_table_row(2, "fileinfo support", "enabled");
	php_info_print_table_row(2, "version", PHP_FILEINFO_VERSION);
	php_info_print_table_row(2, "libmagic", magic_ver);
	php_info_print_table_end();
}
/* }}} */

/* {{{ proto resource finfo_open([int options [, string arg]])
   Create a new fileinfo resource. */
PHP_FUNCTION(finfo_open)
{
	zend_long options = MAGIC_NONE;
	char *file = NULL;
	size_t file_len = 0;
	php_fileinfo *finfo;
	FILEINFO_DECLARE_INIT_OBJECT(object)
	char resolved_path[MAXPATHLEN];
	zend_error_handling zeh;
	int flags = object ? ZEND_PARSE_PARAMS_THROW : 0;

	if (zend_parse_parameters_ex(flags, ZEND_NUM_ARGS(), "|lp", &options, &file, &file_len) == FAILURE) {
		RETURN_FALSE;
	}

	if (object) {
		finfo_object *finfo_obj = Z_FINFO_P(object);

		zend_replace_error_handling(EH_THROW, NULL, &zeh);

		if (finfo_obj->ptr) {
			magic_close(finfo_obj->ptr->magic);
			efree(finfo_obj->ptr);
			finfo_obj->ptr = NULL;
		}
	}

	if (file_len == 0) {
		file = NULL;
	} else if (file && *file) { /* user specified file, perform open_basedir checks */

		if (php_check_open_basedir(file)) {
			if (object) {
				zend_restore_error_handling(&zeh);
				if (!EG(exception)) {
					zend_throw_exception(NULL, "Constructor failed", 0);
				}
			}
			RETURN_FALSE;
		}
		if (!expand_filepath_with_mode(file, resolved_path, NULL, 0, CWD_EXPAND)) {
			if (object) {
				zend_restore_error_handling(&zeh);
				if (!EG(exception)) {
					zend_throw_exception(NULL, "Constructor failed", 0);
				}
			}
			RETURN_FALSE;
		}
		file = resolved_path;
	}

	finfo = emalloc(sizeof(php_fileinfo));

	finfo->options = options;
	finfo->magic = magic_open(options);

	if (finfo->magic == NULL) {
		efree(finfo);
		php_error_docref(NULL, E_WARNING, "Invalid mode '%pd'.", options);
		if (object) {
			zend_restore_error_handling(&zeh);
			if (!EG(exception)) {
				zend_throw_exception(NULL, "Constructor failed", 0);
			}
		}
		RETURN_FALSE;
	}

	if (magic_load(finfo->magic, file) == -1) {
		php_error_docref(NULL, E_WARNING, "Failed to load magic database at '%s'.", file);
		magic_close(finfo->magic);
		efree(finfo);
		if (object) {
			zend_restore_error_handling(&zeh);
			if (!EG(exception)) {
				zend_throw_exception(NULL, "Constructor failed", 0);
			}
		}
		RETURN_FALSE;
	}

	if (object) {
		zend_restore_error_handling(&zeh);
		FILEINFO_REGISTER_OBJECT(object, finfo);
	} else {
		RETURN_RES(zend_register_resource(finfo, le_fileinfo));
	}
}
/* }}} */

/* {{{ proto resource finfo_close(resource finfo)
   Close fileinfo resource. */
PHP_FUNCTION(finfo_close)
{
	php_fileinfo *finfo;
	zval *zfinfo;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &zfinfo) == FAILURE) {
		RETURN_FALSE;
	}

	if ((finfo = (php_fileinfo *)zend_fetch_resource(Z_RES_P(zfinfo), "file_info", le_fileinfo)) == NULL) {
		RETURN_FALSE;
	}

	zend_list_close(Z_RES_P(zfinfo));

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool finfo_set_flags(resource finfo, int options)
   Set libmagic configuration options. */
PHP_FUNCTION(finfo_set_flags)
{
	zend_long options;
	php_fileinfo *finfo;
	zval *zfinfo;
	FILEINFO_DECLARE_INIT_OBJECT(object)

	if (object) {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &options) == FAILURE) {
			RETURN_FALSE;
		}
		FILEINFO_FROM_OBJECT(finfo, object);
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "rl", &zfinfo, &options) == FAILURE) {
			RETURN_FALSE;
		}
		if ((finfo = (php_fileinfo *)zend_fetch_resource(Z_RES_P(zfinfo), "file_info", le_fileinfo)) == NULL) {
			RETURN_FALSE;
		}
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
	zend_long options = 0;
	char *ret_val = NULL, *buffer = NULL;
	size_t buffer_len;
	php_fileinfo *finfo = NULL;
	zval *zfinfo, *zcontext = NULL;
	zval *what;
	char mime_directory[] = "directory";

	struct magic_set *magic = NULL;
	FILEINFO_DECLARE_INIT_OBJECT(object)

	if (mimetype_emu) {

		/* mime_content_type(..) emulation */
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &what) == FAILURE) {
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
				php_error_docref(NULL, E_WARNING, "Can only process string or stream arguments");
				RETURN_FALSE;
		}

		magic = magic_open(MAGIC_MIME_TYPE);
		if (magic_load(magic, NULL) == -1) {
			php_error_docref(NULL, E_WARNING, "Failed to load magic database.");
			goto common;
		}
	} else if (object) {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|lr", &buffer, &buffer_len, &options, &zcontext) == FAILURE) {
			RETURN_FALSE;
		}
		FILEINFO_FROM_OBJECT(finfo, object);
		magic = finfo->magic;
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "rs|lr", &zfinfo, &buffer, &buffer_len, &options, &zcontext) == FAILURE) {
			RETURN_FALSE;
		}
		if ((finfo = (php_fileinfo *)zend_fetch_resource(Z_RES_P(zfinfo), "file_info", le_fileinfo)) == NULL) {
			RETURN_FALSE;
		}
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
				zend_off_t streampos;

				php_stream_from_zval_no_verify(stream, what);
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
			const char *tmp2;
			php_stream_wrapper *wrap;
			php_stream_statbuf ssb;

			if (buffer == NULL || !*buffer) {
				php_error_docref(NULL, E_WARNING, "Empty filename or path");
				RETVAL_FALSE;
				goto clean;
			}
			if (CHECK_NULL_PATH(buffer, buffer_len)) {
				php_error_docref(NULL, E_WARNING, "Invalid path");
				RETVAL_FALSE;
				goto clean;
			}

			wrap = php_stream_locate_url_wrapper(buffer, &tmp2, 0);

			if (wrap) {
				php_stream *stream;
				php_stream_context *context = php_stream_context_from_zval(zcontext, 0);

#ifdef PHP_WIN32
				if (php_stream_stat_path_ex(buffer, 0, &ssb, context) == SUCCESS) {
					if (ssb.sb.st_mode & S_IFDIR) {
						ret_val = mime_directory;
						goto common;
					}
				}
#endif

#if PHP_API_VERSION < 20100412
				stream = php_stream_open_wrapper_ex(buffer, "rb", ENFORCE_SAFE_MODE | REPORT_ERRORS, NULL, context);
#else
				stream = php_stream_open_wrapper_ex(buffer, "rb", REPORT_ERRORS, NULL, context);
#endif

				if (!stream) {
					RETVAL_FALSE;
					goto clean;
				}

				if (php_stream_stat(stream, &ssb) == SUCCESS) {
					if (ssb.sb.st_mode & S_IFDIR) {
						ret_val = mime_directory;
					} else {
						ret_val = (char *)magic_stream(magic, stream);
					}
				}

				php_stream_close(stream);
			}
			break;
		}

		default:
			php_error_docref(NULL, E_WARNING, "Can only process string or stream arguments");
	}

common:
	if (ret_val) {
		RETVAL_STRING(ret_val);
	} else {
		php_error_docref(NULL, E_WARNING, "Failed identify data %d:%s", magic_errno(magic), magic_error(magic));
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
