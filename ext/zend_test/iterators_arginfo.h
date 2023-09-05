/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: f9558686a7393ddd4ba3302e811f70d4496317ee */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ZendTest_Iterators_TraversableTest___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_ZendTest_Iterators_TraversableTest_getIterator, 0, 0, Iterator, 0)
ZEND_END_ARG_INFO()


static ZEND_METHOD(ZendTest_Iterators_TraversableTest, __construct);
static ZEND_METHOD(ZendTest_Iterators_TraversableTest, getIterator);


static const zend_function_entry class_ZendTest_Iterators_TraversableTest_methods[] = {
	ZEND_ME(ZendTest_Iterators_TraversableTest, __construct, arginfo_class_ZendTest_Iterators_TraversableTest___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(ZendTest_Iterators_TraversableTest, getIterator, arginfo_class_ZendTest_Iterators_TraversableTest_getIterator, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_ZendTest_Iterators_TraversableTest(zend_class_entry *class_entry_IteratorAggregate)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "ZendTest\\Iterators", "TraversableTest", class_ZendTest_Iterators_TraversableTest_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;
	zend_class_implements(class_entry, 1, class_entry_IteratorAggregate);

	return class_entry;
}
