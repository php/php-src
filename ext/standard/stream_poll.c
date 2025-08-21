/*
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | https://www.php.net/license/3_01.txt                                 |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Jakub Zelenka <bukka@php.net>                               |
  +----------------------------------------------------------------------+
*/

#include "php.h"
#include "php_network.h"
#include "php_poll.h"
#include "stream_poll_arginfo.h"
#include "zend_exceptions.h"

static zend_class_entry *stream_poll_context_class_entry;
static zend_class_entry *stream_poll_event_class_entry;
static zend_class_entry *stream_poll_exception_class_entry;
static zend_object_handlers php_stream_poll_context_object_handlers;

/* Internal structure to hold stream data */
typedef struct {
	php_stream *stream;
	zval data;
} php_stream_poll_entry;

/* Object wrapper for userspace */
typedef struct {
	php_poll_ctx *ctx;
	HashTable *stream_map; /* Maps fd -> php_stream_poll_entry */
	zend_object std;
} php_stream_poll_context_object;

#define PHP_STREAM_POLL_CONTEXT_OBJ_FROM_ZOBJ php_stream_poll_context_object_from_zend_object
#define PHP_STREAM_POLL_CONTEXT_OBJ_FROM_ZV(_zv) \
	php_stream_poll_context_object_from_zend_object(Z_OBJ_P(_zv))

static inline php_stream_poll_context_object *php_stream_poll_context_object_from_zend_object(
		zend_object *obj)
{
	return (php_stream_poll_context_object *) ((char *) (obj) -XtOffsetOf(
			php_stream_poll_context_object, std));
}

static void stream_map_entry_dtor(zval *zv)
{
	php_stream_poll_entry *entry = Z_PTR_P(zv);
	if (entry) {
		zval_ptr_dtor(&entry->data);
		efree(entry);
	}
}

static void php_stream_poll_context_free_object_storage(zend_object *obj)
{
	php_stream_poll_context_object *intern = PHP_STREAM_POLL_CONTEXT_OBJ_FROM_ZOBJ(obj);

	if (intern->ctx) {
		php_poll_destroy(intern->ctx);
	}
	if (intern->stream_map) {
		zend_hash_destroy(intern->stream_map);
		efree(intern->stream_map);
	}
	zend_object_std_dtor(&intern->std);
}

static inline zend_object *php_stream_poll_context_create_object_ex(
		zend_class_entry *ce, php_poll_ctx **ctx)
{
	php_stream_poll_context_object *intern
			= zend_object_alloc(sizeof(php_stream_poll_context_object), ce);

	zend_object_std_init(&intern->std, ce);
	object_properties_init(&intern->std, ce);

	intern->ctx = NULL;
	intern->stream_map = NULL;
	*ctx = NULL;

	return &intern->std;
}

static zend_object *php_stream_poll_context_create_object(zend_class_entry *ce)
{
	php_poll_ctx *ctx;
	return php_stream_poll_context_create_object_ex(ce, &ctx);
}

/* Create a new stream polling context */
PHP_FUNCTION(stream_poll_create)
{
	zend_long backend_long = PHP_POLL_BACKEND_AUTO;
	zend_string *backend_str = NULL;
	php_poll_ctx *poll_ctx;

	ZEND_PARSE_PARAMETERS_START(0, 1)
	Z_PARAM_OPTIONAL
	Z_PARAM_STR_OR_LONG(backend_str, backend_long)
	ZEND_PARSE_PARAMETERS_END();

	if (backend_str == NULL) {
		poll_ctx = php_poll_create((php_poll_backend_type) backend_long, false);
	} else {
		poll_ctx = php_poll_create_by_name(ZSTR_VAL(backend_str), false);
	}
	if (!poll_ctx) {
		zend_throw_exception(
				stream_poll_exception_class_entry, "Failed to create polling context", 0);
		RETURN_THROWS();
	}
	if (php_poll_init(poll_ctx) != SUCCESS) {
		php_poll_destroy(poll_ctx);
		zend_throw_exception(
				stream_poll_exception_class_entry, "Failed to initialize polling context", 0);
		RETURN_THROWS();
	}

	/* Create object */
	object_init_ex(return_value, stream_poll_context_class_entry);
	php_stream_poll_context_object *intern = PHP_STREAM_POLL_CONTEXT_OBJ_FROM_ZV(return_value);

	intern->ctx = poll_ctx;
	intern->stream_map = emalloc(sizeof(HashTable));
	zend_hash_init(intern->stream_map, 8, NULL, stream_map_entry_dtor, 0);
}

static php_stream_poll_context_object *get_stream_poll_context_object(zval *obj)
{
	if (Z_TYPE_P(obj) != IS_OBJECT
			|| !instanceof_function(Z_OBJCE_P(obj), stream_poll_context_class_entry)) {
		return NULL;
	}
	return PHP_STREAM_POLL_CONTEXT_OBJ_FROM_ZV(obj);
}

