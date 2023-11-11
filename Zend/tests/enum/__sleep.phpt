--TEST--
Enum __sleep
--FILE--
<?php

enum Foo {
    case Bar;

    public function __sleep(): array {

    }
}

?>
--EXPECTF--
Fatal error: Enum Foo cannot include magic method __sleep in %s on line %d
