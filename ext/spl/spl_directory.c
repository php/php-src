/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2008 The PHP Group                                |
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

/* declare the class handlers */
static zend_object_handlers spl_filesystem_object_handlers;

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
	
	if (intern->_path.v) {
		efree(intern->_path.v);
	}
	if (intern->file_name.v) {
		efree(intern->file_name.v);
	}
	switch(intern->type) {
	case SPL_FS_INFO:
		break;
	case SPL_FS_DIR:
		if (intern->u.dir.dirp) {
			php_stream_close(intern->u.dir.dirp);
			intern->u.dir.dirp = NULL;
		}
		if (intern->u.dir.sub_path.v) {
			efree(intern->u.dir.sub_path.v);
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
		}
		spl_filesystem_file_free_line(intern TSRMLS_CC);
		break;
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
	zval *tmp;

	intern = emalloc(sizeof(spl_filesystem_object));
	memset(intern, 0, sizeof(spl_filesystem_object));
	/* intern->type = SPL_FS_INFO; done by set 0 */
	intern->file_class = spl_ce_SplFileObject;
	intern->info_class = spl_ce_SplFileInfo;
	if (obj) *obj = intern;

	zend_object_std_init(&intern->std, class_type TSRMLS_CC);
	zend_hash_copy(intern->std.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));

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

PHPAPI zstr spl_filesystem_object_get_path(spl_filesystem_object *intern, int *len, zend_uchar *type TSRMLS_DC) /* {{{ */
{
	if (intern->type == SPL_FS_DIR) {
		if (php_stream_is(intern->u.dir.dirp ,&php_glob_stream_ops)) {
			zstr ret;
			if (type) {
				*type = IS_STRING;
			}
			ret.s = php_glob_stream_get_path(intern->u.dir.dirp, 0, len);
			return ret;
		}
	}
	if (len) {
		*len = intern->_path_len;
	}
	if (type) {
		*type = intern->_path_type;
	}
	return intern->_path;
} /* }}} */

static inline void spl_filesystem_object_get_file_name(spl_filesystem_object *intern TSRMLS_DC) /* {{{ */
{
	zstr path;
	zend_uchar path_type;
	if (!intern->file_name.v) {
		switch (intern->type) {
		case SPL_FS_INFO:
		case SPL_FS_FILE:
			php_error_docref(NULL TSRMLS_CC, E_ERROR, "Object not initialized");
			break;
		case SPL_FS_DIR:
			path = spl_filesystem_object_get_path(intern, NULL, &path_type TSRMLS_CC);
			intern->file_name_len = zspprintf(path_type, &intern->file_name, 0, "%R%c%s",
			                                  path_type, path,
			                                  DEFAULT_SLASH, intern->u.dir.entry.d_name);
			intern->file_name_type = path_type;
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

#define IS_SLASH_AT(type, zs, pos) (type == IS_UNICODE ? IS_U_SLASH(zs.u[pos]) : IS_SLASH(zs.s[pos]))

/* {{{ spl_filesystem_dir_open */
/* open a directory resource */
static void spl_filesystem_dir_open(spl_filesystem_object* intern, zend_uchar type, zstr path, int path_len TSRMLS_DC)
{
	int options = REPORT_ERRORS;

#if HELLY_0	
	if (php_stream_is(intern->u.dir.dirp, &php_glob_stream_ops)) {
		options |= STREAM_USE_GLOB_DIR_OPEN;
	}
#endif

	intern->type = SPL_FS_DIR;
	intern->_path_type= type;
	intern->_path_len = path_len;
	intern->u.dir.dirp = php_stream_u_opendir(type, path, path_len, options, NULL);

	if (intern->_path_len && IS_SLASH_AT(type, path, intern->_path_len-1)) {
		intern->_path = ezstrndup(type, path, --intern->_path_len);
	} else {
		intern->_path = ezstrndup(type, path, intern->_path_len);
	}
	intern->u.dir.index = 0;

	if (intern->u.dir.dirp == NULL) {
		/* throw exception: should've been already happened */
		intern->u.dir.entry.d_name[0] = '\0';
	} else {
		spl_filesystem_dir_read(intern TSRMLS_CC);
	}
}
/* }}} */

static int spl_filesystem_file_open(spl_filesystem_object *intern, int use_include_path, int silent TSRMLS_DC) /* {{{ */
{
	intern->type = SPL_FS_FILE;
	intern->u.file.context = php_stream_context_from_zval(intern->u.file.zcontext, 0);
	intern->u.file.stream = php_stream_u_open_wrapper(intern->file_name_type, intern->file_name, intern->file_name_len, intern->u.file.open_mode, (use_include_path ? USE_PATH : 0) | REPORT_ERRORS, NULL, intern->u.file.context);

	if (!intern->file_name_len || !intern->u.file.stream) {
		if (!EG(exception)) {
			zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC, "Cannot open file '%R'", intern->file_name_type, intern->file_name_len ? intern->file_name : EMPTY_ZSTR);
		}
		intern->file_name = NULL_ZSTR; /* until here it is not a copy */
		intern->u.file.open_mode = NULL;
		return FAILURE;
	}

	if (intern->u.file.zcontext) {
		zend_list_addref(Z_RESVAL_P(intern->u.file.zcontext));
	}

	if (intern->file_name_len && IS_SLASH_AT(intern->file_name_type, intern->file_name, intern->file_name_len-1)) {
		intern->file_name_len--;
	}

	intern->file_name = ezstrndup(intern->file_name_type, intern->file_name, intern->file_name_len);
	intern->u.file.open_mode = estrndup(intern->u.file.open_mode, intern->u.file.open_mode_len);

	/* avoid reference counting in debug mode, thus do it manually */
	ZVAL_RESOURCE(&intern->u.file.zresource, php_stream_get_resource_id(intern->u.file.stream));
	Z_SET_REFCOUNT(intern->u.file.zresource, 1);
	
	intern->u.file.delimiter = ',';
	intern->u.file.enclosure = '"';

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

	old_object = zend_objects_get_address(zobject TSRMLS_CC);
	source = (spl_filesystem_object*)old_object;

	new_obj_val = spl_filesystem_object_new_ex(old_object->ce, &intern TSRMLS_CC);
	new_object = &intern->std;

	switch (source->type) {
	case SPL_FS_INFO:
		intern->_path_type = source->_path_type;
		intern->_path_len = source->_path_len;
		intern->_path = ezstrndup(source->_path_type, source->_path, source->_path_len);
		intern->file_name_type = source->file_name_type;
		intern->file_name_len = source->file_name_len;
		intern->file_name = ezstrndup(source->file_name_type, source->file_name, intern->file_name_len);
		break;
	case SPL_FS_DIR:
		spl_filesystem_dir_open(intern, source->_path_type, source->_path, source->_path_len TSRMLS_CC);
		break;
	case SPL_FS_FILE:
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "An object of class %v cannot be cloned", old_object->ce->name);
		break;
	}
	
	intern->file_class = source->file_class;
	intern->info_class = source->info_class;
	intern->flags = source->flags;
	intern->oth = source->oth;
	intern->oth_handler = source->oth_handler;

	zend_objects_clone_members(new_object, new_obj_val, old_object, handle TSRMLS_CC);

	if (intern->oth_handler && intern->oth_handler->clone) {
		intern->oth_handler->clone(source, intern TSRMLS_CC);
	}

	return new_obj_val;
}
/* }}} */

void spl_filesystem_info_set_filename(spl_filesystem_object *intern, zend_uchar type, zstr path, int len, int use_copy TSRMLS_DC) /* {{{ */
{
	zstr p1, p2;

	intern->file_name_type = type;
	intern->file_name = use_copy ? ezstrndup(type, path, len) : path;
	intern->file_name_len = len;

	if (type == IS_UNICODE) {
		p1.u = u_strrchr(path.u, '/');
	} else {
		p1.s = strrchr(path.s, '/');
	}
#if defined(PHP_WIN32) || defined(NETWARE)
	if (type == IS_UNICODE) {
		p2.u = u_strrchr(path.u, '\\');
	} else {
		p2.s = strrchr(path.s, '\\');
	}
#else
	p2.v = 0;
#endif
	if (p1.v || p2.v) {
		if (type == IS_UNICODE) {
			intern->_path_len = (p1.u > p2.u ? p1.u : p2.u) - path.u;
		} else {
			intern->_path_len = (p1.s > p2.s ? p1.s : p2.s) - path.s;
		}
	} else {
		intern->_path_len = 0;
	}
	intern->_path_type = type;
	intern->_path = ezstrndup(type, path, intern->_path_len);
} /* }}} */

static spl_filesystem_object * spl_filesystem_object_create_info(spl_filesystem_object *source, zend_uchar file_type, zstr file_path, int file_path_len, int use_copy, zend_class_entry *ce, zval *return_value TSRMLS_DC) /* {{{ */
{
	spl_filesystem_object *intern;
	zval *arg1;

	if (!file_path.v || !file_path_len) {
#if defined(PHP_WIN32)
		zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC, "Cannot create SplFileInfo for empty path");
		if (file_path.v && !use_copy) {
			efree(file_path.v);
		}
#else
		if (file_path.v && !use_copy) {
			efree(file_path.v);
		}
		use_copy = 1;
		file_path_len = 1;
		file_path.s = "/";
		file_type = IS_STRING;
#endif
		return NULL;
	}

	php_set_error_handling(EH_THROW, spl_ce_RuntimeException TSRMLS_CC);

	ce = ce ? ce : source->info_class;
	return_value->value.obj = spl_filesystem_object_new_ex(ce, &intern TSRMLS_CC);
	Z_TYPE_P(return_value) = IS_OBJECT;

	if (ce->constructor->common.scope != spl_ce_SplFileInfo) {
		MAKE_STD_ZVAL(arg1);
		ZVAL_ZSTRL(arg1, file_type, file_path, file_path_len, use_copy);
		zend_call_method_with_1_params(&return_value, ce, &ce->constructor, "__construct", NULL, arg1);
		zval_ptr_dtor(&arg1);
	} else {
		spl_filesystem_info_set_filename(intern, file_type, file_path, file_path_len, use_copy TSRMLS_CC);
	}
	
	php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
	
	return intern;
} /* }}} */

