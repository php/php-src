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
Fatal error: Enum may not include magic method __isset in %s on line %d
