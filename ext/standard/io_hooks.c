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

/* The userland bridge of the IO hooks: Io\Operation wrappers over the C
 * ops, Io\Completion, Io\Poll\OperationQueue over the C poll queue, and the
 * Io\Hooks\Hooks adapter that set_hooks() installs as the C provider. */

#include "php.h"
#include "zend_enum.h"
#include "zend_exceptions.h"
#include "zend_interfaces.h"
#include "ext/standard/file.h"
#include "ext/standard/io_poll.h"
#include "ext/standard/io_hooks.h"
#include "ext/date/php_time.h"
#include "io_hooks_arginfo.h"
#include <signal.h>
#include "io_hooks_decl.h"
#include "io_poll_decl.h"

#include <errno.h>
#ifndef PHP_WIN32
# include <netdb.h>
# include <arpa/inet.h>
#endif

static zend_class_entry *php_io_completion_status_ce;
static zend_class_entry *php_io_operation_ce;
static zend_class_entry *php_io_operation_poll_ce;
static zend_class_entry *php_io_operation_timer_ce;
static zend_class_entry *php_io_operation_any_ce;
static zend_class_entry *php_io_operation_read_ce;
static zend_class_entry *php_io_operation_write_ce;
static zend_class_entry *php_io_operation_recv_ce;
static zend_class_entry *php_io_operation_send_ce;
static zend_class_entry *php_io_operation_accept_ce;
static zend_class_entry *php_io_operation_connect_ce;
static zend_class_entry *php_io_operation_fsync_ce;
static zend_class_entry *php_io_operation_waitpid_ce;
static zend_class_entry *php_io_operation_sigwait_ce;
static zend_class_entry *php_io_operation_getaddrinfo_ce;
static zend_class_entry *php_io_operation_getnameinfo_ce;
static zend_class_entry *php_io_completion_ce;
static zend_class_entry *php_io_invalid_operation_exception_ce;
static zend_class_entry *php_io_registration_ce;
static zend_class_entry *php_io_invalid_registration_exception_ce;
static zend_class_entry *php_io_poll_trigger_ce;
PHPAPI zend_class_entry *php_io_operation_queue_ce;
static zend_class_entry *php_io_poll_operation_queue_ce;
static zend_class_entry *php_io_hooks_ce;
static zend_class_entry *php_io_hooks_capability_ce;
static zend_class_entry *php_io_poll_context_ce;

static zend_object_handlers php_io_operation_handlers;
static zend_object_handlers php_io_registration_handlers;
static zend_object_handlers php_io_completion_handlers;
PHPAPI zend_object_handlers php_io_opqueue_handlers;

typedef struct {
	php_io_op *op; /* NULL once ended */
	zend_object *lazy_handle; /* created by getHandle() */
	zend_object std;
} php_io_operation_obj;

typedef struct {
	php_io_registration *reg; /* NULL once ended */
	zend_object std;
} php_io_registration_obj;

typedef struct {
	zend_object *operation;
	php_io_status status;
	int64_t res;
	int error;
	uint32_t events;
	bool produced; /* the result carries real data */
	zval data;
	zval completions; /* Any only */
	zend_object std;
} php_io_completion_obj;

/* One submission: what comes back as the completion's operation and data */
struct _php_io_opqueue_sub {
	zend_object *operation;
	zval data;
	php_io_opqueue_obj *owner;
	php_io_opqueue_sub *prev;
	php_io_opqueue_sub *next;
};

#define PHP_IO_OPERATION_FROM_ZOBJ(o) ZEND_CONTAINER_OF(o, php_io_operation_obj, std)
#define PHP_IO_REGISTRATION_FROM_ZOBJ(o) ZEND_CONTAINER_OF(o, php_io_registration_obj, std)
#define PHP_IO_COMPLETION_FROM_ZOBJ(o) ZEND_CONTAINER_OF(o, php_io_completion_obj, std)

/* Completion status enum */

/* The enum cases are declared in the order of php_io_status */
static_assert(ZEND_ENUM_Io_CompletionStatus_Unsupported - ZEND_ENUM_Io_CompletionStatus_Done == PHP_IO_UNSUPPORTED - PHP_IO_DONE,
		"Io\\CompletionStatus must mirror php_io_status");

static zend_object *php_io_status_case(php_io_status status)
{
	ZEND_ASSERT(status <= PHP_IO_UNSUPPORTED);
	return zend_enum_get_case_by_id(php_io_completion_status_ce, ZEND_ENUM_Io_CompletionStatus_Done + status);
}

static php_io_status php_io_status_from_case(zend_object *case_obj)
{
	return (php_io_status) (zend_enum_fetch_case_id(case_obj) - ZEND_ENUM_Io_CompletionStatus_Done);
}

/* Operation objects */

static zend_object *php_io_operation_create_object(zend_class_entry *ce)
{
	php_io_operation_obj *intern = zend_object_alloc(sizeof(php_io_operation_obj), ce);
	zend_object_std_init(&intern->std, ce);
	object_properties_init(&intern->std, ce);
	intern->op = NULL;
	intern->lazy_handle = NULL;
	return &intern->std;
}

static void php_io_operation_free_object(zend_object *obj)
{
	php_io_operation_obj *intern = PHP_IO_OPERATION_FROM_ZOBJ(obj);
	if (intern->lazy_handle) {
		OBJ_RELEASE(intern->lazy_handle);
	}
	zend_object_std_dtor(&intern->std);
}

static zend_class_entry *php_io_operation_ce_for(php_io_op_type type)
{
	switch (type) {
		case PHP_IO_OP_POLL: return php_io_operation_poll_ce;
		case PHP_IO_OP_TIMER: return php_io_operation_timer_ce;
		case PHP_IO_OP_ANY: return php_io_operation_any_ce;
		case PHP_IO_OP_READ: return php_io_operation_read_ce;
		case PHP_IO_OP_WRITE: return php_io_operation_write_ce;
		case PHP_IO_OP_RECV: return php_io_operation_recv_ce;
		case PHP_IO_OP_SEND: return php_io_operation_send_ce;
		case PHP_IO_OP_ACCEPT: return php_io_operation_accept_ce;
		case PHP_IO_OP_CONNECT: return php_io_operation_connect_ce;
		case PHP_IO_OP_FSYNC: return php_io_operation_fsync_ce;
		case PHP_IO_OP_WAITPID: return php_io_operation_waitpid_ce;
		case PHP_IO_OP_SIGWAIT: return php_io_operation_sigwait_ce;
		case PHP_IO_OP_GETADDRINFO: return php_io_operation_getaddrinfo_ce;
		case PHP_IO_OP_GETNAMEINFO: return php_io_operation_getnameinfo_ce;
		default: return php_io_operation_ce;
	}
}

PHPAPI zend_object *php_io_operation_get_zobj(php_io_op *op)
{
	if (!op->zobj) {
		/* Through the handler directly: the base class is abstract and the
		 * constructors are private, the core is the only creator */
		zend_object *zobj = php_io_operation_create_object(php_io_operation_ce_for(op->type));
		PHP_IO_OPERATION_FROM_ZOBJ(zobj)->op = op;
		op->zobj = zobj;
	}
	return op->zobj;
}

static void php_io_opqueue_sub_unlink(php_io_opqueue_obj *q, php_io_opqueue_sub *sub);
static void php_io_opqueue_sub_free(php_io_opqueue_sub *sub);
static void php_io_opqueue_completion_to_zval(zval *rv, php_io_queue_completion *c);

static void php_io_operation_detach(zend_object *zobj)
{
	php_io_operation_obj *intern = PHP_IO_OPERATION_FROM_ZOBJ(zobj);
	php_io_op *op = intern->op;
	intern->op = NULL;
	/* Ended without a delivery (cancelled, orphaned): the submission
	 * record of a queue of ours goes with it, or it would keep the
	 * operation and its data alive for the life of the queue */
	if (op && op->provider_data) {
		php_io_opqueue_sub *sub = op->provider_data;
		op->provider_data = NULL;
		php_io_opqueue_sub_unlink(sub->owner, sub);
		php_io_opqueue_sub_free(sub);
	}
	/* What a signal handle consumed for this wait goes back with the op */
	if (op && op->type == PHP_IO_OP_SIGWAIT && op->u.sigwait.taken == 0) {
		zend_object *handle = intern->lazy_handle ? intern->lazy_handle : op->handle;
		if (handle) {
			op->u.sigwait.taken = php_io_poll_signal_handle_take(handle, op->u.sigwait.set, op->u.sigwait.info);
		}
	}
}

