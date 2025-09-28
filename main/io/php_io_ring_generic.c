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
   | Authors: Jakub Zelenka <bukka@php.net>                               |
   +----------------------------------------------------------------------+
*/

#include "php_io_internal.h"

/* Generic fallback - used when no platform-specific ring implementation is compiled */
#if !defined(__linux__) && !defined(_WIN32) && !defined(__FreeBSD__) && !defined(__OpenBSD__) && !defined(__NetBSD__) && !defined(__APPLE__) && !defined(__sun)

void php_io_register_ring(php_io_ring_ops *ops, uint32_t *capabilities)
{
    /* No ring support in generic implementation */
    ops->create = NULL;
    ops->submit = NULL;
    ops->wait_cqe = NULL;
    ops->cqe_seen = NULL;
    ops->destroy = NULL;
    ops->capabilities = 0;
}

#endif
