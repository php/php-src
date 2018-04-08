--TEST--
Testing Closure::fromCallable() functionality: Rebinding
--FILE--
<?php

class A {
    public function method() {
        var_dump($this);
    }
}

class B {
}

$fn = Closure::fromCallable([new A, 'method']);
$fn->call(new B);

?>
--EXPECTF--
Warning: Cannot bind method A::method() to object of class B in %s on line %d
