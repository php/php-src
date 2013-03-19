--TEST--
Bug #64239 (get_class_methods() changed behavior)
--FILE--
<?php
class A {
	public function test() { $this->backtrace(); }
}
class B {
	use T2 { t2method as Bmethod; }
}
trait T2 {
	public function t2method() {
	}
}
var_dump(get_class_methods("B"));
--EXPECT--
array(2) {
  [0]=>
  string(7) "bmethod"
  [1]=>
  string(8) "t2method"
}
