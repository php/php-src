/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 08e29f02953f23bfce6ce04f435227b4e5e61545 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_phpdbg_break_next, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_phpdbg_break_file, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, file, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, line, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_phpdbg_break_method, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, class, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, method, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_phpdbg_break_function, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, function, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_phpdbg_color, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, element, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, color, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_phpdbg_prompt, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_phpdbg_exec, 0, 1, MAY_BE_STRING|MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, context, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_phpdbg_clear arginfo_phpdbg_break_next

#define arginfo_phpdbg_start_oplog arginfo_phpdbg_break_next

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_phpdbg_end_oplog, 0, 0, IS_ARRAY, 1)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_phpdbg_get_executable, 0, 0, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()


ZEND_FUNCTION(phpdbg_break_next);
ZEND_FUNCTION(phpdbg_break_file);
ZEND_FUNCTION(phpdbg_break_method);
ZEND_FUNCTION(phpdbg_break_function);
ZEND_FUNCTION(phpdbg_color);
ZEND_FUNCTION(phpdbg_prompt);
ZEND_FUNCTION(phpdbg_exec);
ZEND_FUNCTION(phpdbg_clear);
ZEND_FUNCTION(phpdbg_start_oplog);
ZEND_FUNCTION(phpdbg_end_oplog);
ZEND_FUNCTION(phpdbg_get_executable);


static const zend_function_entry ext_functions[] = {
	ZEND_FE(phpdbg_break_next, arginfo_phpdbg_break_next)
	ZEND_FE(phpdbg_break_file, arginfo_phpdbg_break_file)
	ZEND_FE(phpdbg_break_method, arginfo_phpdbg_break_method)
	ZEND_FE(phpdbg_break_function, arginfo_phpdbg_break_function)
	ZEND_FE(phpdbg_color, arginfo_phpdbg_color)
	ZEND_FE(phpdbg_prompt, arginfo_phpdbg_prompt)
	ZEND_FE(phpdbg_exec, arginfo_phpdbg_exec)
	ZEND_FE(phpdbg_clear, arginfo_phpdbg_clear)
	ZEND_FE(phpdbg_start_oplog, arginfo_phpdbg_start_oplog)
	ZEND_FE(phpdbg_end_oplog, arginfo_phpdbg_end_oplog)
	ZEND_FE(phpdbg_get_executable, arginfo_phpdbg_get_executable)
	ZEND_FE_END
};

static void register_phpdbg_symbols(int module_number)
{
	REGISTER_STRING_CONSTANT("PHPDBG_VERSION", PHPDBG_VERSION, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PHPDBG_COLOR_PROMPT", PHPDBG_COLOR_PROMPT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PHPDBG_COLOR_NOTICE", PHPDBG_COLOR_NOTICE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PHPDBG_COLOR_ERROR", PHPDBG_COLOR_ERROR, CONST_PERSISTENT);
}
