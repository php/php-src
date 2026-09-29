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
#include "php_network.h"
#include "php_io.h"

#ifdef HAVE_IOR

#include <ior.h>
#include <errno.h>
#ifndef PHP_WIN32
# include <netdb.h>
# include <unistd.h>
# include <sys/wait.h>
#endif
#if defined(HAVE_FDATASYNC) && defined(__APPLE__)
/* The symbol is present, the headers do not declare it */
extern int fdatasync(int);
#endif

/* The op layer's signal types are handed to ior as they are */
static_assert(sizeof(php_sigset_t) == sizeof(ior_sigset_t), "php_sigset_t must match ior_sigset_t");
static_assert(sizeof(php_siginfo_t) == sizeof(ior_siginfo_t), "php_siginfo_t must match ior_siginfo_t");

/* One submitted op, alive until the main cqe and its linked timeout's were reaped. What the backend
 * reads or writes, apart from a stream's read buffer, lives in the record and reaches the op only
 * at delivery; it is malloc'ed, since the record outlives the request when the backend cannot be
 * stopped. */
typedef struct _php_io_ring_req php_io_ring_req;
typedef struct _php_io_ring_reg php_io_ring_reg;

/* An Edge-registered descriptor: what the ring keeps between the waits on it. The first wait after
 * a drain arms a multishot poll whose edges answer the waits parked on the record, or are kept as
 * ready bits for the next one; a Poll op that checks readiness at arm time stays single-shot. */
struct _php_io_ring_reg {
	php_socket_t fd;
	uint32_t edge; /* PHP_POLL_* events registered Edge */
	uint32_t ready; /* IOR_POLL_* reported by the multishot and consumed by no wait */
	bool hup; /* a hangup or error was reported: every wait completes at once */
	php_io_ring_req *poll; /* the armed multishot poll, NULL when none */
	uint32_t poll_mask; /* what it watches */
	php_io_ring_req *accept; /* the armed multishot accept of a listener, NULL when none */
	bool accept_failed; /* the backend refused it: accepts take their own entry */
	int *fds; /* connections accepted with no wait to take them, oldest first */
	uint32_t n_fds;
	uint32_t fds_cap;
};

/* Buffered connections past which the multishot accept is let go, until a take brings the buffer
 * below half: the listen backlog keeps the rest, where a process sharing the listener can still
 * take them */
#define PHP_IO_RING_ACCEPT_CAP 32

struct _php_io_ring_req {
	php_io_op *op;
	void *data;
	php_io_op_type type;
	php_deadline deadline;
	php_io_op_result result;
	int32_t main_res; /* the main cqe's result, -1 until then */
	ior_timespec ts; /* the timer's or the linked timeout's */
	php_io_ring_req *group; /* member: the Any's request */
	uint32_t index; /* member: position in the Any */
	bool has_lt; /* a linked timeout was submitted */
	bool main_done; /* the main cqe was reaped */
	bool lt_done; /* the linked timeout's cqe was reaped */
	bool early; /* completed at its linked timeout, before its main cqe */
	bool cancelled; /* cancel() was called */
	bool cancel_pending; /* the cancel still needs an entry */
	bool backlogged; /* waiting for room in the ring */
	bool orphaned; /* nobody wants the completion */
	bool delivered; /* the output went to the op */
	php_stream *orphan_stream; /* frozen until the record settled */
	bool ready; /* top-level: completion to deliver */
	bool fired; /* group: in the fired list */
	bool group_done; /* member: the group folded already */
	bool multishot; /* an Edge record's multishot poll, without an op */
	php_io_ring_reg *reg; /* multishot: its record, NULL once removed */
	php_io_ring_reg *waiting; /* a wait parked on an Edge record, without an entry */
	uint32_t w_mask; /* IOR_POLL_* the parked wait wants */
	php_io_ring_req *w_prev; /* the parked waits */
	php_io_ring_req *w_next;
	php_io_ring_req **members; /* group */
	uint32_t n_members;
	php_io_ring_req *prev; /* live list */
	php_io_ring_req *next;
	php_io_ring_req *bl_prev; /* backlog */
	php_io_ring_req *bl_next;
	union {
		struct { char *node; char *service; struct addrinfo hints; bool has_hints;
		         struct addrinfo *res; } gai;
		struct { struct sockaddr *addr; socklen_t addrlen; int flags;
		         char *host; size_t hostlen; char *service; size_t servicelen; } gni;
		struct { struct sockaddr *addr; socklen_t addrlen; socklen_t cap; } sock;
		struct { php_socket_t fd; bool data_only; } fsync;
		struct { int status; pid_t pgid; } waitpid; /* pgid: the child's group at submit, 0 unknown */
		struct { php_sigset_t set; php_siginfo_t info; } sigwait;
		struct { char *buf; } io; /* bounce buffer, or NULL */
	} u;
};

typedef struct {
	uintptr_t data;
	int32_t res;
	bool more; /* IOR_CQE_F_MORE: a multishot's edge, not its last completion */
} php_io_ring_cqe;

struct php_io_ring {
	ior_ctx *ctx;
	uint64_t id; /* tells this ring's records on a registration from another's */
	uint32_t features;
	bool fd_nonblock;
	pid_t owner_pid; /* a forked child must not touch the ring */
	php_io_queue *queue; /* the queue over the ring, if any */
	php_io_ring *prev_ring; /* the rings of this thread */
	php_io_ring *next_ring;
	uint32_t unsubmitted; /* taken, not submitted yet */
	php_io_ring_req *live; /* records with a cqe or a completion pending */
	uint32_t pending; /* not delivered yet, orphans excluded */
	uint32_t n_cancel_pending;
	php_io_ring_req *bl_head; /* ops waiting for room, oldest first */
	php_io_ring_req *bl_tail;
	HashTable regs; /* fd -> php_io_ring_reg */
	php_io_ring_req *waiting; /* waits parked on Edge records */
	php_io_ring_req **ready;
	uint32_t n_ready;
	uint32_t ready_cap;
	php_io_ring_req **fired;
	uint32_t n_fired;
	uint32_t fired_cap;
	ior_cqe **cqes;
	php_io_ring_cqe *batch;
	uint32_t cqes_cap;
	bool notify_created;
};

/* Tags in the cqe user data: the linked timeout's cqe and a cancel's cqe
 * carry the record pointer with a low bit set */
#define PHP_IO_RING_TAG_LT     ((uintptr_t) 1)
#define PHP_IO_RING_TAG_CANCEL ((uintptr_t) 2)
#define PHP_IO_RING_TAG_MASK   ((uintptr_t) 3)

/* Every cqe of the record arrived */
static zend_always_inline bool php_io_ring_req_settled(php_io_ring_req *req)
{
	return req->main_done && (!req->has_lt || req->lt_done);
}

static void php_io_ring_list_push(php_io_ring_req ***list, uint32_t *n, uint32_t *cap, php_io_ring_req *req)
{
	if (*n == *cap) {
		*cap = *cap ? *cap * 2 : 16;
		*list = safe_erealloc(*list, *cap, sizeof(**list), 0);
	}
	(*list)[(*n)++] = req;
}

static void php_io_ring_list_remove(php_io_ring_req **list, uint32_t *n, php_io_ring_req *req)
{
	for (uint32_t i = 0; i < *n; i++) {
		if (list[i] == req) {
			memmove(&list[i], &list[i + 1], (*n - i - 1) * sizeof(*list));
			(*n)--;
			return;
		}
	}
	ZEND_UNREACHABLE();
}

ZEND_TLS php_io_ring *php_io_rings = NULL;

/* Inherited across fork: the kernel ring is shared with the parent and the
 * worker threads do not exist here, so ior drops the child's copy */
static bool php_io_ring_foreign(php_io_ring *ring)
{
	if (EXPECTED(ring->owner_pid == getpid())) {
		return false;
	}
#ifndef PHP_WIN32
	if (ring->ctx) {
		ior_queue_forget(ring->ctx);
		ring->ctx = NULL;
	}
#endif
	return true;
}

PHPAPI bool php_io_ring_inherited(php_io_ring *ring)
{
	return php_io_ring_foreign(ring);
}

PHPAPI void php_io_ring_after_fork(void)
{
	for (php_io_ring *ring = php_io_rings; ring; ring = ring->next_ring) {
		php_io_ring_foreign(ring);
	}
}

PHPAPI php_io_ring *php_io_ring_create(uint32_t entries, bool fd_nonblock)
{
	ior_params params;
	memset(&params, 0, sizeof(params));
	params.backend = IOR_BACKEND_AUTO;
	if (fd_nonblock) {
		params.flags |= IOR_SETUP_FD_NONBLOCK;
	}

	/* An op and its linked timeout go in one submission */
	params.sq_entries = MAX(entries ? entries : PHP_IO_RING_DEFAULT_ENTRIES, 2);

	ior_ctx *ctx;
	int rc = ior_queue_init_params(params.sq_entries, &ctx, &params);
	if (rc < 0) {
		errno = -rc;
		return NULL;
	}

	php_io_ring *ring = ecalloc(1, sizeof(*ring));
	ring->id = php_io_queue_new_id();
	ring->next_ring = php_io_rings;
	if (php_io_rings) {
		php_io_rings->prev_ring = ring;
	}
	php_io_rings = ring;
	ring->ctx = ctx;
	ring->features = params.features;
	ring->owner_pid = getpid();
	ring->fd_nonblock = fd_nonblock;
	ring->cqes_cap = 64;
	ring->cqes = safe_emalloc(ring->cqes_cap, sizeof(*ring->cqes), 0);
	ring->batch = safe_emalloc(ring->cqes_cap, sizeof(*ring->batch), 0);
	zend_hash_init(&ring->regs, 8, NULL, NULL, 0);
	return ring;
}

PHPAPI uint32_t php_io_ring_features(php_io_ring *ring)
{
	return ring->features;
}

PHPAPI php_io_ring_backend_type php_io_ring_get_backend_type(php_io_ring *ring)
{
	switch (ior_get_backend_type(ring->ctx)) {
		case IOR_BACKEND_IOURING: return PHP_IO_RING_BACKEND_IO_URING;
		case IOR_BACKEND_IOCP: return PHP_IO_RING_BACKEND_IOCP;
		default: return PHP_IO_RING_BACKEND_THREADS;
	}
}

