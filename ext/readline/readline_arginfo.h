/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_readline, 0, 0, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, prompt, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_readline_info, 0, 0, 0)
	ZEND_ARG_TYPE_INFO(0, varname, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, newvalue, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_readline_add_history, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, prompt, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_readline_clear_history, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#if defined(HAVE_HISTORY_LIST)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_readline_list_history, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_readline_read_history, 0, 0, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_readline_write_history arginfo_readline_read_history

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_readline_completion_function, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, funcname)
ZEND_END_ARG_INFO()

#if HAVE_RL_CALLBACK_READ_CHAR
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_readline_callback_handler_install, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, prompt, IS_STRING, 0)
	ZEND_ARG_INFO(0, callback)
ZEND_END_ARG_INFO()
#endif

#if HAVE_RL_CALLBACK_READ_CHAR
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_readline_callback_read_char, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()
#endif

#if HAVE_RL_CALLBACK_READ_CHAR
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_readline_callback_handler_remove, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()
#endif

#if HAVE_RL_CALLBACK_READ_CHAR
#define arginfo_readline_redisplay arginfo_readline_callback_read_char
#endif

#if HAVE_RL_CALLBACK_READ_CHAR && HAVE_RL_ON_NEW_LINE
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_readline_on_new_line, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()
#endif
