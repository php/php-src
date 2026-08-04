/*
   +----------------------------------------------------------------------+
   | Zend OPcache                                                         |
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
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
