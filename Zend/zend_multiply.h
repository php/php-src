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

#define HALF   (4*sizeof(long))
#define UH(b)  ((b) >> HALF)
#define LH(a)  ((a) & ~(-1L << HALF))

#define ZEND_SIGNED_MULTIPLY_LONG(a, b, lval, dval, usedval) do {		\
	long __mid,__upr,__res = (a) * (b);									\
	if (-(long)(a) > 0 && -(long)(b) > 0) { /* overflow intended */		\
		__mid = UH(-(a))*LH(-(b)) + LH(-(a))*UH(-(b));					\
		__upr = UH(-(a))*UH(-(b)) + UH(__mid);							\
	} else {															\
		__mid = UH(a)*LH(b) + LH(a)*UH(b);								\
		__upr = UH(a)*UH(b) + UH(__mid);								\
	}																	\
	if (((usedval) = !((__upr==-1&&__res<0)||(!__upr&&__res>=0)))) {	\
		(dval) = (double)(a)*(double)(b);								\
	} else {															\
		(lval) = __res;													\
	}																	\
} while (0)
