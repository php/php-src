--TEST--
Enum __construct
--FILE--
<?php

enum Foo {
    case Bar;

    public function __construct() {
    }
}

?>
--EXPECTF--
Fatal error: Enum may not include magic method __construct in %s on line %d
