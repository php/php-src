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
 * Test behaviour with interfaces.
 */

interface I {
	public function pubI();

}

class C implements I {
	public function pubI() {}

	private function privC() {}
	protected function protC() {}
	public function pubC() {}

	public static function testFromC() {
		echo "Accessing I from C:\n";
		var_dump(get_class_methods("I"));
		echo "Accessing C from C:\n";
		var_dump(get_class_methods("C"));
	}
}


echo "Accessing I from global scope:\n";
var_dump(get_class_methods("I"));
echo "Accessing C from global scope:\n";
var_dump(get_class_methods("C"));
C::testFromC();
echo "Done";
?>
--EXPECT--
Accessing I from global scope:
array(1) {
  [0]=>
  string(4) "pubI"
}
Accessing C from global scope:
array(3) {
  [0]=>
  string(4) "pubI"
  [1]=>
  string(4) "pubC"
  [2]=>
  string(9) "testFromC"
}
Accessing I from C:
array(1) {
  [0]=>
  string(4) "pubI"
}
Accessing C from C:
array(5) {
  [0]=>
  string(4) "pubI"
  [1]=>
  string(5) "privC"
  [2]=>
  string(5) "protC"
  [3]=>
  string(4) "pubC"
  [4]=>
  string(9) "testFromC"
}
Done
