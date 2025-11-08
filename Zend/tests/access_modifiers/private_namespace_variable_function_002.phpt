--TEST--
private(namespace) method via variable function call - different namespace
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
    $callable = ['\Foo\A', 'test'];
    var_dump($callable());
}

?>
--EXPECTF--
Fatal error: Uncaught Error: Call to private(namespace) method Foo\A::test() from global scope in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
