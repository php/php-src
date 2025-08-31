--TEST--
ZE2 method inheritance without interfaces
--FILE--
<?php

class A
{
    function f($x) {}
}

class B extends A
{
    function f() {}
}

?>
--EXPECTF--
Fatal error: Declaration of B::f() must be compatible with A::f($x) in %sinheritance_003.php on line %d
