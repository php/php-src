--TEST--
Verify that declare(c_include='...') works in stubs
--EXTENSIONS--
zend_test
--FILE--
<?php

var_dump(ZEND_TEST_STUB_CVALUE_A);

?>
--EXPECT--
string(8) "my_value"
