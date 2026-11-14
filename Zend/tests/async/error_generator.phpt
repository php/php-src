--TEST--
async functions cannot be generators
--FILE--
<?php
async function g() { yield 1; }
?>
--EXPECTF--
Fatal error: Async functions cannot be generators in %s on line %d
