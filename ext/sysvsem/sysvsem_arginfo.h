/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 745e7cf135c7d1c9ad09d1ea1ab6cf2a8181433a */

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
