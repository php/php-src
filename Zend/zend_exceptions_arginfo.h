/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 2fa61163fb7db8d87b14f9cf9a42c3cd8093f2a6 */

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

#define arginfo_class_Exception___wakeup arginfo_class_Throwable_getCode

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

#define arginfo_class_Error___wakeup arginfo_class_Throwable_getCode

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
	ZEND_ME(Exception, __clone, arginfo_class_Exception___clone, ZEND_ACC_PRIVATE|ZEND_ACC_FINAL)
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
	ZEND_MALIAS(Exception, __clone, __clone, arginfo_class_Error___clone, ZEND_ACC_PRIVATE|ZEND_ACC_FINAL)
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
