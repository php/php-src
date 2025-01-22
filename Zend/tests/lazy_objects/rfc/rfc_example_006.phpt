--TEST--
Lazy objects: RFC example 006
--FILE--
<?php

class MyClass {
    public $propA;
    public $propB;
}

// Creating two lazy objects. The initializer of $object1 causes the initialization
// of $object2, which fails.

$reflector = new ReflectionClass(MyClass::class);

$object2 = $reflector->newLazyGhost(function ($object2) {
    $object2->propB = 'value';
    throw new \Exception('initializer exception');
});
$reflector->getProperty('propA')->setRawValueWithoutLazyInitialization($object2, 'object-2');

$object1 = $reflector->newLazyGhost(function ($object1) use ($object2) {
    $object1->propB = 'updated';
    $object1->propB = $object2->propB;
});
$reflector->getProperty('propA')->setRawValueWithoutLazyInitialization($object1, 'object-1');

// Both objects are uninitalized at this point

var_dump($object1);
var_dump($object2);

try {
    var_dump($object1->propB);
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

// The state of both objects is unchanged

var_dump($object1);
var_dump($object2);

?>
==DONE==
--EXPECTF--
lazy ghost object(MyClass)#%d (1) {
  ["propA"]=>
  string(8) "object-1"
}
lazy ghost object(MyClass)#%d (1) {
  ["propA"]=>
  string(8) "object-2"
}
initializer exception
lazy ghost object(MyClass)#%d (1) {
  ["propA"]=>
  string(8) "object-1"
}
lazy ghost object(MyClass)#%d (1) {
  ["propA"]=>
  string(8) "object-2"
}
==DONE==
