--TEST--
Stringable should be automatically implemented for internal classes
--SKIPIF--
<?php
if (!extension_loaded('zend-test')) die('skip');
?>
--FILE--
<?php

// _ZendTestClass defines __toString() but does not explicitly implement Stringable.
$obj = new _ZendTestClass;
var_dump($obj instanceof Stringable);

?>
--EXPECT--
bool(true)
