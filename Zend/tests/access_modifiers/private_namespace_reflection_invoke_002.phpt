--TEST--
private(namespace) method via ReflectionMethod::invoke - different namespace requires setAccessible
--FILE--
<?php

namespace Foo {
    class A {
        private(namespace) function test($arg) {
            return "A::test: $arg";
        }
    }
}

namespace Bar {
    $obj = new \Foo\A();
    $method = new \ReflectionMethod(\Foo\A::class, 'test');

    // Reflection bypasses namespace visibility - should work
    var_dump($method->invoke($obj, 'hello'));

    // setAccessible has no effect (deprecated) - still works
    $method->setAccessible(true);
    var_dump($method->invoke($obj, 'hello'));
}

?>
--EXPECTF--
string(14) "A::test: hello"

Deprecated: Method ReflectionMethod::setAccessible() is deprecated since 8.5, as it has no effect in %s on line %d
string(14) "A::test: hello"
