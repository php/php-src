--TEST--
PREG_THROW_ON_ERROR: without the flag, a malformed pattern still warns and returns false
--FILE--
<?php

var_dump(preg_match('/[/', 'x'));
var_dump(preg_last_error() === PREG_INTERNAL_ERROR);

?>
--EXPECTF--
Warning: preg_match(): Compilation failed: %s in %s on line %d
bool(false)
bool(true)
