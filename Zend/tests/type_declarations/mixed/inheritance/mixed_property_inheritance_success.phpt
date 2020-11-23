--TEST--
Test that a property of mixed property type can be overridden by a property of mixed type
--FILE--
<?php

class Foo
{
    public mixed $property1;
}

class Bar extends Foo
{
    public mixed $property1;
}

?>
--EXPECT--
