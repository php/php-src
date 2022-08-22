--TEST--
Enum __set_state
--FILE--
<?php

enum Foo {
    case Bar;

    public static function __set_state(array $properties): object {

    }
}

?>
--EXPECTF--
Fatal error: Enum Foo cannot include magic method __set_state in %s on line %d
