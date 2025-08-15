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

/* Event types */
#define PHP_POLL_READ     0x01
#define PHP_POLL_WRITE    0x02
#define PHP_POLL_ERROR    0x04
#define PHP_POLL_HUP      0x08
#define PHP_POLL_RDHUP    0x10
#define PHP_POLL_ONESHOT  0x20
#define PHP_POLL_ET       0x40  /* Edge-triggered */

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

/* Error codes */
#define PHP_POLL_OK          0
#define PHP_POLL_ERROR      -1
#define PHP_POLL_NOMEM      -2
#define PHP_POLL_INVALID    -3
#define PHP_POLL_EXISTS     -4
#define PHP_POLL_NOTFOUND   -5
#define PHP_POLL_TIMEOUT    -6

/* Forward declarations */
typedef struct php_poll_ctx php_poll_ctx;
typedef struct php_poll_fd_entry php_poll_fd_entry;

/* Poll event structure */
typedef struct {
	int fd;           /* File descriptor */
	uint32_t events;  /* Requested events */
	uint32_t revents; /* Returned events */
	void *data;       /* User data pointer */
} php_poll_event_t;

/* FD entry for tracking state */
struct php_poll_fd_entry {
	int fd;
	uint32_t events;
	uint32_t last_revents; /* For edge-trigger simulation */
	void *data;
	bool active;
	bool et_armed; /* Edge-trigger state */
};

/* Backend interface */
typedef struct php_poll_backend_ops {
	const char *name;
	
	/* Initialize backend */
	int (*init)(php_poll_ctx *ctx, int max_events);
	
	/* Cleanup backend */
	void (*cleanup)(php_poll_ctx *ctx);
	
	/* Add file descriptor */
	int (*add)(php_poll_ctx *ctx, int fd, uint32_t events, void *data);
	
	/* Modify file descriptor */
	int (*modify)(php_poll_ctx *ctx, int fd, uint32_t events, void *data);
	
	/* Remove file descriptor */
	int (*remove)(php_poll_ctx *ctx, int fd);
	
	/* Wait for events */
	int (*wait)(php_poll_ctx *ctx, php_poll_event_t *events, int max_events, int timeout);
	
	/* Check if backend is available */
	bool (*is_available)(void);
	
	/* Backend supports edge triggering natively */
	bool supports_et;
} php_poll_backend_ops;

/* Main poll context */
struct php_poll_ctx {
	const php_poll_backend_ops *backend_ops;
	php_poll_backend_type backend_type;
	
	int max_events;
	int num_fds;
	bool initialized;
	bool simulate_et; /* Whether to simulate edge triggering */
	
	/* FD tracking for edge-trigger simulation */
	php_poll_fd_entry *fd_entries;
	int fd_entries_size;
	
	/* Backend-specific data */
	void *backend_data;
};

/* Public API */
php_poll_ctx* php_poll_create(int max_events, php_poll_backend_type preferred_backend);
void php_poll_destroy(php_poll_ctx *ctx);

int php_poll_add(php_poll_ctx *ctx, int fd, uint32_t events, void *data);
int php_poll_modify(php_poll_ctx *ctx, int fd, uint32_t events, void *data);
int php_poll_remove(php_poll_ctx *ctx, int fd);

int php_poll_wait(php_poll_ctx *ctx, php_poll_event_t *events,  int max_events, int timeout);

const char* php_poll_backend_name(php_poll_ctx *ctx);
php_poll_backend_type php_poll_get_backend_type(php_poll_ctx *ctx);
bool php_poll_supports_et(php_poll_ctx *ctx);

/* Backend registration */
void php_poll_register_backends(void);
const php_poll_backend_ops* php_poll_get_backend_ops(php_poll_backend_type backend);

#endif /* PHP_POLL_H */
