/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplObserver_update, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, subject, SplSubject, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplSubject_attach, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, observer, SplObserver, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SplSubject_detach arginfo_class_SplSubject_attach

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplSubject_notify, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplObjectStorage_attach, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, object, IS_OBJECT, 0)
	ZEND_ARG_INFO(0, info)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplObjectStorage_detach, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, object, IS_OBJECT, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SplObjectStorage_contains arginfo_class_SplObjectStorage_detach

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplObjectStorage_addAll, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, storage, SplObjectStorage, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SplObjectStorage_removeAll arginfo_class_SplObjectStorage_addAll

#define arginfo_class_SplObjectStorage_removeAllExcept arginfo_class_SplObjectStorage_addAll

#define arginfo_class_SplObjectStorage_getInfo arginfo_class_SplSubject_notify

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplObjectStorage_setInfo, 0, 0, 1)
	ZEND_ARG_INFO(0, info)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplObjectStorage_count, 0, 0, 0)
	ZEND_ARG_TYPE_INFO(0, mode, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SplObjectStorage_rewind arginfo_class_SplSubject_notify

#define arginfo_class_SplObjectStorage_valid arginfo_class_SplSubject_notify

#define arginfo_class_SplObjectStorage_key arginfo_class_SplSubject_notify

#define arginfo_class_SplObjectStorage_current arginfo_class_SplSubject_notify

#define arginfo_class_SplObjectStorage_next arginfo_class_SplSubject_notify

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplObjectStorage_unserialize, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, serialized, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SplObjectStorage_serialize arginfo_class_SplSubject_notify

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplObjectStorage_offsetExists, 0, 0, 1)
	ZEND_ARG_INFO(0, object)
ZEND_END_ARG_INFO()

#define arginfo_class_SplObjectStorage_offsetGet arginfo_class_SplObjectStorage_offsetExists

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplObjectStorage_offsetSet, 0, 0, 1)
	ZEND_ARG_INFO(0, object)
	ZEND_ARG_INFO(0, info)
ZEND_END_ARG_INFO()

#define arginfo_class_SplObjectStorage_offsetUnset arginfo_class_SplObjectStorage_offsetExists

#define arginfo_class_SplObjectStorage_getHash arginfo_class_SplObjectStorage_detach

#define arginfo_class_SplObjectStorage___serialize arginfo_class_SplSubject_notify

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SplObjectStorage___unserialize, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_MultipleIterator___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_MultipleIterator_getFlags arginfo_class_SplSubject_notify

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_MultipleIterator_setFlags, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_MultipleIterator_attachIterator, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, iterator, Iterator, 0)
	ZEND_ARG_INFO(0, info)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_MultipleIterator_detachIterator, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, iterator, IS_OBJECT, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_MultipleIterator_containsIterator arginfo_class_MultipleIterator_detachIterator

#define arginfo_class_MultipleIterator_countIterators arginfo_class_SplSubject_notify

#define arginfo_class_MultipleIterator_rewind arginfo_class_SplSubject_notify

#define arginfo_class_MultipleIterator_valid arginfo_class_SplSubject_notify

#define arginfo_class_MultipleIterator_key arginfo_class_SplSubject_notify

#define arginfo_class_MultipleIterator_current arginfo_class_SplSubject_notify

#define arginfo_class_MultipleIterator_next arginfo_class_SplSubject_notify
