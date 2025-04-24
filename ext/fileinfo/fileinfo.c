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
  | Author: Ilia Alshanetsky <ilia@php.net>                              |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include "php.h"

#include "libmagic/magic.h"
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
#include "fileinfo_arginfo.h"
#include "fopen_wrappers.h" /* needed for is_url */
#include "Zend/zend_exceptions.h"

static zend_object_handlers finfo_object_handlers;
zend_class_entry *finfo_class_entry;

typedef struct _finfo_object {
	struct magic_set *magic;
	zend_object zo;
} finfo_object;

static inline finfo_object *php_finfo_fetch_object(zend_object *obj) {
	return (finfo_object *)((char*)(obj) - XtOffsetOf(finfo_object, zo));
}

#define Z_FINFO_P(zv) php_finfo_fetch_object(Z_OBJ_P((zv)))

/* {{{ finfo_objects_free */
static void finfo_objects_free(zend_object *object)
{
	finfo_object *intern = php_finfo_fetch_object(object);

	magic_close(intern->magic);
	zend_object_std_dtor(&intern->zo);
}
/* }}} */

/* {{{ finfo_objects_new */
PHP_FILEINFO_API zend_object *finfo_objects_new(zend_class_entry *class_type)
{
	finfo_object *intern;

	intern = zend_object_alloc(sizeof(finfo_object), class_type);

	zend_object_std_init(&intern->zo, class_type);
	object_properties_init(&intern->zo, class_type);

	return &intern->zo;
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(finfo)
{
	finfo_class_entry = register_class_finfo();
	finfo_class_entry->create_object = finfo_objects_new;
	finfo_class_entry->default_object_handlers = &finfo_object_handlers;

	/* copy the standard object handlers to you handler table */
	memcpy(&finfo_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	finfo_object_handlers.offset = XtOffsetOf(finfo_object, zo);
	finfo_object_handlers.free_obj = finfo_objects_free;
	finfo_object_handlers.clone_obj = NULL;

	register_fileinfo_symbols(module_number);

	return SUCCESS;
}
/* }}} */

/* {{{ fileinfo_module_entry */
zend_module_entry fileinfo_module_entry = {
	STANDARD_MODULE_HEADER,
	"fileinfo",
	ext_functions,
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

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(fileinfo)
{
	char magic_ver[15];

	int raw_version = magic_version();
	(void)snprintf(magic_ver, sizeof(magic_ver), "%d.%d", raw_version / 100, raw_version % 100);

	php_info_print_table_start();
	php_info_print_table_row(2, "fileinfo support", "enabled");
	php_info_print_table_row(2, "libmagic", magic_ver);
	php_info_print_table_end();
}
/* }}} */

/* {{{ Construct a new fileinfo object. */
PHP_FUNCTION(finfo_open)
{
	zend_long options = MAGIC_NONE;
	char *file = NULL;
	size_t file_len = 0;
	zval *object = getThis();
	char resolved_path[MAXPATHLEN];
	zend_error_handling zeh;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|lp!", &options, &file, &file_len) == FAILURE) {
		RETURN_THROWS();
	}

	if (object) {
		zend_replace_error_handling(EH_THROW, NULL, &zeh);

		magic_close(Z_FINFO_P(object)->magic);
		Z_FINFO_P(object)->magic = NULL;
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

	struct magic_set *magic = magic_open(options);

	if (magic == NULL) {
		php_error_docref(NULL, E_WARNING, "Invalid mode '" ZEND_LONG_FMT "'.", options);
		if (object) {
			zend_restore_error_handling(&zeh);
			if (!EG(exception)) {
				zend_throw_exception(NULL, "Constructor failed", 0);
			}
		}
		RETURN_FALSE;
	}

	if (magic_load(magic, file) == -1) {
		php_error_docref(NULL, E_WARNING, "Failed to load magic database at \"%s\"", file);
		magic_close(magic);
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
		finfo_object *obj = Z_FINFO_P(object);
		obj->magic = magic;
	} else {
		zend_object *zobj = finfo_objects_new(finfo_class_entry);
		finfo_object *obj = php_finfo_fetch_object(zobj);
		obj->magic = magic;
		RETURN_OBJ(zobj);
	}
}
/* }}} */

/* {{{ Close fileinfo object - a NOP. */
PHP_FUNCTION(finfo_close)
{
	zval *self;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &self, finfo_class_entry) == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ Set libmagic configuration options. */
PHP_FUNCTION(finfo_set_flags)
{
	zend_long options;
	zval *self;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Ol", &self, finfo_class_entry, &options) == FAILURE) {
		RETURN_THROWS();
	}

	if (!Z_FINFO_P(self)->magic) {
		zend_throw_error(NULL, "Invalid finfo object");
		RETURN_THROWS();
	}

	/* We do not check the return value as it can only ever fail if options contains MAGIC_PRESERVE_ATIME
	 * and the system neither has utime(3) nor utimes(2). Something incredibly unlikely. */
	magic_setflags(Z_FINFO_P(self)->magic, options);

	RETURN_TRUE;
}
/* }}} */

static const char* php_fileinfo_from_path(struct magic_set *magic, const zend_string *path, php_stream_context *context)
{
	ZEND_ASSERT(magic != NULL);
	ZEND_ASSERT(path);
	ZEND_ASSERT(ZSTR_LEN(path) != 0);
	ZEND_ASSERT(!zend_str_has_nul_byte(path));
	ZEND_ASSERT(context != NULL);

	/* determine if the file is a local file or remote URL */
	const char *dummy;
	php_stream_statbuf ssb;

	const php_stream_wrapper *wrap = php_stream_locate_url_wrapper(ZSTR_VAL(path), &dummy, 0);
	if (UNEXPECTED(wrap == NULL)) {
		return NULL;
	}

#ifdef PHP_WIN32
	if (php_stream_stat_path_ex(ZSTR_VAL(path), 0, &ssb, context) == SUCCESS) {
		if (ssb.sb.st_mode & S_IFDIR) {
			return "directory";
		}
	}
#endif

	php_stream *stream = php_stream_open_wrapper_ex(ZSTR_VAL(path), "rb", REPORT_ERRORS, NULL, context);
	if (!stream) {
		return NULL;
	}

	const char *ret_val = NULL;
	if (php_stream_stat(stream, &ssb) == SUCCESS) {
		if (ssb.sb.st_mode & S_IFDIR) {
			ret_val = "directory";
		} else {
			ret_val = magic_stream(magic, stream);
			if (UNEXPECTED(ret_val == NULL)) {
				php_error_docref(NULL, E_WARNING, "Failed identify data %d:%s", magic_errno(magic), magic_error(magic));
			}
		}
	}

	php_stream_close(stream);

	return ret_val;
}

/* Return information about a file. */
PHP_FUNCTION(finfo_file)
{
	zval *self;
	zend_string *path = NULL;
	zend_long options = 0;
	zval *zcontext = NULL;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "OP|lr!", &self, finfo_class_entry, &path, &options, &zcontext) == FAILURE) {
		RETURN_THROWS();
	}

	if (!Z_FINFO_P(self)->magic) {
		zend_throw_error(NULL, "Invalid finfo object");
		RETURN_THROWS();
	}

	struct magic_set *magic = Z_FINFO_P(self)->magic;

	if (UNEXPECTED(ZSTR_LEN(path) == 0)) {
		zend_argument_must_not_be_empty_error(2);
		RETURN_THROWS();
	}
	php_stream_context *context = php_stream_context_from_zval(zcontext, false);
	if (!context) {
		RETURN_THROWS();
	}

	/* Set options for the current file/buffer. */
	int old_options = magic_getflags(magic);
	if (options) {
		/* We do not check the return value as it can only ever fail if options contains MAGIC_PRESERVE_ATIME
		 * and the system neither has utime(3) nor utimes(2). Something incredibly unlikely. */
		magic_setflags(magic, options);
	}

	const char *ret_val = php_fileinfo_from_path(magic, path, context);

	/* Restore options */
	if (options) {
		magic_setflags(magic, old_options);
	}

	if (UNEXPECTED(ret_val == NULL)) {
		RETURN_FALSE;
	} else {
		RETURN_STRING(ret_val);
	}
}

