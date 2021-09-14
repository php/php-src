/*
   +----------------------------------------------------------------------+
   | Zend OPcache                                                         |
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
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

#if SIZEOF_SIZE_T <= SIZEOF_ZEND_LONG
/* If sizeof(void*) == sizeof(zend_ulong) we can use zend_hash index functions */
# define accel_xlat_set(old, new)	zend_hash_index_add_new_ptr(&ZCG(bind_hash), (zend_ulong)(zend_uintptr_t)(old), (new))
# define accel_xlat_get(old)		zend_hash_index_find_ptr(&ZCG(bind_hash), (zend_ulong)(zend_uintptr_t)(old))
#else
# define accel_xlat_set(old, new)	zend_hash_str_add_new_ptr(&ZCG(bind_hash), (char*)&(old), sizeof(void*), (new))
# define accel_xlat_get(old)	    zend_hash_str_find_ptr(&ZCG(bind_hash), (char*)&(old), sizeof(void*))
#endif

#define IN_ARENA(ptr) \
	((void*)(ptr) >= ZCG(current_persistent_script)->arena_mem && \
	 (void*)(ptr) < (void*)((char*)ZCG(current_persistent_script)->arena_mem + ZCG(current_persistent_script)->arena_size))

#define ARENA_REALLOC(ptr) \
	(void*)(((char*)(ptr)) + ((char*)ZCG(arena_mem) - (char*)ZCG(current_persistent_script)->arena_mem))

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

static void zend_hash_clone_constants(HashTable *ht)
{
	Bucket *p, *end;
	zend_class_constant *c;

	if (HT_FLAGS(ht) & HASH_FLAG_UNINITIALIZED) {
		return;
	}

	p = emalloc(HT_SIZE(ht));
	memcpy(p, HT_GET_DATA_ADDR(ht), HT_USED_SIZE(ht));
	HT_SET_DATA_ADDR(ht, p);

	p = ht->arData;
	end = p + ht->nNumUsed;
	for (; p != end; p++) {
		ZEND_ASSERT(Z_TYPE(p->val) != IS_UNDEF);
		c = Z_PTR(p->val);
		if (IN_ARENA(c)) {
			c = ARENA_REALLOC(c);
			Z_PTR(p->val) = c;

			if (IN_ARENA(c->ce)) {
				c->ce = ARENA_REALLOC(c->ce);
			}
		}
	}
}

static void zend_hash_clone_methods(HashTable *ht)
{
	Bucket *p, *end;
	zend_op_array *new_entry;

	ht->pDestructor = ZEND_FUNCTION_DTOR;

	if (HT_FLAGS(ht) & HASH_FLAG_UNINITIALIZED) {
		return;
	}

	p = emalloc(HT_SIZE(ht));
	memcpy(p, HT_GET_DATA_ADDR(ht), HT_USED_SIZE(ht));
	HT_SET_DATA_ADDR(ht, p);

	p = ht->arData;
	end = p + ht->nNumUsed;
	for (; p != end; p++) {
		ZEND_ASSERT(Z_TYPE(p->val) != IS_UNDEF);
		new_entry = Z_PTR(p->val);
		if (IN_ARENA(new_entry)) {
			new_entry = ARENA_REALLOC(new_entry);
			Z_PTR(p->val) = new_entry;

			if (IN_ARENA(new_entry->scope)) {
				new_entry->scope = ARENA_REALLOC(new_entry->scope);

				/* update prototype */
				if (IN_ARENA(new_entry->prototype)) {
					new_entry->prototype = ARENA_REALLOC(new_entry->prototype);
				}
			}
			if (IN_ARENA(ZEND_MAP_PTR(new_entry->run_time_cache))) {
				ZEND_MAP_PTR_INIT(new_entry->run_time_cache, ARENA_REALLOC(ZEND_MAP_PTR(new_entry->run_time_cache)));
			}
			ZEND_MAP_PTR_INIT(new_entry->static_variables_ptr, &new_entry->static_variables);
		}
	}
}

