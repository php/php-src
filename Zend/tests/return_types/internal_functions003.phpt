--TEST--
Return type for internal functions 3: Void return type
--EXTENSIONS--
zend_test
--FILE--
<?php
var_dump(zend_test_void_return());
?>
--EXPECT--
NULL
