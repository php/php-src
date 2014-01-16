/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2014 The PHP Group                                |
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

/* $Id$ */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "ext/standard/file.h"
#include "ext/standard/php_string.h"
#include "zend_compile.h"
#include "zend_exceptions.h"
#include "zend_interfaces.h"

#include "php_spl.h"
#include "spl_functions.h"
#include "spl_engine.h"
#include "spl_iterators.h"
#include "spl_directory.h"
#include "spl_exceptions.h"

#include "php.h"
#include "fopen_wrappers.h"

#include "ext/standard/basic_functions.h"
#include "ext/standard/php_filestat.h"

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

static void spl_filesystem_file_free_line(spl_filesystem_object *intern TSRMLS_DC) /* {{{ */
{
	if (intern->u.file.current_line) {
		efree(intern->u.file.current_line);
		intern->u.file.current_line = NULL;
	}
	if (intern->u.file.current_zval) {
		zval_ptr_dtor(&intern->u.file.current_zval);
		intern->u.file.current_zval = NULL;
	}
} /* }}} */

static void spl_filesystem_object_free_storage(void *object TSRMLS_DC) /* {{{ */
{
	spl_filesystem_object *intern = (spl_filesystem_object*)object;

	if (intern->oth_handler && intern->oth_handler->dtor) {
		intern->oth_handler->dtor(intern TSRMLS_CC);
	}
	
	zend_object_std_dtor(&intern->std TSRMLS_CC);
	
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
		if (intern->u.dir.dirp) {
			php_stream_close(intern->u.dir.dirp);
			intern->u.dir.dirp = NULL;
		}
		if (intern->u.dir.sub_path) {
			efree(intern->u.dir.sub_path);
		}		
		break;
	case SPL_FS_FILE:
		if (intern->u.file.stream) {
			if (intern->u.file.zcontext) {
/*				zend_list_delref(Z_RESVAL_P(intern->zcontext));*/
			}
			if (!intern->u.file.stream->is_persistent) {
				php_stream_free(intern->u.file.stream, PHP_STREAM_FREE_CLOSE);
			} else {
				php_stream_free(intern->u.file.stream, PHP_STREAM_FREE_CLOSE_PERSISTENT);
			}
			if (intern->u.file.open_mode) {
				efree(intern->u.file.open_mode);
			}
			if (intern->orig_path) {
				efree(intern->orig_path);
			}
		}
		spl_filesystem_file_free_line(intern TSRMLS_CC);
		break;
	}

	{
		zend_object_iterator *iterator;
		iterator = (zend_object_iterator*)
				spl_filesystem_object_to_iterator(intern);
		if (iterator->data != NULL) {
			iterator->data = NULL;
			iterator->funcs->dtor(iterator TSRMLS_CC);
		}
	}
	efree(object);
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
static zend_object_value spl_filesystem_object_new_ex(zend_class_entry *class_type, spl_filesystem_object **obj TSRMLS_DC)
{
	zend_object_value retval;
	spl_filesystem_object *intern;

	intern = emalloc(sizeof(spl_filesystem_object));
	memset(intern, 0, sizeof(spl_filesystem_object));
	/* intern->type = SPL_FS_INFO; done by set 0 */
	intern->file_class = spl_ce_SplFileObject;
	intern->info_class = spl_ce_SplFileInfo;
	if (obj) *obj = intern;

	zend_object_std_init(&intern->std, class_type TSRMLS_CC);
	object_properties_init(&intern->std, class_type);

	retval.handle = zend_objects_store_put(intern, (zend_objects_store_dtor_t) zend_objects_destroy_object, (zend_objects_free_object_storage_t) spl_filesystem_object_free_storage, NULL TSRMLS_CC);
	retval.handlers = &spl_filesystem_object_handlers;
	return retval;
}
/* }}} */

/* {{{ spl_filesystem_object_new */
/* See spl_filesystem_object_new_ex */
static zend_object_value spl_filesystem_object_new(zend_class_entry *class_type TSRMLS_DC)
{
	return spl_filesystem_object_new_ex(class_type, NULL TSRMLS_CC);
}
/* }}} */

/* {{{ spl_filesystem_object_new_ex */
static zend_object_value spl_filesystem_object_new_check(zend_class_entry *class_type TSRMLS_DC)
{
	zend_object_value ret = spl_filesystem_object_new_ex(class_type, NULL TSRMLS_CC);
	ret.handlers = &spl_filesystem_object_check_handlers;
	return ret;
}
/* }}} */


PHPAPI char* spl_filesystem_object_get_path(spl_filesystem_object *intern, int *len TSRMLS_DC) /* {{{ */
{
#ifdef HAVE_GLOB
	if (intern->type == SPL_FS_DIR) {
		if (php_stream_is(intern->u.dir.dirp ,&php_glob_stream_ops)) {
			return php_glob_stream_get_path(intern->u.dir.dirp, 0, len);
		}
	}
#endif
	if (len) {
		*len = intern->_path_len;
	}
	return intern->_path;
} /* }}} */

static inline void spl_filesystem_object_get_file_name(spl_filesystem_object *intern TSRMLS_DC) /* {{{ */
{
	char slash = SPL_HAS_FLAG(intern->flags, SPL_FILE_DIR_UNIXPATHS) ? '/' : DEFAULT_SLASH;

	if (!intern->file_name) {
		switch (intern->type) {
		case SPL_FS_INFO:
		case SPL_FS_FILE:
			php_error_docref(NULL TSRMLS_CC, E_ERROR, "Object not initialized");
			break;
		case SPL_FS_DIR:
			intern->file_name_len = spprintf(&intern->file_name, 0, "%s%c%s",
			                                 spl_filesystem_object_get_path(intern, NULL TSRMLS_CC),
			                                 slash, intern->u.dir.entry.d_name);
			break;
		}
	}
} /* }}} */

static int spl_filesystem_dir_read(spl_filesystem_object *intern TSRMLS_DC) /* {{{ */
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
static void spl_filesystem_dir_open(spl_filesystem_object* intern, char *path TSRMLS_DC)
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
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0
				TSRMLS_CC, "Failed to open directory \"%s\"", path);
		}
	} else {
		do {
			spl_filesystem_dir_read(intern TSRMLS_CC);
		} while (skip_dots && spl_filesystem_is_dot(intern->u.dir.entry.d_name));
	}
}
/* }}} */

static int spl_filesystem_file_open(spl_filesystem_object *intern, int use_include_path, int silent TSRMLS_DC) /* {{{ */
{
	zval  tmp;

	intern->type = SPL_FS_FILE;

	php_stat(intern->file_name, intern->file_name_len, FS_IS_DIR, &tmp TSRMLS_CC);
	if (Z_LVAL(tmp)) {
		intern->u.file.open_mode = NULL;
		intern->file_name = NULL;
		zend_throw_exception_ex(spl_ce_LogicException, 0 TSRMLS_CC, "Cannot use SplFileObject with directories");
		return FAILURE;
	}

	intern->u.file.context = php_stream_context_from_zval(intern->u.file.zcontext, 0);
	intern->u.file.stream = php_stream_open_wrapper_ex(intern->file_name, intern->u.file.open_mode, (use_include_path ? USE_PATH : 0) | REPORT_ERRORS, NULL, intern->u.file.context);

	if (!intern->file_name_len || !intern->u.file.stream) {
		if (!EG(exception)) {
			zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC, "Cannot open file '%s'", intern->file_name_len ? intern->file_name : "");
		}
		intern->file_name = NULL; /* until here it is not a copy */
		intern->u.file.open_mode = NULL;
		return FAILURE;
	}

	if (intern->u.file.zcontext) {
		zend_list_addref(Z_RESVAL_P(intern->u.file.zcontext));
	}

	if (intern->file_name_len > 1 && IS_SLASH_AT(intern->file_name, intern->file_name_len-1)) {
		intern->file_name_len--;
	}

	intern->orig_path = estrndup(intern->u.file.stream->orig_path, strlen(intern->u.file.stream->orig_path));

	intern->file_name = estrndup(intern->file_name, intern->file_name_len);
	intern->u.file.open_mode = estrndup(intern->u.file.open_mode, intern->u.file.open_mode_len);

	/* avoid reference counting in debug mode, thus do it manually */
	ZVAL_RESOURCE(&intern->u.file.zresource, php_stream_get_resource_id(intern->u.file.stream));
	Z_SET_REFCOUNT(intern->u.file.zresource, 1);
	
	intern->u.file.delimiter = ',';
	intern->u.file.enclosure = '"';
	intern->u.file.escape = '\\';

	zend_hash_find(&intern->std.ce->function_table, "getcurrentline", sizeof("getcurrentline"), (void **) &intern->u.file.func_getCurr);

	return SUCCESS;
} /* }}} */

/* {{{ spl_filesystem_object_clone */
/* Local zend_object_value creation (on stack)
   Load the 'other' object 
   Create a new empty object (See spl_filesystem_object_new_ex)
   Open the directory
   Clone other members (properties)
 */
static zend_object_value spl_filesystem_object_clone(zval *zobject TSRMLS_DC)
{
	zend_object_value new_obj_val;
	zend_object *old_object;
	zend_object *new_object;
	zend_object_handle handle = Z_OBJ_HANDLE_P(zobject);
	spl_filesystem_object *intern;
	spl_filesystem_object *source;
	int index, skip_dots;

	old_object = zend_objects_get_address(zobject TSRMLS_CC);
	source = (spl_filesystem_object*)old_object;

	new_obj_val = spl_filesystem_object_new_ex(old_object->ce, &intern TSRMLS_CC);
	new_object = &intern->std;

	intern->flags = source->flags;

	switch (source->type) {
	case SPL_FS_INFO:
		intern->_path_len = source->_path_len;
		intern->_path = estrndup(source->_path, source->_path_len);
		intern->file_name_len = source->file_name_len;
		intern->file_name = estrndup(source->file_name, intern->file_name_len);
		break;
	case SPL_FS_DIR:
		spl_filesystem_dir_open(intern, source->_path TSRMLS_CC);
		/* read until we hit the position in which we were before */
		skip_dots = SPL_HAS_FLAG(source->flags, SPL_FILE_DIR_SKIPDOTS);
		for(index = 0; index < source->u.dir.index; ++index) {
			do {
				spl_filesystem_dir_read(intern TSRMLS_CC);
			} while (skip_dots && spl_filesystem_is_dot(intern->u.dir.entry.d_name));
		}
		intern->u.dir.index = index;
		break;
	case SPL_FS_FILE:
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "An object of class %s cannot be cloned", old_object->ce->name);
		break;
	}
	
	intern->file_class = source->file_class;
	intern->info_class = source->info_class;
	intern->oth = source->oth;
	intern->oth_handler = source->oth_handler;

	zend_objects_clone_members(new_object, new_obj_val, old_object, handle TSRMLS_CC);

	if (intern->oth_handler && intern->oth_handler->clone) {
		intern->oth_handler->clone(source, intern TSRMLS_CC);
	}

	return new_obj_val;
}
/* }}} */

void spl_filesystem_info_set_filename(spl_filesystem_object *intern, char *path, int len, int use_copy TSRMLS_DC) /* {{{ */
{
	char *p1, *p2;
	
	if (intern->file_name) {
		efree(intern->file_name);
	}

	intern->file_name = use_copy ? estrndup(path, len) : path;
	intern->file_name_len = len;

	while(IS_SLASH_AT(intern->file_name, intern->file_name_len-1) && intern->file_name_len > 1) {
		intern->file_name[intern->file_name_len-1] = 0;
		intern->file_name_len--;
	}

	p1 = strrchr(intern->file_name, '/');
#if defined(PHP_WIN32) || defined(NETWARE)
	p2 = strrchr(intern->file_name, '\\');
#else
	p2 = 0;
#endif
	if (p1 || p2) {
		intern->_path_len = (p1 > p2 ? p1 : p2) - intern->file_name;
	} else {
		intern->_path_len = 0;
	}
	
	if (intern->_path) {
		efree(intern->_path);
	}
	intern->_path = estrndup(path, intern->_path_len);
} /* }}} */

static spl_filesystem_object * spl_filesystem_object_create_info(spl_filesystem_object *source, char *file_path, int file_path_len, int use_copy, zend_class_entry *ce, zval *return_value TSRMLS_DC) /* {{{ */
{
	spl_filesystem_object *intern;
	zval *arg1;
	zend_error_handling error_handling;

	if (!file_path || !file_path_len) {
#if defined(PHP_WIN32)
		zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC, "Cannot create SplFileInfo for empty path");
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

	zend_replace_error_handling(EH_THROW, spl_ce_RuntimeException, &error_handling TSRMLS_CC);

	ce = ce ? ce : source->info_class;

	zend_update_class_constants(ce TSRMLS_CC);

	return_value->value.obj = spl_filesystem_object_new_ex(ce, &intern TSRMLS_CC);
	Z_TYPE_P(return_value) = IS_OBJECT;

	if (ce->constructor->common.scope != spl_ce_SplFileInfo) {
		MAKE_STD_ZVAL(arg1);
		ZVAL_STRINGL(arg1, file_path, file_path_len, use_copy);
		zend_call_method_with_1_params(&return_value, ce, &ce->constructor, "__construct", NULL, arg1);
		zval_ptr_dtor(&arg1);
	} else {
		spl_filesystem_info_set_filename(intern, file_path, file_path_len, use_copy TSRMLS_CC);
	}
	
	zend_restore_error_handling(&error_handling TSRMLS_CC);
	return intern;
} /* }}} */

