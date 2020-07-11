/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 1230e3161d7c7fb549c5ac2ec5cde2dec585d849 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ArrayObject___construct, 0, 0, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, input, "[]")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, iterator_class, IS_STRING, 0, "ArrayIterator::class")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ArrayObject_offsetExists, 0, 0, 1)
	ZEND_ARG_INFO(0, index)
ZEND_END_ARG_INFO()

#define arginfo_class_ArrayObject_offsetGet arginfo_class_ArrayObject_offsetExists

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ArrayObject_offsetSet, 0, 0, 2)
	ZEND_ARG_INFO(0, index)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

#define arginfo_class_ArrayObject_offsetUnset arginfo_class_ArrayObject_offsetExists

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ArrayObject_append, 0, 0, 1)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ArrayObject_getArrayCopy, 0, 0, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ArrayObject_count arginfo_class_ArrayObject_getArrayCopy

#define arginfo_class_ArrayObject_getFlags arginfo_class_ArrayObject_getArrayCopy

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ArrayObject_setFlags, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ArrayObject_asort, 0, 0, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, sort_flags, "SORT_REGULAR")
ZEND_END_ARG_INFO()

#define arginfo_class_ArrayObject_ksort arginfo_class_ArrayObject_asort

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ArrayObject_uasort, 0, 0, 1)
	ZEND_ARG_INFO(0, cmp_function)
ZEND_END_ARG_INFO()

#define arginfo_class_ArrayObject_uksort arginfo_class_ArrayObject_uasort

#define arginfo_class_ArrayObject_natsort arginfo_class_ArrayObject_getArrayCopy

#define arginfo_class_ArrayObject_natcasesort arginfo_class_ArrayObject_getArrayCopy

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ArrayObject_unserialize, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, serialized, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ArrayObject_serialize arginfo_class_ArrayObject_getArrayCopy

#define arginfo_class_ArrayObject___serialize arginfo_class_ArrayObject_getArrayCopy

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ArrayObject___unserialize, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ArrayObject_getIterator arginfo_class_ArrayObject_getArrayCopy

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ArrayObject_exchangeArray, 0, 0, 1)
	ZEND_ARG_INFO(0, input)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ArrayObject_setIteratorClass, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, iteratorClass, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ArrayObject_getIteratorClass arginfo_class_ArrayObject_getArrayCopy

#define arginfo_class_ArrayObject___debugInfo arginfo_class_ArrayObject_getArrayCopy

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ArrayIterator___construct, 0, 0, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, array, "[]")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_class_ArrayIterator_offsetExists arginfo_class_ArrayObject_offsetExists

#define arginfo_class_ArrayIterator_offsetGet arginfo_class_ArrayObject_offsetExists

#define arginfo_class_ArrayIterator_offsetSet arginfo_class_ArrayObject_offsetSet

#define arginfo_class_ArrayIterator_offsetUnset arginfo_class_ArrayObject_offsetExists

#define arginfo_class_ArrayIterator_append arginfo_class_ArrayObject_append

#define arginfo_class_ArrayIterator_getArrayCopy arginfo_class_ArrayObject_getArrayCopy

#define arginfo_class_ArrayIterator_count arginfo_class_ArrayObject_getArrayCopy

#define arginfo_class_ArrayIterator_getFlags arginfo_class_ArrayObject_getArrayCopy

#define arginfo_class_ArrayIterator_setFlags arginfo_class_ArrayObject_setFlags

#define arginfo_class_ArrayIterator_asort arginfo_class_ArrayObject_asort

#define arginfo_class_ArrayIterator_ksort arginfo_class_ArrayObject_asort

#define arginfo_class_ArrayIterator_uasort arginfo_class_ArrayObject_uasort

#define arginfo_class_ArrayIterator_uksort arginfo_class_ArrayObject_uasort

#define arginfo_class_ArrayIterator_natsort arginfo_class_ArrayObject_getArrayCopy

#define arginfo_class_ArrayIterator_natcasesort arginfo_class_ArrayObject_getArrayCopy

#define arginfo_class_ArrayIterator_unserialize arginfo_class_ArrayObject_unserialize

#define arginfo_class_ArrayIterator_serialize arginfo_class_ArrayObject_getArrayCopy

#define arginfo_class_ArrayIterator___serialize arginfo_class_ArrayObject_getArrayCopy

#define arginfo_class_ArrayIterator___unserialize arginfo_class_ArrayObject___unserialize

#define arginfo_class_ArrayIterator_rewind arginfo_class_ArrayObject_getArrayCopy

#define arginfo_class_ArrayIterator_current arginfo_class_ArrayObject_getArrayCopy

