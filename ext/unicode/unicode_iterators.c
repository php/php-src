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
	zval*			text;
	int32_t			offset;
	int32_t			index;
	text_iter_type	type;
} text_iter_t;

PHPAPI zend_class_entry* text_iterator_aggregate_ce;
PHPAPI zend_class_entry* text_iterator_ce;

static void text_iterator_free_storage(void *object TSRMLS_DC)
{
	text_iter_t *intern = (text_iter_t *) object;

	zend_hash_destroy(intern->std.properties);
	FREE_HASHTABLE(intern->std.properties);

	if (intern->text) zval_ptr_dtor(&intern->text);
	efree(object);
}

static zend_object_value text_iterator_new(zend_class_entry *class_type TSRMLS_DC)
{
	zend_object_value retval;
	text_iter_t *intern;
	zval *tmp;

	intern = emalloc(sizeof(text_iter_t));
	memset(intern, 0, sizeof(text_iter_t));
	intern->std.ce = class_type;

	ALLOC_HASHTABLE(intern->std.properties);
	zend_hash_init(intern->std.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
	zend_hash_copy(intern->std.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));

	intern->type = ITER_CODE_POINT;

	retval.handle = zend_objects_store_put(intern, (zend_objects_store_dtor_t)zend_objects_destroy_object, (zend_objects_free_object_storage_t) text_iterator_free_storage, NULL TSRMLS_CC);
	retval.handlers = zend_get_std_object_handlers();

	return retval;
}

static void text_iter_rewind(text_iter_t *intern TSRMLS_DC)
{
	intern->offset = 0;
	intern->index  = 0;
}


PHP_METHOD(TextIterator, __construct)
{
	zval *text;
	zval *object = getThis();
	text_iter_t *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &text) == FAILURE) {
		return;
	}

	if (Z_TYPE_P(text) != IS_UNICODE) {
		zend_throw_exception(U_CLASS_ENTRY(spl_ce_InvalidArgumentException), "Text iterator expects argument to be a Unicode string", 0 TSRMLS_CC);
		return;
	}

	intern = (text_iter_t*) zend_object_store_get_object(object TSRMLS_CC);

	ZVAL_ADDREF(text);
	intern->text = text;

	text_iter_rewind(intern TSRMLS_CC);
}

PHP_METHOD(TextIterator, current)
{
	UChar32 cp;
	UChar buf[3];
	uint32_t tmp, buf_len;
	zval *object = getThis();
	text_iter_t *intern = (text_iter_t*) zend_object_store_get_object(object TSRMLS_CC);

	tmp = intern->offset;
	U16_NEXT(Z_USTRVAL_P(intern->text), tmp, Z_USTRLEN_P(intern->text), cp);
	buf_len = zend_codepoint_to_uchar(cp, buf);
	RETURN_UNICODEL(buf, buf_len, 1);
}

PHP_METHOD(TextIterator, next)
{
	zval *object = getThis();
	text_iter_t *intern = (text_iter_t*) zend_object_store_get_object(object TSRMLS_CC);

	U16_FWD_1(Z_USTRVAL_P(intern->text), intern->offset, Z_USTRLEN_P(intern->text));
	intern->index++;
}

PHP_METHOD(TextIterator, key)
{
	zval *object = getThis();
	text_iter_t *intern = (text_iter_t*) zend_object_store_get_object(object TSRMLS_CC);

	RETURN_LONG(intern->index);
}

PHP_METHOD(TextIterator, valid)
{
	zval *object = getThis();
	text_iter_t *intern = (text_iter_t*) zend_object_store_get_object(object TSRMLS_CC);

	RETURN_BOOL(intern->offset < Z_USTRLEN_P(intern->text));
}

PHP_METHOD(TextIterator, rewind)
{
	zval *object = getThis();
	text_iter_t *intern = (text_iter_t*) zend_object_store_get_object(object TSRMLS_CC);

	text_iter_rewind(intern TSRMLS_CC);
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
	zend_class_implements(text_iterator_ce TSRMLS_CC, 1, zend_ce_iterator);
	text_iterator_ce->create_object = text_iterator_new;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */

