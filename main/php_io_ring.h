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

#ifndef PHP_IO_RING_H
#define PHP_IO_RING_H

#include "php.h"
#include "main/php_io_hooks.h"

#ifdef HAVE_IOR

BEGIN_EXTERN_C()

/* A PHPAPI wrapper over ior */

typedef struct php_io_ring php_io_ring;

/* The core's queue depth, and Io\Ring\Engine's for 0 */
#define PHP_IO_RING_DEFAULT_ENTRIES 256

/* Keep in sync with Io\Ring\Backend */
typedef enum php_io_ring_backend_type {
	PHP_IO_RING_BACKEND_IO_URING,
	PHP_IO_RING_BACKEND_IOCP,
	PHP_IO_RING_BACKEND_THREADS,
} php_io_ring_backend_type;

/* fd_nonblock: every submitted descriptor is non-blocking */
PHPAPI php_io_ring *php_io_ring_create(uint32_t entries, bool fd_nonblock);
PHPAPI void php_io_ring_destroy(php_io_ring *ring);

/* After it returns the op is completed or cancellable */
PHPAPI zend_result php_io_ring_submit_op(php_io_ring *ring, php_io_op *op, void *data);
/* An op completed at submit (an accept from the buffer, a wait answered from the ready bits) */
PHPAPI bool php_io_ring_take_inline(php_io_ring *ring, php_io_op *op, php_io_queue_completion *out);
PHPAPI zend_result php_io_ring_cancel(php_io_ring *ring, php_io_op *op);
/* Cancels the op silently; true when it stays in flight and its stream must stay frozen */
PHPAPI bool php_io_ring_orphan(php_io_ring *ring, php_io_op *op);
PHPAPI void php_io_ring_drain(php_io_ring *ring, php_stream *stream);

/* dl NULL waits for good; with the non-blocking deadline it first clears the notification
 * descriptor and reaps once */
PHPAPI int php_io_ring_wait(php_io_ring *ring, php_io_queue_completion *out, uint32_t max,
		const php_deadline *dl);
PHPAPI uint32_t php_io_ring_count_pending(php_io_ring *ring);

PHPAPI php_socket_t php_io_ring_notify_fd(php_io_ring *ring);
PHPAPI void php_io_ring_notify_clear(php_io_ring *ring);
PHPAPI uint32_t php_io_ring_features(php_io_ring *ring);
PHPAPI php_io_ring_backend_type php_io_ring_get_backend_type(php_io_ring *ring);
PHPAPI const char *php_io_ring_backend_name(php_io_ring *ring);
/* PHP_IO_HOOKS_F_* a provider on this ring should register with by default */
PHPAPI uint32_t php_io_ring_hook_flags(php_io_ring *ring);
/* PHP_IO_HOOKS_F_* this ring can serve, for a provider that opts in */
PHPAPI uint32_t php_io_ring_supported_hook_flags(php_io_ring *ring);

/* Edge registrations; Level ones never reach the ring */
PHPAPI zend_result php_io_ring_add(php_io_ring *ring, php_io_registration *reg);
PHPAPI void php_io_ring_remove(php_io_ring *ring, php_io_registration *reg);

/* Created by another process: every operation fails */
PHPAPI bool php_io_ring_inherited(php_io_ring *ring);
/* In a forked child: closes the descriptors of every ring of this thread */
PHPAPI void php_io_ring_after_fork(void);

PHPAPI php_io_queue *php_io_queue_create_ring(uint32_t entries);
/* Only for a queue created by php_io_queue_create_ring() */
PHPAPI php_io_ring *php_io_queue_ring(php_io_queue *q);

END_EXTERN_C()

#endif /* HAVE_IOR */

#endif /* PHP_IO_RING_H */
