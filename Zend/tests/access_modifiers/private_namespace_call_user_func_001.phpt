--TEST--
private(namespace) method visibility with call_user_func - same namespace
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
var_dump(call_user_func([$obj, 'test']));

?>
--EXPECT--
string(7) "A::test"
