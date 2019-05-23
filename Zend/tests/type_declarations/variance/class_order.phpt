--TEST--
Returns are covariant, but we don't allow the code due to class ordering
--FILE--
<?php

class A {
    public function method() : B {}
}
class B extends A {
    public function method() : C {}
}
class C extends B {
}

new C;

?>
--EXPECTF--
Fatal error: Declaration of B::method(): C must be compatible with A::method(): B. Failed to load class C in %s on line %d
