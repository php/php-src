--TEST--
Stringable should be automatically implemented for internal classes
--EXTENSIONS--
zend_test
--FILE--
<?php

// _ZendTestClass defines __toString() but does not explicitly implement Stringable.
$obj = new _ZendTestClass;
var_dump($obj instanceof Stringable);

?>
--EXPECT--
bool(true)