static void zend_hash_clone_prop_info(HashTable *ht)
{
	Bucket *p, *end;
	zend_property_info *prop_info;

	if (HT_FLAGS(ht) & HASH_FLAG_UNINITIALIZED) {
		return;
	}

	p = emalloc(HT_SIZE(ht));
	memcpy(p, HT_GET_DATA_ADDR(ht), HT_USED_SIZE(ht));
	HT_SET_DATA_ADDR(ht, p);

	p = ht->arData;
	end = p + ht->nNumUsed;
	for (; p != end; p++) {
		ZEND_ASSERT(Z_TYPE(p->val) != IS_UNDEF);
		prop_info = Z_PTR(p->val);
		if (IN_ARENA(prop_info)) {
			prop_info = ARENA_REALLOC(prop_info);
			Z_PTR(p->val) = prop_info;

			if (IN_ARENA(prop_info->ce)) {
				prop_info->ce = ARENA_REALLOC(prop_info->ce);
			}

			if (ZEND_TYPE_HAS_LIST(prop_info->type)) {
				zend_type_list *list = ZEND_TYPE_LIST(prop_info->type);
				if (IN_ARENA(list)) {
					list = ARENA_REALLOC(list);
					ZEND_TYPE_SET_PTR(prop_info->type, list);

					zend_type *list_type;
					ZEND_TYPE_LIST_FOREACH(ZEND_TYPE_LIST(prop_info->type), list_type) {
						if (ZEND_TYPE_HAS_CE(*list_type)) {
							zend_class_entry *ce = ZEND_TYPE_CE(*list_type);
							if (IN_ARENA(ce)) {
								ce = ARENA_REALLOC(ce);
								ZEND_TYPE_SET_PTR(*list_type, ce);
							}
						}
					} ZEND_TYPE_LIST_FOREACH_END();
				}
			} else if (ZEND_TYPE_HAS_CE(prop_info->type)) {
				zend_class_entry *ce = ZEND_TYPE_CE(prop_info->type);
				if (IN_ARENA(ce)) {
					ce = ARENA_REALLOC(ce);
					ZEND_TYPE_SET_PTR(prop_info->type, ce);
				}
			}
		}
	}
}

#define zend_update_inherited_handler(handler) \
{ \
	if (ce->handler != NULL && IN_ARENA(ce->handler)) { \
		ce->handler = ARENA_REALLOC(ce->handler); \
	} \
}

