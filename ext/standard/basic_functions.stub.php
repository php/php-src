<?php

/** @generate-function-entries */

/* main/main.c */

function set_time_limit(int $seconds): bool {}

/* main/SAPI.c */

function header_register_callback(callable $callback): bool {}

/* main/output.c */

/** @param callable $callback */
function ob_start($callback = null, int $chunk_size = 0, int $flags = PHP_OUTPUT_HANDLER_STDFLAGS): bool {}

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

function ob_implicit_flush(bool $enable = true): void {}

function output_reset_rewrite_vars(): bool {}

function output_add_rewrite_var(string $name, string $value): bool {}

/* main/streams/userspace.c */

function stream_wrapper_register(string $protocol, string $class, int $flags = 0): bool {}

/** @alias stream_wrapper_register */
function stream_register_wrapper(string $protocol, string $class, int $flags = 0): bool {}

function stream_wrapper_unregister(string $protocol): bool {}

function stream_wrapper_restore(string $protocol): bool {}

/* array.c */

function array_push(array &$array, mixed ...$values): int {}

function krsort(array &$array, int $flags = SORT_REGULAR): bool {}

function ksort(array &$array, int $flags = SORT_REGULAR): bool {}

function count(Countable|array $value, int $mode = COUNT_NORMAL): int {}

/** @alias count */
function sizeof(Countable|array $value, int $mode = COUNT_NORMAL): int {}

function natsort(array &$array): bool {}

function natcasesort(array &$array): bool {}

function asort(array &$array, int $flags = SORT_REGULAR): bool {}

function arsort(array &$array, int $flags = SORT_REGULAR): bool {}

function sort(array &$array, int $flags = SORT_REGULAR): bool {}

function rsort(array &$array, int $flags = SORT_REGULAR): bool {}

function usort(array &$array, callable $callback): bool {}

function uasort(array &$array, callable $callback): bool {}

function uksort(array &$array, callable $callback): bool {}

function end(array|object &$array): mixed {}

function prev(array|object &$array): mixed {}

function next(array|object &$array): mixed {}

function reset(array|object &$array): mixed {}

function current(array|object $array): mixed {}

/** @alias current */
function pos(array|object $array): mixed {}

function key(array|object $array): int|string|null {}

function min(mixed $value, mixed ...$values): mixed {}

function max(mixed $value, mixed ...$values): mixed {}

function array_walk(array|object &$array, callable $callback, mixed $arg = UNKNOWN): bool {}

function array_walk_recursive(array|object &$array, callable $callback, mixed $arg = UNKNOWN): bool {}

function in_array(mixed $needle, array $haystack, bool $strict = false): bool {}

function array_search(mixed $needle, array $haystack, bool $strict = false): int|string|false {}

/** @prefer-ref $array */
function extract(array &$array, int $flags = EXTR_OVERWRITE, string $prefix = ""): int {}

/**
 * @param array|string $var_name
 * @param array|string $var_names
 */
function compact($var_name, ...$var_names): array {}

function array_fill(int $start_index, int $count, mixed $value): array {}

function array_fill_keys(array $keys, mixed $value): array {}

/**
 * @param string|int|float $start
 * @param string|int|float $end
 */
function range($start, $end, int|float $step = 1): array {}

function shuffle(array &$array): bool {}

function array_pop(array &$array): mixed {}

function array_shift(array &$array): mixed {}

function array_unshift(array &$array, mixed ...$values): int {}

function array_splice(array &$array, int $offset, ?int $length = null, mixed $replacement = []): array {}

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

function array_pad(array $array, int $length, mixed $value): array {}

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
 * @param array $array
 * @param array|int $rest
 * @prefer-ref $array
 * @prefer-ref $rest
 */
function array_multisort(&$array, &...$rest): bool {}

function array_rand(array $array, int $num = 1): int|string|array {}

function array_sum(array $array): int|float {}

function array_product(array $array): int|float {}

