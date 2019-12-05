/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_implements, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, what)
	ZEND_ARG_TYPE_INFO(0, autoload, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_parents, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, instance)
	ZEND_ARG_TYPE_INFO(0, autoload, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_uses arginfo_class_implements

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_spl_autoload, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, class_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, file_extensions, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_spl_autoload_call, 0, 1, IS_VOID, 0)
	ZEND_ARG_INFO(0, class_name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_spl_autoload_extensions, 0, 0, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, file_extensions, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_spl_autoload_functions, 0, 0, MAY_BE_ARRAY|MAY_BE_FALSE)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_spl_autoload_register, 0, 0, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, autoload_function)
	ZEND_ARG_TYPE_INFO(0, throw, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, prepend, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_spl_autoload_unregister, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, autoload_function)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_spl_classes, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_spl_object_hash, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, obj, IS_OBJECT, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_spl_object_id, 0, 1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, obj, IS_OBJECT, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_iterator_apply, 0, 2, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, iterator, Traversable, 0)
	ZEND_ARG_INFO(0, function)
	ZEND_ARG_TYPE_INFO(0, args, IS_ARRAY, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_iterator_count, 0, 1, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, iterator, Traversable, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_iterator_to_array, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_OBJ_INFO(0, iterator, Traversable, 0)
	ZEND_ARG_TYPE_INFO(0, use_keys, _IS_BOOL, 0)
ZEND_END_ARG_INFO()
