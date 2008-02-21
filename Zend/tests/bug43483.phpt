--TEST--
Bug #43483 (get_class_methods() does not list all visible methods)
--FILE--
<?php
class C {
	public static function test() {
		D::prot();
		print_r(get_class_methods("D"));
	}
}
class D extends C {
	protected static function prot() {
		echo "Successfully called D::prot().\n";
	}
}
D::test();
?>
--EXPECT--
Successfully called D::prot().
Array
(
    [0] => prot
    [1] => test
)
