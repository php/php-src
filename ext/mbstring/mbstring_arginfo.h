/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: ea642b9010bc38a3b13710662fef48663d4385e1 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_mb_language, 0, 0, MAY_BE_STRING|MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, language, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_mb_internal_encoding, 0, 0, MAY_BE_STRING|MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encoding, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_mb_http_input, 0, 0, MAY_BE_ARRAY|MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, type, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_mb_http_output arginfo_mb_internal_encoding

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_mb_detect_order, 0, 0, MAY_BE_ARRAY|MAY_BE_BOOL)
	ZEND_ARG_TYPE_MASK(0, encoding, MAY_BE_ARRAY|MAY_BE_STRING|MAY_BE_NULL, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_mb_substitute_character, 0, 0, MAY_BE_STRING|MAY_BE_LONG|MAY_BE_BOOL)
	ZEND_ARG_TYPE_MASK(0, substitute_character, MAY_BE_STRING|MAY_BE_LONG|MAY_BE_NULL, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_mb_preferred_mime_name, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, encoding, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mb_parse_str, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_INFO(1, result)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mb_output_handler, 0, 2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, status, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mb_str_split, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 0, "1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encoding, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mb_strlen, 0, 1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encoding, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_mb_strpos, 0, 2, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, haystack, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, needle, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, offset, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encoding, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_mb_strrpos arginfo_mb_strpos

#define arginfo_mb_stripos arginfo_mb_strpos

#define arginfo_mb_strripos arginfo_mb_strpos

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_mb_strstr, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, haystack, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, needle, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, before_needle, _IS_BOOL, 0, "false")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encoding, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_mb_strrchr arginfo_mb_strstr

#define arginfo_mb_stristr arginfo_mb_strstr

#define arginfo_mb_strrichr arginfo_mb_strstr

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mb_substr_count, 0, 2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, haystack, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, needle, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encoding, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mb_substr, 0, 2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, start, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encoding, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_mb_strcut arginfo_mb_substr

#define arginfo_mb_strwidth arginfo_mb_strlen

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mb_strimwidth, 0, 3, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, start, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, width, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, trim_marker, IS_STRING, 0, "\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encoding, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_mb_convert_encoding, 0, 2, MAY_BE_ARRAY|MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_MASK(0, string, MAY_BE_ARRAY|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO(0, to_encoding, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, from_encoding, MAY_BE_ARRAY|MAY_BE_STRING|MAY_BE_NULL, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mb_convert_case, 0, 2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, mode, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encoding, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mb_strtoupper, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encoding, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_mb_strtolower arginfo_mb_strtoupper

#define arginfo_mb_ucfirst arginfo_mb_strtoupper

#define arginfo_mb_lcfirst arginfo_mb_strtoupper

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mb_trim, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, characters, IS_STRING, 0, "\" \\f\\n\\r\\t\\v\\x00                 　᠎\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encoding, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_mb_ltrim arginfo_mb_trim

#define arginfo_mb_rtrim arginfo_mb_trim

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_mb_detect_encoding, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, encodings, MAY_BE_ARRAY|MAY_BE_STRING|MAY_BE_NULL, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, strict, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mb_list_encodings, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mb_encoding_aliases, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, encoding, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mb_encode_mimeheader, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, charset, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, transfer_encoding, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, newline, IS_STRING, 0, "\"\\r\\n\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, indent, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mb_decode_mimeheader, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mb_convert_kana, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_STRING, 0, "\"KV\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encoding, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_mb_convert_variables, 0, 3, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, to_encoding, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, from_encoding, MAY_BE_ARRAY|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO(1, var, IS_MIXED, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(1, vars, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mb_encode_numericentity, 0, 2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, map, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encoding, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, hex, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mb_decode_numericentity, 0, 2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, map, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encoding, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mb_send_mail, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, to, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, subject, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, message, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, additional_headers, MAY_BE_ARRAY|MAY_BE_STRING, "[]")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, additional_params, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_mb_get_info, 0, 0, MAY_BE_ARRAY|MAY_BE_STRING|MAY_BE_LONG|MAY_BE_FALSE|MAY_BE_NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, type, IS_STRING, 0, "\"all\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mb_check_encoding, 0, 0, _IS_BOOL, 0)
	ZEND_ARG_TYPE_MASK(0, value, MAY_BE_ARRAY|MAY_BE_STRING|MAY_BE_NULL, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encoding, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_mb_scrub arginfo_mb_strtoupper

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_mb_ord, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encoding, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_mb_chr, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, codepoint, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encoding, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mb_str_pad, 0, 2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, pad_string, IS_STRING, 0, "\" \"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, pad_type, IS_LONG, 0, "STR_PAD_RIGHT")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encoding, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

#if defined(HAVE_MBREGEX)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_mb_regex_encoding, 0, 0, MAY_BE_STRING|MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encoding, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_MBREGEX)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mb_ereg, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, matches, "null")
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_MBREGEX)
#define arginfo_mb_eregi arginfo_mb_ereg
#endif

