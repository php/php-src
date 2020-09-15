<?php

/** @generate-function-entries */

/* main/main.c */

function set_time_limit(int $seconds): bool {}

/* main/SAPI.c */

function header_register_callback(callable $callback): bool {}

/* main/output.c */

/** @param callable $user_function */
function ob_start($user_function = null, int $chunk_size = 0, int $flags = PHP_OUTPUT_HANDLER_STDFLAGS): bool {}

function ob_flush(): bool {}

function ob_clean(): bool {}

function ob_end_flush(): bool {}

function ob_end_clean(): bool {}

function ob_get_flush(): string|false {}

function ob_get_clean(): string|false {}

function ob_get_contents(): string|false {}

function ob_get_level(): int {}

function ob_get_length(): int|false {}

function ob_list_handlers(): array {}

function ob_get_status(bool $full_status = false): array {}

// TODO: Shouldn't this be a bool argument?
function ob_implicit_flush(int $flag = 1): void {}

function output_reset_rewrite_vars(): bool {}

function output_add_rewrite_var(string $name, string $value): bool {}

/* main/streams/userspace.c */

function stream_wrapper_register(string $protocol, string $classname, int $flags = 0): bool {}

/** @alias stream_wrapper_register */
function stream_register_wrapper(string $protocol, string $classname, int $flags = 0): bool {}

function stream_wrapper_unregister(string $protocol): bool {}

function stream_wrapper_restore(string $protocol): bool {}

/* array.c */

function array_push(array &$stack, mixed ...$values): int {}

function krsort(array &$array, int $flags = SORT_REGULAR): bool {}

function ksort(array &$array, int $flags = SORT_REGULAR): bool {}

/** @param Countable|array $var */
function count($var, int $mode = COUNT_NORMAL): int {}

/**
 * @param Countable|array $var
 * @alias count
 */
function sizeof($var, int $mode = COUNT_NORMAL): int {}

function natsort(array &$array): bool {}

function natcasesort(array &$array): bool {}

function asort(array &$array, int $flags = SORT_REGULAR): bool {}

function arsort(array &$array, int $flags = SORT_REGULAR): bool {}

function sort(array &$array, int $flags = SORT_REGULAR): bool {}

function rsort(array &$array, int $flags = SORT_REGULAR): bool {}

function usort(array &$array, callable $callback): bool {}

function uasort(array &$array, callable $callback): bool {}

function uksort(array &$array, callable $callback): bool {}

function end(array|object &$arg): mixed {}

function prev(array|object &$arg): mixed {}

function next(array|object &$arg): mixed {}

function reset(array|object &$arg): mixed {}

function current(array|object $arg): mixed {}

/** @alias current */
function pos(array|object $arg): mixed {}

function key(array|object $arg): int|string|null {}

function min(mixed $arg, mixed ...$args): mixed {}

function max(mixed $arg, mixed ...$args): mixed {}

function array_walk(array|object &$input, callable $callback, mixed $argument = UNKNOWN): bool {}

function array_walk_recursive(array|object &$input, callable $callback, mixed $argument = UNKNOWN): bool {}

function in_array(mixed $needle, array $haystack, bool $strict = false): bool {}

function array_search(mixed $needle, array $haystack, bool $strict = false): int|string|false {}

/** @prefer-ref $array */
function extract(array &$array, int $extract_type = EXTR_OVERWRITE, string $prefix = ""): int {}

/**
 * @param array|string $var_name
 * @param array|string $var_names
 */
function compact($var_name, ...$var_names): array {}

function array_fill(int $start_key, int $count, mixed $value): array {}

function array_fill_keys(array $keys, mixed $value): array {}

/**
 * @param string|int|float $low
 * @param string|int|float $high
 */
function range($low, $high, int|float $step = 1): array {}

function shuffle(array &$array): bool {}

function array_pop(array &$stack): mixed {}

function array_shift(array &$stack): mixed {}

function array_unshift(array &$stack, mixed ...$values): int {}

function array_splice(array &$array, int $offset, ?int $length = null, mixed $replacements = []): array {}

function array_slice(array $array, int $offset, ?int $length = null, bool $preserve_keys = false): array {}

function array_merge(array ...$arrays): array {}

function array_merge_recursive(array ...$arrays): array {}

function array_replace(array $array, array ...$replacements): array {}

function array_replace_recursive(array $array, array ...$replacements): array {}

function array_keys(array $array, mixed $filter_value = UNKNOWN, bool $strict = false): array {}

function array_key_first(array $array): int|string|null {}

function array_key_last(array $array): int|string|null {}

function array_values(array $array): array {}

function array_count_values(array $array): array {}

function array_column(array $array, int|string|null $column_key, int|string|null $index_key = null): array {}

function array_reverse(array $array, bool $preserve_keys = false): array {}

function array_pad(array $array, int $pad_size, mixed $pad_value): array {}

function array_flip(array $array): array {}

function array_change_key_case(array $array, int $case = CASE_LOWER): array {}

function array_unique(array $array, int $flags = SORT_STRING): array {}