static spl_filesystem_object * spl_filesystem_object_create_type(int ht, spl_filesystem_object *source, int type, zend_class_entry *ce, zval *return_value TSRMLS_DC) /* {{{ */
{
	spl_filesystem_object *intern;
	zend_bool use_include_path = 0;
	zval *arg1, *arg2;
	zend_error_handling error_handling;

	zend_replace_error_handling(EH_THROW, spl_ce_RuntimeException, &error_handling TSRMLS_CC);

	switch (source->type) {
	case SPL_FS_INFO:
	case SPL_FS_FILE:
		break;
	case SPL_FS_DIR:
		if (!source->u.dir.entry.d_name[0]) {
			zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC, "Could not open file");
			zend_restore_error_handling(&error_handling TSRMLS_CC);
			return NULL;
		}
	}

	switch (type) {
	case SPL_FS_INFO:
		ce = ce ? ce : source->info_class;

		zend_update_class_constants(ce TSRMLS_CC);

		return_value->value.obj = spl_filesystem_object_new_ex(ce, &intern TSRMLS_CC);
		Z_TYPE_P(return_value) = IS_OBJECT;

		spl_filesystem_object_get_file_name(source TSRMLS_CC);
		if (ce->constructor->common.scope != spl_ce_SplFileInfo) {
			MAKE_STD_ZVAL(arg1);
			ZVAL_STRINGL(arg1, source->file_name, source->file_name_len, 1);
			zend_call_method_with_1_params(&return_value, ce, &ce->constructor, "__construct", NULL, arg1);
			zval_ptr_dtor(&arg1);
		} else {
			intern->file_name = estrndup(source->file_name, source->file_name_len);
			intern->file_name_len = source->file_name_len;
			intern->_path = spl_filesystem_object_get_path(source, &intern->_path_len TSRMLS_CC);
			intern->_path = estrndup(intern->_path, intern->_path_len);
		}
		break;
	case SPL_FS_FILE:
		ce = ce ? ce : source->file_class;

		zend_update_class_constants(ce TSRMLS_CC);

		return_value->value.obj = spl_filesystem_object_new_ex(ce, &intern TSRMLS_CC);
		Z_TYPE_P(return_value) = IS_OBJECT;
	
		spl_filesystem_object_get_file_name(source TSRMLS_CC);

		if (ce->constructor->common.scope != spl_ce_SplFileObject) {
			MAKE_STD_ZVAL(arg1);
			MAKE_STD_ZVAL(arg2);
			ZVAL_STRINGL(arg1, source->file_name, source->file_name_len, 1);
			ZVAL_STRINGL(arg2, "r", 1, 1);
			zend_call_method_with_2_params(&return_value, ce, &ce->constructor, "__construct", NULL, arg1, arg2);
			zval_ptr_dtor(&arg1);
			zval_ptr_dtor(&arg2);
		} else {
			intern->file_name = source->file_name;
			intern->file_name_len = source->file_name_len;
			intern->_path = spl_filesystem_object_get_path(source, &intern->_path_len TSRMLS_CC);
			intern->_path = estrndup(intern->_path, intern->_path_len);
		
			intern->u.file.open_mode = "r";
			intern->u.file.open_mode_len = 1;
		
			if (ht && zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|sbr", 
					&intern->u.file.open_mode, &intern->u.file.open_mode_len, 
					&use_include_path, &intern->u.file.zcontext) == FAILURE) {
				zend_restore_error_handling(&error_handling TSRMLS_CC);
				intern->u.file.open_mode = NULL;
				intern->file_name = NULL;
				zval_dtor(return_value);
				Z_TYPE_P(return_value) = IS_NULL;
				return NULL;
			}
		
			if (spl_filesystem_file_open(intern, use_include_path, 0 TSRMLS_CC) == FAILURE) {
				zend_restore_error_handling(&error_handling TSRMLS_CC);
				zval_dtor(return_value);
				Z_TYPE_P(return_value) = IS_NULL;
				return NULL;
			}
		}
		break;
	case SPL_FS_DIR:	
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC, "Operation not supported");
		return NULL;
	}
	zend_restore_error_handling(&error_handling TSRMLS_CC);
	return NULL;
} /* }}} */

static int spl_filesystem_is_invalid_or_dot(const char * d_name) /* {{{ */
{
	return d_name[0] == '\0' || spl_filesystem_is_dot(d_name);
}
/* }}} */

static char *spl_filesystem_object_get_pathname(spl_filesystem_object *intern, int *len TSRMLS_DC) { /* {{{ */
	switch (intern->type) {
	case SPL_FS_INFO:
	case SPL_FS_FILE:
		*len = intern->file_name_len;
		return intern->file_name;
	case SPL_FS_DIR:
		if (intern->u.dir.entry.d_name[0]) {
			spl_filesystem_object_get_file_name(intern TSRMLS_CC);
			*len = intern->file_name_len;
			return intern->file_name;
		}
	}
	*len = 0;
	return NULL;
}
/* }}} */

static HashTable* spl_filesystem_object_get_debug_info(zval *obj, int *is_temp TSRMLS_DC) /* {{{ */
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(obj TSRMLS_CC);
	HashTable *rv;
	zval *tmp, zrv;
	char *pnstr, *path;
	int  pnlen, path_len;
	char stmp[2];

	*is_temp = 1;

	if (!intern->std.properties) {
		rebuild_object_properties(&intern->std);
	}

	ALLOC_HASHTABLE(rv);
	ZEND_INIT_SYMTABLE_EX(rv, zend_hash_num_elements(intern->std.properties) + 3, 0);

	INIT_PZVAL(&zrv);
	Z_ARRVAL(zrv) = rv;

	zend_hash_copy(rv, intern->std.properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));

	pnstr = spl_gen_private_prop_name(spl_ce_SplFileInfo, "pathName", sizeof("pathName")-1, &pnlen TSRMLS_CC);
	path = spl_filesystem_object_get_pathname(intern, &path_len TSRMLS_CC);
	add_assoc_stringl_ex(&zrv, pnstr, pnlen+1, path, path_len, 1);
	efree(pnstr);

	if (intern->file_name) {
		pnstr = spl_gen_private_prop_name(spl_ce_SplFileInfo, "fileName", sizeof("fileName")-1, &pnlen TSRMLS_CC);
		spl_filesystem_object_get_path(intern, &path_len TSRMLS_CC);
		
		if (path_len && path_len < intern->file_name_len) {
			add_assoc_stringl_ex(&zrv, pnstr, pnlen+1, intern->file_name + path_len + 1, intern->file_name_len - (path_len + 1), 1);
		} else {
			add_assoc_stringl_ex(&zrv, pnstr, pnlen+1, intern->file_name, intern->file_name_len, 1);
		}
		efree(pnstr);
	}
	if (intern->type == SPL_FS_DIR) {
#ifdef HAVE_GLOB
		pnstr = spl_gen_private_prop_name(spl_ce_DirectoryIterator, "glob", sizeof("glob")-1, &pnlen TSRMLS_CC);
		if (php_stream_is(intern->u.dir.dirp ,&php_glob_stream_ops)) {
			add_assoc_stringl_ex(&zrv, pnstr, pnlen+1, intern->_path, intern->_path_len, 1);
		} else {
			add_assoc_bool_ex(&zrv, pnstr, pnlen+1, 0);
		}
		efree(pnstr);
#endif
		pnstr = spl_gen_private_prop_name(spl_ce_RecursiveDirectoryIterator, "subPathName", sizeof("subPathName")-1, &pnlen TSRMLS_CC);
		if (intern->u.dir.sub_path) {
			add_assoc_stringl_ex(&zrv, pnstr, pnlen+1, intern->u.dir.sub_path, intern->u.dir.sub_path_len, 1);
		} else {
			add_assoc_stringl_ex(&zrv, pnstr, pnlen+1, "", 0, 1);
		}
		efree(pnstr);
	}
	if (intern->type == SPL_FS_FILE) {
		pnstr = spl_gen_private_prop_name(spl_ce_SplFileObject, "openMode", sizeof("openMode")-1, &pnlen TSRMLS_CC);
		add_assoc_stringl_ex(&zrv, pnstr, pnlen+1, intern->u.file.open_mode, intern->u.file.open_mode_len, 1);
		efree(pnstr);
		stmp[1] = '\0';
		stmp[0] = intern->u.file.delimiter;
		pnstr = spl_gen_private_prop_name(spl_ce_SplFileObject, "delimiter", sizeof("delimiter")-1, &pnlen TSRMLS_CC);
		add_assoc_stringl_ex(&zrv, pnstr, pnlen+1, stmp, 1, 1);
		efree(pnstr);
		stmp[0] = intern->u.file.enclosure;
		pnstr = spl_gen_private_prop_name(spl_ce_SplFileObject, "enclosure", sizeof("enclosure")-1, &pnlen TSRMLS_CC);
		add_assoc_stringl_ex(&zrv, pnstr, pnlen+1, stmp, 1, 1);
		efree(pnstr);
	}

	return rv;
}
/* }}} */

zend_function *spl_filesystem_object_get_method_check(zval **object_ptr, char *method, int method_len, const struct _zend_literal *key TSRMLS_DC) /* {{{ */
{
	spl_filesystem_object *fsobj = zend_object_store_get_object(*object_ptr TSRMLS_CC);
	
	if (fsobj->u.dir.entry.d_name[0] == '\0' && fsobj->orig_path == NULL) {
		method = "_bad_state_ex";
		method_len = sizeof("_bad_state_ex") - 1;
		key = NULL;
	}
	
	return zend_get_std_object_handlers()->get_method(object_ptr, method, method_len, key TSRMLS_CC);
}
/* }}} */

#define DIT_CTOR_FLAGS  0x00000001
#define DIT_CTOR_GLOB   0x00000002

void spl_filesystem_object_construct(INTERNAL_FUNCTION_PARAMETERS, long ctor_flags) /* {{{ */
{
	spl_filesystem_object *intern;
	char *path;
	int parsed, len;
	long flags;
	zend_error_handling error_handling;

	zend_replace_error_handling(EH_THROW, spl_ce_UnexpectedValueException, &error_handling TSRMLS_CC);

	if (SPL_HAS_FLAG(ctor_flags, DIT_CTOR_FLAGS)) {
		flags = SPL_FILE_DIR_KEY_AS_PATHNAME|SPL_FILE_DIR_CURRENT_AS_FILEINFO;
		parsed = zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &path, &len, &flags);
	} else {
		flags = SPL_FILE_DIR_KEY_AS_PATHNAME|SPL_FILE_DIR_CURRENT_AS_SELF;
		parsed = zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &path, &len);
	}
	if (SPL_HAS_FLAG(ctor_flags, SPL_FILE_DIR_SKIPDOTS)) {
		flags |= SPL_FILE_DIR_SKIPDOTS;
	}
	if (SPL_HAS_FLAG(ctor_flags, SPL_FILE_DIR_UNIXPATHS)) {
		flags |= SPL_FILE_DIR_UNIXPATHS;
	}
	if (parsed == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}
	if (!len) {
		zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC, "Directory name must not be empty.");
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}

	intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	if (intern->_path) {
		/* object is alreay initialized */
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Directory object is already initialized");
		return;
	}
	intern->flags = flags;
#ifdef HAVE_GLOB
	if (SPL_HAS_FLAG(ctor_flags, DIT_CTOR_GLOB) && strstr(path, "glob://") != path) {
		spprintf(&path, 0, "glob://%s", path);
		spl_filesystem_dir_open(intern, path TSRMLS_CC);
		efree(path);
	} else
#endif
	{
		spl_filesystem_dir_open(intern, path TSRMLS_CC);

	}

	intern->u.dir.is_recursive = instanceof_function(intern->std.ce, spl_ce_RecursiveDirectoryIterator TSRMLS_CC) ? 1 : 0;

	zend_restore_error_handling(&error_handling TSRMLS_CC);
}
/* }}} */

/* {{{ proto void DirectoryIterator::__construct(string path)
 Cronstructs a new dir iterator from a path. */
SPL_METHOD(DirectoryIterator, __construct)
{
	spl_filesystem_object_construct(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto void DirectoryIterator::rewind()
   Rewind dir back to the start */
SPL_METHOD(DirectoryIterator, rewind)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	intern->u.dir.index = 0;
	if (intern->u.dir.dirp) {
		php_stream_rewinddir(intern->u.dir.dirp);
	}
	spl_filesystem_dir_read(intern TSRMLS_CC);
}
/* }}} */

/* {{{ proto string DirectoryIterator::key()
   Return current dir entry */
SPL_METHOD(DirectoryIterator, key)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (intern->u.dir.dirp) {
		RETURN_LONG(intern->u.dir.index);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto DirectoryIterator DirectoryIterator::current()
   Return this (needed for Iterator interface) */
SPL_METHOD(DirectoryIterator, current)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	RETURN_ZVAL(getThis(), 1, 0);
}
/* }}} */

/* {{{ proto void DirectoryIterator::next()
   Move to next entry */
SPL_METHOD(DirectoryIterator, next)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	int skip_dots = SPL_HAS_FLAG(intern->flags, SPL_FILE_DIR_SKIPDOTS);
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	intern->u.dir.index++;
	do {
		spl_filesystem_dir_read(intern TSRMLS_CC);
	} while (skip_dots && spl_filesystem_is_dot(intern->u.dir.entry.d_name));
	if (intern->file_name) {
		efree(intern->file_name);
		intern->file_name = NULL;
	}
}
/* }}} */

