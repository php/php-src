<?php

class FFI
{
    /** @return ?FFI */
    static function cdef(string $code = UNKNOWN, string $lib = UNKNOWN) {}

    /** @return ?FFI */
    static function load(string $filename) {}

    /** @return ?FFI */
    static function scope(string $scope_name) {}

    /**
     * @param FFI\CType|string $type
     * @return ?FFI\CData
     */
    static function new($type, bool $owned = true, bool $persistent = false) {}

    /** 
     * @prefer-ref $ptr
     * @return void
     */
    static function free(FFI\CData $ptr) {}

    /** 
     * @param FFI\CType|string $type
     * @prefer-ref $ptr
     * @return ?FFI\CData
     */
    static function cast($type, $ptr) {}

    /** @return ?FFI\CType */
    static function type(string $type) {}

    /**
     * @prefer-ref $ptr
     * @return FFI\CType
     */
    static function typeof(FFI\CData $ptr) {}

    /** @return ?FFI\CType */
    static function arrayType(FFI\CType $type, array $dims) {}

    /**
     * @prefer-ref $ptr
     * @return FFI\CData
     */
    static function addr(FFI\CData $ptr) {}

    /**
     * @prefer-ref $ptr
     * @return ?int
     */
    static function sizeof(object $ptr) {}

    /**
     * @prefer-ref $ptr
     * @return ?int
     */
    static function alignof(object $ptr) {}

    /**
     * @prefer-ref $dst
     * @prefer-ref $src
     * @param string|FFI\CData $dst
     * @return void
     */
    static function memcpy(FFI\CData $dst, $src, int $size) {}

    /**
     * @prefer-ref $ptr1
     * @param string|FFI\CData $ptr1
     * @prefer-ref $ptr2
     * @param string|FFI\CData $ptr2
     * @return ?int
     */
    static function memcmp($ptr1, $ptr2, int $size) {}

    /**
     * @prefer-ref $ptr
     * @return void
     */
    static function memset(FFI\CData $ptr, int $ch, int $size) {}

    /**
     * @prefer-ref $ptr
     * @return ?string
     */
    static function string(FFI\CData $ptr, int $size = UNKNOWN) {}

    /**
     * @prefer-ref $ptr
     */
    static function isNull(FFI\CData $ptr) {}
}
