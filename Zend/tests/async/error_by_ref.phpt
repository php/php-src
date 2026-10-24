--TEST--
async functions cannot return by reference
--FILE--
<?php
async function &f() { return 1; }
?>
--EXPECTF--
Fatal error: Async functions cannot return by reference in %s on line %d
