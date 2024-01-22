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
   | Authors: Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/

#include "zend.h"
#include "zend_virtual_cwd.h"
#include "zend_compile.h"
#include "zend_vm.h"
#include "zend_interfaces.h"
#include "zend_attributes.h"
#include "zend_system_id.h"
#include "zend_enum.h"

#include "php.h"
#ifdef ZEND_WIN32
#include "ext/standard/md5.h"
#endif

#include "ZendAccelerator.h"
#include "zend_file_cache.h"
#include "zend_shared_alloc.h"
#include "zend_accelerator_util_funcs.h"
#include "zend_accelerator_hash.h"

#if HAVE_JIT
#include "jit/zend_jit.h"
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_SYS_UIO_H
# include <sys/uio.h>
#endif

#ifdef HAVE_SYS_FILE_H
# include <sys/file.h>
#endif

#if __has_feature(memory_sanitizer)
# include <sanitizer/msan_interface.h>
#endif

#ifndef ZEND_WIN32
#define zend_file_cache_unlink unlink
#define zend_file_cache_open open
#else
#define zend_file_cache_unlink php_win32_ioutil_unlink
#define zend_file_cache_open php_win32_ioutil_open
#endif

#ifdef ZEND_WIN32
# define LOCK_SH 0
# define LOCK_EX 1
# define LOCK_UN 2
static int zend_file_cache_flock(int fd, int op)
{
	OVERLAPPED offset = {0,0,0,0,NULL};
	if (op == LOCK_EX) {
		if (LockFileEx((HANDLE)_get_osfhandle(fd),
		               LOCKFILE_EXCLUSIVE_LOCK, 0, 1, 0, &offset) == TRUE) {
			return 0;
		}
	} else if (op == LOCK_SH) {
		if (LockFileEx((HANDLE)_get_osfhandle(fd),
		               0, 0, 1, 0, &offset) == TRUE) {
			return 0;
		}
	} else if (op == LOCK_UN) {
		if (UnlockFileEx((HANDLE)_get_osfhandle(fd),
		                 0, 1, 0, &offset) == TRUE) {
			return 0;
		}
	}
	return -1;
}
#elif defined(HAVE_FLOCK)
# define zend_file_cache_flock flock
#else
# define LOCK_SH 0
# define LOCK_EX 1
# define LOCK_UN 2
static int zend_file_cache_flock(int fd, int type)
{
	return 0;
}
#endif

#ifndef O_BINARY
#  define O_BINARY 0
#endif

#define SUFFIX ".bin"

#define IS_SERIALIZED_INTERNED(ptr) \
	((size_t)(ptr) & Z_UL(1))

/* Allowing == on the upper bound accounts for a potential empty allocation at the end of the
 * memory region. This can also happen for a return-type-only arg_info, where &arg_info[1] is
 * stored, which may point to the end of the region. */
#define IS_SERIALIZED(ptr) \
	((char*)(ptr) <= (char*)script->size)
#define IS_UNSERIALIZED(ptr) \
	(((char*)(ptr) >= (char*)script->mem && (char*)(ptr) <= (char*)script->mem + script->size) || \
	 IS_ACCEL_INTERNED(ptr))
#define SERIALIZE_PTR(ptr) do { \
		if (ptr) { \
			ZEND_ASSERT(IS_UNSERIALIZED(ptr)); \
			(ptr) = (void*)((char*)(ptr) - (char*)script->mem); \
		} \
	} while (0)
#define UNSERIALIZE_PTR(ptr) do { \
		if (ptr) { \
			ZEND_ASSERT(IS_SERIALIZED(ptr)); \
			(ptr) = (void*)((char*)buf + (size_t)(ptr)); \
		} \
	} while (0)
#define SERIALIZE_STR(ptr) do { \
		if (ptr) { \
			if (IS_ACCEL_INTERNED(ptr)) { \
				(ptr) = zend_file_cache_serialize_interned((zend_string*)(ptr), info); \
			} else { \
				ZEND_ASSERT(IS_UNSERIALIZED(ptr)); \
				/* script->corrupted shows if the script in SHM or not */ \
				if (EXPECTED(script->corrupted)) { \
					GC_ADD_FLAGS(ptr, IS_STR_INTERNED); \
					GC_DEL_FLAGS(ptr, IS_STR_PERMANENT); \
				} \
				(ptr) = (void*)((char*)(ptr) - (char*)script->mem); \
			} \
		} \
	} while (0)
#define UNSERIALIZE_STR(ptr) do { \
		if (ptr) { \
			if (IS_SERIALIZED_INTERNED(ptr)) { \
				(ptr) = (void*)zend_file_cache_unserialize_interned((zend_string*)(ptr), !script->corrupted); \
			} else { \
				ZEND_ASSERT(IS_SERIALIZED(ptr)); \
				(ptr) = (void*)((char*)buf + (size_t)(ptr)); \
				/* script->corrupted shows if the script in SHM or not */ \
				if (EXPECTED(!script->corrupted)) { \
					GC_ADD_FLAGS(ptr, IS_STR_INTERNED | IS_STR_PERMANENT); \
				} else { \
					GC_ADD_FLAGS(ptr, IS_STR_INTERNED); \
					GC_DEL_FLAGS(ptr, IS_STR_PERMANENT); \
				} \
			} \
		} \
	} while (0)

#define SERIALIZE_ATTRIBUTES(attributes) do { \
	if ((attributes) && !IS_SERIALIZED(attributes)) { \
		HashTable *ht; \
		SERIALIZE_PTR(attributes); \
		ht = (attributes); \
		UNSERIALIZE_PTR(ht); \
		zend_file_cache_serialize_hash(ht, script, info, buf, zend_file_cache_serialize_attribute); \
	} \
} while (0)

#define UNSERIALIZE_ATTRIBUTES(attributes) do { \
	if ((attributes) && !IS_UNSERIALIZED(attributes)) { \
		HashTable *ht; \
		UNSERIALIZE_PTR(attributes); \
		ht = (attributes); \
		zend_file_cache_unserialize_hash(ht, script, buf, zend_file_cache_unserialize_attribute, NULL); \
	} \
} while (0)

static const uint32_t uninitialized_bucket[-HT_MIN_MASK] =
	{HT_INVALID_IDX, HT_INVALID_IDX};

typedef struct _zend_file_cache_metainfo {
	char         magic[8];
	char         system_id[32];
	size_t       mem_size;
	size_t       str_size;
	size_t       script_offset;
	accel_time_t timestamp;
	uint32_t     checksum;
} zend_file_cache_metainfo;

