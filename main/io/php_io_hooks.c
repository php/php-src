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

#include "php.h"
#include "php_io.h"
#include "ext/standard/file.h"
#include "ext/standard/io_poll.h"

#include <errno.h>
#ifndef PHP_WIN32
# include <netdb.h>
# include <arpa/inet.h>
#endif
#include <time.h>
#include "zend_fibers.h"
#ifndef PHP_WIN32
# include <sys/wait.h>
# include <signal.h>
# include <fcntl.h>
#endif

PHPAPI void (*php_io_op_zobj_detach)(zend_object *zobj) = NULL;
PHPAPI bool (*php_io_signal_pending)(void) = NULL;

PHPAPI bool php_io_interrupt_pending(void)
{
	return zend_atomic_bool_load_ex(&EG(vm_interrupt)) || (php_io_signal_pending && php_io_signal_pending());
}

/* The callers of the socket entry points read php_socket_errno(), which on
 * Windows is the Winsock error rather than errno */
#ifdef PHP_WIN32
static int php_io_wsa_error(int err)
{
	switch (err) {
		case EINTR: return WSAEINTR;
		case EAGAIN: return WSAEWOULDBLOCK;
		case EBADF: return WSAEBADF;
		case EACCES: return WSAEACCES;
		case EINVAL: return WSAEINVAL;
		case EMFILE: return WSAEMFILE;
		case ETIMEDOUT: return WSAETIMEDOUT;
		case ECANCELED: return WSAECANCELLED;
		case ENOTSUP: return WSAEOPNOTSUPP;
		case EALREADY: return WSAEALREADY;
		case EISCONN: return WSAEISCONN;
		case ENOTCONN: return WSAENOTCONN;
		case ECONNREFUSED: return WSAECONNREFUSED;
		case ECONNRESET: return WSAECONNRESET;
		case ECONNABORTED: return WSAECONNABORTED;
		default: return err;
	}
}
#endif

static zend_always_inline void php_io_set_errno(int err)
{
#ifdef PHP_WIN32
	WSASetLastError(php_io_wsa_error(err));
#endif
	errno = err;
}

/* A stream whose in-flight op outlived its frame, kept by a queue */
typedef struct {
	php_io_queue *queue;
	php_stream *stream;
	bool freeing; /* php_stream_free() is draining it */
} php_io_orphan;

/* Operation constructors */

static void php_io_op_init(php_io_op *op, php_io_op_type type, zend_object *handle,
		php_socket_t fd, uint32_t ready_events, php_deadline dl)
{
	memset(op, 0, sizeof(*op));
	op->type = type;
	op->handle = handle;
	op->fd = fd;
	op->ready_events = ready_events;
	op->deadline = dl;
}

PHPAPI void php_io_op_poll(php_io_op *op, zend_object *handle, php_socket_t fd, uint32_t events, php_deadline dl)
{
	php_io_op_init(op, PHP_IO_OP_POLL, handle, fd, events, dl);
	op->u.poll.events = events;
}

PHPAPI void php_io_op_timer(php_io_op *op, php_deadline dl)
{
	php_io_op_init(op, PHP_IO_OP_TIMER, NULL, SOCK_ERR, PHP_POLL_TIMER, dl);
}

PHPAPI void php_io_op_waitpid(php_io_op *op, zend_object *handle, pid_t pid, int options, int *status, php_deadline dl)
{
	php_io_op_init(op, PHP_IO_OP_WAITPID, handle, SOCK_ERR, PHP_POLL_PROCESS, dl);
	op->u.waitpid.pid = pid;
	op->u.waitpid.options = options;
	op->u.waitpid.status = status;
}

PHPAPI void php_io_op_sigwait(php_io_op *op, zend_object *handle, const php_sigset_t *set, php_siginfo_t *info, php_deadline dl)
{
	php_io_op_init(op, PHP_IO_OP_SIGWAIT, handle, SOCK_ERR, PHP_POLL_SIGNAL, dl);
	op->u.sigwait.set = set;
	op->u.sigwait.info = info;
	op->u.sigwait.taken = 0;
}

PHPAPI void php_io_op_read(php_io_op *op, zend_object *handle, php_socket_t fd, void *buf, size_t len, int64_t off, php_deadline dl)
{
	php_io_op_init(op, PHP_IO_OP_READ, handle, fd, PHP_POLL_READ, dl);
	op->u.io.buf = buf;
	op->u.io.len = len;
	op->u.io.offset = off;
}

PHPAPI void php_io_op_write(php_io_op *op, zend_object *handle, php_socket_t fd, const void *buf, size_t len, int64_t off, php_deadline dl)
{
	php_io_op_init(op, PHP_IO_OP_WRITE, handle, fd, PHP_POLL_WRITE, dl);
	op->u.io.buf = (void *) buf;
	op->u.io.len = len;
	op->u.io.offset = off;
}

PHPAPI void php_io_op_recv(php_io_op *op, zend_object *handle, php_socket_t fd, void *buf, size_t len, int flags, php_deadline dl)
{
	php_io_op_init(op, PHP_IO_OP_RECV, handle, fd, PHP_POLL_READ, dl);
	op->u.io.buf = buf;
	op->u.io.len = len;
	op->u.io.offset = -1;
	op->u.io.flags = flags;
}

PHPAPI void php_io_op_send(php_io_op *op, zend_object *handle, php_socket_t fd, const void *buf, size_t len, int flags, php_deadline dl)
{
	php_io_op_init(op, PHP_IO_OP_SEND, handle, fd, PHP_POLL_WRITE, dl);
	op->u.io.buf = (void *) buf;
	op->u.io.len = len;
	op->u.io.offset = -1;
	op->u.io.flags = flags;
}

PHPAPI void php_io_op_accept(php_io_op *op, zend_object *handle, php_socket_t fd, struct sockaddr *addr, socklen_t *addrlen, php_deadline dl)
{
	php_io_op_init(op, PHP_IO_OP_ACCEPT, handle, fd, PHP_POLL_READ, dl);
	op->u.accept.addr = addr;
	op->u.accept.addrlen = addrlen;
}

PHPAPI void php_io_op_connect(php_io_op *op, zend_object *handle, php_socket_t fd, const struct sockaddr *addr, socklen_t addrlen, php_deadline dl)
{
	php_io_op_init(op, PHP_IO_OP_CONNECT, handle, fd, PHP_POLL_WRITE, dl);
	op->u.connect.addr = addr;
	op->u.connect.addrlen = addrlen;
}

PHPAPI void php_io_op_getaddrinfo(php_io_op *op, const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res, php_deadline dl)
{
	php_io_op_init(op, PHP_IO_OP_GETADDRINFO, NULL, SOCK_ERR, 0, dl);
	op->u.getaddrinfo.node = node;
	op->u.getaddrinfo.service = service;
	op->u.getaddrinfo.hints = hints;
	op->u.getaddrinfo.res = res;
}

PHPAPI void php_io_op_getnameinfo(php_io_op *op, const struct sockaddr *addr, socklen_t addrlen, int flags, char *host, size_t hostlen, char *service, size_t servicelen, php_deadline dl)
{
	php_io_op_init(op, PHP_IO_OP_GETNAMEINFO, NULL, SOCK_ERR, 0, dl);
	op->u.getnameinfo.addr = addr;
	op->u.getnameinfo.addrlen = addrlen;
	op->u.getnameinfo.flags = flags;
	op->u.getnameinfo.host = host;
	op->u.getnameinfo.hostlen = hostlen;
	op->u.getnameinfo.service = service;
	op->u.getnameinfo.servicelen = servicelen;
}

PHPAPI void php_io_op_fsync(php_io_op *op, zend_object *handle, php_socket_t fd, bool data_only)
{
	php_io_op_init(op, PHP_IO_OP_FSYNC, handle, fd, 0, php_io_deadline_infinite());
	op->u.fsync.data_only = data_only;
}

PHPAPI void php_io_op_any(php_io_op *op, php_io_op **members, uint32_t n, php_io_op_result *results)
{
	php_io_op_init(op, PHP_IO_OP_ANY, NULL, SOCK_ERR, 0, php_io_deadline_infinite());
	op->u.any.ops = members;
	op->u.any.n = n;
	op->u.any.results = results;
	op->u.any.n_results = 0;
}

