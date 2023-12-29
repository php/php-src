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
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <zeev@php.net>                                 |
   |          Rasmus Lerdorf <rasmus@php.net>                             |
   |          Andrei Zmievski <andrei@php.net>                            |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_ARRAY_H
#define PHP_ARRAY_H

# include "ext/random/php_random.h"

PHP_MINIT_FUNCTION(array);
PHP_MSHUTDOWN_FUNCTION(array);

PHPAPI int php_array_merge(HashTable *dest, HashTable *src);
PHPAPI int php_array_merge_recursive(HashTable *dest, HashTable *src);
PHPAPI int php_array_replace_recursive(HashTable *dest, HashTable *src);
PHPAPI int php_multisort_compare(const void *a, const void *b);
PHPAPI zend_long php_count_recursive(HashTable *ht);

PHPAPI bool php_array_data_shuffle(const php_random_algo *algo, void *status, zval *array);
PHPAPI bool php_array_pick_keys(const php_random_algo *algo, void *status, zval *input, zend_long num_req, zval *retval, bool silent);

#define PHP_EXTR_OVERWRITE			0
#define PHP_EXTR_SKIP				1
#define PHP_EXTR_PREFIX_SAME		2
#define PHP_EXTR_PREFIX_ALL			3
#define PHP_EXTR_PREFIX_INVALID		4
#define PHP_EXTR_PREFIX_IF_EXISTS	5
#define PHP_EXTR_IF_EXISTS			6

#define PHP_EXTR_REFS				0x100

#define PHP_CASE_LOWER				0
#define PHP_CASE_UPPER				1

#define PHP_SORT_REGULAR            0
#define PHP_SORT_NUMERIC            1
#define PHP_SORT_STRING             2
#define PHP_SORT_DESC               3
#define PHP_SORT_ASC                4
#define PHP_SORT_LOCALE_STRING      5
#define PHP_SORT_NATURAL            6
#define PHP_SORT_FLAG_CASE          8

#define PHP_COUNT_NORMAL      0
#define PHP_COUNT_RECURSIVE   1

#define ARRAY_FILTER_USE_BOTH	1
#define ARRAY_FILTER_USE_KEY	2

ZEND_BEGIN_MODULE_GLOBALS(array)
	bucket_compare_func_t *multisort_func;
	bool compare_deprecation_thrown;
ZEND_END_MODULE_GLOBALS(array)

#define ARRAYG(v) ZEND_MODULE_GLOBALS_ACCESSOR(array, v)

#endif /* PHP_ARRAY_H */
