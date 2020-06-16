--TEST--
Variance check for abstract constructor
--FILE--
<?php
class X {
}
abstract class A {
    abstract function __construct(X $x);
}
class B extends A {
    function __construct(object $x) {}
}
class C extends B {
    function __construct(Y $x) {}
}
?>
--EXPECTF--
Fatal error: Could not check compatibility between C::__construct(Y $x): void and A::__construct(X $x): void, because class Y is not available in %s on line %d
