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
   | Author: Marcus Boerger <helly@php.net>                               |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/file.h"
#include "ext/standard/php_filestat.h"
#include "ext/standard/flock_compat.h"
#include "ext/standard/scanf.h"
#include "ext/standard/php_string.h"
#include "zend_exceptions.h"
#include "zend_interfaces.h"

#include "php_spl.h"
#include "spl_functions.h"
#include "spl_engine.h"
#include "spl_iterators.h"
#include "spl_directory.h"
#include "spl_directory_arginfo.h"
#include "spl_exceptions.h"

#define SPL_HAS_FLAG(flags, test_flag) ((flags & test_flag) ? 1 : 0)

/* declare the class handlers */
static zend_object_handlers spl_filesystem_object_handlers;
/* includes handler to validate object state when retrieving methods */
static zend_object_handlers spl_filesystem_object_check_handlers;

/* decalre the class entry */
PHPAPI zend_class_entry *spl_ce_SplFileInfo;
PHPAPI zend_class_entry *spl_ce_DirectoryIterator;
PHPAPI zend_class_entry *spl_ce_FilesystemIterator;
PHPAPI zend_class_entry *spl_ce_RecursiveDirectoryIterator;
PHPAPI zend_class_entry *spl_ce_GlobIterator;
PHPAPI zend_class_entry *spl_ce_SplFileObject;
PHPAPI zend_class_entry *spl_ce_SplTempFileObject;

/* Object helper */
static inline spl_filesystem_object *spl_filesystem_from_obj(zend_object *obj) /* {{{ */ {
	return (spl_filesystem_object*)((char*)(obj) - XtOffsetOf(spl_filesystem_object, std));
}
/* }}} */

/* define an overloaded iterator structure */
typedef struct {
	zend_object_iterator  intern;
	zval                  current;
	void                 *object;
} spl_filesystem_iterator;

static inline spl_filesystem_iterator* spl_filesystem_object_to_iterator(spl_filesystem_object *obj)
{
	spl_filesystem_iterator    *it;

	it = ecalloc(1, sizeof(spl_filesystem_iterator));
	it->object = (void *)obj;
	zend_iterator_init(&it->intern);
	return it;
}

static inline spl_filesystem_object* spl_filesystem_iterator_to_object(spl_filesystem_iterator *it)
{
	return (spl_filesystem_object*)it->object;
}

#define CHECK_SPL_FILE_OBJECT_IS_INITIALIZED(spl_filesystem_object_pointer) \
	if (!(spl_filesystem_object_pointer)->u.file.stream) { \
		zend_throw_error(NULL, "Object not initialized"); \
		RETURN_THROWS(); \
	}

#define CHECK_DIRECTORY_ITERATOR_IS_INITIALIZED(intern) \
	if (!(intern)->u.dir.dirp) { \
		zend_throw_error(NULL, "Object not initialized"); \
		RETURN_THROWS(); \
	}

static void spl_filesystem_file_free_line(spl_filesystem_object *intern) /* {{{ */
{
	if (intern->u.file.current_line) {
		efree(intern->u.file.current_line);
		intern->u.file.current_line = NULL;
	}
	if (!Z_ISUNDEF(intern->u.file.current_zval)) {
		zval_ptr_dtor(&intern->u.file.current_zval);
		ZVAL_UNDEF(&intern->u.file.current_zval);
	}
} /* }}} */

static void spl_filesystem_object_destroy_object(zend_object *object) /* {{{ */
{
	spl_filesystem_object *intern = spl_filesystem_from_obj(object);

	zend_objects_destroy_object(object);

	switch(intern->type) {
	case SPL_FS_DIR:
		if (intern->u.dir.dirp) {
			php_stream_close(intern->u.dir.dirp);
			intern->u.dir.dirp = NULL;
		}
		break;
	case SPL_FS_FILE:
		if (intern->u.file.stream) {
			/*
			if (intern->u.file.zcontext) {
			   zend_list_delref(Z_RESVAL_P(intern->zcontext));
			}
			*/
			if (!intern->u.file.stream->is_persistent) {
				php_stream_close(intern->u.file.stream);
			} else {
				php_stream_pclose(intern->u.file.stream);
			}
			intern->u.file.stream = NULL;
			ZVAL_UNDEF(&intern->u.file.zresource);
		}
		break;
	default:
		break;
	}
} /* }}} */

static void spl_filesystem_object_free_storage(zend_object *object) /* {{{ */
{
	spl_filesystem_object *intern = spl_filesystem_from_obj(object);

	if (intern->oth_handler && intern->oth_handler->dtor) {
		intern->oth_handler->dtor(intern);
	}

	zend_object_std_dtor(&intern->std);

	if (intern->path) {
		zend_string_release(intern->path);
	}
	if (intern->file_name) {
		zend_string_release(intern->file_name);
	}
	switch(intern->type) {
	case SPL_FS_INFO:
		break;
	case SPL_FS_DIR:
		if (intern->u.dir.sub_path) {
			zend_string_release(intern->u.dir.sub_path);
		}
		break;
	case SPL_FS_FILE:
		if (intern->u.file.open_mode) {
			zend_string_release(intern->u.file.open_mode);
		}
		if (intern->orig_path) {
			zend_string_release(intern->orig_path);
		}
		spl_filesystem_file_free_line(intern);
		break;
	}
} /* }}} */

/* {{{ spl_ce_dir_object_new */
/* creates the object by
   - allocating memory
   - initializing the object members
   - storing the object
   - setting it's handlers

   called from
   - clone
   - new
 */
static zend_object *spl_filesystem_object_new_ex(zend_class_entry *class_type)
{
	spl_filesystem_object *intern;

	intern = emalloc(sizeof(spl_filesystem_object) + zend_object_properties_size(class_type));
	memset(intern, 0,
		MAX(XtOffsetOf(spl_filesystem_object, u.dir.entry),
			XtOffsetOf(spl_filesystem_object, u.file.escape) + sizeof(int)));
	/* intern->type = SPL_FS_INFO; done by set 0 */
	intern->file_class = spl_ce_SplFileObject;
	intern->info_class = spl_ce_SplFileInfo;

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);
	intern->std.handlers = &spl_filesystem_object_handlers;

	return &intern->std;
}
/* }}} */

/* {{{ spl_filesystem_object_new */
/* See spl_filesystem_object_new_ex */
static zend_object *spl_filesystem_object_new(zend_class_entry *class_type)
{
	return spl_filesystem_object_new_ex(class_type);
}
/* }}} */

/* {{{ spl_filesystem_object_new_check */
static zend_object *spl_filesystem_object_new_check(zend_class_entry *class_type)
{
	spl_filesystem_object *ret = spl_filesystem_from_obj(spl_filesystem_object_new_ex(class_type));
	ret->std.handlers = &spl_filesystem_object_check_handlers;
	return &ret->std;
}
/* }}} */

PHPAPI zend_string *spl_filesystem_object_get_path(spl_filesystem_object *intern) /* {{{ */
{
#ifdef HAVE_GLOB
	if (intern->type == SPL_FS_DIR && php_stream_is(intern->u.dir.dirp, &php_glob_stream_ops)) {
		size_t len = 0;
		char *tmp = php_glob_stream_get_path(intern->u.dir.dirp, &len);
		if (len == 0) {
			return NULL;
		}
		return zend_string_init(tmp, len, /* persistent */ false);
	}
#endif
	if (!intern->path) {
		return NULL;
	}
	return zend_string_copy(intern->path);
} /* }}} */

static inline zend_result spl_filesystem_object_get_file_name(spl_filesystem_object *intern) /* {{{ */
{
	if (intern->file_name) {
		/* already known */
		return SUCCESS;
	}

	switch (intern->type) {
		case SPL_FS_INFO:
		case SPL_FS_FILE:
			zend_throw_error(NULL, "Object not initialized");
			return FAILURE;
		case SPL_FS_DIR: {
			size_t name_len;
			zend_string *path;
			char slash = SPL_HAS_FLAG(intern->flags, SPL_FILE_DIR_UNIXPATHS) ? '/' : DEFAULT_SLASH;

			path = spl_filesystem_object_get_path(intern);
			/* if there is parent path, amend it, otherwise just use the given path as is */
			name_len = strlen(intern->u.dir.entry.d_name);
			if (!path) {
				intern->file_name = zend_string_init(intern->u.dir.entry.d_name, name_len, 0);
				return SUCCESS;
			}

			ZEND_ASSERT(ZSTR_LEN(path) != 0);
			intern->file_name = zend_string_concat3(
				ZSTR_VAL(path), ZSTR_LEN(path), &slash, 1, intern->u.dir.entry.d_name, name_len);
			zend_string_release_ex(path, /* persistent */ false);
			break;
		}
	}
	return SUCCESS;
} /* }}} */

/* TODO Make void or have callers check return value */
static bool spl_filesystem_dir_read(spl_filesystem_object *intern) /* {{{ */
{
	if (intern->file_name) {
		/* invalidate */
		zend_string_release(intern->file_name);
		intern->file_name = NULL;
	}
	if (!intern->u.dir.dirp || !php_stream_readdir(intern->u.dir.dirp, &intern->u.dir.entry)) {
		intern->u.dir.entry.d_name[0] = '\0';
		return 0;
	} else {
		return 1;
	}
}
/* }}} */

#define IS_SLASH_AT(zs, pos) (IS_SLASH(zs[pos]))

static inline bool spl_filesystem_is_dot(const char * d_name) /* {{{ */
{
	return !strcmp(d_name, ".") || !strcmp(d_name, "..");
}
/* }}} */

/* {{{ spl_filesystem_dir_open */
/* open a directory resource
 * Can emit an E_WARNING as it reports errors from php_stream_opendir() */
static void spl_filesystem_dir_open(spl_filesystem_object* intern, zend_string *path)
{
	bool skip_dots = SPL_HAS_FLAG(intern->flags, SPL_FILE_DIR_SKIPDOTS);

	intern->type = SPL_FS_DIR;
	intern->u.dir.dirp = php_stream_opendir(ZSTR_VAL(path), REPORT_ERRORS, FG(default_context));

	if (ZSTR_LEN(path) > 1 && IS_SLASH_AT(ZSTR_VAL(path), ZSTR_LEN(path)-1)) {
		intern->path = zend_string_init(ZSTR_VAL(path), ZSTR_LEN(path)-1, 0);
	} else {
		intern->path = zend_string_copy(path);
	}
	intern->u.dir.index = 0;

	if (EG(exception) || intern->u.dir.dirp == NULL) {
		intern->u.dir.entry.d_name[0] = '\0';
		if (!EG(exception)) {
			/* open failed w/out notice (turned to exception due to EH_THROW) */
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0,
				"Failed to open directory \"%s\"", ZSTR_VAL(path));
		}
	} else {
		do {
			spl_filesystem_dir_read(intern);
		} while (skip_dots && spl_filesystem_is_dot(intern->u.dir.entry.d_name));
	}
}
/* }}} */

/* Can generate E_WARNINGS as we report errors from stream initialized via
 * php_stream_open_wrapper_ex() */
