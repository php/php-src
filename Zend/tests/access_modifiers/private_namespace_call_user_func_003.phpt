--TEST--
private(namespace) method visibility with call_user_func - global namespace
--FILE--
<?php

namespace Foo {
    class A {
        private(namespace) function test() {
            return "A::test";
        }
    }
}

namespace {
    // Global namespace - should fail
    $obj = new \Foo\A();
    var_dump(call_user_func([$obj, 'test']));
}

?>
--EXPECTF--
Fatal error: Uncaught TypeError: call_user_func(): Argument #1 ($callback) must be a valid callback, cannot access private(namespace) method Foo\A::test() in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
