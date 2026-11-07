/* This is a generated file, edit strmatch.stub.php instead.
 * Stub hash: a9596ed4be91cbd0089cf33773381e66261de065 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Str_Matcher___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, keywords, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Str_Matcher_matches, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, subject, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Str_Matcher_countMatches, 0, 1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, subject, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Str_Matcher_getMatchedKeywords, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, subject, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Str_Matcher_matchAll arginfo_class_Str_Matcher_getMatchedKeywords

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Str_Matcher_getKeywords, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Str_MatchResult___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Str_Replacer___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, replacements, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Str_Replacer_replace, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, subject, IS_STRING, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, count, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_Str_Replacer_getReplacements arginfo_class_Str_Matcher_getKeywords

ZEND_METHOD(Str_Matcher, __construct);
ZEND_METHOD(Str_Matcher, matches);
ZEND_METHOD(Str_Matcher, countMatches);
ZEND_METHOD(Str_Matcher, getMatchedKeywords);
ZEND_METHOD(Str_Matcher, matchAll);
ZEND_METHOD(Str_Matcher, getKeywords);
ZEND_METHOD(Str_MatchResult, __construct);
ZEND_METHOD(Str_Replacer, __construct);
ZEND_METHOD(Str_Replacer, replace);
ZEND_METHOD(Str_Replacer, getReplacements);

static const zend_function_entry class_Str_Matcher_methods[] = {
	ZEND_ME(Str_Matcher, __construct, arginfo_class_Str_Matcher___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Str_Matcher, matches, arginfo_class_Str_Matcher_matches, ZEND_ACC_PUBLIC)
	ZEND_ME(Str_Matcher, countMatches, arginfo_class_Str_Matcher_countMatches, ZEND_ACC_PUBLIC)
	ZEND_ME(Str_Matcher, getMatchedKeywords, arginfo_class_Str_Matcher_getMatchedKeywords, ZEND_ACC_PUBLIC)
	ZEND_ME(Str_Matcher, matchAll, arginfo_class_Str_Matcher_matchAll, ZEND_ACC_PUBLIC)
	ZEND_ME(Str_Matcher, getKeywords, arginfo_class_Str_Matcher_getKeywords, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_Str_MatchResult_methods[] = {
	ZEND_ME(Str_MatchResult, __construct, arginfo_class_Str_MatchResult___construct, ZEND_ACC_PRIVATE)
	ZEND_FE_END
};

static const zend_function_entry class_Str_Replacer_methods[] = {
	ZEND_ME(Str_Replacer, __construct, arginfo_class_Str_Replacer___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Str_Replacer, replace, arginfo_class_Str_Replacer_replace, ZEND_ACC_PUBLIC)
	ZEND_ME(Str_Replacer, getReplacements, arginfo_class_Str_Replacer_getReplacements, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Str_Matcher(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Str", "Matcher", class_Str_Matcher_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE);

	return class_entry;
}

static zend_class_entry *register_class_Str_MatchResult(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Str", "MatchResult", class_Str_MatchResult_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES);

	zval property_offset_default_value;
	ZVAL_UNDEF(&property_offset_default_value);
	zend_string *property_offset_name = zend_string_init("offset", sizeof("offset") - 1, true);
	zend_declare_typed_property(class_entry, property_offset_name, &property_offset_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(property_offset_name, true);

	zval property_length_default_value;
	ZVAL_UNDEF(&property_length_default_value);
	zend_string *property_length_name = zend_string_init("length", sizeof("length") - 1, true);
	zend_declare_typed_property(class_entry, property_length_name, &property_length_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(property_length_name, true);

	zval property_keyword_default_value;
	ZVAL_UNDEF(&property_keyword_default_value);
	zend_string *property_keyword_name = zend_string_init("keyword", sizeof("keyword") - 1, true);
	zend_declare_typed_property(class_entry, property_keyword_name, &property_keyword_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release_ex(property_keyword_name, true);

	return class_entry;
}

static zend_class_entry *register_class_Str_Replacer(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Str", "Replacer", class_Str_Replacer_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE);

	return class_entry;
}
