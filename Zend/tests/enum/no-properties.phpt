--TEST--
Enum disallows properties
--FILE--
<?php

enum Foo {
    public $bar;
}

?>
--EXPECTF--
Fatal error: Enums may not include properties in %s on line %d