/* {{{ proto void DirectoryIterator::seek(int position)
   Seek to the given position */
SPL_METHOD(DirectoryIterator, seek)
{
	spl_filesystem_object *intern    = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	zval                  *retval    = NULL;
	long                   pos;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &pos) == FAILURE) {
		return;
	}

	if (intern->u.dir.index > pos) {
		/* we first rewind */
		zend_call_method_with_0_params(&this_ptr, Z_OBJCE_P(getThis()), &intern->u.dir.func_rewind, "rewind", &retval);
		if (retval) {
			zval_ptr_dtor(&retval);
		}
	}

	while (intern->u.dir.index < pos) {
		int valid = 0;
		zend_call_method_with_0_params(&this_ptr, Z_OBJCE_P(getThis()), &intern->u.dir.func_valid, "valid", &retval);
		if (retval) {
			valid = zend_is_true(retval TSRMLS_CC);
			zval_ptr_dtor(&retval);
		}
		if (!valid) {
			break;
		}
		zend_call_method_with_0_params(&this_ptr, Z_OBJCE_P(getThis()), &intern->u.dir.func_next, "next", &retval);
		if (retval) {
			zval_ptr_dtor(&retval);
		}
	}
} /* }}} */

/* {{{ proto string DirectoryIterator::valid()
   Check whether dir contains more entries */
SPL_METHOD(DirectoryIterator, valid)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_BOOL(intern->u.dir.entry.d_name[0] != '\0');
}
/* }}} */

/* {{{ proto string SplFileInfo::getPath()
   Return the path */
SPL_METHOD(SplFileInfo, getPath)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	char *path;
	int path_len;
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

  	path = spl_filesystem_object_get_path(intern, &path_len TSRMLS_CC);
	RETURN_STRINGL(path, path_len, 1);
}
/* }}} */

/* {{{ proto string SplFileInfo::getFilename()
   Return filename only */
SPL_METHOD(SplFileInfo, getFilename)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	int path_len;
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	spl_filesystem_object_get_path(intern, &path_len TSRMLS_CC);
	
	if (path_len && path_len < intern->file_name_len) {
		RETURN_STRINGL(intern->file_name + path_len + 1, intern->file_name_len - (path_len + 1), 1);
	} else {
		RETURN_STRINGL(intern->file_name, intern->file_name_len, 1);
	}
}
/* }}} */

/* {{{ proto string DirectoryIterator::getFilename()
   Return filename of current dir entry */
SPL_METHOD(DirectoryIterator, getFilename)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_STRING(intern->u.dir.entry.d_name, 1);
}
/* }}} */

/* {{{ proto string SplFileInfo::getExtension()
   Returns file extension component of path */
SPL_METHOD(SplFileInfo, getExtension)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	char *fname = NULL;
	const char *p;
	size_t flen;
	int path_len, idx;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	spl_filesystem_object_get_path(intern, &path_len TSRMLS_CC);

	if (path_len && path_len < intern->file_name_len) {
		fname = intern->file_name + path_len + 1;
		flen = intern->file_name_len - (path_len + 1);
	} else {
		fname = intern->file_name;
		flen = intern->file_name_len;
	}

	php_basename(fname, flen, NULL, 0, &fname, &flen TSRMLS_CC);

	p = zend_memrchr(fname, '.', flen);
	if (p) {
		idx = p - fname;
		RETVAL_STRINGL(fname + idx + 1, flen - idx - 1, 1);
		efree(fname);
		return;
	} else {
		if (fname) {
			efree(fname);
		}
		RETURN_EMPTY_STRING();
	}
}
/* }}}*/

/* {{{ proto string DirectoryIterator::getExtension()
   Returns the file extension component of path */
SPL_METHOD(DirectoryIterator, getExtension)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	char *fname = NULL;
	const char *p;
	size_t flen;
	int idx;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	php_basename(intern->u.dir.entry.d_name, strlen(intern->u.dir.entry.d_name), NULL, 0, &fname, &flen TSRMLS_CC);

	p = zend_memrchr(fname, '.', flen);
	if (p) {
		idx = p - fname;
		RETVAL_STRINGL(fname + idx + 1, flen - idx - 1, 1);
		efree(fname);
		return;
	} else {
		if (fname) {
			efree(fname);
		}
		RETURN_EMPTY_STRING();
	}
}
/* }}} */

/* {{{ proto string SplFileInfo::getBasename([string $suffix]) U
   Returns filename component of path */
SPL_METHOD(SplFileInfo, getBasename)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	char *fname, *suffix = 0;
	size_t flen;
	int slen = 0, path_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &suffix, &slen) == FAILURE) {
		return;
	}

	spl_filesystem_object_get_path(intern, &path_len TSRMLS_CC);

	if (path_len && path_len < intern->file_name_len) {
		fname = intern->file_name + path_len + 1;
		flen = intern->file_name_len - (path_len + 1);
	} else {
		fname = intern->file_name;
		flen = intern->file_name_len;
	}

	php_basename(fname, flen, suffix, slen, &fname, &flen TSRMLS_CC);

	RETURN_STRINGL(fname, flen, 0);
}
/* }}}*/   

/* {{{ proto string DirectoryIterator::getBasename([string $suffix]) U
   Returns filename component of current dir entry */
SPL_METHOD(DirectoryIterator, getBasename)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	char *suffix = 0, *fname;
	int slen = 0;
	size_t flen;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &suffix, &slen) == FAILURE) {
		return;
	}

	php_basename(intern->u.dir.entry.d_name, strlen(intern->u.dir.entry.d_name), suffix, slen, &fname, &flen TSRMLS_CC);

	RETURN_STRINGL(fname, flen, 0);
}
/* }}} */

/* {{{ proto string SplFileInfo::getPathname()
   Return path and filename */
SPL_METHOD(SplFileInfo, getPathname)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	char *path;
	int path_len;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	path = spl_filesystem_object_get_pathname(intern, &path_len TSRMLS_CC);
	if (path != NULL) {
		RETURN_STRINGL(path, path_len, 1);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto string FilesystemIterator::key()
   Return getPathname() or getFilename() depending on flags */
SPL_METHOD(FilesystemIterator, key)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (SPL_FILE_DIR_KEY(intern, SPL_FILE_DIR_KEY_AS_FILENAME)) {
		RETURN_STRING(intern->u.dir.entry.d_name, 1);
	} else {
		spl_filesystem_object_get_file_name(intern TSRMLS_CC);
		RETURN_STRINGL(intern->file_name, intern->file_name_len, 1);
	}
}
/* }}} */

/* {{{ proto string FilesystemIterator::current()
   Return getFilename(), getFileInfo() or $this depending on flags */
SPL_METHOD(FilesystemIterator, current)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (SPL_FILE_DIR_CURRENT(intern, SPL_FILE_DIR_CURRENT_AS_PATHNAME)) {
		spl_filesystem_object_get_file_name(intern TSRMLS_CC);
		RETURN_STRINGL(intern->file_name, intern->file_name_len, 1);
	} else if (SPL_FILE_DIR_CURRENT(intern, SPL_FILE_DIR_CURRENT_AS_FILEINFO)) {
		spl_filesystem_object_get_file_name(intern TSRMLS_CC);
		spl_filesystem_object_create_type(0, intern, SPL_FS_INFO, NULL, return_value TSRMLS_CC);
	} else {
		RETURN_ZVAL(getThis(), 1, 0);
		/*RETURN_STRING(intern->u.dir.entry.d_name, 1);*/
	}
}
/* }}} */

/* {{{ proto bool DirectoryIterator::isDot()
   Returns true if current entry is '.' or  '..' */
SPL_METHOD(DirectoryIterator, isDot)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_BOOL(spl_filesystem_is_dot(intern->u.dir.entry.d_name));
}
/* }}} */

/* {{{ proto void SplFileInfo::__construct(string file_name)
 Cronstructs a new SplFileInfo from a path. */
/* zend_replace_error_handling() is used to throw exceptions in case
   the constructor fails. Here we use this to ensure the object
   has a valid directory resource.
   
   When the constructor gets called the object is already created 
   by the engine, so we must only call 'additional' initializations.
 */
SPL_METHOD(SplFileInfo, __construct)
{
	spl_filesystem_object *intern;
	char *path;
	int len;
	zend_error_handling error_handling;

	zend_replace_error_handling(EH_THROW, spl_ce_RuntimeException, &error_handling TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &path, &len) == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}

	intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	spl_filesystem_info_set_filename(intern, path, len, 1 TSRMLS_CC);

	zend_restore_error_handling(&error_handling TSRMLS_CC);
	
	/* intern->type = SPL_FS_INFO; already set */
}
/* }}} */

/* {{{ FileInfoFunction */
#define FileInfoFunction(func_name, func_num) \
SPL_METHOD(SplFileInfo, func_name) \
{ \
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC); \
	zend_error_handling error_handling; \
	if (zend_parse_parameters_none() == FAILURE) { \
		return; \
	} \
 \
	zend_replace_error_handling(EH_THROW, spl_ce_RuntimeException, &error_handling TSRMLS_CC);\
	spl_filesystem_object_get_file_name(intern TSRMLS_CC); \
	php_stat(intern->file_name, intern->file_name_len, func_num, return_value TSRMLS_CC); \
	zend_restore_error_handling(&error_handling TSRMLS_CC); \
}
/* }}} */

/* {{{ proto int SplFileInfo::getPerms()
   Get file permissions */
FileInfoFunction(getPerms, FS_PERMS)
/* }}} */

/* {{{ proto int SplFileInfo::getInode()
   Get file inode */
FileInfoFunction(getInode, FS_INODE)
/* }}} */

/* {{{ proto int SplFileInfo::getSize()
   Get file size */
FileInfoFunction(getSize, FS_SIZE)
/* }}} */

/* {{{ proto int SplFileInfo::getOwner()
   Get file owner */
FileInfoFunction(getOwner, FS_OWNER)
/* }}} */

/* {{{ proto int SplFileInfo::getGroup()
   Get file group */
FileInfoFunction(getGroup, FS_GROUP)
/* }}} */

/* {{{ proto int SplFileInfo::getATime()
   Get last access time of file */
FileInfoFunction(getATime, FS_ATIME)
/* }}} */

/* {{{ proto int SplFileInfo::getMTime()
   Get last modification time of file */
FileInfoFunction(getMTime, FS_MTIME)
/* }}} */

/* {{{ proto int SplFileInfo::getCTime()
   Get inode modification time of file */
FileInfoFunction(getCTime, FS_CTIME)
/* }}} */

/* {{{ proto string SplFileInfo::getType()
   Get file type */
FileInfoFunction(getType, FS_TYPE)
/* }}} */

/* {{{ proto bool SplFileInfo::isWritable()
   Returns true if file can be written */
FileInfoFunction(isWritable, FS_IS_W)
/* }}} */

/* {{{ proto bool SplFileInfo::isReadable()
   Returns true if file can be read */
FileInfoFunction(isReadable, FS_IS_R)
/* }}} */

/* {{{ proto bool SplFileInfo::isExecutable()
   Returns true if file is executable */
FileInfoFunction(isExecutable, FS_IS_X)
/* }}} */

/* {{{ proto bool SplFileInfo::isFile()
   Returns true if file is a regular file */
FileInfoFunction(isFile, FS_IS_FILE)
/* }}} */

/* {{{ proto bool SplFileInfo::isDir()
   Returns true if file is directory */
FileInfoFunction(isDir, FS_IS_DIR)
/* }}} */

/* {{{ proto bool SplFileInfo::isLink()
   Returns true if file is symbolic link */
FileInfoFunction(isLink, FS_IS_LINK)
/* }}} */

/* {{{ proto string SplFileInfo::getLinkTarget() U
   Return the target of a symbolic link */
SPL_METHOD(SplFileInfo, getLinkTarget)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	int ret;
	char buff[MAXPATHLEN];
	zend_error_handling error_handling;
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	zend_replace_error_handling(EH_THROW, spl_ce_RuntimeException, &error_handling TSRMLS_CC);

#if defined(PHP_WIN32) || HAVE_SYMLINK
	if (intern->file_name == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Empty filename");
		RETURN_FALSE;
	} else if (!IS_ABSOLUTE_PATH(intern->file_name, intern->file_name_len)) {
		char expanded_path[MAXPATHLEN];
		if (!expand_filepath_with_mode(intern->file_name, expanded_path, NULL, 0, CWD_EXPAND  TSRMLS_CC)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "No such file or directory");
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
		zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC, "Unable to read link %s, error: %s", intern->file_name, strerror(errno));
		RETVAL_FALSE;
	} else {
		/* Append NULL to the end of the string */
		buff[ret] = '\0';

		RETVAL_STRINGL(buff, ret, 1);
	}

	zend_restore_error_handling(&error_handling TSRMLS_CC);
}
/* }}} */

