--TEST--
Using both iterable and Traversable
--FILE--
<?php

function test(): iterable|Traversable {
}

?>
--EXPECTF--
Fatal error: Duplicate type Traversable is redundant in %s on line %d
