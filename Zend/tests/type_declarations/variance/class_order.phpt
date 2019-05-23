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
Fatal error: Could not check compatibility between B::method(): C and A::method(): B, because class C is not available in %s on line %d
