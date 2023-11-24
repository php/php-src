<?php

/** @generate-class-entries static */

namespace {

    interface _ZendTestInterface
    {
    }

    /** @alias _ZendTestClassAlias */
    class _ZendTestClass implements _ZendTestInterface {
        /** @var mixed */
        public static $_StaticProp;
        public static int $staticIntProp = 123;

        public int $intProp = 123;
        public ?stdClass $classProp = null;
        public stdClass|Iterator|null $classUnionProp = null;
        public readonly int $readonlyProp;

        public static function is_object(): int {}

        /**
         * @deprecated
         * @return string
         */
        public function __toString() {}

        public function returnsStatic(): static {}

        public function returnsThrowable(): Throwable {}

        static public function variadicTest(string|Iterator ...$elements) : static {}
    }

    class _ZendTestChildClass extends _ZendTestClass
    {
        public function returnsThrowable(): Exception {}
    }

    trait _ZendTestTrait {
        /** @var mixed */
        public $testProp;

        public function testMethod(): bool {}
    }

    final class ZendTestAttribute {

    }

    final class ZendTestParameterAttribute {
        public string $parameter;

        public function __construct(string $parameter) {}
    }

    class ZendTestClassWithMethodWithParameterAttribute {
        final public function no_override(string $parameter): int {}
        public function override(string $parameter): int {}
    }

    class ZendTestChildClassWithMethodWithParameterAttribute extends ZendTestClassWithMethodWithParameterAttribute {
        public function override(string $parameter): int {}
    }

    enum ZendTestUnitEnum {
        case Foo;
        case Bar;
    }

    enum ZendTestStringEnum: string {
        case Foo = "Test1";
        case Bar = "Test2";
        case Baz = "Test2\\a";
        case FortyTwo = "42";
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

    function zend_get_unit_enum(): ZendTestUnitEnum {}

    function zend_test_parameter_with_attribute(string $parameter): int {}

    function zend_get_current_func_name(): string {}

    function zend_call_method(string $class, string $method, mixed $arg1 = UNKNOWN, mixed $arg2 = UNKNOWN): mixed {}

    function zend_get_map_ptr_last(): int {}

    function zend_test_crash(?string $message = null): void {}

#if defined(HAVE_LIBXML) && !defined(PHP_WIN32)
function zend_test_override_libxml_global_state(): void {}
#endif

    function zend_test_is_pcre_bundled(): bool {}
}

namespace ZendTestNS {

    class Foo {
        public function method(): void {}
    }

}

namespace ZendTestNS2 {

    class Foo {
        public ZendSubNS\Foo $foo;

        public function method(): void {}
    }

}

namespace ZendTestNS2\ZendSubNS {

    class Foo {
        public function method(): void {}
    }

    function namespaced_func(): bool {}

}
