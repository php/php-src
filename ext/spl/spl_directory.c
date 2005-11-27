/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2005 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_0.txt.                                  |
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
static zend_object_handlers spl_ce_dir_handlers;

/* decalre the class entry */
PHPAPI zend_class_entry *spl_ce_DirectoryIterator;
PHPAPI zend_class_entry *spl_ce_RecursiveDirectoryIterator;
PHPAPI zend_class_entry *spl_ce_SplFileObject;

static zend_object_value spl_file_object_new_ex(zend_class_entry *class_type, spl_file_object **obj TSRMLS_DC);
static int spl_file_object_open(spl_file_object *intern, int use_include_path, int silent TSRMLS_DC);

/* {{{ spl_ce_dir_object_free_storage */
/* close all resources and the memory allocated for the object */
static void spl_ce_dir_object_free_storage(void *object TSRMLS_DC)
{
	spl_ce_dir_object *intern = (spl_ce_dir_object *)object;

	zend_hash_destroy(intern->std.properties);
	FREE_HASHTABLE(intern->std.properties);

	if (intern->path) {
		efree(intern->path);
	}
	if (intern->dirp) {
		php_stream_close(intern->dirp);
	}
	if (intern->path_name) {
		efree(intern->path_name);
	}
	if (intern->sub_path) {
		efree(intern->sub_path);
	}
	efree(object);
}
/* }}} */


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
static zend_object_value spl_ce_dir_object_new_ex(zend_class_entry *class_type, spl_ce_dir_object **obj TSRMLS_DC)
{
	zend_object_value retval;
	spl_ce_dir_object *intern;
	zval *tmp;

	intern = emalloc(sizeof(spl_ce_dir_object));
	memset(intern, 0, sizeof(spl_ce_dir_object));
	intern->std.ce = class_type;
	if (obj) *obj = intern;

	ALLOC_HASHTABLE(intern->std.properties);
	zend_hash_init(intern->std.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
	zend_hash_copy(intern->std.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));

	retval.handle = zend_objects_store_put(intern, (zend_objects_store_dtor_t) zend_objects_destroy_object, (zend_objects_free_object_storage_t) spl_ce_dir_object_free_storage, NULL TSRMLS_CC);
	retval.handlers = &spl_ce_dir_handlers;
	return retval;
}
/* }}} */


/* {{{ spl_ce_dir_object_new */
/* See spl_ce_dir_object_new_ex */
static zend_object_value spl_ce_dir_object_new(zend_class_entry *class_type TSRMLS_DC)
{
	return spl_ce_dir_object_new_ex(class_type, NULL TSRMLS_CC);
}
/* }}} */

/* {{{ spl_ce_dir_open */
/* open a directory resource */
static void spl_ce_dir_open(spl_ce_dir_object* intern, char *path TSRMLS_DC)
{
	int path_len = strlen(path);

	intern->dirp = php_stream_opendir(path, ENFORCE_SAFE_MODE|REPORT_ERRORS, NULL);

	if (path_len && (path[path_len-1] == '/' || path[path_len-1] == '\\')) {
		intern->path = estrndup(path, --path_len);
	} else {
		intern->path = estrndup(path, path_len);
	}
	intern->index = 0;

	if (intern->dirp == NULL) {
		/* throw exception: should've been already happened */
		intern->entry.d_name[0] = '\0';
	} else {
		if (!php_stream_readdir(intern->dirp, &intern->entry)) {
			intern->entry.d_name[0] = '\0';
		}
	}
}
/* }}} */

/* {{{ spl_ce_dir_object_clone */
/* Local zend_object_value creation (on stack)
   Load the 'other' object 
   Create a new empty object (See spl_ce_dir_object_new_ex)
   Open the directory
   Clone other members (properties)
 */
static zend_object_value spl_ce_dir_object_clone(zval *zobject TSRMLS_DC)
{
	zend_object_value new_obj_val;
	zend_object *old_object;
	zend_object *new_object;
	zend_object_handle handle = Z_OBJ_HANDLE_P(zobject);
	spl_ce_dir_object *intern;

	old_object = zend_objects_get_address(zobject TSRMLS_CC);
	new_obj_val = spl_ce_dir_object_new_ex(old_object->ce, &intern TSRMLS_CC);
	new_object = &intern->std;

	spl_ce_dir_open(intern, ((spl_ce_dir_object*)old_object)->path TSRMLS_CC);

	zend_objects_clone_members(new_object, new_obj_val, old_object, handle TSRMLS_CC);

	return new_obj_val;
}
/* }}} */

/* {{{ proto void DirectoryIterator::__construct(string path)
 Cronstructs a new dir iterator from a path. */
/* php_set_error_handling() is used to throw exceptions in case
   the constructor fails. Here we use this to ensure the object
   has a valid directory resource.
   
   When the constructor gets called the object is already created 
   by the engine, so we must only call 'additional' initializations.
 */
SPL_METHOD(DirectoryIterator, __construct)
{
	zval *object = getThis();
	spl_ce_dir_object *intern;
	char *path;
	int len;

	php_set_error_handling(EH_THROW, spl_ce_RuntimeException TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &path, &len) == FAILURE) {
		php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
		return;
	}

	intern = (spl_ce_dir_object*)zend_object_store_get_object(object TSRMLS_CC);
	spl_ce_dir_open(intern, path TSRMLS_CC);

	php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
}
/* }}} */

/* {{{ proto void DirectoryIterator::rewind()
   Rewind dir back to the start */
SPL_METHOD(DirectoryIterator, rewind)
{
	zval *object = getThis();
	spl_ce_dir_object *intern = (spl_ce_dir_object*)zend_object_store_get_object(object TSRMLS_CC);

	intern->index = 0;
	if (intern->dirp) {
		php_stream_rewinddir(intern->dirp);
	}
	if (!intern->dirp || !php_stream_readdir(intern->dirp, &intern->entry)) {
		intern->entry.d_name[0] = '\0';
	}
}
/* }}} */

/* {{{ proto string DirectoryIterator::key()
   Return current dir entry */
