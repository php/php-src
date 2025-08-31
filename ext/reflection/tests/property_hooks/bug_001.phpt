--TEST--
ReflectionProperty::{get,set}RawValue() crashes on dynamic properties
--FILE--
<?php

class A {
    public $a;
}

$a = new A();
$b = new A();
$b->dyn = 1;

$prop = new ReflectionProperty($b, 'dyn');
var_dump($prop->getRawValue($a));
$prop->setRawValue($a, 1);

?>
--EXPECTF--
Deprecated: Creation of dynamic property A::$dyn is deprecated in %s on line %d

Warning: Undefined property: A::$dyn in %s on line %d
NULL

Deprecated: Creation of dynamic property A::$dyn is deprecated in %s on line %d
