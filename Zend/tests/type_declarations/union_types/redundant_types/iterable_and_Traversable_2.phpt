--TEST--
Using both iterable and Traversable, with extra classes
--FILE--
<?php

function test(): iterable|Traversable|ArrayAccess {
}

?>
--EXPECTF--
Fatal error: Duplicate type Traversable is redundant in %s on line %d
