--TEST--
Method override allows optional default argument
--FILE--
<?php

class A {
	function foo($arg1 = 1) {
	}
}

class B extends A {
	function foo($arg1 = 2, $arg2 = 3) {
		var_dump($arg1);
		var_dump($arg2);
	}
}

class C extends A {
	function foo() {
	}
}

$b = new B();

$b->foo(1);

?>
--EXPECTF--
Warning: Declaration of C::foo() should be compatible with A::foo($arg1 = 1) in %s on line %d
int(1)
int(3)
