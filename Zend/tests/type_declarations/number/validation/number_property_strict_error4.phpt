--TEST--
Test that the number property type doesn't accept any non-numeric type in strict mode
--FILE--
<?php
declare(strict_types=1);

class Foo
{
    public number $property1 = "0";
}

?>
--EXPECTF--
Fatal error: Cannot use string as default value for property Foo::$property1 of type number in %s on line %d
