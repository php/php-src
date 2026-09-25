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

#include "php_poll_internal.h"
#include "php_io.h"
#ifndef PHP_WIN32
# include <unistd.h>
#endif

/* Backend registry */
static const php_poll_backend_ops *registered_backends[8];
static int num_registered_backends = 0;

/* Forward declarations for backend ops */

#ifdef HAVE_EPOLL
extern const php_poll_backend_ops php_poll_backend_epoll_ops;
#endif
#ifdef HAVE_KQUEUE
extern const php_poll_backend_ops php_poll_backend_kqueue_ops;
#endif
#ifdef HAVE_EVENT_PORTS
extern const php_poll_backend_ops php_poll_backend_eventport_ops;
#endif
#ifdef PHP_WIN32
extern const php_poll_backend_ops php_poll_backend_wsapoll_ops;
#else
extern const php_poll_backend_ops php_poll_backend_poll_ops;
#endif

/* Register all available backends */
PHPAPI void php_poll_register_backends(void)
{
	num_registered_backends = 0;

#ifdef HAVE_EVENT_PORTS
	/* Event Ports are preferred on Solaris */
	if (php_poll_backend_eventport_ops.is_available()) {
		registered_backends[num_registered_backends++] = &php_poll_backend_eventport_ops;
	}
#endif

#ifdef HAVE_KQUEUE
	if (php_poll_backend_kqueue_ops.is_available()) {
		registered_backends[num_registered_backends++] = &php_poll_backend_kqueue_ops;
	}
#endif

#ifdef HAVE_EPOLL
	if (php_poll_backend_epoll_ops.is_available()) {
		registered_backends[num_registered_backends++] = &php_poll_backend_epoll_ops;
	}
#endif

#ifdef PHP_WIN32
	registered_backends[num_registered_backends++] = &php_poll_backend_wsapoll_ops;
#else
	registered_backends[num_registered_backends++] = &php_poll_backend_poll_ops;
#endif
}

/* Get backend operations */
static const php_poll_backend_ops *php_poll_get_backend_ops(php_poll_backend_type backend)
{
	if (backend == PHP_POLL_BACKEND_AUTO) {
		/* Return the first (best) available backend */
		return num_registered_backends > 0 ? registered_backends[0] : NULL;
	}

	for (int i = 0; i < num_registered_backends; i++) {
		if (registered_backends[i] && registered_backends[i]->type == backend) {
			return registered_backends[i];
		}
	}

	return NULL;
}

/* Get backend operations by backend name */
static const php_poll_backend_ops *php_poll_get_backend_ops_by_name(const char *backend_name)
{
	if (!backend_name) {
		return NULL;
	}

	for (int i = 0; i < num_registered_backends; i++) {
		if (registered_backends[i] && strcmp(registered_backends[i]->name, backend_name) == 0) {
			return registered_backends[i];
		}
	}

	return NULL;
}

PHPAPI bool php_poll_is_backend_available(php_poll_backend_type backend)
{
	if (backend == PHP_POLL_BACKEND_AUTO) {
		return true; /* Auto is always available */
	}

	for (int i = 0; i < num_registered_backends; i++) {
		if (registered_backends[i] && registered_backends[i]->type == backend) {
			return registered_backends[i]->is_available();
		}
	}

	return false;
}

PHPAPI bool php_poll_backend_supports_edge_triggering(php_poll_backend_type backend)
{
	if (backend == PHP_POLL_BACKEND_AUTO) {
		/* Check the first (best) available backend */
		if (num_registered_backends > 0 && registered_backends[0]) {
			return registered_backends[0]->supports_et;
		}
		return false;
	}

	for (int i = 0; i < num_registered_backends; i++) {
		if (registered_backends[i] && registered_backends[i]->type == backend) {
			return registered_backends[i]->supports_et;
		}
	}

	return false;
}

static php_poll_ctx *php_poll_create_context(uint32_t flags)
{
	bool persistent = flags & PHP_POLL_FLAG_PERSISTENT;
	php_poll_ctx *ctx = php_poll_calloc(1, sizeof(php_poll_ctx), persistent);
	if (!ctx) {
		return NULL;
	}
	ctx->persistent = persistent;
	ctx->raw_events = (flags & PHP_POLL_FLAG_RAW_EVENTS) != 0;
#ifndef PHP_WIN32
	ctx->owner_pid = getpid();
#endif

	return ctx;
}

