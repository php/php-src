/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_set_time_limit, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, seconds, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_header_register_callback, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, callback)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ob_start, 0, 0, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, user_function)
	ZEND_ARG_TYPE_INFO(0, chunk_size, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ob_flush, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_ob_clean arginfo_ob_flush

#define arginfo_ob_end_flush arginfo_ob_flush

#define arginfo_ob_end_clean arginfo_ob_flush

ZEND_BEGIN_ARG_INFO_EX(arginfo_ob_get_flush, 0, 0, 0)
ZEND_END_ARG_INFO()

#define arginfo_ob_get_clean arginfo_ob_get_flush

#define arginfo_ob_get_contents arginfo_ob_get_flush

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ob_get_level, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_ob_get_length arginfo_ob_get_flush

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ob_list_handlers, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ob_get_status, 0, 0, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, full_status, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ob_implicit_flush, 0, 0, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, flag, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_output_reset_rewrite_vars arginfo_ob_flush

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_output_add_rewrite_var, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_stream_wrapper_register, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, protocol, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, classname, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_stream_wrapper_unregister, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, protocol, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_stream_wrapper_restore arginfo_stream_wrapper_unregister

ZEND_BEGIN_ARG_INFO_EX(arginfo_array_push, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(1, stack, IS_ARRAY, 0)
	ZEND_ARG_VARIADIC_INFO(0, args)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_krsort, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(1, arg, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, sort_flags, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_ksort arginfo_krsort

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_count, 0, 1, IS_LONG, 0)
	ZEND_ARG_INFO(0, var)
	ZEND_ARG_TYPE_INFO(0, mode, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_natsort, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(1, arg, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_natcasesort arginfo_natsort

#define arginfo_asort arginfo_krsort

#define arginfo_arsort arginfo_krsort

#define arginfo_sort arginfo_krsort

#define arginfo_rsort arginfo_krsort

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_usort, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(1, arg, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, cmp_function, IS_CALLABLE, 0)
ZEND_END_ARG_INFO()

#define arginfo_uasort arginfo_usort

#define arginfo_uksort arginfo_usort

ZEND_BEGIN_ARG_INFO_EX(arginfo_end, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(1, arg, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_prev, 0, 0, 1)
	ZEND_ARG_INFO(1, arg)
ZEND_END_ARG_INFO()

#define arginfo_next arginfo_prev

#define arginfo_reset arginfo_prev

ZEND_BEGIN_ARG_INFO_EX(arginfo_current, 0, 0, 1)
	ZEND_ARG_INFO(0, arg)
ZEND_END_ARG_INFO()

#define arginfo_key arginfo_current

ZEND_BEGIN_ARG_INFO_EX(arginfo_min, 0, 0, 1)
	ZEND_ARG_INFO(0, arg)
	ZEND_ARG_VARIADIC_INFO(0, args)
ZEND_END_ARG_INFO()

#define arginfo_max arginfo_min

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_array_walk, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(1, input)
	ZEND_ARG_TYPE_INFO(0, funcname, IS_CALLABLE, 0)
	ZEND_ARG_INFO(0, userdata)
ZEND_END_ARG_INFO()

#define arginfo_array_walk_recursive arginfo_array_walk

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_in_array, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, needle)
	ZEND_ARG_TYPE_INFO(0, haystack, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, strict, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_array_search, 0, 0, 2)
	ZEND_ARG_INFO(0, needle)
	ZEND_ARG_TYPE_INFO(0, haystack, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, strict, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_extract, 0, 1, IS_LONG, 1)
	ZEND_ARG_TYPE_INFO(ZEND_SEND_PREFER_REF, arg, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, extract_type, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, prefix, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_compact, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_INFO(0, var_name)
	ZEND_ARG_VARIADIC_INFO(0, var_names)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_array_fill, 0, 0, 3)
	ZEND_ARG_TYPE_INFO(0, start_key, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, num, IS_LONG, 0)
	ZEND_ARG_INFO(0, val)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_array_fill_keys, 0, 2, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, keys, IS_ARRAY, 0)
	ZEND_ARG_INFO(0, val)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_range, 0, 2, IS_ARRAY, 0)
	ZEND_ARG_INFO(0, low)
	ZEND_ARG_INFO(0, high)
	ZEND_ARG_INFO(0, step)
ZEND_END_ARG_INFO()

#define arginfo_shuffle arginfo_natsort

ZEND_BEGIN_ARG_INFO_EX(arginfo_array_pop, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(1, stack, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_array_shift arginfo_array_pop

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_array_unshift, 0, 1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(1, stack, IS_ARRAY, 0)
	ZEND_ARG_VARIADIC_INFO(0, vars)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_array_splice, 0, 2, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(1, arg, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 0)
	ZEND_ARG_INFO(0, replacement)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_array_slice, 0, 2, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, arg, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 1)
	ZEND_ARG_TYPE_INFO(0, preserve_keys, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_array_merge, 0, 0, IS_ARRAY, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, arrays, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_array_merge_recursive arginfo_array_merge

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_array_replace, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, arr1, IS_ARRAY, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, arrays, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_array_replace_recursive arginfo_array_replace

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_array_keys, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, arg, IS_ARRAY, 0)
	ZEND_ARG_INFO(0, search_value)
	ZEND_ARG_TYPE_INFO(0, strict, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_array_key_first, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, arg, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_array_key_last arginfo_array_key_first

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_array_values, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, arg, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_array_count_values arginfo_array_values

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_array_column, 0, 2, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, arg, IS_ARRAY, 0)
	ZEND_ARG_INFO(0, column_key)
	ZEND_ARG_INFO(0, index_key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_array_reverse, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, input, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, preserve_keys, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_array_pad, 0, 3, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, arg, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, pad_size, IS_LONG, 0)
	ZEND_ARG_INFO(0, pad_value)
ZEND_END_ARG_INFO()

#define arginfo_array_flip arginfo_array_values

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_array_change_key_case, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, input, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, case, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_array_unique, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, arg, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_array_intersect_key, 0, 2, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, arr1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, arr2, IS_ARRAY, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, arrays, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_array_intersect_ukey, 0, 2, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, arr1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, arr2, IS_ARRAY, 0)
	ZEND_ARG_VARIADIC_INFO(0, rest)
ZEND_END_ARG_INFO()

#define arginfo_array_intersect arginfo_array_intersect_key

#define arginfo_array_uintersect arginfo_array_intersect_ukey

#define arginfo_array_intersect_assoc arginfo_array_intersect_key

#define arginfo_array_uintersect_assoc arginfo_array_intersect_ukey

#define arginfo_array_intersect_uassoc arginfo_array_intersect_ukey

#define arginfo_array_uintersect_uassoc arginfo_array_intersect_ukey

#define arginfo_array_diff_key arginfo_array_intersect_key

#define arginfo_array_diff_ukey arginfo_array_intersect_ukey

#define arginfo_array_diff arginfo_array_intersect_key

#define arginfo_array_udiff arginfo_array_intersect_ukey

#define arginfo_array_diff_assoc arginfo_array_intersect_key

#define arginfo_array_diff_uassoc arginfo_array_intersect_ukey

#define arginfo_array_udiff_assoc arginfo_array_intersect_ukey

#define arginfo_array_udiff_uassoc arginfo_array_intersect_ukey

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_array_multisort, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(ZEND_SEND_PREFER_REF, arr1)
	ZEND_ARG_INFO(ZEND_SEND_PREFER_REF, sort_order)
	ZEND_ARG_INFO(ZEND_SEND_PREFER_REF, sort_flags)
	ZEND_ARG_VARIADIC_INFO(ZEND_SEND_PREFER_REF, arr2)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_array_rand, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, arg, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, num_req, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_array_sum arginfo_array_key_first

#define arginfo_array_product arginfo_array_key_first

ZEND_BEGIN_ARG_INFO_EX(arginfo_array_reduce, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, arg, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
	ZEND_ARG_INFO(0, initial)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_array_filter, 0, 2, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, arg, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
	ZEND_ARG_TYPE_INFO(0, use_keys, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_array_map, 0, 2, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 1)
	ZEND_ARG_TYPE_INFO(0, arr1, IS_ARRAY, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, arrays, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_array_key_exists, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, search)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_array_chunk, 0, 2, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, arg, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, size, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, preserve_keys, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_array_combine, 0, 2, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, keys, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, values, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_base64_encode, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_base64_decode, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, strict, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_crc32, 0, 1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_crypt, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, salt, IS_STRING, 0)
ZEND_END_ARG_INFO()

#if HAVE_FTOK
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ftok, 0, 2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, pathname, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, proj, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_hrtime, 0, 0, 0)
	ZEND_ARG_TYPE_INFO(0, get_as_number, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_lcg_value, 0, 0, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_md5, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, raw_output, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_md5_file, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, raw_output, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_getmyuid arginfo_ob_get_flush

#define arginfo_getmygid arginfo_ob_get_flush

#define arginfo_getmypid arginfo_ob_get_flush

#define arginfo_getmyinode arginfo_ob_get_flush

#define arginfo_getlastmod arginfo_ob_get_level

#define arginfo_sha1 arginfo_md5

#define arginfo_sha1_file arginfo_md5_file

#if defined(HAVE_SYSLOG_H)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_openlog, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, ident, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, option, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, facility, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_SYSLOG_H)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_closelog, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_SYSLOG_H)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_syslog, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, priority, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, message, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_INET_NTOP)
ZEND_BEGIN_ARG_INFO_EX(arginfo_inet_ntop, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, in_addr, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_INET_PTON)
ZEND_BEGIN_ARG_INFO_EX(arginfo_inet_pton, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, ip_address, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_metaphone, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, text, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, phones, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_header, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, replace, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, http_response_code, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_header_remove, 0, 0, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_setrawcookie, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
	ZEND_ARG_INFO(0, expires_or_options)
	ZEND_ARG_TYPE_INFO(0, path, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, domain, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, secure, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, httponly, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_setcookie arginfo_setrawcookie

ZEND_BEGIN_ARG_INFO_EX(arginfo_http_response_code, 0, 0, 0)
	ZEND_ARG_TYPE_INFO(0, response_code, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_headers_sent, 0, 0, _IS_BOOL, 0)
	ZEND_ARG_INFO(1, file)
	ZEND_ARG_INFO(1, line)
ZEND_END_ARG_INFO()

#define arginfo_headers_list arginfo_ob_list_handlers

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_htmlspecialchars, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, quote_style, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, encoding, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, double_encode, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_htmlspecialchars_decode, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, quote_style, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_html_entity_decode, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, quote_style, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, encoding, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_htmlentities arginfo_htmlspecialchars

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_get_html_translation_table, 0, 0, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, table, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, quote_style, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, encoding, IS_STRING, 0)
ZEND_END_ARG_INFO()
