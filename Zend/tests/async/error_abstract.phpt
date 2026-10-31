--TEST--
async methods must have a body
--FILE--
<?php
abstract class A { abstract public async function f(); }
?>
--EXPECTF--
Fatal error: Async method f() must have a body in %s on line %d