/* Create new poll context */
PHPAPI php_poll_ctx *php_poll_create(php_poll_backend_type preferred_backend, uint32_t flags)
{
	php_poll_ctx *ctx = php_poll_create_context(flags);
	if (ctx == NULL) {
		return NULL;
	}

	/* Get backend operations */
	ctx->backend_ops = php_poll_get_backend_ops(preferred_backend);
	if (!ctx->backend_ops) {
		pefree(ctx, ctx->persistent);
		return NULL;
	}
	ctx->backend_type = preferred_backend;

	return ctx;
}

/* The process and signal sources (poll_source.c) are descriptors, a pidfd
 * or signalfd on Linux and a private kqueue on kqueue platforms, so any
 * backend that watches descriptors of every kind serves them */
static bool php_poll_backend_watches_any_descriptor(php_poll_backend_type backend)
{
	if (backend == PHP_POLL_BACKEND_AUTO) {
		if (num_registered_backends > 0 && registered_backends[0]) {
			backend = registered_backends[0]->type;
		} else {
			return false;
		}
	}
	return backend == PHP_POLL_BACKEND_EPOLL || backend == PHP_POLL_BACKEND_POLL
			|| backend == PHP_POLL_BACKEND_KQUEUE;
}

PHPAPI bool php_poll_backend_supports_process_handles(php_poll_backend_type backend)
{
#ifdef PHP_WIN32
	return false;
#else
	return php_poll_has_process_source() && php_poll_backend_watches_any_descriptor(backend);
#endif
}

PHPAPI bool php_poll_backend_supports_signal_handles(php_poll_backend_type backend)
{
#ifdef PHP_WIN32
	return false;
#else
	return php_poll_has_signal_source() && php_poll_backend_watches_any_descriptor(backend);
#endif
}

PHPAPI bool php_poll_backend_supports_priority(php_poll_backend_type backend)
{
	if (backend == PHP_POLL_BACKEND_AUTO) {
		if (num_registered_backends > 0 && registered_backends[0]) {
			return registered_backends[0]->supports_priority;
		}
		return false;
	}

	for (int i = 0; i < num_registered_backends; i++) {
		if (registered_backends[i] && registered_backends[i]->type == backend) {
			return registered_backends[i]->supports_priority;
		}
	}

	return false;
}

/* Create new poll context */
PHPAPI php_poll_ctx *php_poll_create_by_name(const char *preferred_backend, uint32_t flags)
{
	if (!strcmp(preferred_backend, "auto")) {
		return php_poll_create(PHP_POLL_BACKEND_AUTO, flags);
	}

	php_poll_ctx *ctx = php_poll_create_context(flags);
	if (ctx == NULL) {
		return NULL;
	}

	/* Get backend operations */
	ctx->backend_ops = php_poll_get_backend_ops_by_name(preferred_backend);
	if (!ctx->backend_ops) {
		pefree(ctx, ctx->persistent);
		return NULL;
	}
	ctx->backend_type = ctx->backend_ops->type;

	return ctx;
}

/* Set event capacity hint (optional optimization) */
PHPAPI zend_result php_poll_set_max_events_hint(php_poll_ctx *ctx, int max_events)
{
	ZEND_ASSERT(ctx);
	if (UNEXPECTED(max_events <= 0)) {
		php_poll_set_error(ctx, PHP_POLL_ERR_INVALID);
		return FAILURE;
	}

	if (UNEXPECTED(ctx->initialized)) {
		php_poll_set_error(ctx, PHP_POLL_ERR_INVALID);
		return FAILURE; /* Cannot change after init */
	}

	ctx->max_events_hint = max_events;
	return SUCCESS;
}

