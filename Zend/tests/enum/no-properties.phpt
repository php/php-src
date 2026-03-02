--TEST--
Enum disallows properties
--FILE--
<?php

enum Foo {
    public $bar;
}

?>
--EXPECTF--
Fatal error: Enum Foo cannot include properties in %s on line %d
