--TEST--
Test that the number property type accepts any numeric value in weak mode
--FILE--
<?php

class Foo
{
    public number $property3 = 1;
    public number $property4 = 3.14;
}

?>
--EXPECT--
