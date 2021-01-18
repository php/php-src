--TEST--
Return type for internal functions 3: Void return type
--SKIPIF--
<?php
if (!extension_loaded('zend_test')) die('skip zend_test extension not loaded');
--FILE--
<?php
var_dump(zend_test_void_return());
?>
--EXPECT--
NULL