static zend_result spl_filesystem_file_open(spl_filesystem_object *intern, bool use_include_path) /* {{{ */
{
	zval tmp;

	intern->type = SPL_FS_FILE;
	php_stat(intern->file_name, FS_IS_DIR, &tmp);
	if (Z_TYPE(tmp) == IS_TRUE) {
		zend_string_release(intern->u.file.open_mode);
		intern->u.file.open_mode = NULL;
		intern->file_name = NULL;
		zend_throw_exception_ex(spl_ce_LogicException, 0, "Cannot use SplFileObject with directories");
		return FAILURE;
	}

	intern->u.file.context = php_stream_context_from_zval(intern->u.file.zcontext, 0);
	intern->u.file.stream = php_stream_open_wrapper_ex(ZSTR_VAL(intern->file_name), ZSTR_VAL(intern->u.file.open_mode), (use_include_path ? USE_PATH : 0) | REPORT_ERRORS, NULL, intern->u.file.context);

	if (!ZSTR_LEN(intern->file_name) || !intern->u.file.stream) {
		if (!EG(exception)) {
			zend_throw_exception_ex(spl_ce_RuntimeException, 0, "Cannot open file '%s'", ZSTR_VAL(intern->file_name));
		}
		zend_string_release(intern->u.file.open_mode);
		intern->u.file.open_mode = NULL;
		intern->file_name = NULL; /* until here it is not a copy */
		return FAILURE;
	}

	/* prevent closing the stream outside of SplFileObject */
	intern->u.file.stream->flags |= PHP_STREAM_FLAG_NO_FCLOSE;

	/*
	if (intern->u.file.zcontext) {
		//zend_list_addref(Z_RES_VAL(intern->u.file.zcontext));
		Z_ADDREF_P(intern->u.file.zcontext);
	}
	*/

	if (ZSTR_LEN(intern->file_name) > 1 && IS_SLASH_AT(ZSTR_VAL(intern->file_name), ZSTR_LEN(intern->file_name)-1)) {
		intern->file_name = zend_string_init(ZSTR_VAL(intern->file_name), ZSTR_LEN(intern->file_name)-1, 0);
	} else {
		intern->file_name = zend_string_copy(intern->file_name);
	}

	intern->orig_path = zend_string_init(intern->u.file.stream->orig_path, strlen(intern->u.file.stream->orig_path), 0);

	/* avoid reference counting in debug mode, thus do it manually */
	ZVAL_RES(&intern->u.file.zresource, intern->u.file.stream->res);
	/*!!! TODO: maybe bug?
	Z_SET_REFCOUNT(intern->u.file.zresource, 1);
	*/

	intern->u.file.delimiter = ',';
	intern->u.file.enclosure = '"';
	intern->u.file.escape = (unsigned char) '\\';

	intern->u.file.func_getCurr = zend_hash_str_find_ptr(&intern->std.ce->function_table, "getcurrentline", sizeof("getcurrentline") - 1);

	return SUCCESS;
} /* }}} */

/* {{{ spl_filesystem_object_clone */
/* Local zend_object creation (on stack)
   Load the 'other' object
   Create a new empty object (See spl_filesystem_object_new_ex)
   Open the directory
   Clone other members (properties)
 */
static zend_object *spl_filesystem_object_clone(zend_object *old_object)
{
	zend_object *new_object;
	spl_filesystem_object *intern;
	spl_filesystem_object *source;

	source = spl_filesystem_from_obj(old_object);
	new_object = spl_filesystem_object_new_ex(old_object->ce);
	intern = spl_filesystem_from_obj(new_object);

	intern->flags = source->flags;

	switch (source->type) {
		case SPL_FS_INFO:
			if (source->path != NULL) {
				intern->path = zend_string_copy(source->path);
			}
			if (source->file_name != NULL) {
				intern->file_name = zend_string_copy(source->file_name);
			}
			break;
		case SPL_FS_DIR: {
			spl_filesystem_dir_open(intern, source->path);
			/* read until we hit the position in which we were before */
			bool skip_dots = SPL_HAS_FLAG(source->flags, SPL_FILE_DIR_SKIPDOTS);
			int index;
			for (index = 0; index < source->u.dir.index; ++index) {
				do {
					spl_filesystem_dir_read(intern);
				} while (skip_dots && spl_filesystem_is_dot(intern->u.dir.entry.d_name));
			}
			intern->u.dir.index = index;
			break;
		}
		case SPL_FS_FILE:
			ZEND_UNREACHABLE();
	}

	intern->file_class = source->file_class;
	intern->info_class = source->info_class;
	intern->oth = source->oth;
	intern->oth_handler = source->oth_handler;

	zend_objects_clone_members(new_object, old_object);

	if (intern->oth_handler && intern->oth_handler->clone) {
		intern->oth_handler->clone(source, intern);
	}

	return new_object;
}
/* }}} */

static void spl_filesystem_info_set_filename(spl_filesystem_object *intern, zend_string *path) /* {{{ */
{
	size_t path_len;

	if (intern->file_name) {
		zend_string_release(intern->file_name);
	}

	path_len = ZSTR_LEN(path);
	if (path_len > 1 && IS_SLASH_AT(ZSTR_VAL(path), path_len-1)) {
		do {
			path_len--;
		} while (path_len > 1 && IS_SLASH_AT(ZSTR_VAL(path), path_len - 1));
		intern->file_name = zend_string_init(ZSTR_VAL(path), path_len, 0);
	} else {
		intern->file_name = zend_string_copy(path);
	}
	while (path_len > 1 && !IS_SLASH_AT(ZSTR_VAL(path), path_len-1)) {
		path_len--;
	}
	if (path_len) {
		path_len--;
	}

	if (intern->path) {
		zend_string_release(intern->path);
	}
	intern->path = zend_string_init(ZSTR_VAL(path), path_len, 0);
} /* }}} */

static spl_filesystem_object *spl_filesystem_object_create_info(spl_filesystem_object *source, zend_string *file_path, zend_class_entry *ce, zval *return_value) /* {{{ */
{
	spl_filesystem_object *intern;
	zval arg1;

	if (!file_path || !ZSTR_LEN(file_path)) {
#ifdef PHP_WIN32
		zend_throw_exception_ex(spl_ce_RuntimeException, 0, "Cannot create SplFileInfo for empty path");
#endif
		return NULL;
	}

	ce = ce ? ce : source->info_class;

	intern = spl_filesystem_from_obj(spl_filesystem_object_new_ex(ce));
	RETVAL_OBJ(&intern->std);

	if (ce->constructor->common.scope != spl_ce_SplFileInfo) {
		ZVAL_STR_COPY(&arg1, file_path);
		zend_call_method_with_1_params(Z_OBJ_P(return_value), ce, &ce->constructor, "__construct", NULL, &arg1);
		zval_ptr_dtor(&arg1);
	} else {
		spl_filesystem_info_set_filename(intern, file_path);
	}

	return intern;
} /* }}} */

static spl_filesystem_object *spl_filesystem_object_create_type(int num_args, spl_filesystem_object *source, int type, zend_class_entry *ce, zval *return_value) /* {{{ */
{
	spl_filesystem_object *intern;
	bool use_include_path = 0;
	zval arg1, arg2;
	zend_error_handling error_handling;

	switch (source->type) {
		case SPL_FS_INFO:
		case SPL_FS_FILE:
			break;
		case SPL_FS_DIR:
			if (!source->u.dir.entry.d_name[0]) {
				zend_throw_exception_ex(spl_ce_RuntimeException, 0, "Could not open file");
				return NULL;
			}
	}

	switch (type) {
		case SPL_FS_INFO:
			ce = ce ? ce : source->info_class;

			intern = spl_filesystem_from_obj(spl_filesystem_object_new_ex(ce));
			RETVAL_OBJ(&intern->std);

			if (spl_filesystem_object_get_file_name(source) == FAILURE) {
				return NULL;
			}

			if (ce->constructor->common.scope != spl_ce_SplFileInfo) {
				ZVAL_STR_COPY(&arg1, source->file_name);
				zend_call_method_with_1_params(Z_OBJ_P(return_value), ce, &ce->constructor, "__construct", NULL, &arg1);
				zval_ptr_dtor(&arg1);
			} else {
				intern->file_name = zend_string_copy(source->file_name);
				intern->path = spl_filesystem_object_get_path(source);
			}
			break;
		case SPL_FS_FILE:
		{
			ce = ce ? ce : source->file_class;

			zend_string *open_mode = ZSTR_CHAR('r');
			zval *resource = NULL;

			if (zend_parse_parameters(num_args, "|Sbr!",
				&open_mode, &use_include_path, &resource) == FAILURE
			) {
				return NULL;
			}

			intern = spl_filesystem_from_obj(spl_filesystem_object_new_ex(ce));
			RETVAL_OBJ(&intern->std);

			if (spl_filesystem_object_get_file_name(source) == FAILURE) {
				return NULL;
			}

			if (ce->constructor->common.scope != spl_ce_SplFileObject) {
				ZVAL_STR_COPY(&arg1, source->file_name);
				ZVAL_STR_COPY(&arg2, open_mode);
				zend_call_method_with_2_params(Z_OBJ_P(return_value), ce, &ce->constructor, "__construct", NULL, &arg1, &arg2);
				zval_ptr_dtor(&arg1);
				zval_ptr_dtor(&arg2);
			} else {
				intern->file_name = source->file_name;
				intern->path = spl_filesystem_object_get_path(source);
				intern->u.file.open_mode = zend_string_copy(open_mode);
				intern->u.file.zcontext = resource;

				/* spl_filesystem_file_open() can generate E_WARNINGs which we want to promote to exceptions */
				zend_replace_error_handling(EH_THROW, spl_ce_RuntimeException, &error_handling);
				if (spl_filesystem_file_open(intern, use_include_path) == FAILURE) {
					zend_restore_error_handling(&error_handling);
					zval_ptr_dtor(return_value);
					ZVAL_NULL(return_value);
					return NULL;
				}
				zend_restore_error_handling(&error_handling);
			}
			break;
		}
		case SPL_FS_DIR:
			zend_throw_exception_ex(spl_ce_RuntimeException, 0, "Operation not supported");
			return NULL;
	}
	return NULL;
} /* }}} */

static bool spl_filesystem_is_invalid_or_dot(const char * d_name) /* {{{ */
{
	return d_name[0] == '\0' || spl_filesystem_is_dot(d_name);
}
/* }}} */

static zend_string *spl_filesystem_object_get_pathname(spl_filesystem_object *intern) { /* {{{ */
	switch (intern->type) {
		case SPL_FS_INFO:
		case SPL_FS_FILE:
			return intern->file_name;
		case SPL_FS_DIR:
			if (intern->u.dir.entry.d_name[0]) {
				spl_filesystem_object_get_file_name(intern);
				return intern->file_name;
			}
	}
	return NULL;
}
/* }}} */

static inline HashTable *spl_filesystem_object_get_debug_info(zend_object *object) /* {{{ */
{
	spl_filesystem_object *intern = spl_filesystem_from_obj(object);
	zval tmp;
	HashTable *rv;
	zend_string *pnstr;
	zend_string *path;
	char stmp[2];

	if (!intern->std.properties) {
		rebuild_object_properties(&intern->std);
	}

	rv = zend_array_dup(intern->std.properties);

	pnstr = spl_gen_private_prop_name(spl_ce_SplFileInfo, "pathName", sizeof("pathName")-1);
	path = spl_filesystem_object_get_pathname(intern);
	if (path) {
		ZVAL_STR_COPY(&tmp, path);
	} else {
		ZVAL_EMPTY_STRING(&tmp);
	}
	zend_symtable_update(rv, pnstr, &tmp);
	zend_string_release_ex(pnstr, 0);

	if (intern->file_name) {
		zend_string *path;

		pnstr = spl_gen_private_prop_name(spl_ce_SplFileInfo, "fileName", sizeof("fileName")-1);
		path = spl_filesystem_object_get_path(intern);

		if (path && ZSTR_LEN(path) && ZSTR_LEN(path) < ZSTR_LEN(intern->file_name)) {
			/* +1 to skip the trailing / of the path in the file name */
			ZVAL_STRINGL(&tmp, ZSTR_VAL(intern->file_name) + ZSTR_LEN(path) + 1, ZSTR_LEN(intern->file_name) - (ZSTR_LEN(path) + 1));
		} else {
			ZVAL_STR_COPY(&tmp, intern->file_name);
		}
		zend_symtable_update(rv, pnstr, &tmp);
		zend_string_release_ex(pnstr, /* persistent */ false);
		if (path) {
			zend_string_release_ex(path, /* persistent */ false);
		}
	}
	if (intern->type == SPL_FS_DIR) {
#ifdef HAVE_GLOB
		pnstr = spl_gen_private_prop_name(spl_ce_DirectoryIterator, "glob", sizeof("glob")-1);
		if (php_stream_is(intern->u.dir.dirp ,&php_glob_stream_ops)) {
			ZVAL_STR_COPY(&tmp, intern->path);
		} else {
			ZVAL_FALSE(&tmp);
		}
		zend_symtable_update(rv, pnstr, &tmp);
		zend_string_release_ex(pnstr, 0);
#endif
		pnstr = spl_gen_private_prop_name(spl_ce_RecursiveDirectoryIterator, "subPathName", sizeof("subPathName")-1);
		if (intern->u.dir.sub_path) {
			ZVAL_STR_COPY(&tmp, intern->u.dir.sub_path);
		} else {
			ZVAL_EMPTY_STRING(&tmp);
		}
		zend_symtable_update(rv, pnstr, &tmp);
		zend_string_release_ex(pnstr, 0);
	}
	if (intern->type == SPL_FS_FILE) {
		pnstr = spl_gen_private_prop_name(spl_ce_SplFileObject, "openMode", sizeof("openMode")-1);
		ZVAL_STR_COPY(&tmp, intern->u.file.open_mode);
		zend_symtable_update(rv, pnstr, &tmp);
		zend_string_release_ex(pnstr, 0);
		stmp[1] = '\0';
		stmp[0] = intern->u.file.delimiter;
		pnstr = spl_gen_private_prop_name(spl_ce_SplFileObject, "delimiter", sizeof("delimiter")-1);
		ZVAL_STRINGL(&tmp, stmp, 1);
		zend_symtable_update(rv, pnstr, &tmp);
		zend_string_release_ex(pnstr, 0);
		stmp[0] = intern->u.file.enclosure;
		pnstr = spl_gen_private_prop_name(spl_ce_SplFileObject, "enclosure", sizeof("enclosure")-1);
		ZVAL_STRINGL(&tmp, stmp, 1);
		zend_symtable_update(rv, pnstr, &tmp);
		zend_string_release_ex(pnstr, 0);
	}

	return rv;
}
/* }}} */

