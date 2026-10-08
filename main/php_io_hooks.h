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

#ifndef PHP_IO_HOOKS_H
#define PHP_IO_HOOKS_H

#include "php.h"
#include "php_network.h"
#include "php_streams.h"
#include "php_deadline.h"
#include "main/php_poll.h"

BEGIN_EXTERN_C()

/* Operations */

typedef enum {
	PHP_IO_OP_POLL,
	PHP_IO_OP_TIMER,
	PHP_IO_OP_READ,
	PHP_IO_OP_WRITE,
	PHP_IO_OP_RECV,
	PHP_IO_OP_SEND,
	PHP_IO_OP_ACCEPT,
	PHP_IO_OP_CONNECT,
	PHP_IO_OP_GETADDRINFO,
	PHP_IO_OP_GETNAMEINFO,
	PHP_IO_OP_FSYNC,
	PHP_IO_OP_WAITPID,
	PHP_IO_OP_SIGWAIT,
	PHP_IO_OP_ANY,
} php_io_op_type;

/* READ/RECV: buf is the stream's read buffer, valid until the stream is drained */
#define PHP_IO_OP_F_STREAM_BUF 0x01
/* The wait follows a drain (EAGAIN, a short write): readiness recorded for an Edge pair since
 * then may answer it. Set by the wrappers; a Poll op without it checks readiness at arm time. */
#define PHP_IO_OP_F_AFTER_DRAIN 0x02
/* POLL: the caller checked readiness at arm time (a zero-timeout poll() found none), so a queue
 * need not check a registered pair again; never answered from recorded readiness */
#define PHP_IO_OP_F_CHECKED 0x04
/* CONNECT: the core's connect() is under way, and a second one fails (IOCP's ConnectEx() with
 * WSAEINVAL); the provider waits for it to finish and completes Ready, leaving SO_ERROR to the
 * core, or Done with the connect's errno */
#define PHP_IO_OP_F_CONNECT_STARTED 0x08
/* READ, WRITE, POLL on Windows: fd is the CRT descriptor of an overlapped pipe
 * (php_io_overlapped_pipes), which no socket poll takes. A provider performs such an op, whatever
 * its data flags, or answers Unsupported. A READ completes with the bytes there as soon as there
 * are any, and with 0 or EPIPE at the writer's close; a WRITE to a closed reader fails with EPIPE.
 * A POLL READ is ready at data, READ | HUP at the writer's close, ERROR on a write end; a POLL
 * WRITE is ready at once, since a pipe has no write readiness. A POLL READ member of an Any may
 * come back with in_flight set: its zero-byte read is still in the kernel, and the provider must
 * drain its stream (the queue's drain) before run() returns. */
#define PHP_IO_OP_F_PIPE 0x10

typedef enum {
	PHP_IO_DONE,
	PHP_IO_READY,
	PHP_IO_TIMEOUT,
	PHP_IO_INTERRUPTED,
	PHP_IO_CANCELLED,
	PHP_IO_UNSUPPORTED,
} php_io_status;

typedef struct _php_io_op_result {
	php_io_status status;
	uint32_t index; /* ANY member */
	int64_t res; /* bytes, fd or revents */
	int error; /* errno or EAI_* */
} php_io_op_result;

typedef struct _php_io_op php_io_op;
typedef struct _php_io_queue php_io_queue;
typedef struct _php_io_registration php_io_registration;

/* getaddrinfo() codes Windows lacks */
#ifndef EAI_SYSTEM
# define EAI_SYSTEM EAI_FAIL
#endif
#ifndef EAI_OVERFLOW
# define EAI_OVERFLOW EAI_FAIL
#endif

