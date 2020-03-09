--TEST--
Test that private final properties can override non-final properties
--FILE--
<?php

class Foo
{
    private static int $property1;
    private int $property2;
}

class Bar extends Foo
{
    final private static int $property1;
    final private int $property2;
}

?>
--EXPECT--
