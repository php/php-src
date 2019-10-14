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
