--TEST--
First class callable with nullsafe method call (nested)
--FILE--
<?php

$foo?->foo->bar(...);

?>
--EXPECTF--
Fatal error: Cannot combine nullsafe operator with Closure creation in %s on line %d
