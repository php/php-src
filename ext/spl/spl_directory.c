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

#define CHECK_SPL_FILE_OBJECT_IS_INITIALIZED(spl_filesystem_object_pointer) \
	if (!(spl_filesystem_object_pointer)->u.file.stream) { \
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

	if (intern->_path) {
		efree(intern->_path);
	}
	if (intern->file_name) {
		efree(intern->file_name);
	}
	switch(intern->type) {
	case SPL_FS_INFO:
		break;
	case SPL_FS_DIR:
		if (intern->u.dir.sub_path) {
			efree(intern->u.dir.sub_path);
		}
		break;
	case SPL_FS_FILE:
		if (intern->u.file.open_mode) {
			efree(intern->u.file.open_mode);
		}
		if (intern->orig_path) {
			efree(intern->orig_path);
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

	intern = zend_object_alloc(sizeof(spl_filesystem_object), class_type);
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

PHPAPI char* spl_filesystem_object_get_path(spl_filesystem_object *intern, size_t *len) /* {{{ */
{
#ifdef HAVE_GLOB
	if (intern->type == SPL_FS_DIR) {
		if (php_stream_is(intern->u.dir.dirp ,&php_glob_stream_ops)) {
			return php_glob_stream_get_path(intern->u.dir.dirp, len);
		}
	}
#endif
	if (len) {
		*len = intern->_path_len;
	}
	return intern->_path;
} /* }}} */

static inline int spl_filesystem_object_get_file_name(spl_filesystem_object *intern) /* {{{ */
{
	char slash = SPL_HAS_FLAG(intern->flags, SPL_FILE_DIR_UNIXPATHS) ? '/' : DEFAULT_SLASH;

	switch (intern->type) {
		case SPL_FS_INFO:
		case SPL_FS_FILE:
			if (!intern->file_name) {
				zend_throw_error(NULL, "Object not initialized");
				return FAILURE;
			}
			break;
		case SPL_FS_DIR:
			{
				size_t path_len = 0;
				char *path = spl_filesystem_object_get_path(intern, &path_len);
				if (intern->file_name) {
					efree(intern->file_name);
				}
				/* if there is parent path, amend it, otherwise just use the given path as is */
				if (path_len == 0) {
					intern->file_name_len = spprintf(
						&intern->file_name, 0, "%s", intern->u.dir.entry.d_name);
				} else {
					intern->file_name_len = spprintf(
						&intern->file_name, 0, "%s%c%s", path, slash, intern->u.dir.entry.d_name);
				}
			}
			break;
	}
	return SUCCESS;
} /* }}} */

static int spl_filesystem_dir_read(spl_filesystem_object *intern) /* {{{ */
{
	if (!intern->u.dir.dirp || !php_stream_readdir(intern->u.dir.dirp, &intern->u.dir.entry)) {
		intern->u.dir.entry.d_name[0] = '\0';
		return 0;
	} else {
		return 1;
	}
}
/* }}} */

#define IS_SLASH_AT(zs, pos) (IS_SLASH(zs[pos]))

static inline int spl_filesystem_is_dot(const char * d_name) /* {{{ */
{
	return !strcmp(d_name, ".") || !strcmp(d_name, "..");
}
/* }}} */

/* {{{ spl_filesystem_dir_open */
/* open a directory resource */
static void spl_filesystem_dir_open(spl_filesystem_object* intern, char *path)
{
	int skip_dots = SPL_HAS_FLAG(intern->flags, SPL_FILE_DIR_SKIPDOTS);

	intern->type = SPL_FS_DIR;
	intern->_path_len = strlen(path);
	intern->u.dir.dirp = php_stream_opendir(path, REPORT_ERRORS, FG(default_context));

	if (intern->_path_len > 1 && IS_SLASH_AT(path, intern->_path_len-1)) {
		intern->_path = estrndup(path, --intern->_path_len);
	} else {
		intern->_path = estrndup(path, intern->_path_len);
	}
	intern->u.dir.index = 0;

	if (EG(exception) || intern->u.dir.dirp == NULL) {
		intern->u.dir.entry.d_name[0] = '\0';
		if (!EG(exception)) {
			/* open failed w/out notice (turned to exception due to EH_THROW) */
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0,
				"Failed to open directory \"%s\"", path);
		}
	} else {
		do {
			spl_filesystem_dir_read(intern);
		} while (skip_dots && spl_filesystem_is_dot(intern->u.dir.entry.d_name));
	}
}
/* }}} */

static int spl_filesystem_file_open(spl_filesystem_object *intern, int use_include_path, int silent) /* {{{ */
{
	zval tmp;

	intern->type = SPL_FS_FILE;

	php_stat(intern->file_name, intern->file_name_len, FS_IS_DIR, &tmp);
	if (Z_TYPE(tmp) == IS_TRUE) {
		intern->u.file.open_mode = NULL;
		intern->file_name = NULL;
		zend_throw_exception_ex(spl_ce_LogicException, 0, "Cannot use SplFileObject with directories");
		return FAILURE;
	}

	intern->u.file.context = php_stream_context_from_zval(intern->u.file.zcontext, 0);
	intern->u.file.stream = php_stream_open_wrapper_ex(intern->file_name, intern->u.file.open_mode, (use_include_path ? USE_PATH : 0) | REPORT_ERRORS, NULL, intern->u.file.context);

	if (!intern->file_name_len || !intern->u.file.stream) {
		if (!EG(exception)) {
			zend_throw_exception_ex(spl_ce_RuntimeException, 0, "Cannot open file '%s'", intern->file_name_len ? intern->file_name : "");
		}
		intern->file_name = NULL; /* until here it is not a copy */
		intern->u.file.open_mode = NULL;
		return FAILURE;
	}

	/*
	if (intern->u.file.zcontext) {
		//zend_list_addref(Z_RES_VAL(intern->u.file.zcontext));
		Z_ADDREF_P(intern->u.file.zcontext);
	}
	*/

	if (intern->file_name_len > 1 && IS_SLASH_AT(intern->file_name, intern->file_name_len-1)) {
		intern->file_name_len--;
	}

	intern->orig_path = estrndup(intern->u.file.stream->orig_path, strlen(intern->u.file.stream->orig_path));

	intern->file_name = estrndup(intern->file_name, intern->file_name_len);
	intern->u.file.open_mode = estrndup(intern->u.file.open_mode, intern->u.file.open_mode_len);

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
	int index, skip_dots;

	source = spl_filesystem_from_obj(old_object);
	new_object = spl_filesystem_object_new_ex(old_object->ce);
	intern = spl_filesystem_from_obj(new_object);

	intern->flags = source->flags;

	switch (source->type) {
		case SPL_FS_INFO:
			intern->_path_len = source->_path_len;
			intern->_path = estrndup(source->_path, source->_path_len);
			intern->file_name_len = source->file_name_len;
			intern->file_name = estrndup(source->file_name, intern->file_name_len);
			break;
		case SPL_FS_DIR:
			spl_filesystem_dir_open(intern, source->_path);
			/* read until we hit the position in which we were before */
			skip_dots = SPL_HAS_FLAG(source->flags, SPL_FILE_DIR_SKIPDOTS);
			for(index = 0; index < source->u.dir.index; ++index) {
				do {
					spl_filesystem_dir_read(intern);
				} while (skip_dots && spl_filesystem_is_dot(intern->u.dir.entry.d_name));
			}
			intern->u.dir.index = index;
			break;
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

void spl_filesystem_info_set_filename(spl_filesystem_object *intern, char *path, size_t len, size_t use_copy) /* {{{ */
{
	char *p1, *p2;

	if (intern->file_name) {
		efree(intern->file_name);
	}

	intern->file_name = use_copy ? estrndup(path, len) : path;
	intern->file_name_len = len;

	while (intern->file_name_len > 1 && IS_SLASH_AT(intern->file_name, intern->file_name_len-1)) {
		intern->file_name[intern->file_name_len-1] = 0;
		intern->file_name_len--;
	}

	p1 = strrchr(intern->file_name, '/');
#ifdef PHP_WIN32
	p2 = strrchr(intern->file_name, '\\');
#else
	p2 = 0;
#endif
	if (p1 || p2) {
		intern->_path_len = ((p1 > p2 ? p1 : p2) - intern->file_name);
	} else {
		intern->_path_len = 0;
	}

	if (intern->_path) {
		efree(intern->_path);
	}
	intern->_path = estrndup(path, intern->_path_len);
} /* }}} */

static spl_filesystem_object *spl_filesystem_object_create_info(spl_filesystem_object *source, char *file_path, size_t file_path_len, int use_copy, zend_class_entry *ce, zval *return_value) /* {{{ */
{
	spl_filesystem_object *intern;
	zval arg1;
	zend_error_handling error_handling;

	if (!file_path || !file_path_len) {
#ifdef PHP_WIN32
		zend_throw_exception_ex(spl_ce_RuntimeException, 0, "Cannot create SplFileInfo for empty path");
		if (file_path && !use_copy) {
			efree(file_path);
		}
#else
		if (file_path && !use_copy) {
			efree(file_path);
		}
		file_path_len = 1;
		file_path = "/";
#endif
		return NULL;
	}

	zend_replace_error_handling(EH_THROW, spl_ce_RuntimeException, &error_handling);

	ce = ce ? ce : source->info_class;

	zend_update_class_constants(ce);

	intern = spl_filesystem_from_obj(spl_filesystem_object_new_ex(ce));
	RETVAL_OBJ(&intern->std);

	if (ce->constructor->common.scope != spl_ce_SplFileInfo) {
		ZVAL_STRINGL(&arg1, file_path, file_path_len);
		zend_call_method_with_1_params(Z_OBJ_P(return_value), ce, &ce->constructor, "__construct", NULL, &arg1);
		zval_ptr_dtor(&arg1);
	} else {
		spl_filesystem_info_set_filename(intern, file_path, file_path_len, use_copy);
	}

	zend_restore_error_handling(&error_handling);
	return intern;
} /* }}} */

static spl_filesystem_object *spl_filesystem_object_create_type(int num_args, spl_filesystem_object *source, int type, zend_class_entry *ce, zval *return_value) /* {{{ */
{
	spl_filesystem_object *intern;
	zend_bool use_include_path = 0;
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

			if (UNEXPECTED(zend_update_class_constants(ce) != SUCCESS)) {
				return NULL;
			}

			intern = spl_filesystem_from_obj(spl_filesystem_object_new_ex(ce));
			RETVAL_OBJ(&intern->std);

			if (spl_filesystem_object_get_file_name(source) != SUCCESS) {
				return NULL;
			}

			if (ce->constructor->common.scope != spl_ce_SplFileInfo) {
				ZVAL_STRINGL(&arg1, source->file_name, source->file_name_len);
				zend_call_method_with_1_params(Z_OBJ_P(return_value), ce, &ce->constructor, "__construct", NULL, &arg1);
				zval_ptr_dtor(&arg1);
			} else {
				intern->file_name = estrndup(source->file_name, source->file_name_len);
				intern->file_name_len = source->file_name_len;
				intern->_path = spl_filesystem_object_get_path(source, &intern->_path_len);
				intern->_path = estrndup(intern->_path, intern->_path_len);
			}
			break;
		case SPL_FS_FILE:
		{
			ce = ce ? ce : source->file_class;

			if (UNEXPECTED(zend_update_class_constants(ce) != SUCCESS)) {
				return NULL;
			}

			char *open_mode = "r";
			size_t open_mode_len = 1;
			zval *resource = NULL;

			if (zend_parse_parameters(num_args, "|sbr!",
				&open_mode, &open_mode_len, &use_include_path, &resource) == FAILURE
			) {
				return NULL;
			}

			intern = spl_filesystem_from_obj(spl_filesystem_object_new_ex(ce));
			RETVAL_OBJ(&intern->std);

			if (spl_filesystem_object_get_file_name(source) != SUCCESS) {
				return NULL;
			}

			if (ce->constructor->common.scope != spl_ce_SplFileObject) {
				ZVAL_STRINGL(&arg1, source->file_name, source->file_name_len);
				ZVAL_STRINGL(&arg2, open_mode, open_mode_len);
				zend_call_method_with_2_params(Z_OBJ_P(return_value), ce, &ce->constructor, "__construct", NULL, &arg1, &arg2);
				zval_ptr_dtor(&arg1);
				zval_ptr_dtor(&arg2);
			} else {
				intern->file_name = source->file_name;
				intern->file_name_len = source->file_name_len;
				intern->_path = spl_filesystem_object_get_path(source, &intern->_path_len);
				intern->_path = estrndup(intern->_path, intern->_path_len);

				intern->u.file.open_mode = open_mode;
				intern->u.file.open_mode_len = open_mode_len;
				intern->u.file.zcontext = resource;

				zend_replace_error_handling(EH_THROW, spl_ce_RuntimeException, &error_handling);
				if (spl_filesystem_file_open(intern, use_include_path, 0) == FAILURE) {
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

static int spl_filesystem_is_invalid_or_dot(const char * d_name) /* {{{ */
{
	return d_name[0] == '\0' || spl_filesystem_is_dot(d_name);
}
/* }}} */

static char *spl_filesystem_object_get_pathname(spl_filesystem_object *intern, size_t *len) { /* {{{ */
	switch (intern->type) {
		case SPL_FS_INFO:
		case SPL_FS_FILE:
			*len = intern->file_name_len;
			return intern->file_name;
		case SPL_FS_DIR:
			if (intern->u.dir.entry.d_name[0]) {
				spl_filesystem_object_get_file_name(intern);
				*len = intern->file_name_len;
				return intern->file_name;
			}
	}
	*len = 0;
	return NULL;
}
/* }}} */

static inline HashTable *spl_filesystem_object_get_debug_info(zend_object *object) /* {{{ */
{
	spl_filesystem_object *intern = spl_filesystem_from_obj(object);
	zval tmp;
	HashTable *rv;
	zend_string *pnstr;
	char *path;
	size_t  path_len;
	char stmp[2];

	if (!intern->std.properties) {
		rebuild_object_properties(&intern->std);
	}

	rv = zend_array_dup(intern->std.properties);

	pnstr = spl_gen_private_prop_name(spl_ce_SplFileInfo, "pathName", sizeof("pathName")-1);
	path = spl_filesystem_object_get_pathname(intern, &path_len);
	ZVAL_STRINGL(&tmp, path ? path : "", path_len);
	zend_symtable_update(rv, pnstr, &tmp);
	zend_string_release_ex(pnstr, 0);

	if (intern->file_name) {
		pnstr = spl_gen_private_prop_name(spl_ce_SplFileInfo, "fileName", sizeof("fileName")-1);
		spl_filesystem_object_get_path(intern, &path_len);

		if (path_len && path_len < intern->file_name_len) {
			ZVAL_STRINGL(&tmp, intern->file_name + path_len + 1, intern->file_name_len - (path_len + 1));
		} else {
			ZVAL_STRINGL(&tmp, intern->file_name, intern->file_name_len);
		}
		zend_symtable_update(rv, pnstr, &tmp);
		zend_string_release_ex(pnstr, 0);
	}
	if (intern->type == SPL_FS_DIR) {
#ifdef HAVE_GLOB
		pnstr = spl_gen_private_prop_name(spl_ce_DirectoryIterator, "glob", sizeof("glob")-1);
		if (php_stream_is(intern->u.dir.dirp ,&php_glob_stream_ops)) {
			ZVAL_STRINGL(&tmp, intern->_path, intern->_path_len);
		} else {
			ZVAL_FALSE(&tmp);
		}
		zend_symtable_update(rv, pnstr, &tmp);
		zend_string_release_ex(pnstr, 0);
#endif
		pnstr = spl_gen_private_prop_name(spl_ce_RecursiveDirectoryIterator, "subPathName", sizeof("subPathName")-1);
		if (intern->u.dir.sub_path) {
			ZVAL_STRINGL(&tmp, intern->u.dir.sub_path, intern->u.dir.sub_path_len);
		} else {
			ZVAL_EMPTY_STRING(&tmp);
		}
		zend_symtable_update(rv, pnstr, &tmp);
		zend_string_release_ex(pnstr, 0);
	}
	if (intern->type == SPL_FS_FILE) {
		pnstr = spl_gen_private_prop_name(spl_ce_SplFileObject, "openMode", sizeof("openMode")-1);
		ZVAL_STRINGL(&tmp, intern->u.file.open_mode, intern->u.file.open_mode_len);
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
		zend_function *func;
		zend_string *tmp = zend_string_init("_bad_state_ex", sizeof("_bad_state_ex") - 1, 0);
		func = zend_std_get_method(object, tmp, NULL);
		zend_string_release_ex(tmp, 0);
		return func;
	}

	return zend_std_get_method(object, method, key);
}
/* }}} */

#define DIT_CTOR_FLAGS  0x00000001
#define DIT_CTOR_GLOB   0x00000002

void spl_filesystem_object_construct(INTERNAL_FUNCTION_PARAMETERS, zend_long ctor_flags) /* {{{ */
{
	spl_filesystem_object *intern;
	char *path;
	int parsed;
	size_t len;
	zend_long flags;
	zend_error_handling error_handling;

	if (SPL_HAS_FLAG(ctor_flags, DIT_CTOR_FLAGS)) {
		flags = SPL_FILE_DIR_KEY_AS_PATHNAME|SPL_FILE_DIR_CURRENT_AS_FILEINFO;
		parsed = zend_parse_parameters(ZEND_NUM_ARGS(), "p|l", &path, &len, &flags);
	} else {
		flags = SPL_FILE_DIR_KEY_AS_PATHNAME|SPL_FILE_DIR_CURRENT_AS_SELF;
		parsed = zend_parse_parameters(ZEND_NUM_ARGS(), "p", &path, &len);
	}
	if (SPL_HAS_FLAG(ctor_flags, SPL_FILE_DIR_SKIPDOTS)) {
		flags |= SPL_FILE_DIR_SKIPDOTS;
	}
	if (SPL_HAS_FLAG(ctor_flags, SPL_FILE_DIR_UNIXPATHS)) {
		flags |= SPL_FILE_DIR_UNIXPATHS;
	}
	if (parsed == FAILURE) {
		RETURN_THROWS();
	}

	if (len == 0) {
		zend_argument_value_error(1, "cannot be empty");
		RETURN_THROWS();
	}

	intern = Z_SPLFILESYSTEM_P(ZEND_THIS);
	if (intern->_path) {
		/* object is already initialized */
		zend_throw_error(NULL, "Directory object is already initialized");
		RETURN_THROWS();
	}
	intern->flags = flags;

	zend_replace_error_handling(EH_THROW, spl_ce_UnexpectedValueException, &error_handling);
#ifdef HAVE_GLOB
	if (SPL_HAS_FLAG(ctor_flags, DIT_CTOR_GLOB) && strstr(path, "glob://") != path) {
		spprintf(&path, 0, "glob://%s", path);
		spl_filesystem_dir_open(intern, path);
		efree(path);
	} else
#endif
	{
		spl_filesystem_dir_open(intern, path);

	}

	intern->u.dir.is_recursive = instanceof_function(intern->std.ce, spl_ce_RecursiveDirectoryIterator) ? 1 : 0;

	zend_restore_error_handling(&error_handling);
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
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(ZEND_THIS);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	intern->u.dir.index = 0;
	if (intern->u.dir.dirp) {
		php_stream_rewinddir(intern->u.dir.dirp);
	}
	spl_filesystem_dir_read(intern);
}
/* }}} */

/* {{{ Return current dir entry */
PHP_METHOD(DirectoryIterator, key)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(ZEND_THIS);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	if (intern->u.dir.dirp) {
		RETURN_LONG(intern->u.dir.index);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ Return this (needed for Iterator interface) */
PHP_METHOD(DirectoryIterator, current)
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	RETURN_OBJ_COPY(Z_OBJ_P(ZEND_THIS));
}
/* }}} */

/* {{{ Move to next entry */
PHP_METHOD(DirectoryIterator, next)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(ZEND_THIS);
	int skip_dots = SPL_HAS_FLAG(intern->flags, SPL_FILE_DIR_SKIPDOTS);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	intern->u.dir.index++;
	do {
		spl_filesystem_dir_read(intern);
	} while (skip_dots && spl_filesystem_is_dot(intern->u.dir.entry.d_name));
	if (intern->file_name) {
		efree(intern->file_name);
		intern->file_name = NULL;
	}
}
/* }}} */

/* {{{ Seek to the given position */
PHP_METHOD(DirectoryIterator, seek)
{
	spl_filesystem_object *intern    = Z_SPLFILESYSTEM_P(ZEND_THIS);
	zval retval;
	zend_long pos;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &pos) == FAILURE) {
		RETURN_THROWS();
	}

	if (intern->u.dir.index > pos) {
		/* we first rewind */
		zend_call_method_with_0_params(Z_OBJ_P(ZEND_THIS), Z_OBJCE_P(ZEND_THIS), &intern->u.dir.func_rewind, "rewind", NULL);
	}

	while (intern->u.dir.index < pos) {
		int valid = 0;
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
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(ZEND_THIS);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_BOOL(intern->u.dir.entry.d_name[0] != '\0');
}
/* }}} */

/* {{{ Return the path */
PHP_METHOD(SplFileInfo, getPath)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(ZEND_THIS);
	char *path;
	size_t path_len;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

  	path = spl_filesystem_object_get_path(intern, &path_len);
	if (path) {
		RETURN_STRINGL(path, path_len);
	} else {
		RETURN_EMPTY_STRING();
	}
}
/* }}} */

/* {{{ Return filename only */
PHP_METHOD(SplFileInfo, getFilename)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(ZEND_THIS);
	size_t path_len;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	if (!intern->file_name) {
		zend_throw_error(NULL, "Object not initialized");
		RETURN_THROWS();
	}

	spl_filesystem_object_get_path(intern, &path_len);

	if (path_len && path_len < intern->file_name_len) {
		RETURN_STRINGL(intern->file_name + path_len + 1, intern->file_name_len - (path_len + 1));
	} else {
		RETURN_STRINGL(intern->file_name, intern->file_name_len);
	}
}
/* }}} */

/* {{{ Return filename of current dir entry */
PHP_METHOD(DirectoryIterator, getFilename)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(ZEND_THIS);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_STRING(intern->u.dir.entry.d_name);
}
/* }}} */

/* {{{ Returns file extension component of path */
PHP_METHOD(SplFileInfo, getExtension)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(ZEND_THIS);
	char *fname = NULL;
	const char *p;
	size_t flen;
	size_t path_len;
	size_t idx;
	zend_string *ret;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	if (!intern->file_name) {
		zend_throw_error(NULL, "Object not initialized");
		RETURN_THROWS();
	}

	spl_filesystem_object_get_path(intern, &path_len);

	if (path_len && path_len < intern->file_name_len) {
		fname = intern->file_name + path_len + 1;
		flen = intern->file_name_len - (path_len + 1);
	} else {
		fname = intern->file_name;
		flen = intern->file_name_len;
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
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(ZEND_THIS);
	const char *p;
	size_t idx;
	zend_string *fname;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

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
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(ZEND_THIS);
	char *fname, *suffix = 0;
	size_t flen;
	size_t slen = 0, path_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|s", &suffix, &slen) == FAILURE) {
		RETURN_THROWS();
	}

	if (!intern->file_name) {
		zend_throw_error(NULL, "Object not initialized");
		RETURN_THROWS();
	}

	spl_filesystem_object_get_path(intern, &path_len);

	if (path_len && path_len < intern->file_name_len) {
		fname = intern->file_name + path_len + 1;
		flen = intern->file_name_len - (path_len + 1);
	} else {
		fname = intern->file_name;
		flen = intern->file_name_len;
	}

	RETURN_STR(php_basename(fname, flen, suffix, slen));
}
/* }}}*/

/* {{{ Returns filename component of current dir entry */
PHP_METHOD(DirectoryIterator, getBasename)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(ZEND_THIS);
	char *suffix = 0;
	size_t slen = 0;
	zend_string *fname;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|s", &suffix, &slen) == FAILURE) {
		RETURN_THROWS();
	}

	fname = php_basename(intern->u.dir.entry.d_name, strlen(intern->u.dir.entry.d_name), suffix, slen);

	RETVAL_STR(fname);
}
/* }}} */

