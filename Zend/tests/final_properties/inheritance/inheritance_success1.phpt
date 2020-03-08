--TEST--
Test that final properties can be overwritten by non-final properties
--FILE--
<?php

class Foo
{
    final protected static int $property1;
    final protected int $property2;
}

class Bar extends Foo
{
    protected static int $property1;
    protected int $property2;
}

?>
--EXPECT--