zend_function *spl_filesystem_object_get_method_check(zend_object **object, zend_string *method, const zval *key) /* {{{ */
{
	spl_filesystem_object *fsobj = spl_filesystem_from_obj(*object);

	if (fsobj->u.dir.dirp == NULL && fsobj->orig_path == NULL) {
		zend_throw_error(NULL, "The parent constructor was not called: the object is in an invalid state");
		return NULL;
	}

	return zend_std_get_method(object, method, key);
}
/* }}} */

#define DIT_CTOR_FLAGS  0x00000001
#define DIT_CTOR_GLOB   0x00000002

void spl_filesystem_object_construct(INTERNAL_FUNCTION_PARAMETERS, zend_long ctor_flags) /* {{{ */
{
	spl_filesystem_object *intern;
	zend_string *path;
	zend_result parsed;
	zend_long flags = (ctor_flags & ~DIT_CTOR_FLAGS);
	zend_error_handling error_handling;

	if (SPL_HAS_FLAG(ctor_flags, DIT_CTOR_FLAGS)) {
		flags |= SPL_FILE_DIR_KEY_AS_PATHNAME|SPL_FILE_DIR_CURRENT_AS_FILEINFO;
		parsed = zend_parse_parameters(ZEND_NUM_ARGS(), "P|l", &path, &flags);
	} else {
		flags |= SPL_FILE_DIR_KEY_AS_PATHNAME|SPL_FILE_DIR_CURRENT_AS_SELF;
		parsed = zend_parse_parameters(ZEND_NUM_ARGS(), "P", &path);
	}
	if (parsed == FAILURE) {
		RETURN_THROWS();
	}

	if (ZSTR_LEN(path) == 0) {
		zend_argument_value_error(1, "cannot be empty");
		RETURN_THROWS();
	}

	intern = spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS));
	if (intern->path) {
		/* object is already initialized */
		zend_throw_error(NULL, "Directory object is already initialized");
		RETURN_THROWS();
	}
	intern->flags = flags;

	/* spl_filesystem_dir_open() may emit an E_WARNING */
	zend_replace_error_handling(EH_THROW, spl_ce_UnexpectedValueException, &error_handling);
#ifdef HAVE_GLOB
	if (SPL_HAS_FLAG(ctor_flags, DIT_CTOR_GLOB) && !zend_string_starts_with_literal(path, "glob://")) {
		path = zend_strpprintf(0, "glob://%s", ZSTR_VAL(path));
		spl_filesystem_dir_open(intern, path);
		zend_string_release(path);
	} else
#endif
	{
		spl_filesystem_dir_open(intern, path);

	}
	zend_restore_error_handling(&error_handling);

	intern->u.dir.is_recursive = instanceof_function(intern->std.ce, spl_ce_RecursiveDirectoryIterator) ? 1 : 0;
}
/* }}} */

/* {{{ Cronstructs a new dir iterator from a path. */
PHP_METHOD(DirectoryIterator, __construct)
{
	spl_filesystem_object_construct(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ Rewind dir back to the start */
PHP_METHOD(DirectoryIterator, rewind)
{
	spl_filesystem_object *intern = spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS));

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	CHECK_DIRECTORY_ITERATOR_IS_INITIALIZED(intern);
	intern->u.dir.index = 0;
	php_stream_rewinddir(intern->u.dir.dirp);
	spl_filesystem_dir_read(intern);
}
/* }}} */

/* {{{ Return current dir entry */
PHP_METHOD(DirectoryIterator, key)
{
	spl_filesystem_object *intern = spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS));

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	CHECK_DIRECTORY_ITERATOR_IS_INITIALIZED(intern);
	RETURN_LONG(intern->u.dir.index);
}
/* }}} */

/* {{{ Return this (needed for Iterator interface) */
PHP_METHOD(DirectoryIterator, current)
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	CHECK_DIRECTORY_ITERATOR_IS_INITIALIZED(spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS)));
	RETURN_OBJ_COPY(Z_OBJ_P(ZEND_THIS));
}
/* }}} */

/* {{{ Move to next entry */
PHP_METHOD(DirectoryIterator, next)
{
	spl_filesystem_object *intern = spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS));
	bool skip_dots = SPL_HAS_FLAG(intern->flags, SPL_FILE_DIR_SKIPDOTS);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	CHECK_DIRECTORY_ITERATOR_IS_INITIALIZED(intern);
	intern->u.dir.index++;
	do {
		spl_filesystem_dir_read(intern);
	} while (skip_dots && spl_filesystem_is_dot(intern->u.dir.entry.d_name));
	if (intern->file_name) {
		zend_string_release(intern->file_name);
		intern->file_name = NULL;
	}
}
/* }}} */

/* {{{ Seek to the given position */
PHP_METHOD(DirectoryIterator, seek)
{
	spl_filesystem_object *intern    = spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS));
	zval retval;
	zend_long pos;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &pos) == FAILURE) {
		RETURN_THROWS();
	}

	CHECK_DIRECTORY_ITERATOR_IS_INITIALIZED(intern);
	if (intern->u.dir.index > pos) {
		/* we first rewind */
		zend_call_method_with_0_params(Z_OBJ_P(ZEND_THIS), Z_OBJCE_P(ZEND_THIS), &intern->u.dir.func_rewind, "rewind", NULL);
	}

	while (intern->u.dir.index < pos) {
		bool valid = 0;
		zend_call_method_with_0_params(Z_OBJ_P(ZEND_THIS), Z_OBJCE_P(ZEND_THIS), &intern->u.dir.func_valid, "valid", &retval);
		valid = zend_is_true(&retval);
		zval_ptr_dtor(&retval);
		if (!valid) {
			zend_throw_exception_ex(spl_ce_OutOfBoundsException, 0, "Seek position " ZEND_LONG_FMT " is out of range", pos);
			RETURN_THROWS();
		}
		zend_call_method_with_0_params(Z_OBJ_P(ZEND_THIS), Z_OBJCE_P(ZEND_THIS), &intern->u.dir.func_next, "next", NULL);
	}
} /* }}} */

/* {{{ Check whether dir contains more entries */
PHP_METHOD(DirectoryIterator, valid)
{
	spl_filesystem_object *intern = spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS));

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	CHECK_DIRECTORY_ITERATOR_IS_INITIALIZED(intern);
	RETURN_BOOL(intern->u.dir.entry.d_name[0] != '\0');
}
/* }}} */

/* {{{ Return the path */
PHP_METHOD(SplFileInfo, getPath)
{
	spl_filesystem_object *intern = spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS));
	zend_string *path;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

  	path = spl_filesystem_object_get_path(intern);
	if (path) {
		RETURN_STR(path);
	} else {
		RETURN_EMPTY_STRING();
	}
}
/* }}} */

/* {{{ Return filename only */
PHP_METHOD(SplFileInfo, getFilename)
{
	spl_filesystem_object *intern = spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS));
	zend_string *path;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	if (!intern->file_name) {
		zend_throw_error(NULL, "Object not initialized");
		RETURN_THROWS();
	}

	path = spl_filesystem_object_get_path(intern);

	if (path && ZSTR_LEN(path) && ZSTR_LEN(path) < ZSTR_LEN(intern->file_name)) {
		/* +1 to skip the trailing / of the path in the file name */
		size_t path_len = ZSTR_LEN(path) + 1;
		RETVAL_STRINGL(ZSTR_VAL(intern->file_name) + path_len, ZSTR_LEN(intern->file_name) - path_len);
	} else {
		RETVAL_STR_COPY(intern->file_name);
	}
	if (path) {
		zend_string_release_ex(path, /* persistent */ false);
	}
}
/* }}} */

/* {{{ Return filename of current dir entry */
PHP_METHOD(DirectoryIterator, getFilename)
{
	spl_filesystem_object *intern = spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS));

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	CHECK_DIRECTORY_ITERATOR_IS_INITIALIZED(intern);
	RETURN_STRING(intern->u.dir.entry.d_name);
}
/* }}} */

/* {{{ Returns file extension component of path */
PHP_METHOD(SplFileInfo, getExtension)
{
	spl_filesystem_object *intern = spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS));
	char *fname = NULL;
	const char *p;
	size_t flen;
	zend_string *path;
	size_t idx;
	zend_string *ret;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	if (!intern->file_name) {
		zend_throw_error(NULL, "Object not initialized");
		RETURN_THROWS();
	}

	path = spl_filesystem_object_get_path(intern);

	if (path && ZSTR_LEN(path) && ZSTR_LEN(path) < ZSTR_LEN(intern->file_name)) {
		fname = ZSTR_VAL(intern->file_name) + ZSTR_LEN(path) + 1;
		flen = ZSTR_LEN(intern->file_name) - (ZSTR_LEN(path) + 1);
	} else {
		fname = ZSTR_VAL(intern->file_name);
		flen = ZSTR_LEN(intern->file_name);
	}
	if (path) {
		zend_string_release_ex(path, /* persistent */ false);
	}

	ret = php_basename(fname, flen, NULL, 0);

	p = zend_memrchr(ZSTR_VAL(ret), '.', ZSTR_LEN(ret));
	if (p) {
		idx = p - ZSTR_VAL(ret);
		RETVAL_STRINGL(ZSTR_VAL(ret) + idx + 1, ZSTR_LEN(ret) - idx - 1);
		zend_string_release_ex(ret, 0);
		return;
	} else {
		zend_string_release_ex(ret, 0);
		RETURN_EMPTY_STRING();
	}
}
/* }}}*/

/* {{{ Returns the file extension component of path */
PHP_METHOD(DirectoryIterator, getExtension)
{
	spl_filesystem_object *intern = spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS));
	const char *p;
	size_t idx;
	zend_string *fname;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	CHECK_DIRECTORY_ITERATOR_IS_INITIALIZED(intern);
	fname = php_basename(intern->u.dir.entry.d_name, strlen(intern->u.dir.entry.d_name), NULL, 0);

	p = zend_memrchr(ZSTR_VAL(fname), '.', ZSTR_LEN(fname));
	if (p) {
		idx = p - ZSTR_VAL(fname);
		RETVAL_STRINGL(ZSTR_VAL(fname) + idx + 1, ZSTR_LEN(fname) - idx - 1);
		zend_string_release_ex(fname, 0);
	} else {
		zend_string_release_ex(fname, 0);
		RETURN_EMPTY_STRING();
	}
}
/* }}} */

