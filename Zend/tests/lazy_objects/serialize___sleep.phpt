--TEST--
Lazy Objects: serialize with __sleep fetches property info from the real instance
--FILE--
<?php

class C {
    public mixed $a;
    private mixed $b = 1;
    function __sleep() {
        return['a', 'b'];
    }
}

$reflector = new ReflectionClass(C::class);

print "Init on serialize and successful initialization\n";

$obj = $reflector->newLazyProxy(function() {
    $c = new C;
    return $c;
});

var_dump(serialize($obj));

print "Init on serialize and failed initialization\n";

$obj = $reflector->newLazyProxy(function() {
    throw new \Exception('initializer');
});

try {
    var_dump(serialize($obj));
} catch (Exception $e) {
    printf("%s: %s\n", $e::class, $e->getMessage());
}

?>
--EXPECTF--
Init on serialize and successful initialization
string(27) "O:1:"C":1:{s:4:"%0C%0b";i:1;}"
Init on serialize and failed initialization
Exception: initializer
