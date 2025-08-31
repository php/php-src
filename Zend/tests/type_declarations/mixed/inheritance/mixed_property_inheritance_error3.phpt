--TEST--
Test that a property of mixed type can't be overridden by an untyped property
--FILE--
<?php

class Foo
{
    public mixed $property1;
}

class Bar extends Foo
{
    public $property1;
}

?>
--EXPECTF--
Fatal error: Type of Bar::$property1 must be mixed (as in class Foo) in %s on line %d
