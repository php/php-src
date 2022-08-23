--TEST--
Enum disallows name property
--FILE--
<?php

enum Foo {
    public string $name;
}

?>
--EXPECTF--
Fatal error: Enum Foo cannot include properties in %s on line %d
