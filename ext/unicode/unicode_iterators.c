/*
   +----------------------------------------------------------------------+
   | PHP Version 6                                                        |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Andrei Zmievski <andrei@php.net>                            |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

/*
 * TODO
 *
 * - test with empty and 1 character strings
 * - optimize current() to pass return_value to the handler so that it fills it
 *   in directly instead of creating a new zval
 * - implement Countable (or count_elements handler) and Seekable interfaces
 */

#include "php.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"
#include <unicode/ubrk.h>

typedef enum {
	ITER_CODE_POINT,
	ITER_COMB_SEQUENCE,
	ITER_CHARACTER,
	ITER_WORD,
	ITER_LINE,
	ITER_SENTENCE,
	ITER_TYPE_LAST,
} text_iter_type;

static const uint32_t ITER_REVERSE = 0x100;
static const uint32_t ITER_TYPE_MASK = 0xFF;

typedef struct {
	zend_object		std;
	UChar*			text;
	uint32_t		text_len;
	text_iter_type	type;
	zval*			current;
	size_t			current_alloc;
	long			flags;
	union {
		struct {
			int32_t offset;
			int32_t cp_offset;
			int32_t index;
		} cp;
		struct {
			int32_t start;
			int32_t end;
			int32_t index;
			int32_t start_cp_offset;
			int32_t end_cp_offset;
		} cs;
		struct {
			int32_t bound;
			int32_t next;
			int32_t index;
			int32_t cp_offset;
			UBreakIterator *iter;
			UBreakIterator *n_iter;
		} brk;
	} u;
	zend_object_iterator iter;
} text_iter_obj;

static inline text_iter_obj* text_iter_to_obj(zend_object_iterator *iter)
{
	return (text_iter_obj *)((char*)iter - offsetof(text_iter_obj, iter));
}

typedef struct {
	int  (*valid)     (text_iter_obj* object, long flags TSRMLS_DC);
	void (*current)   (text_iter_obj* object, long flags TSRMLS_DC);
	int  (*key)       (text_iter_obj* object, long flags TSRMLS_DC);
	int  (*offset)    (text_iter_obj* object, long flags, int32_t *cp_offset TSRMLS_DC);
	void (*next)      (text_iter_obj* object, long flags TSRMLS_DC);
	void (*rewind)    (text_iter_obj* object, long flags TSRMLS_DC);
	void (*following) (text_iter_obj* object, int32_t offset, long flags TSRMLS_DC);
	zend_bool (*isBoundary)(text_iter_obj* object, int32_t offset, long flags TSRMLS_DC);
} text_iter_ops;

enum UBreakIteratorType brk_type_map[] = {
	UBRK_CHARACTER,
	UBRK_WORD,
	UBRK_LINE,
	UBRK_SENTENCE,
};

PHPAPI zend_class_entry* text_iterator_ce;
PHPAPI zend_class_entry* rev_text_iterator_ce;

/* Code point ops */

static int text_iter_cp_valid(text_iter_obj* object, long flags TSRMLS_DC)
{
	if (object->u.cp.offset == UBRK_DONE) {
		return 0;
	}

	if (flags & ITER_REVERSE) {
		return (object->u.cp.offset != 0);
	} else {
		return (object->u.cp.offset != object->text_len);
	}
}

static void text_iter_cp_current(text_iter_obj* object, long flags TSRMLS_DC)
{
	UChar32 cp = 0;
	int32_t tmp, buf_len = 0;

	tmp = object->u.cp.offset;

	if (flags & ITER_REVERSE) {
		if (object->u.cp.offset != UBRK_DONE && object->u.cp.offset > 0) {
			U16_PREV(object->text, 0, tmp, cp);
			buf_len = zend_codepoint_to_uchar(cp, Z_USTRVAL_P(object->current));
		}
	} else {
		if (object->u.cp.offset != UBRK_DONE && object->u.cp.offset < object->text_len) {
			U16_NEXT(object->text, tmp, object->text_len, cp);
			buf_len = zend_codepoint_to_uchar(cp, Z_USTRVAL_P(object->current));
		}
	}
	Z_USTRVAL_P(object->current)[buf_len] = 0;
	Z_USTRLEN_P(object->current) = buf_len;
}

static int text_iter_cp_key(text_iter_obj* object, long flags TSRMLS_DC)
{
	return object->u.cp.index;
}

static int text_iter_cp_offset(text_iter_obj* object, long flags, int32_t *cp_offset TSRMLS_DC)
{
	if (cp_offset) {
		*cp_offset = object->u.cp.cp_offset;
	}
	return object->u.cp.offset;
}

static void text_iter_cp_next(text_iter_obj* object, long flags TSRMLS_DC)
{
	if (object->u.cp.offset == UBRK_DONE) {
		return;
	}

	if (flags & ITER_REVERSE) {
		U16_BACK_1(object->text, 0, object->u.cp.offset);
		if (object->u.cp.offset <= object->text_len) {
			object->u.cp.cp_offset--;
		} else {
			object->u.cp.offset = object->u.cp.cp_offset = UBRK_DONE;
		}
	} else {
		U16_FWD_1(object->text, object->u.cp.offset, object->text_len);
		if (object->u.cp.offset <= object->text_len) {
			object->u.cp.cp_offset++;
		} else {
			object->u.cp.offset = object->u.cp.cp_offset = UBRK_DONE;
		}
	}
	object->u.cp.index++;
}

static void text_iter_cp_rewind(text_iter_obj *object, long flags TSRMLS_DC)
{
	if (flags & ITER_REVERSE) {
		object->u.cp.offset = object->text_len;
		object->u.cp.cp_offset = u_countChar32(object->text, object->text_len);
	} else {
		object->u.cp.offset = 0;
		object->u.cp.cp_offset = 0;
	}
	object->u.cp.index  = 0;
}

