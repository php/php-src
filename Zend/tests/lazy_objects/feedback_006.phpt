--TEST--
Lazy objects: feedback 006
--FILE--
<?php

class A {
}

class B extends A {
    private $foo;
    public function getFoo() {
        return $this->foo;
    }
}

$r = new ReflectionClass(B::class);
$obj = $r->newLazyProxy(function ($obj) {
    return new A();
});

try {
    $obj->getFoo();
} catch (\Throwable $e) {
    printf("%s: %s\n", $e::class, $e->getMessage());
}

?>
--EXPECT--
TypeError: The real instance class A is not compatible with the proxy class B. The proxy must be a instance of the same class as the real instance, or a sub-class with no additional properties, and no overrides of the __destructor or __clone methods.
