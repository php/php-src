<?php

/** @param resource $handle */
function curl_close($handle): void {}

/**
 * @param resource $handle
 *
 * @return resource|false
 */
function curl_copy_handle($handle) {}

/** @param resource $handle */
function curl_errno($handle): int {}

/** @param resource $handle */
function curl_error($handle): string {}

/**
 * @param resource $handle
 *
 * @return string|false
 */
function curl_escape($handle, string $string) {}

/**
 * @param resource $handle
 *
 * @return mixed|false
 */
function curl_exec($handle) {}

/** @return CURLFile */
function curl_file_create(
    string $filename,
    string $mimetype = UNKNOWN,
    string $postname = UNKNOWN
) {}

function curl_file_name(string $name): void {}

/**
 * @param resource $handle
 *
 * @return mixed|mixed[]|false
 */
function curl_getinfo($handle, int $option = UNKNOWN) {}

/**
 * @param resource $handle
 *
 * @return resource|false
 */
function curl_init($handle) {}

/**
 * @param resource $multi_handle
 * @param resource $handle
 */
function curl_multi_add_handle($multi_handle, $handle): int {}

/** @param resource $multi_handle */
function curl_multi_close($multi_handle): void {}

/**
 * @param resource $multi_handle
 *
 * @return int|false
 */
function curl_multi_errno($multi_handle) {}

/** @param resource $multi_handle */
function curl_multi_exec($multi_handle, int &$still_running): int {}

/** @param resource $multi_handle */
function curl_multi_getcontent($multi_handle): ?string {}

/**
 * @param resource $multi_handle
 *
 * @return array<string, mixed>|false
 */
function curl_multi_info_read($multi_handle, ?int &$msgs_in_queue = null) {}

/** @return resource */
function curl_multi_init() {}

/**
 * @param resource $multi_handle
 * @param resource $handle
 */
function curl_multi_remove_handle($multi_handle, $handle): int {}

/** @param resource $multi_handle */
function curl_multi_select($multi_handle, float $timeout = UNKNOWN): int {}

/**
 * @param resource $multi_handle
 * @param mixed $value
 */
function curl_multi_setopt($multi_handle, int $option, $value): bool {}

function curl_multi_strerror(int $error_number): ?string {}

/** @param resource $handle */
function curl_pause($handle, int $bitmask): int {}

/** @param resource $handle */
function curl_reset($handle): void {}

/**
 * @param resource $handle
 * @param array<int, mixed> $options
 */
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
function curl_share_setopt($share_handle, int $option, int $value): bool {}

function curl_share_strerror(int $error_number): ?string {}

function curl_strerror(int $error_number): ?string {}

/**
 * @param resource $handle
 *
 * @return string|false
 */
function curl_unescape($handle, string $string) {}

/** @return array<string, mixed>|false */
function curl_version(int $age = UNKNOWN) {}