function array_intersect_key(array $array, array ...$arrays): array {}

/** @param array|callable $rest */
function array_intersect_ukey(array $array, ...$rest): array {}

function array_intersect(array $array, array ...$arrays): array {}

/** @param array|callable $rest */
function array_uintersect(array $array, ...$rest): array {}

function array_intersect_assoc(array $array, array ...$arrays): array {}

/** @param array|callable $rest */
function array_uintersect_assoc(array $array, ...$rest): array {}

/** @param array|callable $rest */
function array_intersect_uassoc(array $array, ...$rest): array {}

/** @param array|callable $rest */
function array_uintersect_uassoc(array $array, ...$rest): array {}

function array_diff_key(array $array, array ...$arrays): array {}

/** @param array|callable $rest */
function array_diff_ukey(array $array, ...$rest): array {}

function array_diff(array $array, array ...$arrays): array {}

/** @param array|callable $rest */
function array_udiff(array $array, ...$rest): array {}

function array_diff_assoc(array $array, array ...$arrays): array {}

/** @param array|callable $rest */
function array_diff_uassoc(array $array, ...$rest): array {}

/** @param array|callable $rest */
function array_udiff_assoc(array $array, ...$rest): array {}

/** @param array|callable $rest */
function array_udiff_uassoc(array $array, ...$rest): array {}

/**
 * @param array $array1
 * @param array|int $rest
 * @prefer-ref $array1
 * @prefer-ref $rest
 */
function array_multisort(&$array1, &...$rest): bool {}

function array_rand(array $array, int $num_req = 1): int|string|array {}

function array_sum(array $array): int|float {}

function array_product(array $array): int|float {}

function array_reduce(array $array, callable $callback, mixed $initial = null): mixed {}

function array_filter(array $array, ?callable $callback = null, int $use_keys = 0): array {}

function array_map(?callable $callback, array $array, array ...$arrays): array {}

/** @param string|int $key */
function array_key_exists($key, array $array): bool {}

/**
 * @param string|int $key
 * @alias array_key_exists
 */
function key_exists($key, array $array): bool {}

function array_chunk(array $array, int $size, bool $preserve_keys = false): array {}

function array_combine(array $keys, array $values): array {}

/* base64.c */

function base64_encode(string $str): string {}

function base64_decode(string $str, bool $strict = false): string|false {}

/* basic_functions.c */

function constant(string $name): mixed {}

function ip2long(string $ip_address): int|false {}

function long2ip(int $proper_address): string|false {}

function getenv(?string $variable = null, bool $local_only = false): string|array|false {}

#ifdef HAVE_PUTENV
function putenv(string $setting): bool {}
#endif

/** @param int $optind */
function getopt(string $options, array $longopts = [], &$optind = null): array|false {}

function flush(): void {}

function sleep(int $seconds): int {}

function usleep(int $microseconds): void {}

#if HAVE_NANOSLEEP
function time_nanosleep(int $seconds, int $nanoseconds): array|bool {}

function time_sleep_until(float $timestamp): bool {}
#endif

function get_current_user(): string {}

function get_cfg_var(string $option_name): string|array|false {}

function error_log(string $message, int $message_type = 0, ?string $destination = null, ?string $extra_headers = null): bool {}

function error_get_last(): ?array {}

function error_clear_last(): void {}

function call_user_func(callable $function, mixed ...$args): mixed {}

function call_user_func_array(callable $function, array $args): mixed {}

function forward_static_call(callable $function, mixed ...$args): mixed {}

function forward_static_call_array(callable $function, array $args): mixed {}

/** @param callable $function */
function register_shutdown_function($function, mixed ...$args): ?bool {}

function highlight_file(string $filename, bool $return = false): string|bool {}

/** @alias highlight_file */
function show_source(string $filename, bool $return = false): string|bool {}

function php_strip_whitespace(string $filename): string {}

function highlight_string(string $string, bool $return = false): string|bool {}

function ini_get(string $varname): string|false {}

function ini_get_all(?string $extension = null, bool $details = true): array|false {}

function ini_set(string $varname, string $value): string|false {}

/** @alias ini_set */
function ini_alter(string $varname, string $value): string|false {}

function ini_restore(string $varname): void {}

function set_include_path(string $include_path): string|false {}

function get_include_path(): string|false {}

function print_r(mixed $var, bool $return = false): string|bool {}

function connection_aborted(): int {}

function connection_status(): int {}

function ignore_user_abort(?bool $value = null): int {}

#if HAVE_GETSERVBYNAME
function getservbyname(string $service, string $protocol): int|false {}
#endif

#if HAVE_GETSERVBYPORT
function getservbyport(int $port, string $protocol): string|false {}
#endif

#if HAVE_GETPROTOBYNAME
function getprotobyname(string $name): int|false {}
#endif

#if HAVE_GETPROTOBYNUMBER
function getprotobynumber(int $protocol): string|false {}
#endif

function register_tick_function(callable $function, mixed ...$args): bool {}

function unregister_tick_function(callable $function): void {}

function is_uploaded_file(string $path): bool {}

