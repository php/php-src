/* This is a generated file, edit converter.stub.php instead.
 * Stub hash: fa5b55d5fa2ef453fbc3f2232df5c8ce1d4d33ee */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_UConverter___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, destination_encoding, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, source_encoding, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_UConverter_convert, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, reverse, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_UConverter_fromUCallback, 0, 4, MAY_BE_STRING|MAY_BE_LONG|MAY_BE_ARRAY|MAY_BE_NULL)
	ZEND_ARG_TYPE_INFO(0, reason, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, source, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, codePoint, IS_LONG, 0)
	ZEND_ARG_INFO(1, error)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_UConverter_getAliases, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE|MAY_BE_NULL)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_UConverter_getAvailable, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_UConverter_getDestinationEncoding, 0, 0, MAY_BE_STRING|MAY_BE_FALSE|MAY_BE_NULL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_UConverter_getDestinationType, 0, 0, MAY_BE_LONG|MAY_BE_FALSE|MAY_BE_NULL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_UConverter_getErrorCode, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_UConverter_getErrorMessage, 0, 0, IS_STRING, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_UConverter_getSourceEncoding arginfo_class_UConverter_getDestinationEncoding

#define arginfo_class_UConverter_getSourceType arginfo_class_UConverter_getDestinationType

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_UConverter_getStandards, 0, 0, IS_ARRAY, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_UConverter_getSubstChars arginfo_class_UConverter_getDestinationEncoding

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_UConverter_reasonText, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, reason, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_UConverter_setDestinationEncoding, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, encoding, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_UConverter_setSourceEncoding arginfo_class_UConverter_setDestinationEncoding

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_UConverter_setSubstChars, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, chars, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_UConverter_toUCallback, 0, 4, MAY_BE_STRING|MAY_BE_LONG|MAY_BE_ARRAY|MAY_BE_NULL)
	ZEND_ARG_TYPE_INFO(0, reason, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, source, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, codeUnits, IS_STRING, 0)
	ZEND_ARG_INFO(1, error)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_UConverter_transcode, 0, 3, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, toEncoding, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, fromEncoding, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 1, "null")
ZEND_END_ARG_INFO()

ZEND_METHOD(UConverter, __construct);
ZEND_METHOD(UConverter, convert);
ZEND_METHOD(UConverter, fromUCallback);
ZEND_METHOD(UConverter, getAliases);
ZEND_METHOD(UConverter, getAvailable);
ZEND_METHOD(UConverter, getDestinationEncoding);
ZEND_METHOD(UConverter, getDestinationType);
ZEND_METHOD(UConverter, getErrorCode);
ZEND_METHOD(UConverter, getErrorMessage);
ZEND_METHOD(UConverter, getSourceEncoding);
ZEND_METHOD(UConverter, getSourceType);
ZEND_METHOD(UConverter, getStandards);
ZEND_METHOD(UConverter, getSubstChars);
ZEND_METHOD(UConverter, reasonText);
ZEND_METHOD(UConverter, setDestinationEncoding);
ZEND_METHOD(UConverter, setSourceEncoding);
ZEND_METHOD(UConverter, setSubstChars);
ZEND_METHOD(UConverter, toUCallback);
ZEND_METHOD(UConverter, transcode);

