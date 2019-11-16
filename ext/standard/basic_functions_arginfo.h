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

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_ob_get_flush, 0, 0, MAY_BE_STRING|MAY_BE_FALSE)
ZEND_END_ARG_INFO()

#define arginfo_ob_get_clean arginfo_ob_get_flush

#define arginfo_ob_get_contents arginfo_ob_get_flush

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ob_get_level, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_ob_get_length, 0, 0, MAY_BE_LONG|MAY_BE_FALSE)
ZEND_END_ARG_INFO()

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

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_array_push, 0, 1, IS_LONG, 0)
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
	ZEND_ARG_TYPE_MASK(1, arg, MAY_BE_ARRAY|MAY_BE_OBJECT)
ZEND_END_ARG_INFO()

#define arginfo_prev arginfo_end

#define arginfo_next arginfo_end

#define arginfo_reset arginfo_end

ZEND_BEGIN_ARG_INFO_EX(arginfo_current, 0, 0, 1)
	ZEND_ARG_TYPE_MASK(0, arg, MAY_BE_ARRAY|MAY_BE_OBJECT)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_key, 0, 1, MAY_BE_LONG|MAY_BE_STRING|MAY_BE_NULL)
	ZEND_ARG_TYPE_MASK(0, arg, MAY_BE_ARRAY|MAY_BE_OBJECT)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_min, 0, 0, 1)
	ZEND_ARG_INFO(0, arg)
	ZEND_ARG_VARIADIC_INFO(0, args)
ZEND_END_ARG_INFO()

#define arginfo_max arginfo_min

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_array_walk, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_MASK(1, input, MAY_BE_ARRAY|MAY_BE_OBJECT)
	ZEND_ARG_TYPE_INFO(0, funcname, IS_CALLABLE, 0)
	ZEND_ARG_INFO(0, userdata)
ZEND_END_ARG_INFO()

#define arginfo_array_walk_recursive arginfo_array_walk

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_in_array, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, needle)
	ZEND_ARG_TYPE_INFO(0, haystack, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, strict, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_array_search, 0, 2, MAY_BE_LONG|MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, needle)
	ZEND_ARG_TYPE_INFO(0, haystack, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, strict, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_extract, 0, 1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(ZEND_SEND_PREFER_REF, arg, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, extract_type, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, prefix, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_compact, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_INFO(0, var_name)
	ZEND_ARG_VARIADIC_INFO(0, var_names)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_array_fill, 0, 3, IS_ARRAY, 0)
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
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 1)
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

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_array_key_first, 0, 1, MAY_BE_LONG|MAY_BE_STRING|MAY_BE_NULL)
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

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_array_rand, 0, 1, MAY_BE_LONG|MAY_BE_STRING|MAY_BE_ARRAY)
	ZEND_ARG_TYPE_INFO(0, arg, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, num_req, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_array_sum, 0, 1, MAY_BE_LONG|MAY_BE_DOUBLE)
	ZEND_ARG_TYPE_INFO(0, arg, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_array_product arginfo_array_sum

ZEND_BEGIN_ARG_INFO_EX(arginfo_array_reduce, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, arg, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
	ZEND_ARG_INFO(0, initial)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_array_filter, 0, 1, IS_ARRAY, 0)
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
	ZEND_ARG_TYPE_INFO(0, search, IS_ARRAY, 0)
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

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_base64_decode, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, strict, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_constant, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_ip2long, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, ip_address, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_long2ip, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, proper_address, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_getenv, 0, 0, MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, variable, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, local_only, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#if defined(HAVE_PUTENV)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_putenv, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, setting, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_getopt, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, options, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, longopts, IS_ARRAY, 0)
	ZEND_ARG_INFO(1, optind)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_flush, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sleep, 0, 1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, seconds, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_usleep, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, microseconds, IS_LONG, 0)
ZEND_END_ARG_INFO()

#if HAVE_NANOSLEEP
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_nanosleep, 0, 2, MAY_BE_ARRAY|MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, seconds, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, nanoseconds, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

#if HAVE_NANOSLEEP
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_time_sleep_until, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, timestamp, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_get_current_user, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_get_cfg_var, 0, 1, MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, option_name, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_get_magic_quotes_runtime arginfo_ob_flush

