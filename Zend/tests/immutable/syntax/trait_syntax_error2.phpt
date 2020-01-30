--TEST--
Test invalid syntax of traits with the immutable modifier
--FILE--
<?php

immutable immutable trait Foo
{
}

?>
--EXPECTF--
Fatal error: Multiple immutable modifiers are not allowed in %s on line %d
