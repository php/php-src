--TEST--
Test valid syntax of final properties
--FILE--
<?php

class Foo
{
    final private int $property1;
    final protected int $property2 = 0;
    final protected static int $property3;
    final protected static int $property4 = 0;
}

?>
--EXPECT--
