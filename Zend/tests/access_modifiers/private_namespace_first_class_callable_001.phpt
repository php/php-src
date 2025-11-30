--TEST--
private(namespace) method with first-class callable - same namespace
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
$fn = $obj->test(...);
var_dump($fn());

?>
--EXPECT--
string(7) "A::test"
