/*
 * Copyright (C) 2011 Patrick O. Perry
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <assert.h>		// assert
#include <errno.h>		// EINVAL
#if defined(_MSC_VER)
# include <malloc.h>		// _alloca
#endif
#include <stddef.h>		// size_t, NULL
#include <stdlib.h>		// malloc, free
#include <string.h>		// memcpy, memmove

/**
 * This is the minimum sized sequence that will be merged.  Shorter
 * sequences will be lengthened by calling binarySort.  If the entire
 * array is less than this length, no merges will be performed.
 *
 * This constant should be a power of two.  It was 64 in Tim Peter's C
 * implementation, but 32 was empirically determined to work better in
 * [Android's Java] implementation.  In the unlikely event that you set
 * this constant to be a number that's not a power of two, you'll need
 * to change the {@link #minRunLength} computation.
 *
 * If you decrease this constant, you must change the stackLen
 * computation in the TimSort constructor, or you risk an
 * ArrayOutOfBounds exception.  See listsort.txt for a discussion
 * of the minimum stack length required as a function of the length
 * of the array being sorted and the minimum merge sequence length.
 */
#define MIN_MERGE 32

/**
 * When we get into galloping mode, we stay there until both runs win less
 * often than MIN_GALLOP consecutive times.
 */
#define MIN_GALLOP 7

/**
 * Maximum initial size of tmp array, which is used for merging.  The array
 * can grow to accommodate demand.
 *
 * Unlike Tim's original C version, we do not allocate this much storage
 * when sorting smaller arrays.  This change was required for performance.
 */
#define INITIAL_TMP_STORAGE_LENGTH 256

/**
 * Maximum stack size.  This depends on MIN_MERGE and sizeof(size_t).
 */
#define MAX_STACK 85

/**
 * Define MALLOC_STACK if you want to allocate the run stack on the heap.
 * Otherwise, 2* MAX_STACK * sizeof(size_t) ~ 1.3K gets reserved on the
 * call stack.
 */
/* #undef MALLOC_STACK */

#if defined(_MSC_VER)
# define DEFINE_TEMP(temp) void *temp = _alloca(WIDTH)
#else
# define DEFINE_TEMP(temp) char temp[WIDTH]
#endif

#define ASSIGN(x, y) memcpy(x, y, WIDTH)
#define INCPTR(x) ((void *)((char *)(x) + WIDTH))
#define DECPTR(x) ((void *)((char *)(x) - WIDTH))
#define ELEM(a,i) ((char *)(a) + (i) * WIDTH)
#define LEN(n) ((n) * WIDTH)

#define MIN(a,b) ((a) <= (b) ? (a) : (b))
#define SUCCESS 0
#define FAILURE (-1)

#define CONCAT(x, y) x ## _ ## y
#define MAKE_STR(x, y) CONCAT(x,y)
#define NAME(x) MAKE_STR(x, WIDTH)
#define CALL(x) NAME(x)


#ifdef IS_TIMSORT_R
/*
 * Note order of elements to comparator matches that of C11 qsort_s,
 * not BSD qsort_r or Windows qsort_s
 */
typedef int (*comparator) (const void *x, const void *y, void *thunk);
#define CMPPARAMS(compar, thunk) comparator compar, void *thunk
#define CMPARGS(compar, thunk) (compar), (thunk)
#define CMP(compar, thunk, x, y) (compar((x), (y), (thunk)))
#define TIMSORT zend_timsort_r

#else

typedef int (*comparator) (const void *x, const void *y);
#define CMPPARAMS(compar, thunk) comparator compar
#define CMPARGS(compar, thunk) (compar)
#define CMP(compar, thunk, x, y) (compar((x), (y)))
#define TIMSORT zend_timsort

#endif /* IS_TIMSORT_R */

struct timsort_run {
	void *base;
	size_t len;
};

struct timsort {
	/**
	 * The array being sorted.
	 */
	void *a;
	size_t a_length;

	/**
	 * The comparator for this sort.
	 */
	comparator c;
#ifdef IS_TIMSORT_R
	void *carg;
#endif

	/**
	 * This controls when we get *into* galloping mode.  It is initialized
	 * to MIN_GALLOP.  The mergeLo and mergeHi methods nudge it higher for
	 * random data, and lower for highly structured data.
	 */
	size_t minGallop;

	/**
	 * Temp storage for merges.
	 */
	void *tmp;
	size_t tmp_length;