#define arginfo_get_magic_quotes_gpc arginfo_ob_flush

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_error_log, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, message, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, message_type, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, destination, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, extra_headers, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_error_get_last, 0, 0, IS_ARRAY, 1)
ZEND_END_ARG_INFO()

#define arginfo_error_clear_last arginfo_flush

ZEND_BEGIN_ARG_INFO_EX(arginfo_call_user_func, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, function, IS_CALLABLE, 0)
	ZEND_ARG_VARIADIC_INFO(0, args)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_call_user_func_array, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, function, IS_CALLABLE, 0)
	ZEND_ARG_TYPE_INFO(0, args, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_forward_static_call arginfo_call_user_func

#define arginfo_forward_static_call_array arginfo_call_user_func_array

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_register_shutdown_function, 0, 1, _IS_BOOL, 1)
	ZEND_ARG_INFO(0, function)
	ZEND_ARG_VARIADIC_INFO(0, args)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_highlight_file, 0, 1, MAY_BE_STRING|MAY_BE_BOOL|MAY_BE_NULL)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, return, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_php_strip_whitespace, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_highlight_string, 0, 1, MAY_BE_STRING|MAY_BE_BOOL|MAY_BE_NULL)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, return, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_ini_get, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, varname, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_ini_get_all, 0, 0, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, extension, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, details, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_ini_set, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, varname, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ini_restore, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, varname, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_set_include_path, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, include_path, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_get_include_path arginfo_ob_get_flush

#define arginfo_restore_include_path arginfo_flush

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_print_r, 0, 1, MAY_BE_STRING|MAY_BE_BOOL)
	ZEND_ARG_INFO(0, var)
	ZEND_ARG_TYPE_INFO(0, return, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_connection_aborted arginfo_ob_get_level

#define arginfo_connection_status arginfo_ob_get_level

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ignore_user_abort, 0, 0, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#if HAVE_GETSERVBYNAME
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_getservbyname, 0, 2, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, service, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, protocol, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if HAVE_GETSERVBYPORT
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_getservbyport, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, port, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, protocol, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if HAVE_GETPROTOBYNAME
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_getprotobyname, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if HAVE_GETPROTOBYNUMBER
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_getprotobynumber, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, protocol, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_register_tick_function, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, function, IS_CALLABLE, 0)
	ZEND_ARG_VARIADIC_INFO(0, args)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_unregister_tick_function, 0, 1, IS_VOID, 0)
	ZEND_ARG_INFO(0, function)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_is_uploaded_file, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, path, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_move_uploaded_file, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, path, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, new_path, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_parse_ini_file, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, process_sections, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, scanner_mode, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_parse_ini_string, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, ini_string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, process_sections, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, scanner_mode, IS_LONG, 0)
ZEND_END_ARG_INFO()

#if ZEND_DEBUG
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_config_get_hash, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, ini_string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, process_sections, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, scanner_mode, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_GETLOADAVG)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_sys_getloadavg, 0, 0, MAY_BE_ARRAY|MAY_BE_FALSE)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_get_browser, 0, 0, MAY_BE_OBJECT|MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, browser_name, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, return_array, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_crc32, 0, 1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_crypt, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, salt, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_convert_cyr_string, 0, 3, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, from, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, to, IS_STRING, 0)
ZEND_END_ARG_INFO()

#if HAVE_STRPTIME
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_strptime, 0, 2, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, timestamp, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, format, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_GETHOSTNAME)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_gethostname, 0, 0, MAY_BE_STRING|MAY_BE_FALSE)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_gethostbyaddr, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, ip_address, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_gethostbyname, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, hostname, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_gethostbynamel, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, hostname, IS_STRING, 0)
ZEND_END_ARG_INFO()

#if defined(PHP_WIN32) || HAVE_DNS_SEARCH_FUNC
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_dns_check_record, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, hostname, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, type, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(PHP_WIN32) || HAVE_DNS_SEARCH_FUNC
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_dns_get_record, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, hostname, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, type, IS_LONG, 0)
	ZEND_ARG_INFO(1, authns)
	ZEND_ARG_INFO(1, addtl)
	ZEND_ARG_TYPE_INFO(0, raw, _IS_BOOL, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(PHP_WIN32) || HAVE_DNS_SEARCH_FUNC
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_dns_get_mx, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, hostname, IS_STRING, 0)
	ZEND_ARG_INFO(1, mxhosts)
	ZEND_ARG_INFO(1, weight)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_net_get_interfaces, 0, 0, MAY_BE_ARRAY|MAY_BE_FALSE)
