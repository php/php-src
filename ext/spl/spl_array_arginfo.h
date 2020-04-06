/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ArrayObject___construct, 0, 0, 0)
	ZEND_ARG_INFO(0, input)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, iterator_class, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ArrayObject_offsetExists, 0, 0, 1)
	ZEND_ARG_INFO(0, index)
ZEND_END_ARG_INFO()

#define arginfo_class_ArrayObject_offsetGet arginfo_class_ArrayObject_offsetExists

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ArrayObject_offsetSet, 0, 0, 2)
	ZEND_ARG_INFO(0, index)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

#define arginfo_class_ArrayObject_offsetUnset arginfo_class_ArrayObject_offsetExists

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ArrayObject_append, 0, 0, 1)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ArrayObject_getArrayCopy, 0, 0, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ArrayObject_count arginfo_class_ArrayObject_getArrayCopy

#define arginfo_class_ArrayObject_getFlags arginfo_class_ArrayObject_getArrayCopy

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ArrayObject_setFlags, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ArrayObject_asort, 0, 0, 0)
	ZEND_ARG_INFO(0, sort_flags)
ZEND_END_ARG_INFO()

#define arginfo_class_ArrayObject_ksort arginfo_class_ArrayObject_asort

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ArrayObject_uasort, 0, 0, 1)
	ZEND_ARG_INFO(0, cmp_function)
ZEND_END_ARG_INFO()

#define arginfo_class_ArrayObject_uksort arginfo_class_ArrayObject_uasort

#define arginfo_class_ArrayObject_natsort arginfo_class_ArrayObject_getArrayCopy

#define arginfo_class_ArrayObject_natcasesort arginfo_class_ArrayObject_getArrayCopy

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ArrayObject_unserialize, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, serialized, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ArrayObject_serialize arginfo_class_ArrayObject_getArrayCopy

#define arginfo_class_ArrayObject___serialize arginfo_class_ArrayObject_getArrayCopy

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ArrayObject___unserialize, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ArrayObject_getIterator arginfo_class_ArrayObject_getArrayCopy

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ArrayObject_exchangeArray, 0, 0, 1)
	ZEND_ARG_INFO(0, input)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ArrayObject_setIteratorClass, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, iteratorClass, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ArrayObject_getIteratorClass arginfo_class_ArrayObject_getArrayCopy

#define arginfo_class_ArrayObject___debugInfo arginfo_class_ArrayObject_getArrayCopy

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ArrayIterator___construct, 0, 0, 0)
	ZEND_ARG_INFO(0, array)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ArrayIterator_offsetExists arginfo_class_ArrayObject_offsetExists

#define arginfo_class_ArrayIterator_offsetGet arginfo_class_ArrayObject_offsetExists

#define arginfo_class_ArrayIterator_offsetSet arginfo_class_ArrayObject_offsetSet

#define arginfo_class_ArrayIterator_offsetUnset arginfo_class_ArrayObject_offsetExists

#define arginfo_class_ArrayIterator_append arginfo_class_ArrayObject_append

#define arginfo_class_ArrayIterator_getArrayCopy arginfo_class_ArrayObject_getArrayCopy

#define arginfo_class_ArrayIterator_count arginfo_class_ArrayObject_getArrayCopy

#define arginfo_class_ArrayIterator_getFlags arginfo_class_ArrayObject_getArrayCopy

#define arginfo_class_ArrayIterator_setFlags arginfo_class_ArrayObject_setFlags

#define arginfo_class_ArrayIterator_asort arginfo_class_ArrayObject_asort

#define arginfo_class_ArrayIterator_ksort arginfo_class_ArrayObject_asort

#define arginfo_class_ArrayIterator_uasort arginfo_class_ArrayObject_uasort

#define arginfo_class_ArrayIterator_uksort arginfo_class_ArrayObject_uasort

#define arginfo_class_ArrayIterator_natsort arginfo_class_ArrayObject_getArrayCopy

#define arginfo_class_ArrayIterator_natcasesort arginfo_class_ArrayObject_getArrayCopy

#define arginfo_class_ArrayIterator_unserialize arginfo_class_ArrayObject_unserialize

#define arginfo_class_ArrayIterator_serialize arginfo_class_ArrayObject_getArrayCopy

#define arginfo_class_ArrayIterator___serialize arginfo_class_ArrayObject_getArrayCopy

#define arginfo_class_ArrayIterator___unserialize arginfo_class_ArrayObject___unserialize

#define arginfo_class_ArrayIterator_rewind arginfo_class_ArrayObject_getArrayCopy

#define arginfo_class_ArrayIterator_current arginfo_class_ArrayObject_getArrayCopy

#define arginfo_class_ArrayIterator_key arginfo_class_ArrayObject_getArrayCopy

#define arginfo_class_ArrayIterator_next arginfo_class_ArrayObject_getArrayCopy

#define arginfo_class_ArrayIterator_valid arginfo_class_ArrayObject_getArrayCopy

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ArrayIterator_seek, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, position, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ArrayIterator___debugInfo arginfo_class_ArrayObject_getArrayCopy

#define arginfo_class_RecursiveArrayIterator_hasChildren arginfo_class_ArrayObject_getArrayCopy

#define arginfo_class_RecursiveArrayIterator_getChildren arginfo_class_ArrayObject_getArrayCopy
