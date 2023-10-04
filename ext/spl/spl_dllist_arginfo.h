/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 9969bd9f785db905720950ae349b903ec80d1f75 */

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
	ZEND_ME(SplDoublyLinkedList, add, arginfo_class_SplDoublyLinkedList_add, ZEND_ACC_PUBLIC)
	ZEND_ME(SplDoublyLinkedList, pop, arginfo_class_SplDoublyLinkedList_pop, ZEND_ACC_PUBLIC)
	ZEND_ME(SplDoublyLinkedList, shift, arginfo_class_SplDoublyLinkedList_shift, ZEND_ACC_PUBLIC)
	ZEND_ME(SplDoublyLinkedList, push, arginfo_class_SplDoublyLinkedList_push, ZEND_ACC_PUBLIC)
	ZEND_ME(SplDoublyLinkedList, unshift, arginfo_class_SplDoublyLinkedList_unshift, ZEND_ACC_PUBLIC)
	ZEND_ME(SplDoublyLinkedList, top, arginfo_class_SplDoublyLinkedList_top, ZEND_ACC_PUBLIC)
	ZEND_ME(SplDoublyLinkedList, bottom, arginfo_class_SplDoublyLinkedList_bottom, ZEND_ACC_PUBLIC)
	ZEND_ME(SplDoublyLinkedList, __debugInfo, arginfo_class_SplDoublyLinkedList___debugInfo, ZEND_ACC_PUBLIC)
	ZEND_ME(SplDoublyLinkedList, count, arginfo_class_SplDoublyLinkedList_count, ZEND_ACC_PUBLIC)
	ZEND_ME(SplDoublyLinkedList, isEmpty, arginfo_class_SplDoublyLinkedList_isEmpty, ZEND_ACC_PUBLIC)
	ZEND_ME(SplDoublyLinkedList, setIteratorMode, arginfo_class_SplDoublyLinkedList_setIteratorMode, ZEND_ACC_PUBLIC)
	ZEND_ME(SplDoublyLinkedList, getIteratorMode, arginfo_class_SplDoublyLinkedList_getIteratorMode, ZEND_ACC_PUBLIC)
	ZEND_ME(SplDoublyLinkedList, offsetExists, arginfo_class_SplDoublyLinkedList_offsetExists, ZEND_ACC_PUBLIC)
	ZEND_ME(SplDoublyLinkedList, offsetGet, arginfo_class_SplDoublyLinkedList_offsetGet, ZEND_ACC_PUBLIC)
	ZEND_ME(SplDoublyLinkedList, offsetSet, arginfo_class_SplDoublyLinkedList_offsetSet, ZEND_ACC_PUBLIC)
	ZEND_ME(SplDoublyLinkedList, offsetUnset, arginfo_class_SplDoublyLinkedList_offsetUnset, ZEND_ACC_PUBLIC)
	ZEND_ME(SplDoublyLinkedList, rewind, arginfo_class_SplDoublyLinkedList_rewind, ZEND_ACC_PUBLIC)
	ZEND_ME(SplDoublyLinkedList, current, arginfo_class_SplDoublyLinkedList_current, ZEND_ACC_PUBLIC)
	ZEND_ME(SplDoublyLinkedList, key, arginfo_class_SplDoublyLinkedList_key, ZEND_ACC_PUBLIC)
	ZEND_ME(SplDoublyLinkedList, prev, arginfo_class_SplDoublyLinkedList_prev, ZEND_ACC_PUBLIC)
	ZEND_ME(SplDoublyLinkedList, next, arginfo_class_SplDoublyLinkedList_next, ZEND_ACC_PUBLIC)
	ZEND_ME(SplDoublyLinkedList, valid, arginfo_class_SplDoublyLinkedList_valid, ZEND_ACC_PUBLIC)
	ZEND_ME(SplDoublyLinkedList, unserialize, arginfo_class_SplDoublyLinkedList_unserialize, ZEND_ACC_PUBLIC)
	ZEND_ME(SplDoublyLinkedList, serialize, arginfo_class_SplDoublyLinkedList_serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(SplDoublyLinkedList, __serialize, arginfo_class_SplDoublyLinkedList___serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(SplDoublyLinkedList, __unserialize, arginfo_class_SplDoublyLinkedList___unserialize, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_SplQueue_methods[] = {
	ZEND_MALIAS(SplDoublyLinkedList, enqueue, push, arginfo_class_SplQueue_enqueue, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(SplDoublyLinkedList, dequeue, shift, arginfo_class_SplQueue_dequeue, ZEND_ACC_PUBLIC)
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
