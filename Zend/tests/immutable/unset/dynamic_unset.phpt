--TEST--
Test that unsetting dynamic properties of immutable classes doesn't have any effect
--FILE--
<?php

immutable class Foo
{
}

$foo = new Foo();

unset($foo->property1);

?>
--EXPECT--
