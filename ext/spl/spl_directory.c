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

#include "php_spl.h"
#include "spl_functions.h"
#include "spl_engine.h"
#include "spl_foreach.h"

#include "php.h"
#include "fopen_wrappers.h"

SPL_CLASS_FUNCTION(dir, __construct);
SPL_CLASS_FUNCTION(dir, rewind);
SPL_CLASS_FUNCTION(dir, current);
SPL_CLASS_FUNCTION(dir, next);
SPL_CLASS_FUNCTION(dir, hasmore);
SPL_CLASS_FUNCTION(dir, getpath);

static
ZEND_BEGIN_ARG_INFO(arginfo_dir___construct, 0)
	ZEND_ARG_INFO(0, path)
ZEND_END_ARG_INFO();

static zend_function_entry spl_dir_class_functions[] = {
	SPL_CLASS_FE(dir, __construct,   arginfo_dir___construct, ZEND_ACC_PUBLIC)
	SPL_CLASS_FE(dir, rewind,        NULL, ZEND_ACC_PUBLIC)
	SPL_CLASS_FE(dir, current,       NULL, ZEND_ACC_PUBLIC)
	SPL_CLASS_FE(dir, next,          NULL, ZEND_ACC_PUBLIC)
	SPL_CLASS_FE(dir, hasmore,       NULL, ZEND_ACC_PUBLIC)
	SPL_CLASS_FE(dir, getpath,       NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

static zend_object_handlers spl_dir_handlers;
static zend_class_entry *spl_ce_dir;

typedef struct _spl_dir_object {
	zend_object       std;
	php_stream        *dirp;
	php_stream_dirent entry;
	char              *path;
} spl_dir_object;

/* {{{ spl_dir_object_dtor */
static void spl_dir_object_dtor(void *object, zend_object_handle handle TSRMLS_DC)
{
	spl_dir_object *intern = (spl_dir_object *)object;

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

/* {{{ spl_dir_object_new */
static zend_object_value spl_dir_object_new_ex(zend_class_entry *class_type, spl_dir_object **obj TSRMLS_DC)
{
	zend_object_value retval;
	spl_dir_object *intern;
	zval *tmp;

	intern = emalloc(sizeof(spl_dir_object));
	memset(intern, 0, sizeof(spl_dir_object));
	intern->std.ce = class_type;
	*obj = intern;

	ALLOC_HASHTABLE(intern->std.properties);
	zend_hash_init(intern->std.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
	zend_hash_copy(intern->std.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));

	retval.handle = zend_objects_store_put(intern, spl_dir_object_dtor, NULL TSRMLS_CC);
	retval.handlers = &spl_dir_handlers;
	return retval;
}
/* }}} */

/* {{{ spl_dir_object_new */
static zend_object_value spl_dir_object_new(zend_class_entry *class_type TSRMLS_DC)
{
	spl_dir_object *tmp;
	return spl_dir_object_new_ex(class_type, &tmp TSRMLS_CC);
}
/* }}} */

/* {{{ spl_dir_open */
static void spl_dir_open(spl_dir_object* intern, char *path TSRMLS_DC)
{
	/* we are using EH_THORW so REPORT_ERRORS results in exceptions */
	intern->dirp = php_stream_opendir(path, ENFORCE_SAFE_MODE|REPORT_ERRORS, NULL);

	intern->path = estrdup(path);

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

/* {{{ spl_dir_object_clone */
static zend_object_value spl_dir_object_clone(zval *zobject TSRMLS_DC)
{
	zend_object_value new_obj_val;
	zend_object *old_object;
	zend_object *new_object;
	zend_object_handle handle = Z_OBJ_HANDLE_P(zobject);
	spl_dir_object *intern;

	old_object = zend_objects_get_address(zobject TSRMLS_CC);
	new_obj_val = spl_dir_object_new_ex(old_object->ce, &intern TSRMLS_CC);
	new_object = &intern->std;

	spl_dir_open(intern, ((spl_dir_object*)old_object)->path TSRMLS_CC);

	zend_objects_clone_members(new_object, new_obj_val, old_object, handle TSRMLS_CC);

	return new_obj_val;
}
/* }}} */

/* {{{ spl_dir_get_ce */
static zend_class_entry *spl_dir_get_ce(zval *object TSRMLS_DC)
{
	return spl_ce_dir;
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION(spl_directory) */
PHP_MINIT_FUNCTION(spl_directory)
{
	REGISTER_SPL_STD_CLASS_EX(dir, spl_dir_object_new, spl_dir_class_functions);
	REGISTER_SPL_IMPLEMENT(dir, sequence);
	memcpy(&spl_dir_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	spl_dir_handlers.clone_obj = spl_dir_object_clone;
	spl_dir_handlers.get_class_entry = spl_dir_get_ce;

	return SUCCESS;
}
/* }}} */

/* {{{ proto void __construct(string path)
 Cronstructs a new dir iterator from a path. */
SPL_CLASS_FUNCTION(dir, __construct)
{
	zval *object = getThis();
	spl_dir_object *intern;
	char *path;
	long len;

	php_set_error_handling(EH_THROW, zend_exception_get_default() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &path, &len) == FAILURE) {
		php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
		return;
	}

	intern = (spl_dir_object*)zend_object_store_get_object(object TSRMLS_CC);
	spl_dir_open(intern, path TSRMLS_CC);

	php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
}
/* }}} */

/* {{{ proto void rewind()
   Rewind dir back to the start */
SPL_CLASS_FUNCTION(dir, rewind)
{
	zval *object = getThis();
	spl_dir_object *intern = (spl_dir_object*)zend_object_store_get_object(object TSRMLS_CC);

	if (intern->dirp) {
		php_stream_rewinddir(intern->dirp);
	}
	if (!intern->dirp || !php_stream_readdir(intern->dirp, &intern->entry)) {
		intern->entry.d_name[0] = '\0';
	}
}
/* }}} */

/* {{{ proto string current()
   Return current dir entry */
SPL_CLASS_FUNCTION(dir, current)
{
	zval *object = getThis();
	spl_dir_object *intern = (spl_dir_object*)zend_object_store_get_object(object TSRMLS_CC);

	if (intern->dirp) {
		RETURN_STRING(intern->entry.d_name, 1);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto void next()
   Move to next entry */
SPL_CLASS_FUNCTION(dir, next)
{
	zval *object = getThis();
	spl_dir_object *intern = (spl_dir_object*)zend_object_store_get_object(object TSRMLS_CC);

	if (!intern->dirp || !php_stream_readdir(intern->dirp, &intern->entry)) {
		intern->entry.d_name[0] = '\0';
	}
}
/* }}} */

/* {{{ proto string hasMore()
   Check whether dir contains more entries */
SPL_CLASS_FUNCTION(dir, hasmore)
{
	zval *object = getThis();
	spl_dir_object *intern = (spl_dir_object*)zend_object_store_get_object(object TSRMLS_CC);

	RETURN_BOOL(intern->entry.d_name[0] != '\0');
}
/* }}} */

/* {{{ proto string getPath()
   Return directory path */
SPL_CLASS_FUNCTION(dir, getpath)
{
	zval *object = getThis();
	spl_dir_object *intern = (spl_dir_object*)zend_object_store_get_object(object TSRMLS_CC);

	RETURN_STRING(intern->path, 1);
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
