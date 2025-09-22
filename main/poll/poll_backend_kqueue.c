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

#ifdef HAVE_KQUEUE

#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>

typedef struct {
	int kqueue_fd;
	struct kevent *events;
	int events_capacity;
	int fd_count; /* Track number of unique FDs (not individual filters) */
	int filter_count; /* Track total number of filters for raw events */
	HashTable *complete_oneshot_fds; /* Track FDs with both read+write oneshot */
	HashTable *garbage_oneshot_fds; /* Pre-cached hash table for FDs to delete */
} kqueue_backend_data_t;

static zend_result kqueue_backend_init(php_poll_ctx *ctx)
{
	kqueue_backend_data_t *data
			= php_poll_calloc(1, sizeof(kqueue_backend_data_t), ctx->persistent);
	if (!data) {
		php_poll_set_error(ctx, PHP_POLL_ERR_NOMEM);
		return FAILURE;
	}

	data->kqueue_fd = kqueue();
	if (data->kqueue_fd == -1) {
		pefree(data, ctx->persistent);
		php_poll_set_error(ctx, PHP_POLL_ERR_SYSTEM);
		return FAILURE;
	}

	/* Use hint for initial allocation if provided, otherwise start with reasonable default */
	int initial_capacity = ctx->max_events_hint > 0 ? ctx->max_events_hint : 64;
	data->events = php_poll_calloc(initial_capacity, sizeof(struct kevent), ctx->persistent);
	if (!data->events) {
		close(data->kqueue_fd);
		pefree(data, ctx->persistent);
		php_poll_set_error(ctx, PHP_POLL_ERR_NOMEM);
		return FAILURE;
	}
	data->events_capacity = initial_capacity;
	data->fd_count = 0; /* Initialize FD counter */
	data->filter_count = 0; /* Initialize filter counter */

	/* Only initialize oneshot related hash tables if not using raw events */
	if (!ctx->raw_events) {
		data->complete_oneshot_fds = php_poll_malloc(sizeof(HashTable), ctx->persistent);
		zend_hash_init(data->complete_oneshot_fds, 8, NULL, NULL, ctx->persistent);
		data->garbage_oneshot_fds = php_poll_malloc(sizeof(HashTable), ctx->persistent);
		zend_hash_init(data->garbage_oneshot_fds, 8, NULL, NULL, ctx->persistent);
	} else {
		data->complete_oneshot_fds = NULL;
		data->garbage_oneshot_fds = NULL;
	}

	ctx->backend_data = data;
	return SUCCESS;
}

static void kqueue_backend_cleanup(php_poll_ctx *ctx)
{
	kqueue_backend_data_t *data = (kqueue_backend_data_t *) ctx->backend_data;
	if (data) {
		if (data->kqueue_fd >= 0) {
			close(data->kqueue_fd);
		}
		pefree(data->events, ctx->persistent);

		/* Only cleanup hash tables if they were initialized */
		if (data->complete_oneshot_fds) {
			zend_hash_destroy(data->complete_oneshot_fds);
			pefree(data->complete_oneshot_fds, ctx->persistent);
		}
		if (data->garbage_oneshot_fds) {
			zend_hash_destroy(data->garbage_oneshot_fds);
			pefree(data->garbage_oneshot_fds, ctx->persistent);
		}

		pefree(data, ctx->persistent);
		ctx->backend_data = NULL;
	}
}

