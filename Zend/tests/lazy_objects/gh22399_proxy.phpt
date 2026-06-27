--TEST--
GH-22399 (Lazy object double free, proxy cleanup variant)
--FILE--
<?php

#[AllowDynamicProperties]
class A {
    public $trigger;
    public $b;
}

class B {
    public function __construct(public $proxy) {}
    public function __destruct() {
        unset($this->proxy->b);
    }
}

class C {
    public function __construct(public $proxy) {}
    public function __destruct() {
        unset($this->proxy->dyn);
    }
}

$reflector = new ReflectionClass(A::class);

$a = $reflector->newLazyProxy(function (A $a) {
    $a->b = new B($a);
    $a->dyn = new C($a);
    return new A();
});

$a->trigger;
var_dump($reflector->isUninitializedLazyObject($a));
echo "done\n";
?>
--EXPECT--
bool(false)
done
