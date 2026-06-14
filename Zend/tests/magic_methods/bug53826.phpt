--TEST--
Bug #53826: __callStatic fired in base class through a parent call if the method is private
--FILE--
<?php

class A1 {
	public function __call($method, $args) { echo "__call\n"; }
	public static function __callStatic($method, $args) { echo "__callStatic\n"; }
}

class A2 { // A1 with private function test
	public function __call($method, $args) { echo "__call\n"; }
	public static function __callStatic($method, $args) { echo "__callStatic\n"; }
	private function test() {}
}

class B1 extends A1 {
	public function test(){	parent::test();	}
}

class B2 extends A2 {
	public function test(){	parent::test();	}
}

$test1 = new B1;
$test2 = new B2;
$test1->test();
$test2->test();

?>
--EXPECT--
__call
__call
