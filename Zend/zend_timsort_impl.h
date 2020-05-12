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

static void NAME(binarySort) (void *a, size_t hi, size_t start,
			      CMPPARAMS(compare, carg), size_t width);
static size_t NAME(countRunAndMakeAscending) (void *a, size_t hi,
					      CMPPARAMS(compare, carg),
					      size_t width);
static void NAME(reverseRange) (void *a, size_t hi, size_t width);
static int NAME(mergeCollapse) (struct timsort * ts, size_t width);
static int NAME(mergeForceCollapse) (struct timsort * ts, size_t width);
static int NAME(mergeAt) (struct timsort * ts, size_t i, size_t width);
static size_t NAME(gallopLeft) (void *key, void *base, size_t len,
				size_t hint, CMPPARAMS(compare, carg),
				size_t width);
static size_t NAME(gallopRight) (void *key, void *base, size_t len,
				 size_t hint, CMPPARAMS(compare, carg),
				 size_t width);
static int NAME(mergeLo) (struct timsort * ts, void *base1, size_t len1,
			  void *base2, size_t len2, size_t width);
static int NAME(mergeHi) (struct timsort * ts, void *base1, size_t len1,
			  void *base2, size_t len2, size_t width);

static int NAME(timsort) (void *a, size_t nel, size_t width, CMPPARAMS(c, carg))
{
	int err = SUCCESS;
	struct timsort ts;
        size_t minRun;

	assert(a || !nel || !width);
	assert(c);

	if (nel < 2 || !width)
		return err;	// Arrays of size 0 and 1 are always sorted

	// If array is small, do a "mini-TimSort" with no merges
	if (nel < MIN_MERGE) {
		size_t initRunLen =
		    CALL(countRunAndMakeAscending) (a, nel, CMPARGS(c, carg), width);
		CALL(binarySort) (a, nel, initRunLen, CMPARGS(c, carg), width);
		return err;
	}

	/**
         * March over the array once, left to right, finding natural runs,
         * extending short natural runs to minRun elements, and merging runs
         * to maintain stack invariant.
         */
	if ((err = timsort_init(&ts, a, nel, CMPARGS(c, carg), width)))
		return err;

	minRun = minRunLength(nel);
	do {
		// Identify next run
		size_t runLen =
		    CALL(countRunAndMakeAscending) (a, nel, CMPARGS(c, carg), width);

		// If run is short, extend to min(minRun, nel)
		if (runLen < minRun) {
			size_t force = nel <= minRun ? nel : minRun;
			CALL(binarySort) (a, force, runLen, CMPARGS(c, carg), width);
			runLen = force;
		}
		// Push run onto pending-run stack, and maybe merge
		pushRun(&ts, a, runLen);
		if ((err = CALL(mergeCollapse) (&ts, width)))
			goto out;

		// Advance to find next run
		a = ELEM(a, runLen);
		nel -= runLen;
	} while (nel != 0);

	// Merge all remaining runs to complete sort
	if ((err = CALL(mergeForceCollapse) (&ts, width)))
		goto out;

	assert(ts.stackSize == 1);
out:
	timsort_deinit(&ts);
	return err;
}

/**
 * Sorts the specified portion of the specified array using a binary
 * insertion sort.  This is the best method for sorting small numbers
 * of elements.  It requires O(n log n) compares, but O(n^2) data
 * movement (worst case).
 *
 * If the initial part of the specified range is already sorted,
 * this method can take advantage of it: the method assumes that the
 * elements from index {@code lo}, inclusive, to {@code start},
 * exclusive are already sorted.
 *
 * @param a the array in which a range is to be sorted
 * @param hi the index after the last element in the range to be sorted
 * @param start the index of the first element in the range that is
 *        not already known to be sorted ({@code lo <= start <= hi})
 * @param c comparator to used for the sort
 */
