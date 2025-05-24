/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 5fb43b3b05e9c04100684dc574b3f4dbbaf38965 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_CancellationException___clone, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_CancellationException___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, message, IS_STRING, 0, "\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, code, IS_LONG, 0, "0")
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, previous, Throwable, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_CancellationException___wakeup, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_CancellationException_getMessage, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_CancellationException_getCode, 0, 0, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_CancellationException_getFile arginfo_class_CancellationException_getMessage

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_CancellationException_getLine, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_CancellationException_getTrace, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_CancellationException_getPrevious, 0, 0, Throwable, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_CancellationException_getTraceAsString arginfo_class_CancellationException_getMessage

#define arginfo_class_CancellationException___toString arginfo_class_CancellationException_getMessage

ZEND_METHOD(Exception, __clone);
ZEND_METHOD(Exception, __construct);
ZEND_METHOD(Exception, __wakeup);
ZEND_METHOD(Exception, getMessage);
ZEND_METHOD(Exception, getCode);
ZEND_METHOD(Exception, getFile);
ZEND_METHOD(Exception, getLine);
ZEND_METHOD(Exception, getTrace);
ZEND_METHOD(Exception, getPrevious);
ZEND_METHOD(Exception, getTraceAsString);
ZEND_METHOD(Exception, __toString);

static const zend_function_entry class_CancellationException_methods[] = {
	ZEND_RAW_FENTRY("__clone", zim_Exception___clone, arginfo_class_CancellationException___clone, ZEND_ACC_PRIVATE, NULL, NULL)
	ZEND_RAW_FENTRY("__construct", zim_Exception___construct, arginfo_class_CancellationException___construct, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("__wakeup", zim_Exception___wakeup, arginfo_class_CancellationException___wakeup, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("getMessage", zim_Exception_getMessage, arginfo_class_CancellationException_getMessage, ZEND_ACC_PUBLIC|ZEND_ACC_FINAL, NULL, NULL)
	ZEND_RAW_FENTRY("getCode", zim_Exception_getCode, arginfo_class_CancellationException_getCode, ZEND_ACC_PUBLIC|ZEND_ACC_FINAL, NULL, NULL)
	ZEND_RAW_FENTRY("getFile", zim_Exception_getFile, arginfo_class_CancellationException_getFile, ZEND_ACC_PUBLIC|ZEND_ACC_FINAL, NULL, NULL)
	ZEND_RAW_FENTRY("getLine", zim_Exception_getLine, arginfo_class_CancellationException_getLine, ZEND_ACC_PUBLIC|ZEND_ACC_FINAL, NULL, NULL)
	ZEND_RAW_FENTRY("getTrace", zim_Exception_getTrace, arginfo_class_CancellationException_getTrace, ZEND_ACC_PUBLIC|ZEND_ACC_FINAL, NULL, NULL)
	ZEND_RAW_FENTRY("getPrevious", zim_Exception_getPrevious, arginfo_class_CancellationException_getPrevious, ZEND_ACC_PUBLIC|ZEND_ACC_FINAL, NULL, NULL)
	ZEND_RAW_FENTRY("getTraceAsString", zim_Exception_getTraceAsString, arginfo_class_CancellationException_getTraceAsString, ZEND_ACC_PUBLIC|ZEND_ACC_FINAL, NULL, NULL)
	ZEND_RAW_FENTRY("__toString", zim_Exception___toString, arginfo_class_CancellationException___toString, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_FE_END
};

static zend_class_entry *register_class_CancellationException(zend_class_entry *class_entry_Throwable)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "CancellationException", class_CancellationException_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, 0);
	zend_class_implements(class_entry, 1, class_entry_Throwable);

	zval property_message_default_value;
	ZVAL_EMPTY_STRING(&property_message_default_value);
	zend_declare_typed_property(class_entry, ZSTR_KNOWN(ZEND_STR_MESSAGE), &property_message_default_value, ZEND_ACC_PROTECTED, NULL, (zend_type) ZEND_TYPE_INIT_NONE(0));

	zval property_string_default_value;
	ZVAL_EMPTY_STRING(&property_string_default_value);
	zend_declare_typed_property(class_entry, ZSTR_KNOWN(ZEND_STR_STRING), &property_string_default_value, ZEND_ACC_PRIVATE, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));

	zval property_code_default_value;
	ZVAL_LONG(&property_code_default_value, 0);
	zend_declare_typed_property(class_entry, ZSTR_KNOWN(ZEND_STR_CODE), &property_code_default_value, ZEND_ACC_PROTECTED, NULL, (zend_type) ZEND_TYPE_INIT_NONE(0));

	zval property_file_default_value;
	ZVAL_EMPTY_STRING(&property_file_default_value);
	zend_declare_typed_property(class_entry, ZSTR_KNOWN(ZEND_STR_FILE), &property_file_default_value, ZEND_ACC_PROTECTED, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));

	zval property_line_default_value;
	ZVAL_UNDEF(&property_line_default_value);
	zend_declare_typed_property(class_entry, ZSTR_KNOWN(ZEND_STR_LINE), &property_line_default_value, ZEND_ACC_PROTECTED, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));

	zval property_trace_default_value;
	ZVAL_EMPTY_ARRAY(&property_trace_default_value);
	zend_declare_typed_property(class_entry, ZSTR_KNOWN(ZEND_STR_TRACE), &property_trace_default_value, ZEND_ACC_PRIVATE, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_ARRAY));

	zval property_previous_default_value;
	ZVAL_NULL(&property_previous_default_value);
	zend_string *property_previous_class_Throwable = zend_string_init("Throwable", sizeof("Throwable")-1, 1);
	zend_declare_typed_property(class_entry, ZSTR_KNOWN(ZEND_STR_PREVIOUS), &property_previous_default_value, ZEND_ACC_PRIVATE, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_previous_class_Throwable, 0, MAY_BE_NULL));

	return class_entry;
}