function array_reduce(array $array, callable $callback, mixed $initial = null): mixed {}

function array_filter(array $array, ?callable $callback = null, int $mode = 0): array {}

function array_map(?callable $callback, array $array, array ...$arrays): array {}

/** @param string|int $key */
function array_key_exists($key, array $array): bool {}

/**
 * @param string|int $key
 * @alias array_key_exists
 */
function key_exists($key, array $array): bool {}

function array_chunk(array $array, int $length, bool $preserve_keys = false): array {}

function array_combine(array $keys, array $values): array {}

/* base64.c */

function base64_encode(string $string): string {}

function base64_decode(string $string, bool $strict = false): string|false {}

/* basic_functions.c */

function constant(string $name): mixed {}

function ip2long(string $ip): int|false {}

function long2ip(int $ip): string|false {}

function getenv(?string $name = null, bool $local_only = false): string|array|false {}

#ifdef HAVE_PUTENV
function putenv(string $assignment): bool {}
#endif

/** @param int $rest_index */
function getopt(string $short_options, array $long_options = [], &$rest_index = null): array|false {}

function flush(): void {}

function sleep(int $seconds): int {}

function usleep(int $microseconds): void {}

#if HAVE_NANOSLEEP
function time_nanosleep(int $seconds, int $nanoseconds): array|bool {}

function time_sleep_until(float $timestamp): bool {}
#endif

function get_current_user(): string {}

function get_cfg_var(string $option): string|array|false {}

function error_log(string $message, int $message_type = 0, ?string $destination = null, ?string $additional_headers = null): bool {}

function error_get_last(): ?array {}

function error_clear_last(): void {}

function call_user_func(callable $callback, mixed ...$args): mixed {}

function call_user_func_array(callable $callback, array $args): mixed {}

function forward_static_call(callable $callback, mixed ...$args): mixed {}

function forward_static_call_array(callable $callback, array $args): mixed {}

function register_shutdown_function(callable $callback, mixed ...$args): ?bool {}

function highlight_file(string $filename, bool $return = false): string|bool {}

/** @alias highlight_file */
function show_source(string $filename, bool $return = false): string|bool {}

function php_strip_whitespace(string $filename): string {}

function highlight_string(string $string, bool $return = false): string|bool {}

function ini_get(string $option): string|false {}

function ini_get_all(?string $extension = null, bool $details = true): array|false {}

function ini_set(string $option, string $value): string|false {}

/** @alias ini_set */
function ini_alter(string $option, string $value): string|false {}

function ini_restore(string $option): void {}

function set_include_path(string $include_path): string|false {}

function get_include_path(): string|false {}

function print_r(mixed $value, bool $return = false): string|bool {}

function connection_aborted(): int {}

function connection_status(): int {}

function ignore_user_abort(?bool $enable = null): int {}

#if HAVE_GETSERVBYNAME
function getservbyname(string $service, string $protocol): int|false {}
#endif

#if HAVE_GETSERVBYPORT
function getservbyport(int $port, string $protocol): string|false {}
#endif

#if HAVE_GETPROTOBYNAME
function getprotobyname(string $protocol): int|false {}
#endif

#if HAVE_GETPROTOBYNUMBER
function getprotobynumber(int $protocol): string|false {}
#endif

function register_tick_function(callable $callback, mixed ...$args): bool {}

function unregister_tick_function(callable $callback): void {}

function is_uploaded_file(string $filename): bool {}

function move_uploaded_file(string $from, string $to): bool {}

function parse_ini_file(string $filename, bool $process_sections = false, int $scanner_mode = INI_SCANNER_NORMAL): array|false {}

function parse_ini_string(string $ini_string, bool $process_sections = false, int $scanner_mode = INI_SCANNER_NORMAL): array|false {}

#if ZEND_DEBUG
function config_get_hash(): array {}
#endif

#ifdef HAVE_GETLOADAVG
function sys_getloadavg(): array|false {}
#endif

