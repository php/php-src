--TEST--
Internal static methods should not be confused with global functions
--EXTENSIONS--
opcache
zend_test
--FILE--
<?php

var_dump(is_bool(_ZendTestClass::is_object()));

?>
--EXPECT--
bool(false)
