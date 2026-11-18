--TEST--
Signal forwarding is compiled out without thread safety
--EXTENSIONS--
zend_test
--SKIPIF--
<?php
if (PHP_ZTS) die('skip NTS only');
?>
--FILE--
<?php
var_dump(function_exists('zend_test_signal_forwarding'));
var_dump(function_exists('zend_test_raise_on_foreign_thread'));
?>
--EXPECT--
bool(false)
bool(false)
