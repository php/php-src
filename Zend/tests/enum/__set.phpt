--TEST--
Enum __set
--FILE--
<?php

enum Foo {
    case Bar;

    public function __set(string $name, mixed $value)
    {
        return '__set';
    }
}

?>
--EXPECTF--
Fatal error: Enum may not include magic method __set in %s on line %d
