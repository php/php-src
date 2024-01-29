/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 45e42d3a0589031651daee5653900d5a4fb61c3d */

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplDoublyLinkedList_add, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplDoublyLinkedList_pop, 0, 0, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SplDoublyLinkedList_shift arginfo_class_SplDoublyLinkedList_pop

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplDoublyLinkedList_push, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SplDoublyLinkedList_unshift arginfo_class_SplDoublyLinkedList_push

#define arginfo_class_SplDoublyLinkedList_top arginfo_class_SplDoublyLinkedList_pop

#define arginfo_class_SplDoublyLinkedList_bottom arginfo_class_SplDoublyLinkedList_pop

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplDoublyLinkedList___debugInfo, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplDoublyLinkedList_count, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplDoublyLinkedList_isEmpty, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplDoublyLinkedList_setIteratorMode, 0, 1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, mode, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SplDoublyLinkedList_getIteratorMode arginfo_class_SplDoublyLinkedList_count

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplDoublyLinkedList_offsetExists, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, index)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplDoublyLinkedList_offsetGet, 0, 1, IS_MIXED, 0)
	ZEND_ARG_INFO(0, index)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplDoublyLinkedList_offsetSet, 0, 2, IS_VOID, 0)
	ZEND_ARG_INFO(0, index)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplDoublyLinkedList_offsetUnset, 0, 1, IS_VOID, 0)
	ZEND_ARG_INFO(0, index)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplDoublyLinkedList_rewind, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SplDoublyLinkedList_current arginfo_class_SplDoublyLinkedList_pop

#define arginfo_class_SplDoublyLinkedList_key arginfo_class_SplDoublyLinkedList_count

#define arginfo_class_SplDoublyLinkedList_prev arginfo_class_SplDoublyLinkedList_rewind

#define arginfo_class_SplDoublyLinkedList_next arginfo_class_SplDoublyLinkedList_rewind

#define arginfo_class_SplDoublyLinkedList_valid arginfo_class_SplDoublyLinkedList_isEmpty

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplDoublyLinkedList_unserialize, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplDoublyLinkedList_serialize, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SplDoublyLinkedList___serialize arginfo_class_SplDoublyLinkedList___debugInfo

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplDoublyLinkedList___unserialize, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SplQueue_enqueue arginfo_class_SplDoublyLinkedList_push

#define arginfo_class_SplQueue_dequeue arginfo_class_SplDoublyLinkedList_pop

ZEND_METHOD(SplDoublyLinkedList, add);
ZEND_METHOD(SplDoublyLinkedList, pop);
ZEND_METHOD(SplDoublyLinkedList, shift);
ZEND_METHOD(SplDoublyLinkedList, push);
ZEND_METHOD(SplDoublyLinkedList, unshift);
ZEND_METHOD(SplDoublyLinkedList, top);
ZEND_METHOD(SplDoublyLinkedList, bottom);
ZEND_METHOD(SplDoublyLinkedList, __debugInfo);
ZEND_METHOD(SplDoublyLinkedList, count);
ZEND_METHOD(SplDoublyLinkedList, isEmpty);
ZEND_METHOD(SplDoublyLinkedList, setIteratorMode);
ZEND_METHOD(SplDoublyLinkedList, getIteratorMode);
ZEND_METHOD(SplDoublyLinkedList, offsetExists);
ZEND_METHOD(SplDoublyLinkedList, offsetGet);
ZEND_METHOD(SplDoublyLinkedList, offsetSet);
ZEND_METHOD(SplDoublyLinkedList, offsetUnset);
ZEND_METHOD(SplDoublyLinkedList, rewind);
ZEND_METHOD(SplDoublyLinkedList, current);
ZEND_METHOD(SplDoublyLinkedList, key);
ZEND_METHOD(SplDoublyLinkedList, prev);
ZEND_METHOD(SplDoublyLinkedList, next);
ZEND_METHOD(SplDoublyLinkedList, valid);
ZEND_METHOD(SplDoublyLinkedList, unserialize);
ZEND_METHOD(SplDoublyLinkedList, serialize);
ZEND_METHOD(SplDoublyLinkedList, __serialize);
ZEND_METHOD(SplDoublyLinkedList, __unserialize);

