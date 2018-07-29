--TEST--
Return type for internal functions 3: Void return type
--SKIPIF--
<?php
if (!function_exists('zend_test_void_return')) die('skip zend_test_void_return function does not exist');
--FILE--
<?php
var_dump(zend_test_void_return());
?>
--EXPECT--
NULL