ZEND_END_ARG_INFO()

#if HAVE_FTOK
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ftok, 0, 2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, pathname, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, proj, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_hrtime, 0, 0, MAY_BE_ARRAY|MAY_BE_LONG|MAY_BE_DOUBLE|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, get_as_number, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_lcg_value, 0, 0, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_md5, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, raw_output, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_md5_file, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, raw_output, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_getmyuid arginfo_ob_get_length

#define arginfo_getmygid arginfo_ob_get_length

#define arginfo_getmypid arginfo_ob_get_length

#define arginfo_getmyinode arginfo_ob_get_length

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
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_inet_ntop, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, in_addr, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_INET_PTON)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_inet_pton, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, ip_address, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_metaphone, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
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

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_http_response_code, 0, 0, MAY_BE_LONG|MAY_BE_BOOL)
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

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_htmlspecialchars_decode, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, quote_style, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_html_entity_decode, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
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

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_assert, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, assertion)
	ZEND_ARG_INFO(0, description)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_assert_options, 0, 1, MAY_BE_ARRAY|MAY_BE_LONG|MAY_BE_STRING|MAY_BE_BOOL|MAY_BE_NULL)
	ZEND_ARG_TYPE_INFO(0, what, IS_LONG, 0)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_bin2hex, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_hex2bin, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_strspn, 0, 2, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, mask, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, start, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, len, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_strcspn arginfo_strspn

#if HAVE_NL_LANGINFO
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_nl_langinfo, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, item, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_strcoll, 0, 2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, str1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, str2, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_trim, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, character_mask, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_rtrim arginfo_trim

#define arginfo_ltrim arginfo_trim

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wordwrap, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, width, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, break, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, cut, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_explode, 0, 2, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, separator, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, limit, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_implode, 0, 1, IS_STRING, 0)
	ZEND_ARG_INFO(0, glue)
	ZEND_ARG_INFO(0, pieces)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_strtok, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, token, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_strtoupper arginfo_base64_encode

#define arginfo_strtolower arginfo_base64_encode

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_basename, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, path, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, suffix, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_dirname, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, path, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, levels, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pathinfo, 0, 1, MAY_BE_ARRAY|MAY_BE_STRING)
	ZEND_ARG_TYPE_INFO(0, path, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, options, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_stristr, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, haystack, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, needle, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, before_needle, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_strstr arginfo_stristr

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_strpos, 0, 2, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, haystack, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, needle, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_stripos arginfo_strpos

#define arginfo_strrpos arginfo_strpos

#define arginfo_strripos arginfo_strpos

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_strrchr, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, haystack, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, needle, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_chunk_split, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, chunklen, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, ending, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_substr, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, start, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_substr_replace, 0, 3, MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, replace)
	ZEND_ARG_INFO(0, start)
	ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO()

#define arginfo_quotemeta arginfo_base64_encode

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ord, 0, 1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, character, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_chr, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, codepoint, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_ucfirst arginfo_base64_encode

#define arginfo_lcfirst arginfo_base64_encode

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ucwords, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, delimiters, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_strtr, 0, 2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
	ZEND_ARG_INFO(0, from)
	ZEND_ARG_TYPE_INFO(0, to, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_strrev arginfo_base64_encode

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_similar_text, 0, 2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, str1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, str2, IS_STRING, 0)
	ZEND_ARG_INFO(1, percent)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_addcslashes, 0, 2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, charlist, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_addslashes arginfo_base64_encode

#define arginfo_stripcslashes arginfo_base64_encode

#define arginfo_stripslashes arginfo_base64_encode

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_str_replace, 0, 3, MAY_BE_STRING|MAY_BE_ARRAY)
	ZEND_ARG_INFO(0, search)
	ZEND_ARG_INFO(0, replace)
	ZEND_ARG_INFO(0, subject)
	ZEND_ARG_INFO(1, replace_count)
ZEND_END_ARG_INFO()

