/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
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
#include "zend_compile.h"
#include "zend_default_classes.h"
#include "zend_interfaces.h"

#include "php_spl.h"
#include "spl_functions.h"
#include "spl_engine.h"
#include "spl_iterators.h"

#include "php.h"
#include "fopen_wrappers.h"

#include "ext/standard/basic_functions.h"
#include "ext/standard/php_filestat.h"

/* forward declaration for all methods use (class-name, method-name) */
SPL_METHOD(DirectoryIterator, __construct);
SPL_METHOD(DirectoryIterator, rewind);
SPL_METHOD(DirectoryIterator, hasMore);
SPL_METHOD(DirectoryIterator, key);
SPL_METHOD(DirectoryIterator, current);
SPL_METHOD(DirectoryIterator, next);
SPL_METHOD(DirectoryIterator, getPath);
SPL_METHOD(DirectoryIterator, getFilename);
SPL_METHOD(DirectoryIterator, getPathname);
SPL_METHOD(DirectoryIterator, isDot);
SPL_METHOD(DirectoryIterator, isDir);

SPL_METHOD(DirectoryTreeIterator, key);
SPL_METHOD(DirectoryTreeIterator, hasChildren);
SPL_METHOD(DirectoryTreeIterator, getChildren);


/* declare method parameters */
/* supply a name and default to call by parameter */
static
ZEND_BEGIN_ARG_INFO(arginfo_dir___construct, 0) 
	ZEND_ARG_INFO(0, path)  /* parameter name */
ZEND_END_ARG_INFO();


