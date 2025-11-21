--TEST--
private(namespace) method visibility with call_user_func_array - same namespace
--FILE--
<?php

namespace Foo;

class A {
    private(namespace) function test($arg) {
        return "A::test: $arg";
    }
}

// Same namespace - should work
$obj = new A();
var_dump(call_user_func_array([$obj, 'test'], ['hello']));

?>
--EXPECT--
string(14) "A::test: hello"
