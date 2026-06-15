--TEST--
Typed local variables: using a typed local as a global is a compile error
--FILE--
<?php
int $x = 1;
global $x;
?>
--EXPECTF--
Fatal error: Cannot use typed local variable $x as global variable in %s on line %d