static int zend_file_cache_mkdir(char *filename, size_t start)
{
	char *s = filename + start;

	while (*s) {
		if (IS_SLASH(*s)) {
			char old = *s;
			*s = '\000';
#ifndef ZEND_WIN32
			if (mkdir(filename, S_IRWXU) < 0 && errno != EEXIST) {
#else
			if (php_win32_ioutil_mkdir(filename, 0700) < 0 && errno != EEXIST) {
#endif
				*s = old;
				return FAILURE;
			}
			*s = old;
		}
		s++;
	}
	return SUCCESS;
}

typedef void (*serialize_callback_t)(zval                     *zv,
                                     zend_persistent_script   *script,
                                     zend_file_cache_metainfo *info,
                                     void                     *buf);

typedef void (*unserialize_callback_t)(zval                    *zv,
                                       zend_persistent_script  *script,
                                       void                    *buf);

static void zend_file_cache_serialize_zval(zval                     *zv,
                                           zend_persistent_script   *script,
                                           zend_file_cache_metainfo *info,
                                           void                     *buf);
static void zend_file_cache_unserialize_zval(zval                    *zv,
                                             zend_persistent_script  *script,
                                             void                    *buf);

static void *zend_file_cache_serialize_interned(zend_string              *str,
                                                zend_file_cache_metainfo *info)
{
	size_t len;
	void *ret;

	/* check if the same interned string was already stored */
	ret = zend_shared_alloc_get_xlat_entry(str);
	if (ret) {
		return ret;
	}

	len = ZEND_MM_ALIGNED_SIZE(_ZSTR_STRUCT_SIZE(ZSTR_LEN(str)));
	ret = (void*)(info->str_size | Z_UL(1));
	zend_shared_alloc_register_xlat_entry(str, ret);

	zend_string *s = (zend_string*)ZCG(mem);
	if (info->str_size + len > ZSTR_LEN(s)) {
		size_t new_len = info->str_size + len;
		s = zend_string_realloc(
			s,
			((_ZSTR_HEADER_SIZE + 1 + new_len + 4095) & ~0xfff) - (_ZSTR_HEADER_SIZE + 1),
			0);
		ZCG(mem) = (void*)s;
	}

	zend_string *new_str = (zend_string *) (ZSTR_VAL(s) + info->str_size);
	memcpy(new_str, str, len);
	GC_ADD_FLAGS(new_str, IS_STR_INTERNED);
	GC_DEL_FLAGS(new_str, IS_STR_PERMANENT|IS_STR_CLASS_NAME_MAP_PTR);
	info->str_size += len;
	return ret;
}

static void *zend_file_cache_unserialize_interned(zend_string *str, bool in_shm)
{
	str = (zend_string*)((char*)ZCG(mem) + ((size_t)(str) & ~Z_UL(1)));
	if (!in_shm) {
		return str;
	}

	zend_string *ret = accel_new_interned_string(str);
	if (ret == str) {
		/* We have to create new SHM allocated string */
		size_t size = _ZSTR_STRUCT_SIZE(ZSTR_LEN(str));
		ret = zend_shared_alloc(size);
		if (!ret) {
			zend_accel_schedule_restart_if_necessary(ACCEL_RESTART_OOM);
			LONGJMP(*EG(bailout), FAILURE);
		}
		memcpy(ret, str, size);
		/* String wasn't interned but we will use it as interned anyway */
		GC_SET_REFCOUNT(ret, 1);
		GC_TYPE_INFO(ret) = GC_STRING | ((IS_STR_INTERNED | IS_STR_PERSISTENT | IS_STR_PERMANENT) << GC_FLAGS_SHIFT);
	}
	return ret;
}

static void zend_file_cache_serialize_hash(HashTable                *ht,
                                           zend_persistent_script   *script,
                                           zend_file_cache_metainfo *info,
                                           void                     *buf,
                                           serialize_callback_t      func)
{
	if (HT_FLAGS(ht) & HASH_FLAG_UNINITIALIZED) {
		ht->arData = NULL;
		return;
	}
	if (IS_SERIALIZED(ht->arData)) {
		return;
	}
	if (HT_IS_PACKED(ht)) {
		zval *p, *end;

		SERIALIZE_PTR(ht->arPacked);
		p = ht->arPacked;
		UNSERIALIZE_PTR(p);
		end = p + ht->nNumUsed;
		while (p < end) {
			if (Z_TYPE_P(p) != IS_UNDEF) {
				func(p, script, info, buf);
			}
			p++;
		}
	} else {
		Bucket *p, *end;

		SERIALIZE_PTR(ht->arData);
		p = ht->arData;
		UNSERIALIZE_PTR(p);
		end = p + ht->nNumUsed;
		while (p < end) {
			if (Z_TYPE(p->val) != IS_UNDEF) {
				SERIALIZE_STR(p->key);
				func(&p->val, script, info, buf);
			}
			p++;
		}
	}
}

static void zend_file_cache_serialize_ast(zend_ast                 *ast,
                                          zend_persistent_script   *script,
                                          zend_file_cache_metainfo *info,
                                          void                     *buf)
{
	uint32_t i;
	zend_ast *tmp;

	if (ast->kind == ZEND_AST_ZVAL || ast->kind == ZEND_AST_CONSTANT) {
		zend_file_cache_serialize_zval(&((zend_ast_zval*)ast)->val, script, info, buf);
	} else if (zend_ast_is_list(ast)) {
		zend_ast_list *list = zend_ast_get_list(ast);
		for (i = 0; i < list->children; i++) {
			if (list->child[i] && !IS_SERIALIZED(list->child[i])) {
				SERIALIZE_PTR(list->child[i]);
				tmp = list->child[i];
				UNSERIALIZE_PTR(tmp);
				zend_file_cache_serialize_ast(tmp, script, info, buf);
			}
		}
	} else {
		uint32_t children = zend_ast_get_num_children(ast);
		for (i = 0; i < children; i++) {
			if (ast->child[i] && !IS_SERIALIZED(ast->child[i])) {
				SERIALIZE_PTR(ast->child[i]);
				tmp = ast->child[i];
				UNSERIALIZE_PTR(tmp);
				zend_file_cache_serialize_ast(tmp, script, info, buf);
			}
		}
	}
}

static void zend_file_cache_serialize_zval(zval                     *zv,
                                           zend_persistent_script   *script,
                                           zend_file_cache_metainfo *info,
                                           void                     *buf)
{
	switch (Z_TYPE_P(zv)) {
		case IS_STRING:
			if (!IS_SERIALIZED(Z_STR_P(zv))) {
				SERIALIZE_STR(Z_STR_P(zv));
			}
			break;
		case IS_ARRAY:
			if (!IS_SERIALIZED(Z_ARR_P(zv))) {
				HashTable *ht;

				SERIALIZE_PTR(Z_ARR_P(zv));
				ht = Z_ARR_P(zv);
				UNSERIALIZE_PTR(ht);
				zend_file_cache_serialize_hash(ht, script, info, buf, zend_file_cache_serialize_zval);
			}
			break;
		case IS_CONSTANT_AST:
			if (!IS_SERIALIZED(Z_AST_P(zv))) {
				zend_ast_ref *ast;

				SERIALIZE_PTR(Z_AST_P(zv));
				ast = Z_AST_P(zv);
				UNSERIALIZE_PTR(ast);
				zend_file_cache_serialize_ast(GC_AST(ast), script, info, buf);
			}
			break;
		case IS_INDIRECT:
			/* Used by static properties. */
			SERIALIZE_PTR(Z_INDIRECT_P(zv));
			break;
		default:
			ZEND_ASSERT(Z_TYPE_P(zv) < IS_STRING);
			break;
	}
}

static void zend_file_cache_serialize_attribute(zval                     *zv,
                                                zend_persistent_script   *script,
                                                zend_file_cache_metainfo *info,
                                                void                     *buf)
{
	zend_attribute *attr = Z_PTR_P(zv);
	uint32_t i;

	SERIALIZE_PTR(Z_PTR_P(zv));
	attr = Z_PTR_P(zv);
	UNSERIALIZE_PTR(attr);

	SERIALIZE_STR(attr->name);
	SERIALIZE_STR(attr->lcname);

	for (i = 0; i < attr->argc; i++) {
		SERIALIZE_STR(attr->args[i].name);
		zend_file_cache_serialize_zval(&attr->args[i].value, script, info, buf);
	}
}

static void zend_file_cache_serialize_type(
		zend_type *type, zend_persistent_script *script, zend_file_cache_metainfo *info, void *buf)
{
	if (ZEND_TYPE_HAS_LIST(*type)) {
		zend_type_list *list = ZEND_TYPE_LIST(*type);
		SERIALIZE_PTR(list);
		ZEND_TYPE_SET_PTR(*type, list);
		UNSERIALIZE_PTR(list);

		zend_type *list_type;
		ZEND_TYPE_LIST_FOREACH(list, list_type) {
			zend_file_cache_serialize_type(list_type, script, info, buf);
		} ZEND_TYPE_LIST_FOREACH_END();
	} else if (ZEND_TYPE_HAS_NAME(*type)) {
		zend_string *type_name = ZEND_TYPE_NAME(*type);
		SERIALIZE_STR(type_name);
		ZEND_TYPE_SET_PTR(*type, type_name);
	}
}

static void zend_file_cache_serialize_op_array(zend_op_array            *op_array,
                                               zend_persistent_script   *script,
                                               zend_file_cache_metainfo *info,
                                               void                     *buf)
{
	ZEND_MAP_PTR_INIT(op_array->static_variables_ptr, NULL);
	ZEND_MAP_PTR_INIT(op_array->run_time_cache, NULL);

	/* Check whether this op_array has already been serialized. */
	if (IS_SERIALIZED(op_array->opcodes)) {
		ZEND_ASSERT(op_array->scope && "Only method op_arrays should be shared");
		return;
	}

	if (op_array->scope) {
		if (UNEXPECTED(zend_shared_alloc_get_xlat_entry(op_array->opcodes))) {
			op_array->refcount = (uint32_t*)(intptr_t)-1;
			SERIALIZE_PTR(op_array->static_variables);
			SERIALIZE_PTR(op_array->literals);
			SERIALIZE_PTR(op_array->opcodes);
			SERIALIZE_PTR(op_array->arg_info);
			SERIALIZE_PTR(op_array->vars);
			SERIALIZE_STR(op_array->function_name);
			SERIALIZE_STR(op_array->filename);
			SERIALIZE_PTR(op_array->live_range);
			SERIALIZE_PTR(op_array->scope);
			SERIALIZE_STR(op_array->doc_comment);
			SERIALIZE_ATTRIBUTES(op_array->attributes);
			SERIALIZE_PTR(op_array->try_catch_array);
			SERIALIZE_PTR(op_array->prototype);
			return;
		}
		zend_shared_alloc_register_xlat_entry(op_array->opcodes, op_array->opcodes);
	}

	if (op_array->static_variables) {
		HashTable *ht;

		SERIALIZE_PTR(op_array->static_variables);
		ht = op_array->static_variables;
		UNSERIALIZE_PTR(ht);
		zend_file_cache_serialize_hash(ht, script, info, buf, zend_file_cache_serialize_zval);
	}

	if (op_array->literals) {
		zval *p, *end;

		SERIALIZE_PTR(op_array->literals);
		p = op_array->literals;
		UNSERIALIZE_PTR(p);
		end = p + op_array->last_literal;
		while (p < end) {
			zend_file_cache_serialize_zval(p, script, info, buf);
			p++;
		}
	}

	{
		zend_op *opline, *end;

#if !ZEND_USE_ABS_CONST_ADDR
		zval *literals = op_array->literals;
		UNSERIALIZE_PTR(literals);
#endif

		SERIALIZE_PTR(op_array->opcodes);
		opline = op_array->opcodes;
		UNSERIALIZE_PTR(opline);
		end = opline + op_array->last;
		while (opline < end) {
#if ZEND_USE_ABS_CONST_ADDR
			if (opline->op1_type == IS_CONST) {
				SERIALIZE_PTR(opline->op1.zv);
			}
			if (opline->op2_type == IS_CONST) {
				SERIALIZE_PTR(opline->op2.zv);
			}
#else
			if (opline->op1_type == IS_CONST) {
				opline->op1.constant = RT_CONSTANT(opline, opline->op1) - literals;
			}
			if (opline->op2_type == IS_CONST) {
				opline->op2.constant = RT_CONSTANT(opline, opline->op2) - literals;
			}
#endif
#if ZEND_USE_ABS_JMP_ADDR
			switch (opline->opcode) {
				case ZEND_JMP:
				case ZEND_FAST_CALL:
					SERIALIZE_PTR(opline->op1.jmp_addr);
					break;
				case ZEND_JMPZ:
				case ZEND_JMPNZ:
				case ZEND_JMPZ_EX:
				case ZEND_JMPNZ_EX:
				case ZEND_JMP_SET:
				case ZEND_COALESCE:
				case ZEND_FE_RESET_R:
				case ZEND_FE_RESET_RW:
				case ZEND_ASSERT_CHECK:
				case ZEND_JMP_NULL:
				case ZEND_BIND_INIT_STATIC_OR_JMP:
					SERIALIZE_PTR(opline->op2.jmp_addr);
					break;
				case ZEND_CATCH:
					if (!(opline->extended_value & ZEND_LAST_CATCH)) {
						SERIALIZE_PTR(opline->op2.jmp_addr);
					}
					break;
				case ZEND_FE_FETCH_R:
				case ZEND_FE_FETCH_RW:
				case ZEND_SWITCH_LONG:
				case ZEND_SWITCH_STRING:
				case ZEND_MATCH:
					/* relative extended_value don't have to be changed */
					break;
			}
#endif
			zend_serialize_opcode_handler(opline);
			opline++;
		}

		if (op_array->arg_info) {
			zend_arg_info *p, *end;
			SERIALIZE_PTR(op_array->arg_info);
			p = op_array->arg_info;
			UNSERIALIZE_PTR(p);
			end = p + op_array->num_args;
			if (op_array->fn_flags & ZEND_ACC_HAS_RETURN_TYPE) {
				p--;
			}
			if (op_array->fn_flags & ZEND_ACC_VARIADIC) {
				end++;
			}
			while (p < end) {
				if (!IS_SERIALIZED(p->name)) {
					SERIALIZE_STR(p->name);
				}
				zend_file_cache_serialize_type(&p->type, script, info, buf);
				p++;
			}
		}

		if (op_array->vars) {
			zend_string **p, **end;

			SERIALIZE_PTR(op_array->vars);
			p = op_array->vars;
			UNSERIALIZE_PTR(p);
			end = p + op_array->last_var;
			while (p < end) {
				if (!IS_SERIALIZED(*p)) {
					SERIALIZE_STR(*p);
				}
				p++;
			}
		}

		if (op_array->num_dynamic_func_defs) {
			zend_op_array **defs;
			SERIALIZE_PTR(op_array->dynamic_func_defs);
			defs = op_array->dynamic_func_defs;
			UNSERIALIZE_PTR(defs);
			for (uint32_t i = 0; i < op_array->num_dynamic_func_defs; i++) {
				zend_op_array *def;
				SERIALIZE_PTR(defs[i]);
				def = defs[i];
				UNSERIALIZE_PTR(def);
				zend_file_cache_serialize_op_array(def, script, info, buf);
			}
		}

		SERIALIZE_STR(op_array->function_name);
		SERIALIZE_STR(op_array->filename);
		SERIALIZE_PTR(op_array->live_range);
		SERIALIZE_PTR(op_array->scope);
		SERIALIZE_STR(op_array->doc_comment);
		SERIALIZE_ATTRIBUTES(op_array->attributes);
		SERIALIZE_PTR(op_array->try_catch_array);
		SERIALIZE_PTR(op_array->prototype);
	}
}

static void zend_file_cache_serialize_func(zval                     *zv,
                                           zend_persistent_script   *script,
                                           zend_file_cache_metainfo *info,
                                           void                     *buf)
{
	zend_function *func;
	SERIALIZE_PTR(Z_PTR_P(zv));
	func = Z_PTR_P(zv);
	UNSERIALIZE_PTR(func);
	ZEND_ASSERT(func->type == ZEND_USER_FUNCTION);
	zend_file_cache_serialize_op_array(&func->op_array, script, info, buf);
}

static void zend_file_cache_serialize_prop_info(zval                     *zv,
                                                zend_persistent_script   *script,
                                                zend_file_cache_metainfo *info,
                                                void                     *buf)
{
	if (!IS_SERIALIZED(Z_PTR_P(zv))) {
		zend_property_info *prop;

		SERIALIZE_PTR(Z_PTR_P(zv));
		prop = Z_PTR_P(zv);
		UNSERIALIZE_PTR(prop);

		ZEND_ASSERT(prop->ce != NULL && prop->name != NULL);
		if (!IS_SERIALIZED(prop->ce)) {
			SERIALIZE_PTR(prop->ce);
			SERIALIZE_STR(prop->name);
			if (prop->doc_comment) {
				SERIALIZE_STR(prop->doc_comment);
			}
			SERIALIZE_ATTRIBUTES(prop->attributes);
			zend_file_cache_serialize_type(&prop->type, script, info, buf);
		}
	}
}

static void zend_file_cache_serialize_class_constant(zval                     *zv,
                                                     zend_persistent_script   *script,
                                                     zend_file_cache_metainfo *info,
                                                     void                     *buf)
{
	if (!IS_SERIALIZED(Z_PTR_P(zv))) {
		zend_class_constant *c;

		SERIALIZE_PTR(Z_PTR_P(zv));
		c = Z_PTR_P(zv);
		UNSERIALIZE_PTR(c);

		ZEND_ASSERT(c->ce != NULL);
		if (!IS_SERIALIZED(c->ce)) {
			SERIALIZE_PTR(c->ce);

			zend_file_cache_serialize_zval(&c->value, script, info, buf);
			if (c->doc_comment) {
				SERIALIZE_STR(c->doc_comment);
			}

			SERIALIZE_ATTRIBUTES(c->attributes);
			zend_file_cache_serialize_type(&c->type, script, info, buf);
		}
	}
}

static void zend_file_cache_serialize_class(zval                     *zv,
                                            zend_persistent_script   *script,
                                            zend_file_cache_metainfo *info,
                                            void                     *buf)
{
	zend_class_entry *ce;

	SERIALIZE_PTR(Z_PTR_P(zv));
	ce = Z_PTR_P(zv);
	UNSERIALIZE_PTR(ce);

	SERIALIZE_STR(ce->name);
	if (ce->parent) {
		if (!(ce->ce_flags & ZEND_ACC_LINKED)) {
			SERIALIZE_STR(ce->parent_name);
		} else {
			SERIALIZE_PTR(ce->parent);
		}
	}
	zend_file_cache_serialize_hash(&ce->function_table, script, info, buf, zend_file_cache_serialize_func);
	if (ce->default_properties_table) {
		zval *p, *end;

		SERIALIZE_PTR(ce->default_properties_table);
		p = ce->default_properties_table;
		UNSERIALIZE_PTR(p);
		end = p + ce->default_properties_count;
		while (p < end) {
			zend_file_cache_serialize_zval(p, script, info, buf);
			p++;
		}
	}
	if (ce->default_static_members_table) {
		zval *p, *end;

		SERIALIZE_PTR(ce->default_static_members_table);
		p = ce->default_static_members_table;
		UNSERIALIZE_PTR(p);

		end = p + ce->default_static_members_count;
		while (p < end) {
			zend_file_cache_serialize_zval(p, script, info, buf);
			p++;
		}
	}
	zend_file_cache_serialize_hash(&ce->constants_table, script, info, buf, zend_file_cache_serialize_class_constant);
	SERIALIZE_STR(ce->info.user.filename);
	SERIALIZE_STR(ce->info.user.doc_comment);
	SERIALIZE_ATTRIBUTES(ce->attributes);
	zend_file_cache_serialize_hash(&ce->properties_info, script, info, buf, zend_file_cache_serialize_prop_info);

	if (ce->properties_info_table) {
		uint32_t i;
		zend_property_info **table;

		SERIALIZE_PTR(ce->properties_info_table);
		table = ce->properties_info_table;
		UNSERIALIZE_PTR(table);

		for (i = 0; i < ce->default_properties_count; i++) {
			SERIALIZE_PTR(table[i]);
		}
	}

	if (ce->num_interfaces) {
		uint32_t i;
		zend_class_name *interface_names;

		ZEND_ASSERT(!(ce->ce_flags & ZEND_ACC_LINKED));

		SERIALIZE_PTR(ce->interface_names);
		interface_names = ce->interface_names;
		UNSERIALIZE_PTR(interface_names);

		for (i = 0; i < ce->num_interfaces; i++) {
			SERIALIZE_STR(interface_names[i].name);
			SERIALIZE_STR(interface_names[i].lc_name);
		}
	}

	if (ce->num_traits) {
		uint32_t i;
		zend_class_name *trait_names;

		SERIALIZE_PTR(ce->trait_names);
		trait_names = ce->trait_names;
		UNSERIALIZE_PTR(trait_names);

		for (i = 0; i < ce->num_traits; i++) {
			SERIALIZE_STR(trait_names[i].name);
			SERIALIZE_STR(trait_names[i].lc_name);
		}

		if (ce->trait_aliases) {
			zend_trait_alias **p, *q;

			SERIALIZE_PTR(ce->trait_aliases);
			p = ce->trait_aliases;
			UNSERIALIZE_PTR(p);

			while (*p) {
				SERIALIZE_PTR(*p);
				q = *p;
				UNSERIALIZE_PTR(q);

				if (q->trait_method.method_name) {
					SERIALIZE_STR(q->trait_method.method_name);
				}
				if (q->trait_method.class_name) {
					SERIALIZE_STR(q->trait_method.class_name);
				}

				if (q->alias) {
					SERIALIZE_STR(q->alias);
				}
				p++;
			}
		}

		if (ce->trait_precedences) {
			zend_trait_precedence **p, *q;
			uint32_t j;

			SERIALIZE_PTR(ce->trait_precedences);
			p = ce->trait_precedences;
			UNSERIALIZE_PTR(p);

			while (*p) {
				SERIALIZE_PTR(*p);
				q = *p;
				UNSERIALIZE_PTR(q);

				if (q->trait_method.method_name) {
					SERIALIZE_STR(q->trait_method.method_name);
				}
				if (q->trait_method.class_name) {
					SERIALIZE_STR(q->trait_method.class_name);
				}

				for (j = 0; j < q->num_excludes; j++) {
					SERIALIZE_STR(q->exclude_class_names[j]);
				}
				p++;
			}
		}
	}

	SERIALIZE_PTR(ce->constructor);
	SERIALIZE_PTR(ce->destructor);
	SERIALIZE_PTR(ce->clone);
	SERIALIZE_PTR(ce->__get);
	SERIALIZE_PTR(ce->__set);
	SERIALIZE_PTR(ce->__call);
	SERIALIZE_PTR(ce->__serialize);
	SERIALIZE_PTR(ce->__unserialize);
	SERIALIZE_PTR(ce->__isset);
	SERIALIZE_PTR(ce->__unset);
	SERIALIZE_PTR(ce->__tostring);
	SERIALIZE_PTR(ce->__callstatic);
	SERIALIZE_PTR(ce->__debugInfo);

	if (ce->iterator_funcs_ptr) {
		SERIALIZE_PTR(ce->iterator_funcs_ptr->zf_new_iterator);
		SERIALIZE_PTR(ce->iterator_funcs_ptr->zf_rewind);
		SERIALIZE_PTR(ce->iterator_funcs_ptr->zf_valid);
		SERIALIZE_PTR(ce->iterator_funcs_ptr->zf_key);
		SERIALIZE_PTR(ce->iterator_funcs_ptr->zf_current);
		SERIALIZE_PTR(ce->iterator_funcs_ptr->zf_next);
		SERIALIZE_PTR(ce->iterator_funcs_ptr);
	}

	if (ce->arrayaccess_funcs_ptr) {
		SERIALIZE_PTR(ce->arrayaccess_funcs_ptr->zf_offsetget);
		SERIALIZE_PTR(ce->arrayaccess_funcs_ptr->zf_offsetexists);
		SERIALIZE_PTR(ce->arrayaccess_funcs_ptr->zf_offsetset);
		SERIALIZE_PTR(ce->arrayaccess_funcs_ptr->zf_offsetunset);
		SERIALIZE_PTR(ce->arrayaccess_funcs_ptr);
	}

	ZEND_MAP_PTR_INIT(ce->static_members_table, NULL);
	ZEND_MAP_PTR_INIT(ce->mutable_data, NULL);

	ce->inheritance_cache = NULL;
}

static void zend_file_cache_serialize_warnings(
		zend_persistent_script *script, zend_file_cache_metainfo *info, void *buf)
{
	if (script->warnings) {
		zend_error_info **warnings;
		SERIALIZE_PTR(script->warnings);
		warnings = script->warnings;
		UNSERIALIZE_PTR(warnings);

		for (uint32_t i = 0; i < script->num_warnings; i++) {
			zend_error_info *warning;
			SERIALIZE_PTR(warnings[i]);
			warning = warnings[i];
			UNSERIALIZE_PTR(warning);
			SERIALIZE_STR(warning->filename);
			SERIALIZE_STR(warning->message);
		}
	}
}

static void zend_file_cache_serialize_early_bindings(
		zend_persistent_script *script, zend_file_cache_metainfo *info, void *buf)
{
	if (script->early_bindings) {
		SERIALIZE_PTR(script->early_bindings);
		zend_early_binding *early_bindings = script->early_bindings;
		UNSERIALIZE_PTR(early_bindings);
		for (uint32_t i = 0; i < script->num_early_bindings; i++) {
			SERIALIZE_STR(early_bindings[i].lcname);
			SERIALIZE_STR(early_bindings[i].rtd_key);
			SERIALIZE_STR(early_bindings[i].lc_parent_name);
		}
	}
}

static void zend_file_cache_serialize(zend_persistent_script   *script,
                                      zend_file_cache_metainfo *info,
                                      void                     *buf)
{
	zend_persistent_script *new_script;

	memcpy(info->magic, "OPCACHE", 8);
	memcpy(info->system_id, zend_system_id, 32);
	info->mem_size = script->size;
	info->str_size = 0;
	info->script_offset = (char*)script - (char*)script->mem;
	info->timestamp = script->timestamp;

	memcpy(buf, script->mem, script->size);

	new_script = (zend_persistent_script*)((char*)buf + info->script_offset);
	SERIALIZE_STR(new_script->script.filename);

	zend_file_cache_serialize_hash(&new_script->script.class_table, script, info, buf, zend_file_cache_serialize_class);
	zend_file_cache_serialize_hash(&new_script->script.function_table, script, info, buf, zend_file_cache_serialize_func);
	zend_file_cache_serialize_op_array(&new_script->script.main_op_array, script, info, buf);
	zend_file_cache_serialize_warnings(new_script, info, buf);
	zend_file_cache_serialize_early_bindings(new_script, info, buf);

	new_script->mem = NULL;
}

static char *zend_file_cache_get_bin_file_path(zend_string *script_path)
{
	size_t len;
	char *filename;

#ifndef ZEND_WIN32
	len = strlen(ZCG(accel_directives).file_cache);
	filename = emalloc(len + 33 + ZSTR_LEN(script_path) + sizeof(SUFFIX));
	memcpy(filename, ZCG(accel_directives).file_cache, len);
	filename[len] = '/';
	memcpy(filename + len + 1, zend_system_id, 32);
	memcpy(filename + len + 33, ZSTR_VAL(script_path), ZSTR_LEN(script_path));
	memcpy(filename + len + 33 + ZSTR_LEN(script_path), SUFFIX, sizeof(SUFFIX));
#else
	len = strlen(ZCG(accel_directives).file_cache);

	filename = emalloc(len + 33 + 33 + ZSTR_LEN(script_path) + sizeof(SUFFIX));

	memcpy(filename, ZCG(accel_directives).file_cache, len);
	filename[len] = '\\';
	memcpy(filename + 1 + len, accel_uname_id, 32);
	len += 1 + 32;
	filename[len] = '\\';

	memcpy(filename + len + 1, zend_system_id, 32);

	if (ZSTR_LEN(script_path) >= 7 && ':' == ZSTR_VAL(script_path)[4] && '/' == ZSTR_VAL(script_path)[5]  && '/' == ZSTR_VAL(script_path)[6]) {
		/* phar:// or file:// */
		*(filename + len + 33) = '\\';
		memcpy(filename + len + 34, ZSTR_VAL(script_path), 4);
		if (ZSTR_LEN(script_path) - 7 >= 2 && ':' == ZSTR_VAL(script_path)[8]) {
			*(filename + len + 38) = '\\';
			*(filename + len + 39) = ZSTR_VAL(script_path)[7];
			memcpy(filename + len + 40, ZSTR_VAL(script_path) + 9, ZSTR_LEN(script_path) - 9);
			memcpy(filename + len + 40 + ZSTR_LEN(script_path) - 9, SUFFIX, sizeof(SUFFIX));
		} else {
			memcpy(filename + len + 38, ZSTR_VAL(script_path) + 7, ZSTR_LEN(script_path) - 7);
			memcpy(filename + len + 38 + ZSTR_LEN(script_path) - 7, SUFFIX, sizeof(SUFFIX));
		}
	} else if (ZSTR_LEN(script_path) >= 2 && ':' == ZSTR_VAL(script_path)[1]) {
		/* local fs */
		*(filename + len + 33) = '\\';
		*(filename + len + 34) = ZSTR_VAL(script_path)[0];
		memcpy(filename + len + 35, ZSTR_VAL(script_path) + 2, ZSTR_LEN(script_path) - 2);
		memcpy(filename + len + 35 + ZSTR_LEN(script_path) - 2, SUFFIX, sizeof(SUFFIX));
	} else {
		/* network path */
		memcpy(filename + len + 33, ZSTR_VAL(script_path), ZSTR_LEN(script_path));
		memcpy(filename + len + 33 + ZSTR_LEN(script_path), SUFFIX, sizeof(SUFFIX));
	}
#endif

	return filename;
}

/**
 * Helper function for zend_file_cache_script_store().
 *
 * @return true on success, false on error and errno is set to indicate the cause of the error
 */
static bool zend_file_cache_script_write(int fd, const zend_persistent_script *script, const zend_file_cache_metainfo *info, const void *buf, const zend_string *s)
{
	ssize_t written;
	const ssize_t total_size = (ssize_t)(sizeof(*info) + script->size + info->str_size);

#ifdef HAVE_SYS_UIO_H
	const struct iovec vec[] = {
		{ .iov_base = (void *)info, .iov_len = sizeof(*info) },
		{ .iov_base = (void *)buf, .iov_len = script->size },
		{ .iov_base = (void *)ZSTR_VAL(s), .iov_len = info->str_size },
	};

	written = writev(fd, vec, sizeof(vec) / sizeof(vec[0]));
	if (EXPECTED(written == total_size)) {
		return true;
	}

	errno = written == -1 ? errno : EAGAIN;
	return false;
#else
	if (UNEXPECTED(ZEND_LONG_MAX < (zend_long)total_size)) {
# ifdef EFBIG
		errno = EFBIG;
# else
		errno = ERANGE;
# endif
		return false;
	}

	written = write(fd, info, sizeof(*info));
	if (UNEXPECTED(written != sizeof(*info))) {
		errno = written == -1 ? errno : EAGAIN;
		return false;
	}

	written = write(fd, buf, script->size);
	if (UNEXPECTED(written != script->size)) {
		errno = written == -1 ? errno : EAGAIN;
		return false;
	}

	written = write(fd, ZSTR_VAL(s), info->str_size);
	if (UNEXPECTED(written != info->str_size)) {
		errno = written == -1 ? errno : EAGAIN;
		return false;
	}

	return true;
#endif
}

int zend_file_cache_script_store(zend_persistent_script *script, bool in_shm)
{
	int fd;
	char *filename;
	zend_file_cache_metainfo info;
	void *mem, *buf;

#ifdef HAVE_JIT
	/* FIXME: dump jited codes out to file cache? */
	if (JIT_G(on)) {
		return FAILURE;
	}
#endif

	filename = zend_file_cache_get_bin_file_path(script->script.filename);

	if (zend_file_cache_mkdir(filename, strlen(ZCG(accel_directives).file_cache)) != SUCCESS) {
		zend_accel_error(ACCEL_LOG_WARNING, "opcache cannot create directory for file '%s', %s\n", filename, strerror(errno));
		efree(filename);
		return FAILURE;
	}

	fd = zend_file_cache_open(filename, O_CREAT | O_EXCL | O_RDWR | O_BINARY, S_IRUSR | S_IWUSR);
	if (fd < 0) {
		if (errno != EEXIST) {
			zend_accel_error(ACCEL_LOG_WARNING, "opcache cannot create file '%s', %s\n", filename, strerror(errno));
		}
		efree(filename);
		return FAILURE;
	}

	if (zend_file_cache_flock(fd, LOCK_EX) != 0) {
		close(fd);
		efree(filename);
		return FAILURE;
	}

#if defined(__AVX__) || defined(__SSE2__)
	/* Align to 64-byte boundary */
	mem = emalloc(script->size + 64);
	buf = (void*)(((uintptr_t)mem + 63L) & ~63L);
#else
	mem = buf = emalloc(script->size);
#endif

	ZCG(mem) = zend_string_alloc(4096 - (_ZSTR_HEADER_SIZE + 1), 0);

	zend_shared_alloc_init_xlat_table();
	if (!in_shm) {
		script->corrupted = true; /* used to check if script restored to SHM or process memory */
	}
	zend_file_cache_serialize(script, &info, buf);
	if (!in_shm) {
		script->corrupted = false;
	}
	zend_shared_alloc_destroy_xlat_table();

	zend_string *const s = (zend_string*)ZCG(mem);

#if __has_feature(memory_sanitizer)
	/* The buffer may contain uninitialized regions. However, the uninitialized parts will not be
	 * used when reading the cache. We should probably still try to get things fully initialized
	 * for reproducibility, but for now ignore this issue. */
	__msan_unpoison(&info, sizeof(info));
	__msan_unpoison(buf, script->size);
#endif

	info.checksum = zend_adler32(ADLER32_INIT, buf, script->size);
	info.checksum = zend_adler32(info.checksum, (unsigned char*)ZSTR_VAL(s), info.str_size);

	if (!zend_file_cache_script_write(fd, script, &info, buf, s)) {
		zend_accel_error(ACCEL_LOG_WARNING, "opcache cannot write to file '%s': %s\n", filename, strerror(errno));
		zend_string_release_ex(s, 0);
		close(fd);
		efree(mem);
		zend_file_cache_unlink(filename);
		efree(filename);
		return FAILURE;
	}

	zend_string_release_ex(s, 0);
	efree(mem);
	if (zend_file_cache_flock(fd, LOCK_UN) != 0) {
		zend_accel_error(ACCEL_LOG_WARNING, "opcache cannot unlock file '%s': %s\n", filename, strerror(errno));
	}
	close(fd);
	efree(filename);

	return SUCCESS;
}

static void zend_file_cache_unserialize_hash(HashTable               *ht,
                                             zend_persistent_script  *script,
                                             void                    *buf,
                                             unserialize_callback_t   func,
                                             dtor_func_t              dtor)
{
	ht->pDestructor = dtor;
	if (HT_FLAGS(ht) & HASH_FLAG_UNINITIALIZED) {
		if (EXPECTED(!file_cache_only)) {
			HT_SET_DATA_ADDR(ht, &ZCSG(uninitialized_bucket));
		} else {
			HT_SET_DATA_ADDR(ht, &uninitialized_bucket);
		}
		return;
	}
	if (IS_UNSERIALIZED(ht->arData)) {
		return;
	}
	UNSERIALIZE_PTR(ht->arData);
	if (HT_IS_PACKED(ht)) {
		zval *p, *end;

		p = ht->arPacked;
		end = p + ht->nNumUsed;
		while (p < end) {
			if (Z_TYPE_P(p) != IS_UNDEF) {
				func(p, script, buf);
			}
			p++;
		}
	} else {
		Bucket *p, *end;

		p = ht->arData;
		end = p + ht->nNumUsed;
		while (p < end) {
			if (Z_TYPE(p->val) != IS_UNDEF) {
				UNSERIALIZE_STR(p->key);
				func(&p->val, script, buf);
			}
			p++;
		}
	}
}

static void zend_file_cache_unserialize_ast(zend_ast                *ast,
                                            zend_persistent_script  *script,
                                            void                    *buf)
{
	uint32_t i;

	if (ast->kind == ZEND_AST_ZVAL || ast->kind == ZEND_AST_CONSTANT) {
		zend_file_cache_unserialize_zval(&((zend_ast_zval*)ast)->val, script, buf);
	} else if (zend_ast_is_list(ast)) {
		zend_ast_list *list = zend_ast_get_list(ast);
		for (i = 0; i < list->children; i++) {
			if (list->child[i] && !IS_UNSERIALIZED(list->child[i])) {
				UNSERIALIZE_PTR(list->child[i]);
				zend_file_cache_unserialize_ast(list->child[i], script, buf);
			}
		}
	} else {
		uint32_t children = zend_ast_get_num_children(ast);
		for (i = 0; i < children; i++) {
			if (ast->child[i] && !IS_UNSERIALIZED(ast->child[i])) {
				UNSERIALIZE_PTR(ast->child[i]);
				zend_file_cache_unserialize_ast(ast->child[i], script, buf);
			}
		}
	}
}

static void zend_file_cache_unserialize_zval(zval                    *zv,
                                             zend_persistent_script  *script,
                                             void                    *buf)
{
	switch (Z_TYPE_P(zv)) {
		case IS_STRING:
			/* We can't use !IS_UNSERIALIZED here, because that does not recognize unserialized
			 * interned strings in non-shm mode. */
			if (IS_SERIALIZED(Z_STR_P(zv)) || IS_SERIALIZED_INTERNED(Z_STR_P(zv))) {
				UNSERIALIZE_STR(Z_STR_P(zv));
			}
			break;
		case IS_ARRAY:
			if (!IS_UNSERIALIZED(Z_ARR_P(zv))) {
				HashTable *ht;

				UNSERIALIZE_PTR(Z_ARR_P(zv));
				ht = Z_ARR_P(zv);
				zend_file_cache_unserialize_hash(ht,
						script, buf, zend_file_cache_unserialize_zval, ZVAL_PTR_DTOR);
			}
			break;
		case IS_CONSTANT_AST:
			if (!IS_UNSERIALIZED(Z_AST_P(zv))) {
				UNSERIALIZE_PTR(Z_AST_P(zv));
				zend_file_cache_unserialize_ast(Z_ASTVAL_P(zv), script, buf);
			}
			break;
		case IS_INDIRECT:
			/* Used by static properties. */
			UNSERIALIZE_PTR(Z_INDIRECT_P(zv));
			break;
		default:
			ZEND_ASSERT(Z_TYPE_P(zv) < IS_STRING);
			break;
	}
}

static void zend_file_cache_unserialize_attribute(zval *zv, zend_persistent_script *script, void *buf)
{
	zend_attribute *attr;
	uint32_t i;

	UNSERIALIZE_PTR(Z_PTR_P(zv));
	attr = Z_PTR_P(zv);

	UNSERIALIZE_STR(attr->name);
	UNSERIALIZE_STR(attr->lcname);

	for (i = 0; i < attr->argc; i++) {
		UNSERIALIZE_STR(attr->args[i].name);
		zend_file_cache_unserialize_zval(&attr->args[i].value, script, buf);
	}
}

static void zend_file_cache_unserialize_type(
		zend_type *type, zend_class_entry *scope, zend_persistent_script *script, void *buf)
{
	if (ZEND_TYPE_HAS_LIST(*type)) {
		zend_type_list *list = ZEND_TYPE_LIST(*type);
		UNSERIALIZE_PTR(list);
		ZEND_TYPE_SET_PTR(*type, list);

		zend_type *list_type;
		ZEND_TYPE_LIST_FOREACH(list, list_type) {
			zend_file_cache_unserialize_type(list_type, scope, script, buf);
		} ZEND_TYPE_LIST_FOREACH_END();
	} else if (ZEND_TYPE_HAS_NAME(*type)) {
		zend_string *type_name = ZEND_TYPE_NAME(*type);
		UNSERIALIZE_STR(type_name);
		ZEND_TYPE_SET_PTR(*type, type_name);
		if (!script->corrupted) {
			zend_accel_get_class_name_map_ptr(type_name);
		} else {
			zend_alloc_ce_cache(type_name);
		}
	}
}

static void zend_file_cache_unserialize_op_array(zend_op_array           *op_array,
                                                 zend_persistent_script  *script,
                                                 void                    *buf)
{
	if (!script->corrupted) {
		if (op_array != &script->script.main_op_array) {
			op_array->fn_flags |= ZEND_ACC_IMMUTABLE;
			ZEND_MAP_PTR_NEW(op_array->run_time_cache);
		} else {
			ZEND_ASSERT(!(op_array->fn_flags & ZEND_ACC_IMMUTABLE));
			ZEND_MAP_PTR_INIT(op_array->run_time_cache, NULL);
		}
		if (op_array->static_variables) {
			ZEND_MAP_PTR_NEW(op_array->static_variables_ptr);
		}
	} else {
		op_array->fn_flags &= ~ZEND_ACC_IMMUTABLE;
		ZEND_MAP_PTR_INIT(op_array->static_variables_ptr, NULL);
		ZEND_MAP_PTR_INIT(op_array->run_time_cache, NULL);
	}

	/* Check whether this op_array has already been unserialized. */
	if (IS_UNSERIALIZED(op_array->opcodes)) {
		ZEND_ASSERT(op_array->scope && "Only method op_arrays should be shared");
		return;
	}

	if (op_array->refcount) {
		op_array->refcount = NULL;
		UNSERIALIZE_PTR(op_array->static_variables);
		UNSERIALIZE_PTR(op_array->literals);
		UNSERIALIZE_PTR(op_array->opcodes);
		UNSERIALIZE_PTR(op_array->arg_info);
		UNSERIALIZE_PTR(op_array->vars);
		UNSERIALIZE_STR(op_array->function_name);
		UNSERIALIZE_STR(op_array->filename);
		UNSERIALIZE_PTR(op_array->live_range);
		UNSERIALIZE_PTR(op_array->scope);
		UNSERIALIZE_STR(op_array->doc_comment);
		UNSERIALIZE_ATTRIBUTES(op_array->attributes);
		UNSERIALIZE_PTR(op_array->try_catch_array);
		UNSERIALIZE_PTR(op_array->prototype);
		return;
	}

	if (op_array->static_variables) {
		HashTable *ht;

		UNSERIALIZE_PTR(op_array->static_variables);
		ht = op_array->static_variables;
		zend_file_cache_unserialize_hash(ht,
				script, buf, zend_file_cache_unserialize_zval, ZVAL_PTR_DTOR);
	}

	if (op_array->literals) {
		zval *p, *end;

		UNSERIALIZE_PTR(op_array->literals);
		p = op_array->literals;
		end = p + op_array->last_literal;
		while (p < end) {
			zend_file_cache_unserialize_zval(p, script, buf);
			p++;
		}
	}

	{
		zend_op *opline, *end;

		UNSERIALIZE_PTR(op_array->opcodes);
		opline = op_array->opcodes;
		end = opline + op_array->last;
		while (opline < end) {
#if ZEND_USE_ABS_CONST_ADDR
			if (opline->op1_type == IS_CONST) {
				UNSERIALIZE_PTR(opline->op1.zv);
			}
			if (opline->op2_type == IS_CONST) {
				UNSERIALIZE_PTR(opline->op2.zv);
			}
#else
			if (opline->op1_type == IS_CONST) {
				ZEND_PASS_TWO_UPDATE_CONSTANT(op_array, opline, opline->op1);
			}
			if (opline->op2_type == IS_CONST) {
				ZEND_PASS_TWO_UPDATE_CONSTANT(op_array, opline, opline->op2);
			}
#endif
#if ZEND_USE_ABS_JMP_ADDR
			switch (opline->opcode) {
				case ZEND_JMP:
				case ZEND_FAST_CALL:
					UNSERIALIZE_PTR(opline->op1.jmp_addr);
					break;
				case ZEND_JMPZ:
				case ZEND_JMPNZ:
				case ZEND_JMPZ_EX:
				case ZEND_JMPNZ_EX:
				case ZEND_JMP_SET:
				case ZEND_COALESCE:
				case ZEND_FE_RESET_R:
				case ZEND_FE_RESET_RW:
				case ZEND_ASSERT_CHECK:
				case ZEND_JMP_NULL:
				case ZEND_BIND_INIT_STATIC_OR_JMP:
					UNSERIALIZE_PTR(opline->op2.jmp_addr);
					break;
				case ZEND_CATCH:
					if (!(opline->extended_value & ZEND_LAST_CATCH)) {
						UNSERIALIZE_PTR(opline->op2.jmp_addr);
					}
					break;
				case ZEND_FE_FETCH_R:
				case ZEND_FE_FETCH_RW:
				case ZEND_SWITCH_LONG:
				case ZEND_SWITCH_STRING:
					/* relative extended_value don't have to be changed */
					break;
			}
#endif
			zend_deserialize_opcode_handler(opline);
			opline++;
		}

		UNSERIALIZE_PTR(op_array->scope);

		if (op_array->arg_info) {
			zend_arg_info *p, *end;
			UNSERIALIZE_PTR(op_array->arg_info);
			p = op_array->arg_info;
			end = p + op_array->num_args;
			if (op_array->fn_flags & ZEND_ACC_HAS_RETURN_TYPE) {
				p--;
			}
			if (op_array->fn_flags & ZEND_ACC_VARIADIC) {
				end++;
			}
			while (p < end) {
				if (!IS_UNSERIALIZED(p->name)) {
					UNSERIALIZE_STR(p->name);
				}
				zend_file_cache_unserialize_type(&p->type, (op_array->fn_flags & ZEND_ACC_CLOSURE) ? NULL : op_array->scope, script, buf);
				p++;
			}
		}

		if (op_array->vars) {
			zend_string **p, **end;

			UNSERIALIZE_PTR(op_array->vars);
			p = op_array->vars;
			end = p + op_array->last_var;
			while (p < end) {
				if (!IS_UNSERIALIZED(*p)) {
					UNSERIALIZE_STR(*p);
				}
				p++;
			}
		}

		if (op_array->num_dynamic_func_defs) {
			UNSERIALIZE_PTR(op_array->dynamic_func_defs);
			for (uint32_t i = 0; i < op_array->num_dynamic_func_defs; i++) {
				UNSERIALIZE_PTR(op_array->dynamic_func_defs[i]);
				zend_file_cache_unserialize_op_array(op_array->dynamic_func_defs[i], script, buf);
			}
		}

		UNSERIALIZE_STR(op_array->function_name);
		UNSERIALIZE_STR(op_array->filename);
		UNSERIALIZE_PTR(op_array->live_range);
		UNSERIALIZE_STR(op_array->doc_comment);
		UNSERIALIZE_ATTRIBUTES(op_array->attributes);
		UNSERIALIZE_PTR(op_array->try_catch_array);
		UNSERIALIZE_PTR(op_array->prototype);
	}
}

static void zend_file_cache_unserialize_func(zval                    *zv,
                                             zend_persistent_script  *script,
                                             void                    *buf)
{
	zend_function *func;
	UNSERIALIZE_PTR(Z_PTR_P(zv));
	func = Z_PTR_P(zv);
	ZEND_ASSERT(func->type == ZEND_USER_FUNCTION);
	zend_file_cache_unserialize_op_array(&func->op_array, script, buf);
}

static void zend_file_cache_unserialize_prop_info(zval                    *zv,
                                                  zend_persistent_script  *script,
                                                  void                    *buf)
{
	if (!IS_UNSERIALIZED(Z_PTR_P(zv))) {
		zend_property_info *prop;

		UNSERIALIZE_PTR(Z_PTR_P(zv));
		prop = Z_PTR_P(zv);

		ZEND_ASSERT(prop->ce != NULL && prop->name != NULL);
		if (!IS_UNSERIALIZED(prop->ce)) {
			UNSERIALIZE_PTR(prop->ce);
			UNSERIALIZE_STR(prop->name);
			if (prop->doc_comment) {
				UNSERIALIZE_STR(prop->doc_comment);
			}
			UNSERIALIZE_ATTRIBUTES(prop->attributes);
			zend_file_cache_unserialize_type(&prop->type, prop->ce, script, buf);
		}
	}
}

static void zend_file_cache_unserialize_class_constant(zval                    *zv,
                                                       zend_persistent_script  *script,
                                                       void                    *buf)
{
	if (!IS_UNSERIALIZED(Z_PTR_P(zv))) {
		zend_class_constant *c;

		UNSERIALIZE_PTR(Z_PTR_P(zv));
		c = Z_PTR_P(zv);

		ZEND_ASSERT(c->ce != NULL);
		if (!IS_UNSERIALIZED(c->ce)) {
			UNSERIALIZE_PTR(c->ce);

			zend_file_cache_unserialize_zval(&c->value, script, buf);

			if (c->doc_comment) {
				UNSERIALIZE_STR(c->doc_comment);
			}
			UNSERIALIZE_ATTRIBUTES(c->attributes);
			zend_file_cache_unserialize_type(&c->type, c->ce, script, buf);
		}
	}
}

static void zend_file_cache_unserialize_class(zval                    *zv,
                                              zend_persistent_script  *script,
                                              void                    *buf)
{
	zend_class_entry *ce;

	UNSERIALIZE_PTR(Z_PTR_P(zv));
	ce = Z_PTR_P(zv);

	UNSERIALIZE_STR(ce->name);
	if (!(ce->ce_flags & ZEND_ACC_ANON_CLASS)) {
		if (!script->corrupted) {
			zend_accel_get_class_name_map_ptr(ce->name);
		} else {
			zend_alloc_ce_cache(ce->name);
		}
	}
	if (ce->parent) {
		if (!(ce->ce_flags & ZEND_ACC_LINKED)) {
			UNSERIALIZE_STR(ce->parent_name);
		} else {
			UNSERIALIZE_PTR(ce->parent);
		}
	}
	zend_file_cache_unserialize_hash(&ce->function_table,
			script, buf, zend_file_cache_unserialize_func, ZEND_FUNCTION_DTOR);
	if (ce->default_properties_table) {
		zval *p, *end;

		UNSERIALIZE_PTR(ce->default_properties_table);
		p = ce->default_properties_table;
		end = p + ce->default_properties_count;
		while (p < end) {
			zend_file_cache_unserialize_zval(p, script, buf);
			p++;
		}
	}
	if (ce->default_static_members_table) {
		zval *p, *end;
		UNSERIALIZE_PTR(ce->default_static_members_table);
		p = ce->default_static_members_table;
		end = p + ce->default_static_members_count;
		while (p < end) {
			zend_file_cache_unserialize_zval(p, script, buf);
			p++;
		}
	}
	zend_file_cache_unserialize_hash(&ce->constants_table,
			script, buf, zend_file_cache_unserialize_class_constant, NULL);
	UNSERIALIZE_STR(ce->info.user.filename);
	UNSERIALIZE_STR(ce->info.user.doc_comment);
	UNSERIALIZE_ATTRIBUTES(ce->attributes);
	zend_file_cache_unserialize_hash(&ce->properties_info,
			script, buf, zend_file_cache_unserialize_prop_info, NULL);

	if (ce->properties_info_table) {
		uint32_t i;
		UNSERIALIZE_PTR(ce->properties_info_table);

		for (i = 0; i < ce->default_properties_count; i++) {
			UNSERIALIZE_PTR(ce->properties_info_table[i]);
		}
	}

	if (ce->num_interfaces) {
		uint32_t i;

		ZEND_ASSERT(!(ce->ce_flags & ZEND_ACC_LINKED));
		UNSERIALIZE_PTR(ce->interface_names);

		for (i = 0; i < ce->num_interfaces; i++) {
			UNSERIALIZE_STR(ce->interface_names[i].name);
			UNSERIALIZE_STR(ce->interface_names[i].lc_name);
		}
	}

	if (ce->num_traits) {
		uint32_t i;

		UNSERIALIZE_PTR(ce->trait_names);

		for (i = 0; i < ce->num_traits; i++) {
			UNSERIALIZE_STR(ce->trait_names[i].name);
			UNSERIALIZE_STR(ce->trait_names[i].lc_name);
		}

		if (ce->trait_aliases) {
			zend_trait_alias **p, *q;

			UNSERIALIZE_PTR(ce->trait_aliases);
			p = ce->trait_aliases;

			while (*p) {
				UNSERIALIZE_PTR(*p);
				q = *p;

				if (q->trait_method.method_name) {
					UNSERIALIZE_STR(q->trait_method.method_name);
				}
				if (q->trait_method.class_name) {
					UNSERIALIZE_STR(q->trait_method.class_name);
				}

				if (q->alias) {
					UNSERIALIZE_STR(q->alias);
				}
				p++;
			}
		}

		if (ce->trait_precedences) {
			zend_trait_precedence **p, *q;
			uint32_t j;

			UNSERIALIZE_PTR(ce->trait_precedences);
			p = ce->trait_precedences;

			while (*p) {
				UNSERIALIZE_PTR(*p);
				q = *p;

				if (q->trait_method.method_name) {
					UNSERIALIZE_STR(q->trait_method.method_name);
				}
				if (q->trait_method.class_name) {
					UNSERIALIZE_STR(q->trait_method.class_name);
				}

				for (j = 0; j < q->num_excludes; j++) {
					UNSERIALIZE_STR(q->exclude_class_names[j]);
				}
				p++;
			}
		}
	}

	UNSERIALIZE_PTR(ce->constructor);
	UNSERIALIZE_PTR(ce->destructor);
	UNSERIALIZE_PTR(ce->clone);
	UNSERIALIZE_PTR(ce->__get);
	UNSERIALIZE_PTR(ce->__set);
	UNSERIALIZE_PTR(ce->__call);
	UNSERIALIZE_PTR(ce->__serialize);
	UNSERIALIZE_PTR(ce->__unserialize);
	UNSERIALIZE_PTR(ce->__isset);
	UNSERIALIZE_PTR(ce->__unset);
	UNSERIALIZE_PTR(ce->__tostring);
	UNSERIALIZE_PTR(ce->__callstatic);
	UNSERIALIZE_PTR(ce->__debugInfo);

	if (ce->iterator_funcs_ptr) {
		UNSERIALIZE_PTR(ce->iterator_funcs_ptr);
		UNSERIALIZE_PTR(ce->iterator_funcs_ptr->zf_new_iterator);
		UNSERIALIZE_PTR(ce->iterator_funcs_ptr->zf_rewind);
		UNSERIALIZE_PTR(ce->iterator_funcs_ptr->zf_valid);
		UNSERIALIZE_PTR(ce->iterator_funcs_ptr->zf_key);
		UNSERIALIZE_PTR(ce->iterator_funcs_ptr->zf_current);
		UNSERIALIZE_PTR(ce->iterator_funcs_ptr->zf_next);
	}
	if (ce->arrayaccess_funcs_ptr) {
		UNSERIALIZE_PTR(ce->arrayaccess_funcs_ptr);
		UNSERIALIZE_PTR(ce->arrayaccess_funcs_ptr->zf_offsetget);
		UNSERIALIZE_PTR(ce->arrayaccess_funcs_ptr->zf_offsetexists);
		UNSERIALIZE_PTR(ce->arrayaccess_funcs_ptr->zf_offsetset);
		UNSERIALIZE_PTR(ce->arrayaccess_funcs_ptr->zf_offsetunset);
	}

	if (!(script->corrupted)) {
		ce->ce_flags |= ZEND_ACC_IMMUTABLE;
		ce->ce_flags &= ~ZEND_ACC_FILE_CACHED;
		ZEND_MAP_PTR_NEW(ce->mutable_data);
		if (ce->default_static_members_count) {
			ZEND_MAP_PTR_NEW(ce->static_members_table);
		}
	} else {
		ce->ce_flags &= ~ZEND_ACC_IMMUTABLE;
		ce->ce_flags |= ZEND_ACC_FILE_CACHED;
		ZEND_MAP_PTR_INIT(ce->mutable_data, NULL);
		ZEND_MAP_PTR_INIT(ce->static_members_table, NULL);
	}

	// Memory addresses of object handlers are not stable. They can change due to ASLR or order of linking dynamic. To
	// avoid pointing to invalid memory we relink default_object_handlers here.
	ce->default_object_handlers = ce->ce_flags & ZEND_ACC_ENUM ? &zend_enum_object_handlers : &std_object_handlers;
}

static void zend_file_cache_unserialize_warnings(zend_persistent_script *script, void *buf)
{
	if (script->warnings) {
		UNSERIALIZE_PTR(script->warnings);
		for (uint32_t i = 0; i < script->num_warnings; i++) {
			UNSERIALIZE_PTR(script->warnings[i]);
			UNSERIALIZE_STR(script->warnings[i]->filename);
			UNSERIALIZE_STR(script->warnings[i]->message);
		}
	}
}

static void zend_file_cache_unserialize_early_bindings(zend_persistent_script *script, void *buf)
{
	if (script->early_bindings) {
		UNSERIALIZE_PTR(script->early_bindings);
		for (uint32_t i = 0; i < script->num_early_bindings; i++) {
			UNSERIALIZE_STR(script->early_bindings[i].lcname);
			UNSERIALIZE_STR(script->early_bindings[i].rtd_key);
			UNSERIALIZE_STR(script->early_bindings[i].lc_parent_name);
		}
	}
}

static void zend_file_cache_unserialize(zend_persistent_script  *script,
                                        void                    *buf)
{
	script->mem = buf;

	UNSERIALIZE_STR(script->script.filename);

	zend_file_cache_unserialize_hash(&script->script.class_table,
			script, buf, zend_file_cache_unserialize_class, ZEND_CLASS_DTOR);
	zend_file_cache_unserialize_hash(&script->script.function_table,
			script, buf, zend_file_cache_unserialize_func, ZEND_FUNCTION_DTOR);
	zend_file_cache_unserialize_op_array(&script->script.main_op_array, script, buf);
	zend_file_cache_unserialize_warnings(script, buf);
	zend_file_cache_unserialize_early_bindings(script, buf);
}

zend_persistent_script *zend_file_cache_script_load(zend_file_handle *file_handle)
{
	zend_string *full_path = file_handle->opened_path;
	int fd;
	char *filename;
	zend_persistent_script *script;
	zend_file_cache_metainfo info;
	zend_accel_hash_entry *bucket;
	void *mem, *checkpoint, *buf;
	bool cache_it = true;
	unsigned int actual_checksum;
	bool ok;

	if (!full_path) {
		return NULL;
	}
	filename = zend_file_cache_get_bin_file_path(full_path);

	fd = zend_file_cache_open(filename, O_RDONLY | O_BINARY);
	if (fd < 0) {
		efree(filename);
		return NULL;
	}

	if (zend_file_cache_flock(fd, LOCK_SH) != 0) {
		close(fd);
		efree(filename);
		return NULL;
	}

	if (read(fd, &info, sizeof(info)) != sizeof(info)) {
		zend_accel_error(ACCEL_LOG_WARNING, "opcache cannot read from file '%s' (info)\n", filename);
		zend_file_cache_flock(fd, LOCK_UN);
		close(fd);
		zend_file_cache_unlink(filename);
		efree(filename);
		return NULL;
	}

	/* verify header */
	if (memcmp(info.magic, "OPCACHE", 8) != 0) {
		zend_accel_error(ACCEL_LOG_WARNING, "opcache cannot read from file '%s' (wrong header)\n", filename);
		zend_file_cache_flock(fd, LOCK_UN);
		close(fd);
		zend_file_cache_unlink(filename);
		efree(filename);
		return NULL;
	}
	if (memcmp(info.system_id, zend_system_id, 32) != 0) {
		zend_accel_error(ACCEL_LOG_WARNING, "opcache cannot read from file '%s' (wrong \"system_id\")\n", filename);
		zend_file_cache_flock(fd, LOCK_UN);
		close(fd);
		zend_file_cache_unlink(filename);
		efree(filename);
		return NULL;
	}

	/* verify timestamp */
	if (ZCG(accel_directives).validate_timestamps &&
	    zend_get_file_handle_timestamp(file_handle, NULL) != info.timestamp) {
		if (zend_file_cache_flock(fd, LOCK_UN) != 0) {
			zend_accel_error(ACCEL_LOG_WARNING, "opcache cannot unlock file '%s'\n", filename);
		}
		close(fd);
		zend_file_cache_unlink(filename);
		efree(filename);
		return NULL;
	}

	checkpoint = zend_arena_checkpoint(CG(arena));
#if defined(__AVX__) || defined(__SSE2__)
	/* Align to 64-byte boundary */
	mem = zend_arena_alloc(&CG(arena), info.mem_size + info.str_size + 64);
	mem = (void*)(((uintptr_t)mem + 63L) & ~63L);
#else
	mem = zend_arena_alloc(&CG(arena), info.mem_size + info.str_size);
#endif

	if (read(fd, mem, info.mem_size + info.str_size) != (ssize_t)(info.mem_size + info.str_size)) {
		zend_accel_error(ACCEL_LOG_WARNING, "opcache cannot read from file '%s' (mem)\n", filename);
		zend_file_cache_flock(fd, LOCK_UN);
		close(fd);
		zend_file_cache_unlink(filename);
		zend_arena_release(&CG(arena), checkpoint);
		efree(filename);
		return NULL;
	}
	if (zend_file_cache_flock(fd, LOCK_UN) != 0) {
		zend_accel_error(ACCEL_LOG_WARNING, "opcache cannot unlock file '%s'\n", filename);
	}
	close(fd);

	/* verify checksum */
	if (ZCG(accel_directives).file_cache_consistency_checks &&
	    (actual_checksum = zend_adler32(ADLER32_INIT, mem, info.mem_size + info.str_size)) != info.checksum) {
		zend_accel_error(ACCEL_LOG_WARNING, "corrupted file '%s' excepted checksum: 0x%08x actual checksum: 0x%08x\n", filename, info.checksum, actual_checksum);
		zend_file_cache_unlink(filename);
		zend_arena_release(&CG(arena), checkpoint);
		efree(filename);
		return NULL;
	}

	if (!file_cache_only &&
	    !ZCSG(restart_in_progress) &&
	    !ZCSG(restart_pending) &&
		!ZSMMG(memory_exhausted) &&
	    accelerator_shm_read_lock() == SUCCESS) {
		/* exclusive lock */
		zend_shared_alloc_lock();

		/* Check if we still need to put the file into the cache (may be it was
		 * already stored by another process. This final check is done under
		 * exclusive lock) */
		bucket = zend_accel_hash_find_entry(&ZCSG(hash), full_path);
		if (bucket) {
			script = (zend_persistent_script *)bucket->data;
			if (!script->corrupted) {
				zend_shared_alloc_unlock();
				zend_arena_release(&CG(arena), checkpoint);
				efree(filename);
				return script;
			}
		}

		if (zend_accel_hash_is_full(&ZCSG(hash))) {
			zend_accel_error(ACCEL_LOG_DEBUG, "No more entries in hash table!");
			ZSMMG(memory_exhausted) = 1;
			zend_accel_schedule_restart_if_necessary(ACCEL_RESTART_HASH);
			zend_shared_alloc_unlock();
			goto use_process_mem;
		}

		buf = zend_shared_alloc_aligned(info.mem_size);

		if (!buf) {
			zend_accel_schedule_restart_if_necessary(ACCEL_RESTART_OOM);
			zend_shared_alloc_unlock();
			goto use_process_mem;
		}
		memcpy(buf, mem, info.mem_size);
		zend_map_ptr_extend(ZCSG(map_ptr_last));
	} else {
use_process_mem:
		buf = mem;
		cache_it = false;
	}

	ZCG(mem) = ((char*)mem + info.mem_size);
	script = (zend_persistent_script*)((char*)buf + info.script_offset);
	script->corrupted = !cache_it; /* used to check if script restored to SHM or process memory */

	ok = true;
	zend_try {
		zend_file_cache_unserialize(script, buf);
	} zend_catch {
		ok = false;
	} zend_end_try();
	if (!ok) {
		if (cache_it) {
			zend_shared_alloc_unlock();
			goto use_process_mem;
		} else {
			zend_arena_release(&CG(arena), checkpoint);
			efree(filename);
			return NULL;
		}
	}

	script->corrupted = false;

	if (cache_it) {
		ZCSG(map_ptr_last) = CG(map_ptr_last);
		script->dynamic_members.last_used = ZCG(request_time);

		zend_accel_hash_update(&ZCSG(hash), script->script.filename, 0, script);

		zend_shared_alloc_unlock();
		zend_accel_error(ACCEL_LOG_INFO, "File cached script loaded into memory '%s'", ZSTR_VAL(script->script.filename));

		zend_arena_release(&CG(arena), checkpoint);
	}
	efree(filename);

	return script;
}

void zend_file_cache_invalidate(zend_string *full_path)
{
	char *filename;

	filename = zend_file_cache_get_bin_file_path(full_path);

	zend_file_cache_unlink(filename);
	efree(filename);
}