PHPAPI const char *php_io_ring_backend_name(php_io_ring *ring)
{
	return ior_get_backend_name(ring->ctx);
}

/* What a provider on a ring registers with unless it chooses: Edge registrations cost nothing and
 * let the ring keep its multishots, and a direct accept is served from the multishot accept's
 * buffer at submit, so the accepting fiber takes a burst without a pass per connection. Direct
 * data ops and file ops lose when data is usually ready and files are in the page cache, so they
 * stay opt-in. */
PHPAPI uint32_t php_io_ring_hook_flags(php_io_ring *ring)
{
	return php_io_ring_supported_hook_flags(ring)
			& (PHP_IO_HOOKS_F_EDGE_REGISTRATIONS | PHP_IO_HOOKS_F_DIRECT_ACCEPT);
}

PHPAPI uint32_t php_io_ring_supported_hook_flags(php_io_ring *ring)
{
	/* Every ior backend keeps a multishot accept, which is what makes a direct
	 * accept serve a burst in order of arrival rather than one per pass */
	uint32_t flags = PHP_IO_HOOKS_F_FILES | PHP_IO_HOOKS_F_DIRECT_ACCEPT;
#ifndef PHP_WIN32
	/* Edge registrations need a multishot poll that reports edges. IOCP's
	 * re-reports readiness that persists (ior_prep_poll_multishot), so the
	 * ring's Edge records would keep completing; not offered there. */
	flags |= PHP_IO_HOOKS_F_EDGE_REGISTRATIONS;
#endif
	if (ring->features & IOR_FEAT_NATIVE_ASYNC) {
		flags |= PHP_IO_HOOKS_F_DIRECT_DATA;
	}
	return flags;
}

static php_io_ring_reg *php_io_ring_reg_get(php_io_ring *ring, php_socket_t fd, bool create)
{
	php_io_ring_reg *rec = zend_hash_index_find_ptr(&ring->regs, (zend_ulong) fd);
	if (!rec && create) {
		rec = ecalloc(1, sizeof(*rec));
		rec->fd = fd;
		zend_hash_index_add_new_ptr(&ring->regs, (zend_ulong) fd, rec);
	}
	return rec;
}

/* The record through the registration, which php_io_ring_add() pointed at it; one another ring
 * left there is told by the id and looked up by descriptor. A registered pair keeps its record. */
static zend_always_inline php_io_ring_reg *php_io_ring_reg_of(php_io_ring *ring,
		php_io_registration *registration, php_socket_t fd)
{
	if (registration && registration->queue_id == ring->id && registration->queue_data) {
		return registration->queue_data;
	}
	return php_io_ring_reg_get(ring, fd, false);
}

static void php_io_ring_reg_disarm(php_io_ring *ring, php_io_ring_reg *rec);
static void php_io_ring_reg_disarm_accept(php_io_ring *ring, php_io_ring_reg *rec, bool keep);

/* The connections nobody took: their clients see a reset, as with a listener closed with a backlog */
static void php_io_ring_reg_close_fds(php_io_ring_reg *rec)
{
	for (uint32_t i = 0; i < rec->n_fds; i++) {
		closesocket(rec->fds[i]);
	}
	rec->n_fds = 0;
	if (rec->fds) {
		efree(rec->fds);
		rec->fds = NULL;
		rec->fds_cap = 0;
	}
}

/* Nothing is armed here: the first wait after a drain does that, when it knows what to keep */
PHPAPI zend_result php_io_ring_add(php_io_ring *ring, php_io_registration *reg)
{
	if (php_io_ring_foreign(ring)) {
		errno = EPERM;
		return FAILURE;
	}
	if (reg->fd == SOCK_ERR || !(reg->event == PHP_POLL_READ || reg->event == PHP_POLL_WRITE)) {
		errno = EBADF;
		return FAILURE;
	}
	if (!(ring->features & IOR_FEAT_POLL_ADD)) {
		/* Served as one-shot waits */
		return SUCCESS;
	}
	php_io_ring_reg *rec = php_io_ring_reg_get(ring, reg->fd, true);
	rec->edge |= reg->event;
	reg->queue_data = rec;
	reg->queue_id = ring->id;
	return SUCCESS;
}

PHPAPI void php_io_ring_remove(php_io_ring *ring, php_io_registration *reg)
{
	php_io_ring_reg *rec = php_io_ring_reg_of(ring, reg, reg->fd);
	reg->queue_data = NULL;
	if (!rec) {
		return;
	}
	rec->edge &= ~reg->event;
	if (rec->poll) {
		/* Cancelled before the descriptor closes; a remaining direction re-arms at its next wait */
		php_io_ring_reg_disarm(ring, rec);
	}
	if (!(rec->edge & PHP_POLL_READ)) {
		if (rec->accept) {
			php_io_ring_reg_disarm_accept(ring, rec, false);
		}
		php_io_ring_reg_close_fds(rec);
	}
	if (!rec->edge) {
		zend_hash_index_del(&ring->regs, (zend_ulong) rec->fd);
		efree(rec);
	}
}

PHPAPI php_socket_t php_io_ring_notify_fd(php_io_ring *ring)
{
	if (php_io_ring_foreign(ring)) {
		errno = EPERM;
		return SOCK_ERR;
	}
	ior_fd_t fd = ior_notify_fd(ring->ctx);
	if (fd == IOR_INVALID_FD) {
		return SOCK_ERR;
	}
	ring->notify_created = true;
	return (php_socket_t) fd;
}

PHPAPI void php_io_ring_notify_clear(php_io_ring *ring)
{
	if (ring->notify_created && !php_io_ring_foreign(ring)) {
		ior_notify_clear(ring->ctx);
	}
}

PHPAPI uint32_t php_io_ring_count_pending(php_io_ring *ring)
{
	if (php_io_ring_foreign(ring)) {
		return 0;
	}
	/* Orphans included: a loop must keep reaping until they settled. A
	 * multishot poll owns nothing of a caller's and settles when it can. */
	uint32_t n = ring->pending;
	for (php_io_ring_req *r = ring->live; r; r = r->next) {
		if (r->orphaned && !r->ready && !r->multishot && !php_io_ring_req_settled(r)) {
			n++;
		}
	}
	return n;
}

static uint32_t php_io_ring_reap(php_io_ring *ring);

/* Records */

/* What one submission of a data op transfers at most */
static zend_always_inline unsigned php_io_ring_io_len(php_io_op *op)
{
	return (unsigned) MIN(op->u.io.len, UINT32_MAX);
}

/* The inputs are copied and the outputs get slots of their own, so that a
 * record outliving the op's frame never touches it */
static void php_io_ring_req_capture(php_io_ring_req *req, php_io_op *op)
{
	switch (op->type) {
		case PHP_IO_OP_GETADDRINFO:
			req->u.gai.node = op->u.getaddrinfo.node ? pestrdup(op->u.getaddrinfo.node, 1) : NULL;
			req->u.gai.service = op->u.getaddrinfo.service ? pestrdup(op->u.getaddrinfo.service, 1) : NULL;
			if (op->u.getaddrinfo.hints) {
				req->u.gai.hints.ai_flags = op->u.getaddrinfo.hints->ai_flags;
				req->u.gai.hints.ai_family = op->u.getaddrinfo.hints->ai_family;
				req->u.gai.hints.ai_socktype = op->u.getaddrinfo.hints->ai_socktype;
				req->u.gai.hints.ai_protocol = op->u.getaddrinfo.hints->ai_protocol;
				req->u.gai.has_hints = true;
			}
			break;
		case PHP_IO_OP_GETNAMEINFO:
			req->u.gni.addrlen = op->u.getnameinfo.addrlen;
			req->u.gni.addr = pemalloc(MAX(req->u.gni.addrlen, 1), 1);
			memcpy(req->u.gni.addr, op->u.getnameinfo.addr, req->u.gni.addrlen);
			req->u.gni.flags = op->u.getnameinfo.flags;
			if (op->u.getnameinfo.host && op->u.getnameinfo.hostlen) {
				req->u.gni.hostlen = op->u.getnameinfo.hostlen;
				req->u.gni.host = pemalloc(req->u.gni.hostlen, 1);
			}
			if (op->u.getnameinfo.service && op->u.getnameinfo.servicelen) {
				req->u.gni.servicelen = op->u.getnameinfo.servicelen;
				req->u.gni.service = pemalloc(req->u.gni.servicelen, 1);
			}
			break;
		case PHP_IO_OP_ACCEPT:
			if (op->u.accept.addr && op->u.accept.addrlen) {
				req->u.sock.cap = *op->u.accept.addrlen;
				req->u.sock.addrlen = req->u.sock.cap;
				req->u.sock.addr = pemalloc(MAX(req->u.sock.cap, 1), 1);
			}
			break;
		case PHP_IO_OP_CONNECT:
			req->u.sock.addrlen = op->u.connect.addrlen;
			req->u.sock.addr = pemalloc(MAX(req->u.sock.addrlen, 1), 1);
			memcpy(req->u.sock.addr, op->u.connect.addr, req->u.sock.addrlen);
			break;
		case PHP_IO_OP_FSYNC:
			req->u.fsync.fd = op->fd;
			req->u.fsync.data_only = op->u.fsync.data_only;
			break;
		case PHP_IO_OP_SIGWAIT:
			req->u.sigwait.set = *op->u.sigwait.set;
			break;
		case PHP_IO_OP_READ:
		case PHP_IO_OP_RECV:
			if (!(op->flags & PHP_IO_OP_F_STREAM_BUF)) {
				req->u.io.buf = pemalloc(MAX(php_io_ring_io_len(op), 1), 1);
			}
			break;
		case PHP_IO_OP_WRITE:
		case PHP_IO_OP_SEND:
			req->u.io.buf = pemalloc(MAX(php_io_ring_io_len(op), 1), 1);
			memcpy(req->u.io.buf, op->u.io.buf, php_io_ring_io_len(op));
			break;
		default:
			break;
	}
}

