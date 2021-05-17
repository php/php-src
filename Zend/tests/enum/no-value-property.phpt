--TEST--
Enum disallows value property
--FILE--
<?php

enum Foo: int {
    public int $value;
}

?>
--EXPECTF--
Fatal error: Enums may not include instance properties in %s on line %d
