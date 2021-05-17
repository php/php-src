--TEST--
Enum disallows instance properties
--FILE--
<?php

enum Foo {
    public $bar;
}

?>
--EXPECTF--
Fatal error: Enums may not include instance properties in %s on line %d
