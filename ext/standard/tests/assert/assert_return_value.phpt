--TEST--
assert() returns boolean on success or failure
--INI--
zend.assertions = 1
assert.exception = 0
--FILE--
<?php
var_dump(assert(true));
var_dump(assert(false));
?>
--EXPECTF--
bool(true)

Warning: assert(): assert(false) failed in %s on line %d
bool(false)