#define arginfo_str_ireplace arginfo_str_replace

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_hebrev, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, max_chars_per_line, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_hebrevc arginfo_hebrev

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_nl2br, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, is_xhtml, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_strip_tags, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
	ZEND_ARG_INFO(0, allowable_tags)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_setlocale, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, category, IS_LONG, 0)
	ZEND_ARG_INFO(0, locales)
	ZEND_ARG_VARIADIC_INFO(0, rest)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_parse_str, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, encoded_string, IS_STRING, 0)
	ZEND_ARG_INFO(1, result)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_str_getcsv, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, delimiter, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, enclosure, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, escape, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_str_repeat, 0, 2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, input, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, mult, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_count_chars, 0, 1, MAY_BE_ARRAY|MAY_BE_STRING)
	ZEND_ARG_TYPE_INFO(0, input, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, mode, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_strnatcmp, 0, 2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, s1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, s2, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_localeconv arginfo_ob_list_handlers

#define arginfo_strnatcasecmp arginfo_strnatcmp

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_substr_count, 0, 2, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, haystack, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, needle, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_str_pad, 0, 2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, input, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, pad_length, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, pad_string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, pad_type, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_sscanf, 0, 2, MAY_BE_ARRAY|MAY_BE_LONG|MAY_BE_NULL)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, format, IS_STRING, 0)
	ZEND_ARG_VARIADIC_INFO(1, vars)
ZEND_END_ARG_INFO()

#define arginfo_str_rot13 arginfo_base64_encode

#define arginfo_str_shuffle arginfo_base64_encode

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_str_word_count, 0, 1, MAY_BE_ARRAY|MAY_BE_LONG)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, format, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, charlist, IS_STRING, 0)
ZEND_END_ARG_INFO()

#if defined(HAVE_STRFMON)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_money_format, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, format, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_str_split, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, split_length, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_strpbrk, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, haystack, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, char_list, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_substr_compare, 0, 3, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, main_str, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 1)
	ZEND_ARG_TYPE_INFO(0, case_insensitivity, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_utf8_encode arginfo_bin2hex

#define arginfo_utf8_decode arginfo_bin2hex

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Directory_close, 0, 0, 0)
	ZEND_ARG_INFO(0, dir_handle)
ZEND_END_ARG_INFO()

#define arginfo_class_Directory_rewind arginfo_class_Directory_close

#define arginfo_class_Directory_read arginfo_class_Directory_close

ZEND_BEGIN_ARG_INFO_EX(arginfo_opendir, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, path, IS_STRING, 0)
	ZEND_ARG_INFO(0, context)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_dir, 0, 1, Directory, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, path, IS_STRING, 0)
	ZEND_ARG_INFO(0, context)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_closedir, 0, 0, IS_VOID, 0)
	ZEND_ARG_INFO(0, dir_handle)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_chdir, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, directory, IS_STRING, 0)
ZEND_END_ARG_INFO()

#if defined(HAVE_CHROOT) && !defined(ZTS) && ENABLE_CHROOT_FUNC
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_chroot, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, directory, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#define arginfo_getcwd arginfo_ob_get_flush

#define arginfo_rewinddir arginfo_closedir

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_readdir, 0, 0, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, dir_handle)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_scandir, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, directory, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, sorting_order, IS_LONG, 0)
	ZEND_ARG_INFO(0, context)
ZEND_END_ARG_INFO()

#if defined(HAVE_GLOB)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_glob, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_fileatime, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_filectime arginfo_fileatime

#define arginfo_filegroup arginfo_fileatime

#define arginfo_fileinode arginfo_fileatime

#define arginfo_filemtime arginfo_fileatime

#define arginfo_fileowner arginfo_fileatime

#define arginfo_fileperms arginfo_fileatime

#define arginfo_filesize arginfo_fileatime

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_filetype, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_file_exists, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_is_writable arginfo_file_exists

#define arginfo_is_writeable arginfo_file_exists

#define arginfo_is_readable arginfo_file_exists

#define arginfo_is_executable arginfo_file_exists

#define arginfo_is_file arginfo_file_exists

#define arginfo_is_dir arginfo_file_exists

#define arginfo_is_link arginfo_file_exists

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_stat, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_lstat arginfo_stat

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_chown, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_INFO(0, user)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_chgrp, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_INFO(0, group)
ZEND_END_ARG_INFO()

