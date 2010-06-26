--TEST--
Bug #48215 - parent::method() calls __construct
--FILE--
<?php
class A
{
	public function __construct() {
		echo __METHOD__ . "\n";
	}
	protected function A()
	{
		echo __METHOD__ . "\n";
	}
}
class B extends A
{
	public function __construct() {
		echo __METHOD__ . "\n";
		parent::__construct();
	}
	public function A()
	{
		echo __METHOD__ . "\n";
		parent::A();
	}
}
$b = new B();
$b->A();
?>
===DONE===
--EXPECTF--
B::__construct
A::__construct
B::A
A::A
===DONE===