	/**
	 * A stack of pending runs yet to be merged.  Run i starts at
	 * address base[i] and extends for len[i] elements.  It's always
	 * true (so long as the indices are in bounds) that:
	 *
	 *     runBase[i] + runLen[i] == runBase[i + 1]
	 *
	 * so we could cut the storage for this, but it's a minor amount,
	 * and keeping all the info explicit simplifies the code.
	 */
	size_t stackSize;	// Number of pending runs on stack
	size_t stackLen;	// maximum stack size
#ifdef MALLOC_STACK
	struct timsort_run *run;
#else
	struct timsort_run run[MAX_STACK];
#endif
};

static int timsort_init(struct timsort *ts, void *a, size_t len,
			CMPPARAMS(c, carg),
			size_t width);
static void timsort_deinit(struct timsort *ts);
static size_t minRunLength(size_t n);
static void pushRun(struct timsort *ts, void *runBase, size_t runLen);
static void *ensureCapacity(struct timsort *ts, size_t minCapacity,
			    size_t width);

/**
 * Creates a TimSort instance to maintain the state of an ongoing sort.
 *
 * @param a the array to be sorted
 * @param nel the length of the array
 * @param c the comparator to determine the order of the sort
 * @param width the element width
 */
static int timsort_init(struct timsort *ts, void *a, size_t len,
			CMPPARAMS(c, carg),
			size_t width)
{
	int err = 0;

	assert(ts);
	assert(a || !len);
	assert(c);
	assert(width);

	ts->minGallop = MIN_GALLOP;
	ts->stackSize = 0;

	ts->a = a;
	ts->a_length = len;
	ts->c = c;
#ifdef IS_TIMSORT_R
	ts->carg = carg;
#endif

	// Allocate temp storage (which may be increased later if necessary)
	ts->tmp_length = (len < 2 * INITIAL_TMP_STORAGE_LENGTH ?
			  len >> 1 : INITIAL_TMP_STORAGE_LENGTH);
	if (ts->tmp_length) {
		ts->tmp = malloc(ts->tmp_length * width);
		err |= ts->tmp == NULL;
	} else {
		ts->tmp = NULL;
	}

	/*
	 * Allocate runs-to-be-merged stack (which cannot be expanded).  The
	 * stack length requirements are described in listsort.txt.  The C
	 * version always uses the same stack length (85), but this was
	 * measured to be too expensive when sorting "mid-sized" arrays (e.g.,
	 * 100 elements) in Java.  Therefore, we use smaller (but sufficiently
	 * large) stack lengths for smaller arrays.  The "magic numbers" in the
	 * computation below must be changed if MIN_MERGE is decreased.  See
	 * the MIN_MERGE declaration above for more information.
	 */

	/* POP:
	 * In listsort.txt, Tim argues that the run lengths form a decreasing
	 * sequence, and each run length is greater than the previous two.
	 * Thus, lower bounds on the minimum runLen numbers on the stack are:
	 *
	 *   [      1           = b[1]
	 *   ,      minRun      = b[2]
	 *   ,  1 * minRun +  2 = b[3]
	 *   ,  2 * minRun +  3 = b[4]
	 *   ,  3 * minRun +  6 = b[5]
	 *   , ...
	 *   ],
	 *
	 * Moreover, minRun >= MIN_MERGE / 2.  Also, note that the sum of the
	 * run lenghts is less than or equal to the length of the array.
	 *
	 * Let s be the stack length and n be the array length.  If s >= 2, then n >= b[1] + b[2].
	 * More generally, if s >= m, then n >= b[1] + b[2] + ... + b[m] = B[m].  Conversely, if
	 * n < B[m], then s < m.
	 *
	 * In Haskell, we can compute the bin sizes using the fibonacci numbers
	 *
	 *     fibs = 1:1:(zipWith (+) fibs (tail fibs))
	 *
	 *     cumSums a = case a of { [] -> [] ; (x:xs) -> x:(map (x+) (cumSums xs)) }
	 *
	 *     fibSums = cumSums fibs
	 *
	 *     binSizes minRun = ([ 1, minRun, minRun + 2 ]
	 *                        ++ [ (1 + minRun) * (fibs !! (i+2))
	 *                             + fibSums !! (i+1) - fibs !! i | i <- [0..] ])
	 *
	 *     arraySizes minRun = cumSums (binSizes minRun)
	 *
	 * We these funcitons, we can compute a table with minRun = MIN_MERGE / 2 = 16:
	 *
	 *     m          B[m]
	 *   ---------------------------
	 *      1                    17
	 *      2                    35
	 *      3                    70
	 *      4                   124
	 *      5                   214
	 *      6                   359
	 *     11                  4220
	 *     17                 76210 # > 2^16 - 1
	 *     40            4885703256 # > 2^32 - 1
	 *     86  20061275507500957239 # > 2^64 - 1
	 *
	 * If len < B[m], then stackLen < m:
	 */
#ifdef MALLOC_STACK
	ts->stackLen = (len < 359 ? 5
			: len < 4220 ? 10
			: len < 76210 ? 16 : len < 4885703256ULL ? 39 : 85);

	/* Note that this is slightly more liberal than in the Java
	 * implementation.  The discrepancy might be because the Java
	 * implementation uses a less accurate lower bound.
	 */
	//stackLen = (len < 120 ? 5 : len < 1542 ? 10 : len < 119151 ? 19 : 40);

	ts->run = malloc(ts->stackLen * sizeof(ts->run[0]));
	err |= ts->run == NULL;
#else
	ts->stackLen = MAX_STACK;
#endif

	if (!err) {
		return SUCCESS;
	} else {
		timsort_deinit(ts);
		return FAILURE;
	}
}

