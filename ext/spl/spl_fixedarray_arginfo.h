/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplFixedArray___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, size, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplFixedArray___wakeup, 0, 0, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SplFixedArray_count arginfo_class_SplFixedArray___wakeup

#define arginfo_class_SplFixedArray_toArray arginfo_class_SplFixedArray___wakeup

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplFixedArray_fromArray, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, save_indexes, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

#define arginfo_class_SplFixedArray_getSize arginfo_class_SplFixedArray___wakeup

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplFixedArray_setSize, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, size, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplFixedArray_offsetExists, 0, 0, 1)
	ZEND_ARG_INFO(0, index)
ZEND_END_ARG_INFO()

#define arginfo_class_SplFixedArray_offsetGet arginfo_class_SplFixedArray_offsetExists

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplFixedArray_offsetSet, 0, 0, 2)
	ZEND_ARG_INFO(0, index)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

#define arginfo_class_SplFixedArray_offsetUnset arginfo_class_SplFixedArray_offsetExists

#define arginfo_class_SplFixedArray_rewind arginfo_class_SplFixedArray___wakeup

#define arginfo_class_SplFixedArray_current arginfo_class_SplFixedArray___wakeup

#define arginfo_class_SplFixedArray_key arginfo_class_SplFixedArray___wakeup

#define arginfo_class_SplFixedArray_next arginfo_class_SplFixedArray___wakeup

#define arginfo_class_SplFixedArray_valid arginfo_class_SplFixedArray___wakeup
