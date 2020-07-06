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
Fatal error: Property Bar::$property1 must not have a type to be compatible with overridden property Foo::$property1 in %s on line %d
