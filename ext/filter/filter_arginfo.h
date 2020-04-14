/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_filter_has_var, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, type, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, variable_name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_filter_input, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, type, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, variable_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, filter, IS_LONG, 0, "FILTER_DEFAULT")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, options, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_filter_var, 0, 0, 1)
	ZEND_ARG_INFO(0, variable)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, filter, IS_LONG, 0, "FILTER_DEFAULT")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, options, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_filter_input_array, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, type, IS_LONG, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, options, "FILTER_DEFAULT")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, add_empty, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_filter_var_array, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, options, "FILTER_DEFAULT")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, add_empty, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_filter_list, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_filter_id, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, filtername, IS_STRING, 0)
ZEND_END_ARG_INFO()


ZEND_FUNCTION(filter_has_var);
ZEND_FUNCTION(filter_input);
ZEND_FUNCTION(filter_var);
ZEND_FUNCTION(filter_input_array);
ZEND_FUNCTION(filter_var_array);
ZEND_FUNCTION(filter_list);
ZEND_FUNCTION(filter_id);


static const zend_function_entry ext_functions[] = {
	ZEND_FE(filter_has_var, arginfo_filter_has_var)
	ZEND_FE(filter_input, arginfo_filter_input)
	ZEND_FE(filter_var, arginfo_filter_var)
	ZEND_FE(filter_input_array, arginfo_filter_input_array)
	ZEND_FE(filter_var_array, arginfo_filter_var_array)
	ZEND_FE(filter_list, arginfo_filter_list)
	ZEND_FE(filter_id, arginfo_filter_id)
	ZEND_FE_END
};
