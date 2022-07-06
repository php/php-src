/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 510a58000a5473c4cbb33886f43b9f3050b3a36d */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplPriorityQueue_compare, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, priority1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, priority2, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplPriorityQueue_insert, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, priority, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplPriorityQueue_setExtractFlags, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplPriorityQueue_top, 0, 0, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SplPriorityQueue_extract arginfo_class_SplPriorityQueue_top

#define arginfo_class_SplPriorityQueue_count arginfo_class_SplPriorityQueue_top

#define arginfo_class_SplPriorityQueue_isEmpty arginfo_class_SplPriorityQueue_top

#define arginfo_class_SplPriorityQueue_rewind arginfo_class_SplPriorityQueue_top

#define arginfo_class_SplPriorityQueue_current arginfo_class_SplPriorityQueue_top

#define arginfo_class_SplPriorityQueue_key arginfo_class_SplPriorityQueue_top

#define arginfo_class_SplPriorityQueue_next arginfo_class_SplPriorityQueue_top

#define arginfo_class_SplPriorityQueue_valid arginfo_class_SplPriorityQueue_top

#define arginfo_class_SplPriorityQueue_recoverFromCorruption arginfo_class_SplPriorityQueue_top

#define arginfo_class_SplPriorityQueue_isCorrupted arginfo_class_SplPriorityQueue_top

#define arginfo_class_SplPriorityQueue_getExtractFlags arginfo_class_SplPriorityQueue_top

#define arginfo_class_SplPriorityQueue___debugInfo arginfo_class_SplPriorityQueue_top

#define arginfo_class_SplHeap_extract arginfo_class_SplPriorityQueue_top

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplHeap_insert, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SplHeap_top arginfo_class_SplPriorityQueue_top

#define arginfo_class_SplHeap_count arginfo_class_SplPriorityQueue_top

#define arginfo_class_SplHeap_isEmpty arginfo_class_SplPriorityQueue_top

#define arginfo_class_SplHeap_rewind arginfo_class_SplPriorityQueue_top

#define arginfo_class_SplHeap_current arginfo_class_SplPriorityQueue_top

#define arginfo_class_SplHeap_key arginfo_class_SplPriorityQueue_top

#define arginfo_class_SplHeap_next arginfo_class_SplPriorityQueue_top

#define arginfo_class_SplHeap_valid arginfo_class_SplPriorityQueue_top

#define arginfo_class_SplHeap_recoverFromCorruption arginfo_class_SplPriorityQueue_top

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplHeap_compare, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, value1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, value2, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SplHeap_isCorrupted arginfo_class_SplPriorityQueue_top

#define arginfo_class_SplHeap___debugInfo arginfo_class_SplPriorityQueue_top

#define arginfo_class_SplMinHeap_compare arginfo_class_SplHeap_compare

#define arginfo_class_SplMaxHeap_compare arginfo_class_SplHeap_compare


ZEND_METHOD(SplPriorityQueue, compare);
ZEND_METHOD(SplPriorityQueue, insert);
ZEND_METHOD(SplPriorityQueue, setExtractFlags);
ZEND_METHOD(SplPriorityQueue, top);
ZEND_METHOD(SplPriorityQueue, extract);
ZEND_METHOD(SplHeap, count);
ZEND_METHOD(SplHeap, isEmpty);
ZEND_METHOD(SplHeap, rewind);
ZEND_METHOD(SplPriorityQueue, current);
ZEND_METHOD(SplHeap, key);
ZEND_METHOD(SplHeap, next);
ZEND_METHOD(SplHeap, valid);
ZEND_METHOD(SplHeap, recoverFromCorruption);
ZEND_METHOD(SplHeap, isCorrupted);
ZEND_METHOD(SplPriorityQueue, getExtractFlags);
ZEND_METHOD(SplPriorityQueue, __debugInfo);
ZEND_METHOD(SplHeap, extract);
ZEND_METHOD(SplHeap, insert);
ZEND_METHOD(SplHeap, top);
ZEND_METHOD(SplHeap, current);
ZEND_METHOD(SplHeap, __debugInfo);
ZEND_METHOD(SplMinHeap, compare);
ZEND_METHOD(SplMaxHeap, compare);


