--TEST--
Errors: duplicate type parameter name
--FILE--
<?php
class Box<T, T> {}
?>
--EXPECTF--
Fatal error: Cannot redeclare type parameter T in %s on line %d
