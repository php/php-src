/*
   +----------------------------------------------------------------------+
   | Zend OPcache                                                         |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2016 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Dmitry Stogov <dmitry@zend.com>                             |
   +----------------------------------------------------------------------+
*/

#include "zend.h"
#include "zend_virtual_cwd.h"
#include "zend_compile.h"
#include "zend_vm.h"
#include "zend_interfaces.h"

#include "php.h"

#ifdef HAVE_OPCACHE_FILE_CACHE

#include "ZendAccelerator.h"
#include "zend_file_cache.h"
#include "zend_shared_alloc.h"
#include "zend_accelerator_util_funcs.h"
#include "zend_accelerator_hash.h"

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
#define IS_SERIALIZED(ptr) \
	((char*)(ptr) < (char*)script->size)
#define IS_UNSERIALIZED(ptr) \
	(((char*)(ptr) >= (char*)script->mem && (char*)(ptr) < (char*)script->mem + script->size) || \
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
					GC_FLAGS(ptr) |= IS_STR_INTERNED; \
					GC_FLAGS(ptr) &= ~IS_STR_PERMANENT; \
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
					GC_FLAGS(ptr) |= IS_STR_INTERNED | IS_STR_PERMANENT; \
				} else { \
					GC_FLAGS(ptr) |= IS_STR_INTERNED; \
					GC_FLAGS(ptr) &= ~IS_STR_PERMANENT; \
				} \
			} \
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
			if (mkdir(filename, S_IRWXU) < 0 && errno != EEXIST) {
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
	if (info->str_size + len > ZSTR_LEN((zend_string*)ZCG(mem))) {
		size_t new_len = info->str_size + len;
		ZCG(mem) = (void*)zend_string_realloc(
			(zend_string*)ZCG(mem),
			((_ZSTR_HEADER_SIZE + 1 + new_len + 4095) & ~0xfff) - (_ZSTR_HEADER_SIZE + 1),
			0);
	}
	memcpy(ZSTR_VAL((zend_string*)ZCG(mem)) + info->str_size, str, len);
	info->str_size += len;
	return ret;
}

static void *zend_file_cache_unserialize_interned(zend_string *str, int in_shm)
{
	zend_string *ret;

	str = (zend_string*)((char*)ZCG(mem) + ((size_t)(str) & ~Z_UL(1)));
	if (in_shm) {
		ret = accel_new_interned_string(str);
		if (ret == str) {
			/* String wasn't interned but we will use it as interned anyway */
			GC_FLAGS(ret) |= IS_STR_INTERNED | IS_STR_PERMANENT;
		}
	} else {
		ret = str;
		GC_FLAGS(ret) |= IS_STR_INTERNED;
		GC_FLAGS(ret) &= ~IS_STR_PERMANENT;
	}
	return ret;
}

