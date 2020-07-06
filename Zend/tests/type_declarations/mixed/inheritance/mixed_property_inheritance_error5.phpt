--TEST--
Test that a property of a built-in type can't be overridden by a property of mixed type
--FILE--
<?php

class Foo
{
    public int $property1;
}

class Bar extends Foo
{
    public mixed $property1;
}

?>
--EXPECTF--
Fatal error: Property Bar::$property1 must be of type int to be compatible with overridden property Foo::$property1 in %s on line %d
