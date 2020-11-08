--TEST--
Return type covariance; extends abstract class
--FILE--
<?php

abstract class A {
    abstract function foo(): A;
}

class B extends A {
    function foo(): StdClass {}
}
?>
--EXPECTF--
Fatal error: Declaration of B::foo(): StdClass must be compatible with A::foo(): A in %s on line %d