/* Add a stream to the polling context */
PHP_FUNCTION(stream_poll_add)
{
	zval *zpoll_ctx, *zdata = NULL;
	zend_long events;
	php_stream *stream;
	php_stream_poll_context_object *context;
	php_stream_poll_entry *entry;
	php_socket_t fd;

	ZEND_PARSE_PARAMETERS_START(3, 4)
	Z_PARAM_OBJECT_OF_CLASS(zpoll_ctx, stream_poll_context_class_entry)
	PHP_Z_PARAM_STREAM(stream)
	Z_PARAM_LONG(events)
	Z_PARAM_OPTIONAL
	Z_PARAM_ZVAL(zdata)
	ZEND_PARSE_PARAMETERS_END();

	context = get_stream_poll_context_object(zpoll_ctx);
	if (!context || !context->ctx) {
		zend_throw_exception(stream_poll_exception_class_entry, "Invalid polling context", 0);
		RETURN_THROWS();
	}

	/* Get file descriptor from stream */
	if (php_stream_cast(stream, PHP_STREAM_AS_FD_FOR_SELECT | PHP_STREAM_CAST_INTERNAL,
				(void *) &fd,
				1) != SUCCESS
			|| fd == -1) {
		zend_throw_exception(stream_poll_exception_class_entry, "Stream cannot be polled", 0);
		RETURN_THROWS();
	}

	/* Check if already exists */
	if (zend_hash_index_exists(context->stream_map, (zend_ulong) fd)) {
		zend_throw_exception(stream_poll_exception_class_entry, "Stream already added", 0);
		RETURN_THROWS();
	}

	/* Create entry */
	entry = emalloc(sizeof(php_stream_poll_entry));
	entry->stream = stream;
	if (zdata) {
		ZVAL_COPY(&entry->data, zdata);
	} else {
		ZVAL_NULL(&entry->data);
	}

	/* Add to internal poll context */
	if (php_poll_add(context->ctx, (int) fd, (uint32_t) events, entry) != SUCCESS) {
		zval_ptr_dtor(&entry->data);
		efree(entry);
		zend_throw_exception(stream_poll_exception_class_entry, "Failed to add stream to poll", 0);
		RETURN_THROWS();
	}

	/* Add to our mapping */
	zend_hash_index_add_ptr(context->stream_map, (zend_ulong) fd, entry);
}

/* Modify events for a stream in the polling context */
PHP_FUNCTION(stream_poll_modify)
{
	zval *zpoll_ctx, *zdata = NULL;
	zend_long events;
	php_stream *stream;
	php_stream_poll_context_object *context;
	php_stream_poll_entry *entry;
	php_socket_t fd;

	ZEND_PARSE_PARAMETERS_START(3, 4)
	Z_PARAM_OBJECT_OF_CLASS(zpoll_ctx, stream_poll_context_class_entry)
	PHP_Z_PARAM_STREAM(stream)
	Z_PARAM_LONG(events)
	Z_PARAM_OPTIONAL
	Z_PARAM_ZVAL(zdata)
	ZEND_PARSE_PARAMETERS_END();

	context = get_stream_poll_context_object(zpoll_ctx);
	if (!context || !context->ctx) {
		zend_throw_exception(stream_poll_exception_class_entry, "Invalid polling context", 0);
		RETURN_THROWS();
	}

	/* Get file descriptor from stream */
	if (php_stream_cast(stream, PHP_STREAM_AS_FD_FOR_SELECT | PHP_STREAM_CAST_INTERNAL,
				(void *) &fd,
				1) != SUCCESS
			|| fd == -1) {
		zend_throw_exception(stream_poll_exception_class_entry, "Stream cannot be polled", 0);
		RETURN_THROWS();
	}

	/* Find existing entry */
	entry = zend_hash_index_find_ptr(context->stream_map, (zend_ulong) fd);
	if (!entry) {
		zend_throw_exception(stream_poll_exception_class_entry, "Stream not found", 0);
		RETURN_THROWS();
	}

	/* Update data if provided */
	if (zdata) {
		zval_ptr_dtor(&entry->data);
		ZVAL_COPY(&entry->data, zdata);
	}

	/* Modify in poll context */
	if (php_poll_modify(context->ctx, (int) fd, (uint32_t) events, entry) != SUCCESS) {
		zend_throw_exception(
				stream_poll_exception_class_entry, "Failed to modify stream in poll", 0);
		RETURN_THROWS();
	}
}