/* Registrations */

PHPAPI void (*php_io_registration_zobj_detach)(zend_object *zobj) = NULL;

PHPAPI void php_io_hooks_lock(void)
{
	FG(io_hooks_locked)++;
	zend_fiber_switch_block();
}

PHPAPI void php_io_hooks_unlock(void)
{
	zend_fiber_switch_unblock();
	FG(io_hooks_locked)--;
}

static void php_io_registration_detach_zobj(php_io_registration *reg)
{
	if (reg->zobj) {
		zend_object *zobj = reg->zobj;
		reg->zobj = NULL;
		if (php_io_registration_zobj_detach) {
			php_io_registration_zobj_detach(zobj);
		}
		OBJ_RELEASE(zobj);
	}
}

static zend_always_inline bool php_io_hooks_take_trigger(const php_io_hooks *hooks, php_io_trigger trigger)
{
	uint32_t flag = trigger == PHP_IO_TRIGGER_EDGE
			? PHP_IO_HOOKS_F_EDGE_REGISTRATIONS : PHP_IO_HOOKS_F_LEVEL_REGISTRATIONS;
	return (hooks->flags & flag) != 0;
}

static void php_io_registration_free(php_io_registration *reg)
{
	if (reg->handle) {
		OBJ_RELEASE(&reg->handle->std);
	}
	efree(reg);
}

static void php_io_registration_unlink(php_io_registration *reg)
{
	php_io_registration **link = reg->stream ? &reg->stream->io_registrations : &reg->handle->registrations;
	while (*link != reg) {
		link = &(*link)->next;
	}
	*link = reg->next;
	if (reg->gprev) {
		reg->gprev->gnext = reg->gnext;
	} else {
		FG(io_registrations) = reg->gnext;
	}
	if (reg->gnext) {
		reg->gnext->gprev = reg->gprev;
	}
}

/* The record of the pair on the registrant's list, created on the first call, and added to the
 * current provider when it has not seen it: on its first wait, and again after a replacement.
 * The trigger is settled when the record is added, as what the caller wants or Level when that
 * is all the provider takes. */
static php_io_registration *php_io_register(php_io_registration **list, php_stream *stream,
		php_poll_handle_object *handle, php_socket_t fd, uint32_t event, php_io_trigger trigger)
{
	php_io_hooks *hooks = FG(io_hooks);

	if (!hooks || fd == SOCK_ERR) {
		return NULL;
	}
	if (!php_io_hooks_take_trigger(hooks, trigger)) {
		if (trigger != PHP_IO_TRIGGER_EDGE || !php_io_hooks_take_trigger(hooks, PHP_IO_TRIGGER_LEVEL)) {
			return NULL;
		}
		trigger = PHP_IO_TRIGGER_LEVEL;
	}

	php_io_registration *reg;
	for (reg = *list; reg; reg = reg->next) {
		if (reg->event == event) {
			break;
		}
	}
	if (!reg) {
		reg = ecalloc(1, sizeof(*reg));
		reg->fd = fd;
		reg->event = event;
		reg->stream = stream;
		reg->handle = handle;
		if (handle) {
			GC_ADDREF(&handle->std);
		}
		reg->next = *list;
		*list = reg;
		reg->gnext = FG(io_registrations);
		if (reg->gnext) {
			reg->gnext->gprev = reg;
		}
		FG(io_registrations) = reg;
	}
	if (reg->generation == FG(io_hooks_generation)) {
		return reg;
	}

	reg->trigger = trigger;
	reg->generation = FG(io_hooks_generation);
	if (hooks->ops->add) {
		/* An add() that unregisters the pair (closes the stream) leaves the record to this frame */
		reg->in_add = true;
		php_io_hooks_lock();
		hooks->ops->add(hooks, reg);
		php_io_hooks_unlock();
		reg->in_add = false;
		if (reg->dead) {
			php_io_registration_free(reg);
			return NULL;
		}
	}
	return reg;
}

PHPAPI php_io_registration *php_io_register_stream(php_stream *stream, php_socket_t fd, uint32_t event,
		php_io_trigger trigger)
{
	return php_io_register(&stream->io_registrations, stream, NULL, fd, event, trigger);
}

PHPAPI php_io_registration *php_io_register_handle(php_poll_handle_object *handle, uint32_t event, php_io_trigger trigger)
{
	return php_io_register(&handle->registrations, NULL, handle, php_poll_handle_get_fd(handle), event, trigger);
}

PHPAPI void php_io_unregister(php_io_registration *reg)
{
	php_io_hooks *hooks = FG(io_hooks);

	php_io_registration_unlink(reg);
	if (hooks && reg->generation == FG(io_hooks_generation) && hooks->ops->remove) {
		php_io_hooks_lock();
		hooks->ops->remove(hooks, reg);
		php_io_hooks_unlock();
	}
	php_io_registration_detach_zobj(reg);
	if (reg->in_add) {
		reg->dead = true;
		return;
	}
	php_io_registration_free(reg);
}

PHPAPI void php_io_unregister_all(php_io_registration **list)
{
	while (*list) {
		php_io_unregister(*list);
	}
}

PHPAPI php_poll_handle_object *php_io_registration_get_handle(php_io_registration *reg)
{
	if (!reg->handle && reg->stream) {
		zend_object *handle = php_stream_get_poll_handle(reg->stream, reg->stream->userland);
		GC_ADDREF(handle);
		reg->handle = PHP_POLL_HANDLE_OBJ_FROM_ZOBJ(handle);
	}
	return reg->handle;
}

PHPAPI zend_object *php_io_op_get_handle(php_io_op *op)
{
	if (!op->handle && op->stream) {
		op->handle = php_stream_get_poll_handle(op->stream, op->stream->userland);
	}
	return op->handle;
}

/* Hooks */

PHPAPI zend_result php_io_hooks_register(php_io_hooks *hooks)
{
	php_io_hooks *current = FG(io_hooks);

	/* Not from add, remove or a dtor of the provider being replaced */
	if (FG(io_hooks_locked)) {
		return FAILURE;
	}

	if (hooks == NULL) {
		if (current) {
			FG(io_hooks) = NULL;
			/* Its registrations end without remove(): the next provider sees add() again */
			for (php_io_registration *reg = FG(io_registrations); reg; reg = reg->gnext) {
				php_io_registration_detach_zobj(reg);
			}
			if (current->ops->dtor) {
				php_io_hooks_lock();
				current->ops->dtor(current);
				php_io_hooks_unlock();
			}
		}
		return SUCCESS;
	}

	if (current) {
		return FAILURE;
	}

	FG(io_hooks_generation)++;
	FG(io_hooks) = hooks;
	return SUCCESS;
}

PHPAPI php_io_hooks *php_io_hooks_current(void)
{
	return FG(io_hooks);
}

PHPAPI bool php_io_hooks_active(void)
{
	return FG(io_hooks) != NULL;
}

/* A bailout while an op was suspended skipped its frame end */
static void php_io_unfreeze_list(HashTable *list)
{
	zend_resource *res;
	ZEND_HASH_FOREACH_PTR(list, res) {
		if (res->type == php_file_le_stream() || res->type == php_file_le_pstream()) {
			((php_stream *) res->ptr)->flags &= ~PHP_STREAM_FLAG_IN_USE;
		}
	} ZEND_HASH_FOREACH_END();
}

