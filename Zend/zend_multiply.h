/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2004 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Author: Ard Biesheuvel <ard@ard.nu>                                  |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#define ZEND_SIGNED_MULTIPLY_LONG(a, b, lval, dval, usedval) do {		\
	long   __lres  = (a) * (b);											\
	double __dres  = (double)(a) * (double)(b);							\
	double __delta = (double) __lres - __dres;							\
	if ( ((usedval) = (( __dres + __delta ) != __dres))) {				\
		(dval) = __dres;												\
	} else {															\
		(lval) = __lres;												\
	}																	\
} while (0)
