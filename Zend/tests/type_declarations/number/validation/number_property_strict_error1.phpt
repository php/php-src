--TEST--
Test that the number property type doesn't accept any non-numeric type in strict mode
--FILE--
<?php
declare(strict_types=1);

class Foo
{
    public number $property1 = null;
}

?>
--EXPECTF--
Fatal error: Default value for property of type number may not be null. Use the nullable type ?number to allow null default value in %s on line %d