static spl_filesystem_object * spl_filesystem_object_create_type(int ht, spl_filesystem_object *source, int type, zend_class_entry *ce, zval *return_value TSRMLS_DC) /* {{{ */
{
	spl_filesystem_object *intern;
	zend_bool use_include_path = 0;
	zval *arg1, *arg2;

	php_set_error_handling(EH_THROW, spl_ce_RuntimeException TSRMLS_CC);

	switch (source->type) {
	case SPL_FS_INFO:
	case SPL_FS_FILE:
		break;
	case SPL_FS_DIR:
		if (!source->u.dir.entry.d_name[0]) {
			zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC, "Could not open file");
			php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
			return NULL;
		}
	}

	switch (type) {
	case SPL_FS_INFO:
		ce = ce ? ce : source->info_class;
		return_value->value.obj = spl_filesystem_object_new_ex(ce, &intern TSRMLS_CC);
		Z_TYPE_P(return_value) = IS_OBJECT;

		spl_filesystem_object_get_file_name(source TSRMLS_CC);
		if (ce->constructor->common.scope != spl_ce_SplFileInfo) {
			MAKE_STD_ZVAL(arg1);
			ZVAL_ZSTRL(arg1, source->file_name_type, source->file_name, source->file_name_len, 1);
			zend_call_method_with_1_params(&return_value, ce, &ce->constructor, "__construct", NULL, arg1);
			zval_ptr_dtor(&arg1);
		} else {
			intern->_path = spl_filesystem_object_get_path(source, &intern->_path_len, &intern->_path_type TSRMLS_CC);
			intern->_path = ezstrndup(intern->_path_type, intern->_path, intern->_path_len);
			intern->file_name_type = source->file_name_type;
			intern->file_name_len = source->file_name_len;
			intern->file_name = ezstrndup(source->file_name_type, source->file_name, source->file_name_len);
		}
		break;
	case SPL_FS_FILE:
		ce = ce ? ce : source->file_class;
		return_value->value.obj = spl_filesystem_object_new_ex(ce, &intern TSRMLS_CC);
		Z_TYPE_P(return_value) = IS_OBJECT;
	
		spl_filesystem_object_get_file_name(source TSRMLS_CC);

		if (ce->constructor->common.scope != spl_ce_SplFileObject) {
			MAKE_STD_ZVAL(arg1);
			MAKE_STD_ZVAL(arg2);
			ZVAL_ZSTRL(arg1, source->file_name_type, source->file_name, source->file_name_len, 1);
			ZVAL_STRINGL(arg2, "r", 1, 1);
			zend_call_method_with_2_params(&return_value, ce, &ce->constructor, "__construct", NULL, arg1, arg2);
			zval_ptr_dtor(&arg1);
			zval_ptr_dtor(&arg2);
		} else {
			intern->_path = spl_filesystem_object_get_path(source, &intern->_path_len, &intern->_path_type TSRMLS_CC);
			intern->_path = ezstrndup(intern->_path_type, intern->_path, intern->_path_len);
			intern->file_name_type = source->file_name_type;
			intern->file_name_len = source->file_name_len;
			intern->file_name = source->file_name;
		
			intern->u.file.open_mode = "r";
			intern->u.file.open_mode_len = 1;
		
			if (ht && zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|sbr", 
					&intern->u.file.open_mode, &intern->u.file.open_mode_len, 
					&use_include_path, &intern->u.file.zcontext) == FAILURE) {
				php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
				intern->u.file.open_mode = NULL;
				zval_dtor(return_value);
				Z_TYPE_P(return_value) = IS_NULL;
				return NULL;
			}
		
			if (spl_filesystem_file_open(intern, use_include_path, 0 TSRMLS_CC) == FAILURE) {
				php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
				zval_dtor(return_value);
				Z_TYPE_P(return_value) = IS_NULL;
				return NULL;
			}
		}
		break;
	case SPL_FS_DIR:	
		php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
		zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC, "Operation not supported");
		return NULL;
	}
	php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
	return NULL;
} /* }}} */

static inline int spl_filesystem_is_dot(const char * d_name) /* {{{ */
{
	return !strcmp(d_name, ".") || !strcmp(d_name, "..");
}
/* }}} */

static int spl_filesystem_is_invalid_or_dot(const char * d_name) /* {{{ */
{
	return d_name[0] == '\0' || spl_filesystem_is_dot(d_name);
}
/* }}} */

static HashTable* spl_filesystem_object_get_debug_info(zval *obj, int *is_temp TSRMLS_DC) /* {{{{ */
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(obj TSRMLS_CC);
	HashTable *rv;
	zval *tmp, zrv;
	zstr pnstr;
	int  pnlen;
	char stmp[2];
	int path_len;
	zstr path;
	zend_uchar path_type;

	*is_temp = 1;

	ALLOC_HASHTABLE(rv);
	ZEND_INIT_SYMTABLE_EX(rv, zend_hash_num_elements(intern->std.properties) + 3, 0);

	INIT_PZVAL(&zrv);
	Z_ARRVAL(zrv) = rv;

	zend_hash_copy(rv, intern->std.properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));

	path = spl_filesystem_object_get_path(intern, &path_len, &path_type TSRMLS_CC);
	pnstr = spl_gen_private_prop_name(spl_ce_SplFileInfo, "pathName", sizeof("pathName")-1, &pnlen TSRMLS_CC);
	add_u_assoc_zstrl_ex(&zrv, ZEND_STR_TYPE, pnstr, pnlen+1, path_type, path, path_len, 1);
	efree(pnstr.v);
	if (intern->file_name.v) {
		pnstr = spl_gen_private_prop_name(spl_ce_SplFileInfo, "fileName", sizeof("fileName")-1, &pnlen TSRMLS_CC);
		add_u_assoc_zstrl_ex(&zrv, ZEND_STR_TYPE, pnstr, pnlen+1, intern->file_name_type, intern->file_name, intern->file_name_len, 1);
		efree(pnstr.v);
	}
	if (intern->type == SPL_FS_DIR) {
		pnstr = spl_gen_private_prop_name(spl_ce_DirectoryIterator, "glob", sizeof("glob")-1, &pnlen TSRMLS_CC);
		if (php_stream_is(intern->u.dir.dirp ,&php_glob_stream_ops)) {
			add_u_assoc_zstrl_ex(&zrv, ZEND_STR_TYPE, pnstr, pnlen+1, intern->_path_type, intern->_path, intern->_path_len, 1);
		} else {
			add_u_assoc_bool_ex(&zrv, ZEND_STR_TYPE, pnstr, pnlen+1, 0);
		}
		efree(pnstr.v);
		pnstr = spl_gen_private_prop_name(spl_ce_RecursiveDirectoryIterator, "subPathName", sizeof("subPathName")-1, &pnlen TSRMLS_CC);
		if (intern->u.dir.sub_path.v) {
			add_u_assoc_zstrl_ex(&zrv, ZEND_STR_TYPE, pnstr, pnlen+1, intern->u.dir.sub_path_type, intern->u.dir.sub_path, intern->u.dir.sub_path_len, 1);
		} else {
			add_u_assoc_zstrl_ex(&zrv, ZEND_STR_TYPE, pnstr, pnlen+1, ZEND_STR_TYPE, EMPTY_ZSTR, 0, 1);
		}
		efree(pnstr.v);
	}
	if (intern->type == SPL_FS_FILE) {
		pnstr = spl_gen_private_prop_name(spl_ce_SplFileObject, "openMode", sizeof("openMode")-1, &pnlen TSRMLS_CC);
		add_u_assoc_stringl_ex(&zrv, ZEND_STR_TYPE, pnstr, pnlen+1, intern->u.file.open_mode, intern->u.file.open_mode_len, 1);
		efree(pnstr.v);
		stmp[1] = '\0';
		stmp[0] = intern->u.file.delimiter;
		pnstr = spl_gen_private_prop_name(spl_ce_SplFileObject, "delimiter", sizeof("delimiter")-1, &pnlen TSRMLS_CC);
		add_u_assoc_stringl_ex(&zrv, ZEND_STR_TYPE, pnstr, pnlen+1, stmp, 1, 1);
		efree(pnstr.v);
		stmp[0] = intern->u.file.enclosure;
		pnstr = spl_gen_private_prop_name(spl_ce_SplFileObject, "enclosure", sizeof("enclosure")-1, &pnlen TSRMLS_CC);
		add_u_assoc_stringl_ex(&zrv, ZEND_STR_TYPE, pnstr, pnlen+1, stmp, 1, 1);
		efree(pnstr.v);
	}

	return rv;
}
/* }}}} */

#define DIT_CTOR_FLAGS  0x00000001
#define DIT_CTOR_GLOB   0x00000002

UChar u_glob[sizeof("glob://")];

void spl_filesystem_object_construct(INTERNAL_FUNCTION_PARAMETERS, int ctor_flags) /* {{{ */
{
	spl_filesystem_object *intern;
	zstr path;
	int parsed, len;
	zend_uchar path_type;
	long flags = 0;
	
	php_set_error_handling(EH_THROW, spl_ce_RuntimeException TSRMLS_CC);

	if (ctor_flags & DIT_CTOR_FLAGS) {
		flags = SPL_FILE_DIR_CURRENT_AS_FILEINFO;
		parsed = zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "t|l", &path, &len, &path_type, &flags);
	} else {
		flags = 0;
		parsed = zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "t", &path, &len, &path_type);
	}
	if (parsed == FAILURE) {
		php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
		return;
	}
	if (!len) {
		php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
		zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC, "Directory name must not be empty.");
		return;
	}

	intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	if ((ctor_flags & DIT_CTOR_GLOB) && (
		(path_type == IS_STRING && strstr(path.s, "glob://") != path.s) ||
		(path_type == IS_UNICODE && u_strstr(path.u, u_glob) != path.u)
	)) {
		len = zspprintf(path_type, &path, 0, "glob://%R", path_type, path);
		spl_filesystem_dir_open(intern, path_type, path, len TSRMLS_CC);
		efree(path.v);
	} else {
		spl_filesystem_dir_open(intern, path_type, path, len TSRMLS_CC);
	}

	intern->u.dir.is_recursive = instanceof_function(intern->std.ce, spl_ce_RecursiveDirectoryIterator TSRMLS_CC) ? 1 : 0;
	intern->flags = flags;

	php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
}
/* }}} */

