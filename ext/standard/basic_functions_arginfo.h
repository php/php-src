/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: b8ea4527467c70a6f665129cd5d5f34ea2386a70 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_set_time_limit, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, seconds, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_header_register_callback, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ob_start, 0, 0, _IS_BOOL, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, callback, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, chunk_size, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "PHP_OUTPUT_HANDLER_STDFLAGS")
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
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, full_status, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ob_implicit_flush, 0, 0, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, enable, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

#define arginfo_output_reset_rewrite_vars arginfo_ob_flush

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_output_add_rewrite_var, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_stream_wrapper_register, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, protocol, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, class, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_stream_register_wrapper arginfo_stream_wrapper_register

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_stream_wrapper_unregister, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, protocol, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_stream_wrapper_restore arginfo_stream_wrapper_unregister

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_array_push, 0, 1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(1, array, IS_ARRAY, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, values, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_krsort, 0, 1, IS_TRUE, 0)
	ZEND_ARG_TYPE_INFO(1, array, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "SORT_REGULAR")
ZEND_END_ARG_INFO()

#define arginfo_ksort arginfo_krsort

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_count, 0, 1, IS_LONG, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, value, Countable, MAY_BE_ARRAY, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "COUNT_NORMAL")
ZEND_END_ARG_INFO()

#define arginfo_sizeof arginfo_count

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_natsort, 0, 1, IS_TRUE, 0)
	ZEND_ARG_TYPE_INFO(1, array, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_natcasesort arginfo_natsort

#define arginfo_asort arginfo_krsort

#define arginfo_arsort arginfo_krsort

#define arginfo_sort arginfo_krsort

#define arginfo_rsort arginfo_krsort

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_usort, 0, 2, IS_TRUE, 0)
	ZEND_ARG_TYPE_INFO(1, array, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
ZEND_END_ARG_INFO()

#define arginfo_uasort arginfo_usort

#define arginfo_uksort arginfo_usort

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_end, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_MASK(1, array, MAY_BE_ARRAY|MAY_BE_OBJECT, NULL)
ZEND_END_ARG_INFO()

#define arginfo_prev arginfo_end

#define arginfo_next arginfo_end

#define arginfo_reset arginfo_end

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_current, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_MASK(0, array, MAY_BE_ARRAY|MAY_BE_OBJECT, NULL)
ZEND_END_ARG_INFO()

#define arginfo_pos arginfo_current

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_key, 0, 1, MAY_BE_LONG|MAY_BE_STRING|MAY_BE_NULL)
	ZEND_ARG_TYPE_MASK(0, array, MAY_BE_ARRAY|MAY_BE_OBJECT, NULL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_min, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, values, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_max arginfo_min

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_array_walk, 0, 2, IS_TRUE, 0)
	ZEND_ARG_TYPE_MASK(1, array, MAY_BE_ARRAY|MAY_BE_OBJECT, NULL)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
	ZEND_ARG_TYPE_INFO(0, arg, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_array_walk_recursive arginfo_array_walk

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_in_array, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, needle, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, haystack, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, strict, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_array_search, 0, 2, MAY_BE_LONG|MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, needle, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, haystack, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, strict, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_extract, 0, 1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(ZEND_SEND_PREFER_REF, array, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "EXTR_OVERWRITE")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, prefix, IS_STRING, 0, "\"\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_compact, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_INFO(0, var_name)
	ZEND_ARG_VARIADIC_INFO(0, var_names)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_array_fill, 0, 3, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, start_index, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, count, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_array_fill_keys, 0, 2, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, keys, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_range, 0, 2, IS_ARRAY, 0)
	ZEND_ARG_TYPE_MASK(0, start, MAY_BE_STRING|MAY_BE_LONG|MAY_BE_DOUBLE, NULL)
	ZEND_ARG_TYPE_MASK(0, end, MAY_BE_STRING|MAY_BE_LONG|MAY_BE_DOUBLE, NULL)
	ZEND_ARG_TYPE_MASK(0, step, MAY_BE_LONG|MAY_BE_DOUBLE, "1")
ZEND_END_ARG_INFO()

#define arginfo_shuffle arginfo_natsort

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_array_pop, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(1, array, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_array_shift arginfo_array_pop

#define arginfo_array_unshift arginfo_array_push

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_array_splice, 0, 2, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(1, array, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, replacement, IS_MIXED, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_array_slice, 0, 2, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, preserve_keys, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_array_merge, 0, 0, IS_ARRAY, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, arrays, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_array_merge_recursive arginfo_array_merge

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_array_replace, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, replacements, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_array_replace_recursive arginfo_array_replace

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_array_keys, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, filter_value, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, strict, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_array_key_first, 0, 1, MAY_BE_LONG|MAY_BE_STRING|MAY_BE_NULL)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_array_key_last arginfo_array_key_first

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_array_values, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_array_count_values arginfo_array_values

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_array_column, 0, 2, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
	ZEND_ARG_TYPE_MASK(0, column_key, MAY_BE_LONG|MAY_BE_STRING|MAY_BE_NULL, NULL)
	ZEND_ARG_TYPE_MASK(0, index_key, MAY_BE_LONG|MAY_BE_STRING|MAY_BE_NULL, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_array_reverse, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, preserve_keys, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_array_pad, 0, 3, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_array_flip arginfo_array_values

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_array_change_key_case, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, case, IS_LONG, 0, "CASE_LOWER")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_array_unique, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "SORT_STRING")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_array_intersect_key, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, arrays, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_array_intersect_ukey, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
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
	ZEND_ARG_INFO(ZEND_SEND_PREFER_REF, array)
	ZEND_ARG_VARIADIC_INFO(ZEND_SEND_PREFER_REF, rest)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_array_rand, 0, 1, MAY_BE_LONG|MAY_BE_STRING|MAY_BE_ARRAY)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, num, IS_LONG, 0, "1")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_array_sum, 0, 1, MAY_BE_LONG|MAY_BE_DOUBLE)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_array_product arginfo_array_sum

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_array_reduce, 0, 2, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, initial, IS_MIXED, 0, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_array_filter, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, callback, IS_CALLABLE, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_array_map, 0, 2, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 1)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, arrays, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_array_key_exists, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_key_exists arginfo_array_key_exists

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_array_chunk, 0, 2, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, preserve_keys, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_array_combine, 0, 2, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, keys, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, values, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_array_is_list, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_base64_encode, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_base64_decode, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, strict, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_constant, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_ip2long, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, ip, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_long2ip, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, ip, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_getenv, 0, 0, MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, name, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, local_only, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

#if defined(HAVE_PUTENV)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_putenv, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, assignment, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_getopt, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, short_options, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, long_options, IS_ARRAY, 0, "[]")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, rest_index, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_flush, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sleep, 0, 1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, seconds, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_usleep, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, microseconds, IS_LONG, 0)
ZEND_END_ARG_INFO()

#if defined(HAVE_NANOSLEEP)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_time_nanosleep, 0, 2, MAY_BE_ARRAY|MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, seconds, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, nanoseconds, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_NANOSLEEP)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_time_sleep_until, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, timestamp, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_get_current_user, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_get_cfg_var, 0, 1, MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, option, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_error_log, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, message, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, message_type, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, destination, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, additional_headers, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_error_get_last, 0, 0, IS_ARRAY, 1)
ZEND_END_ARG_INFO()

#define arginfo_error_clear_last arginfo_flush

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_call_user_func, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, args, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_call_user_func_array, 0, 2, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
	ZEND_ARG_TYPE_INFO(0, args, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_forward_static_call arginfo_call_user_func

#define arginfo_forward_static_call_array arginfo_call_user_func_array

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_register_shutdown_function, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, args, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_highlight_file, 0, 1, MAY_BE_STRING|MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, return, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

#define arginfo_show_source arginfo_highlight_file

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_php_strip_whitespace, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_highlight_string, 0, 1, MAY_BE_STRING|MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, return, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_ini_get, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, option, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_ini_get_all, 0, 0, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, extension, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, details, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_ini_set, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, option, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, value, MAY_BE_STRING|MAY_BE_LONG|MAY_BE_DOUBLE|MAY_BE_BOOL|MAY_BE_NULL, NULL)
ZEND_END_ARG_INFO()

#define arginfo_ini_alter arginfo_ini_set

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ini_restore, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, option, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ini_parse_quantity, 0, 1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, shorthand, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_set_include_path, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, include_path, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_get_include_path arginfo_ob_get_flush

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_print_r, 0, 1, MAY_BE_STRING|MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, return, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

#define arginfo_connection_aborted arginfo_ob_get_level

#define arginfo_connection_status arginfo_ob_get_level

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ignore_user_abort, 0, 0, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, enable, _IS_BOOL, 1, "null")
ZEND_END_ARG_INFO()

#if defined(HAVE_GETSERVBYNAME)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_getservbyname, 0, 2, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, service, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, protocol, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_GETSERVBYPORT)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_getservbyport, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, port, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, protocol, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_GETPROTOBYNAME)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_getprotobyname, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, protocol, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_GETPROTOBYNUMBER)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_getprotobynumber, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, protocol, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_register_tick_function, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, args, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_unregister_tick_function, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_is_uploaded_file, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_move_uploaded_file, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, from, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, to, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_parse_ini_file, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, process_sections, _IS_BOOL, 0, "false")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, scanner_mode, IS_LONG, 0, "INI_SCANNER_NORMAL")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_parse_ini_string, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, ini_string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, process_sections, _IS_BOOL, 0, "false")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, scanner_mode, IS_LONG, 0, "INI_SCANNER_NORMAL")
ZEND_END_ARG_INFO()

#if ZEND_DEBUG
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_config_get_hash, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_GETLOADAVG)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_sys_getloadavg, 0, 0, MAY_BE_ARRAY|MAY_BE_FALSE)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_get_browser, 0, 0, MAY_BE_OBJECT|MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, user_agent, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, return_array, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_crc32, 0, 1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_crypt, 0, 2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, salt, IS_STRING, 0)
ZEND_END_ARG_INFO()

#if defined(HAVE_STRPTIME)
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
	ZEND_ARG_TYPE_INFO(0, ip, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_gethostbyname, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, hostname, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_gethostbynamel, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, hostname, IS_STRING, 0)
ZEND_END_ARG_INFO()

#if (defined(PHP_WIN32) || defined(HAVE_DNS_SEARCH_FUNC))
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_dns_check_record, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, hostname, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, type, IS_STRING, 0, "\"MX\"")
ZEND_END_ARG_INFO()
#endif

#if (defined(PHP_WIN32) || defined(HAVE_DNS_SEARCH_FUNC))
#define arginfo_checkdnsrr arginfo_dns_check_record
#endif

#if (defined(PHP_WIN32) || defined(HAVE_DNS_SEARCH_FUNC))
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_dns_get_record, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, hostname, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, type, IS_LONG, 0, "DNS_ANY")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, authoritative_name_servers, "null")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, additional_records, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, raw, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()
#endif

#if (defined(PHP_WIN32) || defined(HAVE_DNS_SEARCH_FUNC))
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_dns_get_mx, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, hostname, IS_STRING, 0)
	ZEND_ARG_INFO(1, hosts)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, weights, "null")
ZEND_END_ARG_INFO()
#endif

#if (defined(PHP_WIN32) || defined(HAVE_DNS_SEARCH_FUNC))
#define arginfo_getmxrr arginfo_dns_get_mx
#endif

#if (defined(PHP_WIN32) || HAVE_GETIFADDRS || defined(__PASE__))
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_net_get_interfaces, 0, 0, MAY_BE_ARRAY|MAY_BE_FALSE)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_FTOK)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ftok, 0, 2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, project_id, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_hrtime, 0, 0, MAY_BE_ARRAY|MAY_BE_LONG|MAY_BE_DOUBLE|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, as_number, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_md5, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, binary, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_md5_file, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, binary, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

#define arginfo_getmyuid arginfo_ob_get_length

#define arginfo_getmygid arginfo_ob_get_length

#define arginfo_getmypid arginfo_ob_get_length

#define arginfo_getmyinode arginfo_ob_get_length

#define arginfo_getlastmod arginfo_ob_get_length

#define arginfo_sha1 arginfo_md5

#define arginfo_sha1_file arginfo_md5_file

#if defined(HAVE_SYSLOG_H)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_openlog, 0, 3, IS_TRUE, 0)
	ZEND_ARG_TYPE_INFO(0, prefix, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, facility, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_SYSLOG_H)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_closelog, 0, 0, IS_TRUE, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_SYSLOG_H)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_syslog, 0, 2, IS_TRUE, 0)
	ZEND_ARG_TYPE_INFO(0, priority, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, message, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#define arginfo_inet_ntop arginfo_gethostbyaddr

#define arginfo_inet_pton arginfo_gethostbyaddr

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_metaphone, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, max_phonemes, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_header, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, header, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, replace, _IS_BOOL, 0, "true")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, response_code, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_header_remove, 0, 0, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, name, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_setrawcookie, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, value, IS_STRING, 0, "\"\"")
	ZEND_ARG_TYPE_MASK(0, expires_or_options, MAY_BE_ARRAY|MAY_BE_LONG, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, path, IS_STRING, 0, "\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, domain, IS_STRING, 0, "\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, secure, _IS_BOOL, 0, "false")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, httponly, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

#define arginfo_setcookie arginfo_setrawcookie

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_http_response_code, 0, 0, MAY_BE_LONG|MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, response_code, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_headers_sent, 0, 0, _IS_BOOL, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, filename, "null")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, line, "null")
ZEND_END_ARG_INFO()

#define arginfo_headers_list arginfo_ob_list_handlers

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_htmlspecialchars, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "ENT_QUOTES | ENT_SUBSTITUTE | ENT_HTML401")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encoding, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, double_encode, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_htmlspecialchars_decode, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "ENT_QUOTES | ENT_SUBSTITUTE | ENT_HTML401")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_html_entity_decode, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "ENT_QUOTES | ENT_SUBSTITUTE | ENT_HTML401")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encoding, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_htmlentities arginfo_htmlspecialchars

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_get_html_translation_table, 0, 0, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, table, IS_LONG, 0, "HTML_SPECIALCHARS")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "ENT_QUOTES | ENT_SUBSTITUTE | ENT_HTML401")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encoding, IS_STRING, 0, "\"UTF-8\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_assert, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, assertion, IS_MIXED, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, description, Throwable, MAY_BE_STRING|MAY_BE_NULL, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_assert_options, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, option, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_bin2hex arginfo_base64_encode

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_hex2bin, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_strspn, 0, 2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, characters, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, offset, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_strcspn arginfo_strspn

#if defined(HAVE_NL_LANGINFO)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_nl_langinfo, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, item, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_strcoll, 0, 2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, string1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string2, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_trim, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, characters, IS_STRING, 0, "\" \\n\\r\\t\\v\\x00\"")
ZEND_END_ARG_INFO()

#define arginfo_rtrim arginfo_trim

#define arginfo_chop arginfo_trim

#define arginfo_ltrim arginfo_trim

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wordwrap, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, width, IS_LONG, 0, "75")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, break, IS_STRING, 0, "\"\\n\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, cut_long_words, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_explode, 0, 2, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, separator, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, limit, IS_LONG, 0, "PHP_INT_MAX")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_implode, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, separator, MAY_BE_STRING|MAY_BE_ARRAY, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, array, IS_ARRAY, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_join arginfo_implode

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_strtok, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, token, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_strtoupper arginfo_base64_encode

#define arginfo_strtolower arginfo_base64_encode

#define arginfo_str_increment arginfo_base64_encode

#define arginfo_str_decrement arginfo_base64_encode

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_basename, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, path, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, suffix, IS_STRING, 0, "\"\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_dirname, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, path, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, levels, IS_LONG, 0, "1")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pathinfo, 0, 1, MAY_BE_ARRAY|MAY_BE_STRING)
	ZEND_ARG_TYPE_INFO(0, path, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "PATHINFO_ALL")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_stristr, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, haystack, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, needle, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, before_needle, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

#define arginfo_strstr arginfo_stristr

#define arginfo_strchr arginfo_stristr

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_strpos, 0, 2, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, haystack, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, needle, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, offset, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_stripos arginfo_strpos

#define arginfo_strrpos arginfo_strpos

#define arginfo_strripos arginfo_strpos

#define arginfo_strrchr arginfo_stristr

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_str_contains, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, haystack, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, needle, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_str_starts_with arginfo_str_contains

#define arginfo_str_ends_with arginfo_str_contains

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_chunk_split, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 0, "76")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, separator, IS_STRING, 0, "\"\\r\\n\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_substr, 0, 2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_substr_replace, 0, 3, MAY_BE_STRING|MAY_BE_ARRAY)
	ZEND_ARG_TYPE_MASK(0, string, MAY_BE_ARRAY|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_MASK(0, replace, MAY_BE_ARRAY|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_MASK(0, offset, MAY_BE_ARRAY|MAY_BE_LONG, NULL)
	ZEND_ARG_TYPE_MASK(0, length, MAY_BE_ARRAY|MAY_BE_LONG|MAY_BE_NULL, "null")
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
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, separators, IS_STRING, 0, "\" \\t\\r\\n\\f\\v\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_strtr, 0, 2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, from, MAY_BE_STRING|MAY_BE_ARRAY, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, to, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_strrev arginfo_base64_encode

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_similar_text, 0, 2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, string1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string2, IS_STRING, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, percent, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_addcslashes, 0, 2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, characters, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_addslashes arginfo_base64_encode

#define arginfo_stripcslashes arginfo_base64_encode

#define arginfo_stripslashes arginfo_base64_encode

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_str_replace, 0, 3, MAY_BE_STRING|MAY_BE_ARRAY)
	ZEND_ARG_TYPE_MASK(0, search, MAY_BE_ARRAY|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_MASK(0, replace, MAY_BE_ARRAY|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_MASK(0, subject, MAY_BE_STRING|MAY_BE_ARRAY, NULL)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, count, "null")
ZEND_END_ARG_INFO()

#define arginfo_str_ireplace arginfo_str_replace

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_hebrev, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, max_chars_per_line, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_nl2br, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, use_xhtml, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_strip_tags, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, allowed_tags, MAY_BE_ARRAY|MAY_BE_STRING|MAY_BE_NULL, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_setlocale, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, category, IS_LONG, 0)
	ZEND_ARG_INFO(0, locales)
	ZEND_ARG_VARIADIC_INFO(0, rest)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_parse_str, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_INFO(1, result)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_str_getcsv, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, separator, IS_STRING, 0, "\",\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, enclosure, IS_STRING, 0, "\"\\\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, escape, IS_STRING, 0, "\"\\\\\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_str_repeat, 0, 2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, times, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_count_chars, 0, 1, MAY_BE_ARRAY|MAY_BE_STRING)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_strnatcmp arginfo_strcoll

#define arginfo_localeconv arginfo_ob_list_handlers

#define arginfo_strnatcasecmp arginfo_strcoll

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_substr_count, 0, 2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, haystack, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, needle, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, offset, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_str_pad, 0, 2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, pad_string, IS_STRING, 0, "\" \"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, pad_type, IS_LONG, 0, "STR_PAD_RIGHT")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_sscanf, 0, 2, MAY_BE_ARRAY|MAY_BE_LONG|MAY_BE_NULL)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, format, IS_STRING, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(1, vars, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_str_rot13 arginfo_base64_encode

#define arginfo_str_shuffle arginfo_base64_encode

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_str_word_count, 0, 1, MAY_BE_ARRAY|MAY_BE_LONG)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, format, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, characters, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_str_split, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 0, "1")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_strpbrk, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, characters, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_substr_compare, 0, 3, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, haystack, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, needle, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, case_insensitive, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

#define arginfo_utf8_encode arginfo_base64_encode

#define arginfo_utf8_decode arginfo_base64_encode

ZEND_BEGIN_ARG_INFO_EX(arginfo_opendir, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, directory, IS_STRING, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, context, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_dir, 0, 1, Directory, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, directory, IS_STRING, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, context, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_closedir, 0, 0, IS_VOID, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, dir_handle, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_chdir, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, directory, IS_STRING, 0)
ZEND_END_ARG_INFO()

#if (defined(HAVE_CHROOT) && !defined(ZTS) && defined(ENABLE_CHROOT_FUNC))
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_chroot, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, directory, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#define arginfo_getcwd arginfo_ob_get_flush

#define arginfo_rewinddir arginfo_closedir

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_readdir, 0, 0, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, dir_handle, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_scandir, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, directory, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, sorting_order, IS_LONG, 0, "SCANDIR_SORT_ASCENDING")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, context, "null")
ZEND_END_ARG_INFO()

#if defined(HAVE_GLOB)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_glob, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_exec, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, command, IS_STRING, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, output, "null")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, result_code, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_system, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, command, IS_STRING, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, result_code, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_passthru, 0, 1, IS_FALSE, 1)
	ZEND_ARG_TYPE_INFO(0, command, IS_STRING, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, result_code, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_escapeshellcmd, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, command, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_escapeshellarg, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, arg, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_shell_exec, 0, 1, MAY_BE_STRING|MAY_BE_FALSE|MAY_BE_NULL)
	ZEND_ARG_TYPE_INFO(0, command, IS_STRING, 0)