/* the method table */
/* each method can have its own parameters and visibility */
static zend_function_entry spl_ce_dir_class_functions[] = {
	SPL_ME(DirectoryIterator, __construct,   arginfo_dir___construct, ZEND_ACC_PUBLIC)
	SPL_ME(DirectoryIterator, rewind,        NULL, ZEND_ACC_PUBLIC)
	SPL_ME(DirectoryIterator, hasMore,       NULL, ZEND_ACC_PUBLIC)
	SPL_ME(DirectoryIterator, key,           NULL, ZEND_ACC_PUBLIC)
	SPL_ME(DirectoryIterator, current,       NULL, ZEND_ACC_PUBLIC)
	SPL_ME(DirectoryIterator, next,          NULL, ZEND_ACC_PUBLIC)
	SPL_ME(DirectoryIterator, getPath,       NULL, ZEND_ACC_PUBLIC)
	SPL_ME(DirectoryIterator, getFilename,   NULL, ZEND_ACC_PUBLIC)
	SPL_ME(DirectoryIterator, getPathname,   NULL, ZEND_ACC_PUBLIC)
	SPL_ME(DirectoryIterator, isDot,         NULL, ZEND_ACC_PUBLIC)
	SPL_ME(DirectoryIterator, isDir,         NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

static zend_function_entry spl_ce_dir_tree_class_functions[] = {
	SPL_ME(DirectoryTreeIterator, key,           NULL, ZEND_ACC_PUBLIC)
	SPL_ME(DirectoryTreeIterator, hasChildren,   NULL, ZEND_ACC_PUBLIC)
	SPL_ME(DirectoryTreeIterator, getChildren,   NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};


/* declare the class handlers */
static zend_object_handlers spl_ce_dir_handlers;


/* decalre the class entry */
zend_class_entry *spl_ce_DirectoryIterator;
zend_class_entry *spl_ce_DirectoryTreeIterator;


/* the overloaded class structure */

/* overloading the structure results in the need of having 
   dedicated creatin/cloning/destruction functions */
typedef struct _spl_ce_dir_object {
	zend_object       std;
	php_stream        *dirp;
	php_stream_dirent entry;
	char              *path;
	int               index;
} spl_ce_dir_object;


/* {{{ spl_ce_dir_object_dtor */
/* close all resources and the memory allocated for the object */
static void spl_ce_dir_object_dtor(void *object, zend_object_handle handle TSRMLS_DC)
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
	*obj = intern;

	ALLOC_HASHTABLE(intern->std.properties);
	zend_hash_init(intern->std.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
	zend_hash_copy(intern->std.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));

	retval.handle = zend_objects_store_put(intern, spl_ce_dir_object_dtor, NULL TSRMLS_CC);
	retval.handlers = &spl_ce_dir_handlers;
	return retval;
}
/* }}} */


/* {{{ spl_ce_dir_object_new */
/* See spl_ce_dir_object_new_ex */
static zend_object_value spl_ce_dir_object_new(zend_class_entry *class_type TSRMLS_DC)
{
	spl_ce_dir_object *tmp;
	return spl_ce_dir_object_new_ex(class_type, &tmp TSRMLS_CC);
}
/* }}} */


/* {{{ spl_ce_dir_open */
/* open a directory resource */
static void spl_ce_dir_open(spl_ce_dir_object* intern, char *path TSRMLS_DC)
{
	intern->dirp = php_stream_opendir(path, ENFORCE_SAFE_MODE|REPORT_ERRORS, NULL);

	intern->path = estrdup(path);
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
	long len;

	php_set_error_handling(EH_THROW, zend_exception_get_default() TSRMLS_CC);

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

/* {{{ proto string|false DirectoryIterator::current()
   Return this (needed for Iterator interface) */
SPL_METHOD(DirectoryIterator, current)
{
	zval *object = getThis();
	spl_ce_dir_object *intern = (spl_ce_dir_object*)zend_object_store_get_object(object TSRMLS_CC);

	if (intern->entry.d_name[0]) {
		RETURN_STRING(intern->entry.d_name, 1);
	} else {
		RETURN_FALSE;
	}
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
}
/* }}} */

/* {{{ proto string DirectoryIterator::hasMore()
   Check whether dir contains more entries */
SPL_METHOD(DirectoryIterator, hasMore)
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

/* {{{ proto string DirectoryIterator::getPathname()
   Return path and filename of current dir entry */
SPL_METHOD(DirectoryIterator, getPathname)
{
	zval *object = getThis();
	spl_ce_dir_object *intern = (spl_ce_dir_object*)zend_object_store_get_object(object TSRMLS_CC);

	char *filename;
	int filename_len = spprintf(&filename, 0, "%s/%s", intern->path, intern->entry.d_name);
	RETURN_STRINGL(filename, filename_len, 0);
}
/* }}} */

/* {{{ proto string DirectoryTreeIterator::key()
   Return path and filename of current dir entry */
SPL_METHOD(DirectoryTreeIterator, key)
{
	zval *object = getThis();
	spl_ce_dir_object *intern = (spl_ce_dir_object*)zend_object_store_get_object(object TSRMLS_CC);

	char *filename;
	int filename_len = spprintf(&filename, 0, "%s/%s", intern->path, intern->entry.d_name);
	RETURN_STRINGL(filename, filename_len, 0);
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

/* {{{ proto bool DirectoryIterator::isDir()
   Returns whether current entry is a directory  */
SPL_METHOD(DirectoryIterator, isDir)
{
	zval *object = getThis();
	spl_ce_dir_object *intern = (spl_ce_dir_object*)zend_object_store_get_object(object TSRMLS_CC);
	char *filename;
	
	php_stat_len filename_len = spprintf(&filename, 0, "%s/%s", intern->path, intern->entry.d_name);

	php_stat(filename, filename_len, /*funcnum*/FS_IS_DIR, return_value TSRMLS_CC);

	efree(filename);
}
/* }}} */

/* {{{ proto bool DirectoryIterator::hasChildren()
   Returns whether current entry is a directory and not '.' or '..' */
SPL_METHOD(DirectoryTreeIterator, hasChildren)
{
	zval *object = getThis();
	spl_ce_dir_object *intern = (spl_ce_dir_object*)zend_object_store_get_object(object TSRMLS_CC);
	
	if (!strcmp(intern->entry.d_name, ".") || !strcmp(intern->entry.d_name, "..")) {
		RETURN_BOOL(0);
	} else {
		char *filename;
		php_stat_len filename_len = spprintf(&filename, 0, "%s/%s", intern->path, intern->entry.d_name);

		php_stat(filename, filename_len, FS_IS_DIR, return_value TSRMLS_CC);
    	efree(filename);
    }
}
/* }}} */

/* {{{ proto DirectoryIterator DirectoryIterator::getChildren()
   Returns an iterator fo rthe current entry if it is a directory */
SPL_METHOD(DirectoryTreeIterator, getChildren)
{
	zval *object = getThis(), zpath;
	spl_ce_dir_object *intern = (spl_ce_dir_object*)zend_object_store_get_object(object TSRMLS_CC);
	
	char *path;
	int path_len = spprintf(&path, 0, "%s/%s", intern->path, intern->entry.d_name);
	
	ZVAL_STRINGL(&zpath, path, path_len, 0);

	spl_instantiate_arg_ex1(spl_ce_DirectoryTreeIterator, &return_value, 0, &zpath TSRMLS_CC);
	zval_dtor(&zpath);
}
/* }}} */

/* define an overloaded iterator structure */
typedef struct {
	zend_object_iterator  intern;
	zval                  *current;
	spl_ce_dir_object       *object;
} spl_ce_dir_it;

/* forward declarations to the iterator handlers */
static void spl_ce_dir_it_dtor(zend_object_iterator *iter TSRMLS_DC);
static int spl_ce_dir_it_has_more(zend_object_iterator *iter TSRMLS_DC);
static void spl_ce_dir_it_current_data(zend_object_iterator *iter, zval ***data TSRMLS_DC);
static int spl_ce_dir_it_current_key(zend_object_iterator *iter, char **str_key, uint *str_key_len, ulong *int_key TSRMLS_DC);
static void spl_ce_dir_it_move_forward(zend_object_iterator *iter TSRMLS_DC);
static void spl_ce_dir_it_rewind(zend_object_iterator *iter TSRMLS_DC);


/* iterator handler table */
zend_object_iterator_funcs spl_ce_dir_it_funcs = {
	spl_ce_dir_it_dtor,
	spl_ce_dir_it_has_more,
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
	MAKE_STD_ZVAL(iterator->current);
	iterator->object = dir_object;
	
	return (zend_object_iterator*)iterator;
}
/* }}} */


/* {{{ spl_ce_dir_it_dtor */
static void spl_ce_dir_it_dtor(zend_object_iterator *iter TSRMLS_DC)
{
	spl_ce_dir_it *iterator = (spl_ce_dir_it *)iter;
	zval        *intern = (zval*)iterator->intern.data;

	zval_ptr_dtor(&iterator->current);
	zval_ptr_dtor(&intern);

	efree(iterator);
}
/* }}} */
	

/* {{{ spl_ce_dir_it_has_more */
static int spl_ce_dir_it_has_more(zend_object_iterator *iter TSRMLS_DC)
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
	zval_dtor(iterator->current);
	if (!object->dirp || !php_stream_readdir(object->dirp, &object->entry)) {
		object->entry.d_name[0] = '\0';
		ZVAL_NULL(iterator->current);
	} else {
		ZVAL_STRING(iterator->current, object->entry.d_name, 1);
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
	zval_dtor(iterator->current);
	if (!object->dirp || !php_stream_readdir(object->dirp, &object->entry)) {
		object->entry.d_name[0] = '\0';
		ZVAL_NULL(iterator->current);
	} else {
		ZVAL_STRING(iterator->current, object->entry.d_name, 1);
	}
}
/* }}} */


/* {{{ spl_ce_dir_tree_it_current_key */
static int spl_ce_dir_tree_it_current_key(zend_object_iterator *iter, char **str_key, uint *str_key_len, ulong *int_key TSRMLS_DC)
{
	spl_ce_dir_it       *iterator = (spl_ce_dir_it *)iter;
	spl_ce_dir_object   *object   = iterator->object;
	
	*str_key_len = spprintf(str_key, 0, "%s/%s", object->path, object->entry.d_name) + 1;
	return HASH_KEY_IS_STRING;
}
/* }}} */


/* {{{ spl_ce_dir_tree_it_move_forward */
static void spl_ce_dir_tree_it_move_forward(zend_object_iterator *iter TSRMLS_DC)
{
	spl_ce_dir_it       *iterator = (spl_ce_dir_it *)iter;
	spl_ce_dir_object   *object   = iterator->object;
	
	object->index++;
	zval_dtor(iterator->current);
skip_dots:
	if (!object->dirp || !php_stream_readdir(object->dirp, &object->entry)) {
		object->entry.d_name[0] = '\0';
		ZVAL_NULL(iterator->current);
	} else {
		if (!strcmp(object->entry.d_name, ".") || !strcmp(object->entry.d_name, "..")) {
			goto skip_dots;
		}
		ZVAL_STRING(iterator->current, object->entry.d_name, 1);
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
	zval_dtor(iterator->current);
skip_dots:
	if (!object->dirp || !php_stream_readdir(object->dirp, &object->entry)) {
		object->entry.d_name[0] = '\0';
		ZVAL_NULL(iterator->current);
	} else {
		if (!strcmp(object->entry.d_name, ".") || !strcmp(object->entry.d_name, "..")) {
			goto skip_dots;
		}
		ZVAL_STRING(iterator->current, object->entry.d_name, 1);
	}
}
/* }}} */


/* iterator handler table */
zend_object_iterator_funcs spl_ce_dir_tree_it_funcs = {
	spl_ce_dir_it_dtor,
	spl_ce_dir_it_has_more,
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
	MAKE_STD_ZVAL(iterator->current);
	iterator->object = dir_object;
	
	return (zend_object_iterator*)iterator;
}
/* }}} */


/* {{{ PHP_MINIT_FUNCTION(spl_directory)
 */
PHP_MINIT_FUNCTION(spl_directory)
{
	REGISTER_SPL_STD_CLASS_EX(DirectoryIterator, spl_ce_dir_object_new, spl_ce_dir_class_functions);
	zend_class_implements(spl_ce_DirectoryIterator TSRMLS_CC, 1, zend_ce_iterator);
	memcpy(&spl_ce_dir_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	spl_ce_dir_handlers.clone_obj = spl_ce_dir_object_clone;

	spl_ce_DirectoryIterator->get_iterator = spl_ce_dir_get_iterator;

	REGISTER_SPL_SUB_CLASS_EX(DirectoryTreeIterator, DirectoryIterator, spl_ce_dir_object_new, spl_ce_dir_tree_class_functions);
	REGISTER_SPL_IMPLEMENTS(DirectoryTreeIterator, RecursiveIterator);

	spl_ce_DirectoryTreeIterator->get_iterator = spl_ce_dir_tree_get_iterator;

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
