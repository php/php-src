--TEST--
Bug #42937 (__call() method not invoked when methods are called on parent from child class)
--FILE--
<?php
class A {
	function __call($strMethod, $arrArgs) {
		echo "$strMethod\n";
	}
}

class C {
	function __call($strMethod, $arrArgs) {
		echo "$strMethod\n";
	}
}

class B extends A {
	function test() {
		self::test1();
		parent::test2();
		A::test3();
		B::test4();
		C::test5();
	}
}

$a = new A();
$a->test();

$b = new B();
$b->test();
?>
--EXPECTF--
test
test1
test2
test3
test4

Fatal error: Call to undefined method C::test5() in %sbug42937.php on line 20
