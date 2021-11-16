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
Fatal error: Cannot use temporary expression in write context in %s on line %d
