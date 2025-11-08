--TEST--
private(namespace) static method with Closure::fromCallable - same namespace
--FILE--
<?php

namespace Foo;

class A {
    private(namespace) static function test() {
        return "A::test";
    }
}

// Same namespace - should work
$closure = \Closure::fromCallable([A::class, 'test']);
var_dump($closure());

?>
--EXPECT--
string(7) "A::test"
