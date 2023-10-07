/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 5dc9ab2cc5862b2082fb9cd5cec909298921b115 */

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_INFO_EX(arginfo_class_IntlPartsIterator_getBreakIterator, 0, 0, IntlBreakIterator, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_IntlPartsIterator_getRuleStatus, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()


ZEND_METHOD(IntlPartsIterator, getBreakIterator);
ZEND_METHOD(IntlPartsIterator, getRuleStatus);


static const zend_function_entry class_IntlPartsIterator_methods[] = {
	ZEND_ME(IntlPartsIterator, getBreakIterator, arginfo_class_IntlPartsIterator_getBreakIterator, ZEND_ACC_PUBLIC)
	ZEND_ME(IntlPartsIterator, getRuleStatus, arginfo_class_IntlPartsIterator_getRuleStatus, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_IntlPartsIterator(zend_class_entry *class_entry_IntlIterator)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "IntlPartsIterator", class_IntlPartsIterator_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_IntlIterator);
	class_entry->ce_flags |= ZEND_ACC_NOT_SERIALIZABLE;

	zval const_KEY_SEQUENTIAL_value;
	ZVAL_LONG(&const_KEY_SEQUENTIAL_value, PARTS_ITERATOR_KEY_SEQUENTIAL);
	zend_string *const_KEY_SEQUENTIAL_name = zend_string_init_interned("KEY_SEQUENTIAL", sizeof("KEY_SEQUENTIAL") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_KEY_SEQUENTIAL_name, &const_KEY_SEQUENTIAL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_KEY_SEQUENTIAL_name);

	zval const_KEY_LEFT_value;
	ZVAL_LONG(&const_KEY_LEFT_value, PARTS_ITERATOR_KEY_LEFT);
	zend_string *const_KEY_LEFT_name = zend_string_init_interned("KEY_LEFT", sizeof("KEY_LEFT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_KEY_LEFT_name, &const_KEY_LEFT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_KEY_LEFT_name);

	zval const_KEY_RIGHT_value;
	ZVAL_LONG(&const_KEY_RIGHT_value, PARTS_ITERATOR_KEY_RIGHT);
	zend_string *const_KEY_RIGHT_name = zend_string_init_interned("KEY_RIGHT", sizeof("KEY_RIGHT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_KEY_RIGHT_name, &const_KEY_RIGHT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_KEY_RIGHT_name);

	return class_entry;
}
