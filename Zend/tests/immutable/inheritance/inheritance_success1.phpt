--TEST--
Test that immutable classes can extend non-immutable classes
--FILE--
<?php

class Foo
{
}

immutable class Bar extends Foo
{
}

?>
--EXPECT--
