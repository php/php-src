/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 0569bc7e10a1ec15a3a9eec481da27b647eb1d1d */

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