static const zend_function_entry class_UConverter_methods[] = {
	ZEND_ME(UConverter, __construct, arginfo_class_UConverter___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(UConverter, convert, arginfo_class_UConverter_convert, ZEND_ACC_PUBLIC)
	ZEND_ME(UConverter, fromUCallback, arginfo_class_UConverter_fromUCallback, ZEND_ACC_PUBLIC)
	ZEND_ME(UConverter, getAliases, arginfo_class_UConverter_getAliases, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(UConverter, getAvailable, arginfo_class_UConverter_getAvailable, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(UConverter, getDestinationEncoding, arginfo_class_UConverter_getDestinationEncoding, ZEND_ACC_PUBLIC)
	ZEND_ME(UConverter, getDestinationType, arginfo_class_UConverter_getDestinationType, ZEND_ACC_PUBLIC)
	ZEND_ME(UConverter, getErrorCode, arginfo_class_UConverter_getErrorCode, ZEND_ACC_PUBLIC)
	ZEND_ME(UConverter, getErrorMessage, arginfo_class_UConverter_getErrorMessage, ZEND_ACC_PUBLIC)
	ZEND_ME(UConverter, getSourceEncoding, arginfo_class_UConverter_getSourceEncoding, ZEND_ACC_PUBLIC)
	ZEND_ME(UConverter, getSourceType, arginfo_class_UConverter_getSourceType, ZEND_ACC_PUBLIC)
	ZEND_ME(UConverter, getStandards, arginfo_class_UConverter_getStandards, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(UConverter, getSubstChars, arginfo_class_UConverter_getSubstChars, ZEND_ACC_PUBLIC)
	ZEND_ME(UConverter, reasonText, arginfo_class_UConverter_reasonText, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(UConverter, setDestinationEncoding, arginfo_class_UConverter_setDestinationEncoding, ZEND_ACC_PUBLIC)
	ZEND_ME(UConverter, setSourceEncoding, arginfo_class_UConverter_setSourceEncoding, ZEND_ACC_PUBLIC)
	ZEND_ME(UConverter, setSubstChars, arginfo_class_UConverter_setSubstChars, ZEND_ACC_PUBLIC)
	ZEND_ME(UConverter, toUCallback, arginfo_class_UConverter_toUCallback, ZEND_ACC_PUBLIC)
	ZEND_ME(UConverter, transcode, arginfo_class_UConverter_transcode, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_UConverter(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "UConverter", class_UConverter_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_NOT_SERIALIZABLE);

	zval const_REASON_UNASSIGNED_value;
	ZVAL_LONG(&const_REASON_UNASSIGNED_value, UCNV_UNASSIGNED);
	zend_string *const_REASON_UNASSIGNED_name = zend_string_init_interned("REASON_UNASSIGNED", sizeof("REASON_UNASSIGNED") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_REASON_UNASSIGNED_name, &const_REASON_UNASSIGNED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_REASON_UNASSIGNED_name, true);

	zval const_REASON_ILLEGAL_value;
	ZVAL_LONG(&const_REASON_ILLEGAL_value, UCNV_ILLEGAL);
	zend_string *const_REASON_ILLEGAL_name = zend_string_init_interned("REASON_ILLEGAL", sizeof("REASON_ILLEGAL") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_REASON_ILLEGAL_name, &const_REASON_ILLEGAL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_REASON_ILLEGAL_name, true);

	zval const_REASON_IRREGULAR_value;
	ZVAL_LONG(&const_REASON_IRREGULAR_value, UCNV_IRREGULAR);
	zend_string *const_REASON_IRREGULAR_name = zend_string_init_interned("REASON_IRREGULAR", sizeof("REASON_IRREGULAR") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_REASON_IRREGULAR_name, &const_REASON_IRREGULAR_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_REASON_IRREGULAR_name, true);

	zval const_REASON_RESET_value;
	ZVAL_LONG(&const_REASON_RESET_value, UCNV_RESET);
	zend_string *const_REASON_RESET_name = zend_string_init_interned("REASON_RESET", sizeof("REASON_RESET") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_REASON_RESET_name, &const_REASON_RESET_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_REASON_RESET_name, true);

	zval const_REASON_CLOSE_value;
	ZVAL_LONG(&const_REASON_CLOSE_value, UCNV_CLOSE);
	zend_string *const_REASON_CLOSE_name = zend_string_init_interned("REASON_CLOSE", sizeof("REASON_CLOSE") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_REASON_CLOSE_name, &const_REASON_CLOSE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_REASON_CLOSE_name, true);

	zval const_REASON_CLONE_value;
	ZVAL_LONG(&const_REASON_CLONE_value, UCNV_CLONE);
	zend_string *const_REASON_CLONE_name = zend_string_init_interned("REASON_CLONE", sizeof("REASON_CLONE") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_REASON_CLONE_name, &const_REASON_CLONE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_REASON_CLONE_name, true);

	zval const_UNSUPPORTED_CONVERTER_value;
	ZVAL_LONG(&const_UNSUPPORTED_CONVERTER_value, UCNV_UNSUPPORTED_CONVERTER);
	zend_string *const_UNSUPPORTED_CONVERTER_name = zend_string_init_interned("UNSUPPORTED_CONVERTER", sizeof("UNSUPPORTED_CONVERTER") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_UNSUPPORTED_CONVERTER_name, &const_UNSUPPORTED_CONVERTER_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_UNSUPPORTED_CONVERTER_name, true);

	zval const_SBCS_value;
	ZVAL_LONG(&const_SBCS_value, UCNV_SBCS);
	zend_string *const_SBCS_name = zend_string_init_interned("SBCS", sizeof("SBCS") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_SBCS_name, &const_SBCS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_SBCS_name, true);

	zval const_DBCS_value;
	ZVAL_LONG(&const_DBCS_value, UCNV_DBCS);
	zend_string *const_DBCS_name = zend_string_init_interned("DBCS", sizeof("DBCS") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_DBCS_name, &const_DBCS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_DBCS_name, true);

	zval const_MBCS_value;
	ZVAL_LONG(&const_MBCS_value, UCNV_MBCS);
	zend_string *const_MBCS_name = zend_string_init_interned("MBCS", sizeof("MBCS") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_MBCS_name, &const_MBCS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_MBCS_name, true);

	zval const_LATIN_1_value;
	ZVAL_LONG(&const_LATIN_1_value, UCNV_LATIN_1);
	zend_string *const_LATIN_1_name = zend_string_init_interned("LATIN_1", sizeof("LATIN_1") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_LATIN_1_name, &const_LATIN_1_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_LATIN_1_name, true);

	zval const_UTF8_value;
	ZVAL_LONG(&const_UTF8_value, UCNV_UTF8);
	zend_string *const_UTF8_name = zend_string_init_interned("UTF8", sizeof("UTF8") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_UTF8_name, &const_UTF8_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_UTF8_name, true);

	zval const_UTF16_BigEndian_value;
	ZVAL_LONG(&const_UTF16_BigEndian_value, UCNV_UTF16_BigEndian);
	zend_string *const_UTF16_BigEndian_name = zend_string_init_interned("UTF16_BigEndian", sizeof("UTF16_BigEndian") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_UTF16_BigEndian_name, &const_UTF16_BigEndian_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_UTF16_BigEndian_name, true);

	zval const_UTF16_LittleEndian_value;
	ZVAL_LONG(&const_UTF16_LittleEndian_value, UCNV_UTF16_LittleEndian);
	zend_string *const_UTF16_LittleEndian_name = zend_string_init_interned("UTF16_LittleEndian", sizeof("UTF16_LittleEndian") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_UTF16_LittleEndian_name, &const_UTF16_LittleEndian_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_UTF16_LittleEndian_name, true);

	zval const_UTF32_BigEndian_value;
	ZVAL_LONG(&const_UTF32_BigEndian_value, UCNV_UTF32_BigEndian);
	zend_string *const_UTF32_BigEndian_name = zend_string_init_interned("UTF32_BigEndian", sizeof("UTF32_BigEndian") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_UTF32_BigEndian_name, &const_UTF32_BigEndian_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_UTF32_BigEndian_name, true);

	zval const_UTF32_LittleEndian_value;
	ZVAL_LONG(&const_UTF32_LittleEndian_value, UCNV_UTF32_LittleEndian);
	zend_string *const_UTF32_LittleEndian_name = zend_string_init_interned("UTF32_LittleEndian", sizeof("UTF32_LittleEndian") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_UTF32_LittleEndian_name, &const_UTF32_LittleEndian_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_UTF32_LittleEndian_name, true);

	zval const_EBCDIC_STATEFUL_value;
	ZVAL_LONG(&const_EBCDIC_STATEFUL_value, UCNV_EBCDIC_STATEFUL);
	zend_string *const_EBCDIC_STATEFUL_name = zend_string_init_interned("EBCDIC_STATEFUL", sizeof("EBCDIC_STATEFUL") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_EBCDIC_STATEFUL_name, &const_EBCDIC_STATEFUL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_EBCDIC_STATEFUL_name, true);

	zval const_ISO_2022_value;
	ZVAL_LONG(&const_ISO_2022_value, UCNV_ISO_2022);
	zend_string *const_ISO_2022_name = zend_string_init_interned("ISO_2022", sizeof("ISO_2022") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_ISO_2022_name, &const_ISO_2022_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_ISO_2022_name, true);

	zval const_LMBCS_1_value;
	ZVAL_LONG(&const_LMBCS_1_value, UCNV_LMBCS_1);
	zend_string *const_LMBCS_1_name = zend_string_init_interned("LMBCS_1", sizeof("LMBCS_1") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_LMBCS_1_name, &const_LMBCS_1_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_LMBCS_1_name, true);

	zval const_LMBCS_2_value;
	ZVAL_LONG(&const_LMBCS_2_value, UCNV_LMBCS_2);
	zend_string *const_LMBCS_2_name = zend_string_init_interned("LMBCS_2", sizeof("LMBCS_2") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_LMBCS_2_name, &const_LMBCS_2_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_LMBCS_2_name, true);

	zval const_LMBCS_3_value;
	ZVAL_LONG(&const_LMBCS_3_value, UCNV_LMBCS_3);
	zend_string *const_LMBCS_3_name = zend_string_init_interned("LMBCS_3", sizeof("LMBCS_3") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_LMBCS_3_name, &const_LMBCS_3_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_LMBCS_3_name, true);

	zval const_LMBCS_4_value;
	ZVAL_LONG(&const_LMBCS_4_value, UCNV_LMBCS_4);
	zend_string *const_LMBCS_4_name = zend_string_init_interned("LMBCS_4", sizeof("LMBCS_4") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_LMBCS_4_name, &const_LMBCS_4_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_LMBCS_4_name, true);

	zval const_LMBCS_5_value;
	ZVAL_LONG(&const_LMBCS_5_value, UCNV_LMBCS_5);
	zend_string *const_LMBCS_5_name = zend_string_init_interned("LMBCS_5", sizeof("LMBCS_5") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_LMBCS_5_name, &const_LMBCS_5_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_LMBCS_5_name, true);

	zval const_LMBCS_6_value;
	ZVAL_LONG(&const_LMBCS_6_value, UCNV_LMBCS_6);
	zend_string *const_LMBCS_6_name = zend_string_init_interned("LMBCS_6", sizeof("LMBCS_6") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_LMBCS_6_name, &const_LMBCS_6_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_LMBCS_6_name, true);

	zval const_LMBCS_8_value;
	ZVAL_LONG(&const_LMBCS_8_value, UCNV_LMBCS_8);
	zend_string *const_LMBCS_8_name = zend_string_init_interned("LMBCS_8", sizeof("LMBCS_8") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_LMBCS_8_name, &const_LMBCS_8_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_LMBCS_8_name, true);

	zval const_LMBCS_11_value;
	ZVAL_LONG(&const_LMBCS_11_value, UCNV_LMBCS_11);
	zend_string *const_LMBCS_11_name = zend_string_init_interned("LMBCS_11", sizeof("LMBCS_11") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_LMBCS_11_name, &const_LMBCS_11_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_LMBCS_11_name, true);

	zval const_LMBCS_16_value;
	ZVAL_LONG(&const_LMBCS_16_value, UCNV_LMBCS_16);
	zend_string *const_LMBCS_16_name = zend_string_init_interned("LMBCS_16", sizeof("LMBCS_16") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_LMBCS_16_name, &const_LMBCS_16_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_LMBCS_16_name, true);

	zval const_LMBCS_17_value;
	ZVAL_LONG(&const_LMBCS_17_value, UCNV_LMBCS_17);
	zend_string *const_LMBCS_17_name = zend_string_init_interned("LMBCS_17", sizeof("LMBCS_17") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_LMBCS_17_name, &const_LMBCS_17_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_LMBCS_17_name, true);

	zval const_LMBCS_18_value;
	ZVAL_LONG(&const_LMBCS_18_value, UCNV_LMBCS_18);
	zend_string *const_LMBCS_18_name = zend_string_init_interned("LMBCS_18", sizeof("LMBCS_18") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_LMBCS_18_name, &const_LMBCS_18_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_LMBCS_18_name, true);

	zval const_LMBCS_19_value;
	ZVAL_LONG(&const_LMBCS_19_value, UCNV_LMBCS_19);
	zend_string *const_LMBCS_19_name = zend_string_init_interned("LMBCS_19", sizeof("LMBCS_19") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_LMBCS_19_name, &const_LMBCS_19_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_LMBCS_19_name, true);

	zval const_LMBCS_LAST_value;
	ZVAL_LONG(&const_LMBCS_LAST_value, UCNV_LMBCS_LAST);
	zend_string *const_LMBCS_LAST_name = zend_string_init_interned("LMBCS_LAST", sizeof("LMBCS_LAST") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_LMBCS_LAST_name, &const_LMBCS_LAST_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_LMBCS_LAST_name, true);

	zval const_HZ_value;
	ZVAL_LONG(&const_HZ_value, UCNV_HZ);
	zend_string *const_HZ_name = zend_string_init_interned("HZ", sizeof("HZ") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_HZ_name, &const_HZ_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_HZ_name, true);

	zval const_SCSU_value;
	ZVAL_LONG(&const_SCSU_value, UCNV_SCSU);
	zend_string *const_SCSU_name = zend_string_init_interned("SCSU", sizeof("SCSU") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_SCSU_name, &const_SCSU_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_SCSU_name, true);

	zval const_ISCII_value;
	ZVAL_LONG(&const_ISCII_value, UCNV_ISCII);
	zend_string *const_ISCII_name = zend_string_init_interned("ISCII", sizeof("ISCII") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_ISCII_name, &const_ISCII_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_ISCII_name, true);

	zval const_US_ASCII_value;
	ZVAL_LONG(&const_US_ASCII_value, UCNV_US_ASCII);
	zend_string *const_US_ASCII_name = zend_string_init_interned("US_ASCII", sizeof("US_ASCII") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_US_ASCII_name, &const_US_ASCII_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_US_ASCII_name, true);

	zval const_UTF7_value;
	ZVAL_LONG(&const_UTF7_value, UCNV_UTF7);
	zend_string *const_UTF7_name = zend_string_init_interned("UTF7", sizeof("UTF7") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_UTF7_name, &const_UTF7_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_UTF7_name, true);

	zval const_BOCU1_value;
	ZVAL_LONG(&const_BOCU1_value, UCNV_BOCU1);
	zend_string *const_BOCU1_name = zend_string_init_interned("BOCU1", sizeof("BOCU1") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_BOCU1_name, &const_BOCU1_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_BOCU1_name, true);

	zval const_UTF16_value;
	ZVAL_LONG(&const_UTF16_value, UCNV_UTF16);
	zend_string *const_UTF16_name = zend_string_init_interned("UTF16", sizeof("UTF16") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_UTF16_name, &const_UTF16_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_UTF16_name, true);

	zval const_UTF32_value;
	ZVAL_LONG(&const_UTF32_value, UCNV_UTF32);
	zend_string *const_UTF32_name = zend_string_init_interned("UTF32", sizeof("UTF32") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_UTF32_name, &const_UTF32_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_UTF32_name, true);

	zval const_CESU8_value;
	ZVAL_LONG(&const_CESU8_value, UCNV_CESU8);
	zend_string *const_CESU8_name = zend_string_init_interned("CESU8", sizeof("CESU8") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CESU8_name, &const_CESU8_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CESU8_name, true);

	zval const_IMAP_MAILBOX_value;
	ZVAL_LONG(&const_IMAP_MAILBOX_value, UCNV_IMAP_MAILBOX);
	zend_string *const_IMAP_MAILBOX_name = zend_string_init_interned("IMAP_MAILBOX", sizeof("IMAP_MAILBOX") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_IMAP_MAILBOX_name, &const_IMAP_MAILBOX_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_IMAP_MAILBOX_name, true);

	return class_entry;
}
