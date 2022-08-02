/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: e2d66473e8e2436652b78abd9c237e1a50c3d0dc */

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplObserver_update, 0, 1, IS_VOID, 0)
	ZEND_ARG_OBJ_INFO(0, subject, SplSubject, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplSubject_attach, 0, 1, IS_VOID, 0)
	ZEND_ARG_OBJ_INFO(0, observer, SplObserver, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SplSubject_detach arginfo_class_SplSubject_attach

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplSubject_notify, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplObjectStorage_attach, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, object, IS_OBJECT, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, info, IS_MIXED, 0, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplObjectStorage_detach, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, object, IS_OBJECT, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplObjectStorage_contains, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, object, IS_OBJECT, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplObjectStorage_addAll, 0, 1, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, storage, SplObjectStorage, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SplObjectStorage_removeAll arginfo_class_SplObjectStorage_addAll

#define arginfo_class_SplObjectStorage_removeAllExcept arginfo_class_SplObjectStorage_addAll

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplObjectStorage_getInfo, 0, 0, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplObjectStorage_setInfo, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, info, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplObjectStorage_count, 0, 0, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "COUNT_NORMAL")
ZEND_END_ARG_INFO()

#define arginfo_class_SplObjectStorage_rewind arginfo_class_SplSubject_notify

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplObjectStorage_valid, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplObjectStorage_key, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplObjectStorage_current, 0, 0, IS_OBJECT, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SplObjectStorage_next arginfo_class_SplSubject_notify

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplObjectStorage_unserialize, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplObjectStorage_serialize, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplObjectStorage_offsetExists, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, object)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplObjectStorage_offsetGet, 0, 1, IS_MIXED, 0)
	ZEND_ARG_INFO(0, object)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplObjectStorage_offsetSet, 0, 1, IS_VOID, 0)
	ZEND_ARG_INFO(0, object)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, info, IS_MIXED, 0, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplObjectStorage_offsetUnset, 0, 1, IS_VOID, 0)
	ZEND_ARG_INFO(0, object)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplObjectStorage_getHash, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, object, IS_OBJECT, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplObjectStorage___serialize, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SplObjectStorage___unserialize, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SplObjectStorage___debugInfo arginfo_class_SplObjectStorage___serialize

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_MultipleIterator___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "MultipleIterator::MIT_NEED_ALL | MultipleIterator::MIT_KEYS_NUMERIC")
ZEND_END_ARG_INFO()

