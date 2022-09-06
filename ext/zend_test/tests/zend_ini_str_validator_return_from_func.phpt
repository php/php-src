--TEST--
Test zend_test_zend_ini_str() to check for GC refcount on global returned
--EXTENSIONS--
zend_test
--FILE--
<?php
var_dump(zend_test_zend_ini_str());
?>
--EXPECT--
string(0) ""
