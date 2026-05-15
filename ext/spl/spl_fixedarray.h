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
  | Author: Antony Dovgal <tony@daylessday.org>                          |
  |         Etienne Kneuss <colder@php.net>                              |
  +----------------------------------------------------------------------+
*/

#ifndef SPL_FIXEDARRAY_H
#define SPL_FIXEDARRAY_H

#include "php.h"

extern PHPAPI zend_class_entry *spl_ce_SplFixedArray;

#ifndef ZEND_OPCACHE_STATIC_CACHE_SAFE_DIRECT_HANDLERS_FWD
# define ZEND_OPCACHE_STATIC_CACHE_SAFE_DIRECT_HANDLERS_FWD
typedef struct _zend_opcache_static_cache_safe_direct_handlers zend_opcache_static_cache_safe_direct_handlers;
#endif
const zend_opcache_static_cache_safe_direct_handlers *spl_fixedarray_object_get_direct_cache_handlers(void);

PHP_MINIT_FUNCTION(spl_fixedarray);

#endif	/* SPL_FIXEDARRAY_H */
