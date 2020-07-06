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
Fatal error: Property Bar::$property1 must be of type mixed to be compatible with overridden property Foo::$property1 in %s on line %d
