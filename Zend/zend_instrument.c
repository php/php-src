/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) Zend Technologies Ltd. (http://www.zend.com)           |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Levi Morrison <levim@php.net>                               |
   |          Sammy Kaye Powers <sammyk@php.net>                          |
   +----------------------------------------------------------------------+
*/

#include "zend.h"
#include "zend_API.h"
#include "zend_instrument.h"

struct zend_instrument_fcall_init_list {
	struct zend_instrument_fcall_init_list *prev;
	zend_instrument_fcall_init instrument;
};
typedef struct zend_instrument_fcall_init_list zend_instrument_fcall_init_list;

static zend_instrument_fcall_init_list *zend_instrument_fcalls;

ZEND_API void zend_instrument_init(void) {
	zend_instrument_fcalls = NULL;
}

ZEND_API void zend_instrument_shutdown(void) {
	zend_instrument_fcall_init_list *curr, *prev;
	for (curr = zend_instrument_fcalls; curr; curr = prev) {
		prev = curr->prev;
		free(curr);
	}
}

ZEND_API void zend_instrument_fcall_register(zend_instrument_fcall_init cb) {
	zend_instrument_fcall_init_list *node =
		malloc(sizeof(zend_instrument_fcall_init_list));
	node->instrument = cb;
	node->prev = zend_instrument_fcalls;
	zend_instrument_fcalls = node;
}

struct zend_instrument_fcall_list {
	struct zend_instrument_fcall_list *prev;
	zend_instrument_fcall handlers;
	size_t count;
};
typedef struct zend_instrument_fcall_list zend_instrument_fcall_list;


extern inline void zend_instrument_fcall_call_begin(
	zend_instrument_fcall_cache *cache,
	zend_execute_data *execute_data);

extern inline void zend_instrument_fcall_call_end(
	zend_instrument_fcall_cache *cache,
	zend_execute_data *execute_data,
	zval *return_value);

static zend_instrument_fcall_list *zend_instrument_fcall_add(
	zend_instrument_fcall_list *instruments,
	zend_instrument_fcall handlers)
{
	if (!handlers.begin && !handlers.end) {
		return instruments;
	}

	zend_instrument_fcall_list *n =
		emalloc(sizeof(zend_instrument_fcall_list));
	if (instruments) {
		n->prev = instruments;
		n->count = instruments->count + 1;
	} else {
		n->prev = NULL;
		n->count = 1;
	}
	n->handlers = handlers;
	return n;
}

static zend_instrument_fcall_cache *zend_instrument_fcall_list_attach(
	zend_function *function,
	zend_instrument_fcall_list *handlers_list)
{
	zend_instrument_fcall_cache *cache =
		malloc(sizeof(zend_instrument_fcall_cache));
	cache->instruments_len = handlers_list->count;
	cache->handlers =
		calloc(handlers_list->count, sizeof(zend_instrument_fcall));

	zend_instrument_fcall *handlers = cache->handlers;
	zend_instrument_fcall_list *curr;
	for (curr = handlers_list; curr; curr = curr->prev) {
		*handlers++ = curr->handlers;
	}

	ZEND_MAP_PTR_SET(function->common.instrument_cache, cache);

	return cache;
}

ZEND_API void zend_instrument_fcall_install(zend_function *function) {
	zend_instrument_fcall_list *fcall_list = NULL;
	zend_instrument_fcall_init_list *elem;

	for (elem = zend_instrument_fcalls; elem; elem = elem->prev) {
		zend_instrument_fcall handlers = elem->instrument(function);
		fcall_list = zend_instrument_fcall_add(fcall_list, handlers);
	}

	if (fcall_list) {
		zend_instrument_fcall_list_attach(function, fcall_list);

		// cleanup fcall_list
		zend_instrument_fcall_list *curr, *prev;
		for (curr = fcall_list; curr; curr = prev) {
			prev = curr->prev;
			efree(curr);
		}
	} else {
		ZEND_MAP_PTR_SET(function->common.instrument_cache,
		                 ZEND_INSTRUMENT_FCALL_NOT_INSTRUMENTED);
	}
}
