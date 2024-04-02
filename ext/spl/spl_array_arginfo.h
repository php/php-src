/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: c52e89992bd3c04877daab47f4328af0b6ce619e */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ArrayObject___construct, 0, 0, 0)
	ZEND_ARG_TYPE_MASK(0, array, MAY_BE_ARRAY|MAY_BE_OBJECT, "[]")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, iteratorClass, IS_STRING, 0, "ArrayIterator::class")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_ArrayObject_offsetExists, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_ArrayObject_offsetGet, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_ArrayObject_offsetSet, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_ArrayObject_offsetUnset, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_ArrayObject_append, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_ArrayObject_getArrayCopy, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_ArrayObject_count, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ArrayObject_getFlags arginfo_class_ArrayObject_count

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_ArrayObject_setFlags, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_ArrayObject_asort, 0, 0, IS_TRUE, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "SORT_REGULAR")
ZEND_END_ARG_INFO()

#define arginfo_class_ArrayObject_ksort arginfo_class_ArrayObject_asort

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_ArrayObject_uasort, 0, 1, IS_TRUE, 0)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ArrayObject_uksort arginfo_class_ArrayObject_uasort

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_ArrayObject_natsort, 0, 0, IS_TRUE, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ArrayObject_natcasesort arginfo_class_ArrayObject_natsort

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_ArrayObject_unserialize, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_ArrayObject_serialize, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ArrayObject___serialize arginfo_class_ArrayObject_getArrayCopy

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_ArrayObject___unserialize, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_INFO_EX(arginfo_class_ArrayObject_getIterator, 0, 0, Iterator, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_ArrayObject_exchangeArray, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_MASK(0, array, MAY_BE_ARRAY|MAY_BE_OBJECT, NULL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_ArrayObject_setIteratorClass, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, iteratorClass, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ArrayObject_getIteratorClass arginfo_class_ArrayObject_serialize

#define arginfo_class_ArrayObject___debugInfo arginfo_class_ArrayObject_getArrayCopy

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ArrayIterator___construct, 0, 0, 0)
	ZEND_ARG_TYPE_MASK(0, array, MAY_BE_ARRAY|MAY_BE_OBJECT, "[]")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_class_ArrayIterator_offsetExists arginfo_class_ArrayObject_offsetExists

#define arginfo_class_ArrayIterator_offsetGet arginfo_class_ArrayObject_offsetGet

#define arginfo_class_ArrayIterator_offsetSet arginfo_class_ArrayObject_offsetSet

#define arginfo_class_ArrayIterator_offsetUnset arginfo_class_ArrayObject_offsetUnset

#define arginfo_class_ArrayIterator_append arginfo_class_ArrayObject_append

#define arginfo_class_ArrayIterator_getArrayCopy arginfo_class_ArrayObject_getArrayCopy

#define arginfo_class_ArrayIterator_count arginfo_class_ArrayObject_count

#define arginfo_class_ArrayIterator_getFlags arginfo_class_ArrayObject_count

#define arginfo_class_ArrayIterator_setFlags arginfo_class_ArrayObject_setFlags

#define arginfo_class_ArrayIterator_asort arginfo_class_ArrayObject_asort

#define arginfo_class_ArrayIterator_ksort arginfo_class_ArrayObject_asort

#define arginfo_class_ArrayIterator_uasort arginfo_class_ArrayObject_uasort

#define arginfo_class_ArrayIterator_uksort arginfo_class_ArrayObject_uasort

#define arginfo_class_ArrayIterator_natsort arginfo_class_ArrayObject_natsort

#define arginfo_class_ArrayIterator_natcasesort arginfo_class_ArrayObject_natsort

#define arginfo_class_ArrayIterator_unserialize arginfo_class_ArrayObject_unserialize

#define arginfo_class_ArrayIterator_serialize arginfo_class_ArrayObject_serialize

#define arginfo_class_ArrayIterator___serialize arginfo_class_ArrayObject_getArrayCopy

#define arginfo_class_ArrayIterator___unserialize arginfo_class_ArrayObject___unserialize

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_ArrayIterator_rewind, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_ArrayIterator_current, 0, 0, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_ArrayIterator_key, 0, 0, MAY_BE_STRING|MAY_BE_LONG|MAY_BE_NULL)
ZEND_END_ARG_INFO()

