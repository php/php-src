<?php

/* main/main.c */

function set_time_limit(int $seconds): bool {}

/* main/SAPI.c */

// TODO: Make this a proper callable argument?
function header_register_callback($callback): bool {}

/* main/output.c */

function ob_start(
    $user_function = null, int $chunk_size = 0, int $flags = PHP_OUTPUT_HANDLER_STDFLAGS): bool {}

function ob_flush(): bool {}

function ob_clean(): bool {}

function ob_end_flush(): bool {}

function ob_end_clean(): bool {}

/** @return string|false */
function ob_get_flush() {}

/** @return string|false */
function ob_get_clean() {}

/** @return string|false */
function ob_get_contents() {}

function ob_get_level(): int {}

/** @return int|false */
function ob_get_length() {}

function ob_list_handlers(): array {}

function ob_get_status(bool $full_status = false): array {}

// TODO: Shouldn't this be a bool argument?
function ob_implicit_flush(int $flag = 1): void {}

function output_reset_rewrite_vars(): bool {}

function output_add_rewrite_var(string $name, string $value): bool {}

/* main/streams/userspace.c */

function stream_wrapper_register(string $protocol, string $classname, int $flags = 0): bool {}

function stream_wrapper_unregister(string $protocol): bool {}

function stream_wrapper_restore(string $protocol): bool {}

/* array.c */

/** @return int|false */
function array_push(array &$stack, ...$args) {}

function krsort(array &$arg, int $sort_flags = SORT_REGULAR): bool {}

function ksort(array &$arg, int $sort_flags = SORT_REGULAR): bool {}

/** @param array|Countable $array */
function count($var, int $mode = COUNT_NORAML): int {}

function natsort(array &$arg): bool {}

function natcasesort(array &$arg): bool {}

function asort(array &$arg, int $sort_flags = SORT_REGULAR): bool {}

function arsort(array &$arg, int $sort_flags = SORT_REGULAR): bool {}

function sort(array &$arg, int $sort_flags = SORT_REGULAR): bool {}

function rsort(array &$arg, int $sort_flags = SORT_REGULAR): bool {}

function usort(array &$arg, callable $cmp_function): bool {}

function uasort(array &$arg, callable $cmp_function): bool {}

function uksort(array &$arg, callable $cmp_function): bool {}

function end(array &$arg) {}

/** @param array|object $arg */
function prev(&$arg) {}

/** @param array|object $arg */
function next(&$arg) {}

function reset(array &$arg) {}

/** @param array|object $arg */
function current($arg) {}

/** @return int|string|null */
function key(array $arg) {}

function min(...$args) {}

function max(...$args) {}

/** @param array|object $input */
function array_walk(&$input, callable $funcname, $userdata = null): bool {}

/** @param array|object $input */
function array_walk_recursive(&$input, callable $funcname, $userdata = null): bool {}

function in_array($needle, array $haystack, bool $strict = false): bool {}

/** @return int|string|false */
function array_search($needle, array $haystack, bool $strict = false) {}

/** @prefer-ref $arg */
function extract(array &$arg, $extract_type = EXTR_OVERWRITE, string $prefix = null): ?int {}

function compact(...$var_names): ?array {}

/** @return array|false */
function array_fill(int $start_key, int $num, $val) {}

function array_fill_keys(array $keys, $val): array {}

/**
 * @param int|float|string $low
 * @param int|float|string $high
 * @param int|float $step
 * @return array|false
 */
function range($low, $high, $step = 1) {}

function shuffle(array &$arg): bool {}

function array_pop(array &$stack) {}

function array_shift(array &$stack) {}

function array_unshift(array &$stack, ...$vars): int {}

function array_splice(array &$arg, int $offset, int $length = UNKNOWN, $replacement = []): array {}

function array_slice(array $arg, int $offset, ?int $length = null, bool $preserve_keys = false): array {}

function array_merge(array ...$arrays): array {}

function array_merge_recursive(array ...$arrays): array {}

function array_replace(array $arr1, array ...$arrays): array {}

function array_replace_recursive(array $arr1, array ...$arrays): array {}

function array_keys(array $arg, $search_value = UNKNOWN, bool $strict = false): array {}

/** @return int|string|null */
function array_key_first(array $arg) {}

/** @return int|string|null */
function array_key_last(array $arg) {}

function array_values(array $arg): array {}

function array_count_values(array $arg): array {}

/**
 * @param int|string|null $column_key
 * @param int|string $index_key
 * @return array|false
 */
function array_column(array $arg, $column_key, $index_key = null) {}

function array_reverse(array $input, bool $preserve_keys = false): array {}

/** @return array|false */
function array_pad(array $arg, int $pad_size, $pad_value) {}

function array_flip(array $arg): array {}

function array_change_key_case(array $input, int $case = CASE_LOWER): array {}

function array_unique(array $arg, int $flags = SORT_STRING): array {}

function array_intersect_key(array $arr1, array $arr2, array ...$arrays): array {}

// tests break if $arr2 is variadic
function array_intersect_ukey(array $arr1, array $arr2, $callback_key_compare_func): array {}

function array_intersect(array $arr1, array ...$arrays): array {}

// tests break if $arr2 is variadic
function array_uintersect(array $arr1, array $arr2, $callback_data_compare_func): array {}

function array_intersect_assoc(array $arr1, array ...$arrays): array {}

// tests break if $arr2 is variadic
function array_uintersect_assoc(array $arr1, array $arr2, $callback_data_compare_func): array {}

function array_intersect_uassoc(array $arr1, array ...$arr2, callable $callback_key_compare_func): array {}

function array_uintersect_uassoc(array $arr1, array ...$arr2, callable $callback_data_compare_func, callable $callback_key_compare_func): array {}

function array_diff_key(array $arr1, array ...$arrays): array {}

// tests break if $arr2 is variadic
function array_diff_ukey(array $arr1, array $arr2, $callback_key_comp_func): array {}

function array_diff(array $arr1, array ...$arrays): array {}

function array_udiff(array $arr1, array ...$arr2, callable $callback_data_comp_func): array {}

function array_diff_assoc(array $arr1, array ...$arrays): array {}

function array_diff_uassoc(array $arr1, array ...$arr2, callable $callback_data_comp_func): array {}

// tests break if $arr2 is variadic
function array_udiff_assoc(array $arr1, array $arr2, $callback_key_comp_func): array {}

function array_udiff_uassoc(array $arr1, array ...$arr2, callable $callback_data_comp_func, callable $callback_key_comp_func): array {}

/**
 * @prefer-ref $arr1
 * @prefer-ref $sort_order
 * @prefer-ref $sort_flags
 * @prefer-ref $arr2
 */
function array_multisort(&$arr1, $sort_order = SORT_ASC, $sort_flags = SORT_REGULAR, &...$arr2): bool {}

/** @return int|string|array|null */
function array_rand(array $arg, int $num_req = 1) {}

/** @return int|float */
function array_sum(array $arg) {}

/** @return int|float */
function array_product(array $arg) {}

function array_reduce(array $arg, callable $callback, $initial = null) {}

function array_filter(array $arg, callable $callback, int $use_keys = 0): array {}

function array_map(?callable $callback, array ...$arrays): array {}

/** @param int|string $key */
function array_key_exists($key, array $search): bool {}

function array_chunk(array $arg, int $size, bool $preserve_keys = false): ?array {}

/** @return array|false */
function array_combine(array $keys, array $values) {}

/* base64.c */

function base64_encode(string $str): string {}

/** @return string|false */
function base64_decode(string $str, bool $strict = false) {}

/* crc32.c */

function crc32(string $str): int {}

/* crypt.c */

function crypt(string $str, string $salt = UNKNOWN): string {}

/* ftok.c */

#if HAVE_FTOK
function ftok(string $pathname, string $proj): int {}
#endif

/* hrtime.c */

/** @return array|int|float|false */
function hrtime(bool $get_as_number = false) {}

/* lcg.c */

function lcg_value(): float {}

/* syslog.c */

#ifdef HAVE_SYSLOG_H
function openlog (string $ident, int $option, int $facility): bool {}

function closelog (): bool {}

function syslog (int $priority, string $message): bool {}
#endif

#ifdef HAVE_INET_NTOP
/** @return string|false */
function inet_ntop (string $in_addr) {}
#endif

#ifdef HAVE_INET_PTON
/** @return string|false */
function inet_pton (string $ip_address) {}
#endif