/* {{{ Return path and filename */
PHP_METHOD(SplFileInfo, getPathname)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(ZEND_THIS);
	char *path;
	size_t path_len;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	path = spl_filesystem_object_get_pathname(intern, &path_len);
	if (path != NULL) {
		RETURN_STRINGL(path, path_len);
	} else {
		RETURN_EMPTY_STRING();
	}
}
/* }}} */

/* {{{ Return getPathname() or getFilename() depending on flags */
PHP_METHOD(FilesystemIterator, key)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(ZEND_THIS);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	if (SPL_FILE_DIR_KEY(intern, SPL_FILE_DIR_KEY_AS_FILENAME)) {
		RETURN_STRING(intern->u.dir.entry.d_name);
	} else {
		if (spl_filesystem_object_get_file_name(intern) != SUCCESS) {
			RETURN_THROWS();
		}
		RETURN_STRINGL(intern->file_name, intern->file_name_len);
	}
}
/* }}} */

/* {{{ Return getFilename(), getFileInfo() or $this depending on flags */
PHP_METHOD(FilesystemIterator, current)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(ZEND_THIS);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	if (SPL_FILE_DIR_CURRENT(intern, SPL_FILE_DIR_CURRENT_AS_PATHNAME)) {
		if (spl_filesystem_object_get_file_name(intern) != SUCCESS) {
			RETURN_THROWS();
		}
		RETURN_STRINGL(intern->file_name, intern->file_name_len);
	} else if (SPL_FILE_DIR_CURRENT(intern, SPL_FILE_DIR_CURRENT_AS_FILEINFO)) {
		if (spl_filesystem_object_get_file_name(intern) != SUCCESS) {
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
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(ZEND_THIS);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_BOOL(spl_filesystem_is_dot(intern->u.dir.entry.d_name));
}
/* }}} */

/* {{{ Cronstructs a new SplFileInfo from a path. */
/* When the constructor gets called the object is already created
   by the engine, so we must only call 'additional' initializations.
 */
PHP_METHOD(SplFileInfo, __construct)
{
	spl_filesystem_object *intern;
	char *path;
	size_t len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "p", &path, &len) == FAILURE) {
		RETURN_THROWS();
	}

	intern = Z_SPLFILESYSTEM_P(ZEND_THIS);

	spl_filesystem_info_set_filename(intern, path, len, 1);

	/* intern->type = SPL_FS_INFO; already set */
}
/* }}} */

