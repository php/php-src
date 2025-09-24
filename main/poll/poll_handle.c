/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Authors: Jakub Zelenka <bukka@php.net>                               |
   +----------------------------------------------------------------------+
*/

#include "php_poll.h"
#include "zend_exceptions.h"

/* Default get_fd implementation - calls PHP method */
static php_socket_t php_poll_handle_default_get_fd(php_poll_handle_object *handle)
{
	zval retval;
	zval obj;
	zval func_name;

	ZVAL_OBJ(&obj, &handle->std);

	/* Prepare function name as zval */
	ZVAL_STRING(&func_name, "getFileDescriptor");

	/* Call getFileDescriptor() method */
	if (EXPECTED(call_user_function(NULL, &obj, &func_name, &retval, 0, NULL) == SUCCESS)) {
		if (Z_TYPE(retval) == IS_LONG) {
			php_socket_t fd = Z_LVAL(retval) < 0 ? SOCK_ERR : (php_socket_t) Z_LVAL(retval);
			zval_ptr_dtor(&retval);
			zval_ptr_dtor(&func_name); /* Clean up function name */
			return fd;
		}
		zval_ptr_dtor(&retval);
	}

	zval_ptr_dtor(&func_name); /* Clean up function name */
	return SOCK_ERR; /* Invalid socket */
}

/* Default is_valid implementation - assume valid if we can get FD */
static int php_poll_handle_default_is_valid(php_poll_handle_object *handle)
{
	return php_poll_handle_get_fd(handle) != SOCK_ERR;
}

/* Default cleanup */
static void php_poll_handle_default_cleanup(php_poll_handle_object *handle)
{
	/* Base implementation has no cleanup */
}

/* Default operations that call PHP userspace methods */
php_poll_handle_ops php_poll_handle_default_ops = { .get_fd = php_poll_handle_default_get_fd,
	.is_valid = php_poll_handle_default_is_valid,
	.cleanup = php_poll_handle_default_cleanup };

/* Allocate a new poll handle object */
PHPAPI php_poll_handle_object *php_poll_handle_object_create(
		size_t obj_size, zend_class_entry *ce, php_poll_handle_ops *ops)
{
	php_poll_handle_object *intern = zend_object_alloc(obj_size, ce);

	zend_object_std_init(&intern->std, ce);
	object_properties_init(&intern->std, ce);

	intern->ops = ops ? ops : &php_poll_handle_default_ops;
	intern->handle_data = NULL;

	return intern;
}

/* Free poll handle object */
PHPAPI void php_poll_handle_object_free(zend_object *obj)
{
	php_poll_handle_object *intern = PHP_POLL_HANDLE_OBJ_FROM_ZOBJ(obj);

	if (intern->ops && intern->ops->cleanup) {
		intern->ops->cleanup(intern);
	}

	zend_object_std_dtor(&intern->std);
}

/* Get file descriptor from handle using ops */
PHPAPI php_socket_t php_poll_handle_get_fd(php_poll_handle_object *handle)
{
	if (!handle || !handle->ops || !handle->ops->get_fd) {
		return SOCK_ERR;
	}

	return handle->ops->get_fd(handle);
}