static zend_result kqueue_backend_add(php_poll_ctx *ctx, int fd, uint32_t events, void *data)
{
	kqueue_backend_data_t *backend_data = (kqueue_backend_data_t *) ctx->backend_data;

	struct kevent changes[2]; /* Max 2 changes: read + write */
	int change_count = 0;

	uint16_t flags = EV_ADD | EV_ENABLE;
	if (events & PHP_POLL_ONESHOT) {
		flags |= EV_ONESHOT;
	}
	if (events & PHP_POLL_ET) {
		flags |= EV_CLEAR;
	}

	if (events & PHP_POLL_READ) {
		EV_SET(&changes[change_count], fd, EVFILT_READ, flags, 0, 0, data);
		change_count++;
	}

	if (events & PHP_POLL_WRITE) {
		EV_SET(&changes[change_count], fd, EVFILT_WRITE, flags, 0, 0, data);
		change_count++;
	}

	if (change_count > 0) {
		int result = kevent(backend_data->kqueue_fd, changes, change_count, NULL, 0, NULL);
		if (result == -1) {
			php_poll_set_current_errno_error(ctx);
			return FAILURE;
		}

		/* Increment FD count only once per unique FD */
		backend_data->fd_count++;
		/* Increment filter count by number of filters added */
		backend_data->filter_count += change_count;

		/* Track oneshot only if not using raw events */
		if (!ctx->raw_events
				&& (events & (PHP_POLL_READ | PHP_POLL_WRITE | PHP_POLL_ONESHOT))
						== (PHP_POLL_READ | PHP_POLL_WRITE | PHP_POLL_ONESHOT)) {
			zend_hash_index_add_empty_element(backend_data->complete_oneshot_fds, fd);
		}
	}

	return SUCCESS;
}

