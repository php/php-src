--TEST--
Lazy Objects: ReflectionProperty::isLazy() with a hooked override
--FILE--
<?php

class B {
    public $plain = 1;
    public $virtual { get => 1; }
    private $priv = 1;
    public static $stat;
}

class C extends B {
    public $plain { get => $this->plain; set => $this->plain = $value; }
    public $priv = 2;
}

$reflector = new ReflectionClass(C::class);

foreach (['Ghost', 'Proxy'] as $kind) {
    printf("# %s:\n", $kind);

    $obj = $kind === 'Ghost'
        ? $reflector->newLazyGhost(function ($obj) { })
        : $reflector->newLazyProxy(function ($obj) { return new C(); });

    foreach (['plain', 'virtual', 'stat'] as $name) {
        printf("%s: B scope %d, C scope %d\n", $name,
            (new ReflectionProperty(B::class, $name))->isLazy($obj),
            (new ReflectionProperty(C::class, $name))->isLazy($obj));
    }

    printf("priv: B scope %d\n",
        (new ReflectionProperty(B::class, 'priv'))->isLazy($obj));
}

?>
--EXPECT--
# Ghost:
plain: B scope 1, C scope 1
virtual: B scope 0, C scope 0
stat: B scope 0, C scope 0
priv: B scope 1
# Proxy:
plain: B scope 1, C scope 1
virtual: B scope 0, C scope 0
stat: B scope 0, C scope 0
priv: B scope 1
