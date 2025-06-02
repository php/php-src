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
   | Author:                                                              |
   +----------------------------------------------------------------------+
*/

#include "php.h"

#ifdef USE_STRLCAT_PHP_IMPL

/*
 * Copyright (c) 1998 Todd C. Miller <Todd.Miller@courtesan.com>
 * All rights reserved.
 * (Original OpenBSD license text preserved above)
 */

#include <sys/types.h>
#include <string.h>

/**
 * Appends src to string dst of size siz (unlike strncat, siz is the
 * full size of dst, not space left). At most siz-1 characters
 * will be copied. Always NUL terminates (unless siz <= strlen(dst)).
 * Returns strlen(src) + MIN(siz, strlen(initial dst)).
 * If retval >= siz, truncation occurred.
 */
PHPAPI size_t php_strlcat(char *dst, const char *src, size_t siz)
{
    const char * const orig_dst = dst;
    const char * const orig_src = src;
    size_t remaining = siz;

    // Find end of dst within bounds
    while (remaining-- != 0 && *dst != '\0') {
        dst++;
    }
    const size_t dlen = dst - orig_dst;
    remaining = siz - dlen;

    // No space left - just return total length
    if (remaining == 0) {
        return dlen + strlen(src);
    }

    // Copy src to dst with remaining space (leave room for NUL)
    while (*src != '\0') {
        if (remaining > 1) {
            *dst++ = *src;
            remaining--;
        }
        src++;
    }
    *dst = '\0';

    return dlen + (src - orig_src);
}

#endif /* !HAVE_STRLCAT */