/* Remove a stream from the polling context */
PHP_FUNCTION(stream_poll_remove)
{
	zval *zpoll_ctx;
	php_stream *stream;
	php_stream_poll_context_object *context;
	php_socket_t fd;

	ZEND_PARSE_PARAMETERS_START(2, 2)
	Z_PARAM_OBJECT_OF_CLASS(zpoll_ctx, stream_poll_context_class_entry)
	PHP_Z_PARAM_STREAM(stream)
	ZEND_PARSE_PARAMETERS_END();

	context = get_stream_poll_context_object(zpoll_ctx);
	if (!context || !context->ctx) {
		zend_throw_exception(stream_poll_exception_class_entry, "Invalid polling context", 0);
		RETURN_THROWS();
	}

	/* Get file descriptor from stream */
	if (php_stream_cast(stream, PHP_STREAM_AS_FD_FOR_SELECT | PHP_STREAM_CAST_INTERNAL,
				(void *) &fd,
				1) != SUCCESS
			|| fd == -1) {
		zend_throw_exception(stream_poll_exception_class_entry, "Stream cannot be polled", 0);
		RETURN_THROWS();
	}

	/* Remove from poll context */
	if (php_poll_remove(context->ctx, (int) fd) != SUCCESS) {
		zend_throw_exception(
				stream_poll_exception_class_entry, "Failed to remove stream from poll", 0);
		RETURN_THROWS();
	}

	/* Remove from our mapping */
	zend_hash_index_del(context->stream_map, (zend_ulong) fd);
}

/* Wait for events on streams in the polling context */
PHP_FUNCTION(stream_poll_wait)
{
	zval *zpoll_ctx;
	zend_long timeout = -1;
	zend_long max_events = -1;
	php_stream_poll_context_object *context;
	php_poll_event *events;
	int num_events, i;

	ZEND_PARSE_PARAMETERS_START(1, 3)
	Z_PARAM_OBJECT_OF_CLASS(zpoll_ctx, stream_poll_context_class_entry)
	Z_PARAM_OPTIONAL
	Z_PARAM_LONG(timeout)
	Z_PARAM_LONG(max_events)
	ZEND_PARSE_PARAMETERS_END();

	context = get_stream_poll_context_object(zpoll_ctx);
	if (!context || !context->ctx) {
		zend_throw_exception(stream_poll_exception_class_entry, "Invalid polling context", 0);
		RETURN_THROWS();
	}

	if (max_events <= 0) {
		// TODO: get some recommended value from polling api basend on number of added events
		max_events = 1024;
	}
	events = emalloc(sizeof(php_poll_event) * max_events);

	num_events = php_poll_wait(context->ctx, events, max_events, (int) timeout);

	if (num_events < 0) {
		efree(events);
		zend_throw_exception(stream_poll_exception_class_entry, "Poll wait failed", 0);
		RETURN_THROWS();
	}

	array_init(return_value);

	for (i = 0; i < num_events; i++) {
		php_stream_poll_entry *entry = (php_stream_poll_entry *) events[i].data;
		zval event_obj;

		object_init_ex(&event_obj, stream_poll_event_class_entry);

		/* Set stream property */
		zval stream_zval;
		php_stream_to_zval(entry->stream, &stream_zval);
		zend_update_property(
				stream_poll_event_class_entry, Z_OBJ(event_obj), ZEND_STRL("stream"), &stream_zval);

		/* Set events property */
		zend_update_property_long(stream_poll_event_class_entry, Z_OBJ(event_obj),
				ZEND_STRL("events"), events[i].revents);

		/* Set data property */
		zend_update_property(
				stream_poll_event_class_entry, Z_OBJ(event_obj), ZEND_STRL("data"), &entry->data);

		add_next_index_zval(return_value, &event_obj);
	}

	efree(events);
}

/* Get the backend name for the polling context */
PHP_FUNCTION(stream_poll_backend_name)
{
	zval *zpoll_ctx;
	php_stream_poll_context_object *context;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_OBJECT_OF_CLASS(zpoll_ctx, stream_poll_context_class_entry)
	ZEND_PARSE_PARAMETERS_END();

	context = get_stream_poll_context_object(zpoll_ctx);
	if (!context || !context->ctx) {
		zend_throw_exception(stream_poll_exception_class_entry, "Invalid polling context", 0);
		RETURN_THROWS();
	}

	const char *backend_name = php_poll_backend_name(context->ctx);
	RETURN_STRING(backend_name);
}

PHP_METHOD(StreamPollContext, __construct)
{
	zend_throw_error(
			NULL, "Cannot directly construct StreamPollContext, use stream_poll_create() instead");
}

/* Initialize the stream poll classes - add to PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(stream_poll)
{
	/* Register symbols */
	register_stream_poll_symbols(module_number);

	/* Register classes */
	stream_poll_context_class_entry = register_class_StreamPollContext();
	stream_poll_context_class_entry->create_object = php_stream_poll_context_create_object;
	stream_poll_context_class_entry->default_object_handlers
			= &php_stream_poll_context_object_handlers;

	stream_poll_event_class_entry = register_class_StreamPollEvent();
	stream_poll_exception_class_entry = register_class_StreamPollException(zend_ce_exception);

	/* Set up object handlers */
	memcpy(&php_stream_poll_context_object_handlers, &std_object_handlers,
			sizeof(zend_object_handlers));
	php_stream_poll_context_object_handlers.offset
			= XtOffsetOf(php_stream_poll_context_object, std);
	php_stream_poll_context_object_handlers.free_obj = php_stream_poll_context_free_object_storage;

	/* Register poll backends */
	php_poll_register_backends();

	return SUCCESS;
}
