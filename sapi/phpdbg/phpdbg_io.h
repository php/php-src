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
   | Authors: Anatol Belski <ab@php.net>                                  |
   +----------------------------------------------------------------------+
*/

#ifndef PHPDBG_IO_H
#define PHPDBG_IO_H

#include "phpdbg.h"

PHPDBG_API int phpdbg_consume_stdin_line(char *buf);

PHPDBG_API int phpdbg_mixed_read(int fd, char *ptr, int len, int tmo);
PHPDBG_API int phpdbg_mixed_write(int fd, const char *ptr, int len);

#endif /* PHPDBG_IO_H */