function move_uploaded_file(string $path, string $new_path): bool {}

function parse_ini_file(string $filename, bool $process_sections = false, int $scanner_mode = INI_SCANNER_NORMAL): array|false {}

function parse_ini_string(string $ini_string, bool $process_sections = false, int $scanner_mode = INI_SCANNER_NORMAL): array|false {}

#if ZEND_DEBUG
function config_get_hash(): array {}
#endif

#ifdef HAVE_GETLOADAVG
function sys_getloadavg(): array|false {}
#endif

/* browscap.c */

function get_browser(?string $browser_name = null, bool $return_array = false): object|array|false {}

/* crc32.c */

function crc32(string $str): int {}

/* crypt.c */

function crypt(string $str, string $salt): string {}

/* datetime.c */

#if HAVE_STRPTIME
function strptime(string $timestamp, string $format): array|false {}
#endif

/* dns.c */

#ifdef HAVE_GETHOSTNAME
function gethostname(): string|false {}
#endif

function gethostbyaddr(string $ip_address): string|false {}

function gethostbyname(string $hostname): string {}

function gethostbynamel(string $hostname): array|false {}

#if defined(PHP_WIN32) || HAVE_DNS_SEARCH_FUNC
function dns_check_record(string $hostname, string $type = "MX"): bool {}

/** @alias dns_check_record */
function checkdnsrr(string $hostname, string $type = "MX"): bool {}

/**
 * @param array $authns
 * @param array $addtl
 */
function dns_get_record(string $hostname, int $type = DNS_ANY, &$authns = null, &$addtl = null, bool $raw = false): array|false {}

/**
 * @param array $mxhosts
 * @param array $weight
 */
function dns_get_mx(string $hostname, &$mxhosts, &$weight = null): bool {}

/**
 * @param array $mxhosts
 * @param array $weight
 * @alias dns_get_mx
 */
function getmxrr(string $hostname, &$mxhosts, &$weight = null): bool {}
#endif

/* net.c */

function net_get_interfaces(): array|false {}

/* ftok.c */

#if HAVE_FTOK
function ftok(string $pathname, string $proj): int {}
#endif

/* hrtime.c */

function hrtime(bool $get_as_number = false): array|int|float|false {}

/* lcg.c */

function lcg_value(): float {}

/* md5.c */

function md5(string $str, bool $raw_output = false): string {}

function md5_file(string $filename, bool $raw_output = false): string|false {}

/* pageinfo.c */

function getmyuid(): int|false {}

function getmygid(): int|false {}

function getmypid(): int|false {}

function getmyinode(): int|false {}

function getlastmod(): int {}

/* sha1.c */

function sha1(string $str, bool $raw_output = false): string {}

function sha1_file(string $filename, bool $raw_output = false): string|false {}

/* syslog.c */

#ifdef HAVE_SYSLOG_H
function openlog(string $ident, int $option, int $facility): bool {}

function closelog(): bool {}

function syslog(int $priority, string $message): bool {}
#endif

#ifdef HAVE_INET_NTOP
function inet_ntop(string $in_addr): string|false {}
#endif

#ifdef HAVE_INET_PTON
function inet_pton(string $ip_address): string|false {}
#endif

/* metaphone.c */

function metaphone(string $text, int $phones = 0): string|false {}

/* {{{ head.c */
function header(string $string, bool $replace = true, int $http_response_code = 0): void {}

function header_remove(?string $name = null): void {}

function setrawcookie(string $name, string $value = '', array|int $expires_or_options = 0, string $path = '', string $domain = '', bool $secure = false, bool $httponly = false): bool {}

function setcookie(string $name, string $value = '', array|int $expires_or_options = 0, string $path = '', string $domain = '', bool $secure = false, bool $httponly = false): bool {}

function http_response_code(int $response_code = 0): int|bool {}

/**
 * @param string $file
 * @param int $line
 */
function headers_sent(&$file = null, &$line = null): bool {}

function headers_list(): array {}

/* {{{ html.c */

function htmlspecialchars(string $string, int $quote_style = ENT_COMPAT, ?string $encoding = null, bool $double_encode = true): string {}

function htmlspecialchars_decode(string $string, int $quote_style = ENT_COMPAT): string|false {}

function html_entity_decode(string $string, int $quote_style = ENT_COMPAT, ?string $encoding = null): string|false {}

function htmlentities(string $string, int $quote_style = ENT_COMPAT, ?string $encoding = null, bool $double_encode = true): string {}

function get_html_translation_table(int $table = HTML_SPECIALCHARS, int $quote_style = ENT_COMPAT, string $encoding = "UTF-8"): array {}

/* }}} */

/* assert.c */

function assert(mixed $assertion, Throwable|string|null $description = null): bool {}

/** @param string|callable|null $value */
function assert_options(int $what, $value = UNKNOWN): array|object|int|string|null {}

/* string.c */

function bin2hex(string $data): string {}

function hex2bin(string $data): string|false {}

function strspn(string $str, string $mask, int $start = 0, ?int $len = null): int|false {}

