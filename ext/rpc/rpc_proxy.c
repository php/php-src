/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Harald Radi <h.radi@nme.at>                                  |
   +----------------------------------------------------------------------+
 */

#include "php.h"

#include "rpc_proxy.h"
#include "handler.h"

/* object handler */
static void rpc_proxy_add_ref(zval * TSRMLS_DC);
static void rpc_proxy_del_ref(zval * TSRMLS_DC);
static void rpc_proxy_delete(zval * TSRMLS_DC);
static zend_object_value rpc_proxy_clone(zval * TSRMLS_DC);
static zval* rpc_proxy_read(zval *, zval *, int  TSRMLS_DC);
static void rpc_proxy_write(zval *, zval *, zval * TSRMLS_DC);
static zval** rpc_proxy_get_property(zval *, zval * TSRMLS_DC);
static zval* rpc_proxy_get(zval * TSRMLS_DC);
static void rpc_proxy_set(zval **, zval * TSRMLS_DC);
static int rpc_proxy_has_property(zval *, zval *, int  TSRMLS_DC);
static void rpc_proxy_unset_property(zval *, zval * TSRMLS_DC);
static HashTable* rpc_proxy_get_properties(zval * TSRMLS_DC);
static union _zend_function* rpc_proxy_get_method(zval *, char *, int TSRMLS_DC);
static union _zend_function* rpc_proxy_get_constructor(zval * TSRMLS_DC);
static zend_class_entry* rpc_proxy_get_class_entry(zval *object TSRMLS_DC);
static int rpc_proxy_get_classname(zval *, char **, zend_uint *, int  TSRMLS_DC);
static int rpc_proxy_compare(zval *, zval * TSRMLS_DC);
/**/

zend_object_handlers rpc_proxy_handlers = {
	rpc_proxy_add_ref,
	rpc_proxy_del_ref,
	rpc_proxy_delete,
	rpc_proxy_clone,
	rpc_proxy_read,
	rpc_proxy_write,
	rpc_proxy_get_property,
	NULL,
	rpc_proxy_get,
	rpc_proxy_set,
	rpc_proxy_has_property,
	rpc_proxy_unset_property,
	rpc_proxy_get_properties,
	rpc_proxy_get_method,
	NULL,
	rpc_proxy_get_constructor,
	rpc_proxy_get_class_entry,
	rpc_proxy_get_classname,
	rpc_proxy_compare
};


/* object handler */

static void rpc_proxy_add_ref(zval *object TSRMLS_DC)
{
}

static void rpc_proxy_del_ref(zval *object TSRMLS_DC)
{
}

static void rpc_proxy_delete(zval *object TSRMLS_DC)
{
}

static zend_object_value rpc_proxy_clone(zval *object TSRMLS_DC)
{
}

static zval* rpc_proxy_read(zval *object, zval *member, int type TSRMLS_DC)
{
	return NULL;
}

static void rpc_proxy_write(zval *object, zval *member, zval *value TSRMLS_DC)
{
}

static zval** rpc_proxy_get_property(zval *object, zval *member TSRMLS_DC)
{
	return NULL;
}

static zval* rpc_proxy_get(zval *property TSRMLS_DC)
{
	return NULL;
}

static void rpc_proxy_set(zval **property, zval *value TSRMLS_DC)
{
}

static int rpc_proxy_has_property(zval *object, zval *member, int check_empty TSRMLS_DC)
{
	return FAILURE;
}

static void rpc_proxy_unset_property(zval *object, zval *member TSRMLS_DC)
{
}

static HashTable* rpc_proxy_get_properties(zval *object TSRMLS_DC)
{
	return NULL;
}

static union _zend_function* rpc_proxy_get_method(zval *object, char *method, int method_len TSRMLS_DC)
{
	return NULL;
}

static union _zend_function* rpc_proxy_get_constructor(zval *object TSRMLS_DC)
{
	return NULL;
}

static zend_class_entry* rpc_proxy_get_class_entry(zval *object TSRMLS_DC)
{
	return NULL;
}

static int rpc_proxy_get_classname(zval *object, char **class_name, zend_uint *class_name_len, int parent TSRMLS_DC)
{
	return FAILURE;
}

static int rpc_proxy_compare(zval *object1, zval *object2 TSRMLS_DC)
{
	return FAILURE;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
