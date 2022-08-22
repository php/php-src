--TEST--
Enum disallows name property
--FILE--
<?php

enum Foo {
    public string $name;
}

?>
--EXPECTF--
Fatal error: Enums may not include properties in %s on line %d