/* Protects class' refcount, copies default properties, functions and class name */
static void zend_class_copy_ctor(zend_class_entry **pce)
{
	zend_class_entry *ce = *pce;
	zval *src, *dst, *end;

	*pce = ce = ARENA_REALLOC(ce);
	ce->refcount = 1;

	if ((ce->ce_flags & ZEND_ACC_LINKED) && IN_ARENA(ce->parent)) {
		ce->parent = ARENA_REALLOC(ce->parent);
	}

	if (ce->default_properties_table) {
		dst = emalloc(sizeof(zval) * ce->default_properties_count);
		src = ce->default_properties_table;
		end = src + ce->default_properties_count;
		ce->default_properties_table = dst;
		for (; src != end; src++, dst++) {
			ZVAL_COPY_VALUE_PROP(dst, src);
		}
	}

	zend_hash_clone_methods(&ce->function_table);

	/* static members */
	if (ce->default_static_members_table) {
		int i, end;
		zend_class_entry *parent = !(ce->ce_flags & ZEND_ACC_LINKED) ? NULL : ce->parent;

		dst = emalloc(sizeof(zval) * ce->default_static_members_count);
		src = ce->default_static_members_table;
		ce->default_static_members_table = dst;
		i = ce->default_static_members_count - 1;

		/* Copy static properties in this class */
		end = parent ? parent->default_static_members_count : 0;
		for (; i >= end; i--) {
			zval *p = &dst[i];
			ZVAL_COPY_VALUE(p, &src[i]);
		}

		/* Create indirections to static properties from parent classes */
		while (parent && parent->default_static_members_table) {
			end = parent->parent ? parent->parent->default_static_members_count : 0;
			for (; i >= end; i--) {
				zval *p = &dst[i];
				ZVAL_INDIRECT(p, &parent->default_static_members_table[i]);
			}

			parent = parent->parent;
		}
	}
	ZEND_MAP_PTR_INIT(ce->static_members_table, &ce->default_static_members_table);

	/* properties_info */
	zend_hash_clone_prop_info(&ce->properties_info);

	/* constants table */
	zend_hash_clone_constants(&ce->constants_table);

	if (ce->properties_info_table) {
		int i;
		ce->properties_info_table = ARENA_REALLOC(ce->properties_info_table);
		for (i = 0; i < ce->default_properties_count; i++) {
			if (IN_ARENA(ce->properties_info_table[i])) {
				ce->properties_info_table[i] = ARENA_REALLOC(ce->properties_info_table[i]);
			}
		}
	}

	if (ce->num_interfaces) {
		zend_class_name *interface_names;

		if (!(ce->ce_flags & ZEND_ACC_LINKED)) {
			interface_names = emalloc(sizeof(zend_class_name) * ce->num_interfaces);
			memcpy(interface_names, ce->interface_names, sizeof(zend_class_name) * ce->num_interfaces);
			ce->interface_names = interface_names;
		} else {
			zend_class_entry **interfaces = emalloc(sizeof(zend_class_entry*) * ce->num_interfaces);
			uint32_t i;

			for (i = 0; i < ce->num_interfaces; i++) {
				if (IN_ARENA(ce->interfaces[i])) {
					interfaces[i] = ARENA_REALLOC(ce->interfaces[i]);
				} else {
					interfaces[i] = ce->interfaces[i];
				}
			}
			ce->interfaces = interfaces;
		}
	}

	zend_update_inherited_handler(constructor);
	zend_update_inherited_handler(destructor);
	zend_update_inherited_handler(clone);
	zend_update_inherited_handler(__get);
	zend_update_inherited_handler(__set);
	zend_update_inherited_handler(__call);
	zend_update_inherited_handler(__isset);
	zend_update_inherited_handler(__unset);
	zend_update_inherited_handler(__tostring);
	zend_update_inherited_handler(__callstatic);
	zend_update_inherited_handler(__debugInfo);
	zend_update_inherited_handler(__serialize);
	zend_update_inherited_handler(__unserialize);

/* 5.4 traits */
	if (ce->num_traits) {
		zend_class_name *trait_names = emalloc(sizeof(zend_class_name) * ce->num_traits);

		memcpy(trait_names, ce->trait_names, sizeof(zend_class_name) * ce->num_traits);
		ce->trait_names = trait_names;

		if (ce->trait_aliases) {
			zend_trait_alias **trait_aliases;
			int i = 0;

			while (ce->trait_aliases[i]) {
				i++;
			}
			trait_aliases = emalloc(sizeof(zend_trait_alias*) * (i + 1));
			i = 0;
			while (ce->trait_aliases[i]) {
				trait_aliases[i] = emalloc(sizeof(zend_trait_alias));
				memcpy(trait_aliases[i], ce->trait_aliases[i], sizeof(zend_trait_alias));
				i++;
			}
			trait_aliases[i] = NULL;
			ce->trait_aliases = trait_aliases;
		}

		if (ce->trait_precedences) {
			zend_trait_precedence **trait_precedences;
			int i = 0;

			while (ce->trait_precedences[i]) {
				i++;
			}
			trait_precedences = emalloc(sizeof(zend_trait_precedence*) * (i + 1));
			i = 0;
			while (ce->trait_precedences[i]) {
				trait_precedences[i] = emalloc(sizeof(zend_trait_precedence) + (ce->trait_precedences[i]->num_excludes - 1) * sizeof(zend_string*));
				memcpy(trait_precedences[i], ce->trait_precedences[i], sizeof(zend_trait_precedence) + (ce->trait_precedences[i]->num_excludes - 1) * sizeof(zend_string*));
				i++;
			}
			trait_precedences[i] = NULL;
			ce->trait_precedences = trait_precedences;
		}
	}
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
			} else {
				goto failure;
			}
		} else {
			_zend_hash_append_ptr(target, p->key, Z_PTR(p->val));
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
		zend_error(E_ERROR, "Cannot redeclare %s() (previously declared in %s:%d)",
				   ZSTR_VAL(function1->common.function_name),
				   ZSTR_VAL(function2->op_array.filename),
				   (int)function2->op_array.opcodes[0].lineno);
	} else {
		zend_error(E_ERROR, "Cannot redeclare %s()", ZSTR_VAL(function1->common.function_name));
	}
}

