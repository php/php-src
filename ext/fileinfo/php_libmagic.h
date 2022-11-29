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
  | Author: Anatol Belski <ab@php.net>                                   |
  +----------------------------------------------------------------------+
*/

#ifndef PHP_LIBMAGIC_H
#define PHP_LIBMAGIC_H

#include "php_fileinfo.h"

#include "main/php_network.h"
#include "ext/standard/php_string.h"
#include "ext/pcre/php_pcre.h"

#ifdef PHP_WIN32
#include "win32/param.h"
#include "win32/unistd.h"

#ifdef _WIN64
#define FINFO_LSEEK_FUNC _lseeki64
#else
#define FINFO_LSEEK_FUNC _lseek
#endif
#define FINFO_READ_FUNC _read

#define strtoull _strtoui64


#define HAVE_ASCTIME_R 1
#define asctime_r php_asctime_r
#define HAVE_CTIME_R 1
#define ctime_r php_ctime_r

#else
#define FINFO_LSEEK_FUNC lseek
#define FINFO_READ_FUNC read
#endif

#if defined(__hpux) && !defined(HAVE_STRTOULL)
#if SIZEOF_LONG == 8
# define strtoull strtoul
#else
# define strtoull __strtoull
#endif
#endif

#ifndef offsetof
#define offsetof(STRUCTURE,FIELD) ((int)((char*)&((STRUCTURE*)0)->FIELD))
#endif

#ifndef UINT32_MAX
# define UINT32_MAX (0xffffffff)
#endif

#ifndef PREG_OFFSET_CAPTURE
# define PREG_OFFSET_CAPTURE                 (1<<8)
#endif

#define abort()	zend_error_noreturn(E_ERROR, "fatal libmagic error")

zend_string* convert_libmagic_pattern(const char *val, size_t len, uint32_t options);

#endif	/* PHP_LIBMAGIC_H */
