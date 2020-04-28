--TEST--
Test that the number property type doesn't accept any non-numeric value in weak mode
--FILE--
<?php

class Foo
{
    public number $property1 = [];
}

$foo = new Foo();

?>
--EXPECTF--
Fatal error: Cannot use array as default value for property Foo::$property1 of type number in %s on line %d
