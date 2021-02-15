/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 5e165fe25d6d13824da9d7c5f0a089ee11626689 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlPartsIterator_getBreakIterator, 0, 0, 0)
ZEND_END_ARG_INFO()


ZEND_METHOD(IntlPartsIterator, getBreakIterator);


static const zend_function_entry class_IntlPartsIterator_methods[] = {
	ZEND_ME(IntlPartsIterator, getBreakIterator, arginfo_class_IntlPartsIterator_getBreakIterator, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_IntlPartsIterator(zend_class_entry *class_entry_IntlIterator)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "IntlPartsIterator", class_IntlPartsIterator_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_IntlIterator);

	return class_entry;
}