/* {{{ FileInfoFunction */
#define FileInfoFunction(func_name, func_num) \
PHP_METHOD(SplFileInfo, func_name) \
{ \
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(ZEND_THIS); \
	zend_error_handling error_handling; \
	if (zend_parse_parameters_none() == FAILURE) { \
		RETURN_THROWS(); \
	} \
	if (spl_filesystem_object_get_file_name(intern) != SUCCESS) { \
		RETURN_THROWS(); \
	} \
	zend_replace_error_handling(EH_THROW, spl_ce_RuntimeException, &error_handling);\
	php_stat(intern->file_name, intern->file_name_len, func_num, return_value); \
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
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(ZEND_THIS);
	ssize_t ret;
	char buff[MAXPATHLEN];
	zend_error_handling error_handling;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	zend_replace_error_handling(EH_THROW, spl_ce_RuntimeException, &error_handling);

	if (intern->file_name == NULL) {
		if (spl_filesystem_object_get_file_name(intern) != SUCCESS) {
			zend_restore_error_handling(&error_handling);
			RETURN_THROWS();
		}
	}
#if defined(PHP_WIN32) || defined(HAVE_SYMLINK)
	if (intern->file_name == NULL) {
		zend_value_error("Filename cannot be empty");
		RETURN_THROWS();
	}
	if (!IS_ABSOLUTE_PATH(intern->file_name, intern->file_name_len)) {
		char expanded_path[MAXPATHLEN];
		if (!expand_filepath_with_mode(intern->file_name, expanded_path, NULL, 0, CWD_EXPAND )) {
			zend_restore_error_handling(&error_handling);
			php_error_docref(NULL, E_WARNING, "No such file or directory");
			RETURN_FALSE;
		}
		ret = php_sys_readlink(expanded_path, buff, MAXPATHLEN - 1);
	} else {
		ret = php_sys_readlink(intern->file_name, buff,  MAXPATHLEN-1);
	}
#else
	ret = -1; /* always fail if not implemented */
#endif

	if (ret == -1) {
		zend_throw_exception_ex(spl_ce_RuntimeException, 0, "Unable to read link %s, error: %s", intern->file_name, strerror(errno));
		RETVAL_FALSE;
	} else {
		/* Append NULL to the end of the string */
		buff[ret] = '\0';

		RETVAL_STRINGL(buff, ret);
	}

	zend_restore_error_handling(&error_handling);
}
/* }}} */

