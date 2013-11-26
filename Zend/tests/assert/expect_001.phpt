--TEST--
test passing assertion
--INI--
zend.assertions=1
--FILE--
<?php
assert(true);
var_dump(true);
?>
--EXPECTF--	
bool(true)
