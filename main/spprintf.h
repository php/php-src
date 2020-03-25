/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Marcus Boerger <helly@php.net>                               |
   +----------------------------------------------------------------------+
 */

#ifndef SPPRINTF_H
#define SPPRINTF_H

#include "snprintf.h"
#include "zend_smart_str_public.h"
#include "zend_smart_string_public.h"

BEGIN_EXTERN_C()
PHPAPI void php_printf_to_smart_string(smart_string *buf, const char *format, va_list ap);
PHPAPI void php_printf_to_smart_str(smart_str *buf, const char *format, va_list ap);
END_EXTERN_C()

#define spprintf zend_spprintf
#define strpprintf zend_strpprintf
#define vspprintf zend_vspprintf
#define vstrpprintf zend_vstrpprintf

#endif /* SPPRINTF_H */
