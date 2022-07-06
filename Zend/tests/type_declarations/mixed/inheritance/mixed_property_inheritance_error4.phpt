--TEST--
Test that a property of mixed type can't be overridden by a property of a union type
--FILE--
<?php

class Foo
{
    public mixed $property1;
}

class Bar extends Foo
{
    public bool|int|float|string|array|object|null $property1;
}

?>
--EXPECTF--
Fatal error: Type of Bar::$property1 must be mixed (as in class Foo) in %s on line %d
