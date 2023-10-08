/*
   +----------------------------------------------------------------------+
   | Zend OPcache                                                         |
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <zeev@php.net>                                 |
   |          Stanislav Malyshev <stas@zend.com>                          |
   |          Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/

#include "zend_API.h"
#include "zend_constants.h"
#include "zend_inheritance.h"
#include "zend_accelerator_util_funcs.h"
#include "zend_persist.h"
#include "zend_shared_alloc.h"
#include "zend_observer.h"

#ifdef __SSE2__
/* For SSE2 adler32 */
#include <immintrin.h>
#endif

typedef int (*id_function_t)(void *, void *);
typedef void (*unique_copy_ctor_func_t)(void *pElement);

zend_persistent_script* create_persistent_script(void)
{
	zend_persistent_script *persistent_script = (zend_persistent_script *) emalloc(sizeof(zend_persistent_script));
	memset(persistent_script, 0, sizeof(zend_persistent_script));

	zend_hash_init(&persistent_script->script.function_table, 0, NULL, ZEND_FUNCTION_DTOR, 0);
	/* class_table is usually destroyed by free_persistent_script() that
	 * overrides destructor. ZEND_CLASS_DTOR may be used by standard
	 * PHP compiler
	 */
	zend_hash_init(&persistent_script->script.class_table, 0, NULL, ZEND_CLASS_DTOR, 0);

	return persistent_script;
}

void free_persistent_script(zend_persistent_script *persistent_script, int destroy_elements)
{
	if (!destroy_elements) {
		/* Both the keys and values have been transferred into the global tables.
		 * Set nNumUsed=0 to only deallocate the table, but not destroy any elements. */
		persistent_script->script.function_table.nNumUsed = 0;
		persistent_script->script.class_table.nNumUsed = 0;
	} else {
		destroy_op_array(&persistent_script->script.main_op_array);
	}

	zend_hash_destroy(&persistent_script->script.function_table);
	zend_hash_destroy(&persistent_script->script.class_table);

	if (persistent_script->script.filename) {
		zend_string_release_ex(persistent_script->script.filename, 0);
	}

	if (persistent_script->warnings) {
		for (uint32_t i = 0; i < persistent_script->num_warnings; i++) {
			zend_error_info *info = persistent_script->warnings[i];
			zend_string_release(info->filename);
			zend_string_release(info->message);
			efree(info);
		}
		efree(persistent_script->warnings);
	}

	zend_accel_free_delayed_early_binding_list(persistent_script);

	efree(persistent_script);
}

void zend_accel_move_user_functions(HashTable *src, uint32_t count, zend_script *script)
{
	Bucket *p, *end;
	HashTable *dst;
	zend_string *filename;
	dtor_func_t orig_dtor;
	zend_function *function;

	if (!count) {
		return;
	}

	dst = &script->function_table;
	filename = script->main_op_array.filename;
	orig_dtor = src->pDestructor;
	src->pDestructor = NULL;
	zend_hash_extend(dst, count, 0);
	end = src->arData + src->nNumUsed;
	p = end - count;
	for (; p != end; p++) {
		if (UNEXPECTED(Z_TYPE(p->val) == IS_UNDEF)) continue;
		function = Z_PTR(p->val);
		if (EXPECTED(function->type == ZEND_USER_FUNCTION)
		 && EXPECTED(function->op_array.filename == filename)) {
			_zend_hash_append_ptr(dst, p->key, function);
			zend_hash_del_bucket(src, p);
		}
	}
	src->pDestructor = orig_dtor;
}

void zend_accel_move_user_classes(HashTable *src, uint32_t count, zend_script *script)
{
	Bucket *p, *end;
	HashTable *dst;
	zend_string *filename;
	dtor_func_t orig_dtor;
	zend_class_entry *ce;

	if (!count) {
		return;
	}

	dst = &script->class_table;
	filename = script->main_op_array.filename;
	orig_dtor = src->pDestructor;
	src->pDestructor = NULL;
	zend_hash_extend(dst, count, 0);
	end = src->arData + src->nNumUsed;
	p = end - count;
	for (; p != end; p++) {
		if (UNEXPECTED(Z_TYPE(p->val) == IS_UNDEF)) continue;
		ce = Z_PTR(p->val);
		if (EXPECTED(ce->type == ZEND_USER_CLASS)
		 && EXPECTED(ce->info.user.filename == filename)) {
			_zend_hash_append_ptr(dst, p->key, ce);
			zend_hash_del_bucket(src, p);
		}
	}
	src->pDestructor = orig_dtor;
}

static zend_always_inline void _zend_accel_function_hash_copy(HashTable *target, HashTable *source, bool call_observers)
{
	zend_function *function1, *function2;
	Bucket *p, *end;
	zval *t;

	zend_hash_extend(target, target->nNumUsed + source->nNumUsed, 0);
	p = source->arData;
	end = p + source->nNumUsed;
	for (; p != end; p++) {
		ZEND_ASSERT(Z_TYPE(p->val) != IS_UNDEF);
		ZEND_ASSERT(p->key);
		t = zend_hash_find_known_hash(target, p->key);
		if (UNEXPECTED(t != NULL)) {
			goto failure;
		}
		_zend_hash_append_ptr_ex(target, p->key, Z_PTR(p->val), 1);
		if (UNEXPECTED(call_observers) && *ZSTR_VAL(p->key)) { // if not rtd key
			_zend_observer_function_declared_notify(Z_PTR(p->val), p->key);
		}
	}
	target->nInternalPointer = 0;

	return;

failure:
	function1 = Z_PTR(p->val);
	function2 = Z_PTR_P(t);
	CG(in_compilation) = 1;
	zend_set_compiled_filename(function1->op_array.filename);
	CG(zend_lineno) = function1->op_array.opcodes[0].lineno;
	if (function2->type == ZEND_USER_FUNCTION
		&& function2->op_array.last > 0) {
		zend_error_noreturn(E_ERROR, "Cannot redeclare %s() (previously declared in %s:%d)",
				   ZSTR_VAL(function1->common.function_name),
				   ZSTR_VAL(function2->op_array.filename),
				   (int)function2->op_array.opcodes[0].lineno);
	} else {
		zend_error_noreturn(E_ERROR, "Cannot redeclare %s()", ZSTR_VAL(function1->common.function_name));
	}
}

static zend_always_inline void zend_accel_function_hash_copy(HashTable *target, HashTable *source)
{
	_zend_accel_function_hash_copy(target, source, 0);
}

static zend_never_inline void zend_accel_function_hash_copy_notify(HashTable *target, HashTable *source)
{
	_zend_accel_function_hash_copy(target, source, 1);
}

static zend_always_inline void _zend_accel_class_hash_copy(HashTable *target, HashTable *source, bool call_observers)
{
	Bucket *p, *end;
	zval *t;

	zend_hash_extend(target, target->nNumUsed + source->nNumUsed, 0);
	p = source->arData;
	end = p + source->nNumUsed;
	for (; p != end; p++) {
		ZEND_ASSERT(Z_TYPE(p->val) != IS_UNDEF);
		ZEND_ASSERT(p->key);
		t = zend_hash_find_known_hash(target, p->key);
		if (UNEXPECTED(t != NULL)) {
			if (EXPECTED(ZSTR_LEN(p->key) > 0) && EXPECTED(ZSTR_VAL(p->key)[0] == 0)) {
				/* Runtime definition key. There are two circumstances under which the key can
				 * already be defined:
				 *  1. The file has been re-included without being changed in the meantime. In
				 *     this case we can keep the old value, because we know that the definition
				 *     hasn't changed.
				 *  2. The file has been changed in the meantime, but the RTD key ends up colliding.
				 *     This would be a bug.
				 * As we can't distinguish these cases, we assume that it is 1. and keep the old
				 * value. */
				continue;
			} else if (UNEXPECTED(!ZCG(accel_directives).ignore_dups)) {
				zend_class_entry *ce1 = Z_PTR(p->val);
				if (!(ce1->ce_flags & ZEND_ACC_ANON_CLASS)) {
					CG(in_compilation) = 1;
					zend_set_compiled_filename(ce1->info.user.filename);
					CG(zend_lineno) = ce1->info.user.line_start;
					zend_error_noreturn(E_ERROR,
							"Cannot declare %s %s, because the name is already in use",
							zend_get_object_type(ce1), ZSTR_VAL(ce1->name));
					return;
				}
				continue;
			}
		} else {
			zend_class_entry *ce = Z_PTR(p->val);
			_zend_hash_append_ptr_ex(target, p->key, Z_PTR(p->val), 1);
			if ((ce->ce_flags & ZEND_ACC_LINKED) && ZSTR_VAL(p->key)[0]) {
				if (ZSTR_HAS_CE_CACHE(ce->name)) {
					ZSTR_SET_CE_CACHE_EX(ce->name, ce, 0);
				}
				if (UNEXPECTED(call_observers)) {
					_zend_observer_class_linked_notify(ce, p->key);
				}
			}
		}
	}
	target->nInternalPointer = 0;
}

static zend_always_inline void zend_accel_class_hash_copy(HashTable *target, HashTable *source)
{
	_zend_accel_class_hash_copy(target, source, 0);
}

static zend_never_inline void zend_accel_class_hash_copy_notify(HashTable *target, HashTable *source)
{
	_zend_accel_class_hash_copy(target, source, 1);
}

void zend_accel_build_delayed_early_binding_list(zend_persistent_script *persistent_script)
{
	zend_op_array *op_array = &persistent_script->script.main_op_array;
	if (!(op_array->fn_flags & ZEND_ACC_EARLY_BINDING)) {
		return;
	}

	zend_op *end = op_array->opcodes + op_array->last;
	for (zend_op *opline = op_array->opcodes; opline < end; opline++) {
		if (opline->opcode == ZEND_DECLARE_CLASS_DELAYED) {
			persistent_script->num_early_bindings++;
		}
	}

	zend_early_binding *early_binding = persistent_script->early_bindings =
		emalloc(sizeof(zend_early_binding) * persistent_script->num_early_bindings);

	for (zend_op *opline = op_array->opcodes; opline < end; opline++) {
		if (opline->opcode == ZEND_DECLARE_CLASS_DELAYED) {
			zval *lcname = RT_CONSTANT(opline, opline->op1);
			early_binding->lcname = zend_string_copy(Z_STR_P(lcname));
			early_binding->rtd_key = zend_string_copy(Z_STR_P(lcname + 1));
			early_binding->lc_parent_name =
				zend_string_copy(Z_STR_P(RT_CONSTANT(opline, opline->op2)));
			early_binding->cache_slot = (uint32_t) -1;
			early_binding++;
		}
	}
}

void zend_accel_finalize_delayed_early_binding_list(zend_persistent_script *persistent_script)
{
	if (!persistent_script->num_early_bindings) {
		return;
	}

	zend_early_binding *early_binding = persistent_script->early_bindings;
	zend_early_binding *early_binding_end = early_binding + persistent_script->num_early_bindings;
	zend_op_array *op_array = &persistent_script->script.main_op_array;
	zend_op *opline_end = op_array->opcodes + op_array->last;
	for (zend_op *opline = op_array->opcodes; opline < opline_end; opline++) {
		if (opline->opcode == ZEND_DECLARE_CLASS_DELAYED) {
			zend_string *rtd_key = Z_STR_P(RT_CONSTANT(opline, opline->op1) + 1);
			/* Skip early_binding entries that don't match, maybe their DECLARE_CLASS_DELAYED
			 * was optimized away. */
			while (!zend_string_equals(early_binding->rtd_key, rtd_key)) {
				early_binding++;
				if (early_binding >= early_binding_end) {
					return;
				}
			}

			early_binding->cache_slot = opline->extended_value;
			early_binding++;
			if (early_binding >= early_binding_end) {
				return;
			}
		}
	}
}

