/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 4cf2c620393f468968a219b5bd12a2b5f6b03ecc */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Throwable_getMessage, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Throwable_getCode, 0, 0, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Throwable_getFile arginfo_class_Throwable_getMessage

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Throwable_getLine, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Throwable_getTrace, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Throwable_getPrevious, 0, 0, Throwable, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_Throwable_getTraceAsString arginfo_class_Throwable_getMessage

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Exception___clone, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Exception___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, message, IS_STRING, 0, "\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, code, IS_LONG, 0, "0")
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, previous, Throwable, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_Exception___wakeup, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Exception_getMessage arginfo_class_Throwable_getMessage

#define arginfo_class_Exception_getCode arginfo_class_Throwable_getCode

#define arginfo_class_Exception_getFile arginfo_class_Throwable_getMessage

#define arginfo_class_Exception_getLine arginfo_class_Throwable_getLine

#define arginfo_class_Exception_getTrace arginfo_class_Throwable_getTrace

#define arginfo_class_Exception_getPrevious arginfo_class_Throwable_getPrevious

#define arginfo_class_Exception_getTraceAsString arginfo_class_Throwable_getMessage

#define arginfo_class_Exception___toString arginfo_class_Throwable_getMessage

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ErrorException___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, message, IS_STRING, 0, "\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, code, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, severity, IS_LONG, 0, "E_ERROR")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, filename, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, line, IS_LONG, 1, "null")
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, previous, Throwable, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_ErrorException_getSeverity arginfo_class_Throwable_getLine

#define arginfo_class_Error___clone arginfo_class_Exception___clone

#define arginfo_class_Error___construct arginfo_class_Exception___construct

#define arginfo_class_Error___wakeup arginfo_class_Exception___wakeup

#define arginfo_class_Error_getMessage arginfo_class_Throwable_getMessage

#define arginfo_class_Error_getCode arginfo_class_Throwable_getCode

#define arginfo_class_Error_getFile arginfo_class_Throwable_getMessage

#define arginfo_class_Error_getLine arginfo_class_Throwable_getLine

#define arginfo_class_Error_getTrace arginfo_class_Throwable_getTrace

#define arginfo_class_Error_getPrevious arginfo_class_Throwable_getPrevious

#define arginfo_class_Error_getTraceAsString arginfo_class_Throwable_getMessage

#define arginfo_class_Error___toString arginfo_class_Throwable_getMessage


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
ZEND_METHOD(ErrorException, __construct);
ZEND_METHOD(ErrorException, getSeverity);


