--TEST--
Return type for internal functions 2
--SKIPIF--
<?php
if (!function_exists('zend_test_nullable_array_return')) die('skip zend_test_nullable_array_return function does not exist');
--FILE--
<?php
zend_test_nullable_array_return();
?>
==DONE==
--EXPECT--
==DONE==