static void text_iter_cp_following(text_iter_obj *object, int32_t offset, long flags TSRMLS_DC)
{
	int32_t k;

	if (offset < 0) {
		offset = 0;
	}

	/*
	 * On invalid iterator we always want to start looking for the code unit
	 * offset from the beginning of the string.
	 */
	if (object->u.cp.cp_offset == UBRK_DONE) {
		object->u.cp.cp_offset	= 0;
		object->u.cp.offset 	= 0;
	}

	/*
	 * Try to locate the code unit position relative to the last known codepoint
	 * offset.
	 */
	k = object->u.cp.offset;
	if (offset > object->u.cp.cp_offset) {
		U16_FWD_N(object->text, k, object->text_len, offset - object->u.cp.cp_offset);
	} else {
		U16_BACK_N(object->text, 0, k, object->u.cp.cp_offset - offset);
	}

	/*
	 * Locate the actual boundary.
	 */
	if (flags & ITER_REVERSE) {
		if (k == 0) {
			object->u.cp.cp_offset = UBRK_DONE;
			object->u.cp.offset = UBRK_DONE;
			return;
		} else {
			U16_BACK_1(object->text, 0, k);
		}
	} else {
		if (k == object->text_len) {
			object->u.cp.cp_offset = UBRK_DONE;
			object->u.cp.offset = UBRK_DONE;
			return;
		} else {
			U16_FWD_1(object->text, k, object->text_len);
		}
	}

	/*
	 * If boundary is the same one as where we were at before, simply return.
	 */
	if (k == object->u.cp.offset) {
		return;
	}

	/*
	 * Adjust the internal codepoint offset based on how far we've moved.
	 */
	if (k > object->u.cp.offset) {
		if (k - object->u.cp.offset > 1) {
			object->u.cp.cp_offset += u_countChar32(object->text + object->u.cp.offset, k - object->u.cp.offset);
		} else {
			object->u.cp.cp_offset++;
		}
	} else {
		if (object->u.cp.offset - k > 1) {
			object->u.cp.cp_offset -= u_countChar32(object->text + k, object->u.cp.offset - k);
		} else {
			object->u.cp.cp_offset--;
		}
	}

	object->u.cp.offset = k;
}

static zend_bool text_iter_cp_isBoundary(text_iter_obj *object, int32_t offset, long flags TSRMLS_DC)
{
	int32_t k;

	if (offset < 0) {
		offset = 0;
	}

	/*
	 * On invalid iterator we always want to start looking for the code unit
	 * offset from the beginning of the string.
	 */
	if (object->u.cp.cp_offset == UBRK_DONE) {
		object->u.cp.cp_offset	= 0;
		object->u.cp.offset 	= 0;
	}

	/*
	 * Try to locate the code unit position relative to the last known codepoint
	 * offset.
	 */
	k = object->u.cp.offset;
	if (offset > object->u.cp.cp_offset) {
		U16_FWD_N(object->text, k, object->text_len, offset - object->u.cp.cp_offset);
	} else {
		U16_BACK_N(object->text, 0, k, object->u.cp.cp_offset - offset);
	}

	if (k == object->text_len) {
		object->u.cp.cp_offset += u_countChar32(object->text + object->u.cp.offset, k - object->u.cp.offset);
	} else {
		object->u.cp.cp_offset = offset;
	}
	object->u.cp.offset = k;

	/*
	 * Every codepoint is a boundary.
	 */
	return TRUE;
}

static text_iter_ops text_iter_cp_ops = {
	text_iter_cp_valid,
	text_iter_cp_current,
	text_iter_cp_key,
	text_iter_cp_offset,
	text_iter_cp_next,
	text_iter_cp_rewind,
	text_iter_cp_following,
	text_iter_cp_isBoundary,
};

/* Combining sequence ops */

static void text_iter_helper_move(zend_bool forward, UChar *text, int32_t text_len, int32_t *offset, int32_t *cp_offset)
{
	UChar32 cp;
	int32_t tmp, tmp2;

	if (*offset == UBRK_DONE) {
		return;
	}

	if (forward) {
		if (*offset == text_len) {
			*offset    = UBRK_DONE;
			*cp_offset = UBRK_DONE;
		} else {
			U16_NEXT(text, (*offset), text_len, cp);
			(*cp_offset)++;

			if (u_getCombiningClass(cp) == 0) {
				tmp = *offset;
				tmp2 = *cp_offset;
				/*
				 * At the end of the string cp will be 0 because of the NULL
				 * terminating NULL, so combining class will be 0 as well.
				 */
				while (tmp < text_len) {
					U16_NEXT(text, tmp, text_len, cp);
					tmp2++;
					if (u_getCombiningClass(cp) == 0) {
						break;
					} else {
						*offset    = tmp;
						*cp_offset = tmp2;
					}
				}
			}
		}
	} else {
		if (*offset == 0) {
			*offset    = UBRK_DONE;
			*cp_offset = UBRK_DONE;
		} else {
			U16_PREV(text, 0, (*offset), cp);
			(*cp_offset)--;
			if (u_getCombiningClass(cp) != 0) {
				do {
					U16_PREV(text, 0, (*offset), cp);
					(*cp_offset)--;
				} while (*offset > 0 && u_getCombiningClass(cp) != 0);
			}
		}
	}
}

static int text_iter_cs_valid(text_iter_obj* object, long flags TSRMLS_DC)
{
	if (object->u.cs.start == UBRK_DONE) {
		return 0;
	}

	if (flags & ITER_REVERSE) {
		return (object->u.cs.start != 0);
	} else {
		return (object->u.cs.start != object->text_len);
	}
}

static void text_iter_cs_current(text_iter_obj* object, long flags TSRMLS_DC)
{
	UChar *start;
	int32_t length = -1;

	if (object->u.cs.start != UBRK_DONE) {
		if (flags & ITER_REVERSE) {
			if (object->u.cs.end == object->u.cs.start) {
				text_iter_helper_move(0, object->text, object->text_len,
									  &object->u.cs.end, &object->u.cs.end_cp_offset);
			}
			start = object->text + object->u.cs.end;
		} else {
			if (object->u.cs.end == object->u.cs.start) {
				text_iter_helper_move(1, object->text, object->text_len,
									  &object->u.cs.end, &object->u.cs.end_cp_offset);
			}
			start = object->text + object->u.cs.start;
		}

		if (object->u.cs.end == UBRK_DONE) {
			length = 0;
		} else {
			length = abs(object->u.cs.end - object->u.cs.start);
		}
	} else {
		length = 0;
	}
   
	if (length != 0) {
		if (length+1 > object->current_alloc) {
			object->current_alloc = length+1;
			Z_USTRVAL_P(object->current) = eurealloc(Z_USTRVAL_P(object->current), object->current_alloc);
		}
		u_memcpy(Z_USTRVAL_P(object->current), start, length);
	}

	Z_USTRVAL_P(object->current)[length] = 0;
	Z_USTRLEN_P(object->current) = length;
}

