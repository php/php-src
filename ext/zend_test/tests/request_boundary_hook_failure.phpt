--TEST--
Request boundary reports a failing request_begin or request_end hook
--EXTENSIONS--
zend_test
--FILE--
<?php
var_dump(zend_test_request_boundary());

ini_set('zend_test.request_end_fails', '1');
var_dump(zend_test_request_boundary());
ini_set('zend_test.request_end_fails', '0');

ini_set('zend_test.request_begin_fails', '1');
var_dump(zend_test_request_boundary());
ini_set('zend_test.request_begin_fails', '0');

var_dump(zend_test_request_boundary());
?>
--EXPECTF--
bool(true)

Warning: request_end() for zend_test module failed in %s on line %d
bool(false)

Warning: request_begin() for zend_test module failed in %s on line %d
bool(false)
bool(true)