/* {{{ Return the resolved path */
PHP_METHOD(SplFileInfo, getRealPath)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(ZEND_THIS);
	char buff[MAXPATHLEN];
	char *filename;
	zend_error_handling error_handling;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	zend_replace_error_handling(EH_THROW, spl_ce_RuntimeException, &error_handling);

	if (intern->type == SPL_FS_DIR && !intern->file_name && intern->u.dir.entry.d_name[0]) {
		if (spl_filesystem_object_get_file_name(intern) != SUCCESS) {
			zend_restore_error_handling(&error_handling);
			RETURN_THROWS();
		}
	}

	if (intern->orig_path) {
		filename = intern->orig_path;
	} else {
		filename = intern->file_name;
	}


	if (filename && VCWD_REALPATH(filename, buff)) {
#ifdef ZTS
		if (VCWD_ACCESS(buff, F_OK)) {
			RETVAL_FALSE;
		} else
#endif
		RETVAL_STRING(buff);
	} else {
		RETVAL_FALSE;
	}

	zend_restore_error_handling(&error_handling);
}
/* }}} */

/* {{{ Open the current file */
PHP_METHOD(SplFileInfo, openFile)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(ZEND_THIS);

	spl_filesystem_object_create_type(ZEND_NUM_ARGS(), intern, SPL_FS_FILE, NULL, return_value);
}
/* }}} */

/* {{{ Class to use in openFile() */
PHP_METHOD(SplFileInfo, setFileClass)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(ZEND_THIS);
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
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(ZEND_THIS);
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
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(ZEND_THIS);
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
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(ZEND_THIS);
	zend_class_entry *ce = intern->info_class;
	size_t path_len;
	char *path;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|C!", &ce) == FAILURE) {
		RETURN_THROWS();
	}

	path = spl_filesystem_object_get_pathname(intern, &path_len);
	if (path) {
		char *dpath = estrndup(path, path_len);
		path_len = php_dirname(dpath, path_len);
		spl_filesystem_object_create_info(intern, dpath, path_len, 1, ce, return_value);
		efree(dpath);
	}
}
/* }}} */

/* {{{ */
PHP_METHOD(SplFileInfo, __debugInfo)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_ARR(spl_filesystem_object_get_debug_info(Z_OBJ_P(ZEND_THIS)));
} /* }}} */

/* {{{ */
PHP_METHOD(SplFileInfo, _bad_state_ex)
{
	zend_throw_error(NULL, "The parent constructor was not called: the object is in an invalid state");
}
/* }}} */

/* {{{ Cronstructs a new dir iterator from a path. */
PHP_METHOD(FilesystemIterator, __construct)
{
	spl_filesystem_object_construct(INTERNAL_FUNCTION_PARAM_PASSTHRU, DIT_CTOR_FLAGS | SPL_FILE_DIR_SKIPDOTS);
}
/* }}} */

/* {{{ Rewind dir back to the start */
PHP_METHOD(FilesystemIterator, rewind)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(ZEND_THIS);
	int skip_dots = SPL_HAS_FLAG(intern->flags, SPL_FILE_DIR_SKIPDOTS);

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
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(ZEND_THIS);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_LONG(intern->flags & (SPL_FILE_DIR_KEY_MODE_MASK | SPL_FILE_DIR_CURRENT_MODE_MASK | SPL_FILE_DIR_OTHERS_MASK));
} /* }}} */

/* {{{ Set handling flags */
PHP_METHOD(FilesystemIterator, setFlags)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(ZEND_THIS);
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
	zend_bool allow_links = 0;
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(ZEND_THIS);

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &allow_links) == FAILURE) {
		RETURN_THROWS();
	}
	if (spl_filesystem_is_invalid_or_dot(intern->u.dir.entry.d_name)) {
		RETURN_FALSE;
	} else {
		if (spl_filesystem_object_get_file_name(intern) != SUCCESS) {
			RETURN_THROWS();
		}
		if (!allow_links && !(intern->flags & SPL_FILE_DIR_FOLLOW_SYMLINKS)) {
			php_stat(intern->file_name, intern->file_name_len, FS_IS_LINK, return_value);
			if (zend_is_true(return_value)) {
				RETURN_FALSE;
			}
		}
		php_stat(intern->file_name, intern->file_name_len, FS_IS_DIR, return_value);
    }
}
/* }}} */

