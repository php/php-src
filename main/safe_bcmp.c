/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: David Carlier <devnexen@gmail.com>                           |
   +----------------------------------------------------------------------+
*/

#include "php.h"
#include <string.h>

/**
 * Constant-time binary comparison of two zend_strings.
 * 
 * @param a First string to compare
 * @param b Second string to compare
 * @return 0 if strings are identical, -1 if lengths differ, non-zero otherwise
 * 
 * @note This is security-sensitive code. Timing attacks are mitigated by:
 *       - Using volatile to prevent compiler optimizations
 *       - Constant-time comparison regardless of input
 *       - Complete length comparison before content comparison
 */
PHPAPI int php_safe_bcmp(const zend_string *a, const zend_string *b)
{
    const volatile unsigned char *ua = (const volatile unsigned char *)ZSTR_VAL(a);
    const volatile unsigned char *ub = (const volatile unsigned char *)ZSTR_VAL(b);
    const size_t len_a = ZSTR_LEN(a);
    const size_t len_b = ZSTR_LEN(b);
    int r = 0;

    // Early length check (safe as lengths are not secret)
    if (len_a != len_b) {
        return -1;
    }

    // Constant-time comparison
    for (size_t i = 0; i < len_a; ++i) {
        r |= ua[i] ^ ub[i];
    }

    return r;
}