/* browscap.c */

function get_browser(?string $user_agent = null, bool $return_array = false): object|array|false {}

/* crc32.c */

function crc32(string $string): int {}

/* crypt.c */

function crypt(string $string, string $salt): string {}

/* datetime.c */

#if HAVE_STRPTIME
function strptime(string $timestamp, string $format): array|false {}
#endif

/* dns.c */

#ifdef HAVE_GETHOSTNAME
function gethostname(): string|false {}
#endif

function gethostbyaddr(string $ip): string|false {}

function gethostbyname(string $hostname): string {}

function gethostbynamel(string $hostname): array|false {}

#if defined(PHP_WIN32) || HAVE_DNS_SEARCH_FUNC
function dns_check_record(string $hostname, string $type = "MX"): bool {}

/** @alias dns_check_record */
function checkdnsrr(string $hostname, string $type = "MX"): bool {}

/**
 * @param array $authoritative_name_servers
 * @param array $additional_records
 */
function dns_get_record(string $hostname, int $type = DNS_ANY, &$authoritative_name_servers = null, &$additional_records = null, bool $raw = false): array|false {}

/**
 * @param array $hosts
 * @param array $weights
 */
function dns_get_mx(string $hostname, &$hosts, &$weights = null): bool {}

/**
 * @param array $hosts
 * @param array $weights
 * @alias dns_get_mx
 */
function getmxrr(string $hostname, &$hosts, &$weights = null): bool {}
#endif

/* net.c */

#if defined(PHP_WIN32) || HAVE_GETIFADDRS || defined(__PASE__)
function net_get_interfaces(): array|false {}
#endif

/* ftok.c */

#if HAVE_FTOK
function ftok(string $filename, string $project_id): int {}
#endif

/* hrtime.c */

function hrtime(bool $as_number = false): array|int|float|false {}

/* lcg.c */

function lcg_value(): float {}

/* md5.c */

function md5(string $string, bool $binary = false): string {}

function md5_file(string $filename, bool $binary = false): string|false {}

/* pageinfo.c */

function getmyuid(): int|false {}

function getmygid(): int|false {}

function getmypid(): int|false {}

function getmyinode(): int|false {}

function getlastmod(): int|false {}

/* sha1.c */

function sha1(string $string, bool $binary = false): string {}

function sha1_file(string $filename, bool $binary = false): string|false {}

/* syslog.c */

#ifdef HAVE_SYSLOG_H
function openlog(string $prefix, int $flags, int $facility): bool {}

function closelog(): bool {}

function syslog(int $priority, string $message): bool {}
#endif

#ifdef HAVE_INET_NTOP
function inet_ntop(string $ip): string|false {}
#endif

#ifdef HAVE_INET_PTON
function inet_pton(string $ip): string|false {}
#endif

/* metaphone.c */

function metaphone(string $string, int $max_phonemes = 0): string {}

/* {{{ head.c */
function header(string $header, bool $replace = true, int $response_code = 0): void {}

function header_remove(?string $name = null): void {}

function setrawcookie(string $name, string $value = "", array|int $expires_or_options = 0, string $path = "", string $domain = "", bool $secure = false, bool $httponly = false): bool {}

function setcookie(string $name, string $value = "", array|int $expires_or_options = 0, string $path = "", string $domain = "", bool $secure = false, bool $httponly = false): bool {}

function http_response_code(int $response_code = 0): int|bool {}

/**
 * @param string $filename
 * @param int $line
 */
function headers_sent(&$filename = null, &$line = null): bool {}

function headers_list(): array {}

/* {{{ html.c */

function htmlspecialchars(string $string, int $flags = ENT_COMPAT, ?string $encoding = null, bool $double_encode = true): string {}

function htmlspecialchars_decode(string $string, int $flags = ENT_COMPAT): string {}

function html_entity_decode(string $string, int $flags = ENT_COMPAT, ?string $encoding = null): string {}

