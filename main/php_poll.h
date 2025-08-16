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

#ifndef PHP_POLL_H
#define PHP_POLL_H

#include "php.h"

/* clang-format off */

/* Event types */
#define PHP_POLL_READ    0x01
#define PHP_POLL_WRITE   0x02
#define PHP_POLL_ERROR   0x04
#define PHP_POLL_HUP     0x08
#define PHP_POLL_RDHUP   0x10
#define PHP_POLL_ONESHOT 0x20
#define PHP_POLL_ET      0x40 /* Edge-triggered */

/* Poll backend types */
typedef enum {
	PHP_POLL_BACKEND_AUTO = -1,
	PHP_POLL_BACKEND_POLL = 0,
	PHP_POLL_BACKEND_EPOLL,
	PHP_POLL_BACKEND_KQUEUE,
	PHP_POLL_BACKEND_EVENTPORT,
	PHP_POLL_BACKEND_SELECT,
	PHP_POLL_BACKEND_IOCP
} php_poll_backend_type;

/* Result codes */
typedef enum {
    PHP_POLL_ERR_NONE,
    PHP_POLL_ERR_SYSTEM,
    PHP_POLL_ERR_NOMEM,
    PHP_POLL_ERR_INVALID,
    PHP_POLL_ERR_EXISTS,
    PHP_POLL_ERR_NOTFOUND,
    PHP_POLL_ERR_TIMEOUT,
} php_poll_error;

/* clang-format on */

/* Forward declarations */
typedef struct php_poll_ctx php_poll_ctx;
typedef struct php_poll_fd_entry php_poll_fd_entry;
typedef struct php_poll_backend_ops php_poll_backend_ops;
typedef struct php_poll_event php_poll_event;

/* Public API */
php_poll_ctx *php_poll_create(
		int max_events, php_poll_backend_type preferred_backend, bool persistent);
void php_poll_destroy(php_poll_ctx *ctx);

zend_result php_poll_add(php_poll_ctx *ctx, int fd, uint32_t events, void *data);
zend_result php_poll_modify(php_poll_ctx *ctx, int fd, uint32_t events, void *data);
zend_result php_poll_remove(php_poll_ctx *ctx, int fd);

int php_poll_wait(php_poll_ctx *ctx, php_poll_event *events, int max_events, int timeout);

const char *php_poll_backend_name(php_poll_ctx *ctx);
php_poll_backend_type php_poll_get_backend_type(php_poll_ctx *ctx);
bool php_poll_supports_et(php_poll_ctx *ctx);

/* Backend registration */
void php_poll_register_backends(void);
const php_poll_backend_ops *php_poll_get_backend_ops(php_poll_backend_type backend);

php_poll_error php_poll_get_error(php_poll_ctx *ctx);

#endif /* PHP_POLL_H */
