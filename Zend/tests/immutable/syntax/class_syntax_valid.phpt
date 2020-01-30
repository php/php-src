--TEST--
Test valid syntax of immutable classes
--FILE--
<?php

immutable class Foo
{
}

IMMUTABLE class CapitalFoo
{
}

final immutable class Foo2
{
}

abstract immutable class Foo3
{
}

?>
--EXPECT--
