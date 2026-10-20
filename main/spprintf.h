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