PHPAPI void php_io_hooks_request_shutdown(void)
{
	FG(io_hooks_locked) = 0;
	php_io_hooks_register(NULL);
	/* A persistent stream outlives the request; its records must not */
	while (FG(io_registrations)) {
		php_io_unregister(FG(io_registrations));
	}
	FG(io_shut_down) = true;
	if (FG(io_queue)) {
		php_io_queue *q = FG(io_queue);
		FG(io_queue) = NULL;
		q->ops->destroy(q);
	}
	if (FG(io_orphans)) {
		/* A queue that outlived the provider (an object held elsewhere)
		 * still owns the buffers of its orphans: settle them before the
		 * resource list closes the streams, which happens before the
		 * object store frees the queue */
		while (zend_hash_num_elements(FG(io_orphans)) > 0) {
			zend_hash_internal_pointer_reset(FG(io_orphans));
			php_io_orphan *o = zend_hash_get_current_data_ptr(FG(io_orphans));
			php_stream *stream = o->stream;
			if (o->queue->ops->drain) {
				o->queue->ops->drain(o->queue, stream);
			}
			php_io_stream_unfreeze(stream);
		}
		zend_hash_destroy(FG(io_orphans));
		efree(FG(io_orphans));
		FG(io_orphans) = NULL;
	}
	if (FG(io_ops_in_flight)) {
		php_io_unfreeze_list(&EG(regular_list));
		php_io_unfreeze_list(&EG(persistent_list));
		FG(io_ops_in_flight) = 0;
	}
	if (FG(io_reaped)) {
		zend_hash_destroy(FG(io_reaped));
		efree(FG(io_reaped));
		FG(io_reaped) = NULL;
	}
	if (FG(io_addrinfo)) {
		struct addrinfo *head;
		ZEND_HASH_FOREACH_PTR(FG(io_addrinfo), head) {
			php_io_addrinfo_free_list(head);
		} ZEND_HASH_FOREACH_END();
		zend_hash_destroy(FG(io_addrinfo));
		efree(FG(io_addrinfo));
		FG(io_addrinfo) = NULL;
	}
}

PHPAPI void php_io_addrinfo_free_list(struct addrinfo *head)
{
	while (head) {
		struct addrinfo *next = head->ai_next;
		free(head);
		head = next;
	}
}

PHPAPI void php_io_addrinfo_register(struct addrinfo *head)
{
	if (!FG(io_addrinfo)) {
		FG(io_addrinfo) = emalloc(sizeof(HashTable));
		zend_hash_init(FG(io_addrinfo), 4, NULL, NULL, 0);
	}
	zend_hash_index_add_new_ptr(FG(io_addrinfo), (zend_ulong) (uintptr_t) head, head);
}

PHPAPI void php_io_freeaddrinfo(struct addrinfo *res)
{
	if (!res) {
		return;
	}
	if (FG(io_addrinfo) && zend_hash_index_del(FG(io_addrinfo), (zend_ulong) (uintptr_t) res) == SUCCESS) {
		php_io_addrinfo_free_list(res);
		return;
	}
	freeaddrinfo(res);
}

typedef struct {
	int status;
	pid_t pgid;
} php_io_reaped_child;

static void php_io_reaped_child_dtor(zval *zv)
{
	efree(Z_PTR_P(zv));
}

PHPAPI void php_io_child_reaped_ex(pid_t pid, pid_t pgid, int status)
{
	if (!FG(io_reaped)) {
		FG(io_reaped) = emalloc(sizeof(HashTable));
		zend_hash_init(FG(io_reaped), 4, NULL, php_io_reaped_child_dtor, 0);
	}
	php_io_reaped_child *child = emalloc(sizeof(*child));
	child->status = status;
	child->pgid = pgid;
	zend_hash_index_update_ptr(FG(io_reaped), (zend_ulong) pid, child);
}

PHPAPI void php_io_child_reaped(pid_t pid, int status)
{
	php_io_child_reaped_ex(pid, 0, status);
}

/* pid selects as waitpid() does: itself, -1 any child, 0 the caller's
 * process group, < -1 the group -pid. A child whose group is unknown is
 * only taken by pid or by -1. */
PHPAPI bool php_io_child_take_reaped(pid_t *pid, int *status)
{
	if (!FG(io_reaped) || zend_hash_num_elements(FG(io_reaped)) == 0) {
		return false;
	}
	php_io_reaped_child *child = NULL;
	zend_ulong key = 0;
	if (*pid > 0) {
		key = (zend_ulong) *pid;
		child = zend_hash_index_find_ptr(FG(io_reaped), key);
	} else {
#ifndef PHP_WIN32
		pid_t pgid = *pid == 0 ? getpgrp() : -*pid;
#else
		pid_t pgid = -*pid;
#endif
		php_io_reaped_child *c;
		ZEND_HASH_FOREACH_NUM_KEY_PTR(FG(io_reaped), key, c) {
			if (*pid == -1 || (c->pgid > 0 && c->pgid == pgid)) {
				child = c;
				break;
			}
		} ZEND_HASH_FOREACH_END();
	}
	if (!child) {
		return false;
	}
	*pid = (pid_t) key;
	*status = child->status;
	zend_hash_index_del(FG(io_reaped), key);
	return true;
}

PHPAPI void php_io_child_forget(pid_t pid)
{
#ifdef HAVE_IOR
	if (pid == 0) {
		php_io_ring_after_fork();
	}
#endif
	if (!FG(io_reaped)) {
		return;
	}
	if (pid == 0) {
		zend_hash_clean(FG(io_reaped));
	} else if (pid > 0) {
		zend_hash_index_del(FG(io_reaped), (zend_ulong) pid);
	}
}

PHPAPI uint32_t php_io_ops_in_flight(void)
{
	return FG(io_ops_in_flight);
}

/* Orphans */

static zend_always_inline zend_ulong php_io_stream_key(php_stream *stream)
{
	zend_ulong key = (zend_ulong) (uintptr_t) stream;
	return (key >> 3) | (key << ((sizeof(key) * 8) - 3));
}


static void php_io_orphan_dtor(zval *zv)
{
	efree(Z_PTR_P(zv));
}

PHPAPI void php_io_stream_orphan(php_stream *stream, php_io_queue *queue)
{
	if (!FG(io_orphans)) {
		FG(io_orphans) = emalloc(sizeof(HashTable));
		zend_hash_init(FG(io_orphans), 4, NULL, php_io_orphan_dtor, 0);
	}
	if (!zend_hash_index_exists(FG(io_orphans), php_io_stream_key(stream))) {
		php_io_orphan *o = emalloc(sizeof(*o));
		o->queue = queue;
		o->stream = stream;
		o->freeing = false;
		/* The buffer belongs to the backend until the completion arrives */
		GC_ADDREF(stream->res);
		zend_hash_index_add_new_ptr(FG(io_orphans), php_io_stream_key(stream), o);
	}
}

PHPAPI void php_io_stream_unfreeze(php_stream *stream)
{
	if (!FG(io_orphans)) {
		return;
	}
	php_io_orphan *o = zend_hash_index_find_ptr(FG(io_orphans), php_io_stream_key(stream));
	if (!o) {
		return;
	}
	bool freeing = o->freeing;
	zend_hash_index_del(FG(io_orphans), php_io_stream_key(stream));
	stream->flags &= ~PHP_STREAM_FLAG_IN_USE;
	if (!freeing) {
		zend_list_delete(stream->res);
	}
}

PHPAPI bool php_io_stream_busy(php_stream *stream)
{
	return (stream->flags & PHP_STREAM_FLAG_IN_USE)
			&& !(FG(io_orphans) && zend_hash_index_exists(FG(io_orphans), php_io_stream_key(stream)));
}

/* Called from php_stream_free() with the stream still frozen */
PHPAPI void php_io_stream_drain(php_stream *stream)
{
	if (!FG(io_orphans)) {
		return;
	}
	php_io_orphan *o = zend_hash_index_find_ptr(FG(io_orphans), php_io_stream_key(stream));
	if (!o) {
		return;
	}
	o->freeing = true;
	if (o->queue->ops->drain) {
		o->queue->ops->drain(o->queue, stream);
	}
	php_io_stream_unfreeze(stream);
}

/* Entry point */

static void php_io_op_detach_zobj(php_io_op *op)
{
	if (op->zobj) {
		zend_object *zobj = op->zobj;
		op->zobj = NULL;
		if (php_io_op_zobj_detach) {
			php_io_op_zobj_detach(zobj);
		}
		OBJ_RELEASE(zobj);
	}
}

/* The op is over from the caller's point of view: make sure no queue still
 * references it and invalidate every userland wrapper. */
static void php_io_op_finish(php_io_op *op)
{
	if (op->queue) {
		op->queue->ops->orphan(op->queue, op);
	}
	if (op->type == PHP_IO_OP_ANY) {
		for (uint32_t i = 0; i < op->u.any.n; i++) {
			php_io_op *m = op->u.any.ops[i];
			/* A member the provider submitted on its own */
			if (m->queue) {
				m->queue->ops->orphan(m->queue, m);
			}
			php_io_op_detach_zobj(m);
		}
	}
	php_io_op_detach_zobj(op);
}

