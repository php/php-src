--TEST--
First class callable with nullsafe method call
--FILE--
<?php

$foo?->bar(...);

?>
--EXPECTF--
Fatal error: Cannot combine nullsafe operator with Closure creation in %s on line %d