function htmlentities(string $string, int $flags = ENT_COMPAT, ?string $encoding = null, bool $double_encode = true): string {}

function get_html_translation_table(int $table = HTML_SPECIALCHARS, int $flags = ENT_COMPAT, string $encoding = "UTF-8"): array {}

/* }}} */

/* assert.c */

function assert(mixed $assertion, Throwable|string|null $description = null): bool {}

function assert_options(int $option, mixed $value = UNKNOWN): mixed {}

/* string.c */

function bin2hex(string $string): string {}

function hex2bin(string $string): string|false {}

function strspn(string $string, string $characters, int $offset = 0, ?int $length = null): int {}

function strcspn(string $string, string $characters, int $offset = 0, ?int $length = null): int {}

#if HAVE_NL_LANGINFO
function nl_langinfo(int $item): string|false {}
#endif

function strcoll(string $string1, string $string2): int {}

function trim(string $string, string $characters = " \n\r\t\v\0"): string {}

function rtrim(string $string, string $characters = " \n\r\t\v\0"): string {}

/** @alias rtrim */
function chop(string $string, string $characters = " \n\r\t\v\0"): string {}

function ltrim(string $string, string $characters = " \n\r\t\v\0"): string {}

function wordwrap(string $string, int $width = 75, string $break = "\n", bool $cut_long_words = false): string {}

function explode(string $separator, string $string, int $limit = PHP_INT_MAX): array {}

function implode(string|array $separator, ?array $array = null): string {}

/** @alias implode */
function join(string|array $separator, ?array $array = null): string {}

function strtok(string $string, ?string $token = null): string|false {}

function strtoupper(string $string): string {}

function strtolower(string $string): string {}

function basename(string $path, string $suffix = ""): string {}

function dirname(string $path, int $levels = 1): string {}

function pathinfo(string $path, int $flags = PATHINFO_ALL): array|string {}

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

function chunk_split(string $string, int $length = 76, string $separator = "\r\n"): string {}

function substr(string $string, int $offset, ?int $length = null): string {}

function substr_replace(array|string $string, array|string $replace, array|int $offset, array|int|null $length = null): string|array {}

function quotemeta(string $string): string {}

function ord(string $character): int {}

function chr(int $codepoint): string {}

function ucfirst(string $string): string {}

function lcfirst(string $string): string {}

function ucwords(string $string, string $separators = " \t\r\n\f\v"): string {}

function strtr(string $string, string|array $from, ?string $to = null): string {}

function strrev(string $string): string {}

/** @param float $percent */
function similar_text(string $string1, string $string2, &$percent = null): int {}

function addcslashes(string $string, string $characters): string {}

function addslashes(string $string): string {}

function stripcslashes(string $string): string {}

function stripslashes(string $string): string {}

/** @param int $count */
function str_replace(array|string $search, array|string $replace, string|array $subject, &$count = null): string|array {}

/** @param int $count */
function str_ireplace(array|string $search, array|string $replace, string|array $subject, &$count = null): string|array {}

function hebrev(string $string, int $max_chars_per_line = 0): string {}

function nl2br(string $string, bool $use_xhtml = true): string {}

function strip_tags(string $string, array|string|null $allowed_tags = null): string {}

/**
 * @param array|string $locales
 * @param string $rest
 */
function setlocale(int $category, $locales, ...$rest): string|false {}

/** @param array $result */
function parse_str(string $string, &$result): void {}

function str_getcsv(string $string, string $separator = ",", string $enclosure = "\"", string $escape = "\\"): array {}

function str_repeat(string $string, int $times): string {}

function count_chars(string $string, int $mode = 0): array|string {}

function strnatcmp(string $string1, string $string2): int {}

function localeconv(): array {}

function strnatcasecmp(string $string1, string $string2): int {}

function substr_count(string $haystack, string $needle, int $offset = 0, ?int $length = null): int {}