#if defined(HAVE_MBREGEX)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_mb_ereg_replace, 0, 3, MAY_BE_STRING|MAY_BE_FALSE|MAY_BE_NULL)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, replacement, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_MBREGEX)
#define arginfo_mb_eregi_replace arginfo_mb_ereg_replace
#endif

#if defined(HAVE_MBREGEX)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_mb_ereg_replace_callback, 0, 3, MAY_BE_STRING|MAY_BE_FALSE|MAY_BE_NULL)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_MBREGEX)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_mb_split, 0, 2, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, limit, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_MBREGEX)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mb_ereg_match, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_MBREGEX)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mb_ereg_search, 0, 0, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, pattern, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_MBREGEX)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_mb_ereg_search_pos, 0, 0, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, pattern, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_MBREGEX)
#define arginfo_mb_ereg_search_regs arginfo_mb_ereg_search_pos
#endif

#if defined(HAVE_MBREGEX)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mb_ereg_search_init, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, pattern, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_MBREGEX)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_mb_ereg_search_getregs, 0, 0, MAY_BE_ARRAY|MAY_BE_FALSE)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_MBREGEX)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mb_ereg_search_getpos, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_MBREGEX)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mb_ereg_search_setpos, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_MBREGEX)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mb_regex_set_options, 0, 0, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()
#endif


ZEND_FUNCTION(mb_language);
ZEND_FUNCTION(mb_internal_encoding);
ZEND_FUNCTION(mb_http_input);
ZEND_FUNCTION(mb_http_output);
ZEND_FUNCTION(mb_detect_order);
ZEND_FUNCTION(mb_substitute_character);
ZEND_FUNCTION(mb_preferred_mime_name);
ZEND_FUNCTION(mb_parse_str);
ZEND_FUNCTION(mb_output_handler);
ZEND_FUNCTION(mb_str_split);
ZEND_FUNCTION(mb_strlen);
ZEND_FUNCTION(mb_strpos);
ZEND_FUNCTION(mb_strrpos);
ZEND_FUNCTION(mb_stripos);
ZEND_FUNCTION(mb_strripos);
ZEND_FUNCTION(mb_strstr);
ZEND_FUNCTION(mb_strrchr);
ZEND_FUNCTION(mb_stristr);
ZEND_FUNCTION(mb_strrichr);
ZEND_FUNCTION(mb_substr_count);
ZEND_FUNCTION(mb_substr);
ZEND_FUNCTION(mb_strcut);
ZEND_FUNCTION(mb_strwidth);
ZEND_FUNCTION(mb_strimwidth);
ZEND_FUNCTION(mb_convert_encoding);
ZEND_FUNCTION(mb_convert_case);
ZEND_FUNCTION(mb_strtoupper);
ZEND_FUNCTION(mb_strtolower);
ZEND_FUNCTION(mb_ucfirst);
ZEND_FUNCTION(mb_lcfirst);
ZEND_FUNCTION(mb_trim);
ZEND_FUNCTION(mb_ltrim);
ZEND_FUNCTION(mb_rtrim);
ZEND_FUNCTION(mb_detect_encoding);
ZEND_FUNCTION(mb_list_encodings);
ZEND_FUNCTION(mb_encoding_aliases);
ZEND_FUNCTION(mb_encode_mimeheader);
ZEND_FUNCTION(mb_decode_mimeheader);
ZEND_FUNCTION(mb_convert_kana);
ZEND_FUNCTION(mb_convert_variables);
ZEND_FUNCTION(mb_encode_numericentity);
ZEND_FUNCTION(mb_decode_numericentity);
ZEND_FUNCTION(mb_send_mail);
ZEND_FUNCTION(mb_get_info);
ZEND_FUNCTION(mb_check_encoding);
ZEND_FUNCTION(mb_scrub);
ZEND_FUNCTION(mb_ord);
ZEND_FUNCTION(mb_chr);
ZEND_FUNCTION(mb_str_pad);
#if defined(HAVE_MBREGEX)
ZEND_FUNCTION(mb_regex_encoding);
#endif
#if defined(HAVE_MBREGEX)
ZEND_FUNCTION(mb_ereg);
#endif
#if defined(HAVE_MBREGEX)
ZEND_FUNCTION(mb_eregi);
#endif
#if defined(HAVE_MBREGEX)
ZEND_FUNCTION(mb_ereg_replace);
#endif
#if defined(HAVE_MBREGEX)
ZEND_FUNCTION(mb_eregi_replace);
#endif
#if defined(HAVE_MBREGEX)
ZEND_FUNCTION(mb_ereg_replace_callback);
#endif
#if defined(HAVE_MBREGEX)
ZEND_FUNCTION(mb_split);
#endif
#if defined(HAVE_MBREGEX)
ZEND_FUNCTION(mb_ereg_match);
#endif
#if defined(HAVE_MBREGEX)
ZEND_FUNCTION(mb_ereg_search);
#endif
#if defined(HAVE_MBREGEX)
ZEND_FUNCTION(mb_ereg_search_pos);
#endif
#if defined(HAVE_MBREGEX)
ZEND_FUNCTION(mb_ereg_search_regs);
#endif
#if defined(HAVE_MBREGEX)
ZEND_FUNCTION(mb_ereg_search_init);
#endif
#if defined(HAVE_MBREGEX)
ZEND_FUNCTION(mb_ereg_search_getregs);
#endif
#if defined(HAVE_MBREGEX)
ZEND_FUNCTION(mb_ereg_search_getpos);
#endif
#if defined(HAVE_MBREGEX)
ZEND_FUNCTION(mb_ereg_search_setpos);
#endif
#if defined(HAVE_MBREGEX)
ZEND_FUNCTION(mb_regex_set_options);
#endif


