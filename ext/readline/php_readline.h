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
   | Author: Thies C. Arntzen <thies@thieso.net>                          |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_READLINE_H
#define PHP_READLINE_H

#ifdef HAVE_LIBEDIT
#define READLINE_LIB "libedit"
#else
#define READLINE_LIB "readline"
#endif

#if defined(HAVE_LIBREADLINE) || defined(HAVE_LIBEDIT)

extern zend_module_entry readline_module_entry;
#define phpext_readline_ptr &readline_module_entry

#include "php_version.h"
#define PHP_READLINE_VERSION PHP_VERSION

#else

#define phpext_readline_ptr NULL

#endif /* HAVE_LIBREADLINE */

#endif /* PHP_READLINE_H */
