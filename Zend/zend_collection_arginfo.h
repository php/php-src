/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 81c3150a5c601ea8866a8efe1dd6b6621c1dd194 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_SeqCollection_add, 0, 1, IS_STATIC, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_SeqCollection_remove, 0, 1, IS_STATIC, 0)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_SeqCollection_has, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_SeqCollection_get, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SeqCollection_with arginfo_class_SeqCollection_add

#define arginfo_class_SeqCollection_without arginfo_class_SeqCollection_remove

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_SeqCollection_set, 0, 2, IS_STATIC, 0)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_SeqCollection_concat, 0, 1, IS_STATIC, 0)
	ZEND_ARG_TYPE_INFO(0, other, IS_OBJECT, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_SeqCollection_equals, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, other, IS_OBJECT, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_SeqCollection_map, 0, 2, IS_OBJECT, 0)
	ZEND_ARG_TYPE_INFO(0, fn, IS_CALLABLE, 0)
	ZEND_ARG_TYPE_INFO(0, targetType, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DictCollection_add, 0, 2, IS_STATIC, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DictCollection_remove, 0, 1, IS_STATIC, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DictCollection_has, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DictCollection_get, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DictCollection_with arginfo_class_DictCollection_add

#define arginfo_class_DictCollection_without arginfo_class_DictCollection_remove

#define arginfo_class_DictCollection_set arginfo_class_DictCollection_add

#define arginfo_class_DictCollection_concat arginfo_class_SeqCollection_concat

#define arginfo_class_DictCollection_equals arginfo_class_SeqCollection_equals

#define arginfo_class_DictCollection_map arginfo_class_SeqCollection_map




static const zend_function_entry class_SeqCollection_methods[] = {
	ZEND_ABSTRACT_ME_WITH_FLAGS(SeqCollection, add, arginfo_class_SeqCollection_add, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(SeqCollection, remove, arginfo_class_SeqCollection_remove, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(SeqCollection, has, arginfo_class_SeqCollection_has, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(SeqCollection, get, arginfo_class_SeqCollection_get, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(SeqCollection, with, arginfo_class_SeqCollection_with, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(SeqCollection, without, arginfo_class_SeqCollection_without, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(SeqCollection, set, arginfo_class_SeqCollection_set, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(SeqCollection, concat, arginfo_class_SeqCollection_concat, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(SeqCollection, equals, arginfo_class_SeqCollection_equals, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(SeqCollection, map, arginfo_class_SeqCollection_map, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_FE_END
};


static const zend_function_entry class_DictCollection_methods[] = {
	ZEND_ABSTRACT_ME_WITH_FLAGS(DictCollection, add, arginfo_class_DictCollection_add, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(DictCollection, remove, arginfo_class_DictCollection_remove, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(DictCollection, has, arginfo_class_DictCollection_has, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(DictCollection, get, arginfo_class_DictCollection_get, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(DictCollection, with, arginfo_class_DictCollection_with, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(DictCollection, without, arginfo_class_DictCollection_without, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(DictCollection, set, arginfo_class_DictCollection_set, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(DictCollection, concat, arginfo_class_DictCollection_concat, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(DictCollection, equals, arginfo_class_DictCollection_equals, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(DictCollection, map, arginfo_class_DictCollection_map, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_FE_END
};

static zend_class_entry *register_class_SeqCollection(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "SeqCollection", class_SeqCollection_methods);
	class_entry = zend_register_internal_interface(&ce);

	return class_entry;
}

static zend_class_entry *register_class_DictCollection(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "DictCollection", class_DictCollection_methods);
	class_entry = zend_register_internal_interface(&ce);

	return class_entry;
}
