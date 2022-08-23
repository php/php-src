--TEST--
Enum __unset
--FILE--
<?php

enum Foo {
    case Bar;

    public function __unset($property) {
        return;
    }
}

?>
--EXPECTF--
Fatal error: Enum Foo cannot include magic method __unset in %s on line %d
