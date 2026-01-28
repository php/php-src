--TEST--
GH-20905: Lazy proxy bailing __clone assertion
--CREDITS--
Viet Hoang Luu (@vi3tL0u1s)
--FILE--
<?php

function test() {
    function f() {}
}

class A {
    public $_;
    public function __clone() {
        test();
    }
}

test();
clone (new ReflectionClass(A::class))->newLazyProxy(fn() => new A);

?>
--EXPECTF--
Fatal error: Cannot redeclare function f() (previously declared in %s:%d) in %s on line %d