static php_io_queue *php_io_core_queue(void)
{
#ifndef PHP_WIN32
	if (FG(io_queue) && FG(io_queue_pid) != getpid()) {
		/* Inherited across fork: its context is inert here (nothing was in
		 * flight, the fork guard saw to that), so it is dropped for a new one */
		php_io_queue *q = FG(io_queue);
		FG(io_queue) = NULL;
		q->ops->destroy(q);
	}
#endif
	if (!FG(io_queue)) {
		/* poll(2) handles regular files and needs no registration syscalls for
		 * the one-shot waits of the synchronous path */
		FG(io_queue) = php_io_queue_create_poll(PHP_POLL_BACKEND_POLL);
		if (!FG(io_queue)) {
			FG(io_queue) = php_io_queue_create_poll(PHP_POLL_BACKEND_AUTO);
		}
#ifndef PHP_WIN32
		FG(io_queue_pid) = getpid();
#endif
	}
	return FG(io_queue);
}

static zend_result php_io_run_sync_timer(php_io_op *op, php_io_op_result *result)
{
	bool infinite = php_deadline_is_infinite(&op->deadline);

	result->index = 0;
	result->res = 0;
	result->error = 0;
	result->status = PHP_IO_DONE;

	for (;;) {
		zend_hrtime_t remaining = infinite ? ZEND_HRTIME_T_MAX : php_io_deadline_remaining(&op->deadline, zend_hrtime());
		if (remaining == 0) {
			break;
		}
#ifdef PHP_WIN32
		zend_hrtime_t ms = (remaining + 999999) / 1000000;
		Sleep(ms >= INFINITE ? INFINITE - 1 : (DWORD) ms);
#else
		zend_hrtime_t sec = remaining / ZEND_NANO_IN_SEC;
		struct timespec ts = {
			.tv_sec = sec > INT_MAX ? INT_MAX : (time_t) sec,
			.tv_nsec = sec > INT_MAX ? 0 : (long) (remaining % ZEND_NANO_IN_SEC),
		};
		if (nanosleep(&ts, NULL) != 0) {
			if (errno == EINTR) {
				result->status = PHP_IO_INTERRUPTED;
			}
			break;
		}
#endif
	}

	return SUCCESS;
}

/* After the request shutdown destroyed the core queue: streams closed later
 * wait with a plain poll(2) rather than creating a queue nobody frees */
static zend_result php_io_run_sync_direct(php_io_op *op, php_io_op_result *result)
{
	uint32_t events = op->type == PHP_IO_OP_POLL ? op->u.poll.events : op->ready_events;
	int pevents = ((events & PHP_POLL_READ) ? POLLIN : 0) | ((events & PHP_POLL_WRITE) ? POLLOUT : 0)
			| ((events & PHP_POLL_PRI) ? POLLPRI : 0);

	result->index = 0;
	result->res = 0;
	result->error = 0;

	if (op->fd == SOCK_ERR || op->type == PHP_IO_OP_ANY || !pevents) {
		result->status = PHP_IO_UNSUPPORTED;
		return SUCCESS;
	}

	int n;
	for (;;) {
		int timeout = -1;
		if (!php_deadline_is_infinite(&op->deadline)) {
			zend_hrtime_t ms = (php_io_deadline_remaining(&op->deadline, zend_hrtime()) + 999999) / 1000000;
			timeout = ms > INT_MAX ? INT_MAX : (int) ms;
		}
		n = php_pollfd_for_ms(op->fd, pevents, timeout);
		if (n >= 0 || php_socket_errno() != EINTR) {
			break;
		}
	}

	if (n < 0) {
		result->status = PHP_IO_DONE;
		result->res = -1;
		result->error = php_socket_errno();
	} else if (n == 0) {
		result->status = PHP_IO_TIMEOUT;
	} else {
		uint32_t revents = ((n & POLLIN) ? PHP_POLL_READ : 0) | ((n & POLLOUT) ? PHP_POLL_WRITE : 0)
				| ((n & POLLPRI) ? PHP_POLL_PRI : 0) | ((n & (POLLERR | POLLNVAL)) ? PHP_POLL_ERROR : 0)
				| ((n & POLLHUP) ? PHP_POLL_HUP : 0);
		result->status = op->type == PHP_IO_OP_POLL ? PHP_IO_DONE : PHP_IO_READY;
		result->res = revents;
	}
	return SUCCESS;
}

static zend_result php_io_run_sync(php_io_op *op, php_io_op_result *result)
{
	if (op->type == PHP_IO_OP_TIMER) {
		return php_io_run_sync_timer(op, result);
	}
	if (FG(io_shut_down)) {
		return php_io_run_sync_direct(op, result);
	}

	php_io_queue *q = php_io_core_queue();

	result->index = 0;
	result->res = -1;
	result->error = 0;

	if (!q) {
		result->status = PHP_IO_DONE;
		result->error = ENOMEM;
		return SUCCESS;
	}

	if (q->ops->submit(q, op, NULL) == FAILURE) {
		result->status = PHP_IO_DONE;
		result->error = errno ? errno : EINVAL;
		return SUCCESS;
	}

	php_io_queue_completion c;
	int n;
	/* Without a provider a blocking op keeps waiting through signals, as
	 * the poll loops it replaced did; the handler runs when it returns */
	do {
		n = q->ops->wait(q, &c, 1, NULL);
	} while (n == 0 || (n < 0 && errno == EINTR));

	if (n < 0) {
		int err = errno;
		if (op->queue) {
			q->ops->orphan(q, op);
		}
		result->status = PHP_IO_DONE;
		result->error = err ? err : EIO;
		return SUCCESS;
	}

	ZEND_ASSERT(c.op == op);
	*result = c.result;
	return SUCCESS;
}

static zend_result php_io_run_ex(php_io_op *op, php_io_op_result *result);

PHPAPI zend_result php_io_run(php_io_op *op, php_io_op_result *result)
{
	FG(io_ops_in_flight)++;
	zend_result rc = php_io_run_ex(op, result);
	/* Reset by the request shutdown after a bailout abandoned frames */
	if (FG(io_ops_in_flight)) {
		FG(io_ops_in_flight)--;
	}
	return rc;
}

/* What a provider may leave behind must never be read uninitialized */
static void php_io_op_clear_outputs(php_io_op *op)
{
	switch (op->type) {
		case PHP_IO_OP_GETADDRINFO:
			*op->u.getaddrinfo.res = NULL;
			break;
		case PHP_IO_OP_GETNAMEINFO:
			if (op->u.getnameinfo.host && op->u.getnameinfo.hostlen) {
				op->u.getnameinfo.host[0] = '\0';
			}
			if (op->u.getnameinfo.service && op->u.getnameinfo.servicelen) {
				op->u.getnameinfo.service[0] = '\0';
			}
			break;
		case PHP_IO_OP_WAITPID:
			if (op->u.waitpid.status) {
				*op->u.waitpid.status = 0;
			}
			break;
		case PHP_IO_OP_SIGWAIT:
			if (op->u.sigwait.info) {
				memset(op->u.sigwait.info, 0, sizeof(*op->u.sigwait.info));
			}
			break;
		default:
			break;
	}
}

/* A successful Done must fit the op: no more bytes than the buffer holds */
static bool php_io_result_valid(const php_io_op *op, const php_io_op_result *result)
{
	if (result->status != PHP_IO_DONE || result->error) {
		return true;
	}
	switch (op->type) {
		case PHP_IO_OP_READ:
		case PHP_IO_OP_WRITE:
		case PHP_IO_OP_RECV:
		case PHP_IO_OP_SEND:
			return result->res >= 0 && (uint64_t) result->res <= op->u.io.len;
		case PHP_IO_OP_ACCEPT:
		case PHP_IO_OP_WAITPID:
		case PHP_IO_OP_SIGWAIT:
			return result->res > 0;
		case PHP_IO_OP_CONNECT:
		case PHP_IO_OP_FSYNC:
			return result->res >= 0;
		case PHP_IO_OP_GETADDRINFO:
			return *op->u.getaddrinfo.res != NULL;
		default:
			return true;
	}
}