#if HAVE_LCHOWN
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_lchown, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_INFO(0, user)
ZEND_END_ARG_INFO()
#endif

#if HAVE_LCHOWN
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_lchgrp, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_INFO(0, group)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_chmod, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, mode, IS_LONG, 0)
ZEND_END_ARG_INFO()

#if HAVE_UTIME
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_touch, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, time, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, atime, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_clearstatcache, 0, 0, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, clear_realpath_cache, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_disk_total_space, 0, 1, MAY_BE_DOUBLE|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, directory, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_disk_free_space arginfo_disk_total_space

#define arginfo_diskfreespace arginfo_disk_total_space

#define arginfo_realpath_cache_get arginfo_ob_list_handlers

#define arginfo_realpath_cache_size arginfo_ob_get_level

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_phpinfo, 0, 0, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, what, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_phpversion, 0, 0, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, extension, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_phpcredits, 0, 0, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, flag, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_php_sapi_name arginfo_ob_get_flush

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_php_uname, 0, 0, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, mode, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_php_ini_scanned_files arginfo_ob_get_flush

#define arginfo_php_ini_loaded_file arginfo_ob_get_flush

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_iptcembed, 0, 2, MAY_BE_STRING|MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, iptcdata, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, jpeg_file_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, spool, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_iptcparse, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, iptcblock, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_levenshtein, 0, 2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, str1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, str2, IS_STRING, 0)
	ZEND_ARG_INFO(0, cost_ins)
	ZEND_ARG_TYPE_INFO(0, cost_rep, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, cost_del, IS_LONG, 0)
ZEND_END_ARG_INFO()

#if defined(HAVE_SYMLINK) || defined(PHP_WIN32)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_readlink, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, path, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_SYMLINK) || defined(PHP_WIN32)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_linkinfo, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, path, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_SYMLINK) || defined(PHP_WIN32)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_symlink, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, target, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, link, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_SYMLINK) || defined(PHP_WIN32)
#define arginfo_link arginfo_symlink
#endif

#define arginfo_ezmlm_hash arginfo_crc32

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mail, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, to, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, subject, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, message, IS_STRING, 0)
	ZEND_ARG_INFO(0, additional_headers)
	ZEND_ARG_TYPE_INFO(0, additional_parameters, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_abs, 0, 1, MAY_BE_LONG|MAY_BE_DOUBLE)
	ZEND_ARG_TYPE_MASK(0, number, MAY_BE_LONG|MAY_BE_DOUBLE)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ceil, 0, 1, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, number, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

#define arginfo_floor arginfo_ceil

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_round, 0, 1, IS_DOUBLE, 0)
	ZEND_ARG_INFO(0, number)
	ZEND_ARG_TYPE_INFO(0, precision, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, mode, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_sin arginfo_ceil

#define arginfo_cos arginfo_ceil

#define arginfo_tan arginfo_ceil

#define arginfo_asin arginfo_ceil

#define arginfo_acos arginfo_ceil

#define arginfo_atan arginfo_ceil

#define arginfo_atanh arginfo_ceil

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_atan2, 0, 2, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, y, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, x, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

#define arginfo_sinh arginfo_ceil

#define arginfo_cosh arginfo_ceil

#define arginfo_tanh arginfo_ceil

#define arginfo_asinh arginfo_ceil

#define arginfo_acosh arginfo_ceil

#define arginfo_expm1 arginfo_ceil

#define arginfo_log1p arginfo_ceil

#define arginfo_pi arginfo_lcg_value

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_is_finite, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, number, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

#define arginfo_is_nan arginfo_is_finite

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_intdiv, 0, 2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, dividend, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, divisor, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_is_infinite arginfo_is_finite

ZEND_BEGIN_ARG_INFO_EX(arginfo_pow, 0, 0, 2)
	ZEND_ARG_INFO(0, base)
	ZEND_ARG_INFO(0, exp)
ZEND_END_ARG_INFO()

#define arginfo_exp arginfo_ceil

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_log, 0, 1, MAY_BE_DOUBLE|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, number, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, base, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

#define arginfo_log10 arginfo_ceil

#define arginfo_sqrt arginfo_ceil

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_hypot, 0, 2, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, x, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, y, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

#define arginfo_deg2rad arginfo_ceil

#define arginfo_rad2deg arginfo_ceil

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_bindec, 0, 1, MAY_BE_LONG|MAY_BE_DOUBLE)
	ZEND_ARG_TYPE_INFO(0, binary_string, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_hexdec, 0, 1, MAY_BE_LONG|MAY_BE_DOUBLE)
	ZEND_ARG_TYPE_INFO(0, hex_string, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_octdec, 0, 1, MAY_BE_LONG|MAY_BE_DOUBLE)
	ZEND_ARG_TYPE_INFO(0, octal_string, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_decbin, 0, 1, IS_STRING, 0)
	ZEND_ARG_INFO(0, number)
