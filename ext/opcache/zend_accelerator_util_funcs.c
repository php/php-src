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
#include "zend_accelerator_util_funcs.h"
#include "zend_persist.h"
#include "zend_shared_alloc.h"

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
		persistent_script->script.function_table.pDestructor = NULL;
		persistent_script->script.class_table.pDestructor = NULL;
	} else {
		destroy_op_array(&persistent_script->script.main_op_array);
	}

	zend_hash_destroy(&persistent_script->script.function_table);
	zend_hash_destroy(&persistent_script->script.class_table);

	if (persistent_script->script.filename) {
		zend_string_release_ex(persistent_script->script.filename, 0);
	}

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

static void zend_accel_function_hash_copy(HashTable *target, HashTable *source)
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
		t = zend_hash_find_ex(target, p->key, 1);
		if (UNEXPECTED(t != NULL)) {
			goto failure;
		}
		_zend_hash_append_ptr_ex(target, p->key, Z_PTR(p->val), 1);
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
		zend_error(E_ERROR, "Cannot redeclare %s() (previously declared in %s:%d)",
				   ZSTR_VAL(function1->common.function_name),
				   ZSTR_VAL(function2->op_array.filename),
				   (int)function2->op_array.opcodes[0].lineno);
	} else {
		zend_error(E_ERROR, "Cannot redeclare %s()", ZSTR_VAL(function1->common.function_name));
	}
}

static void zend_accel_class_hash_copy(HashTable *target, HashTable *source)
{
	Bucket *p, *end;
	zval *t;

	zend_hash_extend(target, target->nNumUsed + source->nNumUsed, 0);
	p = source->arData;
	end = p + source->nNumUsed;
	for (; p != end; p++) {
		ZEND_ASSERT(Z_TYPE(p->val) != IS_UNDEF);
		ZEND_ASSERT(p->key);
		t = zend_hash_find_ex(target, p->key, 1);
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
					zend_error(E_ERROR,
							"Cannot declare %s %s, because the name is already in use",
							zend_get_object_type(ce1), ZSTR_VAL(ce1->name));
					return;
				}
				continue;
			}
		} else {
			zend_class_entry *ce = Z_PTR(p->val);
			t = _zend_hash_append_ptr_ex(target, p->key, Z_PTR(p->val), 1);
			if ((ce->ce_flags & ZEND_ACC_LINKED)
			 && ZSTR_HAS_CE_CACHE(ce->name)
			 && ZSTR_VAL(p->key)[0]) {
				ZSTR_SET_CE_CACHE(ce->name, ce);
			}
		}
	}
	target->nInternalPointer = 0;
	return;
}

zend_op_array* zend_accel_load_script(zend_persistent_script *persistent_script, int from_shared_memory)
{
	zend_op_array *op_array;

	op_array = (zend_op_array *) emalloc(sizeof(zend_op_array));
	*op_array = persistent_script->script.main_op_array;

	if (zend_hash_num_elements(&persistent_script->script.function_table) > 0) {
		zend_accel_function_hash_copy(CG(function_table), &persistent_script->script.function_table);
	}

	if (zend_hash_num_elements(&persistent_script->script.class_table) > 0) {
		zend_accel_class_hash_copy(CG(class_table), &persistent_script->script.class_table);
	}

	if (EXPECTED(from_shared_memory)) {
		/* Register __COMPILER_HALT_OFFSET__ constant */
		if (persistent_script->compiler_halt_offset != 0 &&
		    persistent_script->script.filename) {
			zend_string *name;
			static const char haltoff[] = "__COMPILER_HALT_OFFSET__";

			name = zend_mangle_property_name(haltoff, sizeof(haltoff) - 1, ZSTR_VAL(persistent_script->script.filename), ZSTR_LEN(persistent_script->script.filename), 0);
			if (!zend_hash_exists(EG(zend_constants), name)) {
				zend_register_long_constant(ZSTR_VAL(name), ZSTR_LEN(name), persistent_script->compiler_halt_offset, CONST_CS, 0);
			}
			zend_string_release_ex(name, 0);
		}

		if (ZCSG(map_ptr_last) > CG(map_ptr_last)) {
			zend_map_ptr_extend(ZCSG(map_ptr_last));
		}
	}

	if (persistent_script->script.first_early_binding_opline != (uint32_t)-1) {
		zend_string *orig_compiled_filename = CG(compiled_filename);
		CG(compiled_filename) = persistent_script->script.filename;
		zend_do_delayed_early_binding(op_array, persistent_script->script.first_early_binding_opline);
		CG(compiled_filename) = orig_compiled_filename;
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

#define ADLER32_DO1(buf)        {s1 += *(buf); s2 += s1;}
#define ADLER32_DO2(buf, i)     ADLER32_DO1(buf + i); ADLER32_DO1(buf + i + 1);
#define ADLER32_DO4(buf, i)     ADLER32_DO2(buf, i); ADLER32_DO2(buf, i + 2);
#define ADLER32_DO8(buf, i)     ADLER32_DO4(buf, i); ADLER32_DO4(buf, i + 4);
#define ADLER32_DO16(buf)       ADLER32_DO8(buf, 0); ADLER32_DO8(buf, 8);

unsigned int zend_adler32(unsigned int checksum, unsigned char *buf, uint32_t len)
{
	unsigned int s1 = checksum & 0xffff;
	unsigned int s2 = (checksum >> 16) & 0xffff;
	unsigned char *end;

	while (len >= ADLER32_NMAX) {
		len -= ADLER32_NMAX;
		end = buf + ADLER32_NMAX;
		do {
			ADLER32_DO16(buf);
			buf += 16;
		} while (buf != end);
		s1 %= ADLER32_BASE;
		s2 %= ADLER32_BASE;
	}

	if (len) {
		if (len >= 16) {
			end = buf + (len & 0xfff0);
			len &= 0xf;
			do {
				ADLER32_DO16(buf);
				buf += 16;
			} while (buf != end);
		}
		if (len) {
			end = buf + len;
			do {
				ADLER32_DO1(buf);
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
