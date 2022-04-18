--TEST--
Int enum invalid const expr
--FILE--
<?php

enum Foo: int {
    case Bar = 1 + $x;
}

var_dump(Foo::Bar->value);

?>
--EXPECTF--
Fatal error: Enum case value must be compile-time evaluatable in %s on line %d
