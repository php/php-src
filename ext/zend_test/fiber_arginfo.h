/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 8cd7626122b050585503ccebe370a61781ff83f2 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class__ZendTestFiber___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class__ZendTestFiber_start, 0, 0, IS_MIXED, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, args, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class__ZendTestFiber_resume, 0, 0, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, value, IS_MIXED, 0, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class__ZendTestFiber_pipeTo, 0, 1, _ZendTestFiber, 0)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
ZEND_END_ARG_INFO()

#define arginfo_class__ZendTestFiber_suspend arginfo_class__ZendTestFiber_resume


static ZEND_METHOD(_ZendTestFiber, __construct);
static ZEND_METHOD(_ZendTestFiber, start);
static ZEND_METHOD(_ZendTestFiber, resume);
static ZEND_METHOD(_ZendTestFiber, pipeTo);
static ZEND_METHOD(_ZendTestFiber, suspend);


static const zend_function_entry class__ZendTestFiber_methods[] = {
	ZEND_ME(_ZendTestFiber, __construct, arginfo_class__ZendTestFiber___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(_ZendTestFiber, start, arginfo_class__ZendTestFiber_start, ZEND_ACC_PUBLIC)
	ZEND_ME(_ZendTestFiber, resume, arginfo_class__ZendTestFiber_resume, ZEND_ACC_PUBLIC)
	ZEND_ME(_ZendTestFiber, pipeTo, arginfo_class__ZendTestFiber_pipeTo, ZEND_ACC_PUBLIC)
	ZEND_ME(_ZendTestFiber, suspend, arginfo_class__ZendTestFiber_suspend, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_FE_END
};

static zend_class_entry *register_class__ZendTestFiber(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "_ZendTestFiber", class__ZendTestFiber_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;

	return class_entry;
}