/* Initialize poll context */
PHPAPI zend_result php_poll_init(php_poll_ctx *ctx)
{
	if (UNEXPECTED(!ctx)) {
		return FAILURE;
	}

	if (UNEXPECTED(ctx->initialized)) {
		return SUCCESS;
	}

	/* Initialize backend - can use ctx->max_events_hint if helpful */
	if (EXPECTED(ctx->backend_ops->init(ctx) == SUCCESS)) {
		ctx->initialized = true;
		return SUCCESS;
	}

	php_poll_set_current_errno_error(ctx);
	return FAILURE;
}

/* Destroy poll context */
PHPAPI void php_poll_destroy(php_poll_ctx *ctx)
{
	if (!ctx) {
		return;
	}

	if (ctx->backend_ops && ctx->backend_ops->cleanup) {
		ctx->backend_ops->cleanup(ctx);
	}

	/* Armed timers are owned by their creators, who must remove them first */
	ZEND_ASSERT(ctx->timer_count == 0);
	if (ctx->timers) {
		pefree(ctx->timers, ctx->persistent);
	}

	pefree(ctx, ctx->persistent);
}

/* Timers */

#define PHP_POLL_TIMER_DISARMED UINT32_MAX

static zend_always_inline void php_poll_timer_heap_set(php_poll_ctx *ctx, uint32_t i, php_poll_timer *t)
{
	ctx->timers[i] = t;
	t->heap_idx = i;
}

static void php_poll_timer_heap_up(php_poll_ctx *ctx, uint32_t i)
{
	php_poll_timer *t = ctx->timers[i];
	while (i > 0) {
		uint32_t parent = (i - 1) / 2;
		if (ctx->timers[parent]->deadline <= t->deadline) {
			break;
		}
		php_poll_timer_heap_set(ctx, i, ctx->timers[parent]);
		i = parent;
	}
	php_poll_timer_heap_set(ctx, i, t);
}

static void php_poll_timer_heap_down(php_poll_ctx *ctx, uint32_t i)
{
	php_poll_timer *t = ctx->timers[i];
	for (;;) {
		uint32_t left = 2 * i + 1, right = left + 1, smallest = i;
		zend_hrtime_t best = t->deadline;
		if (left < ctx->timer_count && ctx->timers[left]->deadline < best) {
			smallest = left;
			best = ctx->timers[left]->deadline;
		}
		if (right < ctx->timer_count && ctx->timers[right]->deadline < best) {
			smallest = right;
		}
		if (smallest == i) {
			break;
		}
		php_poll_timer_heap_set(ctx, i, ctx->timers[smallest]);
		i = smallest;
	}
	php_poll_timer_heap_set(ctx, i, t);
}

static void php_poll_timer_arm(php_poll_ctx *ctx, php_poll_timer *t)
{
	if (ctx->timer_count == ctx->timer_cap) {
		ctx->timer_cap = ctx->timer_cap ? ctx->timer_cap * 2 : 16;
		ctx->timers = perealloc(ctx->timers, ctx->timer_cap * sizeof(*ctx->timers), ctx->persistent);
	}
	php_poll_timer_heap_set(ctx, ctx->timer_count++, t);
	php_poll_timer_heap_up(ctx, t->heap_idx);
}

static void php_poll_timer_disarm(php_poll_ctx *ctx, php_poll_timer *t)
{
	uint32_t i = t->heap_idx;
	if (i == PHP_POLL_TIMER_DISARMED) {
		return;
	}
	ZEND_ASSERT(i < ctx->timer_count && ctx->timers[i] == t);
	t->heap_idx = PHP_POLL_TIMER_DISARMED;
	ctx->timer_count--;
	if (i == ctx->timer_count) {
		return;
	}
	php_poll_timer_heap_set(ctx, i, ctx->timers[ctx->timer_count]);
	php_poll_timer_heap_down(ctx, i);
	php_poll_timer_heap_up(ctx, ctx->timers[i]->heap_idx);
}

PHPAPI php_poll_timer *php_poll_timer_add(php_poll_ctx *ctx, zend_hrtime_t deadline, zend_hrtime_t period, void *data)
{
	php_poll_timer *t = pemalloc(sizeof(*t), ctx->persistent);
	t->deadline = deadline;
	t->period = period;
	t->data = data;
	t->heap_idx = PHP_POLL_TIMER_DISARMED;
	php_poll_timer_arm(ctx, t);
	return t;
}

