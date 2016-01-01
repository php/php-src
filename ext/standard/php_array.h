/* 
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2016 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   |          Rasmus Lerdorf <rasmus@php.net>                             |
   |          Andrei Zmievski <andrei@php.net>                            |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_ARRAY_H
#define PHP_ARRAY_H

PHP_MINIT_FUNCTION(array);
PHP_MSHUTDOWN_FUNCTION(array);

PHP_FUNCTION(ksort);
PHP_FUNCTION(krsort);
PHP_FUNCTION(natsort);
PHP_FUNCTION(natcasesort);
PHP_FUNCTION(asort);
PHP_FUNCTION(arsort);
PHP_FUNCTION(sort);
PHP_FUNCTION(rsort);
PHP_FUNCTION(usort);
PHP_FUNCTION(uasort);
PHP_FUNCTION(uksort);
PHP_FUNCTION(array_walk);
PHP_FUNCTION(array_walk_recursive);
PHP_FUNCTION(count);
PHP_FUNCTION(end);
PHP_FUNCTION(prev);
PHP_FUNCTION(next);
PHP_FUNCTION(reset);
PHP_FUNCTION(current);
PHP_FUNCTION(key);
PHP_FUNCTION(min);
PHP_FUNCTION(max);
PHP_FUNCTION(in_array);
PHP_FUNCTION(array_search);
PHP_FUNCTION(extract);
PHP_FUNCTION(compact);
PHP_FUNCTION(array_fill);
PHP_FUNCTION(array_fill_keys);
PHP_FUNCTION(range);
PHP_FUNCTION(shuffle);
PHP_FUNCTION(array_multisort);
PHP_FUNCTION(array_push);
PHP_FUNCTION(array_pop);
PHP_FUNCTION(array_shift);
PHP_FUNCTION(array_unshift);
PHP_FUNCTION(array_splice);
PHP_FUNCTION(array_slice);
PHP_FUNCTION(array_merge);
PHP_FUNCTION(array_merge_recursive);
PHP_FUNCTION(array_replace);
PHP_FUNCTION(array_replace_recursive);
PHP_FUNCTION(array_keys);
PHP_FUNCTION(array_values);
PHP_FUNCTION(array_count_values);
PHP_FUNCTION(array_column);
PHP_FUNCTION(array_reverse);
PHP_FUNCTION(array_reduce);
PHP_FUNCTION(array_pad);
PHP_FUNCTION(array_flip);
PHP_FUNCTION(array_change_key_case);
PHP_FUNCTION(array_rand);
PHP_FUNCTION(array_unique);
PHP_FUNCTION(array_intersect);
PHP_FUNCTION(array_intersect_key);
PHP_FUNCTION(array_intersect_ukey);
PHP_FUNCTION(array_uintersect);
PHP_FUNCTION(array_intersect_assoc);
PHP_FUNCTION(array_uintersect_assoc);
PHP_FUNCTION(array_intersect_uassoc);
PHP_FUNCTION(array_uintersect_uassoc);
PHP_FUNCTION(array_diff);
PHP_FUNCTION(array_diff_key);
PHP_FUNCTION(array_diff_ukey);
PHP_FUNCTION(array_udiff);
PHP_FUNCTION(array_diff_assoc);
PHP_FUNCTION(array_udiff_assoc);
PHP_FUNCTION(array_diff_uassoc);
PHP_FUNCTION(array_udiff_uassoc);
PHP_FUNCTION(array_sum);
PHP_FUNCTION(array_product);
PHP_FUNCTION(array_filter);
PHP_FUNCTION(array_map);
PHP_FUNCTION(array_key_exists);
PHP_FUNCTION(array_chunk);
PHP_FUNCTION(array_combine);

PHPAPI void php_splice(HashTable *ht, zend_uint offset, zend_uint length, zval ***list, zend_uint list_count, HashTable *removed TSRMLS_DC);
PHPAPI int php_array_merge(HashTable *dest, HashTable *src, int recursive TSRMLS_DC);
PHPAPI int php_array_replace_recursive(HashTable *dest, HashTable *src TSRMLS_DC);
PHPAPI int php_multisort_compare(const void *a, const void *b TSRMLS_DC);
PHPAPI int php_count_recursive(zval *array, long mode TSRMLS_DC);

#define PHP_SORT_REGULAR            0
#define PHP_SORT_NUMERIC            1
#define PHP_SORT_STRING             2
#define PHP_SORT_DESC               3
#define PHP_SORT_ASC                4
#define PHP_SORT_LOCALE_STRING      5
#define PHP_SORT_NATURAL            6
#define PHP_SORT_FLAG_CASE          8

#define COUNT_NORMAL      0
#define COUNT_RECURSIVE   1

#define ARRAY_FILTER_USE_BOTH	1
#define ARRAY_FILTER_USE_KEY	2

ZEND_BEGIN_MODULE_GLOBALS(array) 
	int *multisort_flags[2];
	int (*compare_func)(zval *result, zval *op1, zval *op2 TSRMLS_DC);
ZEND_END_MODULE_GLOBALS(array) 

#ifdef ZTS
#define ARRAYG(v) TSRMG(array_globals_id, zend_array_globals *, v)
#else
#define ARRAYG(v) (array_globals.v)
#endif

#endif /* PHP_ARRAY_H */