static void zend_accel_function_hash_copy_from_shm(HashTable *target, HashTable *source)
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
			if (EXPECTED(ZSTR_LEN(p->key) > 0) && EXPECTED(ZSTR_VAL(p->key)[0] == 0)) {
				/* See comment in zend_accel_function_hash_copy(). */
				continue;
			} else {
				goto failure;
			}
		} else {
			_zend_hash_append_ptr_ex(target, p->key, Z_PTR(p->val), 1);
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
		if (UNEXPECTED(Z_TYPE(p->val) == IS_UNDEF)) continue;
		ZEND_ASSERT(p->key);
		t = zend_hash_find_ex(target, p->key, 1);
		if (UNEXPECTED(t != NULL)) {
			if (EXPECTED(ZSTR_LEN(p->key) > 0) && EXPECTED(ZSTR_VAL(p->key)[0] == 0)) {
				/* See comment in zend_accel_function_hash_copy(). */
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
			t = _zend_hash_append_ptr(target, p->key, Z_PTR(p->val));
		}
	}
	target->nInternalPointer = 0;
	return;
}

static void zend_accel_class_hash_copy_from_shm(HashTable *target, HashTable *source)
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
				/* See comment in zend_accel_function_hash_copy(). */
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
			t = _zend_hash_append_ptr_ex(target, p->key, Z_PTR(p->val), 1);
			if (!(((zend_class_entry*)Z_PTR_P(t))->ce_flags & ZEND_ACC_IMMUTABLE)) {
				zend_class_copy_ctor((zend_class_entry**)&Z_PTR_P(t));
			}
		}
	}
	target->nInternalPointer = 0;
	return;
}