static php_io_op *php_io_operation_fetch(zval *zv)
{
	php_io_operation_obj *intern = PHP_IO_OPERATION_FROM_ZOBJ(Z_OBJ_P(zv));
	if (!intern->op) {
		zend_throw_exception(php_io_invalid_operation_exception_ce, "The operation has ended", 0);
	}
	return intern->op;
}

static uint32_t php_io_op_events(php_io_op *op)
{
	return op->type == PHP_IO_OP_POLL ? op->u.poll.events : op->ready_events;
}

/* Registration objects */

static zend_object *php_io_registration_create_object(zend_class_entry *ce)
{
	php_io_registration_obj *intern = zend_object_alloc(sizeof(php_io_registration_obj), ce);
	zend_object_std_init(&intern->std, ce);
	object_properties_init(&intern->std, ce);
	intern->reg = NULL;
	return &intern->std;
}

/* The same object for the record's life: created at the first userland add() or
 * getRegistration(), released by the core when the record ends */
static zend_object *php_io_registration_get_zobj(php_io_registration *reg)
{
	if (!reg->zobj) {
		zend_object *zobj = php_io_registration_create_object(php_io_registration_ce);
		PHP_IO_REGISTRATION_FROM_ZOBJ(zobj)->reg = reg;
		reg->zobj = zobj;
	}
	return reg->zobj;
}

static void php_io_registration_detach(zend_object *zobj)
{
	PHP_IO_REGISTRATION_FROM_ZOBJ(zobj)->reg = NULL;
}

static php_io_registration *php_io_registration_fetch(zval *zv)
{
	php_io_registration_obj *intern = PHP_IO_REGISTRATION_FROM_ZOBJ(Z_OBJ_P(zv));
	if (!intern->reg) {
		zend_throw_exception(php_io_invalid_registration_exception_ce, "The registration has ended", 0);
	}
	return intern->reg;
}

PHP_METHOD(Io_Registration, __construct)
{
	zend_throw_error(NULL, "Registrations are created by the engine");
}

PHP_METHOD(Io_Registration, getHandle)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_io_registration *reg = php_io_registration_fetch(ZEND_THIS);
	if (!reg) {
		RETURN_THROWS();
	}
	RETURN_OBJ_COPY(&php_io_registration_get_handle(reg)->std);
}

PHP_METHOD(Io_Registration, getEvent)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_io_registration *reg = php_io_registration_fetch(ZEND_THIS);
	if (!reg) {
		RETURN_THROWS();
	}
	RETURN_OBJ_COPY(zend_enum_get_case_by_id(php_io_poll_event_class_entry,
			reg->event == PHP_POLL_READ ? ZEND_ENUM_Io_Poll_Event_Read : ZEND_ENUM_Io_Poll_Event_Write));
}

PHP_METHOD(Io_Registration, getTrigger)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_io_registration *reg = php_io_registration_fetch(ZEND_THIS);
	if (!reg) {
		RETURN_THROWS();
	}
	RETURN_OBJ_COPY(zend_enum_get_case_by_id(php_io_poll_trigger_ce,
			reg->trigger == PHP_IO_TRIGGER_EDGE ? ZEND_ENUM_Io_Poll_Trigger_Edge : ZEND_ENUM_Io_Poll_Trigger_Level));
}

PHP_METHOD(Io_Registration, isValid)
{
	ZEND_PARSE_PARAMETERS_NONE();

	RETURN_BOOL(PHP_IO_REGISTRATION_FROM_ZOBJ(Z_OBJ_P(ZEND_THIS))->reg != NULL);
}

/* Completion objects */

static zend_object *php_io_completion_create_object(zend_class_entry *ce)
{
	php_io_completion_obj *intern = zend_object_alloc(sizeof(php_io_completion_obj), ce);
	zend_object_std_init(&intern->std, ce);
	object_properties_init(&intern->std, ce);
	intern->operation = NULL;
	intern->produced = false;
	ZVAL_NULL(&intern->data);
	ZVAL_UNDEF(&intern->completions);
	return &intern->std;
}

static void php_io_completion_free_object(zend_object *obj)
{
	php_io_completion_obj *intern = PHP_IO_COMPLETION_FROM_ZOBJ(obj);
	if (intern->operation) {
		OBJ_RELEASE(intern->operation);
	}
	zval_ptr_dtor(&intern->data);
	zval_ptr_dtor(&intern->completions);
	zend_object_std_dtor(&intern->std);
}

static HashTable *php_io_completion_get_gc(zend_object *obj, zval **table, int *n)
{
	php_io_completion_obj *intern = PHP_IO_COMPLETION_FROM_ZOBJ(obj);
	zend_get_gc_buffer *gc_buffer = zend_get_gc_buffer_create();
	if (intern->operation) {
		zend_get_gc_buffer_add_obj(gc_buffer, intern->operation);
	}
	zend_get_gc_buffer_add_zval(gc_buffer, &intern->data);
	zend_get_gc_buffer_add_zval(gc_buffer, &intern->completions);
	zend_get_gc_buffer_use(gc_buffer, table, n);
	return NULL;
}

/* operation: reference added here; data and completions: copied, may be NULL */
static php_io_completion_obj *php_io_completion_create(zval *rv, php_io_op *op, zend_object *operation,
		php_io_status status, int64_t res, int error, zval *data, zval *completions)
{
	object_init_ex(rv, php_io_completion_ce);
	php_io_completion_obj *c = PHP_IO_COMPLETION_FROM_ZOBJ(Z_OBJ_P(rv));

	GC_ADDREF(operation);
	c->operation = operation;
	c->status = status;
	c->res = res;
	c->error = error;
	c->events = (op && (op->type == PHP_IO_OP_POLL || status == PHP_IO_READY)) ? (uint32_t) res : 0;
	if (data) {
		ZVAL_COPY(&c->data, data);
	}
	if (completions) {
		ZVAL_COPY(&c->completions, completions);
	}
	return c;
}

/* Io\Operation */

PHP_METHOD(Io_Operation, __construct)
{
	zend_throw_error(NULL, "Operations are created by the engine");
}

PHP_METHOD(Io_Operation, getHandle)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_io_op *op = php_io_operation_fetch(ZEND_THIS);
	if (!op) {
		RETURN_THROWS();
	}
	zend_object *handle = php_io_op_get_handle(op);
	if (handle) {
		RETURN_OBJ_COPY(handle);
	}

	/* The generic handles are created on demand, so a provider on a
	 * Context can watch the op like any other handle */
	php_io_operation_obj *intern = PHP_IO_OPERATION_FROM_ZOBJ(Z_OBJ_P(ZEND_THIS));
	if (!intern->lazy_handle) {
		zval handle_zv;
		switch (op->type) {
			case PHP_IO_OP_TIMER: {
				zend_hrtime_t remaining = php_deadline_is_infinite(&op->deadline)
						? ZEND_HRTIME_T_MAX / 2 : php_io_deadline_remaining(&op->deadline, zend_hrtime());
				php_io_poll_timer_handle_create(&handle_zv, remaining, false);
				break;
			}
			case PHP_IO_OP_WAITPID:
				if (op->u.waitpid.pid <= 0) {
					RETURN_NULL();
				}
				php_io_poll_process_handle_create(&handle_zv, op->u.waitpid.pid);
				break;
			case PHP_IO_OP_SIGWAIT:
				php_io_poll_signal_handle_create(&handle_zv, op->u.sigwait.set);
				break;
			default:
				RETURN_NULL();
		}
		intern->lazy_handle = Z_OBJ(handle_zv);
	}
	RETURN_OBJ_COPY(intern->lazy_handle);
}

PHP_METHOD(Io_Operation, getRegistration)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_io_op *op = php_io_operation_fetch(ZEND_THIS);
	if (!op) {
		RETURN_THROWS();
	}
	if (!op->registration) {
		RETURN_NULL();
	}
	RETURN_OBJ_COPY(php_io_registration_get_zobj(op->registration));
}

