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
===DONE===
--EXPECTF--

Strict Standards: Declaration of B::f() should be compatible with that of A::f() in %sinheritance_003.php on line %d
===DONE===
