--TEST--
ZE2 method inheritance without interfaces
--FILE--
<?php

class A
{
	function f() {}
}

class B extends A
{
	function f($x) {}
}

?>
===DONE===
--EXPECTF--

Strict Standards: Declaration of B::f() should be compatible with A::f() in %sinheritance_004.php on line %d
===DONE===
