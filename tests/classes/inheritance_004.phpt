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

Fatal error: Declaration of B::f() must be compatible with that of A::f() in %sinheritance_004.php on line %d
