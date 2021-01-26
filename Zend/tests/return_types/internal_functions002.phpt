--TEST--
Return type for internal functions 2
--SKIPIF--
<?php
if (!extension_loaded('zend_test')) die('skip zend_test extension not loaded');
--FILE--
<?php
zend_test_nullable_array_return();
?>
==DONE==
--EXPECT--
==DONE==