struct _php_io_op {
	php_io_op_type type;
	uint32_t flags;
	zend_object *handle; /* NULL for TIMER, DNS, ANY and a stream op until php_io_op_get_handle() */
	uint32_t ready_events;
	php_socket_t fd;
	php_deadline deadline;
	union {
		struct { uint32_t events; } poll;
		struct { void *buf; size_t len; int64_t offset; int flags; } io;
		struct { struct sockaddr *addr; socklen_t *addrlen; } accept;
		struct { const struct sockaddr *addr; socklen_t addrlen; } connect;
		struct {
			const char *node;
			const char *service;
			const struct addrinfo *hints;
			struct addrinfo **res;
		} getaddrinfo;
		struct {
			const struct sockaddr *addr;
			socklen_t addrlen;
			int flags;
			char *host;
			size_t hostlen;
			char *service;
			size_t servicelen;
		} getnameinfo;
		struct { bool data_only; } fsync;
		/* fd is the pidfd where available */
		struct { pid_t pid; int options; int *status; } waitpid;
		/* fd is the signalfd, taken a signal a handle recorded */
		struct { const php_sigset_t *set; php_siginfo_t *info; int taken; } sigwait;
		struct {
			php_io_op **ops; /* caller owned */
			uint32_t n;
			php_io_op_result *results;
			uint32_t n_results;
		} any;
	} u;
	php_stream *stream; /* frozen for the op */
	php_io_registration *registration; /* the pair's record for a wait on a registered pair */
	zend_object *zobj; /* Io\Operation wrapper */
	void *provider_data;
	php_io_queue *queue; /* set while submitted */
	void *queue_data;
	bool in_flight; /* the backend references buf or addr */
};

PHPAPI void php_io_op_poll(php_io_op *op, zend_object *handle, php_socket_t fd, uint32_t events,
		php_deadline dl);

/* A read whose bytes cannot be read again */
static zend_always_inline bool php_io_op_read_advances(const php_io_op *op)
{
	return (op->type == PHP_IO_OP_READ || op->type == PHP_IO_OP_RECV) && op->u.io.offset < 0
			&& !(op->type == PHP_IO_OP_RECV && (op->u.io.flags & MSG_PEEK));
}

/* Bytes a read took for nobody */
PHPAPI void php_io_stream_keep_read(php_stream *stream, bool in_buffer, ssize_t res);
PHPAPI void php_io_op_timer(php_io_op *op, php_deadline dl);
PHPAPI void php_io_op_read(php_io_op *op, zend_object *handle, php_socket_t fd, void *buf, size_t len,
		int64_t off, php_deadline dl);
PHPAPI void php_io_op_write(php_io_op *op, zend_object *handle, php_socket_t fd, const void *buf,
		size_t len, int64_t off, php_deadline dl);
PHPAPI void php_io_op_recv(php_io_op *op, zend_object *handle, php_socket_t fd, void *buf, size_t len,
		int flags, php_deadline dl);
PHPAPI void php_io_op_send(php_io_op *op, zend_object *handle, php_socket_t fd, const void *buf,
		size_t len, int flags, php_deadline dl);
PHPAPI void php_io_op_accept(php_io_op *op, zend_object *handle, php_socket_t fd,
		struct sockaddr *addr, socklen_t *addrlen, php_deadline dl);
PHPAPI void php_io_op_connect(php_io_op *op, zend_object *handle, php_socket_t fd,
		const struct sockaddr *addr, socklen_t addrlen, php_deadline dl);
PHPAPI void php_io_op_getaddrinfo(php_io_op *op, const char *node, const char *service,
		const struct addrinfo *hints, struct addrinfo **res, php_deadline dl);
PHPAPI void php_io_op_getnameinfo(php_io_op *op, const struct sockaddr *addr, socklen_t addrlen,
		int flags, char *host, size_t hostlen, char *service, size_t servicelen, php_deadline dl);
PHPAPI void php_io_op_fsync(php_io_op *op, zend_object *handle, php_socket_t fd, bool data_only);
PHPAPI void php_io_op_waitpid(php_io_op *op, zend_object *handle, pid_t pid, int options, int *status,
		php_deadline dl);
