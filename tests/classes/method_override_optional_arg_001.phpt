--TEST--
Method override allows optional default argument
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
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
Strict Standards: Declaration of C::foo() should be compatible with that of A::foo() in %s on line %d
int(1)
int(3)