/* {{{ Returns an iterator for the current entry if it is a directory */
PHP_METHOD(RecursiveDirectoryIterator, getChildren)
{
	zval zpath, zflags;
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(ZEND_THIS);
	spl_filesystem_object *subdir;
	char slash = SPL_HAS_FLAG(intern->flags, SPL_FILE_DIR_UNIXPATHS) ? '/' : DEFAULT_SLASH;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	if (spl_filesystem_object_get_file_name(intern) != SUCCESS) {
		RETURN_THROWS();
	}

	ZVAL_LONG(&zflags, intern->flags);
	ZVAL_STRINGL(&zpath, intern->file_name, intern->file_name_len);
	spl_instantiate_arg_ex2(Z_OBJCE_P(ZEND_THIS), return_value, &zpath, &zflags);
	zval_ptr_dtor(&zpath);

	subdir = Z_SPLFILESYSTEM_P(return_value);
	if (subdir) {
		if (intern->u.dir.sub_path && intern->u.dir.sub_path[0]) {
			subdir->u.dir.sub_path_len = spprintf(&subdir->u.dir.sub_path, 0, "%s%c%s", intern->u.dir.sub_path, slash, intern->u.dir.entry.d_name);
		} else {
			subdir->u.dir.sub_path_len = strlen(intern->u.dir.entry.d_name);
			subdir->u.dir.sub_path = estrndup(intern->u.dir.entry.d_name, subdir->u.dir.sub_path_len);
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
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(ZEND_THIS);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	if (intern->u.dir.sub_path) {
		RETURN_STRINGL(intern->u.dir.sub_path, intern->u.dir.sub_path_len);
	} else {
		RETURN_EMPTY_STRING();
	}
}
/* }}} */

/* {{{ Get sub path and file name */
PHP_METHOD(RecursiveDirectoryIterator, getSubPathname)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(ZEND_THIS);
	char slash = SPL_HAS_FLAG(intern->flags, SPL_FILE_DIR_UNIXPATHS) ? '/' : DEFAULT_SLASH;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	if (intern->u.dir.sub_path) {
		RETURN_NEW_STR(strpprintf(0, "%s%c%s", intern->u.dir.sub_path, slash, intern->u.dir.entry.d_name));
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
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(ZEND_THIS);

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
	dir_object = Z_SPLFILESYSTEM_P(object);
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
		efree(object->file_name);
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
			if (spl_filesystem_object_get_file_name(object) != SUCCESS) {
				return NULL;
			}
			ZVAL_STRINGL(&iterator->current, object->file_name, object->file_name_len);
		}
		return &iterator->current;
	} else if (SPL_FILE_DIR_CURRENT(object, SPL_FILE_DIR_CURRENT_AS_FILEINFO)) {
		if (Z_ISUNDEF(iterator->current)) {
			if (spl_filesystem_object_get_file_name(object) != SUCCESS) {
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
		if (spl_filesystem_object_get_file_name(object) != SUCCESS) {
			return;
		}
		ZVAL_STRINGL(key, object->file_name, object->file_name_len);
	}
}
/* }}} */

/* {{{ spl_filesystem_tree_it_move_forward */
static void spl_filesystem_tree_it_move_forward(zend_object_iterator *iter)
{
	spl_filesystem_iterator *iterator = (spl_filesystem_iterator *)iter;
	spl_filesystem_object   *object   = spl_filesystem_iterator_to_object(iterator);

	object->u.dir.index++;
	do {
		spl_filesystem_dir_read(object);
	} while (spl_filesystem_is_dot(object->u.dir.entry.d_name));
	if (object->file_name) {
		efree(object->file_name);
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

	object->u.dir.index = 0;
	if (object->u.dir.dirp) {
		php_stream_rewinddir(object->u.dir.dirp);
	}
	do {
		spl_filesystem_dir_read(object);
	} while (spl_filesystem_is_dot(object->u.dir.entry.d_name));
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
	dir_object = Z_SPLFILESYSTEM_P(object);
	iterator = spl_filesystem_object_to_iterator(dir_object);

	ZVAL_OBJ_COPY(&iterator->intern.data, Z_OBJ_P(object));
	iterator->intern.funcs = &spl_filesystem_tree_it_funcs;

	return &iterator->intern;
}
/* }}} */

/* {{{ spl_filesystem_object_cast */
static int spl_filesystem_object_cast(zend_object *readobj, zval *writeobj, int type)
{
	spl_filesystem_object *intern = spl_filesystem_from_obj(readobj);

	if (type == IS_STRING) {
		if (readobj->ce->__tostring) {
			return zend_std_cast_object_tostring(readobj, writeobj, type);
		}

		switch (intern->type) {
		case SPL_FS_INFO:
		case SPL_FS_FILE:
			ZVAL_STRINGL(writeobj, intern->file_name, intern->file_name_len);
			return SUCCESS;
		case SPL_FS_DIR:
			ZVAL_STRING(writeobj, intern->u.dir.entry.d_name);
			return SUCCESS;
		}
	} else if (type == _IS_BOOL) {
		ZVAL_TRUE(writeobj);
		return SUCCESS;
	}
	ZVAL_NULL(writeobj);
	return FAILURE;
}
/* }}} */

static zend_result spl_filesystem_file_read_ex(spl_filesystem_object *intern, bool silent, zend_long line_add) /* {{{ */
{
	char *buf;
	size_t line_len = 0;

	spl_filesystem_file_free_line(intern);

	if (php_stream_eof(intern->u.file.stream)) {
		if (!silent) {
			zend_throw_exception_ex(spl_ce_RuntimeException, 0, "Cannot read from file %s", intern->file_name);
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
		if (SPL_HAS_FLAG(intern->flags, SPL_FILE_OBJECT_DROP_NEW_LINE)) {
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

static inline zend_result spl_filesystem_file_read(spl_filesystem_object *intern, bool silent)
{
	zend_long line_add = (intern->u.file.current_line) ? 1 : 0;
	return spl_filesystem_file_read_ex(intern, silent, line_add);
}

static zend_result spl_filesystem_file_read_csv(spl_filesystem_object *intern, char delimiter, char enclosure, int escape, zval *return_value) /* {{{ */
{
	int ret = SUCCESS;
	zval *value;

	do {
		ret = spl_filesystem_file_read(intern, 1);
	} while (ret == SUCCESS && !intern->u.file.current_line_len && SPL_HAS_FLAG(intern->flags, SPL_FILE_OBJECT_SKIP_EMPTY));

	if (ret == SUCCESS) {
		size_t buf_len = intern->u.file.current_line_len;
		char *buf = estrndup(intern->u.file.current_line, buf_len);

		if (!Z_ISUNDEF(intern->u.file.current_zval)) {
			zval_ptr_dtor(&intern->u.file.current_zval);
			ZVAL_UNDEF(&intern->u.file.current_zval);
		}

		php_fgetcsv(intern->u.file.stream, delimiter, enclosure, escape, buf_len, buf, &intern->u.file.current_zval);
		if (return_value) {
			value = &intern->u.file.current_zval;
			ZVAL_COPY_DEREF(return_value, value);
		}
	}
	return ret;
}
/* }}} */

static int spl_filesystem_file_read_line_ex(zval * this_ptr, spl_filesystem_object *intern, int silent) /* {{{ */
{
	zval retval;

	/* 1) use fgetcsv? 2) overloaded call the function, 3) do it directly */
	if (SPL_HAS_FLAG(intern->flags, SPL_FILE_OBJECT_READ_CSV)) {
		return spl_filesystem_file_read_csv(intern, intern->u.file.delimiter, intern->u.file.enclosure, intern->u.file.escape, NULL);
	}
	if (intern->u.file.func_getCurr->common.scope != spl_ce_SplFileObject) {
		zend_execute_data *execute_data = EG(current_execute_data);
		spl_filesystem_file_free_line(intern);

		if (php_stream_eof(intern->u.file.stream)) {
			if (!silent) {
				zend_throw_exception_ex(spl_ce_RuntimeException, 0, "Cannot read from file %s", intern->file_name);
			}
			return FAILURE;
		}
		zend_call_method_with_0_params(Z_OBJ_P(this_ptr), Z_OBJCE_P(ZEND_THIS), &intern->u.file.func_getCurr, "getCurrentLine", &retval);
		if (!Z_ISUNDEF(retval)) {
			if (intern->u.file.current_line || !Z_ISUNDEF(intern->u.file.current_zval)) {
				intern->u.file.current_line_num++;
			}
			spl_filesystem_file_free_line(intern);
			if (Z_TYPE(retval) == IS_STRING) {
				intern->u.file.current_line = estrndup(Z_STRVAL(retval), Z_STRLEN(retval));
				intern->u.file.current_line_len = Z_STRLEN(retval);
			} else {
				zval *value = &retval;

				ZVAL_COPY_DEREF(&intern->u.file.current_zval, value);
			}
			zval_ptr_dtor(&retval);
			return SUCCESS;
		} else {
			return FAILURE;
		}
	} else {
		return spl_filesystem_file_read(intern, silent);
	}
} /* }}} */

static int spl_filesystem_file_is_empty_line(spl_filesystem_object *intern) /* {{{ */
{
	if (intern->u.file.current_line) {
		return intern->u.file.current_line_len == 0;
	} else if (!Z_ISUNDEF(intern->u.file.current_zval)) {
		switch(Z_TYPE(intern->u.file.current_zval)) {
			case IS_STRING:
				return Z_STRLEN(intern->u.file.current_zval) == 0;
			case IS_ARRAY:
				if (SPL_HAS_FLAG(intern->flags, SPL_FILE_OBJECT_READ_CSV)
						&& zend_hash_num_elements(Z_ARRVAL(intern->u.file.current_zval)) == 1) {
					uint32_t idx = 0;
					zval *first;

					while (Z_ISUNDEF(Z_ARRVAL(intern->u.file.current_zval)->arData[idx].val)) {
						idx++;
					}
					first = &Z_ARRVAL(intern->u.file.current_zval)->arData[idx].val;
					return Z_TYPE_P(first) == IS_STRING && Z_STRLEN_P(first) == 0;
				}
				return zend_hash_num_elements(Z_ARRVAL(intern->u.file.current_zval)) == 0;
			case IS_NULL:
				return 1;
			default:
				return 0;
		}
	} else {
		return 1;
	}
}
/* }}} */

static int spl_filesystem_file_read_line(zval * this_ptr, spl_filesystem_object *intern, int silent) /* {{{ */
{
	int ret = spl_filesystem_file_read_line_ex(this_ptr, intern, silent);

	while (SPL_HAS_FLAG(intern->flags, SPL_FILE_OBJECT_SKIP_EMPTY) && ret == SUCCESS && spl_filesystem_file_is_empty_line(intern)) {
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
		zend_throw_exception_ex(spl_ce_RuntimeException, 0, "Cannot rewind file %s", intern->file_name);
	} else {
		spl_filesystem_file_free_line(intern);
		intern->u.file.current_line_num = 0;
	}
	if (SPL_HAS_FLAG(intern->flags, SPL_FILE_OBJECT_READ_AHEAD)) {
		spl_filesystem_file_read_line(this_ptr, intern, 1);
	}
} /* }}} */

/* {{{ Construct a new file object */
PHP_METHOD(SplFileObject, __construct)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(ZEND_THIS);
	zend_bool use_include_path = 0;
	char *p1, *p2;
	char *tmp_path;
	size_t   tmp_path_len;
	zend_error_handling error_handling;

	intern->u.file.open_mode = NULL;
	intern->u.file.open_mode_len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "p|sbr!",
			&intern->file_name, &intern->file_name_len,
			&intern->u.file.open_mode, &intern->u.file.open_mode_len,
			&use_include_path, &intern->u.file.zcontext) == FAILURE) {
		intern->u.file.open_mode = NULL;
		intern->file_name = NULL;
		RETURN_THROWS();
	}

	if (intern->u.file.open_mode == NULL) {
		intern->u.file.open_mode = "r";
		intern->u.file.open_mode_len = 1;
	}

	zend_replace_error_handling(EH_THROW, spl_ce_RuntimeException, &error_handling);

	if (spl_filesystem_file_open(intern, use_include_path, 0) == SUCCESS) {
		tmp_path_len = strlen(intern->u.file.stream->orig_path);

		if (tmp_path_len > 1 && IS_SLASH_AT(intern->u.file.stream->orig_path, tmp_path_len-1)) {
			tmp_path_len--;
		}

		tmp_path = estrndup(intern->u.file.stream->orig_path, tmp_path_len);

		p1 = strrchr(tmp_path, '/');
#ifdef PHP_WIN32
		p2 = strrchr(tmp_path, '\\');
#else
		p2 = 0;
#endif
		if (p1 || p2) {
			intern->_path_len = ((p1 > p2 ? p1 : p2) - tmp_path);
		} else {
			intern->_path_len = 0;
		}

		efree(tmp_path);

		intern->_path = estrndup(intern->u.file.stream->orig_path, intern->_path_len);
	}

	zend_restore_error_handling(&error_handling);

} /* }}} */

/* {{{ Construct a new temp file object */
PHP_METHOD(SplTempFileObject, __construct)
{
	zend_long max_memory = PHP_STREAM_MAX_MEM;
	char tmp_fname[48];
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(ZEND_THIS);
	zend_error_handling error_handling;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|l", &max_memory) == FAILURE) {
		RETURN_THROWS();
	}

	if (max_memory < 0) {
		intern->file_name = "php://memory";
		intern->file_name_len = 12;
	} else if (ZEND_NUM_ARGS()) {
		intern->file_name_len = slprintf(tmp_fname, sizeof(tmp_fname), "php://temp/maxmemory:" ZEND_LONG_FMT, max_memory);
		intern->file_name = tmp_fname;
	} else {
		intern->file_name = "php://temp";
		intern->file_name_len = 10;
	}
	intern->u.file.open_mode = "wb";
	intern->u.file.open_mode_len = 1;

	zend_replace_error_handling(EH_THROW, spl_ce_RuntimeException, &error_handling);
	if (spl_filesystem_file_open(intern, 0, 0) == SUCCESS) {
		intern->_path_len = 0;
		intern->_path = estrndup("", 0);
	}
	zend_restore_error_handling(&error_handling);
} /* }}} */

/* {{{ Rewind the file and read the first line */
PHP_METHOD(SplFileObject, rewind)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(ZEND_THIS);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	spl_filesystem_file_rewind(ZEND_THIS, intern);
} /* }}} */

/* {{{ Return whether end of file is reached */
PHP_METHOD(SplFileObject, eof)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(ZEND_THIS);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	CHECK_SPL_FILE_OBJECT_IS_INITIALIZED(intern);

	RETURN_BOOL(php_stream_eof(intern->u.file.stream));
} /* }}} */

/* {{{ Return !eof() */
PHP_METHOD(SplFileObject, valid)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(ZEND_THIS);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	if (SPL_HAS_FLAG(intern->flags, SPL_FILE_OBJECT_READ_AHEAD)) {
		RETURN_BOOL(intern->u.file.current_line || !Z_ISUNDEF(intern->u.file.current_zval));
	} else {
		if(!intern->u.file.stream) {
			RETURN_FALSE;
		}
		RETVAL_BOOL(!php_stream_eof(intern->u.file.stream));
	}
} /* }}} */

