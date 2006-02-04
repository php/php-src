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
 * - return code units as binary strings? integers? or leave as unicode strings?
 */

#include "php.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"
#include "ext/spl/spl_exceptions.h"

typedef enum {
	ITER_CODE_UNIT,
	ITER_CODE_POINT,
	ITER_COMB_SEQUENCE,
} text_iter_type;

typedef struct {
	zend_object		std;
	UChar*			text;
	uint32_t		text_len;
	text_iter_type	type;
	zval*			current;
	union {
		struct {
			int32_t	offset;
			int32_t	index;
		} cp;
		struct {
			int32_t	index;
		} cu;
	} u;
} text_iter_obj;

typedef struct {
	zend_object_iterator intern;
	text_iter_obj*		 object;
} text_iter_it;

typedef struct {
	int  (*valid)  (text_iter_obj* object TSRMLS_DC);
	void (*current)(text_iter_obj* object TSRMLS_DC);
	int  (*key)    (text_iter_obj* object TSRMLS_DC);
	void (*next)   (text_iter_obj* object TSRMLS_DC);
	void (*rewind) (text_iter_obj* object TSRMLS_DC);
} text_iter_ops;

PHPAPI zend_class_entry* text_iterator_aggregate_ce;
PHPAPI zend_class_entry* text_iterator_ce;

/* Code unit ops */

static int text_iter_cu_valid(text_iter_obj* object TSRMLS_DC)
{
	return (object->u.cu.index < object->text_len);
}

static void text_iter_cu_current(text_iter_obj* object TSRMLS_DC)
{
	if (!object->current) {
		MAKE_STD_ZVAL(object->current);
		ZVAL_UNICODEL(object->current, object->text + object->u.cu.index, 1, 1);
	}
}

static int text_iter_cu_key(text_iter_obj* object TSRMLS_DC)
{
	return object->u.cu.index;
}

static void text_iter_cu_next(text_iter_obj* object TSRMLS_DC)
{
	object->u.cu.index++;
	if (object->current) {
		zval_ptr_dtor(&object->current);
		object->current = NULL;
	}
}

static void text_iter_cu_rewind(text_iter_obj *object TSRMLS_DC)
{
	object->u.cu.index  = 0;
	if (object->current) {
		zval_ptr_dtor(&object->current);
		object->current = NULL;
	}
}

static text_iter_ops text_iter_cu_ops = {
	text_iter_cu_valid,
	text_iter_cu_current,
	text_iter_cu_key,
	text_iter_cu_next,
	text_iter_cu_rewind,
};

/* Code point ops */

static int text_iter_cp_valid(text_iter_obj* object TSRMLS_DC)
{
	return (object->u.cp.offset < object->text_len);
}

static void text_iter_cp_current(text_iter_obj* object TSRMLS_DC)
{
	UChar32 cp;
	int32_t tmp, buf_len;

	if (!object->current) {
		MAKE_STD_ZVAL(object->current);
		Z_USTRVAL_P(object->current) = eumalloc(3);
		Z_TYPE_P(object->current) = IS_UNICODE;
		tmp = object->u.cp.offset;
		U16_NEXT(object->text, tmp, object->text_len, cp);
		buf_len = zend_codepoint_to_uchar(cp, Z_USTRVAL_P(object->current));
		Z_USTRVAL_P(object->current)[buf_len] = 0;
		Z_USTRLEN_P(object->current) = buf_len;
	}
}

static int text_iter_cp_key(text_iter_obj* object TSRMLS_DC)
{
	return object->u.cp.index;
}

static void text_iter_cp_next(text_iter_obj* object TSRMLS_DC)
{
	U16_FWD_1(object->text, object->u.cp.offset, object->text_len);
	object->u.cp.index++;
	if (object->current) {
		zval_ptr_dtor(&object->current);
		object->current = NULL;
	}
}

static void text_iter_cp_rewind(text_iter_obj *object TSRMLS_DC)
{
	object->u.cp.offset = 0;
	object->u.cp.index  = 0;
	if (object->current) {
		zval_ptr_dtor(&object->current);
		object->current = NULL;
	}
}

static text_iter_ops text_iter_cp_ops = {
	text_iter_cp_valid,
	text_iter_cp_current,
	text_iter_cp_key,
	text_iter_cp_next,
	text_iter_cp_rewind,
};

static text_iter_ops* iter_ops[2] = {
	&text_iter_cu_ops,
	&text_iter_cp_ops,
};

/* Iterator Funcs */

