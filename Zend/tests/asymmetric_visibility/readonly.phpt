--TEST--
Asymmetric visibility with readonly
--FILE--
<?php

class P {
    public readonly int $pDefault;
    public private(set) readonly int $pPrivate;
    public protected(set) readonly int $pProtected;
    public public(set) readonly int $pPublic;

    public function __construct() {
        $this->pDefault = 1;
        $this->pPrivate = 1;
        $this->pProtected = 1;
        $this->pPublic = 1;
    }
}

class C extends P {
    public function __construct() {
        $this->pDefault = 1;
        try {
            $this->pPrivate = 1;
        } catch (Error $e) {
            echo $e->getMessage(), "\n";
        }
        $this->pProtected = 1;
        $this->pPublic = 1;
    }
}

function test() {
    $p = new ReflectionClass(P::class)->newInstanceWithoutConstructor();
    try {
        $p->pDefault = 1;
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
    try {
        $p->pPrivate = 1;
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
    try {
        $p->pProtected = 1;
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
    $p->pPublic = 1;
}

new P();
new C();
test();

?>
--EXPECT--
Cannot modify private(set) property P::$pPrivate from scope C
Cannot modify protected(set) readonly property P::$pDefault from global scope
Cannot modify private(set) property P::$pPrivate from global scope
Cannot modify protected(set) readonly property P::$pProtected from global scope