ZEND_END_ARG_INFO()

#if defined(HAVE_NICE)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_proc_nice, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, priority, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_flock, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, stream)
	ZEND_ARG_TYPE_INFO(0, operation, IS_LONG, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, would_block, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_get_meta_tags, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, use_include_path, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pclose, 0, 1, IS_LONG, 0)
	ZEND_ARG_INFO(0, handle)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_popen, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, command, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, mode, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_readfile, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, use_include_path, _IS_BOOL, 0, "false")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, context, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_rewind, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, stream)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_rmdir, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, directory, IS_STRING, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, context, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_umask, 0, 0, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mask, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_fclose arginfo_rewind

#define arginfo_feof arginfo_rewind

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_fgetc, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, stream)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_fgets, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, stream)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_fread, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, stream)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_fopen, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, mode, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, use_include_path, _IS_BOOL, 0, "false")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, context, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_fscanf, 0, 2, MAY_BE_ARRAY|MAY_BE_LONG|MAY_BE_FALSE|MAY_BE_NULL)
	ZEND_ARG_INFO(0, stream)
	ZEND_ARG_TYPE_INFO(0, format, IS_STRING, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(1, vars, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_fpassthru, 0, 1, IS_LONG, 0)
	ZEND_ARG_INFO(0, stream)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ftruncate, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, stream)
	ZEND_ARG_TYPE_INFO(0, size, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_fstat, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, stream)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_fseek, 0, 2, IS_LONG, 0)
	ZEND_ARG_INFO(0, stream)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, whence, IS_LONG, 0, "SEEK_SET")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_ftell, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, stream)
ZEND_END_ARG_INFO()

#define arginfo_fflush arginfo_rewind

#define arginfo_fsync arginfo_rewind

#define arginfo_fdatasync arginfo_rewind

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_fwrite, 0, 2, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, stream)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_fputs arginfo_fwrite

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mkdir, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, directory, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, permissions, IS_LONG, 0, "0777")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, recursive, _IS_BOOL, 0, "false")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, context, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_rename, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, from, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, to, IS_STRING, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, context, "null")
ZEND_END_ARG_INFO()

#define arginfo_copy arginfo_rename

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_tempnam, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, directory, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, prefix, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_tmpfile, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_file, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, context, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_file_get_contents, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, use_include_path, _IS_BOOL, 0, "false")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, context, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, offset, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_unlink, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, context, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_file_put_contents, 0, 2, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, context, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_fputcsv, 0, 2, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, stream)
	ZEND_ARG_TYPE_INFO(0, fields, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, separator, IS_STRING, 0, "\",\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, enclosure, IS_STRING, 0, "\"\\\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, escape, IS_STRING, 0, "\"\\\\\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, eol, IS_STRING, 0, "\"\\n\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_fgetcsv, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, stream)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, separator, IS_STRING, 0, "\",\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, enclosure, IS_STRING, 0, "\"\\\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, escape, IS_STRING, 0, "\"\\\\\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_realpath, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, path, IS_STRING, 0)
ZEND_END_ARG_INFO()

#if defined(HAVE_FNMATCH)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_fnmatch, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()
#endif

#define arginfo_sys_get_temp_dir arginfo_get_current_user

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

#define arginfo_file_exists arginfo_is_uploaded_file

#define arginfo_is_writable arginfo_is_uploaded_file

#define arginfo_is_writeable arginfo_is_uploaded_file

#define arginfo_is_readable arginfo_is_uploaded_file

#define arginfo_is_executable arginfo_is_uploaded_file

#define arginfo_is_file arginfo_is_uploaded_file

#define arginfo_is_dir arginfo_is_uploaded_file

#define arginfo_is_link arginfo_is_uploaded_file

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_stat, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_lstat arginfo_stat

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_chown, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, user, MAY_BE_STRING|MAY_BE_LONG, NULL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_chgrp, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, group, MAY_BE_STRING|MAY_BE_LONG, NULL)
ZEND_END_ARG_INFO()

#if defined(HAVE_LCHOWN)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_lchown, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, user, MAY_BE_STRING|MAY_BE_LONG, NULL)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_LCHOWN)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_lchgrp, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, group, MAY_BE_STRING|MAY_BE_LONG, NULL)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_chmod, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, permissions, IS_LONG, 0)
ZEND_END_ARG_INFO()

#if defined(HAVE_UTIME)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_touch, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mtime, IS_LONG, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, atime, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_clearstatcache, 0, 0, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, clear_realpath_cache, _IS_BOOL, 0, "false")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, filename, IS_STRING, 0, "\"\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_disk_total_space, 0, 1, MAY_BE_DOUBLE|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, directory, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_disk_free_space arginfo_disk_total_space

#define arginfo_diskfreespace arginfo_disk_total_space

#define arginfo_realpath_cache_get arginfo_ob_list_handlers

#define arginfo_realpath_cache_size arginfo_ob_get_level

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sprintf, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, format, IS_STRING, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, values, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_printf, 0, 1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, format, IS_STRING, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, values, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vprintf, 0, 2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, format, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, values, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vsprintf, 0, 2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, format, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, values, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_fprintf, 0, 2, IS_LONG, 0)
	ZEND_ARG_INFO(0, stream)
	ZEND_ARG_TYPE_INFO(0, format, IS_STRING, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, values, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vfprintf, 0, 3, IS_LONG, 0)
	ZEND_ARG_INFO(0, stream)
	ZEND_ARG_TYPE_INFO(0, format, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, values, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_fsockopen, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, hostname, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, port, IS_LONG, 0, "-1")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, error_code, "null")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, error_message, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timeout, IS_DOUBLE, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_pfsockopen arginfo_fsockopen

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_http_build_query, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, data, MAY_BE_ARRAY|MAY_BE_OBJECT, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, numeric_prefix, IS_STRING, 0, "\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, arg_separator, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encoding_type, IS_LONG, 0, "PHP_QUERY_RFC1738")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_request_parse_body, 0, 0, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_image_type_to_mime_type, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, image_type, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_image_type_to_extension, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, image_type, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, include_dot, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_getimagesize, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, image_info, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_getimagesizefromstring, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, image_info, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_phpinfo, 0, 0, IS_TRUE, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "INFO_ALL")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_phpversion, 0, 0, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, extension, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_phpcredits, 0, 0, IS_TRUE, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "CREDITS_ALL")
ZEND_END_ARG_INFO()

#define arginfo_php_sapi_name arginfo_ob_get_flush

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_php_uname, 0, 0, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_STRING, 0, "\"a\"")
ZEND_END_ARG_INFO()

#define arginfo_php_ini_scanned_files arginfo_ob_get_flush

#define arginfo_php_ini_loaded_file arginfo_ob_get_flush

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_iptcembed, 0, 2, MAY_BE_STRING|MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, iptc_data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, spool, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_iptcparse, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, iptc_block, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_levenshtein, 0, 2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, string1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, insertion_cost, IS_LONG, 0, "1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, replacement_cost, IS_LONG, 0, "1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, deletion_cost, IS_LONG, 0, "1")
ZEND_END_ARG_INFO()

#if (defined(HAVE_SYMLINK) || defined(PHP_WIN32))
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_readlink, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, path, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if (defined(HAVE_SYMLINK) || defined(PHP_WIN32))
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_linkinfo, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, path, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if (defined(HAVE_SYMLINK) || defined(PHP_WIN32))
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_symlink, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, target, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, link, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if (defined(HAVE_SYMLINK) || defined(PHP_WIN32))
#define arginfo_link arginfo_symlink
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mail, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, to, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, subject, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, message, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, additional_headers, MAY_BE_ARRAY|MAY_BE_STRING, "[]")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, additional_params, IS_STRING, 0, "\"\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_abs, 0, 1, MAY_BE_LONG|MAY_BE_DOUBLE)
	ZEND_ARG_TYPE_MASK(0, num, MAY_BE_LONG|MAY_BE_DOUBLE, NULL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ceil, 0, 1, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_MASK(0, num, MAY_BE_LONG|MAY_BE_DOUBLE, NULL)
ZEND_END_ARG_INFO()

#define arginfo_floor arginfo_ceil

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_round, 0, 1, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_MASK(0, num, MAY_BE_LONG|MAY_BE_DOUBLE, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, precision, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "PHP_ROUND_HALF_UP")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sin, 0, 1, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, num, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

#define arginfo_cos arginfo_sin

#define arginfo_tan arginfo_sin

#define arginfo_asin arginfo_sin

#define arginfo_acos arginfo_sin

#define arginfo_atan arginfo_sin

#define arginfo_atanh arginfo_sin

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_atan2, 0, 2, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, y, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, x, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

#define arginfo_sinh arginfo_sin

#define arginfo_cosh arginfo_sin

#define arginfo_tanh arginfo_sin

#define arginfo_asinh arginfo_sin

#define arginfo_acosh arginfo_sin

#define arginfo_expm1 arginfo_sin

#define arginfo_log1p arginfo_sin

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pi, 0, 0, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_is_finite, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, num, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

#define arginfo_is_nan arginfo_is_finite

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_intdiv, 0, 2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, num1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, num2, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_is_infinite arginfo_is_finite

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pow, 0, 2, MAY_BE_LONG|MAY_BE_DOUBLE|MAY_BE_OBJECT)
	ZEND_ARG_TYPE_INFO(0, num, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, exponent, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_exp arginfo_sin

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_log, 0, 1, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, num, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, base, IS_DOUBLE, 0, "M_E")
ZEND_END_ARG_INFO()

#define arginfo_log10 arginfo_sin

#define arginfo_sqrt arginfo_sin

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_hypot, 0, 2, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, x, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, y, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

#define arginfo_deg2rad arginfo_sin

#define arginfo_rad2deg arginfo_sin

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
	ZEND_ARG_TYPE_INFO(0, num, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_decoct arginfo_decbin

#define arginfo_dechex arginfo_decbin

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_base_convert, 0, 3, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, num, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, from_base, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, to_base, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_number_format, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, num, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, decimals, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, decimal_separator, IS_STRING, 1, "\".\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, thousands_separator, IS_STRING, 1, "\",\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_fmod, 0, 2, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, num1, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, num2, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

#define arginfo_fdiv arginfo_fmod

#if defined(HAVE_GETTIMEOFDAY)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_microtime, 0, 0, MAY_BE_STRING|MAY_BE_DOUBLE)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, as_float, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_GETTIMEOFDAY)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_gettimeofday, 0, 0, MAY_BE_ARRAY|MAY_BE_DOUBLE)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, as_float, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_GETRUSAGE)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_getrusage, 0, 0, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()
#endif

#define arginfo_pack arginfo_sprintf

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_unpack, 0, 2, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, format, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, offset, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_password_get_info, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, hash, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_password_hash, 0, 2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, password, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, algo, MAY_BE_STRING|MAY_BE_LONG|MAY_BE_NULL, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_password_needs_rehash, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, hash, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, algo, MAY_BE_STRING|MAY_BE_LONG|MAY_BE_NULL, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_password_verify, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, password, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, hash, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_password_algos arginfo_ob_list_handlers

#if defined(PHP_CAN_SUPPORT_PROC_OPEN)
ZEND_BEGIN_ARG_INFO_EX(arginfo_proc_open, 0, 0, 3)
	ZEND_ARG_TYPE_MASK(0, command, MAY_BE_ARRAY|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO(0, descriptor_spec, IS_ARRAY, 0)
	ZEND_ARG_INFO(1, pipes)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, cwd, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, env_vars, IS_ARRAY, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 1, "null")
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
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, signal, IS_LONG, 0, "15")
ZEND_END_ARG_INFO()
#endif

#if defined(PHP_CAN_SUPPORT_PROC_OPEN)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_proc_get_status, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_INFO(0, process)
ZEND_END_ARG_INFO()
#endif

#define arginfo_quoted_printable_decode arginfo_base64_encode

#define arginfo_quoted_printable_encode arginfo_base64_encode

#define arginfo_soundex arginfo_base64_encode

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_stream_select, 0, 4, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(1, read, IS_ARRAY, 1)
	ZEND_ARG_TYPE_INFO(1, write, IS_ARRAY, 1)
	ZEND_ARG_TYPE_INFO(1, except, IS_ARRAY, 1)
	ZEND_ARG_TYPE_INFO(0, seconds, IS_LONG, 1)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, microseconds, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_stream_context_create, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, params, IS_ARRAY, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_stream_context_set_params, 0, 2, IS_TRUE, 0)
	ZEND_ARG_INFO(0, context)
	ZEND_ARG_TYPE_INFO(0, params, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_stream_context_get_params, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_INFO(0, context)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_stream_context_set_option, 0, 2, IS_TRUE, 0)
	ZEND_ARG_INFO(0, context)
	ZEND_ARG_TYPE_MASK(0, wrapper_or_options, MAY_BE_ARRAY|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, option_name, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_stream_context_set_options, 0, 2, IS_TRUE, 0)
	ZEND_ARG_INFO(0, context)
	ZEND_ARG_TYPE_INFO(0, options, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_stream_context_get_options, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_INFO(0, stream_or_context)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_stream_context_get_default, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_stream_context_set_default, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, options, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_stream_filter_prepend, 0, 0, 2)
	ZEND_ARG_INFO(0, stream)
	ZEND_ARG_TYPE_INFO(0, filter_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO(0, params, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_stream_filter_append arginfo_stream_filter_prepend

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_stream_filter_remove, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, stream_filter)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_stream_socket_client, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, address, IS_STRING, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, error_code, "null")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, error_message, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timeout, IS_DOUBLE, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "STREAM_CLIENT_CONNECT")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, context, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_stream_socket_server, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, address, IS_STRING, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, error_code, "null")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, error_message, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "STREAM_SERVER_BIND | STREAM_SERVER_LISTEN")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, context, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_stream_socket_accept, 0, 0, 1)
	ZEND_ARG_INFO(0, socket)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timeout, IS_DOUBLE, 1, "null")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, peer_name, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_stream_socket_get_name, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, socket)
	ZEND_ARG_TYPE_INFO(0, remote, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_stream_socket_recvfrom, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, socket)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, address, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_stream_socket_sendto, 0, 2, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, socket)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, address, IS_STRING, 0, "\"\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_stream_socket_enable_crypto, 0, 2, MAY_BE_LONG|MAY_BE_BOOL)
	ZEND_ARG_INFO(0, stream)
	ZEND_ARG_TYPE_INFO(0, enable, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, crypto_method, IS_LONG, 1, "null")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, session_stream, "null")
ZEND_END_ARG_INFO()

#if defined(HAVE_SHUTDOWN)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_stream_socket_shutdown, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, stream)
	ZEND_ARG_TYPE_INFO(0, mode, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_SOCKETPAIR)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_stream_socket_pair, 0, 3, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, domain, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, type, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, protocol, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_stream_copy_to_stream, 0, 2, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, from)
	ZEND_ARG_INFO(0, to)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, offset, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_stream_get_contents, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, stream)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, offset, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()

#define arginfo_stream_supports_lock arginfo_rewind

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_stream_set_write_buffer, 0, 2, IS_LONG, 0)
	ZEND_ARG_INFO(0, stream)
	ZEND_ARG_TYPE_INFO(0, size, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_set_file_buffer arginfo_stream_set_write_buffer

#define arginfo_stream_set_read_buffer arginfo_stream_set_write_buffer

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_stream_set_blocking, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, stream)
	ZEND_ARG_TYPE_INFO(0, enable, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_socket_set_blocking arginfo_stream_set_blocking

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_stream_get_meta_data, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_INFO(0, stream)
ZEND_END_ARG_INFO()

#define arginfo_socket_get_status arginfo_stream_get_meta_data

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_stream_get_line, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, stream)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, ending, IS_STRING, 0, "\"\"")
ZEND_END_ARG_INFO()

#define arginfo_stream_resolve_include_path arginfo_filetype

#define arginfo_stream_get_wrappers arginfo_ob_list_handlers

#define arginfo_stream_get_transports arginfo_ob_list_handlers

#define arginfo_stream_is_local arginfo_rewind

#define arginfo_stream_isatty arginfo_rewind

#if defined(PHP_WIN32)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sapi_windows_vt100_support, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, stream)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, enable, _IS_BOOL, 1, "null")
ZEND_END_ARG_INFO()
#endif

#define arginfo_stream_set_chunk_size arginfo_stream_set_write_buffer

#if (defined(HAVE_SYS_TIME_H) || defined(PHP_WIN32))
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_stream_set_timeout, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, stream)
	ZEND_ARG_TYPE_INFO(0, seconds, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, microseconds, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()
#endif

#if (defined(HAVE_SYS_TIME_H) || defined(PHP_WIN32))
#define arginfo_socket_set_timeout arginfo_stream_set_timeout
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_gettype, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_get_debug_type arginfo_gettype

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_settype, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(1, var, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, type, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_intval, 0, 1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, base, IS_LONG, 0, "10")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_floatval, 0, 1, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_doubleval arginfo_floatval

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_boolval, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_strval arginfo_gettype

#define arginfo_is_null arginfo_boolval

#define arginfo_is_resource arginfo_boolval

#define arginfo_is_bool arginfo_boolval

#define arginfo_is_int arginfo_boolval

#define arginfo_is_integer arginfo_boolval

#define arginfo_is_long arginfo_boolval

#define arginfo_is_float arginfo_boolval

#define arginfo_is_double arginfo_boolval

#define arginfo_is_numeric arginfo_boolval

#define arginfo_is_string arginfo_boolval

#define arginfo_is_array arginfo_boolval

#define arginfo_is_object arginfo_boolval

#define arginfo_is_scalar arginfo_boolval

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_is_callable, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, syntax_only, _IS_BOOL, 0, "false")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, callable_name, "null")
ZEND_END_ARG_INFO()

#define arginfo_is_iterable arginfo_boolval

#define arginfo_is_countable arginfo_boolval

#if defined(HAVE_GETTIMEOFDAY)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_uniqid, 0, 0, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, prefix, IS_STRING, 0, "\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, more_entropy, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_parse_url, 0, 1, MAY_BE_LONG|MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_NULL|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, url, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, component, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()

#define arginfo_urlencode arginfo_base64_encode

#define arginfo_urldecode arginfo_base64_encode

#define arginfo_rawurlencode arginfo_base64_encode

#define arginfo_rawurldecode arginfo_base64_encode

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_get_headers, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, url, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, associative, _IS_BOOL, 0, "false")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, context, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_stream_bucket_make_writeable, 0, 1, IS_OBJECT, 1)
	ZEND_ARG_INFO(0, brigade)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_stream_bucket_prepend, 0, 2, IS_VOID, 0)
	ZEND_ARG_INFO(0, brigade)
	ZEND_ARG_TYPE_INFO(0, bucket, IS_OBJECT, 0)
ZEND_END_ARG_INFO()

