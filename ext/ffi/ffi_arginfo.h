/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: d083aa82ea3e49553b731df605d8a4c3ebc95332 */

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_FFI_cdef, 0, 0, FFI, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, code, IS_STRING, 0, "\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, lib, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_FFI_load, 0, 1, FFI, 1)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_FFI_scope, 0, 1, FFI, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_FFI_new, 0, 1, FFI\\CData, 1)
	ZEND_ARG_OBJ_TYPE_MASK(0, type, FFI\\CType, MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, owned, _IS_BOOL, 0, "true")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, persistent, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_FFI_free, 0, 1, IS_VOID, 0)
	ZEND_ARG_OBJ_INFO(ZEND_SEND_PREFER_REF, ptr, FFI\\CData, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_FFI_cast, 0, 2, FFI\\CData, 1)
	ZEND_ARG_OBJ_TYPE_MASK(0, type, FFI\\CType, MAY_BE_STRING, NULL)
	ZEND_ARG_INFO(ZEND_SEND_PREFER_REF, ptr)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_FFI_type, 0, 1, FFI\\CType, 1)
	ZEND_ARG_TYPE_INFO(0, type, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_FFI_typeof, 0, 1, FFI\\CType, 0)
	ZEND_ARG_OBJ_INFO(ZEND_SEND_PREFER_REF, ptr, FFI\\CData, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_FFI_arrayType, 0, 2, FFI\\CType, 0)
	ZEND_ARG_OBJ_INFO(0, type, FFI\\CType, 0)
	ZEND_ARG_TYPE_INFO(0, dimensions, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_FFI_addr, 0, 1, FFI\\CData, 0)
	ZEND_ARG_OBJ_INFO(ZEND_SEND_PREFER_REF, ptr, FFI\\CData, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_FFI_sizeof, 0, 1, IS_LONG, 0)
	ZEND_ARG_OBJ_TYPE_MASK(ZEND_SEND_PREFER_REF, ptr, FFI\\CData|FFI\\CType, 0, NULL)
ZEND_END_ARG_INFO()