SPL_METHOD(DirectoryIterator, key)
{
	zval *object = getThis();
	spl_ce_dir_object *intern = (spl_ce_dir_object*)zend_object_store_get_object(object TSRMLS_CC);

	if (intern->dirp) {
		RETURN_LONG(intern->index);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto DirectoryIterator DirectoryIterator::current()
   Return this (needed for Iterator interface) */
SPL_METHOD(DirectoryIterator, current)
{
	RETURN_ZVAL(getThis(), 1, 0);
}
/* }}} */

/* {{{ proto void DirectoryIterator::next()
   Move to next entry */
SPL_METHOD(DirectoryIterator, next)
{
	zval *object = getThis();
	spl_ce_dir_object *intern = (spl_ce_dir_object*)zend_object_store_get_object(object TSRMLS_CC);

	intern->index++;
	if (!intern->dirp || !php_stream_readdir(intern->dirp, &intern->entry)) {
		intern->entry.d_name[0] = '\0';
	}
	if (intern->path_name) {
		efree(intern->path_name);
		intern->path_name = NULL;
	}
}
/* }}} */

/* {{{ proto string DirectoryIterator::valid()
   Check whether dir contains more entries */
SPL_METHOD(DirectoryIterator, valid)
{
	zval *object = getThis();
	spl_ce_dir_object *intern = (spl_ce_dir_object*)zend_object_store_get_object(object TSRMLS_CC);

	RETURN_BOOL(intern->entry.d_name[0] != '\0');
}
/* }}} */

/* {{{ proto string DirectoryIterator::getPath()
   Return directory path */
SPL_METHOD(DirectoryIterator, getPath)
{
	zval *object = getThis();
	spl_ce_dir_object *intern = (spl_ce_dir_object*)zend_object_store_get_object(object TSRMLS_CC);

	RETURN_STRING(intern->path, 1);
}
/* }}} */

/* {{{ proto string DirectoryIterator::getFilename()
   Return filename of current dir entry */
SPL_METHOD(DirectoryIterator, getFilename)
{
	zval *object = getThis();
	spl_ce_dir_object *intern = (spl_ce_dir_object*)zend_object_store_get_object(object TSRMLS_CC);

	RETURN_STRING(intern->entry.d_name, 1);
}
/* }}} */

static inline void spl_dir_get_path_name(spl_ce_dir_object *intern)
{
	if (!intern->path_name) {
		intern->path_name_len = spprintf(&intern->path_name, 0, "%s/%s", intern->path, intern->entry.d_name);
	}
}

/* {{{ proto string DirectoryIterator::getPathname()
   Return path and filename of current dir entry */
SPL_METHOD(DirectoryIterator, getPathname)
{
	zval *object = getThis();
	spl_ce_dir_object *intern = (spl_ce_dir_object*)zend_object_store_get_object(object TSRMLS_CC);

	if (intern->entry.d_name[0]) {
		spl_dir_get_path_name(intern);
		RETURN_STRINGL(intern->path_name, intern->path_name_len, 1);
	} else {
		RETURN_BOOL(0);
	}
}
/* }}} */

/* {{{ proto string RecursiveDirectoryIterator::key()
   Return path and filename of current dir entry */
SPL_METHOD(RecursiveDirectoryIterator, key)
{
	zval *object = getThis();
	spl_ce_dir_object *intern = (spl_ce_dir_object*)zend_object_store_get_object(object TSRMLS_CC);

	spl_dir_get_path_name(intern);
	RETURN_STRINGL(intern->path_name, intern->path_name_len, 1);
}
/* }}} */

/* {{{ proto bool DirectoryIterator::isDot()
   Returns true if current entry is '.' or  '..' */
SPL_METHOD(DirectoryIterator, isDot)
{
	zval *object = getThis();
	spl_ce_dir_object *intern = (spl_ce_dir_object*)zend_object_store_get_object(object TSRMLS_CC);

	RETURN_BOOL(!strcmp(intern->entry.d_name, ".") || !strcmp(intern->entry.d_name, ".."));
}
/* }}} */

/* {{{ DirectoryFunction */
#define DirectoryFunction(func_name, func_num) \
SPL_METHOD(DirectoryIterator, func_name) \
{ \
	spl_ce_dir_object *intern = (spl_ce_dir_object*)zend_object_store_get_object(getThis() TSRMLS_CC); \
 \
	spl_dir_get_path_name(intern); \
	php_stat(intern->path_name, intern->path_name_len, func_num, return_value TSRMLS_CC); \
}
/* }}} */

/* {{{ proto int DirectoryIterator::getPerms()
   Get file permissions */
DirectoryFunction(getPerms, FS_PERMS)
/* }}} */

/* {{{ proto int DirectoryIterator::getInode()
   Get file inode */
DirectoryFunction(getInode, FS_INODE)
/* }}} */

/* {{{ proto int DirectoryIterator::getSize()
   Get file size */
DirectoryFunction(getSize, FS_SIZE)
/* }}} */

/* {{{ proto int DirectoryIterator::getOwner()
   Get file owner */
DirectoryFunction(getOwner, FS_OWNER)
/* }}} */

/* {{{ proto int DirectoryIterator::getGroup()
   Get file group */
DirectoryFunction(getGroup, FS_GROUP)
/* }}} */

/* {{{ proto int DirectoryIterator::getATime()
   Get last access time of file */
DirectoryFunction(getATime, FS_ATIME)
/* }}} */

/* {{{ proto int DirectoryIterator::getMTime()
   Get last modification time of file */
DirectoryFunction(getMTime, FS_MTIME)
/* }}} */

/* {{{ proto int DirectoryIterator::getCTime()
   Get inode modification time of file */
DirectoryFunction(getCTime, FS_CTIME)
/* }}} */

/* {{{ proto string DirectoryIterator::getType()
   Get file type */
DirectoryFunction(getType, FS_TYPE)
/* }}} */

/* {{{ proto bool DirectoryIterator::isWritable()
   Returns true if file can be written */
DirectoryFunction(isWritable, FS_IS_W)
/* }}} */

/* {{{ proto bool DirectoryIterator::isReadable()
   Returns true if file can be read */
DirectoryFunction(isReadable, FS_IS_R)
/* }}} */

/* {{{ proto bool DirectoryIterator::isExecutable()
   Returns true if file is executable */
DirectoryFunction(isExecutable, FS_IS_X)
/* }}} */

/* {{{ proto bool DirectoryIterator::isFile()
   Returns true if file is a regular file */
DirectoryFunction(isFile, FS_IS_FILE)
/* }}} */

/* {{{ proto bool DirectoryIterator::isDir()
   Returns true if file is directory */
DirectoryFunction(isDir, FS_IS_DIR)
/* }}} */

/* {{{ proto bool DirectoryIterator::isLink()
   Returns true if file is symbolic link */
DirectoryFunction(isLink, FS_IS_LINK)
/* }}} */

/* {{{ proto SplFileObject DirectoryIterator::openFile([string mode = 'r' [, bool use_include_path  [, resource context]]])
   Open the current file */
SPL_METHOD(DirectoryIterator, openFile)
{
	zval *object = getThis();
	spl_ce_dir_object *dir_obj = (spl_ce_dir_object*)zend_object_store_get_object(object TSRMLS_CC);
	spl_file_object *intern;
	zend_bool use_include_path = 0;

	php_set_error_handling(EH_THROW, spl_ce_RuntimeException TSRMLS_CC);

	if (!dir_obj->entry.d_name[0]) {
		zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC, "Could not open file");
		zval_dtor(return_value);
		return;
	}

	return_value->value.obj = spl_file_object_new_ex(spl_ce_SplFileObject, &intern TSRMLS_CC);

	spl_dir_get_path_name(dir_obj);
	intern->file_name = dir_obj->path_name;
	intern->file_name_len = dir_obj->path_name_len;

	intern->open_mode = "r";
	intern->open_mode_len = 1;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|sbr", 
			&intern->open_mode, &intern->open_mode_len, 
			&use_include_path, &intern->zcontext) == FAILURE) {
		php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
		zval_dtor(return_value);
		return;
	}

	if (spl_file_object_open(intern, use_include_path, 0 TSRMLS_CC) == SUCCESS) {
		Z_TYPE_P(return_value) = IS_OBJECT;
	} else {
		zval_dtor(return_value);
		return;
	}

	php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
} /* }}} */

