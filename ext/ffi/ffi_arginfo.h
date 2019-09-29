/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_FFI_cdef, 0, 0, 0)
	ZEND_ARG_TYPE_INFO(0, code, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, lib, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_FFI_load, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_FFI_scope, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, scope_name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_FFI_new, 0, 0, 1)
	ZEND_ARG_INFO(0, type)
	ZEND_ARG_TYPE_INFO(0, owned, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, persistent, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_FFI_free, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(ZEND_SEND_PREFER_REF, ptr, FFI\\CData, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_FFI_cast, 0, 0, 2)
	ZEND_ARG_INFO(0, type)
	ZEND_ARG_INFO(ZEND_SEND_PREFER_REF, ptr)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_FFI_type, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, type, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_FFI_typeof arginfo_class_FFI_free

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_FFI_arrayType, 0, 0, 2)
	ZEND_ARG_OBJ_INFO(0, type, FFI\\CType, 0)
	ZEND_ARG_TYPE_INFO(0, dims, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_FFI_addr arginfo_class_FFI_free

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_FFI_sizeof, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(ZEND_SEND_PREFER_REF, ptr, IS_OBJECT, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_FFI_alignof arginfo_class_FFI_sizeof

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_FFI_memcpy, 0, 0, 3)
	ZEND_ARG_OBJ_INFO(ZEND_SEND_PREFER_REF, dst, FFI\\CData, 0)
	ZEND_ARG_INFO(ZEND_SEND_PREFER_REF, src)
	ZEND_ARG_TYPE_INFO(0, size, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_FFI_memcmp, 0, 0, 3)
	ZEND_ARG_INFO(ZEND_SEND_PREFER_REF, ptr1)
	ZEND_ARG_INFO(ZEND_SEND_PREFER_REF, ptr2)
	ZEND_ARG_TYPE_INFO(0, size, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_FFI_memset, 0, 0, 3)
	ZEND_ARG_OBJ_INFO(ZEND_SEND_PREFER_REF, ptr, FFI\\CData, 0)
	ZEND_ARG_TYPE_INFO(0, ch, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, size, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_FFI_string, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(ZEND_SEND_PREFER_REF, ptr, FFI\\CData, 0)
	ZEND_ARG_TYPE_INFO(0, size, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_FFI_isNull arginfo_class_FFI_free
