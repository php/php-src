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
  | Author: Georg Richter <georg@php.net>                                |
  +----------------------------------------------------------------------+

*/
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <signal.h>

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_mysqli.h"
#include "zend_exceptions.h"


#define MAP_PROPERTY_MYG_BOOL_READ(name, value) \
static int name(mysqli_object *obj, zval **retval TSRMLS_DC) \
{ \
	ALLOC_ZVAL(*retval); \
	ZVAL_BOOL(*retval, MyG(value)); \
	return SUCCESS; \
} \

#define MAP_PROPERTY_MYG_BOOL_WRITE(name, value) \
static int name(mysqli_object *obj, zval *value TSRMLS_DC) \
{ \
	MyG(value) = Z_LVAL_P(value) > 0; \
	return SUCCESS; \
} \

#define MAP_PROPERTY_MYG_LONG_READ(name, value) \
static int name(mysqli_object *obj, zval **retval TSRMLS_DC) \
{ \
	ALLOC_ZVAL(*retval); \
	ZVAL_LONG(*retval, MyG(value)); \
	return SUCCESS; \
} \

#define MAP_PROPERTY_MYG_LONG_WRITE(name, value) \
static int name(mysqli_object *obj, zval *value TSRMLS_DC) \
{ \
	MyG(value) = Z_LVAL_P(value); \
	return SUCCESS; \
} \

#define MAP_PROPERTY_MYG_STRING_READ(name, value) \
static int name(mysqli_object *obj, zval **retval TSRMLS_DC) \
{ \
	ALLOC_ZVAL(*retval); \
	ZVAL_STRING(*retval, MyG(value), 1); \
	return SUCCESS; \
} \

#define MAP_PROPERTY_MYG_STRING_WRITE(name, value) \
static int name(mysqli_object *obj, zval *value TSRMLS_DC) \
{ \
	MyG(value) = Z_STRVAL_P(value); \
	return SUCCESS; \
} \

/* {{{ property driver_report_write */
static int driver_report_write(mysqli_object *obj, zval *value TSRMLS_DC)
{
	MyG(report_mode) = Z_LVAL_P(value);
	php_set_error_handling(MyG(report_mode) & MYSQLI_REPORT_STRICT ? EH_THROW : EH_NORMAL, 
							zend_exception_get_default(TSRMLS_C) TSRMLS_CC);
	return SUCCESS;
}
/* }}} */

/* {{{ property driver_embedded_read */
static int driver_embedded_read(mysqli_object *obj, zval **retval TSRMLS_DC)
{
	ALLOC_ZVAL(*retval);
#ifdef HAVE_EMBEDDED_MYSQLI
	ZVAL_BOOL(*retval, 1);
#else
	ZVAL_BOOL(*retval, 0);
#endif
	return SUCCESS;
}
/* }}} */

/* {{{ property driver_client_version_read */
static int driver_client_version_read(mysqli_object *obj, zval **retval TSRMLS_DC)
{
	ALLOC_ZVAL(*retval);
	ZVAL_LONG(*retval, MYSQL_VERSION_ID);
	return SUCCESS;
}
/* }}} */

/* {{{ property driver_client_info_read */
static int driver_client_info_read(mysqli_object *obj, zval **retval TSRMLS_DC)
{
	ALLOC_ZVAL(*retval);
	ZVAL_STRING(*retval, MYSQL_SERVER_VERSION, 1);
	return SUCCESS;
}
/* }}} */

/* {{{ property driver_driver_version_read */
static int driver_driver_version_read(mysqli_object *obj, zval **retval TSRMLS_DC)
{
	ALLOC_ZVAL(*retval);
	ZVAL_LONG(*retval, MYSQLI_VERSION_ID);
	return SUCCESS;
}
/* }}} */

MAP_PROPERTY_MYG_BOOL_READ(driver_reconnect_read, reconnect);
MAP_PROPERTY_MYG_BOOL_WRITE(driver_reconnect_write, reconnect);
MAP_PROPERTY_MYG_LONG_READ(driver_report_read, report_mode);

ZEND_FUNCTION(mysqli_driver_construct)
{
}

mysqli_property_entry mysqli_driver_property_entries[] = {
	{"client_info", driver_client_info_read, NULL},
	{"client_version", driver_client_version_read, NULL},
	{"driver_version", driver_driver_version_read, NULL},
	{"embedded", driver_embedded_read, NULL},
	{"reconnect", driver_reconnect_read, driver_reconnect_write},
	{"report_mode", driver_report_read, driver_report_write},
	{NULL, NULL, NULL}
};

/* {{{ mysqli_driver_methods[]
 */
zend_function_entry mysqli_driver_methods[] = {
	PHP_FALIAS(embedded_server_start, mysqli_embedded_server_start, NULL)
	PHP_FALIAS(embedded_server_end, mysqli_embedded_server_end, NULL)
	{NULL, NULL, NULL}
};
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
