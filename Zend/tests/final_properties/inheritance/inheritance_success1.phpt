--TEST--
Test that non-final properties can override final properties
--FILE--
<?php

class Foo
{
    final private static int $property1;
    final private int $property2;

    final protected static int $property3;
    final protected int $property4;
}

class Bar extends Foo
{
    final private static int $property1;
    final private int $property2;

    protected static int $property3;
    protected int $property4;
}

?>
--EXPECT--