static void NAME(binarySort) (void *a, size_t hi, size_t start,
			      CMPPARAMS(compare, carg), size_t width) {
	DEFINE_TEMP(pivot);
        char *startp;

	assert(start <= hi);

	if (start == 0)
		start++;

	startp = ELEM(a, start);

	for (; start < hi; start++, startp = INCPTR(startp)) {

		// Set left (and right) to the index where a[start] (pivot) belongs
		char *leftp = a;
		size_t right = start;
                size_t n;

		/*
		 * Invariants:
		 *   pivot >= all in [0, left).
		 *   pivot <  all in [right, start).
		 */
		while (0 < right) {
			size_t mid = right >> 1;
			void *midp = ELEM(leftp, mid);
			if (CMP(compare, carg, startp, midp) < 0) {
				right = mid;
			} else {
				leftp = INCPTR(midp);
				right -= (mid + 1);
			}
		}
		assert(0 == right);

		/*
		 * The invariants still hold: pivot >= all in [lo, left) and
		 * pivot < all in [left, start), so pivot belongs at left.  Note
		 * that if there are elements equal to pivot, left points to the
		 * first slot after them -- that's why this sort is stable.
		 * Slide elements over to make room to make room for pivot.
		 */
		n = startp - leftp;	// The number of bytes to move

		ASSIGN(pivot, startp);
		memmove(INCPTR(leftp), leftp, n); // POP: overlaps

		// a[left] = pivot;
		ASSIGN(leftp, pivot);
	}
	(void)width;
}

/**
 * Returns the length of the run beginning at the specified position in
 * the specified array and reverses the run if it is descending (ensuring
 * that the run will always be ascending when the method returns).
 *
 * A run is the longest ascending sequence with:
 *
 *    a[0] <= a[1] <= a[2] <= ...
 *
 * or the longest descending sequence with:
 *
 *    a[0] >  a[1] >  a[2] >  ...
 *
 * For its intended use in a stable mergesort, the strictness of the
 * definition of "descending" is needed so that the call can safely
 * reverse a descending sequence without violating stability.
 *
 * @param a the array in which a run is to be counted and possibly reversed
 * @param hi index after the last element that may be contained in the run.
 *        It is required that {@code 0 < hi}.
 * @param compare the comparator to used for the sort
 * @return  the length of the run beginning at the specified position in
 *          the specified array
 */
static size_t NAME(countRunAndMakeAscending) (void *a, size_t hi,
					      CMPPARAMS(compare, carg), size_t width)
{
	size_t runHi = 1;
        char *cur;
        char *next;

	assert(0 < hi);
	if (runHi == hi)
		return 1;

	cur = INCPTR(a);
	next = INCPTR(cur);
	runHi++;

	// Find end of run, and reverse range if descending
	if (CMP(compare, carg, cur, a) < 0) {	// Descending
		while (runHi < hi && CMP(compare, carg, next, cur) < 0) {
			runHi++;
			cur = next;
			next = INCPTR(next);
		}
		CALL(reverseRange) (a, runHi, width);
	} else {		// Ascending
		while (runHi < hi && CMP(compare, carg, next, cur) >= 0) {
			runHi++;
			cur = next;
			next = INCPTR(next);
		}
	}

	(void)width;
	return runHi;
}

/**
 * Reverse the specified range of the specified array.
 *
 * @param a the array in which a range is to be reversed
 * @param hi the index after the last element in the range to be reversed
 */
static void NAME(reverseRange) (void *a, size_t hi, size_t width) {
	DEFINE_TEMP(t);
	char *front = a;
	char *back = ELEM(a, hi - 1);

	assert(hi > 0);

	while (front < back) {
		ASSIGN(t, front);
		ASSIGN(front, back);
		ASSIGN(back, t);
		front = INCPTR(front);
		back = DECPTR(back);
	}
	(void)width;
}

/**
 * Examines the stack of runs waiting to be merged and merges adjacent runs
 * until the stack invariants are reestablished:
 *
 *     1. runLen[i - 3] > runLen[i - 2] + runLen[i - 1]
 *     2. runLen[i - 2] > runLen[i - 1]
 *
 * This method is called each time a new run is pushed onto the stack,
 * so the invariants are guaranteed to hold for i < stackSize upon
 * entry to the method.
 *
 * POP:
 * Modified according to http://envisage-project.eu/wp-content/uploads/2015/02/sorting.pdf
 *
 * and
 *
 * https://bugs.openjdk.java.net/browse/JDK-8072909 (suggestion 2)
 *
 */
