/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2013 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        | 
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Sterling Hughes <sterling@php.net>                          |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef ZEND_QSORT_H
#define ZEND_QSORT_H

BEGIN_EXTERN_C()
typedef int  (*compare_r_func_t)(const void *, const void * TSRMLS_DC, void *);
ZEND_API void zend_qsort(void *base, size_t nmemb, size_t siz, compare_func_t compare TSRMLS_DC);
ZEND_API void zend_qsort_r(void *base, size_t nmemb, size_t siz, compare_r_func_t compare, void *arg TSRMLS_DC);
END_EXTERN_C()

#endif       /* ZEND_QSORT_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
