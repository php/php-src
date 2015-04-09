--TEST--
get_class_methods(): Testing scope
--FILE--
<?php

interface A { 
	function aa();
	function bb();
	static function cc();
}

class C {
	public function a() { }
	protected function b() { }
	private function c() { }
	
	static public function static_a() { }
	static protected function static_b() { }
	static private function static_c() { }
}

class B extends C implements A {
	public function aa() { }
	public function bb() { }
	
	static function cc() { }
	
	public function __construct() {
		var_dump(get_class_methods('A'));
		var_dump(get_class_methods('B'));
		var_dump(get_class_methods('C'));
	}
	
	public function __destruct() { }
}

new B;

?>
--EXPECT--
array(3) {
  [0]=>
  string(2) "aa"
  [1]=>
  string(2) "bb"
  [2]=>
  string(2) "cc"
}
array(9) {
  [0]=>
  string(2) "aa"
  [1]=>
  string(2) "bb"
  [2]=>
  string(2) "cc"
  [3]=>
  string(11) "__construct"
  [4]=>
  string(10) "__destruct"
  [5]=>
  string(1) "a"
  [6]=>
  string(1) "b"
  [7]=>
  string(8) "static_a"
  [8]=>
  string(8) "static_b"
}
array(4) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "b"
  [2]=>
  string(8) "static_a"
  [3]=>
  string(8) "static_b"
}