static int NAME(mergeCollapse) (struct timsort * ts, size_t width) {
	int err = SUCCESS;

	while (ts->stackSize > 1) {
		size_t n = ts->stackSize - 2;
		struct timsort_run *run = ts->run;

		if ((n > 0 && run[n-1].len <= run[n].len + run[n+1].len)
				|| (n > 1 && run[n-2].len <= run[n].len + run[n-1].len)) {
			if (run[n - 1].len < run[n + 1].len)
				n--;
		} else if (run[n].len > run[n + 1].len) {
			break; /* Invariant is established */
		}
		err = CALL(mergeAt) (ts, n, width);
		if (err)
			break;
        }
	return err;
}

/**
 * Merges all runs on the stack until only one remains.  This method is
 * called once, to complete the sort.
 */
static int NAME(mergeForceCollapse) (struct timsort * ts, size_t width) {
	int err = SUCCESS;

	while (ts->stackSize > 1) {
		size_t n = ts->stackSize - 2;
		if (n > 0 && ts->run[n - 1].len < ts->run[n + 1].len)
			n--;
		err = CALL(mergeAt) (ts, n, width);
		if (err)
			break;
	}
	return err;
}

/**
 * Merges the two runs at stack indices i and i+1.  Run i must be
 * the penultimate or antepenultimate run on the stack.  In other words,
 * i must be equal to stackSize-2 or stackSize-3.
 *
 * @param i stack index of the first of the two runs to merge
 */
static int NAME(mergeAt) (struct timsort * ts, size_t i, size_t width) {
	void *base1 = ts->run[i].base;
	size_t len1 = ts->run[i].len;
	void *base2 = ts->run[i + 1].base;
	size_t len2 = ts->run[i + 1].len;
        size_t k;

	assert(ts->stackSize >= 2);
	assert(i == ts->stackSize - 2 || i == ts->stackSize - 3);
	assert(len1 > 0 && len2 > 0);
	assert(ELEM(base1, len1) == base2);

	/*
	 * Record the length of the combined runs; if i is the 3rd-last
	 * run now, also slide over the last run (which isn't involved
	 * in this merge).  The current run (i+1) goes away in any case.
	 */
	ts->run[i].len = len1 + len2;
	if (i == ts->stackSize - 3) {
		ts->run[i + 1] = ts->run[i + 2];
	}
	ts->stackSize--;

	/*
	 * Find where the first element of run2 goes in run1. Prior elements
	 * in run1 can be ignored (because they're already in place).
	 */
	k = CALL(gallopRight) (base2, base1, len1, 0, CMPARGS(ts->c, ts->carg), width);
	base1 = ELEM(base1, k);
	len1 -= k;
	if (len1 == 0)
		return SUCCESS;

	/*
	 * Find where the last element of run1 goes in run2. Subsequent elements
	 * in run2 can be ignored (because they're already in place).
	 */
	len2 =
	    CALL(gallopLeft) (ELEM(base1, len1 - 1), base2, len2, len2 - 1,
			      CMPARGS(ts->c, ts->carg), width);
	if (len2 == 0)
		return SUCCESS;

	// Merge remaining runs, using tmp array with min(len1, len2) elements
	if (len1 <= len2)
		return CALL(mergeLo) (ts, base1, len1, base2, len2, width);
	else
		return CALL(mergeHi) (ts, base1, len1, base2, len2, width);
}

/**
 * Locates the position at which to insert the specified key into the
 * specified sorted range; if the range contains an element equal to key,
 * returns the index of the leftmost equal element.
 *
 * @param key the key whose insertion point to search for
 * @param base the array in which to search
 * @param len the length of the range; must be > 0
 * @param hint the index at which to begin the search, 0 <= hint < n.
 *     The closer hint is to the result, the faster this method will run.
 * @param c the comparator used to order the range, and to search
 * @return the int k,  0 <= k <= n such that a[b + k - 1] < key <= a[b + k],
 *    pretending that a[b - 1] is minus infinity and a[b + n] is infinity.
 *    In other words, key belongs at index b + k; or in other words,
 *    the first k elements of a should precede key, and the last n - k
 *    should follow it.
 */