static zend_result php_io_run_ex(php_io_op *op, php_io_op_result *result)
{
	php_io_hooks *hooks = FG(io_hooks);
	if (hooks) {
		php_io_op_clear_outputs(op);
		result->status = PHP_IO_UNSUPPORTED;
		result->index = 0;
		result->res = 0;
		result->error = 0;
		zend_result rc = hooks->ops->run(hooks, op, result);
		php_io_op_finish(op);

		if (rc == SUCCESS && !php_io_result_valid(op, result)) {
			if (op->type == PHP_IO_OP_GETADDRINFO) {
				result->error = EAI_FAIL;
				result->res = -1;
			} else {
				zend_throw_error(NULL, "The IO provider completed an operation with an invalid result");
				rc = FAILURE;
			}
		}
		if (rc == FAILURE) {
			result->status = PHP_IO_CANCELLED;
			result->res = -1;
			result->error = ECANCELED;
			return FAILURE;
		}
		if (result->status != PHP_IO_UNSUPPORTED) {
			return SUCCESS;
		}
	}

	zend_result rc = php_io_run_sync(op, result);
	php_io_op_finish(op);
	return rc;
}

/* Wrappers */

static int php_io_poll_result_to_revents(const php_io_op_result *result, uint32_t events)
{
	switch (result->status) {
		case PHP_IO_DONE:
		case PHP_IO_READY:
			if (result->error) {
				php_io_set_errno(result->error);
				return -1;
			}
			return result->res > 0 && result->res <= INT_MAX ? (int) result->res : (int) events;
		case PHP_IO_TIMEOUT:
			php_io_set_errno(ETIMEDOUT);
			return 0;
		case PHP_IO_INTERRUPTED:
			php_io_set_errno(EINTR);
			return -1;
		case PHP_IO_CANCELLED:
			php_io_set_errno(ECANCELED);
			return -1;
		case PHP_IO_UNSUPPORTED:
		default:
			php_io_set_errno(ENOTSUP);
			return -1;
	}
}

/* The stream is frozen for the duration; a queue keeping the op past this
 * frame (the ring on an abnormal exit) keeps it frozen until the op settled */
typedef struct {
	php_stream *stream;
	zend_resource *res;
} php_io_frame;

/* Fails with the Error of argument parsing when the stream is frozen by an
 * op of another flow: internal consumers reach streams without it */
static zend_result php_io_frame_begin(php_io_frame *f, php_stream *stream)
{
	f->stream = stream;
	f->res = NULL;
	if (stream) {
		if (UNEXPECTED(stream->flags & PHP_STREAM_FLAG_IN_USE)) {
			f->stream = NULL;
			zend_throw_error(NULL, "Concurrent access to a stream");
			php_io_set_errno(ECANCELED);
			return FAILURE;
		}
		/* No resource release can free the stream under the op */
		if (stream->res) {
			f->res = stream->res;
			GC_ADDREF(f->res);
		}
		stream->flags |= PHP_STREAM_FLAG_IN_USE;
	}
	return SUCCESS;
}

static void php_io_frame_end(php_io_frame *f, php_io_op *op)
{
	if (f->stream && !(op && op->in_flight)) {
		f->stream->flags &= ~PHP_STREAM_FLAG_IN_USE;
	}
	/* A last reference dropped meanwhile leaves the stream to the request
	 * shutdown: the stream layer above still uses it */
	if (f->res) {
		GC_DELREF(f->res);
	}
}

static zend_always_inline uint32_t php_io_hook_flags(void)
{
	return FG(io_hooks) ? FG(io_hooks)->flags : 0;
}

/* A wait on a stream's pair that reaches the provider registers the pair, as Edge or as Level when
 * that is all the provider takes; an add() that threw cancels the wait */
static zend_result php_io_op_register_wait(php_io_op *op, php_stream *stream, uint32_t event)
{
	if (!stream || !FG(io_hooks) || (stream->flags & PHP_STREAM_FLAG_NO_IO_REGISTRATION)
			|| !(event == PHP_POLL_READ || event == PHP_POLL_WRITE)) {
		return SUCCESS;
	}
	zend_object *pending = EG(exception);
	op->registration = php_io_register_stream(stream, op->fd, event, PHP_IO_TRIGGER_EDGE);
	if (EG(exception) != pending) {
		php_io_set_errno(ECANCELED);
		return FAILURE;
	}
	return SUCCESS;
}

static int php_io_poll_ex(php_stream *stream, php_socket_t fd, uint32_t events, php_deadline *dl,
		uint32_t op_flags)
{
	php_io_op op;
	php_io_op_result result;
	php_io_frame f;

	if (php_io_frame_begin(&f, stream) == FAILURE) {
		return -1;
	}
	php_io_op_poll(&op, NULL, fd, events, *dl);
	op.flags |= op_flags;
	op.stream = stream;
	zend_result rc = php_io_op_register_wait(&op, stream, events);
	if (rc == SUCCESS) {
		rc = php_io_run(&op, &result);
	}
	php_io_frame_end(&f, &op);

	if (rc == FAILURE) {
		php_io_set_errno(ECANCELED);
		return -1;
	}
	return php_io_poll_result_to_revents(&result, events);
}

PHPAPI int php_io_poll(php_stream *stream, php_socket_t fd, uint32_t events, php_deadline *dl)
{
	return php_io_poll_ex(stream, fd, events, dl, 0);
}

/* Status to a syscall-like return for a data op; true when the caller is done */
static bool php_io_data_result(const php_io_op_result *result, ssize_t *ret)
{
	switch (result->status) {
		case PHP_IO_DONE:
			if (result->error) {
				php_io_set_errno(result->error);
				*ret = -1;
			} else {
				*ret = (ssize_t) result->res;
			}
			return true;
		case PHP_IO_TIMEOUT:
			php_io_set_errno(ETIMEDOUT);
			*ret = -1;
			return true;
		case PHP_IO_INTERRUPTED:
			php_io_set_errno(EINTR);
			*ret = -1;
			return true;
		case PHP_IO_CANCELLED:
			php_io_set_errno(ECANCELED);
			*ret = -1;
			return true;
		case PHP_IO_READY:
		case PHP_IO_UNSUPPORTED:
		default:
			return false;
	}
}

/* The arguments of a socket call made through the ladders below */
typedef struct {
	php_socket_t fd;
	void *buf;
	size_t len;
	int flags;
	struct sockaddr *addr;
	socklen_t *addrlen;
	socklen_t addrlen_in;
} php_io_sock_call;

typedef ssize_t (*php_io_sock_syscall)(const php_io_sock_call *c);
typedef void (*php_io_sock_prep)(php_io_op *op, php_stream *stream, const php_io_sock_call *c,
		php_deadline dl);

/* The descriptor ladder: the syscall first and the op on EAGAIN, or the op first with the direct
 * flag; Ready means retry the syscall, Unsupported means syscall first from now on. */
static zend_always_inline ssize_t php_io_descriptor_op(php_stream *stream, php_deadline *dl,
		uint32_t direct_flag, php_io_sock_syscall syscall_fn, php_io_sock_prep prep,
		const php_io_sock_call *c)
{
	php_io_frame f;
	php_io_op op;
	php_io_op_result result;
	ssize_t ret;
	bool direct = (php_io_hook_flags() & direct_flag) != 0;
	bool waited = false;

	if (php_io_frame_begin(&f, stream) == FAILURE) {
		return -1;
	}
	memset(&op, 0, sizeof(op));
	for (;;) {
		if (!direct) {
			ret = syscall_fn(c);
			if (ret >= 0 || !PHP_IS_TRANSIENT_ERROR(php_socket_errno())) {
				break;
			}
			if (waited && dl->hrtime == 0) {
				/* A non-blocking deadline gets one readiness check */
				break;
			}
		}
		prep(&op, stream, c, *dl);
		op.stream = stream;
		if (!direct) {
			op.flags |= PHP_IO_OP_F_AFTER_DRAIN;
		}
		if (php_io_op_register_wait(&op, stream, op.ready_events) == FAILURE
				|| php_io_run(&op, &result) == FAILURE) {
			php_io_set_errno(ECANCELED);
			ret = -1;
			break;
		}
		if (php_io_data_result(&result, &ret)) {
			break;
		}
		waited = true;
		if (result.status == PHP_IO_UNSUPPORTED && !direct) {
			php_io_set_errno(ENOTSUP);
			ret = -1;
			break;
		}
		direct = false;
	}
	php_io_frame_end(&f, &op);
	return ret;
}

