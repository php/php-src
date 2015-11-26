--TEST--
ReflectionParameter::isDefault()
--FILE--
<?php
class A {
public $defprop;
}
$a = new A;
$a->myprop = null;

$ro = new ReflectionObject($a);
$props = $ro->getProperties();
$prop1 = $props[0];
var_dump($prop1->isDefault());
$prop2 = $props[1];
var_dump($prop2->isDefault());

var_dump($ro->getProperty('defprop')->isDefault());
var_dump($ro->getProperty('myprop')->isDefault());

$prop1 = new ReflectionProperty($a, 'defprop');
$prop2 = new ReflectionProperty($a, 'myprop');
var_dump($prop1->isDefault());
var_dump($prop2->isDefault());
?>
==DONE==
--EXPECT--
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
==DONE==