/* {{{ Returns filename component of path */
PHP_METHOD(SplFileInfo, getBasename)
{
	spl_filesystem_object *intern = spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS));
	char *fname, *suffix = 0;
	size_t flen;
	size_t slen = 0;
	zend_string *path;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|s", &suffix, &slen) == FAILURE) {
		RETURN_THROWS();
	}

	if (!intern->file_name) {
		zend_throw_error(NULL, "Object not initialized");
		RETURN_THROWS();
	}

	path = spl_filesystem_object_get_path(intern);

	if (path && ZSTR_LEN(path) && ZSTR_LEN(path) < ZSTR_LEN(intern->file_name)) {
		/* +1 to skip the trailing / of the path in the file name */
		fname = ZSTR_VAL(intern->file_name) + ZSTR_LEN(path) + 1;
		flen = ZSTR_LEN(intern->file_name) - (ZSTR_LEN(path) + 1);
	} else {
		fname = ZSTR_VAL(intern->file_name);
		flen = ZSTR_LEN(intern->file_name);
	}
	if (path) {
		zend_string_release_ex(path, /* persistent */ false);
	}

	RETURN_STR(php_basename(fname, flen, suffix, slen));
}
/* }}}*/

/* {{{ Returns filename component of current dir entry */
PHP_METHOD(DirectoryIterator, getBasename)
{
	spl_filesystem_object *intern = spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS));
	char *suffix = 0;
	size_t slen = 0;
	zend_string *fname;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|s", &suffix, &slen) == FAILURE) {
		RETURN_THROWS();
	}

	CHECK_DIRECTORY_ITERATOR_IS_INITIALIZED(intern);
	fname = php_basename(intern->u.dir.entry.d_name, strlen(intern->u.dir.entry.d_name), suffix, slen);

	RETURN_STR(fname);
}
/* }}} */

/* {{{ Return path and filename */
PHP_METHOD(SplFileInfo, getPathname)
{
	spl_filesystem_object *intern = spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS));
	zend_string *path;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	path = spl_filesystem_object_get_pathname(intern);
	if (path) {
		RETURN_STR_COPY(path);
	} else {
		RETURN_EMPTY_STRING();
	}
}
/* }}} */

/* {{{ Return getPathname() or getFilename() depending on flags */
PHP_METHOD(FilesystemIterator, key)
{
	spl_filesystem_object *intern = spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS));

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	if (SPL_FILE_DIR_KEY(intern, SPL_FILE_DIR_KEY_AS_FILENAME)) {
		RETURN_STRING(intern->u.dir.entry.d_name);
	} else {
		if (spl_filesystem_object_get_file_name(intern) == FAILURE) {
			RETURN_THROWS();
		}
		RETURN_STR_COPY(intern->file_name);
	}
}
/* }}} */

/* {{{ Return getFilename(), getFileInfo() or $this depending on flags */
PHP_METHOD(FilesystemIterator, current)
{
	spl_filesystem_object *intern = spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS));

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	if (SPL_FILE_DIR_CURRENT(intern, SPL_FILE_DIR_CURRENT_AS_PATHNAME)) {
		if (spl_filesystem_object_get_file_name(intern) == FAILURE) {
			RETURN_THROWS();
		}
		RETURN_STR_COPY(intern->file_name);
	} else if (SPL_FILE_DIR_CURRENT(intern, SPL_FILE_DIR_CURRENT_AS_FILEINFO)) {
		if (spl_filesystem_object_get_file_name(intern) == FAILURE) {
			RETURN_THROWS();
		}
		spl_filesystem_object_create_type(0, intern, SPL_FS_INFO, NULL, return_value);
	} else {
		RETURN_OBJ_COPY(Z_OBJ_P(ZEND_THIS));
	}
}
/* }}} */

/* {{{ Returns true if current entry is '.' or  '..' */
PHP_METHOD(DirectoryIterator, isDot)
{
	spl_filesystem_object *intern = spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS));

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	CHECK_DIRECTORY_ITERATOR_IS_INITIALIZED(intern);
	RETURN_BOOL(spl_filesystem_is_dot(intern->u.dir.entry.d_name));
}
/* }}} */

/* {{{ Constructs a new SplFileInfo from a path. */
/* When the constructor gets called the object is already created
   by the engine, so we must only call 'additional' initializations.
 */
PHP_METHOD(SplFileInfo, __construct)
{
	spl_filesystem_object *intern;
	zend_string *path;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "P", &path) == FAILURE) {
		RETURN_THROWS();
	}

	intern = spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS));

	spl_filesystem_info_set_filename(intern, path);

	/* intern->type = SPL_FS_INFO; already set */
}
/* }}} */

/* {{{ FileInfoFunction */
#define FileInfoFunction(func_name, func_num) \
PHP_METHOD(SplFileInfo, func_name) \
{ \
	spl_filesystem_object *intern = spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS)); \
	zend_error_handling error_handling; \
	if (zend_parse_parameters_none() == FAILURE) { \
		RETURN_THROWS(); \
	} \
	if (spl_filesystem_object_get_file_name(intern) == FAILURE) { \
		RETURN_THROWS(); \
	} \
	zend_replace_error_handling(EH_THROW, spl_ce_RuntimeException, &error_handling);\
	php_stat(intern->file_name, func_num, return_value); \
	zend_restore_error_handling(&error_handling); \
}
/* }}} */

/* {{{ Get file permissions */
FileInfoFunction(getPerms, FS_PERMS)
/* }}} */

/* {{{ Get file inode */
FileInfoFunction(getInode, FS_INODE)
/* }}} */

/* {{{ Get file size */
FileInfoFunction(getSize, FS_SIZE)
/* }}} */

/* {{{ Get file owner */
FileInfoFunction(getOwner, FS_OWNER)
/* }}} */

/* {{{ Get file group */
FileInfoFunction(getGroup, FS_GROUP)
/* }}} */

/* {{{ Get last access time of file */
FileInfoFunction(getATime, FS_ATIME)
/* }}} */

/* {{{ Get last modification time of file */
FileInfoFunction(getMTime, FS_MTIME)
/* }}} */

/* {{{ Get inode modification time of file */
FileInfoFunction(getCTime, FS_CTIME)
/* }}} */

/* {{{ Get file type */
FileInfoFunction(getType, FS_TYPE)
/* }}} */

/* {{{ Returns true if file can be written */
FileInfoFunction(isWritable, FS_IS_W)
/* }}} */

/* {{{ Returns true if file can be read */
FileInfoFunction(isReadable, FS_IS_R)
/* }}} */

/* {{{ Returns true if file is executable */
FileInfoFunction(isExecutable, FS_IS_X)
/* }}} */

/* {{{ Returns true if file is a regular file */
FileInfoFunction(isFile, FS_IS_FILE)
/* }}} */

/* {{{ Returns true if file is directory */
FileInfoFunction(isDir, FS_IS_DIR)
/* }}} */

/* {{{ Returns true if file is symbolic link */
FileInfoFunction(isLink, FS_IS_LINK)
/* }}} */

/* {{{ Return the target of a symbolic link */
PHP_METHOD(SplFileInfo, getLinkTarget)
{
	spl_filesystem_object *intern = spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS));
	ssize_t ret;
	char buff[MAXPATHLEN];

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	if (intern->file_name == NULL) {
		if (spl_filesystem_object_get_file_name(intern) == FAILURE) {
			RETURN_THROWS();
		}
	}
#if defined(PHP_WIN32) || defined(HAVE_SYMLINK)
	if (intern->file_name == NULL) {
		zend_value_error("Filename cannot be empty");
		RETURN_THROWS();
	}
	if (!IS_ABSOLUTE_PATH(ZSTR_VAL(intern->file_name), ZSTR_LEN(intern->file_name))) {
		char expanded_path[MAXPATHLEN];
		if (!expand_filepath_with_mode(ZSTR_VAL(intern->file_name), expanded_path, NULL, 0, CWD_EXPAND )) {
			php_error_docref(NULL, E_WARNING, "No such file or directory");
			RETURN_FALSE;
		}
		ret = php_sys_readlink(expanded_path, buff, MAXPATHLEN - 1);
	} else {
		ret = php_sys_readlink(ZSTR_VAL(intern->file_name), buff,  MAXPATHLEN-1);
	}
#else
	ret = -1; /* always fail if not implemented */
#endif

	if (ret == -1) {
		zend_throw_exception_ex(spl_ce_RuntimeException, 0, "Unable to read link %s, error: %s", ZSTR_VAL(intern->file_name), strerror(errno));
		RETVAL_FALSE;
	} else {
		/* Append NULL to the end of the string */
		buff[ret] = '\0';

		RETVAL_STRINGL(buff, ret);
	}
}
/* }}} */

/* {{{ Return the resolved path */
PHP_METHOD(SplFileInfo, getRealPath)
{
	spl_filesystem_object *intern = spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS));
	char buff[MAXPATHLEN];
	char *filename;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	if (intern->type == SPL_FS_DIR && !intern->file_name && intern->u.dir.entry.d_name[0]) {
		if (spl_filesystem_object_get_file_name(intern) == FAILURE) {
			RETURN_THROWS();
		}
	}

	if (intern->orig_path) {
		filename = ZSTR_VAL(intern->orig_path);
	} else {
		filename = intern->file_name ? ZSTR_VAL(intern->file_name) : NULL;
	}


	if (filename && VCWD_REALPATH(filename, buff)) {
#ifdef ZTS
		if (VCWD_ACCESS(buff, F_OK)) {
			RETURN_FALSE;
		} else
#endif
		RETURN_STRING(buff);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ Open the current file */
PHP_METHOD(SplFileInfo, openFile)
{
	spl_filesystem_object *intern = spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS));

	spl_filesystem_object_create_type(ZEND_NUM_ARGS(), intern, SPL_FS_FILE, NULL, return_value);
}
/* }}} */

/* {{{ Class to use in openFile() */
PHP_METHOD(SplFileInfo, setFileClass)
{
	spl_filesystem_object *intern = spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS));
	zend_class_entry *ce = spl_ce_SplFileObject;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|C", &ce) == FAILURE) {
		RETURN_THROWS();
	}

	intern->file_class = ce;
}
/* }}} */

/* {{{ Class to use in getFileInfo(), getPathInfo() */
PHP_METHOD(SplFileInfo, setInfoClass)
{
	spl_filesystem_object *intern = spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS));
	zend_class_entry *ce = spl_ce_SplFileInfo;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|C", &ce) == FAILURE) {
		RETURN_THROWS();
	}

	intern->info_class = ce;
}
/* }}} */

/* {{{ Get/copy file info */
PHP_METHOD(SplFileInfo, getFileInfo)
{
	spl_filesystem_object *intern = spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS));
	zend_class_entry *ce = intern->info_class;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|C!", &ce) == FAILURE) {
		RETURN_THROWS();
	}

	spl_filesystem_object_create_type(ZEND_NUM_ARGS(), intern, SPL_FS_INFO, ce, return_value);
}
/* }}} */

/* {{{ Get/copy file info */
PHP_METHOD(SplFileInfo, getPathInfo)
{
	spl_filesystem_object *intern = spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS));
	zend_class_entry *ce = intern->info_class;
	zend_string *path;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|C!", &ce) == FAILURE) {
		RETURN_THROWS();
	}

	path = spl_filesystem_object_get_pathname(intern);
	if (path && ZSTR_LEN(path)) {
		zend_string *dpath = zend_string_init(ZSTR_VAL(path), ZSTR_LEN(path), 0);
		ZSTR_LEN(dpath) = php_dirname(ZSTR_VAL(dpath), ZSTR_LEN(path));
		spl_filesystem_object_create_info(intern, dpath, ce, return_value);
		zend_string_release(dpath);
	}
}
/* }}} */

