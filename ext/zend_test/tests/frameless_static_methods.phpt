--TEST--
Frameless static methods
--EXTENSIONS--
zend_test
--FILE--
<?php

var_dump(_ZendTestClass::framelessStaticMethod(42, 69));

?>
--EXPECT--
int(111)
