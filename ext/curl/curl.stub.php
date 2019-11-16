<?php

/** @param resource $handle */
function curl_close($handle): void {}

/**
 * @param resource $handle
 * @return resource|false
 */
function curl_copy_handle($handle) {}

/** @param resource $handle */
function curl_errno($handle): int {}

/** @param resource $handle */
function curl_error($handle): string {}

/** @param resource $handle */
function curl_escape($handle, string $string): string|false {}

/** @param resource $handle */
function curl_exec($handle): string|bool {}

function curl_file_create(
    string $filename,
    string $mimetype = UNKNOWN,
    string $postname = UNKNOWN
): CURLFile {}

/**
 * @param resource $handle
 * @return mixed
 */
function curl_getinfo($handle, int $option = UNKNOWN) {}

/**
 * @param resource $handle
 * @return resource|false
 */
function curl_init(string $url = UNKNOWN) {}

/**
 * @param resource $multi_handle
 * @param resource $handle
 */
function curl_multi_add_handle($multi_handle, $handle): int {}

/** @param resource $multi_handle */
function curl_multi_close($multi_handle): void {}

/** @param resource $multi_handle */
function curl_multi_errno($multi_handle): int {}

/** @param resource $multi_handle */
function curl_multi_exec($multi_handle, &$still_running): int {}

/** @param resource $multi_handle */
function curl_multi_getcontent($multi_handle): ?string {}

/** @param resource $multi_handle */
function curl_multi_info_read($multi_handle, &$msgs_in_queue = null): array|false {}

/** @return resource */
function curl_multi_init() {}

/**
 * @param resource $multi_handle
 * @param resource $handle
 */
function curl_multi_remove_handle($multi_handle, $handle): int {}

/** @param resource $multi_handle */
function curl_multi_select($multi_handle, float $timeout = 1.0): int {}

/**
 * @param resource $multi_handle
 * @param mixed $value
 */
function curl_multi_setopt($multi_handle, int $option, $value): bool {}

function curl_multi_strerror(int $error_number): ?string {}

#if LIBCURL_VERSION_NUM >= 0x071200 /* 7.18.0 */
/** @param resource $handle */
function curl_pause($handle, int $bitmask): int {}
#endif

/** @param resource $handle */
function curl_reset($handle): void {}

/** @param resource $handle */
function curl_setopt_array($handle, array $options): bool {}

/**
 * @param resource $handle
 * @param mixed $value
 */
function curl_setopt($handle, int $option, $value): bool {}

/** @param resource $share_handle */
function curl_share_close($share_handle): void {}

/** @param resource $share_handle */
function curl_share_errno($share_handle): int {}

/** @return resource */
function curl_share_init() {}

/** @param resource $share_handle */
function curl_share_setopt($share_handle, int $option, $value): bool {}

function curl_share_strerror(int $error_number): ?string {}

function curl_strerror(int $error_number): ?string {}

/** @param resource $handle */
function curl_unescape($handle, string $string): string|false {}

function curl_version(int $age = UNKNOWN): array|false {}
