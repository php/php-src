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
   | Author: Kévin Dunglas <kevin@dunglas.dev>                            |
   +----------------------------------------------------------------------+
 */

#ifndef ZEND_TIMER_H
#define ZEND_TIMER_H

#ifdef __linux__
#include "php_config.h"

#  if defined(ZTS) && defined(HAVE_TIMER_CREATE)
#define ZEND_TIMER 1

#include "zend_long.h"

ZEND_API void zend_timer_create(void);
ZEND_API void zend_timer_settime(zend_long seconds);
ZEND_API void zend_timer_delete(void);

#  endif
# endif
#endif
