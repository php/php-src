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
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   |          Stanislav Malyshev <stas@zend.com>                          |
   |          Dmitry Stogov <dmitry@zend.com>                             |
   +----------------------------------------------------------------------+
*/

#include "zend_API.h"
#include "zend_constants.h"
#include "zend_accelerator_util_funcs.h"
#include "zend_persist.h"
#include "zend_shared_alloc.h"

#define ZEND_PROTECTED_REFCOUNT	(1<<30)

static zend_uint zend_accel_refcount = ZEND_PROTECTED_REFCOUNT;

#if SIZEOF_SIZE_T <= SIZEOF_LONG
/* If sizeof(void*) == sizeof(ulong) we can use zend_hash index functions */
# define accel_xlat_set(old, new)	zend_hash_index_update(&ZCG(bind_hash), (ulong)(zend_uintptr_t)(old), &(new), sizeof(void*), NULL)
# define accel_xlat_get(old, new)	zend_hash_index_find(&ZCG(bind_hash), (ulong)(zend_uintptr_t)(old), (void**)&(new))
#else
# define accel_xlat_set(old, new)	zend_hash_quick_add(&ZCG(bind_hash), (char*)&(old), sizeof(void*), (ulong)(zend_uintptr_t)(old), (void**)&(new), sizeof(void*), NULL)
# define accel_xlat_get(old, new)	zend_hash_quick_find(&ZCG(bind_hash), (char*)&(old), sizeof(void*), (ulong)(zend_uintptr_t)(old), (void**)&(new))
#endif

typedef int (*id_function_t)(void *, void *);
typedef void (*unique_copy_ctor_func_t)(void *pElement);

#if ZEND_EXTENSION_API_NO > PHP_5_3_X_API_NO
static const Bucket *uninitialized_bucket = NULL;
#endif

static int zend_prepare_function_for_execution(zend_op_array *op_array);
static void zend_hash_clone_zval(HashTable *ht, HashTable *source, int bind);

static void zend_accel_destroy_zend_function(zend_function *function)
{
	TSRMLS_FETCH();

	if (function->type == ZEND_USER_FUNCTION) {
		if (function->op_array.static_variables) {

			efree(function->op_array.static_variables);
			function->op_array.static_variables = NULL;
		}
	}

	destroy_zend_function(function TSRMLS_CC);
}

static void zend_accel_destroy_zend_class(zend_class_entry **pce)
{
	zend_class_entry *ce = *pce;

	ce->function_table.pDestructor = (dtor_func_t) zend_accel_destroy_zend_function;
	destroy_zend_class(pce);
}

zend_persistent_script* create_persistent_script(void)
{
	zend_persistent_script *persistent_script = (zend_persistent_script *) emalloc(sizeof(zend_persistent_script));
	memset(persistent_script, 0, sizeof(zend_persistent_script));

	zend_hash_init(&persistent_script->function_table, 100, NULL, (dtor_func_t) zend_accel_destroy_zend_function, 0);
	/* class_table is usually destroyed by free_persistent_script() that
	 * overrides destructor. ZEND_CLASS_DTOR may be used by standard
	 * PHP compiler
	 */
	zend_hash_init(&persistent_script->class_table, 10, NULL, ZEND_CLASS_DTOR, 0);

	return persistent_script;
}

static int compact_hash_table(HashTable *ht)
{
	uint i = 3;
	uint nSize;
	Bucket **t;

	if (!ht->nNumOfElements) {
		/* Empty tables don't allocate space for Buckets */
		return 1;
	}

	if (ht->nNumOfElements >= 0x80000000) {
		/* prevent overflow */
		nSize = 0x80000000;
	} else {
		while ((1U << i) < ht->nNumOfElements) {
			i++;
		}
		nSize = 1 << i;
	}

	if (nSize >= ht->nTableSize) {
		/* Keep the size */
		return 1;
	}

	t = (Bucket **)pemalloc(nSize * sizeof(Bucket *), ht->persistent);
	if (!t) {
		return 0;
	}

	pefree(ht->arBuckets, ht->persistent);

	ht->arBuckets = t;
	ht->nTableSize = nSize;
	ht->nTableMask = ht->nTableSize - 1;
	zend_hash_rehash(ht);
	
	return 1;
}

int compact_persistent_script(zend_persistent_script *persistent_script)
{
	return compact_hash_table(&persistent_script->function_table) &&
	       compact_hash_table(&persistent_script->class_table);
}

void free_persistent_script(zend_persistent_script *persistent_script, int destroy_elements)
{
	if (destroy_elements) {
		persistent_script->function_table.pDestructor = (dtor_func_t)zend_accel_destroy_zend_function;
		persistent_script->class_table.pDestructor = (dtor_func_t)zend_accel_destroy_zend_class;
	} else {
		persistent_script->function_table.pDestructor = NULL;
		persistent_script->class_table.pDestructor = NULL;
	}

	zend_hash_destroy(&persistent_script->function_table);
	zend_hash_destroy(&persistent_script->class_table);

	if (persistent_script->full_path) {
		efree(persistent_script->full_path);
	}

	efree(persistent_script);
}

static int is_not_internal_function(zend_function *function)
{
	return(function->type != ZEND_INTERNAL_FUNCTION);
}

