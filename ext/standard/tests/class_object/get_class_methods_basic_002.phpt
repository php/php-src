--TEST--
Test get_class_methods() function : basic functionality
--FILE--
<?php
/* Prototype  : proto array get_class_methods(mixed class)
 * Description: Returns an array of method names for class or class instance. 
 * Source code: Zend/zend_builtin_functions.c
 * Alias to functions: 
 */

/*
 * Test behaviour with various visibility levels.
 */

class C {
	private function privC() {}
	protected function protC() {}
	public function pubC() {}
	
	public static function testFromC() {
		echo "Accessing C from C:\n";
		var_dump(get_class_methods("C"));
		echo "Accessing D from C:\n";
		var_dump(get_class_methods("D"));
		echo "Accessing X from C:\n";
		var_dump(get_class_methods("X"));
	}
}

class D extends C {
	private function privD() {}
	protected function protD() {}
	public function pubD() {}
	
	public static function testFromD() {
		echo "Accessing C from D:\n";
		var_dump(get_class_methods("C"));
		echo "Accessing D from D:\n";
		var_dump(get_class_methods("D"));
		echo "Accessing X from D:\n";
		var_dump(get_class_methods("X"));
	}
}

class X {
	private function privX() {}
	protected function protX() {}
	public function pubX() {}
	
	public static function testFromX() {
		echo "Accessing C from X:\n";
		var_dump(get_class_methods("C"));
		echo "Accessing D from X:\n";
		var_dump(get_class_methods("D"));
		echo "Accessing X from X:\n";
		var_dump(get_class_methods("X"));
	}
}

echo "Accessing D from global scope:\n";
var_dump(get_class_methods("D"));

C::testFromC();
D::testFromD();
X::testFromX();

echo "Done";
?>
--EXPECTF--
Accessing D from global scope:
array(4) {
  [0]=>
  string(4) "pubD"
  [1]=>
  string(9) "testFromD"
  [2]=>
  string(4) "pubC"
  [3]=>
  string(9) "testFromC"
}
Accessing C from C:
array(4) {
  [0]=>
  string(5) "privC"
  [1]=>
  string(5) "protC"
  [2]=>
  string(4) "pubC"
  [3]=>
  string(9) "testFromC"
}
Accessing D from C:
array(7) {
  [0]=>
  string(5) "protD"
  [1]=>
  string(4) "pubD"
  [2]=>
  string(9) "testFromD"
  [3]=>
  string(5) "privC"
  [4]=>
  string(5) "protC"
  [5]=>
  string(4) "pubC"
  [6]=>
  string(9) "testFromC"
}
Accessing X from C:
array(2) {
  [0]=>
  string(4) "pubX"
  [1]=>
  string(9) "testFromX"
}
Accessing C from D:
array(3) {
  [0]=>
  string(5) "protC"
  [1]=>
  string(4) "pubC"
  [2]=>
  string(9) "testFromC"
}
Accessing D from D:
array(7) {
  [0]=>
  string(5) "privD"
  [1]=>
  string(5) "protD"
  [2]=>
  string(4) "pubD"
  [3]=>
  string(9) "testFromD"
  [4]=>
  string(5) "protC"
  [5]=>
  string(4) "pubC"
  [6]=>
  string(9) "testFromC"
}
Accessing X from D:
array(2) {
  [0]=>
  string(4) "pubX"
  [1]=>
  string(9) "testFromX"
}
Accessing C from X:
array(2) {
  [0]=>
  string(4) "pubC"
  [1]=>
  string(9) "testFromC"
}
Accessing D from X:
array(4) {
  [0]=>
  string(4) "pubD"
  [1]=>
  string(9) "testFromD"
  [2]=>
  string(4) "pubC"
  [3]=>
  string(9) "testFromC"
}
Accessing X from X:
array(4) {
  [0]=>
  string(5) "privX"
  [1]=>
  string(5) "protX"
  [2]=>
  string(4) "pubX"
  [3]=>
  string(9) "testFromX"
}
Done