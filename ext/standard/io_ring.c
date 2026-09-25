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
*/

/* Io\Ring\Engine: the Ring as an Io\OperationQueue, over the C ring queue
 * of main/io. The queue methods are the shared implementation of
 * ext/standard/io_hooks.c; this file adds what only a ring has. */

#include "php.h"
#include "zend_enum.h"
#include "zend_exceptions.h"
#include "ext/standard/file.h"
#include "ext/standard/io_poll.h"
#include "ext/standard/io_hooks.h"
#include "php_io.h"

#include <errno.h>

#ifdef HAVE_IOR

#include "io_ring_arginfo.h"
#include "io_ring_decl.h"

static zend_class_entry *php_io_ring_backend_ce;
static zend_class_entry *php_io_ring_engine_ce;
static zend_class_entry *php_io_ring_exception_ce;
static zend_class_entry *php_io_ring_failed_operation_exception_ce;

static php_io_ring *php_io_ring_engine_ring(php_io_opqueue_obj *intern)
{
	return php_io_queue_ring(intern->queue);
}

ZEND_METHOD(Io_Poll_OperationQueue, submit);
ZEND_METHOD(Io_Poll_OperationQueue, cancel);
ZEND_METHOD(Io_Poll_OperationQueue, add);
ZEND_METHOD(Io_Poll_OperationQueue, remove);
ZEND_METHOD(Io_Poll_OperationQueue, waitCompletions);
ZEND_METHOD(Io_Poll_OperationQueue, countPending);
ZEND_METHOD(Io_Poll_OperationQueue, getHookCapabilities);

/* A forked child cannot use the ring it inherited */
static zend_result php_io_ring_engine_check(zval *zv)
{
	php_io_opqueue_obj *intern = PHP_IO_OPQUEUE_FROM_ZOBJ(Z_OBJ_P(zv));
	if (intern->queue && php_io_ring_inherited(php_io_ring_engine_ring(intern))) {
		zend_throw_exception(php_io_ring_exception_ce, "The ring was created in another process", 0);
		return FAILURE;
	}
	return SUCCESS;
}

static void php_io_ring_engine_forward(INTERNAL_FUNCTION_PARAMETERS, zif_handler handler)
{
	if (php_io_ring_engine_check(ZEND_THIS) == FAILURE) {
		RETURN_THROWS();
	}
	handler(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

PHP_METHOD(Io_Ring_Engine, submit)
{
	php_io_ring_engine_forward(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_MN(Io_Poll_OperationQueue_submit));
}

PHP_METHOD(Io_Ring_Engine, cancel)
{
	php_io_ring_engine_forward(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_MN(Io_Poll_OperationQueue_cancel));
}

PHP_METHOD(Io_Ring_Engine, add)
{
	php_io_ring_engine_forward(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_MN(Io_Poll_OperationQueue_add));
}

PHP_METHOD(Io_Ring_Engine, remove)
{
	php_io_ring_engine_forward(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_MN(Io_Poll_OperationQueue_remove));
}

PHP_METHOD(Io_Ring_Engine, waitCompletions)
{
	php_io_ring_engine_forward(INTERNAL_FUNCTION_PARAM_PASSTHRU,
		ZEND_MN(Io_Poll_OperationQueue_waitCompletions));
}

PHP_METHOD(Io_Ring_Engine, countPending)
{
	php_io_ring_engine_forward(INTERNAL_FUNCTION_PARAM_PASSTHRU,
		ZEND_MN(Io_Poll_OperationQueue_countPending));
}

PHP_METHOD(Io_Ring_Engine, getHookCapabilities)
{
	php_io_ring_engine_forward(INTERNAL_FUNCTION_PARAM_PASSTHRU,
		ZEND_MN(Io_Poll_OperationQueue_getHookCapabilities));
}

PHP_METHOD(Io_Ring_Engine, __construct)
{
	zend_long entries = 0;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(entries)
	ZEND_PARSE_PARAMETERS_END();

	php_io_opqueue_obj *intern = PHP_IO_OPQUEUE_FROM_ZOBJ(Z_OBJ_P(ZEND_THIS));

	if (intern->queue) {
		zend_throw_error(NULL, "Io\\Ring\\Engine object is already constructed");
		RETURN_THROWS();
	}
	if (entries < 0 || entries > UINT32_MAX) {
		zend_argument_value_error(1, "must be between 0 and %u", UINT32_MAX);
		RETURN_THROWS();
	}

	intern->queue = php_io_queue_create_ring((uint32_t) entries);
	if (!intern->queue) {
		zend_throw_exception_ex(php_io_ring_exception_ce, errno,
				"Failed to create the ring: %s", strerror(errno));
		RETURN_THROWS();
	}
}

PHP_METHOD(Io_Ring_Engine, getBackend)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_io_opqueue_obj *intern = PHP_IO_OPQUEUE_FROM_ZOBJ(Z_OBJ_P(ZEND_THIS));
	if (!intern->queue) {
		zend_throw_error(NULL, "Io\\Ring\\Engine object is not constructed");
		RETURN_THROWS();
	}
	if (php_io_ring_engine_check(ZEND_THIS) == FAILURE) {
		RETURN_THROWS();
	}

	zend_long id;
	switch (php_io_ring_get_backend_type(php_io_ring_engine_ring(intern))) {
		case PHP_IO_RING_BACKEND_IO_URING: id = ZEND_ENUM_Io_Ring_Backend_IoUring; break;
		case PHP_IO_RING_BACKEND_IOCP: id = ZEND_ENUM_Io_Ring_Backend_Iocp; break;
		default: id = ZEND_ENUM_Io_Ring_Backend_Threads; break;
	}
	RETURN_OBJ_COPY(zend_enum_get_case_by_id(php_io_ring_backend_ce, id));
}

static void php_io_ring_engine_notify_clear(void *arg)
{
	php_io_ring_notify_clear(arg);
}

PHP_METHOD(Io_Ring_Engine, getHandle)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_io_opqueue_obj *intern = PHP_IO_OPQUEUE_FROM_ZOBJ(Z_OBJ_P(ZEND_THIS));
	if (!intern->queue) {
		zend_throw_error(NULL, "Io\\Ring\\Engine object is not constructed");
		RETURN_THROWS();
	}
	if (php_io_ring_engine_check(ZEND_THIS) == FAILURE) {
		RETURN_THROWS();
	}

	php_io_ring *ring = php_io_ring_engine_ring(intern);
	php_socket_t fd = php_io_ring_notify_fd(ring);
	if (fd == SOCK_ERR) {
		zend_throw_exception(php_io_ring_exception_ce, "The ring has no notification descriptor", 0);
		RETURN_THROWS();
	}
	php_io_poll_notify_handle_create_external(return_value, fd, php_io_ring_engine_notify_clear, ring, Z_OBJ_P(ZEND_THIS));
}

#endif /* HAVE_IOR */

PHP_MINIT_FUNCTION(io_ring)
{
#ifdef HAVE_IOR
	php_io_ring_backend_ce = register_class_Io_Ring_Backend();

	php_io_ring_engine_ce = register_class_Io_Ring_Engine(php_io_operation_queue_ce);
	php_io_ring_engine_ce->create_object = php_io_opqueue_create_object;
	php_io_ring_engine_ce->default_object_handlers = &php_io_opqueue_handlers;

	php_io_ring_exception_ce = register_class_Io_Ring_RingException(php_io_exception_class_entry);
	php_io_ring_failed_operation_exception_ce
			= register_class_Io_Ring_FailedRingOperationException(php_io_ring_exception_ce);
#endif
	return SUCCESS;
}
