/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) Zend Technologies Ltd. (http://www.zend.com)           |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Gina Peter Banyard <girgias@php.net>                        |
   +----------------------------------------------------------------------+
*/

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