/* {{{ proto void RecursiveDirectoryIterator::rewind()
   Rewind dir back to the start */
SPL_METHOD(RecursiveDirectoryIterator, rewind)
{
	zval *object = getThis();
	spl_ce_dir_object *intern = (spl_ce_dir_object*)zend_object_store_get_object(object TSRMLS_CC);

	intern->index = 0;
	if (intern->dirp) {
		php_stream_rewinddir(intern->dirp);
	}
	do {
		if (!intern->dirp || !php_stream_readdir(intern->dirp, &intern->entry)) {
			intern->entry.d_name[0] = '\0';
		}
	} while (!strcmp(intern->entry.d_name, ".") || !strcmp(intern->entry.d_name, ".."));
}
/* }}} */

/* {{{ proto void RecursiveDirectoryIterator::next()
   Move to next entry */
SPL_METHOD(RecursiveDirectoryIterator, next)
{
	zval *object = getThis();
	spl_ce_dir_object *intern = (spl_ce_dir_object*)zend_object_store_get_object(object TSRMLS_CC);

	intern->index++;
	do {
		if (!intern->dirp || !php_stream_readdir(intern->dirp, &intern->entry)) {
			intern->entry.d_name[0] = '\0';
		}
	} while (!strcmp(intern->entry.d_name, ".") || !strcmp(intern->entry.d_name, ".."));
	if (intern->path_name) {
		efree(intern->path_name);
		intern->path_name = NULL;
	}
}
/* }}} */

/* {{{ proto bool RecursiveDirectoryIterator::hasChildren([bool $allow_links = false])
   Returns whether current entry is a directory and not '.' or '..' */
SPL_METHOD(RecursiveDirectoryIterator, hasChildren)
{
	zend_bool allow_links = 0;
	zval *object = getThis();
	spl_ce_dir_object *intern = (spl_ce_dir_object*)zend_object_store_get_object(object TSRMLS_CC);
	
	if (!strcmp(intern->entry.d_name, ".") || !strcmp(intern->entry.d_name, "..")) {
		RETURN_BOOL(0);
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|b", &allow_links) == FAILURE) {
			return;
		}
		spl_dir_get_path_name(intern);
		if (!allow_links) {
			php_stat(intern->path_name, intern->path_name_len, FS_IS_LINK, return_value TSRMLS_CC);
			if (zend_is_true(return_value)) {
				RETURN_BOOL(0);
			}
		}
		php_stat(intern->path_name, intern->path_name_len, FS_IS_DIR, return_value TSRMLS_CC);
    }
}
/* }}} */

/* {{{ proto RecursiveDirectoryIterator DirectoryIterator::getChildren()
   Returns an iterator for the current entry if it is a directory */
SPL_METHOD(RecursiveDirectoryIterator, getChildren)
{
	zval *object = getThis(), zpath;
	spl_ce_dir_object *intern = (spl_ce_dir_object*)zend_object_store_get_object(object TSRMLS_CC);
	spl_ce_dir_object *subdir;
	
	spl_dir_get_path_name(intern);

	INIT_PZVAL(&zpath);
	ZVAL_STRINGL(&zpath, intern->path_name, intern->path_name_len, 0);

	spl_instantiate_arg_ex1(spl_ce_RecursiveDirectoryIterator, &return_value, 0, &zpath TSRMLS_CC);
	
	subdir = (spl_ce_dir_object*)zend_object_store_get_object(return_value TSRMLS_CC);
	if (subdir) {
		if (intern->sub_path && intern->sub_path[0]) {
			subdir->sub_path_len = spprintf(&subdir->sub_path, 0, "%s/%s", intern->sub_path, intern->entry.d_name);
		} else {
			subdir->sub_path_len = strlen(intern->entry.d_name);
			subdir->sub_path = estrndup(intern->entry.d_name, subdir->sub_path_len);
		}
	}
}
/* }}} */

/* {{{ proto void RecursiveDirectoryIterator::getSubPath()
   Get sub path */
SPL_METHOD(RecursiveDirectoryIterator, getSubPath)
{
	zval *object = getThis();
	spl_ce_dir_object *intern = (spl_ce_dir_object*)zend_object_store_get_object(object TSRMLS_CC);

	if (intern->sub_path) {
		RETURN_STRINGL(intern->sub_path, intern->sub_path_len, 1);
	} else {
		RETURN_STRINGL("", 0, 1);
	}
}
/* }}} */

/* {{{ proto void RecursiveDirectoryIterator::getSubPathname()
   Get sub path and file name */
SPL_METHOD(RecursiveDirectoryIterator, getSubPathname)
{
	zval *object = getThis();
	spl_ce_dir_object *intern = (spl_ce_dir_object*)zend_object_store_get_object(object TSRMLS_CC);
	char *sub_name;
	int len;

	if (intern->sub_path) {
		len = spprintf(&sub_name, 0, "%s/%s", intern->sub_path, intern->entry.d_name);
		RETURN_STRINGL(sub_name, len, 0);
	} else {
		RETURN_STRING(intern->entry.d_name, 1);
	}
}
/* }}} */

/* define an overloaded iterator structure */
typedef struct {
	zend_object_iterator  intern;
	zval                  *current;
	spl_ce_dir_object     *object;
} spl_ce_dir_it;

/* forward declarations to the iterator handlers */
static void spl_ce_dir_it_dtor(zend_object_iterator *iter TSRMLS_DC);
static int spl_ce_dir_it_valid(zend_object_iterator *iter TSRMLS_DC);
static void spl_ce_dir_it_current_data(zend_object_iterator *iter, zval ***data TSRMLS_DC);
static int spl_ce_dir_it_current_key(zend_object_iterator *iter, char **str_key, uint *str_key_len, ulong *int_key TSRMLS_DC);
static void spl_ce_dir_it_move_forward(zend_object_iterator *iter TSRMLS_DC);
static void spl_ce_dir_it_rewind(zend_object_iterator *iter TSRMLS_DC);

/* iterator handler table */
zend_object_iterator_funcs spl_ce_dir_it_funcs = {
	spl_ce_dir_it_dtor,
	spl_ce_dir_it_valid,
	spl_ce_dir_it_current_data,
	spl_ce_dir_it_current_key,
	spl_ce_dir_it_move_forward,
	spl_ce_dir_it_rewind
};

