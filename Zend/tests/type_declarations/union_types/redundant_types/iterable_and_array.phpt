--TEST--
Using both iterable and array
--FILE--
<?php

function test(): iterable|array {
}

?>
--EXPECTF--
Fatal error: Duplicate type array is redundant in %s on line %d
