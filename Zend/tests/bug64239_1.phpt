--TEST--
Bug #64239 (get_class_methods() changed behavior)
--FILE--
<?php
class A {
	use T2 { t2method as Bmethod; }
}

class B extends A {
}

trait T2 {
	public function t2method() {
	}
}
print_r(get_class_methods("A"));
print_r(get_class_methods("B"));
--EXPECT--
Array
(
    [0] => Bmethod
    [1] => t2method
)
Array
(
    [0] => Bmethod
    [1] => t2method
)
