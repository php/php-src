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
		static::test3();
		A::test4();
		B::test5();
		C::test6();
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
test5

Fatal error: Call to undefined method C::test6() in %sbug42937.php on line 21
