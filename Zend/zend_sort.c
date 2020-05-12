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

#include "zend.h"
#include "zend_sort.h"
#include <limits.h>

#define QSORT_STACK_SIZE (sizeof(size_t) * CHAR_BIT)

ZEND_API void zend_qsort(void *base, size_t nmemb, size_t siz, compare_func_t compare, swap_func_t swp) /* {{{ */
{
	void           *begin_stack[QSORT_STACK_SIZE];
	void           *end_stack[QSORT_STACK_SIZE];
	register char  *begin;
	register char  *end;
	register char  *seg1;
	register char  *seg2;
	register char  *seg2p;
	register int    loop;
	size_t          offset;

	begin_stack[0] = (char *) base;
	end_stack[0]   = (char *) base + ((nmemb - 1) * siz);

	for (loop = 0; loop >= 0; --loop) {
		begin = begin_stack[loop];
		end   = end_stack[loop];

		while (begin < end) {
			offset = (end - begin) >> Z_L(1);
			swp(begin, begin + (offset - (offset % siz)));

			seg1 = begin + siz;
			seg2 = end;

			while (1) {
				for (; seg1 < seg2 && compare(begin, seg1) > 0;
				     seg1 += siz);

				for (; seg2 >= seg1 && compare(seg2, begin) > 0;
				     seg2 -= siz);

				if (seg1 >= seg2)
					break;

				swp(seg1, seg2);

				seg1 += siz;
				seg2 -= siz;
			}

			swp(begin, seg2);

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
/* }}} */

static inline void zend_sort_2(void *a, void *b, compare_func_t cmp, swap_func_t swp) /* {{{ */ {
	if (cmp(a, b) > 0) {
		swp(a, b);
	}
}
/* }}} */

static inline void zend_sort_3(void *a, void *b, void *c, compare_func_t cmp, swap_func_t swp) /* {{{ */ {
	if (!(cmp(a, b) > 0)) {
		if (!(cmp(b, c) > 0)) {
			return;
		}
		swp(b, c);
		if (cmp(a, b) > 0) {
			swp(a, b);
		}
		return;
	}
	if (!(cmp(c, b) > 0)) {
		swp(a, c);
		return;
	}
	swp(a, b);
	if (cmp(b, c) > 0) {
		swp(b, c);
	}
}
/* }}} */

static void zend_sort_4(void *a, void *b, void *c, void *d, compare_func_t cmp, swap_func_t swp) /* {{{ */ {
	zend_sort_3(a, b, c, cmp, swp);
	if (cmp(c, d) > 0) {
		swp(c, d);
		if (cmp(b, c) > 0) {
			swp(b, c);
			if (cmp(a, b) > 0) {
				swp(a, b);
			}
		}
	}
}
/* }}} */

static void zend_sort_5(void *a, void *b, void *c, void *d, void *e, compare_func_t cmp, swap_func_t swp) /* {{{ */ {
	zend_sort_4(a, b, c, d, cmp, swp);
	if (cmp(d, e) > 0) {
		swp(d, e);
		if (cmp(c, d) > 0) {
			swp(c, d);
			if (cmp(b, c) > 0) {
				swp(b, c);
				if (cmp(a, b) > 0) {
					swp(a, b);
				}
			}
		}
	}
}
/* }}} */

ZEND_API void zend_insert_sort(void *base, size_t nmemb, size_t siz, compare_func_t cmp, swap_func_t swp) /* {{{ */{
	switch (nmemb) {
		case 0:
		case 1:
			break;
		case 2:
			zend_sort_2(base, (char *)base + siz, cmp, swp);
			break;
		case 3:
			zend_sort_3(base, (char *)base + siz, (char *)base + siz + siz, cmp, swp);
			break;
		case 4:
			{
				size_t siz2 = siz + siz;
				zend_sort_4(base, (char *)base + siz, (char *)base + siz2, (char *)base + siz + siz2, cmp, swp);
			}
			break;
		case 5:
			{
				size_t siz2 = siz + siz;
				zend_sort_5(base, (char *)base + siz, (char *)base + siz2, (char *)base + siz + siz2, (char *)base + siz2 + siz2, cmp, swp);
			}
			break;
		default:
			{
				char *i, *j, *k;
				char *start = (char *)base;
				char *end = start + (nmemb * siz);
				size_t siz2= siz + siz;
				char *sentry = start + (6 * siz);
				for (i = start + siz; i < sentry; i += siz) {
					j = i - siz;
					if (!(cmp(j, i) > 0)) {
						continue;
					}
					while (j != start) {
						j -= siz;
						if (!(cmp(j, i) > 0)) {
							j += siz;
							break;
						}
					}
					for (k = i; k > j; k -= siz) {
						swp(k, k - siz);
					}
				}
				for (i = sentry; i < end; i += siz) {
					j = i - siz;
					if (!(cmp(j, i) > 0)) {
						continue;
					}
					do {
						j -= siz2;
						if (!(cmp(j, i) > 0)) {
							j += siz;
							if (!(cmp(j, i) > 0)) {
								j += siz;
							}
							break;
						}
						if (j == start) {
							break;
						}
						if (j == start + siz) {
							j -= siz;
							if (cmp(i, j) > 0) {
								j += siz;
							}
							break;
						}
					} while (1);
					for (k = i; k > j; k -= siz) {
						swp(k, k - siz);
					}
				}
			}
			break;
	}
}
/* }}} */

/* {{{ ZEND_API void zend_sort(void *base, size_t nmemb, size_t siz, compare_func_t cmp, swap_func_t swp)
 *
 * Derived from LLVM's libc++ implementation of std::sort.
 *
 * ===========================================================================
 * libc++ License
 * ===========================================================================
 *
 * The libc++ library is dual licensed under both the University of Illinois
 * "BSD-Like" license and the MIT license. As a user of this code you may
 * choose to use it under either license. As a contributor, you agree to allow
 * your code to be used under both.
 *
 * Full text of the relevant licenses is included below.
 *
 * ===========================================================================
 *
 * University of Illinois/NCSA
 * Open Source License
 *
 * Copyright (c) 2009-2012 by the contributors listed at
 * http://llvm.org/svn/llvm-project/libcxx/trunk/CREDITS.TXT
 *
 * All rights reserved.
 *
 * Developed by:
 *
 *     LLVM Team
 *
 *     University of Illinois at Urbana-Champaign
 *
 *     http://llvm.org
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal with the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 *     * Redistributions of source code must retain the above copyright notice,
 *       this list of conditions and the following disclaimers.
 *
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimers in the
 *       documentation and/or other materials provided with the distribution.
 *
 *     * Neither the names of the LLVM Team, University of Illinois at
 *       Urbana-Champaign, nor the names of its contributors may be used to
 *       endorse or promote products derived from this Software without
 *       specific prior written permission.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * WITH THE SOFTWARE.
 *
 * ===========================================================================
 *
 * Copyright (c) 2009-2012 by the contributors listed at
 * http://llvm.org/svn/llvm-project/libcxx/trunk/CREDITS.TXT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */
ZEND_API void zend_sort_unstable(void *base, size_t nmemb, size_t siz, compare_func_t cmp, swap_func_t swp)
{
	while (1) {
		if (nmemb <= 16) {
			zend_insert_sort(base, nmemb, siz, cmp, swp);
			return;
		} else {
			char *i, *j;
			char *start = (char *)base;
			char *end = start + (nmemb * siz);
			size_t offset = (nmemb >> Z_L(1));
			char *pivot = start + (offset * siz);

			if ((nmemb >> Z_L(10))) {
				size_t delta = (offset >> Z_L(1)) * siz;
				zend_sort_5(start, start + delta, pivot, pivot + delta, end - siz, cmp, swp);
			} else {
				zend_sort_3(start, pivot, end - siz, cmp, swp);
			}
			swp(start + siz, pivot);
			pivot = start + siz;
			i = pivot + siz;
			j = end - siz;
			while (1) {
				while (cmp(pivot, i) > 0) {
					i += siz;
					if (UNEXPECTED(i == j)) {
						goto done;
					}
				}
				j -= siz;
				if (UNEXPECTED(j == i)) {
					goto done;
				}
				while (cmp(j, pivot) > 0) {
					j -= siz;
					if (UNEXPECTED(j == i)) {
						goto done;
					}
				}
				swp(i, j);
				i += siz;
				if (UNEXPECTED(i == j)) {
					goto done;
				}
			}
done:
			swp(pivot, i - siz);
			if ((i - siz) - start < end - i) {
				zend_sort(start, (i - start)/siz - 1, siz, cmp, swp);
				base = i;
				nmemb = (end - i)/siz;
			} else {
				zend_sort(i, (end - i)/siz, siz, cmp, swp);
				nmemb = (i - start)/siz - 1;
			}
		}
	}
}
/* }}} */

/**
 * A stable, adaptive, iterative mergesort that requires far fewer than
 * n lg(n) comparisons when running on partially sorted arrays, while
 * offering performance comparable to a traditional mergesort when run
 * on random arrays.  Like all proper mergesorts, this sort is stable and
 * runs O(n log n) time (worst case).  In the worst case, this sort requires
 * temporary storage space for n/2 object references; in the best case,
 * it requires only a small constant amount of space.
 *
 * This implementation was adapted from Josh Bloch's Java implementation of
 * Tim Peters's list sort for Python, which is described in detail here:
 *
 *   http://svn.python.org/projects/python/trunk/Objects/listsort.txt
 *
 * Tim's C code may be found here:
 *
 *   http://svn.python.org/projects/python/trunk/Objects/listobject.c
 *
 * Josh's (Apache 2.0 Licenced) Java code may be found here:
 *
 *   http://gee.cs.oswego.edu/cgi-bin/viewcvs.cgi/jsr166/src/main/java/util/TimSort.java?view=co
 *
 * The underlying techniques are described in this paper (and may have
 * even earlier origins):
 *
 *  "Optimistic Sorting and Information Theoretic Complexity"
 *  Peter McIlroy
 *  SODA (Fourth Annual ACM-SIAM Symposium on Discrete Algorithms),
 *  pp 467-474, Austin, Texas, 25-27 January 1993.
 *
 * While the API to this class consists solely of static methods, it is
 * (privately) instantiable; a TimSort instance holds the state of an ongoing
 * sort, assuming the input array is large enough to warrant the full-blown
 * TimSort. Small arrays are sorted in place, using a binary insertion sort.
 *
 * @author Josh Bloch
 * @author Patrick O. Perry
 */
int zend_timsort(
	void *base, size_t nel, size_t width, int (*compar) (const void *, const void *));

int zend_timsort_r(
	void *base, size_t nel, size_t width, int (*compar) (const void *, const void *, void *),
	void *context);

ZEND_API void zend_sort(void *base, size_t nmemb, size_t siz, compare_func_t cmp, swap_func_t swp)
{
	zend_timsort(base, nmemb, siz, cmp);
}
