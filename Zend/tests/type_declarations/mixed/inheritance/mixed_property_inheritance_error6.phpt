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
Fatal error: Type of Bar::$property1 must be stdClass (as in class Foo) in %s on line %d