function str_pad(string $string, int $length, string $pad_string = " ", int $pad_type = STR_PAD_RIGHT): string {}

function sscanf(string $string, string $format, mixed &...$vars): array|int|null {}

function str_rot13(string $string): string {}

function str_shuffle(string $string): string {}

function str_word_count(string $string, int $format = 0, ?string $characters = null): array|int {}

function str_split(string $string, int $length = 1): array {}

function strpbrk(string $string, string $characters): string|false {}

function substr_compare(string $haystack, string $needle, int $offset, ?int $length = null, bool $case_insensitive = false): int {}

function utf8_encode(string $string): string {}

function utf8_decode(string $string): string {}

/* dir.c */

/**
 * @param resource|null $context
 * @return resource|false
 */
function opendir(string $directory, $context = null) {}

/** @param resource|null $context */
function dir(string $directory, $context = null): Directory|false {}

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
function scandir(string $directory, int $sorting_order = SCANDIR_SORT_ASCENDING, $context = null): array|false {}

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
function passthru(string $command, &$result_code = null): ?bool {}

function escapeshellcmd(string $command): string {}

function escapeshellarg(string $arg): string {}

function shell_exec(string $command): string|false|null {}

#ifdef HAVE_NICE
function proc_nice(int $priority): bool {}
#endif

/* file.c */

/**
 * @param resource $stream
 * @param int $would_block
 */
function flock($stream, int $operation, &$would_block = null): bool {}

function get_meta_tags(string $filename, bool $use_include_path = false): array|false {}

/** @param resource $handle */
function pclose($handle): int {}

/** @return resource|false */
function popen(string $command, string $mode) {}

/** @param resource|null $context */
function readfile(string $filename, bool $use_include_path = false, $context = null): int|false {}

/** @param resource $stream */
function rewind($stream): bool {}

/** @param resource|null $context */
function rmdir(string $directory, $context = null): bool {}

function umask(?int $mask = null): int {}

/** @param resource $stream */
function fclose($stream): bool {}

/** @param resource $stream */
function feof($stream): bool {}

/** @param resource $stream */
function fgetc($stream): string|false {}

/** @param resource $stream */
function fgets($stream, ?int $length = null): string|false {}

/** @param resource $stream */
function fread($stream, int $length): string|false {}

/**
 * @param resource|null $context
 * @return resource|false
 */
function fopen(string $filename, string $mode, bool $use_include_path = false, $context = null) {}

/** @param resource $stream */
function fscanf($stream, string $format, mixed &...$vars): array|int|false|null {}

/** @param resource $stream */
function fpassthru($stream): int {}

/** @param resource $stream */
function ftruncate($stream, int $size): bool {}

/** @param resource $stream */
function fstat($stream): array|false {}

/** @param resource $stream */
function fseek($stream, int $offset, int $whence = SEEK_SET): int {}

/** @param resource $stream */
function ftell($stream): int|false {}

/** @param resource $stream */
function fflush($stream): bool {}

/** @param resource $stream */
function fwrite($stream, string $data, ?int $length = null): int|false {}

/**
 * @param resource $stream
 * @alias fwrite
 */
function fputs($stream, string $data, ?int $length = null): int|false {}

/** @param resource|null $context */
function mkdir(string $directory, int $permissions = 0777, bool $recursive = false, $context = null): bool {}

/** @param resource|null $context */
function rename(string $from, string $to, $context = null): bool {}

/** @param resource|null $context */
function copy(string $from, string $to, $context = null): bool {}

function tempnam(string $directory, string $prefix): string|false {}

/** @return resource|false */
function tmpfile() {}

/** @param resource|null $context */
function file(string $filename, int $flags = 0, $context = null): array|false {}

/** @param resource|null $context */
function file_get_contents(string $filename, bool $use_include_path = false, $context = null, int $offset = 0, ?int $length = null): string|false {}

/** @param resource|null $context */
function unlink(string $filename, $context = null): bool {}

