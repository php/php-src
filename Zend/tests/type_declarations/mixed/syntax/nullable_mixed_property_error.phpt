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
Fatal error: Type mixed cannot be marked as nullable since mixed already includes null in %s on line %d
