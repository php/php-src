/* This is a generated file, edit encoding.stub.php instead.
 * Stub hash: ada1d3a8882e099da6ed20ce66a7c6ebacf95dee
 * Has decl header: yes */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_Encoding_base16_encode, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, variant, Encoding\\Base16, 0, "Encoding\\Base16::Upper")
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, timingMode, Encoding\\TimingMode, 0, "Encoding\\TimingMode::Variable")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_Encoding_base16_decode, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, variant, Encoding\\Base16, 0, "Encoding\\Base16::Upper")
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, decodingMode, Encoding\\DecodingMode, 0, "Encoding\\DecodingMode::Strict")
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, timingMode, Encoding\\TimingMode, 0, "Encoding\\TimingMode::Variable")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_Encoding_base32_encode, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, variant, Encoding\\Base32, 0, "Encoding\\Base32::Ascii")
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, paddingMode, Encoding\\PaddingMode, 0, "Encoding\\PaddingMode::VariantControlled")
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, timingMode, Encoding\\TimingMode, 0, "Encoding\\TimingMode::Variable")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_Encoding_base32_decode, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, variant, Encoding\\Base32, 0, "Encoding\\Base32::Ascii")
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, decodingMode, Encoding\\DecodingMode, 0, "Encoding\\DecodingMode::Strict")
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, timingMode, Encoding\\TimingMode, 0, "Encoding\\TimingMode::Variable")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_Encoding_base58_encode, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, variant, Encoding\\Base58, 0, "Encoding\\Base58::Bitcoin")
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, timingMode, Encoding\\TimingMode, 0, "Encoding\\TimingMode::Variable")
ZEND_END_ARG_INFO()

#define arginfo_Encoding_base58_decode arginfo_Encoding_base58_encode

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_Encoding_base64_encode, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, variant, Encoding\\Base64, 0, "Encoding\\Base64::Standard")
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, paddingMode, Encoding\\PaddingMode, 0, "Encoding\\PaddingMode::VariantControlled")
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, timingMode, Encoding\\TimingMode, 0, "Encoding\\TimingMode::Variable")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_Encoding_base64_decode, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, variant, Encoding\\Base64, 0, "Encoding\\Base64::Standard")
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, decodingMode, Encoding\\DecodingMode, 0, "Encoding\\DecodingMode::Strict")
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, timingMode, Encoding\\TimingMode, 0, "Encoding\\TimingMode::Variable")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_Encoding_base85_encode, 0, 2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO(0, variant, Encoding\\Base85, 0)
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, paddingMode, Encoding\\PaddingMode, 0, "Encoding\\PaddingMode::VariantControlled")
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, timingMode, Encoding\\TimingMode, 0, "Encoding\\TimingMode::Variable")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_Encoding_base85_decode, 0, 2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO(0, variant, Encoding\\Base85, 0)
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, decodingMode, Encoding\\DecodingMode, 0, "Encoding\\DecodingMode::Strict")
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, timingMode, Encoding\\TimingMode, 0, "Encoding\\TimingMode::Variable")
ZEND_END_ARG_INFO()

ZEND_FUNCTION(Encoding_base16_encode);
ZEND_FUNCTION(Encoding_base16_decode);
ZEND_FUNCTION(Encoding_base32_encode);
ZEND_FUNCTION(Encoding_base32_decode);
ZEND_FUNCTION(Encoding_base58_encode);
ZEND_FUNCTION(Encoding_base58_decode);
ZEND_FUNCTION(Encoding_base64_encode);
ZEND_FUNCTION(Encoding_base64_decode);
ZEND_FUNCTION(Encoding_base85_encode);
ZEND_FUNCTION(Encoding_base85_decode);

static const zend_function_entry ext_functions[] = {
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Encoding", "base16_encode"), zif_Encoding_base16_encode, arginfo_Encoding_base16_encode, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Encoding", "base16_decode"), zif_Encoding_base16_decode, arginfo_Encoding_base16_decode, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Encoding", "base32_encode"), zif_Encoding_base32_encode, arginfo_Encoding_base32_encode, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Encoding", "base32_decode"), zif_Encoding_base32_decode, arginfo_Encoding_base32_decode, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Encoding", "base58_encode"), zif_Encoding_base58_encode, arginfo_Encoding_base58_encode, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Encoding", "base58_decode"), zif_Encoding_base58_decode, arginfo_Encoding_base58_decode, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Encoding", "base64_encode"), zif_Encoding_base64_encode, arginfo_Encoding_base64_encode, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Encoding", "base64_decode"), zif_Encoding_base64_decode, arginfo_Encoding_base64_decode, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Encoding", "base85_encode"), zif_Encoding_base85_encode, arginfo_Encoding_base85_encode, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Encoding", "base85_decode"), zif_Encoding_base85_decode, arginfo_Encoding_base85_decode, 0, NULL, NULL)
	ZEND_FE_END
};