/* Return information about a string buffer. */
PHP_FUNCTION(finfo_buffer)
{
	zval *self;
	zend_string *buffer = NULL;
	zend_long options = 0;
	zval *dummy_context = NULL;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "OS|lr!", &self, finfo_class_entry, &buffer, &options, &dummy_context) == FAILURE) {
		RETURN_THROWS();
	}

	if (!Z_FINFO_P(self)->magic) {
		zend_throw_error(NULL, "Invalid finfo object");
		RETURN_THROWS();
	}

	struct magic_set *magic = Z_FINFO_P(self)->magic;

	/* Set options for the current file/buffer. */
	int old_options = magic_getflags(magic);
	if (options) {
		/* We do not check the return value as it can only ever fail if options contains MAGIC_PRESERVE_ATIME
		 * and the system neither has utime(3) nor utimes(2). Something incredibly unlikely. */
		magic_setflags(magic, options);
	}

	const char *ret_val = magic_buffer(magic, ZSTR_VAL(buffer), ZSTR_LEN(buffer));

	/* Restore options */
	if (options) {
		magic_setflags(magic, old_options);
	}

	if (UNEXPECTED(ret_val == NULL)) {
		php_error_docref(NULL, E_WARNING, "Failed identify data %d:%s", magic_errno(magic), magic_error(magic));
		RETURN_FALSE;
	} else {
		RETURN_STRING(ret_val);
	}
}