#if (!defined(__BEOS__) && !defined(NETWARE) && HAVE_REALPATH) || defined(ZTS)
/* {{{ proto string SplFileInfo::getRealPath()
   Return the resolved path */
SPL_METHOD(SplFileInfo, getRealPath)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	char buff[MAXPATHLEN];
	char *filename;
	zend_error_handling error_handling;
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	zend_replace_error_handling(EH_THROW, spl_ce_RuntimeException, &error_handling TSRMLS_CC);

	if (intern->type == SPL_FS_DIR && !intern->file_name && intern->u.dir.entry.d_name[0]) {
		spl_filesystem_object_get_file_name(intern TSRMLS_CC);
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
		RETVAL_STRING(buff, 1);
	} else {
		RETVAL_FALSE;
	}

	zend_restore_error_handling(&error_handling TSRMLS_CC);
}
/* }}} */
#endif

/* {{{ proto SplFileObject SplFileInfo::openFile([string mode = 'r' [, bool use_include_path  [, resource context]]])
   Open the current file */
SPL_METHOD(SplFileInfo, openFile)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	spl_filesystem_object_create_type(ht, intern, SPL_FS_FILE, NULL, return_value TSRMLS_CC);
}
/* }}} */

/* {{{ proto void SplFileInfo::setFileClass([string class_name])
   Class to use in openFile() */
SPL_METHOD(SplFileInfo, setFileClass)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	zend_class_entry *ce = spl_ce_SplFileObject;
	zend_error_handling error_handling;
	
	zend_replace_error_handling(EH_THROW, spl_ce_UnexpectedValueException, &error_handling TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|C", &ce) == SUCCESS) {
		intern->file_class = ce;
	}

	zend_restore_error_handling(&error_handling TSRMLS_CC);
}
/* }}} */

/* {{{ proto void SplFileInfo::setInfoClass([string class_name])
   Class to use in getFileInfo(), getPathInfo() */
SPL_METHOD(SplFileInfo, setInfoClass)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	zend_class_entry *ce = spl_ce_SplFileInfo;
	zend_error_handling error_handling;
	
	zend_replace_error_handling(EH_THROW, spl_ce_UnexpectedValueException, &error_handling  TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|C", &ce) == SUCCESS) {
		intern->info_class = ce;
	}

	zend_restore_error_handling(&error_handling TSRMLS_CC);
}
/* }}} */

/* {{{ proto SplFileInfo SplFileInfo::getFileInfo([string $class_name])
   Get/copy file info */
SPL_METHOD(SplFileInfo, getFileInfo)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	zend_class_entry *ce = intern->info_class;
	zend_error_handling error_handling;
	
	zend_replace_error_handling(EH_THROW, spl_ce_UnexpectedValueException, &error_handling TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|C", &ce) == SUCCESS) {
		spl_filesystem_object_create_type(ht, intern, SPL_FS_INFO, ce, return_value TSRMLS_CC);
	}

	zend_restore_error_handling(&error_handling TSRMLS_CC);
}
/* }}} */

/* {{{ proto SplFileInfo SplFileInfo::getPathInfo([string $class_name])
   Get/copy file info */
SPL_METHOD(SplFileInfo, getPathInfo)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	zend_class_entry *ce = intern->info_class;
	zend_error_handling error_handling;
	
	zend_replace_error_handling(EH_THROW, spl_ce_UnexpectedValueException, &error_handling TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|C", &ce) == SUCCESS) {
		int path_len;
		char *path = spl_filesystem_object_get_pathname(intern, &path_len TSRMLS_CC);
		if (path) {
			char *dpath = estrndup(path, path_len);
			path_len = php_dirname(dpath, path_len);
			spl_filesystem_object_create_info(intern, dpath, path_len, 1, ce, return_value TSRMLS_CC);
			efree(dpath);
		}
	}

	zend_restore_error_handling(&error_handling TSRMLS_CC);
}
/* }}} */

/* {{{ */
SPL_METHOD(SplFileInfo, _bad_state_ex)
{
	zend_throw_exception_ex(spl_ce_LogicException, 0 TSRMLS_CC,
		"The parent constructor was not called: the object is in an "
		"invalid state ");
}
/* }}} */

/* {{{ proto void FilesystemIterator::__construct(string path [, int flags])
 Cronstructs a new dir iterator from a path. */
SPL_METHOD(FilesystemIterator, __construct)
{
	spl_filesystem_object_construct(INTERNAL_FUNCTION_PARAM_PASSTHRU, DIT_CTOR_FLAGS | SPL_FILE_DIR_SKIPDOTS);
}
/* }}} */

/* {{{ proto void FilesystemIterator::rewind()
   Rewind dir back to the start */
SPL_METHOD(FilesystemIterator, rewind)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	int skip_dots = SPL_HAS_FLAG(intern->flags, SPL_FILE_DIR_SKIPDOTS);

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	intern->u.dir.index = 0;
	if (intern->u.dir.dirp) {
		php_stream_rewinddir(intern->u.dir.dirp);
	}
	do {
		spl_filesystem_dir_read(intern TSRMLS_CC);
	} while (skip_dots && spl_filesystem_is_dot(intern->u.dir.entry.d_name));
}
/* }}} */

/* {{{ proto int FilesystemIterator::getFlags()
   Get handling flags */
SPL_METHOD(FilesystemIterator, getFlags)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_LONG(intern->flags & (SPL_FILE_DIR_KEY_MODE_MASK | SPL_FILE_DIR_CURRENT_MODE_MASK | SPL_FILE_DIR_OTHERS_MASK));
} /* }}} */

/* {{{ proto void FilesystemIterator::setFlags(long $flags)
   Set handling flags */
SPL_METHOD(FilesystemIterator, setFlags)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	long flags;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &flags) == FAILURE) {
		return;
	}

	intern->flags &= ~(SPL_FILE_DIR_KEY_MODE_MASK|SPL_FILE_DIR_CURRENT_MODE_MASK|SPL_FILE_DIR_OTHERS_MASK);
	intern->flags |= ((SPL_FILE_DIR_KEY_MODE_MASK|SPL_FILE_DIR_CURRENT_MODE_MASK|SPL_FILE_DIR_OTHERS_MASK) & flags);
} /* }}} */

/* {{{ proto bool RecursiveDirectoryIterator::hasChildren([bool $allow_links = false])
   Returns whether current entry is a directory and not '.' or '..' */
SPL_METHOD(RecursiveDirectoryIterator, hasChildren)
{
	zend_bool allow_links = 0;
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|b", &allow_links) == FAILURE) {
		return;
	}
	if (spl_filesystem_is_invalid_or_dot(intern->u.dir.entry.d_name)) {
		RETURN_FALSE;
	} else {
		spl_filesystem_object_get_file_name(intern TSRMLS_CC);
		if (!allow_links && !(intern->flags & SPL_FILE_DIR_FOLLOW_SYMLINKS)) {
			php_stat(intern->file_name, intern->file_name_len, FS_IS_LINK, return_value TSRMLS_CC);
			if (zend_is_true(return_value TSRMLS_CC)) {
				RETURN_FALSE;
			}
		}
		php_stat(intern->file_name, intern->file_name_len, FS_IS_DIR, return_value TSRMLS_CC);
    }
}
/* }}} */

/* {{{ proto RecursiveDirectoryIterator DirectoryIterator::getChildren()
   Returns an iterator for the current entry if it is a directory */
SPL_METHOD(RecursiveDirectoryIterator, getChildren)
{
	zval *zpath, *zflags;
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	spl_filesystem_object *subdir;
	char slash = SPL_HAS_FLAG(intern->flags, SPL_FILE_DIR_UNIXPATHS) ? '/' : DEFAULT_SLASH;
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	
	spl_filesystem_object_get_file_name(intern TSRMLS_CC);

	if (SPL_HAS_FLAG(intern->flags, SPL_FILE_DIR_CURRENT_AS_PATHNAME)) {
		RETURN_STRINGL(intern->file_name, intern->file_name_len, 1);
	} else {
		MAKE_STD_ZVAL(zflags);
		MAKE_STD_ZVAL(zpath);
		ZVAL_LONG(zflags, intern->flags);
		ZVAL_STRINGL(zpath, intern->file_name, intern->file_name_len, 1);
		spl_instantiate_arg_ex2(Z_OBJCE_P(getThis()), &return_value, 0, zpath, zflags TSRMLS_CC);
		zval_ptr_dtor(&zpath);
		zval_ptr_dtor(&zflags);
		
		subdir = (spl_filesystem_object*)zend_object_store_get_object(return_value TSRMLS_CC);
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
}
/* }}} */

/* {{{ proto void RecursiveDirectoryIterator::getSubPath()
   Get sub path */
SPL_METHOD(RecursiveDirectoryIterator, getSubPath)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (intern->u.dir.sub_path) {
		RETURN_STRINGL(intern->u.dir.sub_path, intern->u.dir.sub_path_len, 1);
	} else {
		RETURN_STRINGL("", 0, 1);
	}
}
/* }}} */

/* {{{ proto void RecursiveDirectoryIterator::getSubPathname()
   Get sub path and file name */
SPL_METHOD(RecursiveDirectoryIterator, getSubPathname)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	char *sub_name;
	int len;
	char slash = SPL_HAS_FLAG(intern->flags, SPL_FILE_DIR_UNIXPATHS) ? '/' : DEFAULT_SLASH;
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (intern->u.dir.sub_path) {
		len = spprintf(&sub_name, 0, "%s%c%s", intern->u.dir.sub_path, slash, intern->u.dir.entry.d_name);
		RETURN_STRINGL(sub_name, len, 0);
	} else {
		RETURN_STRING(intern->u.dir.entry.d_name, 1);
	}
}
/* }}} */

/* {{{ proto int RecursiveDirectoryIterator::__construct(string path [, int flags])
 Cronstructs a new dir iterator from a path. */
SPL_METHOD(RecursiveDirectoryIterator, __construct)
{
	spl_filesystem_object_construct(INTERNAL_FUNCTION_PARAM_PASSTHRU, DIT_CTOR_FLAGS);
}
/* }}} */

#ifdef HAVE_GLOB
/* {{{ proto int GlobIterator::__construct(string path [, int flags])
 Cronstructs a new dir iterator from a glob expression (no glob:// needed). */
SPL_METHOD(GlobIterator, __construct)
{
	spl_filesystem_object_construct(INTERNAL_FUNCTION_PARAM_PASSTHRU, DIT_CTOR_FLAGS|DIT_CTOR_GLOB);
}
/* }}} */

/* {{{ proto int GlobIterator::cont()
   Return the number of directories and files found by globbing */
SPL_METHOD(GlobIterator, count)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (php_stream_is(intern->u.dir.dirp ,&php_glob_stream_ops)) {
		RETURN_LONG(php_glob_stream_get_count(intern->u.dir.dirp, NULL));
	} else {
		/* should not happen */
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "GlobIterator lost glob state");
	}
}
/* }}} */
#endif /* HAVE_GLOB */

/* {{{ forward declarations to the iterator handlers */
static void spl_filesystem_dir_it_dtor(zend_object_iterator *iter TSRMLS_DC);
static int spl_filesystem_dir_it_valid(zend_object_iterator *iter TSRMLS_DC);
static void spl_filesystem_dir_it_current_data(zend_object_iterator *iter, zval ***data TSRMLS_DC);
static void spl_filesystem_dir_it_current_key(zend_object_iterator *iter, zval *key TSRMLS_DC);
static void spl_filesystem_dir_it_move_forward(zend_object_iterator *iter TSRMLS_DC);
static void spl_filesystem_dir_it_rewind(zend_object_iterator *iter TSRMLS_DC);

/* iterator handler table */
zend_object_iterator_funcs spl_filesystem_dir_it_funcs = {
	spl_filesystem_dir_it_dtor,
	spl_filesystem_dir_it_valid,
	spl_filesystem_dir_it_current_data,
	spl_filesystem_dir_it_current_key,
	spl_filesystem_dir_it_move_forward,
	spl_filesystem_dir_it_rewind
};
/* }}} */

/* {{{ spl_ce_dir_get_iterator */
zend_object_iterator *spl_filesystem_dir_get_iterator(zend_class_entry *ce, zval *object, int by_ref TSRMLS_DC)
{
	spl_filesystem_iterator *iterator;
	spl_filesystem_object   *dir_object;

	if (by_ref) {
		zend_error(E_ERROR, "An iterator cannot be used with foreach by reference");
	}
	dir_object = (spl_filesystem_object*)zend_object_store_get_object(object TSRMLS_CC);
	iterator   = spl_filesystem_object_to_iterator(dir_object);

	/* initialize iterator if it wasn't gotten before */
	if (iterator->intern.data == NULL) {
		iterator->intern.data = object;
		iterator->intern.funcs = &spl_filesystem_dir_it_funcs;
		/* ->current must be initialized; rewind doesn't set it and valid
		 * doesn't check whether it's set */
		iterator->current = object;
	}
	zval_add_ref(&object);
	
	return (zend_object_iterator*)iterator;
}
/* }}} */

