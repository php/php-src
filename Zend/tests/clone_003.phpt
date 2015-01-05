--TEST--
Using clone statement on undefined variable
--FILE--
<?php

$a = clone $b;

?>
--EXPECTF--
Notice: Undefined variable: b in %s on line %d

Fatal error: __clone method called on non-object in %s on line %d
