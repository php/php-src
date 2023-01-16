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
   | Authors: Xinchen Hui <laruence@php.net>                              |
   |          Sterling Hughes <sterling@php.net>                          |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_SORT_H
#define ZEND_SORT_H

BEGIN_EXTERN_C()
ZEND_API void zend_sort(void *base, size_t nmemb, size_t siz, compare_func_t cmp, swap_func_t swp);
ZEND_API void zend_insert_sort(void *base, size_t nmemb, size_t siz, compare_func_t cmp, swap_func_t swp);
END_EXTERN_C()

#endif       /* ZEND_SORT_H */
