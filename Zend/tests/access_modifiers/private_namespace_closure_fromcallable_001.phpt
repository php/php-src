--TEST--
private(namespace) method visibility with Closure::fromCallable - same namespace
--FILE--
<?php

namespace Foo;

class A {
    private(namespace) function test() {
        return "A::test";
    }
}

// Same namespace - should work
$obj = new A();
$closure = \Closure::fromCallable([$obj, 'test']);
var_dump($closure());

?>
--EXPECT--
string(7) "A::test"