void zend_accel_free_delayed_early_binding_list(zend_persistent_script *persistent_script)
{
	if (persistent_script->num_early_bindings) {
		for (uint32_t i = 0; i < persistent_script->num_early_bindings; i++) {
			zend_early_binding *early_binding = &persistent_script->early_bindings[i];
			zend_string_release(early_binding->lcname);
			zend_string_release(early_binding->rtd_key);
			zend_string_release(early_binding->lc_parent_name);
		}
		efree(persistent_script->early_bindings);
		persistent_script->early_bindings = NULL;
		persistent_script->num_early_bindings = 0;
	}
}

static void zend_accel_do_delayed_early_binding(
		zend_persistent_script *persistent_script, zend_op_array *op_array)
{
	ZEND_ASSERT(!ZEND_MAP_PTR(op_array->run_time_cache));
	ZEND_ASSERT(op_array->fn_flags & ZEND_ACC_HEAP_RT_CACHE);
	void *run_time_cache = emalloc(op_array->cache_size);

	ZEND_MAP_PTR_INIT(op_array->run_time_cache, run_time_cache);
	memset(run_time_cache, 0, op_array->cache_size);

	zend_string *orig_compiled_filename = CG(compiled_filename);
	bool orig_in_compilation = CG(in_compilation);
	CG(compiled_filename) = persistent_script->script.filename;
	CG(in_compilation) = 1;
	for (uint32_t i = 0; i < persistent_script->num_early_bindings; i++) {
		zend_early_binding *early_binding = &persistent_script->early_bindings[i];
		zend_class_entry *ce = zend_hash_find_ex_ptr(EG(class_table), early_binding->lcname, 1);
		if (!ce) {
			zval *zv = zend_hash_find_known_hash(EG(class_table), early_binding->rtd_key);
			if (zv) {
				zend_class_entry *orig_ce = Z_CE_P(zv);
				zend_class_entry *parent_ce = !(orig_ce->ce_flags & ZEND_ACC_LINKED)
					? zend_hash_find_ex_ptr(EG(class_table), early_binding->lc_parent_name, 1)
					: NULL;
				if (parent_ce || (orig_ce->ce_flags & ZEND_ACC_LINKED)) {
					ce = zend_try_early_bind(orig_ce, parent_ce, early_binding->lcname, zv);
				}
			}
			if (ce && early_binding->cache_slot != (uint32_t) -1) {
				*(void**)((char*)run_time_cache + early_binding->cache_slot) = ce;
			}
		}
	}
	CG(compiled_filename) = orig_compiled_filename;
	CG(in_compilation) = orig_in_compilation;
}

zend_op_array* zend_accel_load_script(zend_persistent_script *persistent_script, int from_shared_memory)
{
	zend_op_array *op_array;

	op_array = (zend_op_array *) emalloc(sizeof(zend_op_array));
	*op_array = persistent_script->script.main_op_array;

	if (EXPECTED(from_shared_memory)) {
		if (ZCSG(map_ptr_last) > CG(map_ptr_last)) {
			zend_map_ptr_extend(ZCSG(map_ptr_last));
		}

		/* Register __COMPILER_HALT_OFFSET__ constant */
		if (persistent_script->compiler_halt_offset != 0 &&
		    persistent_script->script.filename) {
			zend_string *name;
			static const char haltoff[] = "__COMPILER_HALT_OFFSET__";

			name = zend_mangle_property_name(haltoff, sizeof(haltoff) - 1, ZSTR_VAL(persistent_script->script.filename), ZSTR_LEN(persistent_script->script.filename), 0);
			if (!zend_hash_exists(EG(zend_constants), name)) {
				zend_register_long_constant(ZSTR_VAL(name), ZSTR_LEN(name), persistent_script->compiler_halt_offset, 0, 0);
			}
			zend_string_release_ex(name, 0);
		}
	}

	if (zend_hash_num_elements(&persistent_script->script.function_table) > 0) {
		if (EXPECTED(!zend_observer_function_declared_observed)) {
			zend_accel_function_hash_copy(CG(function_table), &persistent_script->script.function_table);
		} else {
			zend_accel_function_hash_copy_notify(CG(function_table), &persistent_script->script.function_table);
		}
	}

	if (zend_hash_num_elements(&persistent_script->script.class_table) > 0) {
		if (EXPECTED(!zend_observer_class_linked_observed)) {
			zend_accel_class_hash_copy(CG(class_table), &persistent_script->script.class_table);
		} else {
			zend_accel_class_hash_copy_notify(CG(class_table), &persistent_script->script.class_table);
		}
	}

	if (persistent_script->num_early_bindings) {
		zend_accel_do_delayed_early_binding(persistent_script, op_array);
	}

	if (UNEXPECTED(!from_shared_memory)) {
		free_persistent_script(persistent_script, 0); /* free only hashes */
	}

	return op_array;
}

