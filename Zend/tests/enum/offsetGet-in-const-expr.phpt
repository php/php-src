--TEST--
Enum offsetGet in constant expression
--FILE--
<?php

enum Foo implements ArrayAccess {
    case Bar;

    public function offsetGet($key) {
        return 42;
    }

    public function offsetExists($key) {}
    public function offsetSet($key, $value) {}
    public function offsetUnset($key) {}
}

class X {
    const FOO_BAR = Foo::Bar[0];
}

var_dump(X::FOO_BAR);

?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot use [] on objects in constant expression in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
