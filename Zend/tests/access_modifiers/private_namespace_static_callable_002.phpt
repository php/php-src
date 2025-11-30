--TEST--
private(namespace) static method via callable array - different namespace
--FILE--
<?php

namespace Foo {
    class A {
        private(namespace) static function test() {
            return "A::test";
        }
    }
}

namespace Bar {
    // Different namespace - should fail
    var_dump(call_user_func(['\Foo\A', 'test']));
}

?>
--EXPECTF--
Fatal error: Uncaught TypeError: call_user_func(): Argument #1 ($callback) must be a valid callback, cannot access private(namespace) method Foo\A::test() in %s:%d
Stack trace:
#0 %s(%d): call_user_func(Array)
#1 {main}
  thrown in %s on line %d
