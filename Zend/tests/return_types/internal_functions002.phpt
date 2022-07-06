--TEST--
Return type for internal functions 2
--SKIPIF--
<?php
if (!extension_loaded('zend-test')) die('skip zend-test extension not loaded');
--FILE--
<?php
zend_test_nullable_array_return();
?>
==DONE==
--EXPECT--
==DONE==