#define arginfo_class_FFI_alignof arginfo_class_FFI_sizeof

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_FFI_memcpy, 0, 3, IS_VOID, 0)
	ZEND_ARG_OBJ_INFO(ZEND_SEND_PREFER_REF, to, FFI\\CData, 0)
	ZEND_ARG_OBJ_TYPE_MASK(ZEND_SEND_PREFER_REF, from, FFI\\CData, MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO(0, size, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_FFI_memcmp, 0, 3, IS_LONG, 0)
	ZEND_ARG_OBJ_TYPE_MASK(ZEND_SEND_PREFER_REF, ptr1, FFI\\CData, MAY_BE_STRING, NULL)
	ZEND_ARG_OBJ_TYPE_MASK(ZEND_SEND_PREFER_REF, ptr2, FFI\\CData, MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO(0, size, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_FFI_memset, 0, 3, IS_VOID, 0)
	ZEND_ARG_OBJ_INFO(ZEND_SEND_PREFER_REF, ptr, FFI\\CData, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, size, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_FFI_string, 0, 1, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO(ZEND_SEND_PREFER_REF, ptr, FFI\\CData, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, size, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_FFI_isNull, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(ZEND_SEND_PREFER_REF, ptr, FFI\\CData, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_FFI_CType_getName, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_FFI_CType_getKind, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_FFI_CType_getSize arginfo_class_FFI_CType_getKind

#define arginfo_class_FFI_CType_getAlignment arginfo_class_FFI_CType_getKind

#define arginfo_class_FFI_CType_getAttributes arginfo_class_FFI_CType_getKind

#define arginfo_class_FFI_CType_getEnumKind arginfo_class_FFI_CType_getKind

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_FFI_CType_getArrayElementType, 0, 0, FFI\\CType, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_FFI_CType_getArrayLength arginfo_class_FFI_CType_getKind

#define arginfo_class_FFI_CType_getPointerType arginfo_class_FFI_CType_getArrayElementType

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_FFI_CType_getStructFieldNames, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_FFI_CType_getStructFieldOffset, 0, 1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_FFI_CType_getStructFieldType, 0, 1, FFI\\CType, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_FFI_CType_getFuncABI arginfo_class_FFI_CType_getKind

#define arginfo_class_FFI_CType_getFuncReturnType arginfo_class_FFI_CType_getArrayElementType

#define arginfo_class_FFI_CType_getFuncParameterCount arginfo_class_FFI_CType_getKind

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_FFI_CType_getFuncParameterType, 0, 1, FFI\\CType, 0)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_METHOD(FFI, cdef);
ZEND_METHOD(FFI, load);
ZEND_METHOD(FFI, scope);
ZEND_METHOD(FFI, new);
ZEND_METHOD(FFI, free);
ZEND_METHOD(FFI, cast);
ZEND_METHOD(FFI, type);
ZEND_METHOD(FFI, typeof);
ZEND_METHOD(FFI, arrayType);
ZEND_METHOD(FFI, addr);
ZEND_METHOD(FFI, sizeof);
ZEND_METHOD(FFI, alignof);
ZEND_METHOD(FFI, memcpy);
ZEND_METHOD(FFI, memcmp);
ZEND_METHOD(FFI, memset);
ZEND_METHOD(FFI, string);
ZEND_METHOD(FFI, isNull);
ZEND_METHOD(FFI_CType, getName);
ZEND_METHOD(FFI_CType, getKind);
ZEND_METHOD(FFI_CType, getSize);
ZEND_METHOD(FFI_CType, getAlignment);
ZEND_METHOD(FFI_CType, getAttributes);
ZEND_METHOD(FFI_CType, getEnumKind);
ZEND_METHOD(FFI_CType, getArrayElementType);
ZEND_METHOD(FFI_CType, getArrayLength);
ZEND_METHOD(FFI_CType, getPointerType);
ZEND_METHOD(FFI_CType, getStructFieldNames);
ZEND_METHOD(FFI_CType, getStructFieldOffset);
ZEND_METHOD(FFI_CType, getStructFieldType);
ZEND_METHOD(FFI_CType, getFuncABI);
ZEND_METHOD(FFI_CType, getFuncReturnType);
ZEND_METHOD(FFI_CType, getFuncParameterCount);
ZEND_METHOD(FFI_CType, getFuncParameterType);

static const zend_function_entry class_FFI_methods[] = {
	ZEND_ME(FFI, cdef, arginfo_class_FFI_cdef, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(FFI, load, arginfo_class_FFI_load, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(FFI, scope, arginfo_class_FFI_scope, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(FFI, new, arginfo_class_FFI_new, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(FFI, free, arginfo_class_FFI_free, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(FFI, cast, arginfo_class_FFI_cast, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(FFI, type, arginfo_class_FFI_type, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(FFI, typeof, arginfo_class_FFI_typeof, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(FFI, arrayType, arginfo_class_FFI_arrayType, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(FFI, addr, arginfo_class_FFI_addr, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(FFI, sizeof, arginfo_class_FFI_sizeof, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(FFI, alignof, arginfo_class_FFI_alignof, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(FFI, memcpy, arginfo_class_FFI_memcpy, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(FFI, memcmp, arginfo_class_FFI_memcmp, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(FFI, memset, arginfo_class_FFI_memset, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(FFI, string, arginfo_class_FFI_string, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(FFI, isNull, arginfo_class_FFI_isNull, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_FE_END
};

static const zend_function_entry class_FFI_CData_methods[] = {
	ZEND_FE_END
};

static const zend_function_entry class_FFI_CType_methods[] = {
	ZEND_ME(FFI_CType, getName, arginfo_class_FFI_CType_getName, ZEND_ACC_PUBLIC)
	ZEND_ME(FFI_CType, getKind, arginfo_class_FFI_CType_getKind, ZEND_ACC_PUBLIC)
	ZEND_ME(FFI_CType, getSize, arginfo_class_FFI_CType_getSize, ZEND_ACC_PUBLIC)
	ZEND_ME(FFI_CType, getAlignment, arginfo_class_FFI_CType_getAlignment, ZEND_ACC_PUBLIC)
	ZEND_ME(FFI_CType, getAttributes, arginfo_class_FFI_CType_getAttributes, ZEND_ACC_PUBLIC)
	ZEND_ME(FFI_CType, getEnumKind, arginfo_class_FFI_CType_getEnumKind, ZEND_ACC_PUBLIC)
	ZEND_ME(FFI_CType, getArrayElementType, arginfo_class_FFI_CType_getArrayElementType, ZEND_ACC_PUBLIC)
	ZEND_ME(FFI_CType, getArrayLength, arginfo_class_FFI_CType_getArrayLength, ZEND_ACC_PUBLIC)
	ZEND_ME(FFI_CType, getPointerType, arginfo_class_FFI_CType_getPointerType, ZEND_ACC_PUBLIC)
	ZEND_ME(FFI_CType, getStructFieldNames, arginfo_class_FFI_CType_getStructFieldNames, ZEND_ACC_PUBLIC)
	ZEND_ME(FFI_CType, getStructFieldOffset, arginfo_class_FFI_CType_getStructFieldOffset, ZEND_ACC_PUBLIC)
	ZEND_ME(FFI_CType, getStructFieldType, arginfo_class_FFI_CType_getStructFieldType, ZEND_ACC_PUBLIC)
	ZEND_ME(FFI_CType, getFuncABI, arginfo_class_FFI_CType_getFuncABI, ZEND_ACC_PUBLIC)
	ZEND_ME(FFI_CType, getFuncReturnType, arginfo_class_FFI_CType_getFuncReturnType, ZEND_ACC_PUBLIC)
	ZEND_ME(FFI_CType, getFuncParameterCount, arginfo_class_FFI_CType_getFuncParameterCount, ZEND_ACC_PUBLIC)
	ZEND_ME(FFI_CType, getFuncParameterType, arginfo_class_FFI_CType_getFuncParameterType, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_FFI_Exception_methods[] = {
	ZEND_FE_END
};

static const zend_function_entry class_FFI_ParserException_methods[] = {
	ZEND_FE_END
};

static zend_class_entry *register_class_FFI(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "FFI", class_FFI_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL|ZEND_ACC_NOT_SERIALIZABLE;

	zval const___BIGGEST_ALIGNMENT___value;
	ZVAL_LONG(&const___BIGGEST_ALIGNMENT___value, __BIGGEST_ALIGNMENT__);
	zend_string *const___BIGGEST_ALIGNMENT___name = zend_string_init_interned("__BIGGEST_ALIGNMENT__", sizeof("__BIGGEST_ALIGNMENT__") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const___BIGGEST_ALIGNMENT___name, &const___BIGGEST_ALIGNMENT___value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const___BIGGEST_ALIGNMENT___name);

	return class_entry;
}

static zend_class_entry *register_class_FFI_CData(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "FFI", "CData", class_FFI_CData_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL|ZEND_ACC_NOT_SERIALIZABLE;

	return class_entry;
}

static zend_class_entry *register_class_FFI_CType(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "FFI", "CType", class_FFI_CType_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL|ZEND_ACC_NOT_SERIALIZABLE;

	zval const_TYPE_VOID_value;
	ZVAL_LONG(&const_TYPE_VOID_value, ZEND_FFI_TYPE_VOID);
	zend_string *const_TYPE_VOID_name = zend_string_init_interned("TYPE_VOID", sizeof("TYPE_VOID") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_TYPE_VOID_name, &const_TYPE_VOID_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_TYPE_VOID_name);

	zval const_TYPE_FLOAT_value;
	ZVAL_LONG(&const_TYPE_FLOAT_value, ZEND_FFI_TYPE_FLOAT);
	zend_string *const_TYPE_FLOAT_name = zend_string_init_interned("TYPE_FLOAT", sizeof("TYPE_FLOAT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_TYPE_FLOAT_name, &const_TYPE_FLOAT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_TYPE_FLOAT_name);

	zval const_TYPE_DOUBLE_value;
	ZVAL_LONG(&const_TYPE_DOUBLE_value, ZEND_FFI_TYPE_DOUBLE);
	zend_string *const_TYPE_DOUBLE_name = zend_string_init_interned("TYPE_DOUBLE", sizeof("TYPE_DOUBLE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_TYPE_DOUBLE_name, &const_TYPE_DOUBLE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_TYPE_DOUBLE_name);
#if defined(HAVE_LONG_DOUBLE)

	zval const_TYPE_LONGDOUBLE_value;
	ZVAL_LONG(&const_TYPE_LONGDOUBLE_value, ZEND_FFI_TYPE_LONGDOUBLE);
	zend_string *const_TYPE_LONGDOUBLE_name = zend_string_init_interned("TYPE_LONGDOUBLE", sizeof("TYPE_LONGDOUBLE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_TYPE_LONGDOUBLE_name, &const_TYPE_LONGDOUBLE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_TYPE_LONGDOUBLE_name);
#endif

	zval const_TYPE_UINT8_value;
	ZVAL_LONG(&const_TYPE_UINT8_value, ZEND_FFI_TYPE_UINT8);
	zend_string *const_TYPE_UINT8_name = zend_string_init_interned("TYPE_UINT8", sizeof("TYPE_UINT8") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_TYPE_UINT8_name, &const_TYPE_UINT8_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_TYPE_UINT8_name);

	zval const_TYPE_SINT8_value;
	ZVAL_LONG(&const_TYPE_SINT8_value, ZEND_FFI_TYPE_SINT8);
	zend_string *const_TYPE_SINT8_name = zend_string_init_interned("TYPE_SINT8", sizeof("TYPE_SINT8") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_TYPE_SINT8_name, &const_TYPE_SINT8_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_TYPE_SINT8_name);

	zval const_TYPE_UINT16_value;
	ZVAL_LONG(&const_TYPE_UINT16_value, ZEND_FFI_TYPE_UINT16);
	zend_string *const_TYPE_UINT16_name = zend_string_init_interned("TYPE_UINT16", sizeof("TYPE_UINT16") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_TYPE_UINT16_name, &const_TYPE_UINT16_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_TYPE_UINT16_name);

	zval const_TYPE_SINT16_value;
	ZVAL_LONG(&const_TYPE_SINT16_value, ZEND_FFI_TYPE_SINT16);
	zend_string *const_TYPE_SINT16_name = zend_string_init_interned("TYPE_SINT16", sizeof("TYPE_SINT16") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_TYPE_SINT16_name, &const_TYPE_SINT16_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_TYPE_SINT16_name);

	zval const_TYPE_UINT32_value;
	ZVAL_LONG(&const_TYPE_UINT32_value, ZEND_FFI_TYPE_UINT32);
	zend_string *const_TYPE_UINT32_name = zend_string_init_interned("TYPE_UINT32", sizeof("TYPE_UINT32") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_TYPE_UINT32_name, &const_TYPE_UINT32_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_TYPE_UINT32_name);

	zval const_TYPE_SINT32_value;
	ZVAL_LONG(&const_TYPE_SINT32_value, ZEND_FFI_TYPE_SINT32);
	zend_string *const_TYPE_SINT32_name = zend_string_init_interned("TYPE_SINT32", sizeof("TYPE_SINT32") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_TYPE_SINT32_name, &const_TYPE_SINT32_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_TYPE_SINT32_name);

	zval const_TYPE_UINT64_value;
	ZVAL_LONG(&const_TYPE_UINT64_value, ZEND_FFI_TYPE_UINT64);
	zend_string *const_TYPE_UINT64_name = zend_string_init_interned("TYPE_UINT64", sizeof("TYPE_UINT64") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_TYPE_UINT64_name, &const_TYPE_UINT64_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_TYPE_UINT64_name);

	zval const_TYPE_SINT64_value;
	ZVAL_LONG(&const_TYPE_SINT64_value, ZEND_FFI_TYPE_SINT64);
	zend_string *const_TYPE_SINT64_name = zend_string_init_interned("TYPE_SINT64", sizeof("TYPE_SINT64") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_TYPE_SINT64_name, &const_TYPE_SINT64_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_TYPE_SINT64_name);

	zval const_TYPE_ENUM_value;
	ZVAL_LONG(&const_TYPE_ENUM_value, ZEND_FFI_TYPE_ENUM);
	zend_string *const_TYPE_ENUM_name = zend_string_init_interned("TYPE_ENUM", sizeof("TYPE_ENUM") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_TYPE_ENUM_name, &const_TYPE_ENUM_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_TYPE_ENUM_name);

	zval const_TYPE_BOOL_value;
	ZVAL_LONG(&const_TYPE_BOOL_value, ZEND_FFI_TYPE_BOOL);
	zend_string *const_TYPE_BOOL_name = zend_string_init_interned("TYPE_BOOL", sizeof("TYPE_BOOL") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_TYPE_BOOL_name, &const_TYPE_BOOL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_TYPE_BOOL_name);

	zval const_TYPE_CHAR_value;
	ZVAL_LONG(&const_TYPE_CHAR_value, ZEND_FFI_TYPE_CHAR);
	zend_string *const_TYPE_CHAR_name = zend_string_init_interned("TYPE_CHAR", sizeof("TYPE_CHAR") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_TYPE_CHAR_name, &const_TYPE_CHAR_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_TYPE_CHAR_name);

	zval const_TYPE_POINTER_value;
	ZVAL_LONG(&const_TYPE_POINTER_value, ZEND_FFI_TYPE_POINTER);
	zend_string *const_TYPE_POINTER_name = zend_string_init_interned("TYPE_POINTER", sizeof("TYPE_POINTER") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_TYPE_POINTER_name, &const_TYPE_POINTER_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_TYPE_POINTER_name);

	zval const_TYPE_FUNC_value;
	ZVAL_LONG(&const_TYPE_FUNC_value, ZEND_FFI_TYPE_FUNC);
	zend_string *const_TYPE_FUNC_name = zend_string_init_interned("TYPE_FUNC", sizeof("TYPE_FUNC") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_TYPE_FUNC_name, &const_TYPE_FUNC_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_TYPE_FUNC_name);

	zval const_TYPE_ARRAY_value;
	ZVAL_LONG(&const_TYPE_ARRAY_value, ZEND_FFI_TYPE_ARRAY);
	zend_string *const_TYPE_ARRAY_name = zend_string_init_interned("TYPE_ARRAY", sizeof("TYPE_ARRAY") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_TYPE_ARRAY_name, &const_TYPE_ARRAY_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_TYPE_ARRAY_name);

	zval const_TYPE_STRUCT_value;
	ZVAL_LONG(&const_TYPE_STRUCT_value, ZEND_FFI_TYPE_STRUCT);
	zend_string *const_TYPE_STRUCT_name = zend_string_init_interned("TYPE_STRUCT", sizeof("TYPE_STRUCT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_TYPE_STRUCT_name, &const_TYPE_STRUCT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_TYPE_STRUCT_name);

	zval const_ATTR_CONST_value;
	ZVAL_LONG(&const_ATTR_CONST_value, ZEND_FFI_ATTR_CONST);
	zend_string *const_ATTR_CONST_name = zend_string_init_interned("ATTR_CONST", sizeof("ATTR_CONST") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_CONST_name, &const_ATTR_CONST_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_CONST_name);

	zval const_ATTR_INCOMPLETE_TAG_value;
	ZVAL_LONG(&const_ATTR_INCOMPLETE_TAG_value, ZEND_FFI_ATTR_INCOMPLETE_TAG);
	zend_string *const_ATTR_INCOMPLETE_TAG_name = zend_string_init_interned("ATTR_INCOMPLETE_TAG", sizeof("ATTR_INCOMPLETE_TAG") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_INCOMPLETE_TAG_name, &const_ATTR_INCOMPLETE_TAG_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_INCOMPLETE_TAG_name);

	zval const_ATTR_VARIADIC_value;
	ZVAL_LONG(&const_ATTR_VARIADIC_value, ZEND_FFI_ATTR_VARIADIC);
	zend_string *const_ATTR_VARIADIC_name = zend_string_init_interned("ATTR_VARIADIC", sizeof("ATTR_VARIADIC") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_VARIADIC_name, &const_ATTR_VARIADIC_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_VARIADIC_name);

	zval const_ATTR_INCOMPLETE_ARRAY_value;
	ZVAL_LONG(&const_ATTR_INCOMPLETE_ARRAY_value, ZEND_FFI_ATTR_INCOMPLETE_ARRAY);
	zend_string *const_ATTR_INCOMPLETE_ARRAY_name = zend_string_init_interned("ATTR_INCOMPLETE_ARRAY", sizeof("ATTR_INCOMPLETE_ARRAY") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_INCOMPLETE_ARRAY_name, &const_ATTR_INCOMPLETE_ARRAY_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_INCOMPLETE_ARRAY_name);

	zval const_ATTR_VLA_value;
	ZVAL_LONG(&const_ATTR_VLA_value, ZEND_FFI_ATTR_VLA);
	zend_string *const_ATTR_VLA_name = zend_string_init_interned("ATTR_VLA", sizeof("ATTR_VLA") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_VLA_name, &const_ATTR_VLA_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_VLA_name);

	zval const_ATTR_UNION_value;
	ZVAL_LONG(&const_ATTR_UNION_value, ZEND_FFI_ATTR_UNION);
	zend_string *const_ATTR_UNION_name = zend_string_init_interned("ATTR_UNION", sizeof("ATTR_UNION") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_UNION_name, &const_ATTR_UNION_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_UNION_name);

	zval const_ATTR_PACKED_value;
	ZVAL_LONG(&const_ATTR_PACKED_value, ZEND_FFI_ATTR_PACKED);
	zend_string *const_ATTR_PACKED_name = zend_string_init_interned("ATTR_PACKED", sizeof("ATTR_PACKED") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_PACKED_name, &const_ATTR_PACKED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_PACKED_name);

	zval const_ATTR_MS_STRUCT_value;
	ZVAL_LONG(&const_ATTR_MS_STRUCT_value, ZEND_FFI_ATTR_MS_STRUCT);
	zend_string *const_ATTR_MS_STRUCT_name = zend_string_init_interned("ATTR_MS_STRUCT", sizeof("ATTR_MS_STRUCT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_MS_STRUCT_name, &const_ATTR_MS_STRUCT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_MS_STRUCT_name);

	zval const_ATTR_GCC_STRUCT_value;
	ZVAL_LONG(&const_ATTR_GCC_STRUCT_value, ZEND_FFI_ATTR_GCC_STRUCT);
	zend_string *const_ATTR_GCC_STRUCT_name = zend_string_init_interned("ATTR_GCC_STRUCT", sizeof("ATTR_GCC_STRUCT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_GCC_STRUCT_name, &const_ATTR_GCC_STRUCT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_GCC_STRUCT_name);

	zval const_ABI_DEFAULT_value;
	ZVAL_LONG(&const_ABI_DEFAULT_value, ZEND_FFI_ABI_DEFAULT);
	zend_string *const_ABI_DEFAULT_name = zend_string_init_interned("ABI_DEFAULT", sizeof("ABI_DEFAULT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ABI_DEFAULT_name, &const_ABI_DEFAULT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ABI_DEFAULT_name);

	zval const_ABI_CDECL_value;
	ZVAL_LONG(&const_ABI_CDECL_value, ZEND_FFI_ABI_CDECL);
	zend_string *const_ABI_CDECL_name = zend_string_init_interned("ABI_CDECL", sizeof("ABI_CDECL") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ABI_CDECL_name, &const_ABI_CDECL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ABI_CDECL_name);

	zval const_ABI_FASTCALL_value;
	ZVAL_LONG(&const_ABI_FASTCALL_value, ZEND_FFI_ABI_FASTCALL);
	zend_string *const_ABI_FASTCALL_name = zend_string_init_interned("ABI_FASTCALL", sizeof("ABI_FASTCALL") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ABI_FASTCALL_name, &const_ABI_FASTCALL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ABI_FASTCALL_name);

	zval const_ABI_THISCALL_value;
	ZVAL_LONG(&const_ABI_THISCALL_value, ZEND_FFI_ABI_THISCALL);
	zend_string *const_ABI_THISCALL_name = zend_string_init_interned("ABI_THISCALL", sizeof("ABI_THISCALL") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ABI_THISCALL_name, &const_ABI_THISCALL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ABI_THISCALL_name);

	zval const_ABI_STDCALL_value;
	ZVAL_LONG(&const_ABI_STDCALL_value, ZEND_FFI_ABI_STDCALL);
	zend_string *const_ABI_STDCALL_name = zend_string_init_interned("ABI_STDCALL", sizeof("ABI_STDCALL") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ABI_STDCALL_name, &const_ABI_STDCALL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ABI_STDCALL_name);

	zval const_ABI_PASCAL_value;
	ZVAL_LONG(&const_ABI_PASCAL_value, ZEND_FFI_ABI_PASCAL);
	zend_string *const_ABI_PASCAL_name = zend_string_init_interned("ABI_PASCAL", sizeof("ABI_PASCAL") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ABI_PASCAL_name, &const_ABI_PASCAL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ABI_PASCAL_name);

	zval const_ABI_REGISTER_value;
	ZVAL_LONG(&const_ABI_REGISTER_value, ZEND_FFI_ABI_REGISTER);
	zend_string *const_ABI_REGISTER_name = zend_string_init_interned("ABI_REGISTER", sizeof("ABI_REGISTER") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ABI_REGISTER_name, &const_ABI_REGISTER_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ABI_REGISTER_name);

	zval const_ABI_MS_value;
	ZVAL_LONG(&const_ABI_MS_value, ZEND_FFI_ABI_MS);
	zend_string *const_ABI_MS_name = zend_string_init_interned("ABI_MS", sizeof("ABI_MS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ABI_MS_name, &const_ABI_MS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ABI_MS_name);

	zval const_ABI_SYSV_value;
	ZVAL_LONG(&const_ABI_SYSV_value, ZEND_FFI_ABI_SYSV);
	zend_string *const_ABI_SYSV_name = zend_string_init_interned("ABI_SYSV", sizeof("ABI_SYSV") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ABI_SYSV_name, &const_ABI_SYSV_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ABI_SYSV_name);

	zval const_ABI_VECTORCALL_value;
	ZVAL_LONG(&const_ABI_VECTORCALL_value, ZEND_FFI_ABI_VECTORCALL);
	zend_string *const_ABI_VECTORCALL_name = zend_string_init_interned("ABI_VECTORCALL", sizeof("ABI_VECTORCALL") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ABI_VECTORCALL_name, &const_ABI_VECTORCALL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ABI_VECTORCALL_name);

	return class_entry;
}

static zend_class_entry *register_class_FFI_Exception(zend_class_entry *class_entry_Error)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "FFI", "Exception", class_FFI_Exception_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_Error);

	return class_entry;
}

static zend_class_entry *register_class_FFI_ParserException(zend_class_entry *class_entry_FFI_Exception)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "FFI", "ParserException", class_FFI_ParserException_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_FFI_Exception);
	class_entry->ce_flags |= ZEND_ACC_FINAL;

	return class_entry;
}