PHP_METHOD(Io_Operation, getEvents)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_io_op *op = php_io_operation_fetch(ZEND_THIS);
	if (!op) {
		RETURN_THROWS();
	}
	php_io_poll_events_to_event_enums(php_io_op_events(op), return_value);
}

PHP_METHOD(Io_Operation, getTimeout)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_io_op *op = php_io_operation_fetch(ZEND_THIS);
	if (!op) {
		RETURN_THROWS();
	}
	if (php_deadline_is_infinite(&op->deadline)) {
		RETURN_NULL();
	}

	zend_hrtime_t remaining = php_io_deadline_remaining(&op->deadline, zend_hrtime());
	zval ns;
	ZVAL_LONG(&ns, (zend_long) MIN(remaining, (zend_hrtime_t) ZEND_LONG_MAX));
	zend_call_method_with_1_params(NULL, php_date_ce_time_duration, NULL, "fromnanoseconds", return_value, &ns);
}

PHP_METHOD(Io_Operation, isValid)
{
	ZEND_PARSE_PARAMETERS_NONE();

	RETURN_BOOL(PHP_IO_OPERATION_FROM_ZOBJ(Z_OBJ_P(ZEND_THIS))->op != NULL);
}

PHP_METHOD(Io_Operation, complete)
{
	zend_object *status_obj;
	zend_long res = 0, error = 0;

	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_OBJ_OF_CLASS(status_obj, php_io_completion_status_ce)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(res)
		Z_PARAM_LONG(error)
	ZEND_PARSE_PARAMETERS_END();

	php_io_op *op = php_io_operation_fetch(ZEND_THIS);
	if (!op) {
		RETURN_THROWS();
	}
	php_io_completion_create(return_value, op, Z_OBJ_P(ZEND_THIS),
			php_io_status_from_case(status_obj), res, (int) error, NULL, NULL);
}

PHP_METHOD(Io_Operation, completeReady)
{
	zval *events_zv;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY(events_zv)
	ZEND_PARSE_PARAMETERS_END();

	php_io_op *op = php_io_operation_fetch(ZEND_THIS);
	if (!op) {
		RETURN_THROWS();
	}

	if (zend_hash_num_elements(Z_ARRVAL_P(events_zv)) == 0) {
		zend_argument_value_error(1, "must not be empty");
		RETURN_THROWS();
	}
	uint32_t events = php_io_poll_event_enums_to_events(events_zv);
	if (!events) {
		zend_argument_type_error(1, "must be a list of Io\\Poll\\Event enums");
		RETURN_THROWS();
	}

	php_io_status status = (op->type == PHP_IO_OP_POLL || op->type == PHP_IO_OP_TIMER)
			? PHP_IO_DONE : PHP_IO_READY;
	php_io_completion_create(return_value, op, Z_OBJ_P(ZEND_THIS), status, events, 0, NULL, NULL);
}

/* Data operations */

static php_io_op *php_io_operation_fetch_type(zval *zv, php_io_op_type type)
{
	php_io_op *op = php_io_operation_fetch(zv);
	if (op && op->type != type) {
		zend_throw_error(NULL, "Operation of an unexpected type");
		return NULL;
	}
	return op;
}

PHP_METHOD(Io_Operation_Read, getLength)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_io_op *op = php_io_operation_fetch_type(ZEND_THIS, PHP_IO_OP_READ);
	if (!op) {
		RETURN_THROWS();
	}
	RETURN_LONG((zend_long) op->u.io.len);
}

PHP_METHOD(Io_Operation_Read, getOffset)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_io_op *op = php_io_operation_fetch_type(ZEND_THIS, PHP_IO_OP_READ);
	if (!op) {
		RETURN_THROWS();
	}
	RETURN_LONG((zend_long) op->u.io.offset);
}

PHP_METHOD(Io_Operation_Write, getLength)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_io_op *op = php_io_operation_fetch_type(ZEND_THIS, PHP_IO_OP_WRITE);
	if (!op) {
		RETURN_THROWS();
	}
	RETURN_LONG((zend_long) op->u.io.len);
}

PHP_METHOD(Io_Operation_Write, getOffset)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_io_op *op = php_io_operation_fetch_type(ZEND_THIS, PHP_IO_OP_WRITE);
	if (!op) {
		RETURN_THROWS();
	}
	RETURN_LONG((zend_long) op->u.io.offset);
}

PHP_METHOD(Io_Operation_Recv, getLength)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_io_op *op = php_io_operation_fetch_type(ZEND_THIS, PHP_IO_OP_RECV);
	if (!op) {
		RETURN_THROWS();
	}
	RETURN_LONG((zend_long) op->u.io.len);
}

PHP_METHOD(Io_Operation_Recv, getFlags)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_io_op *op = php_io_operation_fetch_type(ZEND_THIS, PHP_IO_OP_RECV);
	if (!op) {
		RETURN_THROWS();
	}
	RETURN_LONG(op->u.io.flags);
}

PHP_METHOD(Io_Operation_Send, getLength)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_io_op *op = php_io_operation_fetch_type(ZEND_THIS, PHP_IO_OP_SEND);
	if (!op) {
		RETURN_THROWS();
	}
	RETURN_LONG((zend_long) op->u.io.len);
}

PHP_METHOD(Io_Operation_Send, getFlags)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_io_op *op = php_io_operation_fetch_type(ZEND_THIS, PHP_IO_OP_SEND);
	if (!op) {
		RETURN_THROWS();
	}
	RETURN_LONG(op->u.io.flags);
}

PHP_METHOD(Io_Operation_Fsync, isDataOnly)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_io_op *op = php_io_operation_fetch_type(ZEND_THIS, PHP_IO_OP_FSYNC);
	if (!op) {
		RETURN_THROWS();
	}
	RETURN_BOOL(op->u.fsync.data_only);
}

PHP_METHOD(Io_Operation_WaitPid, getPid)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_io_op *op = php_io_operation_fetch_type(ZEND_THIS, PHP_IO_OP_WAITPID);
	if (!op) {
		RETURN_THROWS();
	}
	RETURN_LONG((zend_long) op->u.waitpid.pid);
}

PHP_METHOD(Io_Operation_SigWait, getSignals)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_io_op *op = php_io_operation_fetch_type(ZEND_THIS, PHP_IO_OP_SIGWAIT);
	if (!op) {
		RETURN_THROWS();
	}
	array_init(return_value);
	for (int signo = 1; signo < PHP_NSIG; signo++) {
		if (php_sigismember(op->u.sigwait.set, signo) == 1) {
			add_next_index_long(return_value, signo);
		}
	}
}

PHP_METHOD(Io_Operation_Connect, getAddress)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_io_op *op = php_io_operation_fetch_type(ZEND_THIS, PHP_IO_OP_CONNECT);
	if (!op) {
		RETURN_THROWS();
	}
	zend_string *textaddr = NULL;
	php_network_populate_name_from_sockaddr((struct sockaddr *) op->u.connect.addr, op->u.connect.addrlen,
			&textaddr, NULL, NULL);
	if (!textaddr) {
		RETURN_EMPTY_STRING();
	}
	RETURN_STR(textaddr);
}

/* DNS operations */

PHP_METHOD(Io_Operation_GetAddrInfo, getHost)
{
	ZEND_PARSE_PARAMETERS_NONE();
	php_io_op *op = php_io_operation_fetch_type(ZEND_THIS, PHP_IO_OP_GETADDRINFO);
	if (!op) {
		RETURN_THROWS();
	}
	RETURN_STRING(op->u.getaddrinfo.node ? op->u.getaddrinfo.node : "");
}

PHP_METHOD(Io_Operation_GetAddrInfo, getService)
{
	ZEND_PARSE_PARAMETERS_NONE();
	php_io_op *op = php_io_operation_fetch_type(ZEND_THIS, PHP_IO_OP_GETADDRINFO);
	if (!op) {
		RETURN_THROWS();
	}
	if (!op->u.getaddrinfo.service) {
		RETURN_NULL();
	}
	RETURN_STRING(op->u.getaddrinfo.service);
}

/* The list is one malloc() block per entry with the address behind the
 * addrinfo; freeaddrinfo() would free it on glibc but not on macOS, whose
 * C library frees ai_addr on its own, so it is registered for
 * php_io_freeaddrinfo() instead */
