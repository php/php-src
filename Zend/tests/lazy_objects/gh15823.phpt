--TEST--
GH-15823: Wrong expectations in zend_lazy_object_get_properties()
--FILE--
<?php

class C {
    public int $a = 1;
}

$reflector = new ReflectionClass(C::class);
$calls = 0;
$obj = $reflector->newLazyGhost(function ($obj) use (&$calls) {
    if ($calls++ === 0) {
        throw new Error("initializer");
    }
    $obj->a = 2;
});

// Builds properties ht without lazy initialization
var_dump($obj);
try {
    // Lazy initialization fails during fetching of properties ht
    json_encode($obj);
} catch (Error $e) {
    printf("%s: %s\n", $e::class, $e->getMessage());
}

var_dump(json_encode($obj));

?>
--EXPECTF--
lazy ghost object(C)#%d (0) {
  ["a"]=>
  uninitialized(int)
}
Error: initializer
string(7) "{"a":2}"