#define arginfo_class_ArrayIterator_key arginfo_class_ArrayObject_getArrayCopy

#define arginfo_class_ArrayIterator_next arginfo_class_ArrayObject_getArrayCopy

#define arginfo_class_ArrayIterator_valid arginfo_class_ArrayObject_getArrayCopy

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ArrayIterator_seek, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, position, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ArrayIterator___debugInfo arginfo_class_ArrayObject_getArrayCopy

#define arginfo_class_RecursiveArrayIterator_hasChildren arginfo_class_ArrayObject_getArrayCopy

#define arginfo_class_RecursiveArrayIterator_getChildren arginfo_class_ArrayObject_getArrayCopy


ZEND_METHOD(ArrayObject, __construct);
ZEND_METHOD(ArrayObject, offsetExists);
ZEND_METHOD(ArrayObject, offsetGet);
ZEND_METHOD(ArrayObject, offsetSet);
ZEND_METHOD(ArrayObject, offsetUnset);
ZEND_METHOD(ArrayObject, append);
ZEND_METHOD(ArrayObject, getArrayCopy);
ZEND_METHOD(ArrayObject, count);
ZEND_METHOD(ArrayObject, getFlags);
ZEND_METHOD(ArrayObject, setFlags);
ZEND_METHOD(ArrayObject, asort);
ZEND_METHOD(ArrayObject, ksort);
ZEND_METHOD(ArrayObject, uasort);
ZEND_METHOD(ArrayObject, uksort);
ZEND_METHOD(ArrayObject, natsort);
ZEND_METHOD(ArrayObject, natcasesort);
ZEND_METHOD(ArrayObject, unserialize);
ZEND_METHOD(ArrayObject, serialize);
ZEND_METHOD(ArrayObject, __serialize);
ZEND_METHOD(ArrayObject, __unserialize);
ZEND_METHOD(ArrayObject, getIterator);
ZEND_METHOD(ArrayObject, exchangeArray);
ZEND_METHOD(ArrayObject, setIteratorClass);
ZEND_METHOD(ArrayObject, getIteratorClass);
ZEND_METHOD(ArrayObject, __debugInfo);
ZEND_METHOD(ArrayIterator, __construct);
ZEND_METHOD(ArrayIterator, rewind);
ZEND_METHOD(ArrayIterator, current);
ZEND_METHOD(ArrayIterator, key);
ZEND_METHOD(ArrayIterator, next);
ZEND_METHOD(ArrayIterator, valid);
ZEND_METHOD(ArrayIterator, seek);
ZEND_METHOD(RecursiveArrayIterator, hasChildren);
ZEND_METHOD(RecursiveArrayIterator, getChildren);