/* Return content-type for file */
PHP_FUNCTION(mime_content_type)
{
	zval *path_or_stream;
	const zend_string *path = NULL;
	php_stream *stream = NULL;
	struct magic_set *magic = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &path_or_stream) == FAILURE) {
		RETURN_THROWS();
	}

	switch (Z_TYPE_P(path_or_stream)) {
		case IS_STRING:
			path = Z_STR_P(path_or_stream);
			if (UNEXPECTED(ZSTR_LEN(path) == 0)) {
				zend_argument_must_not_be_empty_error(1);
				RETURN_THROWS();
			}
			if (UNEXPECTED(zend_str_has_nul_byte(path))) {
				zend_argument_type_error(1, "must not contain any null bytes");
				RETURN_THROWS();
			}
			break;

		case IS_RESOURCE:
			php_stream_from_zval(stream, path_or_stream);
			break;

		default:
			zend_argument_type_error(1, "must be of type resource|string, %s given", zend_zval_value_name(path_or_stream));
			RETURN_THROWS();
	}

	magic = magic_open(MAGIC_MIME_TYPE);
	if (UNEXPECTED(magic == NULL)) {
		php_error_docref(NULL, E_WARNING, "Failed to load magic database");
		RETURN_FALSE;
	}

	if (UNEXPECTED(magic_load(magic, NULL) == -1)) {
		php_error_docref(NULL, E_WARNING, "Failed identify data %d:%s", magic_errno(magic), magic_error(magic));
		magic_close(magic);
		RETURN_FALSE;
	}

	const char *ret_val;
	if (path) {
		php_stream_context *context = php_stream_context_get_default(false);
		ret_val = php_fileinfo_from_path(magic, path, context);
	} else {
		/* remember stream position for restoration */
		zend_off_t current_stream_pos = php_stream_tell(stream);
		php_stream_seek(stream, 0, SEEK_SET);

		ret_val = magic_stream(magic, stream);
		if (UNEXPECTED(ret_val == NULL)) {
			php_error_docref(NULL, E_WARNING, "Failed identify data %d:%s", magic_errno(magic), magic_error(magic));
		}

		php_stream_seek(stream, current_stream_pos, SEEK_SET);
	}

	if (UNEXPECTED(ret_val == NULL)) {
		RETVAL_FALSE;
	} else {
		RETVAL_STRING(ret_val);
	}
	magic_close(magic);
}
