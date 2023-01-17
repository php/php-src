<?php

/** @generate-class-entries */

namespace {

	/** @not-serializable */
    final class FFI
    {
        /**
         * @var int
         * @cvalue __BIGGEST_ALIGNMENT__
         * @link ffi-ffi.constants.biggest-alignment
         */
        public const __BIGGEST_ALIGNMENT__ = UNKNOWN;

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

	/** @not-serializable */
    final class CData {
    }

	/** @not-serializable */
    final class CType {
        /**
         * @var int
         * @cvalue ZEND_FFI_TYPE_VOID
         */
        public const TYPE_VOID = UNKNOWN;
        /**
         * @var int
         * @cvalue ZEND_FFI_TYPE_FLOAT
         */
        public const TYPE_FLOAT = UNKNOWN;
        /**
         * @var int
         * @cvalue ZEND_FFI_TYPE_DOUBLE
         */
        public const TYPE_DOUBLE = UNKNOWN;
#ifdef HAVE_LONG_DOUBLE
        /**
         * @var int
         * @cvalue ZEND_FFI_TYPE_LONGDOUBLE
         */
        public const TYPE_LONGDOUBLE = UNKNOWN;
#endif
        /**
         * @var int
         * @cvalue ZEND_FFI_TYPE_UINT8
         */
        public const TYPE_UINT8 = UNKNOWN;
        /**
         * @var int
         * @cvalue ZEND_FFI_TYPE_SINT8
         */
        public const TYPE_SINT8 = UNKNOWN;
        /**
         * @var int
         * @cvalue ZEND_FFI_TYPE_UINT16
         */
        public const TYPE_UINT16 = UNKNOWN;
        /**
         * @var int
         * @cvalue ZEND_FFI_TYPE_SINT16
         */
        public const TYPE_SINT16 = UNKNOWN;
        /**
         * @var int
         * @cvalue ZEND_FFI_TYPE_UINT32
         */
        public const TYPE_UINT32 = UNKNOWN;
        /**
         * @var int
         * @cvalue ZEND_FFI_TYPE_SINT32
         */
        public const TYPE_SINT32 = UNKNOWN;
        /**
         * @var int
         * @cvalue ZEND_FFI_TYPE_UINT64
         */
        public const TYPE_UINT64 = UNKNOWN;
        /**
         * @var int
         * @cvalue ZEND_FFI_TYPE_SINT64
         */
        public const TYPE_SINT64 = UNKNOWN;
        /**
         * @var int
         * @cvalue ZEND_FFI_TYPE_ENUM
         */
        public const TYPE_ENUM = UNKNOWN;
        /**
         * @var int
         * @cvalue ZEND_FFI_TYPE_BOOL
         */
        public const TYPE_BOOL = UNKNOWN;
        /**
         * @var int
         * @cvalue ZEND_FFI_TYPE_CHAR
         */
        public const TYPE_CHAR = UNKNOWN;
        /**
         * @var int
         * @cvalue ZEND_FFI_TYPE_POINTER
         */
        public const TYPE_POINTER = UNKNOWN;
        /**
         * @var int
         * @cvalue ZEND_FFI_TYPE_FUNC
         */
        public const TYPE_FUNC = UNKNOWN;
        /**
         * @var int
         * @cvalue ZEND_FFI_TYPE_ARRAY
         */
        public const TYPE_ARRAY = UNKNOWN;
        /**
         * @var int
         * @cvalue ZEND_FFI_TYPE_STRUCT
         */
        public const TYPE_STRUCT = UNKNOWN;

        /**
         * @var int
         * @cvalue ZEND_FFI_ATTR_CONST
         */
        public const ATTR_CONST = UNKNOWN;
        /**
         * @var int
         * @cvalue ZEND_FFI_ATTR_INCOMPLETE_TAG
         */
        public const ATTR_INCOMPLETE_TAG = UNKNOWN;
        /**
         * @var int
         * @cvalue ZEND_FFI_ATTR_VARIADIC
         */
        public const ATTR_VARIADIC = UNKNOWN;
        /**
         * @var int
         * @cvalue ZEND_FFI_ATTR_INCOMPLETE_ARRAY
         */
        public const ATTR_INCOMPLETE_ARRAY = UNKNOWN;
        /**
         * @var int
         * @cvalue ZEND_FFI_ATTR_VLA
         */
        public const ATTR_VLA = UNKNOWN;
        /**
         * @var int
         * @cvalue ZEND_FFI_ATTR_UNION
         */
        public const ATTR_UNION = UNKNOWN;
        /**
         * @var int
         * @cvalue ZEND_FFI_ATTR_PACKED
         */
        public const ATTR_PACKED = UNKNOWN;
        /**
         * @var int
         * @cvalue ZEND_FFI_ATTR_MS_STRUCT
         */
        public const ATTR_MS_STRUCT = UNKNOWN;
        /**
         * @var int
         * @cvalue ZEND_FFI_ATTR_GCC_STRUCT
         */
        public const ATTR_GCC_STRUCT = UNKNOWN;

        /**
         * @var int
         * @cvalue ZEND_FFI_ABI_DEFAULT
         */
        public const ABI_DEFAULT = UNKNOWN;
        /**
         * @var int
         * @cvalue ZEND_FFI_ABI_CDECL
         */
        public const ABI_CDECL = UNKNOWN;
        /**
         * @var int
         * @cvalue ZEND_FFI_ABI_FASTCALL
         */
        public const ABI_FASTCALL = UNKNOWN;
        /**
         * @var int
         * @cvalue ZEND_FFI_ABI_THISCALL
         */
        public const ABI_THISCALL = UNKNOWN;
        /**
         * @var int
         * @cvalue ZEND_FFI_ABI_STDCALL
         */
        public const ABI_STDCALL = UNKNOWN;
        /**
         * @var int
         * @cvalue ZEND_FFI_ABI_PASCAL
         */
        public const ABI_PASCAL = UNKNOWN;
        /**
         * @var int
         * @cvalue ZEND_FFI_ABI_REGISTER
         */
        public const ABI_REGISTER = UNKNOWN;
        /**
         * @var int
         * @cvalue ZEND_FFI_ABI_MS
         */
        public const ABI_MS = UNKNOWN;
        /**
         * @var int
         * @cvalue ZEND_FFI_ABI_SYSV
         */
        public const ABI_SYSV = UNKNOWN;
        /**
         * @var int
         * @cvalue ZEND_FFI_ABI_VECTORCALL
         */
        public const ABI_VECTORCALL = UNKNOWN;

        public function getName(): string {}

        public function getKind(): int {}
        public function getSize(): int {}
        public function getAlignment(): int {}
        public function getAttributes(): int {}

        public function getEnumKind(): int {}

        public function getArrayElementType(): CType {}
        public function getArrayLength(): int {}

        public function getPointerType(): CType {}

        public function getStructFieldNames(): array {}
        public function getStructFieldOffset(string $name): int {}
        public function getStructFieldType(string $name): CType {}

        public function getFuncABI(): int {}
        public function getFuncReturnType(): CType {}
        public function getFuncParameterCount(): int {}
        public function getFuncParameterType(int $index): CType {}
    }

    class Exception extends \Error {
    }

    final class ParserException extends Exception {
    }

}
