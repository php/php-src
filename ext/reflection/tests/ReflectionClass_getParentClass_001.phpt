--TEST--
ReflectionClass::getParentClass()
--CREDITS--
Robin Fernandes <robinf@php.net>
Steve Seear <stevseea@php.net>
--FILE--
<?php
class A {}
class B extends A {}

$rc = new ReflectionClass('B');
$parent = $rc->getParentClass();
$grandParent = $parent->getParentClass();
var_dump($parent, $grandParent);

echo "\nTest bad params:\n";
var_dump($rc->getParentClass(null));
var_dump($rc->getParentClass('x'));
var_dump($rc->getParentClass('x', 123));

?>
--EXPECTF--
object(ReflectionClass)#%d (1) {
  ["name"]=>
  string(1) "A"
}
bool(false)

Test bad params:

Warning: Wrong parameter count for ReflectionClass::getParentClass() in %s on line 11
NULL

Warning: Wrong parameter count for ReflectionClass::getParentClass() in %s on line 12
NULL

Warning: Wrong parameter count for ReflectionClass::getParentClass() in %s on line 13
NULL