/*
 * zend_adler32() is based on zlib implementation
 * Computes the Adler-32 checksum of a data stream
 *
 * Copyright (C) 1995-2005 Mark Adler
 * For conditions of distribution and use, see copyright notice in zlib.h
 *
 * Copyright (C) 1995-2005 Jean-loup Gailly and Mark Adler
 *
 *  This software is provided 'as-is', without any express or implied
 *  warranty.  In no event will the authors be held liable for any damages
 *  arising from the use of this software.
 *
 *  Permission is granted to anyone to use this software for any purpose,
 *  including commercial applications, and to alter it and redistribute it
 *  freely, subject to the following restrictions:
 *
 *  1. The origin of this software must not be misrepresented; you must not
 *     claim that you wrote the original software. If you use this software
 *     in a product, an acknowledgment in the product documentation would be
 *     appreciated but is not required.
 *  2. Altered source versions must be plainly marked as such, and must not be
 *     misrepresented as being the original software.
 *  3. This notice may not be removed or altered from any source distribution.
 *
 */

#define ADLER32_BASE 65521 /* largest prime smaller than 65536 */
#define ADLER32_NMAX 5552
/* NMAX is the largest n such that 255n(n+1)/2 + (n+1)(BASE-1) <= 2^32-1 */

#define ADLER32_SCALAR_DO1(buf)        {s1 += *(buf); s2 += s1;}
#define ADLER32_SCALAR_DO2(buf, i)     ADLER32_SCALAR_DO1(buf + i); ADLER32_SCALAR_DO1(buf + i + 1);
#define ADLER32_SCALAR_DO4(buf, i)     ADLER32_SCALAR_DO2(buf, i); ADLER32_SCALAR_DO2(buf, i + 2);
#define ADLER32_SCALAR_DO8(buf, i)     ADLER32_SCALAR_DO4(buf, i); ADLER32_SCALAR_DO4(buf, i + 4);
#define ADLER32_SCALAR_DO16(buf)       ADLER32_SCALAR_DO8(buf, 0); ADLER32_SCALAR_DO8(buf, 8);

