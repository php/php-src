/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 0a2dd53716d30893aa5dd92a9907b2298abb3f70 */

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


ZEND_METHOD(Closure, __construct);
ZEND_METHOD(Closure, bind);
ZEND_METHOD(Closure, bindTo);
ZEND_METHOD(Closure, call);
ZEND_METHOD(Closure, fromCallable);


static const zend_function_entry class_Closure_methods[] = {
	ZEND_ME(Closure, __construct, arginfo_class_Closure___construct, ZEND_ACC_PRIVATE)
	ZEND_ME(Closure, bind, arginfo_class_Closure_bind, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Closure, bindTo, arginfo_class_Closure_bindTo, ZEND_ACC_PUBLIC)
	ZEND_ME(Closure, call, arginfo_class_Closure_call, ZEND_ACC_PUBLIC)
	ZEND_ME(Closure, fromCallable, arginfo_class_Closure_fromCallable, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_FE_END
};