/* {{{ Return next line from file */
PHP_METHOD(SplFileObject, fgets)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(ZEND_THIS);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	CHECK_SPL_FILE_OBJECT_IS_INITIALIZED(intern);

	if (spl_filesystem_file_read_ex(intern, /* silent */ false, /* line_add */ 1) == FAILURE) {
		RETURN_THROWS();
	}
	RETURN_STRINGL(intern->u.file.current_line, intern->u.file.current_line_len);
} /* }}} */

/* {{{ Return current line from file */
PHP_METHOD(SplFileObject, current)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(ZEND_THIS);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	CHECK_SPL_FILE_OBJECT_IS_INITIALIZED(intern);

	if (!intern->u.file.current_line && Z_ISUNDEF(intern->u.file.current_zval)) {
		spl_filesystem_file_read_line(ZEND_THIS, intern, 1);
	}
	if (intern->u.file.current_line && (!SPL_HAS_FLAG(intern->flags, SPL_FILE_OBJECT_READ_CSV) || Z_ISUNDEF(intern->u.file.current_zval))) {
		RETURN_STRINGL(intern->u.file.current_line, intern->u.file.current_line_len);
	} else if (!Z_ISUNDEF(intern->u.file.current_zval)) {
		zval *value = &intern->u.file.current_zval;

		ZVAL_COPY_DEREF(return_value, value);
		return;
	}
	RETURN_FALSE;
} /* }}} */

