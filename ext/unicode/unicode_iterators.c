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
		int32_t		start;
		struct {
			int32_t start;
			int32_t index;
		} cp;
		struct {
			int32_t start;
			int32_t end;
			int32_t index;
		} cs;
		struct {
			int32_t start;
			int32_t end;
			int32_t index;
			UBreakIterator *iter;
		} brk;
	} u;
	zend_object_iterator iter;
} text_iter_obj;

static inline text_iter_obj* text_iter_to_obj(zend_object_iterator *iter)
{
	return (text_iter_obj *)((char*)iter - offsetof(text_iter_obj, iter));
}

typedef struct {
	int  (*valid)  (text_iter_obj* object TSRMLS_DC);
	void (*current)(text_iter_obj* object TSRMLS_DC);
	int  (*key)    (text_iter_obj* object TSRMLS_DC);
	void (*next)   (text_iter_obj* object TSRMLS_DC);
	void (*rewind) (text_iter_obj* object TSRMLS_DC);
} text_iter_ops;

enum UBreakIteratorType brk_type_map[] = {
	UBRK_CHARACTER,
	UBRK_WORD,
	UBRK_LINE,
	UBRK_SENTENCE,
};

PHPAPI zend_class_entry* text_iterator_aggregate_ce;
PHPAPI zend_class_entry* text_iterator_ce;
PHPAPI zend_class_entry* rev_text_iterator_ce;

/* Code point ops */

static int text_iter_cp_valid(text_iter_obj* object TSRMLS_DC)
{
	if (object->flags & ITER_REVERSE) {
		return (object->u.cp.start > 0);
	} else {
		return (object->u.cp.start < object->text_len);
	}
}

static void text_iter_cp_current(text_iter_obj* object TSRMLS_DC)
{
	UChar32 cp;
	int32_t tmp, buf_len;

	tmp = object->u.cp.start;
	if (object->flags & ITER_REVERSE) {
		U16_PREV(object->text, 0, tmp, cp);
	} else {
		U16_NEXT(object->text, tmp, object->text_len, cp);
	}
	buf_len = zend_codepoint_to_uchar(cp, Z_USTRVAL_P(object->current));
	Z_USTRVAL_P(object->current)[buf_len] = 0;
	Z_USTRLEN_P(object->current) = buf_len;
}

static int text_iter_cp_key(text_iter_obj* object TSRMLS_DC)
{
	return object->u.cp.index;
}

static void text_iter_cp_next(text_iter_obj* object TSRMLS_DC)
{
	if (object->flags & ITER_REVERSE) {
		U16_BACK_1(object->text, 0, object->u.cp.start);
	} else {
		U16_FWD_1(object->text, object->u.cp.start, object->text_len);
	}
	object->u.cp.index++;
}

static void text_iter_cp_rewind(text_iter_obj *object TSRMLS_DC)
{
	if (object->flags & ITER_REVERSE) {
		object->u.cp.start = object->text_len;
	} else {
		object->u.cp.start = 0;
	}
	object->u.cp.index  = 0;
}

static text_iter_ops text_iter_cp_ops = {
	text_iter_cp_valid,
	text_iter_cp_current,
	text_iter_cp_key,
	text_iter_cp_next,
	text_iter_cp_rewind,
};

/* Combining sequence ops */

static int text_iter_cs_valid(text_iter_obj* object TSRMLS_DC)
{
	if (object->flags & ITER_REVERSE) {
		return (object->u.cs.end > 0);
	} else {
		return (object->u.cs.end <= object->text_len);
	}
}

static void text_iter_cs_current(text_iter_obj* object TSRMLS_DC)
{
	uint32_t length = object->u.cs.end - object->u.cs.start;
	if (length > object->current_alloc) {
		object->current_alloc = length+1;
		Z_USTRVAL_P(object->current) = eurealloc(Z_USTRVAL_P(object->current), object->current_alloc);
	}
	u_memcpy(Z_USTRVAL_P(object->current), object->text + object->u.cs.start, length);
	Z_USTRVAL_P(object->current)[length] = 0;
	Z_USTRLEN_P(object->current) = length;
}

static int text_iter_cs_key(text_iter_obj* object TSRMLS_DC)
{
	return object->u.cs.index;
}