static void timsort_deinit(struct timsort *ts)
{
	free(ts->tmp);
#ifdef MALLOC_STACK
	free(ts->run);
#endif
}

/**
 * Returns the minimum acceptable run length for an array of the specified
 * length. Natural runs shorter than this will be extended with
 * {@link #binarySort}.
 *
 * Roughly speaking, the computation is:
 *
 *  If n < MIN_MERGE, return n (it's too small to bother with fancy stuff).
 *  Else if n is an exact power of 2, return MIN_MERGE/2.
 *  Else return an int k, MIN_MERGE/2 <= k <= MIN_MERGE, such that n/k
 *   is close to, but strictly less than, an exact power of 2.
 *
 * For the rationale, see listsort.txt.
 *
 * @param n the length of the array to be sorted
 * @return the length of the minimum run to be merged
 */
static size_t minRunLength(size_t n)
{
	size_t r = 0;		// Becomes 1 if any 1 bits are shifted off
	while (n >= MIN_MERGE) {
		r |= (n & 1);
		n >>= 1;
	}
	return n + r;
}

/**
 * Pushes the specified run onto the pending-run stack.
 *
 * @param runBase index of the first element in the run
 * @param runLen  the number of elements in the run
 */
static void pushRun(struct timsort *ts, void *runBase, size_t runLen)
{
	assert(ts->stackSize < ts->stackLen);

	ts->run[ts->stackSize].base = runBase;
	ts->run[ts->stackSize].len = runLen;
	ts->stackSize++;
}

/**
 * Ensures that the external array tmp has at least the specified
 * number of elements, increasing its size if necessary.  The size
 * increases exponentially to ensure amortized linear time complexity.
 *
 * @param minCapacity the minimum required capacity of the tmp array
 * @return tmp, whether or not it grew
 */
static void *ensureCapacity(struct timsort *ts, size_t minCapacity,
			    size_t width)
{
	if (ts->tmp_length < minCapacity) {
		// Compute smallest power of 2 > minCapacity
		size_t newSize = minCapacity;
		newSize |= newSize >> 1;
		newSize |= newSize >> 2;
		newSize |= newSize >> 4;
		newSize |= newSize >> 8;
		newSize |= newSize >> 16;
		if (sizeof(newSize) > 4)
			newSize |= newSize >> 32;

		newSize++;
		newSize = MIN(newSize, ts->a_length >> 1);
		if (newSize == 0) {	// (overflow) Not bloody likely!
			newSize = minCapacity;
		}

		free(ts->tmp);
		ts->tmp_length = newSize;
		ts->tmp = malloc(ts->tmp_length * width);
	}

	return ts->tmp;
}

/*#define WIDTH 4
#include "zend_timsort_impl.h"
#undef WIDTH

#define WIDTH 8
#include "zend_timsort_impl.h"
#undef WIDTH*/

#define WIDTH 32 /* sizeof(Bucket) */
#include "zend_timsort_impl.h"
#undef WIDTH

#define WIDTH width
#include "zend_timsort_impl.h"
#undef WIDTH


int TIMSORT(void *a, size_t nel, size_t width, CMPPARAMS(c, carg))
{
	switch (width) {
	/*case 4:
		return timsort_4(a, nel, width, CMPARGS(c, carg));
	case 8:
		return timsort_8(a, nel, width, CMPARGS(c, carg));*/
	case 32:
		return timsort_32(a, nel, width, CMPARGS(c, carg));
	default:
		return timsort_width(a, nel, width, CMPARGS(c, carg));
	}
}