/* {{{ proto void DirectoryIterator::__construct(string path) U
 Cronstructs a new dir iterator from a path. */
SPL_METHOD(DirectoryIterator, __construct)
{
	spl_filesystem_object_construct(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto void DirectoryIterator::rewind() U
   Rewind dir back to the start */
SPL_METHOD(DirectoryIterator, rewind)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	intern->u.dir.index = 0;
	if (intern->u.dir.dirp) {
		php_stream_rewinddir(intern->u.dir.dirp);
	}
	spl_filesystem_dir_read(intern TSRMLS_CC);
}
/* }}} */

/* {{{ proto string DirectoryIterator::key() U
   Return current dir entry */
SPL_METHOD(DirectoryIterator, key)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (intern->u.dir.dirp) {
		RETURN_LONG(intern->u.dir.index);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto DirectoryIterator DirectoryIterator::current() U
   Return this (needed for Iterator interface) */
SPL_METHOD(DirectoryIterator, current)
{
	RETURN_ZVAL(getThis(), 1, 0);
}
/* }}} */

/* {{{ proto void DirectoryIterator::next() U
   Move to next entry */
SPL_METHOD(DirectoryIterator, next)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	intern->u.dir.index++;
	spl_filesystem_dir_read(intern TSRMLS_CC);
	if (intern->file_name.v) {
		efree(intern->file_name.v);
		intern->file_name = NULL_ZSTR;
	}
}
/* }}} */

/* {{{ proto string DirectoryIterator::valid() U
   Check whether dir contains more entries */
SPL_METHOD(DirectoryIterator, valid)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	RETURN_BOOL(intern->u.dir.entry.d_name[0] != '\0');
}
/* }}} */

/* {{{ proto string SplFileInfo::getPath() U
   Return the path */
SPL_METHOD(SplFileInfo, getPath)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	int path_len;
	zend_uchar path_type;
	zstr path = spl_filesystem_object_get_path(intern, &path_len, &path_type TSRMLS_CC);

	RETURN_ZSTRL(path_type, path, path_len, ZSTR_DUPLICATE);
}
/* }}} */

/* {{{ proto string SplFileInfo::getFilename() U
   Return filename only */
SPL_METHOD(SplFileInfo, getFilename)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	zstr ret, path;
	zend_uchar path_type;
	int path_len;

    path = spl_filesystem_object_get_path(intern, &path_len, &path_type TSRMLS_CC);
	if (path_len && path_len < intern->file_name_len) {
		if (intern->file_name_type == IS_UNICODE) {
			ret.u = intern->file_name.u + path_len + 1;
		} else {
			ret.s = intern->file_name.s + path_len + 1;
		}
		RETURN_ZSTRL(intern->file_name_type, ret, intern->file_name_len - (path_len + 1), ZSTR_DUPLICATE);
	} else {
		RETURN_ZSTRL(intern->file_name_type, intern->file_name, intern->file_name_len, ZSTR_DUPLICATE);
	}
}
/* }}} */

/* {{{ proto string DirectoryIterator::getFilename() U
   Return filename of current dir entry */
SPL_METHOD(DirectoryIterator, getFilename)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	RETURN_RT_STRING(intern->u.dir.entry.d_name, ZSTR_DUPLICATE);
}
/* }}} */

/* {{{ proto string SplFileInfo::getBasename([string $suffix]) U
   Returns filename component of path */
SPL_METHOD(SplFileInfo, getBasename)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	zstr fname, suffix, path;
	int flen, slen = 0, path_len;
	zend_uchar path_type;

	suffix.v = 0;
	if (intern->file_name_type == IS_UNICODE) {	
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|t", &suffix.u, &slen) == FAILURE) {
			return;
		}
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &suffix.s, &slen) == FAILURE) {
			return;
		}
	}

	path = spl_filesystem_object_get_path(intern, &path_len, &path_type TSRMLS_CC);
	if (path_len && path_len < intern->file_name_len) {
		if (intern->file_name_type == IS_UNICODE) {
			fname.u = intern->file_name.u + path_len + 1;
		} else {
			fname.s = intern->file_name.s + path_len + 1;
		}
		flen = intern->file_name_len - (path_len + 1);
	} else {
		fname = intern->file_name;
		flen = intern->file_name_len;
	}

	if (intern->file_name_type == IS_UNICODE) {	
		php_u_basename(fname.u, flen, suffix.u, slen, &fname.u, &flen TSRMLS_CC);
	} else {
		php_basename(fname.s, flen, suffix.s, slen, &fname.s, &flen TSRMLS_CC);
	}

	RETURN_ZSTRL(intern->file_name_type, fname, flen, 0);
}
/* }}}*/   

/* {{{ proto string DirectoryIterator::getBasename([string $suffix]) U
   Returns filename component of current dir entry */
SPL_METHOD(DirectoryIterator, getBasename)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	char *suffix = 0, *fname;
	int slen = 0, flen;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &suffix, &slen) == FAILURE) {
		return;
	}

	php_basename(intern->u.dir.entry.d_name, strlen(intern->u.dir.entry.d_name), suffix, slen, &fname, &flen TSRMLS_CC);

	RETURN_RT_STRINGL(fname, flen, ZSTR_AUTOFREE);
}
/* }}} */

/* {{{ proto string SplFileInfo::getPathname() U
   Return path and filename */
SPL_METHOD(SplFileInfo, getPathname)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	switch (intern->type) {
	case SPL_FS_INFO:
	case SPL_FS_FILE:
		RETURN_ZSTRL(intern->file_name_type, intern->file_name, intern->file_name_len, ZSTR_DUPLICATE);
	case SPL_FS_DIR:
		if (intern->u.dir.entry.d_name[0]) {
			spl_filesystem_object_get_file_name(intern TSRMLS_CC);
			RETURN_ZSTRL(intern->file_name_type, intern->file_name, intern->file_name_len, ZSTR_DUPLICATE);
		}
	}
	RETURN_BOOL(0);
}
/* }}} */

/* {{{ proto string FilesystemIterator::key() U
   Return getPathname() or getFilename() depending on flags */
SPL_METHOD(FilesystemIterator, key)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (intern->flags & SPL_FILE_DIR_KEY_AS_FILENAME) {
		RETURN_RT_STRING(intern->u.dir.entry.d_name, ZSTR_DUPLICATE);
	} else {
		spl_filesystem_object_get_file_name(intern TSRMLS_CC);
		RETURN_ZSTRL(intern->file_name_type, intern->file_name, intern->file_name_len, 1);
	}
}
/* }}} */

/* {{{ proto string FilesystemIterator::current() U
   Return getFilename(), getFileInfo() or $this depending on flags */
SPL_METHOD(FilesystemIterator, current)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (intern->flags & SPL_FILE_DIR_CURRENT_AS_PATHNAME) {
		spl_filesystem_object_get_file_name(intern TSRMLS_CC);
		RETURN_ZSTRL(intern->file_name_type, intern->file_name, intern->file_name_len, ZSTR_DUPLICATE);
	} else if (intern->flags & SPL_FILE_DIR_CURRENT_AS_FILEINFO) {
		spl_filesystem_object_get_file_name(intern TSRMLS_CC);
		spl_filesystem_object_create_type(0, intern, SPL_FS_INFO, NULL, return_value TSRMLS_CC);
	} else {
		RETURN_ZVAL(getThis(), 1, 0);
		/*RETURN_STRING(intern->u.dir.entry.d_name, 1);*/
	}
}
/* }}} */

/* {{{ proto bool DirectoryIterator::isDot() U
   Returns true if current entry is '.' or  '..' */
SPL_METHOD(DirectoryIterator, isDot)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	RETURN_BOOL(spl_filesystem_is_dot(intern->u.dir.entry.d_name));
}
/* }}} */

/* {{{ proto void SplFileInfo::__construct(string file_name) U
 Cronstructs a new SplFileInfo from a path. */
/* php_set_error_handling() is used to throw exceptions in case
   the constructor fails. Here we use this to ensure the object
   has a valid directory resource.
   
   When the constructor gets called the object is already created 
   by the engine, so we must only call 'additional' initializations.
 */
SPL_METHOD(SplFileInfo, __construct)
{
	spl_filesystem_object *intern;
	zstr path;
	int path_len;
	zend_uchar path_type;

	php_set_error_handling(EH_THROW, spl_ce_RuntimeException TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "t", &path, &path_len, &path_type) == FAILURE) {
		php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
		return;
	}

	intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	spl_filesystem_info_set_filename(intern, path_type, path, path_len, 1 TSRMLS_CC);
	
	/* intern->type = SPL_FS_INFO; already set */

	php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
}
/* }}} */

/* {{{ FileInfoFunction */
#define FileInfoFunction(func_name, func_num) \
SPL_METHOD(SplFileInfo, func_name) \
{ \
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC); \
 \
	php_set_error_handling(EH_THROW, spl_ce_RuntimeException TSRMLS_CC);\
	spl_filesystem_object_get_file_name(intern TSRMLS_CC); \
	php_u_stat(intern->file_name_type, intern->file_name, intern->file_name_len, func_num, FG(default_context), return_value TSRMLS_CC); \
	php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);\
}
/* }}} */