/** @param resource|null $context */
function file_put_contents(string $filename, mixed $data, int $flags = 0, $context = null): int|false {}

/** @param resource $stream */
function fputcsv($stream, array $fields, string $separator = ",", string $enclosure = "\"", string $escape = "\\"): int|false {}

/** @param resource $stream */
function fgetcsv($stream, ?int $length = null, string $separator = ",", string $enclosure = "\"", string $escape = "\\"): array|false {}

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

function chmod(string $filename, int $permissions): bool {}

#if HAVE_UTIME
function touch(string $filename, ?int $mtime = null, ?int $atime = null): bool {}
#endif

function clearstatcache(bool $clear_realpath_cache = false, string $filename = ""): void {}

function disk_total_space(string $directory): float|false {}

function disk_free_space(string $directory): float|false {}

/** @alias disk_free_space */
function diskfreespace(string $directory): float|false {}

function realpath_cache_get(): array {}

function realpath_cache_size(): int {}

/* formatted_print.c */

function sprintf(string $format, mixed ...$values): string {}

function printf(string $format, mixed ...$values): int {}

function vprintf(string $format, array $values): int {}

function vsprintf(string $format, array $values): string {}

/** @param resource $stream */
function fprintf($stream, string $format, mixed ...$values): int {}

/** @param resource $stream */
function vfprintf($stream, string $format, array $values): int {}

/* fsock.c */

/**
 * @param int $error_code
 * @param string $error_message
 * @return resource|false
 */
function fsockopen(string $hostname, int $port = -1, &$error_code = null, &$error_message = null, ?float $timeout = null) {}

/**
 * @param int $error_code
 * @param string $error_message
 * @return resource|false
 */
function pfsockopen(string $hostname, int $port = -1, &$error_code = null, &$error_message = null, ?float $timeout = null) {}

/* http.c */

function http_build_query(array|object $data, string $numeric_prefix = "", ?string $arg_separator = null, int $encoding_type = PHP_QUERY_RFC1738): string {}

/* image.c */

function image_type_to_mime_type(int $image_type): string {}

function image_type_to_extension(int $image_type, bool $include_dot = true): string|false {}

/** @param array $image_info */
function getimagesize(string $filename, &$image_info = null): array|false {}

/** @param array $image_info */
function getimagesizefromstring(string $string, &$image_info = null): array|false {}

/* info.c */

function phpinfo(int $flags = INFO_ALL): bool {}

function phpversion(?string $extension = null): string|false {}

function phpcredits(int $flags = CREDITS_ALL): bool {}

function php_sapi_name(): string|false {}

function php_uname(string $mode = "a"): string {}

function php_ini_scanned_files(): string|false {}

function php_ini_loaded_file(): string|false {}

/* iptc.c */

function iptcembed(string $iptc_data, string $filename, int $spool = 0): string|bool {}

function iptcparse(string $iptc_block): array|false {}

/* levenshtein.c */

function levenshtein(string $string1, string $string2, int $insertion_cost = 1, int $replacement_cost = 1, int $deletion_cost = 1): int {}

/* link.c */

#if defined(HAVE_SYMLINK) || defined(PHP_WIN32)
function readlink(string $path): string|false {}

function linkinfo(string $path): int|false {}

function symlink(string $target, string $link): bool {}

function link(string $target, string $link): bool {}
#endif

/* mail.c */

function mail(string $to, string $subject, string $message, array|string $additional_headers = [], string $additional_params = ""): bool {}

/* math.c */

function abs(int|float $num): int|float {}

function ceil(int|float $num): float {}

function floor(int|float $num): float {}

function round(int|float $num, int $precision = 0, int $mode = PHP_ROUND_HALF_UP): float {}

function sin(float $num): float {}

function cos(float $num): float {}

function tan(float $num): float {}

function asin(float $num): float {}

function acos(float $num): float {}

function atan(float $num): float {}

function atanh(float $num): float {}

function atan2(float $y, float $x): float {}

