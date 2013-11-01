--TEST--
test passing expectation
--SKIPIF--
<?php
if (!ini_get("zend.expectations"))
    die("skip: expectations disabled");
?>
--FILE--
<?php
expect true;
var_dump(true);
?>
--EXPECTF--	
bool(true)
