--TEST--
Test that an untyped property can't be overridden by a property of mixed type
--FILE--
<?php

class Foo
{
    public $property1;
}

class Bar extends Foo
{
    public mixed $property1;
}

?>
--EXPECTF--
Fatal error: Type of Bar::$property1 must be omitted to match the parent definition in class Foo in %s on line %d