PHP_METHOD(Io_Operation_GetAddrInfo, completeWithAddresses)
{
	zval *addresses;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY(addresses)
	ZEND_PARSE_PARAMETERS_END();

	php_io_op *op = php_io_operation_fetch_type(ZEND_THIS, PHP_IO_OP_GETADDRINFO);
	if (!op) {
		RETURN_THROWS();
	}

	const struct addrinfo *hints = op->u.getaddrinfo.hints;
	struct addrinfo *head = NULL, **tail = &head;
	zval *entry;
	ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(addresses), entry) {
		if (Z_TYPE_P(entry) != IS_STRING) {
			zend_argument_type_error(1, "must be a list of IP address strings");
			goto fail;
		}
		if (zend_str_has_nul_byte(Z_STR_P(entry))) {
			zend_argument_value_error(1, "must not contain any null bytes");
			goto fail;
		}
		struct sockaddr_storage ss;
		socklen_t len;
		memset(&ss, 0, sizeof(ss));
		if (inet_pton(AF_INET, Z_STRVAL_P(entry), &((struct sockaddr_in *) &ss)->sin_addr) == 1) {
			ss.ss_family = AF_INET;
			len = sizeof(struct sockaddr_in);
#ifdef HAVE_IPV6
		} else if (inet_pton(AF_INET6, Z_STRVAL_P(entry), &((struct sockaddr_in6 *) &ss)->sin6_addr) == 1) {
			ss.ss_family = AF_INET6;
			len = sizeof(struct sockaddr_in6);
#endif
		} else {
			zend_argument_value_error(1, "must contain valid IP addresses, \"%s\" given", Z_STRVAL_P(entry));
			goto fail;
		}
		if (hints && hints->ai_family != AF_UNSPEC && hints->ai_family != ss.ss_family) {
			/* Not asked for: skip it */
			continue;
		}
		struct addrinfo *ai = malloc(sizeof(struct addrinfo) + len);
		if (!ai) {
			zend_throw_error(NULL, "Out of memory");
			goto fail;
		}
		memset(ai, 0, sizeof(*ai));
		ai->ai_family = ss.ss_family;
		ai->ai_socktype = hints ? hints->ai_socktype : SOCK_STREAM;
		ai->ai_protocol = hints ? hints->ai_protocol : 0;
		ai->ai_addrlen = len;
		ai->ai_addr = (struct sockaddr *) (ai + 1);
		memcpy(ai->ai_addr, &ss, len);
		*tail = ai;
		tail = &ai->ai_next;
	} ZEND_HASH_FOREACH_END();

	if (!head) {
		php_io_completion_create(return_value, op, Z_OBJ_P(ZEND_THIS), PHP_IO_DONE, -1, EAI_NONAME, NULL, NULL);
		return;
	}
	php_io_addrinfo_register(head);
	*op->u.getaddrinfo.res = head;
	php_io_completion_create(return_value, op, Z_OBJ_P(ZEND_THIS), PHP_IO_DONE, 0, 0, NULL, NULL)->produced = true;
	return;

fail:
	php_io_addrinfo_free_list(head);
	RETURN_THROWS();
}

PHP_METHOD(Io_Operation_GetNameInfo, getAddress)
{
	ZEND_PARSE_PARAMETERS_NONE();
	php_io_op *op = php_io_operation_fetch_type(ZEND_THIS, PHP_IO_OP_GETNAMEINFO);
	if (!op) {
		RETURN_THROWS();
	}
	zend_string *textaddr = NULL;
	php_network_populate_name_from_sockaddr((struct sockaddr *) op->u.getnameinfo.addr, op->u.getnameinfo.addrlen,
			&textaddr, NULL, NULL);
	if (!textaddr) {
		RETURN_EMPTY_STRING();
	}
	RETURN_STR(textaddr);
}

PHP_METHOD(Io_Operation_GetNameInfo, completeWithName)
{
	zend_string *host, *service = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_PATH_STR(host)
		Z_PARAM_OPTIONAL
		Z_PARAM_PATH_STR_OR_NULL(service)
	ZEND_PARSE_PARAMETERS_END();

	php_io_op *op = php_io_operation_fetch_type(ZEND_THIS, PHP_IO_OP_GETNAMEINFO);
	if (!op) {
		RETURN_THROWS();
	}
	if (op->u.getnameinfo.host && op->u.getnameinfo.hostlen) {
		if (ZSTR_LEN(host) >= op->u.getnameinfo.hostlen) {
			php_io_completion_create(return_value, op, Z_OBJ_P(ZEND_THIS), PHP_IO_DONE, -1, EAI_OVERFLOW, NULL, NULL);
			return;
		}
		memcpy(op->u.getnameinfo.host, ZSTR_VAL(host), ZSTR_LEN(host) + 1);
	}
	if (op->u.getnameinfo.service && op->u.getnameinfo.servicelen) {
		size_t len = service ? ZSTR_LEN(service) : 0;
		if (len >= op->u.getnameinfo.servicelen) {
			php_io_completion_create(return_value, op, Z_OBJ_P(ZEND_THIS), PHP_IO_DONE, -1, EAI_OVERFLOW, NULL, NULL);
			return;
		}
		if (service) {
			memcpy(op->u.getnameinfo.service, ZSTR_VAL(service), len + 1);
		} else {
			op->u.getnameinfo.service[0] = '\0';
		}
	}
	php_io_completion_create(return_value, op, Z_OBJ_P(ZEND_THIS), PHP_IO_DONE, 0, 0, NULL, NULL)->produced = true;
}

/* Io\Operation\Any */

PHP_METHOD(Io_Operation_Any, getOperations)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_io_op *op = php_io_operation_fetch(ZEND_THIS);
	if (!op) {
		RETURN_THROWS();
	}

	array_init_size(return_value, op->u.any.n);
	for (uint32_t i = 0; i < op->u.any.n; i++) {
		zval member;
		ZVAL_OBJ_COPY(&member, php_io_operation_get_zobj(op->u.any.ops[i]));
		zend_hash_next_index_insert_new(Z_ARRVAL_P(return_value), &member);
	}
}

/* The index of the member an operation object wraps, or -1 */
static int32_t php_io_any_member_index(php_io_op *any, zend_object *operation)
{
	for (uint32_t i = 0; i < any->u.any.n; i++) {
		if (any->u.any.ops[i]->zobj == operation) {
			return (int32_t) i;
		}
	}
	return -1;
}

PHP_METHOD(Io_Operation_Any, completeWith)
{
	zval *completions;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY(completions)
	ZEND_PARSE_PARAMETERS_END();

	php_io_op *op = php_io_operation_fetch(ZEND_THIS);
	if (!op) {
		RETURN_THROWS();
	}

	if (zend_hash_num_elements(Z_ARRVAL_P(completions)) == 0) {
		zend_argument_value_error(1, "must not be empty");
		RETURN_THROWS();
	}

	bool *seen = ecalloc(op->u.any.n, sizeof(bool));
	zval *entry;
	ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(completions), entry) {
		if (Z_TYPE_P(entry) != IS_OBJECT || !instanceof_function(Z_OBJCE_P(entry), php_io_completion_ce)) {
			efree(seen);
			zend_argument_type_error(1, "must be a list of Io\\Completion objects");
			RETURN_THROWS();
		}
		php_io_completion_obj *c = PHP_IO_COMPLETION_FROM_ZOBJ(Z_OBJ_P(entry));
		int32_t index = php_io_any_member_index(op, c->operation);
		if (index < 0) {
			efree(seen);
			zend_argument_value_error(1, "must only contain completions of the members of this operation");
			RETURN_THROWS();
		}
		if (seen[index]) {
			efree(seen);
			zend_argument_value_error(1, "must not contain two completions of the same member");
			RETURN_THROWS();
		}
		seen[index] = true;
	} ZEND_HASH_FOREACH_END();
	efree(seen);

	php_io_completion_create(return_value, op, Z_OBJ_P(ZEND_THIS), PHP_IO_DONE, 0, 0, NULL, completions);
}

/* Io\Completion */

PHP_METHOD(Io_Completion, __construct)
{
	zend_throw_error(NULL, "Completions are created from operations");
}

PHP_METHOD(Io_Completion, getOperation)
{
	ZEND_PARSE_PARAMETERS_NONE();
	RETURN_OBJ_COPY(PHP_IO_COMPLETION_FROM_ZOBJ(Z_OBJ_P(ZEND_THIS))->operation);
}

