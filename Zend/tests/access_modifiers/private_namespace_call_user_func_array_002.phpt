--TEST--
private(namespace) method visibility with call_user_func_array - different namespace
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
    // Different namespace - should fail
    $obj = new \Foo\A();
    var_dump(call_user_func_array([$obj, 'test'], ['hello']));
}

?>
--EXPECTF--
Fatal error: Uncaught TypeError: call_user_func_array(): Argument #1 ($callback) must be a valid callback, cannot access private(namespace) method Foo\A::test() in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