function strcspn(string $str, string $mask, int $start = 0, ?int $len = null): int|false {}

#if HAVE_NL_LANGINFO
function nl_langinfo(int $item): string|false {}
#endif

function strcoll(string $str1, string $str2): int {}

function trim(string $str, string $character_mask = " \n\r\t\v\0"): string {}

function rtrim(string $str, string $character_mask = " \n\r\t\v\0"): string {}

/** @alias rtrim */
function chop(string $str, string $character_mask = " \n\r\t\v\0"): string {}

function ltrim(string $str, string $character_mask = " \n\r\t\v\0"): string {}

function wordwrap(string $str, int $width = 75, string $break = "\n", bool $cut = false): string {}

function explode(string $separator, string $str, int $limit = PHP_INT_MAX): array {}

function implode(string|array $glue, ?array $pieces = null): string {}

/** @alias implode */
function join(string|array $glue, ?array $pieces = null): string {}

function strtok(string $str, ?string $token = null): string|false {}

function strtoupper(string $str): string {}

function strtolower(string $str): string {}

function basename(string $path, string $suffix = ""): string {}

function dirname(string $path, int $levels = 1): string {}

function pathinfo(string $path, int $options = PATHINFO_ALL): array|string {}

function stristr(string $haystack, string $needle, bool $before_needle = false): string|false {}

function strstr(string $haystack, string $needle, bool $before_needle = false): string|false {}

/** @alias strstr */
function strchr(string $haystack, string $needle, bool $before_needle = false): string|false {}

function strpos(string $haystack, string $needle, int $offset = 0): int|false {}

function stripos(string $haystack, string $needle, int $offset = 0): int|false {}

function strrpos(string $haystack, string $needle, int $offset = 0): int|false {}

function strripos(string $haystack, string $needle, int $offset = 0): int|false {}

function strrchr(string $haystack, string $needle): string|false {}

function str_contains(string $haystack, string $needle): bool {}

function str_starts_with(string $haystack, string $needle): bool {}

function str_ends_with(string $haystack, string $needle): bool {}

function chunk_split(string $str, int $chunklen = 76, string $ending = "\r\n"): string {}

function substr(string $str, int $start, ?int $length = null): string|false {}

function substr_replace(array|string $str, array|string $replace, array|int $start, array|int|null $length = null): string|array|false {}

function quotemeta(string $str): string {}

function ord(string $character): int {}

function chr(int $codepoint): string {}

function ucfirst(string $str): string {}

function lcfirst(string $str): string {}

function ucwords(string $str, string $delimiters = " \t\r\n\f\v"): string {}

function strtr(string $str, string|array $from, ?string $to = null): string {}

function strrev(string $str): string {}

/** @param float $percent */
function similar_text(string $str1, string $str2, &$percent = null): int {}

function addcslashes(string $str, string $charlist): string {}

function addslashes(string $str): string {}

function stripcslashes(string $str): string {}

function stripslashes(string $str): string {}

/** @param int $replace_count */
function str_replace(array|string $search, array|string $replace, string|array $subject, &$replace_count = null): string|array {}

/** @param int $replace_count */
function str_ireplace(array|string $search, array|string $replace, string|array $subject, &$replace_count = null): string|array {}

function hebrev(string $str, int $max_chars_per_line = 0): string {}

function nl2br(string $str, bool $is_xhtml = true): string {}

function strip_tags(string $str, array|string|null $allowable_tags = null): string {}

/**
 * @param array|string $locales
 * @param string $rest
 */
function setlocale(int $category, $locales, ...$rest): string|false {}

/** @param array $result */
function parse_str(string $encoded_string, &$result): void {}

function str_getcsv(string $string, string $delimiter = ',', string $enclosure = '"', string $escape = '\\'): array {}

function str_repeat(string $input, int $mult): string {}

function count_chars(string $input, int $mode = 0): array|string {}

function strnatcmp(string $s1, string $s2): int {}

function localeconv(): array {}

function strnatcasecmp(string $s1, string $s2): int {}

function substr_count(string $haystack, string $needle, int $offset = 0, ?int $length = null): int {}

function str_pad(string $input, int $pad_length, string $pad_string = " ", int $pad_type = STR_PAD_RIGHT): string {}

function sscanf(string $str, string $format, mixed &...$vars): array|int|null {}

function str_rot13(string $str): string {}

function str_shuffle(string $str): string {}

function str_word_count(string $str, int $format = 0, ?string $charlist = null): array|int {}

function str_split(string $str, int $split_length = 1): array {}

function strpbrk(string $haystack, string $char_list): string|false {}

function substr_compare(string $main_str, string $str, int $offset, ?int $length = null, bool $case_insensitivity = false): int|false {}

function utf8_encode(string $data): string {}

function utf8_decode(string $data): string {}

/**
 * @param resource|null $context
 * @return resource|false
 */
function opendir(string $path, $context = null) {}

/** @param resource $context */
function getdir(string $path, $context = null): Directory|false {}

/**
 * @param resource|null $context
 * @alias getdir
 */
