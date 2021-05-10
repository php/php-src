--TEST--
Internal class variance
--SKIPIF--
<?php if (!extension_loaded('zend_test')) die('skip requires zend_test'); ?>
--FILE--
<?php
$test = new _ZendTestChildClass;

var_dump($test->returnsThrowable());
?>
--EXPECT--
string(16) "returnsThrowable"
