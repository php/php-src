--TEST--
GH-21999: GC inconsistency with lazy object, var_dump(), and object comparison
--FILE--
<?php

class C {
    public function __construct() {
        printf("%s\n".	__METHOD__);
    }
    public $a;
    public $b;
}

function test(string $name, object $obj) {
    $reflector = new ReflectionClass(C::class);
    $reflector->getProperty('a')->setRawValueWithoutLazyInitialization($obj, new stdClass);
    var_dump(!$reflector->isUninitializedLazyObject($obj));
    var_dump($obj);
}

$reflector = new ReflectionClass(C::class);
$obj = $reflector->newLazyGhost(function ($obj) {
    $obj->__construct();
});
$reflector->getProperty('a')->setRawValueWithoutLazyInitialization($obj, $obj);

try {
    var_dump($obj < $reflector->newLazyGhost(function () {}));
} catch (\Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
    gc_collect_cycles();
}

?>
--EXPECT--
ArgumentCountError: 2 arguments are required, 1 given
