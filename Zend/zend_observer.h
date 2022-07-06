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

#ifndef ZEND_OBSERVER_H
#define ZEND_OBSERVER_H

#include "zend.h"
#include "zend_compile.h"

BEGIN_EXTERN_C()

extern ZEND_API int zend_observer_fcall_op_array_extension;

#define ZEND_OBSERVER_ENABLED (zend_observer_fcall_op_array_extension != -1)

#define ZEND_OBSERVER_FCALL_BEGIN(execute_data) do { \
		if (ZEND_OBSERVER_ENABLED) { \
			zend_observer_fcall_begin(execute_data); \
		} \
	} while (0)

#define ZEND_OBSERVER_FCALL_END(execute_data, return_value) do { \
		if (ZEND_OBSERVER_ENABLED) { \
			zend_observer_fcall_end(execute_data, return_value); \
		} \
	} while (0)

typedef void (*zend_observer_fcall_begin_handler)(zend_execute_data *execute_data);
typedef void (*zend_observer_fcall_end_handler)(zend_execute_data *execute_data, zval *retval);

typedef struct _zend_observer_fcall_handlers {
	zend_observer_fcall_begin_handler begin;
	zend_observer_fcall_end_handler end;
} zend_observer_fcall_handlers;

/* If the fn should not be observed then return {NULL, NULL} */
typedef zend_observer_fcall_handlers (*zend_observer_fcall_init)(zend_execute_data *execute_data);

// Call during minit/startup ONLY
ZEND_API void zend_observer_fcall_register(zend_observer_fcall_init);

ZEND_API void zend_observer_startup(void); // Called by engine before MINITs
ZEND_API void zend_observer_post_startup(void); // Called by engine after MINITs
ZEND_API void zend_observer_activate(void);
ZEND_API void zend_observer_deactivate(void);
ZEND_API void zend_observer_shutdown(void);

ZEND_API void ZEND_FASTCALL zend_observer_fcall_begin(
	zend_execute_data *execute_data);

ZEND_API void ZEND_FASTCALL zend_observer_generator_resume(
	zend_execute_data *execute_data);

ZEND_API void ZEND_FASTCALL zend_observer_fcall_end(
	zend_execute_data *execute_data,
	zval *return_value);

ZEND_API void zend_observer_fcall_end_all(void);

typedef void (*zend_observer_error_cb)(int type, const char *error_filename, uint32_t error_lineno, zend_string *message);

ZEND_API void zend_observer_error_register(zend_observer_error_cb callback);
void zend_observer_error_notify(int type, const char *error_filename, uint32_t error_lineno, zend_string *message);

END_EXTERN_C()

#endif /* ZEND_OBSERVER_H */
