/* This is a generated file, edit io_terminal.stub.php instead.
 * Stub hash: e24d70debb197766b78c9a58b9d5300f6ced77a5
 * Has decl header: yes */

#include "zend_attributes.h"
#include "zend_enum.h"

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Io_Terminal_Dimensions___construct, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, cols, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, rows, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Io_Terminal_ModeToken___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Io_Terminal_Terminal___construct arginfo_class_Io_Terminal_ModeToken___construct

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Io_Terminal_Terminal_fromStdio, 0, 0, Io\\Terminal\\Terminal, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Io_Terminal_Terminal_fromStreams, 0, 1, Io\\Terminal\\Terminal, 0)
	ZEND_ARG_INFO(0, input)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, output, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Io_Terminal_Terminal_getDimensions, 0, 0, Io\\Terminal\\Dimensions, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Io_Terminal_Terminal_enableRawMode, 0, 0, Io\\Terminal\\ModeToken, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Io_Terminal_Terminal_restoreMode, 0, 1, IS_VOID, 0)
	ZEND_ARG_OBJ_INFO(0, mode, Io\\Terminal\\ModeToken, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_Io_Terminal_Terminal_readKey, 0, 0, Io\\Terminal\\Key, MAY_BE_STRING|MAY_BE_NULL)
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, timeout, Time\\Duration, 1, "null")
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, sequenceTimeout, Time\\Duration, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Io_Terminal_Terminal_readLine, 0, 0, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Io_Terminal_Terminal_readSecret, 0, 0, IS_STRING, 1)
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, timeout, Time\\Duration, 1, "null")
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, sequenceTimeout, Time\\Duration, 1, "null")
ZEND_END_ARG_INFO()

ZEND_METHOD(Io_Terminal_Dimensions, __construct);
ZEND_METHOD(Io_Terminal_ModeToken, __construct);
ZEND_METHOD(Io_Terminal_Terminal, __construct);
ZEND_METHOD(Io_Terminal_Terminal, fromStdio);
ZEND_METHOD(Io_Terminal_Terminal, fromStreams);
ZEND_METHOD(Io_Terminal_Terminal, getDimensions);
ZEND_METHOD(Io_Terminal_Terminal, enableRawMode);
ZEND_METHOD(Io_Terminal_Terminal, restoreMode);
ZEND_METHOD(Io_Terminal_Terminal, readKey);
ZEND_METHOD(Io_Terminal_Terminal, readLine);
ZEND_METHOD(Io_Terminal_Terminal, readSecret);

