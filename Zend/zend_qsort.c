/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2001 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        | 
   | available at through the world-wide-web at                           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Sterling Hughes <sterling@php.net>                          |
   +----------------------------------------------------------------------+
*/

#include "zend.h"

#include <limits.h>

#define QSORT_STACK_SIZE (sizeof(size_t) * CHAR_BIT)

static void _zend_qsort_swap(void *a, void *b, size_t siz)
{
	register size_t i;
	register int    t_i;
	register char   t_c;
	
	for (i = sizeof(int); i <= siz; i += sizeof(int)) {
		t_i = *(int *) a;
		*((int *) a)++ = *(int *) b;
		*((int *) b)++ = t_i;
	}

	for (i = i - sizeof(int) + 1; i <= siz; ++i) {
		t_c = *(char *) a;
		*((char *) a)++ = *(char *) b;
		*((char *) b)++ = t_c;
	}
}

ZEND_API void zend_qsort(void *base, size_t nmemb, size_t siz, compare_func_t compare TSRMLS_DC)
{
	void           *begin_stack[QSORT_STACK_SIZE];
	void           *end_stack[QSORT_STACK_SIZE];
	register char  *begin;
	register char  *end;
	register char  *seg1;
	register char  *seg2;
	register char  *seg2p;
	register int    loop;
	uint            offset;

	begin_stack[0] = (char *) base;
	end_stack[0]   = (char *) base + ((nmemb - 1) * siz);

	for (loop = 0; loop >= 0; --loop) {
		begin = begin_stack[loop];
		end   = end_stack[loop];

		while (begin < end) {
			offset = (end - begin) >> 1;
			_zend_qsort_swap(begin, begin + (offset - (offset % siz)), siz);

			seg1 = begin + siz;
			seg2 = end;

			while (1) {
				for (; seg1 < seg2 && compare(begin, seg1 TSRMLS_CC) > 0;
				     seg1 += siz);

				for (; seg2 >= seg1 && compare(seg2, begin TSRMLS_CC) > 0;
				     seg2 -= siz);
				
				if (seg1 >= seg2)
					break;
				
				_zend_qsort_swap(seg1, seg2, siz);

				seg1 += siz;
				seg2 -= siz;
			}

			_zend_qsort_swap(begin, seg2, siz);

			seg2p = seg2;
			
			if ((seg2p - begin) <= (end - seg2p)) {
				if ((seg2p + siz) < end) {
					begin_stack[loop] = seg2p + siz;
					end_stack[loop++] = end;
				}
				end = seg2p - siz;
			}
			else {
				if ((seg2p - siz) > begin) {
					begin_stack[loop] = begin;
					end_stack[loop++] = seg2p - siz;
				}
				begin = seg2p + siz;
			}
		}
	}
}
