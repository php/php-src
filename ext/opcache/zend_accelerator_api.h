/*
   +----------------------------------------------------------------------+
   | Zend OPcache                                                         |
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
*/

#ifndef ZEND_ACCELERATOR_API_H
#define ZEND_ACCELERATOR_API_H

#include "Zend/zend_portability.h"

BEGIN_EXTERN_C()

/* Returns true during preloading */
ZEND_API bool opcache_preloading(void);

END_EXTERN_C()

#endif /* ZEND_ACCELERATOR_API_H */
