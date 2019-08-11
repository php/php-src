/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_INFO_EX(arginfo_sem_get, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, key, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, max_acquire, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, perm, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, auto_release, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sem_acquire, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, sem_identifier)
	ZEND_ARG_TYPE_INFO(0, nowait, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sem_release, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, sem_identifier)
ZEND_END_ARG_INFO()

#define arginfo_sem_remove arginfo_sem_release
