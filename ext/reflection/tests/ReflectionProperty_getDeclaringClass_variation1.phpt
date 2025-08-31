--TEST--
Test ReflectionProperty::getDeclaringClass() with inherited properties.
--FILE--
<?php

class A {
    public $prop;
}

class B extends A {
}

$propInfo = new ReflectionProperty('B', 'prop');
var_dump($propInfo->getDeclaringClass());

?>
--EXPECTF--
object(ReflectionClass)#%d (1) {
  ["name"]=>
  string(1) "A"
}