static int text_iter_cs_key(text_iter_obj* object, long flags TSRMLS_DC)
{
	return object->u.cs.index;
}

static int text_iter_cs_offset(text_iter_obj* object, long flags, int32_t *cp_offset TSRMLS_DC)
{
	if (cp_offset) {
		*cp_offset = object->u.cs.start_cp_offset;
	}
	return object->u.cs.start;
}

static void text_iter_cs_next(text_iter_obj* object, long flags TSRMLS_DC)
{
	if (object->u.cs.start == UBRK_DONE) {
		return;
	}

	if (flags & ITER_REVERSE) {
		text_iter_helper_move(0, object->text, object->text_len,
							  &object->u.cs.start, &object->u.cs.start_cp_offset);
	} else {
		text_iter_helper_move(1, object->text, object->text_len,
							  &object->u.cs.start, &object->u.cs.start_cp_offset);
	}
	object->u.cs.end = object->u.cs.start;
	object->u.cs.end_cp_offset = object->u.cs.start_cp_offset;
	object->u.cs.index++;
}

static void text_iter_cs_rewind(text_iter_obj *object, long flags TSRMLS_DC)
{
	if (flags & ITER_REVERSE) {
		object->u.cs.start = object->u.cs.end = object->text_len;
		object->u.cs.start_cp_offset = object->u.cs.end_cp_offset =
				u_countChar32(object->text, object->text_len);
	} else {
		object->u.cs.start = object->u.cs.end = 0;
		object->u.cs.start_cp_offset = object->u.cs.end_cp_offset = 0;
	}
	object->u.cs.index = 0; /* because _next increments index */
}

static void text_iter_cs_following(text_iter_obj *object, int32_t offset, long flags TSRMLS_DC)
{
	int32_t k;

	if (offset < 0) {
		offset = 0;
	}

	/*
	 * On invalid iterator we always want to start looking for the code unit
	 * offset from the beginning of the string.
	 */
	if (object->u.cs.start_cp_offset == UBRK_DONE) {
		object->u.cs.start_cp_offset = 0;
		object->u.cs.start = 0;
	}

	/*
	 * Try to locate the code unit position relative to the last known codepoint
	 * offset.
	 */
	k = object->u.cs.start;
	if (offset > object->u.cs.start_cp_offset) {
		U16_FWD_N(object->text, k, object->text_len, offset - object->u.cs.start_cp_offset);
	} else {
		U16_BACK_N(object->text, 0, k, object->u.cs.start_cp_offset - offset);
	}

	/*
	 * Locate the actual boundary.
	 */
	if (flags & ITER_REVERSE) {
		/*
		 * If offset was at or beyond the length of text, we need to adjust it
		 * to the number of codepoints in the text.
		 */
		if (k == object->text_len) {
			offset = u_countChar32(object->text, object->text_len);
		}
		text_iter_helper_move(0, object->text, object->text_len, &k, &offset);
	} else {
		text_iter_helper_move(1, object->text, object->text_len, &k, &offset);
	}

	if (k == object->u.cs.start) {
		return;
	}

	object->u.cs.start = k;
	object->u.cs.start_cp_offset = offset;
	object->u.cs.end = object->u.cs.start;
}

static zend_bool text_iter_cs_isBoundary(text_iter_obj *object, int32_t offset, long flags TSRMLS_DC)
{
	UChar32 cp;
	int32_t k, tmp;
	zend_bool result;

	if (offset < 0) {
		offset = 0;
	}

	/*
	 * On invalid iterator we always want to start looking for the code unit
	 * offset from the beginning of the string.
	 */
	if (object->u.cs.start_cp_offset == UBRK_DONE) {
		object->u.cs.start_cp_offset = 0;
		object->u.cs.start = 0;
	}

	/*
	 * Try to locate the code unit position relative to the last known codepoint
	 * offset.
	 */
	k = object->u.cs.start;
	if (offset > object->u.cs.start_cp_offset) {
		U16_FWD_N(object->text, k, object->text_len, offset - object->u.cs.start_cp_offset);
	} else {
		U16_BACK_N(object->text, 0, k, object->u.cs.start_cp_offset - offset);
	}

	/* end of the text is always a boundary */
	if (k == object->text_len) {
		offset = u_countChar32(object->text, object->text_len);
		result = 1;
	} else {
	/* if the next codepoint is a base character, it's a boundary */
		tmp = k;
		U16_NEXT(object->text, tmp, object->text_len, cp);
		result = (u_getCombiningClass(cp) == 0);
	}

	if (k == object->u.cs.start) {
		return result;
	}

	object->u.cs.start = k;
	object->u.cs.start_cp_offset = offset;
	object->u.cs.end = object->u.cs.start;

	return result;
}

static text_iter_ops text_iter_cs_ops = {
	text_iter_cs_valid,
	text_iter_cs_current,
	text_iter_cs_key,
	text_iter_cs_offset,
	text_iter_cs_next,
	text_iter_cs_rewind,
	text_iter_cs_following,
	text_iter_cs_isBoundary,
};


/* UBreakIterator Ops */

static int text_iter_brk_valid(text_iter_obj* object, long flags TSRMLS_DC)
{
	if (object->u.brk.bound == UBRK_DONE) {
		return 0;
	}

	if (flags & ITER_REVERSE) {
		return (object->u.brk.bound != 0);
	} else {
		return (object->u.brk.bound != object->text_len);
	}
}

static void text_iter_brk_current(text_iter_obj* object, long flags TSRMLS_DC)
{
	UChar *start;
	int32_t length = -1;

	if (object->u.brk.bound != UBRK_DONE) {
		if (flags & ITER_REVERSE) {
			if (object->u.brk.next == object->u.brk.bound) {
				object->u.brk.next = ubrk_preceding(object->u.brk.n_iter, object->u.brk.bound);
			}
			start = object->text + object->u.brk.next;
		} else {
			if (object->u.brk.next == object->u.brk.bound) {
				object->u.brk.next = ubrk_following(object->u.brk.n_iter, object->u.brk.bound);
			}
			start = object->text + object->u.brk.bound;
		}

		if (object->u.brk.next == UBRK_DONE) {
			length = 0;
		} else {
			length = abs(object->u.brk.next - object->u.brk.bound);
		}
	} else {
		length = 0;
	}

	if (length != 0) {
		if (length+1 > object->current_alloc) {
			object->current_alloc = length+1;
			Z_USTRVAL_P(object->current) = eurealloc(Z_USTRVAL_P(object->current), object->current_alloc);
		}
		u_memcpy(Z_USTRVAL_P(object->current), start, length);
	}

	Z_USTRVAL_P(object->current)[length] = 0;
	Z_USTRLEN_P(object->current) = length;
}

