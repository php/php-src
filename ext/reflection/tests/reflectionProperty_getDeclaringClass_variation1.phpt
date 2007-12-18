--TEST--
Test ReflectionProperty::getDeclaringClass() with inherited properties.
--SKIPIF--
<?php extension_loaded('reflection') or die('skip'); ?>
--FILE--
<?php

class A {
    public $prop;
}

class B extends A {
}

$propInfo = new ReflectionProperty('B', 'prop');
var_dump($propInfo->getDeclaringClass());

echo "Wrong number of params:\n";
$propInfo->getDeclaringClass(1);

?> 
--EXPECTF--
object(ReflectionClass)#%d (1) {
  ["name"]=>
  string(1) "A"
}
Wrong number of params:

Warning: Wrong parameter count for ReflectionProperty::getDeclaringClass() in %s on line %d
