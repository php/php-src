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
   | Authors: George Peter Banyard <girgias@php.net>                      |
   +----------------------------------------------------------------------+
*/

#include "zend.h"
#include "zend_API.h"
#include "zend_hash.h"

ZEND_FUNCTION(autoload_register_class);
ZEND_FUNCTION(autoload_unregister_class);
ZEND_FUNCTION(autoload_call_class);
ZEND_FUNCTION(autoload_list_class);
ZEND_FUNCTION(autoload_register_function);
ZEND_FUNCTION(autoload_unregister_function);
ZEND_FUNCTION(autoload_call_function);
ZEND_FUNCTION(autoload_list_function);

ZEND_API zend_class_entry *zend_perform_class_autoload(zend_string *class_name, zend_string *lc_name);
ZEND_API zend_function *zend_perform_function_autoload(zend_string *function_name, zend_string *lc_name);
ZEND_API void zend_autoload_callback_zval_destroy(zval *entry);
ZEND_API void zend_register_class_autoloader(zend_fcall_info *fci, zend_fcall_info_cache *fcc, bool prepend);
void zend_autoload_shutdown(void);
