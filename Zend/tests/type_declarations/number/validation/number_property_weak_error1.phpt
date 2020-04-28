--TEST--
Test that the number property type doesn't accept any non-numeric value in weak mode
--FILE--
<?php

class Foo
{
    public number $property1 = null;
}

$foo = new Foo();

?>
--EXPECTF--
Fatal error: Default value for property of type number may not be null. Use the nullable type ?number to allow null default value in %s on line %d