PHPAPI void php_io_op_sigwait(php_io_op *op, zend_object *handle, const php_sigset_t *set,
		php_siginfo_t *info, php_deadline dl);
PHPAPI void php_io_op_any(php_io_op *op, php_io_op **members, uint32_t n, php_io_op_result *results);

/* The op's handle, created for a stream op on the first call and kept by the stream: borrowed */
PHPAPI zend_object *php_io_op_get_handle(php_io_op *op);

/* Registrations: a (descriptor, event) pair whose waits repeat until the pair is removed */

typedef enum {
	PHP_IO_TRIGGER_LEVEL, /* waits need readiness current at arm time */
	PHP_IO_TRIGGER_EDGE, /* waits follow a drain; recorded readiness may answer them */
} php_io_trigger;

/* Owned by the core, kept on the registrant (a stream, or the handle of a registrant without one),
 * stable between the provider's add() and remove(). */
struct _php_io_registration {
	php_socket_t fd;
	uint32_t event; /* PHP_POLL_READ or PHP_POLL_WRITE */
	php_io_trigger trigger;
	php_stream *stream; /* the registrant, for a stream; NULL otherwise */
	php_poll_handle_object *handle; /* referenced; for a stream NULL until asked */
	zend_object *zobj; /* Io\Registration wrapper, created lazily, detached at unregister */
	void *provider_data; /* never read by the core */
	void *queue_data; /* the queue's record of the pair, never read by the core */
	uint64_t queue_id; /* the queue that set queue_data, never read by the core */
	uint32_t generation; /* the provider it was added to */
	bool in_add; /* the provider's add() is running */
	bool dead; /* unregistered inside add(), freed when it returns */
	php_io_registration *next; /* the registrant's list */
	php_io_registration *gprev; /* FG(io_registrations) */
	php_io_registration *gnext;
};

/* Idempotent per pair: the provider's add() runs at most once per pair and provider, and only
 * when its flags carry the trigger's capability; an Edge pair a provider takes as Level only is
 * registered as Level. NULL when the pair is not registered. The stream form takes the descriptor
 * the wait is on, since a cast may touch the buffers, and creates no handle. */
PHPAPI php_io_registration *php_io_register_stream(php_stream *stream, php_socket_t fd, uint32_t event,
		php_io_trigger trigger);
PHPAPI php_io_registration *php_io_register_handle(php_poll_handle_object *handle, uint32_t event,
		php_io_trigger trigger);
PHPAPI void php_io_unregister(php_io_registration *reg);
/* Every record of a registrant's list; before the descriptor closes */
PHPAPI void php_io_unregister_all(php_io_registration **list);
/* The registrant's handle, created for a stream on the first call; borrowed */
PHPAPI php_poll_handle_object *php_io_registration_get_handle(php_io_registration *reg);

/* Invalidates the Io\Registration wrapper of a record that ended */
PHPAPI extern void (*php_io_registration_zobj_detach)(zend_object *zobj);

/* Hooks */

/* Regular file ops and Fsync go to the provider */
#define PHP_IO_HOOKS_F_FILES               0x01
/* Read, Write, Recv, Send and Connect are submitted without trying the syscall first */
#define PHP_IO_HOOKS_F_DIRECT_DATA         0x02
/* Accept is submitted without trying accept() first */
#define PHP_IO_HOOKS_F_DIRECT_ACCEPT       0x04
/* add() and remove() for Edge and for Level registrations */
#define PHP_IO_HOOKS_F_EDGE_REGISTRATIONS  0x08
#define PHP_IO_HOOKS_F_LEVEL_REGISTRATIONS 0x10

typedef struct _php_io_hooks php_io_hooks;

typedef struct _php_io_hooks_ops {
	/* May suspend; FAILURE means EG(exception) is set */
	zend_result (*run)(php_io_hooks *hooks, php_io_op *op, php_io_op_result *result);
	/* Called only with the capability for the trigger; may be NULL without either */
	void (*add)(php_io_hooks *hooks, php_io_registration *reg);
	void (*remove)(php_io_hooks *hooks, php_io_registration *reg);
	/* Unregistered or replaced: its registrations end without remove() */
	void (*dtor)(php_io_hooks *hooks);
} php_io_hooks_ops;

