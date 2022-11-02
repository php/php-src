--TEST--
Falsifiable should be automatically implemented for internal classes
--EXTENSIONS--
zend_test
--FILE--
<?php

// _ZendTestClass defines __toBool() but does not explicitly implement Falsifiable.
$obj = new _ZendTestClass;
var_dump($obj instanceof Falsifiable);

?>
--EXPECT--
bool(true)
