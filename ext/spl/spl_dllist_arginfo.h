/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 404df73577d733275e386226e11887b37a500ade */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplDoublyLinkedList_add, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplDoublyLinkedList_pop, 0, 0, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SplDoublyLinkedList_shift arginfo_class_SplDoublyLinkedList_pop

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplDoublyLinkedList_push, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SplDoublyLinkedList_unshift arginfo_class_SplDoublyLinkedList_push

#define arginfo_class_SplDoublyLinkedList_top arginfo_class_SplDoublyLinkedList_pop

#define arginfo_class_SplDoublyLinkedList_bottom arginfo_class_SplDoublyLinkedList_pop

#define arginfo_class_SplDoublyLinkedList___debugInfo arginfo_class_SplDoublyLinkedList_pop

#define arginfo_class_SplDoublyLinkedList_count arginfo_class_SplDoublyLinkedList_pop

#define arginfo_class_SplDoublyLinkedList_isEmpty arginfo_class_SplDoublyLinkedList_pop

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplDoublyLinkedList_setIteratorMode, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, mode, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SplDoublyLinkedList_getIteratorMode arginfo_class_SplDoublyLinkedList_pop

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplDoublyLinkedList_offsetExists, 0, 0, 1)
	ZEND_ARG_INFO(0, index)
ZEND_END_ARG_INFO()

#define arginfo_class_SplDoublyLinkedList_offsetGet arginfo_class_SplDoublyLinkedList_offsetExists

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplDoublyLinkedList_offsetSet, 0, 0, 2)
	ZEND_ARG_INFO(0, index)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SplDoublyLinkedList_offsetUnset arginfo_class_SplDoublyLinkedList_offsetExists

#define arginfo_class_SplDoublyLinkedList_rewind arginfo_class_SplDoublyLinkedList_pop

#define arginfo_class_SplDoublyLinkedList_current arginfo_class_SplDoublyLinkedList_pop

#define arginfo_class_SplDoublyLinkedList_key arginfo_class_SplDoublyLinkedList_pop

#define arginfo_class_SplDoublyLinkedList_prev arginfo_class_SplDoublyLinkedList_pop

#define arginfo_class_SplDoublyLinkedList_next arginfo_class_SplDoublyLinkedList_pop

#define arginfo_class_SplDoublyLinkedList_valid arginfo_class_SplDoublyLinkedList_pop

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplDoublyLinkedList_unserialize, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SplDoublyLinkedList_serialize arginfo_class_SplDoublyLinkedList_pop

#define arginfo_class_SplDoublyLinkedList___serialize arginfo_class_SplDoublyLinkedList_pop

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplDoublyLinkedList___unserialize, 0, 0, 1)
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
