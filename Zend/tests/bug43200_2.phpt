--TEST--
Bug #43200.2 (Interface implementation / inheritance not possible in abstract classes)
--FILE--
<?php

interface A {
	function foo();
}

abstract class B implements A {
	abstract public function foo();
}

class C extends B {
	public function foo() {
		echo 'works';
	}
}

$o = new C();
$o->foo();

?>
--EXPECTF--
works
