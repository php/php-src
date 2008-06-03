--TEST--
When __call() is invoked via ::, ensure private implementation of __call() in superclass is accessed without error. 
--FILE--
<?php
class A {
	private function __call($strMethod, $arrArgs) {
		echo "In " . __METHOD__ . "($strMethod, array(" . implode(',',$arrArgs) . "))\n";
		var_dump($this);
	}
}

class B extends A {
	function test() {
		A::test1(1,'a');
		B::test2(1,'a');
		self::test3(1,'a');
		parent::test4(1,'a');
	}
}

$b = new B();
$b->test();
?>
--EXPECTF--
Fatal error: The magic method __call() must have public visibility and can not be static in %s on line %d