/* Embedded in the provider's own struct, which finds itself with ZEND_CONTAINER_OF */
struct _php_io_hooks {
	const php_io_hooks_ops *ops;
	uint32_t flags; /* PHP_IO_HOOKS_F_* */
};

/* NULL uninstalls. Fails if a provider is installed, and while its add, remove or dtor runs. */
PHPAPI zend_result php_io_hooks_register(php_io_hooks *hooks);
PHPAPI php_io_hooks *php_io_hooks_current(void);
PHPAPI bool php_io_hooks_active(void);
PHPAPI void php_io_hooks_request_shutdown(void);
/* No set_hooks() and no fiber switch until unlocked */
PHPAPI void php_io_hooks_lock(void);
PHPAPI void php_io_hooks_unlock(void);

/* Invalidates the Io\Operation wrapper of an op that ended */
PHPAPI extern void (*php_io_op_zobj_detach)(zend_object *zobj);

/* Set by pcntl: a signal waits for its PHP handler */
PHPAPI extern bool (*php_io_signal_pending)(void);
/* Whether a wait woken by a signal gives up rather than restarts */
PHPAPI bool php_io_interrupt_pending(void);

#ifdef PHP_WIN32
/* Set at MINIT by the extension that installs the provider: proc_open() then makes overlapped
 * named pipes, whose ops a provider may perform (PHP_IO_OP_F_PIPE), and php_select() judges every
 * pipe by its bytes. False keeps anonymous pipes. */
PHPAPI extern bool php_io_overlapped_pipes;
/* An overlapped pipe's buffer each way, and the most one write through a provider moves: a
 * cancelled write leaves the reader an unknown part of it */
#define PHP_IO_PIPE_BUFFER_SIZE (64 * 1024)
#endif

/* Without a provider it waits itself */
PHPAPI zend_result php_io_run(php_io_op *op, php_io_op_result *result);

/* The wrappers return like the syscall, with errno ETIMEDOUT or ECANCELED. ECANCELED with
 * EG(exception) set means the op was abandoned, or never started, because of that exception, and
 * the exception reports it; without one, the provider completed the op as Cancelled, a failure like
 * any other errno. stream may be NULL; a stream is frozen for the call. On Windows
 * php_socket_errno() is the Winsock code, see PHP_IO_SOCK_*. */
#ifdef PHP_WIN32
# define PHP_IO_SOCK_ETIMEDOUT WSAETIMEDOUT
# define PHP_IO_SOCK_ECANCELED WSAECANCELLED
# define PHP_IO_SOCK_EINTR     WSAEINTR
#else
# define PHP_IO_SOCK_ETIMEDOUT ETIMEDOUT
# define PHP_IO_SOCK_ECANCELED ECANCELED
# define PHP_IO_SOCK_EINTR     EINTR
#endif

/* revents, 0 on timeout, -1 on error */
PHPAPI int php_io_poll(php_stream *stream, php_socket_t fd, uint32_t events, php_deadline *dl);
PHPAPI ssize_t php_io_recv(php_stream *stream, php_socket_t fd, void *buf, size_t len, int flags,
		php_deadline *dl);
PHPAPI ssize_t php_io_send(php_stream *stream, php_socket_t fd, const void *buf, size_t len, int flags,
		php_deadline *dl);
/* The _ex forms take the handle of a registrant that is not a stream (ext/sockets' Socket): the op
 * carries it and a wait registers the pair on it. A stream call passes NULL. */
PHPAPI int php_io_poll_ex(php_stream *stream, zend_object *handle, php_socket_t fd, uint32_t events,
		php_deadline *dl, uint32_t op_flags);