PHP_METHOD(Io_Completion, getStatus)
{
	ZEND_PARSE_PARAMETERS_NONE();
	RETURN_OBJ_COPY(php_io_status_case(PHP_IO_COMPLETION_FROM_ZOBJ(Z_OBJ_P(ZEND_THIS))->status));
}

PHP_METHOD(Io_Completion, getResult)
{
	ZEND_PARSE_PARAMETERS_NONE();
	RETURN_LONG((zend_long) PHP_IO_COMPLETION_FROM_ZOBJ(Z_OBJ_P(ZEND_THIS))->res);
}

PHP_METHOD(Io_Completion, getEvents)
{
	ZEND_PARSE_PARAMETERS_NONE();
	php_io_poll_events_to_event_enums(PHP_IO_COMPLETION_FROM_ZOBJ(Z_OBJ_P(ZEND_THIS))->events, return_value);
}

PHP_METHOD(Io_Completion, getError)
{
	ZEND_PARSE_PARAMETERS_NONE();
	RETURN_LONG(PHP_IO_COMPLETION_FROM_ZOBJ(Z_OBJ_P(ZEND_THIS))->error);
}

PHP_METHOD(Io_Completion, getData)
{
	ZEND_PARSE_PARAMETERS_NONE();
	RETURN_COPY(&PHP_IO_COMPLETION_FROM_ZOBJ(Z_OBJ_P(ZEND_THIS))->data);
}

PHP_METHOD(Io_Completion, getCompletions)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_io_completion_obj *c = PHP_IO_COMPLETION_FROM_ZOBJ(Z_OBJ_P(ZEND_THIS));
	if (Z_TYPE(c->completions) == IS_ARRAY) {
		RETURN_COPY(&c->completions);
	}
	RETURN_EMPTY_ARRAY();
}

/* Io\Poll\OperationQueue */

PHPAPI zend_object *php_io_opqueue_create_object(zend_class_entry *ce)
{
	php_io_opqueue_obj *intern = zend_object_alloc(sizeof(php_io_opqueue_obj), ce);
	zend_object_std_init(&intern->std, ce);
	object_properties_init(&intern->std, ce);
	intern->queue = NULL;
	intern->subs = NULL;
	return &intern->std;
}

static void php_io_opqueue_sub_unlink(php_io_opqueue_obj *q, php_io_opqueue_sub *sub)
{
	if (sub->prev) {
		sub->prev->next = sub->next;
	} else {
		q->subs = sub->next;
	}
	if (sub->next) {
		sub->next->prev = sub->prev;
	}
}

static void php_io_opqueue_sub_free(php_io_opqueue_sub *sub)
{
	OBJ_RELEASE(sub->operation);
	zval_ptr_dtor(&sub->data);
	efree(sub);
}

static void php_io_poll_operation_queue_free_object(zend_object *obj)
{
	php_io_opqueue_obj *intern = PHP_IO_OPQUEUE_FROM_ZOBJ(obj);

	if (intern->queue) {
		/* Cancels and withdraws everything still submitted */
		intern->queue->ops->destroy(intern->queue);
		intern->queue = NULL;
	}
	while (intern->subs) {
		php_io_opqueue_sub *sub = intern->subs;
		/* An op still on a suspended frame must not find the record later */
		php_io_op *op = PHP_IO_OPERATION_FROM_ZOBJ(sub->operation)->op;
		if (op && op->provider_data == sub) {
			op->provider_data = NULL;
		}
		php_io_opqueue_sub_unlink(intern, sub);
		php_io_opqueue_sub_free(sub);
	}
	zend_object_std_dtor(&intern->std);
}

static HashTable *php_io_poll_operation_queue_get_gc(zend_object *obj, zval **table, int *n)
{
	php_io_opqueue_obj *intern = PHP_IO_OPQUEUE_FROM_ZOBJ(obj);
	zend_get_gc_buffer *gc_buffer = zend_get_gc_buffer_create();
	for (php_io_opqueue_sub *sub = intern->subs; sub; sub = sub->next) {
		zend_get_gc_buffer_add_obj(gc_buffer, sub->operation);
		zend_get_gc_buffer_add_zval(gc_buffer, &sub->data);
	}
	zend_get_gc_buffer_use(gc_buffer, table, n);
	return NULL;
}

static php_io_opqueue_obj *php_io_opqueue_fetch(zval *zv)
{
	php_io_opqueue_obj *intern = PHP_IO_OPQUEUE_FROM_ZOBJ(Z_OBJ_P(zv));
	if (!intern->queue) {
		zend_throw_error(NULL, "%s object is not constructed", ZSTR_VAL(Z_OBJCE_P(zv)->name));
	}
	return intern;
}

PHP_METHOD(Io_Poll_OperationQueue, __construct)
{
	zval *context = NULL;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_OBJECT_OF_CLASS_OR_NULL(context, php_io_poll_context_ce)
	ZEND_PARSE_PARAMETERS_END();

	php_io_opqueue_obj *intern = PHP_IO_OPQUEUE_FROM_ZOBJ(Z_OBJ_P(ZEND_THIS));

	if (intern->queue) {
		zend_throw_error(NULL, "Io\\Poll\\OperationQueue object is already constructed");
		RETURN_THROWS();
	}
	if (context) {
		zend_throw_error(NULL, "Sharing an Io\\Poll\\Context is not supported yet");
		RETURN_THROWS();
	}

	intern->queue = php_io_queue_create_poll(PHP_POLL_BACKEND_AUTO);
	if (!intern->queue) {
		zend_throw_exception(php_io_exception_class_entry, "Failed to create the poll queue", 0);
		RETURN_THROWS();
	}
}

PHP_METHOD(Io_Poll_OperationQueue, getContext)
{
	ZEND_PARSE_PARAMETERS_NONE();
	zend_throw_error(NULL, "Io\\Poll\\OperationQueue::getContext() is not supported yet");
}

PHP_METHOD(Io_Poll_OperationQueue, submit)
{
	zval *op_zv, *data = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_OBJECT_OF_CLASS(op_zv, php_io_operation_ce)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(data)
	ZEND_PARSE_PARAMETERS_END();

	php_io_opqueue_obj *intern = php_io_opqueue_fetch(ZEND_THIS);
	if (!intern) {
		RETURN_THROWS();
	}
	php_io_op *op = php_io_operation_fetch(op_zv);
	if (!op) {
		RETURN_THROWS();
	}
	if (op->queue) {
		zend_throw_error(NULL, "The operation is already submitted");
		RETURN_THROWS();
	}

	php_io_opqueue_sub *sub = emalloc(sizeof(*sub));
	sub->operation = Z_OBJ_P(op_zv);
	GC_ADDREF(sub->operation);
	if (data) {
		ZVAL_COPY(&sub->data, data);
	} else {
		ZVAL_NULL(&sub->data);
	}
	sub->owner = intern;
	sub->prev = NULL;
	sub->next = intern->subs;
	if (intern->subs) {
		intern->subs->prev = sub;
	}
	intern->subs = sub;
	op->provider_data = sub;

	if (intern->queue->ops->submit(intern->queue, op, sub) == FAILURE) {
		op->provider_data = NULL;
		php_io_opqueue_sub_unlink(intern, sub);
		php_io_opqueue_sub_free(sub);
		zend_throw_exception_ex(php_io_exception_class_entry, errno,
				"Failed to submit the operation: %s", strerror(errno));
		RETURN_THROWS();
	}

	/* Completed at submit: the caller gets it here and never from waitCompletions() */
	php_io_queue_completion c;
	if (intern->queue->ops->take_inline(intern->queue, op, &c)) {
		php_io_opqueue_completion_to_zval(return_value, &c);
		php_io_opqueue_sub_unlink(intern, sub);
		php_io_opqueue_sub_free(sub);
		return;
	}
	RETURN_NULL();
}

