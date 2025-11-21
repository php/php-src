--TEST--
private(namespace) static method with first-class callable - same namespace
--FILE--
<?php

namespace Foo;

class A {
    private(namespace) static function test() {
        return "A::test";
    }
}

// Same namespace - should work
$fn = A::test(...);
var_dump($fn());

?>
--EXPECT--
string(7) "A::test"
