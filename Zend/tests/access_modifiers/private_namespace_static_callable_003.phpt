--TEST--
private(namespace) static method via callable array - string class name
--FILE--
<?php

namespace Foo {
    class A {
        private(namespace) static function test() {
            return "A::test";
        }
    }
}

namespace Foo {
    // Same namespace with string class name - should work
    var_dump(call_user_func(['A', 'test']));
    var_dump(call_user_func(['Foo\A', 'test']));
}

?>
--EXPECT--
string(7) "A::test"
string(7) "A::test"
