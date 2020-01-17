--TEST--
Internal constant deprecation
--SKIPIF--
<?php
if (!extension_loaded('zend-test')) die('skip requires zend-test');
?>
--FILE--
<?php

var_dump(ZEND_TEST_DEPRECATED);
var_dump(constant('ZEND_TEST_DEPRECATED'));

const X = ZEND_TEST_DEPRECATED;
var_dump(X);

?>
--EXPECTF--
Deprecated: Constant ZEND_TEST_DEPRECATED is deprecated in %s on line %d
int(42)

Deprecated: Constant ZEND_TEST_DEPRECATED is deprecated in %s on line %d
int(42)

Deprecated: Constant ZEND_TEST_DEPRECATED is deprecated in %s on line %d
int(42)