PHPAPI zend_result php_poll_timer_modify(php_poll_ctx *ctx, php_poll_timer *t, zend_hrtime_t deadline, zend_hrtime_t period, void *data)
{
	php_poll_timer_disarm(ctx, t);
	t->deadline = deadline;
	t->period = period;
	t->data = data;
	php_poll_timer_arm(ctx, t);
	return SUCCESS;
}

PHPAPI void php_poll_timer_remove(php_poll_ctx *ctx, php_poll_timer *t)
{
	php_poll_timer_disarm(ctx, t);
	pefree(t, ctx->persistent);
}

PHPAPI uint32_t php_poll_timer_count(php_poll_ctx *ctx)
{
	return ctx->timer_count;
}

/* Expired timers go first in the event array; what does not fit stays at
 * the head of the heap and makes the next wait return at once. */
static int php_poll_timer_report(php_poll_ctx *ctx, php_poll_event *events, int max_events, int n_fd_events)
{
	zend_hrtime_t now = zend_hrtime();
	int n_timers = 0;

	while (ctx->timer_count && ctx->timers[0]->deadline <= now && n_fd_events + n_timers < max_events) {
		php_poll_timer *t = ctx->timers[0];
		void *data = t->data;

		if (t->period) {
			/* Skip the missed periods at once; one past the end never fires */
			zend_hrtime_t periods = (now - t->deadline) / t->period + 1;
			if (periods > (ZEND_HRTIME_T_MAX - t->deadline) / t->period) {
				t->deadline = ZEND_HRTIME_T_MAX;
			} else {
				t->deadline += periods * t->period;
			}
			php_poll_timer_heap_down(ctx, 0);
		} else {
			php_poll_timer_disarm(ctx, t);
		}

		memmove(&events[1], &events[0], (n_fd_events + n_timers) * sizeof(*events));
		events[0].fd = -1;
		events[0].events = PHP_POLL_TIMER;
		events[0].revents = PHP_POLL_TIMER;
		events[0].data = data;
		n_timers++;
	}

	return n_fd_events + n_timers;
}

/* Add file descriptor */
static zend_always_inline bool php_poll_ctx_foreign(php_poll_ctx *ctx)
{
#ifdef PHP_WIN32
	return false;
#else
	return ctx->owner_pid != getpid();
#endif
}

PHPAPI zend_result php_poll_add(php_poll_ctx *ctx, int fd, uint32_t events, void *data)
{
	if (php_poll_ctx_foreign(ctx)) {
		ctx->last_error = PHP_POLL_ERR_PERMISSION;
		return FAILURE;
	}
	ZEND_ASSERT(ctx);
	if (UNEXPECTED(!ctx->initialized || fd < 0)) {
		php_poll_set_error(ctx, PHP_POLL_ERR_INVALID);
		return FAILURE;
	}

	if (UNEXPECTED((events & PHP_POLL_PRI) && !ctx->backend_ops->supports_priority)) {
		php_poll_set_error(ctx, PHP_POLL_ERR_NOSUPPORT);
		return FAILURE;
	}

	/* Delegate to backend - it handles all validation and tracking */
	if (EXPECTED(ctx->backend_ops->add(ctx, fd, events, data) == SUCCESS)) {
		return SUCCESS;
	}

	return FAILURE;
}

/* Modify file descriptor */
PHPAPI zend_result php_poll_modify(php_poll_ctx *ctx, int fd, uint32_t events, void *data)
{
	if (php_poll_ctx_foreign(ctx)) {
		ctx->last_error = PHP_POLL_ERR_PERMISSION;
		return FAILURE;
	}
	ZEND_ASSERT(ctx);
	if (UNEXPECTED(!ctx->initialized || fd < 0)) {
		php_poll_set_error(ctx, PHP_POLL_ERR_INVALID);
		return FAILURE;
	}

	if (UNEXPECTED((events & PHP_POLL_PRI) && !ctx->backend_ops->supports_priority)) {
		php_poll_set_error(ctx, PHP_POLL_ERR_NOSUPPORT);
		return FAILURE;
	}

	/* Delegate to backend - it handles validation */
	if (EXPECTED(ctx->backend_ops->modify(ctx, fd, events, data) == SUCCESS)) {
		return SUCCESS;
	}

	return FAILURE;
}

