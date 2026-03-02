--TEST--
Enum __set
--FILE--
<?php

enum Foo {
    case Bar;

    public function __set(string $name, mixed $value)
    {
    }
}

?>
--EXPECTF--
Fatal error: Enum Foo cannot include magic method __set in %s on line %d
