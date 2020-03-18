/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2015 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Tjerk Meesters <datibbaw@php.net>                           |
   +----------------------------------------------------------------------+
*/

typedef zend_bool (*php_traverse_each_t)(zval *value, zval *key, void *context);

#define PHP_TRAVERSE_MODE_VAL     2
#define PHP_TRAVERSE_MODE_KEY_VAL 3

#define PHP_TRAVERSE_CONTEXT_STANDARD_MEMBERS() \
	zval *traversable; \
	zend_fcall_info fci; \
	zend_fcall_info_cache fci_cache;

PHPAPI void php_traverse(zval *t, php_traverse_each_t each_func, int traverse_mode, void *context);
