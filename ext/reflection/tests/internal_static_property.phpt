--TEST--
ReflectionProperty::get/setValue() on internal static property
--EXTENSIONS--
zend_test
--FILE--
<?php

$rp = new ReflectionProperty('_ZendTestClass', '_StaticProp');
$rp->setValue(42);
var_dump($rp->getValue());

?>
--EXPECT--
int(42)
