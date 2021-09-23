/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 92769a3b5fa4af0222ab47445b174406288c303d */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Vector___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, iterator, IS_ITERABLE, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Vector_getIterator, 0, 0, InternalIterator, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Vector_count, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Vector_shrinkToFit, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Vector_clear arginfo_class_Vector_shrinkToFit

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Vector_setSize, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, size, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, value, IS_MIXED, 0, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Vector___serialize, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Vector___unserialize, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Vector___set_state, 0, 1, Vector, 0)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Vector_push, 0, 0, IS_VOID, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, values, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Vector_pop, 0, 0, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Vector_toArray arginfo_class_Vector___serialize

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Vector_get, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Vector_set, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Vector_offsetGet, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Vector_offsetExists, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Vector_offsetSet, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Vector_offsetUnset, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Vector_indexOf, 0, 1, IS_LONG, 1)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Vector_contains, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Vector_map, 0, 1, Vector, 0)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Vector_filter, 0, 0, Vector, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, callback, IS_CALLABLE, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_Vector_jsonSerialize arginfo_class_Vector___serialize


ZEND_METHOD(Vector, __construct);
ZEND_METHOD(Vector, getIterator);
ZEND_METHOD(Vector, count);
ZEND_METHOD(Vector, shrinkToFit);
ZEND_METHOD(Vector, clear);
ZEND_METHOD(Vector, setSize);
ZEND_METHOD(Vector, __serialize);
ZEND_METHOD(Vector, __unserialize);
ZEND_METHOD(Vector, __set_state);
ZEND_METHOD(Vector, push);
ZEND_METHOD(Vector, pop);
ZEND_METHOD(Vector, toArray);
ZEND_METHOD(Vector, get);
ZEND_METHOD(Vector, set);
ZEND_METHOD(Vector, offsetGet);
ZEND_METHOD(Vector, offsetExists);
ZEND_METHOD(Vector, offsetSet);
ZEND_METHOD(Vector, offsetUnset);
ZEND_METHOD(Vector, indexOf);
ZEND_METHOD(Vector, contains);
ZEND_METHOD(Vector, map);
ZEND_METHOD(Vector, filter);
ZEND_METHOD(Vector, jsonSerialize);


static const zend_function_entry class_Vector_methods[] = {
	ZEND_ME(Vector, __construct, arginfo_class_Vector___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Vector, getIterator, arginfo_class_Vector_getIterator, ZEND_ACC_PUBLIC)
	ZEND_ME(Vector, count, arginfo_class_Vector_count, ZEND_ACC_PUBLIC)
	ZEND_ME(Vector, shrinkToFit, arginfo_class_Vector_shrinkToFit, ZEND_ACC_PUBLIC)
	ZEND_ME(Vector, clear, arginfo_class_Vector_clear, ZEND_ACC_PUBLIC)
	ZEND_ME(Vector, setSize, arginfo_class_Vector_setSize, ZEND_ACC_PUBLIC)
	ZEND_ME(Vector, __serialize, arginfo_class_Vector___serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Vector, __unserialize, arginfo_class_Vector___unserialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Vector, __set_state, arginfo_class_Vector___set_state, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Vector, push, arginfo_class_Vector_push, ZEND_ACC_PUBLIC)
	ZEND_ME(Vector, pop, arginfo_class_Vector_pop, ZEND_ACC_PUBLIC)
	ZEND_ME(Vector, toArray, arginfo_class_Vector_toArray, ZEND_ACC_PUBLIC)
	ZEND_ME(Vector, get, arginfo_class_Vector_get, ZEND_ACC_PUBLIC)
	ZEND_ME(Vector, set, arginfo_class_Vector_set, ZEND_ACC_PUBLIC)
	ZEND_ME(Vector, offsetGet, arginfo_class_Vector_offsetGet, ZEND_ACC_PUBLIC)
	ZEND_ME(Vector, offsetExists, arginfo_class_Vector_offsetExists, ZEND_ACC_PUBLIC)
	ZEND_ME(Vector, offsetSet, arginfo_class_Vector_offsetSet, ZEND_ACC_PUBLIC)
	ZEND_ME(Vector, offsetUnset, arginfo_class_Vector_offsetUnset, ZEND_ACC_PUBLIC)
	ZEND_ME(Vector, indexOf, arginfo_class_Vector_indexOf, ZEND_ACC_PUBLIC)
	ZEND_ME(Vector, contains, arginfo_class_Vector_contains, ZEND_ACC_PUBLIC)
	ZEND_ME(Vector, map, arginfo_class_Vector_map, ZEND_ACC_PUBLIC)
	ZEND_ME(Vector, filter, arginfo_class_Vector_filter, ZEND_ACC_PUBLIC)
	ZEND_ME(Vector, jsonSerialize, arginfo_class_Vector_jsonSerialize, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Vector(zend_class_entry *class_entry_IteratorAggregate, zend_class_entry *class_entry_Countable, zend_class_entry *class_entry_JsonSerializable, zend_class_entry *class_entry_ArrayAccess)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "Vector", class_Vector_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;
	zend_class_implements(class_entry, 4, class_entry_IteratorAggregate, class_entry_Countable, class_entry_JsonSerializable, class_entry_ArrayAccess);

	return class_entry;
}
