--TEST--
GH-20657: GC during zend_lazy_object_realize()
--CREDITS--
vi3tL0u1s
--FILE--
<?php

class C {
    public $a;
}

$reflector = new ReflectionClass(C::class);

for ($i = 0; $i < 10000; $i++) {
    $obj = $reflector->newLazyGhost(function ($obj) {});

    // Add to roots
    $obj2 = $obj;
    unset($obj2);

    // Initialize all props to mark object non-lazy. Also create a cycle.
    $reflector->getProperty('a')->setRawValueWithoutLazyInitialization($obj, $obj);
}

var_dump($obj);

?>
--EXPECTF--
object(C)#%d (1) {
  ["a"]=>
  *RECURSION*
}