/* The completion is delivered: the op gets what the record collected */
static void php_io_ring_req_output(php_io_ring_req *req, php_io_op *op)
{
	int32_t res = req->main_res;

	/* Completed early: what the op produces later is released with the record */
	if (!req->main_done) {
		return;
	}
	req->delivered = true;
	switch (req->type) {
		case PHP_IO_OP_GETADDRINFO:
			if (op->u.getaddrinfo.res) {
				*op->u.getaddrinfo.res = req->u.gai.res;
				req->u.gai.res = NULL;
			}
			break;
		case PHP_IO_OP_GETNAMEINFO:
			if (res == 0) {
				if (req->u.gni.host) {
					memcpy(op->u.getnameinfo.host, req->u.gni.host, req->u.gni.hostlen);
				}
				if (req->u.gni.service) {
					memcpy(op->u.getnameinfo.service, req->u.gni.service, req->u.gni.servicelen);
				}
			}
			break;
		case PHP_IO_OP_ACCEPT:
			if (res >= 0 && req->u.sock.addr) {
				memcpy(op->u.accept.addr, req->u.sock.addr, MIN(req->u.sock.addrlen, req->u.sock.cap));
				*op->u.accept.addrlen = req->u.sock.addrlen;
			}
			break;
		case PHP_IO_OP_WAITPID:
			if (res > 0 && op->u.waitpid.status) {
				*op->u.waitpid.status = req->u.waitpid.status;
			}
			break;
		case PHP_IO_OP_SIGWAIT:
			if (res > 0 && op->u.sigwait.info) {
				*op->u.sigwait.info = req->u.sigwait.info;
			}
			break;
		case PHP_IO_OP_READ:
		case PHP_IO_OP_RECV:
			if (res > 0 && req->u.io.buf) {
				memcpy(op->u.io.buf, req->u.io.buf, res);
			}
			break;
		default:
			break;
	}
}

