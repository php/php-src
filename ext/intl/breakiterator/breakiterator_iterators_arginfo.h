/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 267199a0a3532b5acf1d700f14329cdb2f2db0e1 */

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

	return class_entry;
}
