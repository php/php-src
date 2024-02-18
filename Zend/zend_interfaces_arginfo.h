/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: a9c915c11e5989d8c7cf2d704ada09ca765670c3 */

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_INFO_EX(arginfo_class_IteratorAggregate_getIterator, 0, 0, Traversable, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_Iterator_current, 0, 0, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_Iterator_next, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Iterator_key arginfo_class_Iterator_current

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_Iterator_valid, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Iterator_rewind arginfo_class_Iterator_next

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_ArrayAccess_offsetExists, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_ArrayAccess_offsetGet, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_ArrayAccess_offsetSet, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_ArrayAccess_offsetUnset, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Serializable_serialize, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Serializable_unserialize, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_Countable_count, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Stringable___toString, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_InternalIterator___construct arginfo_class_Serializable_serialize

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_InternalIterator_current, 0, 0, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_InternalIterator_key arginfo_class_InternalIterator_current

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_InternalIterator_next, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_InternalIterator_valid, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_InternalIterator_rewind arginfo_class_InternalIterator_next

ZEND_METHOD(InternalIterator, __construct);
ZEND_METHOD(InternalIterator, current);
ZEND_METHOD(InternalIterator, key);
ZEND_METHOD(InternalIterator, next);
ZEND_METHOD(InternalIterator, valid);
ZEND_METHOD(InternalIterator, rewind);

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

static const zend_function_entry class_InternalIterator_methods[] = {
	ZEND_ME(InternalIterator, __construct, arginfo_class_InternalIterator___construct, ZEND_ACC_PRIVATE)
	ZEND_ME(InternalIterator, current, arginfo_class_InternalIterator_current, ZEND_ACC_PUBLIC)
	ZEND_ME(InternalIterator, key, arginfo_class_InternalIterator_key, ZEND_ACC_PUBLIC)
	ZEND_ME(InternalIterator, next, arginfo_class_InternalIterator_next, ZEND_ACC_PUBLIC)
	ZEND_ME(InternalIterator, valid, arginfo_class_InternalIterator_valid, ZEND_ACC_PUBLIC)
	ZEND_ME(InternalIterator, rewind, arginfo_class_InternalIterator_rewind, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Traversable(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "Traversable", class_Traversable_methods);
	class_entry = zend_register_internal_interface(&ce);

	return class_entry;
}

static zend_class_entry *register_class_IteratorAggregate(zend_class_entry *class_entry_Traversable)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "IteratorAggregate", class_IteratorAggregate_methods);
	class_entry = zend_register_internal_interface(&ce);
	zend_class_implements(class_entry, 1, class_entry_Traversable);

	return class_entry;
}

static zend_class_entry *register_class_Iterator(zend_class_entry *class_entry_Traversable)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "Iterator", class_Iterator_methods);
	class_entry = zend_register_internal_interface(&ce);
	zend_class_implements(class_entry, 1, class_entry_Traversable);

	return class_entry;
}

static zend_class_entry *register_class_ArrayAccess(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "ArrayAccess", class_ArrayAccess_methods);
	class_entry = zend_register_internal_interface(&ce);

	return class_entry;
}

static zend_class_entry *register_class_Serializable(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "Serializable", class_Serializable_methods);
	class_entry = zend_register_internal_interface(&ce);

	return class_entry;
}

static zend_class_entry *register_class_Countable(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "Countable", class_Countable_methods);
	class_entry = zend_register_internal_interface(&ce);

	return class_entry;
}

static zend_class_entry *register_class_Stringable(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "Stringable", class_Stringable_methods);
	class_entry = zend_register_internal_interface(&ce);

	return class_entry;
}

static zend_class_entry *register_class_InternalIterator(zend_class_entry *class_entry_Iterator)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "InternalIterator", class_InternalIterator_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL|ZEND_ACC_NOT_SERIALIZABLE;
	zend_class_implements(class_entry, 1, class_entry_Iterator);

	return class_entry;
}
