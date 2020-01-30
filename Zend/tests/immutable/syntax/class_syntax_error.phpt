--TEST--
Test invalid syntax of immutable classes
--FILE--
<?php

immutable immutable class Foo
{
}

?>
--EXPECTF--
Fatal error: Multiple immutable modifiers are not allowed in %s on line %d