void zend_accel_free_user_functions(HashTable *ht TSRMLS_DC)
{
	dtor_func_t orig_dtor = ht->pDestructor;

	ht->pDestructor = NULL;
	zend_hash_apply(ht, (apply_func_t) is_not_internal_function TSRMLS_CC);
	ht->pDestructor = orig_dtor;
}

static int move_user_function(zend_function *function
#if ZEND_EXTENSION_API_NO >= PHP_5_3_X_API_NO
	TSRMLS_DC 
#endif
	, int num_args, va_list args, zend_hash_key *hash_key) 
{
	HashTable *function_table = va_arg(args, HashTable *);
	(void)num_args; /* keep the compiler happy */
#if ZEND_EXTENSION_API_NO < PHP_5_3_X_API_NO
	TSRMLS_FETCH();
#endif 

	if (function->type == ZEND_USER_FUNCTION) {
		zend_hash_quick_update(function_table, hash_key->arKey, hash_key->nKeyLength, hash_key->h, function, sizeof(zend_function), NULL);
		return 1;
	} else {
		return 0;
	}
}

void zend_accel_move_user_functions(HashTable *src, HashTable *dst TSRMLS_DC)
{
	dtor_func_t orig_dtor = src->pDestructor;

	src->pDestructor = NULL;
#if ZEND_EXTENSION_API_NO < PHP_5_3_X_API_NO
	zend_hash_apply_with_arguments(src, (apply_func_args_t)move_user_function, 1, dst);
#else
	zend_hash_apply_with_arguments(src TSRMLS_CC, (apply_func_args_t)move_user_function, 1, dst);
#endif 
	src->pDestructor = orig_dtor;
}

static int copy_internal_function(zend_function *function, HashTable *function_table TSRMLS_DC)
{
	if (function->type == ZEND_INTERNAL_FUNCTION) {
		zend_hash_update(function_table, function->common.function_name, strlen(function->common.function_name) + 1, function, sizeof(zend_function), NULL);
	}
	return 0;
}

void zend_accel_copy_internal_functions(TSRMLS_D)
{
	zend_hash_apply_with_argument(CG(function_table), (apply_func_arg_t)copy_internal_function, &ZCG(function_table) TSRMLS_CC);
	ZCG(internal_functions_count) = zend_hash_num_elements(&ZCG(function_table));
}

static void zend_destroy_property_info(zend_property_info *property_info)
{
	interned_efree((char*)property_info->name);
	if (property_info->doc_comment) {
		efree((char*)property_info->doc_comment);
	}
}

#if ZEND_EXTENSION_API_NO > PHP_5_5_X_API_NO
static zend_ast *zend_ast_clone(zend_ast *ast TSRMLS_DC)
{
	int i;
	zend_ast *node;

	if (ast->kind == ZEND_CONST) {
		node = emalloc(sizeof(zend_ast) + sizeof(zval));
		node->kind = ZEND_CONST;
		node->children = 0;
		node->u.val = (zval*)(node + 1);
		*node->u.val = *ast->u.val;
		if ((Z_TYPE_P(ast->u.val) & IS_CONSTANT_TYPE_MASK) >= IS_ARRAY) {
			switch ((Z_TYPE_P(ast->u.val) & IS_CONSTANT_TYPE_MASK)) {
				case IS_STRING:
				case IS_CONSTANT:
					Z_STRVAL_P(node->u.val) = (char *) interned_estrndup(Z_STRVAL_P(ast->u.val), Z_STRLEN_P(ast->u.val));
					break;
				case IS_ARRAY:
#if ZEND_EXTENSION_API_NO <= PHP_5_5_API_NO
				case IS_CONSTANT_ARRAY:
#endif
					if (ast->u.val->value.ht && ast->u.val->value.ht != &EG(symbol_table)) {
						ALLOC_HASHTABLE(node->u.val->value.ht);
						zend_hash_clone_zval(node->u.val->value.ht, ast->u.val->value.ht, 0);
					}
					break;
				case IS_CONSTANT_AST:
					Z_AST_P(node->u.val) = zend_ast_clone(Z_AST_P(ast->u.val) TSRMLS_CC);
					break;
			}
		}
	} else {
		node = emalloc(sizeof(zend_ast) + sizeof(zend_ast*) * (ast->children - 1));
		node->kind = ast->kind;
		node->children = ast->children;
		for (i = 0; i < ast->children; i++) {
			if ((&ast->u.child)[i]) {
				(&node->u.child)[i] = zend_ast_clone((&ast->u.child)[i] TSRMLS_CC);
			} else {
				(&node->u.child)[i] = NULL;
			}
		}
	}
	return node;
}
#endif

