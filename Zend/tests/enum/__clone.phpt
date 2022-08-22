--TEST--
Enum __clone
--FILE--
<?php

enum Foo {
    case Bar;

    public function __clone() {
    }
}

?>
--EXPECTF--
Fatal error: Enum Foo cannot include magic method __clone in %s on line %d
