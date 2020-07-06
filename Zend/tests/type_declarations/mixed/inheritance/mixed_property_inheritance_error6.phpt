--TEST--
Test that a property of class type can't be overridden by a property of mixed type
--FILE--
<?php

class Foo
{
    public stdClass $property1;
}

class Bar extends Foo
{
    public mixed $property1;
}

?>
--EXPECTF--
Fatal error: Property Bar::$property1 must be of type stdClass to be compatible with overridden property Foo::$property1 in %s on line %d