#define arginfo_class_MultipleIterator_getFlags arginfo_class_SplObjectStorage_key

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_MultipleIterator_setFlags, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_MultipleIterator_attachIterator, 0, 1, IS_VOID, 0)
	ZEND_ARG_OBJ_INFO(0, iterator, Iterator, 0)
	ZEND_ARG_TYPE_MASK(0, info, MAY_BE_STRING|MAY_BE_LONG|MAY_BE_NULL, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_MultipleIterator_detachIterator, 0, 1, IS_VOID, 0)
	ZEND_ARG_OBJ_INFO(0, iterator, Iterator, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_MultipleIterator_containsIterator, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, iterator, Iterator, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_MultipleIterator_countIterators arginfo_class_SplObjectStorage_key

#define arginfo_class_MultipleIterator_rewind arginfo_class_SplSubject_notify

#define arginfo_class_MultipleIterator_valid arginfo_class_SplObjectStorage_valid

#define arginfo_class_MultipleIterator_key arginfo_class_SplObjectStorage___serialize

#define arginfo_class_MultipleIterator_current arginfo_class_SplObjectStorage___serialize

#define arginfo_class_MultipleIterator_next arginfo_class_SplSubject_notify

#define arginfo_class_MultipleIterator___debugInfo arginfo_class_SplObjectStorage___serialize


ZEND_METHOD(SplObjectStorage, attach);
ZEND_METHOD(SplObjectStorage, detach);
ZEND_METHOD(SplObjectStorage, contains);
ZEND_METHOD(SplObjectStorage, addAll);
ZEND_METHOD(SplObjectStorage, removeAll);
ZEND_METHOD(SplObjectStorage, removeAllExcept);
ZEND_METHOD(SplObjectStorage, getInfo);
ZEND_METHOD(SplObjectStorage, setInfo);
ZEND_METHOD(SplObjectStorage, count);
ZEND_METHOD(SplObjectStorage, rewind);
ZEND_METHOD(SplObjectStorage, valid);
ZEND_METHOD(SplObjectStorage, key);
ZEND_METHOD(SplObjectStorage, current);
ZEND_METHOD(SplObjectStorage, next);
ZEND_METHOD(SplObjectStorage, unserialize);
ZEND_METHOD(SplObjectStorage, serialize);
ZEND_METHOD(SplObjectStorage, offsetGet);
ZEND_METHOD(SplObjectStorage, getHash);
ZEND_METHOD(SplObjectStorage, __serialize);
ZEND_METHOD(SplObjectStorage, __unserialize);
ZEND_METHOD(SplObjectStorage, __debugInfo);
ZEND_METHOD(MultipleIterator, __construct);
ZEND_METHOD(MultipleIterator, getFlags);
ZEND_METHOD(MultipleIterator, setFlags);
ZEND_METHOD(MultipleIterator, attachIterator);
ZEND_METHOD(MultipleIterator, detachIterator);
ZEND_METHOD(MultipleIterator, containsIterator);
ZEND_METHOD(MultipleIterator, countIterators);
ZEND_METHOD(MultipleIterator, rewind);
ZEND_METHOD(MultipleIterator, valid);
ZEND_METHOD(MultipleIterator, key);
ZEND_METHOD(MultipleIterator, current);
ZEND_METHOD(MultipleIterator, next);


static const zend_function_entry class_SplObserver_methods[] = {
	ZEND_ABSTRACT_ME_WITH_FLAGS(SplObserver, update, arginfo_class_SplObserver_update, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_FE_END
};


static const zend_function_entry class_SplSubject_methods[] = {
	ZEND_ABSTRACT_ME_WITH_FLAGS(SplSubject, attach, arginfo_class_SplSubject_attach, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(SplSubject, detach, arginfo_class_SplSubject_detach, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(SplSubject, notify, arginfo_class_SplSubject_notify, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_FE_END
};


static const zend_function_entry class_SplObjectStorage_methods[] = {
	ZEND_ME(SplObjectStorage, attach, arginfo_class_SplObjectStorage_attach, ZEND_ACC_PUBLIC)
	ZEND_ME(SplObjectStorage, detach, arginfo_class_SplObjectStorage_detach, ZEND_ACC_PUBLIC)
	ZEND_ME(SplObjectStorage, contains, arginfo_class_SplObjectStorage_contains, ZEND_ACC_PUBLIC)
	ZEND_ME(SplObjectStorage, addAll, arginfo_class_SplObjectStorage_addAll, ZEND_ACC_PUBLIC)
	ZEND_ME(SplObjectStorage, removeAll, arginfo_class_SplObjectStorage_removeAll, ZEND_ACC_PUBLIC)
	ZEND_ME(SplObjectStorage, removeAllExcept, arginfo_class_SplObjectStorage_removeAllExcept, ZEND_ACC_PUBLIC)
	ZEND_ME(SplObjectStorage, getInfo, arginfo_class_SplObjectStorage_getInfo, ZEND_ACC_PUBLIC)
	ZEND_ME(SplObjectStorage, setInfo, arginfo_class_SplObjectStorage_setInfo, ZEND_ACC_PUBLIC)
	ZEND_ME(SplObjectStorage, count, arginfo_class_SplObjectStorage_count, ZEND_ACC_PUBLIC)
	ZEND_ME(SplObjectStorage, rewind, arginfo_class_SplObjectStorage_rewind, ZEND_ACC_PUBLIC)
	ZEND_ME(SplObjectStorage, valid, arginfo_class_SplObjectStorage_valid, ZEND_ACC_PUBLIC)
	ZEND_ME(SplObjectStorage, key, arginfo_class_SplObjectStorage_key, ZEND_ACC_PUBLIC)
	ZEND_ME(SplObjectStorage, current, arginfo_class_SplObjectStorage_current, ZEND_ACC_PUBLIC)
	ZEND_ME(SplObjectStorage, next, arginfo_class_SplObjectStorage_next, ZEND_ACC_PUBLIC)
	ZEND_ME(SplObjectStorage, unserialize, arginfo_class_SplObjectStorage_unserialize, ZEND_ACC_PUBLIC)
	ZEND_ME(SplObjectStorage, serialize, arginfo_class_SplObjectStorage_serialize, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(SplObjectStorage, offsetExists, contains, arginfo_class_SplObjectStorage_offsetExists, ZEND_ACC_PUBLIC)
	ZEND_ME(SplObjectStorage, offsetGet, arginfo_class_SplObjectStorage_offsetGet, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(SplObjectStorage, offsetSet, attach, arginfo_class_SplObjectStorage_offsetSet, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(SplObjectStorage, offsetUnset, detach, arginfo_class_SplObjectStorage_offsetUnset, ZEND_ACC_PUBLIC)
	ZEND_ME(SplObjectStorage, getHash, arginfo_class_SplObjectStorage_getHash, ZEND_ACC_PUBLIC)
	ZEND_ME(SplObjectStorage, __serialize, arginfo_class_SplObjectStorage___serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(SplObjectStorage, __unserialize, arginfo_class_SplObjectStorage___unserialize, ZEND_ACC_PUBLIC)
	ZEND_ME(SplObjectStorage, __debugInfo, arginfo_class_SplObjectStorage___debugInfo, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_MultipleIterator_methods[] = {
	ZEND_ME(MultipleIterator, __construct, arginfo_class_MultipleIterator___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(MultipleIterator, getFlags, arginfo_class_MultipleIterator_getFlags, ZEND_ACC_PUBLIC)
	ZEND_ME(MultipleIterator, setFlags, arginfo_class_MultipleIterator_setFlags, ZEND_ACC_PUBLIC)
	ZEND_ME(MultipleIterator, attachIterator, arginfo_class_MultipleIterator_attachIterator, ZEND_ACC_PUBLIC)
	ZEND_ME(MultipleIterator, detachIterator, arginfo_class_MultipleIterator_detachIterator, ZEND_ACC_PUBLIC)
	ZEND_ME(MultipleIterator, containsIterator, arginfo_class_MultipleIterator_containsIterator, ZEND_ACC_PUBLIC)
	ZEND_ME(MultipleIterator, countIterators, arginfo_class_MultipleIterator_countIterators, ZEND_ACC_PUBLIC)
	ZEND_ME(MultipleIterator, rewind, arginfo_class_MultipleIterator_rewind, ZEND_ACC_PUBLIC)
	ZEND_ME(MultipleIterator, valid, arginfo_class_MultipleIterator_valid, ZEND_ACC_PUBLIC)
	ZEND_ME(MultipleIterator, key, arginfo_class_MultipleIterator_key, ZEND_ACC_PUBLIC)
	ZEND_ME(MultipleIterator, current, arginfo_class_MultipleIterator_current, ZEND_ACC_PUBLIC)
	ZEND_ME(MultipleIterator, next, arginfo_class_MultipleIterator_next, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(SplObjectStorage, __debugInfo, __debugInfo, arginfo_class_MultipleIterator___debugInfo, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_SplObserver(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "SplObserver", class_SplObserver_methods);
	class_entry = zend_register_internal_interface(&ce);

	return class_entry;
}

static zend_class_entry *register_class_SplSubject(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "SplSubject", class_SplSubject_methods);
	class_entry = zend_register_internal_interface(&ce);

	return class_entry;
}

static zend_class_entry *register_class_SplObjectStorage(zend_class_entry *class_entry_Countable, zend_class_entry *class_entry_Iterator, zend_class_entry *class_entry_Serializable, zend_class_entry *class_entry_ArrayAccess)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "SplObjectStorage", class_SplObjectStorage_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	zend_class_implements(class_entry, 4, class_entry_Countable, class_entry_Iterator, class_entry_Serializable, class_entry_ArrayAccess);

	return class_entry;
}

static zend_class_entry *register_class_MultipleIterator(zend_class_entry *class_entry_Iterator)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "MultipleIterator", class_MultipleIterator_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	zend_class_implements(class_entry, 1, class_entry_Iterator);

	zval const_MIT_NEED_ANY_value;
	ZVAL_LONG(&const_MIT_NEED_ANY_value, MIT_NEED_ANY);
	zend_string *const_MIT_NEED_ANY_name = zend_string_init_interned("MIT_NEED_ANY", sizeof("MIT_NEED_ANY") - 1, 1);
	zend_declare_class_constant_ex(class_entry, const_MIT_NEED_ANY_name, &const_MIT_NEED_ANY_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(const_MIT_NEED_ANY_name);

	zval const_MIT_NEED_ALL_value;
	ZVAL_LONG(&const_MIT_NEED_ALL_value, MIT_NEED_ALL);
	zend_string *const_MIT_NEED_ALL_name = zend_string_init_interned("MIT_NEED_ALL", sizeof("MIT_NEED_ALL") - 1, 1);
	zend_declare_class_constant_ex(class_entry, const_MIT_NEED_ALL_name, &const_MIT_NEED_ALL_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(const_MIT_NEED_ALL_name);

	zval const_MIT_KEYS_NUMERIC_value;
	ZVAL_LONG(&const_MIT_KEYS_NUMERIC_value, MIT_KEYS_NUMERIC);
	zend_string *const_MIT_KEYS_NUMERIC_name = zend_string_init_interned("MIT_KEYS_NUMERIC", sizeof("MIT_KEYS_NUMERIC") - 1, 1);
	zend_declare_class_constant_ex(class_entry, const_MIT_KEYS_NUMERIC_name, &const_MIT_KEYS_NUMERIC_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(const_MIT_KEYS_NUMERIC_name);

	zval const_MIT_KEYS_ASSOC_value;
	ZVAL_LONG(&const_MIT_KEYS_ASSOC_value, MIT_KEYS_ASSOC);
	zend_string *const_MIT_KEYS_ASSOC_name = zend_string_init_interned("MIT_KEYS_ASSOC", sizeof("MIT_KEYS_ASSOC") - 1, 1);
	zend_declare_class_constant_ex(class_entry, const_MIT_KEYS_ASSOC_name, &const_MIT_KEYS_ASSOC_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(const_MIT_KEYS_ASSOC_name);

	return class_entry;
}