PHPAPI ssize_t php_io_recv_ex(php_stream *stream, zend_object *handle, php_socket_t fd, void *buf,
		size_t len, int flags, php_deadline *dl);
PHPAPI ssize_t php_io_send_ex(php_stream *stream, zend_object *handle, php_socket_t fd, const void *buf,
		size_t len, int flags, php_deadline *dl);
PHPAPI php_socket_t php_io_accept_ex(php_stream *stream, zend_object *handle, php_socket_t fd,
		struct sockaddr *addr, socklen_t *addrlen, php_deadline *dl);
PHPAPI int php_io_connect_ex(php_stream *stream, zend_object *handle, php_socket_t fd,
		const struct sockaddr *addr, socklen_t addrlen, php_deadline *dl);
PHPAPI ssize_t php_io_sendto_ex(php_stream *stream, zend_object *handle, php_socket_t fd,
		const void *buf, size_t len, int flags, const struct sockaddr *addr, socklen_t addrlen,
		php_deadline *dl);
PHPAPI ssize_t php_io_recvfrom_ex(php_stream *stream, zend_object *handle, php_socket_t fd, void *buf,
		size_t len, int flags, struct sockaddr *addr, socklen_t *addrlen, php_deadline *dl);
PHPAPI ssize_t php_io_read(php_stream *stream, int fd, void *buf, size_t len, php_deadline *dl);
PHPAPI ssize_t php_io_write(php_stream *stream, int fd, const void *buf, size_t len, php_deadline *dl);
/* At an explicit offset, for a Windows overlapped file; -1 is the current position */
PHPAPI ssize_t php_io_read_at(php_stream *stream, int fd, void *buf, size_t len, int64_t offset,
		php_deadline *dl);
PHPAPI ssize_t php_io_write_at(php_stream *stream, int fd, const void *buf, size_t len, int64_t offset,
		php_deadline *dl);
#ifdef PHP_WIN32
/* An overlapped pipe (PHP_IO_OP_F_PIPE). A read returns the bytes there, waiting for some unless dl
 * is non-blocking (EAGAIN; any other dl waits without limit); 0 is the writer's close. A write
 * through the provider moves at most PHP_IO_PIPE_BUFFER_SIZE, the thread's write the whole count; a
 * closed reader is EPIPE. Without hooks, or when the provider answers Unsupported, both block the
 * thread. no_provider: the descriptor was handed to another process. */
PHPAPI ssize_t php_io_pipe_read(php_stream *stream, int fd, void *buf, size_t len,
		const php_deadline *dl, bool no_provider);
PHPAPI ssize_t php_io_pipe_write(php_stream *stream, int fd, const void *buf, size_t len,
		bool no_provider);
/* What a select reports for a pipe in its read set: bytes in it, or a peek failure (the writer's
 * close, a write end) that the read reports */
PHPAPI bool php_io_pipe_readable(int fd);
#endif
PHPAPI php_socket_t php_io_accept(php_stream *stream, php_socket_t fd, struct sockaddr *addr,
		socklen_t *addrlen, php_deadline *dl);
PHPAPI int php_io_connect(php_stream *stream, php_socket_t fd, const struct sockaddr *addr,
		socklen_t addrlen, php_deadline *dl);
/* Poll and retry on EAGAIN; a NULL deadline is the plain syscall, addr NULL is send() and recv() */
PHPAPI ssize_t php_io_sendto(php_stream *stream, php_socket_t fd, const void *buf, size_t len,
		int flags, const struct sockaddr *addr, socklen_t addrlen, php_deadline *dl);
PHPAPI ssize_t php_io_recvfrom(php_stream *stream, php_socket_t fd, void *buf, size_t len,
		int flags, struct sockaddr *addr, socklen_t *addrlen, php_deadline *dl);
PHPAPI int php_io_fsync(php_stream *stream, int fd, bool data_only);
/* Returns the EAI_* code; the list must be freed with php_io_freeaddrinfo() */
PHPAPI int php_io_getaddrinfo(const char *node, const char *service, const struct addrinfo *hints,
		struct addrinfo **res, php_deadline *dl);