static zend_result kqueue_backend_modify(php_poll_ctx *ctx, int fd, uint32_t events, void *data)
{
	kqueue_backend_data_t *backend_data = (kqueue_backend_data_t *) ctx->backend_data;

	struct kevent deletes[2];
	struct kevent adds[2];
	int delete_count = 0;
	int add_count = 0;
	int successful_deletes = 0;

	uint16_t add_flags = EV_ADD | EV_ENABLE;
	if (events & PHP_POLL_ONESHOT) {
		add_flags |= EV_ONESHOT;
	}
	if (events & PHP_POLL_ET) {
		add_flags |= EV_CLEAR;
	}

	/* Delete existing filters that are not in the new events */
	if (!(events & PHP_POLL_READ)) {
		EV_SET(&deletes[delete_count], fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
		delete_count++;
	}
	if (!(events & PHP_POLL_WRITE)) {
		EV_SET(&deletes[delete_count], fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
		delete_count++;
	}

	/* Prepare add operations for requested events */
	if (events & PHP_POLL_READ) {
		EV_SET(&adds[add_count], fd, EVFILT_READ, add_flags, 0, 0, data);
		add_count++;
	}
	if (events & PHP_POLL_WRITE) {
		EV_SET(&adds[add_count], fd, EVFILT_WRITE, add_flags, 0, 0, data);
		add_count++;
	}

	/* Delete existing filters individually to count successes */
	for (int i = 0; i < delete_count; i++) {
		int result = kevent(backend_data->kqueue_fd, &deletes[i], 1, NULL, 0, NULL);
		if (result == 0) {
			successful_deletes++;
		} else if (!php_poll_is_not_found_error()) {
			php_poll_set_current_errno_error(ctx);
			return FAILURE;
		}
		/* ENOENT is ignored - filter didn't exist */
	}

	/* Add new filters */
	if (add_count > 0) {
		int result = kevent(backend_data->kqueue_fd, adds, add_count, NULL, 0, NULL);
		if (result == -1) {
			php_poll_set_current_errno_error(ctx);
			return FAILURE;
		}
	}

	/* Update counters and oneshot tracking */
	if (successful_deletes > 0 && add_count == 0) {
		/* Removed all filters - FD is gone */
		backend_data->fd_count--;
		backend_data->filter_count -= successful_deletes;
		if (!ctx->raw_events) {
			zend_hash_index_del(backend_data->complete_oneshot_fds, fd);
		}
	} else if (successful_deletes == 0 && add_count > 0) {
		/* Added filters to previously empty FD */
		backend_data->fd_count++;
		backend_data->filter_count += add_count;
		if (!ctx->raw_events
				&& (events & (PHP_POLL_READ | PHP_POLL_WRITE | PHP_POLL_ONESHOT))
						== (PHP_POLL_READ | PHP_POLL_WRITE | PHP_POLL_ONESHOT)) {
			zend_hash_index_add_empty_element(backend_data->complete_oneshot_fds, fd);
		}
	} else if (successful_deletes > 0 || add_count > 0) {
		/* Mixed operation - update filter count */
		backend_data->filter_count = backend_data->filter_count - successful_deletes + add_count;
		if (!ctx->raw_events) {
			/* One of the filters was deleted so remove from oneshot tracking */
			zend_hash_index_del(backend_data->complete_oneshot_fds, fd);
		}
	}

	return SUCCESS;
}

static zend_result kqueue_backend_remove(php_poll_ctx *ctx, int fd)
{
	kqueue_backend_data_t *backend_data = (kqueue_backend_data_t *) ctx->backend_data;
	struct kevent change;
	int successful_deletes = 0;

	/* Try to remove read filter */
	EV_SET(&change, fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
	int result = kevent(backend_data->kqueue_fd, &change, 1, NULL, 0, NULL);
	if (result == 0) {
		successful_deletes++;
	} else if (!php_poll_is_not_found_error()) {
		php_poll_set_current_errno_error(ctx);
		return FAILURE;
	}

	/* Try to remove write filter */
	EV_SET(&change, fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
	result = kevent(backend_data->kqueue_fd, &change, 1, NULL, 0, NULL);
	if (result == 0) {
		successful_deletes++;
	} else if (!php_poll_is_not_found_error()) {
		php_poll_set_current_errno_error(ctx);
		return FAILURE;
	}

	/* If no filters were successfully deleted, that's an error */
	if (successful_deletes == 0) {
		php_poll_set_error(ctx, PHP_POLL_ERR_NOTFOUND);
		return FAILURE;
	}

	/* Update counters - we removed all filters for this FD */
	backend_data->fd_count--;
	backend_data->filter_count -= successful_deletes;

	/* Remove from complete oneshot tracking if not using raw events */
	if (!ctx->raw_events) {
		zend_hash_index_del(backend_data->complete_oneshot_fds, fd);
	}

	return SUCCESS;
}

static int kqueue_backend_wait(
		php_poll_ctx *ctx, php_poll_event *events, int max_events, int timeout)
{
	kqueue_backend_data_t *backend_data = (kqueue_backend_data_t *) ctx->backend_data;

	/* For raw events, we need capacity for max_events.
	 * For grouped events, kqueue can return up to 2 events per FD, so we need 2x capacity. */
	int required_capacity = ctx->raw_events ? max_events : (max_events * 2);
	if (required_capacity > backend_data->events_capacity) {
		struct kevent *new_events = php_poll_realloc(
				backend_data->events, required_capacity * sizeof(struct kevent), ctx->persistent);
		if (!new_events) {
			php_poll_set_error(ctx, PHP_POLL_ERR_NOMEM);
			return -1;
		}
		backend_data->events = new_events;
		backend_data->events_capacity = required_capacity;
	}

	struct timespec ts = { 0 }, *tsp = NULL;
	if (timeout >= 0) {
		ts.tv_sec = timeout / 1000;
		ts.tv_nsec = (timeout % 1000) * 1000000;
		tsp = &ts;
	}

	int nfds = kevent(
			backend_data->kqueue_fd, NULL, 0, backend_data->events, required_capacity, tsp);

	if (nfds > 0) {
		if (ctx->raw_events) {
			/* Raw events mode - direct 1:1 mapping, no grouping */
			for (int i = 0; i < nfds && i < max_events; i++) {
				events[i].fd = (int) backend_data->events[i].ident;
				events[i].events = 0; /* Not used in raw mode */
				events[i].revents = 0;
				events[i].data = backend_data->events[i].udata;

				/* Convert kqueue filter to poll event */
				if (backend_data->events[i].filter == EVFILT_READ) {
					events[i].revents |= PHP_POLL_READ;
				} else if (backend_data->events[i].filter == EVFILT_WRITE) {
					events[i].revents |= PHP_POLL_WRITE;
				}

				/* Convert kqueue flags to poll events */
				if (backend_data->events[i].flags & EV_EOF) {
					events[i].revents |= PHP_POLL_HUP;
				}
				if (backend_data->events[i].flags & EV_ERROR) {
					events[i].revents |= PHP_POLL_ERROR;
				}
			}
			/* In raw mode, we might return fewer events than nfds if max_events < nfds */
			return nfds > max_events ? max_events : nfds;
		} else {
			/* Grouped events mode - existing complex logic */
			int unique_events = 0, fd;
			zend_hash_clean(backend_data->garbage_oneshot_fds);

			for (int i = 0; i < nfds; i++) {
				fd = (int) backend_data->events[i].ident;
				uint32_t revents = 0;
				void *data = backend_data->events[i].udata;
				bool is_oneshot = (backend_data->events[i].flags & EV_ONESHOT) != 0;

				/* Convert this event */
				if (backend_data->events[i].filter == EVFILT_READ) {
					revents |= PHP_POLL_READ;
				} else if (backend_data->events[i].filter == EVFILT_WRITE) {
					revents |= PHP_POLL_WRITE;
				}

				if (backend_data->events[i].flags & EV_EOF) {
					revents |= PHP_POLL_HUP;
				}
				if (backend_data->events[i].flags & EV_ERROR) {
					revents |= PHP_POLL_ERROR;
				}

				/* Look for existing event for this FD */
				bool found = false;
				for (int j = 0; j < unique_events; j++) {
					if (events[j].fd == fd) {
						/* Combine with existing event */
						events[j].revents |= revents;
						found = true;
						break;
					}
				}

				if (!found) {
					/* New FD, create new event */
					ZEND_ASSERT(unique_events < max_events);
					events[unique_events].fd = fd;
					events[unique_events].events = 0;
					events[unique_events].revents = revents;
					events[unique_events].data = data;
					unique_events++;

					if (is_oneshot
							&& zend_hash_index_exists(backend_data->complete_oneshot_fds, fd)) {
						zval dummy;
						ZVAL_BOOL(&dummy, revents & PHP_POLL_READ);
						zend_hash_index_add(backend_data->garbage_oneshot_fds, fd, &dummy);
						zend_hash_index_del(backend_data->complete_oneshot_fds, fd);
						backend_data->fd_count--;
					}
				} else if (is_oneshot) {
					zend_hash_index_del(backend_data->garbage_oneshot_fds, fd);
				}
			}

			/* Clean up all the same FD filters for other read or write side */
			zval *item;
			struct kevent cleanup_change;
			ZEND_HASH_FOREACH_NUM_KEY_VAL(backend_data->garbage_oneshot_fds, fd, item)
			{
				int filter = Z_TYPE_P(item) == IS_TRUE ? EVFILT_WRITE : EVFILT_READ;
				EV_SET(&cleanup_change, fd, filter, EV_DELETE, 0, 0, NULL);
				kevent(backend_data->kqueue_fd, &cleanup_change, 1, NULL, 0, NULL);
			}
			ZEND_HASH_FOREACH_END();

			return unique_events;
		}
	}

	return nfds;
}

static bool kqueue_backend_is_available(void)
{
	int fd = kqueue();
	if (fd >= 0) {
		close(fd);
		return true;
	}
	return false;
}

static int kqueue_backend_get_suitable_max_events(php_poll_ctx *ctx)
{
	kqueue_backend_data_t *backend_data = (kqueue_backend_data_t *) ctx->backend_data;

	if (!backend_data) {
		return -1;
	}

	if (ctx->raw_events) {
		/* For raw events, return the total number of filters */
		int active_filters = backend_data->filter_count;
		return active_filters == 0 ? 1 : active_filters;
	} else {
		/* For grouped events, return the number of unique FDs */
		int active_fds = backend_data->fd_count;
		return active_fds == 0 ? 1 : active_fds;
	}
}

const php_poll_backend_ops php_poll_backend_kqueue_ops = {
	.type = PHP_POLL_BACKEND_KQUEUE,
	.name = "kqueue",
	.init = kqueue_backend_init,
	.cleanup = kqueue_backend_cleanup,
	.add = kqueue_backend_add,
	.modify = kqueue_backend_modify,
	.remove = kqueue_backend_remove,
	.wait = kqueue_backend_wait,
	.is_available = kqueue_backend_is_available,
	.get_suitable_max_events = kqueue_backend_get_suitable_max_events,
	.supports_et = true /* kqueue supports EV_CLEAR for edge triggering */
};

#endif /* HAVE_KQUEUE */