ZEND_END_ARG_INFO()

#define arginfo_decoct arginfo_decbin

#define arginfo_dechex arginfo_decbin

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_base_convert, 0, 3, IS_STRING, 0)
	ZEND_ARG_INFO(0, number)
	ZEND_ARG_TYPE_INFO(0, frombase, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, tobase, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_number_format, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, number, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, decimals, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, decimal_point, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, thousands_separator, IS_STRING, 1)
ZEND_END_ARG_INFO()

#define arginfo_fmod arginfo_hypot

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_fdiv, 0, 2, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, dividend, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, divisor, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

#if defined(HAVE_GETTIMEOFDAY)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_microtime, 0, 0, MAY_BE_STRING|MAY_BE_DOUBLE)
	ZEND_ARG_TYPE_INFO(0, get_as_float, _IS_BOOL, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_GETTIMEOFDAY)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_gettimeofday, 0, 0, MAY_BE_ARRAY|MAY_BE_DOUBLE)
	ZEND_ARG_TYPE_INFO(0, return_float, _IS_BOOL, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_GETRUSAGE)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_getrusage, 0, 0, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, who, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pack, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, format, IS_STRING, 0)
	ZEND_ARG_VARIADIC_INFO(0, args)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_unpack, 0, 2, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, format, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_password_get_info, 0, 1, IS_ARRAY, 1)
	ZEND_ARG_TYPE_INFO(0, hash, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_password_hash, 0, 2, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, password, IS_STRING, 0)
	ZEND_ARG_INFO(0, algo)
	ZEND_ARG_TYPE_INFO(0, options, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_password_needs_rehash, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, hash, IS_STRING, 0)
	ZEND_ARG_INFO(0, algo)
	ZEND_ARG_TYPE_INFO(0, options, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_password_verify, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, password, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, hash, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_password_algos arginfo_ob_list_handlers

#if defined(PHP_CAN_SUPPORT_PROC_OPEN)
ZEND_BEGIN_ARG_INFO_EX(arginfo_proc_open, 0, 0, 3)
	ZEND_ARG_INFO(0, cmd)
	ZEND_ARG_TYPE_INFO(0, descriptorspec, IS_ARRAY, 0)
	ZEND_ARG_INFO(1, pipes)
	ZEND_ARG_TYPE_INFO(0, cwd, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, env, IS_ARRAY, 1)
	ZEND_ARG_TYPE_INFO(0, other_options, IS_ARRAY, 1)
ZEND_END_ARG_INFO()
#endif

#if defined(PHP_CAN_SUPPORT_PROC_OPEN)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_proc_close, 0, 1, IS_LONG, 0)
	ZEND_ARG_INFO(0, process)
ZEND_END_ARG_INFO()
#endif

#if defined(PHP_CAN_SUPPORT_PROC_OPEN)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_proc_terminate, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, process)
	ZEND_ARG_TYPE_INFO(0, signal, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(PHP_CAN_SUPPORT_PROC_OPEN)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_proc_get_status, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_INFO(0, process)
ZEND_END_ARG_INFO()
#endif

#define arginfo_quoted_printable_decode arginfo_base64_encode

#define arginfo_quoted_printable_encode arginfo_base64_encode

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mt_srand, 0, 0, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, seed, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, mode, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mt_rand, 0, 0, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, min, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, max, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_mt_getrandmax arginfo_ob_get_level

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_random_bytes, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_random_int, 0, 2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, min, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, max, IS_LONG, 0)
ZEND_END_ARG_INFO()