/* Where _php_stream_fill_read_buffer() reads: memory the stream owns and
 * frees only after its orphans were drained. A queue may leave an op on it
 * past the frame; any other buffer is the caller's. */
static zend_always_inline uint32_t php_io_stream_buf_flag(php_stream *stream, const void *buf)
{
	return stream && stream->readbuf && buf == stream->readbuf + stream->writepos ? PHP_IO_OP_F_STREAM_BUF : 0;
}

#ifdef PHP_WIN32
# define PHP_IO_SOCKLEN(n) ((int) (n))
#else
# define PHP_IO_SOCKLEN(n) (n)
#endif

static ssize_t php_io_recv_syscall(const php_io_sock_call *c)
{
	return recv(c->fd, c->buf, c->len, c->flags);
}

static void php_io_recv_prep(php_io_op *op, php_stream *stream, const php_io_sock_call *c,
		php_deadline dl)
{
	php_io_op_recv(op, NULL, c->fd, c->buf, c->len, c->flags, dl);
	op->flags |= php_io_stream_buf_flag(stream, c->buf);
}

PHPAPI ssize_t php_io_recv(php_stream *stream, php_socket_t fd, void *buf, size_t len, int flags,
		php_deadline *dl)
{
	php_io_sock_call c = { .fd = fd, .buf = buf, .len = len, .flags = flags };
	return php_io_descriptor_op(stream, dl, PHP_IO_HOOKS_F_DIRECT_DATA, php_io_recv_syscall, php_io_recv_prep, &c);
}

static ssize_t php_io_send_syscall(const php_io_sock_call *c)
{
	return send(c->fd, c->buf, c->len, c->flags);
}

static void php_io_send_prep(php_io_op *op, php_stream *stream, const php_io_sock_call *c,
		php_deadline dl)
{
	php_io_op_send(op, NULL, c->fd, c->buf, c->len, c->flags, dl);
}

PHPAPI ssize_t php_io_send(php_stream *stream, php_socket_t fd, const void *buf, size_t len, int flags,
		php_deadline *dl)
{
	php_io_sock_call c = { .fd = fd, .buf = (void *) buf, .len = len, .flags = flags };
	return php_io_descriptor_op(stream, dl, PHP_IO_HOOKS_F_DIRECT_DATA, php_io_send_syscall, php_io_send_prep, &c);
}

/* The readiness form of the ladder, for calls without a data op: a Poll op on EAGAIN, bounded by
 * the caller's deadline; a non-blocking deadline gets one readiness check. */
static zend_always_inline ssize_t php_io_readiness_op(php_stream *stream, uint32_t events,
		php_deadline *dl, php_io_sock_syscall syscall_fn, const php_io_sock_call *c)
{
	ssize_t ret;
	bool waited = false;

	for (;;) {
		ret = syscall_fn(c);
		if (ret >= 0 || !dl || !PHP_IS_TRANSIENT_ERROR(php_socket_errno())) {
			break;
		}
		if (waited && dl->hrtime == 0) {
			break;
		}
		if (php_io_poll_ex(stream, c->fd, events, dl, PHP_IO_OP_F_AFTER_DRAIN) <= 0) {
			/* errno: ETIMEDOUT, ECANCELED or the failure */
			ret = -1;
			break;
		}
		waited = true;
	}
	return ret;
}

static ssize_t php_io_sendto_syscall(const php_io_sock_call *c)
{
	if (c->addr) {
		return sendto(c->fd, c->buf, PHP_IO_SOCKLEN(c->len), c->flags, c->addr, PHP_IO_SOCKLEN(c->addrlen_in));
	}
	return send(c->fd, c->buf, PHP_IO_SOCKLEN(c->len), c->flags);
}

PHPAPI ssize_t php_io_sendto(php_stream *stream, php_socket_t fd, const void *buf, size_t len, int flags,
		const struct sockaddr *addr, socklen_t addrlen, php_deadline *dl)
{
	php_io_sock_call c = { .fd = fd, .buf = (void *) buf, .len = len, .flags = flags,
		.addr = (struct sockaddr *) addr, .addrlen_in = addrlen };
	return php_io_readiness_op(stream, PHP_POLL_WRITE, dl, php_io_sendto_syscall, &c);
}

static ssize_t php_io_recvfrom_syscall(const php_io_sock_call *c)
{
	if (c->addr) {
		return recvfrom(c->fd, c->buf, PHP_IO_SOCKLEN(c->len), c->flags, c->addr, c->addrlen);
	}
	return recv(c->fd, c->buf, PHP_IO_SOCKLEN(c->len), c->flags);
}

PHPAPI ssize_t php_io_recvfrom(php_stream *stream, php_socket_t fd, void *buf, size_t len, int flags,
		struct sockaddr *addr, socklen_t *addrlen, php_deadline *dl)
{
	php_io_sock_call c = { .fd = fd, .buf = buf, .len = len, .flags = flags, .addr = addr, .addrlen = addrlen };
	return php_io_readiness_op(stream, PHP_POLL_READ, dl, php_io_recvfrom_syscall, &c);
}

static ssize_t php_io_accept_syscall(const php_io_sock_call *c)
{
	return (ssize_t) accept(c->fd, c->addr, c->addrlen);
}

static void php_io_accept_prep(php_io_op *op, php_stream *stream, const php_io_sock_call *c,
		php_deadline dl)
{
	php_io_op_accept(op, NULL, c->fd, c->addr, c->addrlen, dl);
}

PHPAPI php_socket_t php_io_accept(php_stream *stream, php_socket_t fd, struct sockaddr *addr,
		socklen_t *addrlen, php_deadline *dl)
{
	php_io_sock_call c = { .fd = fd, .addr = addr, .addrlen = addrlen };
	return php_io_descriptor_op(stream, dl, PHP_IO_HOOKS_F_DIRECT_ACCEPT, php_io_accept_syscall, php_io_accept_prep, &c);
}

/* A non-blocking connect that is under way: EINPROGRESS, EAGAIN on some
 * systems, WSAEWOULDBLOCK on Windows (where EINPROGRESS is defined as it),
 * EALREADY for one started before */
#ifdef PHP_WIN32
# define PHP_IO_IS_EALREADY(err) ((err) == EALREADY || (err) == WSAEALREADY)
#else
# define PHP_IO_IS_EALREADY(err) ((err) == EALREADY)
#endif
#define PHP_IO_CONNECT_PENDING(err) ((err) == EINPROGRESS || (err) == EAGAIN || (err) == EWOULDBLOCK || PHP_IO_IS_EALREADY(err))

/* The connect is started once; the wait completes as Ready and the result
 * is read from SO_ERROR, or as Done when the provider connected itself */
