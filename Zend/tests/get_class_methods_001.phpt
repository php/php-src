--TEST--
get_class_methods(): Testing scope
--FILE--
<?php

abstract class A { 
	public function a() { }
	private function b() { }
	protected function c() { }	
}

class B extends A {
	private function bb() { }
	
	static public function test() { 
		var_dump(get_class_methods('A'));
		var_dump(get_class_methods('B'));
	}
}


var_dump(get_class_methods('A'));
var_dump(get_class_methods('B'));


B::test();

?>
--EXPECT--
array(1) {
  [0]=>
  unicode(1) "a"
}
array(2) {
  [0]=>
  unicode(4) "test"
  [1]=>
  unicode(1) "a"
}
array(2) {
  [0]=>
  unicode(1) "a"
  [1]=>
  unicode(1) "c"
}
array(4) {
  [0]=>
  unicode(2) "bb"
  [1]=>
  unicode(4) "test"
  [2]=>
  unicode(1) "a"
  [3]=>
  unicode(1) "c"
}
