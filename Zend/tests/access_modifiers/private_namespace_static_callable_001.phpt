--TEST--
private(namespace) static method via callable array - same namespace
--FILE--
<?php

namespace Foo;

class A {
    private(namespace) static function test() {
        return "A::test";
    }
}

// Same namespace - should work
var_dump(call_user_func([A::class, 'test']));
var_dump(call_user_func(['Foo\A', 'test']));

?>
--EXPECT--
string(7) "A::test"
string(7) "A::test"