/* {{{ */
PHP_METHOD(SplFileInfo, __debugInfo)
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_ARR(spl_filesystem_object_get_debug_info(Z_OBJ_P(ZEND_THIS)));
} /* }}} */

/* {{{ */
PHP_METHOD(SplFileInfo, _bad_state_ex)
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	zend_throw_error(NULL, "The parent constructor was not called: the object is in an invalid state");
	RETURN_THROWS();
}
/* }}} */

/* {{{ Constructs a new dir iterator from a path. */
PHP_METHOD(FilesystemIterator, __construct)
{
	spl_filesystem_object_construct(INTERNAL_FUNCTION_PARAM_PASSTHRU, DIT_CTOR_FLAGS | SPL_FILE_DIR_SKIPDOTS);
}
/* }}} */

/* {{{ Rewind dir back to the start */
PHP_METHOD(FilesystemIterator, rewind)
{
	spl_filesystem_object *intern = spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS));
	bool skip_dots = SPL_HAS_FLAG(intern->flags, SPL_FILE_DIR_SKIPDOTS);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	intern->u.dir.index = 0;
	if (intern->u.dir.dirp) {
		php_stream_rewinddir(intern->u.dir.dirp);
	}
	do {
		spl_filesystem_dir_read(intern);
	} while (skip_dots && spl_filesystem_is_dot(intern->u.dir.entry.d_name));
}
/* }}} */

/* {{{ Get handling flags */
PHP_METHOD(FilesystemIterator, getFlags)
{
	spl_filesystem_object *intern = spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS));

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_LONG(intern->flags & (SPL_FILE_DIR_KEY_MODE_MASK | SPL_FILE_DIR_CURRENT_MODE_MASK | SPL_FILE_DIR_OTHERS_MASK));
} /* }}} */

/* {{{ Set handling flags */
PHP_METHOD(FilesystemIterator, setFlags)
{
	spl_filesystem_object *intern = spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS));
	zend_long flags;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &flags) == FAILURE) {
		RETURN_THROWS();
	}

	intern->flags &= ~(SPL_FILE_DIR_KEY_MODE_MASK|SPL_FILE_DIR_CURRENT_MODE_MASK|SPL_FILE_DIR_OTHERS_MASK);
	intern->flags |= ((SPL_FILE_DIR_KEY_MODE_MASK|SPL_FILE_DIR_CURRENT_MODE_MASK|SPL_FILE_DIR_OTHERS_MASK) & flags);
} /* }}} */

/* {{{ Returns whether current entry is a directory and not '.' or '..' */
PHP_METHOD(RecursiveDirectoryIterator, hasChildren)
{
	bool allow_links = 0;
	spl_filesystem_object *intern = spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS));

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(allow_links)
	ZEND_PARSE_PARAMETERS_END();

	if (spl_filesystem_is_invalid_or_dot(intern->u.dir.entry.d_name)) {
		RETURN_FALSE;
	} else {
		if (spl_filesystem_object_get_file_name(intern) == FAILURE) {
			RETURN_THROWS();
		}
		php_stat(intern->file_name, FS_LPERMS, return_value);
		if (Z_TYPE_P(return_value) == IS_FALSE) {
			return;
		} else if (!S_ISLNK(Z_LVAL_P(return_value))) {
			RETURN_BOOL(S_ISDIR(Z_LVAL_P(return_value)));
		} else {
			if (!allow_links
			 && !(intern->flags & SPL_FILE_DIR_FOLLOW_SYMLINKS)) {
				RETURN_FALSE;
			}
			php_stat(intern->file_name, FS_IS_DIR, return_value);
		}
    }
}
/* }}} */

/* {{{ Returns an iterator for the current entry if it is a directory */
PHP_METHOD(RecursiveDirectoryIterator, getChildren)
{
	zval zpath, zflags;
	spl_filesystem_object *intern = spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS));
	spl_filesystem_object *subdir;
	char slash = SPL_HAS_FLAG(intern->flags, SPL_FILE_DIR_UNIXPATHS) ? '/' : DEFAULT_SLASH;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	if (spl_filesystem_object_get_file_name(intern) == FAILURE) {
		RETURN_THROWS();
	}

	ZVAL_LONG(&zflags, intern->flags);
	ZVAL_STR_COPY(&zpath, intern->file_name);
	spl_instantiate_arg_ex2(Z_OBJCE_P(ZEND_THIS), return_value, &zpath, &zflags);
	zval_ptr_dtor(&zpath);

	subdir = spl_filesystem_from_obj(Z_OBJ_P(return_value));
	if (subdir) {
		size_t name_len = strlen(intern->u.dir.entry.d_name);
		if (intern->u.dir.sub_path && ZSTR_LEN(intern->u.dir.sub_path)) {
			zend_string *sub_path = zend_string_alloc(ZSTR_LEN(intern->u.dir.sub_path) + 1 + name_len, 0);
			memcpy(ZSTR_VAL(sub_path), ZSTR_VAL(intern->u.dir.sub_path), ZSTR_LEN(intern->u.dir.sub_path));
			ZSTR_VAL(sub_path)[ZSTR_LEN(intern->u.dir.sub_path)] = slash;
			memcpy(ZSTR_VAL(sub_path) + ZSTR_LEN(intern->u.dir.sub_path) + 1, intern->u.dir.entry.d_name, name_len);
			ZSTR_VAL(sub_path)[ZSTR_LEN(intern->u.dir.sub_path) + 1 + name_len] = 0;
			subdir->u.dir.sub_path = sub_path;
		} else {
			subdir->u.dir.sub_path = zend_string_init(intern->u.dir.entry.d_name, name_len, 0);
		}
		subdir->info_class = intern->info_class;
		subdir->file_class = intern->file_class;
		subdir->oth = intern->oth;
	}
}
/* }}} */

/* {{{ Get sub path */
PHP_METHOD(RecursiveDirectoryIterator, getSubPath)
{
	spl_filesystem_object *intern = spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS));

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	if (intern->u.dir.sub_path) {
		RETURN_STR_COPY(intern->u.dir.sub_path);
	} else {
		RETURN_EMPTY_STRING();
	}
}
/* }}} */

/* {{{ Get sub path and file name */
PHP_METHOD(RecursiveDirectoryIterator, getSubPathname)
{
	spl_filesystem_object *intern = spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS));
	char slash = SPL_HAS_FLAG(intern->flags, SPL_FILE_DIR_UNIXPATHS) ? '/' : DEFAULT_SLASH;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	if (intern->u.dir.sub_path) {
		RETURN_NEW_STR(strpprintf(0, "%s%c%s", ZSTR_VAL(intern->u.dir.sub_path), slash, intern->u.dir.entry.d_name));
	} else {
		RETURN_STRING(intern->u.dir.entry.d_name);
	}
}
/* }}} */

/* {{{ Cronstructs a new dir iterator from a path. */
PHP_METHOD(RecursiveDirectoryIterator, __construct)
{
	spl_filesystem_object_construct(INTERNAL_FUNCTION_PARAM_PASSTHRU, DIT_CTOR_FLAGS);
}
/* }}} */

#ifdef HAVE_GLOB
/* {{{ Cronstructs a new dir iterator from a glob expression (no glob:// needed). */
PHP_METHOD(GlobIterator, __construct)
{
	spl_filesystem_object_construct(INTERNAL_FUNCTION_PARAM_PASSTHRU, DIT_CTOR_FLAGS|DIT_CTOR_GLOB);
}
/* }}} */

/* {{{ Return the number of directories and files found by globbing */
PHP_METHOD(GlobIterator, count)
{
	spl_filesystem_object *intern = spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS));

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	if (intern->u.dir.dirp && php_stream_is(intern->u.dir.dirp ,&php_glob_stream_ops)) {
		RETURN_LONG(php_glob_stream_get_count(intern->u.dir.dirp, NULL));
	} else {
		/* should not happen */
		// TODO ZEND_ASSERT ?
		php_error_docref(NULL, E_ERROR, "GlobIterator lost glob state");
	}
}
/* }}} */
#endif /* HAVE_GLOB */

/* {{{ forward declarations to the iterator handlers */
static void spl_filesystem_dir_it_dtor(zend_object_iterator *iter);
static int spl_filesystem_dir_it_valid(zend_object_iterator *iter);
static zval *spl_filesystem_dir_it_current_data(zend_object_iterator *iter);
static void spl_filesystem_dir_it_current_key(zend_object_iterator *iter, zval *key);
static void spl_filesystem_dir_it_move_forward(zend_object_iterator *iter);
static void spl_filesystem_dir_it_rewind(zend_object_iterator *iter);

/* iterator handler table */
static const zend_object_iterator_funcs spl_filesystem_dir_it_funcs = {
	spl_filesystem_dir_it_dtor,
	spl_filesystem_dir_it_valid,
	spl_filesystem_dir_it_current_data,
	spl_filesystem_dir_it_current_key,
	spl_filesystem_dir_it_move_forward,
	spl_filesystem_dir_it_rewind,
	NULL,
	NULL, /* get_gc */
};
/* }}} */

/* {{{ spl_ce_dir_get_iterator */
zend_object_iterator *spl_filesystem_dir_get_iterator(zend_class_entry *ce, zval *object, int by_ref)
{
	spl_filesystem_iterator *iterator;
	spl_filesystem_object *dir_object;

	if (by_ref) {
		zend_throw_error(NULL, "An iterator cannot be used with foreach by reference");
		return NULL;
	}
	dir_object = spl_filesystem_from_obj(Z_OBJ_P(object));
	iterator = spl_filesystem_object_to_iterator(dir_object);
	ZVAL_OBJ_COPY(&iterator->intern.data, Z_OBJ_P(object));
	iterator->intern.funcs = &spl_filesystem_dir_it_funcs;
	/* ->current must be initialized; rewind doesn't set it and valid
	 * doesn't check whether it's set */
	iterator->current = *object;

	return &iterator->intern;
}
/* }}} */

/* {{{ spl_filesystem_dir_it_dtor */
static void spl_filesystem_dir_it_dtor(zend_object_iterator *iter)
{
	spl_filesystem_iterator *iterator = (spl_filesystem_iterator *)iter;
	zval_ptr_dtor(&iterator->intern.data);
}
/* }}} */

/* {{{ spl_filesystem_dir_it_valid */
static int spl_filesystem_dir_it_valid(zend_object_iterator *iter)
{
	spl_filesystem_object *object = spl_filesystem_iterator_to_object((spl_filesystem_iterator *)iter);

	return object->u.dir.entry.d_name[0] != '\0' ? SUCCESS : FAILURE;
}
/* }}} */

/* {{{ spl_filesystem_dir_it_current_data */
static zval *spl_filesystem_dir_it_current_data(zend_object_iterator *iter)
{
	spl_filesystem_iterator *iterator = (spl_filesystem_iterator *)iter;

	return &iterator->current;
}
/* }}} */

/* {{{ spl_filesystem_dir_it_current_key */
static void spl_filesystem_dir_it_current_key(zend_object_iterator *iter, zval *key)
{
	spl_filesystem_object *object = spl_filesystem_iterator_to_object((spl_filesystem_iterator *)iter);

	ZVAL_LONG(key, object->u.dir.index);
}
/* }}} */

/* {{{ spl_filesystem_dir_it_move_forward */
static void spl_filesystem_dir_it_move_forward(zend_object_iterator *iter)
{
	spl_filesystem_object *object = spl_filesystem_iterator_to_object((spl_filesystem_iterator *)iter);

	object->u.dir.index++;
	spl_filesystem_dir_read(object);
	if (object->file_name) {
		zend_string_release(object->file_name);
		object->file_name = NULL;
	}
}
/* }}} */

/* {{{ spl_filesystem_dir_it_rewind */
static void spl_filesystem_dir_it_rewind(zend_object_iterator *iter)
{
	spl_filesystem_object *object = spl_filesystem_iterator_to_object((spl_filesystem_iterator *)iter);

	object->u.dir.index = 0;
	if (object->u.dir.dirp) {
		php_stream_rewinddir(object->u.dir.dirp);
	}
	spl_filesystem_dir_read(object);
}
/* }}} */