/* {{{ spl_filesystem_dir_it_dtor */
static void spl_filesystem_dir_it_dtor(zend_object_iterator *iter TSRMLS_DC)
{
	spl_filesystem_iterator *iterator = (spl_filesystem_iterator *)iter;

	if (iterator->intern.data) {
		zval *object =  iterator->intern.data;
		zval_ptr_dtor(&object);
	}
	/* Otherwise we were called from the owning object free storage handler as
	 * it sets
	 * iterator->intern.data to NULL.
	 * We don't even need to destroy iterator->current as we didn't add a
	 * reference to it in move_forward or get_iterator */
}
/* }}} */

/* {{{ spl_filesystem_dir_it_valid */
static int spl_filesystem_dir_it_valid(zend_object_iterator *iter TSRMLS_DC)
{
	spl_filesystem_object *object = spl_filesystem_iterator_to_object((spl_filesystem_iterator *)iter);

	return object->u.dir.entry.d_name[0] != '\0' ? SUCCESS : FAILURE;
}
/* }}} */

/* {{{ spl_filesystem_dir_it_current_data */
static void spl_filesystem_dir_it_current_data(zend_object_iterator *iter, zval ***data TSRMLS_DC)
{
	spl_filesystem_iterator *iterator = (spl_filesystem_iterator *)iter;
	
	*data = &iterator->current;
}
/* }}} */

/* {{{ spl_filesystem_dir_it_current_key */
static void spl_filesystem_dir_it_current_key(zend_object_iterator *iter, zval *key TSRMLS_DC)
{
	spl_filesystem_object *object = spl_filesystem_iterator_to_object((spl_filesystem_iterator *)iter);

	ZVAL_LONG(key, object->u.dir.index);
}
/* }}} */

/* {{{ spl_filesystem_dir_it_move_forward */
static void spl_filesystem_dir_it_move_forward(zend_object_iterator *iter TSRMLS_DC)
{
	spl_filesystem_object *object = spl_filesystem_iterator_to_object((spl_filesystem_iterator *)iter);
	
	object->u.dir.index++;
	spl_filesystem_dir_read(object TSRMLS_CC);
	if (object->file_name) {
		efree(object->file_name);
		object->file_name = NULL;
	}
}
/* }}} */

/* {{{ spl_filesystem_dir_it_rewind */
static void spl_filesystem_dir_it_rewind(zend_object_iterator *iter TSRMLS_DC)
{
	spl_filesystem_object *object = spl_filesystem_iterator_to_object((spl_filesystem_iterator *)iter);
	
	object->u.dir.index = 0;
	if (object->u.dir.dirp) {
		php_stream_rewinddir(object->u.dir.dirp);
	}
	spl_filesystem_dir_read(object TSRMLS_CC);
}
/* }}} */

/* {{{ spl_filesystem_tree_it_dtor */
static void spl_filesystem_tree_it_dtor(zend_object_iterator *iter TSRMLS_DC)
{
	spl_filesystem_iterator *iterator = (spl_filesystem_iterator *)iter;

	if (iterator->intern.data) {
		zval *object = 	iterator->intern.data;
		zval_ptr_dtor(&object);
	} else {
		if (iterator->current) {
			zval_ptr_dtor(&iterator->current);
		}
	}
}
/* }}} */

/* {{{ spl_filesystem_tree_it_current_data */
static void spl_filesystem_tree_it_current_data(zend_object_iterator *iter, zval ***data TSRMLS_DC)
{
	spl_filesystem_iterator *iterator = (spl_filesystem_iterator *)iter;
	spl_filesystem_object   *object   = spl_filesystem_iterator_to_object(iterator);

	if (SPL_FILE_DIR_CURRENT(object, SPL_FILE_DIR_CURRENT_AS_PATHNAME)) {
		if (!iterator->current) {
			ALLOC_INIT_ZVAL(iterator->current);
			spl_filesystem_object_get_file_name(object TSRMLS_CC);
			ZVAL_STRINGL(iterator->current, object->file_name, object->file_name_len, 1);
		}
		*data = &iterator->current;
	} else if (SPL_FILE_DIR_CURRENT(object, SPL_FILE_DIR_CURRENT_AS_FILEINFO)) {
		if (!iterator->current) {
			ALLOC_INIT_ZVAL(iterator->current);
			spl_filesystem_object_get_file_name(object TSRMLS_CC);
			spl_filesystem_object_create_type(0, object, SPL_FS_INFO, NULL, iterator->current TSRMLS_CC);
		}
		*data = &iterator->current;
	} else {
		*data = (zval**)&iterator->intern.data;
	}
}
/* }}} */

/* {{{ spl_filesystem_tree_it_current_key */
static void spl_filesystem_tree_it_current_key(zend_object_iterator *iter, zval *key TSRMLS_DC)
{
	spl_filesystem_object *object = spl_filesystem_iterator_to_object((spl_filesystem_iterator *)iter);

	if (SPL_FILE_DIR_KEY(object, SPL_FILE_DIR_KEY_AS_FILENAME)) {
		ZVAL_STRING(key, object->u.dir.entry.d_name, 1);
	} else {
		spl_filesystem_object_get_file_name(object TSRMLS_CC);
		ZVAL_STRINGL(key, object->file_name, object->file_name_len, 1);
	}
}
/* }}} */

/* {{{ spl_filesystem_tree_it_move_forward */
static void spl_filesystem_tree_it_move_forward(zend_object_iterator *iter TSRMLS_DC)
{
	spl_filesystem_iterator *iterator = (spl_filesystem_iterator *)iter;
	spl_filesystem_object   *object   = spl_filesystem_iterator_to_object(iterator);
	
	object->u.dir.index++;
	do {
		spl_filesystem_dir_read(object TSRMLS_CC);
	} while (spl_filesystem_is_dot(object->u.dir.entry.d_name));
	if (object->file_name) {
		efree(object->file_name);
		object->file_name = NULL;
	}
	if (iterator->current) {
		zval_ptr_dtor(&iterator->current);
		iterator->current = NULL;
	}
}
/* }}} */

/* {{{ spl_filesystem_tree_it_rewind */
static void spl_filesystem_tree_it_rewind(zend_object_iterator *iter TSRMLS_DC)
{
	spl_filesystem_iterator *iterator = (spl_filesystem_iterator *)iter;
	spl_filesystem_object   *object   = spl_filesystem_iterator_to_object(iterator);
	
	object->u.dir.index = 0;
	if (object->u.dir.dirp) {
		php_stream_rewinddir(object->u.dir.dirp);
	}
	do {
		spl_filesystem_dir_read(object TSRMLS_CC);
	} while (spl_filesystem_is_dot(object->u.dir.entry.d_name));
	if (iterator->current) {
		zval_ptr_dtor(&iterator->current);
		iterator->current = NULL;
	}
}
/* }}} */

/* {{{ iterator handler table */
zend_object_iterator_funcs spl_filesystem_tree_it_funcs = {
	spl_filesystem_tree_it_dtor,
	spl_filesystem_dir_it_valid,
	spl_filesystem_tree_it_current_data,
	spl_filesystem_tree_it_current_key,
	spl_filesystem_tree_it_move_forward,
	spl_filesystem_tree_it_rewind
};
/* }}} */

/* {{{ spl_ce_dir_get_iterator */
zend_object_iterator *spl_filesystem_tree_get_iterator(zend_class_entry *ce, zval *object, int by_ref TSRMLS_DC)
{
	spl_filesystem_iterator *iterator;
	spl_filesystem_object *dir_object;

	if (by_ref) {
		zend_error(E_ERROR, "An iterator cannot be used with foreach by reference");
	}
	dir_object = (spl_filesystem_object*)zend_object_store_get_object(object TSRMLS_CC);
	iterator   = spl_filesystem_object_to_iterator(dir_object);

	/* initialize iterator if wasn't gotten before */
	if (iterator->intern.data == NULL) {
		iterator->intern.data = object;
		iterator->intern.funcs = &spl_filesystem_tree_it_funcs;
	}
	zval_add_ref(&object);
	
	return (zend_object_iterator*)iterator;
}
/* }}} */

/* {{{ spl_filesystem_object_cast */
static int spl_filesystem_object_cast(zval *readobj, zval *writeobj, int type TSRMLS_DC)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(readobj TSRMLS_CC);

	if (type == IS_STRING) {
		if (Z_OBJCE_P(readobj)->__tostring) {
			return std_object_handlers.cast_object(readobj, writeobj, type TSRMLS_CC);
		}

		switch (intern->type) {
		case SPL_FS_INFO:
		case SPL_FS_FILE:
			if (readobj == writeobj) {
				zval retval;
				zval *retval_ptr = &retval;

				ZVAL_STRINGL(retval_ptr, intern->file_name, intern->file_name_len, 1);
				zval_dtor(readobj);
				ZVAL_ZVAL(writeobj, retval_ptr, 0, 0);
			} else {
				ZVAL_STRINGL(writeobj, intern->file_name, intern->file_name_len, 1);
			}
			return SUCCESS;
		case SPL_FS_DIR:
			if (readobj == writeobj) {
				zval retval;
				zval *retval_ptr = &retval;

				ZVAL_STRING(retval_ptr, intern->u.dir.entry.d_name, 1);
				zval_dtor(readobj);
				ZVAL_ZVAL(writeobj, retval_ptr, 0, 0);
			} else {
				ZVAL_STRING(writeobj, intern->u.dir.entry.d_name, 1);
			}
			return SUCCESS;
		}
	}
	if (readobj == writeobj) {
		zval_dtor(readobj);
	}
	ZVAL_NULL(writeobj);
	return FAILURE;
}
/* }}} */

/* {{{ declare method parameters */
/* supply a name and default to call by parameter */
ZEND_BEGIN_ARG_INFO(arginfo_info___construct, 0) 
	ZEND_ARG_INFO(0, file_name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_info_openFile, 0, 0, 0)
	ZEND_ARG_INFO(0, open_mode)
	ZEND_ARG_INFO(0, use_include_path)
	ZEND_ARG_INFO(0, context)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_info_optinalFileClass, 0, 0, 0)
	ZEND_ARG_INFO(0, class_name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_optinalSuffix, 0, 0, 0)
	ZEND_ARG_INFO(0, suffix)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_splfileinfo_void, 0)
ZEND_END_ARG_INFO()

