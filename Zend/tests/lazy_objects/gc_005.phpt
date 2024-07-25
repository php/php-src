--TEST--
Lazy objects: GC 005
--FILE--
<?php

class Canary {
    public $value;
    public function __destruct() {
        var_dump(__FUNCTION__);
    }
}

class C {
    public $value;
}

function ghost() {
    printf("# Ghost:\n");

    $canary = new Canary();

    $reflector = new ReflectionClass(C::class);
    $obj = $reflector->newInstanceWithoutConstructor();
    $reflector->resetAsLazyGhost($obj, function () use ($canary) {
    });

    $reflector->getProperty('value')->setRawValueWithoutLazyInitialization($obj, $obj);
    $reflector = null;

    $canary->value = $obj;
    $obj = null;
    $canary = null;

    gc_collect_cycles();
}

function proxy() {
    printf("# Proxy:\n");

    $canary = new Canary();

    $reflector = new ReflectionClass(C::class);
    $obj = $reflector->newInstanceWithoutConstructor();
    $reflector->resetAsLazyProxy($obj, function () use ($canary) {
        return new C();
    });

    $reflector->getProperty('value')->setRawValueWithoutLazyInitialization($obj, $obj);
    $reflector = null;

    $canary->value = $obj;
    $obj = null;
    $canary = null;

    gc_collect_cycles();
}

ghost();
proxy();

?>
==DONE==
--EXPECT--
# Ghost:
string(10) "__destruct"
# Proxy:
string(10) "__destruct"
==DONE==
