/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 730dc375507a0bea337b9c242d493a2d9cfbf268 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ImmutableIterable___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, iterator, IS_ITERABLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_ImmutableIterable_getIterator, 0, 0, InternalIterator, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_ImmutableIterable_count, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_ImmutableIterable_fromPairs, 0, 1, ImmutableIterable, 0)
	ZEND_ARG_TYPE_INFO(0, pairs, IS_ITERABLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_ImmutableIterable_toPairs, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ImmutableIterable___serialize arginfo_class_ImmutableIterable_toPairs

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_ImmutableIterable___unserialize, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_ImmutableIterable___set_state, 0, 1, ImmutableIterable, 0)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ImmutableIterable_keys arginfo_class_ImmutableIterable_toPairs

#define arginfo_class_ImmutableIterable_values arginfo_class_ImmutableIterable_toPairs

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_ImmutableIterable_keyAt, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ImmutableIterable_valueAt arginfo_class_ImmutableIterable_keyAt

#define arginfo_class_ImmutableIterable_jsonSerialize arginfo_class_ImmutableIterable_toPairs


ZEND_METHOD(ImmutableIterable, __construct);
ZEND_METHOD(ImmutableIterable, getIterator);
ZEND_METHOD(ImmutableIterable, count);
ZEND_METHOD(ImmutableIterable, fromPairs);
ZEND_METHOD(ImmutableIterable, toPairs);
ZEND_METHOD(ImmutableIterable, __serialize);
ZEND_METHOD(ImmutableIterable, __unserialize);
ZEND_METHOD(ImmutableIterable, __set_state);
ZEND_METHOD(ImmutableIterable, keys);
ZEND_METHOD(ImmutableIterable, values);
ZEND_METHOD(ImmutableIterable, keyAt);
ZEND_METHOD(ImmutableIterable, valueAt);
ZEND_METHOD(ImmutableIterable, jsonSerialize);


static const zend_function_entry class_ImmutableIterable_methods[] = {
	ZEND_ME(ImmutableIterable, __construct, arginfo_class_ImmutableIterable___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(ImmutableIterable, getIterator, arginfo_class_ImmutableIterable_getIterator, ZEND_ACC_PUBLIC)
	ZEND_ME(ImmutableIterable, count, arginfo_class_ImmutableIterable_count, ZEND_ACC_PUBLIC)
	ZEND_ME(ImmutableIterable, fromPairs, arginfo_class_ImmutableIterable_fromPairs, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(ImmutableIterable, toPairs, arginfo_class_ImmutableIterable_toPairs, ZEND_ACC_PUBLIC)
	ZEND_ME(ImmutableIterable, __serialize, arginfo_class_ImmutableIterable___serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(ImmutableIterable, __unserialize, arginfo_class_ImmutableIterable___unserialize, ZEND_ACC_PUBLIC)
	ZEND_ME(ImmutableIterable, __set_state, arginfo_class_ImmutableIterable___set_state, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(ImmutableIterable, keys, arginfo_class_ImmutableIterable_keys, ZEND_ACC_PUBLIC)
	ZEND_ME(ImmutableIterable, values, arginfo_class_ImmutableIterable_values, ZEND_ACC_PUBLIC)
	ZEND_ME(ImmutableIterable, keyAt, arginfo_class_ImmutableIterable_keyAt, ZEND_ACC_PUBLIC)
	ZEND_ME(ImmutableIterable, valueAt, arginfo_class_ImmutableIterable_valueAt, ZEND_ACC_PUBLIC)
	ZEND_ME(ImmutableIterable, jsonSerialize, arginfo_class_ImmutableIterable_jsonSerialize, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_ImmutableIterable(zend_class_entry *class_entry_IteratorAggregate, zend_class_entry *class_entry_Countable, zend_class_entry *class_entry_JsonSerializable)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "ImmutableIterable", class_ImmutableIterable_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;
	zend_class_implements(class_entry, 3, class_entry_IteratorAggregate, class_entry_Countable, class_entry_JsonSerializable);

	return class_entry;
}