static int text_iter_brk_key(text_iter_obj* object, long flags TSRMLS_DC)
{
	return object->u.brk.index;
}

static int text_iter_brk_offset(text_iter_obj* object, long flags, int32_t *cp_offset TSRMLS_DC)
{
	if (cp_offset) {
		*cp_offset = object->u.brk.cp_offset;
	}
	return object->u.brk.bound;
}

static void text_iter_brk_next(text_iter_obj* object, long flags TSRMLS_DC)
{
	int32_t tmp = object->u.brk.bound;

	if (object->u.brk.bound == UBRK_DONE) {
		return;
	}

	if (flags & ITER_REVERSE) {
		object->u.brk.bound = ubrk_previous(object->u.brk.iter);
		object->u.brk.next  = object->u.brk.bound;
		if (object->u.brk.bound != UBRK_DONE) {
			if (tmp - object->u.brk.bound > 1) {
				object->u.brk.cp_offset -= u_countChar32(object->text + object->u.brk.bound, tmp - object->u.brk.bound);
			} else {
				object->u.brk.cp_offset--;
			}
		} else {
			object->u.brk.cp_offset = UBRK_DONE;
		}
	} else {
		object->u.brk.bound = ubrk_next(object->u.brk.iter);
		object->u.brk.next  = object->u.brk.bound;
		if (object->u.brk.bound != UBRK_DONE) {
			if (object->u.brk.bound - tmp > 1) {
				object->u.brk.cp_offset += u_countChar32(object->text + tmp, object->u.brk.bound - tmp);
			} else {
				object->u.brk.cp_offset++;
			}
		} else {
			object->u.brk.cp_offset = UBRK_DONE;
		}
	}
	object->u.brk.index++;
}

static void text_iter_brk_rewind(text_iter_obj *object, long flags TSRMLS_DC)
{
	if (flags & ITER_REVERSE) {
		object->u.brk.bound   	= ubrk_last(object->u.brk.iter);
		object->u.brk.next		= ubrk_last(object->u.brk.n_iter);
		object->u.brk.cp_offset = u_countChar32(object->text, object->u.brk.bound);
	} else {
		object->u.brk.bound 	= ubrk_first(object->u.brk.iter);
		object->u.brk.next		= ubrk_first(object->u.brk.n_iter);
		object->u.brk.cp_offset = 0;
	}
	object->u.brk.index = 0;
}

static void text_iter_brk_following(text_iter_obj *object, int32_t offset, long flags TSRMLS_DC)
{
	int32_t k, tmp;

	if (offset < 0) {
		offset = 0;
	}

	/*
	 * On invalid iterator we always want to start looking for the code unit
	 * offset from the beginning of the string.
	 */
	if (object->u.brk.cp_offset == UBRK_DONE) {
		object->u.brk.cp_offset	= 0;
		object->u.brk.bound 	= 0;
	}

	/*
	 * Try to locate the code unit position relative to the last known codepoint
	 * offset.
	 */
	k = tmp = object->u.brk.bound;
	if (offset > object->u.brk.cp_offset) {
		U16_FWD_N(object->text, k, object->text_len, offset - object->u.brk.cp_offset);
	} else {
		U16_BACK_N(object->text, 0, k, object->u.brk.cp_offset - offset);
	}

	/*
	 * Locate the actual boundary.
	 */
	if (flags & ITER_REVERSE) {
		object->u.brk.bound = ubrk_preceding(object->u.brk.iter, k);
	} else {
		object->u.brk.bound = ubrk_following(object->u.brk.iter, k);
	}

	/*
	 * If boundary is the same one as where we were at before, simply return.
	 */
	if (object->u.brk.bound == tmp) {
		return;
	}

	object->u.brk.next  = object->u.brk.bound;

	/*
	 * Adjust the internal codepoint offset based on how far we've moved.
	 */
	if (object->u.brk.bound != UBRK_DONE) {
		if (object->u.brk.bound > tmp) {
			if (object->u.brk.bound - tmp > 1) {
				object->u.brk.cp_offset += u_countChar32(object->text + tmp, object->u.brk.bound - tmp);
			} else {
				object->u.brk.cp_offset++;
			}
		} else {
			if (tmp - object->u.brk.bound > 1) {
				object->u.brk.cp_offset -= u_countChar32(object->text + object->u.brk.bound, tmp - object->u.brk.bound);
			} else {
				object->u.brk.cp_offset--;
			}
		}
	} else {
		object->u.brk.cp_offset = UBRK_DONE;
	}
}

static zend_bool text_iter_brk_isBoundary(text_iter_obj *object, int32_t offset, long flags TSRMLS_DC)
{
	int32_t k, tmp;
	UBool result;

	if (offset < 0) {
		offset = 0;
	}

	/*
	 * On invalid iterator we always want to start looking for the code unit
	 * offset from the beginning of the string.
	 */
	if (object->u.brk.cp_offset == UBRK_DONE) {
		object->u.brk.cp_offset	= 0;
		object->u.brk.bound 	= 0;
	}

	/*
	 * Try to locate the code unit position relative to the last known codepoint
	 * offset.
	 */
	k = tmp = object->u.brk.bound;
	if (offset > object->u.brk.cp_offset) {
		U16_FWD_N(object->text, k, object->text_len, offset - object->u.brk.cp_offset);
	} else {
		U16_BACK_N(object->text, 0, k, object->u.brk.cp_offset - offset);
	}

	result = ubrk_isBoundary(object->u.brk.iter, k);

	object->u.brk.bound = ubrk_current(object->u.brk.iter);
	object->u.brk.next  = object->u.brk.bound;

	/*
	 * If boundary is the same one as where we were at before, simply return.
	 */
	if (object->u.brk.bound == tmp) {
		return result;
	}

	/*
	 * Adjust the internal codepoint offset based on how far we've moved.
	 */
	if (object->u.brk.bound != UBRK_DONE) {
		if (object->u.brk.bound > tmp) {
			if (object->u.brk.bound - tmp > 1) {
				object->u.brk.cp_offset += u_countChar32(object->text + tmp, object->u.brk.bound - tmp);
			} else {
				object->u.brk.cp_offset++;
			}
		} else {
			if (tmp - object->u.brk.bound > 1) {
				object->u.brk.cp_offset -= u_countChar32(object->text + object->u.brk.bound, tmp - object->u.brk.bound);
			} else {
				object->u.brk.cp_offset--;
			}
		}
	} else {
		object->u.brk.cp_offset = UBRK_DONE;
	}

	return result;
}