/* The record goes away: what the op produced and nobody took is released */
static void php_io_ring_req_discard(php_io_ring_req *req)
{
	bool unclaimed = req->main_done && !req->delivered;
	int32_t res = req->main_res;

	switch (req->type) {
		case PHP_IO_OP_GETADDRINFO:
			if (req->u.gai.res) {
				freeaddrinfo(req->u.gai.res);
			}
			if (req->u.gai.node) {
				pefree(req->u.gai.node, 1);
			}
			if (req->u.gai.service) {
				pefree(req->u.gai.service, 1);
			}
			break;
		case PHP_IO_OP_GETNAMEINFO:
			pefree(req->u.gni.addr, 1);
			if (req->u.gni.host) {
				pefree(req->u.gni.host, 1);
			}
			if (req->u.gni.service) {
				pefree(req->u.gni.service, 1);
			}
			break;
		case PHP_IO_OP_ACCEPT:
			if (unclaimed && res >= 0) {
				closesocket((php_socket_t) res);
			}
			ZEND_FALLTHROUGH;
		case PHP_IO_OP_CONNECT:
			if (req->u.sock.addr) {
				pefree(req->u.sock.addr, 1);
			}
			break;
		case PHP_IO_OP_READ:
		case PHP_IO_OP_RECV:
		case PHP_IO_OP_WRITE:
		case PHP_IO_OP_SEND:
			if (req->u.io.buf) {
				pefree(req->u.io.buf, 1);
			}
			break;
		case PHP_IO_OP_WAITPID:
			/* ior reaped the child: its status goes to the next wait for it */
#ifdef PHP_WIN32
			if (unclaimed && res > 0) {
#else
			if (unclaimed && res > 0 && (WIFEXITED(req->u.waitpid.status) || WIFSIGNALED(req->u.waitpid.status))) {
#endif
				php_io_child_reaped_ex((pid_t) res, req->u.waitpid.pgid, req->u.waitpid.status);
			}
			break;
		case PHP_IO_OP_SIGWAIT:
			/* The op took the signal: it goes back to the process */
			if (unclaimed && res > 0) {
				ior_sigrequeue((const ior_siginfo_t *) &req->u.sigwait.info);
			}
			break;
		default:
			break;
	}
}

static php_io_ring_req *php_io_ring_req_alloc(php_io_ring *ring)
{
	php_io_ring_req *req = pecalloc(1, sizeof(*req), 1);
	req->main_res = -1;
	req->next = ring->live;
	if (ring->live) {
		ring->live->prev = req;
	}
	ring->live = req;
	return req;
}

static php_io_ring_req *php_io_ring_req_create(php_io_ring *ring, php_io_op *op, void *data)
{
	php_io_ring_req *req = php_io_ring_req_alloc(ring);
	req->op = op;
	req->data = data;
	req->type = op->type;
	req->deadline = op->deadline;
	php_io_ring_req_capture(req, op);
	return req;
}

/* Records with a completion still to come, the armed multishots aside: what a wait with nothing
 * pending has to see settled before it returns */
static bool php_io_ring_settling(php_io_ring *ring)
{
	for (php_io_ring_req *r = ring->live; r; r = r->next) {
		if (!r->multishot) {
			return true;
		}
	}
	return false;
}

/* Parked waits */

static void php_io_ring_waiter_park(php_io_ring *ring, php_io_ring_req *req, php_io_ring_reg *rec, uint32_t mask)
{
	req->waiting = rec;
	req->w_mask = mask;
	req->w_prev = NULL;
	req->w_next = ring->waiting;
	if (ring->waiting) {
		ring->waiting->w_prev = req;
	}
	ring->waiting = req;
}

static void php_io_ring_waiter_settle(php_io_ring *ring, php_io_ring_req *req)
{
	if (req->w_prev) {
		req->w_prev->w_next = req->w_next;
	} else {
		ring->waiting = req->w_next;
	}
	if (req->w_next) {
		req->w_next->w_prev = req->w_prev;
	}
	req->w_prev = req->w_next = NULL;
	req->waiting = NULL;
}

static void php_io_ring_backlog_push(php_io_ring *ring, php_io_ring_req *req)
{
	req->backlogged = true;
	req->bl_next = NULL;
	req->bl_prev = ring->bl_tail;
	if (ring->bl_tail) {
		ring->bl_tail->bl_next = req;
	} else {
		ring->bl_head = req;
	}
	ring->bl_tail = req;
}

static void php_io_ring_backlog_remove(php_io_ring *ring, php_io_ring_req *req)
{
	if (req->bl_prev) {
		req->bl_prev->bl_next = req->bl_next;
	} else {
		ring->bl_head = req->bl_next;
	}
	if (req->bl_next) {
		req->bl_next->bl_prev = req->bl_prev;
	} else {
		ring->bl_tail = req->bl_prev;
	}
	req->bl_prev = req->bl_next = NULL;
	req->backlogged = false;
}

static void php_io_ring_req_free(php_io_ring *ring, php_io_ring_req *req)
{
	if (req->orphan_stream) {
		php_io_stream_unfreeze(req->orphan_stream);
		req->orphan_stream = NULL;
	}
	if (req->backlogged) {
		php_io_ring_backlog_remove(ring, req);
	}
	if (req->waiting) {
		php_io_ring_waiter_settle(ring, req);
	}
	if (req->reg) {
		if (req->type == PHP_IO_OP_ACCEPT) {
			if (req->reg->accept == req) {
				req->reg->accept = NULL;
			}
		} else if (req->reg->poll == req) {
			req->reg->poll = NULL;
		}
		req->reg = NULL;
	}
	if (req->cancel_pending) {
		ring->n_cancel_pending--;
	}
	if (req->prev) {
		req->prev->next = req->next;
	} else {
		ring->live = req->next;
	}
	if (req->next) {
		req->next->prev = req->prev;
	}
	php_io_ring_req_discard(req);
	if (req->members) {
		efree(req->members);
	}
	pefree(req, 1);
}

/* Read and Write ops carry a file descriptor: on Windows a CRT one, whose
 * handle is what IOCP works on (an overlapped one, the plain wrapper's
 * promise for the files it hands over) */
static zend_always_inline ior_fd_t php_io_ring_file_fd(php_io_op *op)
{
#ifdef PHP_WIN32
	return (ior_fd_t) _get_osfhandle((int) op->fd);
#else
	return (ior_fd_t) op->fd;
#endif
}

/* The bounce buffer, or the stream's read buffer the op points at */
static zend_always_inline void *php_io_ring_io_buf(php_io_ring_req *req, php_io_op *op)
{
	return req->u.io.buf ? req->u.io.buf : op->u.io.buf;
}

/* Work callbacks: they run on a worker and see only the record */

static int32_t php_io_ring_work_getaddrinfo(ior_work_token *token, void *arg)
{
	php_io_ring_req *req = arg;
	return getaddrinfo(req->u.gai.node, req->u.gai.service,
			req->u.gai.has_hints ? &req->u.gai.hints : NULL, &req->u.gai.res);
}

static int32_t php_io_ring_work_getnameinfo(ior_work_token *token, void *arg)
{
	php_io_ring_req *req = arg;
	return getnameinfo(req->u.gni.addr, req->u.gni.addrlen,
			req->u.gni.host, req->u.gni.hostlen,
			req->u.gni.service, req->u.gni.servicelen, req->u.gni.flags);
}

static int32_t php_io_ring_work_fsync(ior_work_token *token, void *arg)
{
	php_io_ring_req *req = arg;
#ifdef HAVE_FDATASYNC
	int rc = req->u.fsync.data_only ? fdatasync((int) req->u.fsync.fd) : fsync((int) req->u.fsync.fd);
#else
	int rc = fsync((int) req->u.fsync.fd);
#endif
	return rc == 0 ? 0 : -errno;
}

static uint32_t php_io_ring_poll_mask_to_ior(uint32_t events)
{
	uint32_t mask = 0;
	if (events & PHP_POLL_READ) {
		mask |= IOR_POLL_IN;
	}
	if (events & PHP_POLL_WRITE) {
		mask |= IOR_POLL_OUT;
	}
	return mask;
}

static uint32_t php_io_ring_poll_mask_from_ior(uint32_t mask)
{
	uint32_t events = 0;
	if (mask & IOR_POLL_IN) {
		events |= PHP_POLL_READ;
	}
	if (mask & IOR_POLL_OUT) {
		events |= PHP_POLL_WRITE;
	}
	if (mask & (IOR_POLL_ERR | IOR_POLL_NVAL)) {
		events |= PHP_POLL_ERROR;
	}
	if (mask & IOR_POLL_HUP) {
		events |= PHP_POLL_HUP;
	}
	return events;
}

/* Relative, because zend_hrtime() runs on CLOCK_MONOTONIC_RAW where it
 * exists and ior's absolute deadlines offer the monotonic, boot-time and
 * wall clocks */
static void php_io_ring_deadline_to_ts(const php_deadline *dl, ior_timespec *ts)
{
	zend_hrtime_t remaining = php_io_deadline_remaining(dl, zend_hrtime());
	ts->tv_sec = (int64_t) (remaining / ZEND_NANO_IN_SEC);
	ts->tv_nsec = (long long) (remaining % ZEND_NANO_IN_SEC);
}

/* Entries the last submits left behind are tried again */
static int php_io_ring_flush(php_io_ring *ring)
{
	if (ring->unsubmitted == 0) {
		return 0;
	}
	int rc = ior_submit(ring->ctx);
	if (rc >= 0) {
		ring->unsubmitted = rc == 0 ? 0 : ring->unsubmitted - MIN((uint32_t) rc, ring->unsubmitted);
	}
	return rc;
}

/* An entry taken for a prep that failed: a nop whose completion carries no
 * record, so the next submit issues nothing stale */
static void php_io_ring_sqe_void(ior_ctx *ctx, ior_sqe *sqe)
{
	ior_prep_nop(ctx, sqe);
	ior_sqe_set_data(ctx, sqe, NULL);
}

/* Takes n entries or none; a full submission queue is submitted first */
static bool php_io_ring_get_sqes(php_io_ring *ring, ior_sqe **sqes, uint32_t n)
{
	for (int attempt = 0; attempt < 2; attempt++) {
		uint32_t got = 0;
		while (got < n && (sqes[got] = ior_get_sqe(ring->ctx)) != NULL) {
			got++;
		}
		ring->unsubmitted += got;
		if (got == n) {
			return true;
		}
		for (uint32_t i = 0; i < got; i++) {
			php_io_ring_sqe_void(ring->ctx, sqes[i]);
		}
		php_io_ring_flush(ring);
	}
	return false;
}

/* Preps and submits one op (a member of an Any included). Returns FAILURE
 * with errno set: EBUSY when the ring has no room now, ENOTSUP when the op
 * has no ring form. */
static zend_result php_io_ring_submit_one(php_io_ring *ring, php_io_ring_req *req)
{
	php_io_op *op = req->op;
	ior_ctx *ctx = ring->ctx;
	bool link_deadline = !php_deadline_is_infinite(&req->deadline) && req->type != PHP_IO_OP_TIMER;
	bool uses_caller = false; /* the backend references buf or fd */
	ior_sqe *sqes[2];
	int err = 0;

	/* What has no ring form is refused before an entry is taken */
	switch (req->type) {
		case PHP_IO_OP_POLL:
			if (!(ring->features & IOR_FEAT_POLL_ADD)) {
				errno = ENOTSUP;
				return FAILURE;
			}
			break;
		case PHP_IO_OP_TIMER:
			if (php_deadline_is_infinite(&req->deadline)) {
				/* Never fires: ior_prep_nop would end it at once */
				errno = ENOTSUP;
				return FAILURE;
			}
			break;
		case PHP_IO_OP_READ:
		case PHP_IO_OP_WRITE:
		case PHP_IO_OP_RECV:
		case PHP_IO_OP_SEND:
		case PHP_IO_OP_ACCEPT:
		case PHP_IO_OP_CONNECT:
		case PHP_IO_OP_GETADDRINFO:
		case PHP_IO_OP_GETNAMEINFO:
		case PHP_IO_OP_FSYNC:
		case PHP_IO_OP_WAITPID:
		case PHP_IO_OP_SIGWAIT:
			break;
		default:
			errno = ENOTSUP;
			return FAILURE;
	}

	if (!php_io_ring_get_sqes(ring, sqes, link_deadline ? 2 : 1)) {
		errno = EBUSY;
		return FAILURE;
	}
	ior_sqe *sqe = sqes[0];

	switch (req->type) {
		case PHP_IO_OP_POLL:
			ior_prep_poll_add(ctx, sqe, (ior_fd_t) op->fd, php_io_ring_poll_mask_to_ior(op->u.poll.events));
			break;
		case PHP_IO_OP_TIMER:
			php_io_ring_deadline_to_ts(&req->deadline, &req->ts);
			ior_prep_timeout(ctx, sqe, &req->ts, 0, 0);
			break;
		case PHP_IO_OP_READ:
			ior_prep_read(ctx, sqe, php_io_ring_file_fd(op), php_io_ring_io_buf(req, op), php_io_ring_io_len(op),
					op->u.io.offset < 0 ? IOR_OFF_NONE : (uint64_t) op->u.io.offset);
			uses_caller = true;
			break;
		case PHP_IO_OP_WRITE:
			ior_prep_write(ctx, sqe, php_io_ring_file_fd(op), php_io_ring_io_buf(req, op), php_io_ring_io_len(op),
					op->u.io.offset < 0 ? IOR_OFF_NONE : (uint64_t) op->u.io.offset);
			uses_caller = true;
			break;
		case PHP_IO_OP_RECV:
			ior_prep_recv(ctx, sqe, (ior_fd_t) op->fd, php_io_ring_io_buf(req, op), php_io_ring_io_len(op), op->u.io.flags);
			uses_caller = true;
			break;
		case PHP_IO_OP_SEND:
			ior_prep_send(ctx, sqe, (ior_fd_t) op->fd, php_io_ring_io_buf(req, op), php_io_ring_io_len(op), op->u.io.flags);
			uses_caller = true;
			break;
		case PHP_IO_OP_ACCEPT:
			ior_prep_accept(ctx, sqe, (ior_fd_t) op->fd, req->u.sock.addr, req->u.sock.addr ? &req->u.sock.addrlen : NULL,
					IOR_ACCEPT_CLOEXEC | (ring->fd_nonblock ? IOR_ACCEPT_NONBLOCK : 0));
			uses_caller = true;
			break;
		case PHP_IO_OP_CONNECT:
			ior_prep_connect(ctx, sqe, (ior_fd_t) op->fd, req->u.sock.addr, req->u.sock.addrlen);
			uses_caller = true;
			break;
		case PHP_IO_OP_GETADDRINFO:
			err = ior_prep_work(ctx, sqe, php_io_ring_work_getaddrinfo, req) < 0 ? ENOTSUP : 0;
			break;
		case PHP_IO_OP_GETNAMEINFO:
			err = ior_prep_work(ctx, sqe, php_io_ring_work_getnameinfo, req) < 0 ? ENOTSUP : 0;
			break;
		case PHP_IO_OP_FSYNC:
			err = ior_prep_work(ctx, sqe, php_io_ring_work_fsync, req) < 0 ? ENOTSUP : 0;
			uses_caller = true;
			break;
		case PHP_IO_OP_WAITPID:
#ifndef PHP_WIN32
			/* A zombie loses its group on macOS: take it while the child is alive */
			req->u.waitpid.pgid = op->u.waitpid.pid > 0 ? getpgid((pid_t) op->u.waitpid.pid) : 0;
			if (req->u.waitpid.pgid < 0) {
				req->u.waitpid.pgid = 0;
			}
#endif
			err = ior_prep_waitpid(ctx, sqe, (ior_pid_t) op->u.waitpid.pid, &req->u.waitpid.status, op->u.waitpid.options) < 0 ? ENOTSUP : 0;
			break;
		case PHP_IO_OP_SIGWAIT: {
			int rc = ior_prep_sigwait(ctx, sqe, (const ior_sigset_t *) &req->u.sigwait.set, (ior_siginfo_t *) &req->u.sigwait.info);
			err = rc < 0 ? -rc : 0;
			break;
		}
		default:
			ZEND_UNREACHABLE();
	}

	if (err) {
		php_io_ring_sqe_void(ctx, sqe);
		if (link_deadline) {
			php_io_ring_sqe_void(ctx, sqes[1]);
		}
		php_io_ring_flush(ring);
		errno = err;
		return FAILURE;
	}

	if (uses_caller) {
		op->in_flight = true;
	}
	ior_sqe_set_data(ctx, sqe, req);

	if (link_deadline) {
		ior_sqe_set_flags(ctx, sqe, IOR_SQE_IO_LINK);
		php_io_ring_deadline_to_ts(&req->deadline, &req->ts);
		ior_prep_link_timeout(ctx, sqes[1], &req->ts, 0);
		ior_sqe_set_data(ctx, sqes[1], (void *) ((uintptr_t) req | PHP_IO_RING_TAG_LT));
		req->has_lt = true;
	}

	/* What a failed or short submit leaves is tried again before a wait blocks */
	php_io_ring_flush(ring);
	return SUCCESS;
}

static bool php_io_ring_submit_cancel(php_io_ring *ring, php_io_ring_req *req)
{
	ior_sqe *sqe;
	if (!php_io_ring_get_sqes(ring, &sqe, 1)) {
		return false;
	}
	ior_prep_cancel(ring->ctx, sqe, req);
	ior_sqe_set_data(ring->ctx, sqe, (void *) ((uintptr_t) req | PHP_IO_RING_TAG_CANCEL));
	php_io_ring_flush(ring);
	return true;
}

/* A completion to hand out: the member fires its group, anything else is ready */
static void php_io_ring_req_complete(php_io_ring *ring, php_io_ring_req *req)
{
	if (req->group) {
		if (!req->group->group_done && !req->group->fired) {
			req->group->fired = true;
			php_io_ring_list_push(&ring->fired, &ring->n_fired, &ring->fired_cap, req->group);
		}
		return;
	}
	req->ready = true;
	php_io_ring_list_push(&ring->ready, &ring->n_ready, &ring->ready_cap, req);
}

/* The op completes without reaching the backend */
static void php_io_ring_req_fail(php_io_ring *ring, php_io_ring_req *req, int err)
{
	req->main_done = true;
	req->result.status = err == ENOTSUP ? PHP_IO_UNSUPPORTED : PHP_IO_DONE;
	req->result.index = req->index;
	req->result.res = -1;
	req->result.error = err == ENOTSUP ? 0 : err;
	php_io_ring_req_complete(ring, req);
}

static void php_io_ring_req_main_cqe(php_io_ring *ring, php_io_ring_req *req, int32_t res);

/* The multishot poll of a record: one entry, its edges come back with IOR_CQE_F_MORE */
static bool php_io_ring_reg_arm(php_io_ring *ring, php_io_ring_reg *rec)
{
	ior_sqe *sqe;
	uint32_t mask = php_io_ring_poll_mask_to_ior(rec->edge);
	if (!php_io_ring_get_sqes(ring, &sqe, 1)) {
		return false;
	}
	php_io_ring_req *req = php_io_ring_req_alloc(ring);
	req->type = PHP_IO_OP_POLL;
	req->multishot = true;
	req->reg = rec;
	php_deadline_init_infinite(&req->deadline);
	ior_prep_poll_multishot(ring->ctx, sqe, (ior_fd_t) rec->fd, mask);
	ior_sqe_set_data(ring->ctx, sqe, req);
	rec->poll = req;
	rec->poll_mask = mask;
	php_io_ring_flush(ring);
	return true;
}

/* The multishot is let go: its last completion frees the entry, the record forgets it */
static void php_io_ring_reg_disarm(php_io_ring *ring, php_io_ring_reg *rec)
{
	php_io_ring_req *req = rec->poll;
	rec->poll = NULL;
	req->reg = NULL;
	if (php_io_ring_foreign(ring)) {
		req->main_done = true;
	} else if (!req->main_done && !req->cancelled) {
		req->cancelled = true;
		if (!php_io_ring_submit_cancel(ring, req)) {
			req->cancel_pending = true;
			ring->n_cancel_pending++;
		}
	}
	req->orphaned = true;
	if (php_io_ring_req_settled(req)) {
		php_io_ring_req_free(ring, req);
	}
}

/* An edge of the record: the parked waits it answers complete, the rest is kept for the next one;
 * a hangup or error answers every wait from now on */
static void php_io_ring_reg_edge(php_io_ring *ring, php_io_ring_reg *rec, uint32_t res)
{
	bool failure = (res & (IOR_POLL_ERR | IOR_POLL_HUP | IOR_POLL_NVAL)) != 0;
	uint32_t consumed = 0;
	if (failure) {
		rec->hup = true;
	}
	php_io_ring_req *w = ring->waiting;
	while (w) {
		php_io_ring_req *next = w->w_next;
		if (w->waiting == rec && (failure || (w->w_mask & res))) {
			consumed |= w->w_mask;
			php_io_ring_waiter_settle(ring, w);
			php_io_ring_req_main_cqe(ring, w, (int32_t) res);
		}
		w = next;
	}
	rec->ready |= res & rec->poll_mask & ~consumed;
}

/* The multishot accept of a registered listener: one entry, its connections come back with
 * IOR_CQE_F_MORE and the accepted socket as the result */
static bool php_io_ring_reg_arm_accept(php_io_ring *ring, php_io_ring_reg *rec)
{
	ior_sqe *sqe;
	if (!php_io_ring_get_sqes(ring, &sqe, 1)) {
		return false;
	}
	php_io_ring_req *req = php_io_ring_req_alloc(ring);
	req->type = PHP_IO_OP_ACCEPT;
	req->multishot = true;
	req->reg = rec;
	php_deadline_init_infinite(&req->deadline);
	ior_prep_accept_multishot(ring->ctx, sqe, (ior_fd_t) rec->fd,
			IOR_ACCEPT_CLOEXEC | (ring->fd_nonblock ? IOR_ACCEPT_NONBLOCK : 0));
	ior_sqe_set_data(ring->ctx, sqe, req);
	rec->accept = req;
	php_io_ring_flush(ring);
	return true;
}

/* Let go for the cap, keeping the record so the connections it still posts are buffered, or for
 * good, when they are closed */
static void php_io_ring_reg_disarm_accept(php_io_ring *ring, php_io_ring_reg *rec, bool keep)
{
	php_io_ring_req *req = rec->accept;
	rec->accept = NULL;
	if (!keep) {
		req->reg = NULL;
	}
	if (php_io_ring_foreign(ring)) {
		req->main_done = true;
	} else if (!req->main_done && !req->cancelled) {
		req->cancelled = true;
		if (!php_io_ring_submit_cancel(ring, req)) {
			req->cancel_pending = true;
			ring->n_cancel_pending++;
		}
	}
	req->orphaned = true;
	if (php_io_ring_req_settled(req)) {
		php_io_ring_req_free(ring, req);
	}
}

/* An Accept completes with a connection: the peer name, which a multishot accept does not report,
 * is asked of the socket when the caller wants it */
static void php_io_ring_accept_complete(php_io_ring *ring, php_io_ring_req *req, int fd)
{
	if (req->u.sock.addr) {
		req->u.sock.addrlen = req->u.sock.cap;
		if (getpeername((php_socket_t) fd, req->u.sock.addr, &req->u.sock.addrlen) != 0) {
			req->u.sock.addrlen = 0;
		}
	}
	php_io_ring_req_main_cqe(ring, req, (int32_t) fd);
}

/* The oldest buffered connection; the multishot comes back once the buffer is half empty, or at
 * the first take after it ended on its own */
static int php_io_ring_reg_take(php_io_ring *ring, php_io_ring_reg *rec)
{
	int fd = rec->fds[0];
	rec->n_fds--;
	memmove(rec->fds, &rec->fds[1], rec->n_fds * sizeof(*rec->fds));
	if (!rec->accept && !rec->accept_failed && rec->n_fds < PHP_IO_RING_ACCEPT_CAP / 2) {
		php_io_ring_reg_arm_accept(ring, rec);
	}
	return fd;
}

/* A connection the multishot accepted: the oldest parked accept takes it, or it is buffered */
static void php_io_ring_reg_connection(php_io_ring *ring, php_io_ring_reg *rec, int fd)
{
	php_io_ring_req *w = ring->waiting, *oldest = NULL;
	while (w) {
		if (w->waiting == rec && w->type == PHP_IO_OP_ACCEPT) {
			oldest = w;
		}
		w = w->w_next;
	}
	if (oldest) {
		php_io_ring_waiter_settle(ring, oldest);
		php_io_ring_accept_complete(ring, oldest, fd);
		return;
	}
	if (rec->n_fds == rec->fds_cap) {
		rec->fds_cap = rec->fds_cap ? rec->fds_cap * 2 : 8;
		rec->fds = safe_erealloc(rec->fds, rec->fds_cap, sizeof(*rec->fds), 0);
	}
	rec->fds[rec->n_fds++] = fd;
	if (rec->accept && rec->n_fds >= PHP_IO_RING_ACCEPT_CAP) {
		php_io_ring_reg_disarm_accept(ring, rec, true);
	}
}

/* An Accept on a registered listener: served from the buffer, or parked behind the multishot
 * accept armed at the first one. False when the accept takes its own entry. */
static bool php_io_ring_accept_wait(php_io_ring *ring, php_io_ring_req *req)
{
	php_io_op *op = req->op;
	if (!op->registration || op->registration->trigger != PHP_IO_TRIGGER_EDGE) {
		return false;
	}
	php_io_ring_reg *rec = php_io_ring_reg_of(ring, op->registration, op->fd);
	if (!rec || !(rec->edge & PHP_POLL_READ) || rec->accept_failed) {
		return false;
	}
	if (rec->n_fds) {
		php_io_ring_accept_complete(ring, req, php_io_ring_reg_take(ring, rec));
		return true;
	}
	if (!rec->accept && !php_io_ring_reg_arm_accept(ring, rec)) {
		return false;
	}
	php_io_ring_waiter_park(ring, req, rec, IOR_POLL_IN);
	return true;
}

/* A wait after a drain on an Edge pair: answered from the record, or parked on it behind the
 * multishot poll armed at the first such wait. False when the wait takes its own entry. */
static bool php_io_ring_edge_wait(php_io_ring *ring, php_io_ring_req *req)
{
	php_io_op *op = req->op;
	if (op->type == PHP_IO_OP_ACCEPT) {
		return php_io_ring_accept_wait(ring, req);
	}
	if (op->type != PHP_IO_OP_POLL || !(op->flags & PHP_IO_OP_F_AFTER_DRAIN) || !op->registration
			|| op->registration->trigger != PHP_IO_TRIGGER_EDGE) {
		return false;
	}
	uint32_t events = op->u.poll.events & (PHP_POLL_READ | PHP_POLL_WRITE);
	php_io_ring_reg *rec = php_io_ring_reg_of(ring, op->registration, op->fd);
	if (!events || !rec || (rec->edge & events) != events) {
		return false;
	}
	uint32_t mask = php_io_ring_poll_mask_to_ior(events);
	if (rec->hup || (rec->ready & mask)) {
		uint32_t res = (rec->ready & mask) | (rec->hup ? IOR_POLL_HUP : 0);
		rec->ready &= ~mask;
		php_io_ring_req_main_cqe(ring, req, (int32_t) res);
		return true;
	}
	if (rec->poll && rec->poll_mask != php_io_ring_poll_mask_to_ior(rec->edge)) {
		/* A direction registered since: the new poll reports what is ready now */
		php_io_ring_reg_disarm(ring, rec);
	}
	if (!rec->poll && !php_io_ring_reg_arm(ring, rec)) {
		return false;
	}
	php_io_ring_waiter_park(ring, req, rec, mask);
	return true;
}

/* Submitted, or kept in the backlog until the ring has room; false when
 * the op failed, errno set */
static bool php_io_ring_start(php_io_ring *ring, php_io_ring_req *req)
{
	if (php_io_ring_edge_wait(ring, req)) {
		return true;
	}
	if (!ring->bl_head && php_io_ring_submit_one(ring, req) == SUCCESS) {
		return true;
	}
	if (ring->bl_head || errno == EBUSY) {
		php_io_ring_backlog_push(ring, req);
		return true;
	}
	return false;
}

static void php_io_ring_flush_backlog(php_io_ring *ring)
{
	while (ring->bl_head) {
		php_io_ring_req *req = ring->bl_head;
		if (php_io_ring_submit_one(ring, req) == SUCCESS) {
			php_io_ring_backlog_remove(ring, req);
			continue;
		}
		if (errno == EBUSY) {
			break;
		}
		int err = errno;
		php_io_ring_backlog_remove(ring, req);
		php_io_ring_req_fail(ring, req, err);
	}
}

PHPAPI zend_result php_io_ring_submit_op(php_io_ring *ring, php_io_op *op, void *data)
{
	if (php_io_ring_foreign(ring)) {
		errno = EPERM;
		return FAILURE;
	}
	if (op->queue_data) {
		errno = EALREADY;
		return FAILURE;
	}

	php_io_ring_req *req = php_io_ring_req_create(ring, op, data);
	op->queue_data = req;
	ring->pending++;

	if (op->type == PHP_IO_OP_ANY) {
		uint32_t n = op->u.any.n;
		op->u.any.n_results = 0;
		req->n_members = n;
		req->members = n ? safe_emalloc(n, sizeof(*req->members), 0) : NULL;
		req->main_done = true; /* the Any itself has no cqe */
		for (uint32_t i = 0; i < n; i++) {
			php_io_ring_req *m = php_io_ring_req_create(ring, op->u.any.ops[i], NULL);
			m->group = req;
			m->index = i;
			op->u.any.ops[i]->queue_data = m;
			req->members[i] = m;
		}
		for (uint32_t i = 0; i < n; i++) {
			php_io_ring_req *m = req->members[i];
			if (!php_io_ring_start(ring, m)) {
				/* A member without a ring form completes at once */
				php_io_ring_req_fail(ring, m, errno);
			}
		}
		if (n == 0) {
			req->fired = true;
			php_io_ring_list_push(&ring->fired, &ring->n_fired, &ring->fired_cap, req);
		}
		return SUCCESS;
	}

	if (!php_io_ring_start(ring, req)) {
		php_io_ring_req_fail(ring, req, errno);
	}
	return SUCCESS;
}

/* Cancel every cqe-producing submission of a record; one still in the
 * backlog never reached the backend and settles at once */
static void php_io_ring_req_cancel(php_io_ring *ring, php_io_ring_req *req)
{
	if (req->backlogged) {
		php_io_ring_backlog_remove(ring, req);
		req->main_done = true;
		return;
	}
	if (req->waiting) {
		/* Never reached the backend: the record's multishot stays */
		php_io_ring_waiter_settle(ring, req);
		req->main_done = true;
		return;
	}
	if (!req->main_done && !req->cancelled) {
		req->cancelled = true;
		if (!php_io_ring_submit_cancel(ring, req)) {
			req->cancel_pending = true;
			ring->n_cancel_pending++;
		}
	}
}

/* Cancels that found no entry, one at a time with a reap in between */
static void php_io_ring_retry_cancels(php_io_ring *ring)
{
	while (ring->n_cancel_pending) {
		php_io_ring_req *target = ring->live;
		while (target && !target->cancel_pending) {
			target = target->next;
		}
		ZEND_ASSERT(target);
		if (!php_io_ring_submit_cancel(ring, target)) {
			return;
		}
		target->cancel_pending = false;
		ring->n_cancel_pending--;
		php_io_ring_reap(ring);
	}
}

/* The record is no longer wanted: drop it now if settled, or let the reap
 * that settles it drop it */
static void php_io_ring_req_release(php_io_ring *ring, php_io_ring_req *req)
{
	req->orphaned = true;
	if (php_io_ring_req_settled(req)) {
		/* Nothing outstanding: no freeze to keep */
		req->orphan_stream = NULL;
	}
	if (req->ready) {
		php_io_ring_list_remove(ring->ready, &ring->n_ready, req);
		req->ready = false;
	}
	if (req->op) {
		req->op->queue_data = NULL;
		req->op->queue = NULL;
		req->op->in_flight = false;
		req->op = NULL;
	}
	if (php_io_ring_req_settled(req)) {
		php_io_ring_req_free(ring, req);
	}
}

/* In a child the record can neither be cancelled nor complete: it is
 * settled here so that the release frees it */
static void php_io_ring_req_cancel_or_forget(php_io_ring *ring, php_io_ring_req *req)
{
	if (php_io_ring_foreign(ring)) {
		if (req->backlogged) {
			php_io_ring_backlog_remove(ring, req);
		}
		if (req->waiting) {
			php_io_ring_waiter_settle(ring, req);
		}
		req->main_done = true;
		req->lt_done = true;
		req->delivered = true;
		req->orphan_stream = NULL;
		return;
	}
	php_io_ring_req_cancel(ring, req);
}

PHPAPI zend_result php_io_ring_cancel(php_io_ring *ring, php_io_op *op)
{
	php_io_ring_req *req = op->queue_data;
	if (!req) {
		errno = ENOENT;
		return FAILURE;
	}
	if (req->group) {
		errno = EINVAL;
		return FAILURE;
	}

	ring->pending--;
	if (op->type == PHP_IO_OP_ANY) {
		if (req->fired) {
			php_io_ring_list_remove(ring->fired, &ring->n_fired, req);
			req->fired = false;
		}
		for (uint32_t i = 0; i < req->n_members; i++) {
			php_io_ring_req *m = req->members[i];
			m->group = NULL;
			php_io_ring_req_cancel_or_forget(ring, m);
			php_io_ring_req_release(ring, m);
		}
		req->n_members = 0;
	} else {
		php_io_ring_req_cancel_or_forget(ring, req);
	}
	php_io_ring_req_release(ring, req);
	return SUCCESS;
}

PHPAPI bool php_io_ring_orphan(php_io_ring *ring, php_io_op *op)
{
	/* The caller's frame is going away; a record still in flight keeps the
	 * stream frozen and finishes silently in a later wait. In a child
	 * nothing completes, so nothing is kept. */
	php_io_ring_req *req = op->queue_data;
	bool keep = req && op->in_flight && op->stream && !req->group
			&& op->type != PHP_IO_OP_ANY && !php_io_ring_req_settled(req)
			&& !php_io_ring_foreign(ring);
	if (keep) {
		req->orphan_stream = op->stream;
	}
	php_io_ring_cancel(ring, op);
	if (keep) {
		op->in_flight = true;
	}
	return keep;
}

/* Completion processing */

static void php_io_ring_result_from_cqe(php_io_ring_req *req, int32_t res)
{
	php_io_op_result *r = &req->result;
	bool dns = req->type == PHP_IO_OP_GETADDRINFO || req->type == PHP_IO_OP_GETNAMEINFO;

	r->index = req->index;
	r->error = 0;
	r->res = res;

	if (res >= 0) {
		r->status = PHP_IO_DONE;
		if (req->type == PHP_IO_OP_POLL) {
			r->res = php_io_ring_poll_mask_from_ior((uint32_t) res);
		} else if (dns && res != 0) {
			/* EAI_* codes are the work result as they are */
			r->error = res;
			r->res = -1;
		}
		return;
	}

	switch (res) {
		case -ETIME:
			r->status = req->type == PHP_IO_OP_TIMER ? PHP_IO_DONE : PHP_IO_TIMEOUT;
			r->res = 0;
			break;
		case -ECANCELED:
			/* A fired linked timeout cancels the guarded op */
			r->status = req->cancelled ? PHP_IO_CANCELLED : (req->has_lt ? PHP_IO_TIMEOUT : PHP_IO_CANCELLED);
			r->res = -1;
			break;
		case -EOPNOTSUPP:
		case -ENOSYS:
#if defined(ENOTSUP) && ENOTSUP != EOPNOTSUPP
		case -ENOTSUP:
#endif
		case -ENOTSOCK:
			r->status = PHP_IO_UNSUPPORTED;
			r->res = -1;
			break;
		default:
			r->status = PHP_IO_DONE;
			/* A negative EAI_* code for the DNS ops */
			r->error = dns ? res : -res;
			r->res = -1;
			break;
	}
}

/* The main cqe of a record arrived */
static void php_io_ring_req_main_cqe(php_io_ring *ring, php_io_ring_req *req, int32_t res)
{
	req->main_done = true;
	req->main_res = res;
	if (req->cancel_pending) {
		req->cancel_pending = false;
		ring->n_cancel_pending--;
	}
	if (req->op) {
		req->op->in_flight = false;
	}

	if (req->orphaned) {
		if (php_io_ring_req_settled(req)) {
			php_io_ring_req_free(ring, req);
		}
		return;
	}

	php_io_ring_result_from_cqe(req, res);
	if (!req->early) {
		php_io_ring_req_complete(ring, req);
	}
}

/* An op still running at its deadline (a work callback, a regular file on a worker) or being
 * cancelled by it: it completes as Timeout now, and its main cqe only releases the record,
 * unless it comes before the completion was delivered */
static void php_io_ring_req_lt_cqe(php_io_ring *ring, php_io_ring_req *req, int32_t res)
{
	req->lt_done = true;
	if (req->orphaned) {
		if (php_io_ring_req_settled(req)) {
			php_io_ring_req_free(ring, req);
		}
		return;
	}
	if (req->main_done || req->cancelled || (res != -EALREADY && res != -ETIME)) {
		return;
	}
	/* Without a queue nothing keeps the stream frozen for the backend */
	if (!ring->queue && req->op && req->op->in_flight && req->op->stream) {
		return;
	}
	req->early = true;
	req->result.index = req->index;
	req->result.status = PHP_IO_TIMEOUT;
	req->result.res = 0;
	req->result.error = 0;
	php_io_ring_req_complete(ring, req);
}

/* Poll members that are ready by now but whose cqe is not there yet (the
 * thread backend posts each from its poller in turn) are reported too */
static void php_io_ring_group_probe(php_io_ring_req *req, bool *probed)
{
	php_pollfd stack[16];
	php_pollfd *fds = stack;
	uint32_t n = 0;

	for (uint32_t i = 0; i < req->n_members; i++) {
		php_io_ring_req *m = req->members[i];
		if (!m->main_done && m->type == PHP_IO_OP_POLL) {
			n++;
		}
	}
	if (n == 0) {
		return;
	}
	if (n > sizeof(stack) / sizeof(stack[0])) {
		fds = safe_emalloc(n, sizeof(*fds), 0);
	}
	n = 0;
	for (uint32_t i = 0; i < req->n_members; i++) {
		php_io_ring_req *m = req->members[i];
		if (!m->main_done && m->type == PHP_IO_OP_POLL) {
			fds[n].fd = m->op->fd;
			fds[n].events = (m->op->u.poll.events & PHP_POLL_READ ? POLLIN : 0)
					| (m->op->u.poll.events & PHP_POLL_WRITE ? POLLOUT : 0);
			fds[n].revents = 0;
			n++;
		}
	}
	if (php_poll2(fds, n, 0) > 0) {
		n = 0;
		for (uint32_t i = 0; i < req->n_members; i++) {
			php_io_ring_req *m = req->members[i];
			if (!m->main_done && m->type == PHP_IO_OP_POLL) {
				short revents = fds[n++].revents;
				if (revents) {
					uint32_t mask = (revents & POLLIN ? IOR_POLL_IN : 0) | (revents & POLLOUT ? IOR_POLL_OUT : 0)
							| (revents & POLLERR ? IOR_POLL_ERR : 0) | (revents & POLLHUP ? IOR_POLL_HUP : 0)
							| (revents & POLLNVAL ? IOR_POLL_NVAL : 0);
					php_io_ring_result_from_cqe(m, (int32_t) mask);
					probed[i] = true;
				}
			}
		}
	}
	if (fds != stack) {
		efree(fds);
	}
}

/* The Any completes with the members that completed by now; the rest are
 * cancelled and their cqes consumed silently */
static void php_io_ring_group_fold(php_io_ring *ring, php_io_ring_req *req)
{
	php_io_op *op = req->op;
	uint32_t n_results = 0;
	bool *probed = ecalloc(MAX(req->n_members, 1), sizeof(bool));

	req->fired = false;
	req->group_done = true;
	php_io_ring_group_probe(req, probed);

	for (uint32_t i = 0; i < req->n_members; i++) {
		php_io_ring_req *m = req->members[i];
		if (m->main_done || m->early || probed[i]) {
			if (op->u.any.results) {
				op->u.any.results[n_results] = m->result;
			}
			php_io_ring_req_output(m, m->op);
			n_results++;
		}
		if (!m->main_done && !m->early) {
			php_io_ring_req_cancel(ring, m);
		}
		m->group = NULL;
		php_io_ring_req_release(ring, m);
	}
	req->n_members = 0;
	op->u.any.n_results = n_results;
	efree(probed);

	req->ready = true;
	req->result.status = PHP_IO_DONE;
	req->result.index = 0;
	req->result.res = 0;
	req->result.error = 0;
	php_io_ring_list_push(&ring->ready, &ring->n_ready, &ring->ready_cap, req);
}

static void php_io_ring_fold_all(php_io_ring *ring)
{
	while (ring->n_fired) {
		php_io_ring_req *req = ring->fired[0];
		php_io_ring_list_remove(ring->fired, &ring->n_fired, req);
		php_io_ring_group_fold(ring, req);
	}
}

/* A multishot poll's completion: an edge of its record, or its last one after a cancel, an error
 * or a full completion queue, after which the record re-arms at the next wait and a possibly
 * missed edge becomes at most one spurious wakeup */
static void php_io_ring_multishot_cqe(php_io_ring *ring, php_io_ring_req *req, int32_t res, bool more)
{
	php_io_ring_reg *rec = req->reg;
	if (req->type == PHP_IO_OP_ACCEPT) {
		/* A connection, buffered or taken; one posted after the record let go is closed */
		if (res >= 0) {
			if (rec) {
				php_io_ring_reg_connection(ring, rec, (int) res);
			} else {
				closesocket((php_socket_t) res);
			}
		}
		if (more) {
			return;
		}
		if (rec) {
			if (rec->accept == req) {
				rec->accept = NULL;
			}
			req->reg = NULL;
			if (res < 0 && res != -ECANCELED) {
				/* Refused (an old kernel, -EMFILE): the parked accepts get the error, later
				 * ones take their own entry */
				rec->accept_failed = true;
				php_io_ring_req *w = ring->waiting;
				while (w) {
					php_io_ring_req *next = w->w_next;
					if (w->waiting == rec && w->type == PHP_IO_OP_ACCEPT) {
						php_io_ring_waiter_settle(ring, w);
						php_io_ring_req_main_cqe(ring, w, res);
					}
					w = next;
				}
			}
		}
	} else if (more) {
		if (rec && res > 0) {
			php_io_ring_reg_edge(ring, rec, (uint32_t) res);
		}
		return;
	} else if (rec) {
		rec->poll = NULL;
		req->reg = NULL;
		php_io_ring_reg_edge(ring, rec, res > 0 ? (uint32_t) res : rec->poll_mask);
	}
	req->main_done = true;
	if (req->cancel_pending) {
		req->cancel_pending = false;
		ring->n_cancel_pending--;
	}
	req->orphaned = true;
	if (php_io_ring_req_settled(req)) {
		php_io_ring_req_free(ring, req);
	}
}

static void php_io_ring_process_cqe(php_io_ring *ring, uintptr_t data, int32_t res, bool more)
{
	php_io_ring_req *req = (php_io_ring_req *) (data & ~PHP_IO_RING_TAG_MASK);
	/* A cancel's own cqe: the target completes on its own and may be freed by
	 * the time it arrives, so it is not looked at */
	if (!req || (data & PHP_IO_RING_TAG_MASK) == PHP_IO_RING_TAG_CANCEL) {
		return;
	}
	if (req->multishot && (data & PHP_IO_RING_TAG_MASK) == 0) {
		php_io_ring_multishot_cqe(ring, req, res, more);
		return;
	}
	switch (data & PHP_IO_RING_TAG_MASK) {
		case PHP_IO_RING_TAG_LT:
			php_io_ring_req_lt_cqe(ring, req, res);
			break;
		default:
			php_io_ring_req_main_cqe(ring, req, res);
			break;
	}
}

/* Take everything the ring has, until a peek finds nothing: the members
 * of an Any that are ready by now are all reported with it. The batch is
 * consumed before it is processed, so a cancel submitted meanwhile finds
 * the room it needs. */
static uint32_t php_io_ring_reap(php_io_ring *ring)
{
	uint32_t total = 0;
	for (;;) {
		unsigned n = ior_peek_batch_cqe(ring->ctx, ring->cqes, ring->cqes_cap);
		if (n == 0) {
			break;
		}
		for (unsigned i = 0; i < n; i++) {
			ring->batch[i].data = (uintptr_t) ior_cqe_get_data(ring->ctx, ring->cqes[i]);
			ring->batch[i].res = ior_cqe_get_res(ring->ctx, ring->cqes[i]);
			ring->batch[i].more = (ior_cqe_get_flags(ring->ctx, ring->cqes[i]) & IOR_CQE_F_MORE) != 0;
		}
		ior_cq_advance(ring->ctx, n);
		for (unsigned i = 0; i < n; i++) {
			php_io_ring_process_cqe(ring, ring->batch[i].data, ring->batch[i].res, ring->batch[i].more);
		}
		total += n;
	}
	php_io_ring_fold_all(ring);
	return total;
}

/* Everything that needs no waiting */
static void php_io_ring_progress(php_io_ring *ring)
{
	php_io_ring_reap(ring);
	php_io_ring_retry_cancels(ring);
	php_io_ring_flush_backlog(ring);
	php_io_ring_flush(ring);
}

/* Ops still in the backlog or parked on a record at their deadline time out
 * here, having no entry to link a timeout to; returns the next such deadline */
static zend_hrtime_t php_io_ring_expire(php_io_ring *ring, zend_hrtime_t now)
{
	zend_hrtime_t next = ZEND_HRTIME_T_MAX;
	php_io_ring_req *r = ring->bl_head;
	while (r) {
		php_io_ring_req *bl_next = r->bl_next;
		if (!php_deadline_is_infinite(&r->deadline)) {
			if (r->deadline.hrtime <= now) {
				php_io_ring_backlog_remove(ring, r);
				php_io_ring_req_main_cqe(ring, r, -ETIME);
			} else if (r->deadline.hrtime < next) {
				next = r->deadline.hrtime;
			}
		}
		r = bl_next;
	}
	r = ring->waiting;
	while (r) {
		php_io_ring_req *w_next = r->w_next;
		if (!php_deadline_is_infinite(&r->deadline)) {
			if (r->deadline.hrtime <= now) {
				php_io_ring_waiter_settle(ring, r);
				php_io_ring_req_main_cqe(ring, r, -ETIME);
			} else if (r->deadline.hrtime < next) {
				next = r->deadline.hrtime;
			}
		}
		r = w_next;
	}
	php_io_ring_fold_all(ring);
	return next;
}

/* Wait until every orphaned op on the stream settled; other completions
 * stay queued for delivery */
PHPAPI void php_io_ring_drain(php_io_ring *ring, php_stream *stream)
{
	if (php_io_ring_foreign(ring)) {
		return;
	}
	for (;;) {
		php_io_ring_progress(ring);
		bool pending = false;
		for (php_io_ring_req *r = ring->live; r; r = r->next) {
			if (r->orphan_stream == stream) {
				pending = true;
				break;
			}
		}
		if (!pending) {
			return;
		}
		ior_cqe *cqe;
		int rc = ior_wait_cqe(ring->ctx, &cqe);
		if (rc < 0 && rc != -EINTR) {
			return;
		}
	}
}

/* A ready record becomes the caller's completion and lets go of the op */
static void php_io_ring_deliver_one(php_io_ring *ring, php_io_ring_req *req, php_io_queue_completion *out)
{
	if (req->type != PHP_IO_OP_ANY) {
		php_io_ring_req_output(req, req->op);
		if (!req->main_done && req->op->in_flight && req->op->stream) {
			/* The backend still uses the stream: it stays frozen until the record settled */
			req->orphan_stream = req->op->stream;
			php_io_stream_orphan(req->op->stream, ring->queue);
		}
	}
	out->op = req->op;
	out->data = req->data;
	out->result = req->result;
	req->ready = false;
	ring->pending--;
	req->op->queue_data = NULL;
	req->op->queue = NULL;
	req->op = NULL;
	req->orphaned = true;
	if (php_io_ring_req_settled(req)) {
		php_io_ring_req_free(ring, req);
	}
}

static uint32_t php_io_ring_deliver(php_io_ring *ring, php_io_queue_completion *out, uint32_t max)
{
	uint32_t n = MIN(max, ring->n_ready);

	for (uint32_t i = 0; i < n; i++) {
		php_io_ring_deliver_one(ring, ring->ready[i], &out[i]);
	}
	memmove(ring->ready, &ring->ready[n], (ring->n_ready - n) * sizeof(*ring->ready));
	ring->n_ready -= n;
	return n;
}

PHPAPI bool php_io_ring_take_inline(php_io_ring *ring, php_io_op *op, php_io_queue_completion *out)
{
	php_io_ring_req *req = op->queue_data;

	if (!req || !req->ready || op->type == PHP_IO_OP_ANY || php_io_ring_foreign(ring)) {
		return false;
	}
	php_io_ring_list_remove(ring->ready, &ring->n_ready, req);
	php_io_ring_deliver_one(ring, req, out);
	return true;
}

PHPAPI int php_io_ring_wait(php_io_ring *ring, php_io_queue_completion *out, uint32_t max, const php_deadline *dl)
{
	if (php_io_ring_foreign(ring)) {
		errno = EPERM;
		return -1;
	}
	zend_hrtime_t limit = dl ? dl->hrtime : ZEND_HRTIME_T_MAX;
	/* Only orphans: once they settled there is nothing to report, as
	 * count_pending() told */
	bool orphans_only = ring->pending == 0 && php_io_ring_settling(ring);

	if (max == 0) {
		return 0;
	}
	if (limit == 0) {
		/* A loop woken by the notification descriptor: clear, then reap until empty */
		php_io_ring_notify_clear(ring);
	}

	for (;;) {
		php_io_ring_progress(ring);
		zend_hrtime_t now = zend_hrtime();
		zend_hrtime_t next = php_io_ring_expire(ring, now);
		if (ring->n_ready) {
			return (int) php_io_ring_deliver(ring, out, max);
		}
		if (limit != ZEND_HRTIME_T_MAX && now >= limit) {
			return 0;
		}
		if (ring->pending == 0) {
			if (php_io_ring_settling(ring)) {
				orphans_only = true;
			} else if (orphans_only) {
				return 0;
			} else if (limit == ZEND_HRTIME_T_MAX) {
				errno = EDEADLK;
				return -1;
			}
		}

		if (ring->unsubmitted) {
			int rc = php_io_ring_flush(ring);
			if (rc < 0 && rc != -EAGAIN && rc != -EBUSY && rc != -EINTR) {
				errno = -rc;
				return -1;
			}
		}

		ior_cqe *cqe;
		int rc;
		zend_hrtime_t until = MIN(limit, next);
		if (until == ZEND_HRTIME_T_MAX) {
			rc = ior_wait_cqe(ring->ctx, &cqe);
		} else {
			now = zend_hrtime();
			zend_hrtime_t remaining = until > now ? until - now : 0;
			ior_timespec ts = { .tv_sec = (int64_t) (remaining / ZEND_NANO_IN_SEC), .tv_nsec = (long long) (remaining % ZEND_NANO_IN_SEC) };
			rc = ior_wait_cqe_timeout(ring->ctx, &cqe, &ts);
		}
		if (rc < 0 && rc != -ETIME && (rc != -EINTR || php_io_interrupt_pending())) {
			errno = -rc;
			return -1;
		}
		/* The next pass consumes it */
	}
}

PHPAPI void php_io_ring_destroy(php_io_ring *ring)
{
	bool foreign = php_io_ring_foreign(ring);

	/* The Edge records go first: their multishots settle like any entry */
	php_io_ring_reg *rec;
	ZEND_HASH_FOREACH_PTR(&ring->regs, rec) {
		if (rec->poll) {
			rec->poll->reg = NULL;
		}
		if (rec->accept) {
			rec->accept->reg = NULL;
		}
		/* In a forked child these are its copies */
		php_io_ring_reg_close_fds(rec);
		efree(rec);
	} ZEND_HASH_FOREACH_END();
	zend_hash_destroy(&ring->regs);

	/* Nobody takes a completion any more: every op forgets its record, as
	 * after a cancel, and every record is an orphan */
	for (php_io_ring_req *r = ring->live; r; r = r->next) {
		if (r->op) {
			r->op->queue = NULL;
			r->op->queue_data = NULL;
			r->op->in_flight = false;
			r->op = NULL;
		}
		if (r->backlogged) {
			php_io_ring_backlog_remove(ring, r);
			r->main_done = true;
		}
		if (r->waiting) {
			php_io_ring_waiter_settle(ring, r);
			r->main_done = true;
		}
		if (foreign) {
			r->main_done = true;
			r->lt_done = true;
			r->delivered = true;
		}
		r->group = NULL;
		r->ready = false;
		r->fired = false;
		r->orphaned = true;
	}
	ring->n_ready = 0;
	ring->n_fired = 0;
	ring->pending = 0;

	/* Cancel everything in flight and drain until each has completed */
	for (;;) {
		php_io_ring_req *r = ring->live;
		while (r) {
			php_io_ring_req *next = r->next;
			if (php_io_ring_req_settled(r)) {
				php_io_ring_req_free(ring, r);
			} else if (!r->cancelled) {
				php_io_ring_req_cancel(ring, r);
			}
			r = next;
		}
		if (!ring->live) {
			break;
		}
		php_io_ring_retry_cancels(ring);
		php_io_ring_flush(ring);
		ior_cqe *cqe;
		int rc = ior_wait_cqe(ring->ctx, &cqe);
		if (rc < 0 && rc != -EINTR) {
			/* The backend may still write into what the records own: they
			 * and the context are leaked rather than freed under it */
			ring->ctx = NULL;
			break;
		}
		php_io_ring_reap(ring);
	}
	if (ring->ctx && !foreign) {
		ior_queue_exit(ring->ctx);
	}
	if (ring->prev_ring) {
		ring->prev_ring->next_ring = ring->next_ring;
	} else {
		php_io_rings = ring->next_ring;
	}
	if (ring->next_ring) {
		ring->next_ring->prev_ring = ring->prev_ring;
	}
	if (ring->ready) {
		efree(ring->ready);
	}
	if (ring->fired) {
		efree(ring->fired);
	}
	efree(ring->cqes);
	efree(ring->batch);
	efree(ring);
}

/* The ring as an operation queue */

typedef struct {
	php_io_queue base;
	php_io_ring *ring;
} php_io_ring_queue;

static zend_result php_io_ring_queue_submit(php_io_queue *base, php_io_op *op, void *data)
{
	php_io_ring_queue *q = (php_io_ring_queue *) base;
	if (op->queue) {
		errno = EALREADY;
		return FAILURE;
	}
	op->queue = base;
	if (op->type == PHP_IO_OP_ANY) {
		for (uint32_t i = 0; i < op->u.any.n; i++) {
			op->u.any.ops[i]->queue = base;
		}
	}
	if (php_io_ring_submit_op(q->ring, op, data) != SUCCESS) {
		op->queue = NULL;
		return FAILURE;
	}
	return SUCCESS;
}

static zend_result php_io_ring_queue_cancel(php_io_queue *base, php_io_op *op)
{
	php_io_ring_queue *q = (php_io_ring_queue *) base;
	if (op->queue != base) {
		errno = ENOENT;
		return FAILURE;
	}
	return php_io_ring_cancel(q->ring, op);
}

static zend_result php_io_ring_queue_add(php_io_queue *base, php_io_registration *reg)
{
	return php_io_ring_add(((php_io_ring_queue *) base)->ring, reg);
}

static void php_io_ring_queue_remove(php_io_queue *base, php_io_registration *reg)
{
	php_io_ring_remove(((php_io_ring_queue *) base)->ring, reg);
}

static bool php_io_ring_queue_take_inline(php_io_queue *base, php_io_op *op, php_io_queue_completion *out)
{
	return php_io_ring_take_inline(((php_io_ring_queue *) base)->ring, op, out);
}

static int php_io_ring_queue_wait(php_io_queue *base, php_io_queue_completion *out, uint32_t max, const php_deadline *dl)
{
	return php_io_ring_wait(((php_io_ring_queue *) base)->ring, out, max, dl);
}

static void php_io_ring_queue_orphan(php_io_queue *base, php_io_op *op)
{
	if (op->queue == base && php_io_ring_orphan(((php_io_ring_queue *) base)->ring, op)) {
		php_io_stream_orphan(op->stream, base);
	}
}

static void php_io_ring_queue_drain(php_io_queue *base, php_stream *stream)
{
	php_io_ring_drain(((php_io_ring_queue *) base)->ring, stream);
}

static uint32_t php_io_ring_queue_count_pending(php_io_queue *base)
{
	return php_io_ring_count_pending(((php_io_ring_queue *) base)->ring);
}

static uint32_t php_io_ring_queue_hook_flags(php_io_queue *base)
{
	return php_io_ring_hook_flags(((php_io_ring_queue *) base)->ring);
}

static void php_io_ring_queue_destroy(php_io_queue *base)
{
	php_io_ring_queue *q = (php_io_ring_queue *) base;
	php_io_ring_destroy(q->ring);
	efree(q);
}

static const php_io_queue_ops php_io_ring_queue_ops = {
	.submit = php_io_ring_queue_submit,
	.take_inline = php_io_ring_queue_take_inline,
	.cancel = php_io_ring_queue_cancel,
	.add = php_io_ring_queue_add,
	.remove = php_io_ring_queue_remove,
	.wait = php_io_ring_queue_wait,
	.orphan = php_io_ring_queue_orphan,
	.drain = php_io_ring_queue_drain,
	.count_pending = php_io_ring_queue_count_pending,
	.hook_flags = php_io_ring_queue_hook_flags,
	.destroy = php_io_ring_queue_destroy,
};

PHPAPI php_io_ring *php_io_queue_ring(php_io_queue *q)
{
	ZEND_ASSERT(q->ops == &php_io_ring_queue_ops);
	return ((php_io_ring_queue *) q)->ring;
}

PHPAPI php_io_queue *php_io_queue_create_ring(uint32_t entries)
{
	/* Ops come only from the core, which makes every descriptor it submits
	 * non-blocking: sockets, accepted ones included, and unseekable plain
	 * files; regular files run to completion either way */
	php_io_ring *ring = php_io_ring_create(entries, true);
	if (!ring) {
		return NULL;
	}
	php_io_ring_queue *q = ecalloc(1, sizeof(*q));
	q->base.ops = &php_io_ring_queue_ops;
	q->ring = ring;
	ring->queue = &q->base;
	return &q->base;
}

#endif /* HAVE_IOR */
