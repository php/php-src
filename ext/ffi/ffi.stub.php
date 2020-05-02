<?php

/** @generate-function-entries */

final class FFI
{
    public static function cdef(string $code = "", ?string $lib = null): FFI {}

    public static function load(string $filename): ?FFI {}

    public static function scope(string $scope_name): ?FFI {}

    /** @param FFI\CType|string $type */
    public static function new($type, bool $owned = true, bool $persistent = false): ?FFI\CData {}

    /** @prefer-ref $ptr */
    public static function free(FFI\CData $ptr): void {}

    /**
     * @param FFI\CType|string $type
     * @prefer-ref $ptr
     */
    public static function cast($type, $ptr): ?FFI\CData {}

    public static function type(string $type): ?FFI\CType {}

    /** @prefer-ref $ptr */
    public static function typeof(FFI\CData $ptr): ?FFI\CType {}

    public static function arrayType(FFI\CType $type, array $dims): ?FFI\CType {}

    /** @prefer-ref $ptr */
    public static function addr(FFI\CData $ptr): FFI\CData {}

    /** @prefer-ref $ptr */
    public static function sizeof(object $ptr): ?int {}

    /** @prefer-ref $ptr */
    public static function alignof(object $ptr): ?int {}

    /**
     * @prefer-ref $dst
     * @prefer-ref $src
     * @param string|FFI\CData $dst
     */
    public static function memcpy(FFI\CData $dst, $src, int $size): void {}

    /**
     * @prefer-ref $ptr1
     * @param string|FFI\CData $ptr1
     * @prefer-ref $ptr2
     * @param string|FFI\CData $ptr2
     */
    public static function memcmp($ptr1, $ptr2, int $size): ?int {}

    /** @prefer-ref $ptr */
    public static function memset(FFI\CData $ptr, int $ch, int $size): void {}

    /** @prefer-ref $ptr */
    public static function string(FFI\CData $ptr, int $size = UNKNOWN): ?string {}

    /** @prefer-ref $ptr */
    public static function isNull(FFI\CData $ptr): bool {}
}