static size_t NAME(gallopLeft) (void *key, void *base, size_t len,
				size_t hint, CMPPARAMS(compare, carg),
				size_t width) {
	char *hintp = ELEM(base, hint);
	size_t lastOfs = 0;
	size_t ofs = 1;

	assert(len > 0 && hint < len);
	if (CMP(compare, carg, key, hintp) > 0) {
		// Gallop right until a[hint+lastOfs] < key <= a[hint+ofs]
		size_t maxOfs = len - hint;
		while (ofs < maxOfs
		       && CMP(compare, carg, key, ELEM(hintp, ofs)) > 0) {
			lastOfs = ofs;
			ofs = (ofs << 1) + 1;	// eventually this becomes SIZE_MAX
		}
		if (ofs > maxOfs)
			ofs = maxOfs;

		// Make offsets relative to base
		lastOfs += hint + 1;	// POP: we add 1 here so lastOfs stays non-negative
		ofs += hint;
	} else {		// key <= a[hint]
		// Gallop left until a[hint-ofs] < key <= a[hint-lastOfs]
		const size_t maxOfs = hint + 1;
                size_t tmp;
		while (ofs < maxOfs
		       && CMP(compare, carg, key, ELEM(hintp, -ofs)) <= 0) {
			lastOfs = ofs;
			ofs = (ofs << 1) + 1;	// no need to check for overflow
		}
		if (ofs > maxOfs)
			ofs = maxOfs;

		// Make offsets relative to base
		tmp = lastOfs;
		lastOfs = hint + 1 - ofs;	// POP: we add 1 here so lastOfs stays non-negative
		ofs = hint - tmp;
	}
	assert(lastOfs <= ofs && ofs <= len);

	/*
	 * Now a[lastOfs-1] < key <= a[ofs], so key belongs somewhere
	 * to the right of lastOfs but no farther right than ofs.  Do a binary
	 * search, with invariant a[lastOfs - 1] < key <= a[ofs].
	 */
	// lastOfs++; POP: we added 1 above to keep lastOfs non-negative
	while (lastOfs < ofs) {
		//size_t m = lastOfs + ((ofs - lastOfs) >> 1);
		// http://stackoverflow.com/questions/4844165/safe-integer-middle-value-formula
		size_t m = (lastOfs & ofs) + ((lastOfs ^ ofs) >> 1);

		if (CMP(compare, carg, key, ELEM(base, m)) > 0)
			lastOfs = m + 1;	// a[m] < key
		else
			ofs = m;	// key <= a[m]
	}
	assert(lastOfs == ofs);	// so a[ofs - 1] < key <= a[ofs]
	(void)width;
	return ofs;
}

/**
 * Like gallopLeft, except that if the range contains an element equal to
 * key, gallopRight returns the index after the rightmost equal element.
 *
 * @param key the key whose insertion point to search for
 * @param base the array in which to search
 * @param len the length of the range; must be > 0
 * @param hint the index at which to begin the search, 0 <= hint < n.
 *     The closer hint is to the result, the faster this method will run.
 * @param c the comparator used to order the range, and to search
 * @return the int k,  0 <= k <= n such that a[b + k - 1] <= key < a[b + k]
 */
static size_t NAME(gallopRight) (void *key, void *base, size_t len,
				 size_t hint, CMPPARAMS(compare, carg),
				 size_t width) {
	char *hintp = ELEM(base, hint);
	size_t ofs = 1;
	size_t lastOfs = 0;

	assert(len > 0 && hint < len);

	if (CMP(compare, carg, key, hintp) < 0) {
		// Gallop left until a[hint - ofs] <= key < a[hint - lastOfs]
		size_t maxOfs = hint + 1;
                size_t tmp;
		while (ofs < maxOfs
		       && CMP(compare, carg, key, ELEM(hintp, -ofs)) < 0) {
			lastOfs = ofs;
			ofs = (ofs << 1) + 1;	// no need to check for overflow
		}
		if (ofs > maxOfs)
			ofs = maxOfs;

		// Make offsets relative to base
		tmp = lastOfs;
		lastOfs = hint + 1 - ofs;
		ofs = hint - tmp;
	} else {		// a[hint] <= key
		// Gallop right until a[hint + lastOfs] <= key < a[hint + ofs]
		size_t maxOfs = len - hint;
		while (ofs < maxOfs
		       && CMP(compare, carg, key, ELEM(hintp, ofs)) >= 0) {
			lastOfs = ofs;
			ofs = (ofs << 1) + 1;	// no need to check for overflow
		}
		if (ofs > maxOfs)
			ofs = maxOfs;

		// Make offsets relative to base
		lastOfs += hint + 1;
		ofs += hint;
	}
	assert(lastOfs <= ofs && ofs <= len);

	/*
	 * Now a[lastOfs - 1] <= key < a[ofs], so key belongs somewhere to
	 * the right of lastOfs but no farther right than ofs.  Do a binary
	 * search, with invariant a[lastOfs - 1] <= key < a[ofs].
	 */
	while (lastOfs < ofs) {
		// size_t m = lastOfs + ((ofs - lastOfs) >> 1);
		size_t m = (lastOfs & ofs) + ((lastOfs ^ ofs) >> 1);

		if (CMP(compare, carg, key, ELEM(base, m)) < 0)
			ofs = m;	// key < a[m]
		else
			lastOfs = m + 1;	// a[m] <= key
	}
	assert(lastOfs == ofs);	// so a[ofs - 1] <= key < a[ofs]
	(void)width;
	return ofs;
}

