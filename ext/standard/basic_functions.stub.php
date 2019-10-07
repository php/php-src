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

function array_push(array &$stack, ...$args): int {}

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

/**
 * @param array|object $arg
 * @return mixed
 */
function end(array &$arg) {}

/**
 * @param array|object $arg
 * @return mixed
 */
function prev(&$arg) {}

/**
 * @param array|object $arg
 * @return mixed
 */
function next(&$arg) {}

/**
 * @param array|object $arg
 * @return mixed
 */
function reset(&$arg) {}

/**
 * @param array|object $arg
 * @return mixed
 */
function current($arg) {}

/**
 * @param array|object $arg
 * @return int|string|null
 */
function key($arg) {}

/** @return mixed */
function min($arg, ...$args) {}

/** @return mixed */
function max($arg, ...$args) {}

/** @param array|object $input */
function array_walk(&$input, callable $funcname, $userdata = null): bool {}

/** @param array|object $input */
function array_walk_recursive(&$input, callable $funcname, $userdata = null): bool {}

function in_array($needle, array $haystack, bool $strict = false): bool {}

/** @return int|string|false */
function array_search($needle, array $haystack, bool $strict = false) {}

/** @prefer-ref $arg */
function extract(array &$arg, int $extract_type = EXTR_OVERWRITE, string $prefix = ""): int {}

function compact($var_name, ...$var_names): array {}

function array_fill(int $start_key, int $num, $val): array {}

function array_fill_keys(array $keys, $val): array {}

/**
 * @param int|float|string $low
 * @param int|float|string $high
 * @param int|float $step
 */
function range($low, $high, $step = 1): array {}

function shuffle(array &$arg): bool {}

/** @return mixed */
function array_pop(array &$stack) {}

/** @return mixed */
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
 * @param int|string|null $index_key
 */
function array_column(array $arg, $column_key, $index_key = null): array {}

function array_reverse(array $input, bool $preserve_keys = false): array {}

function array_pad(array $arg, int $pad_size, $pad_value): array {}

function array_flip(array $arg): array {}

function array_change_key_case(array $input, int $case = CASE_LOWER): array {}

function array_unique(array $arg, int $flags = SORT_STRING): array {}

function array_intersect_key(array $arr1, array $arr2, array ...$arrays): array {}

function array_intersect_ukey(array $arr1, array $arr2, ...$rest): array {}

function array_intersect(array $arr1, array $arr2, array ...$arrays): array {}

function array_uintersect(array $arr1, array $arr2, ...$rest): array {}

function array_intersect_assoc(array $arr1, array $arr2, array ...$arrays): array {}

function array_uintersect_assoc(array $arr1, array $arr2, ...$rest): array {}

function array_intersect_uassoc(array $arr1, array $arr2, ...$rest): array {}

function array_uintersect_uassoc(array $arr1, array $arr2, ...$rest): array {}

function array_diff_key(array $arr1, array $arr2, array ...$arrays): array {}

function array_diff_ukey(array $arr1, array $arr2, ...$rest): array {}

function array_diff(array $arr1, array $arr2, array ...$arrays): array {}

function array_udiff(array $arr1, array $arr2, ...$rest): array {}

function array_diff_assoc(array $arr1, array $arr2, array ...$arrays): array {}

function array_diff_uassoc(array $arr1, array $arr2, ...$rest): array {}

function array_udiff_assoc(array $arr1, array $arr2, ...$rest): array {}

function array_udiff_uassoc(array $arr1, array $arr2, ...$rest): array {}

/**
 * @prefer-ref $arr1
 * @prefer-ref $sort_order
 * @prefer-ref $sort_flags
 * @prefer-ref $arr2
 */
function array_multisort(&$arr1, $sort_order = SORT_ASC, $sort_flags = SORT_REGULAR, &...$arr2): bool {}

/** @return int|string|array */
function array_rand(array $arg, int $num_req = 1) {}

/** @return int|float */
function array_sum(array $arg) {}

/** @return int|float */
function array_product(array $arg) {}

function array_reduce(array $arg, callable $callback, $initial = null) {}

function array_filter(array $arg, callable $callback = UNKNOWN, int $use_keys = 0): array {}

