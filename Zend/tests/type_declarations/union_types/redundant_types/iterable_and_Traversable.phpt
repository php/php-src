--TEST--
Using both iterable and Traversable
--FILE--
<?php

function test(): iterable|Traversable {
}

?>
--EXPECTF--
Fatal error: Type Traversable|iterable contains both iterable and Traversable, which is redundant in %s on line %d
