--TEST--
Trying to write on method return
--FILE--
<?php

error_reporting(E_ALL);

class foo {
	public $x = array();

	public function b() {
		return $this->x;
	}

	public function c() {
		return $x;
	}

	static public function d() {

	}
}

$foo = new foo;

$foo->b()[0] = 1;

$foo->c()[100] = 2;

foo::d()[] = 3;

print "ok\n";

?>
--EXPECTF--
Notice: Undefined variable: x in %s on line %d
ok
