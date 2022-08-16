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
Fatal error: Enum may not include magic method __sleep in %s on line %d