/* {{{ proto int SplFileInfo::getPerms() U
   Get file permissions */
FileInfoFunction(getPerms, FS_PERMS)
/* }}} */

/* {{{ proto int SplFileInfo::getInode() U
   Get file inode */
FileInfoFunction(getInode, FS_INODE)
/* }}} */

/* {{{ proto int SplFileInfo::getSize() U
   Get file size */
FileInfoFunction(getSize, FS_SIZE)
/* }}} */

/* {{{ proto int SplFileInfo::getOwner() U
   Get file owner */
FileInfoFunction(getOwner, FS_OWNER)
/* }}} */

/* {{{ proto int SplFileInfo::getGroup() U
   Get file group */
FileInfoFunction(getGroup, FS_GROUP)
/* }}} */

/* {{{ proto int SplFileInfo::getATime() U
   Get last access time of file */
FileInfoFunction(getATime, FS_ATIME)
/* }}} */

/* {{{ proto int SplFileInfo::getMTime() U
   Get last modification time of file */
FileInfoFunction(getMTime, FS_MTIME)
/* }}} */

/* {{{ proto int SplFileInfo::getCTime() U
   Get inode modification time of file */
FileInfoFunction(getCTime, FS_CTIME)
/* }}} */

/* {{{ proto string SplFileInfo::getType() U
   Get file type */
FileInfoFunction(getType, FS_TYPE)
/* }}} */

/* {{{ proto bool SplFileInfo::isWritable() U
   Returns true if file can be written */
FileInfoFunction(isWritable, FS_IS_W)
/* }}} */

/* {{{ proto bool SplFileInfo::isReadable() U
   Returns true if file can be read */
FileInfoFunction(isReadable, FS_IS_R)
/* }}} */

/* {{{ proto bool SplFileInfo::isExecutable() U
   Returns true if file is executable */
FileInfoFunction(isExecutable, FS_IS_X)
/* }}} */

/* {{{ proto bool SplFileInfo::isFile() U
   Returns true if file is a regular file */
FileInfoFunction(isFile, FS_IS_FILE)
/* }}} */

/* {{{ proto bool SplFileInfo::isDir() U
   Returns true if file is directory */
FileInfoFunction(isDir, FS_IS_DIR)
/* }}} */

/* {{{ proto bool SplFileInfo::isLink() U
   Returns true if file is symbolic link */
FileInfoFunction(isLink, FS_IS_LINK)
/* }}} */

/* {{{ proto string SplFileInfo::getLinkTarget() U
   Return the target of a symbolic link */
SPL_METHOD(SplFileInfo, getLinkTarget)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	UChar *target;
	int ret, link_len, target_len;
	char *link, buff[MAXPATHLEN];

	php_set_error_handling(EH_THROW, spl_ce_RuntimeException TSRMLS_CC);

	if (intern->file_name_type == IS_UNICODE) {
		php_stream_path_encode(NULL, &link, &link_len, intern->file_name.u, intern->file_name_len, REPORT_ERRORS, FG(default_context));
	} else {
		link = intern->file_name.s;
	}

#ifdef HAVE_SYMLINK
	ret = readlink(intern->file_name.s, buff, MAXPATHLEN-1);
#else
	ret = -1; /* always fail if not implemented */
#endif

	if (ret == -1) {
		zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC, "Unable to read link %R, error: %s", intern->file_name_type, intern->file_name, strerror(errno));
		RETVAL_FALSE;
	} else {
		/* Append NULL to the end of the string */
		buff[ret] = '\0';

		if (UG(unicode)) {
			if (SUCCESS == php_stream_path_decode(NULL, &target, &target_len, buff, ret, REPORT_ERRORS, FG(default_context))) {
				RETVAL_UNICODEL(target, target_len, 0);
			} else {
				RETVAL_STRING(buff, 1);
			}
		} else {
			RETVAL_STRING(buff, 1);
		}
	}
	php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
}
/* }}} */

#if (!defined(__BEOS__) && !defined(NETWARE) && HAVE_REALPATH) || defined(ZTS)
/* {{{ proto string SplFileInfo::getRealPath() U
   Return the resolved path */
SPL_METHOD(SplFileInfo, getRealPath)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	UChar *path;
	int filename_len, path_len;
	char *filename, buff[MAXPATHLEN];

	php_set_error_handling(EH_THROW, spl_ce_RuntimeException TSRMLS_CC);

	if (intern->type == SPL_FS_DIR && !intern->file_name.v && intern->u.dir.entry.d_name[0]) {
		spl_filesystem_object_get_file_name(intern TSRMLS_CC);
	}

	if (intern->file_name_type == IS_UNICODE) {
		php_stream_path_encode(NULL, &filename, &filename_len, intern->file_name.u, intern->file_name_len, REPORT_ERRORS, FG(default_context));
	} else {
		filename = intern->file_name.s;
	}

	if (filename && VCWD_REALPATH(filename, buff)) {
#ifdef ZTS
		if (VCWD_ACCESS(buff, F_OK)) {
			RETVAL_FALSE;
		} else
#endif
		if (UG(unicode)) {
			if (php_stream_path_decode(NULL, &path, &path_len, buff, strlen(buff), REPORT_ERRORS, FG(default_context)) == SUCCESS) {
				RETVAL_UNICODEL(path, path_len, 0);
			} else {
				RETVAL_STRING(buff, 1);
			}
		} else {
			RETVAL_STRING(buff, 1);
		}
	} else {
		RETVAL_FALSE;
	}

	if (intern->file_name_type == IS_UNICODE && filename) {
		efree(filename);
	}

	php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
}
/* }}} */
#endif

/* {{{ proto SplFileObject SplFileInfo::openFile([string mode = 'r' [, bool use_include_path  [, resource context]]]) U
   Open the current file */
SPL_METHOD(SplFileInfo, openFile)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	spl_filesystem_object_create_type(ht, intern, SPL_FS_FILE, NULL, return_value TSRMLS_CC);
}
/* }}} */

/* {{{ proto void SplFileInfo::setFileClass([string class_name]) U
   Class to use in openFile() */
SPL_METHOD(SplFileInfo, setFileClass)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	zend_class_entry *ce = spl_ce_SplFileObject;
	
	php_set_error_handling(EH_THROW, spl_ce_UnexpectedValueException TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|C", &ce) == SUCCESS) {
		intern->file_class = ce;
	}

	php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
}
/* }}} */

/* {{{ proto void SplFileInfo::setInfoClass([string class_name]) U
   Class to use in getFileInfo(), getPathInfo() */
SPL_METHOD(SplFileInfo, setInfoClass)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	zend_class_entry *ce = spl_ce_SplFileInfo;
	
	php_set_error_handling(EH_THROW, spl_ce_UnexpectedValueException TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|C", &ce) == SUCCESS) {
		intern->info_class = ce;
	}

	php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
}
/* }}} */

/* {{{ proto SplFileInfo SplFileInfo::getFileInfo([string $class_name]) U
   Get/copy file info */
SPL_METHOD(SplFileInfo, getFileInfo)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	zend_class_entry *ce = intern->info_class;
	
	php_set_error_handling(EH_THROW, spl_ce_UnexpectedValueException TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|C", &ce) == SUCCESS) {
		spl_filesystem_object_create_type(ht, intern, SPL_FS_INFO, ce, return_value TSRMLS_CC);
	}

	php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
}
/* }}} */

/* {{{ proto SplFileInfo SplFileInfo::getPathInfo([string $class_name]) U
   Get/copy file info */
SPL_METHOD(SplFileInfo, getPathInfo)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	zend_class_entry *ce = intern->info_class;
	
	php_set_error_handling(EH_THROW, spl_ce_UnexpectedValueException TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|C", &ce) == SUCCESS) {
		zend_uchar path_type;
		int path_len;
		zstr path = spl_filesystem_object_get_path(intern, &path_len, &path_type TSRMLS_CC);
		spl_filesystem_object_create_info(intern, path_type, path, path_len, 1, ce, return_value TSRMLS_CC);
	}

	php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
}
/* }}} */

/* {{{ proto void FilesystemIterator::__construct(string path [, int flags])
 Cronstructs a new dir iterator from a path. */
SPL_METHOD(FilesystemIterator, __construct)
{
	spl_filesystem_object_construct(INTERNAL_FUNCTION_PARAM_PASSTHRU, DIT_CTOR_FLAGS);
}
/* }}} */

/* {{{ proto void FilesystemIterator::rewind() U
   Rewind dir back to the start */
SPL_METHOD(FilesystemIterator, rewind)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	intern->u.dir.index = 0;
	if (intern->u.dir.dirp) {
		php_stream_rewinddir(intern->u.dir.dirp);
	}
	do {
		spl_filesystem_dir_read(intern TSRMLS_CC);
	} while (spl_filesystem_is_dot(intern->u.dir.entry.d_name));
}
/* }}} */

/* {{{ proto void FilesystemIterator::next() U
   Move to next entry */
SPL_METHOD(FilesystemIterator, next)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	intern->u.dir.index++;
	do {
		spl_filesystem_dir_read(intern TSRMLS_CC);
	} while (spl_filesystem_is_dot(intern->u.dir.entry.d_name));
	if (intern->file_name.v) {
		efree(intern->file_name.v);
		intern->file_name = NULL_ZSTR;
	}
}
/* }}} */

/* {{{ proto bool RecursiveDirectoryIterator::hasChildren([bool $allow_links = false]) U
   Returns whether current entry is a directory and not '.' or '..' */
