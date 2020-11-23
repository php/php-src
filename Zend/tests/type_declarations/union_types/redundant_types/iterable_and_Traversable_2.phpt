--TEST--
Using both iterable and Traversable, with extra classes
--FILE--
<?php

function test(): iterable|Traversable|ArrayAccess {
}

?>
--EXPECTF--
Fatal error: Type Traversable|ArrayAccess|iterable contains both iterable and Traversable, which is redundant in %s on line %d
