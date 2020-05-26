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
--EXPECT--
Fatal error: zend_test_array_return(): Return value must be of type array, null returned in Unknown on line 0
