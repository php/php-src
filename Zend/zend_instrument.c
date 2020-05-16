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

struct zend_instruments_list {
	struct zend_instruments_list *prev;
	zend_instrument instrument;
};
typedef struct zend_instruments_list zend_instruments_list;

static zend_instruments_list *_zend_instruments;

ZEND_API void zend_instrument_init(void)
{
	_zend_instruments = NULL;
}

ZEND_API void zend_instrument_shutdown(void)
{
	zend_instruments_list *curr, *prev;
	for (curr = _zend_instruments; curr; curr = prev) {
		prev = curr->prev;
		free(curr);
	}
}

ZEND_API void zend_instrument_register(zend_instrument instrument)
{
	zend_instruments_list *node = malloc(sizeof(zend_instruments_list));
	node->instrument = instrument;
	node->prev = _zend_instruments;
	_zend_instruments = node;
}

struct zend_instrument_handlers_list {
	struct zend_instrument_handlers_list *prev;
	zend_instrument_handlers handlers;
	size_t count;
};
typedef struct zend_instrument_handlers_list zend_instrument_handlers_list;


extern inline void zend_instrument_call_begin_handlers(
	zend_execute_data *execute_data, zend_instrument_cache *cache);
extern inline void zend_instrument_call_end_handlers(
	zend_execute_data *execute_data, zend_instrument_cache *cache);

static zend_instrument_handlers_list *_instrument_add(
	zend_instrument_handlers_list *instruments,
	zend_instrument_handlers handlers)
{
	if (!handlers.begin && !handlers.end) {
		return instruments;
	}

	zend_instrument_handlers_list *n =
		emalloc(sizeof(zend_instrument_handlers_list));
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

static zend_instrument_cache *_instrument_attach_handler(
	zend_function *function,
	zend_instrument_handlers_list *handlers_list)
{
	zend_instrument_cache *cache =
		malloc(sizeof(zend_instrument_cache));
	cache->instruments_len = handlers_list->count;
	cache->handlers =
		calloc(handlers_list->count, sizeof(zend_instrument_handlers));

	zend_instrument_handlers *handlers = cache->handlers;
	zend_instrument_handlers_list *curr;
	for (curr = handlers_list; curr; curr = curr->prev) {
		*handlers++ = curr->handlers;
	}

	ZEND_MAP_PTR_SET(function->common.instrument_cache, cache);

	return cache;
}

ZEND_API void zend_instrument_install_handlers(zend_function *function)
{
	zend_instrument_handlers_list *handlers_list = NULL;
	zend_instruments_list *elem;

	for (elem = _zend_instruments; elem; elem = elem->prev) {
		zend_instrument_handlers handlers = elem->instrument(function);
		handlers_list = _instrument_add(handlers_list, handlers);
	}

	if (handlers_list) {
		_instrument_attach_handler(function, handlers_list);

		// cleanup handlers_list
		zend_instrument_handlers_list *curr, *prev;
		for (curr = handlers_list; curr; curr = prev) {
			prev = curr->prev;
			efree(curr);
		}
	} else {
		ZEND_MAP_PTR_SET(function->common.instrument_cache, ZEND_NOT_INSTRUMENTED);
	}
}
