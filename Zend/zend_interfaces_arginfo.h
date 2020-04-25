/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IteratorAggregate_getIterator, 0, 0, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Iterator_current arginfo_class_IteratorAggregate_getIterator

#define arginfo_class_Iterator_next arginfo_class_IteratorAggregate_getIterator

#define arginfo_class_Iterator_key arginfo_class_IteratorAggregate_getIterator

#define arginfo_class_Iterator_valid arginfo_class_IteratorAggregate_getIterator

#define arginfo_class_Iterator_rewind arginfo_class_IteratorAggregate_getIterator

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ArrayAccess_offsetExists, 0, 0, 1)
	ZEND_ARG_INFO(0, offset)
ZEND_END_ARG_INFO()

#define arginfo_class_ArrayAccess_offsetGet arginfo_class_ArrayAccess_offsetExists

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ArrayAccess_offsetSet, 0, 0, 2)
	ZEND_ARG_INFO(0, offset)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

#define arginfo_class_ArrayAccess_offsetUnset arginfo_class_ArrayAccess_offsetExists

#define arginfo_class_Serializable_serialize arginfo_class_IteratorAggregate_getIterator

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Serializable_unserialize, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, serialized, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Countable_count arginfo_class_IteratorAggregate_getIterator

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Stringable___toString, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()




static const zend_function_entry class_Traversable_methods[] = {
	ZEND_FE_END
};


static const zend_function_entry class_IteratorAggregate_methods[] = {
	ZEND_ABSTRACT_ME_WITH_FLAGS(IteratorAggregate, getIterator, arginfo_class_IteratorAggregate_getIterator, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_FE_END
};


static const zend_function_entry class_Iterator_methods[] = {
	ZEND_ABSTRACT_ME_WITH_FLAGS(Iterator, current, arginfo_class_Iterator_current, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(Iterator, next, arginfo_class_Iterator_next, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(Iterator, key, arginfo_class_Iterator_key, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(Iterator, valid, arginfo_class_Iterator_valid, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(Iterator, rewind, arginfo_class_Iterator_rewind, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_FE_END
};


static const zend_function_entry class_ArrayAccess_methods[] = {
	ZEND_ABSTRACT_ME_WITH_FLAGS(ArrayAccess, offsetExists, arginfo_class_ArrayAccess_offsetExists, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(ArrayAccess, offsetGet, arginfo_class_ArrayAccess_offsetGet, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(ArrayAccess, offsetSet, arginfo_class_ArrayAccess_offsetSet, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(ArrayAccess, offsetUnset, arginfo_class_ArrayAccess_offsetUnset, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_FE_END
};


static const zend_function_entry class_Serializable_methods[] = {
	ZEND_ABSTRACT_ME_WITH_FLAGS(Serializable, serialize, arginfo_class_Serializable_serialize, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(Serializable, unserialize, arginfo_class_Serializable_unserialize, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_FE_END
};


static const zend_function_entry class_Countable_methods[] = {
	ZEND_ABSTRACT_ME_WITH_FLAGS(Countable, count, arginfo_class_Countable_count, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_FE_END
};


static const zend_function_entry class_Stringable_methods[] = {
	ZEND_ABSTRACT_ME_WITH_FLAGS(Stringable, __toString, arginfo_class_Stringable___toString, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_FE_END
};