PHP_METHOD(Io_Poll_OperationQueue, cancel)
{
	zval *op_zv;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(op_zv, php_io_operation_ce)
	ZEND_PARSE_PARAMETERS_END();

	php_io_opqueue_obj *intern = php_io_opqueue_fetch(ZEND_THIS);
	if (!intern) {
		RETURN_THROWS();
	}
	php_io_op *op = php_io_operation_fetch(op_zv);
	if (!op) {
		RETURN_THROWS();
	}
	if (op->queue != intern->queue) {
		zend_throw_error(NULL, "The operation is not submitted to this queue");
		RETURN_THROWS();
	}

	php_io_opqueue_sub *sub = op->provider_data;
	if (intern->queue->ops->cancel(intern->queue, op) == FAILURE) {
		zend_throw_exception_ex(php_io_exception_class_entry, errno,
				"Failed to cancel the operation: %s", strerror(errno));
		RETURN_THROWS();
	}
	if (sub) {
		op->provider_data = NULL;
		php_io_opqueue_sub_unlink(intern, sub);
		php_io_opqueue_sub_free(sub);
	}
}

PHP_METHOD(Io_Poll_OperationQueue, add)
{
	zval *reg_zv;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(reg_zv, php_io_registration_ce)
	ZEND_PARSE_PARAMETERS_END();

	php_io_opqueue_obj *intern = php_io_opqueue_fetch(ZEND_THIS);
	if (!intern) {
		RETURN_THROWS();
	}
	php_io_registration *reg = php_io_registration_fetch(reg_zv);
	if (!reg) {
		RETURN_THROWS();
	}
	if (intern->queue->ops->add(intern->queue, reg) == FAILURE) {
		zend_throw_exception_ex(php_io_exception_class_entry, errno,
				"Failed to add the registration: %s", strerror(errno));
		RETURN_THROWS();
	}
}

PHP_METHOD(Io_Poll_OperationQueue, remove)
{
	zval *reg_zv;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(reg_zv, php_io_registration_ce)
	ZEND_PARSE_PARAMETERS_END();

	php_io_opqueue_obj *intern = php_io_opqueue_fetch(ZEND_THIS);
	if (!intern) {
		RETURN_THROWS();
	}
	php_io_registration *reg = php_io_registration_fetch(reg_zv);
	if (!reg) {
		RETURN_THROWS();
	}
	intern->queue->ops->remove(intern->queue, reg);
}

/* Build the Completion for one delivered completion, members of an Any included */
static void php_io_opqueue_completion_to_zval(zval *rv, php_io_queue_completion *c)
{
	php_io_op *op = c->op;
	php_io_opqueue_sub *sub = c->data;
	zval members;

	ZVAL_UNDEF(&members);
	if (op->type == PHP_IO_OP_ANY) {
		array_init_size(&members, op->u.any.n_results);
		for (uint32_t i = 0; i < op->u.any.n_results; i++) {
			php_io_op_result *r = &op->u.any.results[i];
			php_io_op *member = op->u.any.ops[r->index];
			zval member_zv;
			php_io_completion_create(&member_zv, member, php_io_operation_get_zobj(member),
					r->status, r->res, r->error, NULL, NULL)->produced = true;
			zend_hash_next_index_insert_new(Z_ARRVAL(members), &member_zv);
		}
	}

	op->provider_data = NULL;
	php_io_completion_create(rv, op, sub->operation, c->result.status, c->result.res,
			c->result.error, &sub->data, Z_TYPE(members) == IS_ARRAY ? &members : NULL)->produced = true;
	zval_ptr_dtor(&members);
}

PHP_METHOD(Io_Poll_OperationQueue, waitCompletions)
{
	php_date_time_duration *timeout = NULL;
	zend_long max = 0;
	bool max_is_null = true;

	ZEND_PARSE_PARAMETERS_START(0, 2)
		Z_PARAM_OPTIONAL
		Z_PARAM_DATE_TIME_DURATION_OR_NULL(timeout)
		Z_PARAM_LONG_OR_NULL(max, max_is_null)
	ZEND_PARSE_PARAMETERS_END();

	php_io_opqueue_obj *intern = php_io_opqueue_fetch(ZEND_THIS);
	if (!intern) {
		RETURN_THROWS();
	}

	/* A zero duration is one reap that never blocks: the non-blocking deadline */
	php_deadline dl;
	if (timeout) {
		if (timeout->duration.negative) {
			zend_argument_value_error(1, "must not be negative");
			RETURN_THROWS();
		}
		if (timeout->duration.seconds == 0 && timeout->duration.nanoseconds == 0) {
			php_deadline_init_nonblock(&dl);
		} else if ((zend_hrtime_t) timeout->duration.seconds >= ZEND_HRTIME_T_MAX / ZEND_NANO_IN_SEC) {
			php_deadline_init_infinite(&dl);
		} else {
			dl = php_io_deadline_from_ns((zend_hrtime_t) timeout->duration.seconds * ZEND_NANO_IN_SEC
					+ (zend_hrtime_t) timeout->duration.nanoseconds);
		}
	}

	if (max_is_null) {
		max = 64;
	} else if (max <= 0) {
		zend_argument_value_error(2, "must be greater than 0");
		RETURN_THROWS();
	} else if (max > 4096) {
		max = 4096;
	}

	php_io_queue_completion *completions = safe_emalloc((size_t) max, sizeof(*completions), 0);
	int n = intern->queue->ops->wait(intern->queue, completions, (uint32_t) max, timeout ? &dl : NULL);
	if (n < 0) {
		int err = errno;
		efree(completions);
		if (err == EINTR) {
			/* A signal handler runs before the caller waits again */
			RETURN_EMPTY_ARRAY();
		}
		if (err == EDEADLK) {
			zend_throw_exception(php_io_exception_class_entry,
					"No operation can complete: nothing pending has a descriptor or a deadline", err);
		} else {
			zend_throw_exception_ex(php_io_exception_class_entry, err,
					"Failed to wait for completions: %s", strerror(err));
		}
		RETURN_THROWS();
	}

	array_init_size(return_value, n);
	for (int i = 0; i < n; i++) {
		php_io_opqueue_sub *sub = completions[i].data;
		zval completion;
		php_io_opqueue_completion_to_zval(&completion, &completions[i]);
		zend_hash_next_index_insert_new(Z_ARRVAL_P(return_value), &completion);
		php_io_opqueue_sub_unlink(intern, sub);
		php_io_opqueue_sub_free(sub);
	}
	efree(completions);
}

PHP_METHOD(Io_Poll_OperationQueue, countPending)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_io_opqueue_obj *intern = php_io_opqueue_fetch(ZEND_THIS);
	if (!intern) {
		RETURN_THROWS();
	}
	RETURN_LONG(intern->queue->ops->count_pending(intern->queue));
}

static const struct {
	uint32_t flag;
	zend_long case_id;
} php_io_hooks_capabilities[] = {
	{ PHP_IO_HOOKS_F_FILES, ZEND_ENUM_Io_Hooks_Capability_Files },
	{ PHP_IO_HOOKS_F_DIRECT_DATA, ZEND_ENUM_Io_Hooks_Capability_DirectData },
	{ PHP_IO_HOOKS_F_DIRECT_ACCEPT, ZEND_ENUM_Io_Hooks_Capability_DirectAccept },
	{ PHP_IO_HOOKS_F_EDGE_REGISTRATIONS, ZEND_ENUM_Io_Hooks_Capability_EdgeRegistrations },
	{ PHP_IO_HOOKS_F_LEVEL_REGISTRATIONS, ZEND_ENUM_Io_Hooks_Capability_LevelRegistrations },
};

PHPAPI void php_io_hook_flags_to_capabilities(uint32_t flags, zval *rv)
{
	array_init(rv);
	for (size_t i = 0; i < sizeof(php_io_hooks_capabilities) / sizeof(php_io_hooks_capabilities[0]); i++) {
		if (flags & php_io_hooks_capabilities[i].flag) {
			zval c;
			ZVAL_OBJ_COPY(&c, zend_enum_get_case_by_id(php_io_hooks_capability_ce, php_io_hooks_capabilities[i].case_id));
			zend_hash_next_index_insert_new(Z_ARRVAL_P(rv), &c);
		}
	}
}

PHP_METHOD(Io_Poll_OperationQueue, getHookCapabilities)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_io_opqueue_obj *intern = php_io_opqueue_fetch(ZEND_THIS);
	if (!intern) {
		RETURN_THROWS();
	}
	php_io_hook_flags_to_capabilities(intern->queue->ops->hook_flags(intern->queue), return_value);
}

