--TEST--
Return type for internal functions
--SKIPIF--
<?php
if (!function_exists('zend_test_array_return')) die('skip');
// Internal function return types are only checked in debug builds
if (!PHP_DEBUG) die('skip requires debug build');
?>
--FILE--
<?php
zend_test_array_return();
?>
--EXPECTF--
Fatal error: Return value of zend_test_array_return() must be of the type array, null returned in %s on line %d