static const zend_function_entry class_Throwable_methods[] = {
	ZEND_ABSTRACT_ME_WITH_FLAGS(Throwable, getMessage, arginfo_class_Throwable_getMessage, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(Throwable, getCode, arginfo_class_Throwable_getCode, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(Throwable, getFile, arginfo_class_Throwable_getFile, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(Throwable, getLine, arginfo_class_Throwable_getLine, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(Throwable, getTrace, arginfo_class_Throwable_getTrace, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(Throwable, getPrevious, arginfo_class_Throwable_getPrevious, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(Throwable, getTraceAsString, arginfo_class_Throwable_getTraceAsString, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_FE_END
};


static const zend_function_entry class_Exception_methods[] = {
	ZEND_ME(Exception, __clone, arginfo_class_Exception___clone, ZEND_ACC_PRIVATE)
	ZEND_ME(Exception, __construct, arginfo_class_Exception___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Exception, __wakeup, arginfo_class_Exception___wakeup, ZEND_ACC_PUBLIC)
	ZEND_ME(Exception, getMessage, arginfo_class_Exception_getMessage, ZEND_ACC_PUBLIC|ZEND_ACC_FINAL)
	ZEND_ME(Exception, getCode, arginfo_class_Exception_getCode, ZEND_ACC_PUBLIC|ZEND_ACC_FINAL)
	ZEND_ME(Exception, getFile, arginfo_class_Exception_getFile, ZEND_ACC_PUBLIC|ZEND_ACC_FINAL)
	ZEND_ME(Exception, getLine, arginfo_class_Exception_getLine, ZEND_ACC_PUBLIC|ZEND_ACC_FINAL)
	ZEND_ME(Exception, getTrace, arginfo_class_Exception_getTrace, ZEND_ACC_PUBLIC|ZEND_ACC_FINAL)
	ZEND_ME(Exception, getPrevious, arginfo_class_Exception_getPrevious, ZEND_ACC_PUBLIC|ZEND_ACC_FINAL)
	ZEND_ME(Exception, getTraceAsString, arginfo_class_Exception_getTraceAsString, ZEND_ACC_PUBLIC|ZEND_ACC_FINAL)
	ZEND_ME(Exception, __toString, arginfo_class_Exception___toString, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_ErrorException_methods[] = {
	ZEND_ME(ErrorException, __construct, arginfo_class_ErrorException___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(ErrorException, getSeverity, arginfo_class_ErrorException_getSeverity, ZEND_ACC_PUBLIC|ZEND_ACC_FINAL)
	ZEND_FE_END
};


static const zend_function_entry class_Error_methods[] = {
	ZEND_MALIAS(Exception, __clone, __clone, arginfo_class_Error___clone, ZEND_ACC_PRIVATE)
	ZEND_MALIAS(Exception, __construct, __construct, arginfo_class_Error___construct, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Exception, __wakeup, __wakeup, arginfo_class_Error___wakeup, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Exception, getMessage, getMessage, arginfo_class_Error_getMessage, ZEND_ACC_PUBLIC|ZEND_ACC_FINAL)
	ZEND_MALIAS(Exception, getCode, getCode, arginfo_class_Error_getCode, ZEND_ACC_PUBLIC|ZEND_ACC_FINAL)
	ZEND_MALIAS(Exception, getFile, getFile, arginfo_class_Error_getFile, ZEND_ACC_PUBLIC|ZEND_ACC_FINAL)
	ZEND_MALIAS(Exception, getLine, getLine, arginfo_class_Error_getLine, ZEND_ACC_PUBLIC|ZEND_ACC_FINAL)
	ZEND_MALIAS(Exception, getTrace, getTrace, arginfo_class_Error_getTrace, ZEND_ACC_PUBLIC|ZEND_ACC_FINAL)
	ZEND_MALIAS(Exception, getPrevious, getPrevious, arginfo_class_Error_getPrevious, ZEND_ACC_PUBLIC|ZEND_ACC_FINAL)
	ZEND_MALIAS(Exception, getTraceAsString, getTraceAsString, arginfo_class_Error_getTraceAsString, ZEND_ACC_PUBLIC|ZEND_ACC_FINAL)
	ZEND_MALIAS(Exception, __toString, __toString, arginfo_class_Error___toString, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_CompileError_methods[] = {
	ZEND_FE_END
};


static const zend_function_entry class_ParseError_methods[] = {
	ZEND_FE_END
};


static const zend_function_entry class_TypeError_methods[] = {
	ZEND_FE_END
};


static const zend_function_entry class_ArgumentCountError_methods[] = {
	ZEND_FE_END
};


static const zend_function_entry class_ValueError_methods[] = {
	ZEND_FE_END
};


static const zend_function_entry class_ArithmeticError_methods[] = {
	ZEND_FE_END
};


static const zend_function_entry class_DivisionByZeroError_methods[] = {
	ZEND_FE_END
};


static const zend_function_entry class_UnhandledMatchError_methods[] = {
	ZEND_FE_END
};

static zend_class_entry *register_class_Throwable(zend_class_entry *class_entry_Stringable)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "Throwable", class_Throwable_methods);
	class_entry = zend_register_internal_interface(&ce);
	zend_class_implements(class_entry, 1, class_entry_Stringable);

	return class_entry;
}

static zend_class_entry *register_class_Exception(zend_class_entry *class_entry_Throwable)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "Exception", class_Exception_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	zend_class_implements(class_entry, 1, class_entry_Throwable);

	zval property_message_default_value;
	ZVAL_EMPTY_STRING(&property_message_default_value);
	zend_string *property_message_name = zend_string_init("message", sizeof("message") - 1, 1);
	zend_declare_property_ex(class_entry, property_message_name, &property_message_default_value, ZEND_ACC_PROTECTED, NULL);
	zend_string_release(property_message_name);

	zval property_string_default_value;
	ZVAL_EMPTY_STRING(&property_string_default_value);
	zend_string *property_string_name = zend_string_init("string", sizeof("string") - 1, 1);
	zend_declare_typed_property(class_entry, property_string_name, &property_string_default_value, ZEND_ACC_PRIVATE, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_string_name);

	zval property_code_default_value;
	ZVAL_LONG(&property_code_default_value, 0);
	zend_string *property_code_name = zend_string_init("code", sizeof("code") - 1, 1);
	zend_declare_property_ex(class_entry, property_code_name, &property_code_default_value, ZEND_ACC_PROTECTED, NULL);
	zend_string_release(property_code_name);

	zval property_file_default_value;
	ZVAL_EMPTY_STRING(&property_file_default_value);
	zend_string *property_file_name = zend_string_init("file", sizeof("file") - 1, 1);
	zend_declare_typed_property(class_entry, property_file_name, &property_file_default_value, ZEND_ACC_PROTECTED, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_file_name);

	zval property_line_default_value;
	ZVAL_LONG(&property_line_default_value, 0);
	zend_string *property_line_name = zend_string_init("line", sizeof("line") - 1, 1);
	zend_declare_typed_property(class_entry, property_line_name, &property_line_default_value, ZEND_ACC_PROTECTED, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_line_name);

	zval property_trace_default_value;
	ZVAL_EMPTY_ARRAY(&property_trace_default_value);
	zend_string *property_trace_name = zend_string_init("trace", sizeof("trace") - 1, 1);
	zend_declare_typed_property(class_entry, property_trace_name, &property_trace_default_value, ZEND_ACC_PRIVATE, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_ARRAY));
	zend_string_release(property_trace_name);

	zend_string *property_previous_class_Throwable = zend_string_init("Throwable", sizeof("Throwable")-1, 1);
	zval property_previous_default_value;
	ZVAL_NULL(&property_previous_default_value);
	zend_string *property_previous_name = zend_string_init("previous", sizeof("previous") - 1, 1);
	zend_declare_typed_property(class_entry, property_previous_name, &property_previous_default_value, ZEND_ACC_PRIVATE, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_previous_class_Throwable, 0, MAY_BE_NULL));
	zend_string_release(property_previous_name);

	return class_entry;
}

static zend_class_entry *register_class_ErrorException(zend_class_entry *class_entry_Exception)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "ErrorException", class_ErrorException_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_Exception);

	zval property_severity_default_value;
	ZVAL_LONG(&property_severity_default_value, E_ERROR);
	zend_string *property_severity_name = zend_string_init("severity", sizeof("severity") - 1, 1);
	zend_declare_typed_property(class_entry, property_severity_name, &property_severity_default_value, ZEND_ACC_PROTECTED, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_severity_name);

	return class_entry;
}

static zend_class_entry *register_class_Error(zend_class_entry *class_entry_Throwable)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "Error", class_Error_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	zend_class_implements(class_entry, 1, class_entry_Throwable);

	zval property_message_default_value;
	ZVAL_EMPTY_STRING(&property_message_default_value);
	zend_string *property_message_name = zend_string_init("message", sizeof("message") - 1, 1);
	zend_declare_property_ex(class_entry, property_message_name, &property_message_default_value, ZEND_ACC_PROTECTED, NULL);
	zend_string_release(property_message_name);

	zval property_string_default_value;
	ZVAL_EMPTY_STRING(&property_string_default_value);
	zend_string *property_string_name = zend_string_init("string", sizeof("string") - 1, 1);
	zend_declare_typed_property(class_entry, property_string_name, &property_string_default_value, ZEND_ACC_PRIVATE, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_string_name);

	zval property_code_default_value;
	ZVAL_LONG(&property_code_default_value, 0);
	zend_string *property_code_name = zend_string_init("code", sizeof("code") - 1, 1);
	zend_declare_property_ex(class_entry, property_code_name, &property_code_default_value, ZEND_ACC_PROTECTED, NULL);
	zend_string_release(property_code_name);

	zval property_file_default_value;
	ZVAL_EMPTY_STRING(&property_file_default_value);
	zend_string *property_file_name = zend_string_init("file", sizeof("file") - 1, 1);
	zend_declare_typed_property(class_entry, property_file_name, &property_file_default_value, ZEND_ACC_PROTECTED, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_file_name);

	zval property_line_default_value;
	ZVAL_UNDEF(&property_line_default_value);
	zend_string *property_line_name = zend_string_init("line", sizeof("line") - 1, 1);
	zend_declare_typed_property(class_entry, property_line_name, &property_line_default_value, ZEND_ACC_PROTECTED, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_line_name);

	zval property_trace_default_value;
	ZVAL_EMPTY_ARRAY(&property_trace_default_value);
	zend_string *property_trace_name = zend_string_init("trace", sizeof("trace") - 1, 1);
	zend_declare_typed_property(class_entry, property_trace_name, &property_trace_default_value, ZEND_ACC_PRIVATE, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_ARRAY));
	zend_string_release(property_trace_name);

	zend_string *property_previous_class_Throwable = zend_string_init("Throwable", sizeof("Throwable")-1, 1);
	zval property_previous_default_value;
	ZVAL_NULL(&property_previous_default_value);
	zend_string *property_previous_name = zend_string_init("previous", sizeof("previous") - 1, 1);
	zend_declare_typed_property(class_entry, property_previous_name, &property_previous_default_value, ZEND_ACC_PRIVATE, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_previous_class_Throwable, 0, MAY_BE_NULL));
	zend_string_release(property_previous_name);

	return class_entry;
}

static zend_class_entry *register_class_CompileError(zend_class_entry *class_entry_Error)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "CompileError", class_CompileError_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_Error);

	return class_entry;
}