/* {{{ spl_ce_dir_get_iterator */
zend_object_iterator *spl_ce_dir_get_iterator(zend_class_entry *ce, zval *object TSRMLS_DC)
{
	spl_ce_dir_it       *iterator   = emalloc(sizeof(spl_ce_dir_it));
	spl_ce_dir_object   *dir_object = (spl_ce_dir_object*)zend_object_store_get_object(object TSRMLS_CC);

	object->refcount++;
	iterator->intern.data = (void*)object;
	iterator->intern.funcs = &spl_ce_dir_it_funcs;
	iterator->current = object;
	object->refcount++;
	iterator->object = dir_object;
	
	return (zend_object_iterator*)iterator;
}
/* }}} */

/* {{{ spl_ce_dir_it_dtor */
static void spl_ce_dir_it_dtor(zend_object_iterator *iter TSRMLS_DC)
{
	spl_ce_dir_it *iterator = (spl_ce_dir_it *)iter;

	zval_ptr_dtor(&iterator->current);
	zval_ptr_dtor((zval**)&iterator->intern.data);

	efree(iterator);
}
/* }}} */
	
/* {{{ spl_ce_dir_it_valid */
static int spl_ce_dir_it_valid(zend_object_iterator *iter TSRMLS_DC)
{
	spl_ce_dir_it       *iterator = (spl_ce_dir_it *)iter;
	spl_ce_dir_object   *object   = iterator->object;

	return object->entry.d_name[0] != '\0' ? SUCCESS : FAILURE;
}
/* }}} */


/* {{{ spl_ce_dir_it_current_data */
static void spl_ce_dir_it_current_data(zend_object_iterator *iter, zval ***data TSRMLS_DC)
{
	spl_ce_dir_it *iterator = (spl_ce_dir_it *)iter;
	
	*data = &iterator->current;
}
/* }}} */

/* {{{ spl_ce_dir_it_current_key */
static int spl_ce_dir_it_current_key(zend_object_iterator *iter, char **str_key, uint *str_key_len, ulong *int_key TSRMLS_DC)
{
	spl_ce_dir_it       *iterator = (spl_ce_dir_it *)iter;
	spl_ce_dir_object   *object   = iterator->object;
	
	*int_key = object->index;
	return HASH_KEY_IS_LONG;
}
/* }}} */

/* {{{ spl_ce_dir_it_move_forward */
static void spl_ce_dir_it_move_forward(zend_object_iterator *iter TSRMLS_DC)
{
	spl_ce_dir_it       *iterator = (spl_ce_dir_it *)iter;
	spl_ce_dir_object   *object   = iterator->object;
	
	object->index++;
	if (!object->dirp || !php_stream_readdir(object->dirp, &object->entry)) {
		object->entry.d_name[0] = '\0';
	}
	if (object->path_name) {
		efree(object->path_name);
		object->path_name = NULL;
	}
}
/* }}} */

/* {{{ spl_ce_dir_it_rewind */
static void spl_ce_dir_it_rewind(zend_object_iterator *iter TSRMLS_DC)
{
	spl_ce_dir_it       *iterator = (spl_ce_dir_it *)iter;
	spl_ce_dir_object   *object   = iterator->object;
	
	object->index = 0;
	if (object->dirp) {
		php_stream_rewinddir(object->dirp);
	}
	if (!object->dirp || !php_stream_readdir(object->dirp, &object->entry)) {
		object->entry.d_name[0] = '\0';
	}
}
/* }}} */

/* {{{ spl_ce_dir_tree_it_current_key */
static int spl_ce_dir_tree_it_current_key(zend_object_iterator *iter, char **str_key, uint *str_key_len, ulong *int_key TSRMLS_DC)
{
	spl_ce_dir_it       *iterator = (spl_ce_dir_it *)iter;
	spl_ce_dir_object   *object   = iterator->object;
	
	spl_dir_get_path_name(object);
	*str_key_len = object->path_name_len + 1;
	*str_key = estrndup(object->path_name, object->path_name_len);
	return HASH_KEY_IS_STRING;
}
/* }}} */

/* {{{ spl_ce_dir_tree_it_move_forward */
static void spl_ce_dir_tree_it_move_forward(zend_object_iterator *iter TSRMLS_DC)
{
	spl_ce_dir_it       *iterator = (spl_ce_dir_it *)iter;
	spl_ce_dir_object   *object   = iterator->object;
	
	object->index++;
	do {
		if (!object->dirp || !php_stream_readdir(object->dirp, &object->entry)) {
			object->entry.d_name[0] = '\0';
		}
	} while (!strcmp(object->entry.d_name, ".") || !strcmp(object->entry.d_name, ".."));
	if (object->path_name) {
		efree(object->path_name);
		object->path_name = NULL;
	}
}
/* }}} */

/* {{{ spl_ce_dir_tree_it_rewind */
static void spl_ce_dir_tree_it_rewind(zend_object_iterator *iter TSRMLS_DC)
{
	spl_ce_dir_it       *iterator = (spl_ce_dir_it *)iter;
	spl_ce_dir_object   *object   = iterator->object;
	
	object->index = 0;
	if (object->dirp) {
		php_stream_rewinddir(object->dirp);
	}
	do {
		if (!object->dirp || !php_stream_readdir(object->dirp, &object->entry)) {
			object->entry.d_name[0] = '\0';
		}
	} while (!strcmp(object->entry.d_name, ".") || !strcmp(object->entry.d_name, ".."));
}
/* }}} */

/* iterator handler table */
zend_object_iterator_funcs spl_ce_dir_tree_it_funcs = {
	spl_ce_dir_it_dtor,
	spl_ce_dir_it_valid,
	spl_ce_dir_it_current_data,
	spl_ce_dir_tree_it_current_key,
	spl_ce_dir_tree_it_move_forward,
	spl_ce_dir_tree_it_rewind
};

/* {{{ spl_ce_dir_get_iterator */
zend_object_iterator *spl_ce_dir_tree_get_iterator(zend_class_entry *ce, zval *object TSRMLS_DC)
{
	spl_ce_dir_it       *iterator   = emalloc(sizeof(spl_ce_dir_it));
	spl_ce_dir_object   *dir_object = (spl_ce_dir_object*)zend_object_store_get_object(object TSRMLS_CC);

	object->refcount++;
	iterator->intern.data = (void*)object;
	iterator->intern.funcs = &spl_ce_dir_tree_it_funcs;
	iterator->current = object;
	object->refcount++;
	iterator->object = dir_object;
	
	return (zend_object_iterator*)iterator;
}
/* }}} */

/* {{{ spl_ce_dir_cast */
static int spl_ce_dir_cast(zval *readobj, zval *writeobj, int type, int should_free TSRMLS_DC)
{
	zval free_obj;
	spl_ce_dir_object   *dir_object = (spl_ce_dir_object*)zend_object_store_get_object(readobj TSRMLS_CC);

	if (type ==IS_STRING && *dir_object->entry.d_name) {
		if (should_free) {
			free_obj = *writeobj;
		}
		ZVAL_STRING(writeobj, dir_object->entry.d_name, 1);
		if (should_free) {
			zval_dtor(&free_obj);
		}
		return SUCCESS;
	}
	return FAILURE;
}
/* }}} */

