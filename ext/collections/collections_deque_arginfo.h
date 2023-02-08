/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: a6e06241621b8552c7f12b33f9a5311672ad09b9 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Collections_Deque___construct, 0, 0, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, iterator, Traversable, MAY_BE_ARRAY, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Collections_Deque_getIterator, 0, 0, InternalIterator, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Collections_Deque_count, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Collections_Deque_isEmpty, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Collections_Deque_clear, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Collections_Deque___serialize, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Collections_Deque___unserialize, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Collections_Deque___set_state, 0, 1, Collections\\Deque, 0)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Collections_Deque_push, 0, 0, IS_VOID, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, values, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Collections_Deque_pushFront arginfo_class_Collections_Deque_push

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Collections_Deque_pop, 0, 0, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Collections_Deque_popFront arginfo_class_Collections_Deque_pop

#define arginfo_class_Collections_Deque_first arginfo_class_Collections_Deque_pop

#define arginfo_class_Collections_Deque_last arginfo_class_Collections_Deque_pop

#define arginfo_class_Collections_Deque_toArray arginfo_class_Collections_Deque___serialize

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Collections_Deque_insert, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, values, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Collections_Deque_offsetGet, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Collections_Deque_offsetExists, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Collections_Deque_offsetSet, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Collections_Deque_offsetUnset, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Collections_Deque_jsonSerialize arginfo_class_Collections_Deque___serialize


ZEND_METHOD(Collections_Deque, __construct);
ZEND_METHOD(Collections_Deque, getIterator);
ZEND_METHOD(Collections_Deque, count);
ZEND_METHOD(Collections_Deque, isEmpty);
ZEND_METHOD(Collections_Deque, clear);
ZEND_METHOD(Collections_Deque, toArray);
ZEND_METHOD(Collections_Deque, __unserialize);
ZEND_METHOD(Collections_Deque, __set_state);
ZEND_METHOD(Collections_Deque, push);
ZEND_METHOD(Collections_Deque, pushFront);
ZEND_METHOD(Collections_Deque, pop);
ZEND_METHOD(Collections_Deque, popFront);
ZEND_METHOD(Collections_Deque, first);
ZEND_METHOD(Collections_Deque, last);
ZEND_METHOD(Collections_Deque, insert);
ZEND_METHOD(Collections_Deque, offsetGet);
ZEND_METHOD(Collections_Deque, offsetExists);
ZEND_METHOD(Collections_Deque, offsetSet);
ZEND_METHOD(Collections_Deque, offsetUnset);


static const zend_function_entry class_Collections_Deque_methods[] = {
	ZEND_ME(Collections_Deque, __construct, arginfo_class_Collections_Deque___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Collections_Deque, getIterator, arginfo_class_Collections_Deque_getIterator, ZEND_ACC_PUBLIC)
	ZEND_ME(Collections_Deque, count, arginfo_class_Collections_Deque_count, ZEND_ACC_PUBLIC)
	ZEND_ME(Collections_Deque, isEmpty, arginfo_class_Collections_Deque_isEmpty, ZEND_ACC_PUBLIC)
	ZEND_ME(Collections_Deque, clear, arginfo_class_Collections_Deque_clear, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Collections_Deque, __serialize, toArray, arginfo_class_Collections_Deque___serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Collections_Deque, __unserialize, arginfo_class_Collections_Deque___unserialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Collections_Deque, __set_state, arginfo_class_Collections_Deque___set_state, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Collections_Deque, push, arginfo_class_Collections_Deque_push, ZEND_ACC_PUBLIC)
	ZEND_ME(Collections_Deque, pushFront, arginfo_class_Collections_Deque_pushFront, ZEND_ACC_PUBLIC)
	ZEND_ME(Collections_Deque, pop, arginfo_class_Collections_Deque_pop, ZEND_ACC_PUBLIC)
	ZEND_ME(Collections_Deque, popFront, arginfo_class_Collections_Deque_popFront, ZEND_ACC_PUBLIC)
	ZEND_ME(Collections_Deque, first, arginfo_class_Collections_Deque_first, ZEND_ACC_PUBLIC)
	ZEND_ME(Collections_Deque, last, arginfo_class_Collections_Deque_last, ZEND_ACC_PUBLIC)
	ZEND_ME(Collections_Deque, toArray, arginfo_class_Collections_Deque_toArray, ZEND_ACC_PUBLIC)
	ZEND_ME(Collections_Deque, insert, arginfo_class_Collections_Deque_insert, ZEND_ACC_PUBLIC)
	ZEND_ME(Collections_Deque, offsetGet, arginfo_class_Collections_Deque_offsetGet, ZEND_ACC_PUBLIC)
	ZEND_ME(Collections_Deque, offsetExists, arginfo_class_Collections_Deque_offsetExists, ZEND_ACC_PUBLIC)
	ZEND_ME(Collections_Deque, offsetSet, arginfo_class_Collections_Deque_offsetSet, ZEND_ACC_PUBLIC)
	ZEND_ME(Collections_Deque, offsetUnset, arginfo_class_Collections_Deque_offsetUnset, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Collections_Deque, jsonSerialize, toArray, arginfo_class_Collections_Deque_jsonSerialize, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Collections_Deque(zend_class_entry *class_entry_IteratorAggregate, zend_class_entry *class_entry_Countable, zend_class_entry *class_entry_JsonSerializable, zend_class_entry *class_entry_ArrayAccess)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Collections", "Deque", class_Collections_Deque_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;
	zend_class_implements(class_entry, 4, class_entry_IteratorAggregate, class_entry_Countable, class_entry_JsonSerializable, class_entry_ArrayAccess);

	return class_entry;
}