function array_map(?callable $callback, array $arr1, array ...$arrays): array {}

/**
 * @param int|string $key
 * @param array|object $search
 */
function array_key_exists($key, $search): bool {}

function array_chunk(array $arg, int $size, bool $preserve_keys = false): array {}

function array_combine(array $keys, array $values): array {}

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

/* md5.c */

function md5(string $str, bool $raw_output = false): string {}

/** @return string|false */
function md5_file(string $filename, bool $raw_output = false) {}

/* pageinfo.c */

/** @return int|false */
function getmyuid() {}

/** @return int|false */
function getmygid() {}

/** @return int|false */
function getmypid() {}

/** @return int|false */
function getmyinode() {}

function getlastmod(): int {}

/* sha1.c */

function sha1(string $str, bool $raw_output = false): string {}

/** @return string|false */
function sha1_file(string $filename, bool $raw_output = false) {}

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

/* metaphone.c */

/** @return string|false */
function metaphone(string $text, int $phones = 0) {}

/* {{{ head.c */
function header(string $string, bool $replace = true, int $http_response_code = 0): void { }

function header_remove(string $name = UNKNOWN): void { }

/** @param int|array $expires_or_options */
function setrawcookie(string $name, string $value = '', $expires_or_options = 0, string $path = '', string $domain = '', bool $secure = false, bool $httponly = false): bool {}


/** @param int|array $expires_or_options */
function setcookie(string $name, string $value = '', $expires_or_options = 0, string $path = '', string $domain = '', bool $secure = false, bool $httponly = false): bool {}

/** @return int|bool */
function http_response_code(int $response_code = 0) {}

function headers_sent(&$file = null, &$line = null): bool {}

function headers_list(): array { }

/* {{{ html.c */

function htmlspecialchars(string $string, int $quote_style = ENT_COMPAT, ?string $encoding = null, bool $double_encode = true): string {}

/** @return string|false */
function htmlspecialchars_decode(string $string, int $quote_style = ENT_COMPAT) {}

/** @return string|false */
function html_entity_decode(string $string, int $quote_style = ENT_COMPAT, string $encoding = UNKNOWN) {}

function htmlentities(string $string, int $quote_style = ENT_COMPAT, ?string $encoding = null, bool $double_encode = true): string {}

function get_html_translation_table(int $table = HTML_SPECIALCHARS, int $quote_style = ENT_COMPAT, string $encoding = "UTF-8"): array {}

/* }}} */

/* assert.c */

/** @param mixed $assertion */
function assert($assertion, $description = null): bool {}

/** @return int|string|bool|null */
function assert_options(int $what, $value = UNKNOWN) {}

/* string.c */

function bin2hex(string $data): string {}

/** @return string|false */
function hex2bin(string $data) {}

/** @return int|false */
function strspn(string $str, string $mask, int $start = 0, int $len = UNKNOWN) {}

/** @return int|false */
function strcspn(string $str, string $mask, int $start = 0, int $len = UNKNOWN) {}

#if HAVE_NL_LANGINFO
/** @return string|false */
function nl_langinfo(int $item) {}
#endif

function strcoll(string $str1, string $str2): int {}

function trim(string $str, string $character_mask = " \n\r\t\v\0"): string {}

function rtrim(string $str, string $character_mask = " \n\r\t\v\0"): string {}

function ltrim(string $str, string $character_mask = " \n\r\t\v\0"): string {}

function wordwrap(string $str, int $width = 75, string $break = "\n", bool $cut = false): string {}

function explode(string $separator, string $str, int $limit = PHP_INT_MAX): array {}

/**
 * @param string $glue Optional - defaults to empty string
 * @param array $pieces
 */
function implode($glue, $pieces = UNKNOWN): string {}

/**
 * @param string $str Optional - defaults to previous string
 * @param string $token
 * @return string|false
 */
function strtok(string $str, string $token = UNKNOWN) {}

function strtoupper(string $str): string {}

function strtolower(string $str): string {}

function basename(string $path, string $suffix = ""): string {}

function dirname(string $path, int $levels = 1): string {}

/** @return array|string */
function pathinfo(string $path, int $options = UNKNOWN) {}

/** @return string|false */
function stristr(string $haystack, string $needle, bool $before_needle = false) {}

/** @return string|false */
function strstr(string $haystack, string $needle, bool $before_needle = false) {}

/** @return int|false */
function strpos(string $haystack, string $needle, int $offset = 0) {}

/** @return int|false */
function stripos(string $haystack, string $needle, int $offset = 0) {}

/** @return int|false */
function strrpos(string $haystack, string $needle, int $offset = 0) {}

/** @return int|false */
function strripos(string $haystack, string $needle, int $offset = 0) {}

/** @return string|false */
function strrchr(string $haystack, string $needle) {}

function chunk_split(string $str, int $chunklen = 76, string $ending = "\r\n"): string {}

/** @return string|false */
function substr(string $str, int $start, int $length = UNKNOWN) {}

/**
 * @param mixed $str
 * @param mixed $replace
 * @param mixed $start
 * @param mixed $length
 * @return string|array|false
 */
function substr_replace($str, $replace, $start, $length = UNKNOWN) {}

function quotemeta(string $str): string {}

function ord(string $character): int {}

function chr(int $codepoint): string {}

function ucfirst(string $str): string {}

function lcfirst(string $str): string {}

function ucwords(string $str, string $delimiters = " \t\r\n\f\v"): string {}

/**
 * @param string|array $from
 * @return string|false
 */
function strtr(string $str, $from, string $to = UNKNOWN) {}

function strrev(string $str): string {}

/** @param float $percent */
function similar_text(string $str1, string $str2, &$percent = null): int {}

function addcslashes(string $str, string $charlist): string {}

function addslashes(string $str): string {}

function stripcslashes(string $str): string {}

function stripslashes(string $str): string {}

/**
 * @param string|array $search
 * @param string|array $replace
 * @param string|array $subject
 * @param int $replace_count
 * @return string|array
 */
function str_replace($search, $replace, $subject, &$replace_count = UNKNOWN) {}

/**
 * @param string|array $search
 * @param string|array $replace
 * @param string|array $subject
 * @param int $replace_count
 * @return string|array
 */
function str_ireplace($search, $replace, $subject, &$replace_count = UNKNOWN) {}

function hebrev(string $str, int $max_chars_per_line = 0): string {}

function hebrevc(string $str, int $max_chars_per_line = 0): string {}

function nl2br(string $str, bool $is_xhtml = true): string {}

/** @param mixed $allowable_tags */
function strip_tags(string $str, $allowable_tags = UNKNOWN): string {}

/**
 * @param string|array $locales
 * @return string|false
 */
function setlocale(int $category, $locales, ...$rest) {}

/** @param array $result */
function parse_str(string $encoded_string, &$result): void {}

function str_getcsv(string $string, string $delimiter = ',', string $enclosure = '"', string $escape = '\\'): array {}

function str_repeat(string $input, int $mult): string {}

/** @return array|string */
function count_chars(string $input, int $mode = 0) {}

function strnatcmp(string $s1, string $s2): int {}

function localeconv(): array {}

function strnatcasecmp(string $s1, string $s2): int {}

/** @return int|false */
function substr_count(string $haystack, string $needle, int $offset = 0, int $length = UNKNOWN) {}

function str_pad(string $input, int $pad_length, string $pad_string = " ", int $pad_type = STR_PAD_RIGHT): string {}

/** @return array|int|null */
function sscanf(string $str, string $format, &...$vars) {}

function str_rot13(string $str): string {}

function str_shuffle(string $str): string {}

/** @return array|int */
function str_word_count(string $str, int $format = 0, string $charlist = UNKNOWN) {}

#ifdef HAVE_STRFMON
/** @return string|false */
function money_format(string $format, float $value) {}
#endif

function str_split(string $str, int $split_length = 1): array {}

/** @return string|false */
function strpbrk(string $haystack, string $char_list) {}

/** @return int|false */
function substr_compare(string $main_str, string $str, int $offset, int $length = UNKNOWN, bool $case_insensitivity = false) {}

function utf8_encode(string $data): string {}

function utf8_decode(string $data): string {}