SPL_METHOD(RecursiveDirectoryIterator, hasChildren)
{
	zend_bool allow_links = 0;
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	if (spl_filesystem_is_invalid_or_dot(intern->u.dir.entry.d_name)) {
		RETURN_FALSE;
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|b", &allow_links) == FAILURE) {
			return;
		}
		spl_filesystem_object_get_file_name(intern TSRMLS_CC);
		if (!allow_links) {
			php_u_stat(intern->file_name_type, intern->file_name, intern->file_name_len, FS_IS_LINK, FG(default_context), return_value TSRMLS_CC);
			if (zend_is_true(return_value)) {
				RETURN_FALSE;
			}
		}
		php_u_stat(intern->file_name_type, intern->file_name, intern->file_name_len, FS_IS_DIR, FG(default_context), return_value TSRMLS_CC);
    }
}
/* }}} */

/* {{{ proto RecursiveDirectoryIterator DirectoryIterator::getChildren() U
   Returns an iterator for the current entry if it is a directory */
SPL_METHOD(RecursiveDirectoryIterator, getChildren)
{
	zval zpath, zflags;
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	spl_filesystem_object *subdir;
	
	spl_filesystem_object_get_file_name(intern TSRMLS_CC);

	INIT_PZVAL(&zflags);
	INIT_PZVAL(&zpath);
	ZVAL_LONG(&zflags, intern->flags);
	ZVAL_ZSTRL(&zpath, intern->file_name_type, intern->file_name, intern->file_name_len, 1);

	spl_instantiate_arg_ex2(spl_ce_RecursiveDirectoryIterator, &return_value, 0, &zpath, &zflags TSRMLS_CC);

	zval_dtor(&zpath);

	subdir = (spl_filesystem_object*)zend_object_store_get_object(return_value TSRMLS_CC);
	if (subdir) {
		if (intern->u.dir.sub_path.v && intern->u.dir.sub_path_len > 1) {
			subdir->u.dir.sub_path_type = intern->u.dir.sub_path_type;
			subdir->u.dir.sub_path_len = zspprintf(intern->u.dir.sub_path_type, &subdir->u.dir.sub_path, 0, "%R%c%s", intern->u.dir.sub_path_type, intern->u.dir.sub_path, DEFAULT_SLASH, intern->u.dir.entry.d_name);
		} else {
			subdir->u.dir.sub_path_len = strlen(intern->u.dir.entry.d_name);
			subdir->u.dir.sub_path_type = IS_STRING;
			subdir->u.dir.sub_path.s = estrndup(intern->u.dir.entry.d_name, subdir->u.dir.sub_path_len);
		}
		subdir->info_class = intern->info_class;
		subdir->file_class = intern->file_class;
		subdir->oth = intern->oth;
	}
}
/* }}} */

/* {{{ proto void RecursiveDirectoryIterator::getSubPath() U
   Get sub path */
SPL_METHOD(RecursiveDirectoryIterator, getSubPath)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (intern->u.dir.sub_path.v) {
		RETURN_ZSTRL(intern->u.dir.sub_path_type, intern->u.dir.sub_path, intern->u.dir.sub_path_len, ZSTR_DUPLICATE);
	} else {
		RETURN_EMPTY_TEXT();
	}
}
/* }}} */

/* {{{ proto void RecursiveDirectoryIterator::getSubPathname() U
   Get sub path and file name */
SPL_METHOD(RecursiveDirectoryIterator, getSubPathname)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	zstr sub_name;
	int sub_len;

	if (intern->u.dir.sub_path.v) {
		sub_len = zspprintf(intern->u.dir.sub_path_type, &sub_name, 0, "%R%c%s", intern->u.dir.sub_path_type, intern->u.dir.sub_path, DEFAULT_SLASH, intern->u.dir.entry.d_name);
		RETURN_ZSTRL(intern->u.dir.sub_path_type, sub_name, sub_len, 0);
	} else {
		RETURN_RT_STRING(intern->u.dir.entry.d_name, ZSTR_DUPLICATE);
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

/* {{{ proto int GlobIterator::__construct(string path [, int flags]) U
 Cronstructs a new dir iterator from a glob expression (no glob:// needed). */
SPL_METHOD(GlobIterator, __construct)
{
	spl_filesystem_object_construct(INTERNAL_FUNCTION_PARAM_PASSTHRU, DIT_CTOR_FLAGS|DIT_CTOR_GLOB);
}
/* }}} */

/* {{{ proto int GlobIterator::cont() U
   Return the number of directories and files found by globbing */
SPL_METHOD(GlobIterator, count)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (php_stream_is(intern->u.dir.dirp ,&php_glob_stream_ops)) {
		RETURN_LONG(php_glob_stream_get_count(intern->u.dir.dirp, NULL));
	} else {
		/* should not happen */
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "GlobIterator lost glob state");
	}
}
/* }}} */

/* forward declarations to the iterator handlers */
static void spl_filesystem_dir_it_dtor(zend_object_iterator *iter TSRMLS_DC);
static int spl_filesystem_dir_it_valid(zend_object_iterator *iter TSRMLS_DC);
static void spl_filesystem_dir_it_current_data(zend_object_iterator *iter, zval ***data TSRMLS_DC);
static int spl_filesystem_dir_it_current_key(zend_object_iterator *iter, zstr *str_key, uint *str_key_len, ulong *int_key TSRMLS_DC);
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

	Z_SET_REFCOUNT_P(object, Z_REFCOUNT_P(object) + 2);
	iterator->intern.data = (void*)object;
	iterator->intern.funcs = &spl_filesystem_dir_it_funcs;
	iterator->current = object;
	
	return (zend_object_iterator*)iterator;
}
/* }}} */