static const zend_function_entry class_SplPriorityQueue_methods[] = {
	ZEND_ME(SplPriorityQueue, compare, arginfo_class_SplPriorityQueue_compare, ZEND_ACC_PUBLIC)
	ZEND_ME(SplPriorityQueue, insert, arginfo_class_SplPriorityQueue_insert, ZEND_ACC_PUBLIC)
	ZEND_ME(SplPriorityQueue, setExtractFlags, arginfo_class_SplPriorityQueue_setExtractFlags, ZEND_ACC_PUBLIC)
	ZEND_ME(SplPriorityQueue, top, arginfo_class_SplPriorityQueue_top, ZEND_ACC_PUBLIC)
	ZEND_ME(SplPriorityQueue, extract, arginfo_class_SplPriorityQueue_extract, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(SplHeap, count, count, arginfo_class_SplPriorityQueue_count, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(SplHeap, isEmpty, isEmpty, arginfo_class_SplPriorityQueue_isEmpty, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(SplHeap, rewind, rewind, arginfo_class_SplPriorityQueue_rewind, ZEND_ACC_PUBLIC)
	ZEND_ME(SplPriorityQueue, current, arginfo_class_SplPriorityQueue_current, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(SplHeap, key, key, arginfo_class_SplPriorityQueue_key, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(SplHeap, next, next, arginfo_class_SplPriorityQueue_next, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(SplHeap, valid, valid, arginfo_class_SplPriorityQueue_valid, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(SplHeap, recoverFromCorruption, recoverFromCorruption, arginfo_class_SplPriorityQueue_recoverFromCorruption, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(SplHeap, isCorrupted, isCorrupted, arginfo_class_SplPriorityQueue_isCorrupted, ZEND_ACC_PUBLIC)
	ZEND_ME(SplPriorityQueue, getExtractFlags, arginfo_class_SplPriorityQueue_getExtractFlags, ZEND_ACC_PUBLIC)
	ZEND_ME(SplPriorityQueue, __debugInfo, arginfo_class_SplPriorityQueue___debugInfo, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_SplHeap_methods[] = {
	ZEND_ME(SplHeap, extract, arginfo_class_SplHeap_extract, ZEND_ACC_PUBLIC)
	ZEND_ME(SplHeap, insert, arginfo_class_SplHeap_insert, ZEND_ACC_PUBLIC)
	ZEND_ME(SplHeap, top, arginfo_class_SplHeap_top, ZEND_ACC_PUBLIC)
	ZEND_ME(SplHeap, count, arginfo_class_SplHeap_count, ZEND_ACC_PUBLIC)
	ZEND_ME(SplHeap, isEmpty, arginfo_class_SplHeap_isEmpty, ZEND_ACC_PUBLIC)
	ZEND_ME(SplHeap, rewind, arginfo_class_SplHeap_rewind, ZEND_ACC_PUBLIC)
	ZEND_ME(SplHeap, current, arginfo_class_SplHeap_current, ZEND_ACC_PUBLIC)
	ZEND_ME(SplHeap, key, arginfo_class_SplHeap_key, ZEND_ACC_PUBLIC)
	ZEND_ME(SplHeap, next, arginfo_class_SplHeap_next, ZEND_ACC_PUBLIC)
	ZEND_ME(SplHeap, valid, arginfo_class_SplHeap_valid, ZEND_ACC_PUBLIC)
	ZEND_ME(SplHeap, recoverFromCorruption, arginfo_class_SplHeap_recoverFromCorruption, ZEND_ACC_PUBLIC)
	ZEND_ABSTRACT_ME_WITH_FLAGS(SplHeap, compare, arginfo_class_SplHeap_compare, ZEND_ACC_PROTECTED|ZEND_ACC_ABSTRACT)
	ZEND_ME(SplHeap, isCorrupted, arginfo_class_SplHeap_isCorrupted, ZEND_ACC_PUBLIC)
	ZEND_ME(SplHeap, __debugInfo, arginfo_class_SplHeap___debugInfo, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_SplMinHeap_methods[] = {
	ZEND_ME(SplMinHeap, compare, arginfo_class_SplMinHeap_compare, ZEND_ACC_PROTECTED)
	ZEND_FE_END
};


static const zend_function_entry class_SplMaxHeap_methods[] = {
	ZEND_ME(SplMaxHeap, compare, arginfo_class_SplMaxHeap_compare, ZEND_ACC_PROTECTED)
	ZEND_FE_END
};
