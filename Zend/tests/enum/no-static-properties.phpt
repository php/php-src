--TEST--
Enum disallows static properties
--FILE--
<?php

enum Foo {
    public static $bar;
}

?>
--EXPECTF--
Fatal error: Enums may not include properties in %s on line %d