static void zend_file_cache_serialize_hash(HashTable                *ht,
                                           zend_persistent_script   *script,
                                           zend_file_cache_metainfo *info,
                                           void                     *buf,
                                           serialize_callback_t      func)
{
	Bucket *p, *end;

	if (!(ht->u.flags & HASH_FLAG_INITIALIZED)) {
		ht->arData = NULL;
		return;
	}
	if (IS_SERIALIZED(ht->arData)) {
		return;
	}
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

static zend_ast *zend_file_cache_serialize_ast(zend_ast                 *ast,
                                               zend_persistent_script   *script,
                                               zend_file_cache_metainfo *info,
                                               void                     *buf)
{
	uint32_t i;
	zend_ast *ret;

	SERIALIZE_PTR(ast);
	ret = ast;
	UNSERIALIZE_PTR(ast);

	if (ast->kind == ZEND_AST_ZVAL) {
		zend_file_cache_serialize_zval(&((zend_ast_zval*)ast)->val, script, info, buf);
	} else if (zend_ast_is_list(ast)) {
		zend_ast_list *list = zend_ast_get_list(ast);
		for (i = 0; i < list->children; i++) {
			if (list->child[i]) {
				list->child[i] = zend_file_cache_serialize_ast(list->child[i], script, info, buf);
			}
		}
	} else {
		uint32_t children = zend_ast_get_num_children(ast);
		for (i = 0; i < children; i++) {
			if (ast->child[i]) {
				ast->child[i] = zend_file_cache_serialize_ast(ast->child[i], script, info, buf);
			}
		}
	}
	return ret;
}

static void zend_file_cache_serialize_zval(zval                     *zv,
                                           zend_persistent_script   *script,
                                           zend_file_cache_metainfo *info,
                                           void                     *buf)
{
	switch (Z_TYPE_P(zv)) {
		case IS_STRING:
		case IS_CONSTANT:
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
		case IS_REFERENCE:
			if (!IS_SERIALIZED(Z_REF_P(zv))) {
				zend_reference *ref;

				SERIALIZE_PTR(Z_REF_P(zv));
				ref = Z_REF_P(zv);
				UNSERIALIZE_PTR(ref);
				zend_file_cache_serialize_zval(&ref->val, script, info, buf);
			}
			break;
		case IS_CONSTANT_AST:
			if (!IS_SERIALIZED(Z_AST_P(zv))) {
				zend_ast_ref *ast;

				SERIALIZE_PTR(Z_AST_P(zv));
				ast = Z_AST_P(zv);
				UNSERIALIZE_PTR(ast);
				if (!IS_SERIALIZED(ast->ast)) {
					ast->ast = zend_file_cache_serialize_ast(ast->ast, script, info, buf);
				}
			}
			break;
	}
}

static void zend_file_cache_serialize_op_array(zend_op_array            *op_array,
                                               zend_persistent_script   *script,
                                               zend_file_cache_metainfo *info,
                                               void                     *buf)
{
	if (op_array->static_variables && !IS_SERIALIZED(op_array->static_variables)) {
		HashTable *ht;

		SERIALIZE_PTR(op_array->static_variables);
		ht = op_array->static_variables;
		UNSERIALIZE_PTR(ht);
		zend_file_cache_serialize_hash(ht, script, info, buf, zend_file_cache_serialize_zval);
	}

	if (op_array->literals && !IS_SERIALIZED(op_array->literals)) {
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

	if (!IS_SERIALIZED(op_array->opcodes)) {
#if ZEND_USE_ABS_CONST_ADDR || ZEND_USE_ABS_JMP_ADDR
		zend_op *opline, *end;

		SERIALIZE_PTR(op_array->opcodes);
		opline = op_array->opcodes;
		UNSERIALIZE_PTR(opline);
		end = opline + op_array->last;
		while (opline < end) {
# if ZEND_USE_ABS_CONST_ADDR
			if (ZEND_OP1_TYPE(opline) == IS_CONST) {
				SERIALIZE_PTR(opline->op1.zv);
			}
			if (ZEND_OP2_TYPE(opline) == IS_CONST) {
				SERIALIZE_PTR(opline->op2.zv);
			}
# endif
# if ZEND_USE_ABS_JMP_ADDR
			switch (opline->opcode) {
				case ZEND_JMP:
				case ZEND_FAST_CALL:
				case ZEND_DECLARE_ANON_CLASS:
				case ZEND_DECLARE_ANON_INHERITED_CLASS:
					SERIALIZE_PTR(opline->op1.jmp_addr);
					break;
				case ZEND_JMPZNZ:
					/* relative extended_value don't have to be changed */
					/* break omitted intentionally */
				case ZEND_JMPZ:
				case ZEND_JMPNZ:
				case ZEND_JMPZ_EX:
				case ZEND_JMPNZ_EX:
				case ZEND_JMP_SET:
				case ZEND_COALESCE:
				case ZEND_NEW:
				case ZEND_FE_RESET_R:
				case ZEND_FE_RESET_RW:
				case ZEND_ASSERT_CHECK:
					SERIALIZE_PTR(opline->op2.jmp_addr);
					break;
				case ZEND_FE_FETCH_R:
				case ZEND_FE_FETCH_RW:
					/* relative extended_value don't have to be changed */
					break;
			}
# endif
			opline++;
		}
#else
		SERIALIZE_PTR(op_array->opcodes);
#endif

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
				if (!IS_SERIALIZED(p->class_name)) {
					SERIALIZE_STR(p->class_name);
				}
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

		SERIALIZE_STR(op_array->function_name);
		SERIALIZE_STR(op_array->filename);
		SERIALIZE_PTR(op_array->brk_cont_array);
		SERIALIZE_PTR(op_array->scope);
		SERIALIZE_STR(op_array->doc_comment);
		SERIALIZE_PTR(op_array->try_catch_array);
		SERIALIZE_PTR(op_array->prototype);
	}
}

static void zend_file_cache_serialize_func(zval                     *zv,
                                           zend_persistent_script   *script,
                                           zend_file_cache_metainfo *info,
                                           void                     *buf)
{
	zend_op_array *op_array;

	SERIALIZE_PTR(Z_PTR_P(zv));
	op_array = Z_PTR_P(zv);
	UNSERIALIZE_PTR(op_array);
	zend_file_cache_serialize_op_array(op_array, script, info, buf);
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

		if (prop->ce && !IS_SERIALIZED(prop->ce)) {
			SERIALIZE_PTR(prop->ce);
		}
		if (prop->name && !IS_SERIALIZED(prop->name)) {
			SERIALIZE_STR(prop->name);
		}
		if (prop->doc_comment && !IS_SERIALIZED(prop->doc_comment)) {
			SERIALIZE_STR(prop->doc_comment);
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
	zend_file_cache_serialize_hash(&ce->constants_table, script, info, buf, zend_file_cache_serialize_zval);
	SERIALIZE_STR(ZEND_CE_FILENAME(ce));
	SERIALIZE_STR(ZEND_CE_DOC_COMMENT(ce));
	zend_file_cache_serialize_hash(&ce->properties_info, script, info, buf, zend_file_cache_serialize_prop_info);

	if (ce->trait_aliases) {
		zend_trait_alias **p, *q;

		SERIALIZE_PTR(ce->trait_aliases);
		p = ce->trait_aliases;
		UNSERIALIZE_PTR(p);

		while (*p) {
			SERIALIZE_PTR(*p);
			q = *p;
			UNSERIALIZE_PTR(q);

			if (q->trait_method) {
				zend_trait_method_reference *m;

				SERIALIZE_PTR(q->trait_method);
				m = q->trait_method;
				UNSERIALIZE_PTR(m);

				if (m->method_name) {
					SERIALIZE_STR(m->method_name);
				}
				if (m->class_name) {
					SERIALIZE_STR(m->class_name);
				}
			}

			if (q->alias) {
				SERIALIZE_STR(q->alias);
			}
			p++;
		}
	}

	if (ce->trait_precedences) {
		zend_trait_precedence **p, *q;

		SERIALIZE_PTR(ce->trait_precedences);
		p = ce->trait_precedences;
		UNSERIALIZE_PTR(p);

		while (*p) {
			SERIALIZE_PTR(*p);
			q = *p;
			UNSERIALIZE_PTR(q);

			if (q->trait_method) {
				zend_trait_method_reference *m;

				SERIALIZE_PTR(q->trait_method);
				m = q->trait_method;
				UNSERIALIZE_PTR(m);

				if (m->method_name) {
					SERIALIZE_STR(m->method_name);
				}
				if (m->class_name) {
					SERIALIZE_STR(m->class_name);
				}
			}

			if (q->exclude_from_classes) {
				zend_string **s;

				SERIALIZE_PTR(q->exclude_from_classes);
				s = (zend_string**)q->exclude_from_classes;
				UNSERIALIZE_PTR(s);

				while (*s) {
					SERIALIZE_STR(*s);
					s++;
				}
			}
			p++;
		}
	}

	SERIALIZE_PTR(ce->parent);
	SERIALIZE_PTR(ce->constructor);
	SERIALIZE_PTR(ce->destructor);
	SERIALIZE_PTR(ce->clone);
	SERIALIZE_PTR(ce->__get);
	SERIALIZE_PTR(ce->__set);
	SERIALIZE_PTR(ce->__call);
	SERIALIZE_PTR(ce->serialize_func);
	SERIALIZE_PTR(ce->unserialize_func);
	SERIALIZE_PTR(ce->__isset);
	SERIALIZE_PTR(ce->__unset);
	SERIALIZE_PTR(ce->__tostring);
	SERIALIZE_PTR(ce->__callstatic);
	SERIALIZE_PTR(ce->__debugInfo);
}

static void zend_file_cache_serialize(zend_persistent_script   *script,
                                      zend_file_cache_metainfo *info,
                                      void                     *buf)
{
	zend_persistent_script *new_script;

	memcpy(info->magic, "OPCACHE", 8);
	memcpy(info->system_id, ZCG(system_id), 32);
	info->mem_size = script->size;
	info->str_size = 0;
	info->script_offset = (char*)script - (char*)script->mem;
	info->timestamp = script->timestamp;

	memcpy(buf, script->mem, script->size);

	new_script = (zend_persistent_script*)((char*)buf + info->script_offset);
	SERIALIZE_STR(new_script->full_path);

	zend_file_cache_serialize_hash(&new_script->class_table, script, info, buf, zend_file_cache_serialize_class);
	zend_file_cache_serialize_hash(&new_script->function_table, script, info, buf, zend_file_cache_serialize_func);
	zend_file_cache_serialize_op_array(&new_script->main_op_array, script, info, buf);

	SERIALIZE_PTR(new_script->arena_mem);
	new_script->mem = NULL;
}

static char *zend_file_cache_get_bin_file_path(zend_string *script_path)
{
	size_t len;
	char *filename;

	len = strlen(ZCG(accel_directives).file_cache);
	filename = emalloc(len + 33 + ZSTR_LEN(script_path) + sizeof(SUFFIX));
	memcpy(filename, ZCG(accel_directives).file_cache, len);
#ifndef ZEND_WIN32
	filename[len] = '/';
	memcpy(filename + len + 1, ZCG(system_id), 32);
	memcpy(filename + len + 33, ZSTR_VAL(script_path), ZSTR_LEN(script_path));
	memcpy(filename + len + 33 + ZSTR_LEN(script_path), SUFFIX, sizeof(SUFFIX));
#else
	filename[len] = '\\';
	memcpy(filename + len + 1, ZCG(system_id), 32);
	if (ZSTR_LEN(script_path) >= 2 && ':' == ZSTR_VAL(script_path)[1]) {
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

int zend_file_cache_script_store(zend_persistent_script *script, int in_shm)
{
	int fd;
	char *filename;
	zend_file_cache_metainfo info;
#ifdef HAVE_SYS_UIO_H
	struct iovec vec[3];
#endif
	void *mem, *buf;

	filename = zend_file_cache_get_bin_file_path(script->full_path);

	if (zend_file_cache_mkdir(filename, strlen(ZCG(accel_directives).file_cache)) != SUCCESS) {
		zend_accel_error(ACCEL_LOG_WARNING, "opcache cannot create directory for file '%s'\n", filename);
		efree(filename);
		return FAILURE;
	}

#ifndef ZEND_WIN32
	fd = open(filename, O_CREAT | O_EXCL | O_RDWR | O_BINARY, S_IRUSR | S_IWUSR);
#else
	fd = open(filename, O_CREAT | O_EXCL | O_RDWR | O_BINARY, _S_IREAD | _S_IWRITE);
#endif
	if (fd < 0) {
		if (errno != EEXIST) {
			zend_accel_error(ACCEL_LOG_WARNING, "opcache cannot create file '%s'\n", filename);
		}
		efree(filename);
		return FAILURE;
	}

	if (zend_file_cache_flock(fd, LOCK_EX) != 0) {
		close(fd);
		efree(filename);
		return FAILURE;
	}

#ifdef __SSE2__
	/* Align to 64-byte boundary */
	mem = emalloc(script->size + 64);
	buf = (void*)(((zend_uintptr_t)mem + 63L) & ~63L);
#else
	mem = buf = emalloc(script->size);
#endif

	ZCG(mem) = zend_string_alloc(4096 - (_ZSTR_HEADER_SIZE + 1), 0);

	zend_shared_alloc_init_xlat_table();
	if (!in_shm) {
		script->corrupted = 1; /* used to check if script restored to SHM or process memory */
	}
	zend_file_cache_serialize(script, &info, buf);
	if (!in_shm) {
		script->corrupted = 0;
	}
	zend_shared_alloc_destroy_xlat_table();

	info.checksum = zend_adler32(ADLER32_INIT, buf, script->size);
	info.checksum = zend_adler32(info.checksum, (signed char*)ZSTR_VAL((zend_string*)ZCG(mem)), info.str_size);

#ifdef HAVE_SYS_UIO_H
	vec[0].iov_base = &info;
	vec[0].iov_len = sizeof(info);
	vec[1].iov_base = buf;
	vec[1].iov_len = script->size;
	vec[2].iov_base = ZSTR_VAL((zend_string*)ZCG(mem));
	vec[2].iov_len = info.str_size;

	if (writev(fd, vec, 3) != (ssize_t)(sizeof(info) + script->size + info.str_size)) {
		zend_accel_error(ACCEL_LOG_WARNING, "opcache cannot write to file '%s'\n", filename);
		zend_string_release((zend_string*)ZCG(mem));
		close(fd);
		efree(mem);
		unlink(filename);
		efree(filename);
		return FAILURE;
	}
#else
	if (ZEND_LONG_MAX < (zend_long)(sizeof(info) + script->size + info.str_size) ||
		write(fd, &info, sizeof(info)) != sizeof(info) ||
		write(fd, buf, script->size) != script->size ||
		write(fd, ((zend_string*)ZCG(mem))->val, info.str_size) != info.str_size
		) {
		zend_accel_error(ACCEL_LOG_WARNING, "opcache cannot write to file '%s'\n", filename);
		zend_string_release((zend_string*)ZCG(mem));
		close(fd);
		efree(mem);
		unlink(filename);
		efree(filename);
		return FAILURE;
	}
#endif

	zend_string_release((zend_string*)ZCG(mem));
	efree(mem);
	if (zend_file_cache_flock(fd, LOCK_UN) != 0) {
		zend_accel_error(ACCEL_LOG_WARNING, "opcache cannot unlock file '%s'\n", filename);
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
	Bucket *p, *end;

	ht->pDestructor = dtor;
	if (!(ht->u.flags & HASH_FLAG_INITIALIZED)) {
		HT_SET_DATA_ADDR(ht, &uninitialized_bucket);
		return;
	}
	if (IS_UNSERIALIZED(ht->arData)) {
		return;
	}
	UNSERIALIZE_PTR(ht->arData);
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

static zend_ast *zend_file_cache_unserialize_ast(zend_ast                *ast,
                                                 zend_persistent_script  *script,
                                                 void                    *buf)
{
	uint32_t i;

	UNSERIALIZE_PTR(ast);

	if (ast->kind == ZEND_AST_ZVAL) {
		zend_file_cache_unserialize_zval(&((zend_ast_zval*)ast)->val, script, buf);
	} else if (zend_ast_is_list(ast)) {
		zend_ast_list *list = zend_ast_get_list(ast);
		for (i = 0; i < list->children; i++) {
			if (list->child[i]) {
				list->child[i] = zend_file_cache_unserialize_ast(list->child[i], script, buf);
			}
		}
	} else {
		uint32_t children = zend_ast_get_num_children(ast);
		for (i = 0; i < children; i++) {
			if (ast->child[i]) {
				ast->child[i] = zend_file_cache_unserialize_ast(ast->child[i], script, buf);
			}
		}
	}
	return ast;
}

static void zend_file_cache_unserialize_zval(zval                    *zv,
                                             zend_persistent_script  *script,
                                             void                    *buf)
{
	switch (Z_TYPE_P(zv)) {
		case IS_STRING:
		case IS_CONSTANT:
			if (!IS_UNSERIALIZED(Z_STR_P(zv))) {
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
		case IS_REFERENCE:
			if (!IS_UNSERIALIZED(Z_REF_P(zv))) {
				zend_reference *ref;

				UNSERIALIZE_PTR(Z_REF_P(zv));
				ref = Z_REF_P(zv);
				zend_file_cache_unserialize_zval(&ref->val, script, buf);
			}
			break;
		case IS_CONSTANT_AST:
			if (!IS_UNSERIALIZED(Z_AST_P(zv))) {
				zend_ast_ref *ast;

				UNSERIALIZE_PTR(Z_AST_P(zv));
				ast = Z_AST_P(zv);
				if (!IS_UNSERIALIZED(ast->ast)) {
					ast->ast = zend_file_cache_unserialize_ast(ast->ast, script, buf);
				}
			}
			break;
	}
}

static void zend_file_cache_unserialize_op_array(zend_op_array           *op_array,
                                                 zend_persistent_script  *script,
                                                 void                    *buf)
{
	if (op_array->static_variables && !IS_UNSERIALIZED(op_array->static_variables)) {
		HashTable *ht;

		UNSERIALIZE_PTR(op_array->static_variables);
		ht = op_array->static_variables;
		zend_file_cache_unserialize_hash(ht,
				script, buf, zend_file_cache_unserialize_zval, ZVAL_PTR_DTOR);
	}

	if (op_array->literals && !IS_UNSERIALIZED(op_array->literals)) {
		zval *p, *end;

		UNSERIALIZE_PTR(op_array->literals);
		p = op_array->literals;
		end = p + op_array->last_literal;
		while (p < end) {
			zend_file_cache_unserialize_zval(p, script, buf);
			p++;
		}
	}

	if (!IS_UNSERIALIZED(op_array->opcodes)) {
		zend_op *opline, *end;

		UNSERIALIZE_PTR(op_array->opcodes);
		opline = op_array->opcodes;
		end = opline + op_array->last;
		while (opline < end) {
# if ZEND_USE_ABS_CONST_ADDR
			if (ZEND_OP1_TYPE(opline) == IS_CONST) {
				UNSERIALIZE_PTR(opline->op1.zv);
			}
			if (ZEND_OP2_TYPE(opline) == IS_CONST) {
				UNSERIALIZE_PTR(opline->op2.zv);
			}
# endif
# if ZEND_USE_ABS_JMP_ADDR
			switch (opline->opcode) {
				case ZEND_JMP:
				case ZEND_FAST_CALL:
				case ZEND_DECLARE_ANON_CLASS:
				case ZEND_DECLARE_ANON_INHERITED_CLASS:
					UNSERIALIZE_PTR(opline->op1.jmp_addr);
					break;
				case ZEND_JMPZNZ:
					/* relative extended_value don't have to be changed */
					/* break omitted intentionally */
				case ZEND_JMPZ:
				case ZEND_JMPNZ:
				case ZEND_JMPZ_EX:
				case ZEND_JMPNZ_EX:
				case ZEND_JMP_SET:
				case ZEND_COALESCE:
				case ZEND_NEW:
				case ZEND_FE_RESET_R:
				case ZEND_FE_RESET_RW:
				case ZEND_ASSERT_CHECK:
					UNSERIALIZE_PTR(opline->op2.jmp_addr);
					break;
				case ZEND_FE_FETCH_R:
				case ZEND_FE_FETCH_RW:
					/* relative extended_value don't have to be changed */
					break;
			}
# endif
			ZEND_VM_SET_OPCODE_HANDLER(opline);
			opline++;
		}

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
				if (!IS_UNSERIALIZED(p->class_name)) {
					UNSERIALIZE_STR(p->class_name);
				}
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

		UNSERIALIZE_STR(op_array->function_name);
		UNSERIALIZE_STR(op_array->filename);
		UNSERIALIZE_PTR(op_array->brk_cont_array);
		UNSERIALIZE_PTR(op_array->scope);
		UNSERIALIZE_STR(op_array->doc_comment);
		UNSERIALIZE_PTR(op_array->try_catch_array);
		UNSERIALIZE_PTR(op_array->prototype);
	}
}

static void zend_file_cache_unserialize_func(zval                    *zv,
                                             zend_persistent_script  *script,
                                             void                    *buf)
{
	zend_op_array *op_array;

	UNSERIALIZE_PTR(Z_PTR_P(zv));
	op_array = Z_PTR_P(zv);
	zend_file_cache_unserialize_op_array(op_array, script, buf);
}

static void zend_file_cache_unserialize_prop_info(zval                    *zv,
                                                  zend_persistent_script  *script,
                                                  void                    *buf)
{
	if (!IS_UNSERIALIZED(Z_PTR_P(zv))) {
		zend_property_info *prop;

		UNSERIALIZE_PTR(Z_PTR_P(zv));
		prop = Z_PTR_P(zv);

		if (prop->ce && !IS_UNSERIALIZED(prop->ce)) {
			UNSERIALIZE_PTR(prop->ce);
		}
		if (prop->name && !IS_UNSERIALIZED(prop->name)) {
			UNSERIALIZE_STR(prop->name);
		}
		if (prop->doc_comment && !IS_UNSERIALIZED(prop->doc_comment)) {
			UNSERIALIZE_STR(prop->doc_comment);
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
			script, buf, zend_file_cache_unserialize_zval, NULL);
	UNSERIALIZE_STR(ZEND_CE_FILENAME(ce));
	UNSERIALIZE_STR(ZEND_CE_DOC_COMMENT(ce));
	zend_file_cache_unserialize_hash(&ce->properties_info,
			script, buf, zend_file_cache_unserialize_prop_info, ZVAL_PTR_DTOR);

	if (ce->trait_aliases) {
		zend_trait_alias **p, *q;

		UNSERIALIZE_PTR(ce->trait_aliases);
		p = ce->trait_aliases;

		while (*p) {
			UNSERIALIZE_PTR(*p);
			q = *p;

			if (q->trait_method) {
				zend_trait_method_reference *m;

				UNSERIALIZE_PTR(q->trait_method);
				m = q->trait_method;

				if (m->method_name) {
					UNSERIALIZE_STR(m->method_name);
				}
				if (m->class_name) {
					UNSERIALIZE_STR(m->class_name);
				}
			}

			if (q->alias) {
				UNSERIALIZE_STR(q->alias);
			}
			p++;
		}
	}

	if (ce->trait_precedences) {
		zend_trait_precedence **p, *q;

		UNSERIALIZE_PTR(ce->trait_precedences);
		p = ce->trait_precedences;

		while (*p) {
			UNSERIALIZE_PTR(*p);
			q = *p;

			if (q->trait_method) {
				zend_trait_method_reference *m;

				UNSERIALIZE_PTR(q->trait_method);
				m = q->trait_method;

				if (m->method_name) {
					UNSERIALIZE_STR(m->method_name);
				}
				if (m->class_name) {
					UNSERIALIZE_STR(m->class_name);
				}
			}

			if (q->exclude_from_classes) {
				zend_string **s;

				UNSERIALIZE_PTR(q->exclude_from_classes);
				s = (zend_string**)q->exclude_from_classes;

				while (*s) {
					UNSERIALIZE_STR(*s);
					s++;
				}
			}
			p++;
		}
	}

	UNSERIALIZE_PTR(ce->parent);
	UNSERIALIZE_PTR(ce->constructor);
	UNSERIALIZE_PTR(ce->destructor);
	UNSERIALIZE_PTR(ce->clone);
	UNSERIALIZE_PTR(ce->__get);
	UNSERIALIZE_PTR(ce->__set);
	UNSERIALIZE_PTR(ce->__call);
	UNSERIALIZE_PTR(ce->serialize_func);
	UNSERIALIZE_PTR(ce->unserialize_func);
	UNSERIALIZE_PTR(ce->__isset);
	UNSERIALIZE_PTR(ce->__unset);
	UNSERIALIZE_PTR(ce->__tostring);
	UNSERIALIZE_PTR(ce->__callstatic);
	UNSERIALIZE_PTR(ce->__debugInfo);

	if (UNEXPECTED((ce->ce_flags & ZEND_ACC_ANON_CLASS))) {
		ce->serialize = zend_class_serialize_deny;
		ce->unserialize = zend_class_unserialize_deny;
	}
}

static void zend_file_cache_unserialize(zend_persistent_script  *script,
                                        void                    *buf)
{
	script->mem = buf;

	UNSERIALIZE_STR(script->full_path);

	zend_file_cache_unserialize_hash(&script->class_table,
			script, buf, zend_file_cache_unserialize_class, ZEND_CLASS_DTOR);
	zend_file_cache_unserialize_hash(&script->function_table,
			script, buf, zend_file_cache_unserialize_func, ZEND_FUNCTION_DTOR);
	zend_file_cache_unserialize_op_array(&script->main_op_array, script, buf);

	UNSERIALIZE_PTR(script->arena_mem);
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
	int cache_it = 1;

	if (!full_path) {
		return NULL;
	}
	filename = zend_file_cache_get_bin_file_path(full_path);

	fd = open(filename, O_RDONLY | O_BINARY);
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
		zend_accel_error(ACCEL_LOG_WARNING, "opcache cannot read from file '%s'\n", filename);
		zend_file_cache_flock(fd, LOCK_UN);
		close(fd);
		unlink(filename);
		efree(filename);
		return NULL;
	}

	/* verify header */
	if (memcmp(info.magic, "OPCACHE", 8) != 0) {
		zend_accel_error(ACCEL_LOG_WARNING, "opcache cannot read from file '%s' (wrong header)\n", filename);
		zend_file_cache_flock(fd, LOCK_UN);
		close(fd);
		unlink(filename);
		efree(filename);
		return NULL;
	}
	if (memcmp(info.system_id, ZCG(system_id), 32) != 0) {
		zend_accel_error(ACCEL_LOG_WARNING, "opcache cannot read from file '%s' (wrong \"system_id\")\n", filename);
		zend_file_cache_flock(fd, LOCK_UN);
		close(fd);
		unlink(filename);
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
		unlink(filename);
		efree(filename);
		return NULL;
	}

	checkpoint = zend_arena_checkpoint(CG(arena));
#ifdef __SSE2__
	/* Align to 64-byte boundary */
	mem = zend_arena_alloc(&CG(arena), info.mem_size + info.str_size + 64);
	mem = (void*)(((zend_uintptr_t)mem + 63L) & ~63L);
#else
	mem = zend_arena_alloc(&CG(arena), info.mem_size + info.str_size);
#endif

	if (read(fd, mem, info.mem_size + info.str_size) != (ssize_t)(info.mem_size + info.str_size)) {
		zend_accel_error(ACCEL_LOG_WARNING, "opcache cannot read from file '%s'\n", filename);
		zend_file_cache_flock(fd, LOCK_UN);
		close(fd);
		unlink(filename);
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
	    zend_adler32(ADLER32_INIT, mem, info.mem_size + info.str_size) != info.checksum) {
		zend_accel_error(ACCEL_LOG_WARNING, "corrupted file '%s'\n", filename);
		unlink(filename);
		zend_arena_release(&CG(arena), checkpoint);
		efree(filename);
		return NULL;
	}

	if (!ZCG(accel_directives).file_cache_only &&
	    !ZCSG(restart_in_progress) &&
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

#ifdef __SSE2__
		/* Align to 64-byte boundary */
		buf = zend_shared_alloc(info.mem_size + 64);
		buf = (void*)(((zend_uintptr_t)buf + 63L) & ~63L);
#else
		buf = zend_shared_alloc(info.mem_size);
#endif

		if (!buf) {
			zend_accel_schedule_restart_if_necessary(ACCEL_RESTART_OOM);
			zend_shared_alloc_unlock();
			goto use_process_mem;
		}
		memcpy(buf, mem, info.mem_size);
	} else {
use_process_mem:
		buf = mem;
		cache_it = 0;
	}

	ZCG(mem) = ((char*)mem + info.mem_size);
	script = (zend_persistent_script*)((char*)buf + info.script_offset);
	script->corrupted = !cache_it; /* used to check if script restored to SHM or process memory */
	zend_file_cache_unserialize(script, buf);
	script->corrupted = 0;

	if (cache_it) {
		script->dynamic_members.checksum = zend_accel_script_checksum(script);
		script->dynamic_members.last_used = ZCG(request_time);

		zend_accel_hash_update(&ZCSG(hash), ZSTR_VAL(script->full_path), ZSTR_LEN(script->full_path), 0, script);

		zend_shared_alloc_unlock();
		zend_arena_release(&CG(arena), checkpoint);
	}
	efree(filename);

	return script;
}

void zend_file_cache_invalidate(zend_string *full_path)
{
	char *filename;

	filename = zend_file_cache_get_bin_file_path(full_path);

	unlink(filename);
	efree(filename);
}

#endif /* HAVE_OPCACHE_FILE_CACHE */
