/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: c198614563ff0b54d21c7572fe747ddc3191afa3 */

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_Normalizer_normalize, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, form, IS_LONG, 0, "Normalizer::FORM_C")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_Normalizer_isNormalized, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, form, IS_LONG, 0, "Normalizer::FORM_C")
ZEND_END_ARG_INFO()

#if U_ICU_VERSION_MAJOR_NUM >= 56
ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_Normalizer_getRawDecomposition, 0, 1, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, form, IS_LONG, 0, "Normalizer::FORM_C")
ZEND_END_ARG_INFO()
#endif

ZEND_FUNCTION(normalizer_normalize);
ZEND_FUNCTION(normalizer_is_normalized);
#if U_ICU_VERSION_MAJOR_NUM >= 56
ZEND_FUNCTION(normalizer_get_raw_decomposition);
#endif

static const zend_function_entry class_Normalizer_methods[] = {
	ZEND_ME_MAPPING(normalize, normalizer_normalize, arginfo_class_Normalizer_normalize, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME_MAPPING(isNormalized, normalizer_is_normalized, arginfo_class_Normalizer_isNormalized, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
#if U_ICU_VERSION_MAJOR_NUM >= 56
	ZEND_ME_MAPPING(getRawDecomposition, normalizer_get_raw_decomposition, arginfo_class_Normalizer_getRawDecomposition, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
#endif
	ZEND_FE_END
};

static zend_class_entry *register_class_Normalizer(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "Normalizer", class_Normalizer_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);

	zval const_FORM_D_value;
	ZVAL_LONG(&const_FORM_D_value, NORMALIZER_FORM_D);
	zend_string *const_FORM_D_name = zend_string_init_interned("FORM_D", sizeof("FORM_D") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FORM_D_name, &const_FORM_D_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FORM_D_name);

	zval const_NFD_value;
	ZVAL_LONG(&const_NFD_value, NORMALIZER_NFD);
	zend_string *const_NFD_name = zend_string_init_interned("NFD", sizeof("NFD") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_NFD_name, &const_NFD_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_NFD_name);

	zval const_FORM_KD_value;
	ZVAL_LONG(&const_FORM_KD_value, NORMALIZER_FORM_KD);
	zend_string *const_FORM_KD_name = zend_string_init_interned("FORM_KD", sizeof("FORM_KD") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FORM_KD_name, &const_FORM_KD_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FORM_KD_name);

	zval const_NFKD_value;
	ZVAL_LONG(&const_NFKD_value, NORMALIZER_NFKD);
	zend_string *const_NFKD_name = zend_string_init_interned("NFKD", sizeof("NFKD") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_NFKD_name, &const_NFKD_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_NFKD_name);

	zval const_FORM_C_value;
	ZVAL_LONG(&const_FORM_C_value, NORMALIZER_FORM_C);
	zend_string *const_FORM_C_name = zend_string_init_interned("FORM_C", sizeof("FORM_C") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FORM_C_name, &const_FORM_C_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FORM_C_name);

	zval const_NFC_value;
	ZVAL_LONG(&const_NFC_value, NORMALIZER_NFC);
	zend_string *const_NFC_name = zend_string_init_interned("NFC", sizeof("NFC") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_NFC_name, &const_NFC_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_NFC_name);

	zval const_FORM_KC_value;
	ZVAL_LONG(&const_FORM_KC_value, NORMALIZER_FORM_KC);
	zend_string *const_FORM_KC_name = zend_string_init_interned("FORM_KC", sizeof("FORM_KC") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FORM_KC_name, &const_FORM_KC_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FORM_KC_name);

	zval const_NFKC_value;
	ZVAL_LONG(&const_NFKC_value, NORMALIZER_NFKC);
	zend_string *const_NFKC_name = zend_string_init_interned("NFKC", sizeof("NFKC") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_NFKC_name, &const_NFKC_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_NFKC_name);
#if U_ICU_VERSION_MAJOR_NUM >= 56

	zval const_FORM_KC_CF_value;
	ZVAL_LONG(&const_FORM_KC_CF_value, NORMALIZER_FORM_KC_CF);
	zend_string *const_FORM_KC_CF_name = zend_string_init_interned("FORM_KC_CF", sizeof("FORM_KC_CF") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FORM_KC_CF_name, &const_FORM_KC_CF_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FORM_KC_CF_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 56

	zval const_NFKC_CF_value;
	ZVAL_LONG(&const_NFKC_CF_value, NORMALIZER_NFKC_CF);
	zend_string *const_NFKC_CF_name = zend_string_init_interned("NFKC_CF", sizeof("NFKC_CF") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_NFKC_CF_name, &const_NFKC_CF_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_NFKC_CF_name);
#endif

	return class_entry;
}
