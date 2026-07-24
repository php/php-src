--TEST--
Enum prevent unsetting value
--FILE--
<?php

enum Foo: int {
    case Bar = 0;
}

unset(Foo::Bar->value);

?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot unset readonly property Foo::$value in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
