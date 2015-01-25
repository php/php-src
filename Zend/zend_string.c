/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2015 Zend Technologies Ltd. (http://www.zend.com) |
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
	GC_FLAGS(str) &= ~IS_STR_INTERNED;
	GC_REFCOUNT(str) = 1;
}
#endif

void zend_interned_strings_init(void)
{
#ifndef ZTS
	zend_string *str;

	zend_hash_init(&CG(interned_strings), 1024, NULL, _str_dtor, 1);

	CG(interned_strings).nTableMask = CG(interned_strings).nTableSize - 1;
	CG(interned_strings).arData = (Bucket*) pecalloc(CG(interned_strings).nTableSize, sizeof(Bucket), 1);
	CG(interned_strings).arHash = (uint32_t*) pecalloc(CG(interned_strings).nTableSize, sizeof(uint32_t), 1);
	memset(CG(interned_strings).arHash, INVALID_IDX, CG(interned_strings).nTableSize * sizeof(uint32_t));

	/* interned empty string */
	str = zend_string_alloc(sizeof("")-1, 1);
	str->val[0] = '\000';
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

	if (IS_INTERNED(str)) {
		return str;
	}

	h = zend_string_hash_val(str);
	nIndex = h & CG(interned_strings).nTableMask;
	idx = CG(interned_strings).arHash[nIndex];
	while (idx != INVALID_IDX) {
		p = CG(interned_strings).arData + idx;
		if ((p->h == h) && (p->key->len == str->len)) {
			if (!memcmp(p->key->val, str->val, str->len)) {
				zend_string_release(str);
				return p->key;
			}
		}
		idx = Z_NEXT(p->val);
	}

	GC_REFCOUNT(str) = 1;
	GC_FLAGS(str) |= IS_STR_INTERNED;

	if (CG(interned_strings).nNumUsed >= CG(interned_strings).nTableSize) {
		if ((CG(interned_strings).nTableSize << 1) > 0) {	/* Let's double the table size */
			Bucket *d = (Bucket *) perealloc_recoverable(CG(interned_strings).arData, (CG(interned_strings).nTableSize << 1) * sizeof(Bucket), 1);
			uint32_t *h = (uint32_t *) perealloc_recoverable(CG(interned_strings).arHash, (CG(interned_strings).nTableSize << 1) * sizeof(uint32_t), 1);

			if (d && h) {
				HANDLE_BLOCK_INTERRUPTIONS();
				CG(interned_strings).arData = d;
				CG(interned_strings).arHash = h;
				CG(interned_strings).nTableSize = (CG(interned_strings).nTableSize << 1);
				CG(interned_strings).nTableMask = CG(interned_strings).nTableSize - 1;
				zend_hash_rehash(&CG(interned_strings));
				HANDLE_UNBLOCK_INTERRUPTIONS();
			}
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
	nIndex = h & CG(interned_strings).nTableMask;
	Z_NEXT(p->val) = CG(interned_strings).arHash[nIndex];
	CG(interned_strings).arHash[nIndex] = idx;

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

		nIndex = p->h & CG(interned_strings).nTableMask;
		if (CG(interned_strings).arHash[nIndex] == idx) {
			CG(interned_strings).arHash[nIndex] = Z_NEXT(p->val);
		} else {
			uint prev = CG(interned_strings).arHash[nIndex];
			while (Z_NEXT(CG(interned_strings).arData[prev].val) != idx) {
				prev = Z_NEXT(CG(interned_strings).arData[prev].val);
 			}
			Z_NEXT(CG(interned_strings).arData[prev].val) = Z_NEXT(p->val);
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
