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
   | Authors: David Carlier <devnexen@gmail.com>                          |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_IO_FREEBSD_H
#define PHP_IO_FREEBSD_H

#define PHP_IO_PLATFORM_COPY php_io_freebsd_copy
#if defined(__DragonFly__)
#  define PHP_IO_PLATFORM_NAME "dragonfly"
#else
#  define PHP_IO_PLATFORM_NAME "freebsd"
#endif

ssize_t php_io_freebsd_copy(php_io_fd *src, php_io_fd *dest, size_t maxlen);

#endif /* PHP_IO_FREEBSD_H */
