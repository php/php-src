/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright © Zend by Perforce and Contributors.                       |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Authors: Max Kellermann <max.kellermann@ionos.com>                   |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_MMAP_H
#define ZEND_MMAP_H

#include "zend_portability.h"

#ifdef HAVE_PRCTL
# include <sys/prctl.h>

/* fallback definitions if our libc is older than the kernel */
# ifndef PR_SET_VMA
#  define PR_SET_VMA 0x53564d41
# endif
# ifndef PR_SET_VMA_ANON_NAME
#  define PR_SET_VMA_ANON_NAME 0
# endif
#endif // HAVE_PRCTL

/**
 * Set a name for the specified memory area.
 *
 * This feature requires Linux 5.17.
 */
static zend_always_inline void zend_mmap_set_name(const void *start, size_t len, const char *name)
{
#ifdef HAVE_PRCTL
	prctl(PR_SET_VMA, PR_SET_VMA_ANON_NAME, (unsigned long)start, len, (unsigned long)name);
#endif
}

#endif /* ZEND_MMAP_H */
