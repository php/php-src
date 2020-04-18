--TEST--
Using both iterable and array
--FILE--
<?php

function test(): iterable|array {
}

?>
--EXPECTF--
Fatal error: Type iterable|array contains both iterable and array, which is redundant in %s on line %d
