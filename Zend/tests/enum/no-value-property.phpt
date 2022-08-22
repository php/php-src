--TEST--
Enum disallows value property
--FILE--
<?php

enum Foo: int {
    public int $value;
}

?>
--EXPECTF--
Fatal error: Enum Foo may not include properties in %s on line %d