function dir(string $path, $context = null): Directory|false {}

/** @param resource|null $dir_handle */
function closedir($dir_handle = null): void {}

function chdir(string $directory): bool {}

#if defined(HAVE_CHROOT) && !defined(ZTS) && ENABLE_CHROOT_FUNC
function chroot(string $directory): bool {}
#endif

function getcwd(): string|false {}

/** @param resource|null $dir_handle */
function rewinddir($dir_handle = null): void {}

/** @param resource|null $dir_handle */
function readdir($dir_handle = null): string|false {}

/** @param resource|null $context */
function scandir(string $directory, int $sorting_order = 0, $context = null): array|false {}

#ifdef HAVE_GLOB
function glob(string $pattern, int $flags = 0): array|false {}
#endif

/* exec.c */

/**
 * @param array $output
 * @param int $result_code
 */
function exec(string $command, &$output = null, &$result_code = null): string|false {}

/** @param int $result_code */
function system(string $command, &$result_code = null): string|false {}

/** @param int $result_code */
function passthru(string $command, &$result_code = null): bool|null {}

function escapeshellcmd(string $command): string {}

function escapeshellarg(string $arg): string {}

function shell_exec(string $command): string|false|null {}

#ifdef HAVE_NICE
function proc_nice(int $priority): bool {}
#endif

/* file.c */

/**
 * @param resource $handle
 * @param int $wouldblock
 */
function flock($handle, int $operation, &$wouldblock = null): bool {}

function get_meta_tags(string $filename, bool $use_include_path = false): array|false {}

/** @param resource $handle */
function pclose($handle): int {}

/** @return resource|false */
function popen(string $command, string $mode) {}

/** @param resource|null $context */
function readfile(string $filename, bool $use_include_path = false, $context = null): int|false {}

/** @param resource $handle */
function rewind($handle): bool {}

/** @param resource|null $context */
function rmdir(string $dirname, $context = null): bool {}

function umask(?int $mask = null): int {}

/** @param resource $handle */
function fclose($handle): bool {}

/** @param resource $handle */
function feof($handle): bool {}

/** @param resource $handle */
function fgetc($handle): string|false {}

/** @param resource $handle */
function fgets($handle, ?int $length = null): string|false {}

/** @param resource $handle */
function fread($handle, int $length): string|false {}

/**
 * @param resource|null $context
 * @return resource|false
 */
function fopen(string $filename, string $mode, bool $use_include_path = false, $context = null) {}

/** @param resource $stream */
function fscanf($stream, string $format, mixed &...$args): array|int|false|null {}

/** @param resource $handle */
function fpassthru($handle): int {}

/** @param resource $handle */
function ftruncate($handle, int $size): bool {}

/** @param resource $handle */
function fstat($handle): array|false {}

/** @param resource $handle */
function fseek($handle, int $offset, int $whence = SEEK_SET): int {}

/** @param resource $handle */
function ftell($handle): int|false {}

/** @param resource $handle */
function fflush($handle): bool {}

/** @param resource $handle */
function fwrite($handle, string $content, ?int $max_length = null): int|false {}

/**
 * @param resource $handle
 * @alias fwrite
 */
function fputs($handle, string $content, ?int $max_length = null): int|false {}

/** @param resource|null $context */
function mkdir(string $pathname, int $mode = 0777, bool $recursive = false, $context = null): bool {}

/** @param resource|null $context */
function rename(string $oldname, string $newname, $context = null): bool {}

/** @param resource|null $context */
function copy(string $source, string $dest, $context = null): bool {}

function tempnam(string $dir, string $prefix): string|false {}

/** @return resource|false */
function tmpfile() {}

/** @param resource|null $context */
function file(string $filename, int $flags = 0, $context = null): array|false {}

/** @param resource|null $context */
function file_get_contents(string $filename, bool $use_include_path = false, $context = null, int $offset = 0, ?int $maxlen = null): string|false {}

/** @param resource|null $context */
function unlink(string $filename, $context = null): bool {}

/** @param resource|null $context */
function file_put_contents(string $filename, mixed $content, int $flags = 0, $context = null): int|false {}

/** @param resource $handle */
function fputcsv($handle, array $fields, string $delimiter = ",", string $enclosure = "\"", string $escape = "\\"): int|false {}

/** @param resource $handle */
function fgetcsv($handle, ?int $length = null, string $delimiter = ",", string $enclosure = '"', string $escape = "\\"): array|false {}

function realpath(string $path): string|false {}

#ifdef HAVE_FNMATCH
function fnmatch(string $pattern, string $filename, int $flags = 0): bool {}
#endif

function sys_get_temp_dir(): string {}

/* filestat.c */

function fileatime(string $filename): int|false {}

function filectime(string $filename): int|false {}

function filegroup(string $filename): int|false {}

function fileinode(string $filename): int|false {}

function filemtime(string $filename): int|false {}

function fileowner(string $filename): int|false {}

function fileperms(string $filename): int|false {}

function filesize(string $filename): int|false {}

function filetype(string $filename): string|false {}

