--TEST--
Test invalid syntax of interfaces with the immutable modifier
--FILE--
<?php

immutable immutable interface Foo
{
}

?>
--EXPECTF--
Fatal error: Multiple immutable modifiers are not allowed in %s on line %d
