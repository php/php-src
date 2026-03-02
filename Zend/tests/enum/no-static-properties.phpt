--TEST--
Enum disallows static properties
--FILE--
<?php

enum Foo {
    public static $bar;
}

?>
--EXPECTF--
Fatal error: Enum Foo cannot include properties in %s on line %d