#define arginfo_class_ArrayIterator_next arginfo_class_ArrayIterator_rewind

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_ArrayIterator_valid, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_ArrayIterator_seek, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ArrayIterator___debugInfo arginfo_class_ArrayObject_getArrayCopy

#define arginfo_class_RecursiveArrayIterator_hasChildren arginfo_class_ArrayIterator_valid

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_INFO_EX(arginfo_class_RecursiveArrayIterator_getChildren, 0, 0, RecursiveArrayIterator, 1)
ZEND_END_ARG_INFO()

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
	ZEND_RAW_FENTRY("offsetExists", zim_ArrayObject_offsetExists, arginfo_class_ArrayIterator_offsetExists, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("offsetGet", zim_ArrayObject_offsetGet, arginfo_class_ArrayIterator_offsetGet, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("offsetSet", zim_ArrayObject_offsetSet, arginfo_class_ArrayIterator_offsetSet, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("offsetUnset", zim_ArrayObject_offsetUnset, arginfo_class_ArrayIterator_offsetUnset, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("append", zim_ArrayObject_append, arginfo_class_ArrayIterator_append, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("getArrayCopy", zim_ArrayObject_getArrayCopy, arginfo_class_ArrayIterator_getArrayCopy, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("count", zim_ArrayObject_count, arginfo_class_ArrayIterator_count, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("getFlags", zim_ArrayObject_getFlags, arginfo_class_ArrayIterator_getFlags, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("setFlags", zim_ArrayObject_setFlags, arginfo_class_ArrayIterator_setFlags, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("asort", zim_ArrayObject_asort, arginfo_class_ArrayIterator_asort, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("ksort", zim_ArrayObject_ksort, arginfo_class_ArrayIterator_ksort, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("uasort", zim_ArrayObject_uasort, arginfo_class_ArrayIterator_uasort, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("uksort", zim_ArrayObject_uksort, arginfo_class_ArrayIterator_uksort, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("natsort", zim_ArrayObject_natsort, arginfo_class_ArrayIterator_natsort, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("natcasesort", zim_ArrayObject_natcasesort, arginfo_class_ArrayIterator_natcasesort, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("unserialize", zim_ArrayObject_unserialize, arginfo_class_ArrayIterator_unserialize, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("serialize", zim_ArrayObject_serialize, arginfo_class_ArrayIterator_serialize, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("__serialize", zim_ArrayObject___serialize, arginfo_class_ArrayIterator___serialize, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("__unserialize", zim_ArrayObject___unserialize, arginfo_class_ArrayIterator___unserialize, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_ME(ArrayIterator, rewind, arginfo_class_ArrayIterator_rewind, ZEND_ACC_PUBLIC)
	ZEND_ME(ArrayIterator, current, arginfo_class_ArrayIterator_current, ZEND_ACC_PUBLIC)
	ZEND_ME(ArrayIterator, key, arginfo_class_ArrayIterator_key, ZEND_ACC_PUBLIC)
	ZEND_ME(ArrayIterator, next, arginfo_class_ArrayIterator_next, ZEND_ACC_PUBLIC)
	ZEND_ME(ArrayIterator, valid, arginfo_class_ArrayIterator_valid, ZEND_ACC_PUBLIC)
	ZEND_ME(ArrayIterator, seek, arginfo_class_ArrayIterator_seek, ZEND_ACC_PUBLIC)
	ZEND_RAW_FENTRY("__debugInfo", zim_ArrayObject___debugInfo, arginfo_class_ArrayIterator___debugInfo, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_RecursiveArrayIterator_methods[] = {
	ZEND_ME(RecursiveArrayIterator, hasChildren, arginfo_class_RecursiveArrayIterator_hasChildren, ZEND_ACC_PUBLIC)
	ZEND_ME(RecursiveArrayIterator, getChildren, arginfo_class_RecursiveArrayIterator_getChildren, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_ArrayObject(zend_class_entry *class_entry_IteratorAggregate, zend_class_entry *class_entry_ArrayAccess, zend_class_entry *class_entry_Serializable, zend_class_entry *class_entry_Countable)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "ArrayObject", class_ArrayObject_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	zend_class_implements(class_entry, 4, class_entry_IteratorAggregate, class_entry_ArrayAccess, class_entry_Serializable, class_entry_Countable);

	zval const_STD_PROP_LIST_value;
	ZVAL_LONG(&const_STD_PROP_LIST_value, SPL_ARRAY_STD_PROP_LIST);
	zend_string *const_STD_PROP_LIST_name = zend_string_init_interned("STD_PROP_LIST", sizeof("STD_PROP_LIST") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_STD_PROP_LIST_name, &const_STD_PROP_LIST_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_STD_PROP_LIST_name);

	zval const_ARRAY_AS_PROPS_value;
	ZVAL_LONG(&const_ARRAY_AS_PROPS_value, SPL_ARRAY_ARRAY_AS_PROPS);
	zend_string *const_ARRAY_AS_PROPS_name = zend_string_init_interned("ARRAY_AS_PROPS", sizeof("ARRAY_AS_PROPS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ARRAY_AS_PROPS_name, &const_ARRAY_AS_PROPS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ARRAY_AS_PROPS_name);

	return class_entry;
}

static zend_class_entry *register_class_ArrayIterator(zend_class_entry *class_entry_SeekableIterator, zend_class_entry *class_entry_ArrayAccess, zend_class_entry *class_entry_Serializable, zend_class_entry *class_entry_Countable)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "ArrayIterator", class_ArrayIterator_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	zend_class_implements(class_entry, 4, class_entry_SeekableIterator, class_entry_ArrayAccess, class_entry_Serializable, class_entry_Countable);

	zval const_STD_PROP_LIST_value;
	ZVAL_LONG(&const_STD_PROP_LIST_value, SPL_ARRAY_STD_PROP_LIST);
	zend_string *const_STD_PROP_LIST_name = zend_string_init_interned("STD_PROP_LIST", sizeof("STD_PROP_LIST") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_STD_PROP_LIST_name, &const_STD_PROP_LIST_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_STD_PROP_LIST_name);

	zval const_ARRAY_AS_PROPS_value;
	ZVAL_LONG(&const_ARRAY_AS_PROPS_value, SPL_ARRAY_ARRAY_AS_PROPS);
	zend_string *const_ARRAY_AS_PROPS_name = zend_string_init_interned("ARRAY_AS_PROPS", sizeof("ARRAY_AS_PROPS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ARRAY_AS_PROPS_name, &const_ARRAY_AS_PROPS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ARRAY_AS_PROPS_name);

	return class_entry;
}

static zend_class_entry *register_class_RecursiveArrayIterator(zend_class_entry *class_entry_ArrayIterator, zend_class_entry *class_entry_RecursiveIterator)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "RecursiveArrayIterator", class_RecursiveArrayIterator_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_ArrayIterator);
	zend_class_implements(class_entry, 1, class_entry_RecursiveIterator);

	zval const_CHILD_ARRAYS_ONLY_value;
	ZVAL_LONG(&const_CHILD_ARRAYS_ONLY_value, SPL_ARRAY_CHILD_ARRAYS_ONLY);
	zend_string *const_CHILD_ARRAYS_ONLY_name = zend_string_init_interned("CHILD_ARRAYS_ONLY", sizeof("CHILD_ARRAYS_ONLY") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CHILD_ARRAYS_ONLY_name, &const_CHILD_ARRAYS_ONLY_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CHILD_ARRAYS_ONLY_name);

	return class_entry;
}