/* the method table */
/* each method can have its own parameters and visibility */
static const zend_function_entry spl_SplFileInfo_functions[] = {
	SPL_ME(SplFileInfo,       __construct,   arginfo_info___construct, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileInfo,       getPath,       arginfo_splfileinfo_void, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileInfo,       getFilename,   arginfo_splfileinfo_void, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileInfo,       getExtension,  arginfo_splfileinfo_void, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileInfo,       getBasename,   arginfo_optinalSuffix, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileInfo,       getPathname,   arginfo_splfileinfo_void, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileInfo,       getPerms,      arginfo_splfileinfo_void, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileInfo,       getInode,      arginfo_splfileinfo_void, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileInfo,       getSize,       arginfo_splfileinfo_void, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileInfo,       getOwner,      arginfo_splfileinfo_void, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileInfo,       getGroup,      arginfo_splfileinfo_void, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileInfo,       getATime,      arginfo_splfileinfo_void, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileInfo,       getMTime,      arginfo_splfileinfo_void, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileInfo,       getCTime,      arginfo_splfileinfo_void, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileInfo,       getType,       arginfo_splfileinfo_void, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileInfo,       isWritable,    arginfo_splfileinfo_void, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileInfo,       isReadable,    arginfo_splfileinfo_void, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileInfo,       isExecutable,  arginfo_splfileinfo_void, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileInfo,       isFile,        arginfo_splfileinfo_void, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileInfo,       isDir,         arginfo_splfileinfo_void, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileInfo,       isLink,        arginfo_splfileinfo_void, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileInfo,       getLinkTarget, arginfo_splfileinfo_void, ZEND_ACC_PUBLIC)
#if (!defined(__BEOS__) && !defined(NETWARE) && HAVE_REALPATH) || defined(ZTS)
	SPL_ME(SplFileInfo,       getRealPath,   arginfo_splfileinfo_void, ZEND_ACC_PUBLIC)
#endif
	SPL_ME(SplFileInfo,       getFileInfo,   arginfo_info_optinalFileClass, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileInfo,       getPathInfo,   arginfo_info_optinalFileClass, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileInfo,       openFile,      arginfo_info_openFile,         ZEND_ACC_PUBLIC)
	SPL_ME(SplFileInfo,       setFileClass,  arginfo_info_optinalFileClass, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileInfo,       setInfoClass,  arginfo_info_optinalFileClass, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileInfo,       _bad_state_ex, NULL,							ZEND_ACC_PUBLIC|ZEND_ACC_FINAL)
	SPL_MA(SplFileInfo,       __toString, SplFileInfo, getPathname, arginfo_splfileinfo_void, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

ZEND_BEGIN_ARG_INFO(arginfo_dir___construct, 0) 
	ZEND_ARG_INFO(0, path)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_dir_it_seek, 0) 
	ZEND_ARG_INFO(0, position)
ZEND_END_ARG_INFO();

/* the method table */
/* each method can have its own parameters and visibility */
static const zend_function_entry spl_DirectoryIterator_functions[] = {
	SPL_ME(DirectoryIterator, __construct,   arginfo_dir___construct, ZEND_ACC_PUBLIC)
	SPL_ME(DirectoryIterator, getFilename,   arginfo_splfileinfo_void, ZEND_ACC_PUBLIC)
	SPL_ME(DirectoryIterator, getExtension,  arginfo_splfileinfo_void, ZEND_ACC_PUBLIC)
	SPL_ME(DirectoryIterator, getBasename,   arginfo_optinalSuffix, ZEND_ACC_PUBLIC)
	SPL_ME(DirectoryIterator, isDot,         arginfo_splfileinfo_void, ZEND_ACC_PUBLIC)
	SPL_ME(DirectoryIterator, rewind,        arginfo_splfileinfo_void, ZEND_ACC_PUBLIC)
	SPL_ME(DirectoryIterator, valid,         arginfo_splfileinfo_void, ZEND_ACC_PUBLIC)
	SPL_ME(DirectoryIterator, key,           arginfo_splfileinfo_void, ZEND_ACC_PUBLIC)
	SPL_ME(DirectoryIterator, current,       arginfo_splfileinfo_void, ZEND_ACC_PUBLIC)
	SPL_ME(DirectoryIterator, next,          arginfo_splfileinfo_void, ZEND_ACC_PUBLIC)
	SPL_ME(DirectoryIterator, seek,          arginfo_dir_it_seek, ZEND_ACC_PUBLIC)
	SPL_MA(DirectoryIterator, __toString, DirectoryIterator, getFilename, arginfo_splfileinfo_void, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

ZEND_BEGIN_ARG_INFO_EX(arginfo_r_dir___construct, 0, 0, 1) 
	ZEND_ARG_INFO(0, path)
	ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_r_dir_hasChildren, 0, 0, 0)
	ZEND_ARG_INFO(0, allow_links)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_r_dir_setFlags, 0, 0, 0)
	ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()

static const zend_function_entry spl_FilesystemIterator_functions[] = {
	SPL_ME(FilesystemIterator, __construct,   arginfo_r_dir___construct, ZEND_ACC_PUBLIC)
	SPL_ME(FilesystemIterator, rewind,        arginfo_splfileinfo_void, ZEND_ACC_PUBLIC)
	SPL_ME(DirectoryIterator,  next,          arginfo_splfileinfo_void, ZEND_ACC_PUBLIC)
	SPL_ME(FilesystemIterator, key,           arginfo_splfileinfo_void, ZEND_ACC_PUBLIC)
	SPL_ME(FilesystemIterator, current,       arginfo_splfileinfo_void, ZEND_ACC_PUBLIC)
	SPL_ME(FilesystemIterator, getFlags,      arginfo_splfileinfo_void, ZEND_ACC_PUBLIC)
	SPL_ME(FilesystemIterator, setFlags,      arginfo_r_dir_setFlags, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

static const zend_function_entry spl_RecursiveDirectoryIterator_functions[] = {
	SPL_ME(RecursiveDirectoryIterator, __construct,   arginfo_r_dir___construct, ZEND_ACC_PUBLIC)
	SPL_ME(RecursiveDirectoryIterator, hasChildren,   arginfo_r_dir_hasChildren, ZEND_ACC_PUBLIC)
	SPL_ME(RecursiveDirectoryIterator, getChildren,   arginfo_splfileinfo_void, ZEND_ACC_PUBLIC)
	SPL_ME(RecursiveDirectoryIterator, getSubPath,    arginfo_splfileinfo_void, ZEND_ACC_PUBLIC)
	SPL_ME(RecursiveDirectoryIterator, getSubPathname,arginfo_splfileinfo_void, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

#ifdef HAVE_GLOB
static const zend_function_entry spl_GlobIterator_functions[] = {
	SPL_ME(GlobIterator, __construct,   arginfo_r_dir___construct, ZEND_ACC_PUBLIC)
	SPL_ME(GlobIterator, count,         arginfo_splfileinfo_void,  ZEND_ACC_PUBLIC)
	PHP_FE_END
};
#endif
/* }}} */

static int spl_filesystem_file_read(spl_filesystem_object *intern, int silent TSRMLS_DC) /* {{{ */
{
	char *buf;
	size_t line_len = 0;
	long line_add = (intern->u.file.current_line || intern->u.file.current_zval) ? 1 : 0;

	spl_filesystem_file_free_line(intern TSRMLS_CC);
	
	if (php_stream_eof(intern->u.file.stream)) {
		if (!silent) {
			zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC, "Cannot read from file %s", intern->file_name);
		}
		return FAILURE;
	}

	if (intern->u.file.max_line_len > 0) {
		buf = safe_emalloc((intern->u.file.max_line_len + 1), sizeof(char), 0);
		if (php_stream_get_line(intern->u.file.stream, buf, intern->u.file.max_line_len, &line_len) == NULL) {
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
			line_len = strcspn(buf, "\r\n");
			buf[line_len] = '\0';
		}
	
		intern->u.file.current_line = buf;
		intern->u.file.current_line_len = line_len;
	}
	intern->u.file.current_line_num += line_add;

	return SUCCESS;
} /* }}} */

static int spl_filesystem_file_call(spl_filesystem_object *intern, zend_function *func_ptr, int pass_num_args, zval *return_value, zval *arg2 TSRMLS_DC) /* {{{ */
{
	zend_fcall_info fci;
	zend_fcall_info_cache fcic;
	zval z_fname;
	zval * zresource_ptr = &intern->u.file.zresource, *retval;
	int result;
	int num_args = pass_num_args + (arg2 ? 2 : 1);

	zval ***params = (zval***)safe_emalloc(num_args, sizeof(zval**), 0);

	params[0] = &zresource_ptr;
	
	if (arg2) {
		params[1] = &arg2;
	}

	zend_get_parameters_array_ex(pass_num_args, params+(arg2 ? 2 : 1));

	ZVAL_STRING(&z_fname, func_ptr->common.function_name, 0);

	fci.size = sizeof(fci);
	fci.function_table = EG(function_table);
	fci.object_ptr = NULL;
	fci.function_name = &z_fname;
	fci.retval_ptr_ptr = &retval;
	fci.param_count = num_args;
	fci.params = params;
	fci.no_separation = 1;
	fci.symbol_table = NULL;

	fcic.initialized = 1;
	fcic.function_handler = func_ptr;
	fcic.calling_scope = NULL;
	fcic.called_scope = NULL;
	fcic.object_ptr = NULL;

	result = zend_call_function(&fci, &fcic TSRMLS_CC);
	
	if (result == FAILURE) {
		RETVAL_FALSE;
	} else {
		ZVAL_ZVAL(return_value, retval, 1, 1);
	}

	efree(params);
	return result;
} /* }}} */

#define FileFunctionCall(func_name, pass_num_args, arg2) /* {{{ */ \
{ \
	zend_function *func_ptr; \
	int ret; \
	ret = zend_hash_find(EG(function_table), #func_name, sizeof(#func_name), (void **) &func_ptr); \
	if (ret != SUCCESS) { \
		zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC, "Internal error, function '%s' not found. Please report", #func_name); \
		return; \
	} \
	spl_filesystem_file_call(intern, func_ptr, pass_num_args, return_value, arg2 TSRMLS_CC); \
} /* }}} */

static int spl_filesystem_file_read_csv(spl_filesystem_object *intern, char delimiter, char enclosure, char escape, zval *return_value TSRMLS_DC) /* {{{ */
{
	int ret = SUCCESS;
	
	do {
		ret = spl_filesystem_file_read(intern, 1 TSRMLS_CC);
	} while (ret == SUCCESS && !intern->u.file.current_line_len && SPL_HAS_FLAG(intern->flags, SPL_FILE_OBJECT_SKIP_EMPTY));
	
	if (ret == SUCCESS) {
		size_t buf_len = intern->u.file.current_line_len;
		char *buf = estrndup(intern->u.file.current_line, buf_len);

		if (intern->u.file.current_zval) {
			zval_ptr_dtor(&intern->u.file.current_zval);
		}
		ALLOC_INIT_ZVAL(intern->u.file.current_zval);

		php_fgetcsv(intern->u.file.stream, delimiter, enclosure, escape, buf_len, buf, intern->u.file.current_zval TSRMLS_CC);
		if (return_value) {
			if (Z_TYPE_P(return_value) != IS_NULL) {
				zval_dtor(return_value);
				ZVAL_NULL(return_value);
			}
			ZVAL_ZVAL(return_value, intern->u.file.current_zval, 1, 0);
		}
	}
	return ret;
}
/* }}} */

static int spl_filesystem_file_read_line_ex(zval * this_ptr, spl_filesystem_object *intern, int silent TSRMLS_DC) /* {{{ */
{
	zval *retval = NULL;

	/* 1) use fgetcsv? 2) overloaded call the function, 3) do it directly */
	if (SPL_HAS_FLAG(intern->flags, SPL_FILE_OBJECT_READ_CSV) || intern->u.file.func_getCurr->common.scope != spl_ce_SplFileObject) {
		if (php_stream_eof(intern->u.file.stream)) {
			if (!silent) {
				zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC, "Cannot read from file %s", intern->file_name);
			}
			return FAILURE;
		}
		if (SPL_HAS_FLAG(intern->flags, SPL_FILE_OBJECT_READ_CSV)) {
			return spl_filesystem_file_read_csv(intern, intern->u.file.delimiter, intern->u.file.enclosure, intern->u.file.escape, NULL TSRMLS_CC);
		} else {
			zend_call_method_with_0_params(&this_ptr, Z_OBJCE_P(getThis()), &intern->u.file.func_getCurr, "getCurrentLine", &retval);
		}
		if (retval) {
			if (intern->u.file.current_line || intern->u.file.current_zval) {
				intern->u.file.current_line_num++;
			}
			spl_filesystem_file_free_line(intern TSRMLS_CC);
			if (Z_TYPE_P(retval) == IS_STRING) {
				intern->u.file.current_line = estrndup(Z_STRVAL_P(retval), Z_STRLEN_P(retval));
				intern->u.file.current_line_len = Z_STRLEN_P(retval);
			} else {
				MAKE_STD_ZVAL(intern->u.file.current_zval);
				ZVAL_ZVAL(intern->u.file.current_zval, retval, 1, 0);
			}
			zval_ptr_dtor(&retval);
			return SUCCESS;
		} else {
			return FAILURE;
		}
	} else {
		return spl_filesystem_file_read(intern, silent TSRMLS_CC);
	}
} /* }}} */

static int spl_filesystem_file_is_empty_line(spl_filesystem_object *intern TSRMLS_DC) /* {{{ */
{
	if (intern->u.file.current_line) {
		return intern->u.file.current_line_len == 0;
	} else if (intern->u.file.current_zval) {
		switch(Z_TYPE_P(intern->u.file.current_zval)) {
		case IS_STRING:
			return Z_STRLEN_P(intern->u.file.current_zval) == 0;
		case IS_ARRAY:
			if (SPL_HAS_FLAG(intern->flags, SPL_FILE_OBJECT_READ_CSV)
			&& zend_hash_num_elements(Z_ARRVAL_P(intern->u.file.current_zval)) == 1) {
				zval ** first = Z_ARRVAL_P(intern->u.file.current_zval)->pListHead->pData;
					
				return Z_TYPE_PP(first) == IS_STRING && Z_STRLEN_PP(first) == 0;
			}
			return zend_hash_num_elements(Z_ARRVAL_P(intern->u.file.current_zval)) == 0;
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

static int spl_filesystem_file_read_line(zval * this_ptr, spl_filesystem_object *intern, int silent TSRMLS_DC) /* {{{ */
{
	int ret = spl_filesystem_file_read_line_ex(this_ptr, intern, silent TSRMLS_CC);

	while (SPL_HAS_FLAG(intern->flags, SPL_FILE_OBJECT_SKIP_EMPTY) && ret == SUCCESS && spl_filesystem_file_is_empty_line(intern TSRMLS_CC)) {
		spl_filesystem_file_free_line(intern TSRMLS_CC);
		ret = spl_filesystem_file_read_line_ex(this_ptr, intern, silent TSRMLS_CC);
	}
	
	return ret;
}
/* }}} */

static void spl_filesystem_file_rewind(zval * this_ptr, spl_filesystem_object *intern TSRMLS_DC) /* {{{ */
{
	if (-1 == php_stream_rewind(intern->u.file.stream)) {
		zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC, "Cannot rewind file %s", intern->file_name);
	} else {
		spl_filesystem_file_free_line(intern TSRMLS_CC);
		intern->u.file.current_line_num = 0;
	}
	if (SPL_HAS_FLAG(intern->flags, SPL_FILE_OBJECT_READ_AHEAD)) {
		spl_filesystem_file_read_line(this_ptr, intern, 1 TSRMLS_CC);
	}
} /* }}} */

/* {{{ proto void SplFileObject::__construct(string filename [, string mode = 'r' [, bool use_include_path  [, resource context]]]])
   Construct a new file object */
SPL_METHOD(SplFileObject, __construct)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	zend_bool use_include_path = 0;
	char *p1, *p2;
	char *tmp_path;
	int   tmp_path_len;
	zend_error_handling error_handling;

	zend_replace_error_handling(EH_THROW, spl_ce_RuntimeException, &error_handling TSRMLS_CC);

	intern->u.file.open_mode = NULL;
	intern->u.file.open_mode_len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "p|sbr!", 
			&intern->file_name, &intern->file_name_len,
			&intern->u.file.open_mode, &intern->u.file.open_mode_len, 
			&use_include_path, &intern->u.file.zcontext) == FAILURE) {		
		intern->u.file.open_mode = NULL;
		intern->file_name = NULL;
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}
	
	if (intern->u.file.open_mode == NULL) {
		intern->u.file.open_mode = "r";
		intern->u.file.open_mode_len = 1;
	}

	if (spl_filesystem_file_open(intern, use_include_path, 0 TSRMLS_CC) == SUCCESS) {
		tmp_path_len = strlen(intern->u.file.stream->orig_path);

		if (tmp_path_len > 1 && IS_SLASH_AT(intern->u.file.stream->orig_path, tmp_path_len-1)) {
			tmp_path_len--;
		}

		tmp_path = estrndup(intern->u.file.stream->orig_path, tmp_path_len);

		p1 = strrchr(tmp_path, '/');
#if defined(PHP_WIN32) || defined(NETWARE)
		p2 = strrchr(tmp_path, '\\');
#else
		p2 = 0;
#endif
		if (p1 || p2) {
			intern->_path_len = (p1 > p2 ? p1 : p2) - tmp_path;
		} else {
			intern->_path_len = 0;
		}

		efree(tmp_path);

		intern->_path = estrndup(intern->u.file.stream->orig_path, intern->_path_len);
	}

	zend_restore_error_handling(&error_handling TSRMLS_CC);

} /* }}} */

/* {{{ proto void SplTempFileObject::__construct([int max_memory])
   Construct a new temp file object */
SPL_METHOD(SplTempFileObject, __construct)
{
	long max_memory = PHP_STREAM_MAX_MEM;
	char tmp_fname[48];
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	zend_error_handling error_handling;

	zend_replace_error_handling(EH_THROW, spl_ce_RuntimeException, &error_handling TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &max_memory) == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}

	if (max_memory < 0) {
		intern->file_name = "php://memory";
		intern->file_name_len = 12;
	} else if (ZEND_NUM_ARGS()) {
		intern->file_name_len = slprintf(tmp_fname, sizeof(tmp_fname), "php://temp/maxmemory:%ld", max_memory);
		intern->file_name = tmp_fname;
	} else {
		intern->file_name = "php://temp";
		intern->file_name_len = 10;
	}
	intern->u.file.open_mode = "wb";
	intern->u.file.open_mode_len = 1;
	intern->u.file.zcontext = NULL;
	
	if (spl_filesystem_file_open(intern, 0, 0 TSRMLS_CC) == SUCCESS) {
		intern->_path_len = 0;
		intern->_path = estrndup("", 0);
	}
	zend_restore_error_handling(&error_handling TSRMLS_CC);
} /* }}} */

/* {{{ proto void SplFileObject::rewind()
   Rewind the file and read the first line */
SPL_METHOD(SplFileObject, rewind)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	spl_filesystem_file_rewind(getThis(), intern TSRMLS_CC);
} /* }}} */

/* {{{ proto void SplFileObject::eof()
   Return whether end of file is reached */
SPL_METHOD(SplFileObject, eof)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_BOOL(php_stream_eof(intern->u.file.stream));
} /* }}} */

