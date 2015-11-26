--TEST--
Testing array dereference on method calls
--FILE--
<?php

error_reporting(E_ALL);

class  foo {
	public $x = 2;
	public function a() {
		$x = array();
		$x[] = new foo;
		return $x;
	}
	public function b() {
		return array(1.2, array(new self));
	}
	public function c() {
		$a = array();
		$b = &$a;
		$b[] = true;
		return $a;
	}
	public function d() {
		return $this->b();
	}
}

$foo = new foo;

var_dump($foo->a()[0]->x);
var_dump($foo->a()[0]);
var_dump($foo->b()[1][0]->a()[0]->x);
var_dump($foo->c()[0]);
var_dump($foo->d()[0]);

?>
--EXPECTF--
int(2)
object(foo)#%d (1) {
  ["x"]=>
  int(2)
}
int(2)
bool(true)
float(1.2)
