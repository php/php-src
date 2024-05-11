/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 946ea9d0d2156ced1bac460d7d5fc3420e1934bb */

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

static const zend_function_entry class_SysvSemaphore_methods[] = {
	ZEND_FE_END
};

static zend_class_entry *register_class_SysvSemaphore(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "SysvSemaphore", class_SysvSemaphore_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE;

	return class_entry;
}
