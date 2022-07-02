--TEST--
Int backed enums with case without value
--FILE--
<?php

enum Foo: int {
    case Bar;
}

var_dump(Foo::Bar->value);

?>
--EXPECTF--
Fatal error: Case Bar of backed enum Foo must have a value in %s on line %d
