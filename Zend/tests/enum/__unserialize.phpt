--TEST--
Enum __unserialize
--FILE--
<?php

enum Foo {
    case Bar;

    public function __unserialize(array $data) {

    }
}

?>
--EXPECTF--
Fatal error: Enum Foo cannot include magic method __unserialize in %s on line %d
