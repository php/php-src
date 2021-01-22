--TEST--
Internal static methods should not be confused with global functions
--SKIPIF--
<?php
if (!extension_loaded('zend_test')) die('skip requires zend_test');
?>
--FILE--
<?php

var_dump(is_bool(_ZendTestClass::is_object()));

?>
--EXPECT--
bool(false)
