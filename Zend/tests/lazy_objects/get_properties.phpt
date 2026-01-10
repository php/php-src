--TEST--
Lazy objects: get_properties failure
--FILE--
<?php

class C {
    public int $a;
    public $b;
}

$reflector = new ReflectionClass(C::class);

$a = $reflector->newLazyProxy(function () {
    throw new \Exception('Initializer');
});

$b = new C();

try {
    var_dump($a > $b);
} catch (Exception $e) {
    printf("%s: %s\n", $e::class, $e->getMessage());
}

?>
--EXPECT--
Exception: Initializer
