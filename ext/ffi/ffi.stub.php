<?php

/** @generate-class-entries */

namespace {
	/** @not-serializable */
    final class FFI
    {
        /**
         * @cvalue __BIGGEST_ALIGNMENT__
         * @link ffi-ffi.constants.biggest-alignment
         */
        public const int __BIGGEST_ALIGNMENT__ = UNKNOWN;

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
        /** @cvalue ZEND_FFI_TYPE_VOID */
        public const int TYPE_VOID = UNKNOWN;
        /** @cvalue ZEND_FFI_TYPE_FLOAT */
        public const int TYPE_FLOAT = UNKNOWN;
        /** @cvalue ZEND_FFI_TYPE_DOUBLE */
        public const int TYPE_DOUBLE = UNKNOWN;
#ifdef HAVE_LONG_DOUBLE
        /** @cvalue ZEND_FFI_TYPE_LONGDOUBLE */
        public const int TYPE_LONGDOUBLE = UNKNOWN;
#endif
        /** @cvalue ZEND_FFI_TYPE_UINT8 */
        public const int TYPE_UINT8 = UNKNOWN;
        /** @cvalue ZEND_FFI_TYPE_SINT8 */
        public const int TYPE_SINT8 = UNKNOWN;
        /** @cvalue ZEND_FFI_TYPE_UINT16 */
        public const int TYPE_UINT16 = UNKNOWN;
        /** @cvalue ZEND_FFI_TYPE_SINT16 */
        public const int TYPE_SINT16 = UNKNOWN;
        /** @cvalue ZEND_FFI_TYPE_UINT32 */
        public const int TYPE_UINT32 = UNKNOWN;
        /** @cvalue ZEND_FFI_TYPE_SINT32 */
        public const int TYPE_SINT32 = UNKNOWN;
        /** @cvalue ZEND_FFI_TYPE_UINT64 */
        public const int TYPE_UINT64 = UNKNOWN;
        /** @cvalue ZEND_FFI_TYPE_SINT64 */
        public const int TYPE_SINT64 = UNKNOWN;
        /** @cvalue ZEND_FFI_TYPE_ENUM */
        public const int TYPE_ENUM = UNKNOWN;
        /** @cvalue ZEND_FFI_TYPE_BOOL */
        public const int TYPE_BOOL = UNKNOWN;
        /** @cvalue ZEND_FFI_TYPE_CHAR */
        public const int TYPE_CHAR = UNKNOWN;
        /** @cvalue ZEND_FFI_TYPE_POINTER */
        public const int TYPE_POINTER = UNKNOWN;
        /** @cvalue ZEND_FFI_TYPE_FUNC */
        public const int TYPE_FUNC = UNKNOWN;
        /** @cvalue ZEND_FFI_TYPE_ARRAY */
        public const int TYPE_ARRAY = UNKNOWN;
        /** @cvalue ZEND_FFI_TYPE_STRUCT */
        public const int TYPE_STRUCT = UNKNOWN;

        /** @cvalue ZEND_FFI_ATTR_CONST */
        public const int ATTR_CONST = UNKNOWN;
        /** @cvalue ZEND_FFI_ATTR_INCOMPLETE_TAG */
        public const int ATTR_INCOMPLETE_TAG = UNKNOWN;
        /** @cvalue ZEND_FFI_ATTR_VARIADIC */
        public const int ATTR_VARIADIC = UNKNOWN;
        /** @cvalue ZEND_FFI_ATTR_INCOMPLETE_ARRAY */
        public const int ATTR_INCOMPLETE_ARRAY = UNKNOWN;
        /** @cvalue ZEND_FFI_ATTR_VLA */
        public const int ATTR_VLA = UNKNOWN;
        /** @cvalue ZEND_FFI_ATTR_UNION */
        public const int ATTR_UNION = UNKNOWN;
        /** @cvalue ZEND_FFI_ATTR_PACKED */
        public const int ATTR_PACKED = UNKNOWN;
        /** @cvalue ZEND_FFI_ATTR_MS_STRUCT */
        public const int ATTR_MS_STRUCT = UNKNOWN;
        /** @cvalue ZEND_FFI_ATTR_GCC_STRUCT */
        public const int ATTR_GCC_STRUCT = UNKNOWN;

        /** @cvalue ZEND_FFI_ABI_DEFAULT */
        public const int ABI_DEFAULT = UNKNOWN;
        /** @cvalue ZEND_FFI_ABI_CDECL */
        public const int ABI_CDECL = UNKNOWN;
        /** @cvalue ZEND_FFI_ABI_FASTCALL */
        public const int ABI_FASTCALL = UNKNOWN;
        /** @cvalue ZEND_FFI_ABI_THISCALL */
        public const int ABI_THISCALL = UNKNOWN;
        /** @cvalue ZEND_FFI_ABI_STDCALL */
        public const int ABI_STDCALL = UNKNOWN;
        /** @cvalue ZEND_FFI_ABI_PASCAL */
        public const int ABI_PASCAL = UNKNOWN;
        /** @cvalue ZEND_FFI_ABI_REGISTER */
        public const int ABI_REGISTER = UNKNOWN;
        /** @cvalue ZEND_FFI_ABI_MS */
        public const int ABI_MS = UNKNOWN;
        /** @cvalue ZEND_FFI_ABI_SYSV */
        public const int ABI_SYSV = UNKNOWN;
        /** @cvalue ZEND_FFI_ABI_VECTORCALL */
        public const int ABI_VECTORCALL = UNKNOWN;

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
