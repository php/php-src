--TEST--
Typed local variables: redeclaring a typed local is a compile error
--FILE--
<?php
int $x = 1;
int $x = 2;
?>
--EXPECTF--
Fatal error: Cannot redeclare typed local variable $x in %s on line %d
