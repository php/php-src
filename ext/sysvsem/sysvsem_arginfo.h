/* This is a generated file, edit sysvsem.stub.php instead.
 * Stub hash: 68b3058bcd5654ca98163bc42081cd31e792ff8d */

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_sem_get, 0, 1, SysvSemaphore, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, max_acquire, IS_LONG, 0, "1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, permissions, IS_LONG, 0, "0666")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, auto_release, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sem_acquire, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, semaphore, SysvSemaphore, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, non_blocking, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sem_release, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, semaphore, SysvSemaphore, 0)
ZEND_END_ARG_INFO()

#define arginfo_sem_remove arginfo_sem_release

ZEND_FUNCTION(sem_get);
ZEND_FUNCTION(sem_acquire);
ZEND_FUNCTION(sem_release);
ZEND_FUNCTION(sem_remove);

static const zend_function_entry ext_functions[] = {
	ZEND_FE(sem_get, arginfo_sem_get)
	ZEND_FE(sem_acquire, arginfo_sem_acquire)
	ZEND_FE(sem_release, arginfo_sem_release)
	ZEND_FE(sem_remove, arginfo_sem_remove)
	ZEND_FE_END
};

static zend_class_entry *register_class_SysvSemaphore(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "SysvSemaphore", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE);

	zend_string *attribute_name_NonInstantiableClass_class_SysvSemaphore_0 = zend_string_init_interned("NonInstantiableClass", sizeof("NonInstantiableClass") - 1, true);
	zend_attribute *attribute_NonInstantiableClass_class_SysvSemaphore_0 = zend_add_class_attribute(class_entry, attribute_name_NonInstantiableClass_class_SysvSemaphore_0, 1);
	zend_string_release_ex(attribute_name_NonInstantiableClass_class_SysvSemaphore_0, true);
	zend_string *attribute_NonInstantiableClass_class_SysvSemaphore_0_arg0_str = zend_string_init("Cannot directly construct SysvSemaphore, use sem_get() instead", strlen("Cannot directly construct SysvSemaphore, use sem_get() instead"), 1);
	ZVAL_STR(&attribute_NonInstantiableClass_class_SysvSemaphore_0->args[0].value, attribute_NonInstantiableClass_class_SysvSemaphore_0_arg0_str);

	class_entry->constructor = (zend_function *) &zend_non_instantiable_constructor;

	return class_entry;
}