function file_exists(string $filename): bool {}

function is_writable(string $filename): bool {}

/** @alias is_writable */
function is_writeable(string $filename): bool {}

function is_readable(string $filename): bool {}

function is_executable(string $filename): bool {}

function is_file(string $filename): bool {}

function is_dir(string $filename): bool {}

function is_link(string $filename): bool {}

function stat(string $filename): array|false {}

function lstat(string $filename): array|false {}

function chown(string $filename, string|int $user): bool {}

function chgrp(string $filename, string|int $group): bool {}

#if HAVE_LCHOWN
function lchown(string $filename, string|int $user): bool {}

function lchgrp(string $filename, string|int $group): bool {}
#endif

function chmod(string $filename, int $mode): bool {}

#if HAVE_UTIME
function touch(string $filename, ?int $time = null, ?int $atime = null): bool {}
#endif

function clearstatcache(bool $clear_realpath_cache = false, string $filename = ""): void {}

function disk_total_space(string $directory): float|false {}

function disk_free_space(string $directory): float|false {}

/** @alias disk_free_space */
function diskfreespace(string $directory): float|false {}

function realpath_cache_get(): array {}

function realpath_cache_size(): int {}

/* formatted_print.c */

function sprintf(string $format, mixed ...$args): string {}

function printf(string $format, mixed ...$args): int {}

function vprintf(string $format, array $args): int {}

function vsprintf(string $format, array $args): string {}

/** @param resource $handle */
function fprintf($handle, string $format, mixed ...$args): int {}

/** @param resource $handle */
function vfprintf($handle, string $format, array $args): int {}

/* fsock.c */

/**
 * @param int $errno
 * @param string $errstr
 * @return resource|false
 */
function fsockopen(string $hostname, int $port = -1, &$errno = null, &$errstr = null, ?float $timeout = null) {}

/**
 * @param int $errno
 * @param string $errstr
 * @return resource|false
 */
function pfsockopen(string $hostname, int $port = -1, &$errno = null, &$errstr = null, ?float $timeout = null) {}

/* http.c */

function http_build_query(array|object $data, string $numeric_prefix = "", ?string $arg_separator = null, int $enc_type = PHP_QUERY_RFC1738): string|false {}

/* image.c */

function image_type_to_mime_type(int $image_type): string {}

function image_type_to_extension(int $image_type, bool $include_dot = true): string|false {}

/** @param array $image_info */
function getimagesize(string $image_path, &$image_info = null): array|false {}

/** @param array $image_info */
function getimagesizefromstring(string $image, &$image_info = null): array|false {}

/* info.c */

function phpinfo(int $what = INFO_ALL): bool {}

function phpversion(?string $extension = null): string|false {}

function phpcredits(int $flag = CREDITS_ALL): bool {}

function php_sapi_name(): string|false {}

function php_uname(string $mode = "a"): string {}

function php_ini_scanned_files(): string|false {}

function php_ini_loaded_file(): string|false {}

/* iptc.c */

function iptcembed(string $iptcdata, string $jpeg_file_name, int $spool = 0): string|bool {}

function iptcparse(string $iptcblock): array|false {}

/* levenshtein.c */

function levenshtein(string $str1, string $str2, int $cost_ins = 1, int $cost_rep = 1, int $cost_del = 1): int {}

/* link.c */

#if defined(HAVE_SYMLINK) || defined(PHP_WIN32)
function readlink(string $path): string|false {}

function linkinfo(string $path): int|false {}

function symlink(string $target, string $link): bool {}

function link(string $target, string $link): bool {}
#endif

/* mail.c */

function mail(string $to, string $subject, string $message, array|string $additional_headers = [], string $additional_parameters = ""): bool {}

/* math.c */

function abs(int|float $number): int|float {}

function ceil(int|float $number): float {}

function floor(int|float $number): float {}

function round(int|float $number, int $precision = 0, int $mode = PHP_ROUND_HALF_UP): float {}

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

function pow(mixed $base, mixed $exp): int|float|object {}

function exp(float $number): float {}

function log(float $number, float $base = M_E): float {}

function log10(float $number): float {}

function sqrt(float $number): float {}

function hypot(float $x, float $y): float {}

function deg2rad(float $number): float {}

function rad2deg(float $number): float {}

function bindec(string $binary_string): int|float {}

function hexdec(string $hex_string): int|float {}

function octdec(string $octal_string): int|float {}

function decbin(int $number): string {}

function decoct(int $number): string {}

function dechex(int $number): string {}

function base_convert(string $number, int $frombase, int $tobase): string {}

function number_format(float $number, int $decimals = 0, ?string $decimal_point = "." , ?string $thousands_separator = ","): string {}

function fmod(float $x, float $y): float {}

function fdiv(float $dividend, float $divisor): float {}

/* microtime.c */

#ifdef HAVE_GETTIMEOFDAY
function microtime(bool $getAsFloat = false): string|float {}

function gettimeofday(bool $returnFloat = false): array|float {}
#endif

#ifdef HAVE_GETRUSAGE
function getrusage(int $who = 0): array|false {}
#endif

