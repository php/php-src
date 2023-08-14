--TEST--
DNF typed properties in internal classes that is disabled
--EXTENSIONS--
zend_test
--INI--
disable_classes=_ZendTestClass
--XFAIL--
Use After Free due to types being freed by zend_disable_class()
--FILE--
<?php
$o = new _ZendTestClass();
//var_dump($o);
?>
--EXPECT--
string(42) "_ZendTestInterface|(Traversable&Countable)"
Cannot assign array to property _ZendTestClass::$dnfProperty of type _ZendTestInterface|(Traversable&Countable)
