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
	if (UNEXPECTED(!ctx || max_events <= 0)) {
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

	pefree(ctx, ctx->persistent);
}

/* Add file descriptor */
PHPAPI zend_result php_poll_add(php_poll_ctx *ctx, int fd, uint32_t events, void *data)
{
	if (UNEXPECTED(!ctx || !ctx->initialized || fd < 0)) {
		php_poll_set_error(ctx, PHP_POLL_ERR_INVALID);
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
	if (UNEXPECTED(!ctx || !ctx->initialized || fd < 0)) {
		php_poll_set_error(ctx, PHP_POLL_ERR_INVALID);
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
	if (UNEXPECTED(!ctx || !ctx->initialized || fd < 0)) {
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
PHPAPI int php_poll_wait(php_poll_ctx *ctx, php_poll_event *events, int max_events, int timeout)
{
	if (UNEXPECTED(!ctx || !ctx->initialized || !events || max_events <= 0)) {
		php_poll_set_error(ctx, PHP_POLL_ERR_INVALID);
		return -1;
	}

	/* Delegate to backend - it handles everything including ET simulation if needed */
	int nfds = ctx->backend_ops->wait(ctx, events, max_events, timeout);

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
	return ctx ? ctx->backend_type : PHP_POLL_BACKEND_AUTO;
}

/* Check edge-triggering support */
PHPAPI bool php_poll_supports_et(php_poll_ctx *ctx)
{
	return ctx && ctx->backend_ops && ctx->backend_ops->supports_et;
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