/* {{{ spl_filesystem_dir_it_dtor */
static void spl_filesystem_dir_it_dtor(zend_object_iterator *iter TSRMLS_DC)
{
	spl_filesystem_iterator *iterator = (spl_filesystem_iterator *)iter;

	zval_ptr_dtor(&iterator->current);
	zval_ptr_dtor((zval**)&iterator->intern.data);
	iterator->intern.data = NULL; /* mark as unused */
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
static int spl_filesystem_dir_it_current_key(zend_object_iterator *iter, zstr *str_key, uint *str_key_len, ulong *int_key TSRMLS_DC)
{
	spl_filesystem_object *object = spl_filesystem_iterator_to_object((spl_filesystem_iterator *)iter);
	
	*int_key = object->u.dir.index;
	return HASH_KEY_IS_LONG;
}
/* }}} */

/* {{{ spl_filesystem_dir_it_move_forward */
static void spl_filesystem_dir_it_move_forward(zend_object_iterator *iter TSRMLS_DC)
{
	spl_filesystem_object *object = spl_filesystem_iterator_to_object((spl_filesystem_iterator *)iter);
	
	object->u.dir.index++;
	spl_filesystem_dir_read(object TSRMLS_CC);
	if (object->file_name.v) {
		efree(object->file_name.v);
		object->file_name = NULL_ZSTR;
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

	if (iterator->current) {
		zval_ptr_dtor(&iterator->current);
	}
	zval_ptr_dtor((zval**)&iterator->intern.data);
	iterator->intern.data = NULL; /* mark as unused */
}
/* }}} */
	
/* {{{ spl_filesystem_tree_it_current_data */
static void spl_filesystem_tree_it_current_data(zend_object_iterator *iter, zval ***data TSRMLS_DC)
{
	spl_filesystem_iterator *iterator = (spl_filesystem_iterator *)iter;
	spl_filesystem_object   *object   = spl_filesystem_iterator_to_object(iterator);

	if (object->flags & SPL_FILE_DIR_CURRENT_AS_PATHNAME) {
		if (!iterator->current) {
			ALLOC_INIT_ZVAL(iterator->current);
			spl_filesystem_object_get_file_name(object TSRMLS_CC);
			ZVAL_ZSTRL(iterator->current, object->file_name_type, object->file_name, object->file_name_len, ZSTR_DUPLICATE);
		}
		*data = &iterator->current;
	} else if (object->flags & SPL_FILE_DIR_CURRENT_AS_FILEINFO) {
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
static int spl_filesystem_tree_it_current_key(zend_object_iterator *iter, zstr *str_key, uint *str_key_len, ulong *int_key TSRMLS_DC)
{
	spl_filesystem_object *object = spl_filesystem_iterator_to_object((spl_filesystem_iterator *)iter);
	
	if (object->flags & SPL_FILE_DIR_KEY_AS_FILENAME) {
		*str_key_len = strlen(object->u.dir.entry.d_name) + 1;
		str_key->s = estrndup(object->u.dir.entry.d_name, *str_key_len - 1);
		return HASH_KEY_IS_STRING;
	} else {
		spl_filesystem_object_get_file_name(object TSRMLS_CC);
		*str_key_len = object->file_name_len + 1;
		*str_key = ezstrndup(object->file_name_type, object->file_name, object->file_name_len);
		return object->file_name_type;
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
	if (object->file_name.v) {
		efree(object->file_name.v);
		object->file_name = NULL_ZSTR;
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

/* iterator handler table */
zend_object_iterator_funcs spl_filesystem_tree_it_funcs = {
	spl_filesystem_tree_it_dtor,
	spl_filesystem_dir_it_valid,
	spl_filesystem_tree_it_current_data,
	spl_filesystem_tree_it_current_key,
	spl_filesystem_tree_it_move_forward,
	spl_filesystem_tree_it_rewind
};

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

	Z_ADDREF_P(object);
	iterator->intern.data = (void*)object;
	iterator->intern.funcs = &spl_filesystem_tree_it_funcs;
	iterator->current = NULL;
	
	return (zend_object_iterator*)iterator;
}
/* }}} */

/* {{{ spl_filesystem_object_cast */
static int spl_filesystem_object_cast(zval *readobj, zval *writeobj, int type, void *extra TSRMLS_DC)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(readobj TSRMLS_CC);

	switch (intern->type) {
	case SPL_FS_INFO:
	case SPL_FS_FILE:
		if (type == intern->file_name_type) {
			ZVAL_ZSTRL(writeobj, intern->file_name_type, intern->file_name, intern->file_name_len, 1);
			return SUCCESS;
		}
		if (type == IS_STRING) {
			ZVAL_ZSTRL(writeobj, intern->file_name_type, intern->file_name, intern->file_name_len, ZSTR_DUPLICATE);
			zval_unicode_to_string_ex(writeobj, ZEND_U_CONVERTER(((UConverter *)extra)) TSRMLS_CC);
			return SUCCESS;
		}
		if (type == IS_UNICODE && UG(unicode)) {
			ZVAL_ASCII_STRINGL(writeobj, intern->file_name.s, intern->file_name_len, 1);
			return SUCCESS;
		}
		break;
	case SPL_FS_DIR:
		if (type == IS_STRING) {
			ZVAL_STRING(writeobj, intern->u.dir.entry.d_name, 1);
			return SUCCESS;
		}
		if (type == IS_UNICODE && UG(unicode)) {
			ZVAL_ASCII_STRING(writeobj, intern->u.dir.entry.d_name, 1);
			return SUCCESS;
		}
		break;
	}
	ZVAL_NULL(writeobj);
	return FAILURE;
}
/* }}} */

/* declare method parameters */
/* supply a name and default to call by parameter */
static
ZEND_BEGIN_ARG_INFO(arginfo_info___construct, 0) 
	ZEND_ARG_INFO(0, file_name)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_info_openFile, 0, 0, 0)
	ZEND_ARG_INFO(0, open_mode)
	ZEND_ARG_INFO(0, use_include_path)
	ZEND_ARG_INFO(0, context)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_info_optinalFileClass, 0, 0, 0)
	ZEND_ARG_INFO(0, class_name)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_optinalSuffix, 0, 0, 0)
	ZEND_ARG_INFO(0, suffix)
ZEND_END_ARG_INFO()

/* the method table */
/* each method can have its own parameters and visibility */
static const zend_function_entry spl_SplFileInfo_functions[] = {
	SPL_ME(SplFileInfo,       __construct,   arginfo_info___construct, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileInfo,       getPath,       NULL, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileInfo,       getFilename,   NULL, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileInfo,       getBasename,   arginfo_optinalSuffix, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileInfo,       getPathname,   NULL, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileInfo,       getPerms,      NULL, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileInfo,       getInode,      NULL, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileInfo,       getSize,       NULL, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileInfo,       getOwner,      NULL, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileInfo,       getGroup,      NULL, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileInfo,       getATime,      NULL, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileInfo,       getMTime,      NULL, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileInfo,       getCTime,      NULL, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileInfo,       getType,       NULL, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileInfo,       isWritable,    NULL, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileInfo,       isReadable,    NULL, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileInfo,       isExecutable,  NULL, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileInfo,       isFile,        NULL, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileInfo,       isDir,         NULL, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileInfo,       isLink,        NULL, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileInfo,       getLinkTarget, NULL, ZEND_ACC_PUBLIC)
#if (!defined(__BEOS__) && !defined(NETWARE) && HAVE_REALPATH) || defined(ZTS)
	SPL_ME(SplFileInfo,       getRealPath,   NULL, ZEND_ACC_PUBLIC)
#endif
	SPL_ME(SplFileInfo,       getFileInfo,   arginfo_info_optinalFileClass, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileInfo,       getPathInfo,   arginfo_info_optinalFileClass, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileInfo,       openFile,      arginfo_info_openFile,         ZEND_ACC_PUBLIC)
	SPL_ME(SplFileInfo,       setFileClass,  arginfo_info_optinalFileClass, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileInfo,       setInfoClass,  arginfo_info_optinalFileClass, ZEND_ACC_PUBLIC)
	SPL_MA(SplFileInfo,       __toString, SplFileInfo, getPathname, NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

static
ZEND_BEGIN_ARG_INFO(arginfo_dir___construct, 0) 
	ZEND_ARG_INFO(0, path)
ZEND_END_ARG_INFO()

/* the method table */
/* each method can have its own parameters and visibility */
static const zend_function_entry spl_DirectoryIterator_functions[] = {
	SPL_ME(DirectoryIterator, __construct,   arginfo_dir___construct, ZEND_ACC_PUBLIC)
	SPL_ME(DirectoryIterator, getFilename,   NULL, ZEND_ACC_PUBLIC)
	SPL_ME(DirectoryIterator, getBasename,   arginfo_optinalSuffix, ZEND_ACC_PUBLIC)
	SPL_ME(DirectoryIterator, isDot,         NULL, ZEND_ACC_PUBLIC)
	SPL_ME(DirectoryIterator, rewind,        NULL, ZEND_ACC_PUBLIC)
	SPL_ME(DirectoryIterator, valid,         NULL, ZEND_ACC_PUBLIC)
	SPL_ME(DirectoryIterator, key,           NULL, ZEND_ACC_PUBLIC)
	SPL_ME(DirectoryIterator, current,       NULL, ZEND_ACC_PUBLIC)
	SPL_ME(DirectoryIterator, next,          NULL, ZEND_ACC_PUBLIC)
	SPL_MA(DirectoryIterator, __toString, DirectoryIterator, getFilename, NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_r_dir___construct, 0, 0, 1) 
	ZEND_ARG_INFO(0, path)
	ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_r_dir_hasChildren, 0, 0, 0)
	ZEND_ARG_INFO(0, allow_links)
ZEND_END_ARG_INFO()

static const zend_function_entry spl_FilesystemIterator_functions[] = {
	SPL_ME(FilesystemIterator, __construct,   arginfo_r_dir___construct, ZEND_ACC_PUBLIC)
	SPL_ME(FilesystemIterator, rewind,        NULL, ZEND_ACC_PUBLIC)
	SPL_ME(FilesystemIterator, next,          NULL, ZEND_ACC_PUBLIC)
	SPL_ME(FilesystemIterator, key,           NULL, ZEND_ACC_PUBLIC)
	SPL_ME(FilesystemIterator, current,       NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

static const zend_function_entry spl_RecursiveDirectoryIterator_functions[] = {
	SPL_ME(RecursiveDirectoryIterator, __construct,   arginfo_r_dir___construct, ZEND_ACC_PUBLIC)
	SPL_ME(RecursiveDirectoryIterator, hasChildren,   arginfo_r_dir_hasChildren, ZEND_ACC_PUBLIC)
	SPL_ME(RecursiveDirectoryIterator, getChildren,   NULL, ZEND_ACC_PUBLIC)
	SPL_ME(RecursiveDirectoryIterator, getSubPath,    NULL, ZEND_ACC_PUBLIC)
	SPL_ME(RecursiveDirectoryIterator, getSubPathname,NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

static const zend_function_entry spl_GlobIterator_functions[] = {
	SPL_ME(GlobIterator, __construct,   arginfo_r_dir___construct, ZEND_ACC_PUBLIC)
	SPL_ME(GlobIterator, count,         NULL,                      ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

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
		if (php_stream_get_line(intern->u.file.stream, ZSTR(buf), intern->u.file.max_line_len, &line_len) == NULL) {
			efree(buf);
			buf = NULL;
		} else {
			buf[line_len] = '\0';
		}
	} else {
		buf = php_stream_get_line(intern->u.file.stream, NULL_ZSTR, 0, &line_len);
	}

	if (!buf) {
		intern->u.file.current_line = estrdup("");
		intern->u.file.current_line_len = 0;
	} else {
		if (intern->flags & SPL_FILE_OBJECT_DROP_NEW_LINE) {
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

	ZVAL_STRING(&z_fname, func_ptr->common.function_name.s, 0);

	fci.size = sizeof(fci);
	fci.function_table = EG(function_table);
	fci.object_pp = NULL;
	fci.function_name = &z_fname;
	fci.retval_ptr_ptr = &retval;
	fci.param_count = num_args;
	fci.params = params;
	fci.no_separation = 1;
	fci.symbol_table = NULL;

	fcic.initialized = 1;
	fcic.function_handler = func_ptr;
	fcic.calling_scope = NULL;
	fcic.object_pp = NULL;

	result = zend_call_function(&fci, &fcic TSRMLS_CC);
	
	ZVAL_ZVAL(return_value, retval, 1, 1);

	efree(params);
	return result;
} /* }}} */

#define FileFunctionCall(func_name, pass_num_args, arg2) \
{ \
	zend_function *func_ptr; \
	zend_hash_find(EG(function_table), #func_name, sizeof(#func_name), (void **) &func_ptr); \
	spl_filesystem_file_call(intern, func_ptr, pass_num_args, return_value, arg2 TSRMLS_CC); \
}

static int spl_filesystem_file_read_csv(spl_filesystem_object *intern, char delimiter, char enclosure, char escape, zval *return_value TSRMLS_DC) /* {{{ */
{
	int ret = SUCCESS;
	
	do {
		ret = spl_filesystem_file_read(intern, 1 TSRMLS_CC);
	} while (ret == SUCCESS && !intern->u.file.current_line_len && (intern->flags & SPL_FILE_OBJECT_SKIP_EMPTY));
	
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
	if (intern->flags & SPL_FILE_OBJECT_READ_CSV || intern->u.file.func_getCurr->common.scope != spl_ce_SplFileObject) {
		if (php_stream_eof(intern->u.file.stream)) {
			if (!silent) {
				zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC, "Cannot read from file %s", intern->file_name);
			}
			return FAILURE;
		}
		if (intern->flags & SPL_FILE_OBJECT_READ_CSV) {
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
		case IS_UNICODE:
			return Z_STRLEN_P(intern->u.file.current_zval) == 0;
		case IS_ARRAY:
			if ((intern->flags & SPL_FILE_OBJECT_READ_CSV) 
			&& zend_hash_num_elements(Z_ARRVAL_P(intern->u.file.current_zval)) == 1) {
				zval ** first = Z_ARRVAL_P(intern->u.file.current_zval)->pListHead->pData;
					
				return (Z_TYPE_PP(first) == IS_STRING || Z_TYPE_PP(first) == IS_UNICODE) 
					&& Z_STRLEN_PP(first) == 0;
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

	while ((intern->flags & SPL_FILE_OBJECT_SKIP_EMPTY) && ret == SUCCESS && spl_filesystem_file_is_empty_line(intern TSRMLS_CC)) {
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
	if (intern->flags & SPL_FILE_OBJECT_READ_AHEAD) {
		spl_filesystem_file_read_line(this_ptr, intern, 1 TSRMLS_CC);
	}
} /* }}} */

/* {{{ proto void SplFileObject::__construct(string filename [, string mode = 'r' [, bool use_include_path  [, resource context]]]]) U
   Construct a new file object */
SPL_METHOD(SplFileObject, __construct)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	zend_bool use_include_path = 0;
	zstr p1, p2;

	php_set_error_handling(EH_THROW, spl_ce_RuntimeException TSRMLS_CC);

	intern->u.file.open_mode = "r";
	intern->u.file.open_mode_len = 1;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "t|sbr", 
			&intern->file_name, &intern->file_name_len, &intern->file_name_type,
			&intern->u.file.open_mode, &intern->u.file.open_mode_len, 
			&use_include_path, &intern->u.file.zcontext) == FAILURE) {
		php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
		return;
	}
	
	if (spl_filesystem_file_open(intern, use_include_path, 0 TSRMLS_CC) == SUCCESS) {
		if (intern->file_name_type == IS_UNICODE) {
			p1.u = u_strrchr(intern->file_name.u, '/');
		} else {
			p1.s = strrchr(intern->file_name.s, '/');
		}
#if defined(PHP_WIN32) || defined(NETWARE)
		if (intern->file_name_type == IS_UNICODE) {
			p2.u = u_strrchr(intern->file_name.u, '\\');
		} else {
			p2.s = strrchr(intern->file_name.s, '\\');
		}
#else
		p2.v = 0;
#endif
		if (p1.v || p2.v) {
			if (intern->file_name_type == IS_UNICODE) {
				intern->_path_len = (p1.u > p2.u ? p1.u : p2.u) - intern->file_name.u;
			} else {
				intern->_path_len = (p1.s > p2.s ? p1.s : p2.s) - intern->file_name.s;
			}
		} else {
			intern->_path_len = 0;
		}
		intern->_path_type = intern->file_name_type;
		intern->_path = ezstrndup(intern->file_name_type, intern->file_name, intern->_path_len);
	}

	php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
} /* }}} */

/* {{{ proto void SplTempFileObject::__construct([int max_memory]) U
   Construct a new temp file object */
SPL_METHOD(SplTempFileObject, __construct)
{
	long max_memory = PHP_STREAM_MAX_MEM;
	char tmp_fname[48];
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	php_set_error_handling(EH_THROW, spl_ce_RuntimeException TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &max_memory) == FAILURE) {
		php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
		return;
	}

	if (max_memory < 0) {
		intern->file_name_type = IS_STRING;
		intern->file_name.s = "php://memory";
		intern->file_name_len = 12;
	} else if (ZEND_NUM_ARGS()) {
		intern->file_name_type = IS_STRING;
		intern->file_name_len = snprintf(tmp_fname, sizeof(tmp_fname), "php://temp/maxmemory:%ld", max_memory);
		intern->file_name.s = tmp_fname;
	} else {
		intern->file_name_type = IS_STRING;
		intern->file_name.s = "php://temp";
		intern->file_name_len = 10;
	}
	intern->u.file.open_mode = "wb";
	intern->u.file.open_mode_len = 1;
	intern->u.file.zcontext = NULL;
	
	if (spl_filesystem_file_open(intern, 0, 0 TSRMLS_CC) == SUCCESS) {
		intern->_path_type = IS_STRING;
		intern->_path_len = 0;
		intern->_path.s = estrndup("", 0);
	}

	php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
} /* }}} */

/* {{{ proto void SplFileObject::rewind() U
   Rewind the file and read the first line */
SPL_METHOD(SplFileObject, rewind)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	spl_filesystem_file_rewind(getThis(), intern TSRMLS_CC);
} /* }}} */

/* {{{ proto void SplFileObject::eof() U
   Return whether end of file is reached */
SPL_METHOD(SplFileObject, eof)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	RETURN_BOOL(php_stream_eof(intern->u.file.stream));
} /* }}} */

/* {{{ proto void SplFileObject::valid() U
   Return !eof() */
SPL_METHOD(SplFileObject, valid)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (intern->flags & SPL_FILE_OBJECT_READ_AHEAD) {
		RETURN_BOOL(intern->u.file.current_line || intern->u.file.current_zval);
	} else {
		RETVAL_BOOL(!php_stream_eof(intern->u.file.stream));
	}
} /* }}} */

/* {{{ proto string SplFileObject::fgets() U
   Rturn next line from file */
SPL_METHOD(SplFileObject, fgets)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (spl_filesystem_file_read(intern, 0 TSRMLS_CC) == FAILURE) {
		RETURN_FALSE;
	}
	RETURN_STRINGL(intern->u.file.current_line, intern->u.file.current_line_len, 1);
} /* }}} */

/* {{{ proto string SplFileObject::current() U
   Return current line from file */
SPL_METHOD(SplFileObject, current)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (!intern->u.file.current_line && !intern->u.file.current_zval) {
		spl_filesystem_file_read_line(getThis(), intern, 1 TSRMLS_CC);
	}
	if (intern->u.file.current_line && (!(intern->flags & SPL_FILE_OBJECT_READ_CSV) || !intern->u.file.current_zval)) {
		RETURN_RT_STRINGL(intern->u.file.current_line, intern->u.file.current_line_len, ZSTR_DUPLICATE);
	} else if (intern->u.file.current_zval) {
		RETURN_ZVAL(intern->u.file.current_zval, 1, 0);
	}
	RETURN_FALSE;
} /* }}} */

/* {{{ proto int SplFileObject::key() U
   Return line number */
SPL_METHOD(SplFileObject, key)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

/*	Do not read the next line to support correct counting with fgetc()
	if (!intern->current_line) {
		spl_filesystem_file_read_line(getThis(), intern, 1 TSRMLS_CC);
	} */
	RETURN_LONG(intern->u.file.current_line_num);
} /* }}} */

/* {{{ proto void SplFileObject::next() U
   Read next line */
SPL_METHOD(SplFileObject, next)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	spl_filesystem_file_free_line(intern TSRMLS_CC);
	if (intern->flags & SPL_FILE_OBJECT_READ_AHEAD) {
		spl_filesystem_file_read_line(getThis(), intern, 1 TSRMLS_CC);
	}
	intern->u.file.current_line_num++;
} /* }}} */

/* {{{ proto void SplFileObject::setFlags(int flags) U
   Set file handling flags */
SPL_METHOD(SplFileObject, setFlags)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &intern->flags);
} /* }}} */

/* {{{ proto int SplFileObject::getFlags() U
   Get file handling flags */
SPL_METHOD(SplFileObject, getFlags)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	RETURN_LONG(intern->flags);
} /* }}} */