function sinh(float $num): float {}

function cosh(float $num): float {}

function tanh(float $num): float {}

function asinh(float $num): float {}

function acosh(float $num): float {}

function expm1(float $num): float {}

function log1p(float $num): float {}

function pi(): float {}

function is_finite(float $num): bool {}

function is_nan(float $num): bool {}

function intdiv(int $num1, int $num2): int {}

function is_infinite(float $num): bool {}

function pow(mixed $num, mixed $exponent): int|float|object {}

function exp(float $num): float {}

function log(float $num, float $base = M_E): float {}

function log10(float $num): float {}

function sqrt(float $num): float {}

function hypot(float $x, float $y): float {}

function deg2rad(float $num): float {}

function rad2deg(float $num): float {}

function bindec(string $binary_string): int|float {}

function hexdec(string $hex_string): int|float {}

function octdec(string $octal_string): int|float {}

function decbin(int $num): string {}

function decoct(int $num): string {}

function dechex(int $num): string {}

function base_convert(string $num, int $from_base, int $to_base): string {}

function number_format(float $num, int $decimals = 0, ?string $decimal_separator = ".", ?string $thousands_separator = ","): string {}

function fmod(float $num1, float $num2): float {}

function fdiv(float $num1, float $num2): float {}

/* microtime.c */

#ifdef HAVE_GETTIMEOFDAY
function microtime(bool $as_float = false): string|float {}

function gettimeofday(bool $as_float = false): array|float {}
#endif

#ifdef HAVE_GETRUSAGE
function getrusage(int $mode = 0): array|false {}
#endif

/* pack.c */

function pack(string $format, mixed ...$values): string {}

function unpack(string $format, string $string, int $offset = 0): array|false {}

/* password.c */

function password_get_info(string $hash): array {}

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
function proc_open(array|string $command, array $descriptor_spec, &$pipes, ?string $cwd = null, ?array $env_vars = null, ?array $options = null) {}

/** @param resource $process */
function proc_close($process): int {}

/** @param resource $process */
function proc_terminate($process, int $signal = 15): bool {}

/** @param resource $process */
function proc_get_status($process): array {}
#endif

/* quot_print.c */

function quoted_printable_decode(string $string): string {}

function quoted_printable_encode(string $string): string {}

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

function soundex(string $string): string {}

/* streamsfuncs.c */

function stream_select(?array &$read, ?array &$write, ?array &$except, ?int $seconds, int $microseconds = 0): int|false {}

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
function stream_filter_prepend($stream, string $filter_name, int $mode = 0, mixed $params = UNKNOWN) {}

/**
 * @param resource $stream
 * @return resource|false
 */
function stream_filter_append($stream, string $filter_name, int $mode = 0, mixed $params = UNKNOWN) {}

/** @param resource $stream_filter */
function stream_filter_remove($stream_filter): bool {}

/**
 * @param int $error_code
 * @param string $error_message
 * @param resource|null $context
 * @return resource|false
 */
function stream_socket_client(string $address, &$error_code = null, &$error_message = null, ?float $timeout = null, int $flags = STREAM_CLIENT_CONNECT, $context = null) {}

/**
 * @param int $error_code
 * @param string $error_message
 * @param resource|null $context
 * @return resource|false
 */
function stream_socket_server(string $address, &$error_code = null, &$error_message = null, int $flags = STREAM_SERVER_BIND | STREAM_SERVER_LISTEN, $context = null) {}

/**
 * @param resource $socket
 * @param float $timeout
 * @param string $peer_name
 * @return resource|false
 */
function stream_socket_accept($socket, ?float $timeout = null, &$peer_name = null) {}

/** @param resource $socket */
function stream_socket_get_name($socket, bool $remote): string|false {}

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
function stream_socket_enable_crypto($stream, bool $enable, ?int $crypto_method = null, $session_stream = null): int|bool {}