static void text_iter_cs_next(text_iter_obj* object TSRMLS_DC)
{
	UChar32 cp;
	uint32_t tmp;

	if (object->flags & ITER_REVERSE) {
		object->u.cs.end = object->u.cs.start;
		U16_PREV(object->text, 0, object->u.cs.start, cp);
		if (u_getCombiningClass(cp) != 0) {
			do {
				U16_PREV(object->text, 0, object->u.cs.start, cp);
			} while (object->u.cs.start > 0 && u_getCombiningClass(cp) != 0);
		}
	} else {
		object->u.cs.start = object->u.cs.end;
		U16_NEXT(object->text, object->u.cs.end, object->text_len, cp);
		if (u_getCombiningClass(cp) == 0) {
			tmp = object->u.cs.end;
			while (tmp < object->text_len) {
				U16_NEXT(object->text, tmp, object->text_len, cp);
				if (u_getCombiningClass(cp) == 0) {
					break;
				} else {
					object->u.cs.end = tmp;
				}
			}
		}
	}
	object->u.cs.index++;
}

static void text_iter_cs_rewind(text_iter_obj *object TSRMLS_DC)
{
	if (object->flags & ITER_REVERSE) {
		object->u.cs.start = object->u.cs.end = object->text_len;
	} else {
		object->u.cs.start = object->u.cs.end = 0;
	}
	text_iter_cs_next(object TSRMLS_CC); /* find first sequence */
	object->u.cs.index = 0; /* because _next increments index */
}

static text_iter_ops text_iter_cs_ops = {
	text_iter_cs_valid,
	text_iter_cs_current,
	text_iter_cs_key,
	text_iter_cs_next,
	text_iter_cs_rewind,
};


/* UBreakIterator Character Ops */

static int text_iter_brk_char_valid(text_iter_obj* object TSRMLS_DC)
{
	if (object->flags & ITER_REVERSE) {
		return (object->u.brk.start != UBRK_DONE);
	} else {
		return (object->u.brk.end != UBRK_DONE);
	}
}

static void text_iter_brk_char_current(text_iter_obj* object TSRMLS_DC)
{
	uint32_t length;
	int32_t start = object->u.brk.start;
	int32_t end = object->u.brk.end;

	if (object->flags & ITER_REVERSE) {
		if (end == UBRK_DONE) {
			end = object->text_len;
		}
	} else {
		if (start == UBRK_DONE) {
			start = 0;
		}
	}
	length = end - start;
	if (length > object->current_alloc-1) {
		object->current_alloc = length+1;
		Z_USTRVAL_P(object->current) = eurealloc(Z_USTRVAL_P(object->current), object->current_alloc);
	}
	u_memcpy(Z_USTRVAL_P(object->current), object->text + start, length);
	Z_USTRVAL_P(object->current)[length] = 0;
	Z_USTRLEN_P(object->current) = length;
}

static int text_iter_brk_char_key(text_iter_obj* object TSRMLS_DC)
{
	return object->u.brk.index;
}

static void text_iter_brk_char_next(text_iter_obj* object TSRMLS_DC)
{
	if (object->flags & ITER_REVERSE) {
		if (object->u.brk.start != UBRK_DONE) {
			object->u.brk.end = object->u.brk.start;
			object->u.brk.start = ubrk_previous(object->u.brk.iter);
			object->u.brk.index++;
		}
	} else {
		if (object->u.brk.end != UBRK_DONE) {
			object->u.brk.start = object->u.brk.end;
			object->u.brk.end = ubrk_next(object->u.brk.iter);
			object->u.brk.index++;
		}
	}
}

static void text_iter_brk_char_rewind(text_iter_obj *object TSRMLS_DC)
{
	if (object->flags & ITER_REVERSE) {
		object->u.brk.end   = ubrk_last(object->u.brk.iter);
		object->u.brk.start = ubrk_previous(object->u.brk.iter);
	} else {
		object->u.brk.start = ubrk_first(object->u.brk.iter);
		object->u.brk.end   = ubrk_next(object->u.brk.iter);
	}
	object->u.brk.index = 0;
}

static text_iter_ops text_iter_brk_ops = {
	text_iter_brk_char_valid,
	text_iter_brk_char_current,
	text_iter_brk_char_key,
	text_iter_brk_char_next,
	text_iter_brk_char_rewind,
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

	if (iter_ops[obj->type]->valid(obj TSRMLS_CC)) {
		return SUCCESS;
	} else {
		return FAILURE;
	}
}

static void text_iter_get_current_data(zend_object_iterator* iter, zval*** data TSRMLS_DC)
{
	text_iter_obj* obj = text_iter_to_obj(iter);

	iter_ops[obj->type]->current(obj TSRMLS_CC);
	*data = &obj->current;
}

static int text_iter_get_current_key(zend_object_iterator* iter, zstr *str_key, uint *str_key_len, ulong *int_key TSRMLS_DC)
{
	text_iter_obj* obj = text_iter_to_obj(iter);

	*int_key = iter_ops[obj->type]->key(obj TSRMLS_CC);
	return HASH_KEY_IS_LONG;
}

