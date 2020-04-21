<?php

/** @generate-function-entries */

function curl_close(Curl $handle): void {}

function curl_copy_handle(Curl $handle): Curl|false {}

function curl_errno(Curl $handle): int {}

function curl_error(Curl $handle): string {}

#if LIBCURL_VERSION_NUM >= 0x070f04 /* 7.15.4 */
function curl_escape(Curl $handle, string $string): string|false {}

function curl_unescape(Curl $handle, string $string): string|false {}

/** @param mixed $value */
function curl_multi_setopt(CurlMulti $multi_handle, int $option, $value): bool {}

#endif

function curl_exec(Curl $handle): string|bool {}

function curl_file_create(string $filename, string $mimetype = UNKNOWN, string $postname = UNKNOWN): CURLFile {}

/** @return mixed */
function curl_getinfo(Curl $handle, int $option = UNKNOWN) {}

function curl_init(string $url = UNKNOWN): Curl|false {}

function curl_multi_add_handle(CurlMulti $multi_handle, Curl $handle): int {}

function curl_multi_close(CurlMulti $multi_handle): void {}

function curl_multi_errno(CurlMulti $multi_handle): int {}

function curl_multi_exec(CurlMulti $multi_handle, &$still_running): int {}

function curl_multi_getcontent(Curl $multi_handle): ?string {}

function curl_multi_info_read(CurlMulti $multi_handle, &$msgs_in_queue = null): array|false {}

function curl_multi_init(): CurlMulti {}

function curl_multi_remove_handle(CurlMulti $multi_handle, Curl $handle): int {}

function curl_multi_select(CurlMulti $multi_handle, float $timeout = 1.0): int {}

function curl_multi_strerror(int $error_number): ?string {}

#if LIBCURL_VERSION_NUM >= 0x071200 /* 7.18.0 */
function curl_pause(Curl $handle, int $bitmask): int {}
#endif

function curl_reset(Curl $handle): void {}

function curl_setopt_array(Curl $handle, array $options): bool {}

/** @param mixed $value */
function curl_setopt(Curl $handle, int $option, $value): bool {}

function curl_share_close(CurlShare $share_handle): void {}

function curl_share_errno(CurlShare $share_handle): int {}

function curl_share_init(): CurlShare {}

/** @param mixed $value */
function curl_share_setopt(CurlShare $share_handle, int $option, $value): bool {}

function curl_share_strerror(int $error_number): ?string {}

function curl_strerror(int $error_number): ?string {}

function curl_version(int $age = UNKNOWN): array|false {}
