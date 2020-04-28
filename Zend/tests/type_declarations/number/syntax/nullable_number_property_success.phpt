--TEST--
Test that the nullable number property type is valid
--FILE--
<?php

class Foo
{
    public ?number $property1;
}

?>
--EXPECT--