static void text_iter_move_forward(zend_object_iterator* iter TSRMLS_DC)
{
	text_iter_obj* obj = text_iter_to_obj(iter);

	iter_ops[obj->type]->next(obj TSRMLS_CC);
}

static void text_iter_rewind(zend_object_iterator* iter TSRMLS_DC)
{
	text_iter_obj* obj = text_iter_to_obj(iter);

	iter_ops[obj->type]->rewind(obj TSRMLS_CC);
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

	ZVAL_ADDREF(object);
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
	if (intern->type > ITER_CHARACTER && intern->u.brk.iter) {
		ubrk_close(intern->u.brk.iter);
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
		locale = locale ? locale : UG(default_locale);
		intern->u.brk.iter = ubrk_open(brk_type_map[intern->type - ITER_CHARACTER], locale, text, text_len, &status);
		if (!U_SUCCESS(status)) {
			php_error(E_RECOVERABLE_ERROR, "Could not create UBreakIterator for '%s' locale: %s", locale, u_errorName(status));
			return;
		}
	}

	iter_ops[intern->type]->rewind(intern TSRMLS_CC);
}

PHP_METHOD(TextIterator, current)
{
	zval *object = getThis();
	text_iter_obj *intern = (text_iter_obj*) zend_object_store_get_object(object TSRMLS_CC);

	iter_ops[intern->type]->current(intern TSRMLS_CC);
	RETURN_UNICODEL(Z_USTRVAL_P(intern->current), Z_USTRLEN_P(intern->current), 1);
}

PHP_METHOD(TextIterator, next)
{
	zval *object = getThis();
	text_iter_obj *intern = (text_iter_obj*) zend_object_store_get_object(object TSRMLS_CC);

	iter_ops[intern->type]->next(intern TSRMLS_CC);
}

PHP_METHOD(TextIterator, key)
{
	zval *object = getThis();
	text_iter_obj *intern = (text_iter_obj*) zend_object_store_get_object(object TSRMLS_CC);

	RETURN_LONG(iter_ops[intern->type]->key(intern TSRMLS_CC));
}

PHP_METHOD(TextIterator, valid)
{
	zval *object = getThis();
	text_iter_obj *intern = (text_iter_obj*) zend_object_store_get_object(object TSRMLS_CC);

	RETURN_BOOL(iter_ops[intern->type]->valid(intern TSRMLS_CC));
}

PHP_METHOD(TextIterator, rewind)
{
	zval *object = getThis();
	text_iter_obj *intern = (text_iter_obj*) zend_object_store_get_object(object TSRMLS_CC);

	iter_ops[intern->type]->rewind(intern TSRMLS_CC);
}

PHP_METHOD(TextIterator, offset)
{
	zval *object = getThis();
	text_iter_obj *intern = (text_iter_obj*) zend_object_store_get_object(object TSRMLS_CC);

	RETURN_LONG(intern->u.start);
}

static zend_function_entry text_iterator_funcs[] = {
	PHP_ME(TextIterator, __construct, NULL, ZEND_ACC_PUBLIC)

	/* Iterator interface methods */
	PHP_ME(TextIterator, current,  	  NULL, ZEND_ACC_PUBLIC)
	PHP_ME(TextIterator, next,        NULL, ZEND_ACC_PUBLIC)
	PHP_ME(TextIterator, key,         NULL, ZEND_ACC_PUBLIC)
	PHP_ME(TextIterator, valid,       NULL, ZEND_ACC_PUBLIC)
	PHP_ME(TextIterator, rewind,      NULL, ZEND_ACC_PUBLIC)

	PHP_ME(TextIterator, offset,	  NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

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

	zend_declare_class_constant_long(text_iterator_ce, "CODE_POINT", sizeof("CODE_POINT")-1, ITER_CODE_POINT TSRMLS_CC);
	zend_declare_class_constant_long(text_iterator_ce, "COMB_SEQUENCE", sizeof("COMB_SEQUENCE")-1, ITER_COMB_SEQUENCE TSRMLS_CC);
	zend_declare_class_constant_long(text_iterator_ce, "CHARACTER", sizeof("CHARACTER")-1, ITER_CHARACTER TSRMLS_CC);
	zend_declare_class_constant_long(text_iterator_ce, "WORD", sizeof("WORD")-1, ITER_WORD TSRMLS_CC);
	zend_declare_class_constant_long(text_iterator_ce, "LINE", sizeof("LINE")-1, ITER_LINE TSRMLS_CC);
	zend_declare_class_constant_long(text_iterator_ce, "SENTENCE", sizeof("SENTENCE")-1, ITER_SENTENCE TSRMLS_CC);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */

