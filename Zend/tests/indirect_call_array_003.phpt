--TEST--
Indirect method call by array - Calling __call() and __callStatic()
--FILE--
<?php

class foo {
	public function __call($a, $b) {
		printf("From %s:\n", __METHOD__);
		var_dump($a);
		var_dump($this);
	}
	static public function __callStatic($a, $b) {
		printf("From %s:\n", __METHOD__);
		var_dump($a);
		var_dump($this);
	}
}

$arr = array('foo', 'abc');
$arr();

$foo = new foo;
$arr = array($foo, 'abc');
$arr();


?>
--EXPECTF--
From foo::__callStatic:
string(3) "abc"

Notice: Undefined variable: this in %s on line %d
NULL
From foo::__call:
string(3) "abc"
object(foo)#%d (0) {
}

