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

struct zend_instrument_fcall {
	void (*begin)(zend_execute_data *execute_data);

	/* todo: need to figure out how to tell end that it is exiting normally
	 * or not; checking for EG(exception) is not enough (unclean shutdown).
	 */
	void (*end)(zend_execute_data* execute_data, zval *return_value);
};
typedef struct zend_instrument_fcall zend_instrument_fcall;

struct zend_instrument_fcall_cache {
	// todo: use arena
	size_t instruments_len;
	zend_instrument_fcall *handlers;
};
typedef struct zend_instrument_fcall_cache zend_instrument_fcall_cache;

/* If the fn should not be instrumented then return {NULL, NULL} */
typedef zend_instrument_fcall (*zend_instrument_fcall_init)(zend_function *func);

#define ZEND_INSTRUMENT_FCALL_NOT_INSTRUMENTED ((void *) 1)

// Call during minit/startup ONLY
ZEND_API void zend_instrument_fcall_register(zend_instrument_fcall_init cb);

// Called by engine before MINITs
ZEND_API void zend_instrument_init(void);
ZEND_API void zend_instrument_shutdown(void);

ZEND_API void zend_instrument_fcall_install(zend_function *function);

inline void zend_instrument_fcall_call_begin(
	zend_instrument_fcall_cache *cache,
	zend_execute_data *execute_data)
{
	zend_instrument_fcall *handler;
	zend_instrument_fcall *stop =
		cache->handlers + cache->instruments_len;

	for (handler = cache->handlers; handler != stop; ++handler) {
		if (handler->begin) {
			handler->begin(execute_data);
		}
	}
}

inline void zend_instrument_fcall_call_end(
	zend_instrument_fcall_cache *cache,
	zend_execute_data *execute_data,
	zval *return_value)
{
	zend_instrument_fcall *handler =
		cache->handlers + cache->instruments_len;
	zend_instrument_fcall *stop = cache->handlers;

	// todo: send return_value to end, maybe protected via zval copy
	// todo: decide if this should run in reverse order or not
	while (handler-- != stop) {
		if (handler->end) {
			handler->end(execute_data, return_value);
		}
	}
}

END_EXTERN_C()

#endif /* ZEND_INSTRUMENT_H */
