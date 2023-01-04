/*
   +----------------------------------------------------------------------+
   | Zend JIT                                                             |
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
   | Authors: Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/

#ifndef HAVE_JIT_TARGET_H
#define HAVE_JIT_TARGET_H

#if defined(__x86_64__) || defined(i386) || defined(ZEND_WIN32)
# define ZEND_JIT_TARGET_X86   1
# define ZEND_JIT_TARGET_ARM64 0
#elif defined (__aarch64__)
# define ZEND_JIT_TARGET_X86   0
# define ZEND_JIT_TARGET_ARM64 1
#else
# error "JIT not supported on this platform"
#endif

#endif /* HAVE_JIT_TARGET_H */