static const zend_function_entry class_SplDoublyLinkedList_methods[] = {
	ZEND_RAW_FENTRY("add", zim_SplDoublyLinkedList_add, arginfo_class_SplDoublyLinkedList_add, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("pop", zim_SplDoublyLinkedList_pop, arginfo_class_SplDoublyLinkedList_pop, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("shift", zim_SplDoublyLinkedList_shift, arginfo_class_SplDoublyLinkedList_shift, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("push", zim_SplDoublyLinkedList_push, arginfo_class_SplDoublyLinkedList_push, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("unshift", zim_SplDoublyLinkedList_unshift, arginfo_class_SplDoublyLinkedList_unshift, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("top", zim_SplDoublyLinkedList_top, arginfo_class_SplDoublyLinkedList_top, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("bottom", zim_SplDoublyLinkedList_bottom, arginfo_class_SplDoublyLinkedList_bottom, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("__debugInfo", zim_SplDoublyLinkedList___debugInfo, arginfo_class_SplDoublyLinkedList___debugInfo, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("count", zim_SplDoublyLinkedList_count, arginfo_class_SplDoublyLinkedList_count, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("isEmpty", zim_SplDoublyLinkedList_isEmpty, arginfo_class_SplDoublyLinkedList_isEmpty, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("setIteratorMode", zim_SplDoublyLinkedList_setIteratorMode, arginfo_class_SplDoublyLinkedList_setIteratorMode, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("getIteratorMode", zim_SplDoublyLinkedList_getIteratorMode, arginfo_class_SplDoublyLinkedList_getIteratorMode, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("offsetExists", zim_SplDoublyLinkedList_offsetExists, arginfo_class_SplDoublyLinkedList_offsetExists, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("offsetGet", zim_SplDoublyLinkedList_offsetGet, arginfo_class_SplDoublyLinkedList_offsetGet, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("offsetSet", zim_SplDoublyLinkedList_offsetSet, arginfo_class_SplDoublyLinkedList_offsetSet, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("offsetUnset", zim_SplDoublyLinkedList_offsetUnset, arginfo_class_SplDoublyLinkedList_offsetUnset, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("rewind", zim_SplDoublyLinkedList_rewind, arginfo_class_SplDoublyLinkedList_rewind, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("current", zim_SplDoublyLinkedList_current, arginfo_class_SplDoublyLinkedList_current, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("key", zim_SplDoublyLinkedList_key, arginfo_class_SplDoublyLinkedList_key, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("prev", zim_SplDoublyLinkedList_prev, arginfo_class_SplDoublyLinkedList_prev, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("next", zim_SplDoublyLinkedList_next, arginfo_class_SplDoublyLinkedList_next, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("valid", zim_SplDoublyLinkedList_valid, arginfo_class_SplDoublyLinkedList_valid, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("unserialize", zim_SplDoublyLinkedList_unserialize, arginfo_class_SplDoublyLinkedList_unserialize, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("serialize", zim_SplDoublyLinkedList_serialize, arginfo_class_SplDoublyLinkedList_serialize, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("__serialize", zim_SplDoublyLinkedList___serialize, arginfo_class_SplDoublyLinkedList___serialize, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("__unserialize", zim_SplDoublyLinkedList___unserialize, arginfo_class_SplDoublyLinkedList___unserialize, ZEND_ACC_PUBLIC, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_SplQueue_methods[] = {
	ZEND_RAW_FENTRY("enqueue", zim_SplDoublyLinkedList_push, arginfo_class_SplQueue_enqueue, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("dequeue", zim_SplDoublyLinkedList_shift, arginfo_class_SplQueue_dequeue, ZEND_ACC_PUBLIC, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_SplStack_methods[] = {
	ZEND_FE_END
};

static zend_class_entry *register_class_SplDoublyLinkedList(zend_class_entry *class_entry_Iterator, zend_class_entry *class_entry_Countable, zend_class_entry *class_entry_ArrayAccess, zend_class_entry *class_entry_Serializable)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "SplDoublyLinkedList", class_SplDoublyLinkedList_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	zend_class_implements(class_entry, 4, class_entry_Iterator, class_entry_Countable, class_entry_ArrayAccess, class_entry_Serializable);

	zval const_IT_MODE_LIFO_value;
	ZVAL_LONG(&const_IT_MODE_LIFO_value, SPL_DLLIST_IT_LIFO);
	zend_string *const_IT_MODE_LIFO_name = zend_string_init_interned("IT_MODE_LIFO", sizeof("IT_MODE_LIFO") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_IT_MODE_LIFO_name, &const_IT_MODE_LIFO_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_IT_MODE_LIFO_name);

	zval const_IT_MODE_FIFO_value;
	ZVAL_LONG(&const_IT_MODE_FIFO_value, SPL_DLLIST_IT_FIFO);
	zend_string *const_IT_MODE_FIFO_name = zend_string_init_interned("IT_MODE_FIFO", sizeof("IT_MODE_FIFO") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_IT_MODE_FIFO_name, &const_IT_MODE_FIFO_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_IT_MODE_FIFO_name);

	zval const_IT_MODE_DELETE_value;
	ZVAL_LONG(&const_IT_MODE_DELETE_value, SPL_DLLIST_IT_DELETE);
	zend_string *const_IT_MODE_DELETE_name = zend_string_init_interned("IT_MODE_DELETE", sizeof("IT_MODE_DELETE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_IT_MODE_DELETE_name, &const_IT_MODE_DELETE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_IT_MODE_DELETE_name);

	zval const_IT_MODE_KEEP_value;
	ZVAL_LONG(&const_IT_MODE_KEEP_value, SPL_DLLIST_IT_KEEP);
	zend_string *const_IT_MODE_KEEP_name = zend_string_init_interned("IT_MODE_KEEP", sizeof("IT_MODE_KEEP") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_IT_MODE_KEEP_name, &const_IT_MODE_KEEP_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_IT_MODE_KEEP_name);

	return class_entry;
}

static zend_class_entry *register_class_SplQueue(zend_class_entry *class_entry_SplDoublyLinkedList)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "SplQueue", class_SplQueue_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_SplDoublyLinkedList);

	return class_entry;
}

static zend_class_entry *register_class_SplStack(zend_class_entry *class_entry_SplDoublyLinkedList)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "SplStack", class_SplStack_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_SplDoublyLinkedList);

	return class_entry;
}
