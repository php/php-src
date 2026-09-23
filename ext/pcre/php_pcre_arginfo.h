/* This is a generated file, edit php_pcre.stub.php instead.
 * Stub hash: 98c059222480865294af3d16f618e0c81c5f1e41 */

#include "zend_constants.h"

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_preg_match, 0, 2, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, subject, IS_STRING, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, matches, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, offset, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_preg_match_all arginfo_preg_match

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_preg_replace, 0, 3, MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_NULL)
	ZEND_ARG_TYPE_MASK(0, pattern, MAY_BE_STRING|MAY_BE_ARRAY, NULL)
	ZEND_ARG_TYPE_MASK(0, replacement, MAY_BE_STRING|MAY_BE_ARRAY, NULL)
	ZEND_ARG_TYPE_MASK(0, subject, MAY_BE_STRING|MAY_BE_ARRAY, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, limit, IS_LONG, 0, "-1")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, count, "null")
ZEND_END_ARG_INFO()

#define arginfo_preg_filter arginfo_preg_replace

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_preg_replace_callback, 0, 3, MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_NULL)
	ZEND_ARG_TYPE_MASK(0, pattern, MAY_BE_STRING|MAY_BE_ARRAY, NULL)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
	ZEND_ARG_TYPE_MASK(0, subject, MAY_BE_STRING|MAY_BE_ARRAY, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, limit, IS_LONG, 0, "-1")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, count, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_preg_replace_callback_array, 0, 2, MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_NULL)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_ARRAY, 0)
	ZEND_ARG_TYPE_MASK(0, subject, MAY_BE_STRING|MAY_BE_ARRAY, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, limit, IS_LONG, 0, "-1")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, count, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_preg_split, 0, 2, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, subject, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, limit, IS_LONG, 0, "-1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_preg_quote, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, delimiter, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_preg_grep, 0, 2, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_preg_last_error, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_preg_last_error_msg, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Regex_CompiledRegex___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, caseSensitive, _IS_BOOL, 0, "true")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, greedy, _IS_BOOL, 0, "true")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, anchor, _IS_BOOL, 0, "false")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, multiLine, _IS_BOOL, 0, "false")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, dotMatchesNewLine, _IS_BOOL, 0, "false")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, ignoreWhitespace, _IS_BOOL, 0, "false")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, captureOnlyNamedGroups, _IS_BOOL, 0, "false")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, allowDuplicateSubPatternNames, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()


ZEND_FRAMELESS_FUNCTION(preg_match, 2);
static const zend_frameless_function_info frameless_function_infos_preg_match[] = {
	{ ZEND_FRAMELESS_FUNCTION_NAME(preg_match, 2), 2 },
	{ 0 },
};

ZEND_FRAMELESS_FUNCTION(preg_replace, 3);
static const zend_frameless_function_info frameless_function_infos_preg_replace[] = {
	{ ZEND_FRAMELESS_FUNCTION_NAME(preg_replace, 3), 3 },
	{ 0 },
};

ZEND_FUNCTION(preg_match);
ZEND_FUNCTION(preg_match_all);
ZEND_FUNCTION(preg_replace);
ZEND_FUNCTION(preg_filter);
ZEND_FUNCTION(preg_replace_callback);
ZEND_FUNCTION(preg_replace_callback_array);
ZEND_FUNCTION(preg_split);
ZEND_FUNCTION(preg_quote);
ZEND_FUNCTION(preg_grep);
ZEND_FUNCTION(preg_last_error);
ZEND_FUNCTION(preg_last_error_msg);
ZEND_METHOD(Regex_CompiledRegex, __construct);

static const zend_function_entry ext_functions[] = {
	ZEND_RAW_FENTRY("preg_match", zif_preg_match, arginfo_preg_match, 0, frameless_function_infos_preg_match, NULL)
	ZEND_FE(preg_match_all, arginfo_preg_match_all)
	ZEND_RAW_FENTRY("preg_replace", zif_preg_replace, arginfo_preg_replace, 0, frameless_function_infos_preg_replace, NULL)
	ZEND_FE(preg_filter, arginfo_preg_filter)
	ZEND_FE(preg_replace_callback, arginfo_preg_replace_callback)
	ZEND_FE(preg_replace_callback_array, arginfo_preg_replace_callback_array)
	ZEND_FE(preg_split, arginfo_preg_split)
	ZEND_RAW_FENTRY("preg_quote", zif_preg_quote, arginfo_preg_quote, ZEND_ACC_COMPILE_TIME_EVAL, NULL, NULL)
	ZEND_FE(preg_grep, arginfo_preg_grep)
	ZEND_FE(preg_last_error, arginfo_preg_last_error)
	ZEND_FE(preg_last_error_msg, arginfo_preg_last_error_msg)
	ZEND_FE_END
};

