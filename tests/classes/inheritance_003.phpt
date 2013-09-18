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
===DONE===