/* {{{ proto void SplFileObject::valid()
   Return !eof() */
SPL_METHOD(SplFileObject, valid)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (SPL_HAS_FLAG(intern->flags, SPL_FILE_OBJECT_READ_AHEAD)) {
		RETURN_BOOL(intern->u.file.current_line || intern->u.file.current_zval);
	} else {
		RETVAL_BOOL(!php_stream_eof(intern->u.file.stream));
	}
} /* }}} */

/* {{{ proto string SplFileObject::fgets()
   Rturn next line from file */
SPL_METHOD(SplFileObject, fgets)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (spl_filesystem_file_read(intern, 0 TSRMLS_CC) == FAILURE) {
		RETURN_FALSE;
	}
	RETURN_STRINGL(intern->u.file.current_line, intern->u.file.current_line_len, 1);
} /* }}} */

/* {{{ proto string SplFileObject::current()
   Return current line from file */
SPL_METHOD(SplFileObject, current)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (!intern->u.file.current_line && !intern->u.file.current_zval) {
		spl_filesystem_file_read_line(getThis(), intern, 1 TSRMLS_CC);
	}
	if (intern->u.file.current_line && (!SPL_HAS_FLAG(intern->flags, SPL_FILE_OBJECT_READ_CSV) || !intern->u.file.current_zval)) {
		RETURN_STRINGL(intern->u.file.current_line, intern->u.file.current_line_len, 1);
	} else if (intern->u.file.current_zval) {
		RETURN_ZVAL(intern->u.file.current_zval, 1, 0);
	}
	RETURN_FALSE;
} /* }}} */

/* {{{ proto int SplFileObject::key()
   Return line number */
SPL_METHOD(SplFileObject, key)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

/*	Do not read the next line to support correct counting with fgetc()
	if (!intern->current_line) {
		spl_filesystem_file_read_line(getThis(), intern, 1 TSRMLS_CC);
	} */
	RETURN_LONG(intern->u.file.current_line_num);
} /* }}} */

/* {{{ proto void SplFileObject::next()
   Read next line */
SPL_METHOD(SplFileObject, next)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	spl_filesystem_file_free_line(intern TSRMLS_CC);
	if (SPL_HAS_FLAG(intern->flags, SPL_FILE_OBJECT_READ_AHEAD)) {
		spl_filesystem_file_read_line(getThis(), intern, 1 TSRMLS_CC);
	}
	intern->u.file.current_line_num++;
} /* }}} */

/* {{{ proto void SplFileObject::setFlags(int flags)
   Set file handling flags */
SPL_METHOD(SplFileObject, setFlags)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &intern->flags) == FAILURE) {
		return;
	}
} /* }}} */

/* {{{ proto int SplFileObject::getFlags()
   Get file handling flags */
SPL_METHOD(SplFileObject, getFlags)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_LONG(intern->flags & SPL_FILE_OBJECT_MASK);
} /* }}} */

/* {{{ proto void SplFileObject::setMaxLineLen(int max_len)
   Set maximum line length */
SPL_METHOD(SplFileObject, setMaxLineLen)
{
	long max_len;

	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &max_len) == FAILURE) {
		return;
	}

	if (max_len < 0) {
		zend_throw_exception_ex(spl_ce_DomainException, 0 TSRMLS_CC, "Maximum line length must be greater than or equal zero");
		return;
	}
	
	intern->u.file.max_line_len = max_len;
} /* }}} */

/* {{{ proto int SplFileObject::getMaxLineLen()
   Get maximum line length */
SPL_METHOD(SplFileObject, getMaxLineLen)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_LONG((long)intern->u.file.max_line_len);
} /* }}} */

/* {{{ proto bool SplFileObject::hasChildren()
   Return false */
SPL_METHOD(SplFileObject, hasChildren)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	
	RETURN_FALSE;
} /* }}} */

/* {{{ proto bool SplFileObject::getChildren()
   Read NULL */
SPL_METHOD(SplFileObject, getChildren)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	/* return NULL */
} /* }}} */

/* {{{ FileFunction */
#define FileFunction(func_name) \
SPL_METHOD(SplFileObject, func_name) \
{ \
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC); \
	FileFunctionCall(func_name, ZEND_NUM_ARGS(), NULL); \
}
/* }}} */

/* {{{ proto array SplFileObject::fgetcsv([string delimiter [, string enclosure [, escape = '\\']]])
   Return current line as csv */
SPL_METHOD(SplFileObject, fgetcsv)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	char delimiter = intern->u.file.delimiter, enclosure = intern->u.file.enclosure, escape = intern->u.file.escape;
	char *delim = NULL, *enclo = NULL, *esc = NULL;
	int d_len = 0, e_len = 0, esc_len = 0;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|sss", &delim, &d_len, &enclo, &e_len, &esc, &esc_len) == SUCCESS) {
		switch(ZEND_NUM_ARGS())
		{
		case 3:
			if (esc_len != 1) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "escape must be a character");
				RETURN_FALSE;
			}
			escape = esc[0];
			/* no break */
		case 2:
			if (e_len != 1) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "enclosure must be a character");
				RETURN_FALSE;
			}
			enclosure = enclo[0];
			/* no break */
		case 1:
			if (d_len != 1) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "delimiter must be a character");
				RETURN_FALSE;
			}
			delimiter = delim[0];
			/* no break */
		case 0:
			break;
		}
		spl_filesystem_file_read_csv(intern, delimiter, enclosure, escape, return_value TSRMLS_CC);
	}
}
/* }}} */

/* {{{ proto int SplFileObject::fputcsv(array fields, [string delimiter [, string enclosure]])
   Output a field array as a CSV line */
SPL_METHOD(SplFileObject, fputcsv)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	char delimiter = intern->u.file.delimiter, enclosure = intern->u.file.enclosure, escape = intern->u.file.escape;
	char *delim = NULL, *enclo = NULL;
	int d_len = 0, e_len = 0, ret;
	zval *fields = NULL;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a|ss", &fields, &delim, &d_len, &enclo, &e_len) == SUCCESS) {
		switch(ZEND_NUM_ARGS())
		{
		case 3:
			if (e_len != 1) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "enclosure must be a character");
				RETURN_FALSE;
			}
			enclosure = enclo[0];
			/* no break */
		case 2:
			if (d_len != 1) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "delimiter must be a character");
				RETURN_FALSE;
			}
			delimiter = delim[0];
			/* no break */
		case 1:
		case 0:
			break;
		}
		ret = php_fputcsv(intern->u.file.stream, fields, delimiter, enclosure, escape TSRMLS_CC);
		RETURN_LONG(ret);
	}
}
/* }}} */

/* {{{ proto void SplFileObject::setCsvControl([string delimiter = ',' [, string enclosure = '"' [, string escape = '\\']]])
   Set the delimiter and enclosure character used in fgetcsv */
SPL_METHOD(SplFileObject, setCsvControl)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	char delimiter = ',', enclosure = '"', escape='\\';
	char *delim = NULL, *enclo = NULL, *esc = NULL;
	int d_len = 0, e_len = 0, esc_len = 0;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|sss", &delim, &d_len, &enclo, &e_len, &esc, &esc_len) == SUCCESS) {
		switch(ZEND_NUM_ARGS())
		{
		case 3:
			if (esc_len != 1) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "escape must be a character");
				RETURN_FALSE;
			}
			escape = esc[0];
			/* no break */
		case 2:
			if (e_len != 1) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "enclosure must be a character");
				RETURN_FALSE;
			}
			enclosure = enclo[0];
			/* no break */
		case 1:
			if (d_len != 1) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "delimiter must be a character");
				RETURN_FALSE;
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

/* {{{ proto array SplFileObject::getCsvControl()
   Get the delimiter and enclosure character used in fgetcsv */
SPL_METHOD(SplFileObject, getCsvControl)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	char delimiter[2], enclosure[2];

	array_init(return_value);
	
	delimiter[0] = intern->u.file.delimiter;
	delimiter[1] = '\0';
	enclosure[0] = intern->u.file.enclosure;
	enclosure[1] = '\0';

	add_next_index_string(return_value, delimiter, 1);
	add_next_index_string(return_value, enclosure, 1);
}
/* }}} */

/* {{{ proto bool SplFileObject::flock(int operation [, int &wouldblock])
   Portable file locking */
FileFunction(flock)
/* }}} */

/* {{{ proto bool SplFileObject::fflush()
   Flush the file */
SPL_METHOD(SplFileObject, fflush)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	RETURN_BOOL(!php_stream_flush(intern->u.file.stream));
} /* }}} */

/* {{{ proto int SplFileObject::ftell()
   Return current file position */
SPL_METHOD(SplFileObject, ftell)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);	
	long ret = php_stream_tell(intern->u.file.stream);

	if (ret == -1) {
		RETURN_FALSE;
	} else {
		RETURN_LONG(ret);
	}
} /* }}} */

/* {{{ proto int SplFileObject::fseek(int pos [, int whence = SEEK_SET])
   Return current file position */
SPL_METHOD(SplFileObject, fseek)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	long pos, whence = SEEK_SET;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|l", &pos, &whence) == FAILURE) {
		return;
	}

	spl_filesystem_file_free_line(intern TSRMLS_CC);
	RETURN_LONG(php_stream_seek(intern->u.file.stream, pos, whence));
} /* }}} */

/* {{{ proto int SplFileObject::fgetc()
   Get a character form the file */
SPL_METHOD(SplFileObject, fgetc)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	char buf[2];
	int result;

	spl_filesystem_file_free_line(intern TSRMLS_CC);

	result = php_stream_getc(intern->u.file.stream);

	if (result == EOF) {
		RETVAL_FALSE;
	} else {
		if (result == '\n') {
			intern->u.file.current_line_num++;
		}
		buf[0] = result;
		buf[1] = '\0';

		RETURN_STRINGL(buf, 1, 1);
	}
} /* }}} */

