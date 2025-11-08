--TEST--
private(namespace) static method with first-class callable - different namespace
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
    $fn = \Foo\A::test(...);
}

?>
--EXPECTF--
Fatal error: Uncaught Error: Call to private(namespace) method Foo\A::test() from namespace Bar in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
