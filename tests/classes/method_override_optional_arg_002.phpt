--TEST--
Omitting optional arg in method inherited from abstract class 
--FILE--
<?php

abstract class A {
	function foo($arg = 1) {}
}

class B extends A {
	function foo() {
		echo "foo\n";
	}
}

$b = new B();
$b->foo();

?>
--EXPECTF--
Strict Standards: Declaration of B::foo() should be compatible with that of A::foo() in %s on line %d
foo
