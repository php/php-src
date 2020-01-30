--TEST--
Test that immutable classes can only be extended by immutable classes
--FILE--
<?php

immutable class Foo
{
}

immutable class Bar extends Foo
{
}

immutable class Baz extends Bar
{
}

?>
--EXPECT--