static text_iter_ops text_iter_brk_ops = {
	text_iter_brk_valid,
	text_iter_brk_current,
	text_iter_brk_key,
	text_iter_brk_offset,
	text_iter_brk_next,
	text_iter_brk_rewind,
	text_iter_brk_following,
	text_iter_brk_isBoundary,
};


/* Ops array */

static text_iter_ops* iter_ops[] = {
	&text_iter_cp_ops,
	&text_iter_cs_ops,
	&text_iter_brk_ops,
	&text_iter_brk_ops,
	&text_iter_brk_ops,
	&text_iter_brk_ops,
};

/* Iterator Funcs */

static void text_iter_dtor(zend_object_iterator* iter TSRMLS_DC)
{
	text_iter_obj* obj = text_iter_to_obj(iter);
	zval *object = obj->iter.data;

	zval_ptr_dtor(&object);
}

static int text_iter_valid(zend_object_iterator* iter TSRMLS_DC)
{
	text_iter_obj* obj = text_iter_to_obj(iter);

	if (iter_ops[obj->type]->valid(obj, obj->flags TSRMLS_CC)) {
		return SUCCESS;
	} else {
		return FAILURE;
	}
}

static void text_iter_get_current_data(zend_object_iterator* iter, zval*** data TSRMLS_DC)
{
	text_iter_obj* obj = text_iter_to_obj(iter);

	iter_ops[obj->type]->current(obj, obj->flags TSRMLS_CC);
	*data = &obj->current;
}

static int text_iter_get_current_key(zend_object_iterator* iter, zstr *str_key, uint *str_key_len, ulong *int_key TSRMLS_DC)
{
	text_iter_obj* obj = text_iter_to_obj(iter);

	*int_key = iter_ops[obj->type]->key(obj, obj->flags TSRMLS_CC);
	return HASH_KEY_IS_LONG;
}

static void text_iter_move_forward(zend_object_iterator* iter TSRMLS_DC)
{
	text_iter_obj* obj = text_iter_to_obj(iter);

	iter_ops[obj->type]->next(obj, obj->flags TSRMLS_CC);
}

static void text_iter_rewind(zend_object_iterator* iter TSRMLS_DC)
{
	text_iter_obj* obj = text_iter_to_obj(iter);

	iter_ops[obj->type]->rewind(obj, obj->flags TSRMLS_CC);
}

zend_object_iterator_funcs text_iter_funcs = {
	text_iter_dtor,
	text_iter_valid,
	text_iter_get_current_data,
	text_iter_get_current_key,
	text_iter_move_forward,
	text_iter_rewind,
};

static zend_object_iterator* text_iter_get_iterator(zend_class_entry *ce, zval *object, int by_ref TSRMLS_DC)
{
	text_iter_obj*	iter_object;

	if (by_ref) {
		zend_error(E_ERROR, "An iterator cannot be used with foreach by reference");
	}
	iter_object = (text_iter_obj *) zend_object_store_get_object(object TSRMLS_CC);

	Z_ADDREF_P(object);
	iter_object->iter.data  = (void *) object;
	iter_object->iter.funcs = &text_iter_funcs;

	return (zend_object_iterator *) &iter_object->iter;
}

static void text_iterator_free_storage(void *object TSRMLS_DC)
{
	text_iter_obj *intern = (text_iter_obj *) object;

	zend_hash_destroy(intern->std.properties);
	FREE_HASHTABLE(intern->std.properties);

	if (intern->text) {
		efree(intern->text);
	}
	if (intern->type > ITER_CHARACTER) {
		if (intern->u.brk.iter) {
			ubrk_close(intern->u.brk.iter);
		}
		if (intern->u.brk.n_iter) {
			ubrk_close(intern->u.brk.n_iter);
		}
	}
	zval_ptr_dtor(&intern->current);
	efree(object);
}

static zend_object_value text_iterator_new(zend_class_entry *class_type TSRMLS_DC)
{
	zend_object_value retval;
	text_iter_obj *intern;
	zval *tmp;

	intern = emalloc(sizeof(text_iter_obj));
	memset(intern, 0, sizeof(text_iter_obj));
	intern->std.ce = class_type;

	ALLOC_HASHTABLE(intern->std.properties);
	zend_hash_init(intern->std.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
	zend_hash_copy(intern->std.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));

	intern->type = ITER_CODE_POINT;
	MAKE_STD_ZVAL(intern->current); /* pre-allocate buffer for codepoint */
	intern->current_alloc = 3;
	Z_USTRVAL_P(intern->current) = eumalloc(3);
	Z_USTRVAL_P(intern->current)[0] = 0;
	Z_USTRLEN_P(intern->current) = 0;
	Z_TYPE_P(intern->current) = IS_UNICODE;

	retval.handle = zend_objects_store_put(intern, (zend_objects_store_dtor_t)zend_objects_destroy_object, (zend_objects_free_object_storage_t) text_iterator_free_storage, NULL TSRMLS_CC);
	retval.handlers = zend_get_std_object_handlers();

	return retval;
}

/* {{{ proto void TextIterator::__construct(unicode text [, int flags = TextIterator::CODEPOINT [, string locale ]]) U
   TextIterator constructor */
