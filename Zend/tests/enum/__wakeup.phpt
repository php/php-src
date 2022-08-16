--TEST--
Enum __wakeup
--FILE--
<?php

enum Foo {
    case Bar;

    public function __wakeup() {

    }
}

?>
--EXPECTF--
Fatal error: Enum may not include magic method __wakeup in %s on line %d
