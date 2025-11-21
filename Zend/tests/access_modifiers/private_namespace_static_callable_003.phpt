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
    // String class names don't get auto-resolved in caller's namespace
    // This fails because 'A' doesn't resolve to 'Foo\A' automatically
    var_dump(call_user_func(['A', 'test']));
}

?>
--EXPECTF--
Fatal error: Uncaught TypeError: call_user_func(): Argument #1 ($callback) must be a valid callback, class "A" not found in %s:%d
Stack trace:
#0 %s(%d): call_user_func(Array)
#1 {main}
  thrown in %s on line %d
