/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2016 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Dmitry Stogov <dmitry@zend.com>                             |
   +----------------------------------------------------------------------+
*/

/* $Id: $ */

#include "zend.h"
#include "zend_globals.h"

ZEND_API zend_string *(*zend_new_interned_string)(zend_string *str);
ZEND_API void (*zend_interned_strings_snapshot)(void);
ZEND_API void (*zend_interned_strings_restore)(void);

static zend_string *zend_new_interned_string_int(zend_string *str);
static void zend_interned_strings_snapshot_int(void);
static void zend_interned_strings_restore_int(void);

ZEND_API zend_ulong zend_hash_func(const char *str, size_t len)
{
	return zend_inline_hash_func(str, len);
}

#ifndef ZTS
static void _str_dtor(zval *zv)
{
	zend_string *str = Z_STR_P(zv);
	pefree(str, GC_FLAGS(str) & IS_STR_PERSISTENT);
}
#endif

void zend_interned_strings_init(void)
{
#ifndef ZTS
	zend_string *str;

	zend_hash_init(&CG(interned_strings), 1024, NULL, _str_dtor, 1);

	CG(interned_strings).nTableMask = -CG(interned_strings).nTableSize;
	HT_SET_DATA_ADDR(&CG(interned_strings), pemalloc(HT_SIZE(&CG(interned_strings)), 1));
	HT_HASH_RESET(&CG(interned_strings));
	CG(interned_strings).u.flags |= HASH_FLAG_INITIALIZED;

	/* interned empty string */
	str = zend_string_alloc(sizeof("")-1, 1);
	ZSTR_VAL(str)[0] = '\000';
	CG(empty_string) = zend_new_interned_string_int(str);
#endif

	/* one char strings (the actual interned strings are going to be created by ext/opcache) */
	memset(CG(one_char_string), 0, sizeof(CG(one_char_string)));

	zend_new_interned_string = zend_new_interned_string_int;
	zend_interned_strings_snapshot = zend_interned_strings_snapshot_int;
	zend_interned_strings_restore = zend_interned_strings_restore_int;
}

void zend_interned_strings_dtor(void)
{
#ifndef ZTS
	zend_hash_destroy(&CG(interned_strings));
#endif
}

static zend_string *zend_new_interned_string_int(zend_string *str)
{
#ifndef ZTS
	zend_ulong h;
	uint nIndex;
	uint idx;
	Bucket *p;

	if (ZSTR_IS_INTERNED(str)) {
		return str;
	}

	h = zend_string_hash_val(str);
	nIndex = h | CG(interned_strings).nTableMask;
	idx = HT_HASH(&CG(interned_strings), nIndex);
	while (idx != HT_INVALID_IDX) {
		p = HT_HASH_TO_BUCKET(&CG(interned_strings), idx);
		if ((p->h == h) && (ZSTR_LEN(p->key) == ZSTR_LEN(str))) {
			if (!memcmp(ZSTR_VAL(p->key), ZSTR_VAL(str), ZSTR_LEN(str))) {
				zend_string_release(str);
				return p->key;
			}
		}
		idx = Z_NEXT(p->val);
	}

	GC_REFCOUNT(str) = 1;
	GC_FLAGS(str) |= IS_STR_INTERNED;

	if (CG(interned_strings).nNumUsed >= CG(interned_strings).nTableSize) {
		if (CG(interned_strings).nTableSize < HT_MAX_SIZE) {	/* Let's double the table size */
			void *new_data;
			void *old_data = HT_GET_DATA_ADDR(&CG(interned_strings));
			Bucket *old_buckets = CG(interned_strings).arData;

			HANDLE_BLOCK_INTERRUPTIONS();
			CG(interned_strings).nTableSize += CG(interned_strings).nTableSize;
			CG(interned_strings).nTableMask = -CG(interned_strings).nTableSize;
			new_data = malloc(HT_SIZE(&CG(interned_strings)));

			if (new_data) {
				HT_SET_DATA_ADDR(&CG(interned_strings), new_data);
				memcpy(CG(interned_strings).arData, old_buckets, sizeof(Bucket) * CG(interned_strings).nNumUsed);
				free(old_data);
				zend_hash_rehash(&CG(interned_strings));
			} else {
				CG(interned_strings).nTableSize = CG(interned_strings).nTableSize >> 1;
				CG(interned_strings).nTableMask = -CG(interned_strings).nTableSize;
			}
			HANDLE_UNBLOCK_INTERRUPTIONS();
		}
	}

	HANDLE_BLOCK_INTERRUPTIONS();

	idx = CG(interned_strings).nNumUsed++;
	CG(interned_strings).nNumOfElements++;
	p = CG(interned_strings).arData + idx;
	p->h = h;
	p->key = str;
	Z_STR(p->val) = str;
	Z_TYPE_INFO(p->val) = IS_INTERNED_STRING_EX;
	nIndex = h | CG(interned_strings).nTableMask;
	Z_NEXT(p->val) = HT_HASH(&CG(interned_strings), nIndex);
	HT_HASH(&CG(interned_strings), nIndex) = HT_IDX_TO_HASH(idx);

	HANDLE_UNBLOCK_INTERRUPTIONS();

	return str;
#else
	return str;
#endif
}

static void zend_interned_strings_snapshot_int(void)
{
#ifndef ZTS
	uint idx;
	Bucket *p;

	idx = CG(interned_strings).nNumUsed;
	while (idx > 0) {
		idx--;
		p = CG(interned_strings).arData + idx;
		ZEND_ASSERT(GC_FLAGS(p->key) & IS_STR_PERSISTENT);
		GC_FLAGS(p->key) |= IS_STR_PERMANENT;
	}
#endif
}

static void zend_interned_strings_restore_int(void)
{
#ifndef ZTS
	uint nIndex;
	uint idx;
	Bucket *p;

	idx = CG(interned_strings).nNumUsed;
	while (idx > 0) {
		idx--;
		p = CG(interned_strings).arData + idx;
		if (GC_FLAGS(p->key) & IS_STR_PERMANENT) break;
		CG(interned_strings).nNumUsed--;
		CG(interned_strings).nNumOfElements--;

		GC_FLAGS(p->key) &= ~IS_STR_INTERNED;
		GC_REFCOUNT(p->key) = 1;
		zend_string_free(p->key);

		nIndex = p->h | CG(interned_strings).nTableMask;
		if (HT_HASH(&CG(interned_strings), nIndex) == HT_IDX_TO_HASH(idx)) {
			HT_HASH(&CG(interned_strings), nIndex) = Z_NEXT(p->val);
		} else {
			uint32_t prev = HT_HASH(&CG(interned_strings), nIndex);
			while (Z_NEXT(HT_HASH_TO_BUCKET(&CG(interned_strings), prev)->val) != idx) {
				prev = Z_NEXT(HT_HASH_TO_BUCKET(&CG(interned_strings), prev)->val);
 			}
			Z_NEXT(HT_HASH_TO_BUCKET(&CG(interned_strings), prev)->val) = Z_NEXT(p->val);
 		}
 	}
#endif
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