static zend_class_entry *register_class_Encoding_EncodingError(zend_class_entry *class_entry_Error)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Encoding", "EncodingError", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_Error, ZEND_ACC_NO_DYNAMIC_PROPERTIES);

	return class_entry;
}

static zend_class_entry *register_class_Encoding_EncodingException(zend_class_entry *class_entry_Exception)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Encoding", "EncodingException", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_Exception, ZEND_ACC_NO_DYNAMIC_PROPERTIES);

	return class_entry;
}

static zend_class_entry *register_class_Encoding_UnableToDecodeException(zend_class_entry *class_entry_Encoding_EncodingException)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Encoding", "UnableToDecodeException", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_Encoding_EncodingException, ZEND_ACC_NO_DYNAMIC_PROPERTIES);

	return class_entry;
}

static zend_class_entry *register_class_Encoding_UnableToEncodeException(zend_class_entry *class_entry_Encoding_EncodingException)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Encoding", "UnableToEncodeException", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_Encoding_EncodingException, ZEND_ACC_NO_DYNAMIC_PROPERTIES);

	return class_entry;
}

static zend_class_entry *register_class_Encoding_Base16(void)
{
	zend_class_entry *class_entry = zend_register_internal_enum("Encoding\\Base16", IS_UNDEF, NULL);

	zend_enum_add_case_cstr(class_entry, "Upper", NULL);

	zend_enum_add_case_cstr(class_entry, "Lower", NULL);

	return class_entry;
}

static zend_class_entry *register_class_Encoding_Base32(void)
{
	zend_class_entry *class_entry = zend_register_internal_enum("Encoding\\Base32", IS_UNDEF, NULL);

	zend_enum_add_case_cstr(class_entry, "Ascii", NULL);

	zend_enum_add_case_cstr(class_entry, "Hex", NULL);

	zend_enum_add_case_cstr(class_entry, "Crockford", NULL);

	zend_enum_add_case_cstr(class_entry, "Z", NULL);

	return class_entry;
}

static zend_class_entry *register_class_Encoding_Base58(void)
{
	zend_class_entry *class_entry = zend_register_internal_enum("Encoding\\Base58", IS_UNDEF, NULL);

	zend_enum_add_case_cstr(class_entry, "Bitcoin", NULL);

	zend_enum_add_case_cstr(class_entry, "Flickr", NULL);

	return class_entry;
}

static zend_class_entry *register_class_Encoding_Base64(void)
{
	zend_class_entry *class_entry = zend_register_internal_enum("Encoding\\Base64", IS_UNDEF, NULL);

	zend_enum_add_case_cstr(class_entry, "Standard", NULL);

	zend_enum_add_case_cstr(class_entry, "UrlSafe", NULL);

	zend_enum_add_case_cstr(class_entry, "Imap", NULL);

	return class_entry;
}

static zend_class_entry *register_class_Encoding_Base85(void)
{
	zend_class_entry *class_entry = zend_register_internal_enum("Encoding\\Base85", IS_UNDEF, NULL);

	zend_enum_add_case_cstr(class_entry, "Adobe", NULL);

	zend_enum_add_case_cstr(class_entry, "Z85", NULL);

	zend_enum_add_case_cstr(class_entry, "Git", NULL);

	return class_entry;
}

static zend_class_entry *register_class_Encoding_PaddingMode(void)
{
	zend_class_entry *class_entry = zend_register_internal_enum("Encoding\\PaddingMode", IS_UNDEF, NULL);

	zend_enum_add_case_cstr(class_entry, "VariantControlled", NULL);

	zend_enum_add_case_cstr(class_entry, "StripPadding", NULL);

	zend_enum_add_case_cstr(class_entry, "PreservePadding", NULL);

	return class_entry;
}

static zend_class_entry *register_class_Encoding_DecodingMode(void)
{
	zend_class_entry *class_entry = zend_register_internal_enum("Encoding\\DecodingMode", IS_UNDEF, NULL);

	zend_enum_add_case_cstr(class_entry, "Forgiving", NULL);

	zend_enum_add_case_cstr(class_entry, "Strict", NULL);

	return class_entry;
}

static zend_class_entry *register_class_Encoding_TimingMode(void)
{
	zend_class_entry *class_entry = zend_register_internal_enum("Encoding\\TimingMode", IS_UNDEF, NULL);

	zend_enum_add_case_cstr(class_entry, "Variable", NULL);

	zend_enum_add_case_cstr(class_entry, "Constant", NULL);

	return class_entry;
}