PHP_METHOD(TextIterator, __construct)
{
	UChar *text;
	int32_t text_len;
	zval *object = getThis();
	text_iter_obj *intern;
	text_iter_type ti_type;
	char *locale = NULL;
	int locale_len;
	long flags = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "u|ls", &text, &text_len, &flags, &locale, &locale_len) == FAILURE) {
		return;
	}

	intern = (text_iter_obj*) zend_object_store_get_object(object TSRMLS_CC);

	intern->text = eustrndup(text, text_len);
	intern->text_len = text_len;
	if (ZEND_NUM_ARGS() > 1) {
		ti_type = flags & ITER_TYPE_MASK;
		if (ti_type < 0 || ti_type >= ITER_TYPE_LAST) { 
			php_error(E_WARNING, "Invalid iterator type in TextIterator constructor");
			ti_type = ITER_CODE_POINT;
		}
		intern->type = ti_type;
		intern->flags = flags;
	}

	if (Z_OBJCE_P(this_ptr) == rev_text_iterator_ce) {
		intern->flags |= ITER_REVERSE;
	}

	if (intern->type >= ITER_CHARACTER && intern->type < ITER_TYPE_LAST) {
		UErrorCode status = U_ZERO_ERROR;
		UErrorCode status2 = U_ZERO_ERROR;
		locale = locale ? locale : UG(default_locale);
		intern->u.brk.iter = ubrk_open(brk_type_map[intern->type - ITER_CHARACTER], locale, intern->text, intern->text_len, &status);
		intern->u.brk.n_iter = ubrk_open(brk_type_map[intern->type - ITER_CHARACTER], locale, intern->text, intern->text_len, &status);
		if (!U_SUCCESS(status) || !U_SUCCESS(status2)) {
			php_error(E_RECOVERABLE_ERROR, "Could not create UBreakIterator for '%s' locale: %s", locale, u_errorName(status));
			return;
		}
	}

	iter_ops[intern->type]->rewind(intern, intern->flags TSRMLS_CC);
}
/* }}} */

/* {{{ proto unicode TextIterator::current() U
   Returns the element at the current boundary */
PHP_METHOD(TextIterator, current)
{
	zval *object = getThis();
	text_iter_obj *intern = (text_iter_obj*) zend_object_store_get_object(object TSRMLS_CC);

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	iter_ops[intern->type]->current(intern, intern->flags TSRMLS_CC);
	RETURN_UNICODEL(Z_USTRVAL_P(intern->current), Z_USTRLEN_P(intern->current), 1);
}
/* }}} */

/* {{{ proto int TextIterator::next([int n]) U
   Advances to the n'th text boundary following the current one and returns its offset */
PHP_METHOD(TextIterator, next)
{
	long i, step = 1;
	int32_t cp_offset;
	zval *object = getThis();
	text_iter_obj *intern = (text_iter_obj*) zend_object_store_get_object(object TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &step) == FAILURE) {
		return;
	}

	if (step <= 0) {
		step = 1;
	}

	for (i = 0; i < step; i++) {
		iter_ops[intern->type]->next(intern, intern->flags TSRMLS_CC);
	}

	if (return_value_used) {
		iter_ops[intern->type]->offset(intern, intern->flags, &cp_offset TSRMLS_CC);
		RETURN_LONG(cp_offset);
	}
}
/* }}} */

/* {{{ proto int TextIterator::key() U
   Returns the number boundaries iterated through */
PHP_METHOD(TextIterator, key)
{
	zval *object = getThis();
	text_iter_obj *intern = (text_iter_obj*) zend_object_store_get_object(object TSRMLS_CC);

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_LONG(iter_ops[intern->type]->key(intern, intern->flags TSRMLS_CC));
}
/* }}} */

/* {{{ proto bool TextIterator::valid() U
   Determines validity of the iterator */
PHP_METHOD(TextIterator, valid)
{
	zval *object = getThis();
	text_iter_obj *intern = (text_iter_obj*) zend_object_store_get_object(object TSRMLS_CC);

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_BOOL(iter_ops[intern->type]->valid(intern, intern->flags TSRMLS_CC));
}
/* }}} */

/* {{{ proto int TextIterator::first() U
   Positions iterator at the first character in the text and returns the offset */
PHP_METHOD(TextIterator, rewind)
{
	int32_t cp_offset;
	zval *object = getThis();
	text_iter_obj *intern = (text_iter_obj*) zend_object_store_get_object(object TSRMLS_CC);

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	iter_ops[intern->type]->rewind(intern, intern->flags TSRMLS_CC);

	if (return_value_used) {
		iter_ops[intern->type]->offset(intern, intern->flags, &cp_offset TSRMLS_CC);
		RETURN_LONG(cp_offset);
	}
}
/* }}} */

/* {{{ proto int TextIterator::last() U
   Positions iterator beyond the last character in the text and returns the offset */
PHP_METHOD(TextIterator, last)
{
	long flags;
	int32_t cp_offset;
	zval *object = getThis();
	text_iter_obj *intern = (text_iter_obj*) zend_object_store_get_object(object TSRMLS_CC);

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	flags = intern->flags ^ ITER_REVERSE;
	iter_ops[intern->type]->rewind(intern, flags TSRMLS_CC);

	if (return_value_used) {
		iter_ops[intern->type]->offset(intern, flags, &cp_offset TSRMLS_CC);
		RETURN_LONG(cp_offset);
	}
}
/* }}} */

/* {{{ proto int TextIterator::offset() U
   Returns the offset of the current text boundary */
PHP_METHOD(TextIterator, offset)
{
	int32_t cp_offset;
	zval *object = getThis();
	text_iter_obj *intern = (text_iter_obj*) zend_object_store_get_object(object TSRMLS_CC);

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	iter_ops[intern->type]->offset(intern, intern->flags, &cp_offset TSRMLS_CC);
	RETURN_LONG(cp_offset);
}
/* }}} */

/* {{{ proto int TextIterator::previous([int n]) U
   Advances to the n'th text boundary preceding the current one and returns its offset */
PHP_METHOD(TextIterator, previous)
{
	long flags, i, step = 1;
	int32_t cp_offset;
	zval *object = getThis();
	text_iter_obj *intern = (text_iter_obj*) zend_object_store_get_object(object TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &step) == FAILURE) {
		return;
	}

	if (step <= 0) {
		step = 1;
	}
	flags = intern->flags ^ ITER_REVERSE;

	for (i = 0; i < step; i++) {
		iter_ops[intern->type]->next(intern, flags TSRMLS_CC);
	}

	if (return_value_used) {
		iter_ops[intern->type]->offset(intern, flags, &cp_offset TSRMLS_CC);
		RETURN_LONG(cp_offset);
	}
}
/* }}} */

/* {{{ proto int TextIterator::following(int offset) U
   Advances to the text boundary following the specified offset and returns its offset */
PHP_METHOD(TextIterator, following)
{
	long offset;
	int32_t cp_offset;
	zval *object = getThis();
	text_iter_obj *intern = (text_iter_obj*) zend_object_store_get_object(object TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &offset) == FAILURE) {
		return;
	}

	iter_ops[intern->type]->following(intern, offset, intern->flags TSRMLS_CC);
	iter_ops[intern->type]->offset(intern, intern->flags, &cp_offset TSRMLS_CC);
	RETURN_LONG(cp_offset);
}
/* }}} */

