<?php

class finfo
{
    function __construct(int $options = FILEINFO_NONE, string $arg = "") {}

    /**
     * @param ?resource $context
     * @return string|false
     */
    function file(string $file_name, int $options = FILEINFO_NONE, $context = null) {}

    /**
     * @param ?resource $context
     * @return string|false
     */
    function buffer(string $string, int $options = FILEINFO_NONE, $context = null) {}

    /** @return bool */
    function set_flags(int $options) {}
}

/** @return resource|false */
function finfo_open(int $options = FILEINFO_NONE, string $arg = "") {}

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
 * @param ?resource $context
 */
function finfo_file($finfo, string $file_name, int $options = FILEINFO_NONE, $context = null): string|false {}

/**
 * @param resource $finfo
 * @param ?resource $context
 */
function finfo_buffer($finfo, string $string, int $options = FILEINFO_NONE, $context = null): string|false {}

/**
 * @param resource|string $filename
 */
function mime_content_type($filename): string|false {}