PHPAPI void php_io_freeaddrinfo(struct addrinfo *res);
/* A provider's list: each entry one malloc() block with the address behind the addrinfo */
PHPAPI void php_io_addrinfo_register(struct addrinfo *head);
PHPAPI void php_io_addrinfo_free_list(struct addrinfo *head);
PHPAPI int php_io_getnameinfo(const struct sockaddr *addr, socklen_t addrlen, int flags, char *host,
		size_t hostlen, char *service, size_t servicelen, php_deadline *dl);
/* FAILURE when cancelled; interrupted may be NULL */
PHPAPI zend_result php_io_sleep(php_deadline dl, bool *interrupted);
/* On Windows pid names any process, options are ignored, a non-blocking deadline asks without
 * waiting and status receives the exit code */
PHPAPI pid_t php_io_waitpid(zend_object *handle, pid_t pid, int *status, int options,
		php_deadline *dl);
#ifndef PHP_WIN32
/* A timed out wait fails with EAGAIN */
PHPAPI int php_io_sigwait(zend_object *handle, const php_sigset_t *set, php_siginfo_t *info,
		php_deadline *dl);
#endif

/* Active php_io_run() frames */
PHPAPI uint32_t php_io_ops_in_flight(void);

/* pid as fork() returned it: the child drops what the parent's ring had */
PHPAPI void php_io_child_forget(pid_t pid);

/* A stream whose in-flight op a queue kept after its frame ended */
PHPAPI void php_io_stream_orphan(php_stream *stream, php_io_queue *queue);
PHPAPI void php_io_stream_unfreeze(php_stream *stream);
PHPAPI void php_io_stream_drain(php_stream *stream);
#ifdef PHP_WIN32
/* php_io_stream_drain() for a stream that lives on: its descriptor goes to the thread or another
 * process */
PHPAPI void php_io_stream_settle(php_stream *stream);
#endif
PHPAPI void php_io_handle_orphan(zend_object *handle, php_io_queue *queue);
PHPAPI void php_io_handle_unfreeze(zend_object *handle);
PHPAPI bool php_io_handle_busy(zend_object *handle);
PHPAPI void php_io_handle_drain(zend_object *handle);
/* Frozen by a running op, not only by an orphan */
PHPAPI bool php_io_stream_busy(php_stream *stream);

/* NULL or a negative tv_sec means no timeout */

static inline php_deadline php_io_deadline_from_timeval(const struct timeval *tv)
{
	php_deadline dl;
	if (tv == NULL || tv->tv_sec < 0) {
		php_deadline_init_infinite(&dl);
	} else if (tv->tv_usec < 0 || tv->tv_usec > 999999) {
		/* Out of range microseconds count against the seconds, never below zero */
		struct timeval norm = {
			.tv_sec = tv->tv_sec + tv->tv_usec / 1000000,
			.tv_usec = tv->tv_usec % 1000000
		};
		if (norm.tv_usec < 0) {
			norm.tv_usec += 1000000;
			norm.tv_sec--;
		}
		if (norm.tv_sec < 0) {
			norm.tv_sec = norm.tv_usec = 0;
		}
		php_deadline_init(&dl, &norm);
	} else {
		php_deadline_init(&dl, (struct timeval *) tv);
	}
	return dl;
}

static inline int php_io_poll_tv(php_stream *stream, php_socket_t fd, uint32_t events,
		const struct timeval *tv)
{
	php_deadline dl = php_io_deadline_from_timeval(tv);
	return php_io_poll(stream, fd, events, &dl);
}

/* Operation queues */

typedef struct _php_io_queue_completion {
	php_io_op *op;
	void *data;
	php_io_op_result result;
} php_io_queue_completion;

