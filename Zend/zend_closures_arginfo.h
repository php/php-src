/* This is a generated file, edit zend_closures.stub.php instead.
 * Stub hash: 95468c1d70556f6ba07327ecc64c396ab4be9206 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Closure___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Closure_bind, 0, 2, Closure, 1)
	ZEND_ARG_OBJ_INFO(0, closure, Closure, 0)
	ZEND_ARG_TYPE_INFO(0, newThis, IS_OBJECT, 1)
	ZEND_ARG_TYPE_MASK(0, newScope, MAY_BE_OBJECT|MAY_BE_STRING|MAY_BE_NULL, "\"static\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Closure_bindTo, 0, 1, Closure, 1)
	ZEND_ARG_TYPE_INFO(0, newThis, IS_OBJECT, 1)
	ZEND_ARG_TYPE_MASK(0, newScope, MAY_BE_OBJECT|MAY_BE_STRING|MAY_BE_NULL, "\"static\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Closure_call, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, newThis, IS_OBJECT, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, args, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Closure_fromCallable, 0, 1, Closure, 0)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Closure_getCurrent, 0, 0, Closure, 0)
ZEND_END_ARG_INFO()

ZEND_METHOD(Closure, __construct);
ZEND_METHOD(Closure, bind);
ZEND_METHOD(Closure, bindTo);
ZEND_METHOD(Closure, call);
ZEND_METHOD(Closure, fromCallable);
ZEND_METHOD(Closure, getCurrent);

static const zend_function_entry class_Closure_methods[] = {
	ZEND_ME(Closure, __construct, arginfo_class_Closure___construct, ZEND_ACC_PRIVATE)
	ZEND_ME(Closure, bind, arginfo_class_Closure_bind, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Closure, bindTo, arginfo_class_Closure_bindTo, ZEND_ACC_PUBLIC)
	ZEND_ME(Closure, call, arginfo_class_Closure_call, ZEND_ACC_PUBLIC)
	ZEND_ME(Closure, fromCallable, arginfo_class_Closure_fromCallable, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Closure, getCurrent, arginfo_class_Closure_getCurrent, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Closure(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "Closure", class_Closure_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE);

	zend_string *attribute_name_NonInstantiableClass_class_Closure_0 = zend_string_init_interned("NonInstantiableClass", sizeof("NonInstantiableClass") - 1, true);
	zend_attribute *attribute_NonInstantiableClass_class_Closure_0 = zend_add_class_attribute(class_entry, attribute_name_NonInstantiableClass_class_Closure_0, 1);
	zend_string_release_ex(attribute_name_NonInstantiableClass_class_Closure_0, true);
	zend_string *attribute_NonInstantiableClass_class_Closure_0_arg0_str = zend_string_init("Instantiation of class Closure is not allowed", strlen("Instantiation of class Closure is not allowed"), 1);
	ZVAL_STR(&attribute_NonInstantiableClass_class_Closure_0->args[0].value, attribute_NonInstantiableClass_class_Closure_0_arg0_str);

	class_entry->constructor = NULL;

	return class_entry;
}
