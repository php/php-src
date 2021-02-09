<?php

/** @generate-function-entries */

namespace {

final class FFI
{
    public static function cdef(string $code = "", ?string $lib = null): FFI {}

    public static function load(string $filename): ?FFI {}

    public static function scope(string $name): FFI {}

    public static function new(FFI\CType|string $type, bool $owned = true, bool $persistent = false): ?FFI\CData {}

    /** @prefer-ref $ptr */
    public static function free(FFI\CData $ptr): void {}

    /**
     * @param FFI\CData|int|float|bool|null $ptr
     * @prefer-ref $ptr
     */
    public static function cast(FFI\CType|string $type, $ptr): ?FFI\CData {}

    public static function type(string $type): ?FFI\CType {}

    /** @prefer-ref $ptr */
    public static function typeof(FFI\CData $ptr): FFI\CType {}

    public static function arrayType(FFI\CType $type, array $dimensions): FFI\CType {}

    /** @prefer-ref $ptr */
    public static function addr(FFI\CData $ptr): FFI\CData {}

    /** @prefer-ref $ptr */
    public static function sizeof(FFI\CData|FFI\CType $ptr): int {}

    /** @prefer-ref $ptr */
    public static function alignof(FFI\CData|FFI\CType $ptr): int {}

    /**
     * @param FFI\CData|string $from
     * @prefer-ref $to
     * @prefer-ref $from
     */
    public static function memcpy(FFI\CData $to, $from, int $size): void {}

    /**
     * @prefer-ref $ptr1
     * @param string|FFI\CData $ptr1
     * @prefer-ref $ptr2
     * @param string|FFI\CData $ptr2
     */
    public static function memcmp($ptr1, $ptr2, int $size): int {}

    /** @prefer-ref $ptr */
    public static function memset(FFI\CData $ptr, int $value, int $size): void {}

    /** @prefer-ref $ptr */
    public static function string(FFI\CData $ptr, ?int $size = null): string {}

    /** @prefer-ref $ptr */
    public static function isNull(FFI\CData $ptr): bool {}
}

}

namespace FFI {

final class CData {
}

final class CType {
    public function getName() : string {}
}

class Exception extends \Error {
}

final class ParserException extends Exception {
}

}
