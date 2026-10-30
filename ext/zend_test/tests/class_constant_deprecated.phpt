--TEST--
ReflectionClassConstant::isDeprecated()
--EXTENSIONS--
zend_test
--FILE--
<?php

$r = new ReflectionClassConstant('_ZendTestClass', 'ZEND_TEST_DEPRECATED');
var_dump($r->isDeprecated());

$r = new ReflectionClassConstant('_ZendTestClass', 'TYPED_CLASS_CONST2');
var_dump($r->isDeprecated());

?>
--EXPECTF--
bool(true)
bool(false)
