<?php

/** @generate-function-entries static */

namespace {

interface _ZendTestInterface
{
}

/** @alias _ZendTestClassAlias */
class _ZendTestClass {
    public static function is_object(): int {}

    /**
     * @deprecated
     * @return string
     */
    public function __toString() {}

    public function returnsStatic(): static {}
}

class _ZendTestChildClass extends _ZendTestClass
{
}

trait _ZendTestTrait {
    public function testMethod(): bool {}
}

final class ZendTestAttribute {

}

function zend_test_array_return(): array {}

function zend_test_nullable_array_return(): ?array {}

function zend_test_void_return(): void {}

/** @deprecated */
function zend_test_deprecated(mixed $arg = null): void {}

function zend_create_unterminated_string(string $str): string {}

function zend_terminate_string(string &$str): void {}

function zend_leak_variable(mixed $variable): void {}

function zend_leak_bytes(int $bytes = 3): void {}

function zend_string_or_object(object|string $param): object|string {}

function zend_string_or_object_or_null(object|string|null $param): object|string|null {}

/** @param stdClass|string $param */
function zend_string_or_stdclass($param): stdClass|string {}

/** @param stdClass|string|null $param */
function zend_string_or_stdclass_or_null($param): stdClass|string|null {}

function zend_iterable(iterable $arg1, ?iterable $arg2 = null): void {}

function zend_weakmap_attach(object $object, mixed $value): bool {}
function zend_weakmap_remove(object $object): bool {}
function zend_weakmap_dump(): array {}

function zend_get_current_func_name(): string {}

#if defined(HAVE_LIBXML) && !defined(PHP_WIN32)
function zend_test_override_libxml_global_state(): void {}
#endif

}

namespace ZendTestNS {

class Foo {
    public function method(): void {}
}

}

namespace ZendTestNS2 {

class Foo {
    public function method(): void {}
}

}

namespace ZendTestNS2\ZendSubNS {

function namespaced_func(): bool {}

}
