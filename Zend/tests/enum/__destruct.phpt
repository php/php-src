--TEST--
Enum __destruct
--FILE--
<?php

enum Foo {
    case Bar;

    public function __destruct() {
    }
}

?>
--EXPECTF--
Fatal error: Enum may not include magic method __destruct in %s on line %d