/* {{{ Return line number */
PHP_METHOD(SplFileObject, key)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(ZEND_THIS);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	/* Do not read the next line to support correct counting with fgetc()
	if (!intern->u.file.current_line) {
		spl_filesystem_file_read_line(ZEND_THIS, intern, 1);
	} */
	RETURN_LONG(intern->u.file.current_line_num);
} /* }}} */

/* {{{ Read next line */
PHP_METHOD(SplFileObject, next)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(ZEND_THIS);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	spl_filesystem_file_free_line(intern);
	if (SPL_HAS_FLAG(intern->flags, SPL_FILE_OBJECT_READ_AHEAD)) {
		spl_filesystem_file_read_line(ZEND_THIS, intern, 1);
	}
	intern->u.file.current_line_num++;
} /* }}} */

/* {{{ Set file handling flags */
PHP_METHOD(SplFileObject, setFlags)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(ZEND_THIS);

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &intern->flags) == FAILURE) {
		RETURN_THROWS();
	}
} /* }}} */

/* {{{ Get file handling flags */
PHP_METHOD(SplFileObject, getFlags)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(ZEND_THIS);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_LONG(intern->flags & SPL_FILE_OBJECT_MASK);
} /* }}} */

/* {{{ Set maximum line length */
PHP_METHOD(SplFileObject, setMaxLineLen)
{
	zend_long max_len;

	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(ZEND_THIS);

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
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(ZEND_THIS);

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

/* {{{ Return current line as csv */
PHP_METHOD(SplFileObject, fgetcsv)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(ZEND_THIS);
	char delimiter = intern->u.file.delimiter, enclosure = intern->u.file.enclosure;
	int escape = intern->u.file.escape;
	char *delim = NULL, *enclo = NULL, *esc = NULL;
	size_t d_len = 0, e_len = 0, esc_len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|sss", &delim, &d_len, &enclo, &e_len, &esc, &esc_len) == SUCCESS) {

		CHECK_SPL_FILE_OBJECT_IS_INITIALIZED(intern);

		switch(ZEND_NUM_ARGS())
		{
		case 3:
			if (esc_len > 1) {
				zend_argument_value_error(3, "must be empty or a single character");
				RETURN_THROWS();
			}
			if (esc_len == 0) {
				escape = PHP_CSV_NO_ESCAPE;
			} else {
				escape = (unsigned char) esc[0];
			}
			/* no break */
		case 2:
			if (e_len != 1) {
				zend_argument_value_error(2, "must be a single character");
				RETURN_THROWS();
			}
			enclosure = enclo[0];
			/* no break */
		case 1:
			if (d_len != 1) {
				zend_argument_value_error(1, "must be a single character");
				RETURN_THROWS();
			}
			delimiter = delim[0];
			/* no break */
		case 0:
			break;
		}
		spl_filesystem_file_read_csv(intern, delimiter, enclosure, escape, return_value);
	}
}
/* }}} */

/* {{{ Output a field array as a CSV line */
PHP_METHOD(SplFileObject, fputcsv)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(ZEND_THIS);
	char delimiter = intern->u.file.delimiter, enclosure = intern->u.file.enclosure;
	int escape = intern->u.file.escape;
	char *delim = NULL, *enclo = NULL, *esc = NULL;
	size_t d_len = 0, e_len = 0, esc_len = 0;
	zend_long ret;
	zval *fields = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "a|sss", &fields, &delim, &d_len, &enclo, &e_len, &esc, &esc_len) == SUCCESS) {

		switch(ZEND_NUM_ARGS())
		{
		case 4:
			switch (esc_len) {
				case 0:
					escape = PHP_CSV_NO_ESCAPE;
					break;
				case 1:
					escape = (unsigned char) esc[0];
					break;
				default:
					zend_argument_value_error(4, "must be empty or a single character");
					RETURN_THROWS();
			}
			/* no break */
		case 3:
			if (e_len != 1) {
				zend_argument_value_error(3, "must be a single character");
				RETURN_THROWS();
			}
			enclosure = enclo[0];
			/* no break */
		case 2:
			if (d_len != 1) {
				zend_argument_value_error(2, "must be a single character");
				RETURN_THROWS();
			}
			delimiter = delim[0];
			/* no break */
		case 1:
		case 0:
			break;
		}
		ret = php_fputcsv(intern->u.file.stream, fields, delimiter, enclosure, escape);
		if (ret < 0) {
			RETURN_FALSE;
		}
		RETURN_LONG(ret);
	}
}
/* }}} */

/* {{{ Set the delimiter, enclosure and escape character used in fgetcsv */
PHP_METHOD(SplFileObject, setCsvControl)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(ZEND_THIS);
	char delimiter = ',', enclosure = '"';
	int escape = (unsigned char) '\\';
	char *delim = NULL, *enclo = NULL, *esc = NULL;
	size_t d_len = 0, e_len = 0, esc_len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|sss", &delim, &d_len, &enclo, &e_len, &esc, &esc_len) == SUCCESS) {
		switch(ZEND_NUM_ARGS())
		{
		case 3:
			switch (esc_len) {
				case 0:
					escape = PHP_CSV_NO_ESCAPE;
					break;
				case 1:
					escape = (unsigned char) esc[0];
					break;
				default:
					zend_argument_value_error(3, "must be empty or a single character");
					RETURN_THROWS();
			}
			/* no break */
		case 2:
			if (e_len != 1) {
				zend_argument_value_error(2, "must be a single character");
				RETURN_THROWS();
			}
			enclosure = enclo[0];
			/* no break */
		case 1:
			if (d_len != 1) {
				zend_argument_value_error(1, "must be a single character");
				RETURN_THROWS();
			}
			delimiter = delim[0];
			/* no break */
		case 0:
			break;
		}
		intern->u.file.delimiter = delimiter;
		intern->u.file.enclosure = enclosure;
		intern->u.file.escape    = escape;
	}
}
/* }}} */

/* {{{ Get the delimiter, enclosure and escape character used in fgetcsv */
PHP_METHOD(SplFileObject, getCsvControl)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(ZEND_THIS);
	char delimiter[2], enclosure[2], escape[2];

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
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(ZEND_THIS);
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
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(ZEND_THIS);

	CHECK_SPL_FILE_OBJECT_IS_INITIALIZED(intern);

	RETURN_BOOL(!php_stream_flush(intern->u.file.stream));
} /* }}} */

/* {{{ Return current file position */
PHP_METHOD(SplFileObject, ftell)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(ZEND_THIS);
	zend_long ret;

	CHECK_SPL_FILE_OBJECT_IS_INITIALIZED(intern);

	ret = php_stream_tell(intern->u.file.stream);

	if (ret == -1) {
		RETURN_FALSE;
	} else {
		RETURN_LONG(ret);
	}
} /* }}} */

/* {{{ Return current file position */
PHP_METHOD(SplFileObject, fseek)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(ZEND_THIS);
	zend_long pos, whence = SEEK_SET;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l|l", &pos, &whence) == FAILURE) {
		RETURN_THROWS();
	}

	CHECK_SPL_FILE_OBJECT_IS_INITIALIZED(intern);

	spl_filesystem_file_free_line(intern);
	RETURN_LONG(php_stream_seek(intern->u.file.stream, pos, (int)whence));
} /* }}} */

/* {{{ Get a character form the file */
PHP_METHOD(SplFileObject, fgetc)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(ZEND_THIS);
	char buf[2];
	int result;

	CHECK_SPL_FILE_OBJECT_IS_INITIALIZED(intern);

	spl_filesystem_file_free_line(intern);

	result = php_stream_getc(intern->u.file.stream);

	if (result == EOF) {
		RETVAL_FALSE;
	} else {
		if (result == '\n') {
			intern->u.file.current_line_num++;
		}
		buf[0] = result;
		buf[1] = '\0';

		RETURN_STRINGL(buf, 1);
	}
} /* }}} */

