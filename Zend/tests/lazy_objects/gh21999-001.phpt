--TEST--
GH-21999: GC inconsistency with lazy object, var_dump(), and object comparison
--CREDITS--
kid-lxy
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
    var_dump($obj); // builds obj->properties
}

$reflector = new ReflectionClass(C::class);
$obj = $reflector->newLazyGhost(function ($obj) {
    $obj->__construct(); // throws: initialization fails
});
test('Ghost', $obj);
try {
    // zend_std_compare() fetches obj->properties. zend_compare_symbol_tables()
    // adds obj->properties to GC roots.
    $obj > $reflector->newLazyProxy(function () {
        return new C();
    });
} catch (\Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
    gc_collect_cycles();
}

?>
--EXPECTF--
bool(false)
lazy ghost object(C)#%d (1) {
  ["a"]=>
  object(stdClass)#%d (0) {
  }
}
ArgumentCountError: 2 arguments are required, 1 given
