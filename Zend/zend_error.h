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
*/

#ifndef ZEND_ERROR_H
#define ZEND_ERROR_H

#include "zend_errors.h" // for E_* (as zend_error_noreturn() parameter)
#include "zend_portability.h" // for BEGIN_EXTERN_C

#include <stdint.h>

typedef struct _zend_string zend_string;

BEGIN_EXTERN_C()

ZEND_API ZEND_COLD void zend_error(int type, const char *format, ...) ZEND_ATTRIBUTE_FORMAT(printf, 2, 3);
ZEND_API ZEND_COLD ZEND_NORETURN void zend_error_noreturn(int type, const char *format, ...) ZEND_ATTRIBUTE_FORMAT(printf, 2, 3);
/* For custom format specifiers like H */
ZEND_API ZEND_COLD void zend_error_unchecked(int type, const char *format, ...);
/* If filename is NULL the default filename is used. */
ZEND_API ZEND_COLD void zend_error_at(int type, zend_string *filename, uint32_t lineno, const char *format, ...) ZEND_ATTRIBUTE_FORMAT(printf, 4, 5);
ZEND_API ZEND_COLD ZEND_NORETURN void zend_error_at_noreturn(int type, zend_string *filename, uint32_t lineno, const char *format, ...) ZEND_ATTRIBUTE_FORMAT(printf, 4, 5);
ZEND_API ZEND_COLD void zend_error_zstr(int type, zend_string *message);
ZEND_API ZEND_COLD void zend_error_zstr_at(int type, zend_string *filename, uint32_t lineno, zend_string *message);

END_EXTERN_C()

#endif /* ZEND_H */
