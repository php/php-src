--TEST--
Lazy Objects: ReflectionProperty::skipLazyInitialization() with a hooked override
--FILE--
<?php

class B {
    public $b = 1;
}

class C extends B {
    public $b { get => $this->b; set => $this->b = $value; }
}

$reflector = new ReflectionClass(C::class);

foreach (['Ghost', 'Proxy'] as $kind) {
    printf("# %s:\n", $kind);

    $obj = $kind === 'Ghost'
        ? $reflector->newLazyGhost(function ($obj) { $obj->b = 9; })
        : $reflector->newLazyProxy(function ($obj) { $c = new C(); $c->b = 9; return $c; });

    (new ReflectionProperty(B::class, 'b'))->skipLazyInitialization($obj);

    printf("is lazy: %d\n", $reflector->isUninitializedLazyObject($obj));
    var_dump($obj->b);
}

?>
--EXPECT--
# Ghost:
is lazy: 0
NULL
# Proxy:
is lazy: 0
NULL
