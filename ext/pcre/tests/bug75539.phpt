--TEST--
Bug #75539 - Recursive call errors are not reported by preg_last_error()
--FILE--
<?php

var_dump(preg_match('/((?1)?z)/', ''));
var_dump(preg_last_error() === \PREG_INTERNAL_ERROR);

?>
--EXPECT--
int(0)
bool(false)
