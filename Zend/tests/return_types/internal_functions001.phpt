--TEST--
Return type for internal functions
--EXTENSIONS--
zend_test
--SKIPIF--
<?php
// Internal function return types are only checked in debug builds
if (!PHP_DEBUG) die('skip requires debug build');
?>
--INI--
opcache.jit=0
--FILE--
<?php
zend_test_array_return();
?>
--EXPECT--
Fatal error: zend_test_array_return(): Return value must be of type array, null returned in Unknown on line 0
