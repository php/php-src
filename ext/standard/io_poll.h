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
   | Author: Jakub Zelenka <bukka@php.net>                                |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_IO_POLL_H
#define PHP_IO_POLL_H

#include "php_streams.h"
#include "main/php_poll.h"

BEGIN_EXTERN_C()

PHPAPI void php_io_poll_stream_notify_close(php_stream *stream);

PHPAPI extern zend_class_entry *php_io_exception_class_entry;
PHPAPI extern zend_class_entry *php_io_poll_event_class_entry;
PHPAPI extern zend_class_entry *php_io_poll_handle_class_entry;
PHPAPI extern zend_class_entry *php_io_poll_weak_handle_class_entry;
PHPAPI extern zend_class_entry *php_stream_poll_handle_class_entry;

PHPAPI zend_result php_io_poll_events_to_event_enums(uint32_t events, zval *event_enums);
PHPAPI void php_io_poll_throw_failed_wait(const char *message, php_poll_error error);
PHPAPI uint32_t php_io_poll_event_enums_to_events(zval *event_enums);

PHPAPI void php_stream_poll_handle_from_stream(zval *dest, php_stream *stream);

PHPAPI void php_io_poll_timer_handle_create(zval *dest, zend_hrtime_t timeout_ns, bool periodic);
PHPAPI void php_io_poll_process_handle_create(zval *dest, pid_t pid);
PHPAPI void php_io_poll_signal_handle_create(zval *dest, const php_sigset_t *set);
PHPAPI bool php_io_poll_process_handle_status(zend_object *handle, int *status);
/* Returns 0 when no signal in the set was recorded */
PHPAPI int php_io_poll_signal_handle_take(zend_object *handle, const php_sigset_t *set,
	php_siginfo_t *info);

/* Thread safe and async-signal safe */
PHPAPI void php_poll_notify(zend_object *handle);

/* A NotifyHandle over a descriptor owned and cleared by owner; notify() is unavailable on it */
PHPAPI void php_io_poll_notify_handle_create_external(zval *dest, php_socket_t fd,
	void (*clear)(void *arg), void *arg, zend_object *owner);

#ifndef PHP_WIN32
/* The current mask without the signals blocked by live SignalHandle objects */
PHPAPI void php_io_poll_signal_child_mask(sigset_t *mask);
#endif
/* The stream's StreamPollWeakHandle, created on the first call and kept by the stream: borrowed.
 * expose lets getStream() hand the stream out, and stays set once it was. */
PHPAPI zend_object *php_stream_get_poll_handle(php_stream *stream, bool expose);
PHPAPI void php_stream_poll_weak_handle_notify(zend_object *handle_obj);
PHPAPI void php_io_poll_handle_remove_from_all_contexts(zend_object *handle_obj);
/* Retires every watcher on the handle while its descriptor is still open */
PHPAPI void php_poll_handle_invalidate(zend_object *handle_obj);

END_EXTERN_C()

#endif /* PHP_IO_POLL_H */
