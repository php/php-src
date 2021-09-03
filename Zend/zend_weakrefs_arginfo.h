/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: d91889851d9732d41e43fffddb6235d033c67534 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_WeakReference___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_WeakReference_create, 0, 1, WeakReference, 0)
	ZEND_ARG_TYPE_INFO(0, object, IS_OBJECT, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_WeakReference_get, 0, 0, IS_OBJECT, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_WeakMap_offsetGet, 0, 1, IS_MIXED, 0)
	ZEND_ARG_INFO(0, object)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_WeakMap_offsetSet, 0, 2, IS_VOID, 0)
	ZEND_ARG_INFO(0, object)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_WeakMap_offsetExists, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, object)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_WeakMap_offsetUnset, 0, 1, IS_VOID, 0)
	ZEND_ARG_INFO(0, object)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_WeakMap_count, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_WeakMap_getIterator, 0, 0, Iterator, 0)
ZEND_END_ARG_INFO()


ZEND_METHOD(WeakReference, __construct);
ZEND_METHOD(WeakReference, create);
ZEND_METHOD(WeakReference, get);
ZEND_METHOD(WeakMap, offsetGet);
ZEND_METHOD(WeakMap, offsetSet);
ZEND_METHOD(WeakMap, offsetExists);
ZEND_METHOD(WeakMap, offsetUnset);
ZEND_METHOD(WeakMap, count);
ZEND_METHOD(WeakMap, getIterator);


static const zend_function_entry class_WeakReference_methods[] = {
	ZEND_ME(WeakReference, __construct, arginfo_class_WeakReference___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(WeakReference, create, arginfo_class_WeakReference_create, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(WeakReference, get, arginfo_class_WeakReference_get, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_WeakMap_methods[] = {
	ZEND_ME(WeakMap, offsetGet, arginfo_class_WeakMap_offsetGet, ZEND_ACC_PUBLIC)
	ZEND_ME(WeakMap, offsetSet, arginfo_class_WeakMap_offsetSet, ZEND_ACC_PUBLIC)
	ZEND_ME(WeakMap, offsetExists, arginfo_class_WeakMap_offsetExists, ZEND_ACC_PUBLIC)
	ZEND_ME(WeakMap, offsetUnset, arginfo_class_WeakMap_offsetUnset, ZEND_ACC_PUBLIC)
	ZEND_ME(WeakMap, count, arginfo_class_WeakMap_count, ZEND_ACC_PUBLIC)
	ZEND_ME(WeakMap, getIterator, arginfo_class_WeakMap_getIterator, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_WeakReference(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "WeakReference", class_WeakReference_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE;

	return class_entry;
}

static zend_class_entry *register_class_WeakMap(zend_class_entry *class_entry_ArrayAccess, zend_class_entry *class_entry_Countable, zend_class_entry *class_entry_IteratorAggregate)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "WeakMap", class_WeakMap_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE;
	zend_class_implements(class_entry, 3, class_entry_ArrayAccess, class_entry_Countable, class_entry_IteratorAggregate);

	return class_entry;
}
