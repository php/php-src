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

#ifndef ZEND_INSTRUMENT_H
#define ZEND_INSTRUMENT_H

BEGIN_EXTERN_C()

typedef void (*zend_instrument_begin_handler)(zend_execute_data *ex);
typedef void (*zend_instrument_end_handler)(zend_execute_data *ex/*, zval *return_value*/);

struct zend_instrument_handlers {
	zend_instrument_begin_handler begin;

	/* todo: need to figure out how to tell end that it is exiting normally
	 * or not; checking for EG(exception) is not enough (unclean shutdown).
	 */
	zend_instrument_end_handler end;
};
typedef struct zend_instrument_handlers zend_instrument_handlers;

struct zend_instrument_cache {
	// todo: use arena
	size_t instruments_len;
	zend_instrument_handlers *handlers;
};
typedef struct zend_instrument_cache zend_instrument_cache;

/* If the fn should not be instrumented then return {NULL, NULL} */
typedef zend_instrument_handlers (*zend_instrument)(zend_function *func);

#define ZEND_NOT_INSTRUMENTED ((void *) 1)

// Call during minit/startup ONLY
ZEND_API void zend_instrument_register(zend_instrument cb);

// Called by engine before MINITs
ZEND_API void zend_instrument_init(void);
ZEND_API void zend_instrument_shutdown(void);

ZEND_API void zend_instrument_install_handlers(zend_function *function);

inline void zend_instrument_call_begin_handlers(
	zend_execute_data *execute_data, zend_instrument_cache *cache)
{
	zend_instrument_handlers *handler;
	zend_instrument_handlers *stop =
		cache->handlers + cache->instruments_len;

	for (handler = cache->handlers; handler != stop; ++handler) {
		if (handler->begin) {
			handler->begin(execute_data);
		}
	}
}

inline void zend_instrument_call_end_handlers(
	zend_execute_data *execute_data, zend_instrument_cache *cache)
{
	zend_instrument_handlers *handler =
		cache->handlers + cache->instruments_len;
	zend_instrument_handlers *stop = cache->handlers;

	// todo: send return_value to end, maybe protected via zval copy
	// todo: decide if this should run in reverse order or not
	while (handler-- != stop) {
		if (handler->end) {
			handler->end(execute_data);
		}
	}
}

END_EXTERN_C()

#endif /* ZEND_INSTRUMENT_H */
