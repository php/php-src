--TEST--
private(namespace) method via ReflectionMethod::invoke - same namespace
--FILE--
<?php

namespace Foo;

class A {
    private(namespace) function test($arg) {
        return "A::test: $arg";
    }
}

// Reflection should bypass visibility checks
$obj = new A();
$method = new \ReflectionMethod(A::class, 'test');
var_dump($method->invoke($obj, 'hello'));

?>
--EXPECT--
string(14) "A::test: hello"
