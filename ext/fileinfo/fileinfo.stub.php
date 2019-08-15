<?php

/**
 * @return resource|false
 */
function finfo_open(int $options, string $file = NULL) {}

/**
 * @param resource $finfo
 */
function finfo_close($finfo): bool {}

/**
 * @param resource $finfo
 */
function finfo_set_flags($finfo, int $options): bool {}

/**
 * @param resource $finfo
 * @param resource|null $context
 */
function finfo_file($finfo, string $file_name, int $options = FILEINFO_NONE, $context = NULL): string {}

/**
 * @param resource $finfo
 * @param resource|null $context
 */
function finfo_buffer($finfo, string $string = NULL, int $options = FILEINFO_NONE, $context = NULL): string {}

/**
 * @param string|resource $file
 */
function mime_content_type($file): string {}