/* pack.c */

function pack(string $format, mixed ...$args): string|false {}

function unpack(string $format, string $data, int $offset = 0): array|false {}

/* password.c */

function password_get_info(string $hash): ?array {}

function password_hash(string $password, string|int|null $algo, array $options = []): string {}

function password_needs_rehash(string $hash, string|int|null $algo, array $options = []): bool {}

function password_verify(string $password, string $hash): bool {}

function password_algos(): array {}

/* proc_open.c */

#ifdef PHP_CAN_SUPPORT_PROC_OPEN
/**
 * @param array $pipes
 * @return resource|false
 */
function proc_open(array|string $cmd, array $descriptorspec, &$pipes, ?string $cwd = null, ?array $env = null, ?array $other_options = null) {}

/** @param resource $process */
function proc_close($process): int {}

/** @param resource $process */
function proc_terminate($process, int $signal = 15): bool {}

/** @param resource $process */
function proc_get_status($process): array {}
#endif

/* quot_print.c */

function quoted_printable_decode(string $str): string {}

function quoted_printable_encode(string $str): string {}

/* mt_rand.c */

function mt_srand(int $seed = 0, int $mode = MT_RAND_MT19937): void {}

/** @alias mt_srand */
function srand(int $seed = 0, int $mode = MT_RAND_MT19937): void {}

function rand(int $min = UNKNOWN, int $max = UNKNOWN): int {}

function mt_rand(int $min = UNKNOWN, int $max = UNKNOWN): int {}

function mt_getrandmax(): int {}

/** @alias mt_getrandmax */
function getrandmax(): int {}

/* random.c */

function random_bytes(int $length): string {}

function random_int(int $min, int $max): int {}

/* soundex.c */

function soundex(string $string): string|false {}

/* streamsfuncs.c */

function stream_select(?array &$read, ?array &$write, ?array &$except, ?int $tv_sec, int $tv_usec = 0): int|false {}

/** @return resource */
function stream_context_create(?array $options = null, ?array $params = null) {}

/** @param resource $context */
function stream_context_set_params($context, array $params): bool {}

/** @param resource $context */
function stream_context_get_params($context): array {}

/** @param resource $context */
function stream_context_set_option($context, array|string $wrapper_or_options, ?string $option_name = null, mixed $value = UNKNOWN): bool {}

/** @param resource $stream_or_context */
function stream_context_get_options($stream_or_context): array {}

/** @return resource */
function stream_context_get_default(?array $options = null) {}

/** @return resource */
function stream_context_set_default(array $options) {}

/**
 * @param resource $stream
 * @return resource|false
 */
function stream_filter_prepend($stream, string $filtername, int $read_write = 0, mixed $params = UNKNOWN) {}

/**
 * @param resource $stream
 * @return resource|false
 */
function stream_filter_append($stream, string $filtername, int $read_write = 0, mixed $params = UNKNOWN) {}

/** @param resource $stream_filter */
function stream_filter_remove($stream_filter): bool {}

/**
 * @param int $errno
 * @param string $errstr
 * @param resource|null $context
 * @return resource|false
 */
function stream_socket_client(string $remote_socket, &$errno = null, &$errstr = null, ?float $timeout = null, int $flags = STREAM_CLIENT_CONNECT, $context = null) {}

/**
 * @param int $errno
 * @param string $errstr
 * @param resource|null $context
 * @return resource|false
 */
function stream_socket_server(string $local_socket, &$errno = null, &$errstr = null, int $flags = STREAM_SERVER_BIND | STREAM_SERVER_LISTEN, $context = null) {}

/**
 * @param resource $server_socket
 * @param float $timeout
 * @param string $peername
 * @return resource|false
 */
function stream_socket_accept($server_socket, ?float $timeout = null, &$peername = null) {}

/** @param resource $handle */
function stream_socket_get_name($handle, bool $want_peer): string|false {}

/**
 * @param resource $socket
 * @param string|null $address
 */
function stream_socket_recvfrom($socket, int $length, int $flags = 0, &$address = null): string|false {}

/** @param resource $socket */
function stream_socket_sendto($socket, string $data, int $flags = 0, string $address = ""): int|false {}

/**
 * @param resource $stream
 * @param resource|null $session_stream
 */
function stream_socket_enable_crypto($stream, bool $enable, ?int $crypto_type = null, $session_stream = null): int|bool {}

#ifdef HAVE_SHUTDOWN
/** @param resource $stream */
function stream_socket_shutdown($stream, int $how): bool {}
#endif

#if HAVE_SOCKETPAIR
function stream_socket_pair(int $domain, int $type, int $protocol): array|false {}
#endif

/**
 * @param resource $source
 * @param resource $dest
 */
function stream_copy_to_stream($source, $dest, ?int $maxlength = null, int $position = 0): int|false {}

/** @param resource $handle */
function stream_get_contents($handle, ?int $maxlength = null, int $position = -1): string|false {}

/** @param resource $stream */
function stream_supports_lock($stream): bool {}

