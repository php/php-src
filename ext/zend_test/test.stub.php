<?php

/**
 * @generate-class-entries static
 * @generate-legacy-arginfo 80000
 * @undocumentable
 */
namespace {
    require "Zend/zend_attributes.stub.php";

    /**
     * @var int
     * @deprecated
     */
    const ZEND_TEST_DEPRECATED = 42;

    /** @var string */
    const ZEND_CONSTANT_A = "global";

    interface _ZendTestInterface
    {
        /** @var int */
        public const DUMMY = 0;
    }

    /** @alias _ZendTestClassAlias */
    class _ZendTestClass implements _ZendTestInterface {
        public const mixed TYPED_CLASS_CONST1 = [];
        public const int|array TYPED_CLASS_CONST2 = 42;
        /**
         * @var int
         * @cvalue 1
         */
        public const int|string TYPED_CLASS_CONST3 = UNKNOWN;

        /** @var mixed */
        public static $_StaticProp;
        public static int $staticIntProp = 123;

        public int $intProp = 123;
        public ?stdClass $classProp = null;
        public stdClass|Iterator|null $classUnionProp = null;
        public Traversable&Countable $classIntersectionProp;
        public readonly int $readonlyProp;

        public static function is_object(): int {}

        /** @deprecated */
        public function __toString(): string {}

        public function returnsStatic(): static {}

        public function returnsThrowable(): Throwable {}

        static public function variadicTest(string|Iterator ...$elements) : static {}

        public function takesUnionType(stdclass|Iterator $arg): void {}
    }

    class _ZendTestChildClass extends _ZendTestClass
    {
        public function returnsThrowable(): Exception {}
    }

    class ZendAttributeTest {
        /** @var int */
        #[ZendTestRepeatableAttribute]
        #[ZendTestRepeatableAttribute]
        public const TEST_CONST = 1;

        /** @var mixed */
        #[ZendTestRepeatableAttribute]
        #[ZendTestPropertyAttribute("testProp")]
        public $testProp;

        #[ZendTestAttribute]
        public function testMethod(): bool {}
    }

    trait _ZendTestTrait {
        /** @var mixed */
        public $testProp;
        public Traversable|Countable $classUnionProp;

        public function testMethod(): bool {}
    }

    #[Attribute(Attribute::TARGET_ALL)]
    final class ZendTestAttribute {
    }

    #[Attribute(Attribute::TARGET_ALL|Attribute::IS_REPEATABLE)]
    final class ZendTestRepeatableAttribute {
    }

    #[Attribute(Attribute::TARGET_PARAMETER)]
    final class ZendTestParameterAttribute {
        public string $parameter;

        public function __construct(string $parameter) {}
    }

    #[Attribute(Attribute::TARGET_PROPERTY)]
    final class ZendTestPropertyAttribute {
        public string $parameter;

        public function __construct(string $parameter) {}
    }

    class ZendTestClassWithMethodWithParameterAttribute {
        final public function no_override(
            #[ZendTestParameterAttribute("value2")]
            string $parameter
        ): int {}
        public function override(
            #[ZendTestParameterAttribute("value3")]
            string $parameter
        ): int {}
    }

    class ZendTestChildClassWithMethodWithParameterAttribute extends ZendTestClassWithMethodWithParameterAttribute {
        public function override(
            #[ZendTestParameterAttribute("value4")]
            string $parameter
        ): int {}
    }

    class ZendTestClassWithPropertyAttribute {
        // this attribute must be added internally in MINIT
        #[ZendTestAttribute]
        public string $attributed;
    }

    final class ZendTestForbidDynamicCall {
        public function call(): void {}
        public static function callStatic(): void {}
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

    enum ZendTestIntEnum: int {
        case Foo = 1;
        case Bar = 3;
        case Baz = -1;
    }

    function zend_test_array_return(): array {}

    function zend_test_nullable_array_return(): null|array {}

    function zend_test_void_return(): void {}

    function zend_test_compile_string(string $source_string, string $filename, int $position): void {}

    /** @deprecated */
    function zend_test_deprecated(mixed $arg = null): void {}

    /** @alias zend_test_void_return */
    function zend_test_aliased(): void {}

    /**
      * @deprecated
      * @alias zend_test_void_return
      */
    function zend_test_deprecated_aliased(): void {}

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

    function zend_number_or_string(string|int|float $param): string|int|float {}

    function zend_number_or_string_or_null(string|int|float|null $param): string|int|float|null {}

    function zend_iterable(iterable $arg1, ?iterable $arg2 = null): void {}

    function zend_weakmap_attach(object $object, mixed $value): bool {}
    function zend_weakmap_remove(object $object): bool {}
    function zend_weakmap_dump(): array {}

    function zend_get_unit_enum(): ZendTestUnitEnum {}

    function zend_test_parameter_with_attribute(
        #[ZendTestParameterAttribute("value1")]
        string $parameter
    ): int {}

    function zend_get_current_func_name(): string {}

    function zend_call_method(object|string $obj_or_class, string $method, mixed $arg1 = UNKNOWN, mixed $arg2 = UNKNOWN): mixed {}

    function zend_test_zend_ini_parse_quantity(string $str): int {}
    function zend_test_zend_ini_parse_uquantity(string $str): int {}

    function zend_test_zend_ini_str(): string {}

#ifdef ZEND_CHECK_STACK_LIMIT
    function zend_test_zend_call_stack_get(): ?array {}
    function zend_test_zend_call_stack_use_all(): int {}
#endif

    function zend_test_is_string_marked_as_valid_utf8(string $string): bool {}

    function zend_get_map_ptr_last(): int {}

    function zend_test_crash(?string $message = null): void {}

    function zend_test_fill_packed_array(array &$array): void {}

    /** @return resource */
    function zend_test_create_throwing_resource() {}

    function get_open_basedir(): ?string {}

#if defined(HAVE_LIBXML) && !defined(PHP_WIN32)
function zend_test_override_libxml_global_state(): void {}
#endif
}

namespace ZendTestNS {

    class Foo {
        /** @tentative-return-type */
        public function method(): int {}
    }

    class UnlikelyCompileError {
        /* This method signature would create a compile error due to the string
         * "ZendTestNS\UnlikelyCompileError" in the generated macro call */
        public function method(): ?UnlikelyCompileError {}
    }

}

namespace ZendTestNS2 {

    /** @var string */
    const ZEND_CONSTANT_A = "namespaced";

    class Foo {
        public ZendSubNS\Foo $foo;

        public function method(): void {}
    }

    function namespaced_func(): bool {}

    /** @deprecated */
    function namespaced_deprecated_func(): void {}

    /** @alias zend_test_void_return */
    function namespaced_aliased_func(): void {}

    /**
     * @deprecated
     * @alias zend_test_void_return
     */
    function namespaced_deprecated_aliased_func(): void {}
}

namespace ZendTestNS2\ZendSubNS {

    /** @var string */
    const ZEND_CONSTANT_A = \ZendTestNS2\ZEND_CONSTANT_A;

    class Foo {
        public function method(): void {}
    }

    function namespaced_func(): bool {}

    /** @deprecated */
    function namespaced_deprecated_func(): void {}

    /** @alias zend_test_void_return */
    function namespaced_aliased_func(): void {}

    /**
     * @deprecated
     * @alias zend_test_void_return
     */
    function namespaced_deprecated_aliased_func(): void {}
}