static inline zval* zend_clone_zval(zval *src, int bind TSRMLS_DC)
{
	zval *ret, **ret_ptr = NULL;

	if (!bind) {
		ALLOC_ZVAL(ret);
		*ret = *src;
		INIT_PZVAL(ret);
	} else if (Z_REFCOUNT_P(src) == 1) {
		ALLOC_ZVAL(ret);
		*ret = *src;
	} else if (accel_xlat_get(src, ret_ptr) != SUCCESS) {
		ALLOC_ZVAL(ret);
		*ret = *src;
		accel_xlat_set(src, ret);
	} else {
		return *ret_ptr;
	}

#if ZEND_EXTENSION_API_NO >= PHP_5_3_X_API_NO
	if ((Z_TYPE_P(ret) & IS_CONSTANT_TYPE_MASK) >= IS_ARRAY) {
		switch ((Z_TYPE_P(ret) & IS_CONSTANT_TYPE_MASK)) {
#else
	if ((Z_TYPE_P(ret) & ~IS_CONSTANT_INDEX) >= IS_ARRAY) {
		switch ((Z_TYPE_P(ret) & ~IS_CONSTANT_INDEX)) {
#endif
			case IS_STRING:
			case IS_CONSTANT:
				Z_STRVAL_P(ret) = (char *) interned_estrndup(Z_STRVAL_P(ret), Z_STRLEN_P(ret));
				break;
			case IS_ARRAY:
#if ZEND_EXTENSION_API_NO <= PHP_5_5_API_NO
			case IS_CONSTANT_ARRAY:
#endif
				if (ret->value.ht && ret->value.ht != &EG(symbol_table)) {
					ALLOC_HASHTABLE(ret->value.ht);
					zend_hash_clone_zval(ret->value.ht, src->value.ht, 0);
				}
				break;
#if ZEND_EXTENSION_API_NO > PHP_5_5_X_API_NO
			case IS_CONSTANT_AST:
				Z_AST_P(ret) = zend_ast_clone(Z_AST_P(ret) TSRMLS_CC);
				break;
#endif
		}
	}
	return ret;
}

static void zend_hash_clone_zval(HashTable *ht, HashTable *source, int bind)
{
	Bucket *p, *q, **prev;
	ulong nIndex;
	zval *ppz;
	TSRMLS_FETCH();

	ht->nTableSize = source->nTableSize;
	ht->nTableMask = source->nTableMask;
	ht->nNumOfElements = source->nNumOfElements;
	ht->nNextFreeElement = source->nNextFreeElement;
	ht->pDestructor = ZVAL_PTR_DTOR;
#if ZEND_DEBUG
	ht->inconsistent = 0;
#endif
	ht->persistent = 0;
	ht->arBuckets = NULL;
	ht->pListHead = NULL;
	ht->pListTail = NULL;
	ht->pInternalPointer = NULL;
	ht->nApplyCount = 0;
	ht->bApplyProtection = 1;

#if ZEND_EXTENSION_API_NO > PHP_5_3_X_API_NO
	if (!ht->nTableMask) {
		ht->arBuckets = (Bucket**)&uninitialized_bucket;
		return;
	}
#endif

	ht->arBuckets = (Bucket **) ecalloc(ht->nTableSize, sizeof(Bucket *));

	prev = &ht->pListHead;
	p = source->pListHead;
	while (p) {
		nIndex = p->h & ht->nTableMask;

		/* Create bucket and initialize key */
#if ZEND_EXTENSION_API_NO > PHP_5_3_X_API_NO
		if (!p->nKeyLength) {
			q = (Bucket *) emalloc(sizeof(Bucket));
			q->arKey = NULL;
		} else if (IS_INTERNED(p->arKey)) {
			q = (Bucket *) emalloc(sizeof(Bucket));
			q->arKey = p->arKey;
		} else {
			q = (Bucket *) emalloc(sizeof(Bucket) + p->nKeyLength);
			q->arKey = ((char*)q) + sizeof(Bucket);
			memcpy((char*)q->arKey, p->arKey, p->nKeyLength);
		}
#else
		q = (Bucket *) emalloc(sizeof(Bucket) - 1 + p->nKeyLength);
		if (p->nKeyLength) {
			memcpy(q->arKey, p->arKey, p->nKeyLength);
		}
#endif
		q->h = p->h;
		q->nKeyLength = p->nKeyLength;

		/* Insert into hash collision list */
		q->pNext = ht->arBuckets[nIndex];
		q->pLast = NULL;
		if (q->pNext) {
			q->pNext->pLast = q;
		}
		ht->arBuckets[nIndex] = q;

		/* Insert into global list */
		q->pListLast = ht->pListTail;
		ht->pListTail = q;
		q->pListNext = NULL;
		*prev = q;
		prev = &q->pListNext;

		/* Copy data */
		q->pData = &q->pDataPtr;
		if (!bind) {
			ALLOC_ZVAL(ppz);
			*ppz = *((zval*)p->pDataPtr);
			INIT_PZVAL(ppz);
		} else if (Z_REFCOUNT_P((zval*)p->pDataPtr) == 1) {
			ALLOC_ZVAL(ppz);
			*ppz = *((zval*)p->pDataPtr);
		} else if (accel_xlat_get(p->pDataPtr, ppz) != SUCCESS) {
			ALLOC_ZVAL(ppz);
			*ppz = *((zval*)p->pDataPtr);
			accel_xlat_set(p->pDataPtr, ppz);
		} else {
			q->pDataPtr = *(void**)ppz;
			p = p->pListNext;
			continue;
		}
		q->pDataPtr = (void*)ppz;

#if ZEND_EXTENSION_API_NO >= PHP_5_3_X_API_NO
		if ((Z_TYPE_P((zval*)p->pDataPtr) & IS_CONSTANT_TYPE_MASK) >= IS_ARRAY) {
			switch ((Z_TYPE_P((zval*)p->pDataPtr) & IS_CONSTANT_TYPE_MASK)) {
#else
		if ((Z_TYPE_P((zval*)p->pDataPtr) & ~IS_CONSTANT_INDEX) >= IS_ARRAY) {
			switch ((Z_TYPE_P((zval*)p->pDataPtr) & ~IS_CONSTANT_INDEX)) {
#endif
				case IS_STRING:
				case IS_CONSTANT:
					Z_STRVAL_P(ppz) = (char *) interned_estrndup(Z_STRVAL_P((zval*)p->pDataPtr), Z_STRLEN_P((zval*)p->pDataPtr));
					break;
				case IS_ARRAY:
#if ZEND_EXTENSION_API_NO <= PHP_5_5_API_NO
				case IS_CONSTANT_ARRAY:
#endif
					if (((zval*)p->pDataPtr)->value.ht && ((zval*)p->pDataPtr)->value.ht != &EG(symbol_table)) {
						ALLOC_HASHTABLE(ppz->value.ht);
						zend_hash_clone_zval(ppz->value.ht, ((zval*)p->pDataPtr)->value.ht, 0);
					}
					break;
#if ZEND_EXTENSION_API_NO > PHP_5_5_X_API_NO
				case IS_CONSTANT_AST:
					Z_AST_P(ppz) = zend_ast_clone(Z_AST_P(ppz) TSRMLS_CC);
					break;
#endif
			}
		}

		p = p->pListNext;
	}
	ht->pInternalPointer = ht->pListHead;
}

static void zend_hash_clone_methods(HashTable *ht, HashTable *source, zend_class_entry *old_ce, zend_class_entry *ce TSRMLS_DC)
{
	Bucket *p, *q, **prev;
	ulong nIndex;
	zend_class_entry **new_ce;
	zend_function** new_prototype;
	zend_op_array *new_entry;

	ht->nTableSize = source->nTableSize;
	ht->nTableMask = source->nTableMask;
	ht->nNumOfElements = source->nNumOfElements;
	ht->nNextFreeElement = source->nNextFreeElement;
	ht->pDestructor = ZEND_FUNCTION_DTOR;
#if ZEND_DEBUG
	ht->inconsistent = 0;
#endif
	ht->persistent = 0;
	ht->pListHead = NULL;
	ht->pListTail = NULL;
	ht->pInternalPointer = NULL;
	ht->nApplyCount = 0;
	ht->bApplyProtection = 1;

#if ZEND_EXTENSION_API_NO > PHP_5_3_X_API_NO
	if (!ht->nTableMask) {
		ht->arBuckets = (Bucket**)&uninitialized_bucket;
		return;
	}
#endif

	ht->arBuckets = (Bucket **) ecalloc(ht->nTableSize, sizeof(Bucket *));

	prev = &ht->pListHead;
	p = source->pListHead;
	while (p) {
		nIndex = p->h & ht->nTableMask;

		/* Create bucket and initialize key */
#if ZEND_EXTENSION_API_NO > PHP_5_3_X_API_NO
		if (!p->nKeyLength) {
			q = (Bucket *) emalloc(sizeof(Bucket));
			q->arKey = NULL;
		} else if (IS_INTERNED(p->arKey)) {
			q = (Bucket *) emalloc(sizeof(Bucket));
			q->arKey = p->arKey;
		} else {
			q = (Bucket *) emalloc(sizeof(Bucket) + p->nKeyLength);
			q->arKey = ((char*)q) + sizeof(Bucket);
			memcpy((char*)q->arKey, p->arKey, p->nKeyLength);
		}
#else
		q = (Bucket *) emalloc(sizeof(Bucket) - 1 + p->nKeyLength);
		if (p->nKeyLength) {
			memcpy(q->arKey, p->arKey, p->nKeyLength);
		}
#endif
		q->h = p->h;
		q->nKeyLength = p->nKeyLength;

		/* Insert into hash collision list */
		q->pNext = ht->arBuckets[nIndex];
		q->pLast = NULL;
		if (q->pNext) {
			q->pNext->pLast = q;
		}
		ht->arBuckets[nIndex] = q;

		/* Insert into global list */
		q->pListLast = ht->pListTail;
		ht->pListTail = q;
		q->pListNext = NULL;
		*prev = q;
		prev = &q->pListNext;

		/* Copy data */
		q->pData = (void *) emalloc(sizeof(zend_function));
		new_entry = (zend_op_array*)q->pData;
		*new_entry = *(zend_op_array*)p->pData;
		q->pDataPtr = NULL;

		/* Copy constructor */
		/* we use refcount to show that op_array is referenced from several places */
		if (new_entry->refcount != NULL) {
			accel_xlat_set(p->pData, new_entry);
		}

		zend_prepare_function_for_execution(new_entry);

		if (old_ce == new_entry->scope) {
			new_entry->scope = ce;
		} else {
			if (accel_xlat_get(new_entry->scope, new_ce) == SUCCESS) {
				new_entry->scope = *new_ce;
			} else {
				zend_error(E_ERROR, ACCELERATOR_PRODUCT_NAME " class loading error, class %s, function %s", ce->name, new_entry->function_name);
			}
		}

		/* update prototype */
		if (new_entry->prototype) {
			if (accel_xlat_get(new_entry->prototype, new_prototype) == SUCCESS) {
				new_entry->prototype = *new_prototype;
			} else {
				zend_error(E_ERROR, ACCELERATOR_PRODUCT_NAME " class loading error, class %s, function %s", ce->name, new_entry->function_name);
			}
		}

		p = p->pListNext;
	}
	ht->pInternalPointer = ht->pListHead;
}

static void zend_hash_clone_prop_info(HashTable *ht, HashTable *source, zend_class_entry *old_ce, zend_class_entry *ce TSRMLS_DC)
{
	Bucket *p, *q, **prev;
	ulong nIndex;
	zend_class_entry **new_ce;
	zend_property_info *prop_info;

	ht->nTableSize = source->nTableSize;
	ht->nTableMask = source->nTableMask;
	ht->nNumOfElements = source->nNumOfElements;
	ht->nNextFreeElement = source->nNextFreeElement;
	ht->pDestructor = (dtor_func_t) zend_destroy_property_info;
#if ZEND_DEBUG
	ht->inconsistent = 0;
#endif
	ht->persistent = 0;
	ht->pListHead = NULL;
	ht->pListTail = NULL;
	ht->pInternalPointer = NULL;
	ht->nApplyCount = 0;
	ht->bApplyProtection = 1;

#if ZEND_EXTENSION_API_NO > PHP_5_3_X_API_NO
	if (!ht->nTableMask) {
		ht->arBuckets = (Bucket**)&uninitialized_bucket;
		return;
	}
#endif

	ht->arBuckets = (Bucket **) ecalloc(ht->nTableSize, sizeof(Bucket *));

	prev = &ht->pListHead;
	p = source->pListHead;
	while (p) {
		nIndex = p->h & ht->nTableMask;

		/* Create bucket and initialize key */
#if ZEND_EXTENSION_API_NO > PHP_5_3_X_API_NO
		if (!p->nKeyLength) {
			q = (Bucket *) emalloc(sizeof(Bucket));
			q->arKey = NULL;
		} else if (IS_INTERNED(p->arKey)) {
			q = (Bucket *) emalloc(sizeof(Bucket));
			q->arKey = p->arKey;
		} else {
			q = (Bucket *) emalloc(sizeof(Bucket) + p->nKeyLength);
			q->arKey = ((char*)q) + sizeof(Bucket);
			memcpy((char*)q->arKey, p->arKey, p->nKeyLength);
		}
#else
		q = (Bucket *) emalloc(sizeof(Bucket) - 1 + p->nKeyLength);
		if (p->nKeyLength) {
			memcpy(q->arKey, p->arKey, p->nKeyLength);
		}
#endif
		q->h = p->h;
		q->nKeyLength = p->nKeyLength;

		/* Insert into hash collision list */
		q->pNext = ht->arBuckets[nIndex];
		q->pLast = NULL;
		if (q->pNext) {
			q->pNext->pLast = q;
		}
		ht->arBuckets[nIndex] = q;

		/* Insert into global list */
		q->pListLast = ht->pListTail;
		ht->pListTail = q;
		q->pListNext = NULL;
		*prev = q;
		prev = &q->pListNext;

		/* Copy data */
		q->pData = (void *) emalloc(sizeof(zend_property_info));
		prop_info = q->pData;
		*prop_info = *(zend_property_info*)p->pData;
		q->pDataPtr = NULL;

		/* Copy constructor */
		prop_info->name = interned_estrndup(prop_info->name, prop_info->name_length);
		if (prop_info->doc_comment) {
			if (ZCG(accel_directives).load_comments) {
				prop_info->doc_comment = estrndup(prop_info->doc_comment, prop_info->doc_comment_len);
			} else {
				prop_info->doc_comment = NULL;
			}
		}
		if (prop_info->ce == old_ce) {
			prop_info->ce = ce;
		} else if (accel_xlat_get(prop_info->ce, new_ce) == SUCCESS) {
			prop_info->ce = *new_ce;
		} else {
			zend_error(E_ERROR, ACCELERATOR_PRODUCT_NAME" class loading error, class %s, property %s", ce->name, prop_info->name);
		}

		p = p->pListNext;
	}
	ht->pInternalPointer = ht->pListHead;
}

/* protects reference count, creates copy of statics */
static int zend_prepare_function_for_execution(zend_op_array *op_array)
{
	HashTable *shared_statics = op_array->static_variables;

	/* protect reference count */
	op_array->refcount = &zend_accel_refcount;
	(*op_array->refcount) = ZEND_PROTECTED_REFCOUNT;

	/* copy statics */
	if (shared_statics) {
		ALLOC_HASHTABLE(op_array->static_variables);
		zend_hash_clone_zval(op_array->static_variables, shared_statics, 0);
	}

	return 0;
}

#define zend_update_inherited_handler(handler) \
{ \
	if (ce->handler != NULL) { \
		if (accel_xlat_get(ce->handler, new_func) == SUCCESS) { \
			ce->handler = *new_func; \
		} else { \
			zend_error(E_ERROR, ACCELERATOR_PRODUCT_NAME " class loading error, class %s", ce->name); \
		} \
	} \
}

/* Protects class' refcount, copies default properties, functions and class name */
static void zend_class_copy_ctor(zend_class_entry **pce)
{
	zend_class_entry *ce = *pce;
	zend_class_entry *old_ce = ce;
	zend_class_entry **new_ce;
	zend_function **new_func;
	TSRMLS_FETCH();

	*pce = ce = emalloc(sizeof(zend_class_entry));
	*ce = *old_ce;
	ce->refcount = 1;

	if (old_ce->refcount != 1) {
		/* this class is not used as a parent for any other classes */
		accel_xlat_set(old_ce, ce);
	}

#if ZEND_EXTENSION_API_NO > PHP_5_3_X_API_NO
	if (old_ce->default_properties_table) {
		int i;

		ce->default_properties_table = emalloc(sizeof(zval*) * old_ce->default_properties_count);
		for (i = 0; i < old_ce->default_properties_count; i++) {
			if (old_ce->default_properties_table[i]) {
				ce->default_properties_table[i] = zend_clone_zval(old_ce->default_properties_table[i], 0 TSRMLS_CC);
			} else {
				ce->default_properties_table[i] = NULL;
			}
		}
	}
#else
	zend_hash_clone_zval(&ce->default_properties, &old_ce->default_properties, 0);
#endif

	zend_hash_clone_methods(&ce->function_table, &old_ce->function_table, old_ce, ce TSRMLS_CC);

	/* static members */
#if ZEND_EXTENSION_API_NO > PHP_5_3_X_API_NO
	if (old_ce->default_static_members_table) {
		int i;

		ce->default_static_members_table = emalloc(sizeof(zval*) * old_ce->default_static_members_count);
		for (i = 0; i < old_ce->default_static_members_count; i++) {
			if (old_ce->default_static_members_table[i]) {
				ce->default_static_members_table[i] = zend_clone_zval(old_ce->default_static_members_table[i], 1 TSRMLS_CC);
			} else {
				ce->default_static_members_table[i] = NULL;
			}
		}
	}
	ce->static_members_table = ce->default_static_members_table;
#else
	zend_hash_clone_zval(&ce->default_static_members, &old_ce->default_static_members, 1);
	ce->static_members = &ce->default_static_members;
#endif

	/* properties_info */
	zend_hash_clone_prop_info(&ce->properties_info, &old_ce->properties_info, old_ce, ce TSRMLS_CC);

	/* constants table */
	zend_hash_clone_zval(&ce->constants_table, &old_ce->constants_table, 0);

	ce->name = interned_estrndup(ce->name, ce->name_length);

	/* interfaces aren't really implemented, so we create a new table */
	if (ce->num_interfaces) {
		ce->interfaces = emalloc(sizeof(zend_class_entry *) * ce->num_interfaces);
		memset(ce->interfaces, 0, sizeof(zend_class_entry *) * ce->num_interfaces);
	} else {
		ce->interfaces = NULL;
	}
	if (ZEND_CE_DOC_COMMENT(ce)) {
		if (ZCG(accel_directives).load_comments) {
			ZEND_CE_DOC_COMMENT(ce) = estrndup(ZEND_CE_DOC_COMMENT(ce), ZEND_CE_DOC_COMMENT_LEN(ce));
		} else {
			ZEND_CE_DOC_COMMENT(ce) =  NULL;
		}
	}

	if (ce->parent) {
		if (accel_xlat_get(ce->parent, new_ce) == SUCCESS) {
			ce->parent = *new_ce;
		} else {
			zend_error(E_ERROR, ACCELERATOR_PRODUCT_NAME" class loading error, class %s", ce->name);
		}
	}

	zend_update_inherited_handler(constructor);
	zend_update_inherited_handler(destructor);
	zend_update_inherited_handler(clone);
	zend_update_inherited_handler(__get);
	zend_update_inherited_handler(__set);
	zend_update_inherited_handler(__call);
/* 5.1 stuff */
	zend_update_inherited_handler(serialize_func);
	zend_update_inherited_handler(unserialize_func);
	zend_update_inherited_handler(__isset);
	zend_update_inherited_handler(__unset);
/* 5.2 stuff */
	zend_update_inherited_handler(__tostring);

#if ZEND_EXTENSION_API_NO >= PHP_5_3_X_API_NO
/* 5.3 stuff */
	zend_update_inherited_handler(__callstatic);
#endif

#if ZEND_EXTENSION_API_NO >= PHP_5_6_X_API_NO
/* 5.6 stuff */
	zend_update_inherited_handler(__debugInfo);
#endif

#if ZEND_EXTENSION_API_NO > PHP_5_3_X_API_NO
/* 5.4 traits */
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
			trait_aliases[i]->trait_method = emalloc(sizeof(zend_trait_method_reference));
			memcpy(trait_aliases[i]->trait_method, ce->trait_aliases[i]->trait_method, sizeof(zend_trait_method_reference));
			if (trait_aliases[i]->trait_method) {
				if (trait_aliases[i]->trait_method->method_name) {
					trait_aliases[i]->trait_method->method_name =
						estrndup(trait_aliases[i]->trait_method->method_name,
							trait_aliases[i]->trait_method->mname_len);
				}
				if (trait_aliases[i]->trait_method->class_name) {
					trait_aliases[i]->trait_method->class_name =
						estrndup(trait_aliases[i]->trait_method->class_name,
							trait_aliases[i]->trait_method->cname_len);
				}
			}

			if (trait_aliases[i]->alias) {
				trait_aliases[i]->alias =
					estrndup(trait_aliases[i]->alias,
						trait_aliases[i]->alias_len);
			}
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
			trait_precedences[i] = emalloc(sizeof(zend_trait_precedence));
			memcpy(trait_precedences[i], ce->trait_precedences[i], sizeof(zend_trait_precedence));
			trait_precedences[i]->trait_method = emalloc(sizeof(zend_trait_method_reference));
			memcpy(trait_precedences[i]->trait_method, ce->trait_precedences[i]->trait_method, sizeof(zend_trait_method_reference));

			trait_precedences[i]->trait_method->method_name =
				estrndup(trait_precedences[i]->trait_method->method_name,
					trait_precedences[i]->trait_method->mname_len);
			trait_precedences[i]->trait_method->class_name =
				estrndup(trait_precedences[i]->trait_method->class_name,
					trait_precedences[i]->trait_method->cname_len);

			if (trait_precedences[i]->exclude_from_classes) {
				zend_class_entry **exclude_from_classes;
				int j = 0;

				while (trait_precedences[i]->exclude_from_classes[j]) {
					j++;
				}
				exclude_from_classes = emalloc(sizeof(zend_class_entry*) * (j + 1));
				j = 0;
				while (trait_precedences[i]->exclude_from_classes[j]) {
					exclude_from_classes[j] = (zend_class_entry*)estrndup(
						(char*)trait_precedences[i]->exclude_from_classes[j],
						strlen((char*)trait_precedences[i]->exclude_from_classes[j]));
					j++;
				}
				exclude_from_classes[j] = NULL;
				trait_precedences[i]->exclude_from_classes = exclude_from_classes;
			}
			i++;
		}
		trait_precedences[i] = NULL;
		ce->trait_precedences = trait_precedences;
	}
#endif
}

static int zend_hash_unique_copy(HashTable *target, HashTable *source, unique_copy_ctor_func_t pCopyConstructor, uint size, int ignore_dups, void **fail_data, void **conflict_data)
{
	Bucket *p;
	void *t;

	p = source->pListHead;
	while (p) {
		if (p->nKeyLength > 0) {
			if (zend_hash_quick_add(target, p->arKey, p->nKeyLength, p->h, p->pData, size, &t) == SUCCESS) {
				if (pCopyConstructor) {
					pCopyConstructor(t);
				}
			} else {
				if (p->nKeyLength > 0 && p->arKey[0] == 0) {
					/* Mangled key */
#if ZEND_EXTENSION_API_NO >= PHP_5_3_X_API_NO
					if (zend_hash_quick_update(target, p->arKey, p->nKeyLength, p->h, p->pData, size, &t) == SUCCESS) {
						if (pCopyConstructor) {
							pCopyConstructor(t);
						}
					}
#endif
				} else if (!ignore_dups && zend_hash_quick_find(target, p->arKey, p->nKeyLength, p->h, &t) == SUCCESS) {
					*fail_data = p->pData;
					*conflict_data = t;
					return FAILURE;
				}
			}
		} else {
			if (!zend_hash_index_exists(target, p->h) && zend_hash_index_update(target, p->h, p->pData, size, &t) == SUCCESS) {
				if (pCopyConstructor) {
					pCopyConstructor(t);
				}
			} else if (!ignore_dups && zend_hash_index_find(target,p->h, &t) == SUCCESS) {
				*fail_data = p->pData;
				*conflict_data = t;
				return FAILURE;
			}
		}
		p = p->pListNext;
	}
	target->pInternalPointer = target->pListHead;

	return SUCCESS;
}

static void zend_accel_function_hash_copy(HashTable *target, HashTable *source, unique_copy_ctor_func_t pCopyConstructor)
{
	zend_function *function1, *function2;
	TSRMLS_FETCH();

	if (zend_hash_unique_copy(target, source, pCopyConstructor, sizeof(zend_function), 0, (void**)&function1, (void**)&function2) != SUCCESS) {
		CG(in_compilation) = 1;
		zend_set_compiled_filename(function1->op_array.filename TSRMLS_CC);
		CG(zend_lineno) = function1->op_array.opcodes[0].lineno;
		if (function2->type == ZEND_USER_FUNCTION
			&& function2->op_array.last > 0) {
			zend_error(E_ERROR, "Cannot redeclare %s() (previously declared in %s:%d)",
					   function1->common.function_name,
					   function2->op_array.filename,
					   (int)function2->op_array.opcodes[0].lineno);
		} else {
			zend_error(E_ERROR, "Cannot redeclare %s()", function1->common.function_name);
		}
	}
}

static void zend_accel_class_hash_copy(HashTable *target, HashTable *source, unique_copy_ctor_func_t pCopyConstructor TSRMLS_DC)
{
	zend_class_entry **pce1, **pce2;

	if (zend_hash_unique_copy(target, source, pCopyConstructor, sizeof(zend_class_entry*), ZCG(accel_directives).ignore_dups, (void**)&pce1, (void**)&pce2) != SUCCESS) {
		CG(in_compilation) = 1;
#if ZEND_EXTENSION_API_NO > PHP_5_3_X_API_NO
		zend_set_compiled_filename((*pce1)->info.user.filename TSRMLS_CC);
		CG(zend_lineno) = (*pce1)->info.user.line_start;
#else
		zend_set_compiled_filename((*pce1)->filename TSRMLS_CC);
		CG(zend_lineno) = (*pce1)->line_start;
#endif
		zend_error(E_ERROR, "Cannot redeclare class %s", (*pce1)->name);
	}
}

#if ZEND_EXTENSION_API_NO < PHP_5_3_X_API_NO
static void zend_do_delayed_early_binding(zend_op_array *op_array, zend_uint early_binding TSRMLS_DC)
{
	zend_uint opline_num = early_binding;

	if ((int)opline_num != -1) {
		zend_bool orig_in_compilation = CG(in_compilation);
		char *orig_compiled_filename = zend_set_compiled_filename(op_array->filename TSRMLS_CC);
		zend_class_entry **pce;

		CG(in_compilation) = 1;
		while ((int)opline_num != -1) {
			if (zend_lookup_class(Z_STRVAL(op_array->opcodes[opline_num - 1].op2.u.constant), Z_STRLEN(op_array->opcodes[opline_num - 1].op2.u.constant), &pce TSRMLS_CC) == SUCCESS) {
				do_bind_inherited_class(&op_array->opcodes[opline_num], EG(class_table), *pce, 1 TSRMLS_CC);
			}
			opline_num = op_array->opcodes[opline_num].result.u.opline_num;
		}
		zend_restore_compiled_filename(orig_compiled_filename TSRMLS_CC);
		CG(in_compilation) = orig_in_compilation;
	}
}
#endif

zend_op_array* zend_accel_load_script(zend_persistent_script *persistent_script, int from_shared_memory TSRMLS_DC)
{
	zend_op_array *op_array;

	op_array = (zend_op_array *) emalloc(sizeof(zend_op_array));
	*op_array = persistent_script->main_op_array;

	if (from_shared_memory) {
		/* Copy all the necessary stuff from shared memory to regular memory, and protect the shared script */
		if (zend_hash_num_elements(&persistent_script->class_table) > 0) {
			zend_hash_init(&ZCG(bind_hash), 10, NULL, NULL, 0);
			zend_accel_class_hash_copy(CG(class_table), &persistent_script->class_table, (unique_copy_ctor_func_t) zend_class_copy_ctor TSRMLS_CC);
			zend_hash_destroy(&ZCG(bind_hash));
		}
		/* we must first to copy all classes and then prepare functions, since functions may try to bind
		   classes - which depend on pre-bind class entries existent in the class table */
		if (zend_hash_num_elements(&persistent_script->function_table) > 0) {
			zend_accel_function_hash_copy(CG(function_table), &persistent_script->function_table, (unique_copy_ctor_func_t)zend_prepare_function_for_execution);
		}

		zend_prepare_function_for_execution(op_array);

		/* Register __COMPILER_HALT_OFFSET__ constant */
		if (persistent_script->compiler_halt_offset != 0 &&
		    persistent_script->full_path) {
			char *name, *cfilename;
			char haltoff[] = "__COMPILER_HALT_OFFSET__";
			int len, clen;

			cfilename = persistent_script->full_path;
			clen = strlen(cfilename);
			zend_mangle_property_name(&name, &len, haltoff, sizeof(haltoff) - 1, cfilename, clen, 0);
			if (!zend_hash_exists(EG(zend_constants), name, len + 1)) {
				zend_register_long_constant(name, len + 1, persistent_script->compiler_halt_offset, CONST_CS, 0 TSRMLS_CC);
			}
			efree(name);
		}

#if ZEND_EXTENSION_API_NO < PHP_5_3_X_API_NO
		if ((int)persistent_script->early_binding != -1) {
			zend_do_delayed_early_binding(op_array, persistent_script->early_binding TSRMLS_CC);
		}
#endif

	} else /* if (!from_shared_memory) */ {
		if (zend_hash_num_elements(&persistent_script->function_table) > 0) {
			zend_accel_function_hash_copy(CG(function_table), &persistent_script->function_table, NULL);
		}
		if (zend_hash_num_elements(&persistent_script->class_table) > 0) {
			zend_accel_class_hash_copy(CG(class_table), &persistent_script->class_table, NULL TSRMLS_CC);
		}
	}

#if ZEND_EXTENSION_API_NO >= PHP_5_3_X_API_NO
	if (op_array->early_binding != (zend_uint)-1) {
		char *orig_compiled_filename = CG(compiled_filename);
		CG(compiled_filename) = persistent_script->full_path;
		zend_do_delayed_early_binding(op_array TSRMLS_CC);
		CG(compiled_filename) = orig_compiled_filename;
	}
#endif

	if (!from_shared_memory) {
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

unsigned int zend_adler32(unsigned int checksum, signed char *buf, uint len)
{
	unsigned int s1 = checksum & 0xffff;
	unsigned int s2 = (checksum >> 16) & 0xffff;
	signed char *end;

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
