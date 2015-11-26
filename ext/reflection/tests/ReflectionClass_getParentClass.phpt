--TEST--
ReflectionClass::getParentClass()
--CREDITS--
Michelangelo van Dam <dragonbe@gmail.com>
#testfest roosendaal on 2008-05-10
--FILE--
<?php

class Foo {}

class Bar extends Foo {}

$rc1 = new ReflectionClass("Bar");
var_dump($rc1->getParentClass());
?>

--EXPECTF--
object(ReflectionClass)#%d (1) {
  ["name"]=>
  string(3) "Foo"
}
