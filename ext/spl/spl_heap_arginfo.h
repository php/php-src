/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 47273e114c9c7089bf708a2f18f2e9e522abceb6 */

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplPriorityQueue_compare, 0, 2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, priority1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, priority2, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplPriorityQueue_insert, 0, 2, IS_TRUE, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, priority, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplPriorityQueue_setExtractFlags, 0, 1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplPriorityQueue_top, 0, 0, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SplPriorityQueue_extract arginfo_class_SplPriorityQueue_top

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplPriorityQueue_count, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplPriorityQueue_isEmpty, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplPriorityQueue_rewind, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SplPriorityQueue_current arginfo_class_SplPriorityQueue_top

#define arginfo_class_SplPriorityQueue_key arginfo_class_SplPriorityQueue_count

#define arginfo_class_SplPriorityQueue_next arginfo_class_SplPriorityQueue_rewind

#define arginfo_class_SplPriorityQueue_valid arginfo_class_SplPriorityQueue_isEmpty

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplPriorityQueue_recoverFromCorruption, 0, 0, IS_TRUE, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SplPriorityQueue_isCorrupted arginfo_class_SplPriorityQueue_isEmpty

#define arginfo_class_SplPriorityQueue_getExtractFlags arginfo_class_SplPriorityQueue_count

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplPriorityQueue___debugInfo, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SplHeap_extract arginfo_class_SplPriorityQueue_top

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplHeap_insert, 0, 1, IS_TRUE, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SplHeap_top arginfo_class_SplPriorityQueue_top

#define arginfo_class_SplHeap_count arginfo_class_SplPriorityQueue_count

#define arginfo_class_SplHeap_isEmpty arginfo_class_SplPriorityQueue_isEmpty

#define arginfo_class_SplHeap_rewind arginfo_class_SplPriorityQueue_rewind

#define arginfo_class_SplHeap_current arginfo_class_SplPriorityQueue_top

#define arginfo_class_SplHeap_key arginfo_class_SplPriorityQueue_count

#define arginfo_class_SplHeap_next arginfo_class_SplPriorityQueue_rewind

#define arginfo_class_SplHeap_valid arginfo_class_SplPriorityQueue_isEmpty

#define arginfo_class_SplHeap_recoverFromCorruption arginfo_class_SplPriorityQueue_recoverFromCorruption

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplHeap_compare, 0, 2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, value2, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SplHeap_isCorrupted arginfo_class_SplPriorityQueue_isEmpty