/* {{{ Output all remaining data from a file pointer */
PHP_METHOD(SplFileObject, fpassthru)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(ZEND_THIS);

	CHECK_SPL_FILE_OBJECT_IS_INITIALIZED(intern);

	RETURN_LONG(php_stream_passthru(intern->u.file.stream));
} /* }}} */

/* {{{ Implements a mostly ANSI compatible fscanf() */
PHP_METHOD(SplFileObject, fscanf)
{
	int result, num_varargs = 0;
	zend_string *format_str;
	zval *varargs= NULL;
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(ZEND_THIS);

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S*", &format_str, &varargs, &num_varargs) == FAILURE) {
		RETURN_THROWS();
	}

	CHECK_SPL_FILE_OBJECT_IS_INITIALIZED(intern);

	/* Get next line */
	if (spl_filesystem_file_read(intern, 0) == FAILURE) {
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
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(ZEND_THIS);
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
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(ZEND_THIS);
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
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(ZEND_THIS);

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
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(ZEND_THIS);
	zend_long size;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &size) == FAILURE) {
		RETURN_THROWS();
	}

	CHECK_SPL_FILE_OBJECT_IS_INITIALIZED(intern);

	if (!php_stream_truncate_supported(intern->u.file.stream)) {
		zend_throw_exception_ex(spl_ce_LogicException, 0, "Can't truncate file %s", intern->file_name);
		RETURN_THROWS();
	}

	RETURN_BOOL(0 == php_stream_truncate_set_size(intern->u.file.stream, size));
} /* }}} */

/* {{{ Seek to specified line */
PHP_METHOD(SplFileObject, seek)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(ZEND_THIS);
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
		if (spl_filesystem_file_read_line(ZEND_THIS, intern, 1) == FAILURE) {
			return;
		}
	}
	if (line_pos > 0) {
		if (!SPL_HAS_FLAG(intern->flags, SPL_FILE_OBJECT_READ_AHEAD)) {
			intern->u.file.current_line_num++;
			spl_filesystem_file_free_line(intern);
		}
	}
} /* }}} */

/* {{{ PHP_MINIT_FUNCTION(spl_directory) */
PHP_MINIT_FUNCTION(spl_directory)
{
	REGISTER_SPL_STD_CLASS_EX(SplFileInfo, spl_filesystem_object_new, class_SplFileInfo_methods);
	memcpy(&spl_filesystem_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	spl_filesystem_object_handlers.offset = XtOffsetOf(spl_filesystem_object, std);
	spl_filesystem_object_handlers.clone_obj = spl_filesystem_object_clone;
	spl_filesystem_object_handlers.cast_object = spl_filesystem_object_cast;
	spl_filesystem_object_handlers.dtor_obj = spl_filesystem_object_destroy_object;
	spl_filesystem_object_handlers.free_obj = spl_filesystem_object_free_storage;
	spl_ce_SplFileInfo->serialize = zend_class_serialize_deny;
	spl_ce_SplFileInfo->unserialize = zend_class_unserialize_deny;
	REGISTER_SPL_IMPLEMENTS(SplFileInfo, Stringable);


	REGISTER_SPL_SUB_CLASS_EX(DirectoryIterator, SplFileInfo, spl_filesystem_object_new, class_DirectoryIterator_methods);
	zend_class_implements(spl_ce_DirectoryIterator, 1, zend_ce_iterator);
	REGISTER_SPL_IMPLEMENTS(DirectoryIterator, SeekableIterator);

	spl_ce_DirectoryIterator->get_iterator = spl_filesystem_dir_get_iterator;

	REGISTER_SPL_SUB_CLASS_EX(FilesystemIterator, DirectoryIterator, spl_filesystem_object_new, class_FilesystemIterator_methods);

	REGISTER_SPL_CLASS_CONST_LONG(FilesystemIterator, "CURRENT_MODE_MASK",   SPL_FILE_DIR_CURRENT_MODE_MASK);
	REGISTER_SPL_CLASS_CONST_LONG(FilesystemIterator, "CURRENT_AS_PATHNAME", SPL_FILE_DIR_CURRENT_AS_PATHNAME);
	REGISTER_SPL_CLASS_CONST_LONG(FilesystemIterator, "CURRENT_AS_FILEINFO", SPL_FILE_DIR_CURRENT_AS_FILEINFO);
	REGISTER_SPL_CLASS_CONST_LONG(FilesystemIterator, "CURRENT_AS_SELF",     SPL_FILE_DIR_CURRENT_AS_SELF);
	REGISTER_SPL_CLASS_CONST_LONG(FilesystemIterator, "KEY_MODE_MASK",       SPL_FILE_DIR_KEY_MODE_MASK);
	REGISTER_SPL_CLASS_CONST_LONG(FilesystemIterator, "KEY_AS_PATHNAME",     SPL_FILE_DIR_KEY_AS_PATHNAME);
	REGISTER_SPL_CLASS_CONST_LONG(FilesystemIterator, "FOLLOW_SYMLINKS",     SPL_FILE_DIR_FOLLOW_SYMLINKS);
	REGISTER_SPL_CLASS_CONST_LONG(FilesystemIterator, "KEY_AS_FILENAME",     SPL_FILE_DIR_KEY_AS_FILENAME);
	REGISTER_SPL_CLASS_CONST_LONG(FilesystemIterator, "NEW_CURRENT_AND_KEY", SPL_FILE_DIR_KEY_AS_FILENAME|SPL_FILE_DIR_CURRENT_AS_FILEINFO);
	REGISTER_SPL_CLASS_CONST_LONG(FilesystemIterator, "OTHER_MODE_MASK",     SPL_FILE_DIR_OTHERS_MASK);
	REGISTER_SPL_CLASS_CONST_LONG(FilesystemIterator, "SKIP_DOTS",           SPL_FILE_DIR_SKIPDOTS);
	REGISTER_SPL_CLASS_CONST_LONG(FilesystemIterator, "UNIX_PATHS",          SPL_FILE_DIR_UNIXPATHS);

	spl_ce_FilesystemIterator->get_iterator = spl_filesystem_tree_get_iterator;

	REGISTER_SPL_SUB_CLASS_EX(RecursiveDirectoryIterator, FilesystemIterator, spl_filesystem_object_new, class_RecursiveDirectoryIterator_methods);
	REGISTER_SPL_IMPLEMENTS(RecursiveDirectoryIterator, RecursiveIterator);

	memcpy(&spl_filesystem_object_check_handlers, &spl_filesystem_object_handlers, sizeof(zend_object_handlers));
	spl_filesystem_object_check_handlers.clone_obj = NULL;
	spl_filesystem_object_check_handlers.get_method = spl_filesystem_object_get_method_check;

#ifdef HAVE_GLOB
	REGISTER_SPL_SUB_CLASS_EX(GlobIterator, FilesystemIterator, spl_filesystem_object_new_check, class_GlobIterator_methods);
	REGISTER_SPL_IMPLEMENTS(GlobIterator, Countable);
#endif

	REGISTER_SPL_SUB_CLASS_EX(SplFileObject, SplFileInfo, spl_filesystem_object_new_check, class_SplFileObject_methods);
	REGISTER_SPL_IMPLEMENTS(SplFileObject, RecursiveIterator);
	REGISTER_SPL_IMPLEMENTS(SplFileObject, SeekableIterator);

	REGISTER_SPL_CLASS_CONST_LONG(SplFileObject, "DROP_NEW_LINE", SPL_FILE_OBJECT_DROP_NEW_LINE);
	REGISTER_SPL_CLASS_CONST_LONG(SplFileObject, "READ_AHEAD",    SPL_FILE_OBJECT_READ_AHEAD);
	REGISTER_SPL_CLASS_CONST_LONG(SplFileObject, "SKIP_EMPTY",    SPL_FILE_OBJECT_SKIP_EMPTY);
	REGISTER_SPL_CLASS_CONST_LONG(SplFileObject, "READ_CSV",      SPL_FILE_OBJECT_READ_CSV);

	REGISTER_SPL_SUB_CLASS_EX(SplTempFileObject, SplFileObject, spl_filesystem_object_new_check, class_SplTempFileObject_methods);
	return SUCCESS;
}
/* }}} */
