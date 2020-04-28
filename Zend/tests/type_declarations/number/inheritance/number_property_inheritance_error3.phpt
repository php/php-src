--TEST--
Test that a property of number type can't be overridden by a property of any other type type
--FILE--
<?php

class Foo
{
    public number $property1;
}

class Bar extends Foo
{
    public ?number $property1;
}

?>
--EXPECTF--
Fatal error: Type of Bar::$property1 must be number (as in class Foo) in %s on line %d
