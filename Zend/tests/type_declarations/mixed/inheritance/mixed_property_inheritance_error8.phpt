--TEST--
Test that a property of a union type can't be overridden by a property of mixed type
--FILE--
<?php

class Foo
{
    public bool|int|float|string|array|object|null $property1;
}

class Bar extends Foo
{
    public mixed $property1;
}

?>
--EXPECTF--
Fatal error: Type of Bar::$property1 must be object|array|string|int|float|bool|null (as in class Foo) in %s on line %d
