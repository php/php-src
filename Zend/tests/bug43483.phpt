--TEST--
Bug #43483 (get_class_methods() does not list all visible methods)
--FILE--
<?php
class C {
	public static function test() {
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
Array
(
    [0] => test
)