#define arginfo_class_SplHeap___debugInfo arginfo_class_SplPriorityQueue___debugInfo

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
	ZEND_RAW_FENTRY("compare", zim_SplPriorityQueue_compare, arginfo_class_SplPriorityQueue_compare, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("insert", zim_SplPriorityQueue_insert, arginfo_class_SplPriorityQueue_insert, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("setExtractFlags", zim_SplPriorityQueue_setExtractFlags, arginfo_class_SplPriorityQueue_setExtractFlags, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("top", zim_SplPriorityQueue_top, arginfo_class_SplPriorityQueue_top, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("extract", zim_SplPriorityQueue_extract, arginfo_class_SplPriorityQueue_extract, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("count", zim_SplHeap_count, arginfo_class_SplPriorityQueue_count, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("isEmpty", zim_SplHeap_isEmpty, arginfo_class_SplPriorityQueue_isEmpty, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("rewind", zim_SplHeap_rewind, arginfo_class_SplPriorityQueue_rewind, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("current", zim_SplPriorityQueue_current, arginfo_class_SplPriorityQueue_current, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("key", zim_SplHeap_key, arginfo_class_SplPriorityQueue_key, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("next", zim_SplHeap_next, arginfo_class_SplPriorityQueue_next, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("valid", zim_SplHeap_valid, arginfo_class_SplPriorityQueue_valid, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("recoverFromCorruption", zim_SplHeap_recoverFromCorruption, arginfo_class_SplPriorityQueue_recoverFromCorruption, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("isCorrupted", zim_SplHeap_isCorrupted, arginfo_class_SplPriorityQueue_isCorrupted, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("getExtractFlags", zim_SplPriorityQueue_getExtractFlags, arginfo_class_SplPriorityQueue_getExtractFlags, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("__debugInfo", zim_SplPriorityQueue___debugInfo, arginfo_class_SplPriorityQueue___debugInfo, ZEND_ACC_PUBLIC, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_SplHeap_methods[] = {
	ZEND_RAW_FENTRY("extract", zim_SplHeap_extract, arginfo_class_SplHeap_extract, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("insert", zim_SplHeap_insert, arginfo_class_SplHeap_insert, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("top", zim_SplHeap_top, arginfo_class_SplHeap_top, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("count", zim_SplHeap_count, arginfo_class_SplHeap_count, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("isEmpty", zim_SplHeap_isEmpty, arginfo_class_SplHeap_isEmpty, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("rewind", zim_SplHeap_rewind, arginfo_class_SplHeap_rewind, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("current", zim_SplHeap_current, arginfo_class_SplHeap_current, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("key", zim_SplHeap_key, arginfo_class_SplHeap_key, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("next", zim_SplHeap_next, arginfo_class_SplHeap_next, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("valid", zim_SplHeap_valid, arginfo_class_SplHeap_valid, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("recoverFromCorruption", zim_SplHeap_recoverFromCorruption, arginfo_class_SplHeap_recoverFromCorruption, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("compare", NULL, arginfo_class_SplHeap_compare, ZEND_ACC_PROTECTED|ZEND_ACC_ABSTRACT, NULL)
	ZEND_RAW_FENTRY("isCorrupted", zim_SplHeap_isCorrupted, arginfo_class_SplHeap_isCorrupted, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("__debugInfo", zim_SplHeap___debugInfo, arginfo_class_SplHeap___debugInfo, ZEND_ACC_PUBLIC, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_SplMinHeap_methods[] = {
	ZEND_RAW_FENTRY("compare", zim_SplMinHeap_compare, arginfo_class_SplMinHeap_compare, ZEND_ACC_PROTECTED, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_SplMaxHeap_methods[] = {
	ZEND_RAW_FENTRY("compare", zim_SplMaxHeap_compare, arginfo_class_SplMaxHeap_compare, ZEND_ACC_PROTECTED, NULL)
	ZEND_FE_END
};

static zend_class_entry *register_class_SplPriorityQueue(zend_class_entry *class_entry_Iterator, zend_class_entry *class_entry_Countable)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "SplPriorityQueue", class_SplPriorityQueue_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	zend_class_implements(class_entry, 2, class_entry_Iterator, class_entry_Countable);

	zval const_EXTR_BOTH_value;
	ZVAL_LONG(&const_EXTR_BOTH_value, SPL_PQUEUE_EXTR_BOTH);
	zend_string *const_EXTR_BOTH_name = zend_string_init_interned("EXTR_BOTH", sizeof("EXTR_BOTH") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_EXTR_BOTH_name, &const_EXTR_BOTH_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_EXTR_BOTH_name);

	zval const_EXTR_PRIORITY_value;
	ZVAL_LONG(&const_EXTR_PRIORITY_value, SPL_PQUEUE_EXTR_PRIORITY);
	zend_string *const_EXTR_PRIORITY_name = zend_string_init_interned("EXTR_PRIORITY", sizeof("EXTR_PRIORITY") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_EXTR_PRIORITY_name, &const_EXTR_PRIORITY_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_EXTR_PRIORITY_name);

	zval const_EXTR_DATA_value;
	ZVAL_LONG(&const_EXTR_DATA_value, SPL_PQUEUE_EXTR_DATA);
	zend_string *const_EXTR_DATA_name = zend_string_init_interned("EXTR_DATA", sizeof("EXTR_DATA") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_EXTR_DATA_name, &const_EXTR_DATA_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_EXTR_DATA_name);

	return class_entry;
}

static zend_class_entry *register_class_SplHeap(zend_class_entry *class_entry_Iterator, zend_class_entry *class_entry_Countable)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "SplHeap", class_SplHeap_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_ABSTRACT;
	zend_class_implements(class_entry, 2, class_entry_Iterator, class_entry_Countable);

	return class_entry;
}

static zend_class_entry *register_class_SplMinHeap(zend_class_entry *class_entry_SplHeap)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "SplMinHeap", class_SplMinHeap_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_SplHeap);

	return class_entry;
}

static zend_class_entry *register_class_SplMaxHeap(zend_class_entry *class_entry_SplHeap)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "SplMaxHeap", class_SplMaxHeap_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_SplHeap);

	return class_entry;
}
