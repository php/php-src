--TEST--
ReflectionProperty::get/setValue() on internal static property
--SKIPIF--
<?php
if (!class_exists('_ZendTestClass')) die('skip zend_test extension required');
?>
--FILE--
<?php

$rp = new ReflectionProperty('_ZendTestClass', '_StaticProp');
$rp->setValue(42);
var_dump($rp->getValue());

?>
--EXPECT--
int(42)
