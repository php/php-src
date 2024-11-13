--TEST--
Tests attributes on internal class properties.
--EXTENSIONS--
zend_test
reflection
--FILE--
<?php

$prop = new \ReflectionProperty(ZendTestClassWithPropertyAttribute::class, 'attributed');
$attr = $prop->getAttributes(ZendTestAttribute::class)[0];
var_dump($attr->getName());
?>
--EXPECT--
string(17) "ZendTestAttribute"
