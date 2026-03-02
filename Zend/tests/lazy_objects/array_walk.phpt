--TEST--
Lazy Objects: array_walk()
--FILE--
<?php

class C {
    public int $a = 1;
}


$reflector = new ReflectionClass(C::class);
$obj = $reflector->newLazyProxy(function () {
    return new C();
});

array_walk($obj, function (&$value, $key) {
    try {
        $value = 'string';
    } catch (Error $e) {
        printf("%s: %s\n", $e::class, $e->getMessage());
    }
    $value = 2;
});

var_dump($obj);

?>
--EXPECTF--
TypeError: Cannot assign string to reference held by property C::$a of type int
lazy proxy object(C)#%d (1) {
  ["instance"]=>
  object(C)#%d (1) {
    ["a"]=>
    int(2)
  }
}