#define arginfo_stream_bucket_append arginfo_stream_bucket_prepend

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_stream_bucket_new, 0, 2, IS_OBJECT, 0)
	ZEND_ARG_INFO(0, stream)
	ZEND_ARG_TYPE_INFO(0, buffer, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_stream_get_filters arginfo_ob_list_handlers

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_stream_filter_register, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, filter_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, class, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_convert_uuencode arginfo_base64_encode

#define arginfo_convert_uudecode arginfo_hex2bin

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_var_dump, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, values, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_var_export, 0, 1, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, return, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

#define arginfo_debug_zval_dump arginfo_var_dump

#define arginfo_serialize arginfo_gettype

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_unserialize, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_memory_get_usage, 0, 0, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, real_usage, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

#define arginfo_memory_get_peak_usage arginfo_memory_get_usage

#define arginfo_memory_reset_peak_usage arginfo_flush

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_version_compare, 0, 2, MAY_BE_LONG|MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, version1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, version2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, operator, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

#if defined(PHP_WIN32)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sapi_windows_cp_set, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, codepage, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(PHP_WIN32)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sapi_windows_cp_get, 0, 0, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, kind, IS_STRING, 0, "\"\"")
ZEND_END_ARG_INFO()
#endif

#if defined(PHP_WIN32)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sapi_windows_cp_conv, 0, 3, IS_STRING, 1)
	ZEND_ARG_TYPE_MASK(0, in_codepage, MAY_BE_LONG|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_MASK(0, out_codepage, MAY_BE_LONG|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO(0, subject, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(PHP_WIN32)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sapi_windows_cp_is_utf8, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(PHP_WIN32)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sapi_windows_set_ctrl_handler, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, handler, IS_CALLABLE, 1)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, add, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()
#endif

#if defined(PHP_WIN32)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sapi_windows_generate_ctrl_event, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, event, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, pid, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()
#endif


ZEND_FRAMELESS_FUNCTION(min, 2);
static const zend_frameless_function_info frameless_function_infos_min[] = {
	{ ZEND_FRAMELESS_FUNCTION_NAME(min, 2), 2 },
	{ 0 },
};

ZEND_FRAMELESS_FUNCTION(max, 2);
static const zend_frameless_function_info frameless_function_infos_max[] = {
	{ ZEND_FRAMELESS_FUNCTION_NAME(max, 2), 2 },
	{ 0 },
};

ZEND_FRAMELESS_FUNCTION(in_array, 2);
ZEND_FRAMELESS_FUNCTION(in_array, 3);
static const zend_frameless_function_info frameless_function_infos_in_array[] = {
	{ ZEND_FRAMELESS_FUNCTION_NAME(in_array, 2), 2 },
	{ ZEND_FRAMELESS_FUNCTION_NAME(in_array, 3), 3 },
	{ 0 },
};

ZEND_FRAMELESS_FUNCTION(trim, 1);
ZEND_FRAMELESS_FUNCTION(trim, 2);
static const zend_frameless_function_info frameless_function_infos_trim[] = {
	{ ZEND_FRAMELESS_FUNCTION_NAME(trim, 1), 1 },
	{ ZEND_FRAMELESS_FUNCTION_NAME(trim, 2), 2 },
	{ 0 },
};

ZEND_FRAMELESS_FUNCTION(implode, 1);
ZEND_FRAMELESS_FUNCTION(implode, 2);
static const zend_frameless_function_info frameless_function_infos_implode[] = {
	{ ZEND_FRAMELESS_FUNCTION_NAME(implode, 1), 1 },
	{ ZEND_FRAMELESS_FUNCTION_NAME(implode, 2), 2 },
	{ 0 },
};

ZEND_FRAMELESS_FUNCTION(dirname, 1);
ZEND_FRAMELESS_FUNCTION(dirname, 2);
static const zend_frameless_function_info frameless_function_infos_dirname[] = {
	{ ZEND_FRAMELESS_FUNCTION_NAME(dirname, 1), 1 },
	{ ZEND_FRAMELESS_FUNCTION_NAME(dirname, 2), 2 },
	{ 0 },
};

ZEND_FRAMELESS_FUNCTION(strstr, 2);
ZEND_FRAMELESS_FUNCTION(strstr, 3);
static const zend_frameless_function_info frameless_function_infos_strstr[] = {
	{ ZEND_FRAMELESS_FUNCTION_NAME(strstr, 2), 2 },
	{ ZEND_FRAMELESS_FUNCTION_NAME(strstr, 3), 3 },
	{ 0 },
};

ZEND_FRAMELESS_FUNCTION(strpos, 2);
ZEND_FRAMELESS_FUNCTION(strpos, 3);
static const zend_frameless_function_info frameless_function_infos_strpos[] = {
	{ ZEND_FRAMELESS_FUNCTION_NAME(strpos, 2), 2 },
	{ ZEND_FRAMELESS_FUNCTION_NAME(strpos, 3), 3 },
	{ 0 },
};

ZEND_FRAMELESS_FUNCTION(str_contains, 2);
static const zend_frameless_function_info frameless_function_infos_str_contains[] = {
	{ ZEND_FRAMELESS_FUNCTION_NAME(str_contains, 2), 2 },
	{ 0 },
};

ZEND_FRAMELESS_FUNCTION(str_starts_with, 2);
static const zend_frameless_function_info frameless_function_infos_str_starts_with[] = {
	{ ZEND_FRAMELESS_FUNCTION_NAME(str_starts_with, 2), 2 },
	{ 0 },
};

ZEND_FRAMELESS_FUNCTION(substr, 2);
ZEND_FRAMELESS_FUNCTION(substr, 3);
static const zend_frameless_function_info frameless_function_infos_substr[] = {
	{ ZEND_FRAMELESS_FUNCTION_NAME(substr, 2), 2 },
	{ ZEND_FRAMELESS_FUNCTION_NAME(substr, 3), 3 },
	{ 0 },
};

ZEND_FRAMELESS_FUNCTION(strtr, 2);
ZEND_FRAMELESS_FUNCTION(strtr, 3);
static const zend_frameless_function_info frameless_function_infos_strtr[] = {
	{ ZEND_FRAMELESS_FUNCTION_NAME(strtr, 2), 2 },
	{ ZEND_FRAMELESS_FUNCTION_NAME(strtr, 3), 3 },
	{ 0 },
};

ZEND_FRAMELESS_FUNCTION(str_replace, 3);
static const zend_frameless_function_info frameless_function_infos_str_replace[] = {
	{ ZEND_FRAMELESS_FUNCTION_NAME(str_replace, 3), 3 },
	{ 0 },
};

ZEND_FRAMELESS_FUNCTION(dechex, 1);
static const zend_frameless_function_info frameless_function_infos_dechex[] = {
	{ ZEND_FRAMELESS_FUNCTION_NAME(dechex, 1), 1 },
	{ 0 },
};

ZEND_FRAMELESS_FUNCTION(is_numeric, 1);
static const zend_frameless_function_info frameless_function_infos_is_numeric[] = {
	{ ZEND_FRAMELESS_FUNCTION_NAME(is_numeric, 1), 1 },
	{ 0 },
};

ZEND_FUNCTION(set_time_limit);
ZEND_FUNCTION(header_register_callback);
ZEND_FUNCTION(ob_start);
ZEND_FUNCTION(ob_flush);
ZEND_FUNCTION(ob_clean);
ZEND_FUNCTION(ob_end_flush);
ZEND_FUNCTION(ob_end_clean);
ZEND_FUNCTION(ob_get_flush);
ZEND_FUNCTION(ob_get_clean);
ZEND_FUNCTION(ob_get_contents);
ZEND_FUNCTION(ob_get_level);
ZEND_FUNCTION(ob_get_length);
ZEND_FUNCTION(ob_list_handlers);
ZEND_FUNCTION(ob_get_status);
ZEND_FUNCTION(ob_implicit_flush);
ZEND_FUNCTION(output_reset_rewrite_vars);
ZEND_FUNCTION(output_add_rewrite_var);
ZEND_FUNCTION(stream_wrapper_register);
ZEND_FUNCTION(stream_wrapper_unregister);
ZEND_FUNCTION(stream_wrapper_restore);
ZEND_FUNCTION(array_push);
ZEND_FUNCTION(krsort);
ZEND_FUNCTION(ksort);
ZEND_FUNCTION(count);
ZEND_FUNCTION(natsort);
ZEND_FUNCTION(natcasesort);
ZEND_FUNCTION(asort);
ZEND_FUNCTION(arsort);
ZEND_FUNCTION(sort);
ZEND_FUNCTION(rsort);
ZEND_FUNCTION(usort);
ZEND_FUNCTION(uasort);
ZEND_FUNCTION(uksort);
ZEND_FUNCTION(end);
ZEND_FUNCTION(prev);
ZEND_FUNCTION(next);
ZEND_FUNCTION(reset);
ZEND_FUNCTION(current);
ZEND_FUNCTION(key);
ZEND_FUNCTION(min);
ZEND_FUNCTION(max);
ZEND_FUNCTION(array_walk);
ZEND_FUNCTION(array_walk_recursive);
ZEND_FUNCTION(in_array);
ZEND_FUNCTION(array_search);
ZEND_FUNCTION(extract);
ZEND_FUNCTION(compact);
ZEND_FUNCTION(array_fill);
ZEND_FUNCTION(array_fill_keys);
ZEND_FUNCTION(range);
ZEND_FUNCTION(shuffle);
ZEND_FUNCTION(array_pop);
ZEND_FUNCTION(array_shift);
ZEND_FUNCTION(array_unshift);
ZEND_FUNCTION(array_splice);
ZEND_FUNCTION(array_slice);
ZEND_FUNCTION(array_merge);
ZEND_FUNCTION(array_merge_recursive);
ZEND_FUNCTION(array_replace);
ZEND_FUNCTION(array_replace_recursive);
ZEND_FUNCTION(array_keys);
ZEND_FUNCTION(array_key_first);
ZEND_FUNCTION(array_key_last);
ZEND_FUNCTION(array_values);
ZEND_FUNCTION(array_count_values);
ZEND_FUNCTION(array_column);
ZEND_FUNCTION(array_reverse);
ZEND_FUNCTION(array_pad);
ZEND_FUNCTION(array_flip);
ZEND_FUNCTION(array_change_key_case);
ZEND_FUNCTION(array_unique);
ZEND_FUNCTION(array_intersect_key);
ZEND_FUNCTION(array_intersect_ukey);
ZEND_FUNCTION(array_intersect);
ZEND_FUNCTION(array_uintersect);
ZEND_FUNCTION(array_intersect_assoc);
ZEND_FUNCTION(array_uintersect_assoc);
ZEND_FUNCTION(array_intersect_uassoc);
ZEND_FUNCTION(array_uintersect_uassoc);
ZEND_FUNCTION(array_diff_key);
ZEND_FUNCTION(array_diff_ukey);
ZEND_FUNCTION(array_diff);
ZEND_FUNCTION(array_udiff);
ZEND_FUNCTION(array_diff_assoc);
ZEND_FUNCTION(array_diff_uassoc);
ZEND_FUNCTION(array_udiff_assoc);
ZEND_FUNCTION(array_udiff_uassoc);
ZEND_FUNCTION(array_multisort);
ZEND_FUNCTION(array_rand);
ZEND_FUNCTION(array_sum);
ZEND_FUNCTION(array_product);
ZEND_FUNCTION(array_reduce);
ZEND_FUNCTION(array_filter);
ZEND_FUNCTION(array_map);
ZEND_FUNCTION(array_key_exists);
ZEND_FUNCTION(array_chunk);
ZEND_FUNCTION(array_combine);
ZEND_FUNCTION(array_is_list);
ZEND_FUNCTION(base64_encode);
ZEND_FUNCTION(base64_decode);
ZEND_FUNCTION(constant);
ZEND_FUNCTION(ip2long);
ZEND_FUNCTION(long2ip);
ZEND_FUNCTION(getenv);
#if defined(HAVE_PUTENV)
ZEND_FUNCTION(putenv);
#endif
ZEND_FUNCTION(getopt);
ZEND_FUNCTION(flush);
ZEND_FUNCTION(sleep);
ZEND_FUNCTION(usleep);
#if defined(HAVE_NANOSLEEP)
ZEND_FUNCTION(time_nanosleep);
#endif
#if defined(HAVE_NANOSLEEP)
ZEND_FUNCTION(time_sleep_until);
#endif
ZEND_FUNCTION(get_current_user);
ZEND_FUNCTION(get_cfg_var);
ZEND_FUNCTION(error_log);
ZEND_FUNCTION(error_get_last);
ZEND_FUNCTION(error_clear_last);
ZEND_FUNCTION(call_user_func);
ZEND_FUNCTION(call_user_func_array);
ZEND_FUNCTION(forward_static_call);
ZEND_FUNCTION(forward_static_call_array);
ZEND_FUNCTION(register_shutdown_function);
ZEND_FUNCTION(highlight_file);
ZEND_FUNCTION(php_strip_whitespace);
ZEND_FUNCTION(highlight_string);
ZEND_FUNCTION(ini_get);
ZEND_FUNCTION(ini_get_all);
ZEND_FUNCTION(ini_set);
ZEND_FUNCTION(ini_restore);
ZEND_FUNCTION(ini_parse_quantity);
ZEND_FUNCTION(set_include_path);
ZEND_FUNCTION(get_include_path);
ZEND_FUNCTION(print_r);
ZEND_FUNCTION(connection_aborted);
ZEND_FUNCTION(connection_status);
ZEND_FUNCTION(ignore_user_abort);
#if defined(HAVE_GETSERVBYNAME)
ZEND_FUNCTION(getservbyname);
#endif
#if defined(HAVE_GETSERVBYPORT)
ZEND_FUNCTION(getservbyport);
#endif
#if defined(HAVE_GETPROTOBYNAME)
ZEND_FUNCTION(getprotobyname);
#endif
#if defined(HAVE_GETPROTOBYNUMBER)
ZEND_FUNCTION(getprotobynumber);
#endif
ZEND_FUNCTION(register_tick_function);
ZEND_FUNCTION(unregister_tick_function);
ZEND_FUNCTION(is_uploaded_file);
ZEND_FUNCTION(move_uploaded_file);
ZEND_FUNCTION(parse_ini_file);
ZEND_FUNCTION(parse_ini_string);
#if ZEND_DEBUG
ZEND_FUNCTION(config_get_hash);
#endif
#if defined(HAVE_GETLOADAVG)
ZEND_FUNCTION(sys_getloadavg);
#endif
ZEND_FUNCTION(get_browser);
ZEND_FUNCTION(crc32);
ZEND_FUNCTION(crypt);
#if defined(HAVE_STRPTIME)
ZEND_FUNCTION(strptime);
#endif
#if defined(HAVE_GETHOSTNAME)
ZEND_FUNCTION(gethostname);
#endif
ZEND_FUNCTION(gethostbyaddr);
ZEND_FUNCTION(gethostbyname);
ZEND_FUNCTION(gethostbynamel);
#if (defined(PHP_WIN32) || defined(HAVE_DNS_SEARCH_FUNC))
ZEND_FUNCTION(dns_check_record);
#endif
#if (defined(PHP_WIN32) || defined(HAVE_DNS_SEARCH_FUNC))
ZEND_FUNCTION(dns_get_record);
#endif
#if (defined(PHP_WIN32) || defined(HAVE_DNS_SEARCH_FUNC))
ZEND_FUNCTION(dns_get_mx);
#endif
#if (defined(PHP_WIN32) || HAVE_GETIFADDRS || defined(__PASE__))
ZEND_FUNCTION(net_get_interfaces);
#endif
#if defined(HAVE_FTOK)
ZEND_FUNCTION(ftok);
#endif
ZEND_FUNCTION(hrtime);
ZEND_FUNCTION(md5);
ZEND_FUNCTION(md5_file);
ZEND_FUNCTION(getmyuid);
ZEND_FUNCTION(getmygid);
ZEND_FUNCTION(getmypid);
ZEND_FUNCTION(getmyinode);
ZEND_FUNCTION(getlastmod);
ZEND_FUNCTION(sha1);
ZEND_FUNCTION(sha1_file);
#if defined(HAVE_SYSLOG_H)
ZEND_FUNCTION(openlog);
#endif
#if defined(HAVE_SYSLOG_H)
ZEND_FUNCTION(closelog);
#endif
#if defined(HAVE_SYSLOG_H)
ZEND_FUNCTION(syslog);
#endif
ZEND_FUNCTION(inet_ntop);
ZEND_FUNCTION(inet_pton);
ZEND_FUNCTION(metaphone);
ZEND_FUNCTION(header);
ZEND_FUNCTION(header_remove);
ZEND_FUNCTION(setrawcookie);
ZEND_FUNCTION(setcookie);
ZEND_FUNCTION(http_response_code);
ZEND_FUNCTION(headers_sent);
ZEND_FUNCTION(headers_list);
ZEND_FUNCTION(htmlspecialchars);
ZEND_FUNCTION(htmlspecialchars_decode);
ZEND_FUNCTION(html_entity_decode);
ZEND_FUNCTION(htmlentities);
ZEND_FUNCTION(get_html_translation_table);
ZEND_FUNCTION(assert);
ZEND_FUNCTION(assert_options);
ZEND_FUNCTION(bin2hex);
ZEND_FUNCTION(hex2bin);
ZEND_FUNCTION(strspn);
ZEND_FUNCTION(strcspn);
#if defined(HAVE_NL_LANGINFO)
ZEND_FUNCTION(nl_langinfo);
#endif
ZEND_FUNCTION(strcoll);
ZEND_FUNCTION(trim);
ZEND_FUNCTION(rtrim);
ZEND_FUNCTION(ltrim);
ZEND_FUNCTION(wordwrap);
ZEND_FUNCTION(explode);
ZEND_FUNCTION(implode);
ZEND_FUNCTION(strtok);
ZEND_FUNCTION(strtoupper);
ZEND_FUNCTION(strtolower);
ZEND_FUNCTION(str_increment);
ZEND_FUNCTION(str_decrement);
ZEND_FUNCTION(basename);
ZEND_FUNCTION(dirname);
ZEND_FUNCTION(pathinfo);
ZEND_FUNCTION(stristr);
ZEND_FUNCTION(strstr);
ZEND_FUNCTION(strpos);
ZEND_FUNCTION(stripos);
ZEND_FUNCTION(strrpos);
ZEND_FUNCTION(strripos);
ZEND_FUNCTION(strrchr);
ZEND_FUNCTION(str_contains);
ZEND_FUNCTION(str_starts_with);
ZEND_FUNCTION(str_ends_with);
ZEND_FUNCTION(chunk_split);
ZEND_FUNCTION(substr);
ZEND_FUNCTION(substr_replace);
ZEND_FUNCTION(quotemeta);
ZEND_FUNCTION(ord);
ZEND_FUNCTION(chr);
ZEND_FUNCTION(ucfirst);
ZEND_FUNCTION(lcfirst);
ZEND_FUNCTION(ucwords);
ZEND_FUNCTION(strtr);
ZEND_FUNCTION(strrev);
ZEND_FUNCTION(similar_text);
ZEND_FUNCTION(addcslashes);
ZEND_FUNCTION(addslashes);
ZEND_FUNCTION(stripcslashes);
ZEND_FUNCTION(stripslashes);
ZEND_FUNCTION(str_replace);
ZEND_FUNCTION(str_ireplace);
ZEND_FUNCTION(hebrev);
ZEND_FUNCTION(nl2br);
ZEND_FUNCTION(strip_tags);
ZEND_FUNCTION(setlocale);
ZEND_FUNCTION(parse_str);
ZEND_FUNCTION(str_getcsv);
ZEND_FUNCTION(str_repeat);
ZEND_FUNCTION(count_chars);
ZEND_FUNCTION(strnatcmp);
ZEND_FUNCTION(localeconv);
ZEND_FUNCTION(strnatcasecmp);
ZEND_FUNCTION(substr_count);
ZEND_FUNCTION(str_pad);
ZEND_FUNCTION(sscanf);
ZEND_FUNCTION(str_rot13);
ZEND_FUNCTION(str_shuffle);
ZEND_FUNCTION(str_word_count);
ZEND_FUNCTION(str_split);
ZEND_FUNCTION(strpbrk);
ZEND_FUNCTION(substr_compare);
ZEND_FUNCTION(utf8_encode);
ZEND_FUNCTION(utf8_decode);
ZEND_FUNCTION(opendir);
ZEND_FUNCTION(dir);
ZEND_FUNCTION(closedir);
ZEND_FUNCTION(chdir);
#if (defined(HAVE_CHROOT) && !defined(ZTS) && defined(ENABLE_CHROOT_FUNC))
ZEND_FUNCTION(chroot);
#endif
ZEND_FUNCTION(getcwd);
ZEND_FUNCTION(rewinddir);
ZEND_FUNCTION(readdir);
ZEND_FUNCTION(scandir);
#if defined(HAVE_GLOB)
ZEND_FUNCTION(glob);
#endif
ZEND_FUNCTION(exec);
ZEND_FUNCTION(system);
ZEND_FUNCTION(passthru);
ZEND_FUNCTION(escapeshellcmd);
ZEND_FUNCTION(escapeshellarg);
ZEND_FUNCTION(shell_exec);
#if defined(HAVE_NICE)
ZEND_FUNCTION(proc_nice);
#endif
ZEND_FUNCTION(flock);
ZEND_FUNCTION(get_meta_tags);
ZEND_FUNCTION(pclose);
ZEND_FUNCTION(popen);
ZEND_FUNCTION(readfile);
ZEND_FUNCTION(rewind);
ZEND_FUNCTION(rmdir);
ZEND_FUNCTION(umask);
ZEND_FUNCTION(fclose);
ZEND_FUNCTION(feof);
ZEND_FUNCTION(fgetc);
ZEND_FUNCTION(fgets);
ZEND_FUNCTION(fread);
ZEND_FUNCTION(fopen);
ZEND_FUNCTION(fscanf);
ZEND_FUNCTION(fpassthru);
ZEND_FUNCTION(ftruncate);
ZEND_FUNCTION(fstat);
ZEND_FUNCTION(fseek);
ZEND_FUNCTION(ftell);
ZEND_FUNCTION(fflush);
ZEND_FUNCTION(fsync);
ZEND_FUNCTION(fdatasync);
ZEND_FUNCTION(fwrite);
ZEND_FUNCTION(mkdir);
ZEND_FUNCTION(rename);
ZEND_FUNCTION(copy);
ZEND_FUNCTION(tempnam);
ZEND_FUNCTION(tmpfile);
ZEND_FUNCTION(file);
ZEND_FUNCTION(file_get_contents);
ZEND_FUNCTION(unlink);
ZEND_FUNCTION(file_put_contents);
ZEND_FUNCTION(fputcsv);
ZEND_FUNCTION(fgetcsv);
ZEND_FUNCTION(realpath);
#if defined(HAVE_FNMATCH)
ZEND_FUNCTION(fnmatch);
#endif
ZEND_FUNCTION(sys_get_temp_dir);
ZEND_FUNCTION(fileatime);
ZEND_FUNCTION(filectime);
ZEND_FUNCTION(filegroup);
ZEND_FUNCTION(fileinode);
ZEND_FUNCTION(filemtime);
ZEND_FUNCTION(fileowner);
ZEND_FUNCTION(fileperms);
ZEND_FUNCTION(filesize);
ZEND_FUNCTION(filetype);
ZEND_FUNCTION(file_exists);
ZEND_FUNCTION(is_writable);
ZEND_FUNCTION(is_readable);
ZEND_FUNCTION(is_executable);
ZEND_FUNCTION(is_file);
ZEND_FUNCTION(is_dir);
ZEND_FUNCTION(is_link);
ZEND_FUNCTION(stat);
ZEND_FUNCTION(lstat);
ZEND_FUNCTION(chown);
ZEND_FUNCTION(chgrp);
#if defined(HAVE_LCHOWN)
ZEND_FUNCTION(lchown);
#endif
#if defined(HAVE_LCHOWN)
ZEND_FUNCTION(lchgrp);
#endif
ZEND_FUNCTION(chmod);
#if defined(HAVE_UTIME)
ZEND_FUNCTION(touch);
#endif
ZEND_FUNCTION(clearstatcache);
ZEND_FUNCTION(disk_total_space);
ZEND_FUNCTION(disk_free_space);
ZEND_FUNCTION(realpath_cache_get);
ZEND_FUNCTION(realpath_cache_size);
ZEND_FUNCTION(sprintf);
ZEND_FUNCTION(printf);
ZEND_FUNCTION(vprintf);
ZEND_FUNCTION(vsprintf);
ZEND_FUNCTION(fprintf);
ZEND_FUNCTION(vfprintf);
ZEND_FUNCTION(fsockopen);
ZEND_FUNCTION(pfsockopen);
ZEND_FUNCTION(http_build_query);
ZEND_FUNCTION(request_parse_body);
ZEND_FUNCTION(image_type_to_mime_type);
ZEND_FUNCTION(image_type_to_extension);
ZEND_FUNCTION(getimagesize);
ZEND_FUNCTION(getimagesizefromstring);
ZEND_FUNCTION(phpinfo);
ZEND_FUNCTION(phpversion);
ZEND_FUNCTION(phpcredits);
ZEND_FUNCTION(php_sapi_name);
ZEND_FUNCTION(php_uname);
ZEND_FUNCTION(php_ini_scanned_files);
ZEND_FUNCTION(php_ini_loaded_file);
ZEND_FUNCTION(iptcembed);
ZEND_FUNCTION(iptcparse);
ZEND_FUNCTION(levenshtein);
#if (defined(HAVE_SYMLINK) || defined(PHP_WIN32))
ZEND_FUNCTION(readlink);
#endif
#if (defined(HAVE_SYMLINK) || defined(PHP_WIN32))
ZEND_FUNCTION(linkinfo);
#endif
#if (defined(HAVE_SYMLINK) || defined(PHP_WIN32))
ZEND_FUNCTION(symlink);
#endif
#if (defined(HAVE_SYMLINK) || defined(PHP_WIN32))
ZEND_FUNCTION(link);
#endif
ZEND_FUNCTION(mail);
ZEND_FUNCTION(abs);
ZEND_FUNCTION(ceil);
ZEND_FUNCTION(floor);
ZEND_FUNCTION(round);
ZEND_FUNCTION(sin);
ZEND_FUNCTION(cos);
ZEND_FUNCTION(tan);
ZEND_FUNCTION(asin);
ZEND_FUNCTION(acos);
ZEND_FUNCTION(atan);
ZEND_FUNCTION(atanh);
ZEND_FUNCTION(atan2);
ZEND_FUNCTION(sinh);
ZEND_FUNCTION(cosh);
ZEND_FUNCTION(tanh);
ZEND_FUNCTION(asinh);
ZEND_FUNCTION(acosh);
ZEND_FUNCTION(expm1);
ZEND_FUNCTION(log1p);
ZEND_FUNCTION(pi);
ZEND_FUNCTION(is_finite);
ZEND_FUNCTION(is_nan);
ZEND_FUNCTION(intdiv);
ZEND_FUNCTION(is_infinite);
ZEND_FUNCTION(pow);
ZEND_FUNCTION(exp);
ZEND_FUNCTION(log);
ZEND_FUNCTION(log10);
ZEND_FUNCTION(sqrt);
ZEND_FUNCTION(hypot);
ZEND_FUNCTION(deg2rad);
ZEND_FUNCTION(rad2deg);
ZEND_FUNCTION(bindec);
ZEND_FUNCTION(hexdec);
ZEND_FUNCTION(octdec);
ZEND_FUNCTION(decbin);
ZEND_FUNCTION(decoct);
ZEND_FUNCTION(dechex);
ZEND_FUNCTION(base_convert);
ZEND_FUNCTION(number_format);
ZEND_FUNCTION(fmod);
ZEND_FUNCTION(fdiv);
#if defined(HAVE_GETTIMEOFDAY)
ZEND_FUNCTION(microtime);
#endif
#if defined(HAVE_GETTIMEOFDAY)
ZEND_FUNCTION(gettimeofday);
#endif
#if defined(HAVE_GETRUSAGE)
ZEND_FUNCTION(getrusage);
#endif
ZEND_FUNCTION(pack);
ZEND_FUNCTION(unpack);
ZEND_FUNCTION(password_get_info);
ZEND_FUNCTION(password_hash);
ZEND_FUNCTION(password_needs_rehash);
ZEND_FUNCTION(password_verify);
ZEND_FUNCTION(password_algos);
#if defined(PHP_CAN_SUPPORT_PROC_OPEN)
ZEND_FUNCTION(proc_open);
#endif
#if defined(PHP_CAN_SUPPORT_PROC_OPEN)
ZEND_FUNCTION(proc_close);
#endif
#if defined(PHP_CAN_SUPPORT_PROC_OPEN)
ZEND_FUNCTION(proc_terminate);
#endif
#if defined(PHP_CAN_SUPPORT_PROC_OPEN)
ZEND_FUNCTION(proc_get_status);
#endif
ZEND_FUNCTION(quoted_printable_decode);
ZEND_FUNCTION(quoted_printable_encode);
ZEND_FUNCTION(soundex);
ZEND_FUNCTION(stream_select);
ZEND_FUNCTION(stream_context_create);
ZEND_FUNCTION(stream_context_set_params);
ZEND_FUNCTION(stream_context_get_params);
ZEND_FUNCTION(stream_context_set_option);
ZEND_FUNCTION(stream_context_set_options);
ZEND_FUNCTION(stream_context_get_options);
ZEND_FUNCTION(stream_context_get_default);
ZEND_FUNCTION(stream_context_set_default);
ZEND_FUNCTION(stream_filter_prepend);
ZEND_FUNCTION(stream_filter_append);
ZEND_FUNCTION(stream_filter_remove);
ZEND_FUNCTION(stream_socket_client);
ZEND_FUNCTION(stream_socket_server);
ZEND_FUNCTION(stream_socket_accept);
ZEND_FUNCTION(stream_socket_get_name);
ZEND_FUNCTION(stream_socket_recvfrom);
ZEND_FUNCTION(stream_socket_sendto);
ZEND_FUNCTION(stream_socket_enable_crypto);
#if defined(HAVE_SHUTDOWN)
ZEND_FUNCTION(stream_socket_shutdown);
#endif
#if defined(HAVE_SOCKETPAIR)
ZEND_FUNCTION(stream_socket_pair);
#endif
ZEND_FUNCTION(stream_copy_to_stream);
ZEND_FUNCTION(stream_get_contents);
ZEND_FUNCTION(stream_supports_lock);
ZEND_FUNCTION(stream_set_write_buffer);
ZEND_FUNCTION(stream_set_read_buffer);
ZEND_FUNCTION(stream_set_blocking);
ZEND_FUNCTION(stream_get_meta_data);
ZEND_FUNCTION(stream_get_line);
ZEND_FUNCTION(stream_resolve_include_path);
ZEND_FUNCTION(stream_get_wrappers);
ZEND_FUNCTION(stream_get_transports);
ZEND_FUNCTION(stream_is_local);
ZEND_FUNCTION(stream_isatty);
#if defined(PHP_WIN32)
ZEND_FUNCTION(sapi_windows_vt100_support);
#endif
ZEND_FUNCTION(stream_set_chunk_size);
#if (defined(HAVE_SYS_TIME_H) || defined(PHP_WIN32))
ZEND_FUNCTION(stream_set_timeout);
#endif
ZEND_FUNCTION(gettype);
ZEND_FUNCTION(get_debug_type);
ZEND_FUNCTION(settype);
ZEND_FUNCTION(intval);
ZEND_FUNCTION(floatval);
ZEND_FUNCTION(boolval);
ZEND_FUNCTION(strval);
ZEND_FUNCTION(is_null);
ZEND_FUNCTION(is_resource);
ZEND_FUNCTION(is_bool);
ZEND_FUNCTION(is_int);
ZEND_FUNCTION(is_float);
ZEND_FUNCTION(is_numeric);
ZEND_FUNCTION(is_string);
ZEND_FUNCTION(is_array);
ZEND_FUNCTION(is_object);
ZEND_FUNCTION(is_scalar);
ZEND_FUNCTION(is_callable);
ZEND_FUNCTION(is_iterable);
ZEND_FUNCTION(is_countable);
#if defined(HAVE_GETTIMEOFDAY)
ZEND_FUNCTION(uniqid);
#endif
ZEND_FUNCTION(parse_url);
ZEND_FUNCTION(urlencode);
ZEND_FUNCTION(urldecode);
ZEND_FUNCTION(rawurlencode);
ZEND_FUNCTION(rawurldecode);
ZEND_FUNCTION(get_headers);
ZEND_FUNCTION(stream_bucket_make_writeable);
ZEND_FUNCTION(stream_bucket_prepend);
ZEND_FUNCTION(stream_bucket_append);
ZEND_FUNCTION(stream_bucket_new);
ZEND_FUNCTION(stream_get_filters);
ZEND_FUNCTION(stream_filter_register);
ZEND_FUNCTION(convert_uuencode);
ZEND_FUNCTION(convert_uudecode);
ZEND_FUNCTION(var_dump);
ZEND_FUNCTION(var_export);
ZEND_FUNCTION(debug_zval_dump);
ZEND_FUNCTION(serialize);
ZEND_FUNCTION(unserialize);
ZEND_FUNCTION(memory_get_usage);
ZEND_FUNCTION(memory_get_peak_usage);
ZEND_FUNCTION(memory_reset_peak_usage);
ZEND_FUNCTION(version_compare);
#if defined(PHP_WIN32)
ZEND_FUNCTION(sapi_windows_cp_set);
#endif
#if defined(PHP_WIN32)
ZEND_FUNCTION(sapi_windows_cp_get);
#endif
#if defined(PHP_WIN32)
ZEND_FUNCTION(sapi_windows_cp_conv);
#endif
#if defined(PHP_WIN32)
ZEND_FUNCTION(sapi_windows_cp_is_utf8);
#endif
#if defined(PHP_WIN32)
ZEND_FUNCTION(sapi_windows_set_ctrl_handler);
#endif
#if defined(PHP_WIN32)
ZEND_FUNCTION(sapi_windows_generate_ctrl_event);
#endif

static const zend_function_entry ext_functions[] = {
	ZEND_RAW_FENTRY("set_time_limit", zif_set_time_limit, arginfo_set_time_limit, 0, NULL)
	ZEND_RAW_FENTRY("header_register_callback", zif_header_register_callback, arginfo_header_register_callback, 0, NULL)
	ZEND_RAW_FENTRY("ob_start", zif_ob_start, arginfo_ob_start, 0, NULL)
	ZEND_RAW_FENTRY("ob_flush", zif_ob_flush, arginfo_ob_flush, 0, NULL)
	ZEND_RAW_FENTRY("ob_clean", zif_ob_clean, arginfo_ob_clean, 0, NULL)
	ZEND_RAW_FENTRY("ob_end_flush", zif_ob_end_flush, arginfo_ob_end_flush, 0, NULL)
	ZEND_RAW_FENTRY("ob_end_clean", zif_ob_end_clean, arginfo_ob_end_clean, 0, NULL)
	ZEND_RAW_FENTRY("ob_get_flush", zif_ob_get_flush, arginfo_ob_get_flush, 0, NULL)
	ZEND_RAW_FENTRY("ob_get_clean", zif_ob_get_clean, arginfo_ob_get_clean, 0, NULL)
	ZEND_RAW_FENTRY("ob_get_contents", zif_ob_get_contents, arginfo_ob_get_contents, 0, NULL)
	ZEND_RAW_FENTRY("ob_get_level", zif_ob_get_level, arginfo_ob_get_level, 0, NULL)
	ZEND_RAW_FENTRY("ob_get_length", zif_ob_get_length, arginfo_ob_get_length, 0, NULL)
	ZEND_RAW_FENTRY("ob_list_handlers", zif_ob_list_handlers, arginfo_ob_list_handlers, 0, NULL)
	ZEND_RAW_FENTRY("ob_get_status", zif_ob_get_status, arginfo_ob_get_status, 0, NULL)
	ZEND_RAW_FENTRY("ob_implicit_flush", zif_ob_implicit_flush, arginfo_ob_implicit_flush, 0, NULL)
	ZEND_RAW_FENTRY("output_reset_rewrite_vars", zif_output_reset_rewrite_vars, arginfo_output_reset_rewrite_vars, 0, NULL)
	ZEND_RAW_FENTRY("output_add_rewrite_var", zif_output_add_rewrite_var, arginfo_output_add_rewrite_var, 0, NULL)
	ZEND_RAW_FENTRY("stream_wrapper_register", zif_stream_wrapper_register, arginfo_stream_wrapper_register, 0, NULL)
	ZEND_RAW_FENTRY("stream_register_wrapper", zif_stream_wrapper_register, arginfo_stream_register_wrapper, 0, NULL)
	ZEND_RAW_FENTRY("stream_wrapper_unregister", zif_stream_wrapper_unregister, arginfo_stream_wrapper_unregister, 0, NULL)
	ZEND_RAW_FENTRY("stream_wrapper_restore", zif_stream_wrapper_restore, arginfo_stream_wrapper_restore, 0, NULL)
	ZEND_RAW_FENTRY("array_push", zif_array_push, arginfo_array_push, 0, NULL)
	ZEND_RAW_FENTRY("krsort", zif_krsort, arginfo_krsort, 0, NULL)
	ZEND_RAW_FENTRY("ksort", zif_ksort, arginfo_ksort, 0, NULL)
	ZEND_RAW_FENTRY("count", zif_count, arginfo_count, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("sizeof", zif_count, arginfo_sizeof, 0, NULL)
	ZEND_RAW_FENTRY("natsort", zif_natsort, arginfo_natsort, 0, NULL)
	ZEND_RAW_FENTRY("natcasesort", zif_natcasesort, arginfo_natcasesort, 0, NULL)
	ZEND_RAW_FENTRY("asort", zif_asort, arginfo_asort, 0, NULL)
	ZEND_RAW_FENTRY("arsort", zif_arsort, arginfo_arsort, 0, NULL)
	ZEND_RAW_FENTRY("sort", zif_sort, arginfo_sort, 0, NULL)
	ZEND_RAW_FENTRY("rsort", zif_rsort, arginfo_rsort, 0, NULL)
	ZEND_RAW_FENTRY("usort", zif_usort, arginfo_usort, 0, NULL)
	ZEND_RAW_FENTRY("uasort", zif_uasort, arginfo_uasort, 0, NULL)
	ZEND_RAW_FENTRY("uksort", zif_uksort, arginfo_uksort, 0, NULL)
	ZEND_RAW_FENTRY("end", zif_end, arginfo_end, 0, NULL)
	ZEND_RAW_FENTRY("prev", zif_prev, arginfo_prev, 0, NULL)
	ZEND_RAW_FENTRY("next", zif_next, arginfo_next, 0, NULL)
	ZEND_RAW_FENTRY("reset", zif_reset, arginfo_reset, 0, NULL)
	ZEND_RAW_FENTRY("current", zif_current, arginfo_current, 0, NULL)
	ZEND_RAW_FENTRY("pos", zif_current, arginfo_pos, 0, NULL)
	ZEND_RAW_FENTRY("key", zif_key, arginfo_key, 0, NULL)
	ZEND_FRAMELESS_FE(min, arginfo_min, ZEND_ACC_COMPILE_TIME_EVAL, frameless_function_infos_min)
	ZEND_FRAMELESS_FE(max, arginfo_max, ZEND_ACC_COMPILE_TIME_EVAL, frameless_function_infos_max)
	ZEND_RAW_FENTRY("array_walk", zif_array_walk, arginfo_array_walk, 0, NULL)
	ZEND_RAW_FENTRY("array_walk_recursive", zif_array_walk_recursive, arginfo_array_walk_recursive, 0, NULL)
	ZEND_FRAMELESS_FE(in_array, arginfo_in_array, ZEND_ACC_COMPILE_TIME_EVAL, frameless_function_infos_in_array)
	ZEND_RAW_FENTRY("array_search", zif_array_search, arginfo_array_search, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("extract", zif_extract, arginfo_extract, 0, NULL)
	ZEND_RAW_FENTRY("compact", zif_compact, arginfo_compact, 0, NULL)
	ZEND_RAW_FENTRY("array_fill", zif_array_fill, arginfo_array_fill, 0, NULL)
	ZEND_RAW_FENTRY("array_fill_keys", zif_array_fill_keys, arginfo_array_fill_keys, 0, NULL)
	ZEND_RAW_FENTRY("range", zif_range, arginfo_range, 0, NULL)
	ZEND_RAW_FENTRY("shuffle", zif_shuffle, arginfo_shuffle, 0, NULL)
	ZEND_RAW_FENTRY("array_pop", zif_array_pop, arginfo_array_pop, 0, NULL)
	ZEND_RAW_FENTRY("array_shift", zif_array_shift, arginfo_array_shift, 0, NULL)
	ZEND_RAW_FENTRY("array_unshift", zif_array_unshift, arginfo_array_unshift, 0, NULL)
	ZEND_RAW_FENTRY("array_splice", zif_array_splice, arginfo_array_splice, 0, NULL)
	ZEND_RAW_FENTRY("array_slice", zif_array_slice, arginfo_array_slice, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("array_merge", zif_array_merge, arginfo_array_merge, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("array_merge_recursive", zif_array_merge_recursive, arginfo_array_merge_recursive, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("array_replace", zif_array_replace, arginfo_array_replace, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("array_replace_recursive", zif_array_replace_recursive, arginfo_array_replace_recursive, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("array_keys", zif_array_keys, arginfo_array_keys, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("array_key_first", zif_array_key_first, arginfo_array_key_first, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("array_key_last", zif_array_key_last, arginfo_array_key_last, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("array_values", zif_array_values, arginfo_array_values, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("array_count_values", zif_array_count_values, arginfo_array_count_values, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("array_column", zif_array_column, arginfo_array_column, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("array_reverse", zif_array_reverse, arginfo_array_reverse, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("array_pad", zif_array_pad, arginfo_array_pad, 0, NULL)
	ZEND_RAW_FENTRY("array_flip", zif_array_flip, arginfo_array_flip, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("array_change_key_case", zif_array_change_key_case, arginfo_array_change_key_case, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("array_unique", zif_array_unique, arginfo_array_unique, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("array_intersect_key", zif_array_intersect_key, arginfo_array_intersect_key, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("array_intersect_ukey", zif_array_intersect_ukey, arginfo_array_intersect_ukey, 0, NULL)
	ZEND_RAW_FENTRY("array_intersect", zif_array_intersect, arginfo_array_intersect, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("array_uintersect", zif_array_uintersect, arginfo_array_uintersect, 0, NULL)
	ZEND_RAW_FENTRY("array_intersect_assoc", zif_array_intersect_assoc, arginfo_array_intersect_assoc, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("array_uintersect_assoc", zif_array_uintersect_assoc, arginfo_array_uintersect_assoc, 0, NULL)
	ZEND_RAW_FENTRY("array_intersect_uassoc", zif_array_intersect_uassoc, arginfo_array_intersect_uassoc, 0, NULL)
	ZEND_RAW_FENTRY("array_uintersect_uassoc", zif_array_uintersect_uassoc, arginfo_array_uintersect_uassoc, 0, NULL)
	ZEND_RAW_FENTRY("array_diff_key", zif_array_diff_key, arginfo_array_diff_key, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("array_diff_ukey", zif_array_diff_ukey, arginfo_array_diff_ukey, 0, NULL)
	ZEND_RAW_FENTRY("array_diff", zif_array_diff, arginfo_array_diff, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("array_udiff", zif_array_udiff, arginfo_array_udiff, 0, NULL)
	ZEND_RAW_FENTRY("array_diff_assoc", zif_array_diff_assoc, arginfo_array_diff_assoc, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("array_diff_uassoc", zif_array_diff_uassoc, arginfo_array_diff_uassoc, 0, NULL)
	ZEND_RAW_FENTRY("array_udiff_assoc", zif_array_udiff_assoc, arginfo_array_udiff_assoc, 0, NULL)
	ZEND_RAW_FENTRY("array_udiff_uassoc", zif_array_udiff_uassoc, arginfo_array_udiff_uassoc, 0, NULL)
	ZEND_RAW_FENTRY("array_multisort", zif_array_multisort, arginfo_array_multisort, 0, NULL)
	ZEND_RAW_FENTRY("array_rand", zif_array_rand, arginfo_array_rand, 0, NULL)
	ZEND_RAW_FENTRY("array_sum", zif_array_sum, arginfo_array_sum, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("array_product", zif_array_product, arginfo_array_product, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("array_reduce", zif_array_reduce, arginfo_array_reduce, 0, NULL)
	ZEND_RAW_FENTRY("array_filter", zif_array_filter, arginfo_array_filter, 0, NULL)
	ZEND_RAW_FENTRY("array_map", zif_array_map, arginfo_array_map, 0, NULL)
	ZEND_RAW_FENTRY("array_key_exists", zif_array_key_exists, arginfo_array_key_exists, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("key_exists", zif_array_key_exists, arginfo_key_exists, 0, NULL)
	ZEND_RAW_FENTRY("array_chunk", zif_array_chunk, arginfo_array_chunk, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("array_combine", zif_array_combine, arginfo_array_combine, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("array_is_list", zif_array_is_list, arginfo_array_is_list, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("base64_encode", zif_base64_encode, arginfo_base64_encode, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("base64_decode", zif_base64_decode, arginfo_base64_decode, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("constant", zif_constant, arginfo_constant, 0, NULL)
	ZEND_RAW_FENTRY("ip2long", zif_ip2long, arginfo_ip2long, 0, NULL)
	ZEND_RAW_FENTRY("long2ip", zif_long2ip, arginfo_long2ip, 0, NULL)
	ZEND_RAW_FENTRY("getenv", zif_getenv, arginfo_getenv, 0, NULL)
#if defined(HAVE_PUTENV)
	ZEND_RAW_FENTRY("putenv", zif_putenv, arginfo_putenv, 0, NULL)
#endif
	ZEND_RAW_FENTRY("getopt", zif_getopt, arginfo_getopt, 0, NULL)
	ZEND_RAW_FENTRY("flush", zif_flush, arginfo_flush, 0, NULL)
	ZEND_RAW_FENTRY("sleep", zif_sleep, arginfo_sleep, 0, NULL)
	ZEND_RAW_FENTRY("usleep", zif_usleep, arginfo_usleep, 0, NULL)
#if defined(HAVE_NANOSLEEP)
	ZEND_RAW_FENTRY("time_nanosleep", zif_time_nanosleep, arginfo_time_nanosleep, 0, NULL)
#endif
#if defined(HAVE_NANOSLEEP)
	ZEND_RAW_FENTRY("time_sleep_until", zif_time_sleep_until, arginfo_time_sleep_until, 0, NULL)
#endif
	ZEND_RAW_FENTRY("get_current_user", zif_get_current_user, arginfo_get_current_user, 0, NULL)
	ZEND_RAW_FENTRY("get_cfg_var", zif_get_cfg_var, arginfo_get_cfg_var, 0, NULL)
	ZEND_RAW_FENTRY("error_log", zif_error_log, arginfo_error_log, 0, NULL)
	ZEND_RAW_FENTRY("error_get_last", zif_error_get_last, arginfo_error_get_last, 0, NULL)
	ZEND_RAW_FENTRY("error_clear_last", zif_error_clear_last, arginfo_error_clear_last, 0, NULL)
	ZEND_RAW_FENTRY("call_user_func", zif_call_user_func, arginfo_call_user_func, 0, NULL)
	ZEND_RAW_FENTRY("call_user_func_array", zif_call_user_func_array, arginfo_call_user_func_array, 0, NULL)
	ZEND_RAW_FENTRY("forward_static_call", zif_forward_static_call, arginfo_forward_static_call, 0, NULL)
	ZEND_RAW_FENTRY("forward_static_call_array", zif_forward_static_call_array, arginfo_forward_static_call_array, 0, NULL)
	ZEND_RAW_FENTRY("register_shutdown_function", zif_register_shutdown_function, arginfo_register_shutdown_function, 0, NULL)
	ZEND_RAW_FENTRY("highlight_file", zif_highlight_file, arginfo_highlight_file, 0, NULL)
	ZEND_RAW_FENTRY("show_source", zif_highlight_file, arginfo_show_source, 0, NULL)
	ZEND_RAW_FENTRY("php_strip_whitespace", zif_php_strip_whitespace, arginfo_php_strip_whitespace, 0, NULL)
	ZEND_RAW_FENTRY("highlight_string", zif_highlight_string, arginfo_highlight_string, 0, NULL)
	ZEND_RAW_FENTRY("ini_get", zif_ini_get, arginfo_ini_get, 0, NULL)
	ZEND_RAW_FENTRY("ini_get_all", zif_ini_get_all, arginfo_ini_get_all, 0, NULL)
	ZEND_RAW_FENTRY("ini_set", zif_ini_set, arginfo_ini_set, 0, NULL)
	ZEND_RAW_FENTRY("ini_alter", zif_ini_set, arginfo_ini_alter, 0, NULL)
	ZEND_RAW_FENTRY("ini_restore", zif_ini_restore, arginfo_ini_restore, 0, NULL)
	ZEND_RAW_FENTRY("ini_parse_quantity", zif_ini_parse_quantity, arginfo_ini_parse_quantity, 0, NULL)
	ZEND_RAW_FENTRY("set_include_path", zif_set_include_path, arginfo_set_include_path, 0, NULL)
	ZEND_RAW_FENTRY("get_include_path", zif_get_include_path, arginfo_get_include_path, 0, NULL)
	ZEND_RAW_FENTRY("print_r", zif_print_r, arginfo_print_r, 0, NULL)
	ZEND_RAW_FENTRY("connection_aborted", zif_connection_aborted, arginfo_connection_aborted, 0, NULL)
	ZEND_RAW_FENTRY("connection_status", zif_connection_status, arginfo_connection_status, 0, NULL)
	ZEND_RAW_FENTRY("ignore_user_abort", zif_ignore_user_abort, arginfo_ignore_user_abort, 0, NULL)
#if defined(HAVE_GETSERVBYNAME)
	ZEND_RAW_FENTRY("getservbyname", zif_getservbyname, arginfo_getservbyname, 0, NULL)
#endif
#if defined(HAVE_GETSERVBYPORT)
	ZEND_RAW_FENTRY("getservbyport", zif_getservbyport, arginfo_getservbyport, 0, NULL)
#endif
#if defined(HAVE_GETPROTOBYNAME)
	ZEND_RAW_FENTRY("getprotobyname", zif_getprotobyname, arginfo_getprotobyname, 0, NULL)
#endif
#if defined(HAVE_GETPROTOBYNUMBER)
	ZEND_RAW_FENTRY("getprotobynumber", zif_getprotobynumber, arginfo_getprotobynumber, 0, NULL)
#endif
	ZEND_RAW_FENTRY("register_tick_function", zif_register_tick_function, arginfo_register_tick_function, 0, NULL)
	ZEND_RAW_FENTRY("unregister_tick_function", zif_unregister_tick_function, arginfo_unregister_tick_function, 0, NULL)
	ZEND_RAW_FENTRY("is_uploaded_file", zif_is_uploaded_file, arginfo_is_uploaded_file, 0, NULL)
	ZEND_RAW_FENTRY("move_uploaded_file", zif_move_uploaded_file, arginfo_move_uploaded_file, 0, NULL)
	ZEND_RAW_FENTRY("parse_ini_file", zif_parse_ini_file, arginfo_parse_ini_file, 0, NULL)
	ZEND_RAW_FENTRY("parse_ini_string", zif_parse_ini_string, arginfo_parse_ini_string, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
#if ZEND_DEBUG
	ZEND_RAW_FENTRY("config_get_hash", zif_config_get_hash, arginfo_config_get_hash, 0, NULL)
#endif
#if defined(HAVE_GETLOADAVG)
	ZEND_RAW_FENTRY("sys_getloadavg", zif_sys_getloadavg, arginfo_sys_getloadavg, 0, NULL)
#endif
	ZEND_RAW_FENTRY("get_browser", zif_get_browser, arginfo_get_browser, 0, NULL)
	ZEND_RAW_FENTRY("crc32", zif_crc32, arginfo_crc32, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("crypt", zif_crypt, arginfo_crypt, 0, NULL)
#if defined(HAVE_STRPTIME)
	ZEND_RAW_FENTRY("strptime", zif_strptime, arginfo_strptime, ZEND_ACC_DEPRECATED, NULL)
#endif
#if defined(HAVE_GETHOSTNAME)
	ZEND_RAW_FENTRY("gethostname", zif_gethostname, arginfo_gethostname, 0, NULL)
#endif
	ZEND_RAW_FENTRY("gethostbyaddr", zif_gethostbyaddr, arginfo_gethostbyaddr, 0, NULL)
	ZEND_RAW_FENTRY("gethostbyname", zif_gethostbyname, arginfo_gethostbyname, 0, NULL)
	ZEND_RAW_FENTRY("gethostbynamel", zif_gethostbynamel, arginfo_gethostbynamel, 0, NULL)
#if (defined(PHP_WIN32) || defined(HAVE_DNS_SEARCH_FUNC))
	ZEND_RAW_FENTRY("dns_check_record", zif_dns_check_record, arginfo_dns_check_record, 0, NULL)
#endif
#if (defined(PHP_WIN32) || defined(HAVE_DNS_SEARCH_FUNC))
	ZEND_RAW_FENTRY("checkdnsrr", zif_dns_check_record, arginfo_checkdnsrr, 0, NULL)
#endif
#if (defined(PHP_WIN32) || defined(HAVE_DNS_SEARCH_FUNC))
	ZEND_RAW_FENTRY("dns_get_record", zif_dns_get_record, arginfo_dns_get_record, 0, NULL)
#endif
#if (defined(PHP_WIN32) || defined(HAVE_DNS_SEARCH_FUNC))
	ZEND_RAW_FENTRY("dns_get_mx", zif_dns_get_mx, arginfo_dns_get_mx, 0, NULL)
#endif
#if (defined(PHP_WIN32) || defined(HAVE_DNS_SEARCH_FUNC))
	ZEND_RAW_FENTRY("getmxrr", zif_dns_get_mx, arginfo_getmxrr, 0, NULL)
#endif
#if (defined(PHP_WIN32) || HAVE_GETIFADDRS || defined(__PASE__))
	ZEND_RAW_FENTRY("net_get_interfaces", zif_net_get_interfaces, arginfo_net_get_interfaces, 0, NULL)
#endif
#if defined(HAVE_FTOK)
	ZEND_RAW_FENTRY("ftok", zif_ftok, arginfo_ftok, 0, NULL)
#endif
	ZEND_RAW_FENTRY("hrtime", zif_hrtime, arginfo_hrtime, 0, NULL)
	ZEND_RAW_FENTRY("md5", zif_md5, arginfo_md5, 0, NULL)
	ZEND_RAW_FENTRY("md5_file", zif_md5_file, arginfo_md5_file, 0, NULL)
	ZEND_RAW_FENTRY("getmyuid", zif_getmyuid, arginfo_getmyuid, 0, NULL)
	ZEND_RAW_FENTRY("getmygid", zif_getmygid, arginfo_getmygid, 0, NULL)
	ZEND_RAW_FENTRY("getmypid", zif_getmypid, arginfo_getmypid, 0, NULL)
	ZEND_RAW_FENTRY("getmyinode", zif_getmyinode, arginfo_getmyinode, 0, NULL)
	ZEND_RAW_FENTRY("getlastmod", zif_getlastmod, arginfo_getlastmod, 0, NULL)
	ZEND_RAW_FENTRY("sha1", zif_sha1, arginfo_sha1, 0, NULL)
	ZEND_RAW_FENTRY("sha1_file", zif_sha1_file, arginfo_sha1_file, 0, NULL)
#if defined(HAVE_SYSLOG_H)
	ZEND_RAW_FENTRY("openlog", zif_openlog, arginfo_openlog, 0, NULL)
#endif
#if defined(HAVE_SYSLOG_H)
	ZEND_RAW_FENTRY("closelog", zif_closelog, arginfo_closelog, 0, NULL)
#endif
#if defined(HAVE_SYSLOG_H)
	ZEND_RAW_FENTRY("syslog", zif_syslog, arginfo_syslog, 0, NULL)
#endif
	ZEND_RAW_FENTRY("inet_ntop", zif_inet_ntop, arginfo_inet_ntop, 0, NULL)
	ZEND_RAW_FENTRY("inet_pton", zif_inet_pton, arginfo_inet_pton, 0, NULL)
	ZEND_RAW_FENTRY("metaphone", zif_metaphone, arginfo_metaphone, 0, NULL)
	ZEND_RAW_FENTRY("header", zif_header, arginfo_header, 0, NULL)
	ZEND_RAW_FENTRY("header_remove", zif_header_remove, arginfo_header_remove, 0, NULL)
	ZEND_RAW_FENTRY("setrawcookie", zif_setrawcookie, arginfo_setrawcookie, 0, NULL)
	ZEND_RAW_FENTRY("setcookie", zif_setcookie, arginfo_setcookie, 0, NULL)
	ZEND_RAW_FENTRY("http_response_code", zif_http_response_code, arginfo_http_response_code, 0, NULL)
	ZEND_RAW_FENTRY("headers_sent", zif_headers_sent, arginfo_headers_sent, 0, NULL)
	ZEND_RAW_FENTRY("headers_list", zif_headers_list, arginfo_headers_list, 0, NULL)
	ZEND_RAW_FENTRY("htmlspecialchars", zif_htmlspecialchars, arginfo_htmlspecialchars, 0, NULL)
	ZEND_RAW_FENTRY("htmlspecialchars_decode", zif_htmlspecialchars_decode, arginfo_htmlspecialchars_decode, 0, NULL)
	ZEND_RAW_FENTRY("html_entity_decode", zif_html_entity_decode, arginfo_html_entity_decode, 0, NULL)
	ZEND_RAW_FENTRY("htmlentities", zif_htmlentities, arginfo_htmlentities, 0, NULL)
	ZEND_RAW_FENTRY("get_html_translation_table", zif_get_html_translation_table, arginfo_get_html_translation_table, 0, NULL)
	ZEND_RAW_FENTRY("assert", zif_assert, arginfo_assert, 0, NULL)
	ZEND_RAW_FENTRY("assert_options", zif_assert_options, arginfo_assert_options, ZEND_ACC_DEPRECATED, NULL)
	ZEND_RAW_FENTRY("bin2hex", zif_bin2hex, arginfo_bin2hex, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("hex2bin", zif_hex2bin, arginfo_hex2bin, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("strspn", zif_strspn, arginfo_strspn, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("strcspn", zif_strcspn, arginfo_strcspn, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
#if defined(HAVE_NL_LANGINFO)
	ZEND_RAW_FENTRY("nl_langinfo", zif_nl_langinfo, arginfo_nl_langinfo, 0, NULL)
#endif
	ZEND_RAW_FENTRY("strcoll", zif_strcoll, arginfo_strcoll, 0, NULL)
	ZEND_FRAMELESS_FE(trim, arginfo_trim, ZEND_ACC_COMPILE_TIME_EVAL, frameless_function_infos_trim)
	ZEND_RAW_FENTRY("rtrim", zif_rtrim, arginfo_rtrim, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("chop", zif_rtrim, arginfo_chop, 0, NULL)
	ZEND_RAW_FENTRY("ltrim", zif_ltrim, arginfo_ltrim, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("wordwrap", zif_wordwrap, arginfo_wordwrap, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("explode", zif_explode, arginfo_explode, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_FRAMELESS_FE(implode, arginfo_implode, ZEND_ACC_COMPILE_TIME_EVAL, frameless_function_infos_implode)
	ZEND_RAW_FENTRY("join", zif_implode, arginfo_join, 0, NULL)
	ZEND_RAW_FENTRY("strtok", zif_strtok, arginfo_strtok, 0, NULL)
	ZEND_RAW_FENTRY("strtoupper", zif_strtoupper, arginfo_strtoupper, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("strtolower", zif_strtolower, arginfo_strtolower, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("str_increment", zif_str_increment, arginfo_str_increment, 0, NULL)
	ZEND_RAW_FENTRY("str_decrement", zif_str_decrement, arginfo_str_decrement, 0, NULL)
	ZEND_RAW_FENTRY("basename", zif_basename, arginfo_basename, 0, NULL)
	ZEND_FRAMELESS_FE(dirname, arginfo_dirname, 0, frameless_function_infos_dirname)
	ZEND_RAW_FENTRY("pathinfo", zif_pathinfo, arginfo_pathinfo, 0, NULL)
	ZEND_RAW_FENTRY("stristr", zif_stristr, arginfo_stristr, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_FRAMELESS_FE(strstr, arginfo_strstr, ZEND_ACC_COMPILE_TIME_EVAL, frameless_function_infos_strstr)
	ZEND_RAW_FENTRY("strchr", zif_strstr, arginfo_strchr, 0, NULL)
	ZEND_FRAMELESS_FE(strpos, arginfo_strpos, ZEND_ACC_COMPILE_TIME_EVAL, frameless_function_infos_strpos)
	ZEND_RAW_FENTRY("stripos", zif_stripos, arginfo_stripos, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("strrpos", zif_strrpos, arginfo_strrpos, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("strripos", zif_strripos, arginfo_strripos, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("strrchr", zif_strrchr, arginfo_strrchr, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_FRAMELESS_FE(str_contains, arginfo_str_contains, ZEND_ACC_COMPILE_TIME_EVAL, frameless_function_infos_str_contains)
	ZEND_FRAMELESS_FE(str_starts_with, arginfo_str_starts_with, ZEND_ACC_COMPILE_TIME_EVAL, frameless_function_infos_str_starts_with)
	ZEND_RAW_FENTRY("str_ends_with", zif_str_ends_with, arginfo_str_ends_with, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("chunk_split", zif_chunk_split, arginfo_chunk_split, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_FRAMELESS_FE(substr, arginfo_substr, ZEND_ACC_COMPILE_TIME_EVAL, frameless_function_infos_substr)
	ZEND_RAW_FENTRY("substr_replace", zif_substr_replace, arginfo_substr_replace, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("quotemeta", zif_quotemeta, arginfo_quotemeta, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("ord", zif_ord, arginfo_ord, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("chr", zif_chr, arginfo_chr, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("ucfirst", zif_ucfirst, arginfo_ucfirst, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("lcfirst", zif_lcfirst, arginfo_lcfirst, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("ucwords", zif_ucwords, arginfo_ucwords, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_FRAMELESS_FE(strtr, arginfo_strtr, ZEND_ACC_COMPILE_TIME_EVAL, frameless_function_infos_strtr)
	ZEND_RAW_FENTRY("strrev", zif_strrev, arginfo_strrev, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("similar_text", zif_similar_text, arginfo_similar_text, 0, NULL)
	ZEND_RAW_FENTRY("addcslashes", zif_addcslashes, arginfo_addcslashes, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("addslashes", zif_addslashes, arginfo_addslashes, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("stripcslashes", zif_stripcslashes, arginfo_stripcslashes, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("stripslashes", zif_stripslashes, arginfo_stripslashes, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_FRAMELESS_FE(str_replace, arginfo_str_replace, ZEND_ACC_COMPILE_TIME_EVAL, frameless_function_infos_str_replace)
	ZEND_RAW_FENTRY("str_ireplace", zif_str_ireplace, arginfo_str_ireplace, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("hebrev", zif_hebrev, arginfo_hebrev, 0, NULL)
	ZEND_RAW_FENTRY("nl2br", zif_nl2br, arginfo_nl2br, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("strip_tags", zif_strip_tags, arginfo_strip_tags, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("setlocale", zif_setlocale, arginfo_setlocale, 0, NULL)
	ZEND_RAW_FENTRY("parse_str", zif_parse_str, arginfo_parse_str, 0, NULL)
	ZEND_RAW_FENTRY("str_getcsv", zif_str_getcsv, arginfo_str_getcsv, 0, NULL)
	ZEND_RAW_FENTRY("str_repeat", zif_str_repeat, arginfo_str_repeat, 0, NULL)
	ZEND_RAW_FENTRY("count_chars", zif_count_chars, arginfo_count_chars, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("strnatcmp", zif_strnatcmp, arginfo_strnatcmp, 0, NULL)
	ZEND_RAW_FENTRY("localeconv", zif_localeconv, arginfo_localeconv, 0, NULL)
	ZEND_RAW_FENTRY("strnatcasecmp", zif_strnatcasecmp, arginfo_strnatcasecmp, 0, NULL)
	ZEND_RAW_FENTRY("substr_count", zif_substr_count, arginfo_substr_count, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("str_pad", zif_str_pad, arginfo_str_pad, 0, NULL)
	ZEND_RAW_FENTRY("sscanf", zif_sscanf, arginfo_sscanf, 0, NULL)
	ZEND_RAW_FENTRY("str_rot13", zif_str_rot13, arginfo_str_rot13, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("str_shuffle", zif_str_shuffle, arginfo_str_shuffle, 0, NULL)
	ZEND_RAW_FENTRY("str_word_count", zif_str_word_count, arginfo_str_word_count, 0, NULL)
	ZEND_RAW_FENTRY("str_split", zif_str_split, arginfo_str_split, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("strpbrk", zif_strpbrk, arginfo_strpbrk, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("substr_compare", zif_substr_compare, arginfo_substr_compare, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("utf8_encode", zif_utf8_encode, arginfo_utf8_encode, ZEND_ACC_DEPRECATED|ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("utf8_decode", zif_utf8_decode, arginfo_utf8_decode, ZEND_ACC_DEPRECATED|ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("opendir", zif_opendir, arginfo_opendir, 0, NULL)
	ZEND_RAW_FENTRY("dir", zif_dir, arginfo_dir, 0, NULL)
	ZEND_RAW_FENTRY("closedir", zif_closedir, arginfo_closedir, 0, NULL)
	ZEND_RAW_FENTRY("chdir", zif_chdir, arginfo_chdir, 0, NULL)
#if (defined(HAVE_CHROOT) && !defined(ZTS) && defined(ENABLE_CHROOT_FUNC))
	ZEND_RAW_FENTRY("chroot", zif_chroot, arginfo_chroot, 0, NULL)
#endif
	ZEND_RAW_FENTRY("getcwd", zif_getcwd, arginfo_getcwd, 0, NULL)
	ZEND_RAW_FENTRY("rewinddir", zif_rewinddir, arginfo_rewinddir, 0, NULL)
	ZEND_RAW_FENTRY("readdir", zif_readdir, arginfo_readdir, 0, NULL)
	ZEND_RAW_FENTRY("scandir", zif_scandir, arginfo_scandir, 0, NULL)
#if defined(HAVE_GLOB)
	ZEND_RAW_FENTRY("glob", zif_glob, arginfo_glob, 0, NULL)
#endif
	ZEND_RAW_FENTRY("exec", zif_exec, arginfo_exec, 0, NULL)
	ZEND_RAW_FENTRY("system", zif_system, arginfo_system, 0, NULL)
	ZEND_RAW_FENTRY("passthru", zif_passthru, arginfo_passthru, 0, NULL)
	ZEND_RAW_FENTRY("escapeshellcmd", zif_escapeshellcmd, arginfo_escapeshellcmd, 0, NULL)
	ZEND_RAW_FENTRY("escapeshellarg", zif_escapeshellarg, arginfo_escapeshellarg, 0, NULL)
	ZEND_RAW_FENTRY("shell_exec", zif_shell_exec, arginfo_shell_exec, 0, NULL)
#if defined(HAVE_NICE)
	ZEND_RAW_FENTRY("proc_nice", zif_proc_nice, arginfo_proc_nice, 0, NULL)
#endif
	ZEND_RAW_FENTRY("flock", zif_flock, arginfo_flock, 0, NULL)
	ZEND_RAW_FENTRY("get_meta_tags", zif_get_meta_tags, arginfo_get_meta_tags, 0, NULL)
	ZEND_RAW_FENTRY("pclose", zif_pclose, arginfo_pclose, 0, NULL)
	ZEND_RAW_FENTRY("popen", zif_popen, arginfo_popen, 0, NULL)
	ZEND_RAW_FENTRY("readfile", zif_readfile, arginfo_readfile, 0, NULL)
	ZEND_RAW_FENTRY("rewind", zif_rewind, arginfo_rewind, 0, NULL)
	ZEND_RAW_FENTRY("rmdir", zif_rmdir, arginfo_rmdir, 0, NULL)
	ZEND_RAW_FENTRY("umask", zif_umask, arginfo_umask, 0, NULL)
	ZEND_RAW_FENTRY("fclose", zif_fclose, arginfo_fclose, 0, NULL)
	ZEND_RAW_FENTRY("feof", zif_feof, arginfo_feof, 0, NULL)
	ZEND_RAW_FENTRY("fgetc", zif_fgetc, arginfo_fgetc, 0, NULL)
	ZEND_RAW_FENTRY("fgets", zif_fgets, arginfo_fgets, 0, NULL)
	ZEND_RAW_FENTRY("fread", zif_fread, arginfo_fread, 0, NULL)
	ZEND_RAW_FENTRY("fopen", zif_fopen, arginfo_fopen, 0, NULL)
	ZEND_RAW_FENTRY("fscanf", zif_fscanf, arginfo_fscanf, 0, NULL)
	ZEND_RAW_FENTRY("fpassthru", zif_fpassthru, arginfo_fpassthru, 0, NULL)
	ZEND_RAW_FENTRY("ftruncate", zif_ftruncate, arginfo_ftruncate, 0, NULL)
	ZEND_RAW_FENTRY("fstat", zif_fstat, arginfo_fstat, 0, NULL)
	ZEND_RAW_FENTRY("fseek", zif_fseek, arginfo_fseek, 0, NULL)
	ZEND_RAW_FENTRY("ftell", zif_ftell, arginfo_ftell, 0, NULL)
	ZEND_RAW_FENTRY("fflush", zif_fflush, arginfo_fflush, 0, NULL)
	ZEND_RAW_FENTRY("fsync", zif_fsync, arginfo_fsync, 0, NULL)
	ZEND_RAW_FENTRY("fdatasync", zif_fdatasync, arginfo_fdatasync, 0, NULL)
	ZEND_RAW_FENTRY("fwrite", zif_fwrite, arginfo_fwrite, 0, NULL)
	ZEND_RAW_FENTRY("fputs", zif_fwrite, arginfo_fputs, 0, NULL)
	ZEND_RAW_FENTRY("mkdir", zif_mkdir, arginfo_mkdir, 0, NULL)
	ZEND_RAW_FENTRY("rename", zif_rename, arginfo_rename, 0, NULL)
	ZEND_RAW_FENTRY("copy", zif_copy, arginfo_copy, 0, NULL)
	ZEND_RAW_FENTRY("tempnam", zif_tempnam, arginfo_tempnam, 0, NULL)
	ZEND_RAW_FENTRY("tmpfile", zif_tmpfile, arginfo_tmpfile, 0, NULL)
	ZEND_RAW_FENTRY("file", zif_file, arginfo_file, 0, NULL)
	ZEND_RAW_FENTRY("file_get_contents", zif_file_get_contents, arginfo_file_get_contents, 0, NULL)
	ZEND_RAW_FENTRY("unlink", zif_unlink, arginfo_unlink, 0, NULL)
	ZEND_RAW_FENTRY("file_put_contents", zif_file_put_contents, arginfo_file_put_contents, 0, NULL)
	ZEND_RAW_FENTRY("fputcsv", zif_fputcsv, arginfo_fputcsv, 0, NULL)
	ZEND_RAW_FENTRY("fgetcsv", zif_fgetcsv, arginfo_fgetcsv, 0, NULL)
	ZEND_RAW_FENTRY("realpath", zif_realpath, arginfo_realpath, 0, NULL)
#if defined(HAVE_FNMATCH)
	ZEND_RAW_FENTRY("fnmatch", zif_fnmatch, arginfo_fnmatch, 0, NULL)
#endif
	ZEND_RAW_FENTRY("sys_get_temp_dir", zif_sys_get_temp_dir, arginfo_sys_get_temp_dir, 0, NULL)
	ZEND_RAW_FENTRY("fileatime", zif_fileatime, arginfo_fileatime, 0, NULL)
	ZEND_RAW_FENTRY("filectime", zif_filectime, arginfo_filectime, 0, NULL)
	ZEND_RAW_FENTRY("filegroup", zif_filegroup, arginfo_filegroup, 0, NULL)
	ZEND_RAW_FENTRY("fileinode", zif_fileinode, arginfo_fileinode, 0, NULL)
	ZEND_RAW_FENTRY("filemtime", zif_filemtime, arginfo_filemtime, 0, NULL)
	ZEND_RAW_FENTRY("fileowner", zif_fileowner, arginfo_fileowner, 0, NULL)
	ZEND_RAW_FENTRY("fileperms", zif_fileperms, arginfo_fileperms, 0, NULL)
	ZEND_RAW_FENTRY("filesize", zif_filesize, arginfo_filesize, 0, NULL)
	ZEND_RAW_FENTRY("filetype", zif_filetype, arginfo_filetype, 0, NULL)
	ZEND_RAW_FENTRY("file_exists", zif_file_exists, arginfo_file_exists, 0, NULL)
	ZEND_RAW_FENTRY("is_writable", zif_is_writable, arginfo_is_writable, 0, NULL)
	ZEND_RAW_FENTRY("is_writeable", zif_is_writable, arginfo_is_writeable, 0, NULL)
	ZEND_RAW_FENTRY("is_readable", zif_is_readable, arginfo_is_readable, 0, NULL)
	ZEND_RAW_FENTRY("is_executable", zif_is_executable, arginfo_is_executable, 0, NULL)
	ZEND_RAW_FENTRY("is_file", zif_is_file, arginfo_is_file, 0, NULL)
	ZEND_RAW_FENTRY("is_dir", zif_is_dir, arginfo_is_dir, 0, NULL)
	ZEND_RAW_FENTRY("is_link", zif_is_link, arginfo_is_link, 0, NULL)
	ZEND_RAW_FENTRY("stat", zif_stat, arginfo_stat, 0, NULL)
	ZEND_RAW_FENTRY("lstat", zif_lstat, arginfo_lstat, 0, NULL)
	ZEND_RAW_FENTRY("chown", zif_chown, arginfo_chown, 0, NULL)
	ZEND_RAW_FENTRY("chgrp", zif_chgrp, arginfo_chgrp, 0, NULL)
#if defined(HAVE_LCHOWN)
	ZEND_RAW_FENTRY("lchown", zif_lchown, arginfo_lchown, 0, NULL)
#endif
#if defined(HAVE_LCHOWN)
	ZEND_RAW_FENTRY("lchgrp", zif_lchgrp, arginfo_lchgrp, 0, NULL)
#endif
	ZEND_RAW_FENTRY("chmod", zif_chmod, arginfo_chmod, 0, NULL)
#if defined(HAVE_UTIME)
	ZEND_RAW_FENTRY("touch", zif_touch, arginfo_touch, 0, NULL)
#endif
	ZEND_RAW_FENTRY("clearstatcache", zif_clearstatcache, arginfo_clearstatcache, 0, NULL)
	ZEND_RAW_FENTRY("disk_total_space", zif_disk_total_space, arginfo_disk_total_space, 0, NULL)
	ZEND_RAW_FENTRY("disk_free_space", zif_disk_free_space, arginfo_disk_free_space, 0, NULL)
	ZEND_RAW_FENTRY("diskfreespace", zif_disk_free_space, arginfo_diskfreespace, 0, NULL)
	ZEND_RAW_FENTRY("realpath_cache_get", zif_realpath_cache_get, arginfo_realpath_cache_get, 0, NULL)
	ZEND_RAW_FENTRY("realpath_cache_size", zif_realpath_cache_size, arginfo_realpath_cache_size, 0, NULL)
	ZEND_RAW_FENTRY("sprintf", zif_sprintf, arginfo_sprintf, 0, NULL)
	ZEND_RAW_FENTRY("printf", zif_printf, arginfo_printf, 0, NULL)
	ZEND_RAW_FENTRY("vprintf", zif_vprintf, arginfo_vprintf, 0, NULL)
	ZEND_RAW_FENTRY("vsprintf", zif_vsprintf, arginfo_vsprintf, 0, NULL)
	ZEND_RAW_FENTRY("fprintf", zif_fprintf, arginfo_fprintf, 0, NULL)
	ZEND_RAW_FENTRY("vfprintf", zif_vfprintf, arginfo_vfprintf, 0, NULL)
	ZEND_RAW_FENTRY("fsockopen", zif_fsockopen, arginfo_fsockopen, 0, NULL)
	ZEND_RAW_FENTRY("pfsockopen", zif_pfsockopen, arginfo_pfsockopen, 0, NULL)
	ZEND_RAW_FENTRY("http_build_query", zif_http_build_query, arginfo_http_build_query, 0, NULL)
	ZEND_RAW_FENTRY("request_parse_body", zif_request_parse_body, arginfo_request_parse_body, 0, NULL)
	ZEND_RAW_FENTRY("image_type_to_mime_type", zif_image_type_to_mime_type, arginfo_image_type_to_mime_type, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("image_type_to_extension", zif_image_type_to_extension, arginfo_image_type_to_extension, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("getimagesize", zif_getimagesize, arginfo_getimagesize, 0, NULL)
	ZEND_RAW_FENTRY("getimagesizefromstring", zif_getimagesizefromstring, arginfo_getimagesizefromstring, 0, NULL)
	ZEND_RAW_FENTRY("phpinfo", zif_phpinfo, arginfo_phpinfo, 0, NULL)
	ZEND_RAW_FENTRY("phpversion", zif_phpversion, arginfo_phpversion, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("phpcredits", zif_phpcredits, arginfo_phpcredits, 0, NULL)
	ZEND_RAW_FENTRY("php_sapi_name", zif_php_sapi_name, arginfo_php_sapi_name, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("php_uname", zif_php_uname, arginfo_php_uname, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("php_ini_scanned_files", zif_php_ini_scanned_files, arginfo_php_ini_scanned_files, 0, NULL)
	ZEND_RAW_FENTRY("php_ini_loaded_file", zif_php_ini_loaded_file, arginfo_php_ini_loaded_file, 0, NULL)
	ZEND_RAW_FENTRY("iptcembed", zif_iptcembed, arginfo_iptcembed, 0, NULL)
	ZEND_RAW_FENTRY("iptcparse", zif_iptcparse, arginfo_iptcparse, 0, NULL)
	ZEND_RAW_FENTRY("levenshtein", zif_levenshtein, arginfo_levenshtein, 0, NULL)
#if (defined(HAVE_SYMLINK) || defined(PHP_WIN32))
	ZEND_RAW_FENTRY("readlink", zif_readlink, arginfo_readlink, 0, NULL)
#endif
#if (defined(HAVE_SYMLINK) || defined(PHP_WIN32))
	ZEND_RAW_FENTRY("linkinfo", zif_linkinfo, arginfo_linkinfo, 0, NULL)
#endif
#if (defined(HAVE_SYMLINK) || defined(PHP_WIN32))
	ZEND_RAW_FENTRY("symlink", zif_symlink, arginfo_symlink, 0, NULL)
#endif
#if (defined(HAVE_SYMLINK) || defined(PHP_WIN32))
	ZEND_RAW_FENTRY("link", zif_link, arginfo_link, 0, NULL)
#endif
	ZEND_RAW_FENTRY("mail", zif_mail, arginfo_mail, 0, NULL)
	ZEND_RAW_FENTRY("abs", zif_abs, arginfo_abs, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("ceil", zif_ceil, arginfo_ceil, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("floor", zif_floor, arginfo_floor, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("round", zif_round, arginfo_round, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("sin", zif_sin, arginfo_sin, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("cos", zif_cos, arginfo_cos, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("tan", zif_tan, arginfo_tan, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("asin", zif_asin, arginfo_asin, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("acos", zif_acos, arginfo_acos, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("atan", zif_atan, arginfo_atan, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("atanh", zif_atanh, arginfo_atanh, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("atan2", zif_atan2, arginfo_atan2, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("sinh", zif_sinh, arginfo_sinh, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("cosh", zif_cosh, arginfo_cosh, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("tanh", zif_tanh, arginfo_tanh, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("asinh", zif_asinh, arginfo_asinh, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("acosh", zif_acosh, arginfo_acosh, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("expm1", zif_expm1, arginfo_expm1, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("log1p", zif_log1p, arginfo_log1p, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("pi", zif_pi, arginfo_pi, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("is_finite", zif_is_finite, arginfo_is_finite, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("is_nan", zif_is_nan, arginfo_is_nan, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("intdiv", zif_intdiv, arginfo_intdiv, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("is_infinite", zif_is_infinite, arginfo_is_infinite, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("pow", zif_pow, arginfo_pow, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("exp", zif_exp, arginfo_exp, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("log", zif_log, arginfo_log, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("log10", zif_log10, arginfo_log10, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("sqrt", zif_sqrt, arginfo_sqrt, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("hypot", zif_hypot, arginfo_hypot, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("deg2rad", zif_deg2rad, arginfo_deg2rad, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("rad2deg", zif_rad2deg, arginfo_rad2deg, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("bindec", zif_bindec, arginfo_bindec, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("hexdec", zif_hexdec, arginfo_hexdec, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("octdec", zif_octdec, arginfo_octdec, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("decbin", zif_decbin, arginfo_decbin, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("decoct", zif_decoct, arginfo_decoct, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_FRAMELESS_FE(dechex, arginfo_dechex, ZEND_ACC_COMPILE_TIME_EVAL, frameless_function_infos_dechex)
	ZEND_RAW_FENTRY("base_convert", zif_base_convert, arginfo_base_convert, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("number_format", zif_number_format, arginfo_number_format, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("fmod", zif_fmod, arginfo_fmod, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("fdiv", zif_fdiv, arginfo_fdiv, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
#if defined(HAVE_GETTIMEOFDAY)
	ZEND_RAW_FENTRY("microtime", zif_microtime, arginfo_microtime, 0, NULL)
#endif
#if defined(HAVE_GETTIMEOFDAY)
	ZEND_RAW_FENTRY("gettimeofday", zif_gettimeofday, arginfo_gettimeofday, 0, NULL)
#endif
#if defined(HAVE_GETRUSAGE)
	ZEND_RAW_FENTRY("getrusage", zif_getrusage, arginfo_getrusage, 0, NULL)
#endif
	ZEND_RAW_FENTRY("pack", zif_pack, arginfo_pack, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("unpack", zif_unpack, arginfo_unpack, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("password_get_info", zif_password_get_info, arginfo_password_get_info, 0, NULL)
	ZEND_RAW_FENTRY("password_hash", zif_password_hash, arginfo_password_hash, 0, NULL)
	ZEND_RAW_FENTRY("password_needs_rehash", zif_password_needs_rehash, arginfo_password_needs_rehash, 0, NULL)
	ZEND_RAW_FENTRY("password_verify", zif_password_verify, arginfo_password_verify, 0, NULL)
	ZEND_RAW_FENTRY("password_algos", zif_password_algos, arginfo_password_algos, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
#if defined(PHP_CAN_SUPPORT_PROC_OPEN)
	ZEND_RAW_FENTRY("proc_open", zif_proc_open, arginfo_proc_open, 0, NULL)
#endif
#if defined(PHP_CAN_SUPPORT_PROC_OPEN)
	ZEND_RAW_FENTRY("proc_close", zif_proc_close, arginfo_proc_close, 0, NULL)
#endif
#if defined(PHP_CAN_SUPPORT_PROC_OPEN)
	ZEND_RAW_FENTRY("proc_terminate", zif_proc_terminate, arginfo_proc_terminate, 0, NULL)
#endif
#if defined(PHP_CAN_SUPPORT_PROC_OPEN)
	ZEND_RAW_FENTRY("proc_get_status", zif_proc_get_status, arginfo_proc_get_status, 0, NULL)
#endif
	ZEND_RAW_FENTRY("quoted_printable_decode", zif_quoted_printable_decode, arginfo_quoted_printable_decode, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("quoted_printable_encode", zif_quoted_printable_encode, arginfo_quoted_printable_encode, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("soundex", zif_soundex, arginfo_soundex, 0, NULL)
	ZEND_RAW_FENTRY("stream_select", zif_stream_select, arginfo_stream_select, 0, NULL)
	ZEND_RAW_FENTRY("stream_context_create", zif_stream_context_create, arginfo_stream_context_create, 0, NULL)
	ZEND_RAW_FENTRY("stream_context_set_params", zif_stream_context_set_params, arginfo_stream_context_set_params, 0, NULL)
	ZEND_RAW_FENTRY("stream_context_get_params", zif_stream_context_get_params, arginfo_stream_context_get_params, 0, NULL)
	ZEND_RAW_FENTRY("stream_context_set_option", zif_stream_context_set_option, arginfo_stream_context_set_option, 0, NULL)
	ZEND_RAW_FENTRY("stream_context_set_options", zif_stream_context_set_options, arginfo_stream_context_set_options, 0, NULL)
	ZEND_RAW_FENTRY("stream_context_get_options", zif_stream_context_get_options, arginfo_stream_context_get_options, 0, NULL)
	ZEND_RAW_FENTRY("stream_context_get_default", zif_stream_context_get_default, arginfo_stream_context_get_default, 0, NULL)
	ZEND_RAW_FENTRY("stream_context_set_default", zif_stream_context_set_default, arginfo_stream_context_set_default, 0, NULL)
	ZEND_RAW_FENTRY("stream_filter_prepend", zif_stream_filter_prepend, arginfo_stream_filter_prepend, 0, NULL)
	ZEND_RAW_FENTRY("stream_filter_append", zif_stream_filter_append, arginfo_stream_filter_append, 0, NULL)
	ZEND_RAW_FENTRY("stream_filter_remove", zif_stream_filter_remove, arginfo_stream_filter_remove, 0, NULL)
	ZEND_RAW_FENTRY("stream_socket_client", zif_stream_socket_client, arginfo_stream_socket_client, 0, NULL)
	ZEND_RAW_FENTRY("stream_socket_server", zif_stream_socket_server, arginfo_stream_socket_server, 0, NULL)
	ZEND_RAW_FENTRY("stream_socket_accept", zif_stream_socket_accept, arginfo_stream_socket_accept, 0, NULL)
	ZEND_RAW_FENTRY("stream_socket_get_name", zif_stream_socket_get_name, arginfo_stream_socket_get_name, 0, NULL)
	ZEND_RAW_FENTRY("stream_socket_recvfrom", zif_stream_socket_recvfrom, arginfo_stream_socket_recvfrom, 0, NULL)
	ZEND_RAW_FENTRY("stream_socket_sendto", zif_stream_socket_sendto, arginfo_stream_socket_sendto, 0, NULL)
	ZEND_RAW_FENTRY("stream_socket_enable_crypto", zif_stream_socket_enable_crypto, arginfo_stream_socket_enable_crypto, 0, NULL)
#if defined(HAVE_SHUTDOWN)
	ZEND_RAW_FENTRY("stream_socket_shutdown", zif_stream_socket_shutdown, arginfo_stream_socket_shutdown, 0, NULL)
#endif
#if defined(HAVE_SOCKETPAIR)
	ZEND_RAW_FENTRY("stream_socket_pair", zif_stream_socket_pair, arginfo_stream_socket_pair, 0, NULL)
#endif
	ZEND_RAW_FENTRY("stream_copy_to_stream", zif_stream_copy_to_stream, arginfo_stream_copy_to_stream, 0, NULL)
	ZEND_RAW_FENTRY("stream_get_contents", zif_stream_get_contents, arginfo_stream_get_contents, 0, NULL)
	ZEND_RAW_FENTRY("stream_supports_lock", zif_stream_supports_lock, arginfo_stream_supports_lock, 0, NULL)
	ZEND_RAW_FENTRY("stream_set_write_buffer", zif_stream_set_write_buffer, arginfo_stream_set_write_buffer, 0, NULL)
	ZEND_RAW_FENTRY("set_file_buffer", zif_stream_set_write_buffer, arginfo_set_file_buffer, 0, NULL)
	ZEND_RAW_FENTRY("stream_set_read_buffer", zif_stream_set_read_buffer, arginfo_stream_set_read_buffer, 0, NULL)
	ZEND_RAW_FENTRY("stream_set_blocking", zif_stream_set_blocking, arginfo_stream_set_blocking, 0, NULL)
	ZEND_RAW_FENTRY("socket_set_blocking", zif_stream_set_blocking, arginfo_socket_set_blocking, 0, NULL)
	ZEND_RAW_FENTRY("stream_get_meta_data", zif_stream_get_meta_data, arginfo_stream_get_meta_data, 0, NULL)
	ZEND_RAW_FENTRY("socket_get_status", zif_stream_get_meta_data, arginfo_socket_get_status, 0, NULL)
	ZEND_RAW_FENTRY("stream_get_line", zif_stream_get_line, arginfo_stream_get_line, 0, NULL)
	ZEND_RAW_FENTRY("stream_resolve_include_path", zif_stream_resolve_include_path, arginfo_stream_resolve_include_path, 0, NULL)
	ZEND_RAW_FENTRY("stream_get_wrappers", zif_stream_get_wrappers, arginfo_stream_get_wrappers, 0, NULL)
	ZEND_RAW_FENTRY("stream_get_transports", zif_stream_get_transports, arginfo_stream_get_transports, 0, NULL)
	ZEND_RAW_FENTRY("stream_is_local", zif_stream_is_local, arginfo_stream_is_local, 0, NULL)
	ZEND_RAW_FENTRY("stream_isatty", zif_stream_isatty, arginfo_stream_isatty, 0, NULL)
#if defined(PHP_WIN32)
	ZEND_RAW_FENTRY("sapi_windows_vt100_support", zif_sapi_windows_vt100_support, arginfo_sapi_windows_vt100_support, 0, NULL)
#endif
	ZEND_RAW_FENTRY("stream_set_chunk_size", zif_stream_set_chunk_size, arginfo_stream_set_chunk_size, 0, NULL)
#if (defined(HAVE_SYS_TIME_H) || defined(PHP_WIN32))
	ZEND_RAW_FENTRY("stream_set_timeout", zif_stream_set_timeout, arginfo_stream_set_timeout, 0, NULL)
#endif
#if (defined(HAVE_SYS_TIME_H) || defined(PHP_WIN32))
	ZEND_RAW_FENTRY("socket_set_timeout", zif_stream_set_timeout, arginfo_socket_set_timeout, 0, NULL)
#endif
	ZEND_RAW_FENTRY("gettype", zif_gettype, arginfo_gettype, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("get_debug_type", zif_get_debug_type, arginfo_get_debug_type, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("settype", zif_settype, arginfo_settype, 0, NULL)
	ZEND_RAW_FENTRY("intval", zif_intval, arginfo_intval, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("floatval", zif_floatval, arginfo_floatval, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("doubleval", zif_floatval, arginfo_doubleval, 0, NULL)
	ZEND_RAW_FENTRY("boolval", zif_boolval, arginfo_boolval, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("strval", zif_strval, arginfo_strval, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("is_null", zif_is_null, arginfo_is_null, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("is_resource", zif_is_resource, arginfo_is_resource, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("is_bool", zif_is_bool, arginfo_is_bool, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("is_int", zif_is_int, arginfo_is_int, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("is_integer", zif_is_int, arginfo_is_integer, 0, NULL)
	ZEND_RAW_FENTRY("is_long", zif_is_int, arginfo_is_long, 0, NULL)
	ZEND_RAW_FENTRY("is_float", zif_is_float, arginfo_is_float, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("is_double", zif_is_float, arginfo_is_double, 0, NULL)
	ZEND_FRAMELESS_FE(is_numeric, arginfo_is_numeric, ZEND_ACC_COMPILE_TIME_EVAL, frameless_function_infos_is_numeric)
	ZEND_RAW_FENTRY("is_string", zif_is_string, arginfo_is_string, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("is_array", zif_is_array, arginfo_is_array, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("is_object", zif_is_object, arginfo_is_object, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("is_scalar", zif_is_scalar, arginfo_is_scalar, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("is_callable", zif_is_callable, arginfo_is_callable, 0, NULL)
	ZEND_RAW_FENTRY("is_iterable", zif_is_iterable, arginfo_is_iterable, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("is_countable", zif_is_countable, arginfo_is_countable, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
#if defined(HAVE_GETTIMEOFDAY)
	ZEND_RAW_FENTRY("uniqid", zif_uniqid, arginfo_uniqid, 0, NULL)
#endif
	ZEND_RAW_FENTRY("parse_url", zif_parse_url, arginfo_parse_url, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("urlencode", zif_urlencode, arginfo_urlencode, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("urldecode", zif_urldecode, arginfo_urldecode, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("rawurlencode", zif_rawurlencode, arginfo_rawurlencode, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("rawurldecode", zif_rawurldecode, arginfo_rawurldecode, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("get_headers", zif_get_headers, arginfo_get_headers, 0, NULL)
	ZEND_RAW_FENTRY("stream_bucket_make_writeable", zif_stream_bucket_make_writeable, arginfo_stream_bucket_make_writeable, 0, NULL)
	ZEND_RAW_FENTRY("stream_bucket_prepend", zif_stream_bucket_prepend, arginfo_stream_bucket_prepend, 0, NULL)
	ZEND_RAW_FENTRY("stream_bucket_append", zif_stream_bucket_append, arginfo_stream_bucket_append, 0, NULL)
	ZEND_RAW_FENTRY("stream_bucket_new", zif_stream_bucket_new, arginfo_stream_bucket_new, 0, NULL)
	ZEND_RAW_FENTRY("stream_get_filters", zif_stream_get_filters, arginfo_stream_get_filters, 0, NULL)
	ZEND_RAW_FENTRY("stream_filter_register", zif_stream_filter_register, arginfo_stream_filter_register, 0, NULL)
	ZEND_RAW_FENTRY("convert_uuencode", zif_convert_uuencode, arginfo_convert_uuencode, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("convert_uudecode", zif_convert_uudecode, arginfo_convert_uudecode, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("var_dump", zif_var_dump, arginfo_var_dump, 0, NULL)
	ZEND_RAW_FENTRY("var_export", zif_var_export, arginfo_var_export, 0, NULL)
	ZEND_RAW_FENTRY("debug_zval_dump", zif_debug_zval_dump, arginfo_debug_zval_dump, 0, NULL)
	ZEND_RAW_FENTRY("serialize", zif_serialize, arginfo_serialize, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("unserialize", zif_unserialize, arginfo_unserialize, 0, NULL)
	ZEND_RAW_FENTRY("memory_get_usage", zif_memory_get_usage, arginfo_memory_get_usage, 0, NULL)
	ZEND_RAW_FENTRY("memory_get_peak_usage", zif_memory_get_peak_usage, arginfo_memory_get_peak_usage, 0, NULL)
	ZEND_RAW_FENTRY("memory_reset_peak_usage", zif_memory_reset_peak_usage, arginfo_memory_reset_peak_usage, 0, NULL)
	ZEND_RAW_FENTRY("version_compare", zif_version_compare, arginfo_version_compare, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
#if defined(PHP_WIN32)
	ZEND_RAW_FENTRY("sapi_windows_cp_set", zif_sapi_windows_cp_set, arginfo_sapi_windows_cp_set, 0, NULL)
#endif
#if defined(PHP_WIN32)
	ZEND_RAW_FENTRY("sapi_windows_cp_get", zif_sapi_windows_cp_get, arginfo_sapi_windows_cp_get, 0, NULL)
#endif
#if defined(PHP_WIN32)
	ZEND_RAW_FENTRY("sapi_windows_cp_conv", zif_sapi_windows_cp_conv, arginfo_sapi_windows_cp_conv, 0, NULL)
#endif
#if defined(PHP_WIN32)
	ZEND_RAW_FENTRY("sapi_windows_cp_is_utf8", zif_sapi_windows_cp_is_utf8, arginfo_sapi_windows_cp_is_utf8, 0, NULL)
#endif
#if defined(PHP_WIN32)
	ZEND_RAW_FENTRY("sapi_windows_set_ctrl_handler", zif_sapi_windows_set_ctrl_handler, arginfo_sapi_windows_set_ctrl_handler, 0, NULL)
#endif
#if defined(PHP_WIN32)
	ZEND_RAW_FENTRY("sapi_windows_generate_ctrl_event", zif_sapi_windows_generate_ctrl_event, arginfo_sapi_windows_generate_ctrl_event, 0, NULL)
#endif
	ZEND_FE_END
};

static const zend_function_entry class___PHP_Incomplete_Class_methods[] = {
	ZEND_FE_END
};

static const zend_function_entry class_AssertionError_methods[] = {
	ZEND_FE_END
};

static void register_basic_functions_symbols(int module_number)
{
	REGISTER_LONG_CONSTANT("EXTR_OVERWRITE", PHP_EXTR_OVERWRITE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EXTR_SKIP", PHP_EXTR_SKIP, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EXTR_PREFIX_SAME", PHP_EXTR_PREFIX_SAME, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EXTR_PREFIX_ALL", PHP_EXTR_PREFIX_ALL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EXTR_PREFIX_INVALID", PHP_EXTR_PREFIX_INVALID, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EXTR_PREFIX_IF_EXISTS", PHP_EXTR_PREFIX_IF_EXISTS, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EXTR_IF_EXISTS", PHP_EXTR_IF_EXISTS, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EXTR_REFS", PHP_EXTR_REFS, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SORT_ASC", PHP_SORT_ASC, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SORT_DESC", PHP_SORT_DESC, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SORT_REGULAR", PHP_SORT_REGULAR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SORT_NUMERIC", PHP_SORT_NUMERIC, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SORT_STRING", PHP_SORT_STRING, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SORT_LOCALE_STRING", PHP_SORT_LOCALE_STRING, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SORT_NATURAL", PHP_SORT_NATURAL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SORT_FLAG_CASE", PHP_SORT_FLAG_CASE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CASE_LOWER", PHP_CASE_LOWER, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CASE_UPPER", PHP_CASE_UPPER, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("COUNT_NORMAL", PHP_COUNT_NORMAL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("COUNT_RECURSIVE", PHP_COUNT_RECURSIVE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ARRAY_FILTER_USE_BOTH", ARRAY_FILTER_USE_BOTH, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ARRAY_FILTER_USE_KEY", ARRAY_FILTER_USE_KEY, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ASSERT_ACTIVE", PHP_ASSERT_ACTIVE, CONST_PERSISTENT | CONST_DEPRECATED);
	REGISTER_LONG_CONSTANT("ASSERT_CALLBACK", PHP_ASSERT_CALLBACK, CONST_PERSISTENT | CONST_DEPRECATED);
	REGISTER_LONG_CONSTANT("ASSERT_BAIL", PHP_ASSERT_BAIL, CONST_PERSISTENT | CONST_DEPRECATED);
	REGISTER_LONG_CONSTANT("ASSERT_WARNING", PHP_ASSERT_WARNING, CONST_PERSISTENT | CONST_DEPRECATED);
	REGISTER_LONG_CONSTANT("ASSERT_EXCEPTION", PHP_ASSERT_EXCEPTION, CONST_PERSISTENT | CONST_DEPRECATED);
	REGISTER_LONG_CONSTANT("CONNECTION_ABORTED", PHP_CONNECTION_ABORTED, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CONNECTION_NORMAL", PHP_CONNECTION_NORMAL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CONNECTION_TIMEOUT", PHP_CONNECTION_TIMEOUT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("INI_USER", ZEND_INI_USER, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("INI_PERDIR", ZEND_INI_PERDIR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("INI_SYSTEM", ZEND_INI_SYSTEM, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("INI_ALL", ZEND_INI_ALL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("INI_SCANNER_NORMAL", ZEND_INI_SCANNER_NORMAL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("INI_SCANNER_RAW", ZEND_INI_SCANNER_RAW, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("INI_SCANNER_TYPED", ZEND_INI_SCANNER_TYPED, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PHP_URL_SCHEME", PHP_URL_SCHEME, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PHP_URL_HOST", PHP_URL_HOST, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PHP_URL_PORT", PHP_URL_PORT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PHP_URL_USER", PHP_URL_USER, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PHP_URL_PASS", PHP_URL_PASS, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PHP_URL_PATH", PHP_URL_PATH, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PHP_URL_QUERY", PHP_URL_QUERY, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PHP_URL_FRAGMENT", PHP_URL_FRAGMENT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PHP_QUERY_RFC1738", PHP_QUERY_RFC1738, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PHP_QUERY_RFC3986", PHP_QUERY_RFC3986, CONST_PERSISTENT);
	REGISTER_DOUBLE_CONSTANT("M_E", M_E, CONST_PERSISTENT);
	ZEND_ASSERT(M_E == 2.718281828459045);
	REGISTER_DOUBLE_CONSTANT("M_LOG2E", M_LOG2E, CONST_PERSISTENT);
	ZEND_ASSERT(M_LOG2E == 1.4426950408889634);
	REGISTER_DOUBLE_CONSTANT("M_LOG10E", M_LOG10E, CONST_PERSISTENT);
	ZEND_ASSERT(M_LOG10E == 0.4342944819032518);
	REGISTER_DOUBLE_CONSTANT("M_LN2", M_LN2, CONST_PERSISTENT);
	ZEND_ASSERT(M_LN2 == 0.6931471805599453);
	REGISTER_DOUBLE_CONSTANT("M_LN10", M_LN10, CONST_PERSISTENT);
	ZEND_ASSERT(M_LN10 == 2.302585092994046);
	REGISTER_DOUBLE_CONSTANT("M_PI", M_PI, CONST_PERSISTENT);
	ZEND_ASSERT(M_PI == 3.141592653589793);
	REGISTER_DOUBLE_CONSTANT("M_PI_2", M_PI_2, CONST_PERSISTENT);
	ZEND_ASSERT(M_PI_2 == 1.5707963267948966);
	REGISTER_DOUBLE_CONSTANT("M_PI_4", M_PI_4, CONST_PERSISTENT);
	ZEND_ASSERT(M_PI_4 == 0.7853981633974483);
	REGISTER_DOUBLE_CONSTANT("M_1_PI", M_1_PI, CONST_PERSISTENT);
	ZEND_ASSERT(M_1_PI == 0.3183098861837907);
	REGISTER_DOUBLE_CONSTANT("M_2_PI", M_2_PI, CONST_PERSISTENT);
	ZEND_ASSERT(M_2_PI == 0.6366197723675814);
	REGISTER_DOUBLE_CONSTANT("M_SQRTPI", M_SQRTPI, CONST_PERSISTENT);
	ZEND_ASSERT(M_SQRTPI == 1.772453850905516);
	REGISTER_DOUBLE_CONSTANT("M_2_SQRTPI", M_2_SQRTPI, CONST_PERSISTENT);
	ZEND_ASSERT(M_2_SQRTPI == 1.1283791670955126);
	REGISTER_DOUBLE_CONSTANT("M_LNPI", M_LNPI, CONST_PERSISTENT);
	ZEND_ASSERT(M_LNPI == 1.1447298858494002);
	REGISTER_DOUBLE_CONSTANT("M_EULER", M_EULER, CONST_PERSISTENT);
	ZEND_ASSERT(M_EULER == 0.5772156649015329);
	REGISTER_DOUBLE_CONSTANT("M_SQRT2", M_SQRT2, CONST_PERSISTENT);
	ZEND_ASSERT(M_SQRT2 == 1.4142135623730951);
	REGISTER_DOUBLE_CONSTANT("M_SQRT1_2", M_SQRT1_2, CONST_PERSISTENT);
	ZEND_ASSERT(M_SQRT1_2 == 0.7071067811865476);
	REGISTER_DOUBLE_CONSTANT("M_SQRT3", M_SQRT3, CONST_PERSISTENT);
	ZEND_ASSERT(M_SQRT3 == 1.7320508075688772);
	REGISTER_DOUBLE_CONSTANT("INF", ZEND_INFINITY, CONST_PERSISTENT);
	REGISTER_DOUBLE_CONSTANT("NAN", ZEND_NAN, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PHP_ROUND_HALF_UP", PHP_ROUND_HALF_UP, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PHP_ROUND_HALF_DOWN", PHP_ROUND_HALF_DOWN, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PHP_ROUND_HALF_EVEN", PHP_ROUND_HALF_EVEN, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PHP_ROUND_HALF_ODD", PHP_ROUND_HALF_ODD, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PHP_ROUND_CEILING", PHP_ROUND_CEILING, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PHP_ROUND_FLOOR", PHP_ROUND_FLOOR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PHP_ROUND_TOWARD_ZERO", PHP_ROUND_TOWARD_ZERO, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PHP_ROUND_AWAY_FROM_ZERO", PHP_ROUND_AWAY_FROM_ZERO, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CRYPT_SALT_LENGTH", PHP_MAX_SALT_LEN, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CRYPT_STD_DES", 1, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CRYPT_EXT_DES", 1, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CRYPT_MD5", 1, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CRYPT_BLOWFISH", 1, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CRYPT_SHA256", 1, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CRYPT_SHA512", 1, CONST_PERSISTENT);
#if (defined(PHP_WIN32) || (defined(HAVE_DNS_SEARCH_FUNC) && defined(HAVE_FULL_DNS_FUNCS)))
	REGISTER_LONG_CONSTANT("DNS_A", PHP_DNS_A, CONST_PERSISTENT);
#endif
#if (defined(PHP_WIN32) || (defined(HAVE_DNS_SEARCH_FUNC) && defined(HAVE_FULL_DNS_FUNCS)))
	REGISTER_LONG_CONSTANT("DNS_NS", PHP_DNS_NS, CONST_PERSISTENT);
#endif
#if (defined(PHP_WIN32) || (defined(HAVE_DNS_SEARCH_FUNC) && defined(HAVE_FULL_DNS_FUNCS)))
	REGISTER_LONG_CONSTANT("DNS_CNAME", PHP_DNS_CNAME, CONST_PERSISTENT);
#endif
#if (defined(PHP_WIN32) || (defined(HAVE_DNS_SEARCH_FUNC) && defined(HAVE_FULL_DNS_FUNCS)))
	REGISTER_LONG_CONSTANT("DNS_SOA", PHP_DNS_SOA, CONST_PERSISTENT);
#endif
#if (defined(PHP_WIN32) || (defined(HAVE_DNS_SEARCH_FUNC) && defined(HAVE_FULL_DNS_FUNCS)))
	REGISTER_LONG_CONSTANT("DNS_PTR", PHP_DNS_PTR, CONST_PERSISTENT);
#endif
#if (defined(PHP_WIN32) || (defined(HAVE_DNS_SEARCH_FUNC) && defined(HAVE_FULL_DNS_FUNCS)))
	REGISTER_LONG_CONSTANT("DNS_HINFO", PHP_DNS_HINFO, CONST_PERSISTENT);
#endif
#if (defined(PHP_WIN32) || (defined(HAVE_DNS_SEARCH_FUNC) && defined(HAVE_FULL_DNS_FUNCS))) && (!defined(PHP_WIN32))
	REGISTER_LONG_CONSTANT("DNS_CAA", PHP_DNS_CAA, CONST_PERSISTENT);
#endif
#if (defined(PHP_WIN32) || (defined(HAVE_DNS_SEARCH_FUNC) && defined(HAVE_FULL_DNS_FUNCS)))
	REGISTER_LONG_CONSTANT("DNS_MX", PHP_DNS_MX, CONST_PERSISTENT);
#endif
#if (defined(PHP_WIN32) || (defined(HAVE_DNS_SEARCH_FUNC) && defined(HAVE_FULL_DNS_FUNCS)))
	REGISTER_LONG_CONSTANT("DNS_TXT", PHP_DNS_TXT, CONST_PERSISTENT);
#endif
#if (defined(PHP_WIN32) || (defined(HAVE_DNS_SEARCH_FUNC) && defined(HAVE_FULL_DNS_FUNCS)))
	REGISTER_LONG_CONSTANT("DNS_SRV", PHP_DNS_SRV, CONST_PERSISTENT);
#endif
#if (defined(PHP_WIN32) || (defined(HAVE_DNS_SEARCH_FUNC) && defined(HAVE_FULL_DNS_FUNCS)))
	REGISTER_LONG_CONSTANT("DNS_NAPTR", PHP_DNS_NAPTR, CONST_PERSISTENT);
#endif
#if (defined(PHP_WIN32) || (defined(HAVE_DNS_SEARCH_FUNC) && defined(HAVE_FULL_DNS_FUNCS)))
	REGISTER_LONG_CONSTANT("DNS_AAAA", PHP_DNS_AAAA, CONST_PERSISTENT);
#endif
#if (defined(PHP_WIN32) || (defined(HAVE_DNS_SEARCH_FUNC) && defined(HAVE_FULL_DNS_FUNCS)))
	REGISTER_LONG_CONSTANT("DNS_A6", PHP_DNS_A6, CONST_PERSISTENT);
#endif
#if (defined(PHP_WIN32) || (defined(HAVE_DNS_SEARCH_FUNC) && defined(HAVE_FULL_DNS_FUNCS)))
	REGISTER_LONG_CONSTANT("DNS_ANY", PHP_DNS_ANY, CONST_PERSISTENT);
#endif
#if (defined(PHP_WIN32) || (defined(HAVE_DNS_SEARCH_FUNC) && defined(HAVE_FULL_DNS_FUNCS)))
	REGISTER_LONG_CONSTANT("DNS_ALL", PHP_DNS_ALL, CONST_PERSISTENT);
#endif
	REGISTER_LONG_CONSTANT("HTML_SPECIALCHARS", PHP_HTML_SPECIALCHARS, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("HTML_ENTITIES", PHP_HTML_ENTITIES, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ENT_COMPAT", ENT_COMPAT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ENT_QUOTES", ENT_QUOTES, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ENT_NOQUOTES", ENT_NOQUOTES, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ENT_IGNORE", ENT_IGNORE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ENT_SUBSTITUTE", ENT_SUBSTITUTE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ENT_DISALLOWED", ENT_DISALLOWED, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ENT_HTML401", ENT_HTML401, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ENT_XML1", ENT_XML1, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ENT_XHTML", ENT_XHTML, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ENT_HTML5", ENT_HTML5, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMAGETYPE_GIF", IMAGE_FILETYPE_GIF, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMAGETYPE_JPEG", IMAGE_FILETYPE_JPEG, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMAGETYPE_PNG", IMAGE_FILETYPE_PNG, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMAGETYPE_SWF", IMAGE_FILETYPE_SWF, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMAGETYPE_PSD", IMAGE_FILETYPE_PSD, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMAGETYPE_BMP", IMAGE_FILETYPE_BMP, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMAGETYPE_TIFF_II", IMAGE_FILETYPE_TIFF_II, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMAGETYPE_TIFF_MM", IMAGE_FILETYPE_TIFF_MM, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMAGETYPE_JPC", IMAGE_FILETYPE_JPC, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMAGETYPE_JP2", IMAGE_FILETYPE_JP2, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMAGETYPE_JPX", IMAGE_FILETYPE_JPX, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMAGETYPE_JB2", IMAGE_FILETYPE_JB2, CONST_PERSISTENT);
#if (defined(HAVE_ZLIB) && !defined(COMPILE_DL_ZLIB))
	REGISTER_LONG_CONSTANT("IMAGETYPE_SWC", IMAGE_FILETYPE_SWC, CONST_PERSISTENT);
#endif
	REGISTER_LONG_CONSTANT("IMAGETYPE_IFF", IMAGE_FILETYPE_IFF, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMAGETYPE_WBMP", IMAGE_FILETYPE_WBMP, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMAGETYPE_JPEG2000", IMAGE_FILETYPE_JPC, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMAGETYPE_XBM", IMAGE_FILETYPE_XBM, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMAGETYPE_ICO", IMAGE_FILETYPE_ICO, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMAGETYPE_WEBP", IMAGE_FILETYPE_WEBP, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMAGETYPE_AVIF", IMAGE_FILETYPE_AVIF, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMAGETYPE_UNKNOWN", IMAGE_FILETYPE_UNKNOWN, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMAGETYPE_COUNT", IMAGE_FILETYPE_COUNT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("INFO_GENERAL", PHP_INFO_GENERAL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("INFO_CREDITS", PHP_INFO_CREDITS, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("INFO_CONFIGURATION", PHP_INFO_CONFIGURATION, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("INFO_MODULES", PHP_INFO_MODULES, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("INFO_ENVIRONMENT", PHP_INFO_ENVIRONMENT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("INFO_VARIABLES", PHP_INFO_VARIABLES, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("INFO_LICENSE", PHP_INFO_LICENSE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("INFO_ALL", PHP_INFO_ALL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CREDITS_GROUP", PHP_CREDITS_GROUP, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CREDITS_GENERAL", PHP_CREDITS_GENERAL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CREDITS_SAPI", PHP_CREDITS_SAPI, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CREDITS_MODULES", PHP_CREDITS_MODULES, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CREDITS_DOCS", PHP_CREDITS_DOCS, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CREDITS_FULLPAGE", PHP_CREDITS_FULLPAGE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CREDITS_QA", PHP_CREDITS_QA, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CREDITS_ALL", PHP_CREDITS_ALL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LOG_EMERG", LOG_EMERG, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LOG_ALERT", LOG_ALERT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LOG_CRIT", LOG_CRIT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LOG_ERR", LOG_ERR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LOG_WARNING", LOG_WARNING, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LOG_NOTICE", LOG_NOTICE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LOG_INFO", LOG_INFO, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LOG_DEBUG", LOG_DEBUG, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LOG_KERN", LOG_KERN, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LOG_USER", LOG_USER, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LOG_MAIL", LOG_MAIL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LOG_DAEMON", LOG_DAEMON, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LOG_AUTH", LOG_AUTH, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LOG_SYSLOG", LOG_SYSLOG, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LOG_LPR", LOG_LPR, CONST_PERSISTENT);
#if defined(LOG_NEWS)
	REGISTER_LONG_CONSTANT("LOG_NEWS", LOG_NEWS, CONST_PERSISTENT);
#endif
#if defined(LOG_UUCP)
	REGISTER_LONG_CONSTANT("LOG_UUCP", LOG_UUCP, CONST_PERSISTENT);
#endif
#if defined(LOG_CRON)
	REGISTER_LONG_CONSTANT("LOG_CRON", LOG_CRON, CONST_PERSISTENT);
#endif
#if defined(LOG_AUTHPRIV)
	REGISTER_LONG_CONSTANT("LOG_AUTHPRIV", LOG_AUTHPRIV, CONST_PERSISTENT);
#endif
#if !defined(PHP_WIN32)
	REGISTER_LONG_CONSTANT("LOG_LOCAL0", LOG_LOCAL0, CONST_PERSISTENT);
#endif
#if !defined(PHP_WIN32)
	REGISTER_LONG_CONSTANT("LOG_LOCAL1", LOG_LOCAL1, CONST_PERSISTENT);
#endif
#if !defined(PHP_WIN32)
	REGISTER_LONG_CONSTANT("LOG_LOCAL2", LOG_LOCAL2, CONST_PERSISTENT);
#endif
#if !defined(PHP_WIN32)
	REGISTER_LONG_CONSTANT("LOG_LOCAL3", LOG_LOCAL3, CONST_PERSISTENT);
#endif
#if !defined(PHP_WIN32)
	REGISTER_LONG_CONSTANT("LOG_LOCAL4", LOG_LOCAL4, CONST_PERSISTENT);
#endif
#if !defined(PHP_WIN32)
	REGISTER_LONG_CONSTANT("LOG_LOCAL5", LOG_LOCAL5, CONST_PERSISTENT);
#endif
#if !defined(PHP_WIN32)
	REGISTER_LONG_CONSTANT("LOG_LOCAL6", LOG_LOCAL6, CONST_PERSISTENT);
#endif
#if !defined(PHP_WIN32)
	REGISTER_LONG_CONSTANT("LOG_LOCAL7", LOG_LOCAL7, CONST_PERSISTENT);
#endif
	REGISTER_LONG_CONSTANT("LOG_PID", LOG_PID, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LOG_CONS", LOG_CONS, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LOG_ODELAY", LOG_ODELAY, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LOG_NDELAY", LOG_NDELAY, CONST_PERSISTENT);
#if defined(LOG_NOWAIT)
	REGISTER_LONG_CONSTANT("LOG_NOWAIT", LOG_NOWAIT, CONST_PERSISTENT);
#endif
#if defined(LOG_PERROR)
	REGISTER_LONG_CONSTANT("LOG_PERROR", LOG_PERROR, CONST_PERSISTENT);
#endif
	REGISTER_LONG_CONSTANT("STR_PAD_LEFT", PHP_STR_PAD_LEFT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STR_PAD_RIGHT", PHP_STR_PAD_RIGHT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STR_PAD_BOTH", PHP_STR_PAD_BOTH, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PATHINFO_DIRNAME", PHP_PATHINFO_DIRNAME, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PATHINFO_BASENAME", PHP_PATHINFO_BASENAME, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PATHINFO_EXTENSION", PHP_PATHINFO_EXTENSION, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PATHINFO_FILENAME", PHP_PATHINFO_FILENAME, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PATHINFO_ALL", PHP_PATHINFO_ALL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CHAR_MAX", CHAR_MAX, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LC_CTYPE", LC_CTYPE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LC_NUMERIC", LC_NUMERIC, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LC_TIME", LC_TIME, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LC_COLLATE", LC_COLLATE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LC_MONETARY", LC_MONETARY, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LC_ALL", LC_ALL, CONST_PERSISTENT);
#if defined(LC_MESSAGES)
	REGISTER_LONG_CONSTANT("LC_MESSAGES", LC_MESSAGES, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(ABDAY_1)
	REGISTER_LONG_CONSTANT("ABDAY_1", ABDAY_1, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(ABDAY_1)
	REGISTER_LONG_CONSTANT("ABDAY_2", ABDAY_2, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(ABDAY_1)
	REGISTER_LONG_CONSTANT("ABDAY_3", ABDAY_3, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(ABDAY_1)
	REGISTER_LONG_CONSTANT("ABDAY_4", ABDAY_4, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(ABDAY_1)
	REGISTER_LONG_CONSTANT("ABDAY_5", ABDAY_5, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(ABDAY_1)
	REGISTER_LONG_CONSTANT("ABDAY_6", ABDAY_6, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(ABDAY_1)
	REGISTER_LONG_CONSTANT("ABDAY_7", ABDAY_7, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(DAY_1)
	REGISTER_LONG_CONSTANT("DAY_1", DAY_1, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(DAY_1)
	REGISTER_LONG_CONSTANT("DAY_2", DAY_2, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(DAY_1)
	REGISTER_LONG_CONSTANT("DAY_3", DAY_3, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(DAY_1)
	REGISTER_LONG_CONSTANT("DAY_4", DAY_4, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(DAY_1)
	REGISTER_LONG_CONSTANT("DAY_5", DAY_5, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(DAY_1)
	REGISTER_LONG_CONSTANT("DAY_6", DAY_6, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(DAY_1)
	REGISTER_LONG_CONSTANT("DAY_7", DAY_7, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(ABMON_1)
	REGISTER_LONG_CONSTANT("ABMON_1", ABMON_1, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(ABMON_1)
	REGISTER_LONG_CONSTANT("ABMON_2", ABMON_2, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(ABMON_1)
	REGISTER_LONG_CONSTANT("ABMON_3", ABMON_3, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(ABMON_1)
	REGISTER_LONG_CONSTANT("ABMON_4", ABMON_4, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(ABMON_1)
	REGISTER_LONG_CONSTANT("ABMON_5", ABMON_5, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(ABMON_1)
	REGISTER_LONG_CONSTANT("ABMON_6", ABMON_6, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(ABMON_1)
	REGISTER_LONG_CONSTANT("ABMON_7", ABMON_7, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(ABMON_1)
	REGISTER_LONG_CONSTANT("ABMON_8", ABMON_8, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(ABMON_1)
	REGISTER_LONG_CONSTANT("ABMON_9", ABMON_9, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(ABMON_1)
	REGISTER_LONG_CONSTANT("ABMON_10", ABMON_10, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(ABMON_1)
	REGISTER_LONG_CONSTANT("ABMON_11", ABMON_11, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(ABMON_1)
	REGISTER_LONG_CONSTANT("ABMON_12", ABMON_12, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(MON_1)
	REGISTER_LONG_CONSTANT("MON_1", MON_1, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(MON_1)
	REGISTER_LONG_CONSTANT("MON_2", MON_2, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(MON_1)
	REGISTER_LONG_CONSTANT("MON_3", MON_3, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(MON_1)
	REGISTER_LONG_CONSTANT("MON_4", MON_4, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(MON_1)
	REGISTER_LONG_CONSTANT("MON_5", MON_5, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(MON_1)
	REGISTER_LONG_CONSTANT("MON_6", MON_6, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(MON_1)
	REGISTER_LONG_CONSTANT("MON_7", MON_7, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(MON_1)
	REGISTER_LONG_CONSTANT("MON_8", MON_8, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(MON_1)
	REGISTER_LONG_CONSTANT("MON_9", MON_9, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(MON_1)
	REGISTER_LONG_CONSTANT("MON_10", MON_10, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(MON_1)
	REGISTER_LONG_CONSTANT("MON_11", MON_11, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(MON_1)
	REGISTER_LONG_CONSTANT("MON_12", MON_12, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(AM_STR)
	REGISTER_LONG_CONSTANT("AM_STR", AM_STR, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(PM_STR)
	REGISTER_LONG_CONSTANT("PM_STR", PM_STR, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(D_T_FMT)
	REGISTER_LONG_CONSTANT("D_T_FMT", D_T_FMT, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(D_FMT)
	REGISTER_LONG_CONSTANT("D_FMT", D_FMT, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(T_FMT)
	REGISTER_LONG_CONSTANT("T_FMT", T_FMT, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(T_FMT_AMPM)
	REGISTER_LONG_CONSTANT("T_FMT_AMPM", T_FMT_AMPM, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(ERA)
	REGISTER_LONG_CONSTANT("ERA", ERA, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(ERA_YEAR)
	REGISTER_LONG_CONSTANT("ERA_YEAR", ERA_YEAR, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(ERA_D_T_FMT)
	REGISTER_LONG_CONSTANT("ERA_D_T_FMT", ERA_D_T_FMT, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(ERA_D_FMT)
	REGISTER_LONG_CONSTANT("ERA_D_FMT", ERA_D_FMT, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(ERA_T_FMT)
	REGISTER_LONG_CONSTANT("ERA_T_FMT", ERA_T_FMT, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(ALT_DIGITS)
	REGISTER_LONG_CONSTANT("ALT_DIGITS", ALT_DIGITS, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(INT_CURR_SYMBOL)
	REGISTER_LONG_CONSTANT("INT_CURR_SYMBOL", INT_CURR_SYMBOL, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(CURRENCY_SYMBOL)
	REGISTER_LONG_CONSTANT("CURRENCY_SYMBOL", CURRENCY_SYMBOL, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(CRNCYSTR)
	REGISTER_LONG_CONSTANT("CRNCYSTR", CRNCYSTR, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(MON_DECIMAL_POINT)
	REGISTER_LONG_CONSTANT("MON_DECIMAL_POINT", MON_DECIMAL_POINT, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(MON_THOUSANDS_SEP)
	REGISTER_LONG_CONSTANT("MON_THOUSANDS_SEP", MON_THOUSANDS_SEP, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(MON_GROUPING)
	REGISTER_LONG_CONSTANT("MON_GROUPING", MON_GROUPING, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(POSITIVE_SIGN)
	REGISTER_LONG_CONSTANT("POSITIVE_SIGN", POSITIVE_SIGN, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(NEGATIVE_SIGN)
	REGISTER_LONG_CONSTANT("NEGATIVE_SIGN", NEGATIVE_SIGN, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(INT_FRAC_DIGITS)
	REGISTER_LONG_CONSTANT("INT_FRAC_DIGITS", INT_FRAC_DIGITS, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(FRAC_DIGITS)
	REGISTER_LONG_CONSTANT("FRAC_DIGITS", FRAC_DIGITS, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(P_CS_PRECEDES)
	REGISTER_LONG_CONSTANT("P_CS_PRECEDES", P_CS_PRECEDES, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(P_SEP_BY_SPACE)
	REGISTER_LONG_CONSTANT("P_SEP_BY_SPACE", P_SEP_BY_SPACE, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(N_CS_PRECEDES)
	REGISTER_LONG_CONSTANT("N_CS_PRECEDES", N_CS_PRECEDES, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(N_SEP_BY_SPACE)
	REGISTER_LONG_CONSTANT("N_SEP_BY_SPACE", N_SEP_BY_SPACE, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(P_SIGN_POSN)
	REGISTER_LONG_CONSTANT("P_SIGN_POSN", P_SIGN_POSN, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(N_SIGN_POSN)
	REGISTER_LONG_CONSTANT("N_SIGN_POSN", N_SIGN_POSN, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(DECIMAL_POINT)
	REGISTER_LONG_CONSTANT("DECIMAL_POINT", DECIMAL_POINT, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(RADIXCHAR)
	REGISTER_LONG_CONSTANT("RADIXCHAR", RADIXCHAR, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(THOUSANDS_SEP)
	REGISTER_LONG_CONSTANT("THOUSANDS_SEP", THOUSANDS_SEP, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(THOUSEP)
	REGISTER_LONG_CONSTANT("THOUSEP", THOUSEP, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(GROUPING)
	REGISTER_LONG_CONSTANT("GROUPING", GROUPING, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(YESEXPR)
	REGISTER_LONG_CONSTANT("YESEXPR", YESEXPR, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(NOEXPR)
	REGISTER_LONG_CONSTANT("NOEXPR", NOEXPR, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(YESSTR)
	REGISTER_LONG_CONSTANT("YESSTR", YESSTR, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(NOSTR)
	REGISTER_LONG_CONSTANT("NOSTR", NOSTR, CONST_PERSISTENT);
#endif
#if defined(HAVE_NL_LANGINFO) && defined(CODESET)
	REGISTER_LONG_CONSTANT("CODESET", CODESET, CONST_PERSISTENT);
#endif


	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "crypt", sizeof("crypt") - 1), 0, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "password_hash", sizeof("password_hash") - 1), 0, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "password_verify", sizeof("password_verify") - 1), 0, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);
}

static zend_class_entry *register_class___PHP_Incomplete_Class(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "__PHP_Incomplete_Class", class___PHP_Incomplete_Class_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL|ZEND_ACC_ALLOW_DYNAMIC_PROPERTIES;

	zend_string *attribute_name_AllowDynamicProperties_class___PHP_Incomplete_Class_0 = zend_string_init_interned("AllowDynamicProperties", sizeof("AllowDynamicProperties") - 1, 1);
	zend_add_class_attribute(class_entry, attribute_name_AllowDynamicProperties_class___PHP_Incomplete_Class_0, 0);
	zend_string_release(attribute_name_AllowDynamicProperties_class___PHP_Incomplete_Class_0);

	return class_entry;
}

static zend_class_entry *register_class_AssertionError(zend_class_entry *class_entry_Error)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "AssertionError", class_AssertionError_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_Error);

	return class_entry;
}