/* Remove file descriptor */
PHPAPI zend_result php_poll_remove(php_poll_ctx *ctx, int fd)
{
	if (php_poll_ctx_foreign(ctx)) {
		ctx->last_error = PHP_POLL_ERR_PERMISSION;
		return FAILURE;
	}
	ZEND_ASSERT(ctx);
	if (UNEXPECTED(!ctx->initialized || fd < 0)) {
		php_poll_set_error(ctx, PHP_POLL_ERR_INVALID);
		return FAILURE;
	}

	/* Delegate to backend - it handles validation */
	if (EXPECTED(ctx->backend_ops->remove(ctx, fd) == SUCCESS)) {
		return SUCCESS;
	}

	return FAILURE;
}

/* Wait for events */
PHPAPI int php_poll_wait(php_poll_ctx *ctx, php_poll_event *events, int max_events,
		const struct timespec *timeout)
{
	if (php_poll_ctx_foreign(ctx)) {
		ctx->last_error = PHP_POLL_ERR_PERMISSION;
		return -1;
	}
	ZEND_ASSERT(ctx);
	if (UNEXPECTED(!ctx->initialized || !events || max_events <= 0)) {
		php_poll_set_error(ctx, PHP_POLL_ERR_INVALID);
		return -1;
	}

	/* The nearest armed timer bounds the wait, and timers already due get
	 * their slots reserved so a level-triggered descriptor cannot starve them */
	struct timespec timer_ts;
	int n_due = 0;
	if (ctx->timer_count) {
		zend_hrtime_t now = zend_hrtime();
		for (uint32_t i = 0; i < ctx->timer_count; i++) {
			if (ctx->timers[i]->deadline <= now) {
				n_due++;
			}
		}
		if (n_due > max_events) {
			n_due = max_events;
		}
		zend_hrtime_t head = ctx->timers[0]->deadline;
		zend_hrtime_t remaining = head > now ? head - now : 0;
		if (!timeout || remaining < php_poll_timespec_to_ns(timeout)) {
			timer_ts.tv_sec = remaining / ZEND_NANO_IN_SEC;
			timer_ts.tv_nsec = remaining % ZEND_NANO_IN_SEC;
			timeout = &timer_ts;
		}
	}

	/* Delegate to backend - it handles everything including ET simulation if needed.
	 * An interrupted wait restarts with the remaining time unless PHP has a
	 * handler to run (an io_uring in the same process interrupts waits for
	 * its task work). */
	int nfds = 0;
	if (n_due < max_events) {
		zend_hrtime_t limit = ZEND_HRTIME_T_MAX;
		struct timespec rest_ts;
		if (timeout) {
			zend_hrtime_t now = zend_hrtime();
			zend_hrtime_t rel = php_poll_timespec_to_ns(timeout);
			limit = rel < ZEND_HRTIME_T_MAX - now ? now + rel : ZEND_HRTIME_T_MAX;
		}
		for (;;) {
			nfds = ctx->backend_ops->wait(ctx, events, max_events - n_due, timeout);
			if (nfds >= 0 || ctx->last_error != PHP_POLL_ERR_INTERRUPTED || php_io_interrupt_pending()) {
				break;
			}
			if (limit != ZEND_HRTIME_T_MAX) {
				zend_hrtime_t now = zend_hrtime();
				zend_hrtime_t remaining = limit > now ? limit - now : 0;
				rest_ts.tv_sec = remaining / ZEND_NANO_IN_SEC;
				rest_ts.tv_nsec = remaining % ZEND_NANO_IN_SEC;
				timeout = &rest_ts;
			}
		}
		if (nfds < 0) {
			return nfds;
		}
	}

	if (ctx->timer_count) {
		nfds = php_poll_timer_report(ctx, events, max_events, nfds);
	}

	return nfds;
}

