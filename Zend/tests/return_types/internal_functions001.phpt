--TEST--
Return type for internal functions
--SKIPIF--
<?php
if (!extension_loaded('zend-test')) die('skip zend-test extension not loaded');
// Internal function return types are only checked in debug builds
if (!PHP_DEBUG) die('skip requires debug build');
?>
--FILE--
<?php
zend_test_array_return();
?>
--EXPECTF--
Fatal error: Return value of zend_test_array_return() must be of the type array, null returned in %s on line %d