/**
 * Merges two adjacent runs in place, in a stable fashion.  The first
 * element of the first run must be greater than the first element of the
 * second run (a[base1] > a[base2]), and the last element of the first run
 * (a[base1 + len1-1]) must be greater than all elements of the second run.
 *
 * For performance, this method should be called only when len1 <= len2;
 * its twin, mergeHi should be called if len1 >= len2.  (Either method
 * may be called if len1 == len2.)
 *
 * @param base1 first element in first run to be merged
 * @param len1  length of first run to be merged (must be > 0)
 * @param base2 first element in second run to be merged
 *        (must be aBase + aLen)
 * @param len2  length of second run to be merged (must be > 0)
 */
static int NAME(mergeLo) (struct timsort * ts, void *base1, size_t len1,
			  void *base2, size_t len2, size_t width) {

	// Copy first run into temp array
	void *tmp = ensureCapacity(ts, len1, width);
	char *cursor1;
	char *cursor2;
	char *dest;
	comparator compare;	// Use local variable for performance
#ifdef IS_TIMSORT_R
	void *carg;		// Use local variable for performance
#endif
	size_t minGallop;	//  "    "       "     "      "

	assert(len1 > 0 && len2 > 0 && ELEM(base1, len1) == base2);
	if (!tmp)
		return FAILURE;

	// System.arraycopy(a, base1, tmp, 0, len1);
	memcpy(tmp, base1, LEN(len1)); // POP: can't overlap

	cursor1 = tmp;		// Indexes into tmp array
	cursor2 = base2;	// Indexes int a
	dest = base1;		// Indexes int a

	// Move first element of second run and deal with degenerate cases
	// a[dest++] = a[cursor2++];
	ASSIGN(dest, cursor2);
	dest = INCPTR(dest);
	cursor2 = INCPTR(cursor2);

	if (--len2 == 0) {
		memcpy(dest, cursor1, LEN(len1)); // POP: can't overlap
		return SUCCESS;
	}
	if (len1 == 1) {
		memmove(dest, cursor2, LEN(len2)); // POP: overlaps

		// a[dest + len2] = tmp[cursor1]; // Last elt of run 1 to end of merge
		ASSIGN(ELEM(dest, len2), cursor1);
		return SUCCESS;
	}

	compare = ts->c;	// Use local variable for performance
#ifdef IS_TIMSORT_R
	carg = ts->carg;	// Use local variable for performance
#endif
	minGallop = ts->minGallop;	//  "    "       "     "      "

	while (1) {
		size_t count1 = 0;	// Number of times in a row that first run won
		size_t count2 = 0;	// Number of times in a row that second run won

		/*
		 * Do the straightforward thing until (if ever) one run starts
		 * winning consistently.
		 */
		do {
			assert(len1 > 1 && len2 > 0);
			if (CMP(compare, carg, cursor2, cursor1) < 0) {
				ASSIGN(dest, cursor2);
				dest = INCPTR(dest);
				cursor2 = INCPTR(cursor2);
				count2++;
				count1 = 0;
				if (--len2 == 0)
					goto outer;
				if (count2 >= minGallop)
					break;
			} else {
				ASSIGN(dest, cursor1);
				dest = INCPTR(dest);
				cursor1 = INCPTR(cursor1);
				count1++;
				count2 = 0;
				if (--len1 == 1)
					goto outer;
				if (count1 >= minGallop)
					break;
			}
		} while (1);	// (count1 | count2) < minGallop);

		/*
		 * One run is winning so consistently that galloping may be a
		 * huge win. So try that, and continue galloping until (if ever)
		 * neither run appears to be winning consistently anymore.
		 */
		do {
			assert(len1 > 1 && len2 > 0);
			count1 =
			    CALL(gallopRight) (cursor2, cursor1, len1, 0,
					       CMPARGS(compare, carg), width);
			if (count1 != 0) {
				memcpy(dest, cursor1, LEN(count1)); // POP: can't overlap
				dest = ELEM(dest, count1);
				cursor1 = ELEM(cursor1, count1);
				len1 -= count1;
				if (len1 <= 1)	// len1 == 1 || len1 == 0
					goto outer;
			}
			ASSIGN(dest, cursor2);
			dest = INCPTR(dest);
			cursor2 = INCPTR(cursor2);
			if (--len2 == 0)
				goto outer;

			count2 =
			    CALL(gallopLeft) (cursor1, cursor2, len2, 0,
					      CMPARGS(compare, carg), width);
			if (count2 != 0) {
				memmove(dest, cursor2, LEN(count2)); // POP: might overlap
				dest = ELEM(dest, count2);
				cursor2 = ELEM(cursor2, count2);
				len2 -= count2;
				if (len2 == 0)
					goto outer;
			}
			ASSIGN(dest, cursor1);
			dest = INCPTR(dest);
			cursor1 = INCPTR(cursor1);
			if (--len1 == 1)
				goto outer;
			if (minGallop > 0)
				minGallop--;
		} while (count1 >= MIN_GALLOP || count2 >= MIN_GALLOP);
		minGallop += 2;	// Penalize for leaving gallop mode
	}			// End of "outer" loop
outer:
	ts->minGallop = minGallop < 1 ? 1 : minGallop;	// Write back to field

	if (len1 == 1) {
		assert(len2 > 0);
		memmove(dest, cursor2, LEN(len2));	//  POP: might overlap
		ASSIGN(ELEM(dest, len2), cursor1);	//  Last elt of run 1 to end of merge

	} else if (len1 == 0) {
		errno = EINVAL;	// Comparison method violates its general contract
		return FAILURE;
	} else {
		assert(len2 == 0);
		assert(len1 > 1);
		memcpy(dest, cursor1, LEN(len1)); // POP: can't overlap
	}
	return SUCCESS;
}