PHPAPI int php_io_connect(php_stream *stream, php_socket_t fd, const struct sockaddr *addr, socklen_t addrlen, php_deadline *dl)
{
	php_io_frame f;
	php_io_op op;
	php_io_op_result result;
	int ret = 0;
	bool direct = (php_io_hook_flags() & PHP_IO_HOOKS_F_DIRECT_DATA) != 0;
	bool started = false; /* our own connect() is in progress */

	if (php_io_frame_begin(&f, stream) == FAILURE) {
		return -1;
	}
	memset(&op, 0, sizeof(op));

	if (!direct) {
		if (connect(fd, addr, addrlen) == 0) {
			goto out;
		}
		if (!PHP_IO_CONNECT_PENDING(php_socket_errno())) {
			ret = -1;
			goto out;
		}
		started = true;
	}

	for (;;) {
		php_io_op_connect(&op, NULL, fd, addr, addrlen, *dl);
		op.stream = stream;
		if (started) {
			op.flags |= PHP_IO_OP_F_AFTER_DRAIN;
		}
		if (php_io_op_register_wait(&op, stream, PHP_POLL_WRITE) == FAILURE
				|| php_io_run(&op, &result) == FAILURE) {
			php_io_set_errno(ECANCELED);
			ret = -1;
			break;
		}
		if ((result.status == PHP_IO_READY && !started)
				|| (result.status == PHP_IO_UNSUPPORTED && direct)) {
			/* The provider only waited, or does not connect: start it
			 * ourselves and wait for writability */
			direct = false;
			if (connect(fd, addr, addrlen) == 0) {
				break;
			}
			if (!PHP_IO_CONNECT_PENDING(php_socket_errno())) {
				ret = -1;
				break;
			}
			started = true;
			continue;
		}
		if (started && result.status == PHP_IO_DONE && result.res < 0 && PHP_IO_IS_EALREADY(result.error)) {
			/* A provider that performs the op found our connect still under
			 * way: wait for its outcome like after a readiness report */
			php_io_op_poll(&op, NULL, fd, PHP_POLL_WRITE, *dl);
			op.flags |= PHP_IO_OP_F_AFTER_DRAIN;
			op.stream = stream;
			if (php_io_op_register_wait(&op, stream, PHP_POLL_WRITE) == FAILURE
					|| php_io_run(&op, &result) == FAILURE) {
				php_io_set_errno(ECANCELED);
				ret = -1;
				break;
			}
			int n = php_io_poll_result_to_revents(&result, PHP_POLL_WRITE);
			if (n <= 0) {
				ret = -1;
				break;
			}
			result.status = PHP_IO_READY;
		}
		/* A provider that performs the op connects a socket whose connect
		 * we started already: EISCONN then means it completed meanwhile
		 * and the outcome is in SO_ERROR, as after a readiness report */
		if (result.status == PHP_IO_READY
				|| (started && result.status == PHP_IO_DONE && result.res < 0 && result.error == EISCONN)) {
			int error = 0;
			socklen_t len = sizeof(error);
			if (getsockopt(fd, SOL_SOCKET, SO_ERROR, (char *) &error, &len) != 0) {
				ret = -1;
			} else if (error) {
				php_io_set_errno(error);
				ret = -1;
			}
			break;
		}
		ssize_t r;
		if (php_io_data_result(&result, &r)) {
			ret = r < 0 ? -1 : 0;
			break;
		}
		php_io_set_errno(ENOTSUP);
		ret = -1;
		break;
	}

out:
	php_io_frame_end(&f, &op);
	return ret;
}

/* Regular files have no readiness form: without F_FILES the call is synchronous. Other descriptors
 * wait for readiness unless the provider performs the op or the deadline is non-blocking. */
static ssize_t php_io_file_op(php_stream *stream, int fd, php_deadline *dl, bool regular,
		ssize_t (*syscall_fn)(int, void *, size_t, int64_t), void *buf, size_t len, int64_t offset,
		void (*prep)(php_io_op *, zend_object *, php_socket_t, void *, size_t, int64_t, php_deadline))
{
	uint32_t flags = php_io_hook_flags();
	php_io_frame f;
	php_io_op op;
	php_io_op_result result;
	ssize_t ret;

	if (php_io_frame_begin(&f, stream) == FAILURE) {
		return -1;
	}
	memset(&op, 0, sizeof(op));

	bool nonblock = !regular && dl->hrtime == 0;
	bool offload = FG(io_hooks) && !nonblock
			&& (regular ? (flags & PHP_IO_HOOKS_F_FILES) : (flags & (PHP_IO_HOOKS_F_FILES | PHP_IO_HOOKS_F_DIRECT_DATA)));
	bool ready = nonblock || !FG(io_hooks);
	bool drained = false; /* the syscall returned EAGAIN: the next wait follows a drain */
	for (;;) {
		if (offload) {
			prep(&op, NULL, fd, buf, len, offset, *dl);
			if (prep == php_io_op_read) {
				op.flags |= php_io_stream_buf_flag(stream, buf);
			}
			op.stream = stream;
			if ((!regular && php_io_op_register_wait(&op, stream, op.ready_events) == FAILURE)
					|| php_io_run(&op, &result) == FAILURE) {
				php_io_set_errno(ECANCELED);
				ret = -1;
				break;
			}
			offload = false;
			if (php_io_data_result(&result, &ret)) {
				if (ret < 0 && !regular && PHP_IS_TRANSIENT_ERROR(errno)) {
					ready = false;
					drained = true;
					continue;
				}
				break;
			}
			/* Ready or Unsupported: perform it here */
			ready = result.status == PHP_IO_READY;
		}
		if (!regular && !ready) {
			/* Wait for readiness before the syscall */
			uint32_t events = prep == php_io_op_read ? PHP_POLL_READ : PHP_POLL_WRITE;
			php_io_op_poll(&op, NULL, fd, events, *dl);
			if (drained) {
				op.flags |= PHP_IO_OP_F_AFTER_DRAIN;
			}
			op.stream = stream;
			if (php_io_op_register_wait(&op, stream, events) == FAILURE
					|| php_io_run(&op, &result) == FAILURE) {
				php_io_set_errno(ECANCELED);
				ret = -1;
				break;
			}
			int n = php_io_poll_result_to_revents(&result, events);
			if (n < 0) {
				ret = -1;
				break;
			}
			if (n == 0) {
				php_io_set_errno(ETIMEDOUT);
				ret = -1;
				break;
			}
		}
		ret = syscall_fn(fd, buf, len, offset);
		if (ret < 0 && errno == EINTR) {
			/* Retried once; a second signal is left to the script */
			ret = syscall_fn(fd, buf, len, offset);
		}
		if (ret < 0 && !regular && !nonblock && PHP_IS_TRANSIENT_ERROR(errno)) {
			ready = false;
			drained = true;
			continue;
		}
		break;
	}

	php_io_frame_end(&f, &op);
	return ret;
}

/* The synchronous forms; an offset means pread and pwrite, on Windows the
 * overlapped call an overlapped descriptor needs */
static ssize_t php_io_read_syscall(int fd, void *buf, size_t len, int64_t offset)
{
	if (offset < 0) {
		return read(fd, buf, len);
	}
#ifdef PHP_WIN32
	return php_win32_ioutil_pread(fd, buf, len, offset);
#else
	return pread(fd, buf, len, (off_t) offset);
#endif
}

static ssize_t php_io_write_syscall(int fd, void *buf, size_t len, int64_t offset)
{
	if (offset < 0) {
		return write(fd, buf, len);
	}
#ifdef PHP_WIN32
	return php_win32_ioutil_pwrite(fd, buf, len, offset);
#else
	return pwrite(fd, buf, len, (off_t) offset);
#endif
}

PHPAPI ssize_t php_io_read_at(php_stream *stream, int fd, void *buf, size_t len, int64_t offset, php_deadline *dl)
{
	bool regular = !stream || !(stream->flags & PHP_STREAM_FLAG_NO_SEEK);
	return php_io_file_op(stream, fd, dl, regular, php_io_read_syscall, buf, len, offset, php_io_op_read);
}

PHPAPI ssize_t php_io_write_at(php_stream *stream, int fd, const void *buf, size_t len, int64_t offset, php_deadline *dl)
{
	bool regular = !stream || !(stream->flags & PHP_STREAM_FLAG_NO_SEEK);
	return php_io_file_op(stream, fd, dl, regular, php_io_write_syscall, (void *) buf, len, offset,
			(void (*)(php_io_op *, zend_object *, php_socket_t, void *, size_t, int64_t, php_deadline)) php_io_op_write);
}

PHPAPI ssize_t php_io_read(php_stream *stream, int fd, void *buf, size_t len, php_deadline *dl)
{
	return php_io_read_at(stream, fd, buf, len, -1, dl);
}

PHPAPI ssize_t php_io_write(php_stream *stream, int fd, const void *buf, size_t len, php_deadline *dl)
{
	return php_io_write_at(stream, fd, buf, len, -1, dl);
}

PHPAPI int php_io_fsync(php_stream *stream, int fd, bool data_only)
{
	if (FG(io_hooks) && (php_io_hook_flags() & PHP_IO_HOOKS_F_FILES)) {
		php_io_frame f;
		php_io_op op;
		php_io_op_result result;
		ssize_t ret;

		if (php_io_frame_begin(&f, stream) == FAILURE) {
			return -1;
		}
		php_io_op_fsync(&op, NULL, fd, data_only);
		op.stream = stream;
		zend_result rc = php_io_run(&op, &result);
		php_io_frame_end(&f, &op);
		if (rc == FAILURE) {
			php_io_set_errno(ECANCELED);
			return -1;
		}
		if (php_io_data_result(&result, &ret)) {
			return ret < 0 ? -1 : 0;
		}
	}
#ifdef HAVE_FDATASYNC
	return data_only ? fdatasync(fd) : fsync(fd);
#else
	return fsync(fd);
#endif
}