static const zend_function_entry class_Io_Terminal_Dimensions_methods[] = {
	ZEND_ME(Io_Terminal_Dimensions, __construct, arginfo_class_Io_Terminal_Dimensions___construct, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_Io_Terminal_ModeToken_methods[] = {
	ZEND_ME(Io_Terminal_ModeToken, __construct, arginfo_class_Io_Terminal_ModeToken___construct, ZEND_ACC_PRIVATE)
	ZEND_FE_END
};

static const zend_function_entry class_Io_Terminal_Terminal_methods[] = {
	ZEND_ME(Io_Terminal_Terminal, __construct, arginfo_class_Io_Terminal_Terminal___construct, ZEND_ACC_PRIVATE)
	ZEND_ME(Io_Terminal_Terminal, fromStdio, arginfo_class_Io_Terminal_Terminal_fromStdio, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Io_Terminal_Terminal, fromStreams, arginfo_class_Io_Terminal_Terminal_fromStreams, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Io_Terminal_Terminal, getDimensions, arginfo_class_Io_Terminal_Terminal_getDimensions, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Terminal_Terminal, enableRawMode, arginfo_class_Io_Terminal_Terminal_enableRawMode, ZEND_ACC_PUBLIC|ZEND_ACC_NODISCARD)
	ZEND_ME(Io_Terminal_Terminal, restoreMode, arginfo_class_Io_Terminal_Terminal_restoreMode, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Terminal_Terminal, readKey, arginfo_class_Io_Terminal_Terminal_readKey, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Terminal_Terminal, readLine, arginfo_class_Io_Terminal_Terminal_readLine, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Terminal_Terminal, readSecret, arginfo_class_Io_Terminal_Terminal_readSecret, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Io_Terminal_TerminalException(zend_class_entry *class_entry_Io_IoException)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Io\\Terminal", "TerminalException", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_Io_IoException, ZEND_ACC_NO_DYNAMIC_PROPERTIES);

	return class_entry;
}

static zend_class_entry *register_class_Io_Terminal_Key(void)
{
	zend_class_entry *class_entry = zend_register_internal_enum("Io\\Terminal\\Key", IS_UNDEF, NULL);

	zend_enum_add_case_cstr(class_entry, "Up", NULL);

	zend_enum_add_case_cstr(class_entry, "Down", NULL);

	zend_enum_add_case_cstr(class_entry, "Right", NULL);

	zend_enum_add_case_cstr(class_entry, "Left", NULL);

	zend_enum_add_case_cstr(class_entry, "Enter", NULL);

	zend_enum_add_case_cstr(class_entry, "Backspace", NULL);

	zend_enum_add_case_cstr(class_entry, "Escape", NULL);

	zend_enum_add_case_cstr(class_entry, "Tab", NULL);

	zend_enum_add_case_cstr(class_entry, "Home", NULL);

	zend_enum_add_case_cstr(class_entry, "End", NULL);

	zend_enum_add_case_cstr(class_entry, "Delete", NULL);

	zend_enum_add_case_cstr(class_entry, "Insert", NULL);

	zend_enum_add_case_cstr(class_entry, "PageUp", NULL);

	zend_enum_add_case_cstr(class_entry, "PageDown", NULL);

	zend_enum_add_case_cstr(class_entry, "Resize", NULL);

	zend_enum_add_case_cstr(class_entry, "ShiftTab", NULL);

	zend_enum_add_case_cstr(class_entry, "F1", NULL);

	zend_enum_add_case_cstr(class_entry, "F2", NULL);

	zend_enum_add_case_cstr(class_entry, "F3", NULL);

	zend_enum_add_case_cstr(class_entry, "F4", NULL);

	zend_enum_add_case_cstr(class_entry, "F5", NULL);

	zend_enum_add_case_cstr(class_entry, "F6", NULL);

	zend_enum_add_case_cstr(class_entry, "F7", NULL);

	zend_enum_add_case_cstr(class_entry, "F8", NULL);

	zend_enum_add_case_cstr(class_entry, "F9", NULL);

	zend_enum_add_case_cstr(class_entry, "F10", NULL);

	zend_enum_add_case_cstr(class_entry, "F11", NULL);

	zend_enum_add_case_cstr(class_entry, "F12", NULL);

	return class_entry;
}

static zend_class_entry *register_class_Io_Terminal_Dimensions(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Io\\Terminal", "Dimensions", class_Io_Terminal_Dimensions_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_READONLY_CLASS);

	zval property_cols_default_value;
	ZVAL_UNDEF(&property_cols_default_value);
	zend_string *property_cols_name = zend_string_init("cols", sizeof("cols") - 1, true);
	zend_declare_typed_property(class_entry, property_cols_name, &property_cols_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(property_cols_name, true);

	zval property_rows_default_value;
	ZVAL_UNDEF(&property_rows_default_value);
	zend_string *property_rows_name = zend_string_init("rows", sizeof("rows") - 1, true);
	zend_declare_typed_property(class_entry, property_rows_name, &property_rows_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(property_rows_name, true);

	return class_entry;
}

static zend_class_entry *register_class_Io_Terminal_ModeToken(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Io\\Terminal", "ModeToken", class_Io_Terminal_ModeToken_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE);

	return class_entry;
}

static zend_class_entry *register_class_Io_Terminal_Terminal(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Io\\Terminal", "Terminal", class_Io_Terminal_Terminal_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE);


	zend_attribute *attribute_NoDiscard_func_enablerawmode_0 = zend_add_function_attribute(zend_hash_str_find_ptr(&class_entry->function_table, "enablerawmode", sizeof("enablerawmode") - 1), ZSTR_KNOWN(ZEND_STR_NODISCARD), 1);
	zend_string *attribute_NoDiscard_func_enablerawmode_0_arg0_str = zend_string_init("as the returned token must stay alive to keep the raw-mode lease active", strlen("as the returned token must stay alive to keep the raw-mode lease active"), 1);
	ZVAL_STR(&attribute_NoDiscard_func_enablerawmode_0->args[0].value, attribute_NoDiscard_func_enablerawmode_0_arg0_str);
	attribute_NoDiscard_func_enablerawmode_0->args[0].name = ZSTR_KNOWN(ZEND_STR_MESSAGE);

	return class_entry;
}