/* {{{ proto string SplFileObject::fgetss([string allowable_tags])
   Get a line from file pointer and strip HTML tags */
SPL_METHOD(SplFileObject, fgetss)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	zval *arg2 = NULL;
	MAKE_STD_ZVAL(arg2);

	if (intern->u.file.max_line_len > 0) {
		ZVAL_LONG(arg2, intern->u.file.max_line_len);
	} else {
		ZVAL_LONG(arg2, 1024);
	}

	spl_filesystem_file_free_line(intern TSRMLS_CC);
	intern->u.file.current_line_num++;

	FileFunctionCall(fgetss, ZEND_NUM_ARGS(), arg2);

	zval_ptr_dtor(&arg2);
} /* }}} */

/* {{{ proto int SplFileObject::fpassthru()
   Output all remaining data from a file pointer */
SPL_METHOD(SplFileObject, fpassthru)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	RETURN_LONG(php_stream_passthru(intern->u.file.stream));
} /* }}} */

/* {{{ proto bool SplFileObject::fscanf(string format [, string ...])
   Implements a mostly ANSI compatible fscanf() */
SPL_METHOD(SplFileObject, fscanf)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	spl_filesystem_file_free_line(intern TSRMLS_CC);
	intern->u.file.current_line_num++;

	FileFunctionCall(fscanf, ZEND_NUM_ARGS(), NULL);
}
/* }}} */

/* {{{ proto mixed SplFileObject::fwrite(string str [, int length])
   Binary-safe file write */
SPL_METHOD(SplFileObject, fwrite)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	char *str;
	int str_len;
	long length = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &str, &str_len, &length) == FAILURE) {
		return;
	}

	if (ZEND_NUM_ARGS() > 1) {
		str_len = MAX(0, MIN(length, str_len));
	}
	if (!str_len) {
		RETURN_LONG(0);
	}

	RETURN_LONG(php_stream_write(intern->u.file.stream, str, str_len));
} /* }}} */

/* {{{ proto bool SplFileObject::fstat()
   Stat() on a filehandle */
FileFunction(fstat)
/* }}} */

/* {{{ proto bool SplFileObject::ftruncate(int size)
   Truncate file to 'size' length */
SPL_METHOD(SplFileObject, ftruncate)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	long size;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &size) == FAILURE) {
		return;
	}

	if (!php_stream_truncate_supported(intern->u.file.stream)) {
		zend_throw_exception_ex(spl_ce_LogicException, 0 TSRMLS_CC, "Can't truncate file %s", intern->file_name);
		RETURN_FALSE;
	}
	
	RETURN_BOOL(0 == php_stream_truncate_set_size(intern->u.file.stream, size));
} /* }}} */

/* {{{ proto void SplFileObject::seek(int line_pos)
   Seek to specified line */
SPL_METHOD(SplFileObject, seek)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	long line_pos;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &line_pos) == FAILURE) {
		return;
	}
	if (line_pos < 0) {
		zend_throw_exception_ex(spl_ce_LogicException, 0 TSRMLS_CC, "Can't seek file %s to negative line %ld", intern->file_name, line_pos);
		RETURN_FALSE;		
	}
	
	spl_filesystem_file_rewind(getThis(), intern TSRMLS_CC);
	
	while(intern->u.file.current_line_num < line_pos) {
		if (spl_filesystem_file_read_line(getThis(), intern, 1 TSRMLS_CC) == FAILURE) {
			break;
		}
	}
} /* }}} */

/* {{{ Function/Class/Method definitions */
ZEND_BEGIN_ARG_INFO_EX(arginfo_file_object___construct, 0, 0, 1)
	ZEND_ARG_INFO(0, file_name)
	ZEND_ARG_INFO(0, open_mode)
	ZEND_ARG_INFO(0, use_include_path)
	ZEND_ARG_INFO(0, context)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_file_object_setFlags, 0)
	ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_file_object_setMaxLineLen, 0)
	ZEND_ARG_INFO(0, max_len)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_file_object_fgetcsv, 0, 0, 0)
	ZEND_ARG_INFO(0, delimiter)
	ZEND_ARG_INFO(0, enclosure)
	ZEND_ARG_INFO(0, escape)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_file_object_fputcsv, 0, 0, 1)
	ZEND_ARG_INFO(0, fields)
	ZEND_ARG_INFO(0, delimiter)
	ZEND_ARG_INFO(0, enclosure)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_file_object_flock, 0, 0, 1) 
	ZEND_ARG_INFO(0, operation)
	ZEND_ARG_INFO(1, wouldblock)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_file_object_fseek, 0, 0, 1) 
	ZEND_ARG_INFO(0, pos)
	ZEND_ARG_INFO(0, whence)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_file_object_fgetss, 0, 0, 0) 
	ZEND_ARG_INFO(0, allowable_tags)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_file_object_fscanf, 0, 0, 1) 
	ZEND_ARG_INFO(0, format)
	ZEND_ARG_VARIADIC_INFO(1, vars)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_file_object_fwrite, 0, 0, 1) 
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_file_object_ftruncate, 0, 0, 1) 
	ZEND_ARG_INFO(0, size)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_file_object_seek, 0, 0, 1) 
	ZEND_ARG_INFO(0, line_pos)
ZEND_END_ARG_INFO()

static const zend_function_entry spl_SplFileObject_functions[] = {
	SPL_ME(SplFileObject, __construct,    arginfo_file_object___construct,   ZEND_ACC_PUBLIC)
	SPL_ME(SplFileObject, rewind,         arginfo_splfileinfo_void,          ZEND_ACC_PUBLIC)
	SPL_ME(SplFileObject, eof,            arginfo_splfileinfo_void,          ZEND_ACC_PUBLIC)
	SPL_ME(SplFileObject, valid,          arginfo_splfileinfo_void,          ZEND_ACC_PUBLIC)
	SPL_ME(SplFileObject, fgets,          arginfo_splfileinfo_void,          ZEND_ACC_PUBLIC)
	SPL_ME(SplFileObject, fgetcsv,        arginfo_file_object_fgetcsv,       ZEND_ACC_PUBLIC)
	SPL_ME(SplFileObject, fputcsv,        arginfo_file_object_fputcsv,       ZEND_ACC_PUBLIC)
	SPL_ME(SplFileObject, setCsvControl,  arginfo_file_object_fgetcsv,       ZEND_ACC_PUBLIC)
	SPL_ME(SplFileObject, getCsvControl,  arginfo_splfileinfo_void,          ZEND_ACC_PUBLIC)
	SPL_ME(SplFileObject, flock,          arginfo_file_object_flock,         ZEND_ACC_PUBLIC)
	SPL_ME(SplFileObject, fflush,         arginfo_splfileinfo_void,          ZEND_ACC_PUBLIC)
	SPL_ME(SplFileObject, ftell,          arginfo_splfileinfo_void,          ZEND_ACC_PUBLIC)
	SPL_ME(SplFileObject, fseek,          arginfo_file_object_fseek,         ZEND_ACC_PUBLIC)
	SPL_ME(SplFileObject, fgetc,          arginfo_splfileinfo_void,          ZEND_ACC_PUBLIC)
	SPL_ME(SplFileObject, fpassthru,      arginfo_splfileinfo_void,          ZEND_ACC_PUBLIC)
	SPL_ME(SplFileObject, fgetss,         arginfo_file_object_fgetss,        ZEND_ACC_PUBLIC)
	SPL_ME(SplFileObject, fscanf,         arginfo_file_object_fscanf,        ZEND_ACC_PUBLIC)
	SPL_ME(SplFileObject, fwrite,         arginfo_file_object_fwrite,        ZEND_ACC_PUBLIC)
	SPL_ME(SplFileObject, fstat,          arginfo_splfileinfo_void,          ZEND_ACC_PUBLIC)
	SPL_ME(SplFileObject, ftruncate,      arginfo_file_object_ftruncate,     ZEND_ACC_PUBLIC)
	SPL_ME(SplFileObject, current,        arginfo_splfileinfo_void,          ZEND_ACC_PUBLIC)
	SPL_ME(SplFileObject, key,            arginfo_splfileinfo_void,          ZEND_ACC_PUBLIC)
	SPL_ME(SplFileObject, next,           arginfo_splfileinfo_void,          ZEND_ACC_PUBLIC)
	SPL_ME(SplFileObject, setFlags,       arginfo_file_object_setFlags,      ZEND_ACC_PUBLIC)
	SPL_ME(SplFileObject, getFlags,       arginfo_splfileinfo_void,          ZEND_ACC_PUBLIC)
	SPL_ME(SplFileObject, setMaxLineLen,  arginfo_file_object_setMaxLineLen, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileObject, getMaxLineLen,  arginfo_splfileinfo_void,          ZEND_ACC_PUBLIC)
	SPL_ME(SplFileObject, hasChildren,    arginfo_splfileinfo_void,          ZEND_ACC_PUBLIC)
	SPL_ME(SplFileObject, getChildren,    arginfo_splfileinfo_void,          ZEND_ACC_PUBLIC)
	SPL_ME(SplFileObject, seek,           arginfo_file_object_seek,          ZEND_ACC_PUBLIC)
	/* mappings */
	SPL_MA(SplFileObject, getCurrentLine, SplFileObject, fgets,      arginfo_splfileinfo_void, ZEND_ACC_PUBLIC)
	SPL_MA(SplFileObject, __toString,     SplFileObject, current,    arginfo_splfileinfo_void, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

ZEND_BEGIN_ARG_INFO_EX(arginfo_temp_file_object___construct, 0, 0, 0)
	ZEND_ARG_INFO(0, max_memory)
ZEND_END_ARG_INFO()

static const zend_function_entry spl_SplTempFileObject_functions[] = {
	SPL_ME(SplTempFileObject, __construct, arginfo_temp_file_object___construct,  ZEND_ACC_PUBLIC)
	PHP_FE_END
};
/* }}} */

/* {{{ PHP_MINIT_FUNCTION(spl_directory)
 */
PHP_MINIT_FUNCTION(spl_directory)
{
	REGISTER_SPL_STD_CLASS_EX(SplFileInfo, spl_filesystem_object_new, spl_SplFileInfo_functions);
	memcpy(&spl_filesystem_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	spl_filesystem_object_handlers.clone_obj       = spl_filesystem_object_clone;
	spl_filesystem_object_handlers.cast_object     = spl_filesystem_object_cast;
	spl_filesystem_object_handlers.get_debug_info  = spl_filesystem_object_get_debug_info;
	spl_ce_SplFileInfo->serialize = zend_class_serialize_deny;
	spl_ce_SplFileInfo->unserialize = zend_class_unserialize_deny;

	REGISTER_SPL_SUB_CLASS_EX(DirectoryIterator, SplFileInfo, spl_filesystem_object_new, spl_DirectoryIterator_functions);
	zend_class_implements(spl_ce_DirectoryIterator TSRMLS_CC, 1, zend_ce_iterator);
	REGISTER_SPL_IMPLEMENTS(DirectoryIterator, SeekableIterator);

	spl_ce_DirectoryIterator->get_iterator = spl_filesystem_dir_get_iterator;

	REGISTER_SPL_SUB_CLASS_EX(FilesystemIterator, DirectoryIterator, spl_filesystem_object_new, spl_FilesystemIterator_functions);

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

	REGISTER_SPL_SUB_CLASS_EX(RecursiveDirectoryIterator, FilesystemIterator, spl_filesystem_object_new, spl_RecursiveDirectoryIterator_functions);
	REGISTER_SPL_IMPLEMENTS(RecursiveDirectoryIterator, RecursiveIterator);
	
	memcpy(&spl_filesystem_object_check_handlers, &spl_filesystem_object_handlers, sizeof(zend_object_handlers));
	spl_filesystem_object_check_handlers.get_method = spl_filesystem_object_get_method_check;

#ifdef HAVE_GLOB
	REGISTER_SPL_SUB_CLASS_EX(GlobIterator, FilesystemIterator, spl_filesystem_object_new_check, spl_GlobIterator_functions);
	REGISTER_SPL_IMPLEMENTS(GlobIterator, Countable);
#endif

	REGISTER_SPL_SUB_CLASS_EX(SplFileObject, SplFileInfo, spl_filesystem_object_new_check, spl_SplFileObject_functions);
	REGISTER_SPL_IMPLEMENTS(SplFileObject, RecursiveIterator);
	REGISTER_SPL_IMPLEMENTS(SplFileObject, SeekableIterator);

	REGISTER_SPL_CLASS_CONST_LONG(SplFileObject, "DROP_NEW_LINE", SPL_FILE_OBJECT_DROP_NEW_LINE);
	REGISTER_SPL_CLASS_CONST_LONG(SplFileObject, "READ_AHEAD",    SPL_FILE_OBJECT_READ_AHEAD);
	REGISTER_SPL_CLASS_CONST_LONG(SplFileObject, "SKIP_EMPTY",    SPL_FILE_OBJECT_SKIP_EMPTY);
	REGISTER_SPL_CLASS_CONST_LONG(SplFileObject, "READ_CSV",      SPL_FILE_OBJECT_READ_CSV);
	
	REGISTER_SPL_SUB_CLASS_EX(SplTempFileObject, SplFileObject, spl_filesystem_object_new_check, spl_SplTempFileObject_functions);
	return SUCCESS;
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