/* {{{ spl_filesystem_tree_it_dtor */
static void spl_filesystem_tree_it_dtor(zend_object_iterator *iter)
{
	spl_filesystem_iterator *iterator = (spl_filesystem_iterator *)iter;
	zval_ptr_dtor(&iterator->intern.data);
	zval_ptr_dtor(&iterator->current);
}
/* }}} */

/* {{{ spl_filesystem_tree_it_current_data */
static zval *spl_filesystem_tree_it_current_data(zend_object_iterator *iter)
{
	spl_filesystem_iterator *iterator = (spl_filesystem_iterator *)iter;
	spl_filesystem_object   *object   = spl_filesystem_iterator_to_object(iterator);

	if (SPL_FILE_DIR_CURRENT(object, SPL_FILE_DIR_CURRENT_AS_PATHNAME)) {
		if (Z_ISUNDEF(iterator->current)) {
			if (spl_filesystem_object_get_file_name(object) == FAILURE) {
				return NULL;
			}
			ZVAL_STR_COPY(&iterator->current, object->file_name);
		}
		return &iterator->current;
	} else if (SPL_FILE_DIR_CURRENT(object, SPL_FILE_DIR_CURRENT_AS_FILEINFO)) {
		if (Z_ISUNDEF(iterator->current)) {
			if (spl_filesystem_object_get_file_name(object) == FAILURE) {
				return NULL;
			}
			spl_filesystem_object_create_type(0, object, SPL_FS_INFO, NULL, &iterator->current);
		}
		return &iterator->current;
	} else {
		return &iterator->intern.data;
	}
}
/* }}} */

/* {{{ spl_filesystem_tree_it_current_key */
static void spl_filesystem_tree_it_current_key(zend_object_iterator *iter, zval *key)
{
	spl_filesystem_object *object = spl_filesystem_iterator_to_object((spl_filesystem_iterator *)iter);

	if (SPL_FILE_DIR_KEY(object, SPL_FILE_DIR_KEY_AS_FILENAME)) {
		ZVAL_STRING(key, object->u.dir.entry.d_name);
	} else {
		if (spl_filesystem_object_get_file_name(object) == FAILURE) {
			return;
		}
		ZVAL_STR_COPY(key, object->file_name);
	}
}
/* }}} */

/* {{{ spl_filesystem_tree_it_move_forward */
static void spl_filesystem_tree_it_move_forward(zend_object_iterator *iter)
{
	spl_filesystem_iterator *iterator = (spl_filesystem_iterator *)iter;
	spl_filesystem_object   *object   = spl_filesystem_iterator_to_object(iterator);
	bool skip_dots = SPL_HAS_FLAG(object->flags, SPL_FILE_DIR_SKIPDOTS);

	object->u.dir.index++;
	do {
		spl_filesystem_dir_read(object);
	} while (skip_dots && spl_filesystem_is_dot(object->u.dir.entry.d_name));
	if (object->file_name) {
		zend_string_release(object->file_name);
		object->file_name = NULL;
	}
	if (!Z_ISUNDEF(iterator->current)) {
		zval_ptr_dtor(&iterator->current);
		ZVAL_UNDEF(&iterator->current);
	}
}
/* }}} */

/* {{{ spl_filesystem_tree_it_rewind */
static void spl_filesystem_tree_it_rewind(zend_object_iterator *iter)
{
	spl_filesystem_iterator *iterator = (spl_filesystem_iterator *)iter;
	spl_filesystem_object   *object   = spl_filesystem_iterator_to_object(iterator);
	bool skip_dots = SPL_HAS_FLAG(object->flags, SPL_FILE_DIR_SKIPDOTS);

	object->u.dir.index = 0;
	if (object->u.dir.dirp) {
		php_stream_rewinddir(object->u.dir.dirp);
	}
	do {
		spl_filesystem_dir_read(object);
	} while (skip_dots && spl_filesystem_is_dot(object->u.dir.entry.d_name));
	if (!Z_ISUNDEF(iterator->current)) {
		zval_ptr_dtor(&iterator->current);
		ZVAL_UNDEF(&iterator->current);
	}
}
/* }}} */

/* {{{ iterator handler table */
static const zend_object_iterator_funcs spl_filesystem_tree_it_funcs = {
	spl_filesystem_tree_it_dtor,
	spl_filesystem_dir_it_valid,
	spl_filesystem_tree_it_current_data,
	spl_filesystem_tree_it_current_key,
	spl_filesystem_tree_it_move_forward,
	spl_filesystem_tree_it_rewind,
	NULL,
	NULL, /* get_gc */
};
/* }}} */

/* {{{ spl_ce_dir_get_iterator */
zend_object_iterator *spl_filesystem_tree_get_iterator(zend_class_entry *ce, zval *object, int by_ref)
{
	spl_filesystem_iterator *iterator;
	spl_filesystem_object *dir_object;

	if (by_ref) {
		zend_throw_error(NULL, "An iterator cannot be used with foreach by reference");
		return NULL;
	}
	dir_object = spl_filesystem_from_obj(Z_OBJ_P(object));
	iterator = spl_filesystem_object_to_iterator(dir_object);

	ZVAL_OBJ_COPY(&iterator->intern.data, Z_OBJ_P(object));
	iterator->intern.funcs = &spl_filesystem_tree_it_funcs;

	return &iterator->intern;
}
/* }}} */

static ZEND_COLD void spl_filesystem_file_cannot_read(spl_filesystem_object *intern)
{
	zend_throw_exception_ex(spl_ce_RuntimeException, 0, "Cannot read from file %s", ZSTR_VAL(intern->file_name));
}

static zend_result spl_filesystem_file_read_ex(spl_filesystem_object *intern, bool silent, zend_long line_add, bool csv)
{
	char *buf;
	size_t line_len = 0;

	spl_filesystem_file_free_line(intern);

	if (php_stream_eof(intern->u.file.stream)) {
		if (!silent) {
			spl_filesystem_file_cannot_read(intern);
		}
		return FAILURE;
	}

	if (intern->u.file.max_line_len > 0) {
		buf = safe_emalloc((intern->u.file.max_line_len + 1), sizeof(char), 0);
		if (php_stream_get_line(intern->u.file.stream, buf, intern->u.file.max_line_len + 1, &line_len) == NULL) {
			efree(buf);
			buf = NULL;
		} else {
			buf[line_len] = '\0';
		}
	} else {
		buf = php_stream_get_line(intern->u.file.stream, NULL, 0, &line_len);
	}

	if (!buf) {
		intern->u.file.current_line = estrdup("");
		intern->u.file.current_line_len = 0;
	} else {
		if (!csv && SPL_HAS_FLAG(intern->flags, SPL_FILE_OBJECT_DROP_NEW_LINE)) {
			if (line_len > 0 && buf[line_len - 1] == '\n') {
				line_len--;
				if (line_len > 0 && buf[line_len - 1] == '\r') {
					line_len--;
				}
				buf[line_len] = '\0';
			}
		}

		intern->u.file.current_line = buf;
		intern->u.file.current_line_len = line_len;
	}
	intern->u.file.current_line_num += line_add;

	return SUCCESS;
} /* }}} */

static inline zend_result spl_filesystem_file_read(spl_filesystem_object *intern, bool silent, bool csv)
{
	zend_long line_add = (intern->u.file.current_line) ? 1 : 0;
	return spl_filesystem_file_read_ex(intern, silent, line_add, csv);
}

static bool is_line_empty(spl_filesystem_object *intern)
{
	char *current_line = intern->u.file.current_line;
	size_t current_line_len = intern->u.file.current_line_len;
	return current_line_len == 0
		|| ((SPL_HAS_FLAG(intern->flags, SPL_FILE_OBJECT_READ_CSV) && SPL_HAS_FLAG(intern->flags, SPL_FILE_OBJECT_DROP_NEW_LINE)
			&& ((current_line_len == 1 && current_line[0] == '\n')
				|| (current_line_len == 2 && current_line[0] == '\r' && current_line[1] == '\n'))));
}

static zend_result spl_filesystem_file_read_csv(spl_filesystem_object *intern, char delimiter, char enclosure, int escape, zval *return_value, bool silent) /* {{{ */
{
	do {
		zend_result ret = spl_filesystem_file_read(intern, silent, /* csv */ true);
		if (ret != SUCCESS) {
			return ret;
		}
	} while (is_line_empty(intern) && SPL_HAS_FLAG(intern->flags, SPL_FILE_OBJECT_SKIP_EMPTY));

	size_t buf_len = intern->u.file.current_line_len;
	char *buf = estrndup(intern->u.file.current_line, buf_len);

	if (!Z_ISUNDEF(intern->u.file.current_zval)) {
		zval_ptr_dtor(&intern->u.file.current_zval);
		ZVAL_UNDEF(&intern->u.file.current_zval);
	}

	HashTable *values = php_fgetcsv(intern->u.file.stream, delimiter, enclosure, escape, buf_len, buf);
	if (values == NULL) {
		values = php_bc_fgetcsv_empty_line();
	}
	ZVAL_ARR(&intern->u.file.current_zval, values);
	if (return_value) {
		ZVAL_COPY(return_value, &intern->u.file.current_zval);
	}
	return SUCCESS;
}
/* }}} */

static zend_result spl_filesystem_file_read_line_ex(zval * this_ptr, spl_filesystem_object *intern, bool silent) /* {{{ */
{
	zval retval;

	/* 1) use fgetcsv? 2) overloaded call the function, 3) do it directly */
	if (SPL_HAS_FLAG(intern->flags, SPL_FILE_OBJECT_READ_CSV)) {
		return spl_filesystem_file_read_csv(intern, intern->u.file.delimiter, intern->u.file.enclosure, intern->u.file.escape, NULL, silent);
	}
	if (intern->u.file.func_getCurr->common.scope != spl_ce_SplFileObject) {
		spl_filesystem_file_free_line(intern);

		if (php_stream_eof(intern->u.file.stream)) {
			if (!silent) {
				spl_filesystem_file_cannot_read(intern);
			}
			return FAILURE;
		}
		zend_call_method_with_0_params(Z_OBJ_P(this_ptr), Z_OBJCE_P(this_ptr), &intern->u.file.func_getCurr, "getCurrentLine", &retval);
		if (Z_ISUNDEF(retval)) {
			return FAILURE;
		}

		if (Z_TYPE(retval) != IS_STRING) {
			zend_type_error("%s::getCurrentLine(): Return value must be of type string, %s returned",
				ZSTR_VAL(Z_OBJCE_P(this_ptr)->name), zend_zval_type_name(&retval));
			zval_ptr_dtor(&retval);
			return FAILURE;
		}

		if (intern->u.file.current_line || !Z_ISUNDEF(intern->u.file.current_zval)) {
			intern->u.file.current_line_num++;
		}
		spl_filesystem_file_free_line(intern);
		intern->u.file.current_line = estrndup(Z_STRVAL(retval), Z_STRLEN(retval));
		intern->u.file.current_line_len = Z_STRLEN(retval);
		zval_ptr_dtor(&retval);
		return SUCCESS;
	} else {
		return spl_filesystem_file_read(intern, silent, /* csv */ false);
	}
} /* }}} */

static zend_result spl_filesystem_file_read_line(zval * this_ptr, spl_filesystem_object *intern, bool silent) /* {{{ */
{
	zend_result ret = spl_filesystem_file_read_line_ex(this_ptr, intern, silent);

	while (SPL_HAS_FLAG(intern->flags, SPL_FILE_OBJECT_SKIP_EMPTY) && ret == SUCCESS && is_line_empty(intern)) {
		spl_filesystem_file_free_line(intern);
		ret = spl_filesystem_file_read_line_ex(this_ptr, intern, silent);
	}

	return ret;
}
/* }}} */

