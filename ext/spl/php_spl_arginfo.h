/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 920cc4fca334f59316fc4276277958c9738afb3e */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_implements, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, object_or_class)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, autoload, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

#define arginfo_class_parents arginfo_class_implements

#define arginfo_class_uses arginfo_class_implements

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_spl_autoload, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, class, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, file_extensions, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_spl_autoload_call, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, class, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_spl_autoload_extensions, 0, 0, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, file_extensions, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_spl_autoload_functions, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_spl_autoload_register, 0, 0, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, callback, IS_CALLABLE, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, throw, _IS_BOOL, 0, "true")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, prepend, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_spl_autoload_unregister, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
ZEND_END_ARG_INFO()

#define arginfo_spl_classes arginfo_spl_autoload_functions

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_spl_object_hash, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, object, IS_OBJECT, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_spl_object_id, 0, 1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, object, IS_OBJECT, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_iterator_apply, 0, 2, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, iterator, Traversable, 0)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, args, IS_ARRAY, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_iterator_count, 0, 1, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, iterator, Traversable, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_iterator_to_array, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_OBJ_INFO(0, iterator, Traversable, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, preserve_keys, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()


ZEND_FUNCTION(class_implements);
ZEND_FUNCTION(class_parents);
ZEND_FUNCTION(class_uses);
ZEND_FUNCTION(spl_autoload);
ZEND_FUNCTION(spl_autoload_call);
ZEND_FUNCTION(spl_autoload_extensions);
ZEND_FUNCTION(spl_autoload_functions);
ZEND_FUNCTION(spl_autoload_register);
ZEND_FUNCTION(spl_autoload_unregister);
ZEND_FUNCTION(spl_classes);
ZEND_FUNCTION(spl_object_hash);
ZEND_FUNCTION(spl_object_id);
ZEND_FUNCTION(iterator_apply);
ZEND_FUNCTION(iterator_count);
ZEND_FUNCTION(iterator_to_array);


static const zend_function_entry ext_functions[] = {
	ZEND_FE(class_implements, arginfo_class_implements)
	ZEND_FE(class_parents, arginfo_class_parents)
	ZEND_FE(class_uses, arginfo_class_uses)
	ZEND_FE(spl_autoload, arginfo_spl_autoload)
	ZEND_FE(spl_autoload_call, arginfo_spl_autoload_call)
	ZEND_FE(spl_autoload_extensions, arginfo_spl_autoload_extensions)
	ZEND_FE(spl_autoload_functions, arginfo_spl_autoload_functions)
	ZEND_FE(spl_autoload_register, arginfo_spl_autoload_register)
	ZEND_FE(spl_autoload_unregister, arginfo_spl_autoload_unregister)
	ZEND_FE(spl_classes, arginfo_spl_classes)
	ZEND_FE(spl_object_hash, arginfo_spl_object_hash)
	ZEND_FE(spl_object_id, arginfo_spl_object_id)
	ZEND_FE(iterator_apply, arginfo_iterator_apply)
	ZEND_FE(iterator_count, arginfo_iterator_count)
	ZEND_FE(iterator_to_array, arginfo_iterator_to_array)
	ZEND_FE_END
};
