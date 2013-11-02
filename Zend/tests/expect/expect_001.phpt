--TEST--
test passing expectation
--INI--
zend.expectations=1
--FILE--
<?php
expect true;
var_dump(true);
?>
--EXPECTF--	
bool(true)
