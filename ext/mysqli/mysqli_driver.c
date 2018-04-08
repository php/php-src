/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2018 The PHP Group                                |
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
#include "php_mysqli_structs.h"
#include "zend_exceptions.h"
#include "mysqli_fe.h"

#define MAP_PROPERTY_MYG_BOOL_READ(name, value) \
static zval *name(mysqli_object *obj, zval *retval) \
{ \
	ZVAL_BOOL(retval, MyG(value)); \
	return retval; \
} \

#define MAP_PROPERTY_MYG_BOOL_WRITE(name, value) \
static int name(mysqli_object *obj, zval *value) \
{ \
	MyG(value) = Z_LVAL_P(value) > 0; \
	return SUCCESS; \
} \

#define MAP_PROPERTY_MYG_LONG_READ(name, value) \
static zval *name(mysqli_object *obj, zval *retval) \
{ \
	ZVAL_LONG(retval, MyG(value)); \
	return retval; \
} \

#define MAP_PROPERTY_MYG_LONG_WRITE(name, value) \
static int name(mysqli_object *obj, zval *value) \
{ \
	MyG(value) = Z_LVAL_P(value); \
	return SUCCESS; \
} \

#define MAP_PROPERTY_MYG_STRING_READ(name, value) \
static zval *name(mysqli_object *obj, zval *retval) \
{ \
	ZVAL_STRING(retval, MyG(value)); \
	return retval; \
} \

#define MAP_PROPERTY_MYG_STRING_WRITE(name, value) \
static int name(mysqli_object *obj, zval *value) \
{ \
	MyG(value) = Z_STRVAL_P(value); \
	return SUCCESS; \
} \

/* {{{ property driver_report_write */
static int driver_report_write(mysqli_object *obj, zval *value)
{
	MyG(report_mode) = Z_LVAL_P(value);
	/*FIXME*/
	/* zend_replace_error_handling(MyG(report_mode) & MYSQLI_REPORT_STRICT ? EH_THROW : EH_NORMAL, NULL, NULL); */
	return SUCCESS;
}
/* }}} */

/* {{{ property driver_embedded_read */
static zval *driver_embedded_read(mysqli_object *obj, zval *retval)
{
#ifdef HAVE_EMBEDDED_MYSQLI
	ZVAL_TRUE(retval);
#else
	ZVAL_FALSE(retval);
#endif
	return retval;
}
/* }}} */

/* {{{ property driver_client_version_read */
static zval *driver_client_version_read(mysqli_object *obj, zval *retval)
{
	ZVAL_LONG(retval, MYSQL_VERSION_ID);
	return retval;
}
/* }}} */

/* {{{ property driver_client_info_read */
static zval *driver_client_info_read(mysqli_object *obj, zval *retval)
{
	ZVAL_STRING(retval, (char *)mysql_get_client_info());
	return retval;
}
/* }}} */

/* {{{ property driver_driver_version_read */
static zval *driver_driver_version_read(mysqli_object *obj, zval *retval)
{
	ZVAL_LONG(retval, MYSQLI_VERSION_ID);
	return retval;
}
/* }}} */

MAP_PROPERTY_MYG_BOOL_READ(driver_reconnect_read, reconnect)
MAP_PROPERTY_MYG_BOOL_WRITE(driver_reconnect_write, reconnect)
MAP_PROPERTY_MYG_LONG_READ(driver_report_read, report_mode)

ZEND_FUNCTION(mysqli_driver_construct)
{
#if G0
	MYSQLI_RESOURCE 	*mysqli_resource;

	mysqli_resource = (MYSQLI_RESOURCE *)ecalloc (1, sizeof(MYSQLI_RESOURCE));
	mysqli_resource->ptr = 1;
	mysqli_resource->status = (ZEND_NUM_ARGS() == 1) ? MYSQLI_STATUS_INITIALIZED : MYSQLI_STATUS_VALID;
	(Z_MYSQLI_P(getThis()))->ptr = mysqli_resource;
#endif
}

const mysqli_property_entry mysqli_driver_property_entries[] = {
	{"client_info", sizeof("client_info") - 1, driver_client_info_read, NULL},
	{"client_version", sizeof("client_version") - 1, driver_client_version_read, NULL},
	{"driver_version", sizeof("driver_version") - 1, driver_driver_version_read, NULL},
	{"embedded", sizeof("embedded") - 1, driver_embedded_read, NULL},
	{"reconnect", sizeof("reconnect") - 1, driver_reconnect_read, driver_reconnect_write},
	{"report_mode", sizeof("report_mode") - 1, driver_report_read, driver_report_write},
	{NULL, 0, NULL, NULL}
};

/* {{{ mysqli_driver_methods[]
 */
const zend_function_entry mysqli_driver_methods[] = {
#if defined(HAVE_EMBEDDED_MYSQLI)
	PHP_FALIAS(embedded_server_start, mysqli_embedded_server_start, NULL)
	PHP_FALIAS(embedded_server_end, mysqli_embedded_server_end, NULL)
#endif
	PHP_FE_END
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
