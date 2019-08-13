/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_INFO_EX(arginfo_shm_attach, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, key, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, memsize, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, perm, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_shm_detach, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, shm_identifier)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_shm_has_var, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, id)
	ZEND_ARG_TYPE_INFO(0, variable_key, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_shm_remove arginfo_shm_detach

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_shm_put_var, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, shm_identifier)
	ZEND_ARG_TYPE_INFO(0, variable_key, IS_LONG, 0)
	ZEND_ARG_INFO(0, variable)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_shm_get_var, 0, 0, 2)
	ZEND_ARG_INFO(0, id)
	ZEND_ARG_TYPE_INFO(0, variable_key, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_shm_remove_var arginfo_shm_has_var