/* declare method parameters */
/* supply a name and default to call by parameter */
static
ZEND_BEGIN_ARG_INFO(arginfo_dir___construct, 0) 
	ZEND_ARG_INFO(0, path)  /* parameter name */
ZEND_END_ARG_INFO();

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_dir_openFile, 0, 0, 0)
	ZEND_ARG_INFO(0, open_mode)
	ZEND_ARG_INFO(0, use_include_path)
	ZEND_ARG_INFO(0, context)
ZEND_END_ARG_INFO();

/* the method table */
/* each method can have its own parameters and visibility */
static zend_function_entry spl_ce_dir_class_functions[] = {
	SPL_ME(DirectoryIterator, __construct,   arginfo_dir___construct, ZEND_ACC_PUBLIC)
	SPL_ME(DirectoryIterator, rewind,        NULL, ZEND_ACC_PUBLIC)
	SPL_ME(DirectoryIterator, valid,         NULL, ZEND_ACC_PUBLIC)
	SPL_ME(DirectoryIterator, key,           NULL, ZEND_ACC_PUBLIC)
	SPL_ME(DirectoryIterator, current,       NULL, ZEND_ACC_PUBLIC)
	SPL_ME(DirectoryIterator, next,          NULL, ZEND_ACC_PUBLIC)
	SPL_ME(DirectoryIterator, getPath,       NULL, ZEND_ACC_PUBLIC)
	SPL_ME(DirectoryIterator, getFilename,   NULL, ZEND_ACC_PUBLIC)
	SPL_ME(DirectoryIterator, getPathname,   NULL, ZEND_ACC_PUBLIC)
	SPL_ME(DirectoryIterator, getPerms,      NULL, ZEND_ACC_PUBLIC)
	SPL_ME(DirectoryIterator, getInode,      NULL, ZEND_ACC_PUBLIC)
	SPL_ME(DirectoryIterator, getSize,       NULL, ZEND_ACC_PUBLIC)
	SPL_ME(DirectoryIterator, getOwner,      NULL, ZEND_ACC_PUBLIC)
	SPL_ME(DirectoryIterator, getGroup,      NULL, ZEND_ACC_PUBLIC)
	SPL_ME(DirectoryIterator, getATime,      NULL, ZEND_ACC_PUBLIC)
	SPL_ME(DirectoryIterator, getMTime,      NULL, ZEND_ACC_PUBLIC)
	SPL_ME(DirectoryIterator, getCTime,      NULL, ZEND_ACC_PUBLIC)
	SPL_ME(DirectoryIterator, getType,       NULL, ZEND_ACC_PUBLIC)
	SPL_ME(DirectoryIterator, isWritable,    NULL, ZEND_ACC_PUBLIC)
	SPL_ME(DirectoryIterator, isReadable,    NULL, ZEND_ACC_PUBLIC)
	SPL_ME(DirectoryIterator, isExecutable,  NULL, ZEND_ACC_PUBLIC)
	SPL_ME(DirectoryIterator, isFile,        NULL, ZEND_ACC_PUBLIC)
	SPL_ME(DirectoryIterator, isDir,         NULL, ZEND_ACC_PUBLIC)
	SPL_ME(DirectoryIterator, isLink,        NULL, ZEND_ACC_PUBLIC)
	SPL_ME(DirectoryIterator, isDot,         NULL, ZEND_ACC_PUBLIC)
	SPL_ME(DirectoryIterator, openFile,      arginfo_dir_openFile, ZEND_ACC_PUBLIC)
	SPL_MA(DirectoryIterator, __toString, DirectoryIterator, getFilename, NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

static zend_function_entry spl_ce_dir_tree_class_functions[] = {
	SPL_ME(RecursiveDirectoryIterator, rewind,        NULL, ZEND_ACC_PUBLIC)
	SPL_ME(RecursiveDirectoryIterator, next,          NULL, ZEND_ACC_PUBLIC)
	SPL_ME(RecursiveDirectoryIterator, key,           NULL, ZEND_ACC_PUBLIC)
	SPL_ME(RecursiveDirectoryIterator, hasChildren,   NULL, ZEND_ACC_PUBLIC)
	SPL_ME(RecursiveDirectoryIterator, getChildren,   NULL, ZEND_ACC_PUBLIC)
	SPL_ME(RecursiveDirectoryIterator, getSubPath,    NULL, ZEND_ACC_PUBLIC)
	SPL_ME(RecursiveDirectoryIterator, getSubPathname,NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

static void spl_file_object_free_line(spl_file_object *intern TSRMLS_DC) /* {{{ */
{
	if (intern->current_line) {
		efree(intern->current_line);
		intern->current_line = NULL;
	}
	if (intern->current_zval) {
		zval_ptr_dtor(&intern->current_zval);
		intern->current_zval = NULL;
	}
} /* }}} */

static void spl_file_object_free_storage(void *object TSRMLS_DC) /* {{{ */
{
	spl_file_object *intern = (spl_file_object*)object;

	zend_hash_destroy(intern->std.properties);
	FREE_HASHTABLE(intern->std.properties);

	if (intern->stream) {
		if (intern->zcontext) {
/*			zend_list_delref(Z_RESVAL_P(intern->zcontext));*/
		}
		if (!intern->stream->is_persistent) {
			php_stream_free(intern->stream, PHP_STREAM_FREE_CLOSE);
		} else {
			php_stream_free(intern->stream, PHP_STREAM_FREE_CLOSE_PERSISTENT);
		}
		if (intern->file_name) {
			efree(intern->file_name);
		}
		if (intern->open_mode) {
			efree(intern->open_mode);
		}
	}
	spl_file_object_free_line(intern TSRMLS_CC);
	efree(object);
} /* }}} */

static zend_object_value spl_file_object_new_ex(zend_class_entry *class_type, spl_file_object **obj TSRMLS_DC) /* {{{ */
{
	zend_object_value retval;
	spl_file_object *intern;
	zval *tmp;

	intern = emalloc(sizeof(spl_ce_dir_object));
	memset(intern, 0, sizeof(spl_ce_dir_object));
	intern->std.ce = class_type;
	if (obj) *obj = intern;

	ALLOC_HASHTABLE(intern->std.properties);
	zend_hash_init(intern->std.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
	zend_hash_copy(intern->std.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));

	retval.handle = zend_objects_store_put(intern, (zend_objects_store_dtor_t) zend_objects_destroy_object, (zend_objects_free_object_storage_t) spl_file_object_free_storage, NULL TSRMLS_CC);
	retval.handlers = &spl_ce_dir_handlers;
	return retval;
} /* }}} */

static zend_object_value spl_file_object_new(zend_class_entry *class_type TSRMLS_DC) /* {{{ */
{
	return spl_file_object_new_ex(class_type, NULL TSRMLS_CC);
} /* }}} */

static int spl_file_object_read(spl_file_object *intern, int silent TSRMLS_DC) /* {{{ */
{
	char *buf;
	size_t line_len;
	int len;
	long line_add = (intern->current_line || intern->current_zval) ? 1 : 0;

	spl_file_object_free_line(intern TSRMLS_CC);
	
	if (php_stream_eof(intern->stream)) {
		if (!silent) {
			zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC, "Cannot read from file %s", intern->file_name);
		}
		return FAILURE;
	}

	buf = php_stream_get_line(intern->stream, NULL, intern->max_line_len, &line_len);

	if (!buf) {
		intern->current_line = estrdup("");
		intern->current_line_len = 0;
	} else {
		if (intern->flags & SPL_FILE_OBJECT_DROP_NEW_LINE) {
			line_len = strcspn(buf, "\r\n");
			buf[line_len] = '\0';
		}
	
		if (PG(magic_quotes_runtime)) {
			buf = php_addslashes(buf, line_len, &len, 1 TSRMLS_CC);
			line_len = len;
		}
	
		intern->current_line = buf;
		intern->current_line_len = line_len;
	}
	intern->current_line_num += line_add;

	return SUCCESS;
} /* }}} */

static int spl_file_object_read_line(zval * this_ptr, spl_file_object *intern, int silent TSRMLS_DC) /* {{{ */
{
	zval *retval;

	/* if overloaded call the function, otherwise do it directly */
	if (intern->func_getCurr->common.scope != spl_ce_SplFileObject) {
		if (php_stream_eof(intern->stream)) {
			if (!silent) {
				zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC, "Cannot read from file %s", intern->file_name);
			}
			return FAILURE;
		}
		zend_call_method_with_0_params(&getThis(), Z_OBJCE_P(getThis()), &intern->func_getCurr, "getCurrentLine", &retval);
		if (retval) {
			if (intern->current_line || intern->current_zval) {
				intern->current_line_num++;
			}
			spl_file_object_free_line(intern TSRMLS_CC);
			if (Z_TYPE_P(retval) == IS_STRING) {
				intern->current_line = estrndup(Z_STRVAL_P(retval), Z_STRLEN_P(retval));
				intern->current_line_len = Z_STRLEN_P(retval);
			} else {
				MAKE_STD_ZVAL(intern->current_zval);
				ZVAL_ZVAL(intern->current_zval, retval, 1, 0);
			}
			zval_ptr_dtor(&retval);
			return SUCCESS;
		} else {
			return FAILURE;
		}
	} else {
		return spl_file_object_read(intern, silent TSRMLS_CC);
	}
} /* }}} */

static void spl_file_object_rewind(spl_file_object *intern TSRMLS_DC) /* {{{ */
{
	if (-1 == php_stream_rewind(intern->stream)) {
		zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC, "Cannot rewind file %s", intern->file_name);
	} else {
		spl_file_object_free_line(intern TSRMLS_CC);
		intern->current_line_num = 0;
	}
} /* }}} */

static int spl_file_object_open(spl_file_object *intern, int use_include_path, int silent TSRMLS_DC) /* {{{ */
{
	intern->context = php_stream_context_from_zval(intern->zcontext, 0);
	intern->stream = php_stream_open_wrapper_ex(intern->file_name, intern->open_mode, (use_include_path ? USE_PATH : 0) | ENFORCE_SAFE_MODE | REPORT_ERRORS, NULL, intern->context);

	if (intern->stream == NULL) {
		if (!EG(exception)) {
			zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC, "Cannot open file %s", intern->file_name);
		}
		return FAILURE;
	}

	if (intern->zcontext) {
		zend_list_addref(Z_RESVAL_P(intern->zcontext));
	}

	intern->file_name = estrndup(intern->file_name, intern->file_name_len);
	intern->open_mode = estrndup(intern->open_mode, intern->open_mode_len);

	/* avoid reference counting in debug mode, thus do it manually */
	ZVAL_RESOURCE(&intern->zresource, php_stream_get_resource_id(intern->stream));
	intern->zresource.refcount = 1;

	zend_hash_find(&intern->std.ce->function_table, "getcurrentline", sizeof("getcurrentline"), (void **) &intern->func_getCurr);

	return SUCCESS;
} /* }}} */

