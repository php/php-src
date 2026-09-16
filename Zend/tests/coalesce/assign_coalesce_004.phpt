--TEST--
Coalesce assign (??=): Non-writable variable
--FILE--
<?php

function foo() { return 123; }
foo() ??= 456;

?>
--EXPECTF--
Fatal error: Can't use function return value in write context in %s on line %d