/* Get backend name */
PHPAPI const char *php_poll_backend_name(php_poll_ctx *ctx)
{
	return ctx && ctx->backend_ops ? ctx->backend_ops->name : "unknown";
}

/* Get backend type */
PHPAPI php_poll_backend_type php_poll_get_backend_type(php_poll_ctx *ctx)
{
	return ctx && ctx->backend_ops ? ctx->backend_ops->type : PHP_POLL_BACKEND_AUTO;
}

/* Check edge-triggering support */
PHPAPI bool php_poll_supports_et(php_poll_ctx *ctx)
{
	return ctx && ctx->backend_ops && ctx->backend_ops->supports_et;
}

PHPAPI bool php_poll_supports_priority(php_poll_ctx *ctx)
{
	return ctx && ctx->backend_ops && ctx->backend_ops->supports_priority;
}

/* Get suitable max_events for backend */
PHPAPI int php_poll_get_suitable_max_events(php_poll_ctx *ctx)
{
	if (UNEXPECTED(!ctx || !ctx->backend_ops)) {
		return -1;
	}

	return ctx->backend_ops->get_suitable_max_events(ctx);
}

/* Error retrieval */
PHPAPI php_poll_error php_poll_get_error(php_poll_ctx *ctx)
{
	return ctx ? ctx->last_error : PHP_POLL_ERR_INVALID;
}

/* Errno to php_poll_error mapping helper */
php_poll_error php_poll_errno_to_error(int err)
{
	switch (err) {
		case 0:
			return PHP_POLL_ERR_NONE;

		case ENOMEM:
			return PHP_POLL_ERR_NOMEM;

		case EINVAL:
		case EBADF:
			return PHP_POLL_ERR_INVALID;

		case EEXIST:
			return PHP_POLL_ERR_EXISTS;

		case ENOENT:
			return PHP_POLL_ERR_NOTFOUND;

#ifdef ETIME
		case ETIME:
#endif
#ifdef ETIMEDOUT
		case ETIMEDOUT:
#endif
			return PHP_POLL_ERR_TIMEOUT;

		case EINTR:
			return PHP_POLL_ERR_INTERRUPTED;

		case EACCES:
#ifdef EPERM
		case EPERM:
#endif
			return PHP_POLL_ERR_PERMISSION;

#ifdef EMFILE
		case EMFILE:
#endif
#ifdef ENFILE
		case ENFILE:
#endif
			return PHP_POLL_ERR_TOOBIG;

		case EAGAIN:
#if defined(EWOULDBLOCK) && EWOULDBLOCK != EAGAIN
		case EWOULDBLOCK:
#endif
			return PHP_POLL_ERR_AGAIN;

#ifdef ENOSYS
		case ENOSYS:
#endif
#if ENOTSUP
		case ENOTSUP:
#endif
#if defined(EOPNOTSUPP) && EOPNOTSUPP != ENOTSUP
		case EOPNOTSUPP:
#endif
			return PHP_POLL_ERR_NOSUPPORT;

		default:
			return PHP_POLL_ERR_SYSTEM;
	}
}

/* Get human-readable error description */
PHPAPI const char *php_poll_error_string(php_poll_error error)
{
	switch (error) {
		case PHP_POLL_ERR_NONE:
			return "No error";
		case PHP_POLL_ERR_SYSTEM:
			return "System error";
		case PHP_POLL_ERR_NOMEM:
			return "Out of memory";
		case PHP_POLL_ERR_INVALID:
			return "Invalid argument";
		case PHP_POLL_ERR_EXISTS:
			return "File descriptor already exists";
		case PHP_POLL_ERR_NOTFOUND:
			return "File descriptor not found";
		case PHP_POLL_ERR_TIMEOUT:
			return "Operation timed out";
		case PHP_POLL_ERR_INTERRUPTED:
			return "Operation interrupted";
		case PHP_POLL_ERR_PERMISSION:
			return "Permission denied";
		case PHP_POLL_ERR_TOOBIG:
			return "Too many open files";
		case PHP_POLL_ERR_AGAIN:
			return "Resource temporarily unavailable";
		case PHP_POLL_ERR_NOSUPPORT:
			return "Operation not supported";
		default:
			return "Unknown error";
	}
}
