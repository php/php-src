<?php

/** @generate-function-entries */

class _ZendTestClass {
    public static function is_object(): int {}

    /** @deprecated */
    public function __toString(): string {}
}

trait _ZendTestTrait {
    public function testMethod(): bool {}
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
