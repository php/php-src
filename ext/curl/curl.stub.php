<?php

/** @generate-function-entries */

final class CurlHandle
{
}

final class CurlMultiHandle
{
}

final class CurlShareHandle
{
}

function curl_close(CurlHandle $handle): void {}

function curl_copy_handle(CurlHandle $handle): CurlHandle|false {}

function curl_errno(CurlHandle $handle): int {}

function curl_error(CurlHandle $handle): string {}

#if LIBCURL_VERSION_NUM >= 0x070f04 /* 7.15.4 */
function curl_escape(CurlHandle $handle, string $string): string|false {}

function curl_unescape(CurlHandle $handle, string $string): string|false {}

function curl_multi_setopt(CurlMultiHandle $multi_handle, int $option, mixed $value): bool {}

#endif

function curl_exec(CurlHandle $handle): string|bool {}

function curl_file_create(string $filename, ?string $mime_type = null, ?string $posted_filename = null): CURLFile {}

function curl_getinfo(CurlHandle $handle, ?int $option = null): mixed {}

function curl_init(?string $url = null): CurlHandle|false {}

function curl_multi_add_handle(CurlMultiHandle $multi_handle, CurlHandle $handle): int {}

function curl_multi_close(CurlMultiHandle $multi_handle): void {}

function curl_multi_errno(CurlMultiHandle $multi_handle): int {}

/** @param int $still_running */
function curl_multi_exec(CurlMultiHandle $multi_handle, &$still_running): int {}

function curl_multi_getcontent(CurlHandle $handle): ?string {}

/** @param int $queued_messages */
function curl_multi_info_read(CurlMultiHandle $multi_handle, &$queued_messages = null): array|false {}

function curl_multi_init(): CurlMultiHandle {}

function curl_multi_remove_handle(CurlMultiHandle $multi_handle, CurlHandle $handle): int {}

function curl_multi_select(CurlMultiHandle $multi_handle, float $timeout = 1.0): int {}

function curl_multi_strerror(int $error_code): ?string {}

#if LIBCURL_VERSION_NUM >= 0x071200 /* 7.18.0 */
function curl_pause(CurlHandle $handle, int $flags): int {}
#endif

function curl_reset(CurlHandle $handle): void {}

function curl_setopt_array(CurlHandle $handle, array $options): bool {}

function curl_setopt(CurlHandle $handle, int $option, mixed $value): bool {}

function curl_share_close(CurlShareHandle $share_handle): void {}

function curl_share_errno(CurlShareHandle $share_handle): int {}

function curl_share_init(): CurlShareHandle {}

function curl_share_setopt(CurlShareHandle $share_handle, int $option, mixed $value): bool {}

function curl_share_strerror(int $error_code): ?string {}

function curl_strerror(int $error_code): ?string {}

function curl_version(): array|false {}
