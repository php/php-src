--TEST--
private(namespace) method with array_map - different namespace
--FILE--
<?php

namespace Foo {
    class A {
        private(namespace) static function double($x) {
            return $x * 2;
        }
    }
}

namespace Bar {
    // Different namespace - should fail
    $result = array_map(['\Foo\A', 'double'], [1, 2, 3]);
    var_dump($result);
}

?>
--EXPECTF--
Fatal error: Uncaught TypeError: array_map(): Argument #1 ($callback) must be a valid callback, cannot access private(namespace) method Foo\A::double() in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
