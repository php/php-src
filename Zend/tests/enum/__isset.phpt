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
Fatal error: Enum cannot include magic method __isset in %s on line %d
