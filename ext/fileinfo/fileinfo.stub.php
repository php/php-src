<?php

/** @generate-class-entries */

/** @not-serializable */
class finfo
{
    /** @alias finfo_open */
    public function __construct(int $flags = FILEINFO_NONE, ?string $magic_database = null) {}

    /**
     * @param resource|null $context
     * @tentative-return-type
     * @alias finfo_file
     */
    public function file(string $filename, int $flags = FILEINFO_NONE, $context = null): string|false {}

    /**
     * @param resource|null $context
     * @tentative-return-type
     * @alias finfo_buffer
     */
    public function buffer(string $string, int $flags = FILEINFO_NONE, $context = null): string|false {}

    /**
     * @return bool
     * @alias finfo_set_flags
     */
    public function set_flags(int $flags) {} // TODO make return type void
}

/** @refcount 1 */
function finfo_open(int $flags = FILEINFO_NONE, ?string $magic_database = null): finfo|false {}

function finfo_close(finfo $finfo): bool {}

function finfo_set_flags(finfo $finfo, int $flags): bool {} // TODO make return type void

/**
 * @param resource|null $context
 * @refcount 1
 */
function finfo_file(finfo $finfo, string $filename, int $flags = FILEINFO_NONE, $context = null): string|false {}

/**
 * @param resource|null $context
 * @refcount 1
 */
function finfo_buffer(finfo $finfo, string $string, int $flags = FILEINFO_NONE, $context = null): string|false {}

/**
 * @param resource|string $filename
 * @refcount 1
 */
function mime_content_type($filename): string|false {}