#if __has_feature(memory_sanitizer)
# define fast_memcpy memcpy
#elif defined(__AVX__)
# include <nmmintrin.h>
# if defined(__GNUC__) && defined(__i386__)
static zend_always_inline void fast_memcpy(void *dest, const void *src, size_t size)
{
	size_t delta = (char*)dest - (char*)src;

	__asm__ volatile (
		".align 16\n\t"
		".LL0%=:\n\t"
		"prefetchnta 0x40(%1)\n\t"
		"vmovaps (%1), %%ymm0\n\t"
		"vmovaps 0x20(%1), %%ymm1\n\t"
		"vmovaps %%ymm0, (%1,%2)\n\t"
		"vmovaps %%ymm1, 0x20(%1,%2)\n\t"
		"addl $0x40, %1\n\t"
		"subl $0x40, %0\n\t"
		"ja .LL0%="
		: "+r"(size),
		"+r"(src)
		: "r"(delta)
		: "cc", "memory", "%ymm0", "%ymm1");
}
# elif defined(__GNUC__) && defined(__x86_64__)
static zend_always_inline void fast_memcpy(void *dest, const void *src, size_t size)
{
	size_t delta = (char*)dest - (char*)src;

	__asm__ volatile (
		".align 16\n\t"
		".LL0%=:\n\t"
		"prefetchnta 0x40(%1)\n\t"
		"vmovaps (%1), %%ymm0\n\t"
		"vmovaps 0x20(%1), %%ymm1\n\t"
		"vmovaps %%ymm0, (%1,%2)\n\t"
		"vmovaps %%ymm1, 0x20(%1,%2)\n\t"
		"addq $0x40, %1\n\t"
		"subq $0x40, %0\n\t"
		"ja .LL0%="
		: "+r"(size),
		"+r"(src)
		: "r"(delta)
		: "cc", "memory", "%ymm0", "%ymm1");
}
# else
static zend_always_inline void fast_memcpy(void *dest, const void *src, size_t size)
{
	__m256 *dqdest = (__m256*)dest;
	const __m256 *dqsrc  = (const __m256*)src;
	const __m256 *end  = (const __m256*)((const char*)src + size);

	do {
#ifdef PHP_WIN32
		_mm_prefetch((const char *)(dqsrc + 2), _MM_HINT_NTA);
#else
		_mm_prefetch(dqsrc + 2, _MM_HINT_NTA);
#endif

		__m256 ymm0 = _mm256_load_ps((const float *)(dqsrc + 0));
		__m256 ymm1 = _mm256_load_ps((const float *)(dqsrc + 1));
		dqsrc  += 2;
		_mm256_store_ps((float *)(dqdest + 0), ymm0);
		_mm256_store_ps((float *)(dqdest + 1), ymm1);
		dqdest += 2;
	} while (dqsrc != end);
}
# endif
#elif defined(__SSE2__)
# include <emmintrin.h>
# if defined(__GNUC__) && defined(__i386__)
static zend_always_inline void fast_memcpy(void *dest, const void *src, size_t size)
{
	size_t delta = (char*)dest - (char*)src;

	__asm__ volatile (
		".align 16\n\t"
		".LL0%=:\n\t"
		"prefetchnta 0x40(%1)\n\t"
		"movdqa (%1), %%xmm0\n\t"
		"movdqa 0x10(%1), %%xmm1\n\t"
		"movdqa 0x20(%1), %%xmm2\n\t"
		"movdqa 0x30(%1), %%xmm3\n\t"
		"movdqa %%xmm0, (%1,%2)\n\t"
		"movdqa %%xmm1, 0x10(%1,%2)\n\t"
		"movdqa %%xmm2, 0x20(%1,%2)\n\t"
		"movdqa %%xmm3, 0x30(%1,%2)\n\t"
		"addl $0x40, %1\n\t"
		"subl $0x40, %0\n\t"
		"ja .LL0%="
		: "+r"(size),
		  "+r"(src)
		: "r"(delta)
		: "cc", "memory", "%xmm0", "%xmm1", "%xmm1", "%xmm2");
}
# elif defined(__GNUC__) && defined(__x86_64__) && !defined(__ILP32__)
static zend_always_inline void fast_memcpy(void *dest, const void *src, size_t size)
{
	size_t delta = (char*)dest - (char*)src;

	__asm__ volatile (
		".align 16\n\t"
		".LL0%=:\n\t"
		"prefetchnta 0x40(%1)\n\t"
		"movdqa (%1), %%xmm0\n\t"
		"movdqa 0x10(%1), %%xmm1\n\t"
		"movdqa 0x20(%1), %%xmm2\n\t"
		"movdqa 0x30(%1), %%xmm3\n\t"
		"movdqa %%xmm0, (%1,%2)\n\t"
		"movdqa %%xmm1, 0x10(%1,%2)\n\t"
		"movdqa %%xmm2, 0x20(%1,%2)\n\t"
		"movdqa %%xmm3, 0x30(%1,%2)\n\t"
		"addq $0x40, %1\n\t"
		"subq $0x40, %0\n\t"
		"ja .LL0%="
		: "+r"(size),
		  "+r"(src)
		: "r"(delta)
		: "cc", "memory", "%xmm0", "%xmm1", "%xmm1", "%xmm2");
}
# else
static zend_always_inline void fast_memcpy(void *dest, const void *src, size_t size)
{
	__m128i *dqdest = (__m128i*)dest;
	const __m128i *dqsrc  = (const __m128i*)src;
	const __m128i *end  = (const __m128i*)((const char*)src + size);

	do {
#ifdef PHP_WIN32
		_mm_prefetch((const char *)(dqsrc + 4), _MM_HINT_NTA);
#else
		_mm_prefetch(dqsrc + 4, _MM_HINT_NTA);
#endif

		__m128i xmm0 = _mm_load_si128(dqsrc + 0);
		__m128i xmm1 = _mm_load_si128(dqsrc + 1);
		__m128i xmm2 = _mm_load_si128(dqsrc + 2);
		__m128i xmm3 = _mm_load_si128(dqsrc + 3);
		dqsrc  += 4;
		_mm_store_si128(dqdest + 0, xmm0);
		_mm_store_si128(dqdest + 1, xmm1);
		_mm_store_si128(dqdest + 2, xmm2);
		_mm_store_si128(dqdest + 3, xmm3);
		dqdest += 4;
	} while (dqsrc != end);
}
# endif
#endif

