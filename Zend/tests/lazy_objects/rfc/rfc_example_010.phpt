--TEST--
Lazy objects: RFC example 010
--FILE--
<?php

class MyClass {
    public $a;
    public $b;
}

$reflector = new ReflectionClass(MyClass::class);
$obj = $reflector->newLazyGhost(function () {});

$reflector->getProperty('a')->setRawValueWithoutLazyInitialization($obj, 'value');

var_dump($obj);

?>
==DONE==
--EXPECTF--
lazy ghost object(MyClass)#%d (1) {
  ["a"]=>
  string(5) "value"
}
==DONE==