static const zend_function_entry class_Regex_CompiledRegex_methods[] = {
	ZEND_ME(Regex_CompiledRegex, __construct, arginfo_class_Regex_CompiledRegex___construct, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static void register_php_pcre_symbols(int module_number)
{
	REGISTER_LONG_CONSTANT("PREG_PATTERN_ORDER", PREG_PATTERN_ORDER, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PREG_SET_ORDER", PREG_SET_ORDER, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PREG_OFFSET_CAPTURE", PREG_OFFSET_CAPTURE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PREG_UNMATCHED_AS_NULL", PREG_UNMATCHED_AS_NULL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PREG_SPLIT_NO_EMPTY", PREG_SPLIT_NO_EMPTY, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PREG_SPLIT_DELIM_CAPTURE", PREG_SPLIT_DELIM_CAPTURE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PREG_SPLIT_OFFSET_CAPTURE", PREG_SPLIT_OFFSET_CAPTURE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PREG_GREP_INVERT", PREG_GREP_INVERT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PREG_NO_ERROR", PHP_PCRE_NO_ERROR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PREG_INTERNAL_ERROR", PHP_PCRE_INTERNAL_ERROR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PREG_BACKTRACK_LIMIT_ERROR", PHP_PCRE_BACKTRACK_LIMIT_ERROR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PREG_RECURSION_LIMIT_ERROR", PHP_PCRE_RECURSION_LIMIT_ERROR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PREG_BAD_UTF8_ERROR", PHP_PCRE_BAD_UTF8_ERROR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PREG_BAD_UTF8_OFFSET_ERROR", PHP_PCRE_BAD_UTF8_OFFSET_ERROR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PREG_JIT_STACKLIMIT_ERROR", PHP_PCRE_JIT_STACKLIMIT_ERROR, CONST_PERSISTENT);
	REGISTER_STRING_CONSTANT("PCRE_VERSION", php_pcre_version, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PCRE_VERSION_MAJOR", PCRE2_MAJOR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PCRE_VERSION_MINOR", PCRE2_MINOR, CONST_PERSISTENT);
	REGISTER_BOOL_CONSTANT("PCRE_JIT_SUPPORT", PHP_PCRE_JIT_SUPPORT, CONST_PERSISTENT);
}

static zend_class_entry *register_class_Regex_CompilationError(zend_class_entry *class_entry_Regex_Exception)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Regex", "CompilationError", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_Regex_Exception, 0);

	return class_entry;
}

static zend_class_entry *register_class_Regex_CompiledRegex(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Regex", "CompiledRegex", class_Regex_CompiledRegex_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE);

	zval const_NO_AUTO_CAPTURE_value;
	ZVAL_LONG(&const_NO_AUTO_CAPTURE_value, PCRE2_NO_AUTO_CAPTURE);
	zend_string *const_NO_AUTO_CAPTURE_name = zend_string_init_interned("NO_AUTO_CAPTURE", sizeof("NO_AUTO_CAPTURE") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_NO_AUTO_CAPTURE_name, &const_NO_AUTO_CAPTURE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_NO_AUTO_CAPTURE_name, true);

	zval const_ANCHORED_value;
	ZVAL_LONG(&const_ANCHORED_value, PCRE2_ANCHORED);
	zend_string *const_ANCHORED_name = zend_string_init_interned("ANCHORED", sizeof("ANCHORED") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_ANCHORED_name, &const_ANCHORED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_ANCHORED_name, true);

	zval const_DUPNAMES_value;
	ZVAL_LONG(&const_DUPNAMES_value, PCRE2_DUPNAMES);
	zend_string *const_DUPNAMES_name = zend_string_init_interned("DUPNAMES", sizeof("DUPNAMES") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_DUPNAMES_name, &const_DUPNAMES_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_DUPNAMES_name, true);
#if defined(PCRE2_EXTRA_CASELESS_RESTRICT)

	zval const_EXTRA_CASELESS_RESTRICT_value;
	ZVAL_LONG(&const_EXTRA_CASELESS_RESTRICT_value, PCRE2_EXTRA_CASELESS_RESTRICT);
	zend_string *const_EXTRA_CASELESS_RESTRICT_name = zend_string_init_interned("EXTRA_CASELESS_RESTRICT", sizeof("EXTRA_CASELESS_RESTRICT") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_EXTRA_CASELESS_RESTRICT_name, &const_EXTRA_CASELESS_RESTRICT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_EXTRA_CASELESS_RESTRICT_name, true);
#endif

	return class_entry;
}
