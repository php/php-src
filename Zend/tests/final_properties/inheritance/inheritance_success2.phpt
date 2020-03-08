--TEST--
Test that non-final properties can be overwritten by final properties
--FILE--
<?php

class Foo
{
    protected static int $property1;
    protected int $property2;
}

class Bar extends Foo
{
    final protected static int $property1;
    final protected int $property2;
}

?>
--EXPECT--