/* {{{ proto void SplFileObject::setMaxLineLen(int max_len) U
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

/* {{{ proto int SplFileObject::getMaxLineLen() U
   Get maximum line length */
SPL_METHOD(SplFileObject, getMaxLineLen)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	RETURN_LONG((long)intern->u.file.max_line_len);
} /* }}} */

/* {{{ proto bool SplFileObject::hasChildren() U
   Return false */
SPL_METHOD(SplFileObject, hasChildren)
{
	RETURN_FALSE;
} /* }}} */

/* {{{ proto bool SplFileObject::getChildren() U
   Read NULL */
SPL_METHOD(SplFileObject, getChildren)
{
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

/* {{{ proto array SplFileObject::fgetcsv([string delimiter [, string enclosure]]) U
   Return current line as csv */
SPL_METHOD(SplFileObject, fgetcsv)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	char delimiter = intern->u.file.delimiter, enclosure = intern->u.file.enclosure, escape = intern->u.file.escape;
	char *delim, *enclo, *esc;
	int d_len, e_len, esc_len;
	
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

/* {{{ proto void SplFileObject::setCsvControl([string delimiter = ',' [, string enclosure = '"' [, string escape = '\\']]])
   Set the delimiter and enclosure character used in fgetcsv */
SPL_METHOD(SplFileObject, setCsvControl)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	char delimiter = ',', enclosure = '"', escape='\\';
	char *delim, *enclo, *esc;
	int d_len, e_len, esc_len;
	
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

/* {{{ proto array SplFileObject::getCsvControl() U
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

	add_next_index_ascii_string(return_value, delimiter, ZSTR_DUPLICATE);
	add_next_index_ascii_string(return_value, enclosure, ZSTR_DUPLICATE);
}
/* }}} */

/* {{{ proto bool SplFileObject::flock(int operation [, int &wouldblock])
   Portable file locking */
FileFunction(flock)
/* }}} */

/* {{{ proto bool SplFileObject::fflush() U
   Flush the file */
SPL_METHOD(SplFileObject, fflush)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	RETURN_BOOL(!php_stream_flush(intern->u.file.stream));
} /* }}} */


/* {{{ proto int SplFileObject::ftell() U
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

/* {{{ proto int SplFileObject::fseek(int pos [, int whence = SEEK_SET]) U
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

/* {{{ proto int SplFileObject::fgetc() U
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

		RETURN_RT_STRINGL(buf, 1, ZSTR_DUPLICATE);
	}
} /* }}} */

/* {{{ proto string SplFileObject::fgetss([string allowable_tags]) U
   Get a line from file pointer and strip HTML tags */
SPL_METHOD(SplFileObject, fgetss)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	zval *arg2 = NULL;
	MAKE_STD_ZVAL(arg2);
	ZVAL_LONG(arg2, intern->u.file.max_line_len);

	spl_filesystem_file_free_line(intern TSRMLS_CC);
	intern->u.file.current_line_num++;

	FileFunctionCall(fgetss, ZEND_NUM_ARGS(), arg2);

	zval_ptr_dtor(&arg2);
} /* }}} */