/* The userland provider adapter, installed as the C provider by set_hooks() */

typedef struct {
	php_io_hooks hooks;
	zend_object *obj;
	zend_fcall_info_cache run_fcc;
	zend_fcall_info_cache add_fcc;
	zend_fcall_info_cache remove_fcc;
} php_io_hooks_php;

#define PHP_IO_HOOKS_PHP(h) ZEND_CONTAINER_OF(h, php_io_hooks_php, hooks)

static void php_io_hooks_method_fcc(zend_object *obj, const char *name, zend_fcall_info_cache *fcc)
{
	zend_string *name_str = zend_string_init(name, strlen(name), false);
	zend_function *fn = obj->handlers->get_method(&obj, name_str, NULL);
	zend_string_release(name_str);
	ZEND_ASSERT(fn != NULL);

	*fcc = (zend_fcall_info_cache) {
		.function_handler = fn,
		.object = obj,
		.called_scope = obj->ce,
	};
	zend_fcc_addref(fcc);
}

/* The provider may be replaced from inside run(): the call keeps its own
 * reference, and nothing of the provider is read after it */
static void php_io_hooks_php_call(zend_fcall_info_cache *fcc, zval *retval, zend_object *arg_obj)
{
	zend_fcall_info_cache held = *fcc;
	zval arg;

	GC_ADDREF(held.object);
	ZVAL_OBJ_COPY(&arg, arg_obj);
	zend_call_known_fcc(&held, retval, 1, &arg, NULL);
	zval_ptr_dtor(&arg);
	OBJ_RELEASE(held.object);
}

/* A Done that hands data to the caller: bytes in its buffer, a descriptor,
 * a reaped child, a taken signal, a resolved name */
static bool php_io_op_result_is_data(php_io_op *op, const php_io_completion_obj *c)
{
	if (c->status != PHP_IO_DONE || c->error) {
		return false;
	}
	switch (op->type) {
		case PHP_IO_OP_READ:
		case PHP_IO_OP_RECV:
			return c->res != 0;
		case PHP_IO_OP_ACCEPT:
		case PHP_IO_OP_WAITPID:
		case PHP_IO_OP_SIGWAIT:
		case PHP_IO_OP_GETADDRINFO:
		case PHP_IO_OP_GETNAMEINFO:
			return true;
		default:
			return false;
	}
}

static zend_result php_io_hooks_php_run(php_io_hooks *hooks, php_io_op *op, php_io_op_result *result)
{
	zend_object *zobj = php_io_operation_get_zobj(op);
	zval retval;

	ZVAL_UNDEF(&retval);
	php_io_hooks_php_call(&PHP_IO_HOOKS_PHP(hooks)->run_fcc, &retval, zobj);

	if (EG(exception)) {
		zval_ptr_dtor(&retval);
		return FAILURE;
	}
	if (Z_TYPE(retval) != IS_OBJECT || !instanceof_function(Z_OBJCE(retval), php_io_completion_ce)) {
		zval_ptr_dtor(&retval);
		zend_throw_error(NULL, "Io\\Hooks\\Hooks::run() must return an Io\\Completion");
		return FAILURE;
	}

	php_io_completion_obj *c = PHP_IO_COMPLETION_FROM_ZOBJ(Z_OBJ(retval));
	if (c->operation != zobj) {
		zval_ptr_dtor(&retval);
		zend_throw_error(NULL, "Io\\Hooks\\Hooks::run() returned the completion of another operation");
		return FAILURE;
	}
	if (op->queue && !c->produced) {
		zval_ptr_dtor(&retval);
		zend_throw_error(NULL, "Io\\Hooks\\Hooks::run() must cancel the operation or wait for its completion from the queue it was submitted to");
		return FAILURE;
	}
	if (!c->produced && php_io_op_result_is_data(op, c)) {
		zval_ptr_dtor(&retval);
		if (op->type == PHP_IO_OP_GETADDRINFO || op->type == PHP_IO_OP_GETNAMEINFO) {
			/* No name without completeWithAddresses() or completeWithName() */
			result->status = PHP_IO_DONE;
			result->index = 0;
			result->res = -1;
			result->error = EAI_FAIL;
			return SUCCESS;
		}
		zend_throw_error(NULL, "Io\\Hooks\\Hooks::run() cannot complete %s with a result it did not produce, "
				"only a queue can", ZSTR_VAL(zobj->ce->name));
		return FAILURE;
	}

	result->status = c->status;
	result->index = 0;
	result->res = c->res;
	result->error = c->error;

	if (op->type == PHP_IO_OP_ANY) {
		uint32_t n = 0;
		if (Z_TYPE(c->completions) == IS_ARRAY) {
			zval *entry;
			ZEND_HASH_FOREACH_VAL(Z_ARRVAL(c->completions), entry) {
				php_io_completion_obj *m = PHP_IO_COMPLETION_FROM_ZOBJ(Z_OBJ_P(entry));
				int32_t index = php_io_any_member_index(op, m->operation);
				if (index < 0 || n >= op->u.any.n) {
					continue;
				}
				op->u.any.results[n].status = m->status;
				op->u.any.results[n].index = (uint32_t) index;
				op->u.any.results[n].res = m->res;
				op->u.any.results[n].error = m->error;
				n++;
			} ZEND_HASH_FOREACH_END();
		}
		op->u.any.n_results = n;
	}

	zval_ptr_dtor(&retval);
	return SUCCESS;
}

static void php_io_hooks_php_add(php_io_hooks *hooks, php_io_registration *reg)
{
	php_io_hooks_php_call(&PHP_IO_HOOKS_PHP(hooks)->add_fcc, NULL, php_io_registration_get_zobj(reg));
}

static void php_io_hooks_php_remove(php_io_hooks *hooks, php_io_registration *reg)
{
	php_io_hooks_php_call(&PHP_IO_HOOKS_PHP(hooks)->remove_fcc, NULL, php_io_registration_get_zobj(reg));
}

static void php_io_hooks_php_dtor(php_io_hooks *hooks)
{
	php_io_hooks_php *php_hooks = PHP_IO_HOOKS_PHP(hooks);
	zend_fcc_dtor(&php_hooks->run_fcc);
	zend_fcc_dtor(&php_hooks->add_fcc);
	zend_fcc_dtor(&php_hooks->remove_fcc);
	OBJ_RELEASE(php_hooks->obj);
	efree(php_hooks);
}

static const php_io_hooks_ops php_io_hooks_php_ops = {
	.run = php_io_hooks_php_run,
	.add = php_io_hooks_php_add,
	.remove = php_io_hooks_php_remove,
	.dtor = php_io_hooks_php_dtor,
};

/* The installed userland provider object, NULL for none or a C provider */
static zend_object *php_io_hooks_php_current(void)
{
	php_io_hooks *hooks = php_io_hooks_current();
	if (!hooks || hooks->ops != &php_io_hooks_php_ops) {
		return NULL;
	}
	return PHP_IO_HOOKS_PHP(hooks)->obj;
}

static uint32_t php_io_hooks_capabilities_to_flags(zval *capabilities)
{
	uint32_t flags = 0;
	zval *entry;

	ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(capabilities), entry) {
		if (Z_TYPE_P(entry) != IS_OBJECT || Z_OBJCE_P(entry) != php_io_hooks_capability_ce) {
			zend_throw_error(NULL, "Io\\Hooks\\Hooks::getCapabilities() must return a list of Io\\Hooks\\Capability");
			return 0;
		}
		zend_long case_id = zend_enum_fetch_case_id(Z_OBJ_P(entry));
		for (size_t i = 0; i < sizeof(php_io_hooks_capabilities) / sizeof(php_io_hooks_capabilities[0]); i++) {
			if (php_io_hooks_capabilities[i].case_id == case_id) {
				flags |= php_io_hooks_capabilities[i].flag;
			}
		}
	} ZEND_HASH_FOREACH_END();

	return flags;
}