static void text_iter_dtor(zend_object_iterator* iter TSRMLS_DC)
{
	text_iter_it* iterator = (text_iter_it *) iter;
	zval_ptr_dtor((zval **)&iterator->intern.data);
	efree(iterator);
}

static int text_iter_valid(zend_object_iterator* iter TSRMLS_DC)
{
	text_iter_it*  iterator = (text_iter_it *) iter;
	text_iter_obj* object   = iterator->object;

	if (iter_ops[object->type]->valid(object TSRMLS_CC))
		return SUCCESS;
	else
		return FAILURE;
}

static void text_iter_get_current_data(zend_object_iterator* iter, zval*** data TSRMLS_DC)
{
	text_iter_it*  iterator = (text_iter_it *) iter;
	text_iter_obj* object   = iterator->object;

	iter_ops[object->type]->current(object TSRMLS_CC);
	*data = &object->current;
}

static int text_iter_get_current_key(zend_object_iterator* iter, char **str_key, uint *str_key_len, ulong *int_key TSRMLS_DC)
{
	text_iter_it*  iterator = (text_iter_it *) iter;
	text_iter_obj* object   = iterator->object;

	*int_key = iter_ops[object->type]->key(object TSRMLS_CC);
	return HASH_KEY_IS_LONG;
}

static void text_iter_move_forward(zend_object_iterator* iter TSRMLS_DC)
{
	text_iter_it*  iterator = (text_iter_it *) iter;
	text_iter_obj* object   = iterator->object;

	iter_ops[object->type]->next(object TSRMLS_CC);
}

static void text_iter_rewind(zend_object_iterator* iter TSRMLS_DC)
{
	text_iter_it*  iterator = (text_iter_it *) iter;
	text_iter_obj* object   = iterator->object;

	iter_ops[object->type]->rewind(object TSRMLS_CC);
}

zend_object_iterator_funcs text_iter_cp_funcs = {
	text_iter_dtor,
	text_iter_valid,
	text_iter_get_current_data,
	text_iter_get_current_key,
	text_iter_move_forward,
	text_iter_rewind,
};

static zend_object_iterator* text_iter_get_iterator(zend_class_entry *ce, zval *object TSRMLS_DC)
{
	text_iter_it* 	iterator 	= emalloc(sizeof(text_iter_it));
	text_iter_obj*	iter_object = (text_iter_obj *) zend_object_store_get_object(object TSRMLS_CC);

	ZVAL_ADDREF(object);
	iterator->intern.data  = (void *) object;
	iterator->intern.funcs = &text_iter_cp_funcs;
	iterator->object 	   = iter_object;

	return (zend_object_iterator *) iterator;
}

static void text_iterator_free_storage(void *object TSRMLS_DC)
{
	text_iter_obj *intern = (text_iter_obj *) object;

	zend_hash_destroy(intern->std.properties);
	FREE_HASHTABLE(intern->std.properties);

	if (intern->text) {
		efree(intern->text);
	}
	if (intern->current) {
		zval_ptr_dtor(&intern->current);
	}
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

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "u", &text, &text_len) == FAILURE) {
		return;
	}

	intern = (text_iter_obj*) zend_object_store_get_object(object TSRMLS_CC);

	intern->text = eustrndup(text, text_len);
	intern->text_len = text_len;

	text_iter_cp_rewind(intern TSRMLS_CC);
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

	iter_ops[object->type]->rewind(intern TSRMLS_CC);
}

static zend_function_entry text_iterator_funcs[] = {
	PHP_ME(TextIterator, __construct, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(TextIterator, current,  	  NULL, ZEND_ACC_PUBLIC)
	PHP_ME(TextIterator, next,        NULL, ZEND_ACC_PUBLIC)
	PHP_ME(TextIterator, key,         NULL, ZEND_ACC_PUBLIC)
	PHP_ME(TextIterator, valid,       NULL, ZEND_ACC_PUBLIC)
	PHP_ME(TextIterator, rewind,      NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

void php_register_unicode_iterators(TSRMLS_D)
{
	zend_class_entry ce;
	
	INIT_CLASS_ENTRY(ce, "TextIterator", text_iterator_funcs);
	text_iterator_ce = zend_register_internal_class(&ce TSRMLS_CC);
	text_iterator_ce->create_object = text_iterator_new;
	text_iterator_ce->get_iterator  = text_iter_get_iterator;
	zend_class_implements(text_iterator_ce TSRMLS_CC, 1, zend_ce_traversable);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */

