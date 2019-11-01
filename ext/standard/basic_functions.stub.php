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

/** @param array|Countable $var */
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

function array_splice(array &$arg, int $offset, ?int $length = null, $replacement = []): array {}

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

/** @param mixed $key */
function array_key_exists($key, array $search): bool {}

function array_chunk(array $arg, int $size, bool $preserve_keys = false): array {}

function array_combine(array $keys, array $values): array {}

/* base64.c */

function base64_encode(string $str): string {}

/** @return string|false */
function base64_decode(string $str, bool $strict = false) {}

/* basic_functions.c */

/** @return mixed */
function constant(string $name) {}

/** @return int|false */
function ip2long(string $ip_address) {}

/** @return string|false */
function long2ip(int $proper_address) {}

/** @return string|array|false */
function getenv(string $variable = UNKNOWN, bool $local_only = false) {}

#ifdef HAVE_PUTENV
function putenv(string $setting): bool {}
#endif

/** @return array|false */
function getopt(string $options, array $longopts = [], &$optind = null) {}

function flush(): void {}

function sleep(int $seconds): int {}

function usleep(int $microseconds): void {}

#if HAVE_NANOSLEEP
/** @return array|bool */
function nanosleep(int $seconds, int $nanoseconds) {}

function time_sleep_until(float $timestamp): bool {}
#endif

function get_current_user(): string {}

/** @return string|array|false */
function get_cfg_var(string $option_name) {}

function get_magic_quotes_runtime(): bool {}

function get_magic_quotes_gpc(): bool {}

function error_log(string $message, int $message_type = 0, string $destination = UNKNOWN, string $extra_headers = UNKNOWN): bool {}

function error_get_last(): ?array {}

function error_clear_last(): void {}

/**
 * @param mixed ...$args
 * @return mixed
 */
function call_user_func(callable $function, ...$args) {}

/** @return mixed */
function call_user_func_array(callable $function, array $args) {}

/**
 * @param mixed ...$args
 * @return mixed
 */
function forward_static_call(callable $function, ...$args) {}

/** @return mixed */
function forward_static_call_array(callable $function, array $args) {}

/**
 * @param callable $function
 * @param mixed ...$args
 * @return false|null
 */
function register_shutdown_function($function, ...$args) {}

/** @return string|bool|null */
function highlight_file(string $filename, bool $return = false) {}

function php_strip_whitespace(string $filename): string {}

/** @return string|bool|null */
function highlight_string(string $string, bool $return = false) {}

/** @return string|false */
function ini_get(string $varname) {}

/** @return array|false */
function ini_get_all(?string $extension = null, bool $details = true) {}

/** @return string|false */
function ini_set(string $varname, string $value) {}

function ini_restore(string $varname): void {}

/** @return string|false */
function set_include_path(string $include_path) {}

/** @return string|false */
function get_include_path() {}

function restore_include_path(): void {}

/**
 * @param mixed $var
 * @return string|bool
 */
function print_r($var, bool $return = false) {}

function connection_aborted(): int {}

function connection_status(): int {}

function ignore_user_abort(bool $value = UNKNOWN): int {}

#if HAVE_GETSERVBYNAME
/** @return int|false */
function getservbyname(string $service, string $protocol) {}
#endif

#if HAVE_GETSERVBYPORT
/** @return string|false */
function getservbyport(int $port, string $protocol) {}
#endif

#if HAVE_GETPROTOBYNAME
/** @return int|false */
function getprotobyname(string $name) {}
#endif

#if HAVE_GETPROTOBYNUMBER
/** @return string|false */
function getprotobynumber(int $protocol) {}
#endif

/** @param mixed $args */
function register_tick_function(callable $function, ...$args): bool {}

function unregister_tick_function($function): void {}

function is_uploaded_file(string $path): bool {}

function move_uploaded_file(string $path, string $new_path): bool {}

/** @return array|false */
function parse_ini_file(string $filename, bool $process_sections = false, int $scanner_mode = INI_SCANNER_NORMAL) {}

/** @return array|false */
function parse_ini_string(string $ini_string, bool $process_sections = false, int $scanner_mode = INI_SCANNER_NORMAL) {}

#if ZEND_DEBUG
function config_get_hash(string $ini_string, bool $process_sections = false, int $scanner_mode = INI_SCANNER_NORMAL): array {}
#endif

#ifdef HAVE_GETLOADAVG
/** @return array|false */
function sys_getloadavg() {}
#endif

/* browscap.c */

/** @return object|array|false */
function get_browser(?string $browser_name = null, bool $return_array = false) {}

/* crc32.c */

function crc32(string $str): int {}

/* crypt.c */

function crypt(string $str, string $salt = UNKNOWN): string {}

/* cyr_convert.c */

function convert_cyr_string(string $str, string $from, string $to): string {}

/* datetime.c */

#if HAVE_STRPTIME
/** @return array|false */
function strptime(string $timestamp, string $format) {}
#endif

/* dns.c */

#ifdef HAVE_GETHOSTNAME
/** @return string|false */
function gethostname() {}
#endif

/** @return string|false */
function gethostbyaddr(string $ip_address) {}

function gethostbyname(string $hostname): string {}

/** @return array|false */
function gethostbynamel(string $hostname) {}

#if defined(PHP_WIN32) || HAVE_DNS_SEARCH_FUNC
function dns_check_record(string $hostname, string $type = "MX"): bool {}

/** @return array|false */
function dns_get_record(string $hostname, int $type = DNS_ANY, &$authns = null, &$addtl = null, bool $raw = false) {}

function dns_get_mx(string $hostname, &$mxhosts, &$weight = null): bool {}
#endif

/* net.c */

/** @return array|false */
function net_get_interfaces() {}

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
function substr(string $str, int $start, ?int $length = null) {}

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
 */
function strtr(string $str, $from, string $to = UNKNOWN): string {}

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
function substr_count(string $haystack, string $needle, int $offset = 0, ?int $length = null) {}

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
function substr_compare(string $main_str, string $str, int $offset, ?int $length = null, bool $case_insensitivity = false) {}

function utf8_encode(string $data): string {}

function utf8_decode(string $data): string {}

/* dir.c */

class Directory
{
    /**
     * @param resource $dir_handle
     * @return void
     */
    public function close($dir_handle = UNKNOWN) {}

    /**
     * @param resource $dir_handle
     * @return void
     */
    public function rewind($dir_handle = UNKNOWN) {}

    /**
     * @param resource $dir_handle
     * @return string|false
     */
    public function read($dir_handle = UNKNOWN) { }
}

/**
 * @param resource $context
 * @return resource|false
 */
function opendir(string $path, $context = UNKNOWN) {}

/**
 * @param resource $context
 * @return Directory|false
 */
function dir(string $path, $context = UNKNOWN) {}

/** @param resource $dir_handle */
function closedir($dir_handle = UNKNOWN): void {}

function chdir(string $directory): bool {}

#if defined(HAVE_CHROOT) && !defined(ZTS) && ENABLE_CHROOT_FUNC
function chroot(string $directory): bool {}
#endif

/** @return string|false */
function getcwd() {}

/** @param resource $dir_handle */
function rewinddir($dir_handle = UNKNOWN): void {}

/** @return string|false */
function readdir($dir_handle = UNKNOWN) {}

/**
 * @param resource $context
 * @return array|false
 */
function scandir(string $directory, int $sorting_order = 0, $context = UNKNOWN) {}

#ifdef HAVE_GLOB
/** @return array|false */
function glob(string $pattern, int $flags = 0) {}
#endif

/* filestat.c */

/** @return int|false */
function fileatime(string $filename) {}

/** @return int|false */
function filectime(string $filename) {}

/** @return int|false */
function filegroup(string $filename) {}

/** @return int|false */
function fileinode(string $filename) {}

/** @return int|false */
function filemtime(string $filename) {}

/** @return int|false */
function fileowner(string $filename) {}

/** @return int|false */
function fileperms(string $filename) {}

/** @return int|false */
function filesize(string $filename) {}

/** @return string|false */
function filetype(string $filename) {}

function file_exists(string $filename): bool {}

function is_writable(string $filename): bool {}

function is_writeable(string $filename): bool {}

function is_readable(string $filename): bool {}

function is_executable(string $filename): bool {}

function is_file(string $filename): bool {}

function is_dir(string $filename): bool {}

function is_link(string $filename): bool {}

/** @return array|false */
function stat(string $filename) {}

/** @return array|false */
function lstat(string $filename) {}

function chown(string $filename, $user): bool {}

function chgrp(string $filename, $group): bool {}

#if HAVE_LCHOWN
function lchown(string $filename, $user): bool {}

function lchgrp(string $filename, $group): bool {}
#endif

function chmod(string $filename, int $mode): bool {}

#if HAVE_UTIME
function touch(string $filename, int $time = 0, int $atime = 0): bool {}
#endif

function clearstatcache(bool $clear_realpath_cache = false, string $filename = ""): void {}

/** @return float|false */
function disk_total_space(string $directory) {}

/** @return float|false */
function disk_free_space(string $directory) {}

/** @return float|false */
function diskfreespace(string $directory) {}

function realpath_cache_get(): array {}

function realpath_cache_size(): int {}

/* info.c */

function phpinfo(int $what = INFO_ALL): bool {}

/** @return string|false */
function phpversion(string $extension = UNKNOWN) {}

function phpcredits(int $flag = CREDITS_ALL): bool {}

/** @return string|false */
function php_sapi_name() {}

function php_uname(string $mode = "a"): string {}

/** @return string|false */
function php_ini_scanned_files() {}

/** @return string|false */
function php_ini_loaded_file() {}

/* iptc.c */

/** @return string|bool */
function iptcembed(string $iptcdata, string $jpeg_file_name, int $spool = 0) {}

/** @return array|false */
function iptcparse(string $iptcblock) {}

/* levenshtein.c */

function levenshtein(string $str1, string $str2, $cost_ins = UNKNOWN, int $cost_rep = UNKNOWN, int $cost_del = UNKNOWN): int {}

/* link.c */

#if defined(HAVE_SYMLINK) || defined(PHP_WIN32)
/** @return string|false */
function readlink(string $path) {}

/** @return int|false */
function linkinfo(string $path) {}

function symlink(string $target, string $link): bool {}

function link(string $target, string $link): bool {}
#endif

/* mail.c */

function ezmlm_hash(string $str): int {}

/** @param string|array $additional_headers */
function mail(string $to, string $subject, string $message, $additional_headers = UNKNOWN, string $additional_parameters = ""): bool {}

/* math.c */

/**
 * @param int|float $number
 * @return int|float
 */
function abs($number) {}

function ceil(float $number): float {}

function floor(float $number): float {}

function round($number, int $precision = 0, int $mode = PHP_ROUND_HALF_UP): float {}

function sin(float $number): float {}

function cos(float $number): float {}

function tan(float $number): float {}

function asin(float $number): float {}

function acos(float $number): float {}

function atan(float $number): float {}

function atanh(float $number): float {}

function atan2(float $y, float $x): float {}

function sinh(float $number): float {}

function cosh(float $number): float {}

function tanh(float $number): float {}

function asinh(float $number): float {}

function acosh(float $number): float {}

function expm1(float $number): float {}

function log1p(float $number): float {}

function pi(): float {}

function is_finite(float $number): bool {}

function is_nan(float $number): bool {}

function intdiv(int $dividend, int $divisor): int {}

function is_infinite(float $number): bool {}

/** @return mixed */
function pow($base, $exp) {}

function exp(float $number): float {}

/** @return float|false */
function log(float $number, float $base = M_E) {}

function log10(float $number): float {}

function sqrt(float $number): float {}

function hypot(float $x, float $y): float {}

function deg2rad(float $number): float {}

function rad2deg(float $number): float {}

/** @return int|float */
function bindec(string $binary_string) {}

/** @return int|float */
function hexdec(string $hex_string) {}

/** @return int|float */
function octdec(string $octal_string) {}

function decbin($number): string {}

function decoct($number): string {}

function dechex($number): string {}

function base_convert($number, int $frombase, int $tobase): string {}

function number_format(float $number, int $decimals = 0, ?string $decimal_point = "." , ?string $thousands_separator = ","): string {}

function fmod(float $x, float $y): float {}

function fdiv(float $dividend, float $divisor): float {}

/* microtime.c */

#ifdef HAVE_GETTIMEOFDAY
/** @return string|float */
function microtime(bool $get_as_float = false) {}

/** @return array|float */
function gettimeofday(bool $return_float = false) {}
#endif

#ifdef HAVE_GETRUSAGE
/** @return array|false */
function getrusage(int $who = 0) {}
#endif

/* pack.c */

/** @return string|false */
function pack(string $format, ...$args) {}

/** @return array|false */
function unpack(string $format, string $data, int $offset = 0) {}

/* password.c */

function password_get_info(string $hash): ?array {}

/** @return string|false */
function password_hash(string $password, $algo, array $options = []) {}

function password_needs_rehash(string $hash, $algo, array $options = []): bool {}

function password_verify(string $password, string $hash): bool {}

function password_algos(): array {}

/* proc_open.c */

#ifdef PHP_CAN_SUPPORT_PROC_OPEN
/** @return resource|false */
function proc_open($cmd, array $descriptorspec, &$pipes, ?string $cwd = null, ?array $env = null, ?array $other_options = null) {}

/** @param resource $process */
function proc_close($process): int {}

/** @param resource $process */
function proc_terminate($process, int $signal = SIGTERM): bool {}

/** @param resource $process */
function proc_get_status($process): array {}
#endif

/* quot_print.c */

function quoted_printable_decode(string $str): string {}

function quoted_printable_encode(string $str): string {}

/* mt_rand.c */

function mt_srand(int $seed = 0, int $mode = MT_RAND_MT19937): void {}

function mt_rand(int $min = 0, int $max = PHP_INT_MAX): int {}

function mt_getrandmax(): int {}

/* random.c */

function random_bytes(int $length): string {}

function random_int(int $min, int $max): int {}
