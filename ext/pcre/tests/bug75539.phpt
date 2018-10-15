--TEST--
Bug #75539 - Recursive call errors are not reported by preg_last_error()
--FILE--
<?php

var_dump(preg_match('/((?1)?z)/', ''));
var_dump(preg_last_error() === \PREG_INTERNAL_ERROR);

?>
--EXPECTF--
Warning: preg_match(): Compilation failed: recursive call could loop indefinitely at offset %s in %s on line %s
bool(false)
bool(true)