PHP_FUNCTION(Io_Hooks_set_hooks)
{
	zend_object *hooks_obj = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJ_OF_CLASS_OR_NULL(hooks_obj, php_io_hooks_ce)
	ZEND_PARSE_PARAMETERS_END();

	if (FG(io_hooks_locked)) {
		zend_throw_error(NULL, "Io\\Hooks\\set_hooks() cannot be called from getCapabilities(), add(), remove() or a provider destructor");
		RETURN_THROWS();
	}
	zend_object *previous = php_io_hooks_php_current();
	if (php_io_hooks_active() && !previous) {
		zend_throw_error(NULL, "IO hooks are owned by an internal provider");
		RETURN_THROWS();
	}

	php_io_hooks_php *php_hooks = NULL;

	if (hooks_obj) {
		zval capabilities;
		zend_fcall_info_cache caps_fcc;
		php_io_hooks_method_fcc(hooks_obj, "getCapabilities", &caps_fcc);
		ZVAL_UNDEF(&capabilities);
		php_io_hooks_lock();
		zend_call_known_fcc(&caps_fcc, &capabilities, 0, NULL, NULL);
		php_io_hooks_unlock();
		zend_fcc_dtor(&caps_fcc);
		if (EG(exception)) {
			zval_ptr_dtor(&capabilities);
			RETURN_THROWS();
		}
		if (Z_TYPE(capabilities) != IS_ARRAY) {
			zval_ptr_dtor(&capabilities);
			zend_throw_error(NULL, "Io\\Hooks\\Hooks::getCapabilities() must return an array");
			RETURN_THROWS();
		}
		uint32_t flags = php_io_hooks_capabilities_to_flags(&capabilities);
		zval_ptr_dtor(&capabilities);
		if (EG(exception)) {
			RETURN_THROWS();
		}

		php_hooks = emalloc(sizeof(*php_hooks));
		php_hooks->hooks.ops = &php_io_hooks_php_ops;
		php_hooks->hooks.flags = flags;
		php_hooks->obj = hooks_obj;
		GC_ADDREF(hooks_obj);
		php_io_hooks_method_fcc(hooks_obj, "run", &php_hooks->run_fcc);
		php_io_hooks_method_fcc(hooks_obj, "add", &php_hooks->add_fcc);
		php_io_hooks_method_fcc(hooks_obj, "remove", &php_hooks->remove_fcc);
	}

	/* The previous provider is returned, so it survives its dtor */
	if (previous) {
		GC_ADDREF(previous);
	}
	php_io_hooks_register(NULL);

	if (php_hooks) {
		zend_result rc = php_io_hooks_register(&php_hooks->hooks);
		ZEND_ASSERT(rc == SUCCESS);
	}

	if (previous) {
		RETURN_OBJ(previous);
	}
	RETURN_NULL();
}

PHP_FUNCTION(Io_Hooks_get_hooks)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_object *current = php_io_hooks_php_current();
	if (!current) {
		RETURN_NULL();
	}
	RETURN_OBJ_COPY(current);
}

PHP_FUNCTION(Io_Hooks_is_active)
{
	ZEND_PARSE_PARAMETERS_NONE();
	RETURN_BOOL(php_io_hooks_active());
}

static zend_class_entry *php_io_operation_init_subclass(zend_class_entry *ce)
{
	ce->create_object = php_io_operation_create_object;
	ce->default_object_handlers = &php_io_operation_handlers;
	return ce;
}

PHP_MINIT_FUNCTION(io_hooks)
{
	php_io_completion_status_ce = register_class_Io_CompletionStatus();

	php_io_operation_ce = register_class_Io_Operation();
	php_io_operation_ce->create_object = php_io_operation_create_object;
	memcpy(&php_io_operation_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	php_io_operation_handlers.offset = offsetof(php_io_operation_obj, std);
	php_io_operation_handlers.free_obj = php_io_operation_free_object;
	php_io_operation_handlers.clone_obj = NULL;
	php_io_operation_ce->default_object_handlers = &php_io_operation_handlers;

	php_io_operation_poll_ce = php_io_operation_init_subclass(
		register_class_Io_Operation_Poll(php_io_operation_ce));
	php_io_operation_timer_ce = php_io_operation_init_subclass(
		register_class_Io_Operation_Timer(php_io_operation_ce));
	php_io_operation_any_ce = php_io_operation_init_subclass(
		register_class_Io_Operation_Any(php_io_operation_ce));
	php_io_operation_read_ce = php_io_operation_init_subclass(
		register_class_Io_Operation_Read(php_io_operation_ce));
	php_io_operation_write_ce = php_io_operation_init_subclass(
		register_class_Io_Operation_Write(php_io_operation_ce));
	php_io_operation_recv_ce = php_io_operation_init_subclass(
		register_class_Io_Operation_Recv(php_io_operation_ce));
	php_io_operation_send_ce = php_io_operation_init_subclass(
		register_class_Io_Operation_Send(php_io_operation_ce));
	php_io_operation_accept_ce = php_io_operation_init_subclass(
		register_class_Io_Operation_Accept(php_io_operation_ce));
	php_io_operation_connect_ce = php_io_operation_init_subclass(
		register_class_Io_Operation_Connect(php_io_operation_ce));
	php_io_operation_fsync_ce = php_io_operation_init_subclass(
		register_class_Io_Operation_Fsync(php_io_operation_ce));
	php_io_operation_getaddrinfo_ce = php_io_operation_init_subclass(
		register_class_Io_Operation_GetAddrInfo(php_io_operation_ce));
	php_io_operation_getnameinfo_ce = php_io_operation_init_subclass(
		register_class_Io_Operation_GetNameInfo(php_io_operation_ce));
	php_io_operation_waitpid_ce = php_io_operation_init_subclass(
		register_class_Io_Operation_WaitPid(php_io_operation_ce));
	php_io_operation_sigwait_ce = php_io_operation_init_subclass(
		register_class_Io_Operation_SigWait(php_io_operation_ce));
	/* Every operation type is ours: userland may not add one */
	php_io_operation_ce->ce_flags |= ZEND_ACC_FINAL;

	php_io_completion_ce = register_class_Io_Completion();
	php_io_completion_ce->create_object = php_io_completion_create_object;
	memcpy(&php_io_completion_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	php_io_completion_handlers.offset = offsetof(php_io_completion_obj, std);
	php_io_completion_handlers.free_obj = php_io_completion_free_object;
	php_io_completion_handlers.get_gc = php_io_completion_get_gc;
	php_io_completion_handlers.clone_obj = NULL;
	php_io_completion_ce->default_object_handlers = &php_io_completion_handlers;

	php_io_invalid_operation_exception_ce
			= register_class_Io_InvalidOperationException(php_io_exception_class_entry);

	php_io_registration_ce = register_class_Io_Registration();
	php_io_registration_ce->create_object = php_io_registration_create_object;
	memcpy(&php_io_registration_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	php_io_registration_handlers.offset = offsetof(php_io_registration_obj, std);
	php_io_registration_handlers.clone_obj = NULL;
	php_io_registration_ce->default_object_handlers = &php_io_registration_handlers;
	php_io_invalid_registration_exception_ce
			= register_class_Io_InvalidRegistrationException(php_io_exception_class_entry);
	php_io_poll_trigger_ce = register_class_Io_Poll_Trigger();

	php_io_operation_queue_ce = register_class_Io_OperationQueue();

	php_io_poll_operation_queue_ce = register_class_Io_Poll_OperationQueue(php_io_operation_queue_ce);
	php_io_poll_operation_queue_ce->create_object = php_io_opqueue_create_object;
	memcpy(&php_io_opqueue_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	php_io_opqueue_handlers.offset = offsetof(php_io_opqueue_obj, std);
	php_io_opqueue_handlers.free_obj = php_io_poll_operation_queue_free_object;
	php_io_opqueue_handlers.get_gc = php_io_poll_operation_queue_get_gc;
	php_io_opqueue_handlers.clone_obj = NULL;
	php_io_poll_operation_queue_ce->default_object_handlers = &php_io_opqueue_handlers;

	php_io_poll_context_ce = zend_hash_str_find_ptr(CG(class_table), ZEND_STRL("io\\poll\\context"));
	ZEND_ASSERT(php_io_poll_context_ce != NULL);

	php_io_hooks_ce = register_class_Io_Hooks_Hooks();
	php_io_hooks_capability_ce = register_class_Io_Hooks_Capability();

	zend_register_functions(NULL, ext_functions, NULL, type);

	php_io_op_zobj_detach = php_io_operation_detach;
	php_io_registration_zobj_detach = php_io_registration_detach;

	return SUCCESS;
}