/* {{{ proto int TextIterator::preceding(int offset) U
   Advances to the text boundary preceding the specified offset and returns its offset */
PHP_METHOD(TextIterator, preceding)
{
	long flags, offset;
	int32_t cp_offset;
	zval *object = getThis();
	text_iter_obj *intern = (text_iter_obj*) zend_object_store_get_object(object TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &offset) == FAILURE) {
		return;
	}

	/*
	 * ReverseTextIterator will behave in the same way as the normal one.
	 */
	flags = intern->flags | ITER_REVERSE;
	iter_ops[intern->type]->following(intern, offset, flags TSRMLS_CC);
	iter_ops[intern->type]->offset(intern, flags, &cp_offset TSRMLS_CC);
	RETURN_LONG(cp_offset);
}
/* }}} */

/* {{{ proto bool TextIterator::isBoundary(int offset) U
   Determines whether specified offset is a text boundary */
PHP_METHOD(TextIterator, isBoundary)
{
	long offset;
	zval *object = getThis();
	text_iter_obj *intern = (text_iter_obj*) zend_object_store_get_object(object TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &offset) == FAILURE) {
		return;
	}

	/*
	 * ReverseTextIterator will behave in the same way as the normal one.
	 */
	RETURN_BOOL(iter_ops[intern->type]->isBoundary(intern, offset, intern->flags TSRMLS_CC));
}
/* }}} */

/* {{{ proto array TextIterator::getAvailableLocales() U
   Returns locales for which text boundary information is available */
PHP_METHOD(TextIterator, getAvailableLocales)
{
	int32_t count, i;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	array_init(return_value);
	count = ubrk_countAvailable();
	for (i = 0; i < count; i++) {
		add_next_index_ascii_string(return_value, (char*)ubrk_getAvailable(i), ZSTR_DUPLICATE);
	}
}
/* }}} */

/* {{{ proto int TextIterator::getRuleStatus() U
   Return the status from the break rule that determined the most recent boundary */
PHP_METHOD(TextIterator, getRuleStatus)
{
	zval *object = getThis();
	text_iter_obj *intern = (text_iter_obj*) zend_object_store_get_object(object TSRMLS_CC);

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (intern->type >= ITER_CHARACTER && intern->type < ITER_TYPE_LAST) {
		RETURN_LONG(ubrk_getRuleStatus(intern->u.brk.iter));
	} else {
		RETURN_LONG(0);
	}
}
/* }}} */

/* {{{ proto array TextIterator::getRuleStatusArray() U
   Return the statuses from the break rules that determined the most recent boundary */
PHP_METHOD(TextIterator, getRuleStatusArray)
{
	int32_t status_vec[32], *vec_ptr = status_vec;
	int32_t vec_size, i;
	zval *object = getThis();
	text_iter_obj *intern = (text_iter_obj*) zend_object_store_get_object(object TSRMLS_CC);
	UErrorCode status = U_ZERO_ERROR;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	array_init(return_value);
	if (intern->type < ITER_CHARACTER) {
		add_next_index_long(return_value, 0);
	} else {
		vec_size = sizeof(status_vec) / sizeof(status_vec[0]);
		vec_size = ubrk_getRuleStatusVec(intern->u.brk.iter, vec_ptr, vec_size, &status);
		if (status == U_BUFFER_OVERFLOW_ERROR) {
			vec_ptr = safe_emalloc(vec_size, sizeof(int32_t), 0);
			status = U_ZERO_ERROR;
			vec_size = ubrk_getRuleStatusVec(intern->u.brk.iter, vec_ptr, vec_size, &status);
		}

		for (i = 0; i < vec_size; i++) {
			add_next_index_long(return_value, vec_ptr[i]);
		}

		if (vec_ptr != status_vec) {
			efree(vec_ptr);
		}
	}
}
/* }}} */

/* {{{ proto array TextIterator::getAll() U
   Return all text pieces determined by the text boundaries */
PHP_METHOD(TextIterator, getAll)
{
	int32_t start, end;
	zval *object = getThis();
	text_iter_obj *intern = (text_iter_obj*) zend_object_store_get_object(object TSRMLS_CC);
	text_iter_ops *ops = iter_ops[intern->type];

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	array_init(return_value);
	ops->rewind(intern, intern->flags TSRMLS_CC);
	start = ops->offset(intern, intern->flags, NULL TSRMLS_CC);
	for (ops->next(intern, intern->flags TSRMLS_CC), end = ops->offset(intern, intern->flags, NULL TSRMLS_CC);
		 end != UBRK_DONE;
		 start = end, ops->next(intern, intern->flags TSRMLS_CC), end = ops->offset(intern, intern->flags, NULL TSRMLS_CC)) {
		if (end > start) {
			add_next_index_unicodel(return_value, intern->text + start, end - start, 1);
		} else {
			add_next_index_unicodel(return_value, intern->text + end, start - end, 1);
		}
	}
}
/* }}} */