/* {{{ proto void SplFileObject::__construct(string filename [, string mode = 'r' [, bool use_include_path  [, resource context]]]])
   Construct a new file reader */
SPL_METHOD(SplFileObject, __construct)
{
	spl_file_object *intern = (spl_file_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	zend_bool use_include_path = 0;

	php_set_error_handling(EH_THROW, spl_ce_RuntimeException TSRMLS_CC);

	intern->open_mode = "r";
	intern->open_mode_len = 1;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|sbr", 
			&intern->file_name, &intern->file_name_len,
			&intern->open_mode, &intern->open_mode_len, 
			&use_include_path, &intern->zcontext) == FAILURE) {
		php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
		return;
	}
	
	spl_file_object_open(intern, use_include_path, 0 TSRMLS_CC);

	php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
} /* }}} */

/* {{{ proto void SplFileObject::rewind()
   Rewind the file and read the first line */
SPL_METHOD(SplFileObject, rewind)
{
	spl_file_object *intern = (spl_file_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	spl_file_object_rewind(intern TSRMLS_CC);
} /* }}} */

/* {{{ proto string SplFileObject::getFilename()
   Return the filename */
SPL_METHOD(SplFileObject, getFilename)
{
	spl_file_object *intern = (spl_file_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	RETURN_STRINGL(intern->file_name, intern->file_name_len, 1);	
} /* }}} */

/* {{{ proto void SplFileObject::eof()
   Return whether end of file is reached */
SPL_METHOD(SplFileObject, eof)
{
	spl_file_object *intern = (spl_file_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	RETURN_BOOL(php_stream_eof(intern->stream));
} /* }}} */

/* {{{ proto void SplFileObject::valid()
   Return !eof() */
SPL_METHOD(SplFileObject, valid)
{
	spl_file_object *intern = (spl_file_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	RETVAL_BOOL(!php_stream_eof(intern->stream));
} /* }}} */

/* {{{ proto string SplFileObject::fgets()
   Rturn next line from file */
SPL_METHOD(SplFileObject, fgets)
{
	spl_file_object *intern = (spl_file_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (spl_file_object_read(intern, 0 TSRMLS_CC) == FAILURE) {
		RETURN_FALSE;
	}
	RETURN_STRINGL(intern->current_line, intern->current_line_len, 1);
} /* }}} */

/* {{{ proto string SplFileObject::current()
   Return current line from file */
SPL_METHOD(SplFileObject, current)
{
	spl_file_object *intern = (spl_file_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (!intern->current_line) {
		spl_file_object_read_line(getThis(), intern, 1 TSRMLS_CC);
	}
	if (intern->current_line) {
		RETURN_STRINGL(intern->current_line, intern->current_line_len, 1);
	} else if (intern->current_zval) {
		RETURN_ZVAL(intern->current_zval, 1, 0);
	}
	RETURN_FALSE;
} /* }}} */

/* {{{ proto int SplFileObject::key()
   Return line number */
SPL_METHOD(SplFileObject, key)
{
	spl_file_object *intern = (spl_file_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

/*	Do not read the next line to support correct counting with fgetc()
	if (!intern->current_line) {
		spl_file_object_read_line(getThis(), intern, 1 TSRMLS_CC);
	} */
	RETURN_LONG(intern->current_line_num);
} /* }}} */

/* {{{ proto void SplFileObject::next()
   Read next line */
SPL_METHOD(SplFileObject, next)
{
	spl_file_object *intern = (spl_file_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	spl_file_object_free_line(intern TSRMLS_CC);
	intern->current_line_num++;
} /* }}} */

/* {{{ proto void SplFileObject::setFlags(int flags)
   Set file handling flags */
SPL_METHOD(SplFileObject, setFlags)
{
	spl_file_object *intern = (spl_file_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &intern->flags);
} /* }}} */

/* {{{ proto int SplFileObject::getFlags()
   Get file handling flags */
SPL_METHOD(SplFileObject, getFlags)
{
	spl_file_object *intern = (spl_file_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	RETURN_LONG(intern->flags);
} /* }}} */

/* {{{ proto void SplFileObject::setMaxLineLen(int max_len)
   Set maximum line length */
SPL_METHOD(SplFileObject, setMaxLineLen)
{
	long max_len;

	spl_file_object *intern = (spl_file_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &max_len) == FAILURE) {
		return;
	}

	if (max_len < 0) {
		zend_throw_exception_ex(spl_ce_DomainException, 0 TSRMLS_CC, "Maximum line length must be greater than or equal zero");
		return;
	}
	
	intern->max_line_len = max_len;
} /* }}} */

/* {{{ proto int SplFileObject::getMaxLineLen()
   Get maximum line length */
SPL_METHOD(SplFileObject, getMaxLineLen)
{
	spl_file_object *intern = (spl_file_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	RETURN_LONG((long)intern->max_line_len);
} /* }}} */

/* {{{ proto bool hasChildren()
   Rturn false */
SPL_METHOD(SplFileObject, hasChildren)
{
	RETURN_FALSE;
} /* }}} */

/* {{{ proto bool SplFileObject::getChildren()
   Read NULL */
SPL_METHOD(SplFileObject, getChildren)
{
	/* return NULL */
} /* }}} */

static int spl_file_object_call(INTERNAL_FUNCTION_PARAMETERS, spl_file_object *intern, zend_function *func_ptr, zval *arg2) /* {{{ */
{
	zend_fcall_info fci;
	zend_fcall_info_cache fcic;
	zval z_fname;
	zval * zresource_ptr = &intern->zresource, *retval;
	int result;

	zval ***params = (zval***)safe_emalloc(ZEND_NUM_ARGS(), sizeof(zval**), (arg2 ? 2 : 1) * sizeof(zval**));

	params[0] = &zresource_ptr;
	
	if (arg2) {
		params[1] = &arg2;
	}

	zend_get_parameters_array_ex(ZEND_NUM_ARGS(), params+(arg2 ? 2 : 1));

	ZVAL_STRING(&z_fname, func_ptr->common.function_name, 0);

	fci.size = sizeof(fci);
	fci.function_table = EG(function_table);
	fci.object_pp = NULL;
	fci.function_name = &z_fname;
	fci.retval_ptr_ptr = &retval;
	fci.param_count = ZEND_NUM_ARGS() + (arg2 ? 2 : 1);
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

#define FileFunctionCall(func_name, arg2) \
{ \
	zend_function *func_ptr; \
	zend_hash_find(EG(function_table), #func_name, sizeof(#func_name), (void **) &func_ptr); \
	spl_file_object_call(INTERNAL_FUNCTION_PARAM_PASSTHRU, intern, func_ptr, arg2); \
}

/* {{{ FileFunction */
#define FileFunction(func_name) \
SPL_METHOD(SplFileObject, func_name) \
{ \
	spl_file_object *intern = (spl_file_object*)zend_object_store_get_object(getThis() TSRMLS_CC); \
	FileFunctionCall(func_name, NULL); \
}
/* }}} */

/* {{{ proto array SplFileObject::fgetcsv([string delimiter [, string enclosure]])
   Return current line as csv */
SPL_METHOD(SplFileObject, fgetcsv)
{
	spl_file_object *intern = (spl_file_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	zval *arg2 = NULL;
	MAKE_STD_ZVAL(arg2);
	ZVAL_LONG(arg2, intern->max_line_len);

	spl_file_object_free_line(intern TSRMLS_CC);
	intern->current_line_num++;

	FileFunctionCall(fgetcsv, arg2);

	zval_ptr_dtor(&arg2);
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
	spl_file_object *intern = (spl_file_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	RETURN_BOOL(!php_stream_flush(intern->stream));
} /* }}} */


/* {{{ proto int SplFileObject::ftell()
   Return current file position */
SPL_METHOD(SplFileObject, ftell)
{
	spl_file_object *intern = (spl_file_object*)zend_object_store_get_object(getThis() TSRMLS_CC);	
	long ret = php_stream_tell(intern->stream);

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
	spl_file_object *intern = (spl_file_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	long pos, whence = SEEK_SET;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|l", &pos, &whence) == FAILURE) {
		return;
	}

	spl_file_object_free_line(intern TSRMLS_CC);
	RETURN_LONG(php_stream_seek(intern->stream, pos, whence));
} /* }}} */

/* {{{ proto int SplFileObject::fgetc()
   Get a character form the file */
SPL_METHOD(SplFileObject, fgetc)
{
	spl_file_object *intern = (spl_file_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	char buf[2];
	int result;

	spl_file_object_free_line(intern TSRMLS_CC);

	result = php_stream_getc(intern->stream);

	if (result == EOF) {
		RETVAL_FALSE;
	} else {
		if (result == '\n') {
			intern->current_line_num++;
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
	spl_file_object *intern = (spl_file_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	zval *arg2 = NULL;
	MAKE_STD_ZVAL(arg2);
	ZVAL_LONG(arg2, intern->max_line_len);

	spl_file_object_free_line(intern TSRMLS_CC);
	intern->current_line_num++;

	FileFunctionCall(fgetss, arg2);

	zval_ptr_dtor(&arg2);
} /* }}} */

/* {{{ proto int SplFileObject::fpassthru()
   Output all remaining data from a file pointer */
SPL_METHOD(SplFileObject, fpassthru)
{
	spl_file_object *intern = (spl_file_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	RETURN_LONG(php_stream_passthru(intern->stream));
} /* }}} */

/* {{{ proto bool SplFileObject::fscanf(string format [, string ...])
   Implements a mostly ANSI compatible fscanf() */
SPL_METHOD(SplFileObject, fscanf)
{
	spl_file_object *intern = (spl_file_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	spl_file_object_free_line(intern TSRMLS_CC);
	intern->current_line_num++;

	FileFunctionCall(fscanf, NULL);
}
/* }}} */

/* {{{ proto mixed SplFileObject::fwrite(string str [, int length])
   Binary-safe file write */
SPL_METHOD(SplFileObject, fwrite)
{
	spl_file_object *intern = (spl_file_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	char *str;
	int str_len;
	int ret;
	long length;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &str, &str_len, &length) == FAILURE) {
		return;
	}

	if (ZEND_NUM_ARGS() < 2) {
		str_len = MAX(0, MIN(length, str_len));
	}
	if (!str_len) {
		RETURN_LONG(0);
	}

	if (PG(magic_quotes_runtime)) {
		str = estrndup(str, str_len);
		php_stripslashes(str, &str_len TSRMLS_CC);
		ret = php_stream_write(intern->stream, str, str_len);
		efree(str);
		RETURN_LONG(ret);
	}

	RETURN_LONG(php_stream_write(intern->stream, str, str_len));
} /* }}} */

/* {{{ proto bool SplFileObject::fstat()
   Stat() on a filehandle */
FileFunction(fstat)
/* }}} */

/* {{{ proto bool SplFileObject::ftruncate(int size)
   Truncate file to 'size' length */
SPL_METHOD(SplFileObject, ftruncate)
{
	spl_file_object *intern = (spl_file_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	long size;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &size) == FAILURE) {
		return;
	}

	if (!php_stream_truncate_supported(intern->stream)) {
		zend_throw_exception_ex(spl_ce_LogicException, 0 TSRMLS_CC, "Can't truncate file %s", intern->file_name);
		RETURN_FALSE;
	}
	
	RETURN_BOOL(0 == php_stream_truncate_set_size(intern->stream, size));
} /* }}} */

/* {{{ proto void SplFileObject::seek(int line_pos)
   Seek to specified line */
SPL_METHOD(SplFileObject, seek)
{
	spl_file_object *intern = (spl_file_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	long line_pos;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &line_pos) == FAILURE) {
		return;
	}
	if (line_pos < 0) {
		zend_throw_exception_ex(spl_ce_LogicException, 0 TSRMLS_CC, "Can't seek file %s to negative line %ld", intern->file_name, line_pos);
		RETURN_FALSE;		
	}
	
	spl_file_object_rewind(intern TSRMLS_CC);
	
	while(intern->current_line_num < line_pos) {
		spl_file_object_read_line(getThis(), intern, 1 TSRMLS_CC);
	}
}
/* }}} */

/* {{{ Function/Class/Method definitions */
static
ZEND_BEGIN_ARG_INFO_EX(arginfo_file_object___construct, 0, 0, 1)
	ZEND_ARG_INFO(0, file_name)
	ZEND_ARG_INFO(0, open_mode)
	ZEND_ARG_INFO(0, use_include_path)
	ZEND_ARG_INFO(0, context)
ZEND_END_ARG_INFO();

static
ZEND_BEGIN_ARG_INFO(arginfo_file_object_setFlags, 0) 
	ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO();

static
ZEND_BEGIN_ARG_INFO(arginfo_file_object_setMaxLineLen, 0) 
	ZEND_ARG_INFO(0, max_len)
ZEND_END_ARG_INFO();

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_file_object_fgetcsv, 0, 0, 0) 
	ZEND_ARG_INFO(0, delimiter)
	ZEND_ARG_INFO(0, enclosure)
ZEND_END_ARG_INFO();

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_file_object_flock, 0, 0, 1) 
	ZEND_ARG_INFO(0, operation)
	ZEND_ARG_INFO(1, wouldblock])
ZEND_END_ARG_INFO();

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_file_object_fseek, 0, 0, 1) 
	ZEND_ARG_INFO(0, pos)
	ZEND_ARG_INFO(0, whence)
ZEND_END_ARG_INFO();

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_file_object_fgetss, 0, 0, 0) 
	ZEND_ARG_INFO(0, allowable_tags)
ZEND_END_ARG_INFO();

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_file_object_fscanf, 0, 0, 1) 
	ZEND_ARG_INFO(0, format)
ZEND_END_ARG_INFO();

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_file_object_fwrite, 0, 0, 1) 
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO();

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_file_object_ftruncate, 0, 0, 1) 
	ZEND_ARG_INFO(0, size)
ZEND_END_ARG_INFO();

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_file_object_seek, 0, 0, 1) 
	ZEND_ARG_INFO(0, line_pos)
ZEND_END_ARG_INFO();

static zend_function_entry spl_file_object_class_functions[] = {
	SPL_ME(SplFileObject, __construct,    arginfo_file_object___construct,   ZEND_ACC_PUBLIC)
	SPL_ME(SplFileObject, getFilename,    NULL, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileObject, rewind,         NULL, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileObject, eof,            NULL, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileObject, valid,          NULL, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileObject, fgets,          NULL, ZEND_ACC_PUBLIC)
	SPL_ME(SplFileObject, fgetcsv,        arginfo_file_object_fgetcsv,       ZEND_ACC_PUBLIC)
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
	// mappings
	SPL_MA(SplFileObject, getCurrentLine, SplFileObject, fgets,      NULL, ZEND_ACC_PUBLIC)
	SPL_MA(SplFileObject, __toString,     SplFileObject, current,    NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */

/* {{{ PHP_MINIT_FUNCTION(spl_directory)
 */
PHP_MINIT_FUNCTION(spl_directory)
{
	REGISTER_SPL_STD_CLASS_EX(DirectoryIterator, spl_ce_dir_object_new, spl_ce_dir_class_functions);
	zend_class_implements(spl_ce_DirectoryIterator TSRMLS_CC, 1, zend_ce_iterator);
	memcpy(&spl_ce_dir_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	spl_ce_dir_handlers.clone_obj = spl_ce_dir_object_clone;
	spl_ce_dir_handlers.cast_object = spl_ce_dir_cast;

	spl_ce_DirectoryIterator->get_iterator = spl_ce_dir_get_iterator;

	REGISTER_SPL_SUB_CLASS_EX(RecursiveDirectoryIterator, DirectoryIterator, spl_ce_dir_object_new, spl_ce_dir_tree_class_functions);
	REGISTER_SPL_IMPLEMENTS(RecursiveDirectoryIterator, RecursiveIterator);

	spl_ce_RecursiveDirectoryIterator->get_iterator = spl_ce_dir_tree_get_iterator;

	REGISTER_SPL_STD_CLASS_EX(SplFileObject, spl_file_object_new, spl_file_object_class_functions);
	REGISTER_SPL_IMPLEMENTS(SplFileObject, RecursiveIterator);
	REGISTER_SPL_IMPLEMENTS(SplFileObject, SeekableIterator);

	REGISTER_SPL_CLASS_CONST_LONG(SplFileObject, "DROP_NEW_LINE", SPL_FILE_OBJECT_DROP_NEW_LINE);

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