static void spl_filesystem_file_rewind(zval * this_ptr, spl_filesystem_object *intern) /* {{{ */
{
	if (!intern->u.file.stream) {
		zend_throw_error(NULL, "Object not initialized");
		return;
	}
	if (-1 == php_stream_rewind(intern->u.file.stream)) {
		zend_throw_exception_ex(spl_ce_RuntimeException, 0, "Cannot rewind file %s", ZSTR_VAL(intern->file_name));
		return;
	}

	spl_filesystem_file_free_line(intern);
	intern->u.file.current_line_num = 0;

	if (SPL_HAS_FLAG(intern->flags, SPL_FILE_OBJECT_READ_AHEAD)) {
		spl_filesystem_file_read_line(this_ptr, intern, true);
	}
} /* }}} */

/* {{{ Construct a new file object */
PHP_METHOD(SplFileObject, __construct)
{
	spl_filesystem_object *intern = spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS));
	zend_string *open_mode = ZSTR_CHAR('r');
	bool use_include_path = 0;
	size_t path_len;
	zend_error_handling error_handling;

	intern->u.file.open_mode = ZSTR_CHAR('r');

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "P|Sbr!",
			&intern->file_name, &open_mode,
			&use_include_path, &intern->u.file.zcontext) == FAILURE) {
		intern->u.file.open_mode = NULL;
		intern->file_name = NULL;
		RETURN_THROWS();
	}

	intern->u.file.open_mode = zend_string_copy(open_mode);

	/* spl_filesystem_file_open() can generate E_WARNINGs which we want to promote to exceptions */
	zend_replace_error_handling(EH_THROW, spl_ce_RuntimeException, &error_handling);
	zend_result retval = spl_filesystem_file_open(intern, use_include_path);
	zend_restore_error_handling(&error_handling);
	if (retval == FAILURE) {
		RETURN_THROWS();
	}

	path_len = strlen(intern->u.file.stream->orig_path);

	if (path_len > 1 && IS_SLASH_AT(intern->u.file.stream->orig_path, path_len-1)) {
		path_len--;
	}

	while (path_len > 1 && !IS_SLASH_AT(intern->u.file.stream->orig_path, path_len-1)) {
		path_len--;
	}

	if (path_len) {
		path_len--;
	}

	intern->path = zend_string_init(intern->u.file.stream->orig_path, path_len, 0);
} /* }}} */

/* {{{ Construct a new temp file object */
PHP_METHOD(SplTempFileObject, __construct)
{
	zend_string *file_name;
	zend_long max_memory = PHP_STREAM_MAX_MEM;
	spl_filesystem_object *intern = spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS));
	zend_error_handling error_handling;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|l", &max_memory) == FAILURE) {
		RETURN_THROWS();
	}

	if (max_memory < 0) {
		file_name = zend_string_init("php://memory", sizeof("php://memory")-1, 0);
	} else if (ZEND_NUM_ARGS()) {
		file_name = zend_strpprintf(0, "php://temp/maxmemory:" ZEND_LONG_FMT, max_memory);
	} else {
		file_name = zend_string_init("php://temp", sizeof("php://temp")-1, 0);
	}
	intern->file_name = file_name;
	intern->u.file.open_mode = zend_string_init("wb", sizeof("wb")-1, 0);

	/* spl_filesystem_file_open() can generate E_WARNINGs which we want to promote to exceptions */
	zend_replace_error_handling(EH_THROW, spl_ce_RuntimeException, &error_handling);
	if (spl_filesystem_file_open(intern, /* use_include_path */ false) == SUCCESS) {
		intern->path = ZSTR_EMPTY_ALLOC();
	}
	zend_string_release(file_name);
	zend_restore_error_handling(&error_handling);
} /* }}} */

/* {{{ Rewind the file and read the first line */
PHP_METHOD(SplFileObject, rewind)
{
	spl_filesystem_object *intern = spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS));

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	spl_filesystem_file_rewind(ZEND_THIS, intern);
} /* }}} */

/* {{{ Return whether end of file is reached */
PHP_METHOD(SplFileObject, eof)
{
	spl_filesystem_object *intern = spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS));

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	CHECK_SPL_FILE_OBJECT_IS_INITIALIZED(intern);

	RETURN_BOOL(php_stream_eof(intern->u.file.stream));
} /* }}} */

/* {{{ Return !eof() */
PHP_METHOD(SplFileObject, valid)
{
	spl_filesystem_object *intern = spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS));

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	if (SPL_HAS_FLAG(intern->flags, SPL_FILE_OBJECT_READ_AHEAD)) {
		RETURN_BOOL(intern->u.file.current_line || !Z_ISUNDEF(intern->u.file.current_zval));
	}
	if (!intern->u.file.stream) {
		RETURN_FALSE;
	}
	RETURN_BOOL(!php_stream_eof(intern->u.file.stream));
} /* }}} */

/* {{{ Return next line from file */
PHP_METHOD(SplFileObject, fgets)
{
	spl_filesystem_object *intern = spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS));

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	CHECK_SPL_FILE_OBJECT_IS_INITIALIZED(intern);

	if (spl_filesystem_file_read_ex(intern, /* silent */ false, /* line_add */ 1, /* csv */ false) == FAILURE) {
		RETURN_THROWS();
	}
	RETURN_STRINGL(intern->u.file.current_line, intern->u.file.current_line_len);
} /* }}} */

/* {{{ Return current line from file */
PHP_METHOD(SplFileObject, current)
{
	spl_filesystem_object *intern = spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS));

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	CHECK_SPL_FILE_OBJECT_IS_INITIALIZED(intern);

	if (!intern->u.file.current_line && Z_ISUNDEF(intern->u.file.current_zval)) {
		spl_filesystem_file_read_line(ZEND_THIS, intern, true);
	}
	if (intern->u.file.current_line && (!SPL_HAS_FLAG(intern->flags, SPL_FILE_OBJECT_READ_CSV) || Z_ISUNDEF(intern->u.file.current_zval))) {
		RETURN_STRINGL(intern->u.file.current_line, intern->u.file.current_line_len);
	} else if (!Z_ISUNDEF(intern->u.file.current_zval)) {
		ZEND_ASSERT(!Z_ISREF(intern->u.file.current_zval));
		ZEND_ASSERT(Z_TYPE(intern->u.file.current_zval) == IS_ARRAY);
		RETURN_COPY(&intern->u.file.current_zval);
	}
	RETURN_FALSE;
} /* }}} */

/* {{{ Return line number */
PHP_METHOD(SplFileObject, key)
{
	spl_filesystem_object *intern = spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS));

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	/* Do not read the next line to support correct counting with fgetc()
	if (!intern->u.file.current_line) {
		spl_filesystem_file_read_line(ZEND_THIS, intern);
	} */
	RETURN_LONG(intern->u.file.current_line_num);
} /* }}} */

/* {{{ Read next line */
PHP_METHOD(SplFileObject, next)
{
	spl_filesystem_object *intern = spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS));

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	spl_filesystem_file_free_line(intern);
	if (SPL_HAS_FLAG(intern->flags, SPL_FILE_OBJECT_READ_AHEAD)) {
		spl_filesystem_file_read_line(ZEND_THIS, intern, true);
	}
	intern->u.file.current_line_num++;
} /* }}} */

/* {{{ Set file handling flags */
PHP_METHOD(SplFileObject, setFlags)
{
	spl_filesystem_object *intern = spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS));

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &intern->flags) == FAILURE) {
		RETURN_THROWS();
	}
} /* }}} */

/* {{{ Get file handling flags */
PHP_METHOD(SplFileObject, getFlags)
{
	spl_filesystem_object *intern = spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS));

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_LONG(intern->flags & SPL_FILE_OBJECT_MASK);
} /* }}} */

/* {{{ Set maximum line length */
PHP_METHOD(SplFileObject, setMaxLineLen)
{
	zend_long max_len;

	spl_filesystem_object *intern = spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS));

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &max_len) == FAILURE) {
		RETURN_THROWS();
	}

	if (max_len < 0) {
		zend_argument_value_error(1, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	intern->u.file.max_line_len = max_len;
} /* }}} */

/* {{{ Get maximum line length */
PHP_METHOD(SplFileObject, getMaxLineLen)
{
	spl_filesystem_object *intern = spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS));

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_LONG((zend_long)intern->u.file.max_line_len);
} /* }}} */

/* {{{ Return false */
PHP_METHOD(SplFileObject, hasChildren)
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_FALSE;
} /* }}} */

/* {{{ Read NULL */
PHP_METHOD(SplFileObject, getChildren)
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	/* return NULL */
} /* }}} */

/* {{{ Return current line as CSV */
PHP_METHOD(SplFileObject, fgetcsv)
{
	spl_filesystem_object *intern = spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS));
	char delimiter = intern->u.file.delimiter, enclosure = intern->u.file.enclosure;
	int escape = intern->u.file.escape;
	char *delim = NULL, *enclo = NULL, *esc = NULL;
	size_t d_len = 0, e_len = 0, esc_len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|sss", &delim, &d_len, &enclo, &e_len, &esc, &esc_len) == FAILURE) {
		RETURN_THROWS();
	}

	CHECK_SPL_FILE_OBJECT_IS_INITIALIZED(intern);

	if (delim) {
		if (d_len != 1) {
			zend_argument_value_error(1, "must be a single character");
			RETURN_THROWS();
		}
		delimiter = delim[0];
	}
	if (enclo) {
		if (e_len != 1) {
			zend_argument_value_error(2, "must be a single character");
			RETURN_THROWS();
		}
		enclosure = enclo[0];
	}
	if (esc) {
		if (esc_len > 1) {
			zend_argument_value_error(3, "must be empty or a single character");
			RETURN_THROWS();
		}
		if (esc_len == 0) {
			escape = PHP_CSV_NO_ESCAPE;
		} else {
			escape = (unsigned char) esc[0];
		}
	}

	if (spl_filesystem_file_read_csv(intern, delimiter, enclosure, escape, return_value, true) == FAILURE) {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ Output a field array as a CSV line */
PHP_METHOD(SplFileObject, fputcsv)
{
	spl_filesystem_object *intern = spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS));
	char delimiter = intern->u.file.delimiter, enclosure = intern->u.file.enclosure;
	int escape = intern->u.file.escape;
	char *delim = NULL, *enclo = NULL, *esc = NULL;
	size_t d_len = 0, e_len = 0, esc_len = 0;
	zend_long ret;
	zval *fields = NULL;
	zend_string *eol = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "a|sssS", &fields, &delim, &d_len, &enclo, &e_len, &esc, &esc_len, &eol) == FAILURE) {
		RETURN_THROWS();
	}

	if (delim) {
		if (d_len != 1) {
			zend_argument_value_error(2, "must be a single character");
			RETURN_THROWS();
		}
		delimiter = delim[0];
	}
	if (enclo) {
		if (e_len != 1) {
			zend_argument_value_error(3, "must be a single character");
			RETURN_THROWS();
		}
		enclosure = enclo[0];
	}
	if (esc) {
		if (esc_len > 1) {
			zend_argument_value_error(4, "must be empty or a single character");
			RETURN_THROWS();
		}
		if (esc_len == 0) {
			escape = PHP_CSV_NO_ESCAPE;
		} else {
			escape = (unsigned char) esc[0];
		}
	}

	ret = php_fputcsv(intern->u.file.stream, fields, delimiter, enclosure, escape, eol);
	if (ret < 0) {
		RETURN_FALSE;
	}
	RETURN_LONG(ret);
}
/* }}} */

