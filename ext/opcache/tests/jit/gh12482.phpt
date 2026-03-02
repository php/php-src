--TEST--
GH-12482: Invalid type inference
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit_hot_func=2
--FILE--
<?php
trait T {
	function foo() {
		$cloned = clone $this;
		$cloned->x = 42;
		return $cloned;
	}
}
class A {
	use T;
	public $a = 1;
	public $b = 2;
	public $c = 3;
	public $x = 4;
}
class B {
	use T;
	public $x = 5;
}
$a = new A;
var_dump($a->foo());
var_dump($a->foo());
$b = new B;
var_dump($b->foo());
?>
--EXPECT--
object(A)#2 (4) {
  ["a"]=>
  int(1)
  ["b"]=>
  int(2)
  ["c"]=>
  int(3)
  ["x"]=>
  int(42)
}
object(A)#2 (4) {
  ["a"]=>
  int(1)
  ["b"]=>
  int(2)
  ["c"]=>
  int(3)
  ["x"]=>
  int(42)
}
object(B)#3 (1) {
  ["x"]=>
  int(42)
}
