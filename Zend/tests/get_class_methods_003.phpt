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
  unicode(2) "aa"
  [1]=>
  unicode(2) "bb"
  [2]=>
  unicode(2) "cc"
}
array(9) {
  [0]=>
  unicode(2) "aa"
  [1]=>
  unicode(2) "bb"
  [2]=>
  unicode(2) "cc"
  [3]=>
  unicode(11) "__construct"
  [4]=>
  unicode(10) "__destruct"
  [5]=>
  unicode(1) "a"
  [6]=>
  unicode(1) "b"
  [7]=>
  unicode(8) "static_a"
  [8]=>
  unicode(8) "static_b"
}
array(4) {
  [0]=>
  unicode(1) "a"
  [1]=>
  unicode(1) "b"
  [2]=>
  unicode(8) "static_a"
  [3]=>
  unicode(8) "static_b"
}