#ifdef HAVE_SHUTDOWN
/** @param resource $stream */
function stream_socket_shutdown($stream, int $mode): bool {}
#endif

#if HAVE_SOCKETPAIR
function stream_socket_pair(int $domain, int $type, int $protocol): array|false {}
#endif

/**
 * @param resource $from
 * @param resource $to
 */
function stream_copy_to_stream($from, $to, ?int $length = null, int $offset = 0): int|false {}

/** @param resource $stream */
function stream_get_contents($stream, ?int $length = null, int $offset = -1): string|false {}

/** @param resource $stream */
function stream_supports_lock($stream): bool {}

/** @param resource $stream */
function stream_set_write_buffer($stream, int $size): int {}

/**
 * @param resource $stream
 * @alias stream_set_write_buffer
 */
function set_file_buffer($stream, int $size): int {}

/** @param resource $stream */
function stream_set_read_buffer($stream, int $size): int {}

/** @param resource $stream */
function stream_set_blocking($stream, bool $enable): bool {}

/**
 * @param resource $stream
 * @alias stream_set_blocking
 */
function socket_set_blocking($stream, bool $enable): bool {}

/** @param resource $stream */
function stream_get_meta_data($stream): array {}

/**
 * @param resource $stream
 * @alias stream_get_meta_data
 */
function socket_get_status($stream): array {}

/** @param resource $stream */
function stream_get_line($stream, int $length, string $ending = ""): string|false {}

function stream_resolve_include_path(string $filename): string|false {}

function stream_get_wrappers(): array {}

function stream_get_transports(): array {}

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
/** @param resource $stream */
function stream_set_timeout($stream, int $seconds, int $microseconds = 0): bool {}

/**
 * @param resource $stream
 * @alias stream_set_timeout
 */
function socket_set_timeout($stream, int $seconds, int $microseconds = 0): bool {}
#endif

/* type.c */

function gettype(mixed $value): string {}

function get_debug_type(mixed $value): string {}

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

function parse_url(string $url, int $component = -1): int|string|array|null|false {}

function urlencode(string $string): string {}

function urldecode(string $string): string {}

function rawurlencode(string $string): string {}

function rawurldecode(string $string): string {}

/** @param resource|null $context */
function get_headers(string $url, bool $associative = false, $context = null): array|false {}

/* user_filters.c */

/** @param resource $brigade */
function stream_bucket_make_writeable($brigade): ?object {}

/** @param resource $brigade */
function stream_bucket_prepend($brigade, object $bucket): void {}

/** @param resource $brigade */
function stream_bucket_append($brigade, object $bucket): void {}

/** @param resource $stream */
function stream_bucket_new($stream, string $buffer): object {}

function stream_get_filters(): array {}

function stream_filter_register(string $filter_name, string $class): bool {}

/* uuencode.c */

function convert_uuencode(string $string): string {}

function convert_uudecode(string $string): string|false {}

/* var.c */

function var_dump(mixed $value, mixed ...$values): void {}

function var_export(mixed $value, bool $return = false): ?string {}

function debug_zval_dump(mixed $value, mixed ...$values): void {}

function serialize(mixed $value): string {}

function unserialize(string $data, array $options = []): mixed {}

function memory_get_usage(bool $real_usage = false): int {}

function memory_get_peak_usage(bool $real_usage = false): int {}

/* versioning.c */

function version_compare(string $version1, string $version2, ?string $operator = null): int|bool {}

/* win32/codepage.c */

#ifdef PHP_WIN32
function sapi_windows_cp_set(int $codepage): bool {}

function sapi_windows_cp_get(string $kind = ""): int {}

function sapi_windows_cp_conv(int|string $in_codepage, int|string $out_codepage, string $subject): ?string {}

function sapi_windows_cp_is_utf8(): bool {}

function sapi_windows_set_ctrl_handler(?callable $handler, bool $add = true): bool {}

function sapi_windows_generate_ctrl_event(int $event, int $pid = 0): bool {}
#endif