zend_op_array* zend_accel_load_script(zend_persistent_script *persistent_script, int from_shared_memory)
{
	zend_op_array *op_array;

	op_array = (zend_op_array *) emalloc(sizeof(zend_op_array));
	*op_array = persistent_script->script.main_op_array;
	ZEND_MAP_PTR_INIT(op_array->static_variables_ptr, &op_array->static_variables);

	if (EXPECTED(from_shared_memory)) {
		zend_hash_init(&ZCG(bind_hash), 10, NULL, NULL, 0);

		ZCG(current_persistent_script) = persistent_script;
		ZCG(arena_mem) = NULL;
		if (EXPECTED(persistent_script->arena_size)) {
#if defined(__AVX__) || defined(__SSE2__)
			/* Target address must be aligned to 64-byte boundary */
			_mm_prefetch(persistent_script->arena_mem, _MM_HINT_NTA);
			ZCG(arena_mem) = zend_arena_alloc(&CG(arena), persistent_script->arena_size + 64);
			ZCG(arena_mem) = (void*)(((zend_uintptr_t)ZCG(arena_mem) + 63L) & ~63L);
			fast_memcpy(ZCG(arena_mem), persistent_script->arena_mem, persistent_script->arena_size);
#else
			ZCG(arena_mem) = zend_arena_alloc(&CG(arena), persistent_script->arena_size);
			memcpy(ZCG(arena_mem), persistent_script->arena_mem, persistent_script->arena_size);
#endif
		}

		/* Copy all the necessary stuff from shared memory to regular memory, and protect the shared script */
		if (zend_hash_num_elements(&persistent_script->script.class_table) > 0) {
			zend_accel_class_hash_copy_from_shm(CG(class_table), &persistent_script->script.class_table);
		}
		/* we must first to copy all classes and then prepare functions, since functions may try to bind
		   classes - which depend on pre-bind class entries existent in the class table */
		if (zend_hash_num_elements(&persistent_script->script.function_table) > 0) {
			zend_accel_function_hash_copy_from_shm(CG(function_table), &persistent_script->script.function_table);
		}

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

		zend_hash_destroy(&ZCG(bind_hash));
		ZCG(current_persistent_script) = NULL;
		zend_map_ptr_extend(ZCSG(map_ptr_last));
	} else /* if (!from_shared_memory) */ {
		if (zend_hash_num_elements(&persistent_script->script.function_table) > 0) {
			zend_accel_function_hash_copy(CG(function_table), &persistent_script->script.function_table);
		}
		if (zend_hash_num_elements(&persistent_script->script.class_table) > 0) {
			zend_accel_class_hash_copy(CG(class_table), &persistent_script->script.class_table);
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
