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
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Marcus Boerger <helly@php.net>                              |
   |          Sterling Hughes <sterling@php.net>                          |
   |          Zeev Suraski <zeev@php.net>                                 |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_EXCEPTIONS_H
#define ZEND_EXCEPTIONS_H

BEGIN_EXTERN_C()

extern ZEND_API zend_class_entry *zend_ce_throwable;
extern ZEND_API zend_class_entry *zend_ce_exception;
extern ZEND_API zend_class_entry *zend_ce_error_exception;
extern ZEND_API zend_class_entry *zend_ce_error;
extern ZEND_API zend_class_entry *zend_ce_compile_error;
extern ZEND_API zend_class_entry *zend_ce_parse_error;
extern ZEND_API zend_class_entry *zend_ce_type_error;
extern ZEND_API zend_class_entry *zend_ce_argument_count_error;
extern ZEND_API zend_class_entry *zend_ce_value_error;
extern ZEND_API zend_class_entry *zend_ce_arithmetic_error;
extern ZEND_API zend_class_entry *zend_ce_division_by_zero_error;
extern ZEND_API zend_class_entry *zend_ce_unhandled_match_error;
extern ZEND_API zend_class_entry *zend_ce_request_parse_body_exception;

ZEND_API void zend_exception_set_previous(zend_object *exception, zend_object *add_previous);
ZEND_API void zend_exception_save(void);
ZEND_API void zend_exception_restore(void);

ZEND_API ZEND_COLD void zend_throw_exception_internal(zend_object *exception);

void zend_register_default_exception(void);

ZEND_API zend_class_entry *zend_get_exception_base(zend_object *object);

/* Deprecated - Use zend_ce_exception directly instead */
ZEND_API zend_class_entry *zend_exception_get_default(void);

/* Deprecated - Use zend_ce_error_exception directly instead */
ZEND_API zend_class_entry *zend_get_error_exception(void);

ZEND_API void zend_register_default_classes(void);

/* exception_ce   NULL, zend_ce_exception, zend_ce_error, or a derived class
 * message        NULL or the message of the exception */
ZEND_API ZEND_COLD zend_object *zend_throw_exception(zend_class_entry *exception_ce, const char *message, zend_long code);
ZEND_API ZEND_COLD zend_object *zend_throw_exception_ex(zend_class_entry *exception_ce, zend_long code, const char *format, ...) ZEND_ATTRIBUTE_FORMAT(printf, 3, 4);
ZEND_API ZEND_COLD void zend_throw_exception_object(zval *exception);
ZEND_API void zend_clear_exception(void);

ZEND_API zend_object *zend_throw_error_exception(zend_class_entry *exception_ce, zend_string *message, zend_long code, int severity);

extern ZEND_API void (*zend_throw_exception_hook)(zend_object *ex);

/* show an exception using zend_error(severity,...), severity should be E_ERROR */
ZEND_API ZEND_COLD zend_result zend_exception_error(zend_object *exception, int severity);
ZEND_NORETURN void zend_exception_uncaught_error(const char *prefix, ...) ZEND_ATTRIBUTE_FORMAT(printf, 1, 2);
ZEND_API zend_string *zend_trace_to_string(HashTable *trace, bool include_main);

ZEND_API ZEND_COLD zend_object *zend_create_unwind_exit(void);
ZEND_API ZEND_COLD zend_object *zend_create_graceful_exit(void);
ZEND_API ZEND_COLD void zend_throw_unwind_exit(void);
ZEND_API ZEND_COLD void zend_throw_graceful_exit(void);
ZEND_API bool zend_is_unwind_exit(const zend_object *ex);
ZEND_API bool zend_is_graceful_exit(const zend_object *ex);

#include "zend_globals.h"

static zend_always_inline void zend_rethrow_exception(zend_execute_data *execute_data)
{
	if (EX(opline)->opcode != ZEND_HANDLE_EXCEPTION) {
		EG(opline_before_exception) = EX(opline);
		EX(opline) = EG(exception_op);
	}
}

END_EXTERN_C()

#endif