static zend_always_inline void adler32_do16_loop(unsigned char *buf, unsigned char *end, unsigned int *s1_out, unsigned int *s2_out)
{
	unsigned int s1 = *s1_out;
	unsigned int s2 = *s2_out;

#ifdef __SSE2__
	const __m128i zero = _mm_setzero_si128();

	__m128i accumulate_s2 = zero;
	unsigned int accumulate_s1 = 0;

	do {
		__m128i read = _mm_loadu_si128((__m128i *) buf); /* [A:P] */

		/* Split the 8-bit-element vector into two 16-bit-element vectors where each element gets zero-extended from 8-bits to 16-bits */
		__m128i lower = _mm_unpacklo_epi8(read, zero);									/* [A:H] zero-extended to 16-bits */
		__m128i higher = _mm_unpackhi_epi8(read, zero);									/* [I:P] zero-extended to 16-bits */
		lower = _mm_madd_epi16(lower, _mm_set_epi16(9, 10, 11, 12, 13, 14, 15, 16));	/* [A * 16:H * 9] */
		higher = _mm_madd_epi16(higher, _mm_set_epi16(1, 2, 3, 4, 5, 6, 7, 8)); 		/* [I * 8:P * 1] */

		/* We'll cheat here: it's difficult to add 16-bit elementwise, but we can do 32-bit additions.
			* The highest value the sum of two elements of the vectors can take is 0xff * 16 + 0xff * 8 < 0xffff.
			* That means there is no carry possible from 16->17 bits so the 32-bit addition is safe. */
		__m128i sum = _mm_add_epi32(lower, higher); /* [A * 16 + I * 8:H * 9 + P * 1] */
		accumulate_s2 = _mm_add_epi32(accumulate_s2, sum);
		accumulate_s1 += s1;

		/* Computes 8-bit element-wise abs(buf - zero) and then sums the elements into two 16 bit parts */
		sum = _mm_sad_epu8(read, zero);
		s1 += _mm_cvtsi128_si32(sum) + _mm_extract_epi16(sum, 4);

		buf += 16;
	} while (buf != end);

	/* For convenience, let's do a rename of variables and let accumulate_s2 = [X, Y, Z, W] */
	__m128i shuffled = _mm_shuffle_epi32(accumulate_s2, _MM_SHUFFLE(1, 0, 0, 2));	/* [Y, X, X, Z] */
	accumulate_s2 = _mm_add_epi32(accumulate_s2, shuffled);							/* [X + Y, Y + X, Z + X, W + Z] */
	shuffled = _mm_shuffle_epi32(accumulate_s2, _MM_SHUFFLE(3, 3, 3, 3));			/* [X + Y, X + Y, X + Y, X + Y] */
	accumulate_s2 = _mm_add_epi32(accumulate_s2, shuffled);							/* [/, /, /, W + Z + X + Y] */
	s2 += accumulate_s1 * 16 + _mm_cvtsi128_si32(accumulate_s2);
#else
	do {
		ADLER32_SCALAR_DO16(buf);
		buf += 16;
	} while (buf != end);
#endif

	*s1_out = s1;
	*s2_out = s2;
}

unsigned int zend_adler32(unsigned int checksum, unsigned char *buf, uint32_t len)
{
	unsigned int s1 = checksum & 0xffff;
	unsigned int s2 = (checksum >> 16) & 0xffff;
	unsigned char *end;

	while (len >= ADLER32_NMAX) {
		len -= ADLER32_NMAX;
		end = buf + ADLER32_NMAX;
		adler32_do16_loop(buf, end, &s1, &s2);
		buf = end;
		s1 %= ADLER32_BASE;
		s2 %= ADLER32_BASE;
	}

	if (len) {
		if (len >= 16) {
			end = buf + (len & 0xfff0);
			len &= 0xf;
			adler32_do16_loop(buf, end, &s1, &s2);
			buf = end;
		}
		if (len) {
			end = buf + len;
			do {
				ADLER32_SCALAR_DO1(buf);
				buf++;
			} while (buf != end);
		}
		s1 %= ADLER32_BASE;
		s2 %= ADLER32_BASE;
	}

	return (s2 << 16) | s1;
}

unsigned int zend_accel_script_checksum(zend_persistent_script *persistent_script)
{
	unsigned char *mem = (unsigned char*)persistent_script->mem;
	size_t size = persistent_script->size;
	size_t persistent_script_check_block_size = ((char *)&(persistent_script->dynamic_members)) - (char *)persistent_script;
	unsigned int checksum = ADLER32_INIT;

	if (mem < (unsigned char*)persistent_script) {
		checksum = zend_adler32(checksum, mem, (unsigned char*)persistent_script - mem);
		size -= (unsigned char*)persistent_script - mem;
		mem  += (unsigned char*)persistent_script - mem;
	}

	zend_adler32(checksum, mem, persistent_script_check_block_size);
	mem  += sizeof(*persistent_script);
	size -= sizeof(*persistent_script);

	if (size > 0) {
		checksum = zend_adler32(checksum, mem, size);
	}
	return checksum;
}
