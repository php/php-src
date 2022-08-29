/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 0b508ad6499b70c92bf25960b30fefa913532a3c */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplFixedArray___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, size, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplFixedArray___wakeup, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_SplFixedArray___serialize, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_SplFixedArray___unserialize, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplFixedArray_count, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplFixedArray_toArray, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_INFO_EX(arginfo_class_SplFixedArray_fromArray, 0, 1, SplFixedArray, 0)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, preserveKeys, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

#define arginfo_class_SplFixedArray_getSize arginfo_class_SplFixedArray_count

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplFixedArray_setSize, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, size, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplFixedArray_offsetExists, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, index)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplFixedArray_offsetGet, 0, 1, IS_MIXED, 0)
	ZEND_ARG_INFO(0, index)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplFixedArray_offsetSet, 0, 2, IS_VOID, 0)
	ZEND_ARG_INFO(0, index)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplFixedArray_offsetUnset, 0, 1, IS_VOID, 0)
	ZEND_ARG_INFO(0, index)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_SplFixedArray_getIterator, 0, 0, Iterator, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SplFixedArray_jsonSerialize arginfo_class_SplFixedArray___serialize


ZEND_METHOD(SplFixedArray, __construct);
ZEND_METHOD(SplFixedArray, __wakeup);
ZEND_METHOD(SplFixedArray, __serialize);
ZEND_METHOD(SplFixedArray, __unserialize);
ZEND_METHOD(SplFixedArray, count);
ZEND_METHOD(SplFixedArray, toArray);
ZEND_METHOD(SplFixedArray, fromArray);
ZEND_METHOD(SplFixedArray, getSize);
ZEND_METHOD(SplFixedArray, setSize);
ZEND_METHOD(SplFixedArray, offsetExists);
ZEND_METHOD(SplFixedArray, offsetGet);
ZEND_METHOD(SplFixedArray, offsetSet);
ZEND_METHOD(SplFixedArray, offsetUnset);
ZEND_METHOD(SplFixedArray, getIterator);
ZEND_METHOD(SplFixedArray, jsonSerialize);


static const zend_function_entry class_SplFixedArray_methods[] = {
	ZEND_ME(SplFixedArray, __construct, arginfo_class_SplFixedArray___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFixedArray, __wakeup, arginfo_class_SplFixedArray___wakeup, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFixedArray, __serialize, arginfo_class_SplFixedArray___serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFixedArray, __unserialize, arginfo_class_SplFixedArray___unserialize, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFixedArray, count, arginfo_class_SplFixedArray_count, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFixedArray, toArray, arginfo_class_SplFixedArray_toArray, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFixedArray, fromArray, arginfo_class_SplFixedArray_fromArray, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(SplFixedArray, getSize, arginfo_class_SplFixedArray_getSize, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFixedArray, setSize, arginfo_class_SplFixedArray_setSize, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFixedArray, offsetExists, arginfo_class_SplFixedArray_offsetExists, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFixedArray, offsetGet, arginfo_class_SplFixedArray_offsetGet, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFixedArray, offsetSet, arginfo_class_SplFixedArray_offsetSet, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFixedArray, offsetUnset, arginfo_class_SplFixedArray_offsetUnset, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFixedArray, getIterator, arginfo_class_SplFixedArray_getIterator, ZEND_ACC_PUBLIC)
	ZEND_ME(SplFixedArray, jsonSerialize, arginfo_class_SplFixedArray_jsonSerialize, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_SplFixedArray(zend_class_entry *class_entry_IteratorAggregate, zend_class_entry *class_entry_ArrayAccess, zend_class_entry *class_entry_Countable, zend_class_entry *class_entry_JsonSerializable)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "SplFixedArray", class_SplFixedArray_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	zend_class_implements(class_entry, 4, class_entry_IteratorAggregate, class_entry_ArrayAccess, class_entry_Countable, class_entry_JsonSerializable);

	return class_entry;
}