static const zend_function_entry ext_functions[] = {
	ZEND_FE(mb_language, arginfo_mb_language)
	ZEND_FE(mb_internal_encoding, arginfo_mb_internal_encoding)
	ZEND_FE(mb_http_input, arginfo_mb_http_input)
	ZEND_FE(mb_http_output, arginfo_mb_http_output)
	ZEND_FE(mb_detect_order, arginfo_mb_detect_order)
	ZEND_FE(mb_substitute_character, arginfo_mb_substitute_character)
	ZEND_FE(mb_preferred_mime_name, arginfo_mb_preferred_mime_name)
	ZEND_FE(mb_parse_str, arginfo_mb_parse_str)
	ZEND_FE(mb_output_handler, arginfo_mb_output_handler)
	ZEND_FE(mb_str_split, arginfo_mb_str_split)
	ZEND_FE(mb_strlen, arginfo_mb_strlen)
	ZEND_FE(mb_strpos, arginfo_mb_strpos)
	ZEND_FE(mb_strrpos, arginfo_mb_strrpos)
	ZEND_FE(mb_stripos, arginfo_mb_stripos)
	ZEND_FE(mb_strripos, arginfo_mb_strripos)
	ZEND_FE(mb_strstr, arginfo_mb_strstr)
	ZEND_FE(mb_strrchr, arginfo_mb_strrchr)
	ZEND_FE(mb_stristr, arginfo_mb_stristr)
	ZEND_FE(mb_strrichr, arginfo_mb_strrichr)
	ZEND_FE(mb_substr_count, arginfo_mb_substr_count)
	ZEND_FE(mb_substr, arginfo_mb_substr)
	ZEND_FE(mb_strcut, arginfo_mb_strcut)
	ZEND_FE(mb_strwidth, arginfo_mb_strwidth)
	ZEND_FE(mb_strimwidth, arginfo_mb_strimwidth)
	ZEND_FE(mb_convert_encoding, arginfo_mb_convert_encoding)
	ZEND_FE(mb_convert_case, arginfo_mb_convert_case)
	ZEND_FE(mb_strtoupper, arginfo_mb_strtoupper)
	ZEND_FE(mb_strtolower, arginfo_mb_strtolower)
	ZEND_FE(mb_ucfirst, arginfo_mb_ucfirst)
	ZEND_FE(mb_lcfirst, arginfo_mb_lcfirst)
	ZEND_FE(mb_trim, arginfo_mb_trim)
	ZEND_FE(mb_ltrim, arginfo_mb_ltrim)
	ZEND_FE(mb_rtrim, arginfo_mb_rtrim)
	ZEND_FE(mb_detect_encoding, arginfo_mb_detect_encoding)
	ZEND_FE(mb_list_encodings, arginfo_mb_list_encodings)
	ZEND_FE(mb_encoding_aliases, arginfo_mb_encoding_aliases)
	ZEND_FE(mb_encode_mimeheader, arginfo_mb_encode_mimeheader)
	ZEND_FE(mb_decode_mimeheader, arginfo_mb_decode_mimeheader)
	ZEND_FE(mb_convert_kana, arginfo_mb_convert_kana)
	ZEND_FE(mb_convert_variables, arginfo_mb_convert_variables)
	ZEND_FE(mb_encode_numericentity, arginfo_mb_encode_numericentity)
	ZEND_FE(mb_decode_numericentity, arginfo_mb_decode_numericentity)
	ZEND_FE(mb_send_mail, arginfo_mb_send_mail)
	ZEND_FE(mb_get_info, arginfo_mb_get_info)
	ZEND_FE(mb_check_encoding, arginfo_mb_check_encoding)
	ZEND_FE(mb_scrub, arginfo_mb_scrub)
	ZEND_FE(mb_ord, arginfo_mb_ord)
	ZEND_FE(mb_chr, arginfo_mb_chr)
	ZEND_FE(mb_str_pad, arginfo_mb_str_pad)
#if defined(HAVE_MBREGEX)
	ZEND_FE(mb_regex_encoding, arginfo_mb_regex_encoding)
#endif
#if defined(HAVE_MBREGEX)
	ZEND_FE(mb_ereg, arginfo_mb_ereg)
#endif
#if defined(HAVE_MBREGEX)
	ZEND_FE(mb_eregi, arginfo_mb_eregi)
#endif
#if defined(HAVE_MBREGEX)
	ZEND_FE(mb_ereg_replace, arginfo_mb_ereg_replace)
#endif
#if defined(HAVE_MBREGEX)
	ZEND_FE(mb_eregi_replace, arginfo_mb_eregi_replace)
#endif
#if defined(HAVE_MBREGEX)
	ZEND_FE(mb_ereg_replace_callback, arginfo_mb_ereg_replace_callback)
#endif
#if defined(HAVE_MBREGEX)
	ZEND_FE(mb_split, arginfo_mb_split)
#endif
#if defined(HAVE_MBREGEX)
	ZEND_FE(mb_ereg_match, arginfo_mb_ereg_match)
#endif
#if defined(HAVE_MBREGEX)
	ZEND_FE(mb_ereg_search, arginfo_mb_ereg_search)
#endif
#if defined(HAVE_MBREGEX)
	ZEND_FE(mb_ereg_search_pos, arginfo_mb_ereg_search_pos)
#endif
#if defined(HAVE_MBREGEX)
	ZEND_FE(mb_ereg_search_regs, arginfo_mb_ereg_search_regs)
#endif
#if defined(HAVE_MBREGEX)
	ZEND_FE(mb_ereg_search_init, arginfo_mb_ereg_search_init)
#endif
#if defined(HAVE_MBREGEX)
	ZEND_FE(mb_ereg_search_getregs, arginfo_mb_ereg_search_getregs)
#endif
#if defined(HAVE_MBREGEX)
	ZEND_FE(mb_ereg_search_getpos, arginfo_mb_ereg_search_getpos)
#endif
#if defined(HAVE_MBREGEX)
	ZEND_FE(mb_ereg_search_setpos, arginfo_mb_ereg_search_setpos)
#endif
#if defined(HAVE_MBREGEX)
	ZEND_FE(mb_regex_set_options, arginfo_mb_regex_set_options)
#endif
	ZEND_FE_END
};

static void register_mbstring_symbols(int module_number)
{
#if defined(HAVE_MBREGEX)
	REGISTER_STRING_CONSTANT("MB_ONIGURUMA_VERSION", php_mb_oniguruma_version, CONST_PERSISTENT);
#endif
	REGISTER_LONG_CONSTANT("MB_CASE_UPPER", PHP_UNICODE_CASE_UPPER, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MB_CASE_LOWER", PHP_UNICODE_CASE_LOWER, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MB_CASE_TITLE", PHP_UNICODE_CASE_TITLE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MB_CASE_FOLD", PHP_UNICODE_CASE_FOLD, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MB_CASE_UPPER_SIMPLE", PHP_UNICODE_CASE_UPPER_SIMPLE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MB_CASE_LOWER_SIMPLE", PHP_UNICODE_CASE_LOWER_SIMPLE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MB_CASE_TITLE_SIMPLE", PHP_UNICODE_CASE_TITLE_SIMPLE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MB_CASE_FOLD_SIMPLE", PHP_UNICODE_CASE_FOLD_SIMPLE, CONST_PERSISTENT);
}