/* {{{ proto int SplFileObject::fpassthru() U
   Output all remaining data from a file pointer */
SPL_METHOD(SplFileObject, fpassthru)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	RETURN_LONG(php_stream_passthru(intern->u.file.stream));
} /* }}} */

/* {{{ proto bool SplFileObject::fscanf(string format [, string ...]) U
   Implements a mostly ANSI compatible fscanf() */
SPL_METHOD(SplFileObject, fscanf)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	spl_filesystem_file_free_line(intern TSRMLS_CC);
	intern->u.file.current_line_num++;

	FileFunctionCall(fscanf, ZEND_NUM_ARGS(), NULL);
}
/* }}} */

/* {{{ proto mixed SplFileObject::fwrite(string str [, int length]) U
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

/* {{{ proto bool SplFileObject::ftruncate(int size) U
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
		spl_filesystem_file_read_line(getThis(), intern, 1 TSRMLS_CC);
	}
} /* }}} */

/* {{{ Function/Class/Method definitions */
static
ZEND_BEGIN_ARG_INFO_EX(arginfo_file_object___construct, 0, 0, 1)
	ZEND_ARG_INFO(0, file_name)
	ZEND_ARG_INFO(0, open_mode)
	ZEND_ARG_INFO(0, use_include_path)
	ZEND_ARG_INFO(0, context)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_file_object_setFlags, 0) 
	ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_file_object_setMaxLineLen, 0) 
	ZEND_ARG_INFO(0, max_len)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_file_object_fgetcsv, 0, 0, 0) 
	ZEND_ARG_INFO(0, delimiter)
	ZEND_ARG_INFO(0, enclosure)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_file_object_flock, 0, 0, 1) 
	ZEND_ARG_INFO(0, operation)
	ZEND_ARG_INFO(1, wouldblock)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_file_object_fseek, 0, 0, 1) 
	ZEND_ARG_INFO(0, pos)
	ZEND_ARG_INFO(0, whence)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_file_object_fgetss, 0, 0, 0) 
	ZEND_ARG_INFO(0, allowable_tags)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_file_object_fscanf, 0, 0, 1) 
	ZEND_ARG_INFO(0, format)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_file_object_fwrite, 0, 0, 1) 
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_file_object_ftruncate, 0, 0, 1) 
	ZEND_ARG_INFO(0, size)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_file_object_seek, 0, 0, 1) 
	ZEND_ARG_INFO(0, line_pos)
ZEND_END_ARG_INFO()

static const zend_function_entry spl_SplFileObject_functions[] = {
	SPL_ME(SplFileObject, __construct,    arginfo_file_object___construct,   ZEND_ACC_PUBLIC)
	SPL_ME(SplFileObject, rewind,         NULL, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileObject, eof,            NULL, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileObject, valid,          NULL, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileObject, fgets,          NULL, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileObject, fgetcsv,        arginfo_file_object_fgetcsv,       ZEND_ACC_PUBLIC)
	SPL_ME(SplFileObject, setCsvControl,  arginfo_file_object_fgetcsv,       ZEND_ACC_PUBLIC)
	SPL_ME(SplFileObject, getCsvControl,  NULL,                              ZEND_ACC_PUBLIC)
	SPL_ME(SplFileObject, flock,          arginfo_file_object_flock,         ZEND_ACC_PUBLIC)
	SPL_ME(SplFileObject, fflush,         NULL, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileObject, ftell,          NULL, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileObject, fseek,          arginfo_file_object_fseek,         ZEND_ACC_PUBLIC)
	SPL_ME(SplFileObject, fgetc,          NULL, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileObject, fpassthru,      NULL, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileObject, fgetss,         arginfo_file_object_fgetss,        ZEND_ACC_PUBLIC)
	SPL_ME(SplFileObject, fscanf,         arginfo_file_object_fscanf,        ZEND_ACC_PUBLIC)
	SPL_ME(SplFileObject, fwrite,         arginfo_file_object_fwrite,        ZEND_ACC_PUBLIC)
	SPL_ME(SplFileObject, fstat,          NULL, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileObject, ftruncate,      arginfo_file_object_ftruncate,     ZEND_ACC_PUBLIC)
	SPL_ME(SplFileObject, current,        NULL, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileObject, key,            NULL, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileObject, next,           NULL, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileObject, setFlags,       arginfo_file_object_setFlags,      ZEND_ACC_PUBLIC)
	SPL_ME(SplFileObject, getFlags,       NULL, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileObject, setMaxLineLen,  arginfo_file_object_setMaxLineLen, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileObject, getMaxLineLen,  NULL, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileObject, hasChildren,    NULL, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileObject, getChildren,    NULL, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileObject, seek,           arginfo_file_object_seek,          ZEND_ACC_PUBLIC)
	/* mappings */
	SPL_MA(SplFileObject, getCurrentLine, SplFileObject, fgets,      NULL, ZEND_ACC_PUBLIC)
	SPL_MA(SplFileObject, __toString,     SplFileObject, current,    NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_temp_file_object___construct, 0, 0, 0)
	ZEND_ARG_INFO(0, max_memory)
ZEND_END_ARG_INFO()

static const zend_function_entry spl_SplTempFileObject_functions[] = {
	SPL_ME(SplTempFileObject, __construct, arginfo_temp_file_object___construct,  ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */

/* {{{ PHP_MINIT_FUNCTION(spl_directory)
 */
PHP_MINIT_FUNCTION(spl_directory)
{
	u_charsToUChars("glob://", u_glob, sizeof("glob://"));

	REGISTER_SPL_STD_CLASS_EX(SplFileInfo, spl_filesystem_object_new, spl_SplFileInfo_functions);
	memcpy(&spl_filesystem_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	spl_filesystem_object_handlers.clone_obj = spl_filesystem_object_clone;
	spl_filesystem_object_handlers.cast_object = spl_filesystem_object_cast;
	spl_filesystem_object_handlers.get_debug_info = spl_filesystem_object_get_debug_info;

	REGISTER_SPL_SUB_CLASS_EX(DirectoryIterator, SplFileInfo, spl_filesystem_object_new, spl_DirectoryIterator_functions);
	zend_class_implements(spl_ce_DirectoryIterator TSRMLS_CC, 1, zend_ce_iterator);

	spl_ce_DirectoryIterator->get_iterator = spl_filesystem_dir_get_iterator;

	REGISTER_SPL_SUB_CLASS_EX(FilesystemIterator, DirectoryIterator, spl_filesystem_object_new, spl_FilesystemIterator_functions);

	REGISTER_SPL_CLASS_CONST_LONG(FilesystemIterator, "CURRENT_MODE_MASK",   SPL_FILE_DIR_CURRENT_MODE_MASK);
	REGISTER_SPL_CLASS_CONST_LONG(FilesystemIterator, "CURRENT_AS_PATHNAME", SPL_FILE_DIR_CURRENT_AS_PATHNAME);
	REGISTER_SPL_CLASS_CONST_LONG(FilesystemIterator, "CURRENT_AS_FILEINFO", SPL_FILE_DIR_CURRENT_AS_FILEINFO);
	REGISTER_SPL_CLASS_CONST_LONG(FilesystemIterator, "CURRENT_AS_SELF",     0);
	REGISTER_SPL_CLASS_CONST_LONG(FilesystemIterator, "KEY_MODE_MASK",       SPL_FILE_DIR_KEY_MODE_MASK);
	REGISTER_SPL_CLASS_CONST_LONG(FilesystemIterator, "KEY_AS_PATHNAME",     0);
	REGISTER_SPL_CLASS_CONST_LONG(FilesystemIterator, "KEY_AS_FILENAME",     SPL_FILE_DIR_KEY_AS_FILENAME);
	REGISTER_SPL_CLASS_CONST_LONG(FilesystemIterator, "NEW_CURRENT_AND_KEY", SPL_FILE_DIR_KEY_AS_FILENAME|SPL_FILE_DIR_CURRENT_AS_FILEINFO);

	spl_ce_FilesystemIterator->get_iterator = spl_filesystem_tree_get_iterator;

	REGISTER_SPL_SUB_CLASS_EX(RecursiveDirectoryIterator, FilesystemIterator, spl_filesystem_object_new, spl_RecursiveDirectoryIterator_functions);
	REGISTER_SPL_IMPLEMENTS(RecursiveDirectoryIterator, RecursiveIterator);

	REGISTER_SPL_SUB_CLASS_EX(GlobIterator, FilesystemIterator, spl_filesystem_object_new, spl_GlobIterator_functions);
	REGISTER_SPL_IMPLEMENTS(GlobIterator, Countable);

	REGISTER_SPL_SUB_CLASS_EX(SplFileObject, SplFileInfo, spl_filesystem_object_new, spl_SplFileObject_functions);
	REGISTER_SPL_IMPLEMENTS(SplFileObject, RecursiveIterator);
	REGISTER_SPL_IMPLEMENTS(SplFileObject, SeekableIterator);

	REGISTER_SPL_CLASS_CONST_LONG(SplFileObject, "DROP_NEW_LINE", SPL_FILE_OBJECT_DROP_NEW_LINE);
	REGISTER_SPL_CLASS_CONST_LONG(SplFileObject, "READ_AHEAD",    SPL_FILE_OBJECT_READ_AHEAD);
	REGISTER_SPL_CLASS_CONST_LONG(SplFileObject, "SKIP_EMPTY",    SPL_FILE_OBJECT_SKIP_EMPTY);
	REGISTER_SPL_CLASS_CONST_LONG(SplFileObject, "READ_CSV",      SPL_FILE_OBJECT_READ_CSV);
	
	REGISTER_SPL_SUB_CLASS_EX(SplTempFileObject, SplFileObject, spl_filesystem_object_new, spl_SplTempFileObject_functions);
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
