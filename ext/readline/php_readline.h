/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2017 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Thies C. Arntzen <thies@thieso.net>                          |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_READLINE_H
#define PHP_READLINE_H

#if HAVE_LIBREADLINE || HAVE_LIBEDIT
#ifdef ZTS
#warning Readline module will *NEVER* be thread-safe
#endif

extern zend_module_entry readline_module_entry;
#define phpext_readline_ptr &readline_module_entry

#include "php_version.h"
#define PHP_READLINE_VERSION PHP_VERSION

#else

#define phpext_readline_ptr NULL

#endif /* HAVE_LIBREADLINE */

#endif /* PHP_READLINE_H */