/** @param resource $stream */
function stream_set_write_buffer($stream, int $buffer): int {}

/**
 * @param resource $stream
 * @alias stream_set_write_buffer */
function set_file_buffer($stream, int $buffer): int {}

/** @param resource $stream */
function stream_set_read_buffer($stream, int $buffer): int {}

/** @param resource $stream */
function stream_set_blocking($stream, bool $mode): bool {}

/**
 * @param resource $stream
 * @alias stream_set_blocking
 */
function socket_set_blocking($stream, bool $mode): bool {}

/** @param resource $stream */
function stream_get_meta_data($stream): array {}

/**
 * @param resource $stream
 * @alias stream_get_meta_data
 */
function socket_get_status($stream): array {}

/** @param resource $handle */
function stream_get_line($handle, int $max_length, string $ending = ""): string|false {}

function stream_resolve_include_path(string $filename): string|false {}

function stream_get_wrappers(): array|false {}

function stream_get_transports(): array|false {}

/** @param resource|string $stream */
function stream_is_local($stream): bool {}

/** @param resource $stream */
function stream_isatty($stream): bool {}

#ifdef PHP_WIN32
/** @param resource $stream */
function sapi_windows_vt100_support($stream, ?bool $enable = null): bool {}
#endif

/** @param resource $stream */
function stream_set_chunk_size($stream, int $size): int {}

#if HAVE_SYS_TIME_H || defined(PHP_WIN32)
/** @param resource $socket */
function stream_set_timeout($socket, int $seconds, int $microseconds = 0): bool {}

/**
 * @param resource $socket
 * @alias stream_set_timeout
 */
function socket_set_timeout($socket, int $seconds, int $microseconds = 0): bool {}
#endif

/* type.c */

function gettype(mixed $var): string {}

function get_debug_type(mixed $var): string {}

function settype(mixed &$var, string $type): bool {}

function intval(mixed $value, int $base = 10): int {}

function floatval(mixed $value): float {}

/** @alias floatval */
function doubleval(mixed $value): float {}

function boolval(mixed $value): bool {}

function strval(mixed $value): string {}

function is_null(mixed $value): bool {}

function is_resource(mixed $value): bool {}

function is_bool(mixed $value): bool {}

function is_int(mixed $value): bool {}

/** @alias is_int */
function is_integer(mixed $value): bool {}

/** @alias is_int */
function is_long(mixed $value): bool {}

function is_float(mixed $value): bool {}

/** @alias is_float */
function is_double(mixed $value): bool {}

function is_numeric(mixed $value): bool {}

function is_string(mixed $value): bool {}

function is_array(mixed $value): bool {}

function is_object(mixed $value): bool {}

function is_scalar(mixed $value): bool {}

/** @param string $callable_name */
function is_callable(mixed $value, bool $syntax_only = false, &$callable_name = null): bool {}

function is_iterable(mixed $value): bool {}

function is_countable(mixed $value): bool {}

/* uniqid.c */

#ifdef HAVE_GETTIMEOFDAY
function uniqid(string $prefix = "", bool $more_entropy = false): string {}
#endif

/* url.c */

/** @return mixed */
function parse_url(string $url, int $component = -1) {}

function urlencode(string $string): string {}

function urldecode(string $string): string {}

function rawurlencode(string $string): string {}

function rawurldecode(string $string): string {}

/** @param resource $context */
function get_headers(string $url, int $format = 0, $context = null): array|false {}

/** @param resource $brigade */
function stream_bucket_make_writeable($brigade): ?object {}

/** @param resource $brigade */
function stream_bucket_prepend($brigade, object $bucket): void {}

/** @param resource $brigade */
function stream_bucket_append($brigade, object $bucket): void {}

/** @param resource $stream */
function stream_bucket_new($stream, string $buffer): object|false {}

function stream_get_filters(): array {}

function stream_filter_register(string $filtername, string $classname): bool {}

/* uuencode.c */

function convert_uuencode(string $data): string|false {}

function convert_uudecode(string $data): string|false {}

/* var.c */

function var_dump(mixed $value, mixed ...$values): void {}

function var_export(mixed $value, bool $return = false): ?string {}

function debug_zval_dump(mixed $value, mixed ...$values): void {}

function serialize(mixed $value): string {}

function unserialize(string $value, array $options = []): mixed {}

function memory_get_usage(bool $real_usage = false): int {}

function memory_get_peak_usage(bool $real_usage = false): int {}

/* versioning.c */

function version_compare(string $version1, string $version2, ?string $operator = null): int|bool {}

/* win32/codepage.c */

#ifdef PHP_WIN32
function sapi_windows_cp_set(int $cp): bool {}

function sapi_windows_cp_get(string $kind = ""): int {}

function sapi_windows_cp_conv(int|string $in_codepage, int|string $out_codepage, string $subject): ?string {}

function sapi_windows_cp_is_utf8(): bool {}

function sapi_windows_set_ctrl_handler(?callable $handler, bool $add = true): bool {}

function sapi_windows_generate_ctrl_event(int $event, int $pid = 0): bool {}
#endif
