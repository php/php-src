/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright © Zend Technologies Ltd., a subsidiary company of          |
   |     Perforce Software, Inc., and Contributors.                       |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Authors: Felix Bünemann <felix.buenemann@gmail.com>                |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_LOCALE_H
#define ZEND_LOCALE_H

#include "zend_portability.h"

#include <locale.h>

BEGIN_EXTERN_C()

/* zend_setlocale() keeps a table indexed by category, which needs LC_ALL not
 * to be negative, as it is on AIX */
#if defined(HAVE_USELOCALE) && LC_ALL >= 0
# define ZEND_THREAD_LOCALE 1
/* Like setlocale(), but during a request it only changes the locale of the
 * current thread */
ZEND_API const char *zend_setlocale(int category, const char *locale);
#else
/* On Windows the locale is already per thread in thread-safe builds, see
 * _configthreadlocale() */
# define zend_setlocale(category, locale) setlocale(category, locale)
#endif

END_EXTERN_C()

#endif /* ZEND_LOCALE_H */
