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
   | Authors: Gina Peter Banyard <girgias@php.net>                        |
   +----------------------------------------------------------------------+
*/

#ifndef _ZEND_AUTOLOAD_H
#define _ZEND_AUTOLOAD_H

#include "zend_string.h"
#include "zend_hash.h"
#include "zend_API.h"
#include "zend.h"

ZEND_API zend_class_entry *zend_perform_class_autoload(zend_string *class_name, zend_string *lc_name);
ZEND_API void zend_autoload_register_class_loader(zend_fcall_info_cache *fcc, bool prepend);
ZEND_API bool zend_autoload_unregister_class_loader(const zend_fcall_info_cache *fcc);
ZEND_API void zend_autoload_fcc_map_to_callable_zval_map(zval *return_value);
/* Only for deprecated strange behaviour of spl_autoload_unregister() */
ZEND_API void zend_autoload_clean_class_loaders(void);
void zend_autoload_shutdown(void);

#endif
