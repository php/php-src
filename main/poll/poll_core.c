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
static const php_poll_backend_ops *registered_backends[16];
static int num_registered_backends = 0;

/* Forward declarations for backend ops */

#ifdef HAVE_POLL
extern const php_poll_backend_ops php_poll_backend_poll_ops;
#endif
#ifdef HAVE_EPOLL
extern const php_poll_backend_ops php_poll_backend_epoll_ops;
#endif
#ifdef HAVE_KQUEUE
extern const php_poll_backend_ops php_poll_backend_kqueue_ops;
#endif
#ifdef HAVE_EVENT_PORTS
extern const php_poll_backend_ops php_poll_backend_eventport_ops;
#endif
#ifdef _WIN32
extern const php_poll_backend_ops php_poll_backend_iocp_ops;
#endif
extern const php_poll_backend_ops php_poll_backend_select_ops;

/* Register all available backends */
PHPAPI void php_poll_register_backends(void)
{
	num_registered_backends = 0;

#ifdef _WIN32
	/* IOCP is preferred on Windows for high performance */
	if (php_poll_backend_iocp_ops.is_available()) {
		registered_backends[num_registered_backends++] = &php_poll_backend_iocp_ops;
	}
#endif

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

#ifdef HAVE_POLL
	/* Poll is available on Unix-like systems */
	registered_backends[num_registered_backends++] = &php_poll_backend_poll_ops;
#endif

	/* select() as a fallback */
	if (php_poll_backend_select_ops.is_available()) {
		registered_backends[num_registered_backends++] = &php_poll_backend_select_ops;
	}
}

/* Get backend operations */
const php_poll_backend_ops *php_poll_get_backend_ops(php_poll_backend_type backend)
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

/* Create new poll context */
PHPAPI php_poll_ctx *php_poll_create(php_poll_backend_type preferred_backend, bool persistent)
{
	php_poll_ctx *ctx = php_poll_calloc(1, sizeof(php_poll_ctx), persistent);
	if (!ctx) {
		return NULL;
	}
	ctx->persistent = persistent;

	/* Get backend operations */
	ctx->backend_ops = php_poll_get_backend_ops(preferred_backend);
	if (!ctx->backend_ops) {
		pefree(ctx, persistent);
		return NULL;
	}

	ctx->backend_type = preferred_backend;
	ctx->max_events_hint = 0; /* No hint by default */

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

	php_poll_set_system_error_if_not_set(ctx);
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

	php_poll_set_system_error_if_not_set(ctx);
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

	php_poll_set_system_error_if_not_set(ctx);
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

	php_poll_set_system_error_if_not_set(ctx);
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

	if (UNEXPECTED(nfds < 0)) {
		php_poll_set_system_error_if_not_set(ctx);
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
	return ctx ? ctx->backend_type : PHP_POLL_BACKEND_AUTO;
}

/* Check edge-triggering support */
PHPAPI bool php_poll_supports_et(php_poll_ctx *ctx)
{
	return ctx && ctx->backend_ops && ctx->backend_ops->supports_et;
}

/* Error retrieval */
PHPAPI php_poll_error php_poll_get_error(php_poll_ctx *ctx)
{
	return ctx ? ctx->last_error : PHP_POLL_ERR_INVALID;
}