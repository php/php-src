--TEST--
Test that the nullable mixed property type is not valid
--FILE--
<?php

class Foo
{
    public ?mixed $property1;
}

?>
--EXPECTF--
Fatal error: Type mixed cannot be nullable in %s on line %d