/* {{{ Set the delimiter, enclosure and escape character used in fgetcsv */
PHP_METHOD(SplFileObject, setCsvControl)
{
	spl_filesystem_object *intern = spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS));
	char delimiter = ',', enclosure = '"';
	int escape = (unsigned char) '\\';
	char *delim = NULL, *enclo = NULL, *esc = NULL;
	size_t d_len = 0, e_len = 0, esc_len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|sss", &delim, &d_len, &enclo, &e_len, &esc, &esc_len) == FAILURE) {
		RETURN_THROWS();
	}

	if (delim) {
		if (d_len != 1) {
			zend_argument_value_error(1, "must be a single character");
			RETURN_THROWS();
		}
		delimiter = delim[0];
	}
	if (enclo) {
		if (e_len != 1) {
			zend_argument_value_error(2, "must be a single character");
			RETURN_THROWS();
		}
		enclosure = enclo[0];
	}
	if (esc) {
		if (esc_len > 1) {
			zend_argument_value_error(3, "must be empty or a single character");
			RETURN_THROWS();
		}
		if (esc_len == 0) {
			escape = PHP_CSV_NO_ESCAPE;
		} else {
			escape = (unsigned char) esc[0];
		}
	}

	intern->u.file.delimiter = delimiter;
	intern->u.file.enclosure = enclosure;
	intern->u.file.escape    = escape;
}
/* }}} */

/* {{{ Get the delimiter, enclosure and escape character used in fgetcsv */
PHP_METHOD(SplFileObject, getCsvControl)
{
	spl_filesystem_object *intern = spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS));
	char delimiter[2], enclosure[2], escape[2];

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	array_init(return_value);

	delimiter[0] = intern->u.file.delimiter;
	delimiter[1] = '\0';
	enclosure[0] = intern->u.file.enclosure;
	enclosure[1] = '\0';
	if (intern->u.file.escape == PHP_CSV_NO_ESCAPE) {
		escape[0] = '\0';
	} else {
		escape[0] = (unsigned char) intern->u.file.escape;
		escape[1] = '\0';
	}

	add_next_index_string(return_value, delimiter);
	add_next_index_string(return_value, enclosure);
	add_next_index_string(return_value, escape);
}
/* }}} */

/* {{{ Portable file locking */
PHP_METHOD(SplFileObject, flock)
{
	spl_filesystem_object *intern = spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS));
	zval *wouldblock = NULL;
	zend_long operation = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l|z", &operation, &wouldblock) == FAILURE) {
		RETURN_THROWS();
	}

	CHECK_SPL_FILE_OBJECT_IS_INITIALIZED(intern);

	php_flock_common(intern->u.file.stream, operation, 1, wouldblock, return_value);
}
/* }}} */

/* {{{ Flush the file */
PHP_METHOD(SplFileObject, fflush)
{
	spl_filesystem_object *intern = spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS));

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	CHECK_SPL_FILE_OBJECT_IS_INITIALIZED(intern);

	RETURN_BOOL(!php_stream_flush(intern->u.file.stream));
} /* }}} */

/* {{{ Return current file position */
PHP_METHOD(SplFileObject, ftell)
{
	spl_filesystem_object *intern = spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS));
	zend_long ret;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	CHECK_SPL_FILE_OBJECT_IS_INITIALIZED(intern);

	ret = php_stream_tell(intern->u.file.stream);

	if (ret == -1) {
		RETURN_FALSE;
	} else {
		RETURN_LONG(ret);
	}
} /* }}} */

/* {{{ Seek to a position */
PHP_METHOD(SplFileObject, fseek)
{
	spl_filesystem_object *intern = spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS));
	zend_long pos, whence = SEEK_SET;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l|l", &pos, &whence) == FAILURE) {
		RETURN_THROWS();
	}

	CHECK_SPL_FILE_OBJECT_IS_INITIALIZED(intern);

	spl_filesystem_file_free_line(intern);
	RETURN_LONG(php_stream_seek(intern->u.file.stream, pos, (int)whence));
} /* }}} */

/* {{{ Get a character from the file */
PHP_METHOD(SplFileObject, fgetc)
{
	spl_filesystem_object *intern = spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS));
	char buf[2];
	int result;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	CHECK_SPL_FILE_OBJECT_IS_INITIALIZED(intern);

	spl_filesystem_file_free_line(intern);

	result = php_stream_getc(intern->u.file.stream);

	if (result == EOF) {
		RETURN_FALSE;
	}
	if (result == '\n') {
		intern->u.file.current_line_num++;
	}
	buf[0] = result;
	buf[1] = '\0';

	RETURN_STRINGL(buf, 1);
} /* }}} */

/* {{{ Output all remaining data from a file pointer */
PHP_METHOD(SplFileObject, fpassthru)
{
	spl_filesystem_object *intern = spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS));

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	CHECK_SPL_FILE_OBJECT_IS_INITIALIZED(intern);

	RETURN_LONG(php_stream_passthru(intern->u.file.stream));
} /* }}} */

/* {{{ Implements a mostly ANSI compatible fscanf() */
PHP_METHOD(SplFileObject, fscanf)
{
	int result, num_varargs = 0;
	zend_string *format_str;
	zval *varargs= NULL;
	spl_filesystem_object *intern = spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS));

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S*", &format_str, &varargs, &num_varargs) == FAILURE) {
		RETURN_THROWS();
	}

	CHECK_SPL_FILE_OBJECT_IS_INITIALIZED(intern);

	/* Get next line */
	if (spl_filesystem_file_read(intern, /* silent */ false, /* csv */ false) == FAILURE) {
		RETURN_THROWS();
	}

	result = php_sscanf_internal(intern->u.file.current_line, ZSTR_VAL(format_str), num_varargs, varargs, 0, return_value);

	if (SCAN_ERROR_WRONG_PARAM_COUNT == result) {
		WRONG_PARAM_COUNT;
	}
}
/* }}} */

/* {{{ Binary-safe file write */
PHP_METHOD(SplFileObject, fwrite)
{
	spl_filesystem_object *intern = spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS));
	char *str;
	size_t str_len;
	zend_long length = 0;
	ssize_t written;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|l", &str, &str_len, &length) == FAILURE) {
		RETURN_THROWS();
	}

	CHECK_SPL_FILE_OBJECT_IS_INITIALIZED(intern);

	if (ZEND_NUM_ARGS() > 1) {
		if (length >= 0) {
			str_len = MIN((size_t)length, str_len);
		} else {
			/* Negative length given, nothing to write */
			str_len = 0;
		}
	}
	if (!str_len) {
		RETURN_LONG(0);
	}

	written = php_stream_write(intern->u.file.stream, str, str_len);
	if (written < 0) {
		RETURN_FALSE;
	}
	RETURN_LONG(written);
} /* }}} */

PHP_METHOD(SplFileObject, fread)
{
	spl_filesystem_object *intern = spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS));
	zend_long length = 0;
	zend_string *str;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &length) == FAILURE) {
		RETURN_THROWS();
	}

	CHECK_SPL_FILE_OBJECT_IS_INITIALIZED(intern);

	if (length <= 0) {
		zend_argument_value_error(1, "must be greater than 0");
		RETURN_THROWS();
	}

	str = php_stream_read_to_str(intern->u.file.stream, length);
	if (!str) {
		RETURN_FALSE;
	}
	RETURN_STR(str);
}

/* {{{ Stat() on a filehandle */
PHP_METHOD(SplFileObject, fstat)
{
	spl_filesystem_object *intern = spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS));

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	CHECK_SPL_FILE_OBJECT_IS_INITIALIZED(intern);

	php_fstat(intern->u.file.stream, return_value);
}
/* }}} */

/* {{{ Truncate file to 'size' length */
PHP_METHOD(SplFileObject, ftruncate)
{
	spl_filesystem_object *intern = spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS));
	zend_long size;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &size) == FAILURE) {
		RETURN_THROWS();
	}

	CHECK_SPL_FILE_OBJECT_IS_INITIALIZED(intern);

	if (!php_stream_truncate_supported(intern->u.file.stream)) {
		zend_throw_exception_ex(spl_ce_LogicException, 0, "Can't truncate file %s", ZSTR_VAL(intern->file_name));
		RETURN_THROWS();
	}

	RETURN_BOOL(0 == php_stream_truncate_set_size(intern->u.file.stream, size));
} /* }}} */

/* {{{ Seek to specified line */
PHP_METHOD(SplFileObject, seek)
{
	spl_filesystem_object *intern = spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS));
	zend_long line_pos, i;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &line_pos) == FAILURE) {
		RETURN_THROWS();
	}

	CHECK_SPL_FILE_OBJECT_IS_INITIALIZED(intern);

	if (line_pos < 0) {
		zend_argument_value_error(1, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	spl_filesystem_file_rewind(ZEND_THIS, intern);

	for (i = 0; i < line_pos; i++) {
		if (spl_filesystem_file_read_line(ZEND_THIS, intern, true) == FAILURE) {
			return;
		}
	}
	if (line_pos > 0 && !SPL_HAS_FLAG(intern->flags, SPL_FILE_OBJECT_READ_AHEAD)) {
		intern->u.file.current_line_num++;
		spl_filesystem_file_free_line(intern);
	}
} /* }}} */

PHP_METHOD(SplFileObject, __toString)
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	spl_filesystem_object *intern = spl_filesystem_from_obj(Z_OBJ_P(ZEND_THIS));

	CHECK_SPL_FILE_OBJECT_IS_INITIALIZED(intern);

	if (!intern->u.file.current_line) {
		ZEND_ASSERT(Z_ISUNDEF(intern->u.file.current_zval));
		zend_result result = spl_filesystem_file_read_line(ZEND_THIS, intern, false);
		if (UNEXPECTED(result != SUCCESS)) {
			RETURN_THROWS();
		}
	}

	RETURN_STRINGL(intern->u.file.current_line, intern->u.file.current_line_len);
}

/* {{{ PHP_MINIT_FUNCTION(spl_directory) */
PHP_MINIT_FUNCTION(spl_directory)
{
	spl_ce_SplFileInfo = register_class_SplFileInfo(zend_ce_stringable);
	spl_ce_SplFileInfo->create_object = spl_filesystem_object_new;

	memcpy(&spl_filesystem_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	spl_filesystem_object_handlers.offset = XtOffsetOf(spl_filesystem_object, std);
	spl_filesystem_object_handlers.clone_obj = spl_filesystem_object_clone;
	spl_filesystem_object_handlers.dtor_obj = spl_filesystem_object_destroy_object;
	spl_filesystem_object_handlers.free_obj = spl_filesystem_object_free_storage;

	spl_ce_DirectoryIterator = register_class_DirectoryIterator(spl_ce_SplFileInfo, spl_ce_SeekableIterator);
	spl_ce_DirectoryIterator->create_object = spl_filesystem_object_new;
	spl_ce_DirectoryIterator->get_iterator = spl_filesystem_dir_get_iterator;

	spl_ce_FilesystemIterator = register_class_FilesystemIterator(spl_ce_DirectoryIterator);
	spl_ce_FilesystemIterator->create_object = spl_filesystem_object_new;
	spl_ce_FilesystemIterator->get_iterator = spl_filesystem_tree_get_iterator;

	spl_ce_RecursiveDirectoryIterator = register_class_RecursiveDirectoryIterator(spl_ce_FilesystemIterator, spl_ce_RecursiveIterator);
	spl_ce_RecursiveDirectoryIterator->create_object = spl_filesystem_object_new;

	memcpy(&spl_filesystem_object_check_handlers, &spl_filesystem_object_handlers, sizeof(zend_object_handlers));
	spl_filesystem_object_check_handlers.clone_obj = NULL;
	spl_filesystem_object_check_handlers.get_method = spl_filesystem_object_get_method_check;

#ifdef HAVE_GLOB
	spl_ce_GlobIterator = register_class_GlobIterator(spl_ce_FilesystemIterator, zend_ce_countable);
	spl_ce_GlobIterator->create_object = spl_filesystem_object_new_check;
#endif

	spl_ce_SplFileObject = register_class_SplFileObject(spl_ce_SplFileInfo, spl_ce_RecursiveIterator, spl_ce_SeekableIterator);
	spl_ce_SplFileObject->create_object = spl_filesystem_object_new_check;

	spl_ce_SplTempFileObject = register_class_SplTempFileObject(spl_ce_SplFileObject);
	spl_ce_SplTempFileObject->create_object = spl_filesystem_object_new_check;

	return SUCCESS;
}
/* }}} */
