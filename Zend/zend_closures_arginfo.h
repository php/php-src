/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Closure___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Closure_bind, 0, 2, Closure, 1)
	ZEND_ARG_OBJ_INFO(0, closure, Closure, 0)
	ZEND_ARG_TYPE_INFO(0, newthis, IS_OBJECT, 1)
	ZEND_ARG_INFO(0, newscope)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Closure_bindTo, 0, 1, Closure, 1)
	ZEND_ARG_TYPE_INFO(0, newthis, IS_OBJECT, 1)
	ZEND_ARG_INFO(0, newscope)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Closure_call, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, newthis, IS_OBJECT, 0)
	ZEND_ARG_VARIADIC_INFO(0, parameters)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Closure_fromCallable, 0, 1, Closure, 0)
	ZEND_ARG_INFO(0, callable)
ZEND_END_ARG_INFO()


ZEND_METHOD(Closure, __construct);
ZEND_METHOD(Closure, bind);
ZEND_METHOD(Closure, call);
ZEND_METHOD(Closure, fromCallable);


static const zend_function_entry class_Closure_methods[] = {
	ZEND_ME(Closure, __construct, arginfo_class_Closure___construct, ZEND_ACC_PRIVATE)
	ZEND_ME(Closure, bind, arginfo_class_Closure_bind, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_MALIAS(Closure, bindTo, bind, arginfo_class_Closure_bindTo, ZEND_ACC_PUBLIC)
	ZEND_ME(Closure, call, arginfo_class_Closure_call, ZEND_ACC_PUBLIC)
	ZEND_ME(Closure, fromCallable, arginfo_class_Closure_fromCallable, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_FE_END
};