static const zend_function_entry class_ArrayObject_methods[] = {
	ZEND_ME(ArrayObject, __construct, arginfo_class_ArrayObject___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(ArrayObject, offsetExists, arginfo_class_ArrayObject_offsetExists, ZEND_ACC_PUBLIC)
	ZEND_ME(ArrayObject, offsetGet, arginfo_class_ArrayObject_offsetGet, ZEND_ACC_PUBLIC)
	ZEND_ME(ArrayObject, offsetSet, arginfo_class_ArrayObject_offsetSet, ZEND_ACC_PUBLIC)
	ZEND_ME(ArrayObject, offsetUnset, arginfo_class_ArrayObject_offsetUnset, ZEND_ACC_PUBLIC)
	ZEND_ME(ArrayObject, append, arginfo_class_ArrayObject_append, ZEND_ACC_PUBLIC)
	ZEND_ME(ArrayObject, getArrayCopy, arginfo_class_ArrayObject_getArrayCopy, ZEND_ACC_PUBLIC)
	ZEND_ME(ArrayObject, count, arginfo_class_ArrayObject_count, ZEND_ACC_PUBLIC)
	ZEND_ME(ArrayObject, getFlags, arginfo_class_ArrayObject_getFlags, ZEND_ACC_PUBLIC)
	ZEND_ME(ArrayObject, setFlags, arginfo_class_ArrayObject_setFlags, ZEND_ACC_PUBLIC)
	ZEND_ME(ArrayObject, asort, arginfo_class_ArrayObject_asort, ZEND_ACC_PUBLIC)
	ZEND_ME(ArrayObject, ksort, arginfo_class_ArrayObject_ksort, ZEND_ACC_PUBLIC)
	ZEND_ME(ArrayObject, uasort, arginfo_class_ArrayObject_uasort, ZEND_ACC_PUBLIC)
	ZEND_ME(ArrayObject, uksort, arginfo_class_ArrayObject_uksort, ZEND_ACC_PUBLIC)
	ZEND_ME(ArrayObject, natsort, arginfo_class_ArrayObject_natsort, ZEND_ACC_PUBLIC)
	ZEND_ME(ArrayObject, natcasesort, arginfo_class_ArrayObject_natcasesort, ZEND_ACC_PUBLIC)
	ZEND_ME(ArrayObject, unserialize, arginfo_class_ArrayObject_unserialize, ZEND_ACC_PUBLIC)
	ZEND_ME(ArrayObject, serialize, arginfo_class_ArrayObject_serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(ArrayObject, __serialize, arginfo_class_ArrayObject___serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(ArrayObject, __unserialize, arginfo_class_ArrayObject___unserialize, ZEND_ACC_PUBLIC)
	ZEND_ME(ArrayObject, getIterator, arginfo_class_ArrayObject_getIterator, ZEND_ACC_PUBLIC)
	ZEND_ME(ArrayObject, exchangeArray, arginfo_class_ArrayObject_exchangeArray, ZEND_ACC_PUBLIC)
	ZEND_ME(ArrayObject, setIteratorClass, arginfo_class_ArrayObject_setIteratorClass, ZEND_ACC_PUBLIC)
	ZEND_ME(ArrayObject, getIteratorClass, arginfo_class_ArrayObject_getIteratorClass, ZEND_ACC_PUBLIC)
	ZEND_ME(ArrayObject, __debugInfo, arginfo_class_ArrayObject___debugInfo, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_ArrayIterator_methods[] = {
	ZEND_ME(ArrayIterator, __construct, arginfo_class_ArrayIterator___construct, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(ArrayObject, offsetExists, offsetExists, arginfo_class_ArrayIterator_offsetExists, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(ArrayObject, offsetGet, offsetGet, arginfo_class_ArrayIterator_offsetGet, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(ArrayObject, offsetSet, offsetSet, arginfo_class_ArrayIterator_offsetSet, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(ArrayObject, offsetUnset, offsetUnset, arginfo_class_ArrayIterator_offsetUnset, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(ArrayObject, append, append, arginfo_class_ArrayIterator_append, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(ArrayObject, getArrayCopy, getArrayCopy, arginfo_class_ArrayIterator_getArrayCopy, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(ArrayObject, count, count, arginfo_class_ArrayIterator_count, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(ArrayObject, getFlags, getFlags, arginfo_class_ArrayIterator_getFlags, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(ArrayObject, setFlags, setFlags, arginfo_class_ArrayIterator_setFlags, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(ArrayObject, asort, asort, arginfo_class_ArrayIterator_asort, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(ArrayObject, ksort, ksort, arginfo_class_ArrayIterator_ksort, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(ArrayObject, uasort, uasort, arginfo_class_ArrayIterator_uasort, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(ArrayObject, uksort, uksort, arginfo_class_ArrayIterator_uksort, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(ArrayObject, natsort, natsort, arginfo_class_ArrayIterator_natsort, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(ArrayObject, natcasesort, natcasesort, arginfo_class_ArrayIterator_natcasesort, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(ArrayObject, unserialize, unserialize, arginfo_class_ArrayIterator_unserialize, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(ArrayObject, serialize, serialize, arginfo_class_ArrayIterator_serialize, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(ArrayObject, __serialize, __serialize, arginfo_class_ArrayIterator___serialize, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(ArrayObject, __unserialize, __unserialize, arginfo_class_ArrayIterator___unserialize, ZEND_ACC_PUBLIC)
	ZEND_ME(ArrayIterator, rewind, arginfo_class_ArrayIterator_rewind, ZEND_ACC_PUBLIC)
	ZEND_ME(ArrayIterator, current, arginfo_class_ArrayIterator_current, ZEND_ACC_PUBLIC)
	ZEND_ME(ArrayIterator, key, arginfo_class_ArrayIterator_key, ZEND_ACC_PUBLIC)
	ZEND_ME(ArrayIterator, next, arginfo_class_ArrayIterator_next, ZEND_ACC_PUBLIC)
	ZEND_ME(ArrayIterator, valid, arginfo_class_ArrayIterator_valid, ZEND_ACC_PUBLIC)
	ZEND_ME(ArrayIterator, seek, arginfo_class_ArrayIterator_seek, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(ArrayObject, __debugInfo, __debugInfo, arginfo_class_ArrayIterator___debugInfo, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_RecursiveArrayIterator_methods[] = {
	ZEND_ME(RecursiveArrayIterator, hasChildren, arginfo_class_RecursiveArrayIterator_hasChildren, ZEND_ACC_PUBLIC)
	ZEND_ME(RecursiveArrayIterator, getChildren, arginfo_class_RecursiveArrayIterator_getChildren, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};