/* DNS: always handed to a provider, which may answer from its own
 * resolver; Unsupported means the library call */
/* A numeric host needs no lookup and must not reach a provider */
static bool php_io_host_is_numeric(const char *node, const struct addrinfo *hints)
{
	if (!node) {
		return true;
	}
	if (hints && (hints->ai_flags & AI_NUMERICHOST)) {
		return true;
	}
	struct in_addr in4;
	if (inet_pton(AF_INET, node, &in4) == 1) {
		return true;
	}
#ifdef HAVE_IPV6
	struct in6_addr in6;
	if (inet_pton(AF_INET6, node, &in6) == 1) {
		return true;
	}
#endif
	return false;
}

PHPAPI int php_io_getaddrinfo(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res, php_deadline *dl)
{
	if (FG(io_hooks) && !php_io_host_is_numeric(node, hints)) {
		php_io_op op;
		php_io_op_result result;
		php_io_op_getaddrinfo(&op, node, service, hints, res, *dl);
		if (php_io_run(&op, &result) == FAILURE) {
			return EAI_SYSTEM;
		}
		if (result.status == PHP_IO_DONE) {
			return result.error;
		}
		if (result.status == PHP_IO_TIMEOUT) {
			return EAI_AGAIN;
		}
		if (result.status == PHP_IO_CANCELLED) {
			return EAI_SYSTEM;
		}
	}
	return getaddrinfo(node, service, hints, res);
}

PHPAPI int php_io_getnameinfo(const struct sockaddr *addr, socklen_t addrlen, int flags, char *host, size_t hostlen, char *service, size_t servicelen, php_deadline *dl)
{
	if (FG(io_hooks)) {
		php_io_op op;
		php_io_op_result result;
		php_io_op_getnameinfo(&op, addr, addrlen, flags, host, hostlen, service, servicelen, *dl);
		if (php_io_run(&op, &result) == FAILURE) {
			return EAI_SYSTEM;
		}
		if (result.status == PHP_IO_DONE) {
			return result.error;
		}
		if (result.status == PHP_IO_TIMEOUT) {
			return EAI_AGAIN;
		}
		if (result.status == PHP_IO_CANCELLED) {
			return EAI_SYSTEM;
		}
	}
	return getnameinfo(addr, addrlen, host, hostlen, service, servicelen, flags);
}

#ifndef PHP_WIN32
/* The wait is the provider's; after Ready the core takes what a handle recorded or asks the kernel
 * without waiting, and waits again when nothing changed yet. The op's descriptor is the process or
 * signal source, so the poll queue completes it Ready without a handle object. */
PHPAPI pid_t php_io_waitpid(zend_object *handle, pid_t pid, int *status, int options, php_deadline *dl)
{
	int recorded;
	pid_t which = pid;
	if (php_io_child_take_reaped(&which, &recorded)) {
		if (status) {
			*status = recorded;
		}
		return which;
	}

	if (FG(io_hooks) && !(options & WNOHANG)) {
		php_io_op op;
		php_io_op_result result;
		pid_t ret;
		int pidfd = -1;
		int watchable = 0;
#ifdef WUNTRACED
		watchable |= WUNTRACED;
#endif
#ifdef WCONTINUED
		watchable |= WCONTINUED;
#endif
		if (pid > 0 && !(options & watchable)) {
			pidfd = php_poll_process_source_open(pid);
		}

		for (;;) {
			php_io_op_waitpid(&op, handle, pid, options, status, *dl);
			if (pidfd >= 0) {
				op.fd = pidfd;
			}
			if (php_io_run(&op, &result) == FAILURE) {
				php_io_set_errno(ECANCELED);
				ret = -1;
				break;
			}
			if (result.status == PHP_IO_READY) {
				which = pid;
				if (php_io_child_take_reaped(&which, &recorded)) {
					if (status) {
						*status = recorded;
					}
					ret = which;
					break;
				}
				ret = waitpid(pid, status, options | WNOHANG);
				if (ret != 0) {
					break;
				}
				continue;
			}
			if (result.status == PHP_IO_UNSUPPORTED) {
				ret = waitpid(pid, status, options);
				break;
			}
			ssize_t r;
			php_io_data_result(&result, &r);
			ret = (pid_t) r;
			break;
		}
		if (pidfd >= 0) {
			close(pidfd);
		}
		return ret;
	}

	return waitpid(pid, status, options);
}

PHPAPI int php_io_sigwait(zend_object *handle, const php_sigset_t *set, php_siginfo_t *info, php_deadline *dl)
{
#ifdef HAVE_SIGTIMEDWAIT
	bool as_op = FG(io_hooks) != NULL;
#else
	/* Without sigtimedwait() the synchronous wait is the op on the core
	 * queue too, which serves the deadline from the signal source */
	bool as_op = true;
#endif
	if (as_op) {
		php_io_op op;
		php_io_op_result result;
		int ret;
		int sfd = php_poll_signal_source_open(set);

		for (;;) {
			php_io_op_sigwait(&op, handle, set, info, *dl);
			if (sfd >= 0) {
				op.fd = sfd;
			}
			if (php_io_run(&op, &result) == FAILURE) {
				php_io_set_errno(ECANCELED);
				ret = -1;
				break;
			}
			bool again = result.status == PHP_IO_DONE && result.error == EAGAIN;
			if (result.status == PHP_IO_READY) {
				if (op.u.sigwait.taken > 0) {
					ret = op.u.sigwait.taken;
					break;
				}
				ret = php_poll_signal_source_take(sfd, set, info);
				if (ret > 0) {
					break;
				}
				again = true;
			}
			if (again) {
				/* Another wait collected the signal first */
				if (!php_deadline_is_infinite(dl) && php_io_deadline_remaining(dl, zend_hrtime()) == 0) {
					php_io_set_errno(EAGAIN);
					ret = -1;
					break;
				}
				continue;
			}
			if (result.status == PHP_IO_UNSUPPORTED) {
				ret = -2;
				break;
			}
			if (result.status == PHP_IO_TIMEOUT) {
				if (op.u.sigwait.taken > 0) {
					/* The handle consumed it in the same reap as the deadline */
					ret = op.u.sigwait.taken;
					break;
				}
				php_io_set_errno(EAGAIN);
				ret = -1;
				break;
			}
			ssize_t r;
			php_io_data_result(&result, &r);
			ret = (int) r;
			break;
		}
		if (sfd >= 0) {
			close(sfd);
		}
		if (ret != -2) {
			return ret;
		}
	}

	/* Unsupported by the provider and by the core queue: no source */
#ifdef HAVE_SIGTIMEDWAIT
	if (php_deadline_is_infinite(dl)) {
		return sigwaitinfo(set, info);
	}
	zend_hrtime_t remaining = php_io_deadline_remaining(dl, zend_hrtime());
	struct timespec ts = {
		.tv_sec = remaining / ZEND_NANO_IN_SEC,
		.tv_nsec = remaining % ZEND_NANO_IN_SEC,
	};
	return sigtimedwait(set, info, &ts);
#else
	if (php_deadline_is_infinite(dl)) {
		int signo;
		int err = sigwait(set, &signo);
		if (err != 0) {
			php_io_set_errno(err);
			return -1;
		}
		if (info) {
			memset(info, 0, sizeof(*info));
			info->si_signo = signo;
		}
		return signo;
	}
	/* A timed wait without a source has nothing to wait on here */
	php_io_set_errno(ENOSYS);
	return -1;
#endif
}
#endif

PHPAPI zend_result php_io_sleep(php_deadline dl, bool *interrupted)
{
	php_io_op op;
	php_io_op_result result;

	php_io_op_timer(&op, dl);
	zend_result rc = php_io_run(&op, &result);
	if (interrupted) {
		*interrupted = rc == SUCCESS && result.status == PHP_IO_INTERRUPTED;
	}
	return rc;
}
