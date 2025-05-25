--TEST--
AST can be recreated (interface with generic types)
--EXTENSIONS--
zend_test
--FILE--
<?php

namespace {
    interface MyInterface1<T1 : string|Stringable|int, T2> {
        public function bar(T1 $v): T2;
    }
}

namespace Foo {
    interface MyInterface2<S : string|\Stringable|int> extends \MyInterface1<S, S> {
        public function foobar(S $v): int;
    }

    class MyClass implements MyInterface2<string> {
        public function bar(string $v): string {}
        public function foobar(string $v): int {}
    }
}

namespace {
    echo zend_test_compile_to_ast( file_get_contents( __FILE__ ) );
}

?>
--EXPECT--
namespace {
    interface MyInterface1<T1 : string|Stringable|int, T2> {
        public function bar(T1 $v): T2;

    }

}

namespace Foo {
    interface MyInterface2<S : string|\Stringable|int> implements \MyInterface1<S, S> {
        public function foobar(S $v): int;

    }

    class MyClass implements MyInterface2<string> {
        public function bar(string $v): string {
        }

        public function foobar(string $v): int {
        }

    }

}

namespace {
    echo zend_test_compile_to_ast(file_get_contents(__FILE__));
}
