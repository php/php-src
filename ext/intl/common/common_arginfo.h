/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: fb470896fab2e63e898a5d49ff8d7f59257be9e3 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlIterator_current, 0, 0, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlIterator_key arginfo_class_IntlIterator_current

#define arginfo_class_IntlIterator_next arginfo_class_IntlIterator_current

#define arginfo_class_IntlIterator_rewind arginfo_class_IntlIterator_current

#define arginfo_class_IntlIterator_valid arginfo_class_IntlIterator_current


ZEND_METHOD(IntlIterator, current);
ZEND_METHOD(IntlIterator, key);
ZEND_METHOD(IntlIterator, next);
ZEND_METHOD(IntlIterator, rewind);
ZEND_METHOD(IntlIterator, valid);


static const zend_function_entry class_IntlIterator_methods[] = {
	ZEND_ME(IntlIterator, current, arginfo_class_IntlIterator_current, ZEND_ACC_PUBLIC)
	ZEND_ME(IntlIterator, key, arginfo_class_IntlIterator_key, ZEND_ACC_PUBLIC)
	ZEND_ME(IntlIterator, next, arginfo_class_IntlIterator_next, ZEND_ACC_PUBLIC)
	ZEND_ME(IntlIterator, rewind, arginfo_class_IntlIterator_rewind, ZEND_ACC_PUBLIC)
	ZEND_ME(IntlIterator, valid, arginfo_class_IntlIterator_valid, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_IntlIterator(zend_class_entry *class_entry_Iterator)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "IntlIterator", class_IntlIterator_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	zend_class_implements(class_entry, 1, class_entry_Iterator);

	return class_entry;
}