/* {{{ TextIterator function entry table */
static const zend_function_entry text_iterator_funcs[] = {

	PHP_ME(TextIterator, __construct, NULL, ZEND_ACC_PUBLIC)

	/* Iterator interface methods */
	PHP_ME(TextIterator, current,  	  NULL, ZEND_ACC_PUBLIC)
	PHP_ME(TextIterator, next,        NULL, ZEND_ACC_PUBLIC)
	PHP_ME(TextIterator, key,         NULL, ZEND_ACC_PUBLIC)
	PHP_ME(TextIterator, valid,       NULL, ZEND_ACC_PUBLIC)
	PHP_ME(TextIterator, rewind,      NULL, ZEND_ACC_PUBLIC)

	PHP_ME(TextIterator, offset,	  NULL, ZEND_ACC_PUBLIC)
	PHP_ME(TextIterator, previous,	  NULL, ZEND_ACC_PUBLIC)
	PHP_ME(TextIterator, last,		  NULL, ZEND_ACC_PUBLIC)
	PHP_ME(TextIterator, following,	  NULL, ZEND_ACC_PUBLIC)
	PHP_ME(TextIterator, preceding,	  NULL, ZEND_ACC_PUBLIC)
	PHP_ME(TextIterator, isBoundary,  NULL, ZEND_ACC_PUBLIC)

	PHP_ME(TextIterator, getAll,      NULL, ZEND_ACC_PUBLIC)

	PHP_ME(TextIterator, getRuleStatus,      NULL, ZEND_ACC_PUBLIC)
	PHP_ME(TextIterator, getRuleStatusArray, NULL, ZEND_ACC_PUBLIC)

	PHP_ME(TextIterator, getAvailableLocales, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)

	PHP_MALIAS(TextIterator, first, rewind, NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */

void php_register_unicode_iterators(TSRMLS_D)
{
	zend_class_entry ce;
	
	INIT_CLASS_ENTRY(ce, "TextIterator", text_iterator_funcs);
	text_iterator_ce = zend_register_internal_class(&ce TSRMLS_CC);
	text_iterator_ce->create_object = text_iterator_new;
	text_iterator_ce->get_iterator  = text_iter_get_iterator;
	text_iterator_ce->ce_flags |= ZEND_ACC_FINAL_CLASS;
	zend_class_implements(text_iterator_ce TSRMLS_CC, 1, zend_ce_iterator);

	INIT_CLASS_ENTRY(ce, "ReverseTextIterator", text_iterator_funcs);
	rev_text_iterator_ce = zend_register_internal_class(&ce TSRMLS_CC);
	rev_text_iterator_ce->create_object = text_iterator_new;
	rev_text_iterator_ce->get_iterator  = text_iter_get_iterator;
	rev_text_iterator_ce->ce_flags |= ZEND_ACC_FINAL_CLASS;
	zend_class_implements(rev_text_iterator_ce TSRMLS_CC, 1, zend_ce_iterator);

    zend_declare_class_constant_long(text_iterator_ce, "CODE_POINT",    sizeof("CODE_POINT")-1,    ITER_CODE_POINT TSRMLS_CC);
    zend_declare_class_constant_long(text_iterator_ce, "COMB_SEQUENCE", sizeof("COMB_SEQUENCE")-1, ITER_COMB_SEQUENCE TSRMLS_CC);
    zend_declare_class_constant_long(text_iterator_ce, "CHARACTER",     sizeof("CHARACTER")-1,     ITER_CHARACTER TSRMLS_CC);
    zend_declare_class_constant_long(text_iterator_ce, "WORD",          sizeof("WORD")-1,          ITER_WORD TSRMLS_CC);
    zend_declare_class_constant_long(text_iterator_ce, "LINE",          sizeof("LINE")-1,          ITER_LINE TSRMLS_CC);
    zend_declare_class_constant_long(text_iterator_ce, "SENTENCE",      sizeof("SENTENCE")-1,      ITER_SENTENCE TSRMLS_CC);

    zend_declare_class_constant_long(text_iterator_ce, "DONE", sizeof("DONE")-1, UBRK_DONE TSRMLS_CC);

    zend_declare_class_constant_long(text_iterator_ce, "WORD_NONE",         sizeof("WORD_NONE")-1,         UBRK_WORD_NONE TSRMLS_CC);
    zend_declare_class_constant_long(text_iterator_ce, "WORD_NONE_LIMIT",   sizeof("WORD_NONE_LIMIT")-1,   UBRK_WORD_NONE_LIMIT TSRMLS_CC);
    zend_declare_class_constant_long(text_iterator_ce, "WORD_NUMBER",       sizeof("WORD_NUMBER")-1,       UBRK_WORD_NUMBER TSRMLS_CC);
    zend_declare_class_constant_long(text_iterator_ce, "WORD_NUMBER_LIMIT", sizeof("WORD_NUMBER_LIMIT")-1, UBRK_WORD_NUMBER_LIMIT TSRMLS_CC);
    zend_declare_class_constant_long(text_iterator_ce, "WORD_LETTER",       sizeof("WORD_LETTER")-1,       UBRK_WORD_LETTER TSRMLS_CC);
    zend_declare_class_constant_long(text_iterator_ce, "WORD_LETTER_LIMIT", sizeof("WORD_LETTER_LIMIT")-1, UBRK_WORD_LETTER_LIMIT TSRMLS_CC);
    zend_declare_class_constant_long(text_iterator_ce, "WORD_KANA",         sizeof("WORD_KANA")-1,         UBRK_WORD_KANA TSRMLS_CC);
    zend_declare_class_constant_long(text_iterator_ce, "WORD_KANA_LIMIT",   sizeof("WORD_KANA_LIMIT")-1,   UBRK_WORD_KANA_LIMIT TSRMLS_CC);
    zend_declare_class_constant_long(text_iterator_ce, "WORD_IDEO",         sizeof("WORD_IDEO")-1,         UBRK_WORD_IDEO TSRMLS_CC);
    zend_declare_class_constant_long(text_iterator_ce, "WORD_IDEO_LIMIT",   sizeof("WORD_IDEO_LIMIT")-1,   UBRK_WORD_IDEO_LIMIT TSRMLS_CC);

    zend_declare_class_constant_long(text_iterator_ce, "LINE_SOFT",         sizeof("LINE_SOFT")-1,         UBRK_LINE_SOFT TSRMLS_CC);
    zend_declare_class_constant_long(text_iterator_ce, "LINE_SOFT_LIMIT",   sizeof("LINE_SOFT_LIMIT")-1,   UBRK_LINE_SOFT_LIMIT TSRMLS_CC);
    zend_declare_class_constant_long(text_iterator_ce, "LINE_HARD",         sizeof("LINE_HARD")-1,         UBRK_LINE_HARD TSRMLS_CC);
    zend_declare_class_constant_long(text_iterator_ce, "LINE_HARD_LIMIT",   sizeof("LINE_HARD_LIMIT")-1,   UBRK_LINE_HARD_LIMIT TSRMLS_CC);

    zend_declare_class_constant_long(text_iterator_ce, "SENTENCE_TERM",       sizeof("SENTENCE_TERM")-1,       UBRK_SENTENCE_TERM TSRMLS_CC);
    zend_declare_class_constant_long(text_iterator_ce, "SENTENCE_TERM_LIMIT", sizeof("SENTENCE_TERM_LIMIT")-1, UBRK_SENTENCE_TERM_LIMIT TSRMLS_CC);
    zend_declare_class_constant_long(text_iterator_ce, "SENTENCE_SEP",        sizeof("SENTENCE_SEP")-1,        UBRK_SENTENCE_SEP TSRMLS_CC);
    zend_declare_class_constant_long(text_iterator_ce, "SENTENCE_SEP_LIMIT",  sizeof("SENTENCE_SEP_LIMIT")-1,  UBRK_SENTENCE_SEP_LIMIT TSRMLS_CC);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */

