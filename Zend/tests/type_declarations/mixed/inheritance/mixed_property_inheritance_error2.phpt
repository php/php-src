--TEST--
Test that a property of mixed type can't be overridden by a property of class type
--FILE--
<?php

class Foo
{
    public mixed $property1;
}

class Bar extends Foo
{
    public stdClass $property1;
}

?>
--EXPECTF--
Fatal error: Type of Bar::$property1 must be mixed (as in class Foo) in %s on line %d
