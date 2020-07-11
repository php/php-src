--TEST--
ReflectionClass::getParentClass()
--CREDITS--
Michelangelo van Dam <dragonbe@gmail.com>
#testfest roosendaal on 2008-05-10
--FILE--
<?php

class Foo {}

class Bar extends Foo {}

$rc = new ReflectionClass("Bar");
$parent = $rc->getParentClass();
$grandParent = $parent->getParentClass();
var_dump($parent, $grandParent);
?>
--EXPECTF--
object(ReflectionClass)#%d (1) {
  ["name"]=>
  string(3) "Foo"
}
bool(false)
