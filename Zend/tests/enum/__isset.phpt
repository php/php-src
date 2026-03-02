--TEST--
Enum __isset
--FILE--
<?php

enum Foo {
    case Bar;

    public function __isset($property) {
        return true;
    }
}

?>
--EXPECTF--
Fatal error: Enum Foo cannot include magic method __isset in %s on line %d
