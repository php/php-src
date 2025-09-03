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
Deprecated: The __sleep() serialization magic method has been deprecated. Implement __serialize() instead (or in addition, if support for old PHP versions is necessary) in %s on line %d

Fatal error: Enum Foo cannot include magic method __sleep in %s on line %d