typedef struct _php_io_queue_ops {
	zend_result (*submit)(php_io_queue *q, php_io_op *op, void *data);
	/* An op (not an Any) the queue completed at submit: taken out of the completions to deliver,
	 * so the caller needs no wait for it. False when the op is in flight. */
	bool (*take_inline)(php_io_queue *q, php_io_op *op, php_io_queue_completion *out);
	zend_result (*cancel)(php_io_queue *q, php_io_op *op);
	/* Bracket a registration; what the queue retains for the pair goes to reg->queue_data */
	zend_result (*add)(php_io_queue *q, php_io_registration *reg);
	void (*remove)(php_io_queue *q, php_io_registration *reg);
	/* Readiness the queue holds for a registered pair itself, which no poll of the descriptor
	 * reports (the connections a multishot accept took): the events among those asked that it
	 * answers now. May be NULL. */
	uint32_t (*held)(php_io_queue *q, php_io_registration *reg, uint32_t events);
	/* NULL waits for good; the non-blocking deadline is one reap that never blocks */
	int (*wait)(php_io_queue *q, php_io_queue_completion *out, uint32_t max, const php_deadline *dl);
	void (*orphan)(php_io_queue *q, php_io_op *op);
	/* May be NULL when orphan() never keeps an op in flight */
	void (*drain)(php_io_queue *q, const void *owner);
	uint32_t (*count_pending)(php_io_queue *q);
	uint32_t (*hook_flags)(php_io_queue *q);
	void (*destroy)(php_io_queue *q);
	/* A descriptor the queue's Read and Write ops use is about to go to another process: the queue's
	 * backend lets go of it unless an op of the queue is still on it. May be NULL. */
	void (*release)(php_io_queue *q, php_socket_t fd);
} php_io_queue_ops;

struct _php_io_queue {
	const php_io_queue_ops *ops;
	uint64_t id; /* the one it leaves on a registration, see php_io_queue_attach() */
	php_io_queue *prev;
	php_io_queue *next;
};

PHPAPI php_io_queue *php_io_queue_create_poll(php_poll_backend_type backend);
/* Never repeats within a thread: a queue tells its own record on a registration from one a queue
 * before it left there */
PHPAPI uint64_t php_io_queue_new_id(void);
/* A queue is found by the id it leaves on a registration from attach() until detach() */
PHPAPI void php_io_queue_attach(php_io_queue *q, uint64_t id);
PHPAPI void php_io_queue_detach(php_io_queue *q);
PHPAPI php_io_queue *php_io_queue_find(uint64_t id);
/* release() on every queue of the thread */
PHPAPI void php_io_queues_release(php_socket_t fd);
/* The events among those asked that the queues of a registrant's pairs hold themselves, for a
 * readiness check made with a syscall */
PHPAPI uint32_t php_io_held_events(php_io_registration *regs, uint32_t events);

static inline php_deadline php_io_deadline_from_ms(zend_long ms)
{
	php_deadline dl;
	if (ms < 0) {
		php_deadline_init_infinite(&dl);
	} else {
		struct timeval tv = { .tv_sec = ms / 1000, .tv_usec = (ms % 1000) * 1000 };
		php_deadline_init(&dl, &tv);
	}
	return dl;
}

static inline php_deadline php_io_deadline_from_ns(zend_hrtime_t ns)
{
	php_deadline dl;
	dl.hrtime = zend_hrtime();
	if (ns >= ZEND_HRTIME_T_MAX - dl.hrtime) {
		php_deadline_init_infinite(&dl);
	} else {
		dl.hrtime += ns;
	}
	return dl;
}

static inline php_deadline php_io_deadline_infinite(void)
{
	php_deadline dl;
	php_deadline_init_infinite(&dl);
	return dl;
}

/* Only for finite deadlines */
static inline zend_hrtime_t php_io_deadline_remaining(const php_deadline *dl, zend_hrtime_t now)
{
	return dl->hrtime > now ? dl->hrtime - now : 0;
}

END_EXTERN_C()

#endif /* PHP_IO_HOOKS_H */
