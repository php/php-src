--TEST--
Return type covariance; extends class

--FILE--
<?php

class A {
    function foo(): A {}
}

class B extends A {
    function foo(): StdClass {}
}

--EXPECTF--
Fatal error: Declaration of B::foo should be compatible with A::foo(): A, return type mismatch in %s on line %d
