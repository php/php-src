/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: abbbe7b04323dc44b0675ad58700e996a6d7c43b */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Closure___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Closure_bind, 0, 2, Closure, 1)
	ZEND_ARG_OBJ_INFO(0, closure, Closure, 0)
	ZEND_ARG_TYPE_INFO(0, newThis, IS_OBJECT, 1)
	ZEND_ARG_INFO(0, newScope)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Closure_bindTo, 0, 1, Closure, 1)
	ZEND_ARG_TYPE_INFO(0, newThis, IS_OBJECT, 1)
	ZEND_ARG_INFO(0, newScope)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Closure_call, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, newThis, IS_OBJECT, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, arguments, IS_MIXED, 0)
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