static zend_class_entry *register_class_ParseError(zend_class_entry *class_entry_CompileError)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "ParseError", class_ParseError_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_CompileError);

	return class_entry;
}

static zend_class_entry *register_class_TypeError(zend_class_entry *class_entry_Error)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "TypeError", class_TypeError_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_Error);

	return class_entry;
}

static zend_class_entry *register_class_ArgumentCountError(zend_class_entry *class_entry_TypeError)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "ArgumentCountError", class_ArgumentCountError_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_TypeError);

	return class_entry;
}

static zend_class_entry *register_class_ValueError(zend_class_entry *class_entry_Error)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "ValueError", class_ValueError_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_Error);

	return class_entry;
}

static zend_class_entry *register_class_ArithmeticError(zend_class_entry *class_entry_Error)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "ArithmeticError", class_ArithmeticError_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_Error);

	return class_entry;
}

static zend_class_entry *register_class_DivisionByZeroError(zend_class_entry *class_entry_ArithmeticError)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "DivisionByZeroError", class_DivisionByZeroError_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_ArithmeticError);

	return class_entry;
}

static zend_class_entry *register_class_UnhandledMatchError(zend_class_entry *class_entry_Error)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "UnhandledMatchError", class_UnhandledMatchError_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_Error);

	return class_entry;
}