/**
 * Like mergeLo, except that this method should be called only if
 * len1 >= len2; mergeLo should be called if len1 <= len2.  (Either method
 * may be called if len1 == len2.)
 *
 * @param base1 first element in first run to be merged
 * @param len1  length of first run to be merged (must be > 0)
 * @param base2 first element in second run to be merged
 *        (must be aBase + aLen)
 * @param len2  length of second run to be merged (must be > 0)
 */
static int NAME(mergeHi) (struct timsort * ts, void *base1, size_t len1,
			  void *base2, size_t len2, size_t width) {

	// Copy second run into temp array
	void *tmp = ensureCapacity(ts, len2, width);
	char *cursor1;	// Indexes into a
	char *cursor2;	// Indexes into tmp array
	char *dest;	// Indexes into a
	comparator compare;	// Use local variable for performance
#ifdef IS_TIMSORT_R
	void *carg;		//  "    "       "     "      "
#endif
	size_t minGallop;	//  "    "       "     "      "

	assert(len1 > 0 && len2 > 0 && ELEM(base1, len1) == base2);
	if (!tmp)
		return FAILURE;

	memcpy(tmp, base2, LEN(len2)); // POP: can't overlap

	cursor1 = ELEM(base1, len1 - 1);// Indexes into a
	cursor2 = ELEM(tmp, len2 - 1);	// Indexes into tmp array
	dest = ELEM(base2, len2 - 1);	// Indexes into a

	// Move last element of first run and deal with degenerate cases
	// a[dest--] = a[cursor1--];
	ASSIGN(dest, cursor1);
	dest = DECPTR(dest);
	cursor1 = DECPTR(cursor1);
	if (--len1 == 0) {
		memcpy(ELEM(dest, -(len2 - 1)), tmp, LEN(len2)); // POP: can't overlap
		return SUCCESS;
	}
	if (len2 == 1) {
		dest = ELEM(dest, -len1);
		cursor1 = ELEM(cursor1, -len1);
		memmove(ELEM(dest, 1), ELEM(cursor1, 1), LEN(len1)); // POP: overlaps
		// a[dest] = tmp[cursor2];
		ASSIGN(dest, cursor2);
		return SUCCESS;
	}

	compare = ts->c;		// Use local variable for performance
#ifdef IS_TIMSORT_R
	carg = ts->carg;		// Use local variable for performance
#endif
	minGallop = ts->minGallop;	//  "    "       "     "      "

	while (1) {
		size_t count1 = 0;	// Number of times in a row that first run won
		size_t count2 = 0;	// Number of times in a row that second run won

		/*
		 * Do the straightforward thing until (if ever) one run
		 * appears to win consistently.
		 */
		do {
			assert(len1 > 0 && len2 > 1);
			if (CMP(compare, carg, cursor2, cursor1) < 0) {
				ASSIGN(dest, cursor1);
				dest = DECPTR(dest);
				cursor1 = DECPTR(cursor1);
				count1++;
				count2 = 0;
				if (--len1 == 0)
					goto outer;
			} else {
				ASSIGN(dest, cursor2);
				dest = DECPTR(dest);
				cursor2 = DECPTR(cursor2);
				count2++;
				count1 = 0;
				if (--len2 == 1)
					goto outer;
			}
		} while ((count1 | count2) < minGallop);

		/*
		 * One run is winning so consistently that galloping may be a
		 * huge win. So try that, and continue galloping until (if ever)
		 * neither run appears to be winning consistently anymore.
		 */
		do {
			assert(len1 > 0 && len2 > 1);
			count1 =
			    len1 - CALL(gallopRight) (cursor2, base1,
						      len1, len1 - 1,
						      CMPARGS(compare, carg),
						      width);
			if (count1 != 0) {
				dest = ELEM(dest, -count1);
				cursor1 = ELEM(cursor1, -count1);
				len1 -= count1;
				memmove(INCPTR(dest), INCPTR(cursor1),
				       LEN(count1)); // POP: might overlap
				if (len1 == 0)
					goto outer;
			}
			ASSIGN(dest, cursor2);
			dest = DECPTR(dest);
			cursor2 = DECPTR(cursor2);
			if (--len2 == 1)
				goto outer;

			count2 =
			    len2 - CALL(gallopLeft) (cursor1, tmp, len2,
						     len2 - 1, CMPARGS(compare, carg),
						     width);
			if (count2 != 0) {
				dest = ELEM(dest, -count2);
				cursor2 = ELEM(cursor2, -count2);
				len2 -= count2;
				memcpy(INCPTR(dest), INCPTR(cursor2), LEN(count2)); // POP: can't overlap
				if (len2 <= 1)	// len2 == 1 || len2 == 0
					goto outer;
			}
			ASSIGN(dest, cursor1);
			dest = DECPTR(dest);
			cursor1 = DECPTR(cursor1);
			if (--len1 == 0)
				goto outer;
			if (minGallop > 0)
				minGallop--;
		} while (count1 >= MIN_GALLOP || count2 >= MIN_GALLOP);
		minGallop += 2;	// Penalize for leaving gallop mode
	}			// End of "outer" loop
outer:
	ts->minGallop = minGallop < 1 ? 1 : minGallop;	// Write back to field

	if (len2 == 1) {
		assert(len1 > 0);
		dest = ELEM(dest, -len1);
		cursor1 = ELEM(cursor1, -len1);
		memmove(INCPTR(dest), INCPTR(cursor1), LEN(len1)); // POP: might overlap
		// a[dest] = tmp[cursor2];  // Move first elt of run2 to front of merge
		ASSIGN(dest, cursor2);
	} else if (len2 == 0) {
		errno = EINVAL;	// Comparison method violates its general contract
		return FAILURE;
	} else {
		assert(len1 == 0);
		assert(len2 > 0);
		memcpy(ELEM(dest, -(len2 - 1)), tmp, LEN(len2)); // POP: can't overlap
	}

	return SUCCESS;
}
