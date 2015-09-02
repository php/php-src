/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2015 The PHP Group                                |
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

/* $Id$ */

/*

The pbuf parameter of all spprintf version receives a pointer to the allocated
buffer. This buffer must be freed manually after usage using efree() function.
The buffer will always be terminated by a zero character. When pbuf is NULL
the function can be used to calculate the required size of the buffer but for
that purpose snprintf is faster. When both pbuf and the return value are 0
than you are out of memory.

There is also snprintf: See difference explained in snprintf.h

*/

#ifndef SPPRINTF_H
#define SPPRINTF_H

#include "snprintf.h"

BEGIN_EXTERN_C()
PHPAPI size_t spprintf( char **pbuf, size_t max_len, const char *format, ...) PHP_ATTRIBUTE_FORMAT(printf, 3, 4);

PHPAPI size_t vspprintf(char **pbuf, size_t max_len, const char *format, va_list ap) PHP_ATTRIBUTE_FORMAT(printf, 3, 0);

PHPAPI zend_string *vstrpprintf(size_t max_len, const char *format, va_list ap) PHP_ATTRIBUTE_FORMAT(printf, 2, 0);

PHPAPI zend_string *strpprintf(size_t max_len, const char *format, ...) PHP_ATTRIBUTE_FORMAT(printf, 2, 3);
END_EXTERN_C()

#endif /* SNPRINTF_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